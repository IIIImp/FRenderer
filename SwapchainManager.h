#ifndef SWAPCHAINMANAGER_H
#define SWAPCHAINMANAGER_H

#include "vulkan/vulkan.h"
#include <GLFW/glfw3.h>
#include "FRender.h"

class SwapchainManager {
public:
	SwapchainManager(VkDevice device, VkPhysicalDevice physicalDevice, GLFWwindow* window, VkSurfaceKHR surface, QueueFamilyIndices indices);
	~SwapchainManager();
	VkSwapchainKHR getSwapchain() const;
	const std::vector<VkImageView>& getImageView() const;
	VkFormat getImageFormat() const;
	VkExtent2D getExtent() const;

	void recreateSwapchain();
private:
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	GLFWwindow* window = nullptr;

	VkFormat imageFormat = {};
	VkExtent2D extent = {};
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;
	QueueFamilyIndices queueFamilyIndices;

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	void createSwapChain();
	void createImageViews();
};

#endif