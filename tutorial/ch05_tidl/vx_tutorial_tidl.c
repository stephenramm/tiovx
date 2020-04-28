/*
 *
 * Copyright (c) 2019 Texas Instruments Incorporated
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



/**
 * \file vx_tutorial_tidl.c Executes the inference of a deep learning network.
 *   It first reads the configuration file 'tidl/tidl_infer.cfg' located in directory test_data and that contains the following information:
 *
 * - path to network model's parameter file generated by the import tools
 *
 * - path to network's file generated by the import tools
 *
 * - path to the input file, usually a grayscale or color image
 *
 * - path to the output file, that will contain the output from the last layer. Not used in the current version of the tutorial.
 *
 * - mode of operation (0:classifier or 1:object detection). Only taken into account for formatting the display on the console window.
 *   Currently only 0:classifier is supported.
 *
 * - processing_core_mode: Specify how the network will be processed if multiple processing cores exist in the system.
 *   0 (default): all cores can be utilized according to each layer's groupID. If a layer's group ID is 1 then it will run on EVE1. If it is 2, it will run on DSP1.
 *   1: The entire network will run on EVE1, even the layers which have group ID 2 (DSP layers).
 *   2: The entire network will run on DSP1, even the layers which have group ID 1 (EVE layers).
 *
 *
 *   All paths are relative to the test_data folder
 *
 *   Using the parameters from the configuration file, vx_tutorial_tidl() will then apply the network model on the input data
 *   and display the result on the console window, which consists of the classification top-5 results.
 *
 *   In this tutorial we learn the below concepts:
 * - How to create OpenVX context, OpenVX user data object and OpenVX tensor objects.
 * - How to read a data file and load the values into the user data object
 * - How to read a data file and load the values into a tensor object
 * - How to create OpenVX node and associate it with previously created graph
 * - How to schedule OpenVX graph for execution then execute the graph
 * - How to cleanup all created resources and exit the OpenVX application
 *
 * To include OpenVX interfaces include below file
 * \code
 * #include <VX/vx.h>
 * \endcode
 *
 * Follow the comments in the function vx_tutorial_tidl()
 * to understand this tutorial
 *
 */

#include <TI/tivx.h>
#include <tivx_utils.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <float.h>
#include <math.h>

#include "../../../common/xdais_types.h" /* In TIDL_PATH directory */
#include "sTIDL_IOBufDesc.h"
#include "tivx_tidl_utils.h"

#include "itidl_ti.h"
#include "vx_tutorial_tidl.h"

#define MAX(_a,_b) (((_a) > (_b)) ? (_a) : (_b))

#define CFG_FILE_NAME       "tivx/tidl/tidl_infer.cfg"

#define NUM_EVE_CPU (obj->num_eve_cores)
#define NUM_DSP_CPU 2
#define MAX_NUM_THREADS 4

typedef struct {
  char tidl_params_file_path[VX_TUTORIAL_MAX_FILE_PATH];
  char tidl_network_file_path[VX_TUTORIAL_MAX_FILE_PATH];
  char input_file_path[VX_TUTORIAL_MAX_FILE_PATH];
  char output_file_path[VX_TUTORIAL_MAX_FILE_PATH];
  uint32_t operation_mode;
  uint32_t processing_core_mode;
  uint32_t num_eve_cores;
} VxTutorialTidl_CfgObj;

VxTutorialTidl_CfgObj gCfgObj;

static vx_status parse_cfg_file(VxTutorialTidl_CfgObj *obj, char *cfg_file_name);
static vx_status createInputTensors(vx_context context, vx_user_data_object config, vx_tensor *input_tensors);
static vx_status createOutputTensor(vx_context context, vx_user_data_object config, vx_tensor *output_tensors);
static vx_status readInput(vx_context context, vx_user_data_object config, vx_tensor *input_tensors, char *input_file, uint32_t operation_mode);
static void displayOutput(void *bmp_context, vx_df_image df_image, void *data_ptr, vx_uint32 img_width, vx_uint32 img_height, vx_uint32 img_stride, vx_user_data_object config, vx_tensor *output_tensors, char *output_file, uint32_t operation_mode);

#ifdef HOST_EMULATION
/* This is a workaround to support spanning graphs on different EVE and DSP cores in PC host emulation environment
 * Plan to remove this workaround in the future ...
 *
 * */
tivx_cpu_id_e gTidlNodeCpuId[2*MAX_NUM_THREADS];
#endif

void vx_tutorial_tidl()
{

  vx_context context;
  vx_user_data_object  config1, config2, realConfig;
  vx_user_data_object  network;
  vx_user_data_object createParams1[MAX_NUM_THREADS];
  vx_user_data_object inArgs1[MAX_NUM_THREADS];
  vx_user_data_object outArgs1[MAX_NUM_THREADS];
  vx_user_data_object createParams2[MAX_NUM_THREADS];
  vx_user_data_object inArgs2[MAX_NUM_THREADS];
  vx_user_data_object outArgs2[MAX_NUM_THREADS];
  vx_reference params[5];
  vx_tensor input_tensors[MAX_NUM_THREADS][VX_TUTORIAL_MAX_TENSORS];
  vx_tensor output_tensors1[MAX_NUM_THREADS][VX_TUTORIAL_MAX_TENSORS];
  vx_tensor output_tensors2[MAX_NUM_THREADS][VX_TUTORIAL_MAX_TENSORS];
  vx_tensor *real_output_tensors;
  vx_perf_t perf_graph, perf_node1, perf_node2;
  int32_t i, threadIdx;
  int32_t quantHistoryBoot, quantHistory, quantMargin;
  uint64_t exe_time=0;

  size_t sizeFilePath;
  char filePath[MAXPATHLENGTH];
  const char *basePath;
  const char *targetCore1[MAX_NUM_THREADS];
  const char *targetCore2[MAX_NUM_THREADS];
  vx_enum targetCpuId1[MAX_NUM_THREADS];
  vx_enum targetCpuId2[MAX_NUM_THREADS];

  vx_status status = (vx_status)VX_SUCCESS;

  VxTutorialTidl_CfgObj *obj = &gCfgObj;

  vx_graph graph[MAX_NUM_THREADS] = {0};
  vx_node node1[MAX_NUM_THREADS] = {0};
  vx_node node2[MAX_NUM_THREADS] = {0};
  vx_kernel kernel1 = 0;
  vx_kernel kernel2 = 0;

  uint32_t num_input_tensors  = 0;
  uint32_t num_output_tensors1 = 0;
  uint32_t num_output_tensors2 = 0;

  uint32_t maxNumThreads= 1;

  printf(" vx_tutorial_tidl: Tutorial Started !!! \n");

  context = vxCreateContext();
  VX_TUTORIAL_ASSERT_VALID_REF(context);

  vxDirective((vx_reference)context, (vx_enum)VX_DIRECTIVE_ENABLE_PERFORMANCE);

  basePath = tivx_utils_get_test_file_dir();

  if (basePath == NULL)
  {
      goto exit;
  }

  sizeFilePath = snprintf(filePath, MAXPATHLENGTH, "%s/%s", basePath, CFG_FILE_NAME);

  if (sizeFilePath > MAXPATHLENGTH) {
    printf("Error: path of config gile too long to fit in string\n");
    goto exit;
  }

  printf(" Reading config file %s ...\n", filePath);

  status= parse_cfg_file(obj, filePath);
  if (status!=(vx_status)VX_SUCCESS) {
    goto exit;
  }


  printf(" Reading network file %s ...\n", obj->tidl_network_file_path);

  network = vx_tidl_utils_readNetwork(context, &obj->tidl_network_file_path[0]);
  VX_TUTORIAL_ASSERT_VALID_REF(network)

  /*
   *   Processing_core_mode: Specify how the network will be processed if multiple processing cores exist in the system.
   *   0 (default): all cores can be utilized according to each layer's groupID. If a layer's group ID is 1 then it will run on EVE1. If it is 2, it will run on DSP1.
   *   1: The entire network will run on EVE1, even the layers which have group ID 2 (DSP layers).
   *   2: The entire network will run on DSP1, even the layers which have group ID 1 (EVE layers).
   *
   */
#ifdef HOST_EMULATION
  /* In host emulation on PC, it is not possible to test for processing_core_mode=2
   * but we can test test processing_core_mode=1, which gives a high confidence that
   * processing_core_mode=2 works as well.
   * For a definitive testing of processing_core_mode=2, test on target.
   */
  if (obj->processing_core_mode== 2) {
    obj->processing_core_mode= 1;
  }
#endif

  if (obj->processing_core_mode== 0){

    /*
     * In case the network has only one group of layer, assign the first core and disable the second core
     * In case there are 2 groups of layers. 1st group is always assigned to EVE and second group always assigned to DSP
     * */
    int32_t layersGroupCount[(vx_enum)TIVX_CPU_ID_MAX];
    int32_t numLayersGroup= vx_tidl_utils_countLayersGroup(network, layersGroupCount);

    if (numLayersGroup== 1) {
      if (layersGroupCount[1]!=0) {
        /* If the entire network runs on EVE, spun the processing into as many threads as there are EVEs to demonstrate parallelism between EVEs */
        maxNumThreads= NUM_EVE_CPU;
        targetCore1[0]= TIVX_TARGET_EVE1;targetCore1[1]= TIVX_TARGET_EVE2;targetCore1[2]= TIVX_TARGET_EVE3;targetCore1[3]= TIVX_TARGET_EVE4;
        targetCpuId1[0]= (vx_enum)TIVX_CPU_ID_EVE1;targetCpuId1[1]= (vx_enum)TIVX_CPU_ID_EVE2;targetCpuId1[2]= (vx_enum)TIVX_CPU_ID_EVE3;targetCpuId1[3]= (vx_enum)TIVX_CPU_ID_EVE4;
      }
      else if (layersGroupCount[2]!=0) {
        /* If the entire network runs on DSP, spun the processing into as many threads as there are DSPs to demonstrate parallelism between DSPs */
        maxNumThreads= NUM_DSP_CPU;
        targetCore1[0]= TIVX_TARGET_DSP1;targetCore1[1]= TIVX_TARGET_DSP2;
        targetCpuId1[0]= (vx_enum)TIVX_CPU_ID_DSP1;targetCpuId1[1]= (vx_enum)TIVX_CPU_ID_DSP2;
      }
      else {
        printf(" Invalid layer group ID detected, exiting ...\n");
        goto exit;
      }
      for (threadIdx= 0; threadIdx < maxNumThreads; threadIdx++) {
        targetCore2[threadIdx]= NULL;
        targetCpuId2[threadIdx]= (vx_enum)TIVX_CPU_ID_INVALID;
      }
    }/* In case the network has 2 groups, it will run on two cores */
    else if (numLayersGroup== 2) {
      maxNumThreads= NUM_EVE_CPU;
      targetCore1[0]= TIVX_TARGET_EVE1;targetCore1[1]= TIVX_TARGET_EVE2;targetCore1[2]= TIVX_TARGET_EVE3;targetCore1[3]= TIVX_TARGET_EVE4;
      targetCpuId1[0]= (vx_enum)TIVX_CPU_ID_EVE1;targetCpuId1[1]= (vx_enum)TIVX_CPU_ID_EVE2;targetCpuId1[2]= (vx_enum)TIVX_CPU_ID_EVE3;targetCpuId1[3]= (vx_enum)TIVX_CPU_ID_EVE4;
      targetCore2[0]= TIVX_TARGET_DSP1;targetCore2[1]= TIVX_TARGET_DSP1;targetCore2[2]= TIVX_TARGET_DSP1;targetCore2[3]= TIVX_TARGET_DSP1;
      targetCpuId2[0]= (vx_enum)TIVX_CPU_ID_DSP1;targetCpuId2[1]= (vx_enum)TIVX_CPU_ID_DSP1;targetCpuId2[2]= (vx_enum)TIVX_CPU_ID_DSP1;targetCpuId2[3]= (vx_enum)TIVX_CPU_ID_DSP1;
    }
    else {
      printf(" Invalid number of groups of layers, exiting ...\n");
      goto exit;
    }

  }
  else if (obj->processing_core_mode== 1) {
    maxNumThreads= NUM_EVE_CPU;
    targetCore1[0]= TIVX_TARGET_EVE1;targetCore1[1]= TIVX_TARGET_EVE2;targetCore1[2]= TIVX_TARGET_EVE3;targetCore1[3]= TIVX_TARGET_EVE4;
    targetCpuId1[0]= (vx_enum)TIVX_CPU_ID_EVE1;targetCpuId1[1]= (vx_enum)TIVX_CPU_ID_EVE2;targetCpuId1[2]= (vx_enum)TIVX_CPU_ID_EVE3;targetCpuId1[3]= (vx_enum)TIVX_CPU_ID_EVE4;
    for (threadIdx= 0; threadIdx < maxNumThreads; threadIdx++) {
      targetCore2[threadIdx]= NULL;
      targetCpuId2[threadIdx]= (vx_enum)TIVX_CPU_ID_INVALID;
    }
  }
  else if (obj->processing_core_mode== 2) {
    maxNumThreads= NUM_DSP_CPU;
    targetCore1[0]= TIVX_TARGET_DSP1;targetCore1[1]= TIVX_TARGET_DSP2;
    targetCpuId1[0]= (vx_enum)TIVX_CPU_ID_DSP1;targetCpuId1[1]= (vx_enum)TIVX_CPU_ID_DSP2;
    for (threadIdx= 0; threadIdx < maxNumThreads; threadIdx++) {
      targetCore2[threadIdx]= NULL;
      targetCpuId2[threadIdx]= (vx_enum)TIVX_CPU_ID_INVALID;
    }
  }
  else {
    printf("Invalid processing core mode, exiting ...\n");
    goto exit;
  }

  /* If processing_core_mode is not 0, update each layer's group ID so that the entire network runs either on EVE or DSP*/
  if (obj->processing_core_mode!= 0) {
    vx_tidl_utils_updateLayersGroup(network, targetCpuId1[0]);
  }

  config1 = vx_tidl_utils_getConfig(context, network, &num_input_tensors, &num_output_tensors1, targetCpuId1[0]);

  /* In case the network runs on one CPU, set num_output_tensors2 to 0 */
  if (targetCpuId2[0]== (vx_enum)TIVX_CPU_ID_INVALID) {
    num_output_tensors2= 0;
    config2= 0;
  }
  else {
    int32_t num_interm_tensors= num_output_tensors1;

    config2 = vx_tidl_utils_getConfig(context, network, &num_output_tensors1, &num_output_tensors2, targetCpuId2[0]);

    if (num_interm_tensors != num_output_tensors1) {
      printf("Number of output tensors from first group of layers not equal to the number of input tensors from second group of layers. Exiting ...\n");
      goto exit;
    }
  }

  printf(" Reading network params file %s ...\n", obj->tidl_params_file_path);

  status= vx_tidl_utils_readParams(network, &obj->tidl_params_file_path[0]);
  VX_TUTORIAL_ASSERT(status==(vx_status)VX_SUCCESS);

  kernel1 = tivxAddKernelTIDL(context, num_input_tensors, num_output_tensors1);
  VX_TUTORIAL_ASSERT_VALID_REF(kernel1)

  if (targetCpuId2[0]!= (vx_enum)TIVX_CPU_ID_INVALID) {
    kernel2 = tivxAddKernelTIDL(context, num_output_tensors1, num_output_tensors2);
    VX_TUTORIAL_ASSERT_VALID_REF(kernel2)
  }


  for (threadIdx= 0; threadIdx < maxNumThreads; threadIdx++) {

    printf("\nThread #%d: Create graph ... \n", threadIdx+1);

    /* Create OpenVx Graph */
    graph[threadIdx] = vxCreateGraph(context);
    VX_TUTORIAL_ASSERT_VALID_REF(graph[threadIdx])

    printf("Thread #%d: Create input and output tensors for node 1 ... \n", threadIdx+1);
    /* Create array of input tensors for the first node */

    status= createInputTensors(context, config1, &input_tensors[threadIdx][0]);
    VX_TUTORIAL_ASSERT(status==(vx_status)VX_SUCCESS);

    /* Create array of output tensors for the first node, which is also the input tensors for the second node */
    status= createOutputTensor(context, config1, &output_tensors1[threadIdx][0]);
    VX_TUTORIAL_ASSERT(status==(vx_status)VX_SUCCESS);

    /*
     * TIDL maintains range statistics for previously processed frames. It quantizes the current inference activations using range statistics from history for processes (weighted average range).
     * Below is the parameters controls quantization.
     * quantMargin is margin added to the average in percentage.
     * quantHistoryBoot weights used for previously processed inference during application boot time for initial few frames
     * quantHistory weights used for previously processed inference during application execution (After initial few frames)
     *
     * Below settings are adequate for running on videos sequences.
     * For still images, set all settings to 0.
     */
    quantHistoryBoot= 20;
    quantHistory= 5;
    quantMargin= 0;

    printf("Thread #%d: Create node 1 ... \n", threadIdx+1);

    createParams1[threadIdx]= vx_tidl_utils_setCreateParams(context, quantHistoryBoot, quantHistory, quantMargin);
    inArgs1[threadIdx]= vx_tidl_utils_setInArgs(context);
    outArgs1[threadIdx]= vx_tidl_utils_setOutArgs(context);

    params[0]= (vx_reference)config1;
    params[1]= (vx_reference)network;
    params[2]= (vx_reference)createParams1[threadIdx];
    params[3]= (vx_reference)inArgs1[threadIdx];
    params[4]= (vx_reference)outArgs1[threadIdx];

    node1[threadIdx] = tivxTIDLNode(graph[threadIdx], kernel1,
        params,
        &input_tensors[threadIdx][0],
        &output_tensors1[threadIdx][0]
    );
    VX_TUTORIAL_ASSERT_VALID_REF(node1[threadIdx])

    /* Set target node to targetCore1 (EVEn or DSP1)*/
    vxSetNodeTarget(node1[threadIdx], (vx_enum)VX_TARGET_STRING, targetCore1[threadIdx]);
#ifdef HOST_EMULATION
    /* This is a workaround to support spanning graphs on different EVE and DSP cores in PC host emulation environment
     * */
    gTidlNodeCpuId[2*threadIdx]= targetCpuId1[threadIdx];
#endif

    if ((targetCpuId2[threadIdx]== (vx_enum)TIVX_CPU_ID_DSP1) || (targetCpuId2[threadIdx]== (vx_enum)TIVX_CPU_ID_DSP2)) {
      printf("Thread #%d: Create output tensors for node 2 ... \n", threadIdx+1);

      /* Create array of output tensors for the second node */
      status= createOutputTensor(context, config2, &output_tensors2[threadIdx][0]);
      VX_TUTORIAL_ASSERT(status==(vx_status)VX_SUCCESS);

      printf("Thread #%d: Create node 2 ... \n", threadIdx+1);

      createParams2[threadIdx]= vx_tidl_utils_setCreateParams(context, quantHistoryBoot, quantHistory, quantMargin);
      inArgs2[threadIdx]= vx_tidl_utils_setInArgs(context);
      outArgs2[threadIdx]= vx_tidl_utils_setOutArgs(context);

      params[0]= (vx_reference)config2;
      params[1]= (vx_reference)network;
      params[2]= (vx_reference)createParams2[threadIdx];
      params[3]= (vx_reference)inArgs2[threadIdx];
      params[4]= (vx_reference)outArgs2[threadIdx];

      node2[threadIdx] = tivxTIDLNode(graph[threadIdx], kernel2,
          params,
          &output_tensors1[threadIdx][0],
          &output_tensors2[threadIdx][0]
      );
      VX_TUTORIAL_ASSERT_VALID_REF(node2[threadIdx])

      /* Set target node to targetCore2 (EVEn or DSP1)*/
      vxSetNodeTarget(node2[threadIdx], (vx_enum)VX_TARGET_STRING, targetCore2[threadIdx]);
#ifdef HOST_EMULATION
      /* This is a workaround to support spanning graphs on different EVE and DSP cores in PC host emulation environment
       * */
      gTidlNodeCpuId[2*threadIdx+1]= targetCpuId2[threadIdx];
#endif

    }
  }

  printf("\n");

  for (threadIdx= 0; threadIdx < maxNumThreads; threadIdx++) {
    printf("Thread #%d: Verify graph ... \n", threadIdx+1);
    /* Verify the TIDL Graph
     * When executed in host emulation on PC, the version of TI-DL library linked displays information about each layer of the network.
     * In target execution, such display is disabled in the library.
     * */
    status = vxVerifyGraph(graph[threadIdx]);
    VX_TUTORIAL_ASSERT(status==(vx_status)VX_SUCCESS);
  }

  if((vx_status)VX_SUCCESS == status) {

    for (threadIdx= 0; threadIdx < maxNumThreads; threadIdx++) {

      /* Read input from file and populate the input tensor #0, we assume here that only one input tensor is used */
      status= readInput(context, config1, &input_tensors[threadIdx][0], &obj->input_file_path[0], obj->operation_mode);
      VX_TUTORIAL_ASSERT(status==(vx_status)VX_SUCCESS);

      if (status!=(vx_status)VX_SUCCESS) {
        goto exit;
      }

    }

    exe_time= tivxPlatformGetTimeInUsecs();

    printf("\n");

#ifdef SEQUENTIAL_SCHEDULE
    for (threadIdx= 0; threadIdx < maxNumThreads; threadIdx++) {
      printf("Thread #%d: Execute graph ... \n",threadIdx + 1);
      /* Execute the network */
      status = vxProcessGraph(graph[threadIdx]);
      VX_TUTORIAL_ASSERT(status==(vx_status)VX_SUCCESS);
    }
#else
    for (threadIdx= 0; threadIdx < maxNumThreads; threadIdx++) {
      printf("Thread #%d: Start graph ... \n",threadIdx + 1);
      /* Execute the network */
      status = vxScheduleGraph(graph[threadIdx]);
      VX_TUTORIAL_ASSERT(status==(vx_status)VX_SUCCESS);
    }

    /* You can do other useful things here, while the graphs execute asynchronously using resources available on other cores */
    printf("\n");

    for (threadIdx= 0; threadIdx < maxNumThreads; threadIdx++) {
      printf("Thread #%d: Wait for graph ... \n",threadIdx + 1);
      /* Execute the network */
      status = vxWaitGraph(graph[threadIdx]);
      VX_TUTORIAL_ASSERT(status==(vx_status)VX_SUCCESS);
    }
#endif

    exe_time= tivxPlatformGetTimeInUsecs() - exe_time;

    for (threadIdx= 0; threadIdx < maxNumThreads; threadIdx++) {

      printf("\nThread #%d: Results\n", threadIdx+1);
      printf("---------------------\n");

      /* Display the output_tensors1 if graph runs 1 cores */
      if (targetCpuId2[threadIdx]== (vx_enum)TIVX_CPU_ID_INVALID) {
        real_output_tensors= &output_tensors1[threadIdx][0];
        realConfig= config1;
      }
      else { /* Display the output_tensors2 if graph runs 2 cores */
        real_output_tensors= &output_tensors2[threadIdx][0];
        realConfig= config2;
      }

      displayOutput(NULL, (vx_df_image)NULL, NULL, 0, 0, 0, realConfig, real_output_tensors, &obj->output_file_path[0], obj->operation_mode);

    }
  }

  for (threadIdx= 0; threadIdx < maxNumThreads; threadIdx++) {
    vxQueryNode(node1[threadIdx], (vx_enum)VX_NODE_PERFORMANCE, &perf_node1, sizeof(perf_node1));
    printf("\n---- Thread #%d: Node 1 (%s) Execution time: %4.6f ms\n", threadIdx+1, targetCore1[threadIdx], perf_node1.min/1000000.0);

    if(node2[threadIdx] != 0) {
      vxQueryNode(node2[threadIdx], (vx_enum)VX_NODE_PERFORMANCE, &perf_node2, sizeof(perf_node2));
      printf("---- Thread #%d: Node 2 (%s) Execution time: %4.6f ms\n", threadIdx+1, targetCore2[threadIdx], perf_node2.min/1000000.0);
    }

    vxQueryGraph(graph[threadIdx], (vx_enum)VX_GRAPH_PERFORMANCE, &perf_graph, sizeof(perf_graph));
     printf("---- Thread #%d: Total Graph Execution time: %4.6f ms\n", threadIdx + 1, perf_graph.min/1000000.0);
  }

#ifdef SEQUENTIAL_SCHEDULE
  printf("\nExecution time of all the threads running sequentially: %4.6f ms\n", exe_time/1000.0);
#else
  printf("\nExecution time of all the threads running in parallel: %4.6f ms\n", exe_time/1000.0);
#endif

  for (threadIdx= 0; threadIdx < maxNumThreads; threadIdx++) {
    if (vxGetStatus((vx_reference)node1[threadIdx]) == (vx_status)VX_SUCCESS) {
      vxReleaseNode(&node1[threadIdx]);
    }
    if (vxGetStatus((vx_reference)createParams1[threadIdx]) == (vx_status)VX_SUCCESS) {
      vxReleaseUserDataObject(&createParams1[threadIdx]);
    }
    if (vxGetStatus((vx_reference)inArgs1[threadIdx]) == (vx_status)VX_SUCCESS) {
      vxReleaseUserDataObject(&inArgs1[threadIdx]);
    }
    if (vxGetStatus((vx_reference)outArgs1[threadIdx]) == (vx_status)VX_SUCCESS) {
      vxReleaseUserDataObject(&outArgs1[threadIdx]);
    }
    if (node2[threadIdx] !=0 ){
      if (vxGetStatus((vx_reference)node2[threadIdx]) == (vx_status)VX_SUCCESS) {
        vxReleaseNode(&node2[threadIdx]);
      }
      if (vxGetStatus((vx_reference)createParams2[threadIdx]) == (vx_status)VX_SUCCESS) {
        vxReleaseUserDataObject(&createParams2[threadIdx]);
      }
      if (vxGetStatus((vx_reference)inArgs2[threadIdx]) == (vx_status)VX_SUCCESS) {
        vxReleaseUserDataObject(&inArgs2[threadIdx]);
      }
      if (vxGetStatus((vx_reference)outArgs2[threadIdx]) == (vx_status)VX_SUCCESS) {
        vxReleaseUserDataObject(&outArgs2[threadIdx]);
      }
    }
  }

  if (config2 !=0 ){
    if (vxGetStatus((vx_reference)config2) == (vx_status)VX_SUCCESS) {
      vxReleaseUserDataObject(&config2);
    }
  }

  if (vxGetStatus((vx_reference)config1) == (vx_status)VX_SUCCESS) {
    vxReleaseUserDataObject(&config1);
  }

  if (vxGetStatus((vx_reference)network) == (vx_status)VX_SUCCESS) {
    vxReleaseUserDataObject(&network);
  }

  for (threadIdx= 0; threadIdx < maxNumThreads; threadIdx++) {

    if (vxGetStatus((vx_reference)graph[threadIdx]) == (vx_status)VX_SUCCESS) {
      vxReleaseGraph(&graph[threadIdx]);
    }

    for (i= 0; i < num_input_tensors; i++) {
      if (vxGetStatus((vx_reference)input_tensors[threadIdx][i]) == (vx_status)VX_SUCCESS) {
        vxReleaseTensor(&input_tensors[threadIdx][i]);
      }
    }

    for (i= 0; i < num_output_tensors1; i++) {
      if (vxGetStatus((vx_reference)output_tensors1[threadIdx][i]) == (vx_status)VX_SUCCESS) {
        vxReleaseTensor(&output_tensors1[threadIdx][i]);
      }
    }

    for (i= 0; i < num_output_tensors2; i++) {
      if (vxGetStatus((vx_reference)output_tensors2[threadIdx][i]) == (vx_status)VX_SUCCESS) {
        vxReleaseTensor(&output_tensors2[threadIdx][i]);
      }
    }

  }

  if (vxGetStatus((vx_reference)kernel1) == (vx_status)VX_SUCCESS) {
    vxRemoveKernel(kernel1);
  }
  if (kernel2!=0){
    if (vxGetStatus((vx_reference)kernel2) == (vx_status)VX_SUCCESS) {
      vxRemoveKernel(kernel2);
    }
  }

  exit:
  printf("\n vx_tutorial_tidl: Tutorial Done !!! \n");
  printf(" \n");

  if (vxGetStatus((vx_reference)context) == (vx_status)VX_SUCCESS) {
    vxReleaseContext(&context);
  }

}

static vx_status parse_cfg_file(VxTutorialTidl_CfgObj *obj, char *cfg_file_name)
{
  FILE *fp = fopen(cfg_file_name, "r");
  char line_str[1024];
  char *token;
  const char *basePath;
  size_t sizeFilePath;
  char filePath[MAXPATHLENGTH];
  vx_status status = (vx_status)VX_SUCCESS;

  basePath = tivx_utils_get_test_file_dir();

  if (basePath == NULL)
  {
      goto exit;
  }

  /* Set processing_core_mode to 0, which means network can be partitioned accross all cores */
  obj->processing_core_mode= 0;
  obj->num_eve_cores= 1;

  if(fp==NULL)
  {
    printf("# ERROR: Unable to open config file [%s]\n", cfg_file_name);
#ifdef HOST_EMULATION
    printf("# ERROR: Please make sure that the environment variable VX_TEST_DATA_PATH is set to .../conformance_tests/test_data\n");
#endif
    status= (vx_status)VX_FAILURE;
    goto exit;
  }

  while(fgets(line_str, sizeof(line_str), fp)!=NULL)
  {
    char s[]=" \t";

    if (strchr(line_str, '#'))
    {
      continue;
    }

    /* get the first token */
    token = strtok(line_str, s);

    if(strcmp(token, "tidl_params_file_path")==0)
    {
      token = strtok(NULL, s);
      token[strlen(token)-1]=0;
      sizeFilePath = snprintf(filePath, MAXPATHLENGTH, "%s/%s", basePath, token);
      if (sizeFilePath > MAXPATHLENGTH) {
        printf("Error in parse_cfg_file, path too long to fit in string\n");
      }
      else {
        strcpy(obj->tidl_params_file_path, filePath);
      }
    }
    else
      if(strcmp(token, "tidl_network_file_path")==0)
      {
        token = strtok(NULL, s);
        token[strlen(token)-1]=0;
        sizeFilePath = snprintf(filePath, MAXPATHLENGTH, "%s/%s", basePath, token);
        if (sizeFilePath > MAXPATHLENGTH) {
          printf("Error in parse_cfg_file, path too long to fit in string\n");
        }
        else {
          strcpy(obj->tidl_network_file_path, filePath);
        }
      }
      else
        if(strcmp(token, "input_file_path")==0)
        {
          token = strtok(NULL, s);
          token[strlen(token)-1]=0;
          sizeFilePath = snprintf(filePath, MAXPATHLENGTH, "%s/%s", basePath, token);
          if (sizeFilePath > MAXPATHLENGTH) {
            printf("Error in parse_cfg_file, path too long to fit in string\n");
          }
          else {
            strcpy(obj->input_file_path, filePath);
          }
        }
        else
          if(strcmp(token, "output_file_path")==0)
          {
            token = strtok(NULL, s);
            token[strlen(token)-1]=0;
            sizeFilePath = snprintf(filePath, MAXPATHLENGTH, "%s/%s", basePath, token);
            if (sizeFilePath > MAXPATHLENGTH) {
              printf("Error in parse_cfg_file, path too long to fit in string\n");
            }
            else {
              strcpy(obj->output_file_path, filePath);
            }
          }
          else
            if(strcmp(token, "operation_mode")==0)
            {
              token = strtok(NULL, s);
              obj->operation_mode = atoi(token);
            }
            else
              if(strcmp(token, "processing_core_mode")==0)
              {
                token = strtok(NULL, s);
                obj->processing_core_mode = atoi(token);
              }
              else
                if(strcmp(token, "num_eve_cores")==0)
                {
                  token = strtok(NULL, s);
                  obj->num_eve_cores = atoi(token);
                }
  }

  fclose(fp);

  exit:
  return status;
}

static vx_status createInputTensors(vx_context context, vx_user_data_object config, vx_tensor *input_tensors)
{
  vx_size   input_sizes[VX_TUTORIAL_MAX_TENSOR_DIMS];
  vx_map_id map_id_config;
  sTIDL_IOBufDesc_t *ioBufDesc;
  uint32_t id;
  vx_status status = (vx_status)VX_SUCCESS;

  vxMapUserDataObject(config, 0, sizeof(sTIDL_IOBufDesc_t), &map_id_config,
      (void **)&ioBufDesc, (vx_enum)VX_READ_ONLY, (vx_enum)VX_MEMORY_TYPE_HOST, 0);

  if (ioBufDesc->numInputBuf < VX_TUTORIAL_MAX_TENSORS) {

    for(id = 0; id < ioBufDesc->numInputBuf; id++) {
      input_sizes[0] = ioBufDesc->inWidth[id]  + ioBufDesc->inPadL[id] + ioBufDesc->inPadR[id];
      input_sizes[1] = ioBufDesc->inHeight[id] + ioBufDesc->inPadT[id] + ioBufDesc->inPadB[id];
      input_sizes[2] = ioBufDesc->inNumChannels[id];

      input_tensors[id] = vxCreateTensor(context, 3, input_sizes, (vx_enum)VX_TYPE_UINT8, 0);
    }

  }
  else {
    status= (vx_status)VX_FAILURE;
  }

  vxUnmapUserDataObject(config, map_id_config);

  return status;
}

static vx_status createOutputTensor(vx_context context, vx_user_data_object config, vx_tensor *output_tensors)
{
  vx_size    output_sizes[VX_TUTORIAL_MAX_TENSOR_DIMS];
  vx_map_id map_id_config;
  uint32_t id;
  sTIDL_IOBufDesc_t *ioBufDesc;
  vx_status status = (vx_status)VX_SUCCESS;

  vxMapUserDataObject(config, 0, sizeof(sTIDL_IOBufDesc_t), &map_id_config,
      (void **)&ioBufDesc, (vx_enum)VX_READ_ONLY, (vx_enum)VX_MEMORY_TYPE_HOST, 0);

  if (ioBufDesc->numOutputBuf < VX_TUTORIAL_MAX_TENSORS) {

    for(id = 0; id < ioBufDesc->numOutputBuf; id++) {
      output_sizes[0] = ioBufDesc->outWidth[id]  + ioBufDesc->outPadL[id] + ioBufDesc->outPadR[id];
      output_sizes[1] = ioBufDesc->outHeight[id] + ioBufDesc->outPadT[id] + ioBufDesc->outPadB[id];
      output_sizes[2] = ioBufDesc->outNumChannels[id];

      output_tensors[id] = vxCreateTensor(context, 3, output_sizes, (vx_enum)VX_TYPE_FLOAT32, 0);
    }

  }
  else {
    status= (vx_status)VX_FAILURE;
  }

  vxUnmapUserDataObject(config, map_id_config);

  return status;
}

static vx_status readDataS8(FILE *fp, int8_t *ptr, int32_t n,
    int32_t width, int32_t height, int32_t pitch,
    int32_t chOffset)
{
  int32_t   i0, i1;
  uint32_t readSize;
  vx_status status = (vx_status)VX_SUCCESS;

  for(i0 = 0; i0 < n; i0++)
  {
    for(i1 = 0; i1 < height; i1++)
    {
      readSize= fread(&ptr[i0*chOffset + i1*pitch], 1, width, fp);
      if (readSize != width) {
        status= (vx_status)VX_FAILURE;
        goto exit;
      }
    }
  }

  exit:
  return status;

}


static vx_status readInput(vx_context context, vx_user_data_object config, vx_tensor *input_tensors, char *input_file, uint32_t operation_mode)
{
  vx_status status = (vx_status)VX_SUCCESS;

  int8_t      *input_buffer = NULL;
  uint32_t   id;

  vx_map_id map_id_config;
  vx_map_id map_id_input;

  vx_size    start[VX_TUTORIAL_MAX_TENSOR_DIMS];
  vx_size    input_strides[VX_TUTORIAL_MAX_TENSOR_DIMS];
  vx_size    input_sizes[VX_TUTORIAL_MAX_TENSOR_DIMS];

  sTIDL_IOBufDesc_t *ioBufDesc;

  FILE *fp;

  fp= fopen(input_file, "rb");

  if(fp==NULL)
  {
    printf("# ERROR: Unable to open input file [%s]\n", input_file);
    return((vx_status)VX_FAILURE);
  }

  vxMapUserDataObject(config, 0, sizeof(sTIDL_IOBufDesc_t), &map_id_config,
      (void **)&ioBufDesc, (vx_enum)VX_READ_ONLY, (vx_enum)VX_MEMORY_TYPE_HOST, 0);

  for(id = 0; id < ioBufDesc->numInputBuf; id++)
  {
    input_sizes[0] = ioBufDesc->inWidth[id]  + ioBufDesc->inPadL[id] + ioBufDesc->inPadR[id];
    input_sizes[1] = ioBufDesc->inHeight[id] + ioBufDesc->inPadT[id] + ioBufDesc->inPadB[id];
    input_sizes[2] = ioBufDesc->inNumChannels[id];

    start[0] = start[1] = start[2] = 0;

    input_strides[0] = 1;
    input_strides[1] = input_sizes[0];
    input_strides[2] = input_sizes[1] * input_strides[1];

    status = tivxMapTensorPatch(input_tensors[id], 3, start, input_sizes, &map_id_input, input_strides, (void **)&input_buffer, (vx_enum)VX_WRITE_ONLY, (vx_enum)VX_MEMORY_TYPE_HOST);

    if ((vx_status)VX_SUCCESS == status)
    {
      status= readDataS8(
          fp,
          &input_buffer[(ioBufDesc->inPadT[id] * input_strides[1]) + ioBufDesc->inPadL[id]],
          ioBufDesc->inNumChannels[id],
          ioBufDesc->inWidth[id],
          ioBufDesc->inHeight[id],
          input_strides[1],
          input_strides[2]);

      tivxUnmapTensorPatch(input_tensors[id], map_id_input);

      if (status== (vx_status)VX_FAILURE) {
        goto exit;
      }
    }
  }

  exit:
  vxUnmapUserDataObject(config, map_id_config);

  fclose(fp);

  return status;
}

static void displayOutput(void *bmp_context, vx_df_image df_image, void *data_ptr, vx_uint32 img_width, vx_uint32 img_height, vx_uint32 img_stride, vx_user_data_object config, vx_tensor *output_tensors, char *output_file, uint32_t operation_mode)
{
  vx_status status = (vx_status)VX_SUCCESS;

  vx_size output_sizes[VX_TUTORIAL_MAX_TENSOR_DIMS];

  vx_map_id map_id_config;

  int32_t id, i, j;

  sTIDL_IOBufDesc_t *ioBufDesc;

  vxMapUserDataObject(config, 0, sizeof(sTIDL_IOBufDesc_t), &map_id_config,
      (void **)&ioBufDesc, (vx_enum)VX_READ_ONLY, (vx_enum)VX_MEMORY_TYPE_HOST, 0);

  for(id = 0; id < 1; id++)
  {
    output_sizes[0] = ioBufDesc->outWidth[id]  + ioBufDesc->outPadL[id] + ioBufDesc->outPadR[id];
    output_sizes[1] = ioBufDesc->outHeight[id] + ioBufDesc->outPadT[id] + ioBufDesc->outPadB[id];
    output_sizes[2] = ioBufDesc->outNumChannels[id];

    status = vxGetStatus((vx_reference)output_tensors[id]);

    if ((vx_status)VX_SUCCESS == status)
    {
      void *output_buffer;

      vx_map_id map_id_output;

      vx_size output_strides[VX_TUTORIAL_MAX_TENSOR_DIMS];
      vx_size start[VX_TUTORIAL_MAX_TENSOR_DIMS];

      start[0] = start[1] = start[2] = start[3] = 0;

      output_strides[0] = 1;
      output_strides[1] = output_sizes[0];
      output_strides[2] = output_sizes[1] * output_strides[1];

      tivxMapTensorPatch(output_tensors[id], 3, start, output_sizes, &map_id_output, output_strides, &output_buffer, (vx_enum)VX_READ_ONLY, (vx_enum)VX_MEMORY_TYPE_HOST);

      if (operation_mode == 0)
      {
        uint8_t *pOut;
        uint8_t score[5];
        vx_uint32 classid[5];

        pOut = (uint8_t *)output_buffer + (ioBufDesc->outPadT[id] * output_sizes[0]) + ioBufDesc->outPadL[id];

        for(i = 0; i < 5; i++)
        {
          score[i] = 0;
          classid[i] = 0xFFFFFFFF;

          for(j = 0; j < output_sizes[0]; j++)
          {
            if(pOut[j] > score[i])
            {
              score[i] = pOut[j];
              classid[i] = j;
            }
          }

          pOut[classid[i]] = 0;
        }

        printf("\nImage classification Top-5 results: \n");

        for(i = 0; i < 5; i++)
        {
          printf(" %s, class-id: %d, score: %u\n", (char *)&imgnet_labels[classid[i]], classid[i], score[i]);
        }
      }
      else
        if (operation_mode== 1)
        {
          typedef struct {
            float objId;
            float label;
            float score;
            float xmin;
            float ymin;
            float xmax;
            float ymax;
          } ODLayerObjInfo;

          /* Display of coordinates of detected objects */
          uint8_t *pOut;
          ODLayerObjInfo *pObjInfo;
          uint32_t numObjs;

          numObjs= 20;

          pOut = (uint8_t *)output_buffer + (ioBufDesc->outPadT[id] * output_sizes[0]) + ioBufDesc->outPadL[id];

          pObjInfo = (ODLayerObjInfo *)pOut;

          printf("\nObjId|label|score| xmin| ymin| xmax| ymax|\n");
          printf("------------------------------------------\n");
          for(i = 0; i < numObjs; i++)
          {
            ODLayerObjInfo * pObj = pObjInfo + i;
            if ((int32_t)(pObj->objId)!=-1) {
              printf("%5d|%5d|%5.2f|%5.2f|%5.2f|%5.2f|%5.2f|\n", (int32_t)pObj->objId, (uint32_t)pObj->label, pObj->score, pObj->xmin, pObj->ymin, pObj->xmax, pObj->ymax);
            }
            else {
              break;
            }
            /*
            if(pPSpots->score >= 0.5f)
            {
              drawBox(obj, pObj);
            }
            */
          }
          printf("\nNumber of detected objects: %d\n\n", i);
        }
      tivxUnmapTensorPatch(output_tensors[id], map_id_output);
    }
  }

  vxUnmapUserDataObject(config, map_id_config);
}
