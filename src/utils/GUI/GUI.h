#ifndef GUI_H
#define GUI_H

#include "utils/ImGui/imconfig.h"
#include "utils/ImGui/imgui.h"
#include "utils/ImGui/imgui_impl_vulkan.h"
#include "utils/ImGui/imgui_impl_glfw.h"
#include "utils/ImGui/imgui_internal.h"
#include "utils/ImGui/imstb_rectpack.h"
#include "utils/ImGui/imstb_textedit.h"
#include "utils/ImGui/imstb_truetype.h"
#include "core/VulkanContext.h"

#include <vulkan/vulkan.h>

class GUI {
public:

	void initialize(GLFWwindow* window, VkRenderPass renderPass, uint32_t imageCount);
	void cleanup();
private:
	VkDescriptorPool imguiDescriptorPool;

	void createImGuiDescriptorPool();
	void initImGui(GLFWwindow* window, VkRenderPass renderPass, uint32_t imageCount);
	ImDrawData* getDrawData();
};

#endif
