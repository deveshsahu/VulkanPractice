#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <fstream>
#include <array>

#include "Util.h"

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

static VkVertexInputBindingDescription getBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
{
	std::array<VkVertexInputAttributeDescription, 3> attributeDescription = {};
	attributeDescription[0].binding = 0;
	attributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescription[0].location = 0;
	attributeDescription[0].offset = offsetof(Vertex, pos);

	attributeDescription[1].binding = 0;
	attributeDescription[1].location = 1;
	attributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescription[1].offset = offsetof(Vertex, color);

	attributeDescription[2].binding = 0;
	attributeDescription[2].location = 2;
	attributeDescription[2].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescription[2].offset = offsetof(Vertex, texCoord);

	return attributeDescription;
}