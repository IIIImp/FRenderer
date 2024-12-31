#define GLFW_INCLUDE_VULKAN

#include "vulkan/vulkan.h"
#include <GLFW/glfw3.h>  
#include "InstanceManager.h"
#include "FRender.h"
#include "DeviceManager.h"
#include "SwapchainManager.h"
#include "PipelineManager.h"
#include "FramebufferManager.h"
#include "CommandBuffersManager.h"
#include "RenderManager.h"

InstanceManager& instanceManager = InstanceManager::getInstanceManager();
VkInstance instance = instanceManager.getVkInstance();     // Store the Vulkan instance
GLFWwindow* window = instanceManager.getWindow();
VkSurfaceKHR surface = instanceManager.getSurface();

DeviceManager deviceManager(instance, surface);
VkPhysicalDevice physicalDevice = deviceManager.getPhysicalDevice();
VkDevice device = deviceManager.getLogicalDevice();
VkQueue graphicsQueue = deviceManager.getGraphicsQueue();
VkQueue presentQueue = deviceManager.getPresentQueue();
QueueFamilyIndices queueFamilyIndices = deviceManager.getQueueFamilyIndices();

SwapchainManager swapchainManager(device, physicalDevice, window, surface, queueFamilyIndices);
VkSwapchainKHR swapChain = swapchainManager.getSwapchain();
VkFormat swapChainImageFormat = swapchainManager.getImageFormat();
VkExtent2D swapChainExtent = swapchainManager.getExtent();
std::vector<VkImageView> swapChainImageViews = swapchainManager.getImageView();

PipelineManager pipelineManager(device, swapChainExtent, swapChainImageFormat);
VkRenderPass renderPass = pipelineManager.getRenderPass();
VkPipelineLayout pipelineLayout = pipelineManager.getPipelineLayout();
VkPipeline graphicsPipeline = pipelineManager.getGraphicsPipeline();

FramebufferManager framebufferManager(device, renderPass, swapChainImageViews, swapChainExtent);
std::vector<VkFramebuffer> swapChainFramebuffers = framebufferManager.getFramebuffers();

CommandBufferManager commandBufferManager(device, queueFamilyIndices, 1);
VkCommandPool commandPool = commandBufferManager.getCommandPool();
std::vector<VkCommandBuffer> commandBuffers = commandBufferManager.getCommandBuffers();

VkSemaphore imageAvailableSemaphore;
VkSemaphore renderFinishedSemaphore;
VkFence inFlightFence;
size_t currentFrame = 0;

void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin record command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapChainExtent;

	VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	vkCmdDraw(commandBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to record command buffer!");
	}
}
void drawFrame() {
	vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, std::numeric_limits<uint64_t>::max());
	vkResetFences(device, 1, &inFlightFence);

	uint32_t imageIndex;
	vkAcquireNextImageKHR(device, swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
	recordCommandBuffer(commandBuffers[0], imageIndex);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[0];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	presentInfo.pResults = nullptr;
	vkQueuePresentKHR(presentQueue, &presentInfo);

	vkQueueWaitIdle(presentQueue);
}
void createSyncObjects() {
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
			vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create synchronization objects for a frame!");
		}
	}
}
void initVulkan() {
	createSyncObjects();
}
void mainLoop() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		drawFrame();
	}
	vkDeviceWaitIdle(device);
}
void cleanup() {
	// vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
	// vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
}

int main() {
	initVulkan();  // Initialize the Vulkan
	mainLoop();    // Loop until the window is closed
	cleanup();     // Clean the memory.
}