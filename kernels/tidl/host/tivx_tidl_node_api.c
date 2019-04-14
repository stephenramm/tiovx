/*
*
* Copyright (c) 2019 Texas Instruments Incorporated
*
* All rights reserved not granted herein.
*
* Limited License.
*
* Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
* license under copyrights and patents it now or hereafter owns or controls to make,
* have made, use, import, offer to sell and sell ("Utilize") this software subject to the
* terms herein.  With respect to the foregoing patent license, such license is granted
* solely to the extent that any such patent is necessary to Utilize the software alone.
* The patent license shall not apply to any combinations which include this software,
* other than combinations with devices manufactured by or for TI ("TI Devices").
* No hardware patent is licensed hereunder.
*
* Redistributions must preserve existing copyright notices and reproduce this license
* (including the above copyright notice and the disclaimer and (if applicable) source
* code license limitations below) in the documentation and/or other materials provided
* with the distribution
*
* Redistribution and use in binary form, without modification, are permitted provided
* that the following conditions are met:
*
* *       No reverse engineering, decompilation, or disassembly of this software is
* permitted with respect to any software provided in binary form.
*
* *       any redistribution and use are licensed by TI for use only with TI Devices.
*
* *       Nothing shall obligate TI to provide you with source code for the software
* licensed and provided to you in object code.
*
* If software source code is provided to you, modification and redistribution of the
* source code are permitted provided that the following conditions are met:
*
* *       any redistribution and use of the source code, including any resulting derivative
* works, are licensed by TI for use only with TI Devices.
*
* *       any redistribution and use of any object code compiled from the source code
* and any resulting derivative works, are licensed by TI for use only with TI Devices.
*
* Neither the name of Texas Instruments Incorporated nor the names of its suppliers
*
* may be used to endorse or promote products derived from this software without
* specific prior written permission.
*
* DISCLAIMER.
*
* THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/



#include <TI/tivx.h>
#include "tivx_tidl_kernels.h"
#include "../../../common/xdais_types.h" /* In TIDL_PATH directory */
#include "sTIDL_IOBufDesc.h"

VX_API_ENTRY vx_node VX_API_CALL tivxTIDLNode(vx_graph  graph,
                                              vx_kernel kernel,
                                              vx_user_data_object config,
                                              vx_user_data_object network,
                                              vx_uint32 max_num_input_tensors,
                                              vx_tensor input_tensors[],
                                              vx_array inDataQ,
                                              vx_uint32 max_num_output_tensors,
                                              vx_tensor output_tensors[],
                                              vx_array outDataQ)
{
    int32_t i;
    vx_reference *params;
    vx_uint32 num_input_tensors, num_output_tensors;
    vx_map_id map_id_config;
    sTIDL_IOBufDesc_t *ioBufDesc;
    vx_int32 num_params= TIVX_KERNEL_TIDL_NUM_BASE_PARAMETERS + max_num_input_tensors + max_num_output_tensors;
    vx_scalar max_num_input_tensors_scalar= vxCreateScalar(vxGetContext((vx_reference)graph), VX_TYPE_UINT32, &max_num_input_tensors);
    vx_scalar max_num_output_tensors_scalar= vxCreateScalar(vxGetContext((vx_reference)graph), VX_TYPE_UINT32, &max_num_output_tensors);

    vxMapUserDataObject(config, 0, sizeof(sTIDL_IOBufDesc_t), &map_id_config,
        (void **)&ioBufDesc, VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);

    num_input_tensors= ioBufDesc->numInputBuf;
    num_output_tensors= ioBufDesc->numOutputBuf;

    vxUnmapUserDataObject(config, map_id_config);

    params= tivxMemAlloc(sizeof(vx_reference)*num_params, TIVX_MEM_EXTERNAL);

    params[0]=  (vx_reference)config;
    params[1]=  (vx_reference)network;
    params[2]=  (vx_reference)inDataQ; /* Input dataQ */
    params[3]=  (vx_reference)outDataQ; /* Output dataQ */
    params[4]=  (vx_reference)max_num_input_tensors_scalar;
    params[5]=  (vx_reference)max_num_output_tensors_scalar;

    for (i= 0; i < num_input_tensors; i++) {
      params[TIVX_KERNEL_TIDL_IN_FIRST_TENSOR + i]=  (vx_reference)input_tensors[i];
    }

    /* Fill the entries of the param table that don't map to any real input tensors to any arbitrary value
     * but that is not NULL. It is to avoid any error returned by tivxTIDLNode() */
    for (i= num_input_tensors; i < max_num_input_tensors; i++) {
      params[TIVX_KERNEL_TIDL_IN_FIRST_TENSOR + i]=  params[TIVX_KERNEL_TIDL_IN_FIRST_TENSOR];
    }

    for (i= 0; i < num_output_tensors; i++) {
      params[TIVX_KERNEL_TIDL_IN_FIRST_TENSOR + max_num_input_tensors + i]=  (vx_reference)output_tensors[i];
    }

    /* Fill the entries of the param table that don't map to any real output tensors to any arbitrary value
     * but that is not NULL. It is to avoid any error returned by tivxTIDLNode() */
    for (i= num_output_tensors; i < max_num_output_tensors; i++) {
      params[TIVX_KERNEL_TIDL_IN_FIRST_TENSOR + max_num_input_tensors + i]=  params[TIVX_KERNEL_TIDL_IN_FIRST_TENSOR + max_num_input_tensors];
    }

    vx_node node = tivxCreateNodeByKernelRef(graph,
                                             kernel,
                                             params,
                                             num_params);

    tivxMemFree(params, sizeof(vx_reference)*num_params, TIVX_MEM_EXTERNAL);
    vxReleaseScalar(&max_num_input_tensors_scalar);
    vxReleaseScalar(&max_num_output_tensors_scalar);

    return node;
}
