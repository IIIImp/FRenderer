//#ifndef RENDERMANAGER_H
//#define RENDERMANAGER_H
//
//#include "vulkan/vulkan.h"
//#include "FRender.h"
//
//class RenderManager {
//public:
//    RenderManager(VkDevice device, VkQueue graphicsQueue, VkQueue presentQueue, VkSwapchainKHR swapchain,
//        const std::vector<VkCommandBuffer>& commandBuffers, VkExtent2D swapchainExtent,
//        VkRenderPass renderPass);
//    ~RenderManager();
//
//    void drawFrame();
//
//private:
//    VkDevice device;
//    VkQueue graphicsQueue;
//    VkQueue presentQueue;
//    VkSwapchainKHR swapchain;
//    std::vector<VkCommandBuffer> commandBuffers;
//    VkExtent2D swapchainExtent;
//
//    VkSemaphore imageAvailableSemaphore;
//    VkSemaphore renderFinishedSemaphore;
//    VkFence inFlightFence;
//
//    uint32_t currentFrame = 0;
//    const int MAX_FRAMES_IN_FLIGHT = 2;
//
//    void createSyncObjects();
//    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
//};
//
//#endif // RENDERMANAGER_H
