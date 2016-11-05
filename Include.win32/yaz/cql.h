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

/** \file cql.h
    \brief Header with public definitions about CQL.
*/

#ifndef CQL_H_INCLUDED
#define CQL_H_INCLUDED
#include <stdio.h>
#include <yaz/nmem.h>
#include <yaz/wrbuf.h>

YAZ_BEGIN_CDECL

/** \brief CQL parser handle (opaque pointer) */
typedef struct cql_parser *CQL_parser;

/** \brief creates a CQL parser.
    \returns CCL parser

    Returns CQL parser or NULL if parser could not be created.
 */
YAZ_EXPORT
CQL_parser cql_parser_create(void);

/** \brief destroys a CQL parser.
    \param cp CQL parser

    This function does nothing if NULL if received.
 */
YAZ_EXPORT
void cql_parser_destroy(CQL_parser cp);

/** \brief parses a CQL query (string)
    \param cp CQL parser
    \param str CQL string
    \retval 0 success
    \retval !=0 failure
 */
YAZ_EXPORT
int cql_parser_string(CQL_parser cp, const char *str);

/** \brief parses CQL query (query stream)
    \param cp CQL parser
    \param getbyte function which reads one character from stream
    \param ungetbyte function which unreads one character from stream
    \param client_data data to be passed to stream functions
    \retval 0 success
    \retval !=0 failure

    This function is similar to cql_parser_string but takes a
    functions to read each query character from a stream.

    The functions pointers getbytes, ungetbyte are similar to
    that known from stdios getc, ungetc.
*/
YAZ_EXPORT
int cql_parser_stream(CQL_parser cp,
                      int (*getbyte)(void *client_data),
                      void (*ungetbyte)(int b, void *client_data),
                      void *client_data);

/** \brief parses CQL query (from FILE)
    \param cp CQL parser
    \param f file where query is read from
    \retval 0 success
    \retval !=0 failure

    This function is similar to cql_parser_string but reads from
    stdio FILE handle instead.
*/
YAZ_EXPORT
int cql_parser_stdio(CQL_parser cp, FILE *f);

/** \brief configures strict mode
    \param cp CQL parser
    \param mode 1=enable strict mode, 0=disable strict mode

    This function is similar to cql_parser_string but reads from
    stdio FILE handle instead.
*/
YAZ_EXPORT
void cql_parser_strict(CQL_parser cp, int mode);

/** \brief Node type: search term */
#define CQL_NODE_ST 1
/** \brief Node type: boolean */
#define CQL_NODE_BOOL 2
/** \brief Node type: sortby single spec */
#define CQL_NODE_SORT 3

/** \brief CQL parse tree (node)
 */
struct cql_node {
    /** node type */
    int which;
    union {
        /** which == CQL_NODE_ST */
        struct {
            /** CQL index */
            char *index;
            /** CQL index URI or NULL if no URI */
            char *index_uri;
            /** Search term */
            char *term;
            /** relation */
            char *relation;
            /** relation URL or NULL if no relation URI) */
            char *relation_uri;
            /** relation modifiers */
            struct cql_node *modifiers;
            /** term list */
            struct cql_node *extra_terms;
        } st;
        /** which == CQL_NODE_BOOL */
        struct {
            /** operator name "and", "or", ... */
            char *value;
            /** left operand */
            struct cql_node *left;
            /** right operand */
            struct cql_node *right;
            /** modifiers (NULL for no list) */
            struct cql_node *modifiers;
        } boolean;
        /** which == CQL_NODE_SORT */
        struct {
            char *index;
            /** next spec */
            struct cql_node *next;
            /** modifiers (NULL for no list) */
            struct cql_node *modifiers;
            /** search node */
            struct cql_node *search;
        } sort;
    } u;
};

/** \brief Private structure that describes the CQL properties (profile)
 */
struct cql_properties;

/** \brief Structure used by cql_buf_write_handler
 */
struct cql_buf_write_info {
    int max;
    int off;
    char *buf;
};

/** \brief Handler for cql_buf_write_info
 */
YAZ_EXPORT
void cql_buf_write_handler(const char *b, void *client_data);

/** \brief Prints a CQL node and all sub nodes.
    Hence this function prints the parse tree which is as returned by
    cql_parser_result.
*/
YAZ_EXPORT
void cql_node_print(struct cql_node *cn);

/** \brief creates a search clause node (st). */
YAZ_EXPORT
struct cql_node *cql_node_mk_sc(NMEM nmem, const char *index,
                                const char *relation, const char *term);

/** \brief applies a prefix+uri to "unresolved" index and relation URIs.
    "unresolved" URIs are those nodes where member index_uri / relation_uri
    is NULL.
*/
YAZ_EXPORT
struct cql_node *cql_apply_prefix(NMEM nmem, struct cql_node *cn,
                                  const char *prefix, const char *uri);

/** \brief creates a boolean node. */
YAZ_EXPORT
struct cql_node *cql_node_mk_boolean(NMEM nmem, const char *op);

/** \brief creates a sort single spec node. */
YAZ_EXPORT
struct cql_node *cql_node_mk_sort(NMEM nmem, const char *index,
    struct cql_node *modifiers);

/** \brief destroys a node and its children. */
YAZ_EXPORT
void cql_node_destroy(struct cql_node *cn);

/** duplicates a node (returns a copy of supplied node) . */
YAZ_EXPORT
struct cql_node *cql_node_dup (NMEM nmem, struct cql_node *cp);

/** \brief returns the parse tree of the most recently parsed CQL query.
    \param cp CQL parser
    \returns CQL node or NULL for failure
*/
YAZ_EXPORT
struct cql_node *cql_parser_result(CQL_parser cp);

/** \brief returns the sortby tree of the most recently parsed CQL query.
    \param cp CQL parser
    \returns CQL node or NULL for failure
*/
YAZ_EXPORT
struct cql_node *cql_parser_sort_result(CQL_parser cp);

/** \brief converts CQL tree to XCQL and writes to user-defined stream
    \param cn CQL node (tree)
    \param pr print function
    \param client_data data to be passed to pr function
 */
YAZ_EXPORT
void cql_to_xml(struct cql_node *cn,
                void (*pr)(const char *buf, void *client_data),
                void *client_data);
/** \brief converts CQL tree to XCQL and writes to file
    \param cn CQL node (tree)
    \param f file handle
 */
YAZ_EXPORT
void cql_to_xml_stdio(struct cql_node *cn, FILE *f);

/** \brief converts CQL tree to XCQL and writes result to buffer
    \param cn CQL node (tree)
    \param out buffer
    \param max size of buffer (max chars to write)
    \returns length of resulting buffer
 */
YAZ_EXPORT
int cql_to_xml_buf(struct cql_node *cn, char *out, int max);

/** \brief converts CQL tree to CCL and writes to user-defined stream
    \param cn CQL node (tree)
    \param pr print function
    \param client_data data to be passed to pr function
 */
YAZ_EXPORT
int cql_to_ccl(struct cql_node *cn,
               void (*pr)(const char *buf, void *client_data),
               void *client_data);

/** \brief converts CQL tree to CCL and writes to file
    \param cn CQL node (tree)
    \param f file handle
 */
YAZ_EXPORT
void cql_to_ccl_stdio(struct cql_node *cn, FILE *f);

/** \brief converts CQL tree to CCL and writes result to buffer
    \param cn CQL node (tree)
    \param out buffer
    \param max size of buffer (max chars to write)
    \retval 0 OK
    \retval -1 conversion error
    \retval -2 buffer too small (truncated)
 */
YAZ_EXPORT
int cql_to_ccl_buf(struct cql_node *cn, char *out, int max);

/** \brief stream handle for file (used by cql_to_xml_stdio) */
YAZ_EXPORT
void cql_fputs(const char *buf, void *client_data);

/** \brief CQL transform handle.
    The transform describes how to convert from CQL to PQF (Type-1 AKA RPN).
*/
typedef struct cql_transform_t_ *cql_transform_t;

/** \brief creates a CQL transform handle
    \returns transform handle or NULL for failure
*/
YAZ_EXPORT
cql_transform_t cql_transform_create(void);

/** \brief creates a CQL transform handle from am opened file handle
    \param f file where transformation spec is read
    \returns transform handle or NULL for failure

    The transformation spec is read from a FILE handle which is assumed
    opened for reading.
*/
YAZ_EXPORT
cql_transform_t cql_transform_open_FILE (FILE *f);

/** \brief creates a CQL transform handle from a file
    \param fname name of where transformation spec is read
    \returns transform handle or NULL for failure
*/
YAZ_EXPORT
cql_transform_t cql_transform_open_fname(const char *fname);


/** \brief defines CQL transform pattern
    \param ct CQL transform handle
    \param pattern pattern string
    \param value pattern value
    \returns 0 for succes; -1 for failure
*/
YAZ_EXPORT
int cql_transform_define_pattern(cql_transform_t ct, const char *pattern,
                                 const char *value);



/** \brief destroys a CQL transform handle
    \param ct CQL transform handle
 */
YAZ_EXPORT
void cql_transform_close(cql_transform_t ct);

/** \brief tranforms PQF given a CQL tree (NOT re-entrant)
    \param ct CQL transform handle
    \param cn CQL node tree
    \param pr print function
    \param client_data data to be passed to pr
    \retval 0 success
    \retval != 0 error

    The result is written to a user-defined stream.
*/
YAZ_EXPORT
int cql_transform(cql_transform_t ct,
                  struct cql_node *cn,
                  void (*pr)(const char *buf, void *client_data),
                  void *client_data);

/** \brief tranforms PQF given a CQL tree (re-entrant)
    \param ct CQL transform handle
    \param cn CQL node tree
    \param addinfo additional information (if error)
    \param pr print function
    \param client_data data to be passed to pr
    \retval 0 success
    \retval != 0 error code

    The result is written to a user-defined stream.
*/
YAZ_EXPORT
int cql_transform_r(cql_transform_t ct, struct cql_node *cn,
                    WRBUF addinfo,
                    void (*pr)(const char *buf, void *client_data),
                    void *client_data);

/** \brief transforms PQF given a CQL tree from FILE (not re-entrant)
    \param ct CQL transform handle
    \param cn CQL tree
    \param f FILE where output is written
    \retval 0 success
    \retval !=0 failure (error code)

    The result is written to a file specified by FILE handle (which must
    be opened for writing.
*/
YAZ_EXPORT
int cql_transform_FILE(cql_transform_t ct,
                       struct cql_node *cn, FILE *f);

/** \brief transforms PQF given a CQL tree from buffer (not re-entrant)
    \param ct CQL transform handle
    \param cn CQL tree
    \param out buffer for output
    \param max maximum bytes for output (size of buffer)
    \retval 0 success
    \retval !=0 failure (error code)
 */
YAZ_EXPORT
int cql_transform_buf(cql_transform_t ct,
                      struct cql_node *cn, char *out, int max);

/** \brief returns additional information for last transform
    \param ct CQL transform handle
    \param addinfo additional info (result)
    \returns error code
 */
YAZ_EXPORT
int cql_transform_error(cql_transform_t ct, const char **addinfo);

/** \brief sets error and addinfo for transform
    \param ct CQL transform handle
    \param error error code
    \param addinfo additional info
 */
YAZ_EXPORT
void cql_transform_set_error(cql_transform_t ct, int error, const char *addinfo);

/** \brief returns the CQL message corresponding to a given error code.
    \param code error code
    \returns text message
*/
YAZ_EXPORT
const char *cql_strerror(int code);

/** \brief returns the standard CQL context set URI.
    \returns CQL URI string
*/
YAZ_EXPORT
const char *cql_uri(void);

/** \brief compares two CQL strings (ala strcmp)
    \param s1 string 1
    \param s2 string 2
    \returns comparison value
    Compares two CQL strings (for relations, operators, etc)
    (unfortunately defined as case-insensitive unlike XML etc)
*/
YAZ_EXPORT
int cql_strcmp(const char *s1, const char *s2);

/** \brief compares two CQL strings (ala strncmp)
    \param s1 string 1
    \param s2 string 2
    \param n size
    \returns comparison value
    Compares two CQL strings at most n bytes
    (unfortunately defined as case-insensitive unlike XML etc)
 */
YAZ_EXPORT
int cql_strncmp(const char *s1, const char *s2, size_t n);

/** \brief converts CQL sortby to sortkeys (ala versions 1.1)
    \param cn CQL tree
    \param pr print function
    \param client_data data to be passed to pr function

    This will take CQL_NODE_SORT entries and conver them to

    path,schema,ascending,caseSensitive,missingValue
    items..

    One for each sort keys. Where

    path is string index for sorting

    schema is schema for sort index

    ascending is a boolean (0=false, 1=true). Default is true.

    caseSensitive is a boolean. Default is false.

    missingValue is a string and one of 'abort', 'highValue', 'lowValue',
    or 'omit'. Default is 'highValue'.

    See also
    http://www.loc.gov/standards/sru/sru-1-1.html#sort
*/
YAZ_EXPORT
int cql_sortby_to_sortkeys(struct cql_node *cn,
                           void (*pr)(const char *buf, void *client_data),
                           void *client_data);

/** \brief converts CQL sortby to sortkeys ..
    \param cn CQL tree
    \param out result buffer
    \param max size of buffer (allocated)
    \retval 0 OK
    \retval -1 ERROR
*/
YAZ_EXPORT
int cql_sortby_to_sortkeys_buf(struct cql_node *cn, char *out, int max);

YAZ_END_CDECL

#endif
/* CQL_H_INCLUDED */
/*
 * Local variables:
 * c-basic-offset: 4
 * c-file-style: "Stroustrup"
 * indent-tabs-mode: nil
 * End:
 * vim: shiftwidth=4 tabstop=8 expandtab
 */

