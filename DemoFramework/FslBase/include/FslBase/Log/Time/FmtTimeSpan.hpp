#ifndef FSLBASE_LOG_TIME_FMTTIMESPAN_HPP
#define FSLBASE_LOG_TIME_FMTTIMESPAN_HPP
/****************************************************************************************************************************************************
 * Copyright 2021, 2024 NXP
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

#include <FslBase/Time/TimeSpan.hpp>
#include <fmt/format.h>

namespace fmt
{
  template <>
  struct formatter<Fsl::TimeSpan>
  {
    template <typename ParseContext>
    // NOLINTNEXTLINE(readability-identifier-naming)
    constexpr auto parse(ParseContext& ctx)
    {
      return ctx.begin();
    }

    template <typename FormatContext>
    // NOLINTNEXTLINE(readability-identifier-naming)
    auto format(const Fsl::TimeSpan& value, FormatContext& ctx) const
    {
      const int64_t ticksLessThanSeconds = (value.Ticks() % Fsl::TimeSpan::TicksPerSecond);
      if (value.Days() > 0)
      {
        return ticksLessThanSeconds <= 0
                 ? fmt::format_to(ctx.out(), "{}.{:02}:{:02}:{:02}", value.Days(), value.Hours(), value.Minutes(), value.Seconds())
                 : fmt::format_to(ctx.out(), "{}.{:02}:{:02}:{:02}.{:07}", value.Days(), value.Hours(), value.Minutes(), value.Seconds(),
                                  ticksLessThanSeconds);
      }
      return ticksLessThanSeconds <= 0
               ? fmt::format_to(ctx.out(), "{:02}:{:02}:{:02}", value.Hours(), value.Minutes(), value.Seconds())
               : fmt::format_to(ctx.out(), "{:02}:{:02}:{:02}.{:07}", value.Hours(), value.Minutes(), value.Seconds(), ticksLessThanSeconds);
    }
  };
}

#endif
