/*
 *******************************************************************************
 *
 * Copyright (C) 2018-2023 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */


#include <vx_internal.h>

#include <utils/rtos/include/app_rtos.h>

vx_status tivxMutexCreate(tivx_mutex *mutex)
{
    vx_status status = (vx_status)VX_FAILURE;
    app_rtos_semaphore_handle_t handle;
    app_rtos_semaphore_params_t semParams;

    if (NULL != mutex)
    {
        /* Default parameter initialization */
        appRtosSemaphoreParamsInit(&semParams);

        semParams.mode = APP_RTOS_SEMAPHORE_MODE_BINARY;
        semParams.initValue = 1U;

        handle = appRtosSemaphoreCreate(semParams);

        if (NULL == handle)
        {
            status = (vx_status)VX_FAILURE;
            VX_PRINT(VX_ZONE_ERROR, "Semaphore create returned NULL\n");
            VX_PRINT(VX_ZONE_ERROR, "  Check for memory leak, or may need to increase\n");
            #ifdef FREERTOS
            VX_PRINT(VX_ZONE_ERROR, "  the value of OSAL_FREERTOS_MAX_SEMAPHOREP_PER_SOC\n");
            #elif SAFERTOS
            VX_PRINT(VX_ZONE_ERROR, "  the value of OSAL_SAFERTOS_MAX_SEMAPHOREP_PER_SOC\n");
            #endif
            VX_PRINT(VX_ZONE_ERROR, "  in pdk/packages/ti/osal/soc/<>/osal_soc.h\n");
        }
        else
        {
            *mutex = (tivx_mutex)handle;
            status = (vx_status)VX_SUCCESS;
        }
    }

    return (status);
}

vx_status tivxMutexDelete(tivx_mutex *mutex)
{
    vx_status status = (vx_status)VX_FAILURE;
    app_rtos_status_t retVal;
    app_rtos_semaphore_handle_t handle;

    if ((NULL != mutex) && (*mutex != NULL))
    {
        handle = (tivx_mutex)*mutex;
        retVal = appRtosSemaphoreDelete(&handle);

        if (APP_RTOS_STATUS_SUCCESS != retVal)
        {
            VX_PRINT(VX_ZONE_ERROR, "Semaphore delete returned an error\n");
        }
        else
        {
            *mutex = NULL;
            status = (vx_status)VX_SUCCESS;
        }
    }

    return (status);
}

vx_status tivxMutexLock(tivx_mutex mutex)
{
    vx_status status = (vx_status)VX_SUCCESS;
    app_rtos_status_t retVal;

    if (NULL != mutex)
    {
        retVal = appRtosSemaphorePend((app_rtos_semaphore_handle_t)mutex,
            APP_RTOS_SEMAPHORE_WAIT_FOREVER);

        if (APP_RTOS_STATUS_SUCCESS != retVal)
        {
            VX_PRINT(VX_ZONE_ERROR, "Semaphore wait failed\n");
            status = (vx_status)VX_FAILURE;
        }
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "Mutex is NULL\n");
        status = (vx_status)VX_FAILURE;
    }

    return (status);
}

vx_status tivxMutexUnlock(tivx_mutex mutex)
{
    vx_status status = (vx_status)VX_SUCCESS;
    app_rtos_status_t retVal;

    if (NULL != mutex)
    {
        retVal = appRtosSemaphorePost((app_rtos_semaphore_handle_t)mutex);
        if (APP_RTOS_STATUS_SUCCESS != retVal)
        {
            VX_PRINT(VX_ZONE_ERROR, "Semaphore post returned an error\n");
            status = (vx_status)VX_FAILURE;
        }
    }

    return (status);
}
