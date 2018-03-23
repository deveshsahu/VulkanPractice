 #include "TriangleApplication.h"

#include <stdexcept>
#include <iostream>
#include <set>
#include <algorithm>

const int WIDTH = 800;
const int HEIGHT = 600;

const std::vector<const char*> validationLayers = { "VK_LAYER_LUNARG_standard_validation" };
const std::vector<const char*> deviceExtensions = { "VK_KHR_swapchain" };

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

TriangleApplication::TriangleApplication()
{
}


TriangleApplication::~TriangleApplication()
{
}

void TriangleApplication::run()
{
	myInitWindow();
	myInitVulkan();
	myMainLoop();
	myCleanup();
}

bool TriangleApplication::checkValidationSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers)
	{
		bool layerFound = false;
		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
			return false;
	}
	return true;
}

std::vector<const char*> TriangleApplication::getRequiredExtensions()
{
	std::vector<const char*> extensions;
	
	unsigned int glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	for (unsigned int i = 0; i < glfwExtensionCount; ++i)
		extensions.push_back(glfwExtensions[i]);
	if (enableValidationLayers)
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	return extensions;
}

QueueFamilyIndex TriangleApplication::findQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndex index;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			index.graphicsFamily = i;
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mySurface, &presentSupport);
		if (queueFamily.queueCount > 0 && presentSupport)	
			index.presentFamily = i;
		if (index.isComplete())
			break;
		++i;
	}
	return index;
}

VKAPI_ATTR VkBool32 VKAPI_CALL TriangleApplication::debugCallBack(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char * layerPrefix, const char * msg, void * userData)
{
	std::cerr << "Validation layer: " << msg << std::endl;
	return VK_FALSE;
}

void TriangleApplication::setupDebugCallback()
{
	if (!enableValidationLayers)
		return;
	VkDebugReportCallbackCreateInfoEXT createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallBack;

	if (CreateDebugReportCallbackEXT(myInstance, &createInfo, nullptr, &myCallback ) != VK_SUCCESS)
		throw std::runtime_error("Failed to setup debug callback");
}

void TriangleApplication::myInitWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	myWindow = glfwCreateWindow(WIDTH, HEIGHT, "Triangle", nullptr, nullptr);

	glfwSetWindowUserPointer(myWindow, this);
	glfwSetWindowSizeCallback(myWindow, TriangleApplication::onWindowResized);
}

void TriangleApplication::myInitVulkan()
{
	myCreateInstance();
	setupDebugCallback();
	myCreateSurface();
	myPickPhysicalDevice();
	myCreateLogicalDevice();
	myCreateSwapChain();
	myCreateImageViews();
	myCreateRenderPass();
	myCreateDescriptorSetLayout();
	myCreateGraphicsPipeline();
	myCreateFramebuffers();
	myCreateCommandPool();
	myCreateVertexBuffer();
	myCreateIndexBuffer();
	myCreateUniformBuffer();
	myCreateDescriptorPool();
	myCreateDescriptorSet();
	myCreateCommandBuffers();
	myCreateSemaphores();
}

void TriangleApplication::myPickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(myInstance, &deviceCount, nullptr);
	if (deviceCount == 0)
		throw std::runtime_error("Failed to find any physical devices");
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(myInstance, &deviceCount, devices.data());
	for (const auto& device : devices)
	{
		if (myIsSuitableDevice(device))
		{
			myPhysicalDevice = device;
			break;
		}
	}
	if (myPhysicalDevice == VK_NULL_HANDLE)
		throw std::runtime_error("No suitable GPU present");
}

void TriangleApplication::myMainLoop()
{
	while (!glfwWindowShouldClose(myWindow))
	{
		glfwPollEvents();
		updateUniformBuffer();
		drawFrame();
	}
	vkDeviceWaitIdle(myLogicalDevice);
}

void TriangleApplication::myCleanup()
{
	myCleanupSwapChain();

	vkDestroyDescriptorPool(myLogicalDevice, myDescriptorPool, nullptr);
	
	vkDestroyDescriptorSetLayout(myLogicalDevice, myDescriptorSetLayout, nullptr);
	vkDestroyBuffer(myLogicalDevice, myUniformBuffer, nullptr);
	vkFreeMemory(myLogicalDevice, myUniformBufferMemory, nullptr);

	vkDestroyBuffer(myLogicalDevice, myVertexBuffer, nullptr);
	vkFreeMemory(myLogicalDevice, myVertexBufferMemory, nullptr);

	vkDestroyBuffer(myLogicalDevice, myIndexBuffer, nullptr);
	vkFreeMemory(myLogicalDevice, myIndexBufferMemory, nullptr);

	vkDestroySemaphore(myLogicalDevice, myRenderFinishedSemaphore, nullptr);
	vkDestroySemaphore(myLogicalDevice, myImageAvailableSemaphore, nullptr);
	
	vkDestroyCommandPool(myLogicalDevice, myCommandPool, nullptr);

	vkDestroyDevice(myLogicalDevice, nullptr);
	DestroyDebugReportCallbackEXT(myInstance, myCallback, nullptr);
	vkDestroySurfaceKHR(myInstance, mySurface, nullptr);
	vkDestroyInstance(myInstance, nullptr);

	glfwDestroyWindow(myWindow);

	glfwTerminate();
}

void TriangleApplication::myCreateSurface()
{
	if (glfwCreateWindowSurface(myInstance, myWindow, nullptr, &mySurface))
		throw std::runtime_error("Failed to create window surface!");
}

void TriangleApplication::myCreateSwapChain()
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(myPhysicalDevice, mySurface);
	
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.minImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		imageCount = swapChainSupport.capabilities.maxImageCount;
	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = mySurface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	auto indices = findQueueFamilies(myPhysicalDevice);
	uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicsFamily, (uint32_t)   indices.presentFamily};
	if (indices.presentFamily != indices.graphicsFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(myLogicalDevice, &createInfo, nullptr, &mySwapChain))
	{
		throw std::runtime_error("Failed to create SwapChain!");
	}

	uint32_t swapChainImageCount = 0;
	vkGetSwapchainImagesKHR(myLogicalDevice, mySwapChain, &swapChainImageCount, nullptr);
	mySwapChainImages.resize(swapChainImageCount);
	vkGetSwapchainImagesKHR(myLogicalDevice, mySwapChain, &swapChainImageCount, mySwapChainImages.data());
	mySwapChainImageFormat = surfaceFormat.format;
	mySwapChainExtent = extent;
}

void TriangleApplication::myCreateImageViews()
{
	mySwapChainImageViews.resize(mySwapChainImages.size());
	for (size_t i = 0; i < mySwapChainImages.size(); ++i)
	{
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = mySwapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = mySwapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseMipLevel = 0;

		if (vkCreateImageView(myLogicalDevice, &createInfo, nullptr, &mySwapChainImageViews[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create Image View!");
	}
}

void TriangleApplication::myCreateRenderPass()
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = mySwapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDesc = {};
	subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDesc.colorAttachmentCount = 1;
	subpassDesc.pColorAttachments = &colorAttachmentRef;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpassDesc;

	if (vkCreateRenderPass(myLogicalDevice, &renderPassInfo, nullptr, &myRenderPass) != VK_SUCCESS)
		throw std::runtime_error("Failed to create render pass!");

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;
}

void TriangleApplication::myCreateDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding	uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	if (vkCreateDescriptorSetLayout(myLogicalDevice, &layoutInfo, nullptr, &myDescriptorSetLayout) != VK_SUCCESS)
		throw std::runtime_error("Failed to create descriptor set layout");
}

void TriangleApplication::myCreateGraphicsPipeline()
{
	auto vertexShaderByteCode = readFile("Shaders/vert.spv");
	auto fragmentShaderByteCode = readFile("Shaders/frag.spv");
	auto vertexShaderModule = myCreateShaderModule(vertexShaderByteCode);
	auto fragmentShaderModule = myCreateShaderModule(fragmentShaderByteCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertexShaderModule;
	vertShaderStageInfo.pName = "main";	

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragmentShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	auto bindingDescription = getBindingDescription();
	auto attributeDescription = getAttributeDescriptions();
	
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)mySwapChainExtent.width;
	viewport.height = (float)mySwapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = mySwapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.scissorCount = 1.0f;
	viewportState.viewportCount = 1.0f;
	viewportState.pScissors = &scissor;
	viewportState.pViewports = &viewport;

	VkPipelineRasterizationStateCreateInfo rasterizerInfo = {};
	rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizerInfo.depthClampEnable = VK_FALSE;
	rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizerInfo.lineWidth = 1.0f;
	rasterizerInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizerInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizerInfo.depthBiasEnable = VK_FALSE;
	rasterizerInfo.depthBiasClamp = 0.0f;
	rasterizerInfo.depthBiasConstantFactor = 0.0f;
	rasterizerInfo.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo multisampleInfo = {};
	multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleInfo.sampleShadingEnable = VK_FALSE;
	multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampleInfo.minSampleShading = 1.0f;
	multisampleInfo.pSampleMask = nullptr;
	multisampleInfo.alphaToCoverageEnable = VK_FALSE;
	multisampleInfo.alphaToOneEnable = VK_FALSE;

	// Use this for depth and stencil testing
	// VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {};

	VkPipelineColorBlendAttachmentState colorBlendAttachmentInfo = {};
	colorBlendAttachmentInfo.colorWriteMask = VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_R_BIT;
	colorBlendAttachmentInfo.blendEnable = VK_FALSE;
	colorBlendAttachmentInfo.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachmentInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachmentInfo.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachmentInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachmentInfo.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachmentInfo.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlendInfo = {};
	colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendInfo.logicOpEnable = VK_FALSE;
	colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
	colorBlendInfo.attachmentCount = 1;
	colorBlendInfo.pAttachments = &colorBlendAttachmentInfo;
	colorBlendInfo.blendConstants[0] = 0.0f;
	colorBlendInfo.blendConstants[1] = 0.0f;
	colorBlendInfo.blendConstants[2] = 0.0f;
	colorBlendInfo.blendConstants[3] = 0.0f;

	VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};
	VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
	dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateInfo.dynamicStateCount = 2;
	dynamicStateInfo.pDynamicStates = dynamicStates;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &myDescriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(myLogicalDevice, &pipelineLayoutInfo, nullptr, &myPipelineLayout) != VK_SUCCESS)
		throw std::runtime_error("Failed to create pipeline layout");

	VkGraphicsPipelineCreateInfo graphicsPipelineInfo = {};
	graphicsPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicsPipelineInfo.stageCount = 2;
	graphicsPipelineInfo.pStages = shaderStages;
	graphicsPipelineInfo.pVertexInputState = &vertexInputInfo;
	graphicsPipelineInfo.pInputAssemblyState = &inputAssembly;
	graphicsPipelineInfo.pViewportState = &viewportState;
	graphicsPipelineInfo.pRasterizationState = &rasterizerInfo;
	graphicsPipelineInfo.pMultisampleState = &multisampleInfo;
	graphicsPipelineInfo.pDepthStencilState = nullptr;
	graphicsPipelineInfo.pColorBlendState = &colorBlendInfo;
	graphicsPipelineInfo.pDynamicState = nullptr;
	graphicsPipelineInfo.layout = myPipelineLayout;
	graphicsPipelineInfo.renderPass = myRenderPass;
	graphicsPipelineInfo.subpass = 0;
	graphicsPipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	graphicsPipelineInfo.basePipelineIndex = -1;


	if (vkCreateGraphicsPipelines(myLogicalDevice, VK_NULL_HANDLE, 1, &graphicsPipelineInfo, nullptr, &myPipeline) != VK_SUCCESS)
		throw std::runtime_error("Failed to create graphics pipeline!");

	vkDestroyShaderModule(myLogicalDevice, vertexShaderModule, nullptr);
	vkDestroyShaderModule(myLogicalDevice, fragmentShaderModule, nullptr);

}

void TriangleApplication::myCreateFramebuffers()
{
	mySwapChainFramebuffers.resize(mySwapChainImageViews.size());
	for (size_t i = 0; i < mySwapChainImageViews.size(); ++i)
	{
		VkImageView attachments[] = {
			mySwapChainImageViews[i]
		};
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = myRenderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = mySwapChainExtent.width;
		framebufferInfo.height = mySwapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(myLogicalDevice, &framebufferInfo, nullptr, &mySwapChainFramebuffers[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create frame buffer!");
	}
}

void TriangleApplication::myCreateCommandPool()
{
	auto queueFamilyIndices = findQueueFamilies(myPhysicalDevice);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
	poolInfo.flags = 0;
	if (vkCreateCommandPool(myLogicalDevice, &poolInfo, nullptr, &myCommandPool) != VK_SUCCESS)
		throw std::runtime_error("Failed to create command pool!");

}

void TriangleApplication::myCreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties, VkBuffer & buffer, VkDeviceMemory & bufferMemory)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(myLogicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to create vertex buffer");

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(myLogicalDevice, buffer, &memoryRequirements);

	VkMemoryAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = memoryRequirements.size;
	allocateInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, properties, myPhysicalDevice);

	if (vkAllocateMemory(myLogicalDevice, &allocateInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate vertex buffer memory!");

	vkBindBufferMemory(myLogicalDevice, buffer, bufferMemory, 0);
}

void TriangleApplication::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize bufferSize)
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = myCommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(myLogicalDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = bufferSize;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(myGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(myGraphicsQueue);

	vkFreeCommandBuffers(myLogicalDevice, myCommandPool, 1, &commandBuffer);
}

void TriangleApplication::myCreateVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(myVertexData[0]) * myVertexData.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	myCreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
	
	void* data;
	vkMapMemory(myLogicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, myVertexData.data(), (size_t)bufferSize);
	vkUnmapMemory(myLogicalDevice, stagingBufferMemory);

	myCreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, myVertexBuffer, myVertexBufferMemory);

	copyBuffer(stagingBuffer, myVertexBuffer, bufferSize);

	vkDestroyBuffer(myLogicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(myLogicalDevice, stagingBufferMemory, nullptr);
}

void TriangleApplication::myCreateIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(myIndexData[0]) * myIndexData.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	myCreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(myLogicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, myIndexData.data(), (size_t)bufferSize);
	vkUnmapMemory(myLogicalDevice, stagingBufferMemory);

	myCreateBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, myIndexBuffer, myIndexBufferMemory);

	copyBuffer(stagingBuffer, myIndexBuffer, bufferSize);

	vkDestroyBuffer(myLogicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(myLogicalDevice, stagingBufferMemory, nullptr);
}

void TriangleApplication::myCreateUniformBuffer()
{
	VkDeviceSize bufferSize = sizeof(MVP);
	myCreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, myUniformBuffer, myUniformBufferMemory);
}

void TriangleApplication::myCreateDescriptorPool()
{
	VkDescriptorPoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = 1;
	
	if (vkCreateDescriptorPool(myLogicalDevice, &poolInfo, nullptr, &myDescriptorPool) != VK_SUCCESS)
		throw std::runtime_error("Failed to create descriptor pool!");
}

void TriangleApplication::myCreateDescriptorSet()
{
	VkDescriptorSetLayout layouts[] = { myDescriptorSetLayout };
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = myDescriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts;

	if (vkAllocateDescriptorSets(myLogicalDevice, &allocInfo, &myDescriptorSet) != VK_SUCCESS)
		throw std::runtime_error("Failed to create Descriptor Set!");

	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = myUniformBuffer;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(MVP);
	
	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = myDescriptorSet;
	descriptorWrite.dstBinding = 0;		// Binding index of the ubo
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = &bufferInfo;
	descriptorWrite.pImageInfo = nullptr;
	descriptorWrite.pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(myLogicalDevice, 1, &descriptorWrite, 0, nullptr);
}

void TriangleApplication::myCreateCommandBuffers()
{
	myCommandBuffers.resize(mySwapChainFramebuffers.size());
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = myCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)myCommandBuffers.size();
	
	if (vkAllocateCommandBuffers(myLogicalDevice, &allocInfo, myCommandBuffers.data()) != VK_SUCCESS)
		throw std::runtime_error("Failed to create command buffers");

	for (size_t i = 0; i < myCommandBuffers.size(); ++i)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr;

		vkBeginCommandBuffer(myCommandBuffers[i], &beginInfo);

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = myRenderPass;
		renderPassInfo.framebuffer = mySwapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = mySwapChainExtent;
		VkClearValue clearValue = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearValue;
		
		// Render pass begins
		vkCmdBeginRenderPass(myCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(myCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, myPipeline);

		VkBuffer vertexBuffers[] = { myVertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(myCommandBuffers[i], 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(myCommandBuffers[i], myIndexBuffer, 0, VK_INDEX_TYPE_UINT16);

		vkCmdBindDescriptorSets(myCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
			myPipelineLayout, 0, 1, &myDescriptorSet, 0, nullptr);

		vkCmdDrawIndexed(myCommandBuffers[i], static_cast<uint32_t> (myIndexData.size()), 1, 0, 0, 0);

		vkCmdEndRenderPass(myCommandBuffers[i]);
		// Render pass ends

		if (vkEndCommandBuffer(myCommandBuffers[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to record command buffer!");
	}
}

void TriangleApplication::recreateSwapchain()
{
	vkDeviceWaitIdle(myLogicalDevice);

	myCleanupSwapChain();

	vkDestroyDescriptorPool(myLogicalDevice, myDescriptorPool, nullptr);

	myCreateSwapChain();
	myCreateImageViews();
	myCreateRenderPass();
	myCreateGraphicsPipeline();
	myCreateFramebuffers();
	myCreateCommandBuffers();
}

void TriangleApplication::updateUniformBuffer()
{
	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	MVP ubo = {};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0, 0.0, 1.0f));
	
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	ubo.projection = glm::perspective(glm::radians(45.0f), static_cast<float>(mySwapChainExtent.width) / static_cast<float>(mySwapChainExtent.height), 0.1f, 100.0f);

	ubo.projection = vulkanMatrixCorrection * ubo.projection;
	
	void* data;
	vkMapMemory(myLogicalDevice, myUniformBufferMemory, 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(myLogicalDevice, myUniformBufferMemory);
}

void TriangleApplication::myCleanupSwapChain()
{
	for (size_t i = 0; i < mySwapChainFramebuffers.size(); ++i)
		vkDestroyFramebuffer(myLogicalDevice, mySwapChainFramebuffers[i], nullptr);

	vkFreeCommandBuffers(myLogicalDevice, myCommandPool, static_cast<uint32_t>(myCommandBuffers.size()), myCommandBuffers.data());

	vkDestroyPipeline(myLogicalDevice, myPipeline, nullptr);
	vkDestroyPipelineLayout(myLogicalDevice, myPipelineLayout, nullptr);
	vkDestroyRenderPass(myLogicalDevice, myRenderPass, nullptr);

	for (size_t i = 0; i < mySwapChainImageViews.size(); ++i)
		vkDestroyImageView(myLogicalDevice, mySwapChainImageViews[i], nullptr);

	vkDestroySwapchainKHR(myLogicalDevice, mySwapChain, nullptr);
}

bool TriangleApplication::myIsSuitableDevice(VkPhysicalDevice device)
{
	// Suitable score system can also be added here
	// The GPUs will be rated based on the score and 
	// the most powerful will be selected.
	// NOT IMPLEMENTED HERE, WILL BE ADDED IF NEEDED

	// Queue family based
	auto index = findQueueFamilies(device);
	bool extensionsSupported = checkDeviceExtensionSupport(device, deviceExtensions);
	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		auto swapChainSupport = querySwapChainSupport(device, mySurface);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}
	return index.isComplete() && extensionsSupported && swapChainAdequate;
}

void TriangleApplication::myCreateSemaphores()
{
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	if (vkCreateSemaphore(myLogicalDevice, &semaphoreInfo, nullptr, &myImageAvailableSemaphore) != VK_SUCCESS
		|| vkCreateSemaphore(myLogicalDevice, &semaphoreInfo, nullptr, &myRenderFinishedSemaphore) != VK_SUCCESS)
		throw std::runtime_error("Failed to create semaphores");
}

void TriangleApplication::myCreateInstance()
{
	if (enableValidationLayers && !checkValidationSupport())
		throw std::runtime_error("Validation layers requested, but not available!");
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Triangle";
	appInfo.pEngineName = "No Engine";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = getRequiredExtensions();

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
		createInfo.enabledLayerCount = 0;

	if (vkCreateInstance(&createInfo, nullptr, &myInstance) != VK_SUCCESS)
		throw std::runtime_error("Could not create instance!");
/*
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	for (auto& extension : extensions)
		std::cout << "\t" << extension.extensionName << std::endl;
*/
}

void TriangleApplication::myCreateLogicalDevice()
{
	// Specifying the queues to be created
	auto familyIndex = findQueueFamilies(myPhysicalDevice);
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> uniqueQueueFamilies{ familyIndex.graphicsFamily, familyIndex.presentFamily };

	// Request specific features
	VkPhysicalDeviceFeatures deviceFeatures = {};

	float queuePriority = 1.f;

	for (int queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	// Creating logical devices
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();
	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(myPhysicalDevice, &createInfo, nullptr, &myLogicalDevice) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device!");
	}

	// Retrieve and save the handles
	vkGetDeviceQueue(myLogicalDevice, familyIndex.graphicsFamily, 0, &myGraphicsQueue);
	vkGetDeviceQueue(myLogicalDevice, familyIndex.presentFamily, 0, &myPresentQueue);
}

VkExtent2D TriangleApplication::chooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		return capabilities.currentExtent;
	else
	{
		int width, height;
		glfwGetWindowSize(myWindow, &width, &height);
		VkExtent2D actualExtent = { width, height };

		actualExtent.width = std::max(capabilities.minImageExtent.width,
			std::min(capabilities.minImageExtent.width, actualExtent.width));
		actualExtent.width = std::max(capabilities.minImageExtent.height,
			std::min(capabilities.minImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

void TriangleApplication::drawFrame()
{
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(myLogicalDevice, mySwapChain, std::numeric_limits<uint64_t>::max(), 
		myImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapchain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		throw std::runtime_error("Failed to acquire swap chain image!");

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { myImageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &myCommandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { myRenderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(myGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
		throw std::runtime_error("Failed to submit draw command buffer!");

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { mySwapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(myPresentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		recreateSwapchain();
	else if (result != VK_SUCCESS)
		throw std::runtime_error("Failed to present swap chain image!");

	vkQueueWaitIdle(myPresentQueue);
}

void TriangleApplication::onWindowResized(GLFWwindow * window, int width, int height)
{
	if (width == 0 || height == 0)
		return;

	auto app = reinterpret_cast<TriangleApplication*> (glfwGetWindowUserPointer(window));
	app->recreateSwapchain();

}

VkShaderModule TriangleApplication::myCreateShaderModule(const std::vector<char>& byteCode)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = byteCode.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*> (byteCode.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(myLogicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		throw std::runtime_error("Failed to create shader module!");
	return shaderModule;
}

