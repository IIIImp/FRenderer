#include "SwapchainManager.h"
#include <stdexcept>

SwapchainManager::SwapchainManager(VkSurfaceKHR surface)
	:surface(surface) {}

SwapchainManager::~SwapchainManager()
{
}

void SwapchainManager::createSwapchain(uint32_t width, uint32_t height)
{
	// Get swapchain support
	VulkanContext& vulkanContext = VulkanContext::getVulkanContext();
	auto swapChainSupport = vulkanContext.getSwapchainSupportDetails();
	auto queueFamilyIndices = vulkanContext.getQueueFamilyIndices();

	// Set format, presentMode and extent
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, width, height);

	// Set imageCount min to max
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	// Get queue indices
	uint32_t queueFamilyIndices_index[] = { (uint32_t)queueFamilyIndices.graphicsFamily, (uint32_t)queueFamilyIndices.presentFamily };

	// Create swapchain
	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	if (queueFamilyIndices.graphicsFamily != queueFamilyIndices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices_index;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;
	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchainData.swapchain) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create swap chain!");
	}

	// Set Images, format and extent
	vkGetSwapchainImagesKHR(device, swapchainData.swapchain, &imageCount, nullptr);
	swapchainImages.images.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapchainData.swapchain, &imageCount, swapchainImages.images.data());
	swapchainData.imageFormat = surfaceFormat.format;
	swapchainData.extent = extent;

	createImageViews();
}

void SwapchainManager::recreateSwapchain(uint32_t width, uint32_t height)
{
	vkDeviceWaitIdle(device);
	cleanup();
	createSwapchain(width, height);
}

void SwapchainManager::cleanup()
{
	VulkanContext& vulkanContext = VulkanContext::getVulkanContext();
	auto device = vulkanContext.getContextData().device;
	for (auto imageView : swapchainImages.imageViews) {
		vkDestroyImageView(device, imageView, nullptr);
	}
	swapchainImages.imageViews.clear();

	// Clean up swapchain
	vkDestroySwapchainKHR(device, swapchainData.swapchain, nullptr);
}

void SwapchainManager::createImageViews()
{
	swapchainImages.imageViews.resize(swapchainImages.images.size());
	for (size_t i = 0; i < swapchainImages.images.size(); i++) {
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapchainImages.images[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapchainData.imageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		if (vkCreateImageView(device, &createInfo, nullptr, &swapchainImages.imageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create image views!");
		}
	}
}

VkSurfaceFormatKHR SwapchainManager::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	return availableFormats[0];
}

VkPresentModeKHR SwapchainManager::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			bestMode = availablePresentMode;
		}
	}
	return bestMode;
}

VkExtent2D SwapchainManager::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		VkExtent2D actualExtent = { width, height };
		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
		return actualExtent;
	}
	return VkExtent2D();
}
