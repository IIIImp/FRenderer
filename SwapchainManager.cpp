#include "SwapchainManager.h"

SwapchainManager::SwapchainManager(VkDevice device, VkPhysicalDevice physicalDevice, GLFWwindow* window, VkSurfaceKHR surface)
{
}

SwapchainManager::~SwapchainManager()
{
}

VkSwapchainKHR SwapchainManager::getSwapchain() const
{
	return VkSwapchainKHR();
}

const std::vector<VkImageView>& SwapchainManager::getImageView() const
{
	// TODO: �ڴ˴����� return ���
}

void SwapchainManager::recreateSwapchain()
{
}

SwapChainSupportDetails SwapchainManager::querySwapChainSupport(VkPhysicalDevice device)
{
	return SwapChainSupportDetails();
}

VkSurfaceFormatKHR SwapchainManager::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	return VkSurfaceFormatKHR();
}

VkPresentModeKHR SwapchainManager::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
	return VkPresentModeKHR();
}

VkExtent2D SwapchainManager::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	return VkExtent2D();
}

void SwapchainManager::createSwapChain()
{
}

void SwapchainManager::createImageViews()
{
}