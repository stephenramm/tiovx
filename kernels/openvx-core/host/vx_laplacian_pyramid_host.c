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
#include <tivx_kernel_laplacian_pyramid.h>
#include <TI/tivx_target_kernel.h>

static vx_kernel vx_laplacian_pyramid_kernel = NULL;

static vx_status VX_CALLBACK tivxAddKernelLaplacianPyramidValidate(vx_node node,
            const vx_reference parameters[],
            vx_uint32 num,
            vx_meta_format metas[])
{
    vx_status status = VX_SUCCESS;
    vx_image img, low_img;
    vx_pyramid pmd;
    vx_uint32 w, h, i;
    vx_uint32 p_w, p_h;
    vx_size num_levels;
    vx_df_image fmt, p_fmt;
    vx_float32 scale;

    for (i = 0U; i < TIVX_KERNEL_LPL_PMD_MAX_PARAMS; i ++)
    {
        /* Check for NULL */
        if (NULL == parameters[i])
        {
            status = VX_ERROR_NO_MEMORY;
            break;
        }
    }
    if (VX_SUCCESS == status)
    {
        img = (vx_image)parameters[TIVX_KERNEL_LPL_PMD_IN_IMG_IDX];
        pmd = (vx_pyramid)parameters[TIVX_KERNEL_LPL_PMD_OUT_PMD_IDX];
        low_img = (vx_image)parameters[TIVX_KERNEL_LPL_PMD_OUT_IMG_IDX];

        /* Get the image width/heigh and format */
        status = vxQueryImage(img, VX_IMAGE_FORMAT, &fmt, sizeof(fmt));

        status |= vxQueryImage(img, VX_IMAGE_WIDTH, &w, sizeof(w));
        status |= vxQueryImage(img, VX_IMAGE_HEIGHT, &h, sizeof(h));
    }

    if (VX_SUCCESS == status)
    {
        /* Check for validity of data format */
        if ((VX_DF_IMAGE_U8 != fmt) && (VX_DF_IMAGE_S16 != fmt))
        {
            status = VX_ERROR_INVALID_PARAMETERS;
        }
    }

    if (VX_SUCCESS == status)
    {
        status = vxQueryPyramid(pmd, VX_PYRAMID_SCALE, &scale,
            sizeof(scale));
        if (VX_SUCCESS == status)
        {
            if (scale!= VX_SCALE_PYRAMID_HALF)
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
        }
    }

    if (VX_SUCCESS == status)
    {
        status = vxQueryPyramid(pmd, VX_PYRAMID_LEVELS, &num_levels,
            sizeof(num_levels));
    }

    if ((VX_SUCCESS == status) &&
        (vx_false_e == tivxIsReferenceVirtual((vx_reference)pmd)))
    {
        status = vxQueryPyramid(pmd, VX_PYRAMID_WIDTH, &p_w, sizeof(p_w));
        status |= vxQueryPyramid(pmd, VX_PYRAMID_HEIGHT, &p_h, sizeof(p_h));
        status |= vxQueryPyramid(pmd, VX_PYRAMID_FORMAT, &p_fmt, sizeof(p_fmt));

        /* Check for frame sizes */
        if ((w != p_w) || (h != p_h))
        {
            status = VX_ERROR_INVALID_PARAMETERS;
        }

        /* Check for format */
        if (VX_DF_IMAGE_S16 != p_fmt)
        {
            status = VX_ERROR_INVALID_PARAMETERS;
        }
    }

    if ((VX_SUCCESS == status) &&
        (vx_false_e == tivxIsReferenceVirtual((vx_reference)low_img)))
    {
        status = vxQueryImage(low_img, VX_IMAGE_WIDTH, &p_w, sizeof(p_w));
        status |= vxQueryImage(low_img, VX_IMAGE_HEIGHT, &p_h, sizeof(p_h));
        status |= vxQueryImage(low_img, VX_IMAGE_FORMAT, &p_fmt, sizeof(p_fmt));

        /* Check for format */
        if (fmt != p_fmt)
        {
            status = VX_ERROR_INVALID_PARAMETERS;
        }
    }
    if (VX_SUCCESS == status)
    {
        p_fmt = VX_DF_IMAGE_S16;
        scale = VX_SCALE_PYRAMID_HALF;
        if (NULL != metas[TIVX_KERNEL_LPL_PMD_OUT_PMD_IDX])
        {
            vxSetMetaFormatAttribute(metas[TIVX_KERNEL_LPL_PMD_OUT_PMD_IDX],
                VX_PYRAMID_WIDTH, &w, sizeof(w));
            vxSetMetaFormatAttribute(metas[TIVX_KERNEL_LPL_PMD_OUT_PMD_IDX],
                VX_PYRAMID_HEIGHT, &h, sizeof(h));
            vxSetMetaFormatAttribute(metas[TIVX_KERNEL_LPL_PMD_OUT_PMD_IDX],
                VX_PYRAMID_FORMAT, &p_fmt, sizeof(p_fmt));
            vxSetMetaFormatAttribute(metas[TIVX_KERNEL_LPL_PMD_OUT_PMD_IDX],
                VX_PYRAMID_LEVELS, &num_levels, sizeof(num_levels));
            vxSetMetaFormatAttribute(metas[TIVX_KERNEL_LPL_PMD_OUT_PMD_IDX],
                VX_PYRAMID_SCALE, &scale, sizeof(scale));
        }
        /* TODO: Need to set meta for low_img? */
    }

    return status;
}

vx_status tivxAddKernelLaplacianPyramid(vx_context context)
{
    vx_kernel kernel;
    vx_status status;
    uint32_t index;

    kernel = vxAddUserKernel(
                            context,
                            "org.khronos.openvx.laplacian_pyramid",
                            VX_KERNEL_LAPLACIAN_PYRAMID,
                            NULL,
                            TIVX_KERNEL_LPL_PMD_MAX_PARAMS,
                            tivxAddKernelLaplacianPyramidValidate,
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
                VX_OUTPUT,
                VX_TYPE_PYRAMID,
                VX_PARAMETER_STATE_REQUIRED
                );
            index++;
        }
        if ( status == VX_SUCCESS)
        {
            status = vxAddParameterToKernel(kernel,
                index,
                VX_OUTPUT,
                VX_TYPE_IMAGE,
                VX_PARAMETER_STATE_REQUIRED
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

    vx_laplacian_pyramid_kernel = kernel;

    return status;
}

vx_status tivxRemoveKernelLaplacianPyramid(vx_context context)
{
    vx_status status;
    vx_kernel kernel = vx_laplacian_pyramid_kernel;

    /* Kernel is released as part of Remove Kernel */
    status = vxRemoveKernel(kernel);

    vx_laplacian_pyramid_kernel = NULL;

    return status;
}





