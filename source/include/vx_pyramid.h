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



#ifndef VX_PYRAMID_H_
#define VX_PYRAMID_H_


#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \file
 * \brief Implementation of Pyramid object
 */

/*!
 * \brief Max levels supported for the pyramid
 *        Note: If this macro is changed, change #gOrbScaleFactor also
 *              in vx_pyramid file.
 * \ingroup group_vx_pyramid
 */
#define TIVX_PYRAMID_MAX_LEVELS_ORB             (17u)


/*!
 * \brief Pyramid object internal state
 *
 * \ingroup group_vx_pyramid
 */
typedef struct _vx_pyramid
{
    /*! \brief reference object */
    tivx_reference_t base;

    /*! \brief array of image objects */
    vx_image img[TIVX_PYRAMID_MAX_LEVEL_OBJECTS];
} tivx_pyramid_t;


/*!
 * \brief function to initialize virtual pyramid's parameters
 *
 * \param prmd      [in] virtual pyramid reference
 * \param width     [in] base width of image pyramid
 * \param height    [in] base height of image pyramid
 * \param format    [in] image format
 *
 * \return VX_SUCCESS on success
 *
 * \ingroup group_tivx_array
 */
vx_status ownInitVirtualPyramid(
    vx_pyramid prmd, vx_uint32 width, vx_uint32 height, vx_df_image format);

#ifdef __cplusplus
}
#endif

#endif
