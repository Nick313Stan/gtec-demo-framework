#ifdef __linux__
/****************************************************************************************************************************************************
 * Copyright 2019, 2024 NXP
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

#include <FslBase/IO/File.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslDemoService/CpuStats/Impl/Adapter/Linux/CpuStatsAdapterLinux.hpp>
#include <unistd.h>
#include <ctime>

namespace Fsl
{
  namespace
  {
    namespace LocalConfig
    {
      constexpr TimeSpan MinIntervalCoreCpuUsage(TimeSpan::FromSeconds(1));
      constexpr TimeSpan MinIntervalApplicationCpuUsage(TimeSpan::FromSeconds(1));
    }


    uint32_t DoGetCpuCount()
    {
      return static_cast<uint32_t>(sysconf(_SC_NPROCESSORS_ONLN));
    }

    bool TryClockGetTime(const clockid_t clockId, int64_t& rNanoSeconds)
    {
      timespec ts{};
      if (clock_gettime(clockId, &ts) != 0)
      {
        rNanoSeconds = {};
        return false;
      }

      rNanoSeconds = ts.tv_sec * 1000000000LL + ts.tv_nsec;
      return true;
    }

    float CalculateCpuUsage(const int64_t currentProcessCpuNanoseconds, const int64_t totalCpuNanoseconds)
    {
      // FSLLOG3_INFO("{} vs {}", currentProcessCpuNanoseconds, totalCpuNanoseconds);
      return totalCpuNanoseconds == 0
               ? 0.0f
               : static_cast<float>(static_cast<double>(currentProcessCpuNanoseconds) / static_cast<double>(totalCpuNanoseconds)) * 100.0f;
    }
  }

  CpuStatsAdapterLinux::CpuStatsAdapterLinux(const bool coreParserEnabled)
    : m_coreParserEnabled(coreParserEnabled)
  {
    m_cpuCount = std::min(DoGetCpuCount(), UncheckedNumericCast<uint32_t>(m_cpuStats.size()));
    m_cpuInfo.reserve(m_cpuCount);

    if (!TryGetProcessTimes(m_appProcessLast))
    {
      throw std::runtime_error("TryGetProcessTimes failed");
    }

    TryParseCpuLoadNow();
    const TickCount currentTime = m_timer.GetTimestamp();
    m_lastTryGetCpuUsage = currentTime - LocalConfig::MinIntervalCoreCpuUsage;
    m_lastTryGetApplicationCpuUsageTime = m_timer.GetTimestamp() - LocalConfig::MinIntervalApplicationCpuUsage;
  }


  uint32_t CpuStatsAdapterLinux::GetCpuCount() const
  {
    return m_cpuCount;
  }


  bool CpuStatsAdapterLinux::TryGetCpuUsage(CpuUsageRecord& rUsageRecord, const uint32_t cpuIndex) const
  {
    rUsageRecord = {};
    // We rely on the service to do the actual validation
    assert(m_cpuCount <= m_cpuStats.size());
    if (cpuIndex >= m_cpuCount)
    {
      return false;
    }

    // Ensure that we only cache the counters after the desired time has passed
    auto currentTime = m_timer.GetTimestamp();
    auto deltaTime = currentTime - m_lastTryGetCpuUsage;
    if (deltaTime >= LocalConfig::MinIntervalCoreCpuUsage)
    {
      m_lastTryGetCpuUsage = currentTime;
      if (!TryParseCpuLoadNow())
      {
        FSLLOG3_DEBUG_WARNING_IF(m_coreParserEnabled, "TryParseCpuLoadNow failed");
        return false;
      }
    }

    auto& rCPU = m_cpuStats[cpuIndex];
    if (rCPU.Current.User < rCPU.Last.User || rCPU.Current.Nice < rCPU.Last.Nice || rCPU.Current.System < rCPU.Last.System ||
        rCPU.Current.Idle < rCPU.Last.Idle)
    {
      rCPU.Current = rCPU.Last;
      return false;
    }

    auto totalAppTime = (rCPU.Current.User - rCPU.Last.User) + (rCPU.Current.Nice - rCPU.Last.Nice) + (rCPU.Current.System - rCPU.Last.System);
    auto totalUsage = totalAppTime + (rCPU.Current.Idle - rCPU.Last.Idle);
    if (totalUsage <= 0)
    {
      return false;
    }

    rUsageRecord = {m_lastTryGetCpuUsage, static_cast<float>(static_cast<double>(totalAppTime) / static_cast<double>(totalUsage)) * 100.0f};
    return true;
  }


  bool CpuStatsAdapterLinux::TryGetApplicationCpuUsage(CpuUsageRecord& rUsageRecord) const
  {
    rUsageRecord = {};

    auto currentTime = m_timer.GetTimestamp();
    auto callDeltaTime = currentTime - m_lastTryGetApplicationCpuUsageTime;
    if (callDeltaTime < LocalConfig::MinIntervalApplicationCpuUsage)
    {
      rUsageRecord = {m_appCpuUsagePercentageTime, m_appCpuUsagePercentage};
      return true;
    }
    m_lastTryGetApplicationCpuUsageTime = currentTime;

    ProcessTimes processTimes;
    if (!TryGetProcessTimes(processTimes))
    {
      FSLLOG3_DEBUG_WARNING("TryGetProcessTimes failed");
      return false;
    }

    m_appCpuUsagePercentage = CalculateCpuUsage(processTimes.CurrentProcessCpuNanoseconds - m_appProcessLast.CurrentProcessCpuNanoseconds,
                                                processTimes.TotalCpuNanoseconds - m_appProcessLast.TotalCpuNanoseconds) /
                              static_cast<float>(m_cpuCount);
    m_appProcessLast = processTimes;
    m_appCpuUsagePercentageTime = currentTime;

    rUsageRecord = {m_appCpuUsagePercentageTime, m_appCpuUsagePercentage};
    return true;
  }


  bool CpuStatsAdapterLinux::TryGetApplicationRamUsage(uint64_t& rRamUsage) const
  {
    rRamUsage = 0;
    if (m_ramParserEnabled)
    {
      try
      {
        m_fileReader.Open("/proc/self/status");
        ProcFileUtil::RAMInfo ramInfo;
        if (ProcFileUtil::TryParseRAMStats(ramInfo, m_fileReader, m_fileParser))
        {
          rRamUsage = ramInfo.VmRSS;
          return true;
        }
      }
      catch (const std::exception& ex)
      {
        m_ramParserEnabled = false;
        FSLLOG3_ERROR("Disabling ram parser as it failed with: {}", ex.what());
      }
      m_fileReader.Close();
    }
    return false;
  }


  bool CpuStatsAdapterLinux::TryParseCpuLoadNow() const
  {
    if (m_coreParserEnabled)
    {
      try
      {
        m_fileReader.Open("/proc/stat");
        if (ProcFileUtil::TryParseCPUStats(m_cpuInfo, m_fileReader, m_fileParser))
        {
          std::size_t validEntries = 0;
          for (const auto& entry : m_cpuInfo)
          {
            if (entry.Id < m_cpuCount)
            {
              auto cpuIndex = entry.Id;
              m_cpuStats[cpuIndex].Last = m_cpuStats[cpuIndex].Current;
              m_cpuStats[cpuIndex].Current = entry;
              ++validEntries;
            }
          }
          FSLLOG3_DEBUG_WARNING_IF(validEntries != m_cpuCount, "Could only find information for {} of {} cpus", validEntries, m_cpuCount);
          FSL_PARAM_NOT_USED(validEntries);
          return true;
        }
      }
      catch (const std::exception& ex)
      {
        m_coreParserEnabled = false;
        FSLLOG3_ERROR("Disabling CPU parser as it failed with: {}", ex.what());
      }
      m_fileReader.Close();
    }
    return false;
  }


  bool CpuStatsAdapterLinux::TryGetProcessTimes(ProcessTimes& rTimes) const
  {
    int64_t currentProcessCpuNanoseconds = 0;
    if (!TryClockGetTime(CLOCK_PROCESS_CPUTIME_ID, currentProcessCpuNanoseconds))
    {
      rTimes = {};
      return false;
    }
    int64_t totalCpuNanoseconds = 0;
    if (!TryClockGetTime(CLOCK_REALTIME, totalCpuNanoseconds))
    {
      rTimes = {};
      return false;
    }
    rTimes = ProcessTimes(currentProcessCpuNanoseconds, totalCpuNanoseconds);
    return true;
  }
}

#endif
