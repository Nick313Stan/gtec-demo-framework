#ifndef FSLGRAPHICS3D_BASICRENDER_ADAPTER_INATIVESHADERFACTORY_HPP
#define FSLGRAPHICS3D_BASICRENDER_ADAPTER_INATIVESHADERFACTORY_HPP
/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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

#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslGraphics/Render/Basic/Adapter/BasicNativeShaderHandle.hpp>

namespace Fsl
{
  struct BasicNativeShaderCreateInfo;
}

namespace Fsl::Graphics3D
{
  class INativeShaderFactory
  {
  public:
    virtual ~INativeShaderFactory() noexcept = default;

    //! @brief Get the predefined shaders.
    virtual ReadOnlySpan<BasicNativeShaderCreateInfo> GetPredefinedShaders() const = 0;

    virtual BasicNativeShaderHandle CreateShader(const BasicNativeShaderCreateInfo& createInfo) = 0;

    //! @brief destroy the given shader
    //! @return true if the shader was destroyed (this will always be true for a valid handle)
    virtual bool DestroyShader(const BasicNativeShaderHandle hShader) noexcept = 0;
  };
}

#endif
