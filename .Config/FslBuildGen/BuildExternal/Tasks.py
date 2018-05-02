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

from typing import Callable
from typing import Dict
from typing import List
from typing import Optional
import os
import subprocess
import time
import urllib.request
import urllib.parse
import urllib.error
from FslBuildGen import IOUtil
from FslBuildGen import PackageConfig
from FslBuildGen.DataTypes import BuildPlatformType
from FslBuildGen.Build.BuildUtil import PlatformBuildUtil
from FslBuildGen.BuildExternal.FileUnpacker import FileUnpack
from FslBuildGen.BuildExternal import CMakeTypes
from FslBuildGen.Context.GeneratorContext import GeneratorContext
from FslBuildGen.DataTypes import BuildVariantConfig
from FslBuildGen.DataTypes import CMakeTargetType
from FslBuildGen.DataTypes import BuildThreads
#from FslBuildGen.PackageConfig import PlatformNameString
from FslBuildGen.PackageToolFinder import PackageToolFinder
from FslBuildGen.PlatformUtil import PlatformUtil


class BasicTask(object):
    def __init__(self, generatorContext: GeneratorContext) -> None:
        super().__init__()
        self.Context = generatorContext
        self.BasicConfig = generatorContext.BasicConfig
        #self._IsAndroid = generatorContext.PlatformName == PlatformNameString.ANDROID

    def LogPrint(self, message: str) -> None:
        self.BasicConfig.LogPrint(message)

    def DoPrint(self, message: str) -> None:
        self.BasicConfig.DoPrint(message)

    def CreateDirectory(self, path: str) -> None:
        if not IOUtil.IsDirectory(path):
            self.BasicConfig.LogPrint("Creating '{0}' as it was missing".format(path))
            IOUtil.SafeMakeDirs(path)


class DownloadTask(BasicTask):
    #def __init__(self, generatorContext: GeneratorContext) -> None:
    #    super().__init__(generatorContext)

    @staticmethod
    def __DownloadReport(fnLogPrint: Callable[[str], None], startTime: float, shortFileName: str,
                         count: int, blockSize: int, totalSize: int) -> None:
        if totalSize <= 0:
            return

        duration = time.time() - startTime
        progressSize = int(count * blockSize)
        speed = int(progressSize / (1024 * duration)) if duration > 0 else 0
        percent = min(int(count * blockSize * 100 / totalSize), 100)
        fnLogPrint("* {0}: {1:3d}% of {2} MB, {3} KB/s, {4:.2f} seconds passed.".format(shortFileName, percent, totalSize / (1024 * 1024), speed, duration))


    @staticmethod
    def __MakeDownReporter(fnLogPrint: Callable[[str], None], shortFileName: str) -> Callable[[int, int, int], None]:
        startTime = time.time()
        return lambda x, y, z: DownloadTask.__DownloadReport(fnLogPrint, startTime, shortFileName, x, y, z)


    def DownloadFromUrl(self, url: str, dstPath: str) -> None:
        if IOUtil.IsFile(dstPath):
            self.LogPrint("Downloaded archive found at '{0}', skipping download.".format(dstPath))
            return

        self.DoPrint("Downloading '{0}' to '{1}'".format(url, dstPath))
        reporter = DownloadTask.__MakeDownReporter(self.DoPrint, IOUtil.GetFileName(dstPath))
        urllib.request.urlretrieve(url, dstPath, reporthook=reporter)


class GitBaseTask(BasicTask):
    def __init__(self, generatorContext: GeneratorContext) -> None:
        super().__init__(generatorContext)
        self.__ConfigureForPlatform(generatorContext)


    def __ConfigureForPlatform(self, generatorContext: GeneratorContext) -> None:
        self.GitCommand = PlatformUtil.GetExecutableName('git', generatorContext.Platform.Name)


class GitCloneTask(GitBaseTask):
    def __init__(self, generatorContext: GeneratorContext) -> None:
        super().__init__(generatorContext)


    def RunGitClone(self, sourcePath: str, branch: str, targetPath: str) -> None:
        if IOUtil.IsDirectory(targetPath):
            self.LogPrint("Running git clone {0} {1}, skipped since it exist.".format(sourcePath, targetPath))
            return

        self.DoPrint("Running git clone {0} {1}".format(sourcePath, targetPath))
        try:
            self.__RunGitClone(sourcePath, targetPath, branch)
        except Exception:
            # A error occurred removing the targetPath
            self.LogPrint("* A error occurred removing '{0}' to be safe.".format(targetPath))
            IOUtil.SafeRemoveDirectoryTree(targetPath, True)
            raise


    def GetCurrentHash(self, path: str) -> str:
        command = [self.GitCommand, "rev-parse", "HEAD"]
        strVersion = None
        proc = subprocess.Popen(command, stderr=subprocess.STDOUT, stdout=subprocess.PIPE, universal_newlines=True, cwd=path)
        try:
            (strVersion, err) = proc.communicate()
            proc.wait()
            strVersion = strVersion.strip()
        finally:
            proc.stdout.close()
        return str(strVersion)


    def __RunGitClone(self, sourcePath: str, targetPath: str, branch: str) -> None:
        buildCommand = [self.GitCommand, 'clone', sourcePath, targetPath]
        # for faster checkout
        ##--single-branch --depth 1
        buildCommand += ['--single-branch']
        if branch != None and len(branch) > 0:
            buildCommand += ['-b', branch]
        result = subprocess.call(buildCommand)
        if result != 0:
            raise Exception("git clone failed {0}".format(buildCommand))


class GitApplyTask(GitBaseTask):
    def __init__(self, generatorContext: GeneratorContext) -> None:
        super().__init__(generatorContext)


    def RunGitApply(self, sourcePatchFile: str, targetPath: str) -> None:
        self.LogPrint("Running git apply {0} in {1}".format(sourcePatchFile, targetPath))
        buildCommand = [self.GitCommand, 'apply', sourcePatchFile, "--whitespace=fix"]
        if self.BasicConfig.Verbosity > 0:
            buildCommand.append("-v")
        result = subprocess.call(buildCommand, cwd=targetPath)
        if result != 0:
            raise Exception("git apply failed {0}".format(buildCommand))


class UnpackAndRenameTask(BasicTask):
    #def __init__(self, generatorContext: GeneratorContext) -> None:
    #    super().__init__(generatorContext)

    def RunUnpack(self, packedFilePath: str, dstPath: str) -> None:
        if not IOUtil.IsDirectory(dstPath):
            self.__RunUnpack(packedFilePath, dstPath)
        else:
            self.LogPrint("Unpacked directory found at '{0}', skipping unpack.".format(dstPath))

    def __RunUnpack(self, packedFilePath: str, dstPath: str) -> None:
        self.LogPrint("* Unpacking archive '{0}' to '{1}'".format(packedFilePath, dstPath))
        FileUnpack.UnpackFile(packedFilePath, dstPath)


class CMakeBuilder(object):
    def __init__(self, generatorContext: GeneratorContext, buildThreads: int) -> None:
        super().__init__()
        self.Context = generatorContext
        self.BasicConfig = generatorContext.BasicConfig
        # Builders like ninja and make only contains a single configuration
        self.IsSingleConfiguration = False
#        #self.__ConfigureForPlatform(generatorContext)
        self.BuilderThreadArguments = [] # type: List[str]
        PlatformBuildUtil.AddBuildThreads(generatorContext.Log, self.BuilderThreadArguments, generatorContext.PlatformName, buildThreads, True)


    def Execute(self, toolFinder: PackageToolFinder, path: str, target: int, cmakeProjectName: str, configuration: int, buildEnv: Dict[str,str]) -> None:
        pass

class CMakeBuilderDummy(CMakeBuilder):
    def __init__(self, generatorContext: GeneratorContext, buildThreads: int) -> None:
        super().__init__(generatorContext, buildThreads)
        self.IsSingleConfiguration = False
    def Execute(self, toolFinder: PackageToolFinder, path: str, target: int, cmakeProjectName: str, configuration: int, buildEnv: Dict[str,str]) -> None:
        raise Exception("This builder's Execute method is not supposed to be called")


class CMakeBuilderNinja(CMakeBuilder):
    def __init__(self, generatorContext: GeneratorContext, buildThreads: int) -> None:
        super().__init__(generatorContext, buildThreads)
        self.IsSingleConfiguration = True
        self.__CommandName = PlatformUtil.GetPlatformDependentExecuteableName("ninja", PlatformUtil.DetectBuildPlatformType())

    def Execute(self, toolFinder: PackageToolFinder, path: str, target: int, cmakeProjectName: str, configuration: int, buildEnv: Dict[str,str]) -> None:
        projectFile = "rules.ninja"

        toolPackage = toolFinder.GetToolPackageByToolName('ninja')
        commandName = IOUtil.Join(toolPackage.AbsoluteToolPath, self.__CommandName)

        self.BasicConfig.LogPrint("* Running ninja at '{0}' for project '{1}' and configuration '{2}'".format(path, projectFile, BuildVariantConfig.ToString(configuration)))
        buildCommand = [commandName]
        if target == CMakeTargetType.Install:
            buildCommand.append('install')

        buildCommand += self.BuilderThreadArguments

        try:
            result = subprocess.call(buildCommand, cwd=path, env=buildEnv)
            if result != 0:
                raise Exception("ninja failed with {0} command {1}".format(result, buildCommand))
        except Exception:
            self.BasicConfig.LogPrint("* ninja failed '{0}'".format(buildCommand))
            raise


class CMakeBuilderMake(CMakeBuilder):
    def __init__(self, generatorContext: GeneratorContext, buildThreads: int) -> None:
        super().__init__(generatorContext, buildThreads)
        # The cmake make files only support one configuration
        self.IsSingleConfiguration = True


    def Execute(self, toolFinder: PackageToolFinder, path: str, target: int, cmakeProjectName: str, configuration: int, buildEnv: Dict[str,str]) -> None:
        projectFile = "Makefile"

        self.BasicConfig.LogPrint("* Running make at '{0}' for project '{1}' and configuration '{2}'".format(path, projectFile, BuildVariantConfig.ToString(configuration)))
        buildCommand = ['make', '-f', projectFile]
        buildCommand += self.BuilderThreadArguments
        if target == CMakeTargetType.Install:
            buildCommand.append('install')
        try:
            result = subprocess.call(buildCommand, cwd=path, env=buildEnv)
            if result != 0:
                raise Exception("make failed {0}".format(buildCommand))
        except Exception:
            self.BasicConfig.LogPrint("* make failed '{0}'".format(buildCommand))
            raise


class CMakeBuilderMSBuild(CMakeBuilder):
    #def __init__(self, generatorContext: GeneratorContext, buildThreads: int) -> None:
    #    super().__init__(generatorContext, buildThreads)

    # msbuild INSTALL.vcxproj /p:Configuration=Debug
    # msbuild INSTALL.vcxproj /p:Configuration=Release
    def Execute(self, toolFinder: PackageToolFinder, path: str, target: int, cmakeProjectName: str, configuration: int, buildEnv: Dict[str,str]) -> None:
        projectFile = self.__GetMSBuildFilename(target, cmakeProjectName)
        configurationString = self.__GetMSBuildBuildVariantConfigString(configuration)
        self.BasicConfig.LogPrint("* Running msbuild at '{0}' for project '{1}' and configuration '{2}'".format(path, projectFile, configurationString))
        configurationString = "/p:Configuration={0}".format(configurationString)
        buildCommand = ['msbuild.exe', projectFile, configurationString]
        buildCommand += self.BuilderThreadArguments
        try:
            result = subprocess.call(buildCommand, cwd=path, env=buildEnv)
            if result != 0:
                raise Exception("msbuild failed {0}".format(buildCommand))
        except Exception:
            self.BasicConfig.LogPrint("* msbuild failed '{0}'".format(buildCommand))
            raise


    def __GetMSBuildFilename(self, target: int, cmakeProjectName: str) -> str:
        if target == CMakeTargetType.Install:
            return "Install.vcxproj"
        return "{0}.sln".format(cmakeProjectName)


    def __GetMSBuildBuildVariantConfigString(self, configuration: int) -> str:
        if configuration == BuildVariantConfig.Debug:
            return "Debug"
        elif configuration == BuildVariantConfig.Release:
            return "Release"
        else:
            raise Exception("Unsupported BuildVariantConfig: {0}".format(configuration))



class CMakeAndBuildTask(BasicTask):
    def __init__(self, generatorContext: GeneratorContext, buildThreads: int) -> None:
        super().__init__(generatorContext)
        self.__ConfigureForPlatform(generatorContext, buildThreads)

    # cmake -G "Visual Studio 14 2015 Win64"
    # -DCMAKE_INSTALL_PREFIX="e:\Work\Down\Windows\final\zlib-1.2.11"
    def RunCMakeAndBuild(self, toolFinder: PackageToolFinder, sourcePath: str, installPath: str, tempBuildPath: str, target: int,
                         cmakeProjectName: str, configurationList: List[int], cmakeOptionList: List[str],
                         allowSkip: bool) -> None:
        if allowSkip and IOUtil.IsDirectory(installPath):
            self.LogPrint("Running cmake and build on source '{0}' and installing to '{1}' was skipped since install directory exist.".format(sourcePath, installPath))
            return

        self.LogPrint("Running cmake and build on source '{0}' and installing to '{1}'".format(sourcePath, installPath))
        try:
            self.CreateDirectory(tempBuildPath)

            # Add platform specific commands
            if len(self.CMakeArguments) > 0:
                cmakeOptionList += self.CMakeArguments

            buildEnv = os.environ.copy()  # type: Dict[str, str]
            self.__ApplyPath(buildEnv, toolFinder.ToolPaths)

            self.__DoBuildNow(toolFinder, sourcePath, installPath, tempBuildPath, target, cmakeProjectName, configurationList, cmakeOptionList, buildEnv)
        except Exception:
            # A error occurred remove the install dir
            self.LogPrint("* A error occurred removing '{0}' to be safe.".format(installPath))
            IOUtil.SafeRemoveDirectoryTree(installPath, True)
            raise


    def __DoBuildNow(self, toolFinder: PackageToolFinder, sourcePath: str, installPath: str, tempBuildPath: str, target: int,
                      cmakeProjectName: str, configurationList: List[int], cmakeOptionList: List[str],
                      buildEnv: Dict[str, str]) -> None:

        if not self.Builder.IsSingleConfiguration:
            self.RunCMake(tempBuildPath, sourcePath, installPath, cmakeOptionList, buildEnv)

            for config in configurationList:
                self.Builder.Execute(toolFinder, tempBuildPath, target, cmakeProjectName, config, buildEnv)
        else:
            for config in configurationList:
                self.RunCMake(tempBuildPath, sourcePath, installPath, cmakeOptionList, buildEnv, config)
                self.Builder.Execute(toolFinder, tempBuildPath, target, cmakeProjectName, config, buildEnv)


    def RunCMake(self, path: str, sourcePath: str, cmakeInstallPrefix: str, cmakeOptionList: List[str],
                 buildEnv: Dict[str, str], buildVariantConfig: Optional[int] = None) -> None:
        defineCMakeInstallPrefix = "-DCMAKE_INSTALL_PREFIX={0}".format(cmakeInstallPrefix)

        defineBuildType = self.__TryGetBuildTypeString(buildVariantConfig)

        self.LogPrint("* Running cmake at '{0}' for source '{1}' with prefix {2} and options {3}".format(path, sourcePath, defineCMakeInstallPrefix, cmakeOptionList))
        buildCommand = [self.CMakeCommand, '-G', self.CMakeFinalGeneratorName, defineCMakeInstallPrefix, sourcePath]
        if not defineBuildType is None:
            buildCommand.append("-D{0}".format(defineBuildType))

        if len(cmakeOptionList) > 0:
            buildCommand += cmakeOptionList

        result = subprocess.call(buildCommand, cwd=path, env=buildEnv)
        if result != 0:
            raise Exception("CMake failed {0}".format(buildCommand))

    #def __AddToolDependencies()

    def __ApplyPath(self, env: Dict[str, str], paths: List[str]) -> None:
        if len(paths) <= 0:
            return
        res = ";{0}".format(";".join(paths))
        if 'PATH' in env:
            env['PATH'] += res
        else:
            env['PATH'] = res


    def __TryGetBuildTypeString(self, buildVariantConfig: Optional[int]) -> Optional[str]:
        if buildVariantConfig is None:
            return None
        buildType = CMakeTypes.CMakeBuildType.FromBuildVariantConfig(buildVariantConfig)
        return "CMAKE_BUILD_TYPE={0}".format(buildType)


    def __ConfigureForPlatform(self, generatorContext: GeneratorContext, buildThreads: int) -> None:
        self.CMakeCommand = CMakeTypes.DetermineCMakeCommand(generatorContext.Platform.Name)
        self.CMakeGeneratorName = CMakeTypes.DetermineCMakeGenerator(generatorContext.Platform)
        self.CompilerShortId = CMakeTypes.GetCompilerShortIdFromGeneratorName(self.CMakeGeneratorName)
        self.CMakeArguments = CMakeTypes.DeterminePlatformArguments(generatorContext.Platform.Name)
        self.Builder = self.__DetermineBuilder(self.CMakeGeneratorName, generatorContext, buildThreads)
        self.CMakeFinalGeneratorName = CMakeTypes.DetermineFinalCMakeGenerator(self.CMakeGeneratorName)


    def __DetermineBuilder(self, generatorName: str, generatorContext: GeneratorContext, buildThreads: int) -> CMakeBuilder:
        if generatorName == CMakeTypes.CMakeGeneratorName.UnixMakeFile:
            return CMakeBuilderMake(generatorContext, buildThreads)
        elif generatorName == CMakeTypes.CMakeGeneratorName.VisualStudio2015_X64 or generatorName == CMakeTypes.CMakeGeneratorName.VisualStudio2017_X64:
            return CMakeBuilderMSBuild(generatorContext, buildThreads)
        elif generatorName == CMakeTypes.CMakeGeneratorName.Android:
            if PlatformUtil.DetectBuildPlatformType() == BuildPlatformType.Windows:
                return CMakeBuilderNinja(generatorContext, buildThreads)
            else:
                return CMakeBuilderMake(generatorContext, buildThreads)
        raise Exception("No Builder defined for the cmake generator '{0}' on platform '{1}'".format(generatorName, generatorContext.PlatformName))
