#include "GUI.h"

void GUI::initialize(GLFWwindow* window, VkRenderPass renderPass, uint32_t imageCount) {
	createImGuiDescriptorPool();
	initImGui(window, renderPass, imageCount);
}

void GUI::cleanup() {
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void GUI::createImGuiDescriptorPool()
{
    VkDescriptorPoolSize pool_sizes[] = {
{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
    pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    if (vkCreateDescriptorPool(VulkanContext::getVulkanContext().getContextData().device, &pool_info, nullptr, &imguiDescriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool for ImGui!");
    }
}

void GUI::initImGui(GLFWwindow* window, VkRenderPass renderPass, uint32_t imageCount)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // 启用键盘控制

    // 设置平台/渲染器绑定
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ContextData vulkanContextData = VulkanContext::getVulkanContext().getContextData();
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = vulkanContextData.instance;
    init_info.PhysicalDevice = vulkanContextData.physicalDevice;
    init_info.Device = vulkanContextData.device;
    init_info.Queue = vulkanContextData.graphicsQueue;
    init_info.DescriptorPool = imguiDescriptorPool;
    init_info.MinImageCount = imageCount;   // 与 Swapchain 设置一致
    init_info.ImageCount = imageCount;     // 与 Swapchain 设置一致
    init_info.RenderPass = renderPass;
    ImGui_ImplVulkan_Init(&init_info);
}

