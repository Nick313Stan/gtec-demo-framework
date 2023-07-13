#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright 2019 NXP
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#    * Redistributions of source code must retain the above copyright notice,
#      this list of conditions and the following disclaimer.
#
#    * Redistributions in binary form must reproduce the above copyright notice,
#      this list of conditions and the following disclaimer in the documentation
#      and/or other materials provided with the distribution.
#
#    * Neither the name of the NXP. nor the names of
#      its contributors may be used to endorse or promote products derived from
#      this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#****************************************************************************************************************************************************

from typing import List
import xml.etree.ElementTree as ET
from FslBuildGen.Log import Log
from FslBuildGen.Xml.XmlBase import XmlBase
from FslBuildGen.Xml.Project.XmlCMakePlatform import XmlCMakePlatform

class XmlCMakeConfiguration(XmlBase):
    __AttribDefaultBuildDir = 'DefaultBuildDir'
    __AttribNinjaRecipe = 'NinjaRecipe'
    __AttribDefaultInstallPrefix = 'DefaultInstallPrefix'
    __AttribMinVersion = 'MinVersion'

    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        self._CheckAttributes({self.__AttribDefaultBuildDir, self.__AttribNinjaRecipe, self.__AttribDefaultInstallPrefix, self.__AttribMinVersion})
        self.DefaultBuildDir = self._ReadAttrib(xmlElement, self.__AttribDefaultBuildDir)
        self.NinjaRecipePackageName = self._ReadAttrib(xmlElement, self.__AttribNinjaRecipe)
        self.DefaultInstallPrefix = self._TryReadAttrib(xmlElement, self.__AttribDefaultInstallPrefix)
        self.MinVersion = self._TryReadAttrib(xmlElement, self.__AttribMinVersion)
        self.Platforms = self.__ReadPlatforms(log, xmlElement)

    def __ReadPlatforms(self, log: Log, xmlElement: ET.Element) -> List[XmlCMakePlatform]:
        res = []  # type: List[XmlCMakePlatform]
        foundElements = xmlElement.findall("Platform")
        for element in foundElements:
            res.append(XmlCMakePlatform(log, element))
        return res
