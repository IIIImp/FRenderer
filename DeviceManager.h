#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include "vulkan/vulkan.h"
#include "FRender.h"

class DeviceManager {
public:
	DeviceManager(VulkanContext &context);
	~DeviceManager();

	VkPhysicalDevice getPhysicalDevice() const;
	VkDevice getLogicalDevice() const;
	VkQueue getGraphicsQueue() const;
	VkQueue getPresentQueue() const;
	QueueFamilyIndices getQueueFamilyIndices() const;

private:
	VkInstance instance;
	VkSurfaceKHR surface;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice logicalDevice = VK_NULL_HANDLE;

	QueueFamilyIndices queueFamilyIndices;
	VkQueue graphicsQueue = VK_NULL_HANDLE;
	VkQueue presentQueue = VK_NULL_HANDLE;

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	bool isDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

	void pickPhysicalDevice();
	void createLogicalDevice();
};


#endif
