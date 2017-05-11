/*
 *******************************************************************************
 *
 * Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

#include <TI/tivx.h>
#include <tivx_openvx_core_kernels.h>
#include <tivx_kernel_harris_corners.h>
#include <TI/tivx_target_kernel.h>

static vx_kernel vx_harris_corners_kernel = NULL;

static vx_status VX_CALLBACK tivxAddKernelHarrisCValidate(vx_node node,
            const vx_reference parameters[ ],
            vx_uint32 num,
            vx_meta_format metas[])
{
    vx_status status = VX_SUCCESS;
    vx_image img;
    vx_uint32 w, h, i;
    vx_df_image fmt;
    vx_enum arr_type;
    vx_array arr;
    vx_scalar scalar;
    vx_enum stype;
    vx_size arr_capacity = 1;
    vx_int32 gs_bs_values;
    vx_border_t border;

    for (i = 0U; i < TIVX_KERNEL_HARRISC_MAX_PARAMS; i ++)
    {
        /* Check for NULL */
        if ((NULL == parameters[i]) && (i != TIVX_KERNEL_HARRISC_OUT_SC_CNT_IDX))
        {
            status = VX_ERROR_NO_MEMORY;
            break;
        }
    }
    if (VX_SUCCESS == status)
    {
        img = (vx_image)parameters[TIVX_KERNEL_HARRISC_IN_IMG_IDX];

        /* Get the image width/heigh and format */
        status = vxQueryImage(img, VX_IMAGE_FORMAT, &fmt,
            sizeof(fmt));

        status |= vxQueryImage(img, VX_IMAGE_WIDTH, &w, sizeof(w));
        status |= vxQueryImage(img, VX_IMAGE_HEIGHT, &h, sizeof(h));
    }

    if (VX_SUCCESS == status)
    {
        /* Check for validity of data format */
        if (VX_DF_IMAGE_U8 != fmt)
        {
            status = VX_ERROR_INVALID_PARAMETERS;
        }

        if ((w < TIVX_KERNEL_HARRISC_MIN_SIZE) ||
            (h < TIVX_KERNEL_HARRISC_MIN_SIZE))
        {
            status = VX_ERROR_INVALID_PARAMETERS;
        }
    }

    if (VX_SUCCESS == status)
    {
        scalar = (vx_scalar)parameters[TIVX_KERNEL_HARRISC_IN_SC_THR_IDX];

        status = vxQueryScalar(scalar, VX_SCALAR_TYPE, &stype,
            sizeof(stype));
        if (VX_SUCCESS == status)
        {
            if (stype != VX_TYPE_FLOAT32)
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
        }
    }
    if (VX_SUCCESS == status)
    {
        scalar = (vx_scalar)parameters[TIVX_KERNEL_HARRISC_IN_SC_DIST_IDX];

        status = vxQueryScalar(scalar, VX_SCALAR_TYPE, &stype,
            sizeof(stype));
        if (VX_SUCCESS == status)
        {
            if (stype != VX_TYPE_FLOAT32)
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
        }
    }

    if (VX_SUCCESS == status)
    {
        scalar = (vx_scalar)parameters[TIVX_KERNEL_HARRISC_IN_SC_SENS_IDX];

        status = vxQueryScalar(scalar, VX_SCALAR_TYPE, &stype,
            sizeof(stype));
        if (VX_SUCCESS == status)
        {
            if (stype != VX_TYPE_FLOAT32)
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
        }
    }

    if (VX_SUCCESS == status)
    {
        scalar = (vx_scalar)parameters[TIVX_KERNEL_HARRISC_IN_SC_GS_IDX];

        status = vxQueryScalar(scalar, VX_SCALAR_TYPE, &stype,
            sizeof(stype));
        if (VX_SUCCESS == status)
        {
            if (stype != VX_TYPE_INT32)
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            else
            {
                status = vxCopyScalar(scalar, &gs_bs_values,
                    VX_READ_ONLY, VX_MEMORY_TYPE_HOST);

                if (status == VX_SUCCESS)
                {
                    if ((3 != gs_bs_values) && (5 != gs_bs_values) &&
                        (7 != gs_bs_values))
                    {
                        status = VX_ERROR_INVALID_PARAMETERS;
                    }
                }
            }
        }
    }

    if (VX_SUCCESS == status)
    {
        scalar = (vx_scalar)parameters[TIVX_KERNEL_HARRISC_IN_SC_BS_IDX];

        status = vxQueryScalar(scalar, VX_SCALAR_TYPE, &stype,
            sizeof(stype));
        if (VX_SUCCESS == status)
        {
            if (stype != VX_TYPE_INT32)
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
            else
            {
                status = vxCopyScalar(scalar, &gs_bs_values,
                    VX_READ_ONLY, VX_MEMORY_TYPE_HOST);

                if (status == VX_SUCCESS)
                {
                    if ((3 != gs_bs_values) && (5 != gs_bs_values) &&
                        (7 != gs_bs_values))
                    {
                        status = VX_ERROR_INVALID_PARAMETERS;
                    }
                }
            }
        }
    }

    if ((VX_SUCCESS == status) &&
        (NULL != parameters[TIVX_KERNEL_HARRISC_OUT_SC_CNT_IDX]))
    {
        scalar = (vx_scalar)parameters[TIVX_KERNEL_HARRISC_OUT_SC_CNT_IDX];

        status = vxQueryScalar(scalar, VX_SCALAR_TYPE, &stype,
            sizeof(stype));
        if (VX_SUCCESS == status)
        {
            if (stype != VX_TYPE_SIZE)
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
        }
    }

    if (VX_SUCCESS == status)
    {
        arr = (vx_array)parameters[TIVX_KERNEL_HARRISC_OUT_ARR_IDX];

        status = vxQueryArray(arr, VX_ARRAY_ITEMTYPE, &arr_type,
            sizeof(arr_type));

        status = vxQueryArray(arr, VX_ARRAY_CAPACITY, &arr_capacity,
            sizeof(arr_capacity));
    }

    if ((VX_SUCCESS == status) &&
        (vx_false_e == tivxIsReferenceVirtual(
            (vx_reference)parameters[TIVX_KERNEL_HARRISC_OUT_ARR_IDX])))
    {
        if (VX_SUCCESS == status)
        {
            if (VX_TYPE_KEYPOINT != arr_type)
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
        }
    }

    if (VX_SUCCESS == status)
    {
        status = vxQueryNode(node, VX_NODE_BORDER, &border, sizeof(border));
        if (VX_SUCCESS == status)
        {
            if (border.mode != VX_BORDER_UNDEFINED)
            {
                status = VX_ERROR_NOT_SUPPORTED;
                VX_PRINT(VX_ZONE_ERROR, "Only undefined border mode is supported for harris corners\n");
            }
        }
    }

    if (VX_SUCCESS == status)
    {
        arr_type = VX_TYPE_KEYPOINT;

        for (i = 0U; i < TIVX_KERNEL_HARRISC_MAX_PARAMS; i ++)
        {
            if (NULL != metas[i])
            {
                vxSetMetaFormatAttribute(metas[i], VX_ARRAY_ITEMTYPE, &arr_type,
                    sizeof(arr_type));
                vxSetMetaFormatAttribute(metas[i], VX_ARRAY_CAPACITY,
                    &arr_capacity, sizeof(arr_capacity));
            }
        }
    }
    return status;
}

vx_status tivxAddKernelHarrisCorners(vx_context context)
{
    vx_kernel kernel;
    vx_status status;
    uint32_t index;

    kernel = vxAddUserKernel(
                            context,
                            "org.khronos.openvx.harris_corners",
                            VX_KERNEL_HARRIS_CORNERS,
                            NULL,
                            TIVX_KERNEL_HARRISC_MAX_PARAMS,
                            tivxAddKernelHarrisCValidate,
                            NULL,
                            NULL);

    status = vxGetStatus((vx_reference)kernel);

    if ( status == VX_SUCCESS)
    {
        index = 0;

        if ( status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                index,
                VX_INPUT,
                VX_TYPE_IMAGE,
                VX_PARAMETER_STATE_REQUIRED
                );
            index++;
        }
        if ( status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                index,
                VX_INPUT,
                VX_TYPE_SCALAR,
                VX_PARAMETER_STATE_REQUIRED
                );
            index++;
        }
        if ( status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                index,
                VX_INPUT,
                VX_TYPE_SCALAR,
                VX_PARAMETER_STATE_REQUIRED
                );
            index++;
        }
        if ( status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                index,
                VX_INPUT,
                VX_TYPE_SCALAR,
                VX_PARAMETER_STATE_REQUIRED
                );
            index++;
        }
        if ( status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                index,
                VX_INPUT,
                VX_TYPE_SCALAR,
                VX_PARAMETER_STATE_REQUIRED
                );
            index++;
        }
        if ( status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                index,
                VX_INPUT,
                VX_TYPE_SCALAR,
                VX_PARAMETER_STATE_REQUIRED
                );
            index++;
        }
        if ( status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                index,
                VX_OUTPUT,
                VX_TYPE_ARRAY,
                VX_PARAMETER_STATE_REQUIRED
                );
            index++;
        }
        if ( status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                index,
                VX_OUTPUT,
                VX_TYPE_SCALAR,
                VX_PARAMETER_STATE_OPTIONAL
                );
            index++;
        }
        if ( status == VX_SUCCESS)
        {
            /* add supported target's */
            tivxAddKernelTarget(kernel, TIVX_TARGET_DSP1);
            tivxAddKernelTarget(kernel, TIVX_TARGET_DSP2);
        }

        if ( status == VX_SUCCESS)
        {
            status = vxFinalizeKernel(kernel);
        }
        if( status != VX_SUCCESS)
        {
            vxReleaseKernel(&kernel);
            kernel = NULL;
        }
    }
    else
    {
        kernel = NULL;
    }

    vx_harris_corners_kernel = kernel;

    return status;
}

vx_status tivxRemoveKernelHarrisCorners(vx_context context)
{
    vx_status status;
    vx_kernel kernel = vx_harris_corners_kernel;

    /* Kernel is released as part of Remove Kernel */
    status = vxRemoveKernel(kernel);

    vx_harris_corners_kernel = NULL;

    return status;
}





