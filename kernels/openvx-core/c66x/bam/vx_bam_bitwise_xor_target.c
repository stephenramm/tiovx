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



#include <TI/tivx.h>
#include <VX/vx.h>
#include <tivx_openvx_core_kernels.h>
#include <tivx_kernel_bitwise.h>
#include <TI/tivx_target_kernel.h>
#include <ti/vxlib/vxlib.h>
#include <tivx_kernel_utils.h>
#include <vx_bam_kernel_wrapper.h>

typedef struct
{
    tivx_bam_graph_handle graph_handle;
} tivxBitwiseXorParams;

static tivx_target_kernel vx_bitwise_xor_target_kernel = NULL;

static vx_status VX_CALLBACK tivxKernelBitwiseXorProcess(
    tivx_target_kernel_instance kernel, tivx_obj_desc_t *obj_desc[],
    uint16_t num_params, void *priv_arg);

static vx_status VX_CALLBACK tivxKernelBitwiseXorCreate(
    tivx_target_kernel_instance kernel, tivx_obj_desc_t *obj_desc[],
    uint16_t num_params, void *priv_arg);

static vx_status VX_CALLBACK tivxKernelBitwiseXorDelete(
    tivx_target_kernel_instance kernel, tivx_obj_desc_t *obj_desc[],
    uint16_t num_params, void *priv_arg);

static vx_status VX_CALLBACK tivxKernelBitwiseXorControl(
    tivx_target_kernel_instance kernel, tivx_obj_desc_t *obj_desc[],
    uint16_t num_params, void *priv_arg);

static vx_status VX_CALLBACK tivxKernelBitwiseXorProcess(
    tivx_target_kernel_instance kernel, tivx_obj_desc_t *obj_desc[],
    uint16_t num_params, void *priv_arg)
{
    vx_status status = VX_SUCCESS;
    tivxBitwiseXorParams *prms = NULL;
    tivx_obj_desc_image_t *src0, *src1, *dst;
    uint8_t *src0_addr, *src1_addr, *dst_addr;
    vx_rectangle_t rect;
    uint32_t size;

    status = ownCheckNullParams(obj_desc, num_params,
            TIVX_KERNEL_BITWISE_MAX_PARAMS);

    if (VX_SUCCESS == status)
    {
        src0 = (tivx_obj_desc_image_t *)obj_desc[TIVX_KERNEL_BITWISE_IN0_IMG_IDX];
        src1 = (tivx_obj_desc_image_t *)obj_desc[TIVX_KERNEL_BITWISE_IN1_IMG_IDX];
        dst = (tivx_obj_desc_image_t *)obj_desc[TIVX_KERNEL_BITWISE_OUT_IMG_IDX];

        status = tivxGetTargetKernelInstanceContext(kernel,
            (void **)&prms, &size);

        if ((VX_SUCCESS != status) || (NULL == prms) ||
            (sizeof(tivxBitwiseXorParams) != size))
        {
            status = VX_FAILURE;
        }
    }

    if (VX_SUCCESS == status)
    {
        void *img_ptrs[3];

        /* Get the correct offset of the images from the valid roi parameter */
        rect = src0->valid_roi;

        src0->mem_ptr[0].target_ptr = tivxMemShared2TargetPtr(
            src0->mem_ptr[0].shared_ptr, src0->mem_ptr[0].mem_type);
        tivxMemBufferMap(src0->mem_ptr[0].target_ptr, src0->mem_size[0],
            src0->mem_ptr[0].mem_type, VX_READ_ONLY);
        src0_addr = (uint8_t *)((uintptr_t)src0->mem_ptr[0U].target_ptr +
            ownComputePatchOffset(rect.start_x, rect.start_y,
            &src0->imagepatch_addr[0U]));

        src1->mem_ptr[0].target_ptr = tivxMemShared2TargetPtr(
            src1->mem_ptr[0].shared_ptr, src1->mem_ptr[0].mem_type);
        tivxMemBufferMap(src1->mem_ptr[0].target_ptr, src1->mem_size[0],
            src1->mem_ptr[0].mem_type, VX_READ_ONLY);
        src1_addr = (uint8_t *)((uintptr_t)src1->mem_ptr[0U].target_ptr +
            ownComputePatchOffset(rect.start_x, rect.start_y,
            &src1->imagepatch_addr[0U]));

        dst->mem_ptr[0].target_ptr = tivxMemShared2TargetPtr(
            dst->mem_ptr[0].shared_ptr, dst->mem_ptr[0].mem_type);
        tivxMemBufferMap(dst->mem_ptr[0].target_ptr, dst->mem_size[0],
            dst->mem_ptr[0].mem_type, VX_READ_ONLY);
        dst_addr = (uint8_t *)((uintptr_t)dst->mem_ptr[0U].target_ptr +
            ownComputePatchOffset(rect.start_x, rect.start_y,
            &dst->imagepatch_addr[0U]));

        img_ptrs[0] = src0_addr;
        img_ptrs[1] = src1_addr;
        img_ptrs[2] = dst_addr;
        tivxBamUpdatePointers(prms->graph_handle, 2U, 1U, img_ptrs);

        status  = tivxBamProcessGraph(prms->graph_handle);

        tivxMemBufferUnmap(dst->mem_ptr[0].target_ptr, dst->mem_size[0],
            dst->mem_ptr[0].mem_type, VX_WRITE_ONLY);
        tivxMemBufferUnmap(src0->mem_ptr[0].target_ptr, src0->mem_size[0],
            src0->mem_ptr[0].mem_type, VX_READ_ONLY);
        tivxMemBufferUnmap(src1->mem_ptr[0].target_ptr, src1->mem_size[0],
            src1->mem_ptr[0].mem_type, VX_READ_ONLY);
    }
    else
    {
        status = VX_ERROR_NO_MEMORY;
    }

    return (status);
}

static vx_status VX_CALLBACK tivxKernelBitwiseXorCreate(
    tivx_target_kernel_instance kernel, tivx_obj_desc_t *obj_desc[],
    uint16_t num_params, void *priv_arg)
{

    vx_status status = VX_SUCCESS;
    tivx_obj_desc_image_t *src0, *src1, *dst;
    tivxBitwiseXorParams *prms = NULL;

    /* Check number of buffers and NULL pointers */
    status = ownCheckNullParams(obj_desc, num_params,
            TIVX_KERNEL_BITWISE_MAX_PARAMS);

    if (VX_SUCCESS == status)
    {
        src0 = (tivx_obj_desc_image_t *)obj_desc[
            TIVX_KERNEL_BITWISE_IN0_IMG_IDX];
        src1 = (tivx_obj_desc_image_t *)obj_desc[
            TIVX_KERNEL_BITWISE_IN1_IMG_IDX];
        dst = (tivx_obj_desc_image_t *)obj_desc[
            TIVX_KERNEL_BITWISE_OUT_IMG_IDX];

        prms = tivxMemAlloc(sizeof(tivxBitwiseXorParams), TIVX_MEM_EXTERNAL);

        if (NULL != prms)
        {
            tivx_bam_kernel_details_t kernel_details;
            VXLIB_bufParams2D_t vxlib_src0, vxlib_src1, vxlib_dst;
            VXLIB_bufParams2D_t *buf_params[3];

            memset(prms, 0, sizeof(tivxBitwiseXorParams));

            vxlib_src0.dim_x = src0->imagepatch_addr[0U].dim_x;
            vxlib_src0.dim_y = src0->imagepatch_addr[0U].dim_y;
            vxlib_src0.stride_y = src0->imagepatch_addr[0U].stride_y;
            vxlib_src0.data_type = VXLIB_UINT8;

            vxlib_src1.dim_x = src1->imagepatch_addr[0U].dim_x;
            vxlib_src1.dim_y = src1->imagepatch_addr[0U].dim_y;
            vxlib_src1.stride_y = src1->imagepatch_addr[0U].stride_y;
            vxlib_src1.data_type = VXLIB_UINT8;

            vxlib_dst.dim_x = dst->imagepatch_addr[0U].dim_x;
            vxlib_dst.dim_y = dst->imagepatch_addr[0U].dim_y;
            vxlib_dst.stride_y = dst->imagepatch_addr[0U].stride_y;
            vxlib_dst.data_type = VXLIB_UINT8;

            /* Fill in the frame level sizes of buffers here. If the port
             * is optionally disabled, put NULL */
            buf_params[0] = &vxlib_src0;
            buf_params[1] = &vxlib_src1;
            buf_params[2] = &vxlib_dst;

            kernel_details.compute_kernel_params = NULL;

            BAM_VXLIB_xor_i8u_i8u_o8u_getKernelInfo(NULL,
                &kernel_details.kernel_info);

            status = tivxBamCreateHandleSingleNode(
                BAM_KERNELID_VXLIB_XOR_I8U_I8U_O8U, buf_params,
                &kernel_details, &prms->graph_handle);
        }
        else
        {
            status = VX_ERROR_NO_MEMORY;
        }

        if (VX_SUCCESS == status)
        {
            tivxSetTargetKernelInstanceContext(kernel, prms,
                sizeof(tivxBitwiseXorParams));
        }
        else
        {
            if (NULL != prms)
            {
                tivxMemFree(prms, sizeof(tivxBitwiseXorParams), TIVX_MEM_EXTERNAL);
            }
        }
    }

    return status;
}

static vx_status VX_CALLBACK tivxKernelBitwiseXorDelete(
    tivx_target_kernel_instance kernel, tivx_obj_desc_t *obj_desc[],
    uint16_t num_params, void *priv_arg)
{
    vx_status status = VX_SUCCESS;
    uint32_t size;
    tivxBitwiseXorParams *prms = NULL;

    /* Check number of buffers and NULL pointers */
    status = ownCheckNullParams(obj_desc, num_params,
            TIVX_KERNEL_BITWISE_MAX_PARAMS);

    if (VX_SUCCESS == status)
    {
        status = tivxGetTargetKernelInstanceContext(kernel,
            (void **)&prms, &size);

        if ((VX_SUCCESS == status) && (NULL != prms) &&
            (sizeof(tivxBitwiseXorParams) == size))
        {
            tivxBamDestroyHandle(prms->graph_handle);
            tivxMemFree(prms, sizeof(tivxBitwiseXorParams), TIVX_MEM_EXTERNAL);
        }
    }

    return (status);
}

static vx_status VX_CALLBACK tivxKernelBitwiseXorControl(
    tivx_target_kernel_instance kernel, tivx_obj_desc_t *obj_desc[],
    uint16_t num_params, void *priv_arg)
{
    return (VX_SUCCESS);
}

void tivxAddTargetKernelBamBitwiseXor(void)
{
    char target_name[TIVX_TARGET_MAX_NAME];
    vx_enum self_cpu;

    self_cpu = tivxGetSelfCpuId();

    if ((self_cpu == TIVX_CPU_ID_DSP1) || (self_cpu == TIVX_CPU_ID_DSP2))
    {
        if (self_cpu == TIVX_CPU_ID_DSP1)
        {
            strncpy(target_name, TIVX_TARGET_DSP1,
                TIVX_TARGET_MAX_NAME);
        }
        else
        {
            strncpy(target_name, TIVX_TARGET_DSP2,
                TIVX_TARGET_MAX_NAME);
        }

        vx_bitwise_xor_target_kernel = tivxAddTargetKernel(
            VX_KERNEL_XOR,
            target_name,
            tivxKernelBitwiseXorProcess,
            tivxKernelBitwiseXorCreate,
            tivxKernelBitwiseXorDelete,
            tivxKernelBitwiseXorControl,
            NULL);
    }
}


void tivxRemoveTargetKernelBamBitwiseXor(void)
{
    tivxRemoveTargetKernel(vx_bitwise_xor_target_kernel);
}
