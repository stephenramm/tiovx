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
 * Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */


#include <vx_internal.h>

#include <xdc/std.h>
#include <osal/bsp_osal.h>



vx_status tivxQueueCreate(
    tivx_queue *queue, uint32_t max_elements, uint32_t *queue_memory,
    uint32_t flags)
{
    vx_status status = VX_FAILURE;

    if ((NULL != queue) && (NULL != queue_memory) && (0 != max_elements))
    {
        /*
         * init queue to 0's
         */
        memset(queue, 0, sizeof(tivx_queue));

        /*
         * init queue with user parameters
         */
        queue->max_ele = max_elements;
        queue->flags = flags;

        queue->queue = queue_memory;

        if (queue->flags & TIVX_QUEUE_FLAG_BLOCK_ON_GET)
        {
            /*
             * user requested block on que get
             */

            /*
             * create semaphore for it
             */
            status = tivxMutexCreate(&queue->mutex_rd);
        }

        if (queue->flags & TIVX_QUEUE_FLAG_BLOCK_ON_PUT)
        {
            /*
             * user requested block on que put
             */

            /*
             * create semaphore for it
             */
            status = tivxMutexCreate(&queue->mutex_wr);
        }

        if (VX_SUCCESS == status)
        {
            queue->blockedOnGet = vx_false_e;
            queue->blockedOnPut = vx_false_e;
        }
        else
        {
            tivxQueueDelete(queue);
        }
    }

    return (status);
}

vx_status tivxQueueDelete(tivx_queue *queue)
{
    vx_status status = VX_FAILURE;

    if (NULL != queue)
    {
        if (((queue->flags & TIVX_QUEUE_FLAG_BLOCK_ON_GET) ==
                TIVX_QUEUE_FLAG_BLOCK_ON_GET) &&
            (NULL != queue->mutex_rd))
        {
            tivxMutexDelete(&queue->mutex_rd);
        }
        if (((queue->flags & TIVX_QUEUE_FLAG_BLOCK_ON_PUT) ==
                TIVX_QUEUE_FLAG_BLOCK_ON_PUT) &&
            (NULL != queue->mutex_wr))
        {
            tivxMutexDelete(&queue->mutex_wr);
        }

        status = VX_SUCCESS;
    }

    return (status);
}

vx_status tivxQueuePut(tivx_queue *queue, uint32_t data, uint32_t timeout)
{
    vx_status status = VX_FAILURE;
    uint32_t cookie;
    volatile vx_bool do_break = vx_false_e;

    do
    {
        /* disable interrupts */
        cookie = BspOsal_disableInterrupt();

        if (queue->count < queue->max_ele)
        {
            /* insert element */
            queue->queue[queue->cur_wr] = data;

            /* increment put pointer */
            queue->cur_wr = (queue->cur_wr + 1) % queue->max_ele;

            /* increment count of number element in que */
            queue->count++;

            /* restore interrupts */
            BspOsal_restoreInterrupt(cookie);

            /* mark status as success */
            status = VX_SUCCESS;

            if (queue->flags & TIVX_QUEUE_FLAG_BLOCK_ON_GET)
            {
                /* blocking on que get enabled */

                /* post semaphore to unblock, blocked tasks */
                tivxMutexUnlock(queue->mutex_rd);
            }

            /* exit, with success */
            do_break = vx_true_e;

        }
        else
        {
            /* que is full */

            /* restore interrupts */
            BspOsal_restoreInterrupt(cookie);

            if (timeout == TIVX_EVENT_TIMEOUT_NO_WAIT)
            {
                do_break = vx_true_e; /* non-blocking, so exit with error */
            }
            else if (queue->flags & TIVX_QUEUE_FLAG_BLOCK_ON_GET)
            {
                vx_status mutex_status;

                /* blocking on que put enabled */

                /* take semaphore and block until timeout occurs or
                 * semaphore is posted */
                queue->blockedOnPut = vx_true_e;
                mutex_status = tivxMutexLock(queue->mutex_wr);
                queue->blockedOnPut = vx_false_e;
                if (VX_SUCCESS != mutex_status)
                {
                    do_break = vx_true_e;
                    /* error, exit with error */
                }
                else
                {
                    do_break = vx_false_e;
                }
                /* received semaphore, recheck for available space in the que */
            }
            else
            {
                /* blocking on que put disabled */

                /* exit with error */
                do_break = vx_true_e;
            }
        }

        if (vx_true_e == do_break)
        {
            break;
        }
    }
    while (1);

    return (status);
}

vx_status tivxQueueGet(tivx_queue *queue, uint32_t *data, uint32_t timeout)
{
    vx_status status = VX_FAILURE;/* init status to error */
    uint32_t cookie;
    volatile vx_bool do_break = vx_false_e;

    do
    {
        /* disable interrupts */
        cookie = BspOsal_disableInterrupt();

        if (queue->count > 0)
        {
            /* extract the element */
            *data = queue->queue[queue->cur_rd];

            /* increment get pointer */
            queue->cur_rd = (queue->cur_rd + 1) % queue->max_ele;

            /* decrmeent number of elements in que */
            queue->count--;

            /* restore interrupts */
            BspOsal_restoreInterrupt(cookie);

            /* set status as success */
            status = VX_SUCCESS;

            if (queue->flags & TIVX_QUEUE_FLAG_BLOCK_ON_PUT)
            {
                /* blocking on que put enabled,
                 * post semaphore to unblock, blocked tasks
                 */
                tivxMutexUnlock(queue->mutex_wr);
            }

            /* exit with success */
            do_break = vx_true_e;
        }
        else
        {
            /* no elements or not enough element in que to extract */

            /* restore interrupts */
            BspOsal_restoreInterrupt(cookie);

            if (timeout == TIVX_EVENT_TIMEOUT_NO_WAIT)
            {
                do_break = vx_true_e; /* non-blocking, exit with error */
            }
            else
            if (queue->flags & TIVX_QUEUE_FLAG_BLOCK_ON_GET)
            {
                vx_status mutex_status;

                /* blocking on que get enabled */

                /* take semaphore and block until timeout occurs or
                 * semaphore is posted
                 */

                queue->blockedOnGet = vx_true_e;
                mutex_status = tivxMutexLock(queue->mutex_rd);
                queue->blockedOnGet = vx_false_e;
                if (VX_SUCCESS != mutex_status)
                {
                    do_break = vx_true_e; /* exit with error */
                }
                else
                {
                    do_break = vx_false_e;
                }
                /* received semaphore, check que again */
            }
            else
            {
                /* blocking on que get disabled */

                /* exit with error */
                do_break = vx_true_e;
            }
        }

        if (vx_true_e == do_break)
        {
            break;
        }
    }
    while (1);

    return (status);
}

