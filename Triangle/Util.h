#pragma once
#include <vector>
#include <fstream>

#define GLM_FORCE_RADIANS
#define GLM_FORECE_DEPTH_ZERO_TO_ONE
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

static std::vector<char> readFile(const std::string & filename)
{
	std::ifstream fs(filename, std::ios::ate | std::ios::binary);
	if (!fs.is_open())
	{
		return std::vector<char>();
	}
	size_t fileSize = (size_t)fs.tellg();
	std::vector<char> buffer(fileSize);
	fs.seekg(0);
	fs.read(buffer.data(), fileSize);
	fs.close();
	return buffer;
}

struct Vertex {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;
};

struct MVP
{
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.f);
	glm::mat4 projection = glm::mat4(1.f);

	glm::mat4 mvp;

	void update() { mvp = projection * view * model; }
	glm::mat4 get() const
	{
		return mvp;
	}
};