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

/**
 * \file tpath.h
 * \brief File Path utilities
 */

#ifndef TPATH_H
#define TPATH_H

#include <yaz/yconfig.h>
#include <stdio.h>

YAZ_BEGIN_CDECL

/** \brief checks whether path is absolute
    \param path path to checked
    \retval 1 path is absolute
    \retval 0 path is relative
*/
YAZ_EXPORT int yaz_is_abspath(const char *path);

/** \brief get next path component in filepath
    \param path_p pointer to path (updated to "next" entry if any)
    \param comp upon pointer to component (if component is found)
    \retval 0 no component found (and no more componennts)
    \retval >0 length of component (length of *comp)

    A filepath has components separted by colon. For example
    /usr/lib/modules:c:/my:/:lib
    which has these components
    "/usr/lib/modules", "c:/my", "/", "lib"
*/
YAZ_EXPORT size_t yaz_filepath_comp(const char **path_p, const char **comp);


/** \brief resolve file on path
    \param fname "short" filename (without path)
    \param path the path (dir1:dir2,..) - ala Unix
    \param base can be added to relative paths (NULL for no append)
    \param fullpath the full path to filename (if successful)
    \returns fullpath or NULL if file could not be found

    Be sure to allocate at least 1024 bytes for fullpath
*/
YAZ_EXPORT char *yaz_filepath_resolve(const char *fname, const char *path,
                                      const char *base, char *fullpath);


/** \brief opens first file in path in path
    \param fname "short" filename (without path)
    \param mode mode as in fopen(3)
    \param path the path (dir1:dir2,..) - ala Unix
    \param base can be added to relative paths (NULL for no append)
    \returns FILE pointer if fname could be found; NULL otherwise
*/
YAZ_EXPORT FILE *yaz_fopen(const char *path, const char *fname,
                           const char *mode, const char *base);

/** \brief opens first file in path in path
    \param fname "short" filename (without path)
    \param mode mode as in fopen(3)
    \param path the path (dir1:dir2,..) - ala Unix
    \returns FILE pointer if fname could be found; NULL otherwise
*/
YAZ_EXPORT FILE *yaz_path_fopen(const char *path, const char *fname,
                                const char *mode);

/** \brief closes file
    \param f FILE handle

    \retval -1 on failure
    \retval 0 on success
*/
YAZ_EXPORT int yaz_fclose(FILE *f);


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

