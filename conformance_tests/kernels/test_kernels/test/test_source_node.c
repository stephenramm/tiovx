/*

 * Copyright (c) 2012-2017 The Khronos Group Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "test_engine/test.h"
#include "test_tiovx/test_tiovx.h"

#include <VX/vx.h>
#include <VX/vxu.h>
#include <VX/vx_khr_pipelining.h>
#include <TI/tivx_test_kernels.h>
#include <TI/tivx_config.h>
#include <TI/tivx_capture.h>
#include <TI/tivx_task.h>
#include "math.h"
#include <limits.h>

#define MAX_LINE_LEN   (256U)
#define NUM_CAMERAS    (4U)

#define LOG_RT_TRACE_ENABLE       (0u)

#define MAX_NUM_BUF               (8u)
#define MAX_IMAGE_PLANES          (3u)
#define MAX_NUM_OBJ_ARR_ELEMENTS  (4u)


TESTCASE(tivxSourceNode,  CT_VXContext, ct_setup_vx_context, 0)

typedef struct {
    const char* testName;
    int width, height;
    int pipe_depth;
    int num_buf;
    int loop_count;
    int measure_perf;
} Pipeline_Arg;

#define ADD_BUF_1(testArgName, nextmacro, ...) \
    CT_EXPAND(nextmacro(testArgName "/buf=1", __VA_ARGS__, 1))

#define ADD_BUF_2(testArgName, nextmacro, ...) \
    CT_EXPAND(nextmacro(testArgName "/buf=2", __VA_ARGS__, 2))

#define ADD_BUF_3(testArgName, nextmacro, ...) \
    CT_EXPAND(nextmacro(testArgName "/buf=3", __VA_ARGS__, 3))

#define ADD_PIPE_1(testArgName, nextmacro, ...) \
    CT_EXPAND(nextmacro(testArgName "/pipe_depth=1", __VA_ARGS__, 1))

#define ADD_PIPE_3(testArgName, nextmacro, ...) \
    CT_EXPAND(nextmacro(testArgName "/pipe_depth=3", __VA_ARGS__, 3))

#define ADD_PIPE_6(testArgName, nextmacro, ...) \
    CT_EXPAND(nextmacro(testArgName "/pipe_depth=6", __VA_ARGS__, 6))

#define ADD_PIPE_MAX(testArgName, nextmacro, ...) \
    CT_EXPAND(nextmacro(testArgName "/pipe_depth=MAX", __VA_ARGS__, TIVX_GRAPH_MAX_PIPELINE_DEPTH-1))

#define ADD_LOOP_0(testArgName, nextmacro, ...) \
    CT_EXPAND(nextmacro(testArgName "/loop_count=0", __VA_ARGS__, 0))

#define ADD_LOOP_1(testArgName, nextmacro, ...) \
    CT_EXPAND(nextmacro(testArgName "/loop_count=1", __VA_ARGS__, 1))

#define ADD_LOOP_10(testArgName, nextmacro, ...) \
    CT_EXPAND(nextmacro(testArgName "/loop_count=10", __VA_ARGS__, 10))

#define ADD_LOOP_1000(testArgName, nextmacro, ...) \
    CT_EXPAND(nextmacro(testArgName "/loop_count=1000", __VA_ARGS__, 1000))

#define ADD_LOOP_100000(testArgName, nextmacro, ...) \
    CT_EXPAND(nextmacro(testArgName "/loop_count=100000", __VA_ARGS__, 100000))

#define ADD_LOOP_1000000(testArgName, nextmacro, ...) \
    CT_EXPAND(nextmacro(testArgName "/loop_count=1000000", __VA_ARGS__, 1000000))

#define MEASURE_PERF_OFF(testArgName, nextmacro, ...) \
    CT_EXPAND(nextmacro(testArgName "/measure_perf=OFF", __VA_ARGS__, 0))

#define MEASURE_PERF_ON(testArgName, nextmacro, ...) \
    CT_EXPAND(nextmacro(testArgName "/measure_perf=ON", __VA_ARGS__, 1))

#define ADD_SIZE_2048x1024(testArgName, nextmacro, ...) \
    CT_EXPAND(nextmacro(testArgName "/sz=2048x1024", __VA_ARGS__, 2048, 1024))

#define PARAMETERS \
    CT_GENERATE_PARAMETERS("random", ADD_SIZE_64x64, ADD_PIPE_3, ADD_BUF_3, ADD_LOOP_0, MEASURE_PERF_OFF, ARG), \
    CT_GENERATE_PARAMETERS("random", ADD_SIZE_64x64, ADD_PIPE_1, ADD_BUF_1, ADD_LOOP_0, MEASURE_PERF_OFF, ARG), \
    CT_GENERATE_PARAMETERS("random", ADD_SIZE_64x64, ADD_PIPE_3, ADD_BUF_3, ADD_LOOP_1, MEASURE_PERF_OFF, ARG), \
    CT_GENERATE_PARAMETERS("random", ADD_SIZE_64x64, ADD_PIPE_6, ADD_BUF_3, ADD_LOOP_1000, MEASURE_PERF_OFF, ARG), \
    CT_GENERATE_PARAMETERS("random", ADD_SIZE_64x64, ADD_PIPE_MAX, ADD_BUF_3, ADD_LOOP_1000, MEASURE_PERF_OFF, ARG), \
    CT_GENERATE_PARAMETERS("random", ADD_SIZE_64x64, ADD_PIPE_1, ADD_BUF_1, ADD_LOOP_1000, MEASURE_PERF_OFF, ARG), \
    CT_GENERATE_PARAMETERS("random", ADD_SIZE_64x64, ADD_PIPE_6, ADD_BUF_2, ADD_LOOP_1000, MEASURE_PERF_OFF, ARG), \
    CT_GENERATE_PARAMETERS("random", ADD_SIZE_64x64, ADD_PIPE_6, ADD_BUF_2, ADD_LOOP_100000, MEASURE_PERF_ON, ARG), \
    CT_GENERATE_PARAMETERS("random", ADD_SIZE_2048x1024, ADD_PIPE_3, ADD_BUF_3, ADD_LOOP_1000, MEASURE_PERF_ON, ARG), \

/*
 * Utility API to set number of buffers at a node parameter
 * The parameter MUST be a output or bidirectonal parameter for the setting
 * to take effect
 */
static vx_status set_num_buf_by_node_index(vx_node node, vx_uint32 node_parameter_index, vx_uint32 num_buf)
{
    return tivxSetNodeParameterNumBufByIndex(node, node_parameter_index, num_buf);
}

/*
 * Utility API to set pipeline depth for a graph
 */
static vx_status set_graph_pipeline_depth(vx_graph graph, vx_uint32 pipeline_depth)
{
    return tivxSetGraphPipelineDepth(graph, pipeline_depth);
}

/*
 * Utility API to set trigger node for a graph
 */
static vx_status set_graph_trigger_node(vx_graph graph, vx_node node)
{
    return tivxEnableGraphStreaming(graph, node);
}

/*
 * Utility API to export graph information to file for debug and visualization
 */
static vx_status export_graph_to_file(vx_graph graph, char *filename_prefix)
{
    return tivxExportGraphToDot(graph, ct_get_test_file_path(), filename_prefix);
}

/*
 * Utility API to log graph run-time trace
 */
static vx_status log_graph_rt_trace(vx_graph graph)
{
    vx_status status = VX_SUCCESS;

    #if LOG_RT_TRACE_ENABLE
    status = tivxLogRtTrace(graph);
    #endif
    return status;
}

typedef struct {
    const char* name;
    int stream_time;
} Arg;

#define STREAMING_PARAMETERS \
    CT_GENERATE_PARAMETERS("streaming", ARG, 100), \
    CT_GENERATE_PARAMETERS("streaming", ARG, 1000), \
    CT_GENERATE_PARAMETERS("streaming", ARG, 10000), \
    //CT_GENERATE_PARAMETERS("streaming", ARG, 100000)

/*
 *       n1         scalar         n2
 * SCALAR_SOURCE2 -- SCALAR -- SCALAR_SINK2
 *
 * Scalar source2 node connected to scalar sink2 node
 * Both nodes on IPU1_0
 * Error will be shown in a print statement if the scalar sink fails
 *
 */
TEST_WITH_ARG(tivxSourceNode, testNewSourceSink, Arg, STREAMING_PARAMETERS)
{
    vx_graph graph;
    vx_context context = context_->vx_context_;
    vx_uint8  scalar_val = 0;
    vx_scalar scalar;
    uint32_t num_streams = 0;
    vx_node n1, n2;


    tivxTestKernelsLoadKernels(context);

    ASSERT_VX_OBJECT(graph = vxCreateGraph(context), VX_TYPE_GRAPH);

    ASSERT_VX_OBJECT(scalar = vxCreateScalar(context, VX_TYPE_UINT8, &scalar_val), VX_TYPE_SCALAR);

    ASSERT_VX_OBJECT(n1 = tivxScalarSource2Node(graph, scalar), VX_TYPE_NODE);

    ASSERT_VX_OBJECT(n2 = tivxScalarSink2Node(graph, scalar), VX_TYPE_NODE);

    VX_CALL(vxSetNodeTarget(n1, VX_TARGET_STRING, TIVX_TARGET_IPU1_0));
    VX_CALL(vxSetNodeTarget(n2, VX_TARGET_STRING, TIVX_TARGET_IPU1_0));

    /* If i remove this, the test case hangs */
    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_pipeline_depth(graph, 3));

    /* If i remove this, I get several more failures */
    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_num_buf_by_node_index(n1, 0, 3));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_trigger_node(graph, n1));

    VX_CALL(vxVerifyGraph(graph));

    VX_CALL(vxStartGraphStreaming(graph));

    tivxTaskWaitMsecs(arg_->stream_time);

    VX_CALL(vxStopGraphStreaming(graph));

    VX_CALL(vxQueryGraph(graph, TIVX_GRAPH_STREAM_EXECUTIONS, &num_streams, sizeof(num_streams)));

    ASSERT(num_streams != 0);

    VX_CALL(vxReleaseScalar(&scalar));
    VX_CALL(vxReleaseNode(&n2));
    VX_CALL(vxReleaseNode(&n1));
    VX_CALL(vxReleaseGraph(&graph));
    tivxTestKernelsUnLoadKernels(context);
}

/*
 * Graph1
 *       n1         scalar         n2
 * SCALAR_SOURCE2 -- SCALAR -- SCALAR_SINK2
 *
 * Graph2
 *       n1         scalar         n2
 * SCALAR_SOURCE2 -- SCALAR -- SCALAR_SINK2
 *
 * Tests multiple graphs with streaming and pipelining enabled
 * Both nodes of graph1 are on DSP1
 * Both nodes of graph2 are on DSP2
 *
 */
TEST_WITH_ARG(tivxSourceNode, testMultiGraphPipelined1, Arg, STREAMING_PARAMETERS)
{
    vx_graph graph1, graph2;
    vx_context context = context_->vx_context_;
    vx_uint8  scalar_val = 0;
    vx_scalar scalar_g1, scalar_g2;
    vx_node n1, n2, n3, n4;
    uint32_t pipeline_depth, num_buf;
    uint32_t num_streams_g1 = 0, num_streams_g2 = 0;

    pipeline_depth = 3;
    num_buf = 3;

    tivxTestKernelsLoadKernels(context);

    ASSERT_VX_OBJECT(graph1 = vxCreateGraph(context), VX_TYPE_GRAPH);

    ASSERT_VX_OBJECT(graph2 = vxCreateGraph(context), VX_TYPE_GRAPH);

    ASSERT_VX_OBJECT(scalar_g1 = vxCreateScalar(context, VX_TYPE_UINT8, &scalar_val), VX_TYPE_SCALAR);

    ASSERT_VX_OBJECT(scalar_g2 = vxCreateScalar(context, VX_TYPE_UINT8, &scalar_val), VX_TYPE_SCALAR);

    ASSERT_VX_OBJECT(n1 = tivxScalarSource2Node(graph1, scalar_g1), VX_TYPE_NODE);

    ASSERT_VX_OBJECT(n2 = tivxScalarSink2Node(graph1, scalar_g1), VX_TYPE_NODE);

    ASSERT_VX_OBJECT(n3 = tivxScalarSource2Node(graph2, scalar_g2), VX_TYPE_NODE);

    ASSERT_VX_OBJECT(n4 = tivxScalarSink2Node(graph2, scalar_g2), VX_TYPE_NODE);

    VX_CALL(vxSetNodeTarget(n1, VX_TARGET_STRING, TIVX_TARGET_DSP1));
    VX_CALL(vxSetNodeTarget(n2, VX_TARGET_STRING, TIVX_TARGET_DSP1));
    VX_CALL(vxSetNodeTarget(n3, VX_TARGET_STRING, TIVX_TARGET_DSP2));
    VX_CALL(vxSetNodeTarget(n4, VX_TARGET_STRING, TIVX_TARGET_DSP2));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_trigger_node(graph1, n1));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_trigger_node(graph2, n3));

    /* explicitly set graph pipeline depth */
    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_pipeline_depth(graph1, pipeline_depth));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_num_buf_by_node_index(n1, 0, num_buf));

    /* explicitly set graph pipeline depth */
    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_pipeline_depth(graph2, pipeline_depth));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_num_buf_by_node_index(n3, 0, num_buf));

    VX_CALL(vxVerifyGraph(graph1));

    export_graph_to_file(graph1, "test_capture_multi_graph1_pipeline_streaming_graph1");
    log_graph_rt_trace(graph1);

    VX_CALL(vxVerifyGraph(graph2));

    export_graph_to_file(graph2, "test_capture_multi_graph1_pipeline_streaming_graph2");
    log_graph_rt_trace(graph2);

    VX_CALL(vxStartGraphStreaming(graph1));

    VX_CALL(vxStartGraphStreaming(graph2));

    tivxTaskWaitMsecs(arg_->stream_time);

    VX_CALL(vxStopGraphStreaming(graph1));

    VX_CALL(vxStopGraphStreaming(graph2));

    /* Beginning of error checking */
    VX_CALL(vxQueryGraph(graph1, TIVX_GRAPH_STREAM_EXECUTIONS, &num_streams_g1, sizeof(num_streams_g1)));

    ASSERT(num_streams_g1 != 0);

    VX_CALL(vxQueryGraph(graph2, TIVX_GRAPH_STREAM_EXECUTIONS, &num_streams_g2, sizeof(num_streams_g2)));

    ASSERT(num_streams_g2 != 0);
    /* End of error checking */

    VX_CALL(vxReleaseScalar(&scalar_g1));
    VX_CALL(vxReleaseScalar(&scalar_g2));
    VX_CALL(vxReleaseNode(&n4));
    VX_CALL(vxReleaseNode(&n3));
    VX_CALL(vxReleaseNode(&n2));
    VX_CALL(vxReleaseNode(&n1));
    VX_CALL(vxReleaseGraph(&graph2));
    VX_CALL(vxReleaseGraph(&graph1));
    tivxTestKernelsUnLoadKernels(context);
}

/*
 * Graph1
 *       n1         scalar         n2
 * SCALAR_SOURCE2 -- SCALAR -- SCALAR_SINK2
 *
 * Graph2
 *       n1         scalar         n2
 * SCALAR_SOURCE2 -- SCALAR -- SCALAR_SINK2
 *
 * Tests multiple graphs with streaming and pipelining enabled
 * Both nodes of graph1 are on DSP1
 * Both nodes of graph2 are on DSP1
 *
 */
TEST_WITH_ARG(tivxSourceNode, testMultiGraphPipelined2, Arg, STREAMING_PARAMETERS)
{
    vx_graph graph1, graph2;
    vx_context context = context_->vx_context_;
    vx_uint8  scalar_val = 0;
    vx_scalar scalar_g1, scalar_g2;
    vx_node n1, n2, n3, n4;
    uint32_t pipeline_depth, num_buf;
    uint32_t num_streams_g1 = 0, num_streams_g2 = 0;

    pipeline_depth = 3;
    num_buf = 3;

    tivxTestKernelsLoadKernels(context);

    ASSERT_VX_OBJECT(graph1 = vxCreateGraph(context), VX_TYPE_GRAPH);

    ASSERT_VX_OBJECT(graph2 = vxCreateGraph(context), VX_TYPE_GRAPH);

    ASSERT_VX_OBJECT(scalar_g1 = vxCreateScalar(context, VX_TYPE_UINT8, &scalar_val), VX_TYPE_SCALAR);

    ASSERT_VX_OBJECT(scalar_g2 = vxCreateScalar(context, VX_TYPE_UINT8, &scalar_val), VX_TYPE_SCALAR);

    ASSERT_VX_OBJECT(n1 = tivxScalarSource2Node(graph1, scalar_g1), VX_TYPE_NODE);

    ASSERT_VX_OBJECT(n2 = tivxScalarSink2Node(graph1, scalar_g1), VX_TYPE_NODE);

    ASSERT_VX_OBJECT(n3 = tivxScalarSource2Node(graph2, scalar_g2), VX_TYPE_NODE);

    ASSERT_VX_OBJECT(n4 = tivxScalarSink2Node(graph2, scalar_g2), VX_TYPE_NODE);

    VX_CALL(vxSetNodeTarget(n1, VX_TARGET_STRING, TIVX_TARGET_DSP1));
    VX_CALL(vxSetNodeTarget(n2, VX_TARGET_STRING, TIVX_TARGET_DSP1));
    VX_CALL(vxSetNodeTarget(n3, VX_TARGET_STRING, TIVX_TARGET_DSP1));
    VX_CALL(vxSetNodeTarget(n4, VX_TARGET_STRING, TIVX_TARGET_DSP1));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_trigger_node(graph1, n1));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_trigger_node(graph2, n3));

    /* explicitly set graph pipeline depth */
    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_pipeline_depth(graph1, pipeline_depth));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_num_buf_by_node_index(n1, 0, num_buf));

    /* explicitly set graph pipeline depth */
    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_pipeline_depth(graph2, pipeline_depth));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_num_buf_by_node_index(n3, 0, num_buf));

    VX_CALL(vxVerifyGraph(graph1));

    export_graph_to_file(graph1, "test_capture_multi_graph2_pipeline_streaming_graph1");
    log_graph_rt_trace(graph1);

    VX_CALL(vxVerifyGraph(graph2));

    export_graph_to_file(graph2, "test_capture_multi_graph2_pipeline_streaming_graph2");
    log_graph_rt_trace(graph2);

    VX_CALL(vxStartGraphStreaming(graph1));

    VX_CALL(vxStartGraphStreaming(graph2));

    tivxTaskWaitMsecs(arg_->stream_time);

    VX_CALL(vxStopGraphStreaming(graph1));

    VX_CALL(vxStopGraphStreaming(graph2));

    /* Beginning of error checking */
    VX_CALL(vxQueryGraph(graph1, TIVX_GRAPH_STREAM_EXECUTIONS, &num_streams_g1, sizeof(num_streams_g1)));

    ASSERT(num_streams_g1 != 0);

    VX_CALL(vxQueryGraph(graph2, TIVX_GRAPH_STREAM_EXECUTIONS, &num_streams_g2, sizeof(num_streams_g2)));

    ASSERT(num_streams_g2 != 0);
    /* End of error checking */

    VX_CALL(vxReleaseScalar(&scalar_g1));
    VX_CALL(vxReleaseScalar(&scalar_g2));
    VX_CALL(vxReleaseNode(&n4));
    VX_CALL(vxReleaseNode(&n3));
    VX_CALL(vxReleaseNode(&n2));
    VX_CALL(vxReleaseNode(&n1));
    VX_CALL(vxReleaseGraph(&graph2));
    VX_CALL(vxReleaseGraph(&graph1));
    tivxTestKernelsUnLoadKernels(context);
}

/*
 * Graph1
 *       n1         scalar         n2
 * SCALAR_SOURCE2 -- SCALAR -- SCALAR_SINK2
 *
 * Graph2
 *       n1         scalar         n2
 * SCALAR_SOURCE2 -- SCALAR -- SCALAR_SINK2
 *
 * Tests multiple graphs with streaming and pipelining enabled
 * Node1 of graph1 is on DSP1 while node2 of graph1 is on DSP2
 * Node1 of graph2 is on DSP1 while node2 of graph2 is on DSP2
 *
 */
TEST_WITH_ARG(tivxSourceNode, testMultiGraphPipelined3, Arg, STREAMING_PARAMETERS)
{
    vx_graph graph1, graph2;
    vx_context context = context_->vx_context_;
    vx_uint8  scalar_val = 0;
    vx_scalar scalar_g1, scalar_g2;
    vx_node n1, n2, n3, n4;
    uint32_t pipeline_depth, num_buf;
    uint32_t num_streams_g1 = 0, num_streams_g2 = 0;

    pipeline_depth = 3;
    num_buf = 3;

    tivxTestKernelsLoadKernels(context);

    ASSERT_VX_OBJECT(graph1 = vxCreateGraph(context), VX_TYPE_GRAPH);

    ASSERT_VX_OBJECT(graph2 = vxCreateGraph(context), VX_TYPE_GRAPH);

    ASSERT_VX_OBJECT(scalar_g1 = vxCreateScalar(context, VX_TYPE_UINT8, &scalar_val), VX_TYPE_SCALAR);

    ASSERT_VX_OBJECT(scalar_g2 = vxCreateScalar(context, VX_TYPE_UINT8, &scalar_val), VX_TYPE_SCALAR);

    ASSERT_VX_OBJECT(n1 = tivxScalarSource2Node(graph1, scalar_g1), VX_TYPE_NODE);

    ASSERT_VX_OBJECT(n2 = tivxScalarSink2Node(graph1, scalar_g1), VX_TYPE_NODE);

    ASSERT_VX_OBJECT(n3 = tivxScalarSource2Node(graph2, scalar_g2), VX_TYPE_NODE);

    ASSERT_VX_OBJECT(n4 = tivxScalarSink2Node(graph2, scalar_g2), VX_TYPE_NODE);

    VX_CALL(vxSetNodeTarget(n1, VX_TARGET_STRING, TIVX_TARGET_DSP1));
    VX_CALL(vxSetNodeTarget(n2, VX_TARGET_STRING, TIVX_TARGET_DSP2));
    VX_CALL(vxSetNodeTarget(n3, VX_TARGET_STRING, TIVX_TARGET_DSP1));
    VX_CALL(vxSetNodeTarget(n4, VX_TARGET_STRING, TIVX_TARGET_DSP2));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_trigger_node(graph1, n1));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_trigger_node(graph2, n3));

    /* explicitly set graph pipeline depth */
    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_pipeline_depth(graph1, pipeline_depth));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_num_buf_by_node_index(n1, 0, num_buf));

    /* explicitly set graph pipeline depth */
    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_pipeline_depth(graph2, pipeline_depth));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_num_buf_by_node_index(n3, 0, num_buf));

    VX_CALL(vxVerifyGraph(graph1));

    export_graph_to_file(graph1, "test_capture_multi_graph3_pipeline_streaming_graph1");
    log_graph_rt_trace(graph1);

    VX_CALL(vxVerifyGraph(graph2));

    export_graph_to_file(graph2, "test_capture_multi_graph3_pipeline_streaming_graph2");
    log_graph_rt_trace(graph2);

    VX_CALL(vxStartGraphStreaming(graph1));

    VX_CALL(vxStartGraphStreaming(graph2));

    tivxTaskWaitMsecs(arg_->stream_time);

    VX_CALL(vxStopGraphStreaming(graph1));

    VX_CALL(vxStopGraphStreaming(graph2));

    /* Beginning of error checking */
    VX_CALL(vxQueryGraph(graph1, TIVX_GRAPH_STREAM_EXECUTIONS, &num_streams_g1, sizeof(num_streams_g1)));

    ASSERT(num_streams_g1 != 0);

    VX_CALL(vxQueryGraph(graph2, TIVX_GRAPH_STREAM_EXECUTIONS, &num_streams_g2, sizeof(num_streams_g2)));

    ASSERT(num_streams_g2 != 0);
    /* End of error checking */

    VX_CALL(vxReleaseScalar(&scalar_g1));
    VX_CALL(vxReleaseScalar(&scalar_g2));
    VX_CALL(vxReleaseNode(&n4));
    VX_CALL(vxReleaseNode(&n3));
    VX_CALL(vxReleaseNode(&n2));
    VX_CALL(vxReleaseNode(&n1));
    VX_CALL(vxReleaseGraph(&graph2));
    VX_CALL(vxReleaseGraph(&graph1));
    tivxTestKernelsUnLoadKernels(context);
}

/*
 * Graph1
 *       n1         scalar         n2
 * SCALAR_SOURCE2 -- SCALAR -- SCALAR_SINK2
 *
 * Graph2
 *       n1         scalar             n2            scalar         n3
 * SCALAR_SOURCE2 -- SCALAR -- SCALAR_INTERMEDIATE -- SCALAR -- SCALAR_SINK2
 *
 * Tests multiple graphs with streaming and pipelining enabled
 * Node1 of graph1 is on DSP1 while node2 of graph1 is on DSP2
 * Node1 of graph2 is on DSP1 while node2 and node3 of graph2 are on DSP2
 *
 */
TEST_WITH_ARG(tivxSourceNode, testMultiGraphPipelined4, Arg, STREAMING_PARAMETERS)
{
    vx_graph graph1, graph2;
    vx_context context = context_->vx_context_;
    vx_uint8  scalar_val = 0;
    vx_scalar scalar_g1, scalar_g2, scalar_out_g2;
    vx_node n1, n2, n3, n4, n5;
    uint32_t pipeline_depth, num_buf;
    uint32_t num_streams_g1 = 0, num_streams_g2 = 0;

    pipeline_depth = 3;
    num_buf = 3;

    tivxTestKernelsLoadKernels(context);

    ASSERT_VX_OBJECT(graph1 = vxCreateGraph(context), VX_TYPE_GRAPH);

    ASSERT_VX_OBJECT(graph2 = vxCreateGraph(context), VX_TYPE_GRAPH);

    ASSERT_VX_OBJECT(scalar_g1 = vxCreateScalar(context, VX_TYPE_UINT8, &scalar_val), VX_TYPE_SCALAR);

    ASSERT_VX_OBJECT(scalar_g2 = vxCreateScalar(context, VX_TYPE_UINT8, &scalar_val), VX_TYPE_SCALAR);

    ASSERT_VX_OBJECT(scalar_out_g2 = vxCreateScalar(context, VX_TYPE_UINT8, &scalar_val), VX_TYPE_SCALAR);

    ASSERT_VX_OBJECT(n1 = tivxScalarSource2Node(graph1, scalar_g1), VX_TYPE_NODE);

    ASSERT_VX_OBJECT(n2 = tivxScalarSink2Node(graph1, scalar_g1), VX_TYPE_NODE);

    ASSERT_VX_OBJECT(n3 = tivxScalarSource2Node(graph2, scalar_g2), VX_TYPE_NODE);

    ASSERT_VX_OBJECT(n4 = tivxScalarIntermediateNode(graph2, scalar_g2, scalar_out_g2), VX_TYPE_NODE);

    ASSERT_VX_OBJECT(n5 = tivxScalarSink2Node(graph2, scalar_out_g2), VX_TYPE_NODE);

    VX_CALL(vxSetNodeTarget(n1, VX_TARGET_STRING, TIVX_TARGET_DSP1));
    VX_CALL(vxSetNodeTarget(n2, VX_TARGET_STRING, TIVX_TARGET_DSP2));
    VX_CALL(vxSetNodeTarget(n3, VX_TARGET_STRING, TIVX_TARGET_DSP1));
    VX_CALL(vxSetNodeTarget(n4, VX_TARGET_STRING, TIVX_TARGET_DSP2));
    VX_CALL(vxSetNodeTarget(n5, VX_TARGET_STRING, TIVX_TARGET_DSP2));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_trigger_node(graph1, n1));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_trigger_node(graph2, n3));

    /* explicitly set graph pipeline depth */
    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_pipeline_depth(graph1, pipeline_depth));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_num_buf_by_node_index(n1, 0, num_buf));

    /* explicitly set graph pipeline depth */
    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_pipeline_depth(graph2, pipeline_depth));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_num_buf_by_node_index(n3, 0, num_buf));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_num_buf_by_node_index(n4, 1, 2));

    VX_CALL(vxVerifyGraph(graph1));

    export_graph_to_file(graph1, "test_capture_multi_graph4_pipeline_streaming_graph1");
    log_graph_rt_trace(graph1);

    VX_CALL(vxVerifyGraph(graph2));

    export_graph_to_file(graph2, "test_capture_multi_graph4_pipeline_streaming_graph2");
    log_graph_rt_trace(graph2);

    VX_CALL(vxStartGraphStreaming(graph1));

    VX_CALL(vxStartGraphStreaming(graph2));

    tivxTaskWaitMsecs(arg_->stream_time);

    VX_CALL(vxStopGraphStreaming(graph1));

    VX_CALL(vxStopGraphStreaming(graph2));

    /* Beginning of error checking */
    VX_CALL(vxQueryGraph(graph1, TIVX_GRAPH_STREAM_EXECUTIONS, &num_streams_g1, sizeof(num_streams_g1)));

    ASSERT(num_streams_g1 != 0);

    VX_CALL(vxQueryGraph(graph2, TIVX_GRAPH_STREAM_EXECUTIONS, &num_streams_g2, sizeof(num_streams_g2)));

    ASSERT(num_streams_g2 != 0);
    /* End of error checking */

    VX_CALL(vxReleaseScalar(&scalar_g1));
    VX_CALL(vxReleaseScalar(&scalar_g2));
    VX_CALL(vxReleaseScalar(&scalar_out_g2));
    VX_CALL(vxReleaseNode(&n5));
    VX_CALL(vxReleaseNode(&n4));
    VX_CALL(vxReleaseNode(&n3));
    VX_CALL(vxReleaseNode(&n2));
    VX_CALL(vxReleaseNode(&n1));
    VX_CALL(vxReleaseGraph(&graph2));
    VX_CALL(vxReleaseGraph(&graph1));
    tivxTestKernelsUnLoadKernels(context);
}

/*
 *       n1         scalar         n2
 * SCALAR_SOURCE2 -- SCALAR -- SCALAR_SINK2
 *
 * Scalar source node connected to scalar sink node with streaming and pipelining enabled
 * Both nodes on DSP1
 * Error will be shown in a print statement if the scalar sink fails
 *
 */
TEST_WITH_ARG(tivxSourceNode, testPipeliningStreaming1, Arg, STREAMING_PARAMETERS)
{
    vx_context context = context_->vx_context_;
    vx_graph graph;
    vx_node n0, n1;

    uint32_t pipeline_depth, num_buf;
    uint64_t exe_time;
    uint32_t num_streams = 0;
    vx_uint8  scalar_val = 0;
    vx_scalar scalar;

    tivxTestKernelsLoadKernels(context);

    tivx_clr_debug_zone(VX_ZONE_INFO);

    pipeline_depth = 3;
    num_buf = 3;

    ASSERT_VX_OBJECT(graph = vxCreateGraph(context), VX_TYPE_GRAPH);

    ASSERT_VX_OBJECT(scalar = vxCreateScalar(context, VX_TYPE_UINT8, &scalar_val), VX_TYPE_SCALAR);

    ASSERT_VX_OBJECT(n0 = tivxScalarSource2Node(graph, scalar), VX_TYPE_NODE);

    ASSERT_VX_OBJECT(n1 = tivxScalarSink2Node(graph, scalar), VX_TYPE_NODE);

    VX_CALL(vxSetNodeTarget(n0, VX_TARGET_STRING, TIVX_TARGET_DSP1));
    VX_CALL(vxSetNodeTarget(n1, VX_TARGET_STRING, TIVX_TARGET_DSP1));

    /* explicitly set graph pipeline depth */
    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_pipeline_depth(graph, pipeline_depth));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_num_buf_by_node_index(n0, 0, num_buf));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_trigger_node(graph, n0));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, vxVerifyGraph(graph));

    export_graph_to_file(graph, "test_capture_pipeline_streaming1");
    log_graph_rt_trace(graph);

    exe_time = tivxPlatformGetTimeInUsecs();

    VX_CALL(vxStartGraphStreaming(graph));

    tivxTaskWaitMsecs(arg_->stream_time);

    VX_CALL(vxStopGraphStreaming(graph));

    VX_CALL(vxQueryGraph(graph, TIVX_GRAPH_STREAM_EXECUTIONS, &num_streams, sizeof(num_streams)));

    ASSERT(num_streams != 0);

    exe_time = tivxPlatformGetTimeInUsecs() - exe_time;

    VX_CALL(vxReleaseNode(&n0));
    VX_CALL(vxReleaseNode(&n1));
    VX_CALL(vxReleaseScalar(&scalar));
    VX_CALL(vxReleaseGraph(&graph));
    tivxTestKernelsUnLoadKernels(context);

    tivx_clr_debug_zone(VX_ZONE_INFO);
}

/*
 *       n0         scalar         n1
 * SCALAR_SOURCE2 -- SCALAR -- SCALAR_SINK2
 *
 * Scalar source node connected to scalar sink node with streaming and pipelining enabled
 * N0 is on DSP1 while N1 is on DSP2
 * Error will be shown in a print statement if the scalar sink fails
 *
 */
TEST_WITH_ARG(tivxSourceNode, testPipeliningStreaming2, Arg, STREAMING_PARAMETERS)
{
    vx_context context = context_->vx_context_;
    vx_graph graph;
    vx_node n0, n1;

    uint32_t pipeline_depth, num_buf;
    uint64_t exe_time;
    uint32_t num_streams = 0;
    vx_uint8  scalar_val = 0;
    vx_scalar scalar;

    tivxTestKernelsLoadKernels(context);

    tivx_clr_debug_zone(VX_ZONE_INFO);

    pipeline_depth = 3;
    num_buf = 3;

    ASSERT_VX_OBJECT(graph = vxCreateGraph(context), VX_TYPE_GRAPH);

    ASSERT_VX_OBJECT(scalar = vxCreateScalar(context, VX_TYPE_UINT8, &scalar_val), VX_TYPE_SCALAR);

    ASSERT_VX_OBJECT(n0 = tivxScalarSource2Node(graph, scalar), VX_TYPE_NODE);

    ASSERT_VX_OBJECT(n1 = tivxScalarSink2Node(graph, scalar), VX_TYPE_NODE);

    VX_CALL(vxSetNodeTarget(n0, VX_TARGET_STRING, TIVX_TARGET_DSP1));
    VX_CALL(vxSetNodeTarget(n1, VX_TARGET_STRING, TIVX_TARGET_DSP2));

    /* explicitly set graph pipeline depth */
    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_pipeline_depth(graph, pipeline_depth));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_num_buf_by_node_index(n0, 0, num_buf));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_trigger_node(graph, n0));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, vxVerifyGraph(graph));

    export_graph_to_file(graph, "test_capture_pipeline_streaming2");
    log_graph_rt_trace(graph);

    exe_time = tivxPlatformGetTimeInUsecs();

    VX_CALL(vxStartGraphStreaming(graph));

    tivxTaskWaitMsecs(arg_->stream_time);

    VX_CALL(vxStopGraphStreaming(graph));

    VX_CALL(vxQueryGraph(graph, TIVX_GRAPH_STREAM_EXECUTIONS, &num_streams, sizeof(num_streams)));

    ASSERT(num_streams != 0);

    exe_time = tivxPlatformGetTimeInUsecs() - exe_time;

    VX_CALL(vxReleaseNode(&n0));
    VX_CALL(vxReleaseNode(&n1));
    VX_CALL(vxReleaseScalar(&scalar));
    VX_CALL(vxReleaseGraph(&graph));
    tivxTestKernelsUnLoadKernels(context);

    tivx_clr_debug_zone(VX_ZONE_INFO);
}

/*
 *       n0         scalar             n1            scalar         n2
 * SCALAR_SOURCE2 -- SCALAR -- SCALAR_INTERMEDIATE -- SCALAR -- SCALAR_SINK2
 *
 * Scalar source node connected to scalar sink node with streaming and pipelining enabled
 * All nodes are on DSP1
 * Error will be shown in a print statement if the scalar sink fails
 *
 */
TEST_WITH_ARG(tivxSourceNode, testPipeliningStreaming3, Pipeline_Arg, PARAMETERS)
{
    vx_context context = context_->vx_context_;
    vx_graph graph;
    vx_node n0, n1, n2, n3;

    uint32_t pipeline_depth, num_buf;
    uint64_t exe_time;
    uint32_t num_streams = 0;
    vx_uint8  scalar_val = 0;
    vx_scalar scalar, scalar_int, scalar_out;

    tivxTestKernelsLoadKernels(context);

    tivx_clr_debug_zone(VX_ZONE_INFO);

    pipeline_depth = 3;
    num_buf = 3;

    ASSERT_VX_OBJECT(graph = vxCreateGraph(context), VX_TYPE_GRAPH);

    ASSERT_VX_OBJECT(scalar = vxCreateScalar(context, VX_TYPE_UINT8, &scalar_val), VX_TYPE_SCALAR);

    ASSERT_VX_OBJECT(scalar_int = vxCreateScalar(context, VX_TYPE_UINT8, &scalar_val), VX_TYPE_SCALAR);

    ASSERT_VX_OBJECT(scalar_out = vxCreateScalar(context, VX_TYPE_UINT8, &scalar_val), VX_TYPE_SCALAR);

    ASSERT_VX_OBJECT(n0 = tivxScalarSource2Node(graph, scalar), VX_TYPE_NODE);

    ASSERT_VX_OBJECT(n1 = tivxScalarIntermediateNode(graph, scalar, scalar_int), VX_TYPE_NODE);

    ASSERT_VX_OBJECT(n2 = tivxScalarIntermediateNode(graph, scalar_int, scalar_out), VX_TYPE_NODE);

    ASSERT_VX_OBJECT(n3 = tivxScalarSink2Node(graph, scalar_out), VX_TYPE_NODE);

    VX_CALL(vxSetNodeTarget(n0, VX_TARGET_STRING, TIVX_TARGET_DSP1));
    VX_CALL(vxSetNodeTarget(n1, VX_TARGET_STRING, TIVX_TARGET_DSP1));
    VX_CALL(vxSetNodeTarget(n2, VX_TARGET_STRING, TIVX_TARGET_DSP1));
    VX_CALL(vxSetNodeTarget(n3, VX_TARGET_STRING, TIVX_TARGET_DSP1));

    /* explicitly set graph pipeline depth */
    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_pipeline_depth(graph, pipeline_depth));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_num_buf_by_node_index(n0, 0, num_buf));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_num_buf_by_node_index(n2, 1, 2));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_trigger_node(graph, n0));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, vxVerifyGraph(graph));

    export_graph_to_file(graph, "test_capture_pipeline_streaming3");
    log_graph_rt_trace(graph);

    exe_time = tivxPlatformGetTimeInUsecs();

    VX_CALL(vxStartGraphStreaming(graph));

    tivxTaskWaitMsecs(5000);

    VX_CALL(vxStopGraphStreaming(graph));

    VX_CALL(vxQueryGraph(graph, TIVX_GRAPH_STREAM_EXECUTIONS, &num_streams, sizeof(num_streams)));

    ASSERT(num_streams != 0);

    exe_time = tivxPlatformGetTimeInUsecs() - exe_time;

    VX_CALL(vxReleaseNode(&n0));
    VX_CALL(vxReleaseNode(&n1));
    VX_CALL(vxReleaseNode(&n2));
    VX_CALL(vxReleaseNode(&n3));
    VX_CALL(vxReleaseScalar(&scalar_out));
    VX_CALL(vxReleaseScalar(&scalar_int));
    VX_CALL(vxReleaseScalar(&scalar));
    VX_CALL(vxReleaseGraph(&graph));
    tivxTestKernelsUnLoadKernels(context);

    tivx_clr_debug_zone(VX_ZONE_INFO);
}

/*
 *       n0         scalar             n1            scalar         n2
 * SCALAR_SOURCE2 -- SCALAR -- SCALAR_INTERMEDIATE -- SCALAR -- SCALAR_SINK2
 *
 * Scalar source node connected to scalar sink node with streaming and pipelining enabled
 * Trigger node is intermediate node
 * All nodes are on DSP1
 * Error will be shown in a print statement if the scalar sink fails
 *
 */
TEST_WITH_ARG(tivxSourceNode, testPipeliningStreaming4, Pipeline_Arg, PARAMETERS)
{
    vx_context context = context_->vx_context_;
    vx_graph graph;
    vx_node n0, n1, n2;

    uint32_t pipeline_depth, num_buf;
    uint64_t exe_time;
    uint32_t num_streams = 0;
    vx_uint8  scalar_val = 0;
    vx_scalar scalar, scalar_out;

    tivxTestKernelsLoadKernels(context);

    tivx_clr_debug_zone(VX_ZONE_INFO);

    pipeline_depth = 3;
    num_buf = 3;

    ASSERT_VX_OBJECT(graph = vxCreateGraph(context), VX_TYPE_GRAPH);

    ASSERT_VX_OBJECT(scalar = vxCreateScalar(context, VX_TYPE_UINT8, &scalar_val), VX_TYPE_SCALAR);

    ASSERT_VX_OBJECT(scalar_out = vxCreateScalar(context, VX_TYPE_UINT8, &scalar_val), VX_TYPE_SCALAR);

    ASSERT_VX_OBJECT(n0 = tivxScalarSource2Node(graph, scalar), VX_TYPE_NODE);

    ASSERT_VX_OBJECT(n1 = tivxScalarIntermediateNode(graph, scalar, scalar_out), VX_TYPE_NODE);

    ASSERT_VX_OBJECT(n2 = tivxScalarSink2Node(graph, scalar_out), VX_TYPE_NODE);

    VX_CALL(vxSetNodeTarget(n0, VX_TARGET_STRING, TIVX_TARGET_DSP1));
    VX_CALL(vxSetNodeTarget(n1, VX_TARGET_STRING, TIVX_TARGET_DSP1));
    VX_CALL(vxSetNodeTarget(n2, VX_TARGET_STRING, TIVX_TARGET_DSP1));

    /* explicitly set graph pipeline depth */
    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_pipeline_depth(graph, pipeline_depth));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_num_buf_by_node_index(n0, 0, num_buf));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_num_buf_by_node_index(n1, 1, 2));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_trigger_node(graph, n1));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, vxVerifyGraph(graph));

    export_graph_to_file(graph, "test_capture_pipeline_streaming3");
    log_graph_rt_trace(graph);

    exe_time = tivxPlatformGetTimeInUsecs();

    VX_CALL(vxStartGraphStreaming(graph));

    tivxTaskWaitMsecs(5000);

    VX_CALL(vxStopGraphStreaming(graph));

    VX_CALL(vxQueryGraph(graph, TIVX_GRAPH_STREAM_EXECUTIONS, &num_streams, sizeof(num_streams)));

    ASSERT(num_streams != 0);

    exe_time = tivxPlatformGetTimeInUsecs() - exe_time;

    VX_CALL(vxReleaseNode(&n0));
    VX_CALL(vxReleaseNode(&n1));
    VX_CALL(vxReleaseNode(&n2));
    VX_CALL(vxReleaseScalar(&scalar_out));
    VX_CALL(vxReleaseScalar(&scalar));
    VX_CALL(vxReleaseGraph(&graph));
    tivxTestKernelsUnLoadKernels(context);

    tivx_clr_debug_zone(VX_ZONE_INFO);
}

/*
 *       n0         scalar             n1            scalar         n2
 * SCALAR_SOURCE2 -- SCALAR -- SCALAR_INTERMEDIATE -- SCALAR -- SCALAR_SINK2
 *
 * Scalar source node connected to scalar sink node with streaming and pipelining enabled
 * Trigger node is sink node
 * All nodes are on DSP1
 * Error will be shown in a print statement if the scalar sink fails
 *
 */
TEST_WITH_ARG(tivxSourceNode, testPipeliningStreaming5, Pipeline_Arg, PARAMETERS)
{
    vx_context context = context_->vx_context_;
    vx_graph graph;
    vx_node n0, n1, n2;

    uint32_t pipeline_depth, num_buf;
    uint64_t exe_time;
    uint32_t num_streams = 0;
    vx_uint8  scalar_val = 0;
    vx_scalar scalar, scalar_out;

    tivxTestKernelsLoadKernels(context);

    tivx_clr_debug_zone(VX_ZONE_INFO);

    pipeline_depth = 3;
    num_buf = 3;

    ASSERT_VX_OBJECT(graph = vxCreateGraph(context), VX_TYPE_GRAPH);

    ASSERT_VX_OBJECT(scalar = vxCreateScalar(context, VX_TYPE_UINT8, &scalar_val), VX_TYPE_SCALAR);

    ASSERT_VX_OBJECT(scalar_out = vxCreateScalar(context, VX_TYPE_UINT8, &scalar_val), VX_TYPE_SCALAR);

    ASSERT_VX_OBJECT(n0 = tivxScalarSource2Node(graph, scalar), VX_TYPE_NODE);

    ASSERT_VX_OBJECT(n1 = tivxScalarIntermediateNode(graph, scalar, scalar_out), VX_TYPE_NODE);

    ASSERT_VX_OBJECT(n2 = tivxScalarSink2Node(graph, scalar_out), VX_TYPE_NODE);

    VX_CALL(vxSetNodeTarget(n0, VX_TARGET_STRING, TIVX_TARGET_DSP1));
    VX_CALL(vxSetNodeTarget(n1, VX_TARGET_STRING, TIVX_TARGET_DSP1));
    VX_CALL(vxSetNodeTarget(n2, VX_TARGET_STRING, TIVX_TARGET_DSP1));

    /* explicitly set graph pipeline depth */
    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_pipeline_depth(graph, pipeline_depth));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_num_buf_by_node_index(n0, 0, num_buf));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_num_buf_by_node_index(n1, 1, 2));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, set_graph_trigger_node(graph, n2));

    ASSERT_EQ_VX_STATUS(VX_SUCCESS, vxVerifyGraph(graph));

    export_graph_to_file(graph, "test_capture_pipeline_streaming3");
    log_graph_rt_trace(graph);

    exe_time = tivxPlatformGetTimeInUsecs();

    VX_CALL(vxStartGraphStreaming(graph));

    tivxTaskWaitMsecs(5000);

    VX_CALL(vxStopGraphStreaming(graph));

    VX_CALL(vxQueryGraph(graph, TIVX_GRAPH_STREAM_EXECUTIONS, &num_streams, sizeof(num_streams)));

    ASSERT(num_streams != 0);

    exe_time = tivxPlatformGetTimeInUsecs() - exe_time;

    VX_CALL(vxReleaseNode(&n0));
    VX_CALL(vxReleaseNode(&n1));
    VX_CALL(vxReleaseNode(&n2));
    VX_CALL(vxReleaseScalar(&scalar_out));
    VX_CALL(vxReleaseScalar(&scalar));
    VX_CALL(vxReleaseGraph(&graph));
    tivxTestKernelsUnLoadKernels(context);

    tivx_clr_debug_zone(VX_ZONE_INFO);
}

TESTCASE_TESTS(tivxSourceNode,
               testNewSourceSink,
               testMultiGraphPipelined1,
               testMultiGraphPipelined2,
               testMultiGraphPipelined3,
               testMultiGraphPipelined4,
               testPipeliningStreaming1,
               testPipeliningStreaming2,
               testPipeliningStreaming3,
               testPipeliningStreaming4,
               testPipeliningStreaming5)

