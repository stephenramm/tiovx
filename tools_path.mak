# Copyright (C) 2011 Texas Insruments, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

VSDK_INSTALL_PATH := D:/ked/ADAS/code/

CGT_ROOT := $(VSDK_INSTALL_PATH)/ti_components/cg_tools/windows

CROSS_COMPILE := arm-none-eabi-

TIARMCGT_ROOT := $(CGT_ROOT)/ti-cgt-arm_5.2.5
GCC_ROOT := $(CGT_ROOT)/gcc-arm-none-eabi-4_7-2013q3
CGT6X_ROOT := $(CGT_ROOT)/c6000_7.4.2
ARP32CGT_ROOT := $(CGT_ROOT)/arp32_1.0.7

XDC_PATH := $(VSDK_INSTALL_PATH)/ti_components/os_tools/windows/xdctools_3_32_00_06_core
BIOS_PATH := $(VSDK_INSTALL_PATH)/ti_components/os_tools/bios_6_46_00_23
STW_PATH := $(VSDK_INSTALL_PATH)/ti_components/drivers/starterware/starterware_
BSP_PATH := $(VSDK_INSTALL_PATH)/ti_components/drivers/vayu_drivers/bspdrivers_
VSDK_PATH := $(VSDK_INSTALL_PATH)/vision_sdk
