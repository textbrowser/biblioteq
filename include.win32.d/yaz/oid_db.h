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
 * \file oid_db.h
 * \brief Header for OID database
 */
#ifndef OID_DB_H
#define OID_DB_H

#include <yaz/yconfig.h>
#include <yaz/oid_util.h>
#include <yaz/odr.h>

YAZ_BEGIN_CDECL

/** \brief OID database */
typedef struct yaz_oid_db *yaz_oid_db_t;


typedef enum oid_class
{
    CLASS_NOP=0,
    CLASS_APPCTX,
    CLASS_ABSYN,
    CLASS_ATTSET,
    CLASS_TRANSYN,
    CLASS_DIAGSET,
    CLASS_RECSYN,
    CLASS_RESFORM,
    CLASS_ACCFORM,
    CLASS_EXTSERV,
    CLASS_USERINFO,
    CLASS_ELEMSPEC,
    CLASS_VARSET,
    CLASS_SCHEMA,
    CLASS_TAGSET,
    CLASS_GENERAL,
    CLASS_NEGOT
} oid_class;


/** \brief returns standard OID database
    \retval OID database handle
*/
YAZ_EXPORT
yaz_oid_db_t yaz_oid_std(void);

/** \brief maps named OID string to raw OID by database lookup
    \param oid_db OID database
    \param oclass class of string (enum oid_class)
    \param name OID name
    \returns raw OID or NULL if name is unknown (bad)

    This function only maps known names in the database provided.
    Use yaz_string_to_oid_nmem or yaz_string_to_oid_odr to map
    any named OID in dot-notation (1.2.8).
*/
YAZ_EXPORT
const Odr_oid *yaz_string_to_oid(yaz_oid_db_t oid_db,
                                 oid_class oclass, const char *name);


/** \brief creates NMEM malloc'ed OID from string
    \param oid_db OID database
    \param oclass class of string (enum oid_class)
    \param name OID name
    \param nmem memory for returned OID
    \returns raw OID or NULL if name is unknown (bad)
*/
YAZ_EXPORT
Odr_oid *yaz_string_to_oid_nmem(yaz_oid_db_t oid_db,
                                oid_class oclass, const char *name, NMEM nmem);

/** \brief creates ODR malloc'ed OID from string
    \param oid_db OID database
    \param oclass class of string (enum oid_class)
    \param name OID name
    \param odr memory for returned OID
    \returns raw OID or NULL if name is unknown (bad)
*/
YAZ_EXPORT
Odr_oid *yaz_string_to_oid_odr(yaz_oid_db_t oid_db,
                               oid_class oclass, const char *name, ODR odr);

/** \brief maps raw OID to string
    \param oid_db OID database
    \param oid raw OID
    \param oclass holds OID class if found (output parameter)
    \returns OID name or NULL if not found in database
*/
YAZ_EXPORT
const char *yaz_oid_to_string(yaz_oid_db_t oid_db,
                              const Odr_oid *oid, oid_class *oclass);


/** \brief maps any OID to string (named or dot-notation)
    \param oid raw OID
    \param oclass holds OID class if found (output parameter)
    \param buf string buffer for result (must be of size OID_STR_MAX)
    \returns OID string (named or dot notatition)
*/
YAZ_EXPORT
const char *yaz_oid_to_string_buf(const Odr_oid *oid,
                                  oid_class *oclass, char *buf);


/** \brief maps named from standard database to dot notation
    \param oclass class of string (enum oid_class)
    \param name named OID
    \param oid_buf buffer for result (must be of size OID_STR_MAX)
    \returns OID string or NULL if name is not registered in database
*/
YAZ_EXPORT
char *oid_name_to_dotstring(oid_class oclass, const char *name, char *oid_buf);


/** \brief traverses OIDs in a database
    \param oid_db OID database
    \param func function to be called for each OID
    \param client_data data to be passed to func (custom defined)
*/
YAZ_EXPORT void yaz_oid_trav(yaz_oid_db_t oid_db,
                             void (*func)(const Odr_oid *oid,
                                          oid_class oclass, const char *name,
                                          void *client_data),
                             void *client_data);

/** \brief checks if OID refers to MARC transfer syntax
    \param oid raw OID
    \retval 1 OID is a MARC type
    \retval 0 OID is not a MARC type
*/
YAZ_EXPORT
int yaz_oid_is_iso2709(const Odr_oid *oid);

/** \brief adds new OID entry to database
    \param oid_db database
    \param oclass OID class
    \param name name of OID
    \param new_oid OID value (raw OID)
    \retval 0 OID added
    \retval -1 OID name+oclass already exists
*/
YAZ_EXPORT
int yaz_oid_add(yaz_oid_db_t oid_db, oid_class oclass, const char *name,
                const Odr_oid *new_oid);


/** \brief creates empty OID database
    \returns database
*/
YAZ_EXPORT
yaz_oid_db_t yaz_oid_db_new(void);

/** \brief destroys OID database
    \param oid_db database
*/
YAZ_EXPORT
void yaz_oid_db_destroy(yaz_oid_db_t oid_db);

struct yaz_oid_entry {
    enum oid_class oclass;
    const Odr_oid *oid;
    char *name;
};

YAZ_END_CDECL

#define Z3950_PREFIX 1, 2, 840, 10003

#include <yaz/oid_std.h>

#endif
/*
 * Local variables:
 * c-basic-offset: 4
 * c-file-style: "Stroustrup"
 * indent-tabs-mode: nil
 * End:
 * vim: shiftwidth=4 tabstop=8 expandtab
 */

