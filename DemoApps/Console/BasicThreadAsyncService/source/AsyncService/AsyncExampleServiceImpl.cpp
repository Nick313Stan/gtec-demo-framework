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

#include "AsyncExampleServiceImpl.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/System/Threading/Thread.hpp>
#include <FslService/Impl/ServiceType/Async/AsynchronousServiceImplCreateInfo.hpp>
#include <fmt/std.h>    // to allow us to print a std::thread::id
#include <functional>
#include <thread>
#include <utility>

namespace Fsl
{
  AsyncExampleServiceImpl::AsyncExampleServiceImpl(const AsynchronousServiceImplCreateInfo& createInfo, const ServiceProvider& serviceProvider)
    : AsynchronousServiceImpl(createInfo, serviceProvider)
  {
    createInfo.MessageHandlerRegistry.Register<AsyncExampleServiceMessages::RunAsyncPromiseMessage>([this](auto& message) { RunAsync(message); });

    FSLLOG3_INFO("AsyncExampleService: created (ThreadId: {})", std::this_thread::get_id());
  }


  AsyncExampleServiceImpl::~AsyncExampleServiceImpl()
  {
    FSLLOG3_INFO("AsyncExampleService: destroyed (ThreadId: {})", std::this_thread::get_id());
  }


  void AsyncExampleServiceImpl::RunAsync(AsyncExampleServiceMessages::RunAsyncPromiseMessage& message) const
  {
    try
    {
      using namespace std::chrono_literals;

      const auto threadId = std::this_thread::get_id();

      int32_t counter = 42;
      while (!message.CancellationToken->IsCanceled())
      {
        FSLLOG3_INFO("AsyncExampleService: Doing work (ThreadId: {})", threadId);
        message.ThreadSafeQueue->Enqueue(counter);
        ++counter;
        std::this_thread::sleep_for(250ms);
      }
      // Post a quit message
      message.ThreadSafeQueue->Enqueue(1337);

      // Simulate a bit of additional delay reacting to the cancel
      std::this_thread::sleep_for(100ms);
      FSLLOG3_INFO("AsyncExampleService: Completed (ThreadId: {})", threadId);

      message.Promise.set_value(true);
    }
    catch (const std::exception&)
    {
      // Forward the exception to the promise
      message.Promise.set_exception(std::current_exception());
    }
  }
}
