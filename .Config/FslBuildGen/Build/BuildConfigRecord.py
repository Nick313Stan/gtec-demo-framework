#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#****************************************************************************************************************************************************
# Copyright 2017 NXP
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

#from typing import Dict
from typing import List
from typing import Optional
#from typing import Union
from FslBuildGen.Build.BuildVariantConfigUtil import BuildVariantConfigUtil
from FslBuildGen.Build.ForAllConfig import ForAllConfig
from FslBuildGen.BuildConfig.UserSetVariables import UserSetVariables
from FslBuildGen.Build.DataTypes import CommandType
from FslBuildGen.ExternalVariantConstraints import ExternalVariantConstraints
#from FslBuildGen.DataTypes import BuildVariantConfig
from FslBuildGen.Generator.GeneratorPluginBase2 import GeneratorPluginBase2
from FslBuildGen.Version import Version
#from FslBuildGen.PackageFilters import PackageFilters
#from FslBuildGen.QualifiedRequirementExtensionName import QualifiedRequirementExtensionName
#from FslBuildGen.SharedGeneration import ToolAddedVariant


class BuildConfigRecord(object):
    def __init__(self,
                 toolVersion: Version,
                 platformName: str,
                 externalVariantConstraints: ExternalVariantConstraints,
                 userSetVariables: UserSetVariables,
                 buildCommand: CommandType,
                 buildCommandArgs: List[str],
                 buildArgs: List[str],
                 forAllConfig: Optional[ForAllConfig],
                 generator: Optional[GeneratorPluginBase2],
                 buildThreads: int) -> None:
        super().__init__()
        self.ToolVersion = toolVersion
        self.PlatformName = platformName
        self.VariantConstraints = externalVariantConstraints
        self.UserSetVariables = userSetVariables
        self.BuildCommand = buildCommand
        self.BuildCommandArgs = buildCommandArgs
        self.BuildArgs = buildArgs
        self.ForAllConfig = forAllConfig
        self.Generator = generator
        self.BuildThreads = buildThreads
        self.ActiveBuildVariantConfig = BuildVariantConfigUtil.GetBuildVariantConfig(externalVariantConstraints)
