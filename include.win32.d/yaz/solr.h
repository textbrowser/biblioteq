/* This file is part of the YAZ toolkit.
 * Copyright (C) 1995-2012 Index Data.
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

/** \file solr.h
    \brief Header with public definitions about SOLR.
*/

#ifndef SOLR_H_INCLUDED
#define SOLR_H_INCLUDED
#include <stdio.h>
#include <yaz/nmem.h>

YAZ_BEGIN_CDECL

/** \brief SOLR parser handle (opaque pointer) */
typedef struct solr_parser *SOLR_parser;

/** \brief creates a SOLR parser.
    \returns CCL parser
    
    Returns SOLR parser or NULL if parser could not be created.
 */
YAZ_EXPORT 
SOLR_parser solr_parser_create(void);

/** \brief destroys a SOLR parser.
    \param cp SOLR parser

    This function does nothing if NULL if received.
 */
YAZ_EXPORT 
void solr_parser_destroy(SOLR_parser cp);

/** \brief parses a SOLR query (string)
    \param cp SOLR parser
    \param str SOLR string
    \retval 0 success
    \retval !=0 failure
 */
YAZ_EXPORT 
int solr_parser_string(SOLR_parser cp, const char *str);

/** \brief parses SOLR query (query stream)
    \param cp SOLR parser
    \param getbyte function which reads one character from stream
    \param ungetbyte function which unreads one character from stream
    \param client_data data to be passed to stream functions
    \retval 0 success
    \retval !=0 failure
    
    This function is similar to solr_parser_string but takes a
    functions to read each query character from a stream.
    
    The functions pointers getbytes, ungetbyte are similar to
    that known from stdios getc, ungetc.
*/
YAZ_EXPORT 
int solr_parser_stream(SOLR_parser cp,
                      int (*getbyte)(void *client_data),
                      void (*ungetbyte)(int b, void *client_data),
                      void *client_data);

/** \brief parses SOLR query (from FILE)
    \param cp SOLR parser
    \param f file where query is read from
    \retval 0 success
    \retval !=0 failure
    
    This function is similar to solr_parser_string but reads from
    stdio FILE handle instead.
*/
YAZ_EXPORT
int solr_parser_stdio(SOLR_parser cp, FILE *f);

/** \brief Node type: search term */
#define SOLR_NODE_ST 1
/** \brief Node type: boolean */
#define SOLR_NODE_BOOL 2
/** \brief SOLR parse tree (node)
 */
struct solr_node {
    /** node type */
    int which;
    union {
        /** which == SOLR_NODE_ST */
        struct {
            /** SOLR index */
            char *index;
            /** SOLR index URI or NULL if no URI */
            char *index_uri;
            /** Search term */
            char *term;
            /** relation */
            char *relation;
            /** relation URL or NULL if no relation URI) */
            char *relation_uri;
            /** relation modifiers */
            struct solr_node *modifiers;
            /** term list */
            struct solr_node *extra_terms;
        } st;
        /** which == SOLR_NODE_BOOL */
        struct {
            /** operator name "and", "or", ... */
            char *value;
            /** left operand */
            struct solr_node *left;
            /** right operand */ 
            struct solr_node *right;
            /** modifiers (NULL for no list) */
            struct solr_node *modifiers;
        } boolean;
    } u;
};

/** \brief Private structure that describes the SOLR properties (profile)
 */
struct solr_properties;

/** \brief Structure used by solr_buf_write_handler
 */
struct solr_buf_write_info {
    int max;
    int off;
    char *buf;
};

/** \brief Handler for solr_buf_write_info
 */
YAZ_EXPORT
void solr_buf_write_handler(const char *b, void *client_data);

/** \brief Prints a SOLR node and all sub nodes.
    Hence this function prints the parse tree which is as returned by
    solr_parser_result.
*/
YAZ_EXPORT
void solr_node_print(struct solr_node *cn);

/** \brief creates a search clause node (st). */
YAZ_EXPORT
struct solr_node *solr_node_mk_sc(NMEM nmem, const char *index,
                                const char *relation, const char *term);

/** \brief applies a prefix+uri to "unresolved" index and relation URIs.
    "unresolved" URIs are those nodes where member index_uri / relation_uri
    is NULL.
*/
YAZ_EXPORT
struct solr_node *solr_apply_prefix(NMEM nmem, struct solr_node *cn,
                                  const char *prefix, const char *uri);

/** \brief creates a boolean node. */
YAZ_EXPORT
struct solr_node *solr_node_mk_boolean(NMEM nmem, const char *op);

/** \brief destroys a node and its children. */
YAZ_EXPORT
void solr_node_destroy(struct solr_node *cn);

/** duplicates a node (returns a copy of supplied node) . */
YAZ_EXPORT
struct solr_node *solr_node_dup (NMEM nmem, struct solr_node *cp);

/** \brief returns the parse tree of the most recently parsed SOLR query.
    \param cp SOLR parser
    \returns SOLR node or NULL for failure
*/
YAZ_EXPORT
struct solr_node *solr_parser_result(SOLR_parser cp);

/** \brief converts SOLR tree to XSOLR and writes to user-defined stream
    \param cn SOLR node (tree)
    \param pr print function
    \param client_data data to be passed to pr function
 */
YAZ_EXPORT
void solr_to_xml(struct solr_node *cn,
                void (*pr)(const char *buf, void *client_data),
                void *client_data);
/** \brief converts SOLR tree to XSOLR and writes to file
    \param cn SOLR node (tree)
    \param f file handle
 */
YAZ_EXPORT
void solr_to_xml_stdio(struct solr_node *cn, FILE *f);

/** \brief converts SOLR tree to XSOLR and writes result to buffer
    \param cn SOLR node (tree)
    \param out buffer
    \param max size of buffer (max chars to write)
    \returns length of resulting buffer
 */
YAZ_EXPORT
int solr_to_xml_buf(struct solr_node *cn, char *out, int max);

/** \brief stream handle for file (used by solr_to_xml_stdio) */
YAZ_EXPORT
void solr_fputs(const char *buf, void *client_data);

/** \brief SOLR transform handle.
    The transform describes how to convert from SOLR to PQF (Type-1 AKA RPN).
*/
typedef struct solr_transform_t_ *solr_transform_t;

/** \brief creates a SOLR transform handle
    \returns transform handle or NULL for failure
*/
YAZ_EXPORT
solr_transform_t solr_transform_create(void);

/** \brief creates a SOLR transform handle from am opened file handle
    \param f file where transformation spec is read
    \returns transform handle or NULL for failure

    The transformation spec is read from a FILE handle which is assumed
    opened for reading.
*/
YAZ_EXPORT
solr_transform_t solr_transform_open_FILE (FILE *f);

/** \brief creates a SOLR transform handle from a file
    \param fname name of where transformation spec is read
    \returns transform handle or NULL for failure
*/
YAZ_EXPORT
solr_transform_t solr_transform_open_fname(const char *fname);


/** \brief defines SOLR transform pattern
    \param ct SOLR transform handle
    \param pattern pattern string
    \param value pattern value
    \returns 0 for succes; -1 for failure
*/
YAZ_EXPORT
int solr_transform_define_pattern(solr_transform_t ct, const char *pattern,
                                 const char *value);
    


/** \brief destroys a SOLR transform handle
    \param ct SOLR transform handle
 */
YAZ_EXPORT
void solr_transform_close(solr_transform_t ct);

/** \brief tranforms PQF given a SOLR tree
    \param ct SOLR transform handle
    \param cn SOLR node tree
    \param pr print function
    \param client_data data to be passed to pr
    \retval 0 success
    \retval != 0 error

    The result is written to a user-defined stream.
*/
YAZ_EXPORT
int solr_transform(solr_transform_t ct,
                  struct solr_node *cn,
                  void (*pr)(const char *buf, void *client_data),
                  void *client_data);

/** \brief transforms PQF given a SOLR tree (from FILE)
    \param ct SOLR transform handle
    \param cn SOLR tree
    \param f FILE where output is written
    \retval 0 success
    \retval !=0 failure (error code)

    The result is written to a file specified by FILE handle (which must
    be opened for writing.
*/
YAZ_EXPORT
int solr_transform_FILE(solr_transform_t ct,
                       struct solr_node *cn, FILE *f);

/** \brief transforms PQF given a SOLR tree (from FILE)
    \param ct SOLR transform handle
    \param cn SOLR tree
    \param out buffer for output
    \param max maximum bytes for output (size of buffer)
    \retval 0 success
    \retval !=0 failure (error code)
 */
YAZ_EXPORT
int solr_transform_buf(solr_transform_t ct,
                      struct solr_node *cn, char *out, int max);

/** \brief returns additional information for last transform
    \param ct SOLR transform handle
    \param addinfo additional info (result)
    \returns error code
 */
YAZ_EXPORT
int solr_transform_error(solr_transform_t ct, const char **addinfo);

/** \brief sets error and addinfo for transform
    \param ct SOLR transform handle
    \param error error code
    \param addinfo additional info
 */
YAZ_EXPORT
void solr_transform_set_error(solr_transform_t ct, int error, const char *addinfo);

/** \brief returns the SOLR message corresponding to a given error code.
    \param code error code
    \returns text message
*/
YAZ_EXPORT
const char *solr_strerror(int code);

/** \brief returns the standard SOLR context set URI.
    \returns SOLR URI string
*/
YAZ_EXPORT
const char *solr_uri(void);

/** \brief compares two SOLR strings (ala strcmp)
    \param s1 string 1
    \param s2 string 2
    \returns comparison value
    Compares two SOLR strings (for relations, operators, etc)
    (unfortunately defined as case-insensitive unlike XML etc)
*/
YAZ_EXPORT
int solr_strcmp(const char *s1, const char *s2);

/** \brief compares two SOLR strings (ala strncmp)
    \param s1 string 1
    \param s2 string 2
    \param n size
    \returns comparison value
    Compares two SOLR strings at most n bytes
    (unfortunately defined as case-insensitive unlike XML etc)
 */
YAZ_EXPORT
int solr_strncmp(const char *s1, const char *s2, size_t n);

YAZ_END_CDECL

#endif
/* SOLR_H_INCLUDED */
/*
 * Local variables:
 * c-basic-offset: 4
 * c-file-style: "Stroustrup"
 * indent-tabs-mode: nil
 * End:
 * vim: shiftwidth=4 tabstop=8 expandtab
 */

