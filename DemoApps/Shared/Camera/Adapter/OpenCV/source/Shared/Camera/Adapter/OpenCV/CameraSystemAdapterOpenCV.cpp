/****************************************************************************************************************************************************
 * Copyright 2018, 2022 NXP
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
// Here we create a camera, the CameraSystem uses this function to create the native camera

#include <Shared/Camera/Adapter/OpenCV/CameraAdapterOpenCV.hpp>
#include <Shared/Camera/Adapter/OpenCV/CameraSystemAdapterOpenCV.hpp>
#include <memory>

namespace Fsl::Helios
{
  CameraSystemAdapterOpenCV::CameraSystemAdapterOpenCV()
    : m_cameraType(CameraType::OpenCV)
  {
  }

  CameraSystemAdapterOpenCV::~CameraSystemAdapterOpenCV() = default;

  CameraType CameraSystemAdapterOpenCV::GetCameraType() const
  {
    return m_cameraType;
  }

  uint32_t CameraSystemAdapterOpenCV::GetCameraCount() const
  {
    // We only support one camera for now
    return 1u;
  }

  std::shared_ptr<ICameraAdapter> CameraSystemAdapterOpenCV::Allocate(const CameraAdapterAllocateInfo& allocateInfo)
  {
    return std::make_shared<CameraAdapterOpenCV>(allocateInfo);
  }
}
