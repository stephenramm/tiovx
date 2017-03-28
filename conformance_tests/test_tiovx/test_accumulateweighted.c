/*
 * Copyright (c) 2012-2016 The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * MODIFICATIONS TO THIS FILE MAY MEAN IT NO LONGER ACCURATELY REFLECTS
 * KHRONOS STANDARDS. THE UNMODIFIED, NORMATIVE VERSIONS OF KHRONOS
 * SPECIFICATIONS AND HEADER INFORMATION ARE LOCATED AT
 *    https://www.khronos.org/registry/
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 */

/*
 *******************************************************************************
 *
 * Copyright (C) 2017 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */


#include "test_tiovx.h"
#include <VX/vx.h>
#include <VX/vxu.h>

TESTCASE(tivxAccumulateWeighted, CT_VXContext, ct_setup_vx_context, 0)

static void referenceNot(CT_Image src, CT_Image dst)
{
    uint32_t i, j;

    ASSERT(src && dst);
    ASSERT(src->width == dst->width);
    ASSERT(src->height == dst->height);
    ASSERT(src->format == dst->format && src->format == VX_DF_IMAGE_U8);

    for (i = 0; i < dst->height; ++i)
        for (j = 0; j < dst->width; ++j)
            dst->data.y[i * dst->stride + j] = ~src->data.y[i * src->stride + j];
}

static CT_Image accumulate_weighted_generate_random_8u(int width, int height)
{
    CT_Image image;

    ASSERT_NO_FAILURE_(return 0,
            image = ct_allocate_ct_image_random(width, height, VX_DF_IMAGE_U8, &CT()->seed_, 0, 256));

    return image;
}


static void accumulate_weighted_reference(CT_Image input, vx_float32 alpha, CT_Image accum)
{
    CT_FILL_IMAGE_8U(return, accum,
            {
                uint8_t* input_data = CT_IMAGE_DATA_PTR_8U(input, x, y);
                vx_float32 res = (1 - alpha) * ((vx_float32)(int32_t)(*dst_data)) + (alpha) * ((vx_float32)(int32_t)(*input_data));
                uint8_t res8 = CT_SATURATE_U8(res);
                *dst_data = res8;
            });
}

static void accumulate_weighted_check(CT_Image input, vx_float32 alpha, CT_Image accum_src, CT_Image accum_dst)
{
    CT_Image accum_ref = NULL;

    ASSERT(input && accum_src && accum_dst);

    ASSERT_NO_FAILURE(accum_ref = ct_image_create_clone(accum_src));

    ASSERT_NO_FAILURE(accumulate_weighted_reference(input, alpha, accum_ref));

    EXPECT_CTIMAGE_NEAR(accum_ref, accum_dst, 1);
}

static void accumulate_multiple_weighted_check(CT_Image input, vx_float32 alpha_intermediate, vx_float32 alpha_final, CT_Image accum_src, CT_Image accum_final, CT_Image accum_dst)
{
    CT_Image accum_ref = NULL, accum_intermediate = NULL;

    ASSERT(input && accum_src && accum_final && accum_dst);

    ASSERT_NO_FAILURE(accum_intermediate = ct_image_create_clone(accum_src));

    ASSERT_NO_FAILURE(accum_ref = ct_image_create_clone(accum_final));

    ASSERT_NO_FAILURE(accumulate_weighted_reference(input, alpha_intermediate, accum_intermediate));

    ASSERT_NO_FAILURE(accumulate_weighted_reference(accum_intermediate, alpha_final, accum_ref));

    EXPECT_CTIMAGE_NEAR(accum_ref, accum_dst, 2); // Changed tolerance to 2 for the amount of nodes
}

static void accumulate_not_multiple_weighted_check(CT_Image input_not, CT_Image input_acc, CT_Image virtual_dummy,
            vx_float32 alpha_intermediate, vx_float32 alpha_final, CT_Image accum_final, CT_Image accum_dst)
{
    CT_Image accum_ref = NULL, accum_intermediate = NULL;

    ASSERT(input_not && input_acc && accum_final && accum_dst);

    ASSERT_NO_FAILURE(accum_ref = ct_image_create_clone(accum_final));

    ASSERT_NO_FAILURE(accum_intermediate = ct_image_create_clone(input_acc));

    ASSERT_NO_FAILURE(referenceNot(input_not, virtual_dummy));

    ASSERT_NO_FAILURE(accumulate_weighted_reference(virtual_dummy, alpha_intermediate, accum_intermediate));

    ASSERT_NO_FAILURE(accumulate_weighted_reference(accum_intermediate, alpha_final, accum_ref));

    EXPECT_CTIMAGE_NEAR(accum_ref, accum_dst, 2); // Changed tolerance to 2
}

static void alternate_node_check(CT_Image input_not, CT_Image input_acc_1, CT_Image input_acc_2, CT_Image virtual_dummy_1,
            CT_Image virtual_dummy_2, CT_Image virtual_dummy_3, vx_float32 alpha_intermediate, vx_float32 alpha_final,
            CT_Image accum_final, CT_Image accum_dst)
{
    CT_Image accum_ref = NULL, accum_intermediate_1 = NULL, accum_intermediate_2 = NULL;

    ASSERT(input_not && input_acc_1 && input_acc_2 && accum_final && accum_dst);

    ASSERT_NO_FAILURE(accum_ref = ct_image_create_clone(accum_final));

    ASSERT_NO_FAILURE(accum_intermediate_1 = ct_image_create_clone(input_acc_1));

    ASSERT_NO_FAILURE(accum_intermediate_2 = ct_image_create_clone(input_acc_2));

    ASSERT_NO_FAILURE(referenceNot(input_not, virtual_dummy_1));

    ASSERT_NO_FAILURE(accumulate_weighted_reference(virtual_dummy_1, alpha_intermediate, accum_intermediate_1));

    ASSERT_NO_FAILURE(referenceNot(accum_intermediate_1, virtual_dummy_2));

    ASSERT_NO_FAILURE(accumulate_weighted_reference(virtual_dummy_2, alpha_intermediate, accum_intermediate_2));

    ASSERT_NO_FAILURE(referenceNot(accum_intermediate_2, virtual_dummy_3));

    ASSERT_NO_FAILURE(accumulate_weighted_reference(virtual_dummy_3, alpha_final, accum_ref));

    EXPECT_CTIMAGE_NEAR(accum_ref, accum_dst, 2); // Changed tolerance to 2
}

typedef struct {
    const char* testName;
    vx_float32 alpha_intermediate, alpha_final;
    int width, height;
} Arg;


#define PARAMETERS \
    CT_GENERATE_PARAMETERS("random/alphaintermediate0.5f/alphafinal0.25f", ADD_SIZE_18x18, ARG, 0.5f, 0.25f), \
    CT_GENERATE_PARAMETERS("random/alpha0.33f/alphafinal0.67f", ADD_SIZE_644x258, ARG, 0.33f, 0.67f), \
    CT_GENERATE_PARAMETERS("random/alpha0.99f/alphafinal0.8f", ADD_SIZE_1600x1200, ARG, 0.99f, 0.8f)

TEST_WITH_ARG(tivxAccumulateWeighted, testGraphProcessing, Arg,
    PARAMETERS
)
{
    vx_context context = context_->vx_context_;
    vx_image input_image = 0, accum_image_intermediate = 0, accum_image_final = 0;
    vx_scalar alpha_scalar, alpha_scalar_final = 0;
    vx_graph graph = 0;
    vx_node node1 = 0, node2 = 0;
    vx_perf_t perf_node1, perf_node2, perf_graph;

    CT_Image input = NULL, accum_src = NULL, accum_final = NULL, accum_dst = NULL;

    VX_CALL(vxDirective((vx_reference)context, VX_DIRECTIVE_ENABLE_PERFORMANCE));

    ASSERT_VX_OBJECT(alpha_scalar = vxCreateScalar(context, VX_TYPE_FLOAT32, &arg_->alpha_intermediate), VX_TYPE_SCALAR);

    ASSERT_VX_OBJECT(alpha_scalar_final = vxCreateScalar(context, VX_TYPE_FLOAT32, &arg_->alpha_final), VX_TYPE_SCALAR);

    ASSERT_NO_FAILURE(input = accumulate_weighted_generate_random_8u(arg_->width, arg_->height));

    ASSERT_NO_FAILURE(accum_src = accumulate_weighted_generate_random_8u(arg_->width, arg_->height));

    ASSERT_NO_FAILURE(accum_final = accumulate_weighted_generate_random_8u(arg_->width, arg_->height));

    ASSERT_VX_OBJECT(input_image = ct_image_to_vx_image(input, context), VX_TYPE_IMAGE);

    ASSERT_VX_OBJECT(accum_image_intermediate = ct_image_to_vx_image(accum_src, context), VX_TYPE_IMAGE);

    ASSERT_VX_OBJECT(accum_image_final = ct_image_to_vx_image(accum_final, context), VX_TYPE_IMAGE);

    ASSERT_VX_OBJECT(graph = vxCreateGraph(context), VX_TYPE_GRAPH);

    ASSERT_VX_OBJECT(node1 = vxAccumulateWeightedImageNode(graph, input_image, alpha_scalar, accum_image_intermediate), VX_TYPE_NODE);

    ASSERT_VX_OBJECT(node2 = vxAccumulateWeightedImageNode(graph, accum_image_intermediate, alpha_scalar_final, accum_image_final), VX_TYPE_NODE);

    VX_CALL(vxSetNodeTarget(node1, VX_TARGET_STRING, "DSP-2"));

    VX_CALL(vxSetNodeTarget(node2, VX_TARGET_STRING, "DSP-1"));

    VX_CALL(vxVerifyGraph(graph));
    VX_CALL(vxProcessGraph(graph));

    vxQueryNode(node1, VX_NODE_PERFORMANCE, &perf_node1, sizeof(perf_node1));
    vxQueryNode(node2, VX_NODE_PERFORMANCE, &perf_node2, sizeof(perf_node2));
    vxQueryGraph(graph, VX_GRAPH_PERFORMANCE, &perf_graph, sizeof(perf_graph));

    ASSERT_NO_FAILURE(accum_dst = ct_image_from_vx_image(accum_image_final));

    ASSERT_NO_FAILURE(accumulate_multiple_weighted_check(input, arg_->alpha_intermediate, arg_->alpha_final, accum_src, accum_final, accum_dst));

    VX_CALL(vxReleaseNode(&node2));
    VX_CALL(vxReleaseNode(&node1));
    VX_CALL(vxReleaseGraph(&graph));

    ASSERT(node2 == 0);
    ASSERT(node1 == 0);
    ASSERT(graph == 0);

    VX_CALL(vxReleaseImage(&accum_image_final));
    VX_CALL(vxReleaseImage(&accum_image_intermediate));
    VX_CALL(vxReleaseImage(&input_image));
    VX_CALL(vxReleaseScalar(&alpha_scalar_final));
    VX_CALL(vxReleaseScalar(&alpha_scalar));

    ASSERT(accum_image_final == 0);
    ASSERT(accum_image_intermediate == 0);
    ASSERT(input_image == 0);

    printPerformance(perf_node1, arg_->width*arg_->height, "N1");
    printPerformance(perf_node2, arg_->width*arg_->height, "N2");
    printPerformance(perf_graph, arg_->width*arg_->height, "G1");
}

TESTCASE_TESTS(tivxAccumulateWeighted,
        testGraphProcessing
)
