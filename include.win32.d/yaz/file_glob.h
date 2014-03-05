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

/** \file
    \brief File globbing (ala POSIX glob, but simpler)
*/

#ifndef YAZ_FILE_GLOB_H
#define YAZ_FILE_GLOB_H

#include <yaz/yconfig.h>
#include <stdio.h>

YAZ_BEGIN_CDECL

/** \brief file glob handle */
typedef struct glob_res *yaz_glob_res_t;

#define YAZ_FILE_GLOB_FAIL_NOTEXIST 1

/** \brief perform glob
    \param pattern glob pattern file spec
    \param res returned glob result
    \retval 0 OK
    \retval -1 ERROR
*/
YAZ_EXPORT
int yaz_file_glob(const char *pattern, yaz_glob_res_t *res);

/** \brief perform glob (with flags)
    \param pattern glob pattern file spec
    \param res returned glob result
    \param flags YAZ_FILE_GLOB_.. flags
    \retval 0 OK
    \retval -1 ERROR
*/
YAZ_EXPORT
int yaz_file_glob2(const char *pattern, yaz_glob_res_t *res, unsigned flags);

/** \brief release glob result
    \param res pointer to glob result

    A value of *res == NULL is allowed. If *res != NULL, then
    *res is set to NULL
    */
YAZ_EXPORT
void yaz_file_globfree(yaz_glob_res_t *res);

/** \brief return resulting matching file
    \param res glob result
    \param idx index 0=first, .. N-1 (where N is yaz_file_glob_get_num)
    \returns file name or NULL if idx is out-of-range
*/
YAZ_EXPORT
const char *yaz_file_glob_get_file(yaz_glob_res_t res, size_t idx);

/** \brief return number of matching files
    \param res glob result
    \returns number of files
*/
YAZ_EXPORT
size_t yaz_file_glob_get_num(yaz_glob_res_t res);

YAZ_END_CDECL

#endif
/*
 * Local variables:
 * c-basic-offset: 4
 * c-file-style: "Stroustrup"
 * indent-tabs-mode: nil
 * End:
 * vim: shiftwidth=4 tabstop=8 expandtab
 */

