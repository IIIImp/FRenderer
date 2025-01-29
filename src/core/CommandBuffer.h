#ifndef COMMANDBUFFER_H
#define COMMANDBUFFER_H

#include <vulkan/vulkan.h>
#include <vector>

#include "VulkanContext.h"
#include "utils/CommandStep.h"

class CommandBuffer {
public:
	CommandBuffer();
	~CommandBuffer();

    void initializeCommandPool();

    void createCommandBuffers(uint32_t count);

    static VkCommandBuffer beginRecording(VkCommandBufferUsageFlags usageFlags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
    void recordRenderCommands(VkFramebuffer framebuffer, VkRenderPass renderPass, VkPipeline pipeline, VkExtent2D extent, CommandPipeline& commandPipeline, uint32_t index);
    static void endRecording(VkCommandBuffer commandBuffer);

    void submitCommands(VkSemaphore waitSemaphore = VK_NULL_HANDLE, VkSemaphore signalSemaphore = VK_NULL_HANDLE, uint32_t index = 0);
    void presentCommands(VkSemaphore signalSemaphore, VkSwapchainKHR swapchain, uint32_t imageIndex);
    void resetCommandBuffer(uint32_t index);

    VkCommandBuffer getCommandBuffer(uint32_t index) const { return commandBuffers[index]; };

private:
    VulkanContext& context;
    VkDevice device;
    static VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    VkSemaphore waitSemaphore;
    VkSemaphore signalSemaphore;
};

#endif