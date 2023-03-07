/*
 * Copyright 2018 NXP
 * Copyright (c) 2015-2018 Cadence Design Systems, Inc.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*******************************************************************************
 * xa-factory.c
 *
 * DSP processing framework core - component factory
 ******************************************************************************/

#define MODULE_TAG                      FACTORY

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "xa_type_def.h"

/* ...platform run-time */
#include "xaf_test.h"

/* ...debugging facility */
#include "xf-debug.h"
#include "xaf-api.h"
#include "xa_type_def.h"

/*******************************************************************************
 * Tracing tags
 ******************************************************************************/

/* ...general initialization sequence */
TRACE_TAG(INIT, 1);

/*******************************************************************************
 * Local types definitions
 ******************************************************************************/

/* ...component descriptor */
typedef struct xf_component_id
{
    /* ...class id (string identifier) */
    const char         *id;

    /* ...class constructor */
    void *  (*factory)(UWORD32 core, xa_codec_func_t process, xaf_comp_type comp_type);

    /* ...component API function */
    xa_codec_func_t    *process;

}   xf_component_id_t;

/*******************************************************************************
 * External functions
 ******************************************************************************/

/* ...components API functions */
extern XA_ERRORCODE xa_mp3_decoder(xa_codec_handle_t, WORD32, WORD32, pVOID);
extern XA_ERRORCODE xa_aac_decoder(xa_codec_handle_t, WORD32, WORD32, pVOID);
extern XA_ERRORCODE xa_mixer(xa_codec_handle_t, WORD32, WORD32, pVOID);
extern XA_ERRORCODE xa_pcm_gain(xa_codec_handle_t, WORD32, WORD32, pVOID);
extern XA_ERRORCODE xa_src_pp_fx(xa_codec_handle_t, WORD32, WORD32, pVOID);
extern XA_ERRORCODE xa_renderer(xa_codec_handle_t , WORD32 , WORD32 , pVOID);
extern XA_ERRORCODE xa_capturer(xa_codec_handle_t , WORD32 , WORD32 , pVOID);
extern XA_ERRORCODE xa_pcm_split(xa_codec_handle_t, WORD32, WORD32, pVOID);

/* ...component class factories */
extern void * xa_audio_codec_factory(UWORD32 core, xa_codec_func_t process, xaf_comp_type comp_type);
extern void * xa_mixer_factory(UWORD32 core, xa_codec_func_t process, xaf_comp_type comp_type);
extern void * xa_renderer_factory(UWORD32 core, xa_codec_func_t process,xaf_comp_type comp_type);
extern void * xa_capturer_factory(UWORD32 core, xa_codec_func_t process,xaf_comp_type comp_type);
extern void * xa_mimo_proc_factory(UWORD32 core, xa_codec_func_t process, xaf_comp_type comp_type);

/*******************************************************************************
 * Local constants definitions
 ******************************************************************************/

const char *comp_id[] = {"audio-decoder",
                         "audio-encoder",
                         "mixer",
                         "pre-proc",
                         "post-proc",
                         "renderer",
                         "capturer",
                         "mimo-proc12",
                         "mimo-proc21",
                         "mimo-proc22",
                         "mimo-proc23" };

/* ...component class id */
static const xf_component_id_t xf_component_id[] =
{
#if XA_MP3_DECODER
    { "audio-decoder/mp3",       xa_audio_codec_factory,     xa_mp3_decoder },
#endif
#if XA_MIXER
    { "mixer",                   xa_mixer_factory,           xa_mixer },
#endif
#if XA_PCM_GAIN
    { "post-proc/pcm_gain",      xa_audio_codec_factory,     xa_pcm_gain },
#endif
#if XA_AAC_DECODER
    { "audio-decoder/aac",      xa_audio_codec_factory,     xa_aac_decoder },
#endif
#if XA_RENDERER
    { "renderer",              xa_renderer_factory,        xa_renderer },
#endif
#if XA_CAPTURER
    { "capturer",              xa_capturer_factory,        xa_capturer },
#endif
#if XA_SRC_PP_FX
    { "audio-fx/src-pp",        xa_audio_codec_factory,     xa_src_pp_fx },
#endif
#if XA_PCM_SPLIT
    { "mimo-proc12/pcm_split",  xa_mimo_proc_factory,       xa_pcm_split },
#endif
};

/* ...number of items in the map */
#define XF_COMPONENT_ID_MAX     (sizeof(xf_component_id) / sizeof(xf_component_id[0]))

/*******************************************************************************
 * Global definition
 ******************************************************************************/
const int xf_io_ports[XAF_MAX_COMPTYPE][2] =
{
    {1, 1},     /* XAF_DECODER      */
    {1, 1},     /* XAF_ENCODER      */
    {4, 1},     /* XAF_MIXER        */
    {1, 1},     /* XAF_PRE_PROC     */
    {1, 1},     /* XAF_POST_PROC    */
    {1, 0},     /* XAF_RENDERER     */
    {0, 1},     /* XAF_CAPTURER     */
    {1, 2},     /* XAF_MIMO_PROC_12 */
    {2, 1},     /* XAF_MIMO_PROC_21 */
    {2, 2},     /* XAF_MIMO_PROC_22 */
    {2, 3},     /* XAF_MIMO_PROC_23 */
};

/*******************************************************************************
 * Enry points
 ******************************************************************************/

void * xf_component_factory(UWORD32 core, xf_id_t id, UWORD32 length)
{
    UWORD32     i;
    xaf_comp_type comp_type;

    /* ...find component-id in static map */
    for (i = 0; i < XF_COMPONENT_ID_MAX; i++)
    {
        for(comp_type = XAF_DECODER; comp_type < XAF_MAX_COMPTYPE; comp_type++)
        {
            if(NULL != strstr(xf_component_id[i].id, comp_id[comp_type]))
                break;
        }

        /* ...symbolic search - not too good; would prefer GUIDs in some form */
        if (!strncmp(id, xf_component_id[i].id, length))
        {
            /* ...pass control to specific class factory */
            return xf_component_id[i].factory(core, xf_component_id[i].process, comp_type);
        }
    }

    /* ...component string id is not recognized */
    TRACE(ERROR, _b("Unknown component type: %s"), id);

    return NULL;
}
