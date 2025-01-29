#include "renderObject.h"
#include <iostream>

RenderObject::RenderObject(const std::string& modelPaths,
	const std::vector<std::string>& texturePaths,
	const VkDescriptorPool& descriptorpool, 
	const VkDescriptorSetLayout& descriptorSetLayout,
	const uint32_t swapChainImageCount)
{
	Model model;
	model.loadModel(modelPaths);
	this->renderObjectInfo.vertices = model.getVertices();
	this->renderObjectInfo.indices = model.getIndices();

	ResourceManager& resourceManager = ResourceManager::getResourceManager();
	BufferInfo vertexBufferInfo = resourceManager.createGPUBuffer(
		this->renderObjectInfo.vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
	);
	BufferInfo indiceBufferInfo = resourceManager.createGPUBuffer(
		this->renderObjectInfo.indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT
	);
	this->renderObjectInfo.vertexBuffer = vertexBufferInfo.buffer;
	this->renderObjectInfo.vertexBufferMemory = vertexBufferInfo.bufferMemory;
	this->renderObjectInfo.indexBuffer = indiceBufferInfo.buffer;
	this->renderObjectInfo.indexBufferMemory = indiceBufferInfo.bufferMemory;
	
	for (auto texturePath : texturePaths) {
		this->renderObjectInfo.textures.push_back(resourceManager.loadTexture(texturePath));
	}
	for (auto &texture : this->renderObjectInfo.textures) {
		texture.textureSampler = resourceManager.createTextureSampler();
	}

	this->renderObjectInfo.descriptorPool = descriptorpool;
	for (uint32_t i = 0; i < swapChainImageCount; i++) {
		this->renderObjectInfo.descriptorSets.push_back(resourceManager.allocateDescriptorSet(descriptorSetLayout,
			this->renderObjectInfo.descriptorPool));
	}
}

RenderObject::RenderObject(const std::vector<Vertex> vertices, const std::vector<uint32_t> indices)
{
	this->renderObjectInfo.vertices = vertices;
	this->renderObjectInfo.indices = indices;
	ResourceManager& resourceManager = ResourceManager::getResourceManager();
	BufferInfo vertexBufferInfo = resourceManager.createGPUBuffer(
		this->renderObjectInfo.vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
	);
	BufferInfo indiceBufferInfo = resourceManager.createGPUBuffer(
		this->renderObjectInfo.indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT
	);
	this->renderObjectInfo.vertexBuffer = vertexBufferInfo.buffer;
	this->renderObjectInfo.vertexBufferMemory = vertexBufferInfo.bufferMemory;
	this->renderObjectInfo.indexBuffer = indiceBufferInfo.buffer;
	this->renderObjectInfo.indexBufferMemory = indiceBufferInfo.bufferMemory;
}

void RenderObject::updateDescriptorSets(const std::vector<DescriptorUpdateInfo>& descriptorUpdateInfos, const uint32_t dstSetIndex)
{
	std::vector<VkWriteDescriptorSet> descriptorWrites;
	for (size_t j = 0; j < descriptorUpdateInfos.size(); j++) {
		VkWriteDescriptorSet writeDescriptorSet = {};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = this->renderObjectInfo.descriptorSets[dstSetIndex];
		writeDescriptorSet.dstBinding = descriptorUpdateInfos[j].binding;
		writeDescriptorSet.dstArrayElement = 0;
		writeDescriptorSet.descriptorType = descriptorUpdateInfos[j].descriptorType;
		writeDescriptorSet.descriptorCount = 1;
		if (descriptorUpdateInfos[j].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
			writeDescriptorSet.pBufferInfo = &descriptorUpdateInfos[j].bufferInfo;
		}
		else if (descriptorUpdateInfos[j].descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
			writeDescriptorSet.pImageInfo = &descriptorUpdateInfos[j].imageInfo;
		}
		descriptorWrites.push_back(writeDescriptorSet);
	}
	vkUpdateDescriptorSets(VulkanContext::getVulkanContext().getContextData().device,
			static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

RenderObject::RenderObject(const std::string& modelPath,
	const VkDescriptorPool& descriptorpool,
	const uint32_t swapChainImageCount)
{
	Model model;
	model.loadPBRModel(modelPath);
	this->renderMultiObjectInfo.descriptorPool = descriptorpool;
	ResourceManager& resourceManager = ResourceManager::getResourceManager();
	for (size_t i = 0; i < model.Meshes.size(); i++) {
		this->renderMultiObjectInfo.vertices.push_back(model.Meshes[i].vertices);
		this->renderMultiObjectInfo.indices.push_back(model.Meshes[i].indices);
		
		BufferInfo vertexBuffer = resourceManager.createGPUBuffer(
			this->renderMultiObjectInfo.vertices[i], VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
		);
		BufferInfo indicesBuffer = resourceManager.createGPUBuffer(
			this->renderMultiObjectInfo.indices[i], VK_BUFFER_USAGE_INDEX_BUFFER_BIT
		);
		this->renderMultiObjectInfo.vertexBuffer.push_back(vertexBuffer.buffer);
		this->renderMultiObjectInfo.vertexBufferMemory.push_back(vertexBuffer.bufferMemory);
		this->renderMultiObjectInfo.indexBuffer.push_back(indicesBuffer.buffer);
		this->renderMultiObjectInfo.indexBufferMemory.push_back(indicesBuffer.bufferMemory);
		
		PBRTextureInfo textureInfo = {};
		if (model.Meshes[i].material.baseColorTexture != "") {
			textureInfo.baseColorTexture = resourceManager.loadMipMapsRGBTexture("assets/models/" + model.Meshes[i].material.baseColorTexture);
		}
		if (model.Meshes[i].material.emissiveTexture != "") {
			textureInfo.emissiveTexture = resourceManager.loadMipMapTexture("assets/models/" + model.Meshes[i].material.emissiveTexture);
 		}
		if (model.Meshes[i].material.metallicRoughnessTexture != "") {
			textureInfo.metallicRoughnessTexture = resourceManager.loadMipMapTexture("assets/models/" + model.Meshes[i].material.metallicRoughnessTexture);
		}
		if (model.Meshes[i].material.normalTexture != "") {
			textureInfo.normalTexture = resourceManager.loadMipMapTexture("assets/models/" + model.Meshes[i].material.normalTexture);
		}
		this->renderMultiObjectInfo.textures.push_back(textureInfo);
	}
	std::vector<VkDescriptorSetLayoutBinding> bindings = {
	{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr},
	{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr},
	{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr},
	{3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr},
	{4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr},
	{5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr}
	};
	VkDescriptorSetLayout descriptorSetLayout = resourceManager.createDescriptorSetLayout(bindings);
	this->renderMultiObjectInfo.descriptorSetLayout = descriptorSetLayout;
	for (uint32_t i = 0; i < swapChainImageCount; i++) {
		this->renderMultiObjectInfo.descriptorSets.push_back(resourceManager.allocateDescriptorSet(descriptorSetLayout,
			this->renderMultiObjectInfo.descriptorPool));
	}
}

void RenderObject::cleanup()
{
	VkDevice device = VulkanContext::getVulkanContext().getContextData().device;
	for (auto texture : this->renderObjectInfo.textures) {
		vkDestroySampler(device, texture.textureSampler, nullptr);
		vkDestroyImageView(device, texture.textureImageView, nullptr);
		vkDestroyImage(device, texture.textureImage, nullptr);
		vkFreeMemory(device, texture.textureImageMemory, nullptr);
	}
	vkDestroyBuffer(device, this->renderObjectInfo.indexBuffer, nullptr);
	vkFreeMemory(device, this->renderObjectInfo.indexBufferMemory, nullptr);
	vkDestroyBuffer(device, this->renderObjectInfo.vertexBuffer, nullptr);
	vkFreeMemory(device, this->renderObjectInfo.vertexBufferMemory, nullptr);
	this->renderObjectInfo.vertices.clear();
	this->renderObjectInfo.indices.clear();
}

void RenderObject::updatePBRDescriptorSets(const std::vector<DescriptorUpdateInfo>& descriptorUpdateInfos, const uint32_t dstSetIndex)
{
	std::vector<VkWriteDescriptorSet> descriptorWrites;
	for (size_t j = 0; j < descriptorUpdateInfos.size(); j++) {
		VkWriteDescriptorSet writeDescriptorSet = {};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = this->renderMultiObjectInfo.descriptorSets[dstSetIndex];
		writeDescriptorSet.dstBinding = descriptorUpdateInfos[j].binding;
		writeDescriptorSet.dstArrayElement = 0;
		writeDescriptorSet.descriptorType = descriptorUpdateInfos[j].descriptorType;
		writeDescriptorSet.descriptorCount = 1;
		if (descriptorUpdateInfos[j].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
			writeDescriptorSet.pBufferInfo = &descriptorUpdateInfos[j].bufferInfo;
		}
		else if (descriptorUpdateInfos[j].descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
			writeDescriptorSet.pImageInfo = &descriptorUpdateInfos[j].imageInfo;
		}
		descriptorWrites.push_back(writeDescriptorSet);
	}
	vkUpdateDescriptorSets(VulkanContext::getVulkanContext().getContextData().device,
		static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

