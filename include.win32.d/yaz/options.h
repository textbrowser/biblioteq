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
 * \file options.h
 * \brief Header for command line options parsing utilities
 */

#ifndef YAZ_OPTIONS_H
#define YAZ_OPTIONS_H
#include <yaz/yconfig.h>

YAZ_BEGIN_CDECL


/** \brief command-line options parsing for main
    \arg desc command argument description (allowed options)
    \arg argv main argv
    \arg argc main argc
    \arg arg returned argument (for options that take an argument).
    \retval 0 non-option arg (e.g. filename).
    \retval -1 unknown option (error). arg is name of unknown option
    \retval -2 no more options (end of options)
    \retval c option char

    This function is called repeatedly for each option. Both single
    char options (-x) as well as GNU long options are supported (--long).
    The description is a sequence specs where each spec is if the form:
    [a-zA-Z0-9]({[a-zA-Z0-9]*})+:? . For example: h{help}f{filename}{fname}:

    The first char is what is returned when met (single char option char).
    The second is zero ore more long option values (synonum for single char)
    If colon is appended, it means the option takes an argument.
    If argv includes empty long option (--) that means "end of options" :
    all argv data following that is considered non-options .. (0 returned
    for each).
*/
YAZ_EXPORT int options(const char *desc, char **argv, int argc, char **arg);

#define YAZ_OPTIONS_ERROR (-1)
#define YAZ_OPTIONS_EOF (-2)

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

