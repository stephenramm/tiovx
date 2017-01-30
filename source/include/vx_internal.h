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


#ifndef _VX_INTERNAL_H_
#define _VX_INTERNAL_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <VX/vx.h>
#include <TI/tivx.h>
#include <TI/tivx_mem.h>
#include <TI/tivx_obj_desc.h>
#include <TI/tivx_target_kernel.h>
#include <tivx_platform.h>
#include <tivx_ipc.h>
#include <tivx_mutex.h>
#include <tivx_event.h>
#include <tivx_task.h>
#include <tivx_queue.h>
#include <tivx_obj_desc_priv.h>
#include <tivx_target.h>
#include <tivx_host.h>
#include <tivx_target_kernel_priv.h>
#include <tivx_target_kernel_instance.h>


#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \file
 * \brief The top level TI OpenVX implementation header.
 */

/*! \brief Macro to align a 'value' to 'align' units
 * \ingroup group_vx_utils
 */
#define TIVX_ALIGN(value, align)      ((((value)+(align-1))/(align))*(align))

/*! \brief Macro to floor a 'value' to 'align' units
 * \ingroup group_vx_utils
 */
#define TIVX_FLOOR(value, align)      (((value)/(align))*(align))

/*! \brief Macro to specify default alignment to use for stride in Y-direction
 * \ingroup group_vx_utils
 */
#define TIVX_DEFAULT_STRIDE_Y_ALIGN   (16U)

/*! \brief Used to determine if a type is a scalar.
 * \ingroup group_vx_utils
 */
#define TIVX_TYPE_IS_SCALAR(type) ((VX_TYPE_INVALID < (type)) && ((type) < VX_TYPE_SCALAR_MAX))

/*! \brief Used to determine if a type is a struct.
 * \ingroup group_vx_utils
 */
#define TIVX_TYPE_IS_STRUCT(type) (((type) >= VX_TYPE_RECTANGLE) && ((type) < VX_TYPE_KHRONOS_STRUCT_MAX))

/*! \brief Used to determine if a type is an object.
 * \ingroup group_vx_utils
 */
#define TIVX_TYPE_IS_OBJECT(type) (((type) >= VX_TYPE_REFERENCE) && ((type) < VX_TYPE_KHRONOS_OBJECT_END))

/*! \brief A magic value to look for and set in references.
 * \ingroup group_vx_utils
 */
#define TIVX_MAGIC            (0xFACEC0DEU)


/*! \brief A magic value to look for and set in references. Used to indicate a free'ed reference
 * \ingroup group_vx_utils
 */
#define TIVX_BAD_MAGIC        (42)

/*! \brief A parameter checker for size and alignment.
 * \ingroup group_vx_utils
 */
#define VX_CHECK_PARAM(ptr, size, type, align) ((size == sizeof(type)) && (((vx_size)ptr & align) == 0))

/*! \brief Macro to find size of array
 * \ingroup group_vx_utils
 */
#ifndef dimof
#define dimof(x) (sizeof(x)/sizeof(x[0]))
#endif

/*! \brief Macro to check if flag is set, flag MUST be of bit type
 * \ingroup group_vx_utils
 */
static inline vx_bool tivxFlagIsBitSet(uint32_t flag_var, uint32_t flag_val)
{
    return (vx_bool)((flag_var & flag_val) == flag_val);
}

/*! \brief Macro to set flag value, flag MUST be of bit type
 * \ingroup group_vx_utils
 */
static inline void tivxFlagBitSet(uint32_t *flag_var, uint32_t flag_val)
{
    *flag_var |= flag_val;
}

/*! \brief Macro to clear flag value, flag MUST be of bit type
 * \ingroup group_vx_utils
 */
static inline void tivxFlagBitClear(uint32_t *flag_var, uint32_t flag_val)
{
    uint32_t value = *flag_var;

    value = value & ~flag_val;

    *flag_var = value;
}

/*! \brief Macro to convert 2x uint32 to uint64
 * \ingroup group_vx_utils
 */
static inline void tivx_uint32_to_uint64(uint64_t *val, uint32_t h, uint32_t l)
{
    *val = ((uint64_t)h<<32) | (uint64_t)l;
}

/*! \brief Macro to convert uint64 to 2x uint32
 * \ingroup group_vx_utils
 */
static inline void tivx_uint64_to_uint32(uint64_t val, uint32_t *h, uint32_t *l)
{
    *h = (uint32_t)(val >> 32);
    *l = (uint32_t)(val >>  0);
}


#include <vx_debug.h>
#include <vx_reference.h>
#include <vx_context.h>
#include <vx_error.h>
#include <vx_graph.h>
#include <vx_kernel.h>
#include <vx_node.h>
#include <vx_parameter.h>
#include <vx_remap.h>
#include <vx_scalar.h>
#include <vx_image.h>
#include <vx_matrix.h>
#include <vx_lut.h>
#include <vx_convolution.h>
#include <vx_distribution.h>
#include <vx_threshold.h>
#include <vx_pyramid.h>
#include <vx_objarray.h>
#include <vx_array.h>
#include <vx_delay.h>
#include <vx_module.h>
#include <vx_meta_format.h>

#include <tivx_objects.h>

#ifdef __cplusplus
}
#endif

#endif

/*!
 *
 * \mainpage OpenVX - TI Implementation - API Guide
 *
 * \par Introduction
 *
 *      TI's implementation of OpenVX
 *
 * \par IMPORTANT NOTE
 *
 *      See also \ref TI_DISCLAIMER.
 */

/*!
 *
 * \page  TI_DISCLAIMER  TI Disclaimer
 *
 * \htmlinclude ti_disclaim.htm
 *
 */

/*!
 * \defgroup group_tivx_api TI Interface Modules
 */

/*!
 * \defgroup group_tivx_ext TI Extention APIs
 * \ingroup group_tivx_api
 */

/*!
 * \defgroup group_tivx_mem Memory APIs
 * \ingroup group_tivx_api
 */

/*!
 * \defgroup group_tivx_obj_desc Object Descriptor APIs
 * \ingroup group_tivx_api
 */

/*!
 * \defgroup group_tivx_target_kernel Target Kernel APIs
 * \ingroup group_tivx_api
 */

/*!
 * \defgroup group_vx_framework TI OpenVX Implementation Modules
 */

/*!
 * \defgroup group_vx_framework_object Framework Objects
 * \ingroup group_vx_framework
 */

/*!
 * \defgroup group_vx_framework_data_object Data Objects
 * \ingroup group_vx_framework
 */

/*!
 * \defgroup group_vx_context Context APIs
 * \ingroup group_vx_framework_object
 */

/*!
 * \defgroup group_vx_graph Graph APIs
 * \ingroup group_vx_framework_object
 */

/*!
 * \defgroup group_vx_node Node APIs
 * \ingroup group_vx_framework_object
 */

/*!
 * \defgroup group_vx_kernel Kernel APIs
 * \ingroup group_vx_framework_object
 */

/*!
 * \defgroup group_vx_parameter Parameter APIs
 * \ingroup group_vx_framework_object
 */

/*!
 * \defgroup group_vx_reference Reference APIs
 * \ingroup group_vx_framework_object
 */

/*!
 * \defgroup group_tivx_target Target APIs
 * \ingroup group_vx_framework_object
 */

/*!
 * \defgroup group_tivx_target_kernel_priv Target Kernel APIs
 * \ingroup group_vx_framework_object
 */

/*!
 * \defgroup group_tivx_target_kernel_instance Target Kernel Instance APIs
 * \ingroup group_vx_framework_object
 */

/*!
 * \defgroup group_tivx_obj_desc_priv Object Descriptor APIs
 * \ingroup group_vx_framework_object
 */

/*!
 * \defgroup group_tivx_obj Object APIs
 * \ingroup group_vx_framework_object
 */

/*!
 * \defgroup group_tivx_host Host APIs
 * \ingroup group_vx_framework_object
 */


/*!
 * \defgroup group_vx_module Module APIs
 * \ingroup group_vx_framework_object
 */

/*!
 * \defgroup group_vx_image Image APIs
 * \ingroup group_vx_framework_data_object
 */

/*!
 * \defgroup group_vx_scalar Scalar APIs
 * \ingroup group_vx_framework_data_object
 */

/*!
 * \defgroup group_vx_remap Remap APIs
 * \ingroup group_vx_framework_data_object
 */

/*!
 * \defgroup group_vx_delay Delay APIs
 * \ingroup group_vx_framework_data_object
 */

/*!
 * \defgroup group_vx_objarray Object Array APIs
 * \ingroup group_vx_framework_data_object
 */

/*!
 * \defgroup group_vx_array Array APIs
 * \ingroup group_vx_framework_data_object
 */

/*!
 * \defgroup group_vx_convolution Convolution APIs
 * \ingroup group_vx_framework_data_object
 */

/*!
 * \defgroup group_vx_distribution Distribution APIs
 * \ingroup group_vx_framework_data_object
 */

/*!
 * \defgroup group_vx_lut LUT APIs
 * \ingroup group_vx_framework_data_object
 */

/*!
 * \defgroup group_vx_matrix Matrix APIs
 * \ingroup group_vx_framework_data_object
 */

/*!
 * \defgroup group_vx_pyramid Pyramid APIs
 * \ingroup group_vx_framework_data_object
 */

/*!
 * \defgroup group_vx_threshold Threshold APIs
 * \ingroup group_vx_framework_data_object
 */

/*!
 * \defgroup group_vx_framework_config Framework Configuration Parameters
 * \ingroup group_vx_framework
 */

/*!
 * \defgroup group_vx_context_cfg Context Configuration
 * \ingroup group_vx_framework_config
 */

/*!
 * \defgroup group_vx_graph_cfg Graph Configuration
 * \ingroup group_vx_framework_config
 */

/*!
 * \defgroup group_tivx_target_cfg Target Configuration
 * \ingroup group_vx_framework_config
 */

/*!
 * \defgroup group_tivx_target_kernel_cfg Target Kernel Configuration
 * \ingroup group_vx_framework_config
 */

/*!
 * \defgroup group_tivx_target_kernel_instance_cfg Target Kernel Instance Configuration
 * \ingroup group_vx_framework_config
 */

/*!
 * \defgroup group_vx_module_cfg Module Configuration Configuration
 * \ingroup group_vx_framework_config
 */

/*!
 * \defgroup group_tivx_obj_desc_cfg Object Descriptor Configuration
 * \ingroup group_vx_framework_config
 */

/*!
 * \defgroup group_vx_image_cfg Data Object: Image Configuration
 * \ingroup group_vx_framework_config
 */

/*!
 * \defgroup group_vx_array_cfg Data Object: Array Configuration
 * \ingroup group_vx_framework_config
 */

/*!
 * \defgroup group_vx_delay_cfg Data Object: Delay Configuration
 * \ingroup group_vx_framework_config
 */

/*!
 * \defgroup group_tivx_obj_cfg Object Configuration
 * \ingroup group_vx_framework_config
 */


/*!
 * \defgroup group_vx_framework_utils Utility and Debug Modules
 * \ingroup group_vx_framework
 */

/*!
 * \defgroup group_vx_utils Utility APIs
 * \ingroup group_vx_framework_utils
 */

/*!
 * \defgroup group_vx_debug Debug APIs
 * \ingroup group_vx_framework_utils
 */

/*!
 * \defgroup group_vx_error Error APIs
 * \ingroup group_vx_framework_utils
 */

/*!
 * \defgroup group_vx_platform TI Platform Modules
 */

/*!
 * \defgroup group_tivx_queue Queue APIs
 * \ingroup group_vx_platform
 */

/*!
 * \defgroup group_tivx_ipc Inter-Processor Communication (IPC) APIs
 * \ingroup group_vx_platform
 */

/*!
 * \defgroup group_tivx_platform Platform APIs
 * \ingroup group_vx_platform
 */

/*!
 * \defgroup group_tivx_mutex Mutex APIs
 * \ingroup group_vx_platform
 */

/*!
 * \defgroup group_tivx_event Event APIs
 * \ingroup group_vx_platform
 */

/*!
 * \defgroup group_tivx_task Task APIs
 * \ingroup group_vx_platform
 */

