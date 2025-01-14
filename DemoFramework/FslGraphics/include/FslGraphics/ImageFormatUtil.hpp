#ifndef FSLGRAPHICS_IMAGEFORMATUTIL_HPP
#define FSLGRAPHICS_IMAGEFORMATUTIL_HPP
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

#include <FslBase/IO/Path.hpp>
#include <FslGraphics/ImageFormat.hpp>

namespace Fsl::ImageFormatUtil
{
  //! @brief Try to guess the image format from the supplied extension
  //! @note accepts both ".png" and "png"
  ImageFormat TryDetectImageFormat(const std::string& extension);

  //! @brief Try to guess the image format from the supplied extension
  //! @note accepts both ".png" and "png"
  ImageFormat TryDetectImageFormat(const IO::Path& extension);

  //! @brief Given a path try to extract the extension and identify the image format
  ImageFormat TryDetectImageFormatFromExtension(const IO::Path& path);

  //! @brief Get the default extension for the image format
  const char* GetDefaultExtension(const ImageFormat imageFormat);

  //! @brief Check if the image format is generally considered lossless
  bool IsLossless(const ImageFormat imageFormat);
}

#endif
