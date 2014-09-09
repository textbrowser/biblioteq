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
 * \file odr.h
 * \brief Header for ODR (Open Data Representation)
 */

#ifndef ODR_H
#define ODR_H

#include <stdio.h>
#include <string.h>

#include <yaz/yconfig.h>
#include <yaz/nmem.h>

/* for definition of Odr_oid */
#include <yaz/oid_util.h>

YAZ_BEGIN_CDECL

typedef nmem_int_t Odr_int;
typedef nmem_bool_t Odr_bool;
#define ODR_INT_PRINTF NMEM_INT_PRINTF

#ifndef bool_t
#define bool_t int
#endif

/*
 * Tag modes
 */
#define ODR_NONE -1
#define ODR_IMPLICIT 0
#define ODR_EXPLICIT 1

/*
 * Classes
 */
#define ODR_UNIVERSAL   0
#define ODR_APPLICATION 1
#define ODR_CONTEXT     2
#define ODR_PRIVATE     3

/*
 * UNIVERSAL tags
 */
#define ODR_BOOLEAN     1
#define ODR_INTEGER     2
#define ODR_BITSTRING   3
#define ODR_OCTETSTRING 4
#define ODR_NULL        5
#define ODR_OID         6
#define ODR_ODESC       7
#define ODR_EXTERNAL    8
#define ODR_REAL        9
#define ODR_ENUM        10
#define ODR_SEQUENCE    16
#define ODR_SET         17
#define ODR_NUMERICSTRING   18
#define ODR_PRINTABLESTRING 19
#define ODR_GENERALIZEDTIME 24
#define ODR_GRAPHICSTRING   25
#define ODR_VISIBLESTRING   26
#define ODR_GENERALSTRING   27

/*
 * odr stream directions
 */
#define ODR_DECODE      0
#define ODR_ENCODE      1
#define ODR_PRINT       2

typedef struct odr_oct
{
    char *buf;
    int len;
} Odr_oct;

typedef void Odr_null;
extern Odr_null *ODR_NULLVAL;

typedef Odr_oct Odr_any;

typedef struct odr_bitmask
{
#define ODR_BITMASK_SIZE 256
    char bits[ODR_BITMASK_SIZE];
    int top;
} Odr_bitmask;

#define ODR_S_SET     0
#define ODR_S_CUR     1
#define ODR_S_END     2

typedef struct odr *ODR;

/** ODR handle and the public structs */
struct odr
{
    int direction;       /* the direction of this stream */

    int error;            /* current error state (0==OK) */

    NMEM mem;            /* memory handle for decoding (primarily) */

    struct Odr_private *op;
};

typedef int (*Odr_fun)(ODR, char **, int, const char *);

typedef struct odr_arm
{
    int tagmode;
    int zclass;
    int tag;
    int which;
    Odr_fun fun;
    char *name;
} Odr_arm;

/*
 * Error control.
 */
#define ONONE           0
#define OMEMORY         1
#define OSYSERR         2
#define OSPACE          3
#define OREQUIRED       4
#define OUNEXPECTED     5
#define OOTHER          6
#define OPROTO          7
#define ODATA           8
#define OSTACK          9
#define OCONLEN        10
#define OLENOV         11
#define OHTTP          12

extern char *odr_errlist[];

YAZ_EXPORT int odr_offset(ODR o);
YAZ_EXPORT int odr_geterror(ODR o);
YAZ_EXPORT int odr_geterrorx(ODR o, int *x);
YAZ_EXPORT void odr_seterror(ODR o, int errorno, int errorid);
YAZ_EXPORT void odr_setelement(ODR o, const char *addinfo);
YAZ_EXPORT const char *odr_getelement(ODR o);
YAZ_EXPORT void odr_perror(ODR o, const char *message);
YAZ_EXPORT void odr_setprint(ODR o, FILE *file);
YAZ_EXPORT void odr_setprint_noclose(ODR o, FILE *file);
YAZ_EXPORT ODR odr_createmem(int direction);
YAZ_EXPORT void odr_reset(ODR o);
YAZ_EXPORT void odr_destroy(ODR o);
YAZ_EXPORT void odr_setbuf(ODR o, char *buf, int len, int can_grow);
YAZ_EXPORT char *odr_getbuf(ODR o, int *len, int *size);
YAZ_EXPORT void *odr_malloc(ODR o, size_t size);
YAZ_EXPORT char *odr_strdup(ODR o, const char *str);
YAZ_EXPORT char *odr_strdupn(ODR o, const char *str, size_t n);
YAZ_EXPORT char *odr_strdup_null(ODR o, const char *str);
YAZ_EXPORT Odr_int *odr_intdup(ODR o, Odr_int v);
YAZ_EXPORT Odr_bool *odr_booldup(ODR o, Odr_bool v);
YAZ_EXPORT Odr_oct *odr_create_Odr_oct(ODR o, const char *buf, int sz);
YAZ_EXPORT NMEM odr_extract_mem(ODR o);
YAZ_EXPORT Odr_null *odr_nullval(void);
#define odr_release_mem(m) nmem_destroy(m)
#define ODR_MEM NMEM

#define odr_implicit_tag(o, t, p, cl, tg, opt, name)\
        (odr_implicit_settag((o), cl, tg), t ((o), (p), (opt), name) )

#define odr_explicit_tag(o, t, p, cl, tg, opt, name)\
        ((int) (odr_constructed_begin((o), (p), (cl), (tg), 0) ? \
        t ((o), (p), (opt), name) &&\
        odr_constructed_end(o) : odr_missing((o), opt, name)))

#define ODR_MASK_ZERO(mask)\
    ((void) (memset((mask)->bits, 0, ODR_BITMASK_SIZE),\
    (mask)->top = -1))

#define ODR_MASK_SET(mask, num)\
    (((mask)->bits[(num) >> 3] |= 0X80 >> ((num) & 0X07)),\
    (void) ((mask)->top < (num) >> 3 ? ((mask)->top = (num) >> 3) : 0))

#define ODR_MASK_CLEAR(mask, num)\
    ((mask)->bits[(num) >> 3] &= ~(0X80 >> ((num) & 0X07)))

#define ODR_MASK_GET(mask, num)  ( ((num) >> 3 <= (mask)->top) ? \
    ((mask)->bits[(num) >> 3] & (0X80 >> ((num) & 0X07)) ? 1 : 0) : 0)


#define odr_ok(o) (!(o)->error)
#define odr_getmem(o) ((o)->mem)

#define ODR_MAXNAME 256

YAZ_EXPORT int ber_boolean(ODR o, int *val);
YAZ_EXPORT int ber_tag(ODR o, void *p, int zclass, int tag,
                       int *constructed, int opt, const char *name);
YAZ_EXPORT int ber_enctag(ODR o, int zclass, int tag, int constructed);
YAZ_EXPORT int ber_dectag(const char *buf, int *zclass,
                          int *tag, int *constructed, int max);
YAZ_EXPORT int odr_bool(ODR o, Odr_bool **p, int opt, const char *name);
YAZ_EXPORT int odr_integer(ODR o, Odr_int **p, int opt, const char *name);
YAZ_EXPORT int odr_enum(ODR o, Odr_int **p, int opt, const char *name);
YAZ_EXPORT int odr_implicit_settag(ODR o, int zclass, int tag);
YAZ_EXPORT int ber_enclen(ODR o, int len, int lenlen, int exact);
YAZ_EXPORT int ber_declen(const char *buf, int *len, int max);
YAZ_EXPORT void odr_prname(ODR o, const char *name);
YAZ_EXPORT int ber_null(ODR o);
YAZ_EXPORT int odr_null(ODR o, Odr_null **p, int opt, const char *name);
YAZ_EXPORT int ber_integer(ODR o, Odr_int *val);
YAZ_EXPORT int odr_constructed_begin(ODR o, void *p, int zclass, int tag,
                                     const char *name);
YAZ_EXPORT int odr_constructed_end(ODR o);
YAZ_EXPORT int odr_sequence_begin(ODR o, void *p, int size, const char *name);
YAZ_EXPORT int odr_set_begin(ODR o, void *p, int size, const char *name);
YAZ_EXPORT int odr_sequence_end(ODR o);
YAZ_EXPORT int odr_set_end(ODR o);
YAZ_EXPORT int ber_octetstring(ODR o, Odr_oct *p, int cons);
YAZ_EXPORT int odr_octetstring(ODR o, Odr_oct **p, int opt, const char *name);
YAZ_EXPORT int odp_more_chunks(ODR o, const char *base, int len);
YAZ_EXPORT int odr_constructed_more(ODR o);
YAZ_EXPORT int odr_bitstring(ODR o, Odr_bitmask **p, int opt,
                             const char *name);
YAZ_EXPORT int ber_bitstring(ODR o, Odr_bitmask *p, int cons);
YAZ_EXPORT int odr_generalstring(ODR o, char **p, int opt, const char *name);
YAZ_EXPORT int ber_oidc(ODR o, Odr_oid *p, int max_oid_size);
YAZ_EXPORT int odr_oid(ODR o, Odr_oid **p, int opt, const char *name);
YAZ_EXPORT int odr_choice(ODR o, Odr_arm arm[], void *p, void *whichp,
                          const char *name);
YAZ_EXPORT int odr_cstring(ODR o, char **p, int opt, const char *name);
YAZ_EXPORT int odr_iconv_string(ODR o, char **p, int opt, const char *name);
YAZ_EXPORT int odr_sequence_of(ODR o, Odr_fun type, void *p, int *num,
                               const char *name);
YAZ_EXPORT int odr_set_of(ODR o, Odr_fun type, void *p, int *num,
                          const char *name);
YAZ_EXPORT int odr_any(ODR o, Odr_any **p, int opt, const char *name);

YAZ_EXPORT int ber_any(ODR o, Odr_any **p);
/** \brief determine whether a buffer is a complete BER buffer
    \param buf BER buffer
    \param len length of buffer
    \retval 0 package is incomplete
    \retval >0 package is complete and length is return value
*/
YAZ_EXPORT int completeBER(const char *buf, int len);

YAZ_EXPORT void odr_begin(ODR o);
YAZ_EXPORT void odr_end(ODR o);
YAZ_EXPORT Odr_oid *odr_oiddup(ODR odr, const Odr_oid *o);
YAZ_EXPORT Odr_oid *odr_oiddup_nmem(NMEM nmem, const Odr_oid *o);
YAZ_EXPORT int odr_grow_block(ODR b, int min_bytes);
YAZ_EXPORT int odr_write(ODR o, const char *buf, int bytes);
YAZ_EXPORT int odr_seek(ODR o, int whence, int offset);
YAZ_EXPORT int odr_dumpBER(FILE *f, const char *buf, int len);
YAZ_EXPORT void odr_choice_bias(ODR o, int what);
YAZ_EXPORT void odr_choice_enable_bias(ODR o, int mode);
YAZ_EXPORT size_t odr_total(ODR o);
YAZ_EXPORT char *odr_errmsg(int n);
YAZ_EXPORT Odr_oid *odr_getoidbystr(ODR o, const char *str);
YAZ_EXPORT Odr_oid *odr_getoidbystr_nmem(NMEM o, const char *str);

YAZ_EXPORT int odr_initmember(ODR o, void *p, int size);
YAZ_EXPORT int odr_peektag(ODR o, int *zclass, int *tag, int *cons);
YAZ_EXPORT void odr_setlenlen(ODR o, int len);
YAZ_EXPORT int odr_missing(ODR o, int opt, const char *name);
YAZ_EXPORT char *odr_prepend(ODR o, const char *prefix, const char *old);

typedef struct Odr_external
{
    Odr_oid *direct_reference;       /* OPTIONAL */
    Odr_int *indirect_reference;     /* OPTIONAL */
    char    *descriptor;             /* OPTIONAL */
    int which;
#define ODR_EXTERNAL_single 0
#define ODR_EXTERNAL_octet 1
#define ODR_EXTERNAL_arbitrary 2
    union
    {
        Odr_any  *single_ASN1_type;
        Odr_oct  *octet_aligned;
        Odr_bitmask *arbitrary;      /* we aren't really equipped for this*/
    } u;
} Odr_external;

YAZ_EXPORT int odr_external(ODR o, Odr_external **p, int opt,
                            const char *name);
YAZ_EXPORT int odr_visiblestring(ODR o, char **p, int opt,
                                 const char *name);
YAZ_EXPORT int odr_graphicstring(ODR o, char **p, int opt,
                                 const char *name);
YAZ_EXPORT int odr_generalizedtime(ODR o, char **p, int opt,
                                   const char *name);

YAZ_EXPORT int odr_set_charset(ODR o, const char *to, const char *from);

YAZ_EXPORT void odr_set_stream(ODR o,  void *handle,
                               void (*stream_write)(ODR o,
                                                    void *handle,
                                                    int type,
                                                    const char *buf,
                                                    int len),
                               void (*stream_close)(void *handle));

YAZ_EXPORT void odr_printf(ODR o, const char *fmt, ...);

YAZ_EXPORT const char **odr_get_element_path(ODR o);

YAZ_EXPORT Odr_int odr_atoi(const char *s);

YAZ_EXPORT Odr_int odr_strtol(const char *nptr, char **endptr, int base);

YAZ_END_CDECL

#include <yaz/xmalloc.h>

#endif
/*
 * Local variables:
 * c-basic-offset: 4
 * c-file-style: "Stroustrup"
 * indent-tabs-mode: nil
 * End:
 * vim: shiftwidth=4 tabstop=8 expandtab
 */

