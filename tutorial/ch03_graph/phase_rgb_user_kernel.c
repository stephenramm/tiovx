/*
 *******************************************************************************
 *
 * Copyright (C) 2017 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <VX/vx.h>
#include <TI/tivx.h>
#include <ch03_graph/phase_rgb_user_kernel.h>

#define PHASE_RGB_IN0_IMG_IDX   (0u)
#define PHASE_RGB_OUT0_IMG_IDX  (1u)
#define PHASE_RGB_MAX_PARAMS    (2u)

static vx_status VX_CALLBACK phase_rgb_user_kernel_init(vx_node node,
            const vx_reference parameters[ ],
            vx_uint32 num);
static vx_status VX_CALLBACK phase_rgb_user_kernel_run(vx_node node,
            const vx_reference parameters[ ],
            vx_uint32 num);
static vx_status VX_CALLBACK phase_rgb_user_kernel_deinit(vx_node node,
            const vx_reference parameters[ ],
            vx_uint32 num);
static vx_status VX_CALLBACK phase_rgb_user_kernel_validate(vx_node node,
            const vx_reference parameters[ ],
            vx_uint32 num,
            vx_meta_format metas[]);

static vx_kernel phase_rgb_user_kernel_add_as_user_kernel(vx_context context);
static vx_kernel phase_rgb_user_kernel_add_as_target_kernel(vx_context context);

static vx_kernel phase_rgb_user_kernel = NULL;
static vx_enum phase_rgb_user_kernel_id = VX_ERROR_INVALID_PARAMETERS;


vx_status phase_rgb_user_kernel_add(vx_context context, vx_bool add_as_target_kernel)
{
    vx_kernel kernel = NULL;
    vx_status status;
    uint32_t index;

    if(add_as_target_kernel==vx_false_e)
    {
        kernel = phase_rgb_user_kernel_add_as_user_kernel(context);
    }
    if(add_as_target_kernel==vx_true_e)
    {
        kernel = phase_rgb_user_kernel_add_as_target_kernel(context);
    }

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
                VX_TYPE_IMAGE,
                VX_PARAMETER_STATE_REQUIRED
                );
            index++;
        }
        if ( status == VX_SUCCESS)
        {
            status = vxFinalizeKernel(kernel);
        }
        if( status != VX_SUCCESS)
        {
            printf(" phase_rgb_user_kernel_add: ERROR: vxAddParameterToKernel, vxFinalizeKernel failed (%d)!!!\n", status);
            vxReleaseKernel(&kernel);
            kernel = NULL;
        }
    }
    else
    {
        kernel = NULL;
        printf(" phase_rgb_user_kernel_add: ERROR: vxAddUserKernel failed (%d)!!!\n", status);
    }

    if(status==VX_SUCCESS)
    {
        phase_rgb_user_kernel = kernel;
        printf(" phase_rgb_user_kernel_add: SUCCESS !!!\n");
    }

    return status;
}

static vx_kernel phase_rgb_user_kernel_add_as_user_kernel(vx_context context)
{
    vx_kernel kernel = NULL;
    vx_status status;

    status = vxAllocateUserKernelId(context, &phase_rgb_user_kernel_id);
    if(status!=VX_SUCCESS)
    {
        printf(" phase_rgb_user_kernel_add_as_user_kernel: ERROR: vxAllocateUserKernelId failed (%d)!!!\n", status);
    }
    if(status==VX_SUCCESS)
    {
        kernel = vxAddUserKernel(
                    context,
                    "vx_tutorial_graph.phase_rgb",
                    phase_rgb_user_kernel_id,
                    phase_rgb_user_kernel_run,
                    2,
                    phase_rgb_user_kernel_validate,
                    phase_rgb_user_kernel_init,
                    phase_rgb_user_kernel_deinit);
    }

    return kernel;
}

static vx_kernel phase_rgb_user_kernel_add_as_target_kernel(vx_context context)
{
    vx_kernel kernel;
    vx_status status;

    phase_rgb_user_kernel_id = 0;

    kernel = vxAddUserKernel(
                context,
                "vx_tutorial_graph.phase_rgb",
                phase_rgb_user_kernel_id,
                NULL,
                2,
                phase_rgb_user_kernel_validate,
                NULL,
                NULL);

    status = vxGetStatus((vx_reference)kernel);

    if ( status == VX_SUCCESS)
    {
        /* add supported target's */
        tivxAddKernelTarget(kernel, TIVX_TARGET_DSP1);
        tivxAddKernelTarget(kernel, TIVX_TARGET_DSP2);
    }
    return kernel;
}

vx_status phase_rgb_user_kernel_remove(vx_context context)
{
    vx_status status;

    status = vxRemoveKernel(phase_rgb_user_kernel);
    phase_rgb_user_kernel = NULL;

    if(status!=VX_SUCCESS)
    {
        printf(" phase_rgb_user_kernel_remove: Unable to remove kernel (%d)!!!\n", status);
    }

    if(status==VX_SUCCESS)
    {
        printf(" phase_rgb_user_kernel_remove: SUCCESS !!!\n");
    }

    return status;
}

static vx_status VX_CALLBACK phase_rgb_user_kernel_validate(vx_node node,
            const vx_reference parameters[ ],
            vx_uint32 num,
            vx_meta_format metas[])
{
    vx_status status = VX_SUCCESS;
    vx_image img[PHASE_RGB_MAX_PARAMS];
    vx_uint32 w[PHASE_RGB_MAX_PARAMS];
    vx_uint32 h[PHASE_RGB_MAX_PARAMS];
    vx_uint32 out_w, out_h, i;
    vx_df_image fmt[PHASE_RGB_MAX_PARAMS], out_fmt;

    if (num != PHASE_RGB_MAX_PARAMS)
    {
        printf(" phase_rgb_user_kernel_validate: ERROR: Number of parameters dont match !!!\n");
        status = VX_ERROR_INVALID_PARAMETERS;
    }

    for (i = 0U; i < PHASE_RGB_MAX_PARAMS; i ++)
    {
        img[i] = (vx_image)parameters[i];

        /* Check for NULL */
        if (NULL == img[i])
        {
            printf(" phase_rgb_user_kernel_validate: ERROR: Parameter %d is NULL !!!\n", i);
            status = VX_ERROR_NO_MEMORY;
            break;
        }
    }
    if (VX_SUCCESS == status)
    {
        /* Get the image width/heigh and format */
        status = vxQueryImage(img[PHASE_RGB_IN0_IMG_IDX],
            VX_IMAGE_FORMAT, &fmt[PHASE_RGB_IN0_IMG_IDX],
            sizeof(vx_df_image));
        status |= vxQueryImage(img[PHASE_RGB_IN0_IMG_IDX],
            VX_IMAGE_WIDTH, &w[PHASE_RGB_IN0_IMG_IDX],
            sizeof(vx_uint32));
        status |= vxQueryImage(img[PHASE_RGB_IN0_IMG_IDX],
            VX_IMAGE_HEIGHT, &h[PHASE_RGB_IN0_IMG_IDX],
            sizeof(vx_uint32));

        if(status!=VX_SUCCESS)
        {
            printf(" phase_rgb_user_kernel_validate: ERROR: Unable to query input image !!!\n");
        }
    }
    if ((VX_SUCCESS == status) &&
        (vx_false_e == tivxIsReferenceVirtual(
            parameters[PHASE_RGB_OUT0_IMG_IDX])))
    {
        /* Get the image width/heigh and format */
        status = vxQueryImage(img[PHASE_RGB_OUT0_IMG_IDX],
            VX_IMAGE_FORMAT, &fmt[PHASE_RGB_OUT0_IMG_IDX],
            sizeof(vx_df_image));
        status |= vxQueryImage(img[PHASE_RGB_OUT0_IMG_IDX],
            VX_IMAGE_WIDTH, &w[PHASE_RGB_OUT0_IMG_IDX],
            sizeof(vx_uint32));
        status |= vxQueryImage(img[PHASE_RGB_OUT0_IMG_IDX],
            VX_IMAGE_HEIGHT, &h[PHASE_RGB_OUT0_IMG_IDX],
            sizeof(vx_uint32));

        if(status!=VX_SUCCESS)
        {
            printf(" phase_rgb_user_kernel_validate: ERROR: Unable to query output image !!!\n");
        }
    }

    if (VX_SUCCESS == status)
    {
        /* Check for validity of data format */
        if (VX_DF_IMAGE_U8 != fmt[PHASE_RGB_IN0_IMG_IDX] &&
            VX_DF_IMAGE_RGB != fmt[PHASE_RGB_OUT0_IMG_IDX]
            )
        {
            status = VX_ERROR_INVALID_PARAMETERS;
            printf(" phase_rgb_user_kernel_validate: ERROR: Input/Output image format not correct !!!\n");
        }

        /* Check for frame sizes */
        if (vx_false_e == tivxIsReferenceVirtual(
            parameters[PHASE_RGB_OUT0_IMG_IDX]))
        {
            if ((w[PHASE_RGB_IN0_IMG_IDX] !=
                 w[PHASE_RGB_OUT0_IMG_IDX]) ||
                (h[PHASE_RGB_IN0_IMG_IDX] !=
                 h[PHASE_RGB_OUT0_IMG_IDX]))
            {
                status = VX_ERROR_INVALID_PARAMETERS;
                printf(" phase_rgb_user_kernel_validate: ERROR: Input/Output image wxh do not match !!!\n");
            }
        }
    }

    if (VX_SUCCESS == status)
    {
        out_fmt = VX_DF_IMAGE_RGB;
        out_w = w[PHASE_RGB_OUT0_IMG_IDX];
        out_h = h[PHASE_RGB_OUT0_IMG_IDX];

        vxSetMetaFormatAttribute(metas[PHASE_RGB_OUT0_IMG_IDX], VX_IMAGE_FORMAT, &out_fmt,
            sizeof(out_fmt));
        vxSetMetaFormatAttribute(metas[PHASE_RGB_OUT0_IMG_IDX], VX_IMAGE_WIDTH, &out_w,
            sizeof(out_w));
        vxSetMetaFormatAttribute(metas[PHASE_RGB_OUT0_IMG_IDX], VX_IMAGE_HEIGHT, &out_h,
            sizeof(out_h));
    }

    if(status==VX_SUCCESS)
    {
        printf(" phase_rgb_user_kernel_validate: SUCCESS !!!\n");
    }

    return status;
}

static vx_status VX_CALLBACK phase_rgb_user_kernel_init(vx_node node,
            const vx_reference parameters[ ],
            vx_uint32 num)
{
    printf(" phase_rgb_user_kernel_init: SUCCESS !!!\n");
    return VX_SUCCESS;
}

static vx_status VX_CALLBACK phase_rgb_user_kernel_run(vx_node node,
            const vx_reference parameters[ ],
            vx_uint32 num)
{
    vx_image in_image = (vx_image)parameters[PHASE_RGB_IN0_IMG_IDX];
    vx_image out_image = (vx_image)parameters[PHASE_RGB_OUT0_IMG_IDX];
    vx_rectangle_t rect = { 0 };
    vx_map_id in_map_id = VX_ERROR_INVALID_PARAMETERS, out_map_id = VX_ERROR_INVALID_PARAMETERS;
    vx_imagepatch_addressing_t in_image_addr, out_image_addr;
    vx_uint32 out_w=0, out_h=0;
    vx_status status=0;
    void *in_data_ptr, *out_data_ptr;

    status |= vxQueryImage(out_image,
        VX_IMAGE_WIDTH, &out_w,
        sizeof(vx_uint32));
    status |= vxQueryImage(out_image,
        VX_IMAGE_HEIGHT, &out_h,
        sizeof(vx_uint32));

    rect.start_x = 0;
    rect.start_y = 0;
    rect.end_x   = out_w;
    rect.end_y   = out_h;

    if(status==VX_SUCCESS)
    {
        status = vxMapImagePatch(in_image,
                &rect,
                0,
                &in_map_id,
                &in_image_addr,
                (void**)&in_data_ptr,
                VX_READ_ONLY, VX_MEMORY_TYPE_HOST, VX_NOGAP_X);
    }

    if(status==VX_SUCCESS)
    {
        status = vxMapImagePatch(out_image,
                &rect,
                0,
                &out_map_id,
                &out_image_addr,
                (void**)&out_data_ptr,
                VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST, VX_NOGAP_X);
    }
    if(status==VX_SUCCESS)
    {
        vx_uint32 x, y;

        for(x=0; x<out_image_addr.dim_x; x++)
        {
            for(y=0; y<out_image_addr.dim_y; y++)
            {
                vx_uint8 *in_pixel = (vx_uint8 *)vxFormatImagePatchAddress2d(in_data_ptr, x, y, &in_image_addr);
                vx_uint32 *out_pixel = (vx_uint32 *)vxFormatImagePatchAddress2d(out_data_ptr, x, y, &out_image_addr);

                if(*in_pixel<64)
                    *out_pixel = 0x00FF0000;
                else
                if(*in_pixel<128)
                    *out_pixel = 0x0000FF00;
                else
                if(*in_pixel<192)
                    *out_pixel = 0x000000FF;
                else
                    *out_pixel = 0x00FFFFFF;
            }
        }

    }

    if(in_map_id!=VX_ERROR_INVALID_PARAMETERS)
    {
        vxUnmapImagePatch(in_image, in_map_id);
    }
    if(out_map_id!=VX_ERROR_INVALID_PARAMETERS)
    {
        vxUnmapImagePatch(out_image, out_map_id);
    }

    if(status!=VX_SUCCESS)
    {
        printf(" phase_rgb_user_kernel_run: ERROR: Run failed (%d)!!!\n", status);
    }
    return status;
}

static vx_status VX_CALLBACK phase_rgb_user_kernel_deinit(vx_node node,
            const vx_reference parameters[ ],
            vx_uint32 num)
{
    printf(" phase_rgb_user_kernel_deinit: SUCCESS !!!\n");
    return VX_SUCCESS;
}

vx_node phase_rgb_user_kernel_node(vx_graph graph, vx_image in, vx_image out)
{
    vx_node node;
    vx_reference refs[] = {(vx_reference)in, (vx_reference)out};

    node = tivxCreateNodeByStructure(graph,
                phase_rgb_user_kernel_id,
                refs, sizeof(refs)/sizeof(refs[0])
                );
    vxSetReferenceName((vx_reference)node, "PHASE_RGB");

    return node;
}


