#ifndef VULKAN_GPUTIMESTAMP_GPUTIMESTAMP_HPP
#define VULKAN_GPUTIMESTAMP_GPUTIMESTAMP_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    * Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *
 *    * Neither the name of the NXP. nor the names of
 *      its contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************************************************************************************/

#include <FslBase/Log/Log3Core.hpp>
#include <FslDemoApp/Vulkan/Basic/DemoAppVulkanBasic.hpp>
#include <FslSimpleUI/Base/Control/Switch.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferManager.hpp>
#include <RapidVulkan/DescriptorPool.hpp>
#include <RapidVulkan/QueryPool.hpp>
#include <Shared/GpuTimestamp/Shared.hpp>
#include <utility>
#include "FractalShaderMandelbrot.hpp"

namespace Fsl
{
  class GpuTimestamp final : public VulkanBasic::DemoAppVulkanBasic
  {
    struct Resources
    {
      std::shared_ptr<Vulkan::VMBufferManager> BufferManager;
      RapidVulkan::DescriptorPool DescriptorPool;
      RapidVulkan::QueryPool QueryPool;
      // VulkanQueryResult QueryResult;

      Resources() = default;
      Resources(const Resources&) = delete;
      Resources& operator=(const Resources&) = delete;
      Resources(Resources&& other) noexcept = delete;
      Resources& operator=(Resources&& other) noexcept = delete;

      Resources(std::shared_ptr<Vulkan::VMBufferManager> bufferManager, RapidVulkan::DescriptorPool descriptorPool, RapidVulkan::QueryPool queryPool)
        : BufferManager(std::move(bufferManager))
        , DescriptorPool(std::move(descriptorPool))
        , QueryPool(std::move(queryPool))
      {
      }
    };

    struct DependentResources
    {
      RapidVulkan::RenderPass MainRenderPass;
      bool QueueContainsTimestamp{false};
      bool HasPendingQuery{false};

      DependentResources() = default;
      DependentResources(const DependentResources&) = delete;
      DependentResources& operator=(const DependentResources&) = delete;
      DependentResources(DependentResources&& other) noexcept = delete;
      DependentResources& operator=(DependentResources&& other) noexcept = delete;

      void Reset() noexcept
      {
        // Reset in destruction order
        HasPendingQuery = false;
        QueueContainsTimestamp = false;
        MainRenderPass.Reset();
      }
    };

    bool m_isTimestampSupported{false};
    Shared m_shared;

    Resources m_resources;
    DependentResources m_dependentResources;

    FractalShaderMandelbrot m_scene;

  public:
    explicit GpuTimestamp(const DemoAppConfig& config);

    void EndDraw(const FrameInfo& frameInfo) override;

  protected:
    void ConfigurationChanged(const DemoWindowMetrics& windowMetrics) final;
    void OnKeyEvent(const KeyEvent& event) final;
    void Update(const DemoTime& demoTime) final;
    void VulkanDraw(const DemoTime& demoTime, RapidVulkan::CommandBuffers& rCmdBuffers, const VulkanBasic::DrawContext& drawContext) final;

    VkRenderPass OnBuildResources(const VulkanBasic::BuildResourcesContext& context) final;
    void OnFreeResources() final;

  private:
    static Resources CreateResources(const Vulkan::VUDevice& device, const Vulkan::VUDeviceQueueRecord& deviceQueue,
                                     const RenderConfig& renderConfig);
  };
}

#endif
