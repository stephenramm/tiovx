/*
 *******************************************************************************
 *
 * Copyright (C) 2018 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */
#include <vx_internal.h>
#include <tivx_platform_psdk_j7.h>

void tivxRegisterOpenVXCoreTargetKernels(void);
void tivxUnRegisterOpenVXCoreTargetKernels(void);
void tivxRegisterIVisionTargetKernels(void);
void tivxUnRegisterIVisionTargetKernels(void);
void tivxRegisterTutorialTargetKernels(void);
void tivxUnRegisterTutorialTargetKernels(void);
void tivxRegisterCaptureTargetArmKernels(void);
void tivxUnRegisterCaptureTargetArmKernels(void);
void tivxRegisterTestKernelsTargetC66Kernels(void);
void tivxUnRegisterTestKernelsTargetC66Kernels(void);
void tivxRegisterTestKernelsTargetArmKernels();
void tivxUnRegisterTestKernelsTargetArmKernels();

static void tivxInitLocal(void);
static void tivxDeInitLocal(void);

/* Counter for tracking the {init, de-init} calls. This is also used to
 * guarantee a single init/de-init operation.
 */
static uint32_t g_init_status = 0U;

#if defined(LINUX) || defined(QNX)
#include <pthread.h>

/* Mutex for controlling access to Init/De-Init. */
static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

void tivxInit(void)
{
    pthread_mutex_lock(&g_mutex);

    tivxInitLocal();

    pthread_mutex_unlock(&g_mutex);

}

void tivxDeInit(void)
{
    pthread_mutex_lock(&g_mutex);

    tivxDeInitLocal();

    pthread_mutex_unlock(&g_mutex);
}

#else
void tivxInit(void)
{
    tivxInitLocal();
}

void tivxDeInit(void)
{
    tivxDeInitLocal();
}
#endif // defined(LINUX) || defined(QNX)

static void tivxInitLocal(void)
{
    if (0U == g_init_status)
    {
        tivx_set_debug_zone((int32_t)VX_ZONE_INIT);
        tivx_set_debug_zone((int32_t)VX_ZONE_ERROR);
        tivx_set_debug_zone((int32_t)VX_ZONE_WARNING);
        tivx_clr_debug_zone((int32_t)VX_ZONE_INFO);

        /* Initialize resource logging */
        tivxLogResourceInit();

        /* Initialize platform */
        tivxPlatformInit();

        /* Initialize Target */
        tivxTargetInit();

        /* Initialize Host */
    #if defined (C66)
        tivxRegisterOpenVXCoreTargetKernels();
        #ifdef BUILD_TUTORIAL
        tivxRegisterTutorialTargetKernels();
        #endif
    #endif

    #ifdef BUILD_CONFORMANCE_TEST
    #if defined (C66)
        tivxRegisterCaptureTargetArmKernels();
        tivxRegisterTestKernelsTargetArmKernels();
    #endif

    #if defined (C66)
        tivxRegisterTestKernelsTargetC66Kernels();
    #endif
    #endif

        tivxObjDescInit();

        tivxPlatformCreateTargets();

        VX_PRINT(VX_ZONE_INIT, "Initialization Done !!!\n");
    }

    g_init_status++;
}

static void tivxDeInitLocal(void)
{
    if (0U != g_init_status)
    {
        g_init_status--;

        if (0U == g_init_status)
        {
            tivxPlatformDeleteTargets();

            /* DeInitialize Host */
        #if defined (C66)
            tivxUnRegisterOpenVXCoreTargetKernels();
            #ifdef BUILD_TUTORIAL
            tivxUnRegisterTutorialTargetKernels();
            #endif
        #endif

        #ifdef BUILD_CONFORMANCE_TEST
        #if defined (C66)
            tivxUnRegisterCaptureTargetArmKernels();
            tivxUnRegisterTestKernelsTargetArmKernels();
        #endif

        #if defined (C66)
            tivxUnRegisterTestKernelsTargetC66Kernels();
        #endif
        #endif

            /* DeInitialize Target */
            tivxTargetDeInit();

            /* DeInitialize platform */
            tivxPlatformDeInit();

            /* DeInitialize resource logging */
            tivxLogResourceDeInit();

            VX_PRINT(VX_ZONE_INIT, "De-Initialization Done !!!\n");
        }
    }
    else
    {
        /* ERROR. */
        VX_PRINT(VX_ZONE_ERROR, "De-Initialization Error !!!\n");
    }
}
