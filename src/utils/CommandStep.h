#ifndef COMMANDSTEP_H
#define COMMANDSTEP_H

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include "utils/GUI/GUI.h"

class CommandStep {
public:
	virtual void execute(VkCommandBuffer commandBuffer) = 0;
	virtual ~CommandStep() = default;
};

class BindPipelineStep : public CommandStep {
	VkPipeline pipeline;
public:
	BindPipelineStep(VkPipeline pipe) : pipeline(pipe) {}
	void execute(VkCommandBuffer commandBuffer) override {
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	}
};

class GUIStep : public CommandStep {
	ImDrawData* drawData;
public:
	GUIStep(ImDrawData* drawData) : drawData(drawData) {}
	void execute(VkCommandBuffer commandBuffer) override {
		ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer);
	}
};

class BindVertexBuffers : public CommandStep {
	VkBuffer* vertexBuffers;
	VkDeviceSize* offsets;
public:
	BindVertexBuffers(VkBuffer* vertexBuffers, VkDeviceSize* offsets)
		: vertexBuffers(vertexBuffers), offsets(offsets) {};
	void execute(VkCommandBuffer commandBuffer) override {
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	}
};

class BindIndexBuffers : public CommandStep {
	VkBuffer indexBuffers;
public:
	BindIndexBuffers(VkBuffer indexBuffers)
		: indexBuffers(indexBuffers) {};
	void execute(VkCommandBuffer commandBuffer) override {
		vkCmdBindIndexBuffer(commandBuffer, indexBuffers, 0, VK_INDEX_TYPE_UINT32);
	}
};

class BindDescriptorSets : public CommandStep {
	VkPipelineLayout pipelineLayout;
	std::vector<VkDescriptorSet> descriptorSets;
public:
	BindDescriptorSets(VkPipelineLayout pipelineLayout, std::vector<VkDescriptorSet>& descriptorSets)
		: pipelineLayout(pipelineLayout), descriptorSets(descriptorSets) {};
	void execute(VkCommandBuffer commandBuffer) override {
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[0], 0, nullptr);
	}
};

class DrawIndexed : public CommandStep {
	uint32_t size;
public:
	DrawIndexed(uint32_t size)
		: size(size) {};
	void execute(VkCommandBuffer commandBuffer) override {
		vkCmdDrawIndexed(commandBuffer, size, 1, 0, 0, 0);
	}
};

class CommandPipeline {
private:
	std::vector<std::unique_ptr<CommandStep>> steps;
public:
	CommandPipeline() = default;
	CommandPipeline(const CommandPipeline&) = delete;
	CommandPipeline& operator=(const CommandPipeline&) = delete;
	CommandPipeline(CommandPipeline&&) = default;
	CommandPipeline& operator=(CommandPipeline&&) = default;
	void addStep(std::unique_ptr<CommandStep> step) {
		steps.push_back(std::move(step));
	}
	void execute(VkCommandBuffer commandBuffer) {
		for (auto& step : steps) {
			step->execute(commandBuffer);
		}
	}
};

#endif