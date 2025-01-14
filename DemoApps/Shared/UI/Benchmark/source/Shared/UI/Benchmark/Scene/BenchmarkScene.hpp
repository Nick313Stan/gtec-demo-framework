#ifndef SHARED_UI_BENCHMARK_SCENE_BENCHMARKSCENE_HPP
#define SHARED_UI_BENCHMARK_SCENE_BENCHMARKSCENE_HPP
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

#include <Shared/UI/Benchmark/Persistence/Bench/AppBenchmarkCpuRecord.hpp>
#include <Shared/UI/Benchmark/Persistence/Bench/AppBenchmarkGpuRecord.hpp>
#include <Shared/UI/Benchmark/Persistence/Bench/AppBenchmarkInfo.hpp>
#include <Shared/UI/Benchmark/Persistence/Bench/AppBenchmarkRenderInfo.hpp>
#include <memory>
#include <vector>
#include "BasicTestScene.hpp"

namespace Fsl
{
  struct AppBenchSettings;
  class BenchResultManager;
  class BenchSceneConfig;
  class IBasicGpuProfiler;
  class InputRecordingManager;

  class BenchmarkScene final : public BasicTestScene
  {
    enum class State
    {
      Playing,
      Closing
    };

    AppBenchmarkInfo m_info;
    AppBenchmarkRenderInfo m_renderInfo;
    std::shared_ptr<InputRecordingManager> m_inputRecordingManager;
    std::shared_ptr<BenchResultManager> m_benchResultManager;
    std::shared_ptr<IBasicGpuProfiler> m_gpuProfiler;

    State m_state{State::Playing};

    std::vector<AppBenchmarkCpuRecord> m_cpuEntries;
    std::vector<AppBenchmarkGpuRecord> m_gpuEntries;
    uint32_t m_entryCount{0};

  public:
    explicit BenchmarkScene(const SceneCreateInfo& createInfo, std::shared_ptr<InputRecordingManager> inputRecordingManager,
                            std::shared_ptr<BenchResultManager> benchResultManager, const BenchSceneConfig& sceneConfig);
    ~BenchmarkScene() override;

    void Update(const DemoTime& demoTime) final;
    bool Resolve(const DemoTime& demoTime) final;
    void Draw(const DemoTime& demoTime) final;
    void OnDrawSkipped(const FrameInfo& frameInfo) final;
    void OnFrameSequenceEnd() final;

  private:
    AppBenchmarkRenderInfo RestartTestApp(const AppBenchSettings& benchSettings);
    bool TryLoad();

    bool TrySaveResult();
  };
}

#endif
