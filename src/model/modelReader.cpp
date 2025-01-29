#define TINYOBJLOADER_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "modelReader.h"
#include "utils/tiny_obj_loader.h"
#include "utils/tinygltf-2.9.5/tiny_gltf.h"
#include <iostream>

void Model::loadModel(const std::string& filePath) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn;
	std::string err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str())) {
		throw std::runtime_error(err);
	}

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex = {};
			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};
			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};
			vertex.normal = {
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2]
			};
			vertices.push_back(vertex);
			indices.push_back(static_cast<uint32_t>(indices.size()));
		}
	}
}

void Model::loadPBRModel(const std::string& modelPath)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    // 加载模型文件
    if (!loader.LoadASCIIFromFile(&model, &err, &warn, modelPath)) {
        throw std::runtime_error("Failed to load glTF model: " + err);
    }

    if (!warn.empty()) {
        std::cerr << "glTF loading warnings: " << warn << std::endl;
    }

    std::vector<Mesh> meshes;

    // 遍历所有 Mesh
    for (const auto& mesh : model.meshes) {
        for (const auto& primitive : mesh.primitives) {
            Mesh newMesh;

            // 读取顶点数据
            const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.at("POSITION")];
            const tinygltf::BufferView& posBufferView = model.bufferViews[posAccessor.bufferView];
            const tinygltf::Buffer& posBuffer = model.buffers[posBufferView.buffer];
            const float* positions = reinterpret_cast<const float*>(
                &posBuffer.data[posBufferView.byteOffset + posAccessor.byteOffset]);

            newMesh.vertices.resize(posAccessor.count);
            for (size_t i = 0; i < posAccessor.count; i++) {
                newMesh.vertices[i].pos = glm::vec3(
                    positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2]);
            }

            // 读取法线数据
            if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                const tinygltf::Accessor& normalAccessor = model.accessors[primitive.attributes.at("NORMAL")];
                const tinygltf::BufferView& normalBufferView = model.bufferViews[normalAccessor.bufferView];
                const tinygltf::Buffer& normalBuffer = model.buffers[normalBufferView.buffer];
                const float* normals = reinterpret_cast<const float*>(
                    &normalBuffer.data[normalBufferView.byteOffset + normalAccessor.byteOffset]);

                for (size_t i = 0; i < normalAccessor.count; i++) {
                    newMesh.vertices[i].normal = glm::vec3(
                        normals[i * 3 + 0], normals[i * 3 + 1], normals[i * 3 + 2]);
                }
            }

            // 读取纹理坐标
            if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
                const tinygltf::Accessor& texCoordAccessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
                const tinygltf::BufferView& texCoordBufferView = model.bufferViews[texCoordAccessor.bufferView];
                const tinygltf::Buffer& texCoordBuffer = model.buffers[texCoordBufferView.buffer];
                const float* texCoords = reinterpret_cast<const float*>(
                    &texCoordBuffer.data[texCoordBufferView.byteOffset + texCoordAccessor.byteOffset]);

                for (size_t i = 0; i < texCoordAccessor.count; i++) {
                    newMesh.vertices[i].texCoord = glm::vec2(
                        texCoords[i * 2 + 0], texCoords[i * 2 + 1]);
                }
            }

            // 读取索引数据
            if (primitive.indices >= 0) {
                const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
                const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
                const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];
                const uint32_t* indices = reinterpret_cast<const uint32_t*>(
                    &indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset]);

                newMesh.indices.assign(indices, indices + indexAccessor.count);
            }

            // 读取材质数据
            if (primitive.material >= 0) {
                const tinygltf::Material& mat = model.materials[primitive.material];

                // 漫反射纹理
                if (mat.pbrMetallicRoughness.baseColorTexture.index >= 0) {
                    const tinygltf::Texture& texture = model.textures[mat.pbrMetallicRoughness.baseColorTexture.index];
                    const tinygltf::Image& image = model.images[texture.source];
                    newMesh.material.baseColorTexture = image.uri;
                }

                // 法线纹理
                if (mat.normalTexture.index >= 0) {
                    const tinygltf::Texture& texture = model.textures[mat.normalTexture.index];
                    const tinygltf::Image& image = model.images[texture.source];
                    newMesh.material.normalTexture = image.uri;
                }

                if (mat.emissiveTexture.index >= 0) {
                    const tinygltf::Texture& texture = model.textures[mat.emissiveTexture.index];
                    const tinygltf::Image& image = model.images[texture.source];
                    newMesh.material.emissiveTexture = image.uri;
                }

                // Metallic-Roughness Texture
                if (mat.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0) {
                    const tinygltf::Texture& texture = model.textures[mat.pbrMetallicRoughness.metallicRoughnessTexture.index];
                    const tinygltf::Image& image = model.images[texture.source];
                    newMesh.material.metallicRoughnessTexture = image.uri;
                }
            }

            meshes.push_back(newMesh);
        }
    }
    this->Meshes = meshes;
}
