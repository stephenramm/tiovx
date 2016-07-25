/*
 *******************************************************************************
 *
 * Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */


#ifndef _TIVX_TASK_H_
#define _TIVX_TASK_H_

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \file
 * \brief Interface to Task APIs
 */


/*!
 * \brief constant to indicate task affinity can be decided by OS on a SMP CPU
 *
 * \ingroup group_tivx_task
 */
#define TIVX_TASK_AFFINITY_ANY      (0xFFFFu)

/*!
 * \brief Highest task priority for the OS
 *
 * \ingroup group_tivx_task
 */
#define TIVX_TASK_PRIORITY_HIGHEST      (0u)

/*!
 * \brief Lowest task priority for the OS
 *
 * \ingroup group_tivx_task
 */
#define TIVX_TASK_PRIORITY_LOWEST       (16u)

/*!
 * \brief Typedef for a task
 *
 * \ingroup group_tivx_task
 */
typedef struct _tivx_task_t *tivx_task;


/*! \brief Entry point of task
 *
 *  \param target [in] target handle
 *
 * \ingroup group_tivx_target
 */
typedef void (VX_CALLBACK *tivx_task_main_f)(void *app_handle);


/*!
 * \brief Parameters that can be set during task creation
 *
 * \ingroup group_tivx_task
 */
typedef struct _tivx_task_create_params
{
    /*! \brief Pointer to task stack, if NULL then task stack
     *         is allcoated by OS and not supplied by user
     */
    uint32_t *stack_ptr;

    /*! \brief Task stack size, if 0, OS allocates stack with default size
     */
    uint32_t stack_size;

    /*! \brief If task runs on a SMP CPU then this value tells the affinity
     *         of task to a given core,
     *         Valid values are 0 .. max cores in the SMP CPU.
     *         when TIVX_TASK_AFFINITY_ANY is used OS decides the task affinity.
     */
    uint32_t core_affinity;

    /*! \brief task priority for task associated with this target
     *         TIVX_TASK_PRI_HIGHEST is highest priority,
     *         TIVX_TASK_PRI_LOWEST is lowest priority
     */
    uint32_t priority;

    /*! \brief Entry point for task */
    tivx_task_main_f task_main;

} tivx_task_create_params_t;




/*!
 * \brief Used to set default task create parameters in parameter structure
 *
 * \param params [out] Params to use for task creation
 *
 * \ingroup group_tivx_target
 */
void tivxTaskSetDefaultCreateParams(tivx_task_create_params_t *params);

/*!
 * \brief Create a task
 *
 * \param task [out] Pointer to task object
 * \param params [in] Task create parameters
 *
 * \return VX_SUCCESS on success
 *
 * \ingroup group_tivx_task
 */
vx_status tivxTaskCreate(tivx_task *task, tivx_task_create_params_t *params);

/*!
 * \brief Delete a task
 *
 * \param task [in] Pointer to task object
 *
 * \return VX_SUCCESS on success
 *
 * \ingroup group_tivx_task
 */
vx_status tivxTaskDelete(tivx_task *task);


#ifdef __cplusplus
}
#endif

#endif
