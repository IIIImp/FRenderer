#ifndef MODEL_H
#define MODEL_H

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include "utils/structUtils.h"


class Model {
public:
	void loadModel(const std::string& filePath);
	void loadPBRModel(const std::string& modelPath);
	std::vector<Vertex> getVertices() { return vertices; }
	std::vector<uint32_t> getIndices() { return indices; }
	std::vector<Mesh> Meshes;
private:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	
};

#endif
