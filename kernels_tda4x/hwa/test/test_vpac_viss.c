/*
 *
 * Copyright (c) 2018 Texas Instruments Incorporated
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

#include <VX/vx.h>
#include <TI/tivx.h>
#include <TI/tda4x.h>
#include "test_engine/test.h"
#include "tivx_utils_png_rd_wr.h"
#include <string.h>


TESTCASE(tivxHwaVpacViss, CT_VXContext, ct_setup_vx_context, 0)

TEST(tivxHwaVpacViss, testNodeCreation)
{
    vx_context context = context_->vx_context_;
    vx_array configuration = NULL;
    vx_array ae_awb_result = NULL;
    vx_image raw0 = NULL, raw1 = NULL, raw2 = NULL;
    vx_image y12 = NULL, uv12_c1 = NULL, y8_r8_c2 = NULL, uv8_g8_c3 = NULL, s8_b8_c4 = NULL;
    vx_distribution histogram = NULL;
    vx_array h3a_aew_af = NULL;

    tivx_vpac_viss_params_t params;
    tivx_ae_awb_params_t ae_awb_params; 

    vx_enum params_type = VX_TYPE_INVALID;

    vx_graph graph = 0;
    vx_node node = 0;

    if (vx_true_e == tivxIsTargetEnabled(TIVX_TARGET_VPAC_VISS1))
    {
        tivxHwaLoadKernels(context);

        ASSERT_VX_OBJECT(raw0 = vxCreateImage(context, 128, 128, VX_DF_IMAGE_U16), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(raw1 = vxCreateImage(context, 128, 128, VX_DF_IMAGE_U16), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(raw2 = vxCreateImage(context, 128, 128, VX_DF_IMAGE_U16), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(y12 = vxCreateImage(context, 128, 128, VX_DF_IMAGE_U16), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(uv12_c1 = vxCreateImage(context, 128, 128/2, VX_DF_IMAGE_U16), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(y8_r8_c2 = vxCreateImage(context, 128, 128, VX_DF_IMAGE_U8), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(uv8_g8_c3 = vxCreateImage(context, 128, 128/2, VX_DF_IMAGE_U8), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(s8_b8_c4 = vxCreateImage(context, 128, 128, VX_DF_IMAGE_U8), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(histogram = vxCreateDistribution(context, 256, 0, 256), VX_TYPE_DISTRIBUTION);

        params_type = vxRegisterUserStruct(context, sizeof(tivx_vpac_viss_params_t));
        ASSERT(params_type >= VX_TYPE_USER_STRUCT_START && params_type <= VX_TYPE_USER_STRUCT_END);
        memset(&params, 0, sizeof(tivx_vpac_viss_params_t));
        ASSERT_VX_OBJECT(configuration = vxCreateArray(context, params_type, 1), VX_TYPE_ARRAY);

        params_type = vxRegisterUserStruct(context, sizeof(tivx_ae_awb_params_t));
        ASSERT(params_type >= VX_TYPE_USER_STRUCT_START && params_type <= VX_TYPE_USER_STRUCT_END);
        memset(&ae_awb_params, 0, sizeof(tivx_ae_awb_params_t));
        ASSERT_VX_OBJECT(ae_awb_result = vxCreateArray(context, params_type, 1), VX_TYPE_ARRAY);

        ASSERT_VX_OBJECT(graph = vxCreateGraph(context), VX_TYPE_GRAPH);

        ASSERT_VX_OBJECT(node = tivxVpacVissNode(graph, configuration, ae_awb_result,
                                                raw0, raw1, raw2, y12, uv12_c1, y8_r8_c2, uv8_g8_c3, s8_b8_c4,
                                                histogram, h3a_aew_af), VX_TYPE_NODE);

        VX_CALL(vxSetNodeTarget(node, VX_TARGET_STRING, TIVX_TARGET_VPAC_VISS1));

        VX_CALL(vxReleaseNode(&node));
        VX_CALL(vxReleaseGraph(&graph));
        VX_CALL(vxReleaseDistribution(&histogram));
        VX_CALL(vxReleaseImage(&s8_b8_c4));
        VX_CALL(vxReleaseImage(&uv8_g8_c3));
        VX_CALL(vxReleaseImage(&y8_r8_c2));
        VX_CALL(vxReleaseImage(&uv12_c1));
        VX_CALL(vxReleaseImage(&y12));
        VX_CALL(vxReleaseImage(&raw2));
        VX_CALL(vxReleaseImage(&raw1));
        VX_CALL(vxReleaseImage(&raw0));
        VX_CALL(vxReleaseArray(&ae_awb_result));
        VX_CALL(vxReleaseArray(&configuration));

        ASSERT(node == 0);
        ASSERT(graph == 0);
        ASSERT(h3a_aew_af == 0);
        ASSERT(histogram == 0);
        ASSERT(s8_b8_c4 == 0);
        ASSERT(uv8_g8_c3 == 0);
        ASSERT(y8_r8_c2 == 0);
        ASSERT(uv12_c1 == 0);
        ASSERT(y12 == 0);
        ASSERT(raw2 == 0);
        ASSERT(raw1 == 0);
        ASSERT(raw0 == 0);
        ASSERT(ae_awb_result == 0);
        ASSERT(configuration == 0);

        tivxHwaUnLoadKernels(context);
    }
}

static CT_Image raw_read_image(const char* fileName, int width, int height)
{
    CT_Image image = NULL;
    ASSERT_(return 0, width == 0 && height == 0);
    image = ct_read_image(fileName, 1);
    ASSERT_(return 0, image);
    ASSERT_(return 0, image->format == VX_DF_IMAGE_U8);
    return image;
}

static CT_Image raw_generate_random(const char* fileName, int width, int height)
{
    CT_Image image;

    ASSERT_NO_FAILURE_(return 0,
            image = ct_allocate_ct_image_random(width, height, VX_DF_IMAGE_U16, &CT()->seed_, 0, 4096));

    return image;
}

typedef struct {
    const char* testName;
    CT_Image (*generator)(const char* fileName, int width, int height);
    const char* fileName;
    vx_border_t border;
    int width, height;
} Arg;

#define PARAMETERS \
    CT_GENERATE_PARAMETERS("randomInput", ADD_VX_BORDERS_REQUIRE_UNDEFINED_ONLY, ADD_SIZE_64x64, ARG, raw_generate_random, NULL), \
    CT_GENERATE_PARAMETERS("lena", ADD_VX_BORDERS_REQUIRE_UNDEFINED_ONLY, ADD_SIZE_NONE, ARG, raw_read_image, "lena.bmp")

TEST_WITH_ARG(tivxHwaVpacViss, testGraphProcessing, Arg,
    PARAMETERS
)
{
    vx_context context = context_->vx_context_;
    vx_array configuration = NULL;
    vx_array ae_awb_result = NULL;
    vx_image raw0 = NULL, raw1 = NULL, raw2 = NULL;
    vx_image y12 = NULL, uv12_c1 = NULL, y8_r8_c2 = NULL, uv8_g8_c3 = NULL, s8_b8_c4 = NULL;
    vx_distribution histogram = NULL;
    vx_array h3a_aew_af = NULL;

    tivx_vpac_viss_params_t params;
    tivx_ae_awb_params_t ae_awb_params; 

    vx_enum params_type = VX_TYPE_INVALID;

    vx_graph graph = 0;
    vx_node node = 0;

    CT_Image src0 = NULL, src1 = NULL,  src2 = NULL;
    vx_border_t border = arg_->border;

    if (vx_true_e == tivxIsTargetEnabled(TIVX_TARGET_VPAC_VISS1))
    {
        vx_uint32 width, height;

        tivxHwaLoadKernels(context);

        ASSERT_NO_FAILURE(src0 = arg_->generator(arg_->fileName, arg_->width, arg_->height));
        ASSERT_NO_FAILURE(src1 = arg_->generator(arg_->fileName, arg_->width, arg_->height));
        ASSERT_NO_FAILURE(src2 = arg_->generator(arg_->fileName, arg_->width, arg_->height));

        ASSERT_VX_OBJECT(raw0 = ct_image_to_vx_image(src0, context), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(raw1 = ct_image_to_vx_image(src1, context), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(raw2 = ct_image_to_vx_image(src2, context), VX_TYPE_IMAGE);
        
        VX_CALL(vxQueryImage(raw0, VX_IMAGE_WIDTH, &width, sizeof(width)));
        VX_CALL(vxQueryImage(raw0, VX_IMAGE_HEIGHT, &height, sizeof(height)));
        
        ASSERT_VX_OBJECT(y12 = vxCreateImage(context, width, height, VX_DF_IMAGE_U16), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(uv12_c1 = vxCreateImage(context, width, height/2, VX_DF_IMAGE_U16), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(y8_r8_c2 = vxCreateImage(context, width, height, VX_DF_IMAGE_U8), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(uv8_g8_c3 = vxCreateImage(context, width, height/2, VX_DF_IMAGE_U8), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(s8_b8_c4 = vxCreateImage(context, width, height, VX_DF_IMAGE_U8), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(histogram = vxCreateDistribution(context, 256, 0, 256), VX_TYPE_DISTRIBUTION);

        /* Create/Configure configuration input structure */
        params_type = vxRegisterUserStruct(context, sizeof(tivx_vpac_viss_params_t));
        ASSERT(params_type >= VX_TYPE_USER_STRUCT_START && params_type <= VX_TYPE_USER_STRUCT_END);
        ASSERT_VX_OBJECT(configuration = vxCreateArray(context, params_type, 1), VX_TYPE_ARRAY);

        memset(&params, 0, sizeof(tivx_vpac_viss_params_t));

        snprintf(params.sensor_name, 256, "%s/viss_config/example_sensor/0/0", ct_get_test_file_path());

        params.mux_ee_port = 1;
        params.mux_uv12_c1_out = 0;
        params.mux_y8_r8_c2_out = 0;
        params.mux_uv8_g8_c3_out = 0;
        params.mux_s8_b8_c4_out = 0;
        params.mux_h3a_out = 0;
        params.chroma_out_mode = 0;
        params.bypass_glbce = 0;
        params.bypass_nsf4 = 0;

        VX_CALL(vxAddArrayItems(configuration, 1, &params, sizeof(tivx_vpac_viss_params_t)));


        params_type = vxRegisterUserStruct(context, sizeof(tivx_ae_awb_params_t));
        ASSERT(params_type >= VX_TYPE_USER_STRUCT_START && params_type <= VX_TYPE_USER_STRUCT_END);
        ASSERT_VX_OBJECT(ae_awb_result = vxCreateArray(context, params_type, 1), VX_TYPE_ARRAY);

        memset(&ae_awb_params, 0, sizeof(tivx_ae_awb_params_t));
        VX_CALL(vxAddArrayItems(ae_awb_result, 1, &ae_awb_params, sizeof(tivx_ae_awb_params_t)));

        ASSERT_VX_OBJECT(graph = vxCreateGraph(context), VX_TYPE_GRAPH);

        ASSERT_VX_OBJECT(node = tivxVpacVissNode(graph, configuration, ae_awb_result,
                                                raw0, raw1, raw2, y12, uv12_c1, y8_r8_c2, uv8_g8_c3, s8_b8_c4,
                                                histogram, h3a_aew_af), VX_TYPE_NODE);

        VX_CALL(vxSetNodeTarget(node, VX_TARGET_STRING, TIVX_TARGET_VPAC_VISS1));
        VX_CALL(vxSetNodeAttribute(node, VX_NODE_BORDER, &border, sizeof(border)));

        VX_CALL(vxVerifyGraph(graph));
        VX_CALL(vxProcessGraph(graph));

        VX_CALL(vxReleaseNode(&node));
        VX_CALL(vxReleaseGraph(&graph));
        VX_CALL(vxReleaseDistribution(&histogram));
        VX_CALL(vxReleaseImage(&s8_b8_c4));
        VX_CALL(vxReleaseImage(&uv8_g8_c3));
        VX_CALL(vxReleaseImage(&y8_r8_c2));
        VX_CALL(vxReleaseImage(&uv12_c1));
        VX_CALL(vxReleaseImage(&y12));
        VX_CALL(vxReleaseImage(&raw2));
        VX_CALL(vxReleaseImage(&raw1));
        VX_CALL(vxReleaseImage(&raw0));
        VX_CALL(vxReleaseArray(&ae_awb_result));
        VX_CALL(vxReleaseArray(&configuration));

        ASSERT(node == 0);
        ASSERT(graph == 0);
        ASSERT(h3a_aew_af == 0);
        ASSERT(histogram == 0);
        ASSERT(s8_b8_c4 == 0);
        ASSERT(uv8_g8_c3 == 0);
        ASSERT(y8_r8_c2 == 0);
        ASSERT(uv12_c1 == 0);
        ASSERT(y12 == 0);
        ASSERT(raw2 == 0);
        ASSERT(raw1 == 0);
        ASSERT(raw0 == 0);
        ASSERT(ae_awb_result == 0);
        ASSERT(configuration == 0);

        tivxHwaUnLoadKernels(context);
    }
}

static void ct_write_image2(vx_image image, const char* fileName)
{
    FILE* f = 0;
    size_t sz;
    char* buf = 0;
    char file[MAXPATHLENGTH];

    if (!fileName)
    {
        CT_ADD_FAILURE("Image file name not specified\n");
        return;
    }

    sz = snprintf(file, MAXPATHLENGTH, "%s/%s", ct_get_test_file_path(), fileName);
    ASSERT_(return, (sz < MAXPATHLENGTH));

    f = fopen(file, "wb");
    if (!f)
    {
        CT_ADD_FAILURE("Can't open image file: %s\n", fileName);
        return;
    }
    else
    {
        vx_uint32 width, height;
        vx_imagepatch_addressing_t image_addr;
        vx_rectangle_t rect;
        vx_map_id map_id;
        vx_df_image df;
        void *data_ptr;
        vx_uint32 num_bytes = 1;

        vxQueryImage(image, VX_IMAGE_WIDTH, &width, sizeof(vx_uint32));
        vxQueryImage(image, VX_IMAGE_HEIGHT, &height, sizeof(vx_uint32));
        vxQueryImage(image, VX_IMAGE_FORMAT, &df, sizeof(vx_df_image));
        
        if( (df == VX_DF_IMAGE_U16) || (df == VX_DF_IMAGE_S16) )
        {
            num_bytes = 2;
        }
        else if( (df == VX_DF_IMAGE_U32) || (df == VX_DF_IMAGE_S32) )
        {
            num_bytes = 4;
        }

        rect.start_x = 0;
        rect.start_y = 0;
        rect.end_x = width;
        rect.end_y = height;

        vxMapImagePatch(image,
            &rect,
            0,
            &map_id,
            &image_addr,
            &data_ptr,
            VX_WRITE_ONLY,
            VX_MEMORY_TYPE_HOST,
            VX_NOGAP_X
            );

        fwrite(data_ptr, 1, width*height*num_bytes, f);

        vxUnmapImagePatch(image, map_id);
    }

    fclose(f);
}

static void ct_read_image2(vx_image image, const char* fileName, uint16_t file_byte_pack)
{
    FILE* f = 0;
    size_t sz;
    char* buf = 0;
    char file[MAXPATHLENGTH];

    if (!fileName)
    {
        CT_ADD_FAILURE("Image file name not specified\n");
        return;
    }

    sz = snprintf(file, MAXPATHLENGTH, "%s/%s", ct_get_test_file_path(), fileName);
    ASSERT_(return, (sz < MAXPATHLENGTH));

    f = fopen(file, "rb");
    if (!f)
    {
        CT_ADD_FAILURE("Can't open image file: %s\n", fileName);
        return;
    }

    fseek(f, 0, SEEK_END);
    sz = ftell(f);
    if( sz > 0 )
    {
        buf = (char*)ct_alloc_mem(sz);
        fseek(f, 0, SEEK_SET);
        if( fread(buf, 1, sz, f) == sz )
        {
            vx_uint32 width, height;
            vx_imagepatch_addressing_t image_addr;
            vx_rectangle_t rect;
            vx_map_id map_id;
            vx_df_image df;
            void *data_ptr;
            vx_uint32 num_bytes = 1;

            vxQueryImage(image, VX_IMAGE_WIDTH, &width, sizeof(vx_uint32));
            vxQueryImage(image, VX_IMAGE_HEIGHT, &height, sizeof(vx_uint32));
            vxQueryImage(image, VX_IMAGE_FORMAT, &df, sizeof(vx_df_image));
            
            if( (df == VX_DF_IMAGE_U16) || (df == VX_DF_IMAGE_S16) )
            {
                num_bytes = 2;
            }
            else if( (df == VX_DF_IMAGE_U32) || (df == VX_DF_IMAGE_S32) )
            {
                num_bytes = 4;
            }

            rect.start_x = 0;
            rect.start_y = 0;
            rect.end_x = width;
            rect.end_y = height;

            vxMapImagePatch(image,
                &rect,
                0,
                &map_id,
                &image_addr,
                &data_ptr,
                VX_WRITE_ONLY,
                VX_MEMORY_TYPE_HOST,
                VX_NOGAP_X
                );

            if(file_byte_pack == num_bytes)
            {
                memcpy(data_ptr, buf, width*height*num_bytes);
            }
            else if((file_byte_pack == 2) && (num_bytes == 1))
            {
                int i;
                uint8_t *dst = data_ptr;
                uint16_t *src = (uint16_t*)buf;
                for(i = 0; i < width*height; i++)
                {
                    dst[i] = src[i];
                }
            }
            vxUnmapImagePatch(image, map_id);
        }
    }

    ct_free_mem(buf);
    fclose(f);
}

static void ct_read_hist(vx_distribution hist, const char* fileName, uint16_t file_byte_pack)
{
    FILE* f = 0;
    size_t sz;
    char* buf = 0;
    char file[MAXPATHLENGTH];

    if (!fileName)
    {
        CT_ADD_FAILURE("Hist file name not specified\n");
        return;
    }

    sz = snprintf(file, MAXPATHLENGTH, "%s/%s", ct_get_test_file_path(), fileName);
    ASSERT_(return, (sz < MAXPATHLENGTH));

    f = fopen(file, "rb");
    if (!f)
    {
        CT_ADD_FAILURE("Can't open hist file: %s\n", fileName);
        return;
    }

    fseek(f, 0, SEEK_END);
    sz = ftell(f);
    if( sz > 0 )
    {
        buf = (char*)ct_alloc_mem(sz);
        fseek(f, 0, SEEK_SET);
        if( fread(buf, 1, sz, f) == sz )
        {
            vx_map_id map_id;
            vx_df_image df;
            void *data_ptr;
            vx_uint32 num_bytes = 1;

            vxCopyDistribution (hist, buf, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);
        }
    }

    ct_free_mem(buf);
    fclose(f);
}

static void ct_read_array2(vx_array array, const char* fileName, uint16_t file_byte_pack)
{
    FILE* f = 0;
    size_t sz;
    char* buf = 0;
    char file[MAXPATHLENGTH];

    if (!fileName)
    {
        CT_ADD_FAILURE("Array file name not specified\n");
        return;
    }

    sz = snprintf(file, MAXPATHLENGTH, "%s/%s", ct_get_test_file_path(), fileName);
    ASSERT_(return, (sz < MAXPATHLENGTH));

    f = fopen(file, "rb");
    if (!f)
    {
        CT_ADD_FAILURE("Can't open arrays file: %s\n", fileName);
        return;
    }

    fseek(f, 0, SEEK_END);
    sz = ftell(f);
    if( sz > 0 )
    {
        buf = (char*)ct_alloc_mem(sz);
        fseek(f, 0, SEEK_SET);
        if( fread(buf, 1, sz, f) == sz )
        {
            vx_size item_size;
            vxQueryArray(array, VX_ARRAY_ITEMSIZE, &item_size, sizeof(item_size));
            vxAddArrayItems(array, 1, buf, item_size);
        }
    }

    ct_free_mem(buf);
    fclose(f);
}

static vx_status save_image_from_viss(vx_image y8, char *filename_prefix)
{
    char filename[MAXPATHLENGTH];
    vx_status status;

    snprintf(filename, MAXPATHLENGTH, "%s/%s_y8.png",
        ct_get_test_file_path(), filename_prefix);

    status = tivx_utils_save_vximage_to_pngfile(filename, y8);

    return status;
}

static vx_int32 ct_cmp_image2(vx_image image, vx_image image_ref)
{
    vx_uint32 width, height;
    vx_imagepatch_addressing_t image_addr, ref_addr;
    vx_rectangle_t rect;
    vx_map_id map_id, map_id_ref;
    vx_df_image df;
    void *data_ptr, *ref_ptr;
    vx_uint32 num_bytes = 1;
    vx_int32 i, j;
    vx_int32 error = 0;

    vxQueryImage(image, VX_IMAGE_WIDTH, &width, sizeof(vx_uint32));
    vxQueryImage(image, VX_IMAGE_HEIGHT, &height, sizeof(vx_uint32));
    vxQueryImage(image, VX_IMAGE_FORMAT, &df, sizeof(vx_df_image));
    
    rect.start_x = 0;
    rect.start_y = 0;
    rect.end_x = width;
    rect.end_y = height;

    vxMapImagePatch(image,
        &rect,
        0,
        &map_id,
        &image_addr,
        &data_ptr,
        VX_READ_ONLY,
        VX_MEMORY_TYPE_HOST,
        VX_NOGAP_X
        );

    vxMapImagePatch(image_ref,
        &rect,
        0,
        &map_id_ref,
        &ref_addr,
        &ref_ptr,
        VX_READ_ONLY,
        VX_MEMORY_TYPE_HOST,
        VX_NOGAP_X
        );

    if( df == VX_DF_IMAGE_U8)
    {
        for(j=0; j<height; j++)
        {
            vx_uint8 *d_ptr = (vx_uint8 *)(data_ptr + (j * image_addr.stride_y));
            vx_uint8 *r_ptr = (vx_uint8 *)(ref_ptr + (j * ref_addr.stride_y));
            for(i=0; i<width; i++)
            {
                if(d_ptr[i] != r_ptr[i])
                {
                    error++;
                }
            }
        }
    }
    else if( (df == VX_DF_IMAGE_U16) || (df == VX_DF_IMAGE_S16) )
    {
        for(j=0; j<height; j++)
        {
            vx_uint16 *d_ptr = (vx_uint16 *)(data_ptr + (j * image_addr.stride_y));
            vx_uint16 *r_ptr = (vx_uint16 *)(ref_ptr + (j * ref_addr.stride_y));
            for(i=0; i<width; i++)
            {
                if(d_ptr[i] != r_ptr[i])
                {
                    error++;
                }
            }
        }
    }
    else if( (df == VX_DF_IMAGE_U32) || (df == VX_DF_IMAGE_S32) )
    {
        for(j=0; j<height; j++)
        {
            vx_uint32 *d_ptr = (vx_uint32 *)(data_ptr + (j * image_addr.stride_y));
            vx_uint32 *r_ptr = (vx_uint32 *)(ref_ptr + (j * ref_addr.stride_y));
            for(i=0; i<width; i++)
            {
                if(d_ptr[i] != r_ptr[i])
                {
                    error++;
                }
            }
        }
    }

    vxUnmapImagePatch(image, map_id);
    vxUnmapImagePatch(image_ref, map_id_ref);

    return error;
}

static vx_int32 ct_cmp_hist(vx_distribution hist, vx_distribution hist_ref)
{
    vx_map_id map_id, map_id_ref;
    uint32_t *data_ptr, *ref_ptr;
    vx_uint32 num_bytes = 1;
    vx_size histogram_numBins;
    vx_int32 i, j;
    vx_int32 error = 0;

    vxQueryDistribution(hist, VX_DISTRIBUTION_BINS, &histogram_numBins, sizeof(histogram_numBins));

    vxMapDistribution(hist,
        &map_id,
        (void *)&data_ptr,
        VX_READ_ONLY,
        VX_MEMORY_TYPE_HOST,
        0);

    vxMapDistribution(hist_ref,
        &map_id_ref,
        (void *)&ref_ptr,
        VX_READ_ONLY,
        VX_MEMORY_TYPE_HOST,
        0);

    for(i=0; i<histogram_numBins; i++)
    {
        if(data_ptr[i] != ref_ptr[i])
        {
            error++;
        }
    }

    vxUnmapDistribution(hist, map_id);
    vxUnmapDistribution(hist_ref, map_id_ref);
    
    return error;
}

TEST(tivxHwaVpacViss, testGraphProcessingRaw)
{
    vx_context context = context_->vx_context_;
    vx_array configuration = NULL;
    vx_array ae_awb_result = NULL;
    vx_image raw0 = NULL, raw1 = NULL, raw2 = NULL;
    vx_image y12 = NULL, uv12_c1 = NULL, y8_r8_c2 = NULL, uv8_g8_c3 = NULL, s8_b8_c4 = NULL;
    vx_image y12_ref = NULL, uv12_c1_ref = NULL, y8_r8_c2_ref = NULL, uv8_g8_c3_ref = NULL, s8_b8_c4_ref = NULL;
    vx_distribution histogram = NULL;
    vx_distribution histogram_ref = NULL;
    vx_array h3a_aew_af = NULL;
    vx_array h3a_aew_af_ref = NULL;
    vx_size h3a_output_size;

    tivx_vpac_viss_params_t params;
    tivx_ae_awb_params_t ae_awb_params; 
    void *h3a_output;

    vx_enum params_type = VX_TYPE_INVALID;

    vx_graph graph = 0;
    vx_node node = 0;

    CT_Image src0 = NULL, src1 = NULL,  src2 = NULL;

    if (vx_true_e == tivxIsTargetEnabled(TIVX_TARGET_VPAC_VISS1))
    {
        vx_uint32 width, height;

        tivxHwaLoadKernels(context);

        ASSERT_VX_OBJECT(raw0 = vxCreateImage(context, 1280, 720, VX_DF_IMAGE_U16), VX_TYPE_IMAGE);
        //ASSERT_VX_OBJECT(raw0 = vxCreateImage(context, 1280, 720, VX_DF_IMAGE_U16), VX_TYPE_IMAGE);
        //ASSERT_VX_OBJECT(raw0 = vxCreateImage(context, 1280, 720, VX_DF_IMAGE_U16), VX_TYPE_IMAGE);

        VX_CALL(vxQueryImage(raw0, VX_IMAGE_WIDTH, &width, sizeof(width)));
        VX_CALL(vxQueryImage(raw0, VX_IMAGE_HEIGHT, &height, sizeof(height)));

        ASSERT_VX_OBJECT(y12 = vxCreateImage(context, width, height, VX_DF_IMAGE_U16), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(uv12_c1 = vxCreateImage(context, width, height/2, VX_DF_IMAGE_U16), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(y8_r8_c2 = vxCreateImage(context, width, height, VX_DF_IMAGE_U8), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(uv8_g8_c3 = vxCreateImage(context, width, height/2, VX_DF_IMAGE_U8), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(s8_b8_c4 = vxCreateImage(context, width, height, VX_DF_IMAGE_U8), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(histogram = vxCreateDistribution(context, 256, 0, 256), VX_TYPE_DISTRIBUTION);

        /* Create/Configure configuration input structure */
        params_type = vxRegisterUserStruct(context, sizeof(tivx_vpac_viss_params_t));
        ASSERT(params_type >= VX_TYPE_USER_STRUCT_START && params_type <= VX_TYPE_USER_STRUCT_END);
        ASSERT_VX_OBJECT(configuration = vxCreateArray(context, params_type, 1), VX_TYPE_ARRAY);

        memset(&params, 0, sizeof(tivx_vpac_viss_params_t));

        snprintf(params.sensor_name, 256, "%s/viss_config/example_sensor/0/0", ct_get_test_file_path());

        params.mux_ee_port = 1;
        params.mux_uv12_c1_out = 0;
        params.mux_y8_r8_c2_out = 0;
        params.mux_uv8_g8_c3_out = 0;
        params.mux_s8_b8_c4_out = 0;
        params.mux_h3a_out = 0;
        params.chroma_out_mode = 0;
        params.bypass_glbce = 0;
        params.bypass_nsf4 = 0;

        VX_CALL(vxAddArrayItems(configuration, 1, &params, sizeof(tivx_vpac_viss_params_t)));

        /* Create/Configure ae_awb_result input structure */
        params_type = vxRegisterUserStruct(context, sizeof(tivx_ae_awb_params_t));
        ASSERT(params_type >= VX_TYPE_USER_STRUCT_START && params_type <= VX_TYPE_USER_STRUCT_END);
        ASSERT_VX_OBJECT(ae_awb_result = vxCreateArray(context, params_type, 1), VX_TYPE_ARRAY);

        memset(&ae_awb_params, 0, sizeof(tivx_ae_awb_params_t));
        VX_CALL(vxAddArrayItems(ae_awb_result, 1, &ae_awb_params, sizeof(tivx_ae_awb_params_t)));

        /* Create h3a_aew_af output buffer (uninitialized) */
        h3a_output_size = 9516; /* Offline calculation, may provide size calculator function later */
        params_type = vxRegisterUserStruct(context, h3a_output_size);
        ASSERT(params_type >= VX_TYPE_USER_STRUCT_START && params_type <= VX_TYPE_USER_STRUCT_END);
        ASSERT_VX_OBJECT(h3a_aew_af = vxCreateArray(context, params_type, 1), VX_TYPE_ARRAY);


        ASSERT_VX_OBJECT(graph = vxCreateGraph(context), VX_TYPE_GRAPH);

        ASSERT_VX_OBJECT(node = tivxVpacVissNode(graph, configuration, ae_awb_result,
                                                raw0, raw1, raw2, y12, uv12_c1, y8_r8_c2, uv8_g8_c3, s8_b8_c4,
                                                histogram, h3a_aew_af), VX_TYPE_NODE);

        VX_CALL(vxVerifyGraph(graph));

        ct_read_image2(raw0, "bayer_1280x720.raw", 2);

        VX_CALL(vxProcessGraph(graph));

        /* Check output */

#if CHECK_OUTPUT

        ASSERT_VX_OBJECT(y12_ref = vxCreateImage(context, width, height, VX_DF_IMAGE_U16), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(uv12_c1_ref = vxCreateImage(context, width, height/2, VX_DF_IMAGE_U16), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(y8_r8_c2_ref = vxCreateImage(context, width, height, VX_DF_IMAGE_U8), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(uv8_g8_c3_ref = vxCreateImage(context, width, height/2, VX_DF_IMAGE_U8), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(s8_b8_c4_ref = vxCreateImage(context, width, height, VX_DF_IMAGE_U8), VX_TYPE_IMAGE);
        ASSERT_VX_OBJECT(histogram_ref = vxCreateDistribution(context, 256, 0, 256), VX_TYPE_DISTRIBUTION);
        ASSERT_VX_OBJECT(h3a_aew_af_ref = vxCreateArray(context, params_type, 1), VX_TYPE_ARRAY);

        ct_read_image2(y12_ref, "out_y12.raw", 2);
        ct_read_image2(uv12_c1_ref, "out_uv12.raw", 2);
        ct_read_image2(y8_r8_c2_ref, "out_y8_ee.raw", 2);
        ct_read_image2(uv8_g8_c3_ref, "out_uv8.raw", 2);
        ct_read_image2(s8_b8_c4_ref, "out_s8.raw", 2);
        ct_read_hist(histogram_ref, "out_hist.raw", 4);
        ct_read_array2(h3a_aew_af_ref, "out_h3a.raw", 4);

        ASSERT(ct_cmp_image2(y12, y12_ref) == 0);
        ASSERT(ct_cmp_image2(uv12_c1, uv12_c1_ref) == 0);
        ASSERT(ct_cmp_image2(y8_r8_c2, y8_r8_c2_ref) == 0);
        ASSERT(ct_cmp_image2(uv8_g8_c3, uv8_g8_c3_ref) == 0);
        ASSERT(ct_cmp_image2(s8_b8_c4, s8_b8_c4_ref) == 0);
        ASSERT(ct_cmp_hist(histogram, histogram_ref) == 0);

        {
            uint32_t *data_ptr;
            tivx_h3a_data_t *h3a_out;
            uint8_t *ptr, *ref_ptr;
            int32_t error = 0;
            vx_map_id map_id, map_id_ref;
            int32_t i;
            vx_size item_size;

            vxMapArrayRange(h3a_aew_af, 0, 1, &map_id, &item_size,
                (void *)&data_ptr,
                VX_READ_ONLY,
                VX_MEMORY_TYPE_HOST, 0);
            h3a_out = (tivx_h3a_data_t*)data_ptr;
            ptr = (uint8_t *)&h3a_out->data;

            vxMapArrayRange(h3a_aew_af_ref, 0, 1, &map_id_ref, &item_size,
                (void *)&ref_ptr,
                VX_READ_ONLY,
                VX_MEMORY_TYPE_HOST, 0);

            for(i=0; i<h3a_out->size; i++)
            {
                if(ptr[i] != ref_ptr[i])
                {
                    error++;
                }
            }
            vxUnmapArrayRange(h3a_aew_af, map_id);
            vxUnmapArrayRange(h3a_aew_af_ref, map_id_ref);
            ASSERT(error == 0);
        }

        VX_CALL(vxReleaseArray(&h3a_aew_af_ref));
        VX_CALL(vxReleaseDistribution(&histogram_ref));
        VX_CALL(vxReleaseImage(&s8_b8_c4_ref));
        VX_CALL(vxReleaseImage(&uv8_g8_c3_ref));
        VX_CALL(vxReleaseImage(&y8_r8_c2_ref));
        VX_CALL(vxReleaseImage(&uv12_c1_ref));
        VX_CALL(vxReleaseImage(&y12_ref));
#endif
        /* For visual verification */
        save_image_from_viss(y8_r8_c2, "out_y8");

        VX_CALL(vxReleaseNode(&node));
        VX_CALL(vxReleaseGraph(&graph));
        VX_CALL(vxReleaseArray(&h3a_aew_af));
        VX_CALL(vxReleaseDistribution(&histogram));
        VX_CALL(vxReleaseImage(&s8_b8_c4));
        VX_CALL(vxReleaseImage(&uv8_g8_c3));
        VX_CALL(vxReleaseImage(&y8_r8_c2));
        VX_CALL(vxReleaseImage(&uv12_c1));
        VX_CALL(vxReleaseImage(&y12));
        //VX_CALL(vxReleaseImage(&raw2));
        //VX_CALL(vxReleaseImage(&raw1));
        VX_CALL(vxReleaseImage(&raw0));
        VX_CALL(vxReleaseArray(&ae_awb_result));
        VX_CALL(vxReleaseArray(&configuration));

        ASSERT(node == 0);
        ASSERT(graph == 0);
        ASSERT(h3a_aew_af == 0);
        ASSERT(histogram == 0);
        ASSERT(s8_b8_c4 == 0);
        ASSERT(uv8_g8_c3 == 0);
        ASSERT(y8_r8_c2 == 0);
        ASSERT(uv12_c1 == 0);
        ASSERT(y12 == 0);
        ASSERT(raw2 == 0);
        ASSERT(raw1 == 0);
        ASSERT(raw0 == 0);
        ASSERT(ae_awb_result == 0);
        ASSERT(configuration == 0);

        tivxHwaUnLoadKernels(context);
    }
}


TESTCASE_TESTS(tivxHwaVpacViss, testNodeCreation, testGraphProcessing, testGraphProcessingRaw)
