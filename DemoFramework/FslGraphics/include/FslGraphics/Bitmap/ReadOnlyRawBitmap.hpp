#ifndef FSLGRAPHICS_BITMAP_READONLYRAWBITMAP_HPP
#define FSLGRAPHICS_BITMAP_READONLYRAWBITMAP_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslGraphics/Bitmap/BitmapOrigin.hpp>
#include <FslGraphics/Bitmap/ReadOnlyRawBitmapArea.hpp>
#include <FslGraphics/PixelFormat.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <cassert>
#include <stdexcept>

namespace Fsl
{
  class RawBitmapEx;
  struct RawBitmapArea;

  class ReadOnlyRawBitmapEmpty
  {
    inline static uint8_t g_empty = 0;

  public:
    static constexpr void* GetEmptyAddress() noexcept
    {
      return &g_empty;
    }
  };

  class ReadOnlyRawBitmap final
  {
    //! The pointer to the raw image data
    const void* m_pContent{ReadOnlyRawBitmapEmpty::GetEmptyAddress()};
    PxSize2D m_sizePx;
    uint32_t m_stride{0};
    PixelFormat m_pixelFormat{PixelFormat::Undefined};
    BitmapOrigin m_origin{BitmapOrigin::UpperLeft};

    constexpr ReadOnlyRawBitmap(ReadOnlySpan<uint8_t> span, const PxSize2D sizePx, const PixelFormat pixelFormat, const BitmapOrigin origin) noexcept
      : m_pContent(span.data() != nullptr ? span.data() : ReadOnlyRawBitmapEmpty::GetEmptyAddress())
      , m_sizePx(sizePx)
      , m_stride(PixelFormatUtil::CalcMinimumStride(sizePx.Width(), pixelFormat))
      , m_pixelFormat(pixelFormat)
      , m_origin(origin)
    {
      // We expect the span to contain the full bitmap
      assert(span.size() >= (sizePx.RawUnsignedHeight() * m_stride));
    }

    constexpr ReadOnlyRawBitmap(ReadOnlySpan<uint8_t> span, const PxSize2D sizePx, const PixelFormat pixelFormat, const uint32_t stride,
                                const BitmapOrigin origin) noexcept
      : m_pContent(span.data() != nullptr ? span.data() : ReadOnlyRawBitmapEmpty::GetEmptyAddress())
      , m_sizePx(sizePx)
      , m_stride(stride)
      , m_pixelFormat(pixelFormat)
      , m_origin(origin)
    {
      // We expect a valid stride
      assert(stride >= PixelFormatUtil::CalcMinimumStride(sizePx.Width(), pixelFormat));
      // We expect the span to contain the full bitmap
      assert(span.size() >= (sizePx.RawUnsignedHeight() * m_stride));
    }

    constexpr ReadOnlyRawBitmap(const void* const pContent, [[maybe_unused]] const uint32_t contentByteSize, const PxSize2D sizePx,
                                const PixelFormat pixelFormat, const BitmapOrigin origin) noexcept
      : m_pContent(pContent != nullptr ? pContent : ReadOnlyRawBitmapEmpty::GetEmptyAddress())
      , m_sizePx(sizePx)
      , m_stride(PixelFormatUtil::CalcMinimumStride(sizePx.Width(), pixelFormat))
      , m_pixelFormat(pixelFormat)
      , m_origin(origin)
    {
      // We expect a valid stride
      assert(m_pContent != nullptr);
      // We expect the span to contain the full bitmap
      assert(contentByteSize >= (sizePx.RawUnsignedHeight() * m_stride));
    }

    constexpr ReadOnlyRawBitmap(const void* const pContent, [[maybe_unused]] const uint32_t contentByteSize, const PxSize2D sizePx,
                                const PixelFormat pixelFormat, const uint32_t stride, const BitmapOrigin origin) noexcept
      : m_pContent(pContent != nullptr ? pContent : ReadOnlyRawBitmapEmpty::GetEmptyAddress())
      , m_sizePx(sizePx)
      , m_stride(stride)
      , m_pixelFormat(pixelFormat)
      , m_origin(origin)
    {
      // We expect a valid pointer
      assert(m_pContent != nullptr);
      // We expect a valid stride
      assert(stride >= PixelFormatUtil::CalcMinimumStride(sizePx.Width(), pixelFormat));
      // We expect the span to contain the full bitmap
      assert(contentByteSize >= (sizePx.RawUnsignedHeight() * m_stride));
    }

  public:
    constexpr ReadOnlyRawBitmap() noexcept = default;

    static constexpr ReadOnlyRawBitmap Create(ReadOnlySpan<uint8_t> span, const PxSize2D sizePx, const PixelFormat pixelFormat,
                                              const BitmapOrigin origin)
    {
      const auto stride = PixelFormatUtil::CalcMinimumStride(sizePx.Width(), pixelFormat);
      if ((sizePx.RawUnsignedHeight() * stride) > span.size())
      {
        throw std::invalid_argument("span is not large enough to contain a bitmap of the given dimensions");
      }
      return {span, sizePx, pixelFormat, stride, origin};
    }

    static constexpr ReadOnlyRawBitmap Create(ReadOnlySpan<uint8_t> span, const PxSize2D sizePx, const PixelFormat pixelFormat, const uint32_t stride,
                                              const BitmapOrigin origin)
    {
      if (stride < PixelFormatUtil::CalcMinimumStride(sizePx.Width(), pixelFormat))
      {
        throw std::invalid_argument("stride is smaller than the width allows");
      }
      if ((sizePx.RawUnsignedHeight() * stride) > span.size())
      {
        throw std::invalid_argument("span is not large enough to contain a bitmap of the given dimensions");
      }
      return {span, sizePx, pixelFormat, stride, origin};
    }


    static ReadOnlyRawBitmap Create(ReadOnlySpan<uint8_t> span, const PxExtent2D extentPx, const PixelFormat pixelFormat, const BitmapOrigin origin);
    static ReadOnlyRawBitmap Create(ReadOnlySpan<uint8_t> span, const PxExtent2D extentPx, const PixelFormat pixelFormat, const uint32_t stride,
                                    const BitmapOrigin origin);


    static ReadOnlyRawBitmap Create(const ReadOnlyRawBitmapArea& rawBitmapArea, const PixelFormat pixelFormat, const BitmapOrigin origin);


    static constexpr ReadOnlyRawBitmap UncheckedCreate(ReadOnlySpan<uint8_t> span, const PxSize2D sizePx, const PixelFormat pixelFormat,
                                                       const BitmapOrigin origin) noexcept
    {
      return {span, sizePx, pixelFormat, origin};
    }

    static constexpr ReadOnlyRawBitmap UncheckedCreate(ReadOnlySpan<uint8_t> span, const PxSize2D sizePx, const PixelFormat pixelFormat,
                                                       const uint32_t stride, const BitmapOrigin origin) noexcept
    {
      return {span, sizePx, pixelFormat, stride, origin};
    }

    static constexpr ReadOnlyRawBitmap UncheckedCreate(const void* const pContent, const uint32_t contentByteSize, const PxSize2D sizePx,
                                                       const PixelFormat pixelFormat, const BitmapOrigin origin) noexcept
    {
      return {pContent, contentByteSize, sizePx, pixelFormat, origin};
    }

    static constexpr ReadOnlyRawBitmap UncheckedCreate(const void* const pContent, const uint32_t contentByteSize, const PxSize2D sizePx,
                                                       const PixelFormat pixelFormat, const uint32_t stride, const BitmapOrigin origin) noexcept
    {
      return {pContent, contentByteSize, sizePx, pixelFormat, stride, origin};
    }

    static ReadOnlyRawBitmap UncheckedCreate(ReadOnlySpan<uint8_t> span, const PxExtent2D extentPx, const PixelFormat pixelFormat,
                                             const BitmapOrigin origin) noexcept;

    static ReadOnlyRawBitmap UncheckedCreate(ReadOnlySpan<uint8_t> span, const PxExtent2D extentPx, const PixelFormat pixelFormat,
                                             const uint32_t stride, const BitmapOrigin origin) noexcept;

    static ReadOnlyRawBitmap UncheckedCreate(const void* const pContent, const uint32_t contentByteSize, const PxExtent2D extentPx,
                                             const PixelFormat pixelFormat, const BitmapOrigin origin) noexcept;

    static ReadOnlyRawBitmap UncheckedCreate(const void* const pContent, const uint32_t contentByteSize, const PxExtent2D extentPx,
                                             const PixelFormat pixelFormat, const uint32_t stride, const BitmapOrigin origin) noexcept;


    constexpr bool IsValid() const noexcept
    {
      return (m_pContent != ReadOnlyRawBitmapEmpty::GetEmptyAddress() && m_stride > 0u) || (RawWidth() == 0 && RawHeight() == 0);
    }

    //! @brief Get a pointer to the content (this will never by a nullptr)
    constexpr const void* Content() const noexcept
    {
      return m_pContent;
    }

    //! The width of the bitmap in pixels
    constexpr PxSize2D::value_type Width() const noexcept
    {
      return m_sizePx.Width();
    }

    //! The height of the bitmap in pixels
    constexpr PxSize2D::value_type Height() const noexcept
    {
      return m_sizePx.Height();
    }

    //! The width of the bitmap in pixels
    constexpr PxSize2D::raw_value_type RawWidth() const noexcept
    {
      return m_sizePx.RawWidth();
    }

    //! The height of the bitmap in pixels
    constexpr PxSize2D::raw_value_type RawHeight() const noexcept
    {
      return m_sizePx.RawHeight();
    }

    //! The width of the bitmap in pixels
    constexpr PxSize2D::unsigned_value_type UnsignedWidth() const noexcept
    {
      return m_sizePx.UnsignedWidth();
    }

    //! The height of the bitmap in pixels
    constexpr PxSize2D::unsigned_value_type UnsignedHeight() const noexcept
    {
      return m_sizePx.UnsignedHeight();
    }

    //! The width of the bitmap in pixels
    constexpr PxSize2D::raw_unsigned_value_type RawUnsignedWidth() const noexcept
    {
      return m_sizePx.RawUnsignedWidth();
    }

    //! The height of the bitmap in pixels
    constexpr PxSize2D::raw_unsigned_value_type RawUnsignedHeight() const noexcept
    {
      return m_sizePx.RawUnsignedHeight();
    }

    //! @brief The number of bytes that represent one scan line of the bitmap.
    //! @warning This will be greater or equal to Width * BytesPerPixel as there might be padding bytes!
    constexpr uint32_t Stride() const noexcept
    {
      return m_stride;
    }

    //! The number of bytes that can be stored in m_pContent
    constexpr uint32_t GetByteSize() const noexcept
    {
      return RawUnsignedHeight() * m_stride;
    }

    constexpr PxExtent2D GetExtent() const noexcept
    {
      return {m_sizePx.UnsignedWidth(), m_sizePx.UnsignedHeight()};
    }

    constexpr PxSize2D GetSize() const noexcept
    {
      return m_sizePx;
    }

    //! Get the origin of the bitmap
    constexpr BitmapOrigin GetOrigin() const noexcept
    {
      return m_origin;
    }

    //! Get the pixel format of the raw bitmap
    constexpr PixelFormat GetPixelFormat() const noexcept
    {
      return m_pixelFormat;
    }

    constexpr PixelFormatLayout GetPixelFormatLayout() const noexcept
    {
      return PixelFormatUtil::GetPixelFormatLayout(m_pixelFormat);
    }

    //! true if the bitmap does not contain any 'alignment' bytes at the end of each line (basically stride = Width * PixelSizeInBytes)
    constexpr bool IsTightlyPacked() const noexcept
    {
      return m_stride == PixelFormatUtil::CalcMinimumStride(m_sizePx.Width(), PixelFormatUtil::GetBytesPerPixel(m_pixelFormat));
    }

    constexpr bool operator==(const ReadOnlyRawBitmap& rhs) const noexcept
    {
      return m_pContent == rhs.m_pContent && m_sizePx == rhs.m_sizePx && m_stride == rhs.m_stride && m_pixelFormat == rhs.m_pixelFormat &&
             m_origin == rhs.m_origin;
    }

    constexpr bool operator!=(const ReadOnlyRawBitmap& rhs) const noexcept
    {
      return !(*this == rhs);
    }
  };
}

#endif