/*
 *******************************************************************************
 *
 * Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

#include <vx_internal.h>

void tivxRegisterOpenVXCoreKernels();
void tivxUnRegisterOpenVXCoreKernels();

void tivxHostInit(void)
{
    tivxObjectInit();
    tivxObjDescInit();
    tivxRegisterOpenVXCoreKernels();
}

void tivxHostDeInit(void)
{
    tivxObjectDeInit();
    tivxUnRegisterOpenVXCoreKernels();
}
