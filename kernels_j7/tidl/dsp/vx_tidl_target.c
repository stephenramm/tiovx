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


#include <stdio.h>
#include <TI/tivx.h>
#include <TI/j7.h>
#include <TI/j7_tidl.h>
#include <TI/tivx_target_kernel.h>
#include <tivx_alg_ivision_if.h>
#include "tivx_platform.h"
#include "itidl_ti.h"



#ifndef x86_64
#include "c7x.h"
#include <ti/osal/HwiP.h>
//#define TIDL_C7X_CLEAR_L1D_SRAM
#define DISABLE_INTERRUPTS_DURING_PROCESS
#endif

#define TIDL_COPY_NETWORK_BUF

typedef struct
{
    IVISION_BufDesc     inBufDesc[TIDL_MAX_ALG_IN_BUFS];
    IVISION_BufDesc     outBufDesc[TIDL_MAX_ALG_OUT_BUFS];

    IVISION_BufDesc    *inBufDescList[TIDL_MAX_ALG_IN_BUFS];
    IVISION_BufDesc    *outBufDescList[TIDL_MAX_ALG_OUT_BUFS];

    IVISION_InBufs      inBufs;
    IVISION_OutBufs     outBufs;

    TIDL_InArgs         *inArgs;
    TIDL_outArgs        *outArgs;

    TIDL_CreateParams   createParams;

    tivxTIDLJ7Params    tidlParams;

    void                *tidlNet;
    vx_uint32            netSize;

    void                *algHandle;

} tivxTIDLObj;

static tivx_target_kernel vx_tidl_target_kernel = NULL;

static void tivxTIDLFreeMem(tivxTIDLObj *tidlObj);
static vx_status testChecksum(void *dataPtr, uint8_t *refQC, vx_int32 data_size, uint32_t loc);
static void getQC(uint8_t *pIn, uint8_t *pOut, int32_t inSize);

#ifdef TIDL_C7X_CLEAR_L1D_SRAM

#define MEMORY_ELEM_8BIT  (1)
#define MEMORY_ELEM_16BIT (2)
#define MEMORY_ELEM_32BIT (4)

#define TIDL_C7X_L1D_SRAM_SIZE (16 * 1024)
#define TIDL_C7X_L1D_SRAM_ADDR (0x64E00000)

void memsetFast(void *array, uint32_t value, int32_t size, uint8_t elementSize)
{
   __STRM_TEMPLATE saTemplate;
   __SA_FLAGS saFlags;

    uint32_t ctr;

   //Initialize SA flags to default
   saFlags = __SA_FLAGS_default();

   //Update necessary flags
   saFlags.DIMFMT  = __SA_DIMFMT_3D;

   //Initialize template
   saTemplate = (0);

   //Set SE_PARAMS
   saTemplate = __set_ICNT0(saTemplate, size);
   saTemplate = __set_ICNT1_DIM1(saTemplate, 1, 0);
   saTemplate = __set_ICNT2_DIM2(saTemplate, 1, 0);

    if( elementSize == MEMORY_ELEM_8BIT )
    {
        uint8_t *restrict pData = (uint8_t *)array;
        uchar64 vVal = (uchar64)(value);

        saFlags.VECLEN  = __SA_VECLEN_64ELEMS;
        saTemplate = __set_SA_FLAGS(saTemplate, &saFlags);

        __SA0_OPEN(saTemplate);
        for( ctr = 0; ctr < size; ctr+=64 )
        {
            __vpred vpStore = __SA0_VPRED(uchar64);
            __vstore_pred(vpStore, __SA0ADV(uchar64, pData), vVal);
        }
        __SA0_CLOSE();

    }
    else if( elementSize == MEMORY_ELEM_16BIT )
    {
        uint16_t *restrict pData = (uint16_t *)array;
        ushort32 vVal = (ushort32)(value);

        saFlags.VECLEN  = __SA_VECLEN_32ELEMS;
        saTemplate = __set_SA_FLAGS(saTemplate, &saFlags);

        __SA0_OPEN(saTemplate);
        for( ctr = 0; ctr < size; ctr+=32 )
        {
            __vpred vpStore = __SA0_VPRED(ushort32);
            __vstore_pred(vpStore, __SA0ADV(ushort32, pData), vVal);
        }
        __SA0_CLOSE();

    }
    else if( elementSize == MEMORY_ELEM_32BIT )
    {
        uint32_t *restrict pData = (uint32_t *)array;
        uint16 vVal = (uint16)(value);

        saFlags.VECLEN  = __SA_VECLEN_16ELEMS;
        saTemplate = __set_SA_FLAGS(saTemplate, &saFlags);

        __SA0_OPEN(saTemplate);
        for( ctr = 0; ctr < size; ctr+=16 )
        {
            __vpred vpStore = __SA0_VPRED(uint16);
            __vstore_pred(vpStore, __SA0ADV(uint16, pData), vVal);
        }
        __SA0_CLOSE();
    }
}
#endif

static int32_t tidl_AllocNetInputMem(IVISION_BufDesc *BufDescList, sTIDL_IOBufDesc_t *pConfig)
{
  uint16_t numBuffs = 0;

  /* Currently only one input buffer supported */
  for(numBuffs = 0; numBuffs < pConfig->numInputBuf; numBuffs++)
  {
    BufDescList[numBuffs].numPlanes                          = 1;
    BufDescList[numBuffs].bufPlanes[0].frameROI.topLeft.x    = 0;
    BufDescList[numBuffs].bufPlanes[0].frameROI.topLeft.y    = 0;

    /* This has to be width + horizontal padding */
    BufDescList[numBuffs].bufPlanes[0].width                 = pConfig->inWidth[numBuffs] + pConfig->inPadL[numBuffs] + pConfig->inPadR[numBuffs];
    /* This has to be numCh * (height + vertical padding) */
    BufDescList[numBuffs].bufPlanes[0].height                = pConfig->inNumChannels[numBuffs] * (pConfig->inHeight[numBuffs]  + pConfig->inPadT[numBuffs] + pConfig->inPadB[numBuffs]);
    /* This has to be just width */
    BufDescList[numBuffs].bufPlanes[0].frameROI.width        = pConfig->inWidth[numBuffs];
    /* This has to be just height */
    BufDescList[numBuffs].bufPlanes[0].frameROI.height       = pConfig->inHeight[numBuffs];

    /* This comes from tidl_io_xxx.txt file (inDataId), not sure how to pass it. Currently hardcoding for Jacinto Net */
    BufDescList[numBuffs].reserved[0]     = pConfig->inDataId[numBuffs];

    /* This comes from tidl_io_xxx.txt file (inDataId), not sure how to pass it. Currently hardcoding for Jacinto Net */
    BufDescList[numBuffs].bufferId = pConfig->inDataId[numBuffs];
  }
  return numBuffs;
}

static int32_t tidl_AllocNetOutputMem(IVISION_BufDesc *BufDescList, sTIDL_IOBufDesc_t *pConfig)
{
  uint16_t numBuffs = 0;

  /* Currently only one output buffer supported */
  for(numBuffs = 0; numBuffs < pConfig->numOutputBuf; numBuffs++)
  {
      BufDescList[numBuffs].numPlanes                          = 1;
      BufDescList[numBuffs].bufPlanes[0].frameROI.topLeft.x    = 0;
      BufDescList[numBuffs].bufPlanes[0].frameROI.topLeft.y    = 0;

      /* This requires output width + horizontal padding */
      BufDescList[numBuffs].bufPlanes[0].width                 = pConfig->outWidth[numBuffs] + pConfig->outPadL[numBuffs] + pConfig->outPadR[numBuffs];
      /* This requires numOutCh * (output height + vertial padding) */
      BufDescList[numBuffs].bufPlanes[0].height                = pConfig->outNumChannels[numBuffs] * (pConfig->outHeight[numBuffs] + pConfig->outPadT[numBuffs] + pConfig->outPadB[numBuffs]);
      /* This requires just output width */
      BufDescList[numBuffs].bufPlanes[0].frameROI.width        = pConfig->outWidth[numBuffs];
      /* This requires just output height */
      BufDescList[numBuffs].bufPlanes[0].frameROI.height       = pConfig->outHeight[numBuffs];

      /* This comes from tidl_io_xxx.txt file (outDataId), not sure how to pass it. Currently hardcoding for Jacinto Net */
      BufDescList[numBuffs].reserved[0]      = pConfig->outDataId[numBuffs];

      /* This comes from tidl_io_xxx.txt file (outDataId), not sure how to pass it. Currently hardcoding for Jacinto Net */
      BufDescList[numBuffs].bufferId         = pConfig->outDataId[numBuffs];
  }
  return numBuffs;
}

static vx_status VX_CALLBACK tivxKernelTIDLProcess
(
    tivx_target_kernel_instance kernel,
    tivx_obj_desc_t *obj_desc[],
    uint16_t num_params,
    void *priv_arg
)
{
    vx_status status = VX_SUCCESS;

    tivxTIDLObj *tidlObj;
    uint32_t i, size;

    #ifdef DISABLE_INTERRUPTS_DURING_PROCESS
    uint32_t oldIntState;
    #endif

    for (i = 0U; i < num_params; i ++)
    {
        if (NULL == obj_desc[i])
        {
            status = VX_FAILURE;
            break;
        }
    }

    if (VX_SUCCESS == status)
    {
        status = tivxGetTargetKernelInstanceContext(kernel, (void **)&tidlObj, &size);

        if ((VX_SUCCESS != status) || (NULL == tidlObj) ||  (sizeof(tivxTIDLObj) != size))
        {
            status = VX_FAILURE;
        }
    }

    if (VX_SUCCESS == status)
    {
        tivx_obj_desc_user_data_object_t *network;
        void *network_target_ptr = NULL;

        /* IMPORTANT! Network data is assumed to be available at index 1 */
        network   = (tivx_obj_desc_user_data_object_t *)obj_desc[1];

        network_target_ptr = tivxMemShared2TargetPtr(&network->mem_ptr);
        tivxMemBufferMap(network_target_ptr, network->mem_size, VX_MEMORY_TYPE_HOST, VX_READ_ONLY);

        if(tidlObj->tidlParams.compute_network_checksum == 1)
        {
            sTIDL_Network_t *pNet = (sTIDL_Network_t *)network_target_ptr;
            uint8_t *pPerfInfo = (uint8_t *)network_target_ptr + pNet->dataFlowInfo;

            status = testChecksum(pPerfInfo, &tidlObj->tidlParams.network_checksum[0], tidlObj->netSize - pNet->dataFlowInfo, 1);
        }
    }

    if (VX_SUCCESS == status)
    {
        tivx_obj_desc_tensor_t *inTensor;
        tivx_obj_desc_tensor_t *outTensor;
        tivx_obj_desc_user_data_object_t *inArgs;
        tivx_obj_desc_user_data_object_t *outArgs;

        void *in_tensor_target_ptr;
        void *out_tensor_target_ptr;
        void *in_args_target_ptr;
        void *out_args_target_ptr;

        /* IMPORTANT! inArgs is assumed to be available at index 3 */
        inArgs   = (tivx_obj_desc_user_data_object_t *)obj_desc[3];

        in_args_target_ptr = tivxMemShared2TargetPtr(&inArgs->mem_ptr);
        tivxMemBufferMap(in_args_target_ptr, inArgs->mem_size, VX_MEMORY_TYPE_HOST, VX_READ_ONLY);

        tidlObj->inArgs = in_args_target_ptr;

        /* IMPORTANT! outArgs is assumed to be available at index 4 */
        outArgs  = (tivx_obj_desc_user_data_object_t *)obj_desc[4];

        out_args_target_ptr = tivxMemShared2TargetPtr(&outArgs->mem_ptr);
        tivxMemBufferMap(out_args_target_ptr, outArgs->mem_size, VX_MEMORY_TYPE_HOST, VX_WRITE_ONLY);

        tidlObj->outArgs = out_args_target_ptr;

        /* Idx 0 - config data,
           Idx 1 - network data,
           Idx 2 - create parameters,
           Idx 3 - inArgs,
           Idx 4 - outArgs,
           Idx 5 - input tensor */
        uint32_t in_tensor_idx = 5;

        /* Idx N - output tensors, where N = Idx 2 + number of input tensors */
        uint32_t out_tensor_idx = in_tensor_idx + tidlObj->inBufs.numBufs;
        uint32_t id;

        for(id = 0; id < tidlObj->inBufs.numBufs; id++) {
            inTensor  = (tivx_obj_desc_tensor_t *)obj_desc[in_tensor_idx + id];
            in_tensor_target_ptr  = tivxMemShared2TargetPtr(&inTensor->mem_ptr);
            tivxMemBufferMap(in_tensor_target_ptr, inTensor->mem_size, VX_MEMORY_TYPE_HOST, VX_READ_ONLY);
            tidlObj->inBufDesc[id].bufPlanes[0].buf = in_tensor_target_ptr;
        }

        for(id = 0; id < tidlObj->outBufs.numBufs; id++) {
            outTensor = (tivx_obj_desc_tensor_t *)obj_desc[out_tensor_idx + id];
            out_tensor_target_ptr = tivxMemShared2TargetPtr(&outTensor->mem_ptr);
            tivxMemBufferMap(out_tensor_target_ptr, outTensor->mem_size, VX_MEMORY_TYPE_HOST, VX_WRITE_ONLY);
            tidlObj->outBufDesc[id].bufPlanes[0].buf = out_tensor_target_ptr;
        }

#ifdef TIDL_C7X_CLEAR_L1D_SRAM
        memsetFast((void *)TIDL_C7X_L1D_SRAM_ADDR, 0, TIDL_C7X_L1D_SRAM_SIZE, MEMORY_ELEM_8BIT);
#endif

        #ifdef DISABLE_INTERRUPTS_DURING_PROCESS
        /* disabling interrupts when doing TIDL processing
         *
         * suspect some stability issue due to interrupt handling,
         * until stability issue is root caused disabling interrupts
         * */
        oldIntState = HwiP_disable();
        #endif

        status = tivxAlgiVisionProcess
                 (
                    tidlObj->algHandle,
                    &tidlObj->inBufs,
                    &tidlObj->outBufs,
                    (IVISION_InArgs  *)tidlObj->inArgs,
                    (IVISION_OutArgs *)tidlObj->outArgs
                 );

        #ifdef DISABLE_INTERRUPTS_DURING_PROCESS
        HwiP_restore(oldIntState);
        #endif

        tivxMemBufferUnmap(in_args_target_ptr, inArgs->mem_size, VX_MEMORY_TYPE_HOST, VX_READ_ONLY);
        tivxMemBufferUnmap(out_args_target_ptr, outArgs->mem_size, VX_MEMORY_TYPE_HOST, VX_WRITE_ONLY);

        for(id = 0; id < tidlObj->inBufs.numBufs; id++) {
            inTensor  = (tivx_obj_desc_tensor_t *)obj_desc[in_tensor_idx + id];
            in_tensor_target_ptr  = tivxMemShared2TargetPtr(&inTensor->mem_ptr);
            tivxMemBufferUnmap(in_tensor_target_ptr, inTensor->mem_size, VX_MEMORY_TYPE_HOST, VX_READ_ONLY);
        }

        for(id = 0; id < tidlObj->outBufs.numBufs; id++) {
            outTensor = (tivx_obj_desc_tensor_t *)obj_desc[out_tensor_idx + id];
            out_tensor_target_ptr = tivxMemShared2TargetPtr(&outTensor->mem_ptr);
            tivxMemBufferUnmap(out_tensor_target_ptr, outTensor->mem_size, VX_MEMORY_TYPE_HOST, VX_WRITE_ONLY);
        }
    }


    return (status);
}

int32_t tivxKernelTIDLLog(const char * format, va_list va_args_ptr)
{
    static char buf[1024];

    #ifndef x86_64
    va_start(va_args_ptr, format);
    vsnprintf(buf, 1024, format, va_args_ptr);
    va_end(va_args_ptr);
    #endif

    printf(buf);

    return 0;
}

int32_t tivxKernelTIDLDumpToFile(const char * fileName, void * addr, int32_t size)
{
    volatile uint32_t i=0;

    printf("saveRaw(0, 0x%08x, \"/ti/j7presi/%s\", %d/4, 32, false);\n",
        (uint32_t)(uintptr_t)addr,
        fileName,
        ((size+3)/4)*4 /* align to 4 bytes for saveRaw */
        );

    i=i; /* to put a break point */

    return 0;
}


static vx_status VX_CALLBACK tivxKernelTIDLCreate
(
  tivx_target_kernel_instance kernel,
  tivx_obj_desc_t *obj_desc[],
  uint16_t num_params,
  void *priv_arg
)
{
    vx_status status = VX_SUCCESS;

    tivx_obj_desc_user_data_object_t *config;
    tivx_obj_desc_user_data_object_t *network;
    tivx_obj_desc_user_data_object_t *createParams;

    tivxTIDLObj *tidlObj = NULL;

    void *config_target_ptr = NULL;
    void *network_target_ptr = NULL;
    void *create_params_target_ptr = NULL;

    tivx_mem_stats l1_stats;
    tivx_mem_stats l2_stats;
    tivx_mem_stats l3_stats;

    uint32_t i;

    #ifdef TIVX_TIDL_TARGET_DEBUG
    tivx_set_debug_zone(VX_ZONE_INFO);
    #endif

    for (i = 0U; i < num_params; i ++)
    {
        if (NULL == obj_desc[i])
        {
            status = VX_FAILURE;
            break;
        }
    }
    if (VX_SUCCESS == status)
    {
        status = tivxMemResetScratchHeap(TIVX_MEM_EXTERNAL_SCRATCH);
    }
    if (VX_SUCCESS == status)
    {
        /* IMPORTANT! Config data is assumed to be available at index 0 */
        config    = (tivx_obj_desc_user_data_object_t *)obj_desc[0];

        /* IMPORTANT! Network data is assumed to be available at index 1 */
        network   = (tivx_obj_desc_user_data_object_t *)obj_desc[1];

        /* IMPORTANT! Create params is assumed to be available at index 2 */
        createParams   = (tivx_obj_desc_user_data_object_t *)obj_desc[2];

        tidlObj = tivxMemAlloc(sizeof(tivxTIDLObj), TIVX_MEM_EXTERNAL);

        if (NULL != tidlObj)
        {
            memset(tidlObj, 0, sizeof(tivxTIDLObj));
        }
        else
        {
            status = VX_ERROR_NO_MEMORY;
        }

        if (VX_SUCCESS == status)
        {
          config_target_ptr = tivxMemShared2TargetPtr(&config->mem_ptr);
          tivxMemBufferMap(config_target_ptr, config->mem_size, VX_MEMORY_TYPE_HOST, VX_READ_ONLY);

          memcpy(&tidlObj->tidlParams, config_target_ptr, sizeof(tivxTIDLJ7Params));

          if(tidlObj->tidlParams.compute_config_checksum == 1)
          {
            status = testChecksum(&tidlObj->tidlParams.ioBufDesc, &tidlObj->tidlParams.config_checksum[0], sizeof(sTIDL_IOBufDesc_t), 0);
          }
        }

        #ifdef TIDL_COPY_NETWORK_BUF
        tidlObj->tidlNet = tivxMemAlloc(network->mem_size, TIVX_MEM_EXTERNAL);
        tidlObj->netSize = network->mem_size;
        if (NULL == tidlObj->tidlNet)
        {
            status = VX_ERROR_NO_MEMORY;
        }
        #else
        tidlObj->tidlNet = NULL;
        tidlObj->netSize = 0;
        #endif

        if (VX_SUCCESS == status)
        {
          network_target_ptr = tivxMemShared2TargetPtr(&network->mem_ptr);
          tivxMemBufferMap(network_target_ptr, network->mem_size, VX_MEMORY_TYPE_HOST, VX_READ_ONLY);

          #ifdef TIDL_COPY_NETWORK_BUF
          memcpy(tidlObj->tidlNet, network_target_ptr, network->mem_size);
          #else
          tidlObj->tidlNet = network_target_ptr;
          tidlObj->netSize = network->mem_size;
          #endif

          if(tidlObj->tidlParams.compute_network_checksum == 1)
          {
            sTIDL_Network_t *pNet = (sTIDL_Network_t *)network_target_ptr;
            uint8_t *pPerfInfo = (uint8_t *)network_target_ptr + pNet->dataFlowInfo;

            status = testChecksum(pPerfInfo, &tidlObj->tidlParams.network_checksum[0], tidlObj->netSize - pNet->dataFlowInfo, 0);
          }
        }

        if (VX_SUCCESS == status)
        {
          create_params_target_ptr = tivxMemShared2TargetPtr(&createParams->mem_ptr);
          tivxMemBufferMap(create_params_target_ptr, createParams->mem_size, VX_MEMORY_TYPE_HOST, VX_READ_AND_WRITE);

          memcpy(&tidlObj->createParams, create_params_target_ptr, sizeof(TIDL_CreateParams));
        }

        if (VX_SUCCESS == status)
        {
            /* reset scratch heap offset to zero by doing a dummy free */
            tivxMemFree(NULL, 0, TIVX_MEM_INTERNAL_L1);
            tivxMemFree(NULL, 0, TIVX_MEM_INTERNAL_L2);
            tivxMemFree(NULL, 0, TIVX_MEM_INTERNAL_L3);

            tivxMemStats(&l1_stats, TIVX_MEM_INTERNAL_L1);
            tivxMemStats(&l2_stats, TIVX_MEM_INTERNAL_L2);
            tivxMemStats(&l3_stats, TIVX_MEM_INTERNAL_L3);

            tidlObj->createParams.l1MemSize = l1_stats.free_size;
            tidlObj->createParams.l2MemSize = l2_stats.free_size;
            tidlObj->createParams.l3MemSize = l3_stats.free_size;

            VX_PRINT(VX_ZONE_INFO, "L1 = %d KB, L2 = %d KB, L3 = %d KB\n",
                l1_stats.free_size/1024,
                l2_stats.free_size/1024,
                l3_stats.free_size/1024
                );

            tidlObj->createParams.udmaDrvObj = tivxPlatformGetDmaObj();

            tidlObj->createParams.net = (sTIDL_Network_t *)tidlObj->tidlNet;

            tidlObj->createParams.traceLogLevel = 0;
            tidlObj->createParams.traceWriteLevel = 0;
            #ifdef TIVX_TIDL_TARGET_DEBUG
            tidlObj->createParams.traceLogLevel = 1;
            tidlObj->createParams.traceWriteLevel = 1;
            #endif
            tidlObj->createParams.TIDLVprintf = tivxKernelTIDLLog;
            tidlObj->createParams.TIDLWriteBinToFile = tivxKernelTIDLDumpToFile;

            tidlObj->algHandle = tivxAlgiVisionCreate
                              (
                                &TIDL_VISION_FXNS,
                                (IALG_Params *)(&tidlObj->createParams)
                              );

            if (NULL == tidlObj->algHandle)
            {
                status = VX_FAILURE;
            }

            tidlObj->inBufs.size     = sizeof(tidlObj->inBufs);
            tidlObj->outBufs.size    = sizeof(tidlObj->outBufs);

            tidlObj->inBufs.bufDesc  = tidlObj->inBufDescList;
            tidlObj->outBufs.bufDesc = tidlObj->outBufDescList;

            tidlObj->inBufs.numBufs  = tidl_AllocNetInputMem(tidlObj->inBufDesc, &tidlObj->tidlParams.ioBufDesc);
            tidlObj->outBufs.numBufs = tidl_AllocNetOutputMem(tidlObj->outBufDesc, &tidlObj->tidlParams.ioBufDesc);

            for(i = 0; i < tidlObj->inBufs.numBufs; i++)
            {
              tidlObj->inBufDescList[i]     = &tidlObj->inBufDesc[i];
            }
            for(i = 0; i < tidlObj->outBufs.numBufs; i++)
            {
              tidlObj->outBufDescList[i]     = &tidlObj->outBufDesc[i];
            }
        }

        tivxMemBufferUnmap(config_target_ptr, config->mem_size, VX_MEMORY_TYPE_HOST, VX_READ_ONLY);

        tivxMemBufferUnmap(network_target_ptr, network->mem_size, VX_MEMORY_TYPE_HOST, VX_READ_ONLY);

        tivxMemBufferUnmap(create_params_target_ptr, createParams->mem_size, VX_MEMORY_TYPE_HOST, VX_READ_AND_WRITE);

        if (VX_SUCCESS == status)
        {
            tivxSetTargetKernelInstanceContext(kernel, tidlObj,  sizeof(tivxTIDLObj));
        }
        else
        {
            #ifdef TIDL_COPY_NETWORK_BUF
            if (NULL != tidlObj->tidlNet)
            {
                tivxMemFree(tidlObj->tidlNet, tidlObj->netSize, TIVX_MEM_EXTERNAL);
            }
            #endif
            if (NULL != tidlObj)
            {
                tivxTIDLFreeMem(tidlObj);
            }
        }
    }

    #ifdef DISABLE_INTERRUPTS_DURING_PROCESS
    VX_PRINT(VX_ZONE_WARNING, "Interrupts DISABLED during TIDL process\n");
    #else
    VX_PRINT(VX_ZONE_WARNING, "Interrupts ENABLED during TIDL process\n");
    #endif

    return (status);
}

static vx_status VX_CALLBACK tivxKernelTIDLDelete(
    tivx_target_kernel_instance kernel, tivx_obj_desc_t *obj_desc[],
    uint16_t num_params, void *priv_arg)
{
    vx_status status = VX_SUCCESS;
    uint32_t i;
    uint32_t size;
    tivxTIDLObj *tidlObj = NULL;

    for (i = 0U; i < num_params; i ++)
    {
        if (NULL == obj_desc[i])
        {
            status = VX_FAILURE;
            break;
        }
    }

    if (VX_SUCCESS == status)
    {
        status = tivxGetTargetKernelInstanceContext(kernel, (void **)&tidlObj, &size);

        if ((VX_SUCCESS == status) && (NULL != tidlObj) && (sizeof(tivxTIDLObj) == size))
        {
            if (tidlObj->algHandle)
            {
                tivxAlgiVisionDelete(tidlObj->algHandle);
            }
            #ifdef TIDL_COPY_NETWORK_BUF
            if (NULL != tidlObj->tidlNet)
            {
                tivxMemFree(tidlObj->tidlNet, tidlObj->netSize, TIVX_MEM_EXTERNAL);
            }
            #endif
            tivxTIDLFreeMem(tidlObj);
        }
    }

    #ifdef TIVX_TIDL_TARGET_DEBUG
    tivx_clr_debug_zone(VX_ZONE_INFO);
    #endif

    return (status);
}

static vx_status VX_CALLBACK tivxKernelTIDLControl(
    tivx_target_kernel_instance kernel, uint32_t node_cmd_id,
    tivx_obj_desc_t *obj_desc[],
    uint16_t num_params, void *priv_arg)
{
    return (VX_SUCCESS);
}

void tivxAddTargetKernelTIDL()
{
    char target_name[TIVX_TARGET_MAX_NAME];
    vx_enum self_cpu;

    self_cpu = tivxGetSelfCpuId();

    if ((self_cpu == TIVX_CPU_ID_DSP_C7_1))
    {
        strncpy(target_name, TIVX_TARGET_DSP_C7_1, TIVX_TARGET_MAX_NAME);

        vx_tidl_target_kernel = tivxAddTargetKernelByName
                                (
                                  TIVX_KERNEL_TIDL_NAME,
                                  target_name,
                                  tivxKernelTIDLProcess,
                                  tivxKernelTIDLCreate,
                                  tivxKernelTIDLDelete,
                                  tivxKernelTIDLControl,
                                  NULL
                                );
    }
}


void tivxRemoveTargetKernelTIDL()
{
    tivxRemoveTargetKernel(vx_tidl_target_kernel);
}

static void tivxTIDLFreeMem(tivxTIDLObj *tidlObj)
{
    if (NULL != tidlObj)
    {
        tivxMemFree(tidlObj, sizeof(tivxTIDLObj), TIVX_MEM_EXTERNAL);
    }
}

static void getQC(uint8_t *pIn, uint8_t *pOut, int32_t inSize)
{
  int32_t i, j;
  uint8_t vec[TIVX_TIDL_J7_CHECKSUM_SIZE];
  int32_t remSize;

  /* Initialize vector */
  for(j = 0; j < TIVX_TIDL_J7_CHECKSUM_SIZE; j++)
  {
     vec[j] = 0;
  }

  /* Create QC */
  remSize = inSize;
  for(i = 0; i < inSize; i+=TIVX_TIDL_J7_CHECKSUM_SIZE)
  {
    int32_t elems;

    if ((remSize - TIVX_TIDL_J7_CHECKSUM_SIZE) < 0)
    {
      elems = TIVX_TIDL_J7_CHECKSUM_SIZE - remSize;
      remSize += TIVX_TIDL_J7_CHECKSUM_SIZE;
    }
    else
    {
      elems = TIVX_TIDL_J7_CHECKSUM_SIZE;
      remSize -= TIVX_TIDL_J7_CHECKSUM_SIZE;
    }

    for(j = 0; j < elems; j++)
    {
      vec[j] ^= pIn[i + j];
    }
  }

  /* Return QC */
  for(j = 0; j < TIVX_TIDL_J7_CHECKSUM_SIZE; j++)
  {
    pOut[j] = vec[j];
  }
}

static vx_status testChecksum(void *dataPtr, uint8_t *refQC, vx_int32 data_size, uint32_t loc)
{
    vx_status status = VX_SUCCESS;

    vx_uint8 qcData[TIVX_TIDL_J7_CHECKSUM_SIZE];
    int32_t match = 1;
    int32_t x;

    /* Get QC of config params passed from host to target */
    getQC(dataPtr, qcData, data_size);

    /* Print QC */
    for(x = 0; x < TIVX_TIDL_J7_CHECKSUM_SIZE; x++)
    {
      if(qcData[x] != refQC[x])
      {
        match = 0;
        break;
      }
    }
    if(match == 0)
    {
      VX_PRINT(VX_ZONE_ERROR, "Computing checksum at 0x%016X, size = %d\n", dataPtr,  data_size);
      VX_PRINT(VX_ZONE_ERROR, "QC code mismatch at %d \n", loc);
      status = VX_FAILURE;
    }
    else
    {
      VX_PRINT(VX_ZONE_INFO, "QC code match! \n");
    }

    return status;
}
