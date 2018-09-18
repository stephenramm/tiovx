/*
 *******************************************************************************
 *
 * Copyright (C) 2018 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */


#include <vx_internal.h>

#include <ti/osal/SemaphoreP.h>

vx_status tivxEventCreate(tivx_event *event)
{
    vx_status status = VX_FAILURE;
    SemaphoreP_Handle handle;
    SemaphoreP_Params semParams;

    if (NULL != event)
    {
        /* Default parameter initialization */
        SemaphoreP_Params_init(&semParams);

        handle = SemaphoreP_create(0U, &semParams);

        if (NULL == handle)
        {
            VX_PRINT(VX_ZONE_ERROR, "tivxEventCreate: Semaphore creation failed\n");
            status = VX_FAILURE;
        }
        else
        {
            *event = (tivx_event)handle;
            status = VX_SUCCESS;
        }
    }

    return (status);
}

vx_status tivxEventDelete(tivx_event *event)
{
    vx_status status = VX_FAILURE;
    SemaphoreP_Handle handle;

    if ((NULL != event) && (*event != NULL))
    {
        handle = (SemaphoreP_Handle)*event;
        SemaphoreP_delete(handle);
        *event = NULL;
        status = VX_SUCCESS;
    }

    return (status);
}

vx_status tivxEventPost(tivx_event event)
{
    if (NULL != event)
    {
        SemaphoreP_post((SemaphoreP_Handle)event);
    }

    return (VX_SUCCESS);
}

vx_status tivxEventWait(tivx_event event, uint32_t timeout)
{
    vx_status status = VX_SUCCESS;
    SemaphoreP_Status retVal;
    uint32_t bsp_timeout;

    if (NULL != event)
    {
        if (TIVX_EVENT_TIMEOUT_WAIT_FOREVER == timeout)
        {
            bsp_timeout = SemaphoreP_WAIT_FOREVER;
        }
        else
        {
            bsp_timeout = SemaphoreP_NO_WAIT;
        }

        retVal = SemaphoreP_pend((SemaphoreP_Handle)event, bsp_timeout);

        if (SemaphoreP_OK != retVal)
        {
            VX_PRINT(VX_ZONE_ERROR, "tivxEventWait: Semaphore wait returned an error\n");
            status = VX_FAILURE;
        }
    }
    else
    {
        VX_PRINT(VX_ZONE_ERROR, "tivxEventWait: Event was NULL\n");
        status = VX_FAILURE;
    }

    return (status);
}

vx_status tivxEventClear(tivx_event event)
{
    /* Should call Semaphore_reset (0) */
    return VX_SUCCESS;
}
