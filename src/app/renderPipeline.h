#ifndef RENDERPIPELINE_H
#define RENDERPIPELINE_H

#include <vulkan/vulkan.h>
#include <string>
#include "core/VulkanContext.h"
#include "utils/structUtils.h"
#include "core/ResourceManager.h"
#include "pipeline/graphicsPipeline.h"

class RenderPipeline {
public:
    RenderPipeline(const std::string& vertexShaderPath,
        const std::string& fragmentShaderPath,
        const VkDescriptorSetLayout& descriptorSetLayout,
        const VkRenderPass& renderPass);

    
    VkPipeline createPipelineBlingPhong(const std::string& vertexShaderPath,
        const std::string& fragmentShaderPath,
        const VkRenderPass& renderPass);

    void cleanup();
    RenderPipelineInfo renderPipeline;
};


#endif