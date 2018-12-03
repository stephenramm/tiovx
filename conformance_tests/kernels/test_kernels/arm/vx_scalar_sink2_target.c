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
#include "TI/tivx_capture.h"
#include "VX/vx.h"
#include "tivx_capture_kernels.h"
#include "tivx_kernel_scalar_sink.h"
#include "TI/tivx_target_kernel.h"
#include <TI/tivx_task.h>

#define MAX_OLD_OBJ_DESC    (1u)

typedef struct
{
    tivx_obj_desc_t *old_obj_desc[MAX_OLD_OBJ_DESC];
    uint8_t local_val;
    uint8_t do_error_print;
} tivxScalarSink2Params;

static tivx_target_kernel vx_scalar_sink_target_kernel = NULL;

static vx_status VX_CALLBACK tivxScalarSink2Process(
       tivx_target_kernel_instance kernel,
       tivx_obj_desc_t *obj_desc[],
       uint16_t num_params, void *priv_arg);
static vx_status VX_CALLBACK tivxScalarSink2Create(
       tivx_target_kernel_instance kernel,
       tivx_obj_desc_t *obj_desc[],
       uint16_t num_params, void *priv_arg);
static vx_status VX_CALLBACK tivxScalarSink2Delete(
       tivx_target_kernel_instance kernel,
       tivx_obj_desc_t *obj_desc[],
       uint16_t num_params, void *priv_arg);
static vx_status VX_CALLBACK tivxScalarSink2Control(
       tivx_target_kernel_instance kernel,
       tivx_obj_desc_t *obj_desc[],
       uint16_t num_params, void *priv_arg);

static vx_status VX_CALLBACK tivxScalarSink2Process(
       tivx_target_kernel_instance kernel,
       tivx_obj_desc_t *obj_desc[],
       uint16_t num_params, void *priv_arg)
{
    vx_status status = VX_SUCCESS;
    tivx_obj_desc_scalar_t *in_desc;
    tivxScalarSink2Params *prms = NULL;
    uint32_t size;

    if ( (num_params != TIVX_KERNEL_SCALAR_SINK_MAX_PARAMS)
        || (NULL == obj_desc[TIVX_KERNEL_SCALAR_SINK_IN_IDX])
    )
    {
        status = VX_FAILURE;
    }

    if(VX_SUCCESS == status)
    {
        in_desc = (tivx_obj_desc_scalar_t *)obj_desc[TIVX_KERNEL_SCALAR_SINK_IN_IDX];

    }

    if(VX_SUCCESS == status)
    {
        vx_uint8 in_value;

        tivxTaskWaitMsecs(1);

        in_value = in_desc->data.u08;

        status = tivxGetTargetKernelInstanceContext(kernel,
            (void **)&prms, &size);

        if (255 == prms->local_val)
        {
            prms->local_val = 0;
        }
        else
        {
            prms->local_val++;
        }

        /* hold onto current obj_desc and release old obj desc */
        obj_desc[TIVX_KERNEL_SCALAR_SINK_IN_IDX] = prms->old_obj_desc[0];
        prms->old_obj_desc[0] = (tivx_obj_desc_t*)in_desc;
        
        if (prms->local_val != in_value && prms->do_error_print)
        {
            if(prms->do_error_print>0)
                prms->do_error_print--;
            VX_PRINT(VX_ZONE_ERROR, "error #%d, local val %d != given val %d !!!\n", prms->do_error_print, prms->local_val, in_value);
            status = VX_FAILURE;
        }
    }

    return status;
}

static vx_status VX_CALLBACK tivxScalarSink2Create(
       tivx_target_kernel_instance kernel,
       tivx_obj_desc_t *obj_desc[],
       uint16_t num_params, void *priv_arg)
{
    vx_status status = VX_SUCCESS;
    tivxScalarSink2Params *prms = NULL;
    uint32_t i;

    prms = tivxMemAlloc(sizeof(tivxScalarSink2Params), TIVX_MEM_EXTERNAL);

    tivxSetTargetKernelInstanceContext(kernel, prms,
       sizeof(tivxScalarSink2Params));

    prms->local_val = 0;
    prms->do_error_print = 10; /* max number of times to do error print */
    
    for(i=0; i<MAX_OLD_OBJ_DESC; i++)
    {
        prms->old_obj_desc[i] = NULL;
    }

    return status;
}

static vx_status VX_CALLBACK tivxScalarSink2Delete(
       tivx_target_kernel_instance kernel,
       tivx_obj_desc_t *obj_desc[],
       uint16_t num_params, void *priv_arg)
{
    vx_status status = VX_SUCCESS;
    tivxScalarSink2Params *prms = NULL;
    uint32_t size;

    status = tivxGetTargetKernelInstanceContext(kernel,
        (void **)&prms, &size);

    tivxMemFree(prms, sizeof(tivxScalarSink2Params), TIVX_MEM_EXTERNAL);

    return status;
}

static vx_status VX_CALLBACK tivxScalarSink2Control(
       tivx_target_kernel_instance kernel,
       tivx_obj_desc_t *obj_desc[],
       uint16_t num_params, void *priv_arg)
{
    vx_status status = VX_SUCCESS;

    /* < DEVELOPER_TODO: (Optional) Add any target kernel control code here (e.g. commands */
    /*                   the user can call to modify the processing of the kernel at run-time) > */

    return status;
}

void tivxAddTargetKernelScalarSink2(void)
{
    vx_status status = VX_FAILURE;
    char target_name[TIVX_TARGET_MAX_NAME];
    vx_enum self_cpu;

    self_cpu = tivxGetSelfCpuId();

    if ( (self_cpu == TIVX_CPU_ID_IPU1_0) ||
          (self_cpu == TIVX_CPU_ID_A15_0) ||
          (self_cpu == TIVX_CPU_ID_IPU2_0) ||
          (self_cpu == TIVX_CPU_ID_IPU1_1) ||
          (self_cpu == TIVX_CPU_ID_DSP1) ||
          (self_cpu == TIVX_CPU_ID_DSP2))
    {
        if (self_cpu == TIVX_CPU_ID_IPU1_0)
        {
            strncpy(target_name, TIVX_TARGET_IPU1_0, TIVX_TARGET_MAX_NAME);
            status = VX_SUCCESS;
        }
        else if (self_cpu == TIVX_CPU_ID_A15_0)
        {
            strncpy(target_name, TIVX_TARGET_A15_0, TIVX_TARGET_MAX_NAME);
            status = VX_SUCCESS;
        }
        else if (self_cpu == TIVX_CPU_ID_IPU2_0)
        {
            strncpy(target_name, TIVX_TARGET_IPU2, TIVX_TARGET_MAX_NAME);
            status = VX_SUCCESS;
        }
        else if (self_cpu == TIVX_CPU_ID_IPU1_1)
        {
            strncpy(target_name, TIVX_TARGET_IPU1_1, TIVX_TARGET_MAX_NAME);
            status = VX_SUCCESS;
        }
        else if (self_cpu == TIVX_CPU_ID_DSP1)
        {
            strncpy(target_name, TIVX_TARGET_DSP1, TIVX_TARGET_MAX_NAME);
            status = VX_SUCCESS;
        }
        else if (self_cpu == TIVX_CPU_ID_DSP2)
        {
            strncpy(target_name, TIVX_TARGET_DSP2, TIVX_TARGET_MAX_NAME);
            status = VX_SUCCESS;
        }
    }
    else
    {
        status = VX_FAILURE;
    }

    if (status == VX_SUCCESS)
    {
        vx_scalar_sink_target_kernel = tivxAddTargetKernelByName(
                            TIVX_KERNEL_SCALAR_SINK2_NAME,
                            target_name,
                            tivxScalarSink2Process,
                            tivxScalarSink2Create,
                            tivxScalarSink2Delete,
                            tivxScalarSink2Control,
                            NULL);
    }
}

void tivxRemoveTargetKernelScalarSink2(void)
{
    vx_status status = VX_SUCCESS;

    status = tivxRemoveTargetKernel(vx_scalar_sink_target_kernel);
    if (status == VX_SUCCESS)
    {
        vx_scalar_sink_target_kernel = NULL;
    }
}


