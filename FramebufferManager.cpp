#include "FramebufferManager.h"

FramebufferManager::FramebufferManager(VkDevice device, VkRenderPass renderPass, const std::vector<VkImageView>& imageViews, VkExtent2D extent)
	: device(device) 
{
	createFramebuffers(renderPass, imageViews, extent);
}

FramebufferManager::FramebufferManager(VulkanContext &context)
{
	device = context.device;
	createFramebuffers(context.renderPass, context.swapchainImageViews, context.swapchainExtent);
	context.swapchainFrameBuffers = framebuffers;
}

FramebufferManager::~FramebufferManager()
{
	for (auto framebuffer : framebuffers) {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}
}

const std::vector<VkFramebuffer>& FramebufferManager::getFramebuffers() const
{
	return framebuffers;
}

void FramebufferManager::createFramebuffers(VkRenderPass renderPass, const std::vector<VkImageView>& imageViews, VkExtent2D extent)
{
	framebuffers.resize(imageViews.size());
	for (size_t i = 0; i < imageViews.size(); i++) {
		VkImageView attachments[] = { imageViews[i] };

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = extent.width;
		framebufferInfo.height = extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create framebuffer!");
		}
	}
}
