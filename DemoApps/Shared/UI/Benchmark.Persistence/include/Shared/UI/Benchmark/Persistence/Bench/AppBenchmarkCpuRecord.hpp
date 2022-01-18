#ifndef SHARED_UI_BENCHMARK_PERSISTENCE_BENCH_APPBENCHMARKCPURECORD_HPP
#define SHARED_UI_BENCHMARK_PERSISTENCE_BENCH_APPBENCHMARKCPURECORD_HPP
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

#include <FslBase/BasicTypes.hpp>

namespace Fsl
{
  struct AppBenchmarkCpuRecord
  {
    uint32_t UIProcessEvents{0};
    uint32_t UIUpdate{0};
    uint32_t UIDraw{0};
    uint32_t UIDrawPreprocess{0};
    uint32_t UIDrawGenMesh{0};
    uint32_t UIDrawFillBuffers{0};
    uint32_t UIDrawSchedule{0};

    constexpr AppBenchmarkCpuRecord() noexcept = default;
    constexpr AppBenchmarkCpuRecord(const uint32_t uiProcessEvents, const uint32_t uiUpdate, const uint32_t uiDraw, const uint32_t uiDrawPreprocess,
                                    const uint32_t uiDrawGenMesh, const uint32_t uiDrawFillBuffers, const uint32_t uiDrawSchedule) noexcept
      : UIProcessEvents(uiProcessEvents)
      , UIUpdate(uiUpdate)
      , UIDraw(uiDraw)
      , UIDrawPreprocess(uiDrawPreprocess)
      , UIDrawGenMesh(uiDrawGenMesh)
      , UIDrawFillBuffers(uiDrawFillBuffers)
      , UIDrawSchedule(uiDrawSchedule)
    {
    }
  };
}

#endif