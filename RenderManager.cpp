//#include "RenderManager.h"
//
//RenderManager::RenderManager(VkDevice device, VkQueue graphicsQueue, VkQueue presentQueue, VkSwapchainKHR swapchain, const std::vector<VkCommandBuffer>& commandBuffers, VkExtent2D swapchainExtent)
//{
//}
//
//RenderManager::~RenderManager()
//{
//	vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
//	vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
//}
//
//void RenderManager::drawFrame()
//{
//}
//
//void RenderManager::createSyncObjects()
//{
//	VkSemaphoreCreateInfo semaphoreInfo = {};
//	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
//	VkFenceCreateInfo fenceInfo = {};
//	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
//	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
//
//	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
//			vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
//			vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
//			throw std::runtime_error("Failed to create synchronization objects for a frame!");
//		}
//	}
//}
//
//void RenderManager::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
//{
//	VkCommandBufferBeginInfo beginInfo = {};
//	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
//	beginInfo.pInheritanceInfo = nullptr;
//
//	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
//		throw std::runtime_error("Failed to begin record command buffer!");
//	}
//
//	VkRenderPassBeginInfo renderPassInfo = {};
//	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//	renderPassInfo.renderPass = renderPass;
//	renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
//
//	renderPassInfo.renderArea.offset = { 0, 0 };
//	renderPassInfo.renderArea.extent = swapchainExtent;
//
//	VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
//	renderPassInfo.clearValueCount = 1;
//	renderPassInfo.pClearValues = &clearColor;
//
//	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
//
//	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
//
//	vkCmdDraw(commandBuffer, 3, 1, 0, 0);
//
//	vkCmdEndRenderPass(commandBuffer);
//
//	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
//		throw std::runtime_error("Failed to record command buffer!");
//	}
//}
