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

from . import *

## Pyramid object (OpenVX equivalent = \ref vx_pyramid)
#
# \par Example Usage: Create a Pyramid object having 10 levels
#
# \code
# from tiovx import *
#
# my_pyramid1 = Pyramid(10, PyramidScale.HALF, 640, 480, DfImage.U8, name="mypyramid")
# my_pyramid2 = Pyramid(10, PyramidScale.HALF, 640, 480, DfImage.U8)
# \endcode
#
# \ingroup DATA
class Pyramid (Reference) :
    ## Constructor used to create this object
    #
    # \see vxCreatePyramid for more details about the parameters
    #
    # \param levels [in] Number of pyramid levels
    # \param scale [in] tiovx::enums::PyramidScale
    # \param width [in] Width of highest pyramid level image
    # \param height [in] Height of highest pyramid level image
    # \param df_format [in] Image data format. tiovx::enums::DfImage
    # \param access_type [in] [optional] Memory access type
    # \param in_file_addr [in] [optional] In the case of memory accessed from file, the path to the input file
    # \param out_file_addr [in] [optional] In the case of memory outputted to file, the path to the output file
    # \param name [in] [optional] Name of the object
    def __init__(self, levels, scale, width, height, df_format, access_type="Host", in_file_addr="./", out_file_addr="./", name="default") :
        Reference.__init__(self, Type.PYRAMID, name)
        self.num_levels = levels;
        self.width = width;
        self.height = height;
        self.scale = scale;
        self.format = df_format;
        self.access_type = access_type;
        self.in_file = in_file_addr;
        self.out_file = out_file_addr;

    def __str__(self):
        return Reference.__str__(self) + ' [ ' + self.format.name + ':' + str(self.scale) + ', ' + str(self.width) + ', ' + str(self.height) + ' ]'
