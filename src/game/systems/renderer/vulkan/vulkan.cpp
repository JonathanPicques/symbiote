#include <vector>
#include <exception>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>

#include "game/systems/renderer/vulkan/vulkan.hpp"

#include <iostream>

VulkanRenderer::VulkanRenderer(SDL_Window *window) {
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

	VkInstanceCreateInfo create_info = {};
	create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	create_info.ppEnabledExtensionNames = &extensions[0];
	if (vkCreateInstance(&create_info, nullptr, &mInstance) != VkResult::VK_SUCCESS) {
		throw std::runtime_error("VulkanRenderer::VulkanRenderer(): vkCreateInstance failed");
	}
	if (SDL_Vulkan_CreateSurface(window, mInstance, &mSurface) != SDL_TRUE) {
		throw std::runtime_error("VulkanRenderer::VulkanRenderer(): SDL_Vulkan_CreateSurface failed");
	}

	unsigned int devices_count = 0;
	if (vkEnumeratePhysicalDevices(mInstance, &devices_count, nullptr) != VkResult::VK_SUCCESS) {
		throw std::runtime_error("VulkanRenderer::VulkanRenderer(): vkEnumeratePhysicalDevices failed");
	}
	mPhysicalDevices.resize(devices_count);
	mPhysicalDeviceProperties.resize(devices_count);
	mPhysicalDeviceSurfaceSupportKHR.resize(devices_count);
	mPhysicalDeviceQueueFamilyProperties.resize(devices_count);
	if (vkEnumeratePhysicalDevices(mInstance, &devices_count, &mPhysicalDevices[0]) != VkResult::VK_SUCCESS) {
		throw std::runtime_error("VulkanRenderer::VulkanRenderer(): vkEnumeratePhysicalDevices failed");
	}

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
	}
}

VulkanRenderer::~VulkanRenderer() {
	vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
	vkDestroyInstance(mInstance, nullptr);
}