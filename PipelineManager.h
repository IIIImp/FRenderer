#ifndef PIPELINEMANAGER_H
#define PIPELINEMANAGER_H

#include "FRender.h"
#include "vulkan/vulkan.h"

class PipelineManager {
public:
	PipelineManager(VkDevice device, VkExtent2D extent, VkFormat format);
	~PipelineManager();

	VkPipeline getGraphicsPipeline() const;
	VkPipelineLayout getPipelineLayout() const;
	VkRenderPass getRenderPass() const;

private:
	VkDevice device = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline = VK_NULL_HANDLE;
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

	VkRenderPass renderPass = VK_NULL_HANDLE;
	VkExtent2D extent = {};
	VkFormat format = {};

	void createRenderPass();
	void createGraphicsPipeline();
	VkShaderModule createShaderModule(const std::vector<char>& code);
};

#endif