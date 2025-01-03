#ifndef COMMANDBUFFERSMANAGER_H
#define COMMANDBUFFERSMANAGER_H

#include "vulkan/vulkan.h"
#include "FRender.h"

class CommandBufferManager {
public:
	CommandBufferManager(VkDevice device, QueueFamilyIndices indices, size_t bufferCount);
	CommandBufferManager(VulkanContext& context);
	~CommandBufferManager();

	VkCommandPool getCommandPool();
	std::vector<VkCommandBuffer> getCommandBuffers() const;

private:
	VkDevice device = VK_NULL_HANDLE;
	VkCommandPool commandPool = VK_NULL_HANDLE;
	std::vector<VkCommandBuffer> commandBuffers;
	QueueFamilyIndices indices;

	void createCommandPool();
	void createCommandBuffers(size_t bufferCount);
};

#endif