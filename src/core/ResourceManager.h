#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>

#include "core/VulkanContext.h"
#include "core/CommandBuffer.h"


class ResourceManager {
public:
    static ResourceManager& getResourceManager() {
        static ResourceManager resourceManager;
        return resourceManager;
    }

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    VkFramebuffer createFramebuffers(std::vector<VkImageView>& attachments, VkRenderPass renderPass, uint32_t width, uint32_t height, uint32_t layers);
    void destroyFrameBuffer(VkFramebuffer frameBuffer);

    BufferInfo createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    void destroyBuffer(VkBuffer buffer);

    template<typename T>
    inline BufferInfo createGPUBuffer(std::vector<T>& data, VkBufferUsageFlags bufferUsage)
    {
        VkDeviceSize bufferSize = sizeof(data[0]) * data.size();

        BufferInfo stagingBuffer = createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        void* temp;
        vkMapMemory(device, stagingBuffer.bufferMemory, 0, bufferSize, 0, &temp);
        memcpy(temp, data.data(), (size_t)bufferSize);
        vkUnmapMemory(device, stagingBuffer.bufferMemory);
        BufferInfo indexBuffer = createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | bufferUsage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        copyBuffer(stagingBuffer.buffer, indexBuffer.buffer, bufferSize);

        vkDestroyBuffer(device, stagingBuffer.buffer, nullptr);
        vkFreeMemory(device, stagingBuffer.bufferMemory, nullptr);
        return indexBuffer;
    }

    void destroyGPUBuffer(VkBuffer buffer);

    VkImage createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
    void destroyImage(VkImage image);

    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectMask);
    void destroyImageView(VkImageView imageView);

    ImageInfo createImageInfo(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
    ImageInfo createMipImageInfo(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
    TextureInfo loadTexture(const std::string& filepath);
    TextureInfo loadsRGBTexture(const std::string& filepath);
    TextureInfo loadMipMapTexture(const std::string& filepath);
    TextureInfo loadMipMapsRGBTexture(const std::string& filepath);
    void destroyTexture(VkImageView texture);
    VkSampler createTextureSampler();
    VkSampler createGunSampler();
    void destroySampler(VkSampler sampler);

    VkShaderModule loadShader(const std::string& filepath);
    void destroyShader(VkShaderModule shader);

    VkDescriptorSetLayout createDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
    void destroyDescriptorSetLayout(VkDescriptorSetLayout layout);

    VkDescriptorPool createDescriptorPool (uint32_t maxSets, const std::vector<VkDescriptorPoolSize>& poolSizes)const;
    void destroyDescriptorPool(VkDescriptorPool pool);

    VkDescriptorSet allocateDescriptorSet(VkDescriptorSetLayout layout, VkDescriptorPool pool);
    void updateDescriptorSet(const std::vector<VkWriteDescriptorSet>& descriptorWrites);

    VkFormat findDepthFormat(std::vector<VkFormat>& candidates);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    
    VkSemaphore createSemaphore();
    VkFence createFence();
    void destroySemaphore(VkSemaphore semaphore);
    void destroyFence(VkFence fence);
private:
    VkDevice device;

    ResourceManager() { device = VulkanContext::getVulkanContext().getContextData().device; }
    const uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void transitionImageResource(VkImage image, VkFormat depthFormat, VkImageLayout oldLayout, VkImageLayout newLayout);
    
    void transitionMipImageResource(VkImage image, uint32_t mipLevels, VkFormat imageFormat, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
};

#endif


