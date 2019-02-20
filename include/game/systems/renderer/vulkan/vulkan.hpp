#pragma once

#include <vector>

#include <glm/fwd.hpp>
#include <vulkan/vulkan.hpp>

class SDL_Window;

class VulkanRenderer final {
public:
	VulkanRenderer(SDL_Window *window, glm::vec4 const &clear_color);
	VulkanRenderer(VulkanRenderer &&) = delete;
	VulkanRenderer(VulkanRenderer const &) = delete;
	VulkanRenderer &operator=(VulkanRenderer const &) = delete;

public:
	~VulkanRenderer();

public:
	auto Render() -> void;

private:
	VkInstance mInstance = {};
	VkSurfaceKHR mSurface = {};

private:
	VkDevice mDevice;
	unsigned int mPhysicalDeviceIndex;
	unsigned int mPhysicalDeviceFamilyQueueIndex;
	std::vector<VkPhysicalDevice> mPhysicalDevices;
	std::vector<VkSurfaceCapabilitiesKHR> mPhysicalDeviceSurfaceCapabilitiesKHR;
	std::vector<VkPhysicalDeviceProperties> mPhysicalDeviceProperties;
	std::vector<std::vector<VkBool32>> mPhysicalDeviceSurfaceSupportKHR;
	std::vector<std::vector<VkSurfaceFormatKHR>> mPhysicalDevicesSurfaceFormatKHR;
	std::vector<std::vector<VkQueueFamilyProperties>> mPhysicalDeviceQueueFamilyProperties;

private:
	VkQueue mQueue;
	VkCommandPool mCommandPool;
	VkSwapchainKHR mSwapchainKHR;
	std::vector<VkImage> mSwapchainImages;
	std::vector<VkCommandBuffer> mCommandBuffers;
};