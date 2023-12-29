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

/** \file json.h
    \brief Header for JSON functions
*/

#ifndef YAZ_JSON_H
#define YAZ_JSON_H
#include <yaz/wrbuf.h>

YAZ_BEGIN_CDECL

/** \brief JSON node type for json_node */
enum json_node_type {
    json_node_object, /**< JSON object, u.link[0] is object content */
    json_node_array,  /**< JSON array, u.link[0] is array content */
    json_node_list,   /**< JSON elements or JSON members,
                         u.link[0] is value, u.link[1] is next elemen in list */
    json_node_pair,   /**< JSON pair, u.link[0] is name, u.link[1] is value */
    json_node_string, /**< JSON string, u.string is content */
    json_node_number, /**< JSON number (floating point), u.number is content */
    json_node_true,   /**< JSON true */
    json_node_false,  /**< JSON false */
    json_node_null    /**< JSON null */
};

/** \brief JSON node */
struct json_node {
    enum json_node_type type;
    union {
        char *string;
        double number;
        struct json_node *link[2];
    } u;
};

/** \brief JSON parser (opaque) */
typedef struct json_parser_s *json_parser_t;

/** \brief create JSON parser
    \returns JSON parser handle
*/
YAZ_EXPORT
json_parser_t json_parser_create(void);

/** \brief destroys JSON parser
    \param p JSON parser handle
*/
YAZ_EXPORT
void json_parser_destroy(json_parser_t p);

/** \brief parses JSON string
    \param p JSON parser handle
    \param json_str JSON string
    \returns JSON tree or NULL if parse error occurred.

    The resulting tree should be removed with a call to json_remove_node.
*/
YAZ_EXPORT
struct json_node *json_parser_parse(json_parser_t p, const char *json_str);

/** \brief returns parser error
    \param p JSON parser handle
    \returns parse error msg

    This function should be called if json_parser_parse returns NULL .
*/
YAZ_EXPORT
const char *json_parser_get_errmsg(json_parser_t p);

/** \brief returns parser position
    \param p JSON parser handle
    \returns number of bytes read from parser

    This function should be called if json_parser_parse returns NULL .
*/
YAZ_EXPORT
size_t json_parser_get_position(json_parser_t p);

/** \brief parses JSON string
    \param json_str JSON string
    \param errmsg pointer to error message string
    \returns JSON tree or NULL if parse error occurred.

    The resulting tree should be removed with a call to json_remove_node.
    The errmsg may be NULL in which case the error message is not returned.
*/
YAZ_EXPORT
struct json_node *json_parse(const char *json_str, const char **errmsg);

/** \brief parses JSON string
    \param json_str JSON string
    \param errmsg pointer to error message string
    \param pos position of parser stop (probably error)
    \returns JSON tree or NULL if parse error occurred.

    The resulting tree should be removed with a call to json_remove_node.
    The errmsg may be NULL in which case the error message is not returned.
*/
YAZ_EXPORT
struct json_node *json_parse2(const char *json_str, const char **errmsg,
                              size_t *pos);

/** \brief destroys JSON tree node and its children
    \param n JSON node
*/
YAZ_EXPORT
void json_remove_node(struct json_node *n);

/** \brief gets object pair value for some name
    \param n JSON node (presumably object node)
    \param name name to match
    \returns node or NULL if not found
*/
YAZ_EXPORT
struct json_node *json_get_object(struct json_node *n, const char *name);

/** \brief gets object value and detaches from existing tree
    \param n JSON node (presumably object node)
    \param name name to match
    \returns node or NULL if not found
*/
YAZ_EXPORT
struct json_node *json_detach_object(struct json_node *n, const char *name);

/** \brief gets array element
    \param n JSON node (presumably array node)
    \param idx (0=first, 1=second, ..)
    \returns node or NULL if not found
*/
YAZ_EXPORT
struct json_node *json_get_elem(struct json_node *n, int idx);

/** \brief returns number of children (array or object)
    \param n JSON node (presumably array node or object node)
    \returns number of children
*/
YAZ_EXPORT
int json_count_children(struct json_node *n);

/** \brief appends array to another
    \param dst original array and resulting array
    \param src array to be appended to dst
    \retval -1 not arrays
    \retval 0 array appended OK
*/
YAZ_EXPORT
int json_append_array(struct json_node *dst, struct json_node *src);

/** \brief configure subst rule
    \param p JSON parser
    \param idx (%id)
    \param n node to be substituted for idx (%idx)
*/
YAZ_EXPORT
void json_parser_subst(json_parser_t p, int idx, struct json_node *n);

/** \brief converts JSON tree to JSON string
    \param node JSON tree
    \param result resulting JSON string buffer
*/
YAZ_EXPORT
void json_write_wrbuf(struct json_node *node, WRBUF result);

/** \brief writes JSON tree with indentation (pretty print)
    \param node JSON tree
    \param result resulting JSON string buffer
*/
YAZ_EXPORT
void json_write_wrbuf_pretty(struct json_node *node, WRBUF result);

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

