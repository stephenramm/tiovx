/*
 *******************************************************************************
 *
 * Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */


#ifndef _TIVX_KENREL_ABSDIFF_
#define _TIVX_KENREL_ABSDIFF_


#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \file
 * \brief Interface file for lut kernel
 */


/*!
 * \brief Index of the input image
 *
 * \ingroup group_tivx_ext
 */
#define TIVX_KERNEL_ABSDIFF_IN0_IMG_IDX          (0U)

/*!
 * \brief Index of the input image
 *
 * \ingroup group_tivx_ext
 */
#define TIVX_KERNEL_ABSDIFF_IN1_IMG_IDX          (1U)

/*!
 * \brief Index of the output image
 *
 * \ingroup group_tivx_ext
 */
#define TIVX_KERNEL_ABSDIFF_OUT_IMG_IDX          (2U)

/*!
 * \brief Index of the output image
 *
 * \ingroup group_tivx_ext
 */
#define TIVX_KERNEL_ABSDIFF_MAX_PARAMS              (3U)

#ifdef __cplusplus
}
#endif

#endif
