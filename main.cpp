#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE


#include <chrono>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <glm.hpp>

#include "app/window.h"
#include "src/core/VulkanContext.h"
#include "src/core/SwapchainManager.h"
#include "src/core/CommandBuffer.h"
#include "src/core/ResourceManager.h"
#include "src/pipeline/renderManager.h"
#include "src/pipeline/graphicsPipeline.h"
#include "src/model/modelReader.h"
#include "utils/GUI/GUI.h"
#include "src/app/camera.h"
#include <iostream>
#include "app/renderPipeline.h"
#include "app/renderObject.h"

void BlingPhongTest() {
	uint32_t width = 800, height = 600;
	Window window(width, height, "FRenderer");
	std::shared_ptr<Camera> camera = std::make_shared<Camera>(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);
	glfwSetWindowUserPointer(window.getWindow(), camera.get());
	glfwSetKeyCallback(window.getWindow(), Window::cameraMoveCallback);
	glfwSetWindowUserPointer(window.getWindow(), camera.get());
	glfwSetMouseButtonCallback(window.getWindow(), Window::mouseButtonCallback);
	glfwSetWindowUserPointer(window.getWindow(), camera.get());
	glfwSetCursorPosCallback(window.getWindow(), Window::mouseCallback);

	// Get Vulkan Context(instance, device, physicalDevice, queueInfo)
	VulkanContext& vulkanContext = VulkanContext::getVulkanContext();
	std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	vulkanContext.init(window, deviceExtensions);
	VkDevice device = vulkanContext.getContextData().device;

	// Create Swapchain with size of the window
	SwapchainManager swapchainManager(window.getSurface());
	swapchainManager.createSwapchain(width, height);
	uint32_t swapchainImageCount = static_cast<uint32_t>(swapchainManager.getSwapchainImages().size());

	// Allocate CommandBuffer
	CommandBuffer commandBuffer;
	commandBuffer.createCommandBuffers((uint32_t)swapchainManager.getSwapchainImages().size());

	// Create Resource Constructor(after context creation)
	ResourceManager& resourceManager = ResourceManager::getResourceManager();

	// Render Main Part
	// Configuration
	uint32_t renderObjectNum = 2;
	std::vector<VkFormat> candidates = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
	glm::vec3 modelRotation = glm::vec3(0.0f, 180.0f, 0.0f);
	lightInform lightInfo = {};
	lightInfo.lightPosition = glm::vec3(0.0f, 1.0f, -1.0f);
	lightInfo.lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	lightInfo.viewPos = glm::vec3(0.0f, 0.0f, 5.0f);
	lightInfo.intensity = 1.0f;

	camera->position = (glm::vec3(0.0f, 0.0f, -5.0f));
	camera->front = (glm::vec3(0.0f, 0.0f, 1.0f));
	camera->up = (glm::vec3(0.0f, 1.0f, 0.0f));
	camera->yaw = 90.0f;

	// DescriptorPool
	std::vector<VkDescriptorPoolSize> poolSize = {
	{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, swapchainImageCount * renderObjectNum * 2},
	{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, swapchainImageCount * renderObjectNum}
	};
	VkDescriptorPool descriptorPool = resourceManager.createDescriptorPool(swapchainImageCount * renderObjectNum, poolSize);

	// DescriptorLayout VertexUBO * 1, Sampler * 1, FragmentUBO * 1
	std::vector<VkDescriptorSetLayoutBinding> bindings = {
		{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr},
		{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr},
		{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr}
	};
	VkDescriptorSetLayout descriptorSetLayout = resourceManager.createDescriptorSetLayout(bindings);

	// VertexUBO and FragmentUBO allocator
	std::vector<BufferInfo> rotationBuffers;
	std::vector<BufferInfo> lightBuffers;
	for (uint32_t i = 0; i < swapchainImageCount; i++) {
		rotationBuffers.push_back(resourceManager.createBuffer(sizeof(rotationMatrix), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
		lightBuffers.push_back(resourceManager.createBuffer(sizeof(lightInform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
	}

	// Initialize Object1
	std::vector<std::string> texturePath = { "assets/textures/Duck.png" };
	RenderObject DuckBlingPhong("assets/models/Duck.obj", texturePath, descriptorPool, descriptorSetLayout, swapchainImageCount);
	// Update DescriptorSets
	VkSampler textureSampler = DuckBlingPhong.renderObjectInfo.textures[0].textureSampler;
	VkImageView textureImageView = DuckBlingPhong.renderObjectInfo.textures[0].textureImageView;
	for (uint32_t i = 0; i < swapchainImageCount; i++) {
		VkDescriptorBufferInfo rotation = { rotationBuffers[i].buffer, 0, sizeof(rotationMatrix) };
		VkDescriptorImageInfo textureInfo = { textureSampler, textureImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		VkDescriptorBufferInfo lightInfo = { lightBuffers[i].buffer, 0, sizeof(lightInform) };
		std::vector<DescriptorUpdateInfo> descriptorUpdateInfos = {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, rotation, {}},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, {}, textureInfo},
			{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, lightInfo, {}}
		};
		DuckBlingPhong.updateDescriptorSets(descriptorUpdateInfos, i);
	}
	std::vector<VkDescriptorSet> descriptorSets = DuckBlingPhong.renderObjectInfo.descriptorSets;

	// Initialize Floor
	std::vector<Vertex> floorVertices = {
	{{-5.0f, -1.0f, -5.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{ 5.0f, -1.0f, -5.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{ 5.0f, -1.0f,  5.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{-5.0f, -1.0f,  5.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}
	};
	std::vector<uint32_t> floorIndices = { 0, 1, 2, 2, 3, 0 };
	RenderObject floor(floorVertices, floorIndices);

	// Pipeline Initialize
	RenderPassManager renderPassManager;
	VkRenderPass renderpass = renderPassManager.createRenderPassColorDepth();

	RenderPipeline renderPipeline("assets/shaders/vert.spv", "assets/shaders/frag.spv",
		descriptorSetLayout, renderpass);
	VkPipeline pipeline = renderPipeline.renderPipeline.graphicsPipeline;
	VkPipelineLayout pipelineLayout = renderPipeline.renderPipeline.pipelineLayout;

	// FrameBuffers and Attachments
	VkImage depthImage = resourceManager.createImage(width, height, resourceManager.findDepthFormat(candidates),
		VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VkImageView depthImageView = resourceManager.createImageView(depthImage, resourceManager.findDepthFormat(candidates), VK_IMAGE_ASPECT_DEPTH_BIT);
	std::vector<VkFramebuffer> frameBuffers;
	for (uint32_t i = 0; i < swapchainImageCount; i++) {
		std::vector<VkImageView> frameBufferAttachments = { swapchainManager.getSwapchainImageViews()[i], depthImageView };
		frameBuffers.push_back(resourceManager.createFramebuffers(frameBufferAttachments, renderpass, width, height, 1));
	}

	// Synchronzation Objects allocator
	uint32_t MAX_FRAMES_IN_FLIGHT = 3;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		imageAvailableSemaphores.push_back(resourceManager.createSemaphore());
		renderFinishedSemaphores.push_back(resourceManager.createSemaphore());
		inFlightFences.push_back(resourceManager.createFence());
	}

	GUI imGUI;
	imGUI.initialize(window.getWindow(), renderpass, swapchainImageCount);

	uint32_t currentFrame = 0;
	while (!window.shouldClose()) {

		window.pollEvents();
		uint32_t imageIndex;
		vkAcquireNextImageKHR(vulkanContext.getContextData().device, swapchainManager.getSwapchain(), std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		rotationMatrix ubo = {};
		ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(modelRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.model = glm::rotate(ubo.model, glm::radians(modelRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		ubo.model = glm::rotate(ubo.model, glm::radians(modelRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		ubo.view = camera->getViewMatrix();
		ubo.proj = camera->getProjectionMatrix();
		ubo.proj[1][1] *= -1;

		void* data;
		lightInfo.viewPos = camera->position;
		vkMapMemory(device, lightBuffers[imageIndex].bufferMemory, 0, sizeof(lightInfo), 0, &data);
		memcpy(data, &lightInfo, sizeof(lightInfo));
		vkUnmapMemory(device, lightBuffers[imageIndex].bufferMemory);
		vkMapMemory(device, rotationBuffers[imageIndex].bufferMemory, 0, sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, rotationBuffers[imageIndex].bufferMemory);
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(commandBuffer.getCommandBuffer(imageIndex), &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin record command buffer!");
		}

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)(width), (float)(height));
		ImGui::NewFrame();
		ImGui::Begin("Performance");
		ImGui::SliderFloat3("Model Rotation (XYZ)", glm::value_ptr(modelRotation), -180.0f, 180.0f);
		ImGui::SliderFloat("Light Intensity", &lightInfo.intensity, 0.0f, 1.0f);
		ImGui::SliderFloat3("Light Position", glm::value_ptr(lightInfo.lightPosition), -10.0f, 10.0f);
		ImGui::End();
		ImGui::Render();

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderpass;
		renderPassInfo.framebuffer = frameBuffers[imageIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchainManager.getSwapchainExtent();

		std::vector<VkClearValue> clearValues(2);
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer.getCommandBuffer(imageIndex), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkBuffer vertexBuffers[] = { DuckBlingPhong.renderObjectInfo.vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindPipeline(commandBuffer.getCommandBuffer(imageIndex), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		vkCmdBindVertexBuffers(commandBuffer.getCommandBuffer(imageIndex), 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer.getCommandBuffer(imageIndex), DuckBlingPhong.renderObjectInfo.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(commandBuffer.getCommandBuffer(imageIndex), VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[imageIndex], 0, nullptr);
		uint32_t size = static_cast<uint32_t>(DuckBlingPhong.renderObjectInfo.indices.size());
		vkCmdDrawIndexed(commandBuffer.getCommandBuffer(imageIndex), size, 1, 0, 0, 0);

		VkBuffer floorVertexBuffers[] = { floor.renderObjectInfo.vertexBuffer };
		vkCmdBindVertexBuffers(commandBuffer.getCommandBuffer(imageIndex), 0, 1, floorVertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer.getCommandBuffer(imageIndex), floor.renderObjectInfo.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		size = static_cast<uint32_t>(floor.renderObjectInfo.indices.size());
		vkCmdDrawIndexed(commandBuffer.getCommandBuffer(imageIndex), size, 1, 0, 0, 0);

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer.getCommandBuffer(imageIndex));

		vkCmdEndRenderPass(commandBuffer.getCommandBuffer(imageIndex));

		if (vkEndCommandBuffer(commandBuffer.getCommandBuffer(imageIndex)) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer!");
		}
		commandBuffer.submitCommands(imageAvailableSemaphores[currentFrame], renderFinishedSemaphores[currentFrame], imageIndex);
		commandBuffer.presentCommands(renderFinishedSemaphores[currentFrame], swapchainManager.getSwapchain(), imageIndex);

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
		vkDeviceWaitIdle(device);
	}

	imGUI.cleanup();
	for (uint32_t i = 0; i < swapchainImageCount; i++) {
		resourceManager.destroySemaphore(renderFinishedSemaphores[i]);
		resourceManager.destroySemaphore(imageAvailableSemaphores[i]);
		resourceManager.destroyFence(inFlightFences[i]);
		resourceManager.destroyFrameBuffer(frameBuffers[i]);
	}
	resourceManager.destroyImageView(depthImageView);
	resourceManager.destroyImage(depthImage);
	for (auto renderPass : renderPassManager.getRenderPasses()) {
		vkDestroyRenderPass(device, renderPass, nullptr);
	}
	vulkanContext.cleanup();

}

int main() {
	// Initializations for Vulkan and Windows
	// Window Settings
	uint32_t width = 800, height = 600;
	Window window(width, height, "FRenderer");
	std::shared_ptr<Camera> camera = std::make_shared<Camera>(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);
	glfwSetWindowUserPointer(window.getWindow(), camera.get());
	glfwSetKeyCallback(window.getWindow(), Window::cameraMoveCallback);
	glfwSetWindowUserPointer(window.getWindow(), camera.get());
	glfwSetMouseButtonCallback(window.getWindow(), Window::mouseButtonCallback);
	glfwSetWindowUserPointer(window.getWindow(), camera.get());
	glfwSetCursorPosCallback(window.getWindow(), Window::mouseCallback);

	// Get Vulkan Context(instance, device, physicalDevice, queueInfo)
	VulkanContext& vulkanContext = VulkanContext::getVulkanContext();
	std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	vulkanContext.init(window, deviceExtensions);
	VkDevice device = vulkanContext.getContextData().device;

	// Create Swapchain with size of the window
	SwapchainManager swapchainManager(window.getSurface());
	swapchainManager.createSwapchain(width, height);
	uint32_t swapchainImageCount = static_cast<uint32_t>(swapchainManager.getSwapchainImages().size());

	// Allocate CommandBuffer
	CommandBuffer commandBuffer;
	commandBuffer.createCommandBuffers((uint32_t)swapchainManager.getSwapchainImages().size());

	// Create Resource Constructor(after context creation)
	ResourceManager& resourceManager = ResourceManager::getResourceManager();
	
	// Render Main Part
	// Configuration
	std::vector<VkFormat> candidates = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
	glm::vec3 modelRotation = glm::vec3(-90.0f, 180.0f, 0.0f);
	lightInform lightInfo = {};
	lightInfo.lightPosition = glm::vec3(0.0f, 1.0f, -1.0f);
	lightInfo.lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	lightInfo.viewPos = glm::vec3(0.0f, 0.0f, 5.0f);
	lightInfo.intensity = 100.0f;

	camera->position = (glm::vec3(0.0f, 0.0f, -1.0f));
	camera->front = (glm::vec3(0.0f, 0.0f, 1.0f));
	camera->up = (glm::vec3(0.0f, 1.0f, 0.0f));
	camera->yaw = 90.0f;

	// DescriptorPool
	std::vector<VkDescriptorPoolSize> poolSize = {
	{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 30},
	{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 30}
	};
	VkDescriptorPool descriptorPool = resourceManager.createDescriptorPool(swapchainImageCount * 5, poolSize);

	// VertexUBO and FragmentUBO allocator
	std::vector<BufferInfo> rotationBuffers;
	std::vector<BufferInfo> lightBuffers;
	for (uint32_t i = 0; i < swapchainImageCount; i++) {
		rotationBuffers.push_back(resourceManager.createBuffer(sizeof(rotationMatrix), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
		lightBuffers.push_back(resourceManager.createBuffer(sizeof(lightInform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
	}

	RenderObject gun = RenderObject("assets/models/scene.gltf", descriptorPool, swapchainImageCount);
	VkSampler textureSampler = resourceManager.createGunSampler();

	std::vector<VkDescriptorSet> descriptorSets = gun.renderMultiObjectInfo.descriptorSets;
	VkDescriptorSetLayout descriptorSetLayout = gun.renderMultiObjectInfo.descriptorSetLayout;

	// Pipeline Initialize
	RenderPassManager renderPassManager;
	VkRenderPass renderpass = renderPassManager.createRenderPassColorDepth();

	RenderPipeline renderPipeline("assets/shaders/vert.spv", "assets/shaders/frag.spv",
		descriptorSetLayout, renderpass);
	VkPipeline pipeline = renderPipeline.renderPipeline.graphicsPipeline;
	VkPipelineLayout pipelineLayout = renderPipeline.renderPipeline.pipelineLayout;

	// FrameBuffers and Attachments
	VkImage depthImage = resourceManager.createImage(width, height, resourceManager.findDepthFormat(candidates),
		VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VkImageView depthImageView = resourceManager.createImageView(depthImage, resourceManager.findDepthFormat(candidates), VK_IMAGE_ASPECT_DEPTH_BIT);
	std::vector<VkFramebuffer> frameBuffers;
	for (uint32_t i = 0; i < swapchainImageCount; i++) {
		std::vector<VkImageView> frameBufferAttachments = { swapchainManager.getSwapchainImageViews()[i], depthImageView };
		frameBuffers.push_back(resourceManager.createFramebuffers(frameBufferAttachments, renderpass, width, height, 1));
	}

	// Synchronzation Objects allocator
	uint32_t MAX_FRAMES_IN_FLIGHT = 3;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		imageAvailableSemaphores.push_back(resourceManager.createSemaphore());
		renderFinishedSemaphores.push_back(resourceManager.createSemaphore());
		inFlightFences.push_back(resourceManager.createFence());
	}

	GUI imGUI;
	imGUI.initialize(window.getWindow(), renderpass, swapchainImageCount);

	uint32_t currentFrame = 0;
	while (!window.shouldClose()) {

		window.pollEvents();
		uint32_t imageIndex;
		vkAcquireNextImageKHR(vulkanContext.getContextData().device, swapchainManager.getSwapchain(), std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		rotationMatrix ubo = {};
		ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(modelRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.model = glm::rotate(ubo.model, glm::radians(modelRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		ubo.model = glm::rotate(ubo.model, glm::radians(modelRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		ubo.view = camera->getViewMatrix();
		ubo.proj = camera->getProjectionMatrix();
		ubo.proj[1][1] *= -1;
		void* data;
		lightInfo.viewPos = camera->position;
		vkMapMemory(device, lightBuffers[imageIndex].bufferMemory, 0, sizeof(lightInfo), 0, &data);
		memcpy(data, &lightInfo, sizeof(lightInfo));
		vkUnmapMemory(device, lightBuffers[imageIndex].bufferMemory);
		vkMapMemory(device, rotationBuffers[imageIndex].bufferMemory, 0, sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, rotationBuffers[imageIndex].bufferMemory);
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(commandBuffer.getCommandBuffer(imageIndex), &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin record command buffer!");
		}

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)(width), (float)(height));
		ImGui::NewFrame();
		ImGui::Begin("Performance");
		ImGui::SliderFloat3("Model Rotation (XYZ)", glm::value_ptr(modelRotation), -180.0f, 180.0f);
		ImGui::SliderFloat("Light Intensity", &lightInfo.intensity, 0.0f, 100.0f);
		ImGui::SliderFloat3("Light Position", glm::value_ptr(lightInfo.lightPosition), -10.0f, 10.0f);
		ImGui::End();
		ImGui::Render();

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderpass;
		renderPassInfo.framebuffer = frameBuffers[imageIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchainManager.getSwapchainExtent();

		std::vector<VkClearValue> clearValues(2);
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer.getCommandBuffer(imageIndex), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		
		for (size_t i = 0; i < gun.renderMultiObjectInfo.indices.size(); i++) {
			vkCmdBindPipeline(commandBuffer.getCommandBuffer(imageIndex), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
			VkBuffer vertexBuffers[] = { gun.renderMultiObjectInfo.vertexBuffer[i] };
			VkDeviceSize offsets[] = { 0 };
			VkDescriptorBufferInfo rotation = { rotationBuffers[imageIndex].buffer, 0, sizeof(rotationMatrix) };
			VkDescriptorImageInfo baseColorTextureInfo = {
				textureSampler, gun.renderMultiObjectInfo.textures[i].baseColorTexture.textureImageView,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
			VkDescriptorBufferInfo lightInfo = { lightBuffers[imageIndex].buffer, 0, sizeof(lightInform) };
			VkDescriptorImageInfo emissiveTextureInfo = {
				textureSampler, gun.renderMultiObjectInfo.textures[i].emissiveTexture.textureImageView,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
			VkDescriptorImageInfo metallicRoughnessTextureInfo = {
				textureSampler, gun.renderMultiObjectInfo.textures[i].metallicRoughnessTexture.textureImageView,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
			VkDescriptorImageInfo normalTextureInfo = {
				textureSampler, gun.renderMultiObjectInfo.textures[i].normalTexture.textureImageView,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
			std::vector<DescriptorUpdateInfo> descriptorUpdateInfos = {
				{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, rotation, {}},
				{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, {}, baseColorTextureInfo},
				{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, lightInfo, {}},
				{3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, {}, emissiveTextureInfo},
				{4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, {}, metallicRoughnessTextureInfo},
				{5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, {}, normalTextureInfo}
			};
			gun.updatePBRDescriptorSets(descriptorUpdateInfos, imageIndex);
			vkCmdBindVertexBuffers(commandBuffer.getCommandBuffer(imageIndex), 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(commandBuffer.getCommandBuffer(imageIndex), gun.renderMultiObjectInfo.indexBuffer[i], 0, VK_INDEX_TYPE_UINT32);
			vkCmdBindDescriptorSets(commandBuffer.getCommandBuffer(imageIndex), VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[imageIndex], 0, nullptr);
			uint32_t size = static_cast<uint32_t>(gun.renderMultiObjectInfo.indices[i].size());
			vkCmdDrawIndexed(commandBuffer.getCommandBuffer(imageIndex), size, 1, 0, 0, 0);
		}

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer.getCommandBuffer(imageIndex));

		vkCmdEndRenderPass(commandBuffer.getCommandBuffer(imageIndex));





		if (vkEndCommandBuffer(commandBuffer.getCommandBuffer(imageIndex)) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer!");
		}
		commandBuffer.submitCommands(imageAvailableSemaphores[currentFrame], renderFinishedSemaphores[currentFrame], imageIndex);
		commandBuffer.presentCommands(renderFinishedSemaphores[currentFrame], swapchainManager.getSwapchain(), imageIndex);

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
		vkDeviceWaitIdle(device);
	}

	imGUI.cleanup();
	for (uint32_t i = 0; i < swapchainImageCount; i++) {
		resourceManager.destroySemaphore(renderFinishedSemaphores[i]);
		resourceManager.destroySemaphore(imageAvailableSemaphores[i]);
		resourceManager.destroyFence(inFlightFences[i]);
		resourceManager.destroyFrameBuffer(frameBuffers[i]);
	}
	resourceManager.destroyImageView(depthImageView);
	resourceManager.destroyImage(depthImage);
	for (auto renderPass : renderPassManager.getRenderPasses()) {
		vkDestroyRenderPass(device, renderPass, nullptr);
	}
	vulkanContext.cleanup();

	return 0;
}