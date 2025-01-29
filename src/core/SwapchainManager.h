#ifndef SWAPCHAINMANAGER_H
#define SWAPCHAINMANAGER_H

#include "utils/structUtils.h"
#include "core/VulkanContext.h"

#include <vulkan/vulkan.h>
#include <vector>

class SwapchainManager {
public:
    SwapchainManager(VkSurfaceKHR surface);
    ~SwapchainManager();

    void createSwapchain(uint32_t width, uint32_t height);

    VkSwapchainKHR getSwapchain()            const { return swapchainData.swapchain; }
    VkFormat       getSwapchainImageFormat() const { return swapchainData.imageFormat; }
    VkExtent2D     getSwapchainExtent()      const { return swapchainData.extent; }
    const std::vector<VkImage>&     getSwapchainImages()     const { return swapchainImages.images; }
    const std::vector<VkImageView>& getSwapchainImageViews() const { return swapchainImages.imageViews; }



    void recreateSwapchain(uint32_t width, uint32_t height);

    void cleanup();

private:
    void createImageViews();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);

    VkDevice device = VulkanContext::getVulkanContext().getContextData().device;
    VkPhysicalDevice physicalDevice = VulkanContext::getVulkanContext().getContextData().physicalDevice;
    VkSurfaceKHR surface;

    SwapchainData swapchainData;
    SwapchainImages swapchainImages;
};

#endif