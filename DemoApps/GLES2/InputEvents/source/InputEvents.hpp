#ifndef GLES2_INPUTEVENTS_INPUTEVENTS_HPP
#define GLES2_INPUTEVENTS_INPUTEVENTS_HPP
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

#include <FslDemoApp/OpenGLES2/DemoAppGLES2.hpp>
#include <FslDemoService/Graphics/IBasic2D.hpp>
#include <FslDemoApp/Base/Service/Gamepad/IGamepads.hpp>
#include <deque>
#include <string>
#include <vector>

namespace Fsl
{
  class InputEvents : public DemoAppGLES2
  {
    std::shared_ptr<IBasic2D> m_basic2D;
    std::deque<std::string> m_console;
    std::shared_ptr<IGamepads> m_gamepads;
    std::vector<GamepadState> m_gamepadStates;
  public:
    InputEvents(const DemoAppConfig& config);
    ~InputEvents();

    virtual void OnKeyEvent(const KeyEvent& event);
    virtual void OnMouseButtonEvent(const MouseButtonEvent& event);
    virtual void OnMouseMoveEvent(const MouseMoveEvent& event);
    virtual void OnMouseWheelEvent(const MouseWheelEvent& event);
    virtual void OnRawMouseMoveEvent(const RawMouseMoveEvent& event);
  protected:
    virtual void Update(const DemoTime& demoTime) override;
    virtual void Draw(const DemoTime& demoTime) override;
  private:
    void UpdateGamepadStates();
  };
}

#endif
