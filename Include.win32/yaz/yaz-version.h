/* This file is part of the YAZ toolkit.
 * Copyright (C) 1995-2015 Index Data.
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
 * \file yaz-version.h
 * \brief Defines YAZ version.
 */
#ifndef YAZ_VERSION

#include <yaz/yconfig.h>

/** \brief YAZ version as string */
#define YAZ_VERSION "5.15.2"

/** \brief YAZ version as integer (for comparison purposes) */
#define YAZ_VERSIONL 0x50f02

/** \brief YAZ file version for YAZ DLL (resource) */
#define YAZ_FILEVERSION 5,15,2,1

/** \brief SHA1 ID for YAZ (Git) */
#define YAZ_VERSION_SHA1 "738b345708b245e67cded6d917393a80b5bd4eca"

YAZ_BEGIN_CDECL

/** \brief returns YAZ version
    \param version_str holds version upon completion (YAZ_VERSION)
    \param sha1_str holds SHA1 (Git) upon completion (YAZ_VERSION_SHA1)
    \returns long version value (YAZ_VERSIONL)

    The version_str may be NULL in which case version is not returned.
    When not-null, version_str, should point to a buffer of at least 20
    charcters in size (including "\0").
    The sha1_str may be NULL in which case the SHA1 is not returnd.
    When not-null, sha1_str whould point to a buffer of at least 41 characters
    in size (including "\0"). 
  */
YAZ_EXPORT unsigned long yaz_version(char *version_str, char *sha1_str);

YAZ_END_CDECL

#endif

/*
 * Local variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 * vim: shiftwidth=4 tabstop=8 expandtab
 */

