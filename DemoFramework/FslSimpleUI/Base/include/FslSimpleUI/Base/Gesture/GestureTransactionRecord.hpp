#ifndef FSLSIMPLEUI_BASE_GESTURE_GESTURETRANSACTIONRECORD_HPP
#define FSLSIMPLEUI_BASE_GESTURE_GESTURETRANSACTIONRECORD_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslBase/Math/Dp/DpPoint2F.hpp>
#include <FslBase/Time/MillisecondTickCount32.hpp>
#include <FslSimpleUI/Base/Gesture/GestureFlags.hpp>
#include <FslSimpleUI/Base/Gesture/GestureTransactionMode.hpp>
#include <FslSimpleUI/Base/MovementOwnership.hpp>
#include <FslSimpleUI/Base/MovementTransactionAction.hpp>

namespace Fsl::UI
{
  class GestureTransactionRecord
  {
    GestureTransactionMode m_mode{GestureTransactionMode::Invalid};
    MillisecondTickCount32 m_downTimestamp;
    DpPoint2F m_initialDownPositionDpf;
    bool m_inMotion{false};
    bool m_allowTap{false};
    MovementTransactionAction m_action{MovementTransactionAction::NotInterested};

  public:
    GestureTransactionRecord() noexcept = default;
    GestureTransactionRecord(const GestureFlags enabledGestures, const GestureTransactionMode mode, const MillisecondTickCount32 downTimestamp,
                             const DpPoint2F initialDownPositionDpf);

    void DisableTap();
    // void MarkAsHold();
    void MarkAsInMotion();

    void TakeControl();

    MillisecondTickCount32 GetDownTimestamp() const noexcept
    {
      return m_downTimestamp;
    }

    DpPoint2F GetInitialDownPositionDpf() const noexcept
    {
      return m_initialDownPositionDpf;
    }

    MovementTransactionAction ClaimAction();
    MovementTransactionAction ClaimCancelAction(const MovementOwnership movementOwnership);
    MovementTransactionAction ClaimEndAction();

    bool CanBeTap() const noexcept
    {
      return m_allowTap && !m_inMotion;
    }

    bool IsHold() const noexcept
    {
      return false;
    }

    bool InMotion() const noexcept
    {
      return m_inMotion;
    }

    GestureTransactionMode Mode() const noexcept
    {
      return m_mode;
    }

    bool IsDefault() const noexcept
    {
      return m_mode == GestureTransactionMode::Invalid;
    }
  };
}

#endif
