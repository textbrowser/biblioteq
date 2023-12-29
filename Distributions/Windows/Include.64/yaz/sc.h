/* This file is part of the YAZ toolkit.
 * Copyright (C) Index Data.
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Index Data nor the names of its contributors
 *       may be used to endorse or promote products derived from this
 *       software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** \file sc.h
    \brief Header for Windows Service Control utility

    This is a simple wrapper for the Windows Service Control system. It
    also operates on Unix in which case the user supplied main function
    (sc_main) is is called immediately. See sc_test.c for an example.
 */

#include <yaz/yconfig.h>

YAZ_BEGIN_CDECL

/** service control handle (opaque type) */
typedef struct sc_s *yaz_sc_t;

/** \brief creates service handle
    \param service_name Service Name
    \param display_name Display Name
    \return service control handle

    This function does not activate the service. Only creates handle.
*/
YAZ_EXPORT yaz_sc_t yaz_sc_create(const char *service_name,
                                  const char *display_name);

/** \brief registers service controlled program
    \param s service control handle
    \param argc argc as given from main
    \param argv argv as given from main
    \param sc_main service main function
    \param sc_stop service stop function
    \return sc_main return value

    sc_main is the main program of the application. It should initialize
    the application.. Function yaz_sc_running MUST be called before
    the application operates (after initialization)
*/
YAZ_EXPORT int yaz_sc_program(yaz_sc_t s, int argc, char **argv,
	 		      int (*sc_main)(yaz_sc_t s, int argc, char **argv),
			      void (*sc_stop)(yaz_sc_t s));

/** \brief signals that sc_main applicatio starts running
    \param s service control handle
*/
YAZ_EXPORT void yaz_sc_running(yaz_sc_t s);

/** \brief frees service control handle
    \param s service control handle
*/
YAZ_EXPORT void yaz_sc_destroy(yaz_sc_t *s);

YAZ_END_CDECL

/*
 * Local variables:
 * c-basic-offset: 4
 * c-file-style: "Stroustrup"
 * indent-tabs-mode: nil
 * End:
 * vim: shiftwidth=4 tabstop=8 expandtab
 */

