/*
 *
 * Copyright (c) 2017 Texas Instruments Incorporated
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

#include "TI/tivx.h"
#include "TI/tda4x.h"
#include "tivx_hwa_kernels.h"
#include "tivx_kernel_vpac_ldc.h"
#include "TI/tivx_target_kernel.h"

static vx_kernel vx_vpac_ldc_kernel = NULL;

static vx_status VX_CALLBACK tivxAddKernelVpacLdcValidate(vx_node node,
            const vx_reference parameters[ ],
            vx_uint32 num,
            vx_meta_format metas[]);
static vx_status VX_CALLBACK tivxAddKernelVpacLdcInitialize(vx_node node,
            const vx_reference parameters[ ],
            vx_uint32 num_params);

static vx_status VX_CALLBACK tivxAddKernelVpacLdcValidate(vx_node node,
            const vx_reference parameters[ ],
            vx_uint32 num,
            vx_meta_format metas[])
{
    vx_status status = VX_SUCCESS;
    vx_image img[7U] = {NULL};
    vx_scalar scalar[1U] = {NULL};
    vx_array array_0 = {NULL};
    vx_enum item_type_0;
    vx_size capacity_0;
    vx_array array_1 = {NULL};
    vx_enum item_type_1;
    vx_size capacity_1;
    vx_matrix matrix_2 = {NULL};
    vx_enum mat_type_2;
    vx_size mat_h_2, mat_w_2;
    vx_lut lut_3 = {NULL};
    vx_enum lut_type_3;
    vx_lut lut_4 = {NULL};
    vx_enum lut_type_4;
    vx_array array_5 = {NULL};
    vx_enum item_type_5;
    vx_size capacity_5;
    vx_uint32 uint32_6 = {NULL};
    vx_enum scalar_type_6;
    vx_df_image fmt[7U] = {NULL};
    /* < DEVELOPER_TODO: Change out_fmt to the correct output format > */
    vx_df_image out_fmt = VX_DF_IMAGE_U8;
    vx_uint32 i, w[7U], h[7U];
    
    status = tivxKernelValidateParametersNotNull(parameters, TIVX_KERNEL_VPAC_LDC_MAX_PARAMS);
    
    if (VX_SUCCESS == status)
    {
        array_0 = (vx_array)parameters[TIVX_KERNEL_VPAC_LDC_CONFIGURATION_IDX];
        array_1 = (vx_array)parameters[TIVX_KERNEL_VPAC_LDC_REGION_PARAMS_IDX];
        img[0U] = (vx_image)parameters[TIVX_KERNEL_VPAC_LDC_MESH_TABLE_IDX];
        matrix_2 = (vx_matrix)parameters[TIVX_KERNEL_VPAC_LDC_WARP_MATRIX_IDX];
        lut_3 = (vx_lut)parameters[TIVX_KERNEL_VPAC_LDC_OUT_2_LUMA_LUT_IDX];
        lut_4 = (vx_lut)parameters[TIVX_KERNEL_VPAC_LDC_OUT_3_CHROMA_LUT_IDX];
        array_5 = (vx_array)parameters[TIVX_KERNEL_VPAC_LDC_BANDWIDTH_PARAMS_IDX];
        img[1U] = (vx_image)parameters[TIVX_KERNEL_VPAC_LDC_IN_LUMA_OR_422_IDX];
        img[2U] = (vx_image)parameters[TIVX_KERNEL_VPAC_LDC_IN_CHROMA_IDX];
        img[3U] = (vx_image)parameters[TIVX_KERNEL_VPAC_LDC_OUT_0_LUMA_OR_422_IDX];
        img[4U] = (vx_image)parameters[TIVX_KERNEL_VPAC_LDC_OUT_1_CHROMA_IDX];
        img[5U] = (vx_image)parameters[TIVX_KERNEL_VPAC_LDC_OUT_2_LUMA_OR_422_IDX];
        img[6U] = (vx_image)parameters[TIVX_KERNEL_VPAC_LDC_OUT_3_CHROMA_IDX];
        scalar[0U] = (vx_scalar)parameters[TIVX_KERNEL_VPAC_LDC_ERROR_STATUS_IDX];
        
    }
    if (VX_SUCCESS == status)
    {
        status |= vxQueryArray(array_0, VX_ARRAY_ITEMTYPE, &item_type_0, sizeof(item_type_0));
        status |= vxQueryArray(array_0, VX_ARRAY_CAPACITY, &capacity_0, sizeof(capacity_0));
    }
    
    if (VX_SUCCESS == status)
    {
        status |= vxQueryArray(array_1, VX_ARRAY_ITEMTYPE, &item_type_1, sizeof(item_type_1));
        status |= vxQueryArray(array_1, VX_ARRAY_CAPACITY, &capacity_1, sizeof(capacity_1));
    }
    
    if (VX_SUCCESS == status)
    {
        /* Get the image width/height and format */
        status = vxQueryImage(img[0U], VX_IMAGE_FORMAT, &fmt[0U],
            sizeof(fmt[0U]));
        status |= vxQueryImage(img[0U], VX_IMAGE_WIDTH, &w[0U], sizeof(w[0U]));
        status |= vxQueryImage(img[0U], VX_IMAGE_HEIGHT, &h[0U], sizeof(h[0U]));
    }
    
    if (VX_SUCCESS == status)
    {
        status = vxQueryMatrix(matrix_2, VX_MATRIX_TYPE, &mat_type_2, sizeof(mat_type_2));
        status |= vxQueryMatrix(matrix_2, VX_MATRIX_COLUMNS, &mat_w_2, sizeof(mat_w_2));
        status |= vxQueryMatrix(matrix_2, VX_MATRIX_ROWS, &mat_h_2, sizeof(mat_h_2));
    }
    
    if (VX_SUCCESS == status)
    {
        status = vxQueryLUT(lut_3, VX_LUT_TYPE, &lut_type_3, sizeof(lut_type_3));
    }
    
    if (VX_SUCCESS == status)
    {
        status = vxQueryLUT(lut_4, VX_LUT_TYPE, &lut_type_4, sizeof(lut_type_4));
    }
    
    if (VX_SUCCESS == status)
    {
        status |= vxQueryArray(array_5, VX_ARRAY_ITEMTYPE, &item_type_5, sizeof(item_type_5));
        status |= vxQueryArray(array_5, VX_ARRAY_CAPACITY, &capacity_5, sizeof(capacity_5));
    }
    
    if (VX_SUCCESS == status)
    {
        /* Get the image width/height and format */
        status = vxQueryImage(img[1U], VX_IMAGE_FORMAT, &fmt[1U],
            sizeof(fmt[1U]));
        status |= vxQueryImage(img[1U], VX_IMAGE_WIDTH, &w[1U], sizeof(w[1U]));
        status |= vxQueryImage(img[1U], VX_IMAGE_HEIGHT, &h[1U], sizeof(h[1U]));
    }
    
    if (VX_SUCCESS == status)
    {
        /* Get the image width/height and format */
        status = vxQueryImage(img[2U], VX_IMAGE_FORMAT, &fmt[2U],
            sizeof(fmt[2U]));
        status |= vxQueryImage(img[2U], VX_IMAGE_WIDTH, &w[2U], sizeof(w[2U]));
        status |= vxQueryImage(img[2U], VX_IMAGE_HEIGHT, &h[2U], sizeof(h[2U]));
    }
    
    if (VX_SUCCESS == status)
    {
        /* Get the image width/height and format */
        status = vxQueryImage(img[3U], VX_IMAGE_FORMAT, &fmt[3U],
            sizeof(fmt[3U]));
        status |= vxQueryImage(img[3U], VX_IMAGE_WIDTH, &w[3U], sizeof(w[3U]));
        status |= vxQueryImage(img[3U], VX_IMAGE_HEIGHT, &h[3U], sizeof(h[3U]));
    }
    
    if (VX_SUCCESS == status)
    {
        /* Get the image width/height and format */
        status = vxQueryImage(img[4U], VX_IMAGE_FORMAT, &fmt[4U],
            sizeof(fmt[4U]));
        status |= vxQueryImage(img[4U], VX_IMAGE_WIDTH, &w[4U], sizeof(w[4U]));
        status |= vxQueryImage(img[4U], VX_IMAGE_HEIGHT, &h[4U], sizeof(h[4U]));
    }
    
    if (VX_SUCCESS == status)
    {
        /* Get the image width/height and format */
        status = vxQueryImage(img[5U], VX_IMAGE_FORMAT, &fmt[5U],
            sizeof(fmt[5U]));
        status |= vxQueryImage(img[5U], VX_IMAGE_WIDTH, &w[5U], sizeof(w[5U]));
        status |= vxQueryImage(img[5U], VX_IMAGE_HEIGHT, &h[5U], sizeof(h[5U]));
    }
    
    if (VX_SUCCESS == status)
    {
        /* Get the image width/height and format */
        status = vxQueryImage(img[6U], VX_IMAGE_FORMAT, &fmt[6U],
            sizeof(fmt[6U]));
        status |= vxQueryImage(img[6U], VX_IMAGE_WIDTH, &w[6U], sizeof(w[6U]));
        status |= vxQueryImage(img[6U], VX_IMAGE_HEIGHT, &h[6U], sizeof(h[6U]));
    }
    
    if (VX_SUCCESS == status)
    {
        status = vxQueryScalar(scalar[0U], VX_SCALAR_TYPE, &scalar_type_6, sizeof(scalar_type_6));
    }
    
    /* Check possible input image formats */
    #if 0
    if (VX_SUCCESS == status)
    {
        status = tivxKernelValidatePossibleFormat(fmt[0U], VX_DF_IMAGE_<possible_format>) &
                 tivxKernelValidatePossibleFormat(fmt[0U], VX_DF_IMAGE_<possible_format>);
    }
    #endif
    
    /* Check possible input image formats */
    #if 0
    if (VX_SUCCESS == status)
    {
        status = tivxKernelValidatePossibleFormat(fmt[1U], VX_DF_IMAGE_<possible_format>) &
                 tivxKernelValidatePossibleFormat(fmt[1U], VX_DF_IMAGE_<possible_format>);
    }
    #endif
    
    /* Check possible input image formats */
    #if 0
    if (VX_SUCCESS == status)
    {
        status = tivxKernelValidatePossibleFormat(fmt[2U], VX_DF_IMAGE_<possible_format>) &
                 tivxKernelValidatePossibleFormat(fmt[2U], VX_DF_IMAGE_<possible_format>);
    }
    #endif
    
    if (VX_SUCCESS == status)
    {
        status = tivxKernelValidateOutputSize(w[0U], w[3U], h[0U], h[3U], img[3U]);
    }
    
    if (VX_SUCCESS == status)
    {
        status = tivxKernelValidateOutputSize(w[0U], w[4U], h[0U], h[4U], img[4U]);
    }
    
    if (VX_SUCCESS == status)
    {
        status = tivxKernelValidateOutputSize(w[0U], w[5U], h[0U], h[5U], img[5U]);
    }
    
    if (VX_SUCCESS == status)
    {
        status = tivxKernelValidateOutputSize(w[0U], w[6U], h[0U], h[6U], img[6U]);
    }
    
    if (VX_SUCCESS == status)
    {
        status = tivxKernelValidateScalarType(scalar_type_6, VX_TYPE_UINT32);
    }
    
    if (VX_SUCCESS == status)
    {
        tivxKernelSetMetas(metas, TIVX_KERNEL_VPAC_LDC_MAX_PARAMS, out_fmt, w[0U], h[0U]);
    }
    
    return status;
}

static vx_status VX_CALLBACK tivxAddKernelVpacLdcInitialize(vx_node node,
            const vx_reference parameters[ ],
            vx_uint32 num_params)
{
    vx_status status = VX_SUCCESS;
    tivxKernelValidRectParams prms;
    
    if (num_params != TIVX_KERNEL_VPAC_LDC_MAX_PARAMS)
    {
        status = VX_ERROR_INVALID_PARAMETERS;
    }
    
    if (VX_SUCCESS == status)
    {
        status = tivxKernelValidateParametersNotNull(parameters, TIVX_KERNEL_VPAC_LDC_MAX_PARAMS);
    }
    
    if (VX_SUCCESS == status)
    {
        tivxKernelValidRectParams_init(&prms);
        
        prms.in_img[0U] = (vx_image)parameters[TIVX_KERNEL_VPAC_LDC_MESH_TABLE_IDX];
        prms.in_img[1U] = (vx_image)parameters[TIVX_KERNEL_VPAC_LDC_IN_LUMA_OR_422_IDX];
        prms.in_img[2U] = (vx_image)parameters[TIVX_KERNEL_VPAC_LDC_IN_CHROMA_IDX];
        prms.out_img[0U] = (vx_image)parameters[TIVX_KERNEL_VPAC_LDC_OUT_0_LUMA_OR_422_IDX];
        prms.out_img[1U] = (vx_image)parameters[TIVX_KERNEL_VPAC_LDC_OUT_1_CHROMA_IDX];
        prms.out_img[2U] = (vx_image)parameters[TIVX_KERNEL_VPAC_LDC_OUT_2_LUMA_OR_422_IDX];
        prms.out_img[3U] = (vx_image)parameters[TIVX_KERNEL_VPAC_LDC_OUT_3_CHROMA_IDX];

        prms.num_input_images = 3;
        prms.num_output_images = 4;
        
        /* < DEVELOPER_TODO: (Optional) Set padding values based on valid region if border mode is */
        /*                    set to VX_BORDER_UNDEFINED and remove the #if 0 and #endif lines. */
        /*                    Else, remove this entire #if 0 ... #endif block > */
        #if 0
        prms.top_pad = 0;
        prms.bot_pad = 0;
        prms.left_pad = 0;
        prms.right_pad = 0;
        prms.border_mode = VX_BORDER_UNDEFINED;
        #endif
        
        status = tivxKernelConfigValidRect(&prms);
    }
    
    return status;
}

vx_status tivxAddKernelVpacLdc(vx_context context)
{
    vx_kernel kernel;
    vx_status status;
    uint32_t index;
    
    kernel = vxAddUserKernel(
                context,
                "com.ti.hwa.vpac_ldc",
                TIVX_KERNEL_VPAC_LDC,
                NULL,
                TIVX_KERNEL_VPAC_LDC_MAX_PARAMS,
                tivxAddKernelVpacLdcValidate,
                tivxAddKernelVpacLdcInitialize,
                NULL);
    
    status = vxGetStatus((vx_reference)kernel);
    if (status == VX_SUCCESS)
    {
        index = 0;
        
        if (status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                        index,
                        VX_INPUT,
                        VX_TYPE_ARRAY,
                        VX_PARAMETER_STATE_REQUIRED
            );
            index++;
        }
        if (status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                        index,
                        VX_INPUT,
                        VX_TYPE_ARRAY,
                        VX_PARAMETER_STATE_REQUIRED
            );
            index++;
        }
        if (status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                        index,
                        VX_INPUT,
                        VX_TYPE_IMAGE,
                        VX_PARAMETER_STATE_OPTIONAL
            );
            index++;
        }
        if (status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                        index,
                        VX_INPUT,
                        VX_TYPE_MATRIX,
                        VX_PARAMETER_STATE_OPTIONAL
            );
            index++;
        }
        if (status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                        index,
                        VX_INPUT,
                        VX_TYPE_LUT,
                        VX_PARAMETER_STATE_OPTIONAL
            );
            index++;
        }
        if (status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                        index,
                        VX_INPUT,
                        VX_TYPE_LUT,
                        VX_PARAMETER_STATE_OPTIONAL
            );
            index++;
        }
        if (status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                        index,
                        VX_INPUT,
                        VX_TYPE_ARRAY,
                        VX_PARAMETER_STATE_OPTIONAL
            );
            index++;
        }
        if (status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                        index,
                        VX_INPUT,
                        VX_TYPE_IMAGE,
                        VX_PARAMETER_STATE_OPTIONAL
            );
            index++;
        }
        if (status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                        index,
                        VX_INPUT,
                        VX_TYPE_IMAGE,
                        VX_PARAMETER_STATE_OPTIONAL
            );
            index++;
        }
        if (status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                        index,
                        VX_OUTPUT,
                        VX_TYPE_IMAGE,
                        VX_PARAMETER_STATE_OPTIONAL
            );
            index++;
        }
        if (status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                        index,
                        VX_OUTPUT,
                        VX_TYPE_IMAGE,
                        VX_PARAMETER_STATE_OPTIONAL
            );
            index++;
        }
        if (status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                        index,
                        VX_OUTPUT,
                        VX_TYPE_IMAGE,
                        VX_PARAMETER_STATE_OPTIONAL
            );
            index++;
        }
        if (status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                        index,
                        VX_OUTPUT,
                        VX_TYPE_IMAGE,
                        VX_PARAMETER_STATE_OPTIONAL
            );
            index++;
        }
        if (status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                        index,
                        VX_OUTPUT,
                        VX_TYPE_UINT32,
                        VX_PARAMETER_STATE_OPTIONAL
            );
            index++;
        }
        if (status == VX_SUCCESS)
        {
            /* add supported target's */
            tivxAddKernelTarget(kernel, TIVX_TARGET_IPU1_0);
        }
        if (status == VX_SUCCESS)
        {
            status = vxFinalizeKernel(kernel);
        }
        if (status != VX_SUCCESS)
        {
            vxReleaseKernel(&kernel);
            kernel = NULL;
        }
    }
    else
    {
        kernel = NULL;
    }
    vx_vpac_ldc_kernel = kernel;
    
    return status;
}

vx_status tivxRemoveKernelVpacLdc(vx_context context)
{
    vx_status status;
    vx_kernel kernel = vx_vpac_ldc_kernel;
    
    status = vxRemoveKernel(kernel);
    vx_vpac_ldc_kernel = NULL;
    
    return status;
}


