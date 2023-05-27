#include <BananaEngine.h>
#include "Application/Application.h"

//Check mem leaks
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#if 1
int main()
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
#endif

	Application newApp("TeeHee Revenge");

	newApp.Init();
	newApp.Awake();
	newApp.Run();
	newApp.Shutdown();
}
#endif


#if 0

#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

//#define IMGUI_UNLIMITED_FRAME_RATE
#ifdef _DEBUG
//#define IMGUI_VULKAN_DEBUG_REPORT
#endif

static VkAllocationCallbacks* g_Allocator = NULL;
static VkInstance               g_Instance = VK_NULL_HANDLE;
static VkPhysicalDevice         g_PhysicalDevice = VK_NULL_HANDLE;
static VkDevice                 g_Device = VK_NULL_HANDLE;
static uint32_t                 g_QueueFamily = (uint32_t)-1;
static VkQueue                  g_Queue = VK_NULL_HANDLE;
static VkDebugReportCallbackEXT g_DebugReport = VK_NULL_HANDLE;
static VkPipelineCache          g_PipelineCache = VK_NULL_HANDLE;
static VkDescriptorPool         g_DescriptorPool = VK_NULL_HANDLE;

static ImGui_ImplVulkanH_Window g_MainWindowData;
static int                      g_MinImageCount = 2;
static bool                     g_SwapChainRebuild = false;


static void check_vk_result(VkResult err)
{
	if (err == 0)
		return;
	fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}

#ifdef IMGUI_VULKAN_DEBUG_REPORT
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
	(void)flags; (void)object; (void)location; (void)messageCode; (void)pUserData; (void)pLayerPrefix; // Unused arguments
	fprintf(stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);
	return VK_FALSE;
}
#endif // IMGUI_VULKAN_DEBUG_REPORT

static void SetupVulkan(const char** extensions, uint32_t extensions_count)
{
	VkResult err;

	// Create Vulkan Instance
	{
		VkInstanceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.enabledExtensionCount = extensions_count;
		create_info.ppEnabledExtensionNames = extensions;
#ifdef IMGUI_VULKAN_DEBUG_REPORT
		// Enabling validation layers
		const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
		create_info.enabledLayerCount = 1;
		create_info.ppEnabledLayerNames = layers;

		// Enable debug report extension (we need additional storage, so we duplicate the user array to add our new extension to it)
		const char** extensions_ext = (const char**)malloc(sizeof(const char*) * (extensions_count + 1));
		memcpy(extensions_ext, extensions, extensions_count * sizeof(const char*));
		extensions_ext[extensions_count] = "VK_EXT_debug_report";
		create_info.enabledExtensionCount = extensions_count + 1;
		create_info.ppEnabledExtensionNames = extensions_ext;

		// Create Vulkan Instance
		err = vkCreateInstance(&create_info, g_Allocator, &g_Instance);
		check_vk_result(err);
		free(extensions_ext);

		// Get the function pointer (required for any extensions)
		auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(g_Instance, "vkCreateDebugReportCallbackEXT");
		IM_ASSERT(vkCreateDebugReportCallbackEXT != NULL);

		// Setup the debug report callback
		VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
		debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
		debug_report_ci.pfnCallback = debug_report;
		debug_report_ci.pUserData = NULL;
		err = vkCreateDebugReportCallbackEXT(g_Instance, &debug_report_ci, g_Allocator, &g_DebugReport);
		check_vk_result(err);
#else
		// Create Vulkan Instance without any debug feature
		err = vkCreateInstance(&create_info, g_Allocator, &g_Instance);
		check_vk_result(err);
		IM_UNUSED(g_DebugReport);
#endif
	}

	// Select GPU
	{
		uint32_t gpu_count;
		err = vkEnumeratePhysicalDevices(g_Instance, &gpu_count, NULL);
		check_vk_result(err);
		IM_ASSERT(gpu_count > 0);

		VkPhysicalDevice* gpus = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * gpu_count);
		err = vkEnumeratePhysicalDevices(g_Instance, &gpu_count, gpus);
		check_vk_result(err);

		// If a number >1 of GPUs got reported, find discrete GPU if present, or use first one available. This covers
		// most common cases (multi-gpu/integrated+dedicated graphics). Handling more complicated setups (multiple
		// dedicated GPUs) is out of scope of this sample.
		int use_gpu = 0;
		for (int i = 0; i < (int)gpu_count; i++)
		{
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(gpus[i], &properties);
			if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				use_gpu = i;
				break;
			}
		}

		g_PhysicalDevice = gpus[use_gpu];
		free(gpus);
	}

	// Select graphics queue family
	{
		uint32_t count;
		vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, NULL);
		VkQueueFamilyProperties* queues = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * count);
		vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, queues);
		for (uint32_t i = 0; i < count; i++)
			if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				g_QueueFamily = i;
				break;
			}
		free(queues);
		IM_ASSERT(g_QueueFamily != (uint32_t)-1);
	}

	// Create Logical Device (with 1 queue)
	{
		int device_extension_count = 1;
		const char* device_extensions[] = { "VK_KHR_swapchain" };
		const float queue_priority[] = { 1.0f };
		VkDeviceQueueCreateInfo queue_info[1] = {};
		queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_info[0].queueFamilyIndex = g_QueueFamily;
		queue_info[0].queueCount = 1;
		queue_info[0].pQueuePriorities = queue_priority;
		VkDeviceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
		create_info.pQueueCreateInfos = queue_info;
		create_info.enabledExtensionCount = device_extension_count;
		create_info.ppEnabledExtensionNames = device_extensions;
		err = vkCreateDevice(g_PhysicalDevice, &create_info, g_Allocator, &g_Device);
		check_vk_result(err);
		vkGetDeviceQueue(g_Device, g_QueueFamily, 0, &g_Queue);
	}

	// Create Descriptor Pool
	{
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		err = vkCreateDescriptorPool(g_Device, &pool_info, g_Allocator, &g_DescriptorPool);
		check_vk_result(err);
	}
}

// All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used by the demo.
// Your real engine/app may not use them.
static void SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height)
{
	wd->Surface = surface;

	// Check for WSI support
	VkBool32 res;
	vkGetPhysicalDeviceSurfaceSupportKHR(g_PhysicalDevice, g_QueueFamily, wd->Surface, &res);
	if (res != VK_TRUE)
	{
		fprintf(stderr, "Error no WSI support on physical device 0\n");
		exit(-1);
	}

	// Select Surface Format
	const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
	const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(g_PhysicalDevice, wd->Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

	// Select Present Mode
#ifdef IMGUI_UNLIMITED_FRAME_RATE
	VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
	VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
	wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(g_PhysicalDevice, wd->Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
	//printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

	// Create SwapChain, RenderPass, Framebuffer, etc.
	IM_ASSERT(g_MinImageCount >= 2);
	ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, wd, g_QueueFamily, g_Allocator, width, height, g_MinImageCount);
}

static void CleanupVulkan()
{
	vkDestroyDescriptorPool(g_Device, g_DescriptorPool, g_Allocator);

#ifdef IMGUI_VULKAN_DEBUG_REPORT
	// Remove the debug report callback
	auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(g_Instance, "vkDestroyDebugReportCallbackEXT");
	vkDestroyDebugReportCallbackEXT(g_Instance, g_DebugReport, g_Allocator);
#endif // IMGUI_VULKAN_DEBUG_REPORT

	vkDestroyDevice(g_Device, g_Allocator);
	vkDestroyInstance(g_Instance, g_Allocator);
}

//static void CleanupVulkanWindow()
//{
//    ImGui_ImplVulkanH_DestroyWindow(g_Instance, g_Device, &g_MainWindowData, g_Allocator);
//}

static void FrameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data)
{
	VkResult err;

	VkSemaphore image_acquired_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
	VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
	err = vkAcquireNextImageKHR(g_Device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex);
	if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
	{
		g_SwapChainRebuild = true;
		return;
	}
	check_vk_result(err);

	ImGui_ImplVulkanH_Frame* fd = &wd->Frames[wd->FrameIndex];
	{
		err = vkWaitForFences(g_Device, 1, &fd->Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
		check_vk_result(err);

		err = vkResetFences(g_Device, 1, &fd->Fence);
		check_vk_result(err);
	}
	{
		err = vkResetCommandPool(g_Device, fd->CommandPool, 0);
		check_vk_result(err);
		VkCommandBufferBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
		check_vk_result(err);
	}
	{
		VkRenderPassBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		info.renderPass = wd->RenderPass;
		info.framebuffer = fd->Framebuffer;
		info.renderArea.extent.width = wd->Width;
		info.renderArea.extent.height = wd->Height;
		info.clearValueCount = 1;
		info.pClearValues = &wd->ClearValue;
		vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
	}

	// Record dear imgui primitives into command buffer
	ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

	// Submit command buffer
	vkCmdEndRenderPass(fd->CommandBuffer);
	{
		VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &image_acquired_semaphore;
		info.pWaitDstStageMask = &wait_stage;
		info.commandBufferCount = 1;
		info.pCommandBuffers = &fd->CommandBuffer;
		info.signalSemaphoreCount = 1;
		info.pSignalSemaphores = &render_complete_semaphore;

		err = vkEndCommandBuffer(fd->CommandBuffer);
		check_vk_result(err);
		err = vkQueueSubmit(g_Queue, 1, &info, fd->Fence);
		check_vk_result(err);
	}
}

static void FramePresent(ImGui_ImplVulkanH_Window* wd)
{
	if (g_SwapChainRebuild)
		return;
	VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
	VkPresentInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	info.waitSemaphoreCount = 1;
	info.pWaitSemaphores = &render_complete_semaphore;
	info.swapchainCount = 1;
	info.pSwapchains = &wd->Swapchain;
	info.pImageIndices = &wd->FrameIndex;
	VkResult err = vkQueuePresentKHR(g_Queue, &info);
	if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
	{
		g_SwapChainRebuild = true;
		return;
	}
	check_vk_result(err);
	wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->ImageCount; // Now we can use the next set of semaphores
}

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

#undef UUID_DEFINED
int main()
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	using namespace BE;
	JPH::RegisterDefaultAllocator();
	GameEngine::CreateInstance();
	//std::cout << typeid(Transform).name() << "\n";

	//Quatf q({ 1,2,3 }, 60.0f);
	//std::cout << q << std::endl;
	//std::cout << q.GetEulerAngles() << std::endl;
	//std::cout << q.GetAxis() << " " << q.GetRotation_Deg() << std::endl;
	//std::cout << Mat3f{}.SetRotation(q) << std::endl;

	//std::cout << Quatf{ 20.84047f,    55.02511f,    28.48606f } << std::endl;
#if 0

	//AssetMgr->LoadAsset<TestFile>(AssetID{ 123 }, "Hello\\Hello.txt", 20, 21.22);
	//AssetMgr->LoadAsset<TestFile>(AssetID{ 456 }, "good/bye.txt", 10, 9999.22);
	//AssetMgr->LoadAsset<TestFile>("goodbye1.txt", 10, 9999.22);
	//AssetMgr->LoadAsset<TestFile>("goodbye2.txt", 10, 9999.22);
	//AssetMgr->LoadAsset<TestFile>("goodbye3.txt", 10, 9999.22);
	//AssetMgr->LoadAsset<TestFile>("goodbye4.txt", 10, 9999.22);
	//AssetMgr->GetAssetData<TestFile>(AssetID{ 123 }).Render();
	//AssetMgr->GetAssetData<TestFile>("Hello/Hello.txt").Render();
	//std::cout << AssetMgr->IsAssetDataLoadedForType<TestFile>(AssetID{ 123 });
	//std::cout << AssetMgr->IsAssetDataLoadedForType<TestFile>(AssetID{ 789 });
	//std::cout << AssetMgr->IsAssetDataLoadedForType<TestFile>("Hello/Hello.txt");
	//std::cout << AssetMgr->IsAssetDataLoadedForType<TestFile>("Hello/Hello123.txt");
	//std::cout << AssetMgr->IsAssetDataLoaded(AssetID{ 123 });
	//std::cout << AssetMgr->IsAssetDataLoaded(AssetID{ 789 });
	//std::cout << AssetMgr->IsAssetDataLoaded("Hello/Hello.txt");
	//std::cout << AssetMgr->IsAssetDataLoaded("Hello/Hello123.txt") << std::endl;
	//
	//std::cout << "|" << AssetMgr->GetAssetID("good\\bye.txt") << "|" << std::endl;
	//std::cout << "|" << AssetMgr->GetAssetFileName(AssetID{ 123 }) << "|" << std::endl;
	//std::cout << "|" << AssetMgr->GetAssetID("go222od\\bye.txt") << "|" << std::endl;
	//std::cout << "|" << AssetMgr->GetAssetFileName(AssetID{ 1234 }) << "|" << std::endl;
	//
	//std::cout << "|" << AssetMgr->GetAssetTruncatedFileName(AssetID{ 123 }) << "|" << std::endl;
	//std::cout << "|" << AssetMgr->GetAssetTypeOf(AssetID{ 123 }) << "|" << std::endl;
	//std::cout << "|" << AssetMgr->GetAssetTypeIDOf(AssetID{ 123 }) << "|" << std::endl;

	EntityID e1 = ECS->CreateEntity(BEID{ 1 });
	EntityID e2 = ECS->CreateEntity(BEID{ 2 });
	EntityID e3 = ECS->CreateEntity(BEID{ 3 });
	EntityID e4 = ECS->CreateEntity(BEID{ 4 });
	EntityID e5 = ECS->CreateEntity(BEID{ 5 });
	EntityID e6 = ECS->CreateEntity(BEID{ 6 });

	ECS->GetComponent<EntityInfo>(e1).SetName("hello1");
	ECS->GetComponent<EntityInfo>(e2).SetName("hello2");
	ECS->GetComponent<EntityInfo>(e3).SetName("hello3");
	ECS->GetComponent<EntityInfo>(e4).SetName("hello4");
	ECS->GetComponent<EntityInfo>(e5).SetName("hello5");
	ECS->GetComponent<EntityInfo>(e6).SetName("hello6");

	Transform& te1 = dynamic_cast<Transform&>(ECS->GetComponent("Transform", e1));
	Transform& te2 = dynamic_cast<Transform&>(ECS->GetComponent("Transform", e2));
	Transform& te3 = dynamic_cast<Transform&>(ECS->GetComponent("Transform", e3));
	Transform& te4 = dynamic_cast<Transform&>(ECS->GetComponent("Transform", e4));
	Transform& te5 = dynamic_cast<Transform&>(ECS->GetComponent("Transform", e5));
	//EntityInfo& ie1 = dynamic_cast<EntityInfo&>(ECS->GetComponent<EntityInfo>(e1));
	//EntityInfo& ie2 = dynamic_cast<EntityInfo&>(ECS->GetComponent<EntityInfo>(e2));
	//EntityInfo& ie3 = dynamic_cast<EntityInfo&>(ECS->GetComponent<EntityInfo>(e3));
	//EntityInfo& ie4 = dynamic_cast<EntityInfo&>(ECS->GetComponent<EntityInfo>(e4));
	//EntityInfo& ie5 = dynamic_cast<EntityInfo&>(ECS->GetComponent<EntityInfo>(e5));

	te1.SetPosition({ 1,0,0 });
	te2.SetPosition({ 2,0,0 });
	te3.SetPosition({ 3,0,0 });
	te4.SetPosition({ 4,0,0 });
	te5.SetPosition({ 5,0,0 });

	te1.AttachChild(e1, e2);
	te1.AttachChild(e1, e3);
	te2.AttachChild(e2, e4);
	te2.AttachChild(e2, e5);

   //std::cout << Quatf({ 1,1,1 }, 120.0f) << std::endl;
   //std::cout << Quatf({ 1,1,1 }, 60.0f) << std::endl;
   //std::cout << Quatf({ 1,1,1 }, -60.0f) << std::endl;
   //std::cout << Mat3f{}.SetRotation(Quatf({ 1,1,1 }, 60.0f)) << std::endl;
   //std::cout << Vec3f(1,1, 0) + Mat3f{}.SetRotation(Quatf({ 1,1,1 }, 60.0f)) * Vec3f(2,0,0) << std::endl;
   //std::cout << Vec3f(1,1, 0) + Mat3f{}.SetRotation(Quatf({ 1,1,1 }, 60.0f)) * Vec3f(3,0,0) << std::endl;

	te1.SetRotation(Quatf{ {1,1,1}, 120.0f });
	te2.SetLocalRotation(Quatf{ {1,1,1}, -60.0f });
	te2.SetLocalRotation(Quatf{ {1,1,1}, -120.0f });
	//te1.SetScale({ 0.5f,0.5f,0.5f });

	ECS->DestroyEntity(e3);

	for (EntityID eid : ECS->GetAllEntities())
		std::cout << eid << std::endl;

	te1.SetPosition({ 1,1,1 });
	te2.SetPosition({ 2,2,2 });
	te4.SetPosition({ 4,4,4 });
	Transform& te6 = ECS->GetComponent<Transform>(e6);

    te6;
    
#endif
#if 0
	BE::EntityID p = BE::ECS->CreateEntity(BE::BEID{ 1 });
	BE::EntityID c = BE::ECS->CreateEntity(BE::BEID{ 2 });
	BehaviourSystem::GetInstance()->RegisterBehaviour<TestBehaviour>();
	BehaviourSystem::GetInstance()->RegisterBehaviour<AnotherBehaviour>();
	for (auto& name : BehaviourSystem::GetInstance()->GetAllBehaviourNames()) {
		std::cout << name << " is registered \n";
	}
	ECS->CreateTag("Special");
	ECS->CreateTag("Diff");

	EntityID floor = ECS->CreateEntity(BEID{ 1 });
	EntityID wshape = ECS->CreateEntity(BEID{ 2 });
	EntityID ball2 = ECS->CreateEntity(BEID{ 3 });
	EntityID ball3 = ECS->CreateEntity(BEID{ 4 });

	EntityInfo& efloor = dynamic_cast<EntityInfo&>(ECS->GetComponent<EntityInfo>(floor));
	efloor.SetName("Floor");
	EntityInfo& ewshape = dynamic_cast<EntityInfo&>(ECS->GetComponent<EntityInfo>(wshape));
	ewshape.SetName("Weird Shape");
	ewshape.AddTag("Special");
	ewshape.AddTag("Diff");
	EntityInfo& eball2 = dynamic_cast<EntityInfo&>(ECS->GetComponent<EntityInfo>(ball2));
	eball2.SetName("Ball 2");
	eball2.AddTag("Special");
	EntityInfo& eball3 = dynamic_cast<EntityInfo&>(ECS->GetComponent<EntityInfo>(ball3));
	eball3.SetName("Ball 3");
	eball3.AddTag("Diff");

	Transform& tfloor = dynamic_cast<Transform&>(ECS->GetComponent("Transform", floor));
	Transform& twshape = dynamic_cast<Transform&>(ECS->GetComponent("Transform", wshape));
	Transform& tball2 = dynamic_cast<Transform&>(ECS->GetComponent("Transform", ball2));
	Transform& tball3 = dynamic_cast<Transform&>(ECS->GetComponent("Transform", ball3));

	std::vector<CompoundSubShape> shappes;
	//add physics component, and set positions
	ECS->AddComponent(floor, Physics(Shape::Box, JPH::EMotionType::Static, 100.0f, 0.5f, 100.0f));
	tfloor.SetPosition({ 0.0f, 0.0f, 0.0f });
	ECS->AddComponent(wshape, Physics(Shape::StaticCompound, JPH::EMotionType::Dynamic, 0.5f, 0.f, 0.f, JPH::EActivation::Activate, false, 0.f, 0.f, {
		{{0.f, 0.f, 0.f}, {0.f,0.f,0.f,1.f}, Shape::Box, {5.f,1.f,1.f}},
		{{5.f, 0.f, 0.f}, {0.f,0.f,0.f,1.f}, Shape::Sphere, {1.5f,0.f,0.f}},
		{{-5.f, 0.f, 0.f}, {0.f,0.f,0.f,1.f}, Shape::Sphere, {1.5f,0.f,0.f}} }));
	twshape.SetPosition({ 0.0f, 10.0f, 0.0f });
	ECS->AddComponent(ball2, Physics(Shape::Sphere, JPH::EMotionType::Dynamic, 1.f));
	tball2.SetPosition({ 0.0f, 6.0f, 0.0f });
	ECS->AddComponent(ball3, Physics(Shape::Sphere, JPH::EMotionType::Dynamic, 1.f));
	tball3.SetPosition({ 0.0f, 7.0f, 0.0f });


	ECS->AddComponent<Script>(ball2);
	Script& sball2 = ECS->GetComponent<Script>(ball2);
	sball2.SetBehaviour<TestBehaviour>();
	std::cout << eball2.GetName() << " has behaviour: " << sball2.GetBehaviourName() << "\n";
	ECS->RemoveComponent<Script>(ball2);


#endif
#if 0
    BE::EntityID p = BE::ECS->CreateEntity(BE::UUID{ 1 });
    BE::EntityID c = BE::ECS->CreateEntity(BE::UUID{ 2 });

	BE::EntityID p = BE::ECS->CreateEntity(BE::UUID{ 1 });
	BE::EntityID c = BE::ECS->CreateEntity(BE::UUID{ 2 });

	BE::ECS->GetComponent<BE::Transform>(p).SetPosition({ 2.0f, 3.0f, 0.0f });
	BE::ECS->GetComponent<BE::Transform>(p).SetRotation({ {1,1,1}, 120.0f });
	BE::ECS->GetComponent<BE::Transform>(p).SetScale({ 2.5f, 0.5f, 1.25f });

	BE::ECS->GetComponent<BE::Transform>(c).SetPosition({ 5.0f, 0.0f, 0.0f });
	BE::ECS->GetComponent<BE::Transform>(c).SetRotation({ {1,1,1}, 240.0f });
	BE::ECS->GetComponent<BE::Transform>(p).AttachChild(p, c);

	//get local pos
	std::cout << BE::ECS->GetComponent<BE::Transform>(c).GetLocalPosition() << std::endl;

	BE::Mat3<float> s, r;
	r.SetRotation(BE::ECS->GetComponent<BE::Transform>(p).GetRotation());
	s.SetScale(BE::ECS->GetComponent<BE::Transform>(p).GetScale().x,
		BE::ECS->GetComponent<BE::Transform>(p).GetScale().y,
		BE::ECS->GetComponent<BE::Transform>(p).GetScale().z);

	//set local pos
	BE::ECS->GetComponent<BE::Transform>(c).SetLocalPosition({ 1, 1, 1 });
	std::cout << BE::ECS->GetComponent<BE::Transform>(c).GetPositon() << std::endl;

	//get local scale
	std::cout << BE::ECS->GetComponent<BE::Transform>(c).GetLocalScale() << std::endl;

	//set local scale
	BE::ECS->GetComponent<BE::Transform>(c).SetLocalScale({ 1, 1, 1 });
	std::cout << BE::ECS->GetComponent<BE::Transform>(c).GetScale() << std::endl;

	//get local rotation
	std::cout << BE::ECS->GetComponent<BE::Transform>(p).GetLocalRotation() << std::endl;
	std::cout << BE::ECS->GetComponent<BE::Transform>(p).GetLocalRotation().GetEulerAngles() << std::endl;
	std::cout << BE::ECS->GetComponent<BE::Transform>(c).GetLocalRotation() << std::endl;
	std::cout << BE::ECS->GetComponent<BE::Transform>(c).GetLocalRotation().GetEulerAngles() << std::endl;

	BE::ECS->GetComponent<BE::Transform>(c).SetLocalRotation(BE::Quaternion{ {1,1,1}, 60.0f });
	std::cout << BE::ECS->GetComponent<BE::Transform>(c).GetRotation() << std::endl;

#endif
	//using namespace BE;
	//BEID theId = ECS->CreateEntity();
	//if(false)
	//	ECS->AddComponent("Transform", theId);
	//else
	//	ECS->AddComponent<Transform>(theId);
	//Transform& tmr = ECS->GetComponent<Transform>(theId);
	//tmr.AttachParent(c);
	//Serialisation::SaveEntities("currentSave.json");
	//Serialisation::LoadEntities("currentSave.json");
	//BE::ECS->DeleteInstance();
	//glfwInit();
	//glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
	//uint32_t extensionCount = 0;
	//vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	//std::cout << extensionCount << " extensions supported\n";
	//glm::mat4 matrix;
	//glm::vec4 vec;
	//auto test = matrix * vec;
	//while (!glfwWindowShouldClose(window)) {
	//    glfwPollEvents();
	//}
	//glfwDestroyWindow(window);
	//glfwTerminate();
#if 1
	std::cout << rightVector << "\n";
	std::cout << zeroQuat << "\n";
	BE::Singleton<BananaEditor::EditorSystem>::SingletonPtr& editor = BananaEditor::EditorSystem::CreateInstance();

	//editor = BananaEditor::EditorSystem::CreateInstance();
	// Setup GLFW window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+Vulkan example", NULL, NULL);

	// Setup Vulkan
	if (!glfwVulkanSupported())
	{
		printf("GLFW: Vulkan Not Supported\n");
		return 1;
	}
	uint32_t extensions_count = 0;
	const char** extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
	SetupVulkan(extensions, extensions_count);

	// Create Window Surface
	VkSurfaceKHR surface;
	VkResult err = glfwCreateWindowSurface(g_Instance, window, g_Allocator, &surface);
	check_vk_result(err);

	// Create Framebuffers
	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	ImGui_ImplVulkanH_Window* wd = &g_MainWindowData;
	SetupVulkanWindow(wd, surface, w, h);

	//// Setup Dear ImGui context
	//IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO(); (void)io;
	////io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	////io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	//// Setup Dear ImGui style
	//ImGui::StyleColorsDark();
	////ImGui::StyleColorsClassic();
	//// Setup Platform/Renderer backends
	//ImGui_ImplGlfw_InitForVulkan(window, true);
	//ImGui_ImplVulkan_InitInfo init_info = {};
	//init_info.Instance = g_Instance;
	//init_info.PhysicalDevice = g_PhysicalDevice;
	//init_info.Device = g_Device;
	//init_info.QueueFamily = g_QueueFamily;
	//init_info.Queue = g_Queue;
	//init_info.PipelineCache = g_PipelineCache;
	//init_info.DescriptorPool = g_DescriptorPool;
	//init_info.Subpass = 0;
	//init_info.MinImageCount = g_MinImageCount;
	//init_info.ImageCount = wd->ImageCount;
	//init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	//init_info.Allocator = g_Allocator;
	//init_info.CheckVkResultFn = check_vk_result;
	//ImGui_ImplVulkan_Init(&init_info, wd->RenderPass);

	editor->Init(window, wd, g_Instance, g_PhysicalDevice,
		g_Device, g_QueueFamily, g_Queue, g_PipelineCache, g_DescriptorPool,
		g_MinImageCount, g_Allocator);
	Engine->Init();

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	// Upload Fonts
	{
		// Use any command queue
		VkCommandPool command_pool = wd->Frames[wd->FrameIndex].CommandPool;
		VkCommandBuffer command_buffer = wd->Frames[wd->FrameIndex].CommandBuffer;

		err = vkResetCommandPool(g_Device, command_pool, 0);
		check_vk_result(err);
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		err = vkBeginCommandBuffer(command_buffer, &begin_info);
		check_vk_result(err);

		ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

		VkSubmitInfo end_info = {};
		end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		end_info.commandBufferCount = 1;
		end_info.pCommandBuffers = &command_buffer;
		err = vkEndCommandBuffer(command_buffer);
		check_vk_result(err);
		err = vkQueueSubmit(g_Queue, 1, &end_info, VK_NULL_HANDLE);
		check_vk_result(err);

		err = vkDeviceWaitIdle(g_Device);
		check_vk_result(err);
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	// Our state
	//bool show_demo_window = true;
	//bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		// Resize swap chain?
		if (g_SwapChainRebuild)
		{
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			if (width > 0 && height > 0)
			{
				ImGui_ImplVulkan_SetMinImageCount(g_MinImageCount);
				ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, &g_MainWindowData, g_QueueFamily, g_Allocator, width, height, g_MinImageCount);
				g_MainWindowData.FrameIndex = 0;
				g_SwapChainRebuild = false;
			}
		}

		Engine->Update();
		editor->Update();

		ImDrawData* draw_data = ImGui::GetDrawData();
		const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
		if (!is_minimized)
		{
			wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
			wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
			wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
			wd->ClearValue.color.float32[3] = clear_color.w;
			FrameRender(wd, draw_data);
			FramePresent(wd);
		}
	}

	// Cleanup
	err = vkDeviceWaitIdle(g_Device);
	check_vk_result(err);

	editor->Shutdown(g_Instance, g_Device, g_MainWindowData, g_Allocator);
	//ImGui_ImplVulkan_Shutdown();
	//ImGui_ImplGlfw_Shutdown();
	//ImGui::DestroyContext();

	//CleanupVulkanWindow();
	CleanupVulkan();

	glfwDestroyWindow(window);
	glfwTerminate();
#endif
	return 0;
}

#endif
