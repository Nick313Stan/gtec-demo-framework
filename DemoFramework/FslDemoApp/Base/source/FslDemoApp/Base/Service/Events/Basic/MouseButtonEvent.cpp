/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslDemoApp/Base/Service/Events/Basic/MouseButtonEvent.hpp>
#include <FslNativeWindow/Base/NativeWindowEventHelper.hpp>

namespace Fsl
{
  MouseButtonEvent::MouseButtonEvent(const VirtualMouseButton::Enum button, const bool isPressed, const PxPoint2& position, const bool isTouch)
    : BasicEvent(EventType::MouseButton, button, Encode(isPressed, isTouch), NativeWindowEventHelper::EncodePosition(position))
  {
  }


  VirtualMouseButton::Enum MouseButtonEvent::GetButton() const
  {
    return static_cast<VirtualMouseButton::Enum>(m_arg1);
  }


  PxPoint2 MouseButtonEvent::GetPosition() const
  {
    return NativeWindowEventHelper::DecodePosition(m_arg3);
  }


  constexpr int32_t MouseButtonEvent::Encode(const bool isPressed, const bool isTouch) noexcept
  {
    return static_cast<int32_t>(isPressed ? Flags::Pressed : Flags::NoFlags) | static_cast<int32_t>(isTouch ? Flags::Touch : Flags::NoFlags);
  }
}
