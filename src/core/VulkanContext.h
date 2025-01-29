#ifndef VULKANCONTEXT_H
#define VULKANCONTEXT_H

#include "app/window.h"
#include "utils/structUtils.h"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <vector>


class VulkanContext {
public:
    // Singleton
    static VulkanContext& getVulkanContext() {
        static VulkanContext instance;
        return instance;
    }
    //Disable copy and =
    VulkanContext(const VulkanContext&) = delete;
    VulkanContext& operator=(const VulkanContext&) = delete;
    // Manage context
    void init(Window& window, std::vector<const char*>& deviceExtensions);
    void cleanup();

    const ContextData& getContextData() const { return contextData; }
    const QueueFamilyIndices& getQueueFamilyIndices() const { return queueFamilyIndices; }
    const SwapChainSupportDetails& getSwapchainSupportDetails() const { return swapchainSupportDetails; }
    
private:
    VulkanContext() {}

    ContextData contextData = {};
    QueueFamilyIndices queueFamilyIndices = {};
    SwapChainSupportDetails swapchainSupportDetails = {};

    void createInstance();
    void pickPhysicalDevice(Window& window, std::vector<const char*>& deviceExtensions);
    void createLogicalDevice(Window& window, std::vector<const char*>& deviceExtensions);

    bool isDeviceSuitable(VkPhysicalDevice device, Window& window, std::vector<const char*>& deviceExtensions);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, Window& window);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device, std::vector<const char*>& deviceExtensions);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, Window& window);
};

#endif