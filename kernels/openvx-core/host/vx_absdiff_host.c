/*
 *******************************************************************************
 *
 * Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */


#include <TI/tivx.h>
#include <VX/vx_types.h>
#include <tivx_kernel_absdiff.h>

static vx_kernel vx_absdiff_kernel = NULL;


static vx_status VX_CALLBACK tivxAddKernelAbsDiffValidate(vx_node node,
            const vx_reference parameters[ ],
            vx_uint32 num,
            vx_meta_format metas[])
{
    vx_status status = VX_SUCCESS;
    vx_image img[TIVX_KERNEL_ABSDIFF_MAX_PARAMS];
    vx_uint32 w[TIVX_KERNEL_ABSDIFF_MAX_PARAMS];
    vx_uint32 h[TIVX_KERNEL_ABSDIFF_MAX_PARAMS];
    vx_uint32 out_w, out_h, i;
    vx_df_image fmt[TIVX_KERNEL_ABSDIFF_MAX_PARAMS], out_fmt;

    if (num != TIVX_KERNEL_ABSDIFF_MAX_PARAMS)
    {
        status = VX_ERROR_INVALID_PARAMETERS;
    }

    for (i = 0U; i < TIVX_KERNEL_ABSDIFF_MAX_PARAMS; i ++)
    {
        img[i] = (vx_image)parameters[i];

        /* Check for NULL */
        if (NULL == img[i])
        {
            status = VX_ERROR_NO_MEMORY;
            break;
        }
    }
    if (VX_SUCCESS == status)
    {
        /* Get the image width/heigh and format */
        status = vxQueryImage(img[TIVX_KERNEL_ABSDIFF_IN0_IMG_IDX],
            VX_IMAGE_FORMAT, &fmt[TIVX_KERNEL_ABSDIFF_IN0_IMG_IDX],
            sizeof(vx_df_image));
        status |= vxQueryImage(img[TIVX_KERNEL_ABSDIFF_IN0_IMG_IDX],
            VX_IMAGE_WIDTH, &w[TIVX_KERNEL_ABSDIFF_IN0_IMG_IDX],
            sizeof(vx_uint32));
        status |= vxQueryImage(img[TIVX_KERNEL_ABSDIFF_IN0_IMG_IDX],
            VX_IMAGE_HEIGHT, &h[TIVX_KERNEL_ABSDIFF_IN0_IMG_IDX],
            sizeof(vx_uint32));
    }
    if (VX_SUCCESS == status)
    {
        /* Get the image width/heigh and format */
        status = vxQueryImage(img[TIVX_KERNEL_ABSDIFF_IN1_IMG_IDX],
            VX_IMAGE_FORMAT, &fmt[TIVX_KERNEL_ABSDIFF_IN1_IMG_IDX],
            sizeof(vx_df_image));
        status |= vxQueryImage(img[TIVX_KERNEL_ABSDIFF_IN1_IMG_IDX],
            VX_IMAGE_WIDTH, &w[TIVX_KERNEL_ABSDIFF_IN1_IMG_IDX],
            sizeof(vx_uint32));
        status |= vxQueryImage(img[TIVX_KERNEL_ABSDIFF_IN1_IMG_IDX],
            VX_IMAGE_HEIGHT, &h[TIVX_KERNEL_ABSDIFF_IN1_IMG_IDX],
            sizeof(vx_uint32));
    }
    if ((VX_SUCCESS == status) &&
        (vx_false_e == tivxIsReferenceVirtual(
            parameters[TIVX_KERNEL_ABSDIFF_OUT_IMG_IDX])))
    {
        /* Get the image width/heigh and format */
        status = vxQueryImage(img[TIVX_KERNEL_ABSDIFF_OUT_IMG_IDX],
            VX_IMAGE_FORMAT, &fmt[TIVX_KERNEL_ABSDIFF_OUT_IMG_IDX],
            sizeof(vx_df_image));
        status |= vxQueryImage(img[TIVX_KERNEL_ABSDIFF_OUT_IMG_IDX],
            VX_IMAGE_WIDTH, &w[TIVX_KERNEL_ABSDIFF_OUT_IMG_IDX],
            sizeof(vx_uint32));
        status |= vxQueryImage(img[TIVX_KERNEL_ABSDIFF_OUT_IMG_IDX],
            VX_IMAGE_HEIGHT, &h[TIVX_KERNEL_ABSDIFF_OUT_IMG_IDX],
            sizeof(vx_uint32));
    }

    if (VX_SUCCESS == status)
    {
        /* Check for validity of data format */
        if (((VX_DF_IMAGE_S16 != fmt[TIVX_KERNEL_ABSDIFF_IN0_IMG_IDX]) &&
             (VX_DF_IMAGE_U8 != fmt[TIVX_KERNEL_ABSDIFF_IN0_IMG_IDX])) ||
            ((VX_DF_IMAGE_S16 != fmt[TIVX_KERNEL_ABSDIFF_IN1_IMG_IDX]) &&
             (VX_DF_IMAGE_U8 != fmt[TIVX_KERNEL_ABSDIFF_IN1_IMG_IDX])))
        {
            status = VX_ERROR_INVALID_PARAMETERS;
        }

        /* Check for frame sizes */
        if ((w[TIVX_KERNEL_ABSDIFF_IN0_IMG_IDX] !=
                w[TIVX_KERNEL_ABSDIFF_IN1_IMG_IDX]) ||
            (h[TIVX_KERNEL_ABSDIFF_IN0_IMG_IDX] !=
                h[TIVX_KERNEL_ABSDIFF_IN1_IMG_IDX]))
        {
            status = VX_ERROR_INVALID_PARAMETERS;
        }

        if (vx_false_e == tivxIsReferenceVirtual(
            parameters[TIVX_KERNEL_ABSDIFF_OUT_IMG_IDX]))
        {
            if ((w[TIVX_KERNEL_ABSDIFF_IN0_IMG_IDX] !=
                 w[TIVX_KERNEL_ABSDIFF_OUT_IMG_IDX]) ||
                (h[TIVX_KERNEL_ABSDIFF_IN0_IMG_IDX] !=
                 h[TIVX_KERNEL_ABSDIFF_OUT_IMG_IDX]))
            {
                status = VX_ERROR_INVALID_PARAMETERS;
            }
        }
    }

    if (VX_SUCCESS == status)
    {
        if ((VX_DF_IMAGE_S16 == fmt[TIVX_KERNEL_ABSDIFF_IN0_IMG_IDX]) ||
            (VX_DF_IMAGE_S16 == fmt[TIVX_KERNEL_ABSDIFF_IN1_IMG_IDX]))
        {
            out_fmt = VX_DF_IMAGE_S16;
        }
        else
        {
            out_fmt = VX_DF_IMAGE_U8;
        }

        /* If the output format is explicitely set to U8, Set it in
           metadata also */
        status = vxQueryImage(img[TIVX_KERNEL_ABSDIFF_OUT_IMG_IDX],
            VX_IMAGE_FORMAT,
            &fmt[TIVX_KERNEL_ABSDIFF_IN1_IMG_IDX],
            sizeof(fmt[TIVX_KERNEL_ABSDIFF_IN1_IMG_IDX]));
        if ((VX_SUCCESS == status) &&
            (VX_DF_IMAGE_U8 == fmt[TIVX_KERNEL_ABSDIFF_IN1_IMG_IDX]))
        {
            out_fmt = VX_DF_IMAGE_U8;
        }

        out_w = w[TIVX_KERNEL_ABSDIFF_IN0_IMG_IDX];
        out_h = h[TIVX_KERNEL_ABSDIFF_IN0_IMG_IDX];

        for (i = 0U; i < TIVX_KERNEL_ABSDIFF_MAX_PARAMS; i ++)
        {
            vxSetMetaFormatAttribute(metas[i], VX_IMAGE_FORMAT, &out_fmt,
                sizeof(out_fmt));
            vxSetMetaFormatAttribute(metas[i], VX_IMAGE_WIDTH, &out_w,
                sizeof(out_w));
            vxSetMetaFormatAttribute(metas[i], VX_IMAGE_HEIGHT, &out_h,
                sizeof(out_h));
        }
    }

    return status;
}

vx_status tivxAddKernelAbsDiff(vx_context context)
{
    vx_kernel kernel;
    vx_status status;
    uint32_t index;

    kernel = vxAddUserKernel(
                            context,
                            "org.khronos.openvx.absdiff",
                            VX_KERNEL_ABSDIFF,
                            NULL,
                            3,
                            tivxAddKernelAbsDiffValidate,
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

    vx_absdiff_kernel = kernel;

    return status;
}

vx_status tivxRemoveKernelAbsDiff(vx_context context)
{
    vx_status status;
    vx_kernel kernel = vx_absdiff_kernel;

    status = vxRemoveKernel(kernel);
    vx_absdiff_kernel = NULL;

    return status;
}





