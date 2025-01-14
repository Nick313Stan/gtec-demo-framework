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

#include <FslBase/Math/Point2.hpp>
#include <FslDemoApp/Base/Overlay/DemoAppProfilerGraph.hpp>
#include <FslDemoService/Graphics/IBasic2D.hpp>
#include <algorithm>
#include <cassert>
#include <stdexcept>

namespace Fsl
{
  DemoAppProfilerGraph::DemoAppProfilerGraph(const int64_t minValue, const int64_t maxValue, const Point2& size)
    : m_minValue(minValue)
    , m_maxValue(maxValue)
    , m_size(size)
    , m_entries(size.X)
    , m_coords(size.X)
  {
    if (m_maxValue < m_minValue)
    {
      throw std::invalid_argument("min value must be less or equal to max value");
    }
  }


  Point2 DemoAppProfilerGraph::GetSize() const
  {
    return m_size;
  }


  void DemoAppProfilerGraph::Add(const int64_t latestValue)
  {
    int64_t cappedValue = std::min(std::max(latestValue - m_minValue, static_cast<int64_t>(0)), m_maxValue - m_minValue);
    m_entries.push_back(cappedValue);
  }


  void DemoAppProfilerGraph::Draw(const std::shared_ptr<IBasic2D>& basic2D, const Vector2& dstPosition, const Color& color)
  {
    int32_t count = 0;
    const float scaleY = static_cast<float>(m_size.Y - 1) / static_cast<float>(m_maxValue - m_minValue);
    float dstX = dstPosition.X;
    if (m_entries.size() < static_cast<std::size_t>(m_size.X))
    {
      dstX += static_cast<float>(m_size.X - static_cast<int32_t>(m_entries.size()));
    }
    const float dstY = dstPosition.Y + static_cast<float>(m_size.Y) - 1;
    for (std::size_t i = 0; i < m_entries.size(); ++i)
    {
      m_coords[count].X = dstX + static_cast<float>(count);
      m_coords[count].Y = dstY - (static_cast<float>(m_entries[i]) * scaleY);
      ++count;
    }

    basic2D->DrawPoints(m_coords.data(), count, color);
  }
}
