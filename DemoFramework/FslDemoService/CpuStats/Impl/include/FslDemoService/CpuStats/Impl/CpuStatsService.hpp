#ifndef FSLDEMOSERVICE_CPUSTATS_IMPL_CPUSTATSSERVICE_HPP
#define FSLDEMOSERVICE_CPUSTATS_IMPL_CPUSTATSSERVICE_HPP
/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <FslDemoService/CpuStats/ICpuStatsService.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>
#include <FslService/Impl/ServiceType/Local/ThreadLocalService.hpp>
#include <functional>
#include <memory>

namespace Fsl
{
  class ICpuStatsAdapter;

  class CpuStatsService final
    : public ThreadLocalService
    , public ICpuStatsService
  {
    std::unique_ptr<ICpuStatsAdapter> m_adapter;
    uint32_t m_cpuCount{0};

  public:
    CpuStatsService(const ServiceProvider& serviceProvider, const std::function<std::unique_ptr<ICpuStatsAdapter>()>& fnAllocateAdapter);
    ~CpuStatsService() final;

    void Update() final;

    uint32_t GetCpuCount() const final;
    bool TryGetCpuUsage(float& rUsagePercentage, const uint32_t cpuIndex) const final;
    bool TryGetCpuUsage(CpuUsageRecord& rUsageRecord, const uint32_t cpuIndex) const final;
    bool TryGetApplicationCpuUsage(float& rUsagePercentage) const final;
    bool TryGetApplicationCpuUsage(CpuUsageRecord& rUsageRecord) const final;
    bool TryGetApplicationRamUsage(uint64_t& rRamUsage) const final;

  private:
  };
}

#endif
