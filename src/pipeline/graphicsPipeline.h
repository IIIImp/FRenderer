#ifndef GRAPHICSPIPELINE_H
#define GRAPHICSPIPELINE_H

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <stdexcept>

#include "core/VulkanContext.h"

class GraphicsPipeline {
public:
    class Builder {
    public:
        Builder& setPipelineLayout(const VkPipelineLayout pipelineLayout) {
            pipelineInfo_.layout = pipelineLayout;
            return *this;
        }
        Builder& setShaderStages(const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages) {
            pipelineInfo_.pStages = shaderStages.data();
            pipelineInfo_.stageCount = static_cast<uint32_t>(shaderStages.size());
            return *this;
        }

        Builder& setVertexInputState(const VkPipelineVertexInputStateCreateInfo& vertexInputState) {
            pipelineInfo_.pVertexInputState = &vertexInputState;
            return *this;
        }

        Builder& setInputAssemblyState(const VkPipelineInputAssemblyStateCreateInfo& inputAssemblyState) {
            pipelineInfo_.pInputAssemblyState = &inputAssemblyState;
            return *this;
        }

        Builder& setViewportState(const VkPipelineViewportStateCreateInfo& viewportState) {
            pipelineInfo_.pViewportState = &viewportState;
            return *this;
        }

        Builder& setRasterizationState(const VkPipelineRasterizationStateCreateInfo& rasterizationState) {
            pipelineInfo_.pRasterizationState = &rasterizationState;
            return *this;
        }

        Builder& setMultisampleState(const VkPipelineMultisampleStateCreateInfo& multisampleState) {
            pipelineInfo_.pMultisampleState = &multisampleState;
            return *this;
        }

        Builder& setColorBlendState(const VkPipelineColorBlendStateCreateInfo& colorBlendState) {
            pipelineInfo_.pColorBlendState = &colorBlendState;
            return *this;
        }

        Builder& setDynamicState(const std::vector<VkDynamicState>& dynamicState) {
            VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
            dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicState.size());
            dynamicStateInfo.pDynamicStates = dynamicState.data();
            pipelineInfo_.pDynamicState = &dynamicStateInfo;
            return *this;
        }

        Builder& setDepthStencil(const VkPipelineDepthStencilStateCreateInfo depthStencil) {
            pipelineInfo_.pDepthStencilState = &depthStencil;
            return *this;
        }

        Builder& setRenderPass(VkRenderPass renderPass) {
            pipelineInfo_.renderPass = renderPass;
            return *this;
        }

        Builder& setSubpass(uint32_t subpass) {
            pipelineInfo_.subpass = subpass;
            return *this;
        }

        GraphicsPipeline build() {
            pipelineInfo_.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo_.basePipelineHandle = VK_NULL_HANDLE;
            pipelineInfo_.basePipelineIndex = -1;
            pipelineInfo_.pDynamicState = nullptr;
            return GraphicsPipeline(pipelineInfo_);
        }

    private:
        VkGraphicsPipelineCreateInfo pipelineInfo_ = {};
    };

    GraphicsPipeline(const VkGraphicsPipelineCreateInfo& pipelineInfo)
        : pipelineInfo_(pipelineInfo) {
        device = VulkanContext::getVulkanContext().getContextData().device;
        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo_, nullptr, &pipeline_) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create graphics pipeline!");
        }
    }

    ~GraphicsPipeline() {
    }

    VkPipeline getPipeline() const { return pipeline_; }

private:
    VkGraphicsPipelineCreateInfo pipelineInfo_;
    VkPipeline pipeline_;
    VkDevice device;
    VkPipelineCache pipelineCache = VK_NULL_HANDLE;
};

#endif