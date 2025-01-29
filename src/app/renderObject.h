#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include <vulkan/vulkan.h>
#include <vector>
#include "utils/structUtils.h"
#include "model/modelReader.h"
#include "core/ResourceManager.h"
#include <string>

class RenderObject {
public:
    RenderObject(const std::string& modelPath, 
        const std::vector<std::string>& texturePaths,
        const VkDescriptorPool& descriptorpool,
        const VkDescriptorSetLayout& descriptorSetLayout,
        const uint32_t swapChainImageCount);
    RenderObject(const std::vector<Vertex> vertices,
        const std::vector<uint32_t> indices);
    RenderObject(const std::string& modelPath,
        const VkDescriptorPool& descriptorpool,
        const uint32_t swapchainImageCount);

    void updateDescriptorSets(const std::vector<DescriptorUpdateInfo>& descriptorUpdateInfos, const uint32_t dstSetIndex);
    void updatePBRDescriptorSets(const std::vector<DescriptorUpdateInfo>& descriptorUpdateInfos, const uint32_t dstSetIndex);
    void cleanup();

    renderObject renderObjectInfo;
    renderMultiObject renderMultiObjectInfo;
};

#endif