#ifndef GRIDRENDER_GLES3_SPRINGBACKGROUND_GRIDRENDERNATIVEBATCHCRSPLINE3_HPP
#define GRIDRENDER_GLES3_SPRINGBACKGROUND_GRIDRENDERNATIVEBATCHCRSPLINE3_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <FslBase/Math/Point2.hpp>
#include <cstddef>
#include "../IGridRender.hpp"

namespace Fsl
{
  class GridRenderNativeBatchCRSpline3 : public IGridRender
  {
    Point2 m_gridSize;
    Point2 m_gridFinalSize;
    std::vector<Vector3> m_coordinates3D;
    std::vector<Vector2> m_coordinates2D;

  public:
    explicit GridRenderNativeBatchCRSpline3(const Point2& gridSize);

    const char* GetName() const override;
    void Update(const DemoTime& demoTime, const Vector2& areaSize, const std::vector<PointMass>& points) override;
    void Draw(const GridRenderDrawContext& drawContext, const std::vector<PointMass>& points) override;

  private:
    void Calc3DCoordinates(std::vector<Vector3>& rDst, const std::vector<PointMass>& points);
    int32_t CreateLinesHorizontal(std::vector<Vector3>& rDst, const std::vector<PointMass>& points, const std::ptrdiff_t dstOffset);
    int32_t CreateLinesVertical(std::vector<Vector3>& rDst, const std::vector<PointMass>& points, const std::ptrdiff_t dstOffset);
    void CalcFinalCoordinates(std::vector<Vector2>& rDst, const std::vector<Vector3>& src, const Vector2& finalSize);
  };
}


#endif
