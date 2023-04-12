/*
 *******************************************************************************
 *
 * Copyright (C) 2018 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

#include <vx_internal.h>
#include <tivx_platform_psdk.h>

#define TIVX_TARGET_DEFAULT_STACK_SIZE  (128U*1024U)

static void ownTargetCreateTargetId(vx_enum target_id, const char *name);
static void ownTargetDeleteTargetId(vx_enum target_id);

static void ownTargetCreateTargetId(vx_enum target_id, const char *name)
{
    vx_status status;
    tivx_target_create_params_t target_create_prms;

    ownTargetSetDefaultCreateParams(&target_create_prms);

    target_create_prms.task_stack_ptr = NULL;
    target_create_prms.task_stack_size = TIVX_TARGET_DEFAULT_STACK_SIZE;
    target_create_prms.task_core_affinity = TIVX_TASK_AFFINITY_ANY;
    target_create_prms.task_priority = 8U;
    strncpy(target_create_prms.task_name, name, TIVX_TARGET_MAX_TASK_NAME);
    target_create_prms.task_name[TIVX_TARGET_MAX_TASK_NAME-1U] = (char)0;

    status = ownTargetCreate(target_id, &target_create_prms);
    if ((vx_status)VX_SUCCESS != status)
    {
        VX_PRINT(VX_ZONE_ERROR, "Could not Add Target %s\n", name);
    }
}

static void ownTargetDeleteTargetId(vx_enum target_id)
{
    vx_status status;

    status = ownTargetDelete(target_id);
    if ((vx_status)VX_SUCCESS != status)
    {
        VX_PRINT(VX_ZONE_ERROR, "Could not Delete Target\n");
    }
}

void ownPlatformCreateTargets(void)
{
    #if defined(SOC_AM62A) && defined(QNX)
    ownTargetCreateTargetId((vx_enum)TIVX_TARGET_ID_CAPTURE1, "TIVX_CAPT1");
    ownTargetCreateTargetId((vx_enum)TIVX_TARGET_ID_CAPTURE2, "TIVX_CAPT2");
    ownTargetCreateTargetId((vx_enum)TIVX_TARGET_ID_CAPTURE3, "TIVX_CAPT3");
    ownTargetCreateTargetId((vx_enum)TIVX_TARGET_ID_CAPTURE4, "TIVX_CAPT4");
    #endif
    ownTargetCreateTargetId((vx_enum)TIVX_TARGET_ID_MPU_0, "TIVX_CPU0");
    ownTargetCreateTargetId((vx_enum)TIVX_TARGET_ID_MPU_1, "TIVX_CPU1");
    ownTargetCreateTargetId((vx_enum)TIVX_TARGET_ID_MPU_2, "TIVX_CPU2");
    ownTargetCreateTargetId((vx_enum)TIVX_TARGET_ID_MPU_3, "TIVX_CPU3");
}

void ownPlatformDeleteTargets(void)
{
    #if defined(SOC_AM62A) && defined(QNX)
    ownTargetDeleteTargetId((vx_enum)TIVX_TARGET_ID_CAPTURE1);
    ownTargetDeleteTargetId((vx_enum)TIVX_TARGET_ID_CAPTURE2);
    ownTargetDeleteTargetId((vx_enum)TIVX_TARGET_ID_CAPTURE3);
    ownTargetDeleteTargetId((vx_enum)TIVX_TARGET_ID_CAPTURE4);
    #endif
    ownTargetDeleteTargetId((vx_enum)TIVX_TARGET_ID_MPU_0);
    ownTargetDeleteTargetId((vx_enum)TIVX_TARGET_ID_MPU_1);
    ownTargetDeleteTargetId((vx_enum)TIVX_TARGET_ID_MPU_2);
    ownTargetDeleteTargetId((vx_enum)TIVX_TARGET_ID_MPU_3);
}
