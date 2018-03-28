#pragma once
#include "Helper.h"
#include <memory>
#include <vector>
#include <glm\glm.hpp>

class TriangleApplication
{
public:
	TriangleApplication();
	~TriangleApplication();

	void run();

	bool checkValidationSupport();

	std::vector<const char*> getRequiredExtensions();

	QueueFamilyIndex findQueueFamilies(VkPhysicalDevice device);

	VkFormat findSupportedFormat(const std::vector<VkFormat>& formatPriority, VkImageTiling tiling, VkFormatFeatureFlags features);

	VkFormat findDepthFormat();

	bool hasStencilComponent(VkFormat format);

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallBack(VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData);
	void setupDebugCallback();
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities);

	void drawFrame();

	void recreateSwapchain();

	void updateUniformBuffer();

	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

public:
	static void onWindowResized(GLFWwindow* window, int width, int height);

private:
	void myInitWindow();
	void myInitVulkan();
	void myPickPhysicalDevice();
	void myMainLoop();
	void myCleanup();
	
	// Vulkan specific calls
	void myCreateInstance();
	void myCreateLogicalDevice();
	void myCreateSurface();
	void myCreateSwapChain();
	void myCreateImageViews();
	void myCreateRenderPass();
	void myCreateDescriptorSetLayout();
	void myCreateGraphicsPipeline();
	void myCreateFramebuffers();
	void myCreateTextureImageView();
	void myCreateTextureSampler();
	void myCreateVertexBuffer();
	void myCreateIndexBuffer();
	void myCreateUniformBuffer();
	void myCreateDescriptorPool();
	void myCreateDescriptorSet();
	void myCreateCommandPool();
	void myCreateDepthTestResources();
	void myCreateTextureImage();
	void myCreateCommandBuffers();
	void myCleanupSwapChain();

	void myCreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
		VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void myCreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags
		usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize bufferSize);

	void myBeginSingleTimeCommands(VkCommandBuffer& buffer);

	void myEndSingleTimeCommands(VkCommandBuffer& buffer);

	VkImageView myCreateImageView(VkImage image, VkFormat format, VkImageAspectFlags flags);

	// Additional Helper functions
	VkShaderModule myCreateShaderModule(const std::vector<char>& byteCode);

	bool myIsSuitableDevice(VkPhysicalDevice device);

	void myCreateSemaphores();

private:
	GLFWwindow * myWindow;

	VkInstance myInstance;
	VkPhysicalDevice myPhysicalDevice = VK_NULL_HANDLE;
	VkDevice	myLogicalDevice;
	VkQueue myGraphicsQueue;	/**< Handle for graphics queue.
								Queue itself is automatically created
								with logical device. */
	VkQueue myPresentQueue;		/**< Presentation queue handle */
	VkSurfaceKHR mySurface;	/**< Vulkan surface to interact with window */
	VkSwapchainKHR mySwapChain; /**< Current swap chain */
	std::vector<VkImage> mySwapChainImages; /**< Handle for images created by swapchain */
	VkFormat mySwapChainImageFormat;
	VkExtent2D mySwapChainExtent;
	std::vector<VkImageView> mySwapChainImageViews;	
	std::vector<VkFramebuffer> mySwapChainFramebuffers;
	std::vector<VkCommandBuffer> myCommandBuffers;
	VkRenderPass myRenderPass;
	VkPipelineLayout myPipelineLayout;
	VkPipeline myPipeline;
	VkCommandPool myCommandPool;

	VkDebugReportCallbackEXT myCallback;

	// Lock mechanism for swapchain and rendering	
	VkSemaphore myImageAvailableSemaphore, myRenderFinishedSemaphore;

	// For UBO Support
	VkDescriptorSetLayout myDescriptorSetLayout;
	VkDescriptorPool myDescriptorPool;	
	VkDescriptorSet myDescriptorSet;

	// Data
	const std::vector<Vertex> myVertexData = 
	{
		{glm::vec3( 0.5f,  0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1, 1)},
		{glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0, 1)},
		{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0, 0)},
		{glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1, 0)},

		{ glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1, 1) },
		{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0, 1) },
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0, 0) },
		{ glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1, 0) }
	};

	const std::vector<uint16_t> myIndexData = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4 };

	VkBuffer myVertexBuffer;
	VkDeviceMemory myVertexBufferMemory;

	VkBuffer myIndexBuffer;
	VkDeviceMemory myIndexBufferMemory;

	VkBuffer myUniformBuffer;
	VkDeviceMemory myUniformBufferMemory; 

	VkImage myTextureImage;
	VkDeviceMemory myTextureImageMemory;

	VkImageView myTextureImageView;

	VkImage myDepthImage;
	VkDeviceMemory myDepthImageMemory;
	VkImageView myDepthImageView;

	VkSampler myTextureSampler;

	const glm::mat4 vulkanMatrixCorrection = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.0f, 0.0f, 0.5f, 1.0f
	);
};