#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

#include <vulkan/vulkan.h>
#include <vector>

#include "core/VulkanContext.h"
#include "core/ResourceManager.h"

class RenderPassManager {
public:
    RenderPassManager();
    ~RenderPassManager();

    void createRenderPass(const std::vector<VkAttachmentDescription>& attachments, const std::vector<VkSubpassDescription>& subpasses, const std::vector<VkSubpassDependency>& dependencies);
    void destroyRenderPass(VkRenderPass renderPass);

    VkRenderPass createRenderPassColorDepth();

    std::vector<VkRenderPass>& getRenderPasses() { return renderPasses; }

private:
    VkDevice device;
    std::vector<VkRenderPass> renderPasses;
};

#endif
