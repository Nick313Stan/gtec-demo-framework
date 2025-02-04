#ifndef FSLSIMPLEUI_DECLARATIVE_THEMEPROPERTIES_TYPEDTHEMEPROPERTY_HPP
#define FSLSIMPLEUI_DECLARATIVE_THEMEPROPERTIES_TYPEDTHEMEPROPERTY_HPP
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

#include <FslSimpleUI/Declarative/ThemeProperties/ThemeProperty.hpp>

namespace Fsl::UI::Declarative
{
  template <typename T>
  class TypedThemeProperty : public ThemeProperty
  {
  public:
    using value_type = T;

    explicit TypedThemeProperty(PropertyName name)
      : ThemeProperty(std::move(name), typeid(value_type))
    {
    }

    TypedThemeProperty(PropertyName name, std::vector<ThemePropertyValueRecord> validValues)
      : ThemeProperty(std::move(name), typeid(value_type), std::move(validValues))
    {
    }

    value_type Parse(const StringViewLite value) const
    {
      if (!IsFreeform())
      {
        const uint32_t genericValueId = GenericParse(value);
        return GenericValueIdToActualTypedValue(genericValueId);
      }
      return ParseTypedValue(value);
    }

  protected:
    //! @brief Convert the record entry to the actual type
    virtual value_type GenericValueIdToActualTypedValue(const uint32_t genericValueId) const = 0;
    virtual value_type ParseTypedValue(const StringViewLite value) const = 0;
  };
}

#endif
