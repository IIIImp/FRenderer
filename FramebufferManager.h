#ifndef FRAMEBUFFERMANAGER_H
#define FRAMEBUFFERMANAGER_H

#include "vulkan/vulkan.h"
#include "FRender.h"

class FramebufferManager {
public:
    FramebufferManager(VkDevice device, VkRenderPass renderPass, const std::vector<VkImageView>& imageViews, VkExtent2D extent);
    ~FramebufferManager();

    const std::vector<VkFramebuffer>& getFramebuffers() const;

private:
    VkDevice device = VK_NULL_HANDLE;
    std::vector<VkFramebuffer> framebuffers;

    void createFramebuffers(VkRenderPass renderPass, const std::vector<VkImageView>& imageViews, VkExtent2D extent);
};

#endif // FRAMEBUFFERMANAGER_H
