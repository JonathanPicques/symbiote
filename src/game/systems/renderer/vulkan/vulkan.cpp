#include <vector>
#include <iostream>
#include <exception>

#include <glm/vec4.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>

#include "game/systems/renderer/vulkan/vulkan.hpp"

static auto vulkan_debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, std::uint64_t object, std::size_t location, std::int32_t messageCode, const char *pLayerPrefix, const char *pMessage, void *pUserData) -> VkBool32 {
	std::cout << pMessage << std::endl;
	return VK_TRUE;
}

VulkanRenderer::VulkanRenderer(SDL_Window *window, glm::vec4 const &clear_color) {
	// Get vulkan instance extensions
	unsigned int extensions_count;
	if (SDL_Vulkan_GetInstanceExtensions(window, &extensions_count, nullptr) != SDL_TRUE) {
		throw std::runtime_error("VulkanRenderer::VulkanRenderer(): SDL_Vulkan_GetInstanceExtensions failed");
	}
	std::vector<const char *> extensions = {VK_EXT_DEBUG_REPORT_EXTENSION_NAME};
	auto additional_extensions_count = extensions.size();
	extensions.resize(extensions_count + additional_extensions_count);
	if (SDL_Vulkan_GetInstanceExtensions(window, &extensions_count, &extensions[additional_extensions_count]) != SDL_TRUE) {
		throw std::runtime_error("VulkanRenderer::VulkanRenderer(): SDL_Vulkan_GetInstanceExtensions failed");
	}

	// Create vulkan instance
	VkInstanceCreateInfo create_info = {};
	create_info.enabledExtensionCount = static_cast<unsigned int32_t>(extensions.size());
	create_info.ppEnabledExtensionNames = &extensions[0];
	if (vkCreateInstance(&create_info, nullptr, &mInstance) != VkResult::VK_SUCCESS) {
		throw std::runtime_error("VulkanRenderer::VulkanRenderer(): vkCreateInstance failed");
	}

	// Create vulkan surface
	if (SDL_Vulkan_CreateSurface(window, mInstance, &mSurface) != SDL_TRUE) {
		throw std::runtime_error("VulkanRenderer::VulkanRenderer(): SDL_Vulkan_CreateSurface failed");
	}

	// Enable vulkan debug report
	VkDebugReportCallbackEXT callback;
	VkDebugReportCallbackCreateInfoEXT callback_create_info = {
		sType : VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT,
		pNext : nullptr,
		flags : VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT,
		pfnCallback : &vulkan_debug_report,
		pUserData : nullptr,
	};
	auto vkCreateDebugReportCallbackEXT_Addr = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(mInstance, "vkCreateDebugReportCallbackEXT"));
	vkCreateDebugReportCallbackEXT_Addr(mInstance, &callback_create_info, nullptr, &callback);

	// Get physical devices
	unsigned int devices_count = 0;
	if (vkEnumeratePhysicalDevices(mInstance, &devices_count, nullptr) != VkResult::VK_SUCCESS) {
		throw std::runtime_error("VulkanRenderer::VulkanRenderer(): vkEnumeratePhysicalDevices failed");
	}
	mPhysicalDevices.resize(devices_count);
	mPhysicalDeviceProperties.resize(devices_count);
	mPhysicalDeviceSurfaceSupportKHR.resize(devices_count);
	mPhysicalDevicesSurfaceFormatKHR.resize(devices_count);
	mPhysicalDeviceQueueFamilyProperties.resize(devices_count);
	mPhysicalDeviceSurfaceCapabilitiesKHR.resize(devices_count);
	if (vkEnumeratePhysicalDevices(mInstance, &devices_count, &mPhysicalDevices[0]) != VkResult::VK_SUCCESS) {
		throw std::runtime_error("VulkanRenderer::VulkanRenderer(): vkEnumeratePhysicalDevices failed");
	}

	// Get physical devices family queues
	unsigned int device_family_queues_count = 0;
	for (unsigned int device_index = 0; device_index < devices_count; ++device_index) {
		auto const &device = mPhysicalDevices[device_index];
		vkGetPhysicalDeviceProperties(device, &mPhysicalDeviceProperties[device_index]);

		vkGetPhysicalDeviceQueueFamilyProperties(device, &device_family_queues_count, nullptr);
		mPhysicalDeviceSurfaceSupportKHR[device_index].resize(device_family_queues_count);
		mPhysicalDeviceQueueFamilyProperties[device_index].resize(device_family_queues_count);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &device_family_queues_count, &mPhysicalDeviceQueueFamilyProperties[device_index][0]);

		for (unsigned int queue_index = 0; queue_index < device_family_queues_count; ++queue_index) {
			vkGetPhysicalDeviceSurfaceSupportKHR(device, queue_index, mSurface, &mPhysicalDeviceSurfaceSupportKHR[device_index][queue_index]);
		}

		if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, mSurface, &mPhysicalDeviceSurfaceCapabilitiesKHR[device_index]) != VkResult::VK_SUCCESS) {
			throw std::runtime_error("VulkanRenderer::VulkanRenderer(): vkGetPhysicalDeviceSurfaceCapabilitiesKHR failed");
		}

		// Get surface format capabilities
		unsigned int format_count = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &format_count, nullptr);
		mPhysicalDevicesSurfaceFormatKHR[device_index].resize(format_count);
		if (vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &format_count, &mPhysicalDevicesSurfaceFormatKHR[device_index][0])) {
			throw std::runtime_error("VulkanRenderer::VulkanRenderer(): vkGetPhysicalDeviceSurfaceFormatsKHR failed");
		}

		// Select physical device
		for (unsigned int device_index = 0; device_index < mPhysicalDevices.size(); ++device_index) {
			for (unsigned int queue_index = 0; queue_index < mPhysicalDeviceQueueFamilyProperties[device_index].size(); ++queue_index) {
				auto &family_properties = mPhysicalDeviceQueueFamilyProperties[device_index][queue_index];
				auto &family_properties_queue_flags = family_properties.queueFlags;
				if (family_properties_queue_flags & VK_QUEUE_GRAPHICS_BIT) {
					if (mPhysicalDeviceSurfaceSupportKHR[device_index][queue_index] == VK_TRUE) {
						std::cout << "VulkanRenderer::SelectPhysicalDevice(): " << mPhysicalDeviceProperties[device_index].deviceName << std::endl;
						mPhysicalDeviceIndex = device_index;
						mPhysicalDeviceFamilyQueueIndex = queue_index;
						break;
					}
				}
			}
		}

		// Create logical device
		auto queue_priorities = 1.0f;
		const char *pDevExt[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
		VkDeviceQueueCreateInfo device_queue_create_info = {
			sType : VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			queueFamilyIndex : mPhysicalDeviceFamilyQueueIndex,
			queueCount : 1,
			pQueuePriorities : &queue_priorities,
		};
		VkDeviceCreateInfo device_create_info = {
			sType : VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			queueCreateInfoCount : 1,
			pQueueCreateInfos : &device_queue_create_info,
			enabledExtensionCount : sizeof(pDevExt) / sizeof(pDevExt[0]),
			ppEnabledExtensionNames : pDevExt,
		};
		if (vkCreateDevice(mPhysicalDevices[mPhysicalDeviceIndex], &device_create_info, NULL, &mDevice) != VK_SUCCESS) {
			throw std::runtime_error("VulkanRenderer::VulkanRenderer(): vkCreateDevice failed");
		}

		// Create device queue
		vkGetDeviceQueue(mDevice, mPhysicalDeviceFamilyQueueIndex, 0, &mQueue);

		// Create swapchain
		VkSwapchainCreateInfoKHR swapchain_create_info = {
			sType : VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			surface : mSurface,
			minImageCount : 2,
			imageFormat : mPhysicalDevicesSurfaceFormatKHR[mPhysicalDeviceIndex][0].format,			// TODO: replace 0
			imageColorSpace : mPhysicalDevicesSurfaceFormatKHR[mPhysicalDeviceIndex][0].colorSpace, // TODO: replace 0
			imageExtent : mPhysicalDeviceSurfaceCapabilitiesKHR[mPhysicalDeviceIndex].currentExtent,
			imageArrayLayers : 1,
			imageUsage : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			imageSharingMode : VK_SHARING_MODE_EXCLUSIVE,
			preTransform : VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
			compositeAlpha : VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			presentMode : VK_PRESENT_MODE_FIFO_KHR,
			clipped : VK_TRUE,
		};

		if (vkCreateSwapchainKHR(mDevice, &swapchain_create_info, nullptr, &mSwapchainKHR) != VK_SUCCESS) {
			throw std::runtime_error("VulkanRenderer::VulkanRenderer(): vkCreateSwapchainKHR failed");
		}

		unsigned int swapchain_image_count = 0;
		if (vkGetSwapchainImagesKHR(mDevice, mSwapchainKHR, &swapchain_image_count, nullptr) != VK_SUCCESS) {
			throw std::runtime_error("VulkanRenderer::VulkanRenderer(): vkGetSwapchainImagesKHR failed");
		}
		mSwapchainImages.resize(swapchain_image_count);
		mCommandBuffers.resize(swapchain_image_count);
		if (vkGetSwapchainImagesKHR(mDevice, mSwapchainKHR, &swapchain_image_count, &mSwapchainImages[0]) != VK_SUCCESS) {
			throw std::runtime_error("VulkanRenderer::VulkanRenderer(): vkGetSwapchainImagesKHR failed");
		}

		VkCommandPoolCreateInfo command_pool_create_info = {
			sType : VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			queueFamilyIndex : mPhysicalDeviceFamilyQueueIndex,
		};

		if (vkCreateCommandPool(mDevice, &command_pool_create_info, nullptr, &mCommandPool) != VK_SUCCESS) {
			throw std::runtime_error("VulkanRenderer::VulkanRenderer(): vkCreateCommandPool failed");
		}

		VkCommandBufferAllocateInfo command_buffer_alloc_info = {
			sType : VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			commandPool : mCommandPool,
			level : VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			commandBufferCount : static_cast<std::uint32_t>(mSwapchainImages.size()),
		};

		if (vkAllocateCommandBuffers(mDevice, &command_buffer_alloc_info, &mCommandBuffers[0]) != VK_SUCCESS) {
			throw std::runtime_error("VulkanRenderer::VulkanRenderer(): vkAllocateCommandBuffers failed");
		}

		VkCommandBufferBeginInfo command_buffer_begin_info = {
			sType : VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			flags : VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
		};
		VkClearColorValue clear_color_value = {clear_color.r, clear_color.g, clear_color.b, clear_color.a};
		VkImageSubresourceRange image_range = {
			aspectMask : VK_IMAGE_ASPECT_COLOR_BIT,
			levelCount : 1,
			layerCount : 1,
		};

		for (unsigned int command_buffer_index = 0; command_buffer_index < mCommandBuffers.size(); command_buffer_index++) {
			if (vkBeginCommandBuffer(mCommandBuffers[command_buffer_index], &command_buffer_begin_info) != VK_SUCCESS) {
				throw std::runtime_error("VulkanRenderer::VulkanRenderer(): vkBeginCommandBuffer failed");
			}
			vkCmdClearColorImage(mCommandBuffers[command_buffer_index], mSwapchainImages[command_buffer_index], VK_IMAGE_LAYOUT_GENERAL, &clear_color_value, 1, &image_range);
			if (vkEndCommandBuffer(mCommandBuffers[command_buffer_index]) != VK_SUCCESS) {
				throw std::runtime_error("VulkanRenderer::VulkanRenderer(): vkEndCommandBuffer failed");
			}
		}
	}
}

VulkanRenderer::~VulkanRenderer() {
	vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
	vkDestroyInstance(mInstance, nullptr);
}

auto VulkanRenderer::Render() -> void {
	unsigned int image_index = 0;
	if (vkAcquireNextImageKHR(mDevice, mSwapchainKHR, UINT64_MAX, nullptr, nullptr, &image_index) != VK_SUCCESS) {
		throw std::runtime_error("VulkanRenderer::VulkanRenderer(): vkAcquireNextImageKHR failed");
	}
	VkSubmitInfo submit_info = {
		sType : VK_STRUCTURE_TYPE_SUBMIT_INFO,
		commandBufferCount : 1,
		pCommandBuffers : &mCommandBuffers[image_index],
	};
	if (vkQueueSubmit(mQueue, 1, &submit_info, nullptr) != VK_SUCCESS) {
		throw std::runtime_error("VulkanRenderer::VulkanRenderer(): vkQueueSubmit failed");
	}
	VkPresentInfoKHR presentInfo = {
		sType : VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		swapchainCount : 1,
		pSwapchains : &mSwapchainKHR,
		pImageIndices : &image_index,
	};
	if (vkQueuePresentKHR(mQueue, &presentInfo) != VK_SUCCESS) {
		throw std::runtime_error("VulkanRenderer::VulkanRenderer(): vkQueuePresentKHR failed");
	}
}