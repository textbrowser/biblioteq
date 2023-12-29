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
 * \file zoom.h
 * \brief Header for ZOOM
 */

#include <stdlib.h>
#include <yaz/yconfig.h>

#define ZOOM_BEGIN_CDECL YAZ_BEGIN_CDECL
#define ZOOM_END_CDECL YAZ_END_CDECL

#ifdef WIN32
#  define ZOOM_API(x) __declspec(dllexport) x __stdcall
#else
#  define ZOOM_API(x) x
# endif

ZOOM_BEGIN_CDECL

/* ----------------------------------------------------------- */
/* the types we use */

typedef struct ZOOM_options_p *ZOOM_options;
typedef struct ZOOM_query_p *ZOOM_query;
typedef struct ZOOM_connection_p *ZOOM_connection;
typedef struct ZOOM_resultset_p *ZOOM_resultset;
typedef struct ZOOM_record_p *ZOOM_record;
typedef struct ZOOM_facet_field_p *ZOOM_facet_field;
typedef struct ZOOM_scanset_p *ZOOM_scanset;
typedef struct ZOOM_package_p *ZOOM_package;

typedef const char *(*ZOOM_options_callback)(void *handle, const char *name);

/* ----------------------------------------------------------- */
/* connections */

/* create connection, connect to host, if portnum is 0, then port is
   read from host string (e.g. myhost:9821) */
ZOOM_API(ZOOM_connection)
ZOOM_connection_new(const char *host, int portnum);

/* create connection, don't connect, apply options */
ZOOM_API(ZOOM_connection)
ZOOM_connection_create(ZOOM_options options);

/* connect given existing connection */
ZOOM_API(void)
ZOOM_connection_connect(ZOOM_connection c, const char *host,
                              int portnum);

ZOOM_API(void)
ZOOM_connection_close(ZOOM_connection c);

/* destroy connection (close connection also) */
ZOOM_API(void)
ZOOM_connection_destroy(ZOOM_connection c);

/* get/set option for connection */
ZOOM_API(const char *)
ZOOM_connection_option_get(ZOOM_connection c, const char *key);

ZOOM_API(const char *)
ZOOM_connection_option_getl(ZOOM_connection c, const char *key, int *lenp);

ZOOM_API(void)
ZOOM_connection_option_set(ZOOM_connection c, const char *key,
                           const char *val);

ZOOM_API(void)
ZOOM_connection_option_setl(ZOOM_connection c, const char *key,
                            const char *val, int len);

/* return error code (0 == success, failure otherwise). cp
   holds error string on failure, addinfo holds addititional info (if any)
*/
ZOOM_API(int)
ZOOM_connection_error(ZOOM_connection c, const char **cp,
                      const char **addinfo);

ZOOM_API(int)
ZOOM_connection_error_x(ZOOM_connection c, const char **cp,
                        const char **addinfo, const char **diagset);

/* returns error code */
ZOOM_API(int)
ZOOM_connection_errcode(ZOOM_connection c);
/* returns error message */
ZOOM_API(const char *)
ZOOM_connection_errmsg(ZOOM_connection c);
/* returns additional info */
ZOOM_API(const char *)
ZOOM_connection_addinfo(ZOOM_connection c);
/* returns diagnostic set */
ZOOM_API(const char *)
ZOOM_connection_diagset(ZOOM_connection c);
/* translates error code into human-readable string */
ZOOM_API(const char *)
ZOOM_diag_str (int error);

#define ZOOM_ERROR_NONE 0
#define ZOOM_ERROR_CONNECT 10000
#define ZOOM_ERROR_MEMORY  10001
#define ZOOM_ERROR_ENCODE  10002
#define ZOOM_ERROR_DECODE  10003
#define ZOOM_ERROR_CONNECTION_LOST 10004
#define ZOOM_ERROR_INIT 10005
#define ZOOM_ERROR_INTERNAL 10006
#define ZOOM_ERROR_TIMEOUT 10007
#define ZOOM_ERROR_UNSUPPORTED_PROTOCOL 10008
#define ZOOM_ERROR_UNSUPPORTED_QUERY 10009
#define ZOOM_ERROR_INVALID_QUERY 10010
#define ZOOM_ERROR_CQL_PARSE 10011
#define ZOOM_ERROR_CQL_TRANSFORM 10012
#define ZOOM_ERROR_CCL_CONFIG 10013
#define ZOOM_ERROR_CCL_PARSE 10014
#define ZOOM_ERROR_ES_INVALID_ACTION 10015
#define ZOOM_ERROR_ES_INVALID_VERSION 10016
#define ZOOM_ERROR_ES_INVALID_SYNTAX 10017
#define ZOOM_ERROR_MEMCACHED 10018

ZOOM_API(int)
ZOOM_connection_last_event(ZOOM_connection cs);

#define ZOOM_EVENT_NONE 0
#define ZOOM_EVENT_CONNECT 1
#define ZOOM_EVENT_SEND_DATA  2
#define ZOOM_EVENT_RECV_DATA 3
#define ZOOM_EVENT_TIMEOUT 4
#define ZOOM_EVENT_UNKNOWN 5
#define ZOOM_EVENT_SEND_APDU 6
#define ZOOM_EVENT_RECV_APDU 7
#define ZOOM_EVENT_RECV_RECORD 8
#define ZOOM_EVENT_RECV_SEARCH 9
#define ZOOM_EVENT_END 10
#define ZOOM_EVENT_MAX 10

/* ----------------------------------------------------------- */
/* result sets */

/* create result set given a search */
ZOOM_API(ZOOM_resultset)
ZOOM_connection_search(ZOOM_connection, ZOOM_query q);
/* create result set given PQF query */
ZOOM_API(ZOOM_resultset)
ZOOM_connection_search_pqf(ZOOM_connection c, const char *q);

/* destroy result set */
ZOOM_API(void)
ZOOM_resultset_destroy(ZOOM_resultset r);

/** release result set from connection.

    The result will will no longer be able to perform retrievals
    from the connection from which it was created.
*/
ZOOM_API(void)
ZOOM_resultset_release(ZOOM_resultset r);

/* result set option */
ZOOM_API(const char *)
ZOOM_resultset_option_get(ZOOM_resultset r, const char *key);
ZOOM_API(void)
ZOOM_resultset_option_set(ZOOM_resultset r, const char *key, const char *val);

/* return size of result set (alias hit count AKA result count) */
ZOOM_API(size_t)
ZOOM_resultset_size(ZOOM_resultset r);

/* retrieve records */
ZOOM_API(void)
ZOOM_resultset_records(ZOOM_resultset r, ZOOM_record *recs,
                       size_t start, size_t count);

/* return record object at pos. Returns 0 if unavailable */
ZOOM_API(ZOOM_record)
ZOOM_resultset_record(ZOOM_resultset s, size_t pos);

/* like ZOOM_resultset_record - but never blocks .. */
ZOOM_API(ZOOM_record)
ZOOM_resultset_record_immediate(ZOOM_resultset s, size_t pos);

/* reset record cache for result set */
ZOOM_API(void)
ZOOM_resultset_cache_reset(ZOOM_resultset r);


/* retrieve facet field */
ZOOM_API(ZOOM_facet_field)
ZOOM_resultset_get_facet_field(ZOOM_resultset r, const char *facet_name);

/* retrieve facet field at position. Returns 0 if unavailable */
ZOOM_API(ZOOM_facet_field)
ZOOM_resultset_get_facet_field_by_index(ZOOM_resultset r, int pos);

/* return number of facets available */
ZOOM_API(size_t)
ZOOM_resultset_facets_size(ZOOM_resultset r);

/* retrieve (array of pointers to) facet fields */
ZOOM_API(ZOOM_facet_field *)
ZOOM_resultset_facets(ZOOM_resultset r);

/* retrieve (array of pointers to) facet fields */
ZOOM_API(const char **)
ZOOM_resultset_facets_names(ZOOM_resultset r)
#ifdef __GNUC__
    __attribute__ ((deprecated))
#endif
    ;

/* ----------------------------------------------------------- */
/* records */

/* get record information, in a form given by type */
ZOOM_API(const char *)
ZOOM_record_get(ZOOM_record rec, const char *type, int *len);

/* destroy record */
ZOOM_API(void)
ZOOM_record_destroy(ZOOM_record rec);

/* return copy of record */
ZOOM_API(ZOOM_record)
ZOOM_record_clone(ZOOM_record srec);

/* return error info (surrogate diagnostic) for record */
ZOOM_API(int)
    ZOOM_record_error(ZOOM_record rec, const char **msg,
                      const char **addinfo, const char **diagset);

/* ----------------------------------------------------------- */
/* facets */

/* get facet name */
ZOOM_API(const char *)
ZOOM_facet_field_name(ZOOM_facet_field facet_field);

/* get terms count on facet */
ZOOM_API(size_t)
ZOOM_facet_field_term_count(ZOOM_facet_field facet_field);

/* get facet information, term and frequency, at a position. Returns 0 if out of bounds */
ZOOM_API(const char *)
ZOOM_facet_field_get_term(ZOOM_facet_field facet_field, size_t idx, int *freq);

/* ----------------------------------------------------------- */


/* queries */

/* create search object */
ZOOM_API(ZOOM_query)
ZOOM_query_create(void);
/* destroy it */
ZOOM_API(void)
ZOOM_query_destroy(ZOOM_query s);
/* CQL */
ZOOM_API(int)
ZOOM_query_cql(ZOOM_query s, const char *str);
/* CQL translated client-side into RPN: `conn' is optional for diagnostics */
ZOOM_API(int)
ZOOM_query_cql2rpn(ZOOM_query s, const char *str, ZOOM_connection conn);
/* CCL translated client-side into RPN: `conn' is optional for diagnostics */
ZOOM_API(int)
ZOOM_query_ccl2rpn(ZOOM_query s, const char *query_str,
                   const char *config,
                   int *ccl_error, const char **error_string, int *error_pos);
/* PQF */
ZOOM_API(int)
ZOOM_query_prefix(ZOOM_query s, const char *str);
/* specify sort criteria for search */
ZOOM_API(int)
ZOOM_query_sortby(ZOOM_query s, const char *criteria);
ZOOM_API(int)
ZOOM_query_sortby2(ZOOM_query s, const char *strategy, const char *criteria);

ZOOM_API(void)
ZOOM_query_addref(ZOOM_query s);

/* ----------------------------------------------------------- */
/* scan */
ZOOM_API(ZOOM_scanset)
ZOOM_connection_scan(ZOOM_connection c, const char *startterm);

ZOOM_API(ZOOM_scanset)
ZOOM_connection_scan1(ZOOM_connection c, ZOOM_query startterm);

ZOOM_API(const char *)
ZOOM_scanset_term(ZOOM_scanset scan, size_t pos,
                  size_t *occ, size_t *len);

ZOOM_API(const char *)
ZOOM_scanset_display_term(ZOOM_scanset scan, size_t pos,
                          size_t *occ, size_t *len);

ZOOM_API(size_t)
ZOOM_scanset_size(ZOOM_scanset scan);

ZOOM_API(void)
ZOOM_scanset_destroy(ZOOM_scanset scan);

ZOOM_API(const char *)
ZOOM_scanset_option_get(ZOOM_scanset scan, const char *key);

ZOOM_API(void)
ZOOM_scanset_option_set(ZOOM_scanset scan, const char *key,
                            const char *val);
/* ----------------------------------------------------------- */
/* Extended Services Packages */

ZOOM_API(ZOOM_package)
ZOOM_connection_package(ZOOM_connection c, ZOOM_options options);

ZOOM_API(void)
ZOOM_package_destroy(ZOOM_package p);

ZOOM_API(void)
ZOOM_package_send(ZOOM_package p, const char *type);

ZOOM_API(const char *)
ZOOM_package_option_get(ZOOM_package p, const char *key);

ZOOM_API(const char *)
ZOOM_package_option_getl(ZOOM_package p, const char *key, int *lenp);

ZOOM_API(void)
ZOOM_package_option_set(ZOOM_package p, const char *key,
                        const char *val);

ZOOM_API(void)
ZOOM_package_option_setl(ZOOM_package p, const char *key,
                         const char *val, int len);

/* ----------------------------------------------------------- */
/* Sort.  First function is deprecated, use second instead */
ZOOM_API(void)
ZOOM_resultset_sort(ZOOM_resultset r,
                    const char *sort_type, const char *sort_spec);
ZOOM_API(int)
ZOOM_resultset_sort1(ZOOM_resultset r,
                     const char *sort_type, const char *sort_spec);

/* ----------------------------------------------------------- */
/* options */

ZOOM_API(ZOOM_options_callback)
ZOOM_options_set_callback(ZOOM_options opt,
                          ZOOM_options_callback c, void *handle);
ZOOM_API(ZOOM_options)
ZOOM_options_create(void);

ZOOM_API(ZOOM_options)
ZOOM_options_create_with_parent(ZOOM_options parent);

ZOOM_API(ZOOM_options)
ZOOM_options_create_with_parent2(ZOOM_options parent1,
                                 ZOOM_options parent2);

ZOOM_API(ZOOM_options)
ZOOM_options_dup(ZOOM_options src);

ZOOM_API(const char *)
ZOOM_options_get(ZOOM_options opt, const char *name);

ZOOM_API(const char *)
ZOOM_options_getl(ZOOM_options opt, const char *name, int *lenp);

ZOOM_API(void)
ZOOM_options_set(ZOOM_options opt, const char *name, const char *v);

ZOOM_API(void)
ZOOM_options_setl(ZOOM_options opt, const char *name, const char *value,
                  int len);

ZOOM_API(void)
ZOOM_options_destroy(ZOOM_options opt);

ZOOM_API(int)
ZOOM_options_get_bool(ZOOM_options opt, const char *name, int defa);

ZOOM_API(int)
ZOOM_options_get_int(ZOOM_options opt, const char *name, int defa);

ZOOM_API(void)
ZOOM_options_set_int(ZOOM_options opt, const char *name, int value);

/** \brief select/poll socket mask: read */
#define ZOOM_SELECT_READ 1
/** \brief select/poll socket mask: write */
#define ZOOM_SELECT_WRITE 2
/** \brief select/poll socket mask: except */
#define ZOOM_SELECT_EXCEPT 4

/** \brief wait for events on connection(s) (BLOCKING)
    \param no number of connections (size of cs)
    \param cs connection array
    \retval 0 no event was fired
    \retval >0 event was fired for connection at (retval-1)

    blocking poll for events on a number of connections. Returns positive
    integer if event occurred ; zero if none occurred and no more
    events are pending. The positive integer specifies the
    connection for which the event occurred.
*/
ZOOM_API(int)
ZOOM_event(int no, ZOOM_connection *cs);


/** \brief determines if connection is idle (no active or pending work)
    \param c connection
    \retval 1 is idle
    \retval 0 is non-idle (active)
*/
ZOOM_API(int)
ZOOM_connection_is_idle(ZOOM_connection c);


/** \brief process one event for one of connections given
    \param no number of connections (size of cs)
    \param cs connection array
    \retval 0 no event was processed
    \retval >0 event was processed for connection at (retval-1)

    This function attemps to deal with outstandings events in a non-blocking
    mode. If no events was processed (return value of 0),  then the system
    should attempt to deal with sockets in blocking mode using socket
    select/poll which means calling the following functions:
    ZOOM_connection_get_socket, ZOOM_connection_get_mask,
    ZOOM_connection_get_timeout.
*/
ZOOM_API(int)
ZOOM_event_nonblock(int no, ZOOM_connection *cs);


/** \brief process one event for connection
    \param c connection
    \retval 0 no event was processed
    \retval 1 event was processed for connection

    This function attemps to deal with outstandings events in
    a non-blocking fashion. If no event was processed (return value of 0),
    then the system should attempt to deal with sockets in blocking mode
    using socket select/poll which means calling the following functions:
    ZOOM_connection_get_socket, ZOOM_connection_get_mask,
    ZOOM_connection_get_timeout. If an event was processed call this
    function again.
*/
ZOOM_API(int)
ZOOM_connection_process(ZOOM_connection c);


/** \brief executes non-blocking tasks for connection
    \param c connection
    \retval 0 no task was executed
    \retval 1 task was executed (but probably not completed)

    This function, unlike, ZOOM_connection_process, does not try to
    return any events (and remove them). But events may be generated
    from it. These are saved and may later be retrieved with
    ZOOM_connection_process and ZOOM_connection_last_event .
*/
ZOOM_API(int)
ZOOM_connection_exec_task(ZOOM_connection c);


/** \brief get socket fd for ZOOM connection
    \param c connection
    \retval -1 no socket assigned for connection
    \retval >=0 socket for connection

    Use this function when preparing for socket/poll and
    in conjunction with ZOOM_connection_get_mask.
*/
ZOOM_API(int)
ZOOM_connection_get_socket(ZOOM_connection c);


/** \brief get socket mask for connection
    \param c connection
    \returns mask for connection (possibly 0)

    Use this function when preparing for socket select/poll and
    in conjunction with ZOOM_connection_get_socket.
*/
ZOOM_API(int)
ZOOM_connection_get_mask(ZOOM_connection c);


/** \brief set socket mask for connection (DO NOT call outside zoom) */
ZOOM_API(int)
ZOOM_connection_set_mask(ZOOM_connection c, int mask);


/** \brief get timeout in seconds for ZOOM connection
    \param c connection
    \returns timeout value in seconds

    Use this function when preparing for socket/poll and
    in conjunction with ZOOM_connection_get_socket.
*/
ZOOM_API(int)
ZOOM_connection_get_timeout(ZOOM_connection c);


/** \brief fire socket event timeout
    \param c connection
    \retval 0 event was fired OK
    \retval -1 event was not fired

    Call this function when a timeout occurs - for example in the
    case of select(2) returning 0.
*/
ZOOM_API(int)
ZOOM_connection_fire_event_timeout(ZOOM_connection c);


/** \brief fire socket event activity (read,write,except)
    \param c connection
    \param mask or'ed mask of ZOOM_SELECT_.. values
    \retval 0 event was fired OK
    \retval -1 event was not fired
*/
ZOOM_API(int)
ZOOM_connection_fire_event_socket(ZOOM_connection c, int mask);


/** \brief peek at next event
    \param c connection
    \returns ZOOM_EVENT_NONE (for no events in queue), ZOOM_EVENT_CONNECT, ..

    Does not actually remove the event from the event queue. ZOOM_event and
    ZOOM_process_event removes one event.
*/

ZOOM_API(int)
ZOOM_connection_peek_event(ZOOM_connection c);

ZOOM_API(const char *)
ZOOM_get_event_str(int event);

#ifdef WRBUF_H

/** \brief log APDUs to WRBUF
    \param c connection
    \param w WRBUF where APDUs are logged
*/
ZOOM_API(void) ZOOM_connection_save_apdu_wrbuf(ZOOM_connection c, WRBUF w);
#endif

ZOOM_END_CDECL

/*
 * Local variables:
 * c-basic-offset: 4
 * c-file-style: "Stroustrup"
 * indent-tabs-mode: nil
 * End:
 * vim: shiftwidth=4 tabstop=8 expandtab
 */

