/*
 *******************************************************************************
 *
 * Copyright (C) 2017 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 *******************************************************************************
 */

/**
 * \file vx_tutorial_image_extract_channel.c Extract channels from RGB image using immediate mode
 * then combine them in opposite order.
 *
 * In this tutorial we learn the below concepts,
 *
 * - How to create OpenVX context and OpenVX image data object
 * - How to read a BMP file and load the pixel values into the image data object
 * - How to query the image data object for attributes like width, height
 * - How to use the immediate mode for image processing
 * - How to read pixel values from an image data object and save it as a BMP file
 * - How to cleanup all created resources and exit the OpenVX application
 *
 * To include OpenVX interfaces include below file
 * \code
 * #include <VX/vx.h>
 * \endcode
 *
 * To include OpenVX interfaces for immediate mode include below file
 * \code
 * #include <VX/vxu.h>
 * \endcode
 *
 * Follow the comments in the function vx_tutorial_image_extract_channel()
 * to understand this tutorial
 *
 */


#include <stdio.h>
#include <VX/vx.h>
#include <VX/vxu.h>
#include <utility.h>

/** \brief Input file name */
#define IN_FILE_NAME       "colors.bmp"

/** \brief Output file name */
#define OUT_FILE_NAME      "vx_tutorial_image_extract_channel_out.bmp"

/**
 * \brief Tutorial Entry Point
 */
void vx_tutorial_image_extract_channel()
{
    /**
     * - Define objects that we wish to create in the OpenVX application.
     *
     * A vx_context object is defined which is used as input parameter for all subesquent
     * OpenVX object create APIs
     * \code
     */
    vx_context context;
    vx_image in_image = NULL;
    vx_image r_channel = NULL;
    vx_image g_channel = NULL;
    vx_image b_channel = NULL;
    vx_image out_image = NULL;
    /** \endcode */
    vx_uint32 width, height;

    printf(" vx_tutorial_image_extract_channel: Tutorial Started !!! \n");

    /**
     * - Create OpenVX context.
     *
     * This MUST be done first before any OpenVX API call.
     * The context that is returned is used as input for subsequent OpenVX APIs
     * \code
     */
    context = vxCreateContext();
    /** \endcode */

    printf(" Loading file %s ...\n", IN_FILE_NAME);

    /**
     * - Create image object.
     *
     * Follow the comments in create_image_from_file() to see
     * how a vx_image object is created and filled with RGB data from BMP file \ref IN_FILE_NAME
     * \code
     */
    in_image = create_image_from_file(context, IN_FILE_NAME, vx_false_e);
    /** \endcode */

    vxSetReferenceName((vx_reference)in_image, "INPUT");
    /**
     * - Show image attributes.
     *
     * Follow the comments in show_image_attributes() to see
     * how image attributes are queried and displayed.
     * \code
     */
    show_image_attributes(in_image);
    /** \endcode */

    vxQueryImage(in_image, VX_IMAGE_WIDTH, &width, sizeof(vx_uint32));
    vxQueryImage(in_image, VX_IMAGE_HEIGHT, &height, sizeof(vx_uint32));

    /**
     * - Create OpenVX image object.
     *
     * Creates an OpenVX image object of 'width' x 'height' and having
     * data format 'df'.
     * \code
     */
    r_channel = vxCreateImage(context, width, height, VX_DF_IMAGE_U8);
    /** \endcode */

    vxSetReferenceName((vx_reference)r_channel, "R_CHANNEL");

    /**
     * - Show image attributes.
     *
     * Follow the comments in show_image_attributes() to see
     * how image attributes are queried and displayed.
     * \code
     */
    show_image_attributes(r_channel);
    /** \endcode */

    /**
     * - Create OpenVX image object.
     *
     * Creates an OpenVX image object of 'width' x 'height' and having
     * data format 'df'.
     * \code
     */
    g_channel = vxCreateImage(context, width, height, VX_DF_IMAGE_U8);
    /** \endcode */

    vxSetReferenceName((vx_reference)g_channel, "G_CHANNEL");
    show_image_attributes(g_channel);

    /**
     * - Create OpenVX image object.
     *
     * Creates an OpenVX image object of 'width' x 'height' and having
     * data format 'df'.
     * \code
     */
    b_channel = vxCreateImage(context, width, height, VX_DF_IMAGE_U8);
    /** \endcode */

    vxSetReferenceName((vx_reference)b_channel, "B_CHANNEL");

    /**
     * - Show image attributes.
     *
     * Follow the comments in show_image_attributes() to see
     * how image attributes are queried and displayed.
     * \code
     */
    show_image_attributes(b_channel);
    /** \endcode */

    /**
     * - Create OpenVX image object.
     *
     * Creates an OpenVX image object of 'width' x 'height' and having
     * data format 'df'.
     * \code
     */
    out_image = vxCreateImage(context, width, height, VX_DF_IMAGE_RGB);
    /** \endcode */

    vxSetReferenceName((vx_reference)out_image, "OUTPUT");

    /**
     * - Show image attributes.
     *
     * Follow the comments in show_image_attributes() to see
     * how image attributes are queried and displayed.
     * \code
     */
    show_image_attributes(out_image);
    /** \endcode */

    /**
     * - Performs immediate mode channel extract.
     *
     * The second argument gives the input image, the third argument gives the channel
     * to extract and the final argument gives the output image.
     *
     * \code
     */
    vxuChannelExtract(context, in_image, VX_CHANNEL_R, r_channel);
    vxuChannelExtract(context, in_image, VX_CHANNEL_G, g_channel);
    vxuChannelExtract(context, in_image, VX_CHANNEL_B, b_channel);
    /** \endcode */
    /**
     * - Performs immediate mode channel combine.
     *
     * The second, third and fourth arguments give channels to combine with the last argument
     * being the output image.
     *
     * \code
     */
    vxuChannelCombine(context, b_channel, g_channel, r_channel, NULL, out_image);
    /** \endcode */

    printf(" Saving to file %s ...\n", OUT_FILE_NAME);

    /**
     * - Save image object to bitmap file \ref OUT_FILE_NAME.
     *
     * Follow the comments in save_image_to_file() to see
     * how data in vx_image object is accessed to store pixel values from the image object to
     * BMP file \ref OUT_FILE_NAME
     * \code
     */
    save_image_to_file(OUT_FILE_NAME, out_image);
    /** \endcode */

    /**
     * - Release image object.
     *
     * Since we are done with using this image object, release it
     * \code
     */
    vxReleaseImage(&in_image);
    vxReleaseImage(&r_channel);
    vxReleaseImage(&g_channel);
    vxReleaseImage(&b_channel);
    vxReleaseImage(&out_image);
    /** \endcode */

    /**
     * - Release context object.
     *
     * Since we are done using OpenVX context, release it.
     * No further OpenVX API calls should be done, until a context is again created using
     * vxCreateContext()
     * \code
     */
    vxReleaseContext(&context);
    /** \endcode */

    printf(" vx_tutorial_image_extract_channel: Tutorial Done !!! \n");
    printf(" \n");
}
