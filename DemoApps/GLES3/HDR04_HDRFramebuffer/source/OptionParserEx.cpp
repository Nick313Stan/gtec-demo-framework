/****************************************************************************************************************************************************
* Copyright 2018 NXP
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

#include "OptionParserEx.hpp"
#include <FslBase/BasicTypes.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/String/StringParseUtil.hpp>
#include <FslBase/Getopt/OptionBaseValues.hpp>
#include <FslBase/Exceptions.hpp>

namespace Fsl
{
  namespace
  {
    struct CommandId
    {
      enum Enum
      {
        DisablePattern = DEMO_APP_OPTION_BASE + 0x100,
        DisableDisplayHDRCheck = DEMO_APP_OPTION_BASE + 0x101,
      };

    };
  }

  OptionParserEx::OptionParserEx()
    : OptionParser()
    , m_disablePattern(false)
    , m_disableDisplayHDRCheck(false)
  {

  }


  void OptionParserEx::OnArgumentSetup(std::deque<Option>& rOptions)
  {
    OptionParser::OnArgumentSetup(rOptions);
    rOptions.push_back(Option("DisablePattern", OptionArgument::OptionNone, CommandId::DisablePattern, "Disable the debug pattern"));
    rOptions.push_back(Option("DisableDisplayHDRCheck", OptionArgument::OptionNone, CommandId::DisableDisplayHDRCheck, "Disables the check that verifies that the display is HDR compatible"));
  }


  OptionParseResult::Enum OptionParserEx::OnParse(const int32_t cmdId, const char*const pszOptArg)
  {
    switch (cmdId)
    {
    case CommandId::DisablePattern:
      m_disablePattern = true;
      return OptionParseResult::Parsed;
    case CommandId::DisableDisplayHDRCheck:
      m_disableDisplayHDRCheck = true;
      return OptionParseResult::Parsed;
    default:
      return OptionParser::OnParse(cmdId, pszOptArg);
    }
  }
}
