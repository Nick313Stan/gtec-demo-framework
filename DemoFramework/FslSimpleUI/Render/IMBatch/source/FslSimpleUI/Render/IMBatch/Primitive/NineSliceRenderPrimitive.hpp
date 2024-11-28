#ifndef FSLSIMPLEUI_RENDER_IMBATCH_PRIMITIVE_NINESLICERENDERPRIMITIVE_HPP
#define FSLSIMPLEUI_RENDER_IMBATCH_PRIMITIVE_NINESLICERENDERPRIMITIVE_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslGraphics/Sprite/Info/Core/RenderOptimizedNineSliceInfo.hpp>

namespace Fsl::UI::RenderIMBatch
{
  struct NineSliceRenderPrimitive
  {
    uint32_t MeshVertexCapacity{0};
    uint32_t MeshIndexCapacity{0};
    RenderOptimizedNineSliceInfo RenderInfo;

    //------------------------------------------------------------------------------------------------------------------------------------------------

    constexpr NineSliceRenderPrimitive() noexcept = default;

    //------------------------------------------------------------------------------------------------------------------------------------------------

    constexpr NineSliceRenderPrimitive(const uint32_t meshVertexCapacity, const uint32_t meshIndexCapacity,
                                       const RenderOptimizedNineSliceInfo& renderInfo) noexcept
      : MeshVertexCapacity(meshVertexCapacity)
      , MeshIndexCapacity(meshIndexCapacity)
      , RenderInfo(renderInfo)
    {
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    constexpr static NineSliceRenderPrimitive PatchVertexCapacity(const NineSliceRenderPrimitive& source, const uint32_t vertexCapacity) noexcept
    {
      return {vertexCapacity, source.MeshIndexCapacity, source.RenderInfo};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    constexpr static NineSliceRenderPrimitive PatchIndexCapacity(const NineSliceRenderPrimitive& source, const uint32_t indexCapacity) noexcept
    {
      return {source.MeshVertexCapacity, indexCapacity, source.RenderInfo};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    constexpr static NineSliceRenderPrimitive PatchRenderInfo(const NineSliceRenderPrimitive& source,
                                                              const RenderOptimizedNineSliceInfo& renderInfo) noexcept
    {
      return {source.MeshVertexCapacity, source.MeshIndexCapacity, renderInfo};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    // op==
    constexpr bool operator==(const NineSliceRenderPrimitive& other) const noexcept = default;
  };
}

#endif
