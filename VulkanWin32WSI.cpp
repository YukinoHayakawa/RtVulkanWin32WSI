﻿/*
 * This file implements platform-dependent functions in VulkanGpuDevice.
 * To choose an implementation on each platform, link with related project.
 */

#ifdef _WIN32

#include <Usagi/Extensions/RtWin32/Win32.hpp>
#define VK_USE_PLATFORM_WIN32_KHR
#include <Usagi/Extensions/RtVulkan/VulkanGpuDevice.hpp>
#include <Usagi/Extensions/RtVulkan/VulkanSwapchain.hpp>

#include <Usagi/Core/Logging.hpp>
#include <Usagi/Utility/TypeCast.hpp>
#include <Usagi/Extensions/RtWin32/Window/Win32Window.hpp>

void usagi::VulkanGpuDevice::addPlatformSurfaceExtension(
    std::vector<const char *> &extensions)
{
    extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
}

std::shared_ptr<usagi::Swapchain> usagi::VulkanGpuDevice::createSwapchain(
    Window *window)
{
    auto &win32_window = dynamic_cast_ref<Win32Window>(window);

    LOG(info, "Creating Win32Surface for window: {}", win32_window.title());

    vk::Win32SurfaceCreateInfoKHR surface_create_info;
    surface_create_info.setHinstance(reinterpret_cast<HINSTANCE>(
        GetWindowLongPtrW(win32_window.handle(), GWLP_HINSTANCE)));
    surface_create_info.setHwnd(win32_window.handle());

    auto surface = mInstance->createWin32SurfaceKHRUnique(surface_create_info);
    if(!mPhysicalDevice.getSurfaceSupportKHR(
        mGraphicsQueueFamilyIndex, surface.get()
    )) USAGI_THROW(std::runtime_error(
        "Graphics queue doesn't support Win32 surface"));
    return std::make_shared<VulkanSwapchain>(this, std::move(surface));
}

void usagi::VulkanGpuDevice::checkQueuePresentationCapacity(
    const uint32_t queue_family_index) const
{
    if(!mPhysicalDevice.getWin32PresentationSupportKHR(queue_family_index))
        USAGI_THROW(std::runtime_error(
            "Graphics queue doesn't support Win32 presentation"));
}

#endif
