#include "CommandBuffersManager.h"

CommandBufferManager::CommandBufferManager(VkDevice device, QueueFamilyIndices indices, size_t bufferCount) : device(device), indices(indices)
{
	createCommandPool();
	createCommandBuffers(bufferCount);
}

CommandBufferManager::CommandBufferManager(VulkanContext& context)
{
	device = context.device;
	indices = context.queueFamilyIndices;
	createCommandPool();
	createCommandBuffers(context.bufferCount);
	context.commandPool = commandPool;
	context.commandBuffers = commandBuffers;
}

CommandBufferManager::~CommandBufferManager()
{
	vkDestroyCommandPool(device, commandPool, nullptr);
	vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
}

VkCommandPool CommandBufferManager::getCommandPool()
{
	return commandPool;
}

std::vector<VkCommandBuffer> CommandBufferManager::getCommandBuffers() const
{
	return commandBuffers;
}


void CommandBufferManager::createCommandPool()
{
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = indices.graphicsFamily;
	poolInfo.flags = 0;

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create command pool!");
	}
}

void CommandBufferManager::createCommandBuffers(size_t bufferCount)
{
	commandBuffers.resize(bufferCount);

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(bufferCount);

	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate command buffers!");
	}
}
