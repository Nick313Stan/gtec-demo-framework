#ifndef SHARED_VULKANWILLEMSDEMOAPPEXPERIMENTAL_SWAPCHAINBUFFERS_HPP
#define SHARED_VULKANWILLEMSDEMOAPPEXPERIMENTAL_SWAPCHAINBUFFERS_HPP
/*
 * Class wrapping access to the swap chain
 *
 * A swap chain is a collection of framebuffers used for rendering and presentation to the windowing system
 *
 * Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
 *
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */


// Based on a code by Sascha Willems from https://github.com/SaschaWillems/Vulkan

// Recreated as a DemoFramework freestyle window sample by Freescale (2016)
// This class simulates the functionality found in vulkanswapchain to make it easier
// to port samples.
// It is not a straight port, but it has instead been converted to follow the RAII principle
// used in this framework

#include <RapidVulkan/Image.hpp>
#include <RapidVulkan/ImageView.hpp>
#include <vulkan/vulkan.h>

namespace Fsl::Willems
{
  class SwapchainBuffers
  {
  public:
    VkImage Image;
    RapidVulkan::ImageView View;

    SwapchainBuffers(const SwapchainBuffers&) = delete;
    SwapchainBuffers& operator=(const SwapchainBuffers&) = delete;

    //! @brief Move assignment operator
    SwapchainBuffers& operator=(SwapchainBuffers&& other) noexcept;

    //! @brief Move constructor
    //! Transfer ownership from other to this
    SwapchainBuffers(SwapchainBuffers&& other) noexcept;

    //! @brief Create a 'invalid' swapchain buffers
    SwapchainBuffers();
  };
}

#endif
