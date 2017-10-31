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

class CodeGenerate :
    def __init__(self, filename, header=True) :
        self.indent_level = 0
        self.indent = "    "
        self.filename = filename
        self.header = header
        self.open()

    def open(self) :
        self.file = open(self.filename,'w')
        if self.header :
            self.write_header()

    def close(self, new_line=True) :
        if new_line :
            self.write_newline()
        self.file.close()

    def write_header(self) :
        self.write_line('/*')
        self.write_line(' *')
        self.write_line(' * Copyright (c) 2017 Texas Instruments Incorporated')
        self.write_line(' *')
        self.write_line(' * All rights reserved not granted herein.')
        self.write_line(' *')
        self.write_line(' * Limited License.')
        self.write_line(' *')
        self.write_line(' * Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive')
        self.write_line(' * license under copyrights and patents it now or hereafter owns or controls to make,')
        self.write_line(' * have made, use, import, offer to sell and sell ("Utilize") this software subject to the')
        self.write_line(' * terms herein.  With respect to the foregoing patent license, such license is granted')
        self.write_line(' * solely to the extent that any such patent is necessary to Utilize the software alone.')
        self.write_line(' * The patent license shall not apply to any combinations which include this software,')
        self.write_line(' * other than combinations with devices manufactured by or for TI ("TI Devices").')
        self.write_line(' * No hardware patent is licensed hereunder.')
        self.write_line(' *')
        self.write_line(' * Redistributions must preserve existing copyright notices and reproduce this license')
        self.write_line(' * (including the above copyright notice and the disclaimer and (if applicable) source')
        self.write_line(' * code license limitations below) in the documentation and/or other materials provided')
        self.write_line(' * with the distribution')
        self.write_line(' *')
        self.write_line(' * Redistribution and use in binary form, without modification, are permitted provided')
        self.write_line(' * that the following conditions are met:')
        self.write_line(' *')
        self.write_line(' * *       No reverse engineering, decompilation, or disassembly of this software is')
        self.write_line(' * permitted with respect to any software provided in binary form.')
        self.write_line(' *')
        self.write_line(' * *       any redistribution and use are licensed by TI for use only with TI Devices.')
        self.write_line(' *')
        self.write_line(' * *       Nothing shall obligate TI to provide you with source code for the software')
        self.write_line(' * licensed and provided to you in object code.')
        self.write_line(' *')
        self.write_line(' * If software source code is provided to you, modification and redistribution of the')
        self.write_line(' * source code are permitted provided that the following conditions are met:')
        self.write_line(' *')
        self.write_line(' * *       any redistribution and use of the source code, including any resulting derivative')
        self.write_line(' * works, are licensed by TI for use only with TI Devices.')
        self.write_line(' *')
        self.write_line(' * *       any redistribution and use of any object code compiled from the source code')
        self.write_line(' * and any resulting derivative works, are licensed by TI for use only with TI Devices.')
        self.write_line(' *')
        self.write_line(' * Neither the name of Texas Instruments Incorporated nor the names of its suppliers')
        self.write_line(' *')
        self.write_line(' * may be used to endorse or promote products derived from this software without')
        self.write_line(' * specific prior written permission.')
        self.write_line(' *')
        self.write_line(' * DISCLAIMER.')
        self.write_line(' *')
        self.write_line(' * THIS SOFTWARE IS PROVIDED BY TI AND TI\'S LICENSORS "AS IS" AND ANY EXPRESS')
        self.write_line(' * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES')
        self.write_line(' * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.')
        self.write_line(' * IN NO EVENT SHALL TI AND TI\'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,')
        self.write_line(' * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,')
        self.write_line(' * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,')
        self.write_line(' * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY')
        self.write_line(' * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE')
        self.write_line(' * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED')
        self.write_line(' * OF THE POSSIBILITY OF SUCH DAMAGE.')
        self.write_line(' *')
        self.write_line(' */')
        self.write_newline()

    def write_comment_line(self, text_line) :
        self.write_line('/* %s */' % text_line)

    def write_line(self, text_line, new_line=True, indent=True) :
        if indent :
            for i in range(0, self.indent_level) :
                self.file.write(self.indent)
        self.file.write(text_line)
        if new_line:
            self.file.write('\n')

    def write_block(self, text_block, new_line=True) :
        self.file.write(text_block)
        if new_line:
            self.file.write('\n')

    def write_open_brace(self) :
        self.write_line('{')
        self.indent_level = self.indent_level+1

    def write_close_brace(self, text="") :
        self.indent_level = self.indent_level-1
        self.write_line('}%s' % text)

    def write_include(self, include_file_name) :
        self.write_line('#include "%s"' % include_file_name)

    def write_ifndef_define(self, text) :
        self.write_line('#ifndef %s' % text)
        self.write_line('#define %s' % text)
        self.write_newline()

    def write_endif(self, text) :
        self.write_line('#endif /* %s */' % text)
        self.write_newline()

    def write_extern_c_top(self) :
        self.write_line("#ifdef __cplusplus")
        self.write_line("extern \"C\" {")
        self.write_line("#endif")
        self.write_newline()

    def write_extern_c_bottom(self) :
        self.write_line("#ifdef __cplusplus")
        self.write_line("}")
        self.write_line("#endif")
        self.write_newline()

    def write_newline(self) :
        self.file.write('\n')

    def write_define_status(self) :
        self.write_line("vx_status status = VX_SUCCESS;")

    def write_if_status(self) :
        self.write_line("if (status == VX_SUCCESS)");


