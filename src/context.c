#include "context.h"

#include "g_context.h"
#include "physical_device.h"
#include "result.h"
#include "util.h"
#include "vk.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <assert.h>
#include <vulkan/vulkan_core.h>

Context gContext = CU_NULL_CONTEXT;

CuResult cu_context_init(const CuContextCreateInfo* pCreateInfo) {
    assert(!gContext.isInitialized);

    CuResult result = CU_ERROR_UNKNOWN;

    if (pCreateInfo == nullptr) {
        pCreateInfo = &CU_DEFAULT_CONTEXT_CREATE_INFO;
    }

    cu_assert_catch(glfwInit() == GLFW_TRUE, CU_ERROR_GLFW_INIT);

    cu_try_catch_vk(create_vk_instance(&gContext.instance, pCreateInfo->appName, 0));

    cu_try_catch(choose_physical_device(&gContext.physicalDeviceInfo, gContext.instance));

    cu_try_catch_vk(create_device(
        &gContext.device, 
        gContext.physicalDeviceInfo.handle,
        gContext.physicalDeviceInfo.iQueueFamily
    ));
    vkGetDeviceQueue(
        gContext.device,
        gContext.physicalDeviceInfo.iQueueFamily,
        0,
        &gContext.queue
    );

    cu_try_catch_vk(create_command_pool(
        &gContext.commandPool,
        gContext.device,
        gContext.physicalDeviceInfo.iQueueFamily
    ));

    gContext.isInitialized = true;
    return CU_SUCCESS;

FAIL:
    cu_context_terminate();
    return result;
}

void cu_context_terminate() {
    glfwTerminate();

    if (gContext.device != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(gContext.device);
        vkDestroyCommandPool(gContext.device, gContext.commandPool, nullptr);
        vkDestroyDevice(gContext.device, nullptr);
    }

    if (gContext.instance != VK_NULL_HANDLE) {
        vkDestroyInstance(gContext.instance, nullptr);
    }

    gContext = CU_NULL_CONTEXT;
}

void cu_context_wait_for_idle() {
    assert(gContext.isInitialized);

    vkDeviceWaitIdle(gContext.device);
}
