#ifndef STRUCTUTILS_H
#define STRUCTUTILS_H



#include <glm.hpp>
#include <vulkan/vulkan.h>
#include <cstdint>
#include <vector>
#include <string>

struct QueueFamilyIndices {
    uint32_t graphicsFamily = UINT32_MAX;
    uint32_t presentFamily = UINT32_MAX;
    bool isComplete() const {
        return graphicsFamily >= 0 && presentFamily >= 0;
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities = {};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct ContextData {
    VkInstance       instance = VK_NULL_HANDLE;
    VkDevice         device = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkQueue          graphicsQueue = VK_NULL_HANDLE;
    VkQueue          presentQueue = VK_NULL_HANDLE;
};

struct SwapchainData {
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkFormat imageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D extent = {};
};

struct SwapchainImages {
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
};

struct BufferInfo {
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
};

struct rotationMatrix {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

struct lightInform {
	alignas(16) glm::vec3 lightPosition;
	alignas(16) glm::vec3 lightColor;
	alignas(16) glm::vec3 viewPos;
	alignas(4) float intensity;
};

struct Material {
	std::string baseColorTexture; // 漫反射纹理路径
	std::string emissiveTexture; // 法线纹理路径
	std::string metallicRoughnessTexture; // 金属度-粗糙度纹理
	std::string normalTexture;          // 法线纹理
};

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;
	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}
	static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		auto attributeDescription = VkVertexInputAttributeDescription{};
		attributeDescription.binding = 0;
		attributeDescription.location = 0;
		attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescription.offset = offsetof(Vertex, pos);
		attributeDescriptions.push_back(attributeDescription);
		attributeDescription.binding = 0;
		attributeDescription.location = 1;
		attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescription.offset = offsetof(Vertex, normal);
		attributeDescriptions.push_back(attributeDescription);
		attributeDescription.binding = 0;
		attributeDescription.location = 2;
		attributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescription.offset = offsetof(Vertex, texCoord);
		attributeDescriptions.push_back(attributeDescription);
		return attributeDescriptions;
	}
};

struct Mesh {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	Material material;
};
struct TextureInfo {
	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;
};

struct DescriptorUpdateInfo {
	uint32_t binding;              
	VkDescriptorType descriptorType;

	VkDescriptorBufferInfo bufferInfo;
	VkDescriptorImageInfo imageInfo;
};

struct renderObject {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	VkBuffer vertexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
	VkBuffer indexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;
	
	std::vector<TextureInfo> textures;

	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	std::vector<VkDescriptorSet> descriptorSets;
};

struct PBRTextureInfo {
	TextureInfo baseColorTexture;
	TextureInfo emissiveTexture;
	TextureInfo metallicRoughnessTexture;
	TextureInfo normalTexture;
};

struct renderMultiObject {
	std::vector<std::vector<Vertex>> vertices;
	std::vector<std::vector<uint32_t>> indices;
	std::vector<VkBuffer> vertexBuffer;
	std::vector<VkDeviceMemory> vertexBufferMemory;
	std::vector<VkBuffer> indexBuffer;
	std::vector<VkDeviceMemory> indexBufferMemory;

	std::vector<PBRTextureInfo> textures;

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	std::vector<VkDescriptorSet> descriptorSets;
};

struct RenderPipelineInfo {
	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
};

struct ImageInfo {
	VkImage image;
	VkDeviceMemory imageMemory;
};

#endif