#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <fstream>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORECE_DEPTH_ZERO_TO_ONE
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <chrono>


VkResult CreateDebugReportCallbackEXT(VkInstance instance,
	const VkDebugReportCallbackCreateInfoEXT * pCreateInfo,
	const VkAllocationCallbacks * pAllocator,
	VkDebugReportCallbackEXT * pCallback);

void DestroyDebugReportCallbackEXT(VkInstance instance,
	VkDebugReportCallbackEXT callback,
	const VkAllocationCallbacks * pAllocator);

struct QueueFamilyIndex
{
	int graphicsFamily	= -1;
	int presentFamily	= -1;

	bool isComplete()
	{
		return graphicsFamily >= 0 && presentFamily >=0 ;
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

bool checkDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char*> & extensions);

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentMode);

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags flags, VkPhysicalDevice device);

static std::vector<char> readFile(const std::string & filename)
{
	std::ifstream fs(filename, std::ios::ate | std::ios::binary);
	if (!fs.is_open())
		throw std::runtime_error("Failed to open file!");
	size_t fileSize = (size_t)fs.tellg();
	std::vector<char> buffer(fileSize);
	fs.seekg(0);
	fs.read(buffer.data(), fileSize);
	fs.close();
	return buffer;
}

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;
};

static VkVertexInputBindingDescription getBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
{
	std::array<VkVertexInputAttributeDescription, 2> attributeDescription = {};
	attributeDescription[0].binding = 0;
	attributeDescription[0].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescription[0].location = 0;
	attributeDescription[0].offset = offsetof(Vertex, pos);

	attributeDescription[1].binding = 0;
	attributeDescription[1].location = 1;
	attributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescription[1].offset = offsetof(Vertex, color);

	return attributeDescription;
}

struct MVP {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
};