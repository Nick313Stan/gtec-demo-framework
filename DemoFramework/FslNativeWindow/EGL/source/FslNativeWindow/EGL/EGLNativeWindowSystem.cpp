/****************************************************************************************************************************************************
 * Copyright 2023 NXP
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

#include <FslBase/Exceptions.hpp>
#include <FslNativeWindow/EGL/Adapter/IEGLNativeWindowAdapter.hpp>
#include <FslNativeWindow/EGL/Adapter/IEGLNativeWindowSystemAdapter.hpp>
#include <FslNativeWindow/EGL/EGLNativeWindow.hpp>
#include <FslNativeWindow/EGL/EGLNativeWindowSystem.hpp>
#include <FslNativeWindow/EGL/IEGLNativeWindow.hpp>
#include <memory>
#include <utility>

namespace Fsl
{
  EGLNativeWindowSystem::EGLNativeWindowSystem(const NativeWindowSystemSetup& setup, std::unique_ptr<IEGLNativeWindowSystemAdapter> adapter)
    : PlatformNativeWindowSystem(setup, std::move(adapter))
    , m_pCachedAdapter(dynamic_cast<IEGLNativeWindowSystemAdapter*>(TryGetAdapter()))
  {
    if (m_pCachedAdapter == nullptr)
    {
      throw std::invalid_argument("can not be null");
    }
  }


  EGLNativeWindowSystem::~EGLNativeWindowSystem() = default;


  void EGLNativeWindowSystem::Shutdown()
  {
    m_pCachedAdapter = nullptr;
    PlatformNativeWindowSystem::Shutdown();
  }


  std::shared_ptr<INativeWindow>
    EGLNativeWindowSystem::CreateNativeWindow(const NativeWindowSetup& nativeWindowSetup,
                                              const PlatformNativeWindowAllocationParams* const pPlatformCustomWindowAllocationParams)
  {
    if (m_pCachedAdapter == nullptr)
    {
      throw ObjectShutdownException("CreateNativeWindow");
    }

    std::shared_ptr<IEGLNativeWindowAdapter> adapterEx;
    {
      std::shared_ptr<IPlatformNativeWindowAdapter> adapter =
        m_pCachedAdapter->CreateNativeWindow(nativeWindowSetup, pPlatformCustomWindowAllocationParams);

      adapterEx = std::dynamic_pointer_cast<IEGLNativeWindowAdapter>(adapter);
      if (!adapterEx)
      {
        throw InternalErrorException("window not of the correct type");
      }
    }
    return std::make_shared<EGLNativeWindow>(adapterEx);
  }


  EGLNativeDisplayType EGLNativeWindowSystem::GetDisplayType() const
  {
    if (m_pCachedAdapter == nullptr)
    {
      throw ObjectShutdownException("GetDisplayType");
    }

    return m_pCachedAdapter->GetDisplayType();
  }


  std::shared_ptr<IEGLNativeWindow> EGLNativeWindowSystem::CreateEGLNativeWindow(const NativeWindowSetup& nativeWindowSetup,
                                                                                 const NativeEGLSetup& nativeEglSetup)
  {
    if (m_pCachedAdapter == nullptr)
    {
      throw ObjectShutdownException("CreateEGLNativeWindow");
    }

    auto windowAdapter = m_pCachedAdapter->CreateEGLNativeWindow(nativeWindowSetup, nativeEglSetup);
    return std::make_shared<EGLNativeWindow>(windowAdapter);
  }
}