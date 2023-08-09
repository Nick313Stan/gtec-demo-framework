#!/bin/bash
set -o errexit

# Prepare the sdk
source /opt/fsl-imx-internal-xwayland/5.15-langdale/environment-setup-armv8a-poky-linux

source $WORKSPACE/.Config/Jenkins/yocto/PrepareJenkinsEnvironment.sh

FslBuild.py --noGitHash --debug -t sdk -vv --BuildTime --UseFeatures $FSL_CI_FEATURES --Variants [WindowSystem=Wayland] -c install --CMakeInstallPrefix bin