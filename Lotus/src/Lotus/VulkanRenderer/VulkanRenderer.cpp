#include "lotuspch.h"
#include "VulkanRenderer.h"
#include "Utilities.h"


int VulkanRenderer::init(GLFWwindow* newWindow)
{
    window = newWindow;

    try 
	{
		createInstance();
		setupDebugMessenger();
		createSurface();
		getPhysicalDevice();
		createLogicalDevice();
		createSwapChain();
		createRenderPass();
		createGraphicsPipeline();
		createFramebuffers();
		createCommandPool();
		createCommandBuffers();
		recordCommands();
	}
	catch (const std::runtime_error& e) 
	{
		printf("ERROR: %s\n", e.what());
		return EXIT_FAILURE;
	}

    return 0;
}

void VulkanRenderer::cleanup()
{
	vkDestroyCommandPool(mainDevice.logicalDevice, graphicsCommandPool, nullptr);
	for (auto framebuffer : swapChainFramebuffers) {
		vkDestroyFramebuffer(mainDevice.logicalDevice, framebuffer, nullptr);
	}
	vkDestroyPipeline(mainDevice.logicalDevice, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(mainDevice.logicalDevice, pipelineLayout, nullptr);
	vkDestroyRenderPass(mainDevice.logicalDevice, renderPass, nullptr);
	for (auto imageView : swapChainImages) {
		vkDestroyImageView(mainDevice.logicalDevice, imageView.imageView, nullptr);
	}
	vkDestroySwapchainKHR(mainDevice.logicalDevice, swapChain, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}
	vkDestroyDevice(mainDevice.logicalDevice, nullptr);
	vkDestroyInstance(instance, nullptr);
}

void VulkanRenderer::createInstance()
{
	// Information about the application itself
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

	// Information about the application
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;		// Structure type
	appInfo.pApplicationName = "Vulkan App";				// pointer to the name of the application (hence pApplicationName)
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);	// Application version
	appInfo.pEngineName = "No Engine"; 						// Pointer to the name of the engine
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0); 		// Engine version
	appInfo.apiVersion = VK_API_VERSION_1_2; 				// Vulkan version

	// Creation information for a Vulkan Instance
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// Check Instance extensions are supported
	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	// Validation layer information
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	// Create instance
	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

	// Check for an error during instance creation
	if (result != VK_SUCCESS) 
	{
		throw std::runtime_error("Failed to create a Vulkan Instance");
	}
	else 
	{
		std::cout << "Successfully created a Vulkan Instance" << std::endl;
	}
}

void VulkanRenderer::createLogicalDevice()
{
	// Get the queue family indices for the chosen Physical Device
	QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

	// Vector for queue creation information, and set for family indices	
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> queueFamilyIndices = { indices.graphicsFamily, indices.presentationFamily };

	// Queues the logical device needs to create and info to do so
	for (int queueFamilyIndex : queueFamilyIndices)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamilyIndex; // The index of the family to create a queue from
		queueCreateInfo.queueCount = 1; // Number of queues to create
		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority; // Vulkan needs to know how to handle multiple queues, so decide priority (1 = highest priority)

		queueCreateInfos.push_back(queueCreateInfo);
	}

	// Information to create logical device
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()); // Number of queue create infos
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data(); // List of queue create infos so device can create required queues
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()); // Number of enabled logical device extensions
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data(); // List of enabled logical device extensions

	// Physical Device Features the Logical Device will be using
	VkPhysicalDeviceFeatures deviceFeatures = {};

	deviceCreateInfo.pEnabledFeatures = &deviceFeatures; // Physical Device features Logical Device will use

	// Create the logical device for the given physical device
	VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceCreateInfo, nullptr, &mainDevice.logicalDevice);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Logical Device!");
	}
	else
	{
		std::cout << "Successfully created a Logical Device" << std::endl;
	}

	// Queues are created at the same time as the device...
	// So we want a handle to the queue
	vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(mainDevice.logicalDevice, indices.presentationFamily, 0, &presentationQueue);
}

void VulkanRenderer::createSurface()
{
	// Create Surface (creates a surface create info struct, runs the create surface function, returns result)
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a surface!");
	}
	else
	{
		std::cout << "Successfully created a surface" << std::endl;
	}
}

void VulkanRenderer::createSwapChain()
{
	// Get Swap Chain details so we can pick best settings
	SwapChainDetails swapChainDetails = getSwapChainDetails(mainDevice.physicalDevice);

	// Find optimal surface values for our swap chain
	VkSurfaceFormatKHR surfaceFormat = chooseBestSurfaceFormat(swapChainDetails.formats);
	VkPresentModeKHR presentMode = chooseBestPresentationMode(swapChainDetails.presentationModes);
	VkExtent2D extent = chooseSwapExtent(swapChainDetails.surfaceCapabilities);

	// How many images are in the swap chain? Get 1 more than the minimum to allow triple buffering
	uint32_t imageCount = swapChainDetails.surfaceCapabilities.minImageCount + 1;

	// If maxImageCount is 0, then limitless amount of images can be in swap chain
	if (swapChainDetails.surfaceCapabilities.maxImageCount > 0 
		&& imageCount > swapChainDetails.surfaceCapabilities.maxImageCount)
	{
		imageCount = swapChainDetails.surfaceCapabilities.maxImageCount;
	}

	// Creation information for the swap chain
	VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
	swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainCreateInfo.surface = surface; // Swapchain surface
	swapChainCreateInfo.imageFormat = surfaceFormat.format; // Swapchain format
	swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace; // Swapchain color space
	swapChainCreateInfo.presentMode = presentMode; // Swapchain presentation mode
	swapChainCreateInfo.imageExtent = extent; // Swapchain image extents
	swapChainCreateInfo.minImageCount = imageCount; // Minimum images in swapchain
	swapChainCreateInfo.imageArrayLayers = 1; // Number of layers for each image in chain
	swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // What attachment images will be used as
	swapChainCreateInfo.preTransform = swapChainDetails.surfaceCapabilities.currentTransform; // Transform to perform on swap chain images
	swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // How to handle blending images with external graphics (e.g. other windows)
	swapChainCreateInfo.clipped = VK_TRUE; // Whether to clip parts of image not in view (e.g. behind another window, off screen, etc)

	// Get Queue Family Indices
	QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

	// If Graphics and Presentation families are different, then swapchain must let images be shared between families
	if (indices.graphicsFamily != indices.presentationFamily)
	{
		// Queues to share between
		uint32_t queueFamilyIndices[] = {
			(uint32_t)indices.graphicsFamily,
			(uint32_t)indices.presentationFamily
		};
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; // Image share handling
		swapChainCreateInfo.queueFamilyIndexCount = 2; // Number of queues to share images between
		swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices; // Array of queues to share between

		std::cout << "\tGraphics and Presentation Families are different" << std::endl;
	}
	else
	{
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // Image share handling
		swapChainCreateInfo.queueFamilyIndexCount = 0; // Number of queues to share between
		swapChainCreateInfo.pQueueFamilyIndices = nullptr; // Array of queues to share between

		std::cout << "\tGraphics and Presentation Families are the same" << std::endl;
	}

	// If old swap chain been destroyed and this one replaces it, then link old one to quickly hand over responsibilities
	swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	// Create Swapchain
	VkResult result = vkCreateSwapchainKHR(mainDevice.logicalDevice, &swapChainCreateInfo, nullptr, &swapChain);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Swapchain!");
	}
	else
	{
		std::cout << "Successfully created a Swapchain" << std::endl;
	}

	// Store for later reference
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;

	// Get swap chain images (first count, then values)
	uint32_t swapChainImageCount;
	vkGetSwapchainImagesKHR(mainDevice.logicalDevice, swapChain, &swapChainImageCount, nullptr);
	std::vector<VkImage> images(swapChainImageCount);
	vkGetSwapchainImagesKHR(mainDevice.logicalDevice, swapChain, &swapChainImageCount, images.data());

	// Create image views for swap chain images
	for (VkImage image : images)
	{
		SwapChainImage swapChainImage = {};
		swapChainImage.image = image;
		swapChainImage.imageView = createImageView(image, swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);

		swapChainImages.push_back(swapChainImage);
	}
}

void VulkanRenderer::createRenderPass()
{
	// ATTACHMENTS
	// Describes the attachments used by the render pass
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = swapChainImageFormat; // Format to use for attachment
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // Number of samples to write for multisampling
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // Describes what to do with attachment before rendering
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // Describes what to do with attachment after rendering
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // Describes what to do with stencil before rendering
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // Describes what to do with stencil after rendering

	// Framebuffer data will be stored as an image, but images can be given different data layouts
	// to give optimal use for certain operations
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // Image data layout before render pass starts
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Image data layout after render pass (to change to)

	// Attachment reference uses an attachment index that refers to index in the attachment list passed to RenderPassCreateInfo
	VkAttachmentReference colorAttachmentReference = {};
	colorAttachmentReference.attachment = 0; // Attachment index
	colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // Layout to use during subpass

	// Information about a particular subpass the Render Pass is using
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // Pipeline type subpass is to be bound to
	subpass.colorAttachmentCount = 1; // Number of color attachments
	subpass.pColorAttachments = &colorAttachmentReference; // Reference to the color attachment in the attachment list

	// Need to determine when layout transitions occur using subpass dependencies
	std::array<VkSubpassDependency, 2> subpassDependencies{};
	// Conversion from VK_IMAGE_LAYOUT_UNDEFINED to VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	// Transition must happen after...
	subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL; // Subpass index (VK_SUBPASS_EXTERNAL = Special value meaning outside of renderpass)
	subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT; // Pipeline stage
	subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT; // Stage access mask (memory access)
	// But must happen before...
	subpassDependencies[0].dstSubpass = 0; // Subpass index (first and only subpass)
	subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // Pipeline stage
	subpassDependencies[0].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; // Stage access mask (memory access)
	subpassDependencies[0].dependencyFlags = 0; // Dependency flags

	// Conversion from VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	// Transition must happen after...
	subpassDependencies[1].srcSubpass = 0; // Sub
	subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // Pipeline stage
	subpassDependencies[1].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; // Stage access mask (memory access)
	// But must happen before...
	subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL; // Subpass index (VK_SUBPASS_EXTERNAL = Special value meaning outside of renderpass)
	subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT; // Pipeline stage
	subpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT; // Stage access mask (memory access)
	subpassDependencies[1].dependencyFlags = 0; // Dependency flags

	// Create info for Render Pass
	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = 1; // Number of attachments used by render pass
	renderPassCreateInfo.pAttachments = &colorAttachment; // Descriptions of attachments
	renderPassCreateInfo.subpassCount = 1; // Number of subpasses
	renderPassCreateInfo.pSubpasses = &subpass; // Subpasses used by render pass
	renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size()); // Number of subpass dependencies
	renderPassCreateInfo.pDependencies = subpassDependencies.data(); // Subpass dependencies used by render pass

	// Create render pass
	VkResult result = vkCreateRenderPass(mainDevice.logicalDevice, &renderPassCreateInfo, nullptr, &renderPass);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Render Pass!");
	}
	else
	{
		std::cout << "Successfully created a Render Pass" << std::endl;
	}
}

void VulkanRenderer::createGraphicsPipeline()
{
	// Read in SPIR-V code of shaders
	auto vertShaderCode = readFile("shaders/vert.spv");
	auto fragShaderCode = readFile("shaders/frag.spv");

	// Build Shader Modules to link to Graphics Pipeline
	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	// -- SHADER STAGE CREATION INFORMATION --
	// Vertex Stage creation information
	VkPipelineShaderStageCreateInfo vertShaderCreateInfo = {};
	vertShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT; // Shader Stage name
	vertShaderCreateInfo.module = vertShaderModule; // Shader module to be used by stage
	vertShaderCreateInfo.pName = "main"; // Entry point in to shader

	// Fragment Stage creation information
	VkPipelineShaderStageCreateInfo fragShaderCreateInfo = {};
	fragShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT; // Shader Stage name
	fragShaderCreateInfo.module = fragShaderModule; // Shader module to be used by stage
	fragShaderCreateInfo.pName = "main"; // Entry point in to shader

	// Put shader stage creation info in to array
	// Graphics Pipeline creation info requires array of shader stages
	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderCreateInfo, fragShaderCreateInfo };

	// CREATE PIPELINE //////////////////////////////

	// -- VERTEX INPUT / (TODO: Put in Vertex Descriptions) --
	VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
	vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputCreateInfo.vertexBindingDescriptionCount = 0;
	vertexInputCreateInfo.pVertexBindingDescriptions = nullptr; // List of Vertex Binding Descriptions (data spacing/stride information)
	vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
	vertexInputCreateInfo.pVertexAttributeDescriptions = nullptr; // List of Vertex Attribute Descriptions (data format and where to bind to/from)
	
	// -- INPUT ASSEMBLY --
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {};
	inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // Primitive type to assemble vertices as
	inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE; // Allow overriding of "strip" topology to start new primitives

	// -- VIEWPORT & SCISSOR --
	// Create a viewport info struct
	VkViewport viewport = {};
	viewport.x = 0.0f; // x start coordinate
	viewport.y = 0.0f; // y start coordinate
	viewport.width = (float)swapChainExtent.width; // width of viewport
	viewport.height = (float)swapChainExtent.height; // height of viewport
	viewport.minDepth = 0.0f; // min framebuffer depth
	viewport.maxDepth = 1.0f; // max framebuffer depth

	// Create a scissor info struct
	VkRect2D scissor = {};
	scissor.offset = { 0, 0 }; // Offset to use region from
	scissor.extent = swapChainExtent; // Extent to describe region to use, starting at offset

	// Combine viewport and scissor to a viewport state info struct
	VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
	viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateCreateInfo.viewportCount = 1;
	viewportStateCreateInfo.pViewports = &viewport;
	viewportStateCreateInfo.scissorCount = 1;
	viewportStateCreateInfo.pScissors = &scissor;

	//// -- DYNAMIC STATES --
	//// Dynamic states to enable
	//std::vector<VkDynamicState> dynamicStateEnables;
	//dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT); // Dynamic Viewport : Can resize in command buffer with vkCmdSetViewport(commandbuffer, 0, 1, &viewport);
	//dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR); // Dynamic Scissor : Can resize in command buffer with vkCmdSetScissor(commandbuffer, 0, 1, &scissor);

	//// Dynamic State creation info
	//VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
	//dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	//dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
	//dynamicStateCreateInfo.pDynamicStates = dynamicStateEnables.data();

	// -- RASTERIZER --
	VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {};
	rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizerCreateInfo.depthClampEnable = VK_FALSE; // Change if fragments beyond near/far planes are clipped (default) or clamped to plane
	rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE; // Whether to discard data and skip rasterizer. Never creates fragments, only suitable for pipeline without framebuffer output
	rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL; // How to handle filling points between vertices
	rasterizerCreateInfo.lineWidth = 1.0f; // How thick lines should be when drawn
	rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT; // Which face of a triangle to cull
	rasterizerCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // Winding to determine which side is front
	rasterizerCreateInfo.depthBiasEnable = VK_FALSE; // Whether to add depth bias to fragments (good for stopping "shadow acne" in shadow mapping)

	// -- MULTISAMPLING --
	VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo = {};
	multisamplingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisamplingCreateInfo.sampleShadingEnable = VK_FALSE; // Enable multisample shading or not
	multisamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; // Number of samples to use per fragment

	// -- BLENDING --
	// Blending decides how to blend a new color being written to a fragment, with the old value 
	
	// Blend Attachment State (how blending is handled)
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT 
										| VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT; // Colors to apply blending to
	colorBlendAttachment.blendEnable = VK_TRUE; // Enable blending

	// Blending uses the following equation: (srcColorBlendFactor * new color) colorBlendOp (dstColorBlendFactor * old color)
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA; // How to blend new color with old color
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; // How to factor old color in blending
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Type of blending to apply
	// Summarized: (VK_BLEND_FACTOR_SRC_ALPHA * new color) + (VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA * old color)
	// 			   (new color alpha * new color) + ((1 - new color alpha) * old color)

	// Additional settings for alpha blending
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // How to blend new alpha with old alpha
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // How to factor old alpha in blending
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Type of blending to apply
	// Summarized: (1 * new alpha) + (0 * old alpha) = new alpha

	VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = {};
	colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendCreateInfo.logicOpEnable = VK_FALSE; // Alternative to calculations is to use logical operations
	colorBlendCreateInfo.attachmentCount = 1;
	colorBlendCreateInfo.pAttachments = &colorBlendAttachment;

	// -- PIPELINE LAYOUT --
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 0; // Number of Layouts to set
	pipelineLayoutCreateInfo.pSetLayouts = nullptr; // Layouts to set (if using push-constants)
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0; // Number of push constant ranges
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr; // push constant ranges

	// Create Pipeline Layout
	VkResult result = vkCreatePipelineLayout(mainDevice.logicalDevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Pipeline Layout!");
	}
	else
	{
		std::cout << "Successfully created a Pipeline Layout" << std::endl;
	}

	// -- DEPTH STENCIL TESTING --
	// TODO: Add Depth Buffering to pipeline

	// -- GRAPHICS PIPELINE CREATION --
	// Array of pipeline shader stages
	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stageCount = 2;
	pipelineCreateInfo.pStages = shaderStages; // List of shader stages
	pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo; // All the fixed function pipeline states
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
	pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
	pipelineCreateInfo.pDynamicState = nullptr; // No dynamic state
	pipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
	pipelineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
	pipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
	pipelineCreateInfo.layout = pipelineLayout; // Pipeline Layout pipeline should use
	pipelineCreateInfo.renderPass = renderPass; // Render Pass the pipeline is compatible with
	pipelineCreateInfo.subpass = 0; // Sub
	
	// Pipeline Derivatives : Can create multiple pipelines that derive from one another for optimisation
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // Handle to existing pipeline to derive from
	pipelineCreateInfo.basePipelineIndex = -1; // Index of base pipeline handle (index in array, not a pipeline)

	// Create Graphics Pipeline
	result = vkCreateGraphicsPipelines(mainDevice.logicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &graphicsPipeline);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Graphics Pipeline!");
	}
	else
	{
		std::cout << "Successfully created a Graphics Pipeline" << std::endl;
	}

	// Destroy Shader Modules, no longer needed after Pipeline created
	vkDestroyShaderModule(mainDevice.logicalDevice, vertShaderModule, nullptr);
	vkDestroyShaderModule(mainDevice.logicalDevice, fragShaderModule, nullptr);
}

void VulkanRenderer::createFramebuffers()
{
	// Resize framebuffer count to equal swap chain image count
	swapChainFramebuffers.resize(swapChainImages.size());

	// Create a framebuffer for each swap chain image
	for (size_t i = 0; i < swapChainFramebuffers.size(); i++)
	{
		std::array<VkImageView, 1> attachments = {
			swapChainImages[i].imageView
		};

		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = renderPass; // Render Pass layout the Framebuffer will be used with
		framebufferCreateInfo.attachmentCount = 1; // Number of attachments
		framebufferCreateInfo.pAttachments = attachments.data(); // List of attachments (1:1 with Render Pass)
		framebufferCreateInfo.width = swapChainExtent.width; // Framebuffer width
		framebufferCreateInfo.height = swapChainExtent.height; // Framebuffer height
		framebufferCreateInfo.layers = 1; // Framebuffer layers

		// Create Framebuffer
		VkResult result = vkCreateFramebuffer(mainDevice.logicalDevice, &framebufferCreateInfo, nullptr, &swapChainFramebuffers[i]);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create a Framebuffer!");
		}
	}
}

void VulkanRenderer::createCommandPool()
{
	// Get the queue family indices for the chosen Physical Device
	QueueFamilyIndices queueFamilyIndices = getQueueFamilies(mainDevice.physicalDevice);

	VkCommandPoolCreateInfo poolCreateInfo = {};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Allow reuse of recorded command buffers
	poolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily; // Queue Family type that buffers from this command pool will use

	// Create a Graphics Queue Family Command Pool
	VkResult result = vkCreateCommandPool(mainDevice.logicalDevice, &poolCreateInfo, nullptr, &graphicsCommandPool);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Command Pool!");
	}
	else
	{
		std::cout << "Successfully created a Command Pool" << std::endl;
	}
}

void VulkanRenderer::createCommandBuffers()
{
	// Resize command buffer count to have one for each framebuffer
	commandBuffers.resize(swapChainFramebuffers.size());

	VkCommandBufferAllocateInfo commandBufferAllocInfo = {};
	commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocInfo.commandPool = graphicsCommandPool; // Command Pool to allocate from
	commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // VK_COMMAND_BUFFER_LEVEL_PRIMARY : Can be submitted to queue for execution, but cannot be called from other command buffers
																	// VK_COMMAND_BUFFER_LEVEL_SECONDARY : Cannot be submitted directly, but can be called from primary command buffers
	commandBufferAllocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	// Allocate command buffers and place handles in array of buffers
	VkResult result = vkAllocateCommandBuffers(mainDevice.logicalDevice, &commandBufferAllocInfo, commandBuffers.data());
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate Command Buffers!");
	}
	else
	{
		std::cout << "Successfully allocated Command Buffers" << std::endl;
	}
}

void VulkanRenderer::recordCommands()
{
	// Information about how to begin each command buffer
	VkCommandBufferBeginInfo bufferBeginInfo = {};
	bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	bufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;	// Buffer can be resubmitted when it has already been submitted and is awaiting execution

	// Information about how to begin a render pass (only needed for graphical applications)
	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = renderPass;							// Render Pass to begin
	renderPassBeginInfo.renderArea.offset = { 0, 0 };						// Start point of render pass in pixels
	renderPassBeginInfo.renderArea.extent = swapChainExtent;				// Size of region to run render pass on (starting at offset)
	VkClearValue clearValues[] = {
		{0.6f, 0.65f, 0.4, 1.0f}
	};
	renderPassBeginInfo.pClearValues = clearValues;							// List of clear values (TODO: Depth Attachment Clear Value)
	renderPassBeginInfo.clearValueCount = 1;

	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		renderPassBeginInfo.framebuffer = swapChainFramebuffers[i];

		// Start recording commands to command buffer!
		VkResult result = vkBeginCommandBuffer(commandBuffers[i], &bufferBeginInfo);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to start recording a Command Buffer!");
		}

		// Begin Render Pass
		vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		// Bind Pipeline to be used in render pass
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

		// Execute pipeline
		vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

		// End Render Pass
		vkCmdEndRenderPass(commandBuffers[i]);

		// Stop recording to command buffer
		result = vkEndCommandBuffer(commandBuffers[i]);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to stop recording a Command Buffer!");
		}
	}
}

void VulkanRenderer::setupDebugMessenger()
{
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

void VulkanRenderer::getPhysicalDevice()
{
	std::cout << "	Getting Physical Device..." << std::endl;
	// Enumerate Physical devices the vkInstance can access
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	// If no devices available, none support Vulkan!
	if (deviceCount == 0) 
	{
		throw std::runtime_error("Can't find GPUs that support Vulkan Instance!");
	}

	// Get list of physical devices
	std::vector<VkPhysicalDevice> deviceList(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());

	for (const auto& device : deviceList) 
	{
		if (checkDeviceSuitable(device)) 
		{
			mainDevice.physicalDevice = device;
			break;
		}
	}

	// Get properties of our newly found physical device
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(mainDevice.physicalDevice, &deviceProperties);

	std::cout << "	Found Physical Device : " << deviceProperties.deviceName << std::endl;
}

bool VulkanRenderer::checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions)
{
	// Need to get number of extensions to create array of correct size to hold extensions
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	// Create a list of VkExtensionProperties using count
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	std::cout << "Available Extensions:" << std::endl;
	for (auto& e : extensions)
	{
		std::cout << "\t" << e.extensionName << std::endl;
	}

	// Check if given extensions are in list of available extensions
	for (const char* extension : *checkExtensions) 
	{
		bool hasExtension = false;
		for (const auto& vkExtension : extensions) 
		{
			// Compare the extension names (strcmp)
			if (strcmp(extension, vkExtension.extensionName) == 0) 
			{
				hasExtension = true;
				break;
			}
		}

		if (!hasExtension) 
		{
			return false; // Missing extension
		}
	}

	return true; // All extensions found
}

bool VulkanRenderer::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	// Get number of extensions for the given device
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	// If no extensions found, return failure
	if (extensionCount == 0)
	{
		return false;
	}

	// Populate list of extensions
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());

	// Check for extension
	for (const char* deviceExtension : deviceExtensions)
	{
		bool hasExtension = false;
		for (const auto& extension : extensions)
		{
			if (strcmp(deviceExtension, extension.extensionName) == 0)
			{
				hasExtension = true;
				break;
			}
		}

		if (!hasExtension)
		{
			return false;
		}
	}

	return true;	
}

bool VulkanRenderer::checkDeviceSuitable(VkPhysicalDevice device)
{
	/*
	// Information about the device itself (ID, name, type, vendor, etc)
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	// Information about what the device can do (geo shader, tess shader, wide lines, etc)
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	*/

	QueueFamilyIndices indices = getQueueFamilies(device);

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainValid = false;
	if (extensionsSupported)
	{
		SwapChainDetails swapChainDetails = getSwapChainDetails(device);
		swapChainValid = !swapChainDetails.formats.empty() && !swapChainDetails.presentationModes.empty();
	}

	return indices.isValid() && extensionsSupported && swapChainValid;
}

void VulkanRenderer::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = /* VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | */ VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

bool VulkanRenderer::checkValidationLayerSupport()
{
	// Get number of validation layers to create array of correct size
	uint32_t validationLayerCount;
	vkEnumerateInstanceLayerProperties(&validationLayerCount, nullptr);

	// Check if no validation layers are available
	if (validationLayerCount == 0)
	{
		return false;
	}

	// Create a list of validation layers with given count
	std::vector<VkLayerProperties> availableLayers(validationLayerCount);
	vkEnumerateInstanceLayerProperties(&validationLayerCount, availableLayers.data());

	// Check if given validation layer is in list of given validation layers
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
		{
			return false;
		}
	}

	return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRenderer::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

VkResult VulkanRenderer::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void VulkanRenderer::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

QueueFamilyIndices VulkanRenderer::getQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	// Get all Queue Family Property info for the given device
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());

	// Go through each queue family and check if it has at least 1 of the required types of queue
	int i = 0;
	for (const auto& queueFamily : queueFamilyList) 
	{
		// First check if queue family has at least 1 queue in that family (could have no queues)
		// Queue can be multiple types defined through bitfield. Need to bitwise AND with VK_QUEUE_*_BIT to check if has required type
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
		{
			indices.graphicsFamily = i; // If queue family is valid, then get index
		}

		// Check if queue family supports presentation
		VkBool32 presentationSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentationSupport);
		if (queueFamily.queueCount > 0 && presentationSupport)
		{
			indices.presentationFamily = i;
		}

		// Check if queue family indices are in a valid state, stop searching if so
		if (indices.isValid())
		{
			break;
		}

		i++;
	}

	return indices;
}

SwapChainDetails VulkanRenderer::getSwapChainDetails(VkPhysicalDevice device)
{
	SwapChainDetails swapChainDetails;

	// -- CAPABILITIES --
	// Get the surface capabilities for the given surface on the given physical device
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &swapChainDetails.surfaceCapabilities); // Get surface capabilities

	// -- FORMATS --
	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr); // Get number of formats

	// If formats returned, get list of formats
	if (formatCount != 0)
	{
		swapChainDetails.formats.resize(formatCount); // Resize list to fit number of formats
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, swapChainDetails.formats.data()); // Get list of surface formats
	}

	// -- PRESENTATION MODES --
	uint32_t presentationCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationCount, nullptr); // Get number of presentation modes

	// If presentation modes returned, get list of presentation modes
	if (presentationCount != 0)
	{
		swapChainDetails.presentationModes.resize(presentationCount); // Resize list to fit number of presentation modes
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationCount, swapChainDetails.presentationModes.data()); // Get list of surface presentation modes
	}

	return swapChainDetails;
}

VkSurfaceFormatKHR VulkanRenderer::chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
{
	// If only 1 format available and is undefined, then this means ALL formats are available
	if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
	{
		return { VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	// If restricted, search for optimal format
	for (const auto& format : formats)
	{
		if (format.format == VK_FORMAT_R8G8B8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return format;
		}
	}

	return formats[0]; // Return first format if can't find a better one
}

VkPresentModeKHR VulkanRenderer::chooseBestPresentationMode(const std::vector<VkPresentModeKHR>& presentationModes)
{
	// Look for mailbox presentation mode
	for (const auto& presentationMode : presentationModes)
	{
		if (presentationMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return presentationMode;
		}
	}

	// If can't find, use FIFO as Vulkan spec says it must be present
	return VK_PRESENT_MODE_FIFO_KHR; // Always available
}

VkExtent2D VulkanRenderer::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities)
{
	// If current extent is at numeric limits, then extent can vary. Otherwise, it is the size of the window
	if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return surfaceCapabilities.currentExtent;
	}
	else
	{
		// If value can vary, need to set manually
		int width, height;
		glfwGetFramebufferSize(window, &width, &height); // Get window size

		// Create new extent using window size
		VkExtent2D newExtent = {};
		newExtent.width = static_cast<uint32_t>(width);
		newExtent.height = static_cast<uint32_t>(height);

		// Surface also defines max and min, so make sure within boundaries by clamping value
		newExtent.width = std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.width, newExtent.width));
		newExtent.height = std::max(surfaceCapabilities.minImageExtent.height, std::min(surfaceCapabilities.maxImageExtent.height, newExtent.height));

		return newExtent;
	}
}

VkImageView VulkanRenderer::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image; // Image to create view for
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // Type of image (1D, 2D, 3D, Cube, etc)
	viewInfo.format = format; // Format of image data
	viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY; // Allows remapping of rgba components to other rgba values
	viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	// Subresources allow the view to view only a part of an image
	viewInfo.subresourceRange.aspectMask = aspectFlags; // Which aspect of image to view (e.g. COLOR_BIT for viewing color)
	viewInfo.subresourceRange.baseMipLevel = 0; // Start mipmap level to view from
	viewInfo.subresourceRange.levelCount = 1; // Number of mipmap levels to view
	viewInfo.subresourceRange.baseArrayLayer = 0; // Start array level to view from
	viewInfo.subresourceRange.layerCount = 1; // Number of array levels to view

	// Create image view and return it
	VkImageView imageView;
	if (vkCreateImageView(mainDevice.logicalDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create an Image View!");
	}

	return imageView;
}

VkShaderModule VulkanRenderer::createShaderModule(const std::vector<char>& code)
{
	// Shader Module creation information
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size(); // Size of code
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data()); // Pointer to code

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(mainDevice.logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a shader module!");
	}

	return shaderModule;
}

std::vector<const char*> VulkanRenderer::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions; // Extensions to use
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount); // Get extensions from GLFW

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); // Name of extension we want to enable
		// Print available extensions
		std::cout << "GLFW Extensions:" << std::endl;
		for (uint32_t i = 0; i < glfwExtensionCount; i++)
		{
			std::cout << "\t" << glfwExtensions[i] << std::endl;
		}
	}

	return extensions;
}
