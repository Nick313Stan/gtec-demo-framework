#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright (c) 2014 Freescale Semiconductor, Inc.
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
#    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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
from FslBuildGen.Xml.ToolConfig.XmlConfigPackageLocation import XmlConfigPackageLocation


class XmlConfigPackageConfiguration(XmlBase):
    __AttribName = 'Name'
    __AttribPreload = 'Preload'

    def __init__(self, log: Log, xmlElement: ET.Element, sourceFile: str) -> None:
        super().__init__(log, xmlElement)
        self._CheckAttributes({self.__AttribName, self.__AttribPreload})
        self.Name = self._ReadAttrib(xmlElement, self.__AttribName)
        self.Preload = self._ReadBoolAttrib(xmlElement, self.__AttribPreload, False)
        self.Locations = self.__LoadLocations(log, xmlElement)  # type: List[XmlConfigPackageLocation]
        self.SourceFile = sourceFile
        self.Id = self.Name.lower()


    def __LoadLocations(self, log: Log, xmlElement: ET.Element) -> List[XmlConfigPackageLocation]:
        res = []
        foundElements = xmlElement.findall("PackageLocation")
        for foundElement in foundElements:
            res.append(XmlConfigPackageLocation(log, foundElement))
        return res


    def Merge(self, other: 'XmlConfigPackageConfiguration') -> None:
        if self.Name != other.Name or self.Id != other.Id:
            raise Exception("Usage error, can only merge two configurations with the same id")
        for entry in other.Locations:
            self.Locations.append(entry)
