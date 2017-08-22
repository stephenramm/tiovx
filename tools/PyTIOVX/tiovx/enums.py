#
# Copyright (c) 2017 Texas Instruments Incorporated
#
# All rights reserved not granted herein.
#
# Limited License.
#
# Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
# license under copyrights and patents it now or hereafter owns or controls to make,
# have made, use, import, offer to sell and sell ("Utilize") this software subject to the
# terms herein.  With respect to the foregoing patent license, such license is granted
# solely to the extent that any such patent is necessary to Utilize the software alone.
# The patent license shall not apply to any combinations which include this software,
# other than combinations with devices manufactured by or for TI ("TI Devices").
# No hardware patent is licensed hereunder.
#
# Redistributions must preserve existing copyright notices and reproduce this license
# (including the above copyright notice and the disclaimer and (if applicable) source
# code license limitations below) in the documentation and/or other materials provided
# with the distribution
#
# Redistribution and use in binary form, without modification, are permitted provided
# that the following conditions are met:
#
#       No reverse engineering, decompilation, or disassembly of this software is
# permitted with respect to any software provided in binary form.
#
#       any redistribution and use are licensed by TI for use only with TI Devices.
#
#       Nothing shall obligate TI to provide you with source code for the software
# licensed and provided to you in object code.
#
# If software source code is provided to you, modification and redistribution of the
# source code are permitted provided that the following conditions are met:
#
#       any redistribution and use of the source code, including any resulting derivative
# works, are licensed by TI for use only with TI Devices.
#
#       any redistribution and use of any object code compiled from the source code
# and any resulting derivative works, are licensed by TI for use only with TI Devices.
#
# Neither the name of Texas Instruments Incorporated nor the names of its suppliers
#
# may be used to endorse or promote products derived from this software without
# specific prior written permission.
#
# DISCLAIMER.
#
# THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
# OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.
#
#

from enum import Enum
from . import *


## Object/Data type (OpenVX equivalent = vx_type_e)
#
# \par Example usage:
# \code
# Type.UINT8
# Type.UINT16
# \endcode
# \ingroup ENUMS
#
class Type(Enum):
    ## OpenVX equivalent = VX_TYPE_INVALID
    INVALID = 1
    ## OpenVX equivalent = VX_TYPE_CHAR
    CHAR    = 2
    ## OpenVX equivalent = VX_TYPE_INT8
    INT8    = 3
    ## OpenVX equivalent = VX_TYPE_UINT8
    UINT8   = 4
    ## OpenVX equivalent = VX_TYPE_INT16
    INT16   = 5
    ## OpenVX equivalent = VX_TYPE_UINT16
    UINT16  = 6
    ## OpenVX equivalent = VX_TYPE_INT32
    INT32   = 7
    ## OpenVX equivalent = VX_TYPE_UINT32
    UINT32  = 8
    ## OpenVX equivalent = VX_TYPE_INT64
    INT64   = 9
    ## OpenVX equivalent = VX_TYPE_UINT64
    UINT64  = 10
    ## OpenVX equivalent = VX_TYPE_FLOAT32
    FLOAT32 = 11
    ## OpenVX equivalent = VX_TYPE_FLOAT64
    FLOAT64 = 12
    ## OpenVX equivalent = VX_TYPE_ENUM
    ENUM    = 13
    ## OpenVX equivalent = VX_TYPE_SIZE
    SIZE    = 14
    ## OpenVX equivalent = VX_TYPE_DF_IMAGE
    DF_IMAGE= 15
    ## OpenVX equivalent = VX_TYPE_BOOL
    BOOL    = 16
    ## OpenVX equivalent = VX_TYPE_REFERENCE
    REFERENCE    = 17
    ## OpenVX equivalent = VX_TYPE_CONTEXT
    CONTEXT      = 18
    ## OpenVX equivalent = VX_TYPE_GRAPH
    GRAPH        = 19
    ## OpenVX equivalent = VX_TYPE_NODE
    NODE         = 20
    ## OpenVX equivalent = VX_TYPE_KERNEL
    KERNEL       = 21
    ## OpenVX equivalent = VX_TYPE_PARAMETER
    PARAMETER    = 22
    ## OpenVX equivalent = VX_TYPE_DELAY
    DELAY        = 23
    ## OpenVX equivalent = VX_TYPE_LUT
    LUT          = 24
    ## OpenVX equivalent = VX_TYPE_DISTRIBUTION
    DISTRIBUTION = 25
    ## OpenVX equivalent = VX_TYPE_PYRAMID
    PYRAMID      = 26
    ## OpenVX equivalent = VX_TYPE_THRESHOLD
    THRESHOLD    = 27
    ## OpenVX equivalent = VX_TYPE_MATRIX
    MATRIX       = 28
    ## OpenVX equivalent = VX_TYPE_CONVOLUTION
    CONVOLUTION  = 29
    ## OpenVX equivalent = VX_TYPE_SCALAR
    SCALAR       = 30
    ## OpenVX equivalent = VX_TYPE_ARRAY
    ARRAY        = 31
    ## OpenVX equivalent = VX_TYPE_IMAGE
    IMAGE        = 32
    ## OpenVX equivalent = VX_TYPE_REMAP
    REMAP        = 33
    ## OpenVX equivalent = VX_TYPE_ERROR
    ERROR        = 34
    ## OpenVX equivalent = VX_TYPE_META_FORMAT
    META_FORMAT  = 35
    ## OpenVX equivalent = VX_TYPE_OBJECT_ARRAY
    OBJECT_ARRAY = 36

    def __lt__(self, other):
        if self.__class__ is other.__class__:
            return self.value < other.value
        return NotImplemented

    def __gt__(self, other):
        if self.__class__ is other.__class__:
            return self.value > other.value
        return NotImplemented

    def __eq__(self, other):
        if self.__class__ is other.__class__:
            return self.value == other.value
        return NotImplemented

    def get_vx_enum_name(type) :
        return "VX_TYPE_" + type.name

    def get_vx_name(type) :
        return "vx_" + type.name.lower()

    def is_scalar_type(type) :
        if type > Type.INVALID and type < Type.REFERENCE :
            return True
        if type == Type.SCALAR :
            return True
        return False

    def get_obj_desc_name(type):
        if Type.is_scalar_type(type) :
            type = Type.SCALAR
        return "tivx_obj_desc_" + type.name.lower() +  "_t"

    def get_scalar_obj_desc_data_name(type) :
        if Type.is_scalar_type(type) :
            if type is Type.CHAR :
                return "chr"
            if type is Type.INT8 :
                return "s8"
            if type is Type.UINT8 :
                return "u8"
            if type is Type.INT16:
                return "s16"
            if type is Type.UINT16:
                return "u16"
            if type is Type.INT32:
                return "s32"
            if type is Type.UINT32:
                return "u32"
            if type is Type.INT64:
                return "s64"
            if type is Type.UINT64:
                return "u64"
            if type is Type.FLOAT32:
                return "f32"
            if type is Type.FLOAT64:
                return "f64"
            if type is Type.ENUM :
                return "enm"
            if type is Type.SIZE :
                return "size"
            if type is Type.DF_IMAGE:
                return "fcc"
            if type is Type.BOOL :
                return "boolean"
            return "invalid"
        return "invalid"

## Image data format (OpenVX equivalent = vx_df_image)
#
# \par Example usage:
# \code
# DfImage.VIRT
# DfImage.U8
# \endcode
# \ingroup ENUMS
#
class DfImage(Enum) :
    ## Place holder for invalid data format
    INVALID = 0
    ## OpenVX equivalent = VX_DF_IMAGE_VIRT
    VIRT = 1
    ## OpenVX equivalent = VX_DF_IMAGE_RGB
    RGB  = 2
    ## OpenVX equivalent = VX_DF_IMAGE_RGBX
    RGBX = 3
    ## OpenVX equivalent = VX_DF_IMAGE_NV12
    NV12 = 4
    ## OpenVX equivalent = VX_DF_IMAGE_NV21
    NV21 = 5
    ## OpenVX equivalent = VX_DF_IMAGE_UYVY
    UYVY = 6
    ## OpenVX equivalent = VX_DF_IMAGE_YUYV
    YUYV = 7
    ## OpenVX equivalent = VX_DF_IMAGE_IYUV
    IYUV = 8
    ## OpenVX equivalent = VX_DF_IMAGE_YUV4
    YUV4 = 9
    ## OpenVX equivalent = VX_DF_IMAGE_U8
    U8   = 10
    ## OpenVX equivalent = VX_DF_IMAGE_U16
    U16  = 11
    ## OpenVX equivalent = VX_DF_IMAGE_S16
    S16  = 12
    ## OpenVX equivalent = VX_DF_IMAGE_U32
    U32  = 13
    ## OpenVX equivalent = VX_DF_IMAGE_S32
    S32  = 14

    def get_vx_enum_name(df_format) :
        return "VX_DF_IMAGE_" + df_format.name

    def get_vx_name(df_format) :
        return "vx_df_image_e"

## Image channel (OpenVX equivalent = vx_channel_e)
#
# \par Example usage:
# \code
# Channel.R
# Channel.C0
# \endcode
# \ingroup ENUMS
#
class Channel(Enum) :
    ## OpenVX equivalent = VX_CHANNEL_0
    C0 = 1
    ## OpenVX equivalent = VX_CHANNEL_1
    C1 = 2
    ## OpenVX equivalent = VX_CHANNEL_2
    C2 = 3
    ## OpenVX equivalent = VX_CHANNEL_3
    C3 = 4
    ## OpenVX equivalent = VX_CHANNEL_R
    R  = 5
    ## OpenVX equivalent = VX_CHANNEL_G
    G  = 6
    ## OpenVX equivalent = VX_CHANNEL_B
    B  = 7
    ## OpenVX equivalent = VX_CHANNEL_A
    A  = 8
    ## OpenVX equivalent = VX_CHANNEL_Y
    Y  = 9
    ## OpenVX equivalent = VX_CHANNEL_U
    U  = 10
    ## OpenVX equivalent = VX_CHANNEL_V
    V  = 11

    def get_vx_enum_name(type) :
        if type == Channel.C0 :
            return "VX_CHANNEL_0"
        if type == Channel.C1 :
            return "VX_CHANNEL_1"
        if type == Channel.C2 :
            return "VX_CHANNEL_2"
        if type == Channel.C3 :
            return "VX_CHANNEL_3"
        return "VX_CHANNEL_" + type.name

    def get_vx_name(type) :
        return "vx_channel_e"

class Cpu(Enum) :
    INVALID = 1
    DSP1    = 2
    DSP2    = 3
    EVE1    = 4
    EVE2    = 5
    EVE3    = 6
    EVE4    = 7
    A15_0   = 8
    IPU1_0  = 9
    IPU1_1  = 10
    IPU2    = 11

    def get_vx_enum_name(type) :
        return "TIVX_CPU_ID_" + type.name


## Target on which to execute a node (TIOVX equivalent = TIVX_TARGET_xxxx)
#
# Used as parameter when creating a node
#
# \par Example usage:
# \code
# Target.DSP1
# Target.DSP2
# \endcode
# \ingroup ENUMS
#
class Target(Enum) :
    ## Placeholder for invalid target ID
    INVALID = 1
    ## TIOVX equivalent = TIVX_TARGET_DSP1
    DSP1    = 2
    ## TIOVX equivalent = TIVX_TARGET_DSP2
    DSP2    = 3
    ## TIOVX equivalent = TIVX_TARGET_EVE1
    EVE1    = 4
    ## TIOVX equivalent = TIVX_TARGET_EVE2
    EVE2    = 5
    ## TIOVX equivalent = TIVX_TARGET_EVE3
    EVE3    = 6
    ## TIOVX equivalent = TIVX_TARGET_EVE4
    EVE4    = 7
    ## TIOVX equivalent = TIVX_TARGET_A15_0
    A15_0   = 8
    ## TIOVX equivalent = TIVX_TARGET_IPU1_0
    IPU1_0  = 9
    ## TIOVX equivalent = TIVX_TARGET_IPU1_1
    IPU1_1  = 10
    ## TIOVX equivalent = TIVX_TARGET_IPU2
    IPU2    = 11
    ## Used internally by the tool
    DEFAULT = DSP1

    def get_vx_enum_name(type) :
        return "TIVX_TARGET_" + type.name

    def get_target_folder_name(type) :
        if target == Target.DSP1 :
            return c66
        if target == Target.DSP2 :
            return c66
        if target == Target.EVE1 :
            return eve
        if target == Target.EVE2 :
            return eve
        if target == Target.EVE3 :
            return eve
        if target == Target.EVE4 :
            return eve
        if target == Target.A15_0 :
            return a15
        if target == Target.IPU1_0 :
            return ipu
        if target == Target.IPU1_1 :
            return ipu
        if target == Target.IPU2 :
            return ipu

        return None

    def get_cpu(target) :
        if target == Target.DSP1 :
            return Cpu.DSP1
        if target == Target.DSP2 :
            return Cpu.DSP2
        if target == Target.EVE1 :
            return Cpu.EVE1
        if target == Target.EVE2 :
            return Cpu.EVE2
        if target == Target.EVE3 :
            return Cpu.EVE3
        if target == Target.EVE4 :
            return Cpu.EVE4
        if target == Target.A15_0 :
            return Cpu.A15_0
        if target == Target.IPU1_0 :
            return Cpu.IPU1_0
        if target == Target.IPU1_1 :
            return Cpu.IPU1_1
        if target == Target.IPU2 :
            return Cpu.IPU2

        return Cpu.INVALID

## Conversion Policy (OpenVX equivalent = vx_convert_policy_e)
#
# \par Example usage:
# \code
# Policy.WRAP
# Policy.SATURATE
# \endcode
# \ingroup ENUMS
#
class Policy(Enum) :
    ## OpenVX equivalent = VX_CONVERT_POLICY_WRAP
    WRAP      = 1
    ## OpenVX equivalent = VX_CONVERT_POLICY_SATURATE
    SATURATE  = 2

    def get_vx_enum_name(type) :
        return "VX_CONVERT_POLICY_" + type.name

    def get_vx_name(type) :
        return "vx_convert_policy_e"

## Non linear filter type (OpenVX equivalent = vx_non_linear_filter_e)
#
# \par Example usage:
# \code
# NonLinearFilter.MEDIAN
# NonLinearFilter.MIN
# \endcode
# \ingroup ENUMS
#
class NonLinearFilter(Enum) :
    ## OpenVX equivalent = VX_NON_LINEAR_FILTER_MEDIAN
    MEDIAN   = 1
    ## OpenVX equivalent = VX_NON_LINEAR_FILTER_MIN
    MIN      = 2
    ## OpenVX equivalent = VX_NON_LINEAR_FILTER_MAX
    MAX      = 3

    def get_vx_enum_name(type) :
        return "VX_NONLINEAR_FILTER_" + type.name

    def get_vx_name(type) :
        return "vx_non_linear_filter_e"

## Matrix Patterns (OpenVX equivalent = vx_pattern_e)
#
# \par Example usage:
# \code
# Pattern.BOX
# Pattern.DISK
# \endcode
# \ingroup ENUMS
#
class Pattern(Enum) :
    ## OpenVX equivalent = VX_PATTERN_BOX
    BOX      = 1
    ## OpenVX equivalent = VX_PATTERN_CROSS
    CROSS    = 2
    ## OpenVX equivalent = VX_PATTERN_DISK
    DISK     = 3
    ## OpenVX equivalent = VX_PATTERN_OTHER
    OTHER    = 4

    def get_vx_enum_name(type) :
        return "VX_PATTERN_" + type.name

    def get_vx_name(type) :
        return "vx_pattern_e"

## Interpollation type (OpenVX equivalent = vx_interpolation_type_e)
#
# \par Example usage:
# \code
# InterpolationType.NEAREST_NEIGHBOR
# InterpolationType.BILINEAR
# \endcode
# \ingroup ENUMS
#
class InterpolationType(Enum) :
    ## OpenVX equivalent = VX_INTERPOLLATION_TYPE_NEAREST_NEIGHBOR
    NEAREST_NEIGHBOR    = 1
    ## OpenVX equivalent = VX_INTERPOLLATION_TYPE_BILINEAR
    BILINEAR            = 2
    ## OpenVX equivalent = VX_INTERPOLLATION_TYPE_AREA
    AREA                = 3

    def get_vx_enum_name(type) :
        return "VX_INTERPOLATION_" + type.name

    def get_vx_name(type) :
        return "vx_interpolation_type_e"

## Boolean type (OpenVX equivalent = vx_bool)
#
# \par Example usage:
# \code
# Bool.FALSE
# Bool.TRUE
# \endcode
# \ingroup ENUMS
#
class Bool(Enum) :
    ## OpenVX equivalent = vx_false_e
    FALSE           = 0
    ## OpenVX equivalent = vx_true_e
    TRUE            = 1

    def get_vx_enum_name(type) :
        return "vx_" + type.name.lower() + "_e"

    def get_vx_name(type) :
        return "vx_bool"

## Normalization type (OpenVX equivalent = vx_norm_type_e)
#
# \par Example usage:
# \code
# Norm.L1
# Norm.L2
# \endcode
# \ingroup ENUMS
#
class Norm(Enum):
    ## OpenVX equivalent = VX_NORM_L1
    L1 = 1
    ## OpenVX equivalent = VX_NORM_L2
    L2 = 2

    def get_vx_enum_name(type) :
        return "VX_NORM_" + type.name

    def get_vx_name(type) :
        return "vx_norm_e"

## Parameter direction (OpenVX equivalent = vx_direction_e)
#
# \par Example usage:
# \code
# Direction.INPUT
# Direction.OUTPUT
# \endcode
# \ingroup ENUMS
#
class Direction(Enum):
    ## OpenVX equivalent = VX_INPUT
    INPUT = 1
    ## OpenVX equivalent = VX_OUTPUT
    OUTPUT = 2
    ## OpenVX equivalent = VX_BIDRECTIONAL
    BIDIRECTIONAL = 3

    def get_vx_enum_name(type) :
        return "VX_" + type.name

    def get_vx_name(type) :
        return "vx_direction_e"

    def get_access_type(type) :
        if type == Direction.INPUT:
            return "VX_READ_ONLY"
        if type == Direction.OUTPUT:
            return "VX_WRITE_ONLY"
        if type == Direction.BIDIRECTIONAL:
            return "VX_READ_WRITE"
        return "INVALID"

    def get_doxygen_name(type) :
        if type == Direction.INPUT:
            return "in"
        if type == Direction.OUTPUT:
            return "out"
        if type == Direction.BIDIRECTIONAL:
            return "in,out"
        return "INVALID"


## Parameter state (OpenVX equivalent = vx_parameter_state_e)
#
# \par Example usage:
# \code
# ParamState.REQUIRED
# ParamState.OPTIONAL
# \endcode
# \ingroup ENUMS
#
class ParamState(Enum):
    ## OpenVX equivalent = VX_PARAMETER_STATE_REQUIRED
    REQUIRED = 1
    ## OpenVX equivalent = VX_PARAMETER_STATE_OPTIONAL
    OPTIONAL = 2

    def get_vx_enum_name(type) :
        return "VX_PARAMETER_STATE_" + type.name

    def get_vx_name(type) :
        return "vx_parameter_state_e"

## Pyramid Scale (OpenVX equivalent = VX_SCALE_PYRAMID_xxx)
#
# \par Example usage:
# \code
# PyramidScale.HALF
# PyramidScale.ORB
# \endcode
# \ingroup ENUMS
#
class PyramidScale(Enum):
    ## OpenVX equivalent = VX_SCALE_PYRAMID_HALF
    HALF = 1
    ## OpenVX equivalent = VX_SCALE_PYRAMID_ORB
    ORB = 2

    def get_vx_enum_name(type) :
        return "VX_SCALE_PYRAMID_" + type.name

## Threshold Type (OpenVX equivalent = vx_threshold_type_e)
#
# \par Example usage:
# \code
# ThresholdType.BINARY
# ThresholdType.RANGE
# \endcode
# \ingroup ENUMS
#
class ThresholdType(Enum):
    ## OpenVX equivalent = VX_THRESHOLD_TYPE_BINARY
    BINARY = 1
    ## OpenVX equivalent = VX_THRESHOLD_TYPE_RANGE
    RANGE = 2

    def get_vx_enum_name(type) :
        return "VX_THRESHOLD_TYPE_" + type.name

    def get_vx_name(type) :
        return "vx_threshold_type_e"
