#ifndef SHARED_UI_BENCHMARK_APP_ITESTAPP_HPP
#define SHARED_UI_BENCHMARK_APP_ITESTAPP_HPP
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

#include <FslSimpleUI/Base/UIColor.hpp>
#include <memory>

namespace Fsl
{
  namespace UI
  {
    struct RenderSystemStats;
    class IRenderSystemBase;
  }

  class CustomUIDemoAppExtension;
  struct DemoTime;
  struct DemoWindowMetrics;
  struct PxRectangle;
  struct PxViewport;

  class ITestApp
  {
  public:
    virtual ~ITestApp() = default;

    virtual UI::UIColor GetRootColor() const = 0;
    virtual bool TrySetRootColor(const UI::UIColor color) = 0;

    virtual const UI::IRenderSystemBase& GetRenderSystem() const = 0;
    virtual UI::IRenderSystemBase* TryGetRenderSystem() = 0;

    virtual std::shared_ptr<CustomUIDemoAppExtension> GetCustomUIDemoAppExtension() const = 0;
    virtual void SetCustomViewport(const PxViewport& viewportPx) = 0;
    virtual void SetClipRectangle(const bool enabled, const PxRectangle& clipRectanglePx) = 0;
    virtual void SetUseDrawCache(const bool useDrawCache) = 0;

    virtual bool IsUIIdle() const = 0;

    virtual void OnConfigurationChanged(const DemoWindowMetrics& windowMetrics) = 0;
    virtual void Update(const DemoTime& demoTime) = 0;
    virtual void Draw() = 0;
    virtual void OnDrawSkipped() = 0;
  };
}

#endif
