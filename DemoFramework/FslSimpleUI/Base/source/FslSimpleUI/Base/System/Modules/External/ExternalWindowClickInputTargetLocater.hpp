#ifndef FSLSIMPLEUI_BASE_SYSTEM_MODULES_EXTERNAL_EXTERNALWINDOWCLICKINPUTTARGETLOCATER_HPP
#define FSLSIMPLEUI_BASE_SYSTEM_MODULES_EXTERNAL_EXTERNALWINDOWCLICKINPUTTARGETLOCATER_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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

#include <FslSimpleUI/Base/Module/IWindowClickInputTargetLocater.hpp>
#include <memory>
#include <utility>
#include "../../ITreeNodeClickInputTargetLocater.hpp"
#include "../../TreeNode.hpp"

namespace Fsl::UI
{
  class ExternalWindowClickInputTargetLocater final : public IWindowClickInputTargetLocater
  {
    std::shared_ptr<ITreeNodeClickInputTargetLocater> m_targetLocator;

  public:
    explicit ExternalWindowClickInputTargetLocater(std::shared_ptr<ITreeNodeClickInputTargetLocater> targetLocator)
      : m_targetLocator(std::move(targetLocator))
    {
    }

    void Dispose() noexcept
    {
      m_targetLocator.reset();
    }

    // From IWindowClickInputTargetLocater
    std::shared_ptr<IWindowId> TryGetMouseOverWindow(const PxPoint2& hitPositionPx) const final
    {
      if (m_targetLocator)
      {
        auto node = m_targetLocator->TryGetMouseOverWindow(hitPositionPx);
        return node ? node->GetWindow() : std::shared_ptr<IWindowId>();
      }
      return {};
    }

    std::shared_ptr<IWindowId> TryGetClickInputWindow(const PxPoint2& hitPositionPx) const final
    {
      if (m_targetLocator)
      {
        auto node = m_targetLocator->TryGetClickInputWindow(hitPositionPx);
        return node ? node->GetWindow() : std::shared_ptr<IWindowId>();
      }
      return {};
    }
  };
}

#endif
