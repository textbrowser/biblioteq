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
 * \file marcdisp.h
 * \brief MARC conversion
 */

#ifndef MARCDISP_H
#define MARCDISP_H

#include <yaz/yconfig.h>
#include <stdio.h>
#include <yaz/wrbuf.h>

#include <yaz/nmem.h>
#include <yaz/xmltypes.h>
#include <yaz/z-opac.h>

YAZ_BEGIN_CDECL

/** \brief a yaz_marc_t handle (private content) */
typedef struct yaz_marc_t_ *yaz_marc_t;

/** \brief construct yaz_marc_t handle */
YAZ_EXPORT yaz_marc_t yaz_marc_create(void);

/** \brief destroy yaz_marc_t handle */
YAZ_EXPORT void yaz_marc_destroy(yaz_marc_t mt);

/** \brief set XML mode YAZ_MARC_LINE, YAZ_MARCXML, YAZ_MARC_ISO2709 ..
    \param mt MARC handle
    \param xmlmode mode.

    This function ONLY affects yaz_marc_write_mode, yaz_marc_write_trailer.
*/
YAZ_EXPORT void yaz_marc_xml(yaz_marc_t mt, int xmlmode);

/** \brief Output format: Line-format */
#define YAZ_MARC_LINE      0
/** \brief Output format: simplexml (no longer supported) */
#define YAZ_MARC_SIMPLEXML 1
/** \brief Output format: OAI-MARC (no longer supported) */
#define YAZ_MARC_OAIMARC   2
/** \brief Output format: MARCXML */
#define YAZ_MARC_MARCXML   3
/** \brief Output format: ISO2709 */
#define YAZ_MARC_ISO2709   4
/** \brief Output format: MarcXchange (ISO25577) */
#define YAZ_MARC_XCHANGE   5
/** \brief Output format: check only (no marc output) */
#define YAZ_MARC_CHECK     6
/** \brief Output format: Turbo MARC Index Data format (XML based) */
#define YAZ_MARC_TURBOMARC 7
/** \brief Output format: JSON */
#define YAZ_MARC_JSON      8

/** \brief set iconv handle for character set conversion */
YAZ_EXPORT void yaz_marc_iconv(yaz_marc_t mt, yaz_iconv_t cd);

/** \brief supply iconv handle for character set conversion */
YAZ_EXPORT yaz_iconv_t yaz_marc_get_iconv(yaz_marc_t mt);

/** \brief set debug level
    \param mt handle
    \param level level, where 0=lowest, 1 more debug, 2 even more
*/
YAZ_EXPORT void yaz_marc_debug(yaz_marc_t mt, int level);

/** \brief decodes ISO2709 buffer using straight buffers
    \param mt marc handle
    \param buf input buffer
    \param bsize size of buffer or (-1 if "any size")
    \param result result to be stored here
    \param rsize size of result (memory "owned" by yaz_marc_mt handle)

    Decodes MARC in buf of size bsize.
    On success, result in *result with size *rsize.
    Returns -1 on error, or size of input record (>0) if OK
*/
YAZ_EXPORT int yaz_marc_decode_buf(yaz_marc_t mt, const char *buf, int bsize,
                                   const char **result, size_t *rsize);

/** \brief decodes ISO2709/MARC buffer and stores result in WRBUF
    \param mt handle
    \param buf input buffer
    \param bsize size of buffer (-1 if "any size")
    \param wrbuf WRBUF for output

    Decodes MARC in buf of size bsize.
    On success, result in wrbuf
    Returns -1 on error, or size of input record (>0) if OK
*/
YAZ_EXPORT int yaz_marc_decode_wrbuf(yaz_marc_t mt, const char *buf,
                                     int bsize, WRBUF wrbuf);

YAZ_EXPORT void yaz_marc_subfield_str(yaz_marc_t mt, const char *s);
YAZ_EXPORT void yaz_marc_endline_str(yaz_marc_t mt, const char *s);

/** \brief modifies part of the MARC leader */
YAZ_EXPORT void yaz_marc_modify_leader(yaz_marc_t mt, size_t off,
                                       const char *str);

/** \brief like atoi(3) except that it reads exactly len characters
    \param buf buffer to read
    \param len number of bytes to consider (being digits)
    \returns value
 */
YAZ_EXPORT int atoi_n(const char *buf, int len);

/** \brief like atoi_n but checks for proper formatting
    \param buf buffer to read values from
    \param size size of buffer
    \param val value of decimal number (if successful)
    \retval 0 no value found (non-digits found)
    \retval 1 value found and *val holds value
*/
YAZ_EXPORT
int atoi_n_check(const char *buf, int size, int *val);

/** \brief MARC control char: record separator (29 Dec, 1D Hex) */
#define ISO2709_RS 035
/** \brief MARC control char: field separator (30 Dec, 1E Hex) */
#define ISO2709_FS 036
/** \brief MARC control char: identifier-field separator (31 Dec, 1F Hex) */
#define ISO2709_IDFS 037

/** \brief read ISO2709/MARC record from buffer
    \param mt handle
    \param buf ISO2709 buffer of size bsize
    \param bsize size of buffer (-1 for unlimited size)

    Parses ISO2709 record from supplied buffer
    \retval -1 ERROR
    \retval >0 OK (length)
*/
YAZ_EXPORT int yaz_marc_read_iso2709(yaz_marc_t mt,
                                     const char *buf, int bsize);

/** \brief read MARC lineformat from stream
    \param mt handle
    \param getbyte get one byte handler
    \param ungetbyte unget one byte handler
    \param client_data opaque data for handers
    \retval -1 ERROR
    \retval >0 OK (length)

    Parses MARC line record from stream
    Returns > 0 for OK (same as length), -1=ERROR
*/
YAZ_EXPORT
int yaz_marc_read_line(yaz_marc_t mt,
                       int (*getbyte)(void *client_data),
                       void (*ungetbyte)(int b, void *client_data),
                       void *client_data);

#if YAZ_HAVE_XML2
/** \brief parses MARCXML/MarcXchange/TurboMARC record from xmlNode pointer
    \param mt handle
    \param ptr is a pointer to root xml node
    \retval 0 OK
    \retval -1 ERROR
*/
YAZ_EXPORT int yaz_marc_read_xml(yaz_marc_t mt, const xmlNode *ptr);
#endif

/** \brief writes record in line format
    \param mt handle
    \param wrbuf WRBUF for output
    \retval 0 OK
    \retval -1 ERROR
*/
YAZ_EXPORT int yaz_marc_write_line(yaz_marc_t mt, WRBUF wrbuf);

/** \brief writes record in MARCXML format
    \param mt handle
    \param wrbuf WRBUF for output
    \retval 0 OK
    \retval -1 ERROR

    Sets leader[9]='a' .
*/
YAZ_EXPORT int yaz_marc_write_marcxml(yaz_marc_t mt, WRBUF wrbuf);

/** \brief writes record in TurboMARC format
    \param mt handle
    \param wrbuf WRBUF for output
    \retval 0 OK
    \retval -1 ERROR
*/
YAZ_EXPORT int yaz_marc_write_turbomarc(yaz_marc_t mt, WRBUF wrbuf);

/** \brief writes record in MarcXchange XML (ISO25577)
    \param mt handle
    \param wrbuf WRBUF for output
    \param format record format (e.g. "MARC21")
    \param type record type (e.g. Bibliographic)
    \retval 0 OK
    \retval -1 ERROR
*/
YAZ_EXPORT int yaz_marc_write_marcxchange(yaz_marc_t mt, WRBUF wrbuf,
                                          const char *format,
                                          const char *type);

/** \brief writes record in ISO2709 format
    \param mt handle
    \param wrbuf WRBUF for output
    \retval 0 OK
    \retval -1 ERROR
*/
YAZ_EXPORT int yaz_marc_write_iso2709(yaz_marc_t mt, WRBUF wrbuf);

/** \brief writes record in mode - given by yaz_marc_xml mode
    \param mt handle
    \param wrbuf WRBUF for output
    \retval 0 OK
    \retval -1 ERROR

    This function calls yaz_marc_write_iso2709, yaz_marc_write_marcxml,
    etc.. depending on mode given by yaz_marc_xml.
*/
YAZ_EXPORT int yaz_marc_write_mode(yaz_marc_t mt, WRBUF wrbuf);

#if YAZ_HAVE_XML2
/** \brief writes MARC record as libxml2 tree
    \param mt handle
    \param root_ptr pointer to record node
    \param ns namespace of record (such as "http://www.loc.gov/MARC21/slim")
    \param format MarcXchange format (NULL for none)
    \param type MarcXchange format (NULL for none)
    \retval 0 Creation successful and *root_ptr is "record" node
    \retval -1 ERROR
*/
YAZ_EXPORT
int yaz_marc_write_xml(yaz_marc_t mt, xmlNode **root_ptr,
                       const char *ns,
                       const char *format,
                       const char *type);
#endif

/** \brief writes MARC record in JSON represenation
    \param mt handle
    \param w WRBUF for output
    \retval 0 Creation successful
    \retval -1 ERROR
*/
YAZ_EXPORT
int yaz_marc_write_json(yaz_marc_t mt, WRBUF w);

/** \brief sets leader spec (for modifying bytes in 24 byte leader)
    \param mt handle
    \param leader_spec
    \retval 0 OK
    \retval -1 ERROR

    Spec takes form pos=val,pos=val,...
    where value is either a number (decimal char value) or a
    string in 'a', e.g. 9='a'

*/
YAZ_EXPORT int yaz_marc_leader_spec(yaz_marc_t mt, const char *leader_spec);


/** \brief sets leader, validates it, and returns important values
    \param mt handle
    \param leader of the 24 byte leader to be set
    \param indicator_length indicator length (returned value)
    \param identifier_length identifier length (returned value)
    \param base_address base address (returned value)
    \param length_data_entry length of data entry (returned value)
    \param length_starting length of starting
    \param length_implementation length of implementation defined data
*/
YAZ_EXPORT
void yaz_marc_set_leader(yaz_marc_t mt, const char *leader,
                         int *indicator_length,
                         int *identifier_length,
                         int *base_address,
                         int *length_data_entry,
                         int *length_starting,
                         int *length_implementation);


/** \brief adds MARC comment string
    \param mt handle
    \param comment comment to be added)
*/
YAZ_EXPORT
void yaz_marc_add_comment(yaz_marc_t mt, char *comment);

/** \brief adds MARC annotation - printf interface
    \param mt handle
    \param fmt printf format string
*/
YAZ_EXPORT
void yaz_marc_cprintf(yaz_marc_t mt, const char *fmt, ...);

/** \brief adds subfield to MARC structure
    \param mt handle
    \param code_data code data buffer
    \param code_data_len length of code data
*/
YAZ_EXPORT
void yaz_marc_add_subfield(yaz_marc_t mt,
                           const char *code_data, size_t code_data_len);


/** \brief adds controlfield to MARC structure
    \param mt handle
    \param tag (e.g. "001"
    \param data value for this tag
    \param data_len length of data
*/
YAZ_EXPORT
void yaz_marc_add_controlfield(yaz_marc_t mt, const char *tag,
                               const char *data, size_t data_len);


#if YAZ_HAVE_XML2
/** \brief adds controlfield to MARC structure using xml Nodes
    \param mt handle
    \param ptr_tag value of tag (TEXT xmlNode)
    \param ptr_data value of data (TEXT xmlNode)
*/
YAZ_EXPORT
void yaz_marc_add_controlfield_xml(yaz_marc_t mt, const xmlNode *ptr_tag,
                                   const xmlNode *ptr_data);

/** \brief adds controlfield to MARC structure using xml Nodes for data
    \param mt handle
    \param tag string tag
    \param ptr_data value of data (TEXT xmlNode)
*/
YAZ_EXPORT
void yaz_marc_add_controlfield_xml2(yaz_marc_t mt, char *tag,
                                    const xmlNode *ptr_data);
#endif

/** \brief adds datafield to MARC structure using strings
    \param mt handle
    \param tag value of tag as string
    \param indicator indicator string
    \param indicator_len length of indicator string
*/
YAZ_EXPORT
void yaz_marc_add_datafield(yaz_marc_t mt, const char *tag,
                            const char *indicator, size_t indicator_len);

#if YAZ_HAVE_XML2
/** \brief adds datafield to MARC structure using xml Nodes
    \param mt handle
    \param ptr_tag value of tag (TEXT xmlNode)
    \param indicator indicator string
    \param indicator_len length of indicator string
*/
YAZ_EXPORT
void yaz_marc_add_datafield_xml(yaz_marc_t mt, const xmlNode *ptr_tag,
                                const char *indicator, size_t indicator_len);

/** \brief adds datafield to MARC structure using xml Nodes
    \param mt handle
    \param tag_value string value (pointer copied verbatim, not strdupped)
    \param indicators indicator string ; pointer copied verbatim; not strdupped
*/
YAZ_EXPORT
void yaz_marc_add_datafield_xml2(yaz_marc_t mt, char *tag_value,
                                 char *indicators);

#endif

/** \brief returns memory for MARC handle
    \param mt handle
    \retval NMEM handle for MARC system
*/
YAZ_EXPORT
NMEM yaz_marc_get_nmem(yaz_marc_t mt);

/** \brief clears memory and MARC record
    \param mt handle
*/
YAZ_EXPORT
void yaz_marc_reset(yaz_marc_t mt);

/** \brief gets debug level for MARC system
    \param mt handle
*/
YAZ_EXPORT
int yaz_marc_get_debug(yaz_marc_t mt);

/** \brief Converts MARC format type to format type(YAZ_MARC_..)
    \param arg string
    \retval -1 unknown format (bad arg)
    \retval >= 0 OK (one of YAZ_MARC - values)
*/
YAZ_EXPORT
int yaz_marc_decode_formatstr(const char *arg);

/** \brief Enables or disables writing of MARC XML records using Libxml2
    \param mt handle
    \param enable 0=disable, 1=enable
*/
YAZ_EXPORT
void yaz_marc_write_using_libxml2(yaz_marc_t mt, int enable);

/** \brief Performs "pretty" display of OPAC record to WRBUF using marc_t
    \param mt handle
    \param r OPAC record
    \param wrbuf WRBUF for resulting display string

    This function uses iconv_handle of yaz_marc_t for character set
    conversion of both OPAC + ISO2709 part.
    \*/
YAZ_EXPORT void yaz_opac_decode_wrbuf(yaz_marc_t mt, Z_OPACRecord *r,
                                      WRBUF wrbuf);

/** \brief Performs "pretty" display of OPAC record to WRBUF using marc_t
    \param mt handle
    \param r OPAC record
    \param wrbuf WRBUF for resulting display string
    \param cd iconv handle for OPAC content (not ISO2709 part)

    This function uses iconv handle of yaz_marc_t for character set
    conversion of ISO2709 part and supplied handle (cd) for OPAC part.
    \*/
YAZ_EXPORT void yaz_opac_decode_wrbuf2(yaz_marc_t mt, Z_OPACRecord *r,
                                       WRBUF wrbuf, yaz_iconv_t cd);

#if YAZ_HAVE_XML2
/** \brief Converts XML to OPAC
    \param mt marc handle
    \param buf_in XML buffer
    \param size_in size of XML buffer
    \param dst Z39.50 OPAC result - allocated by NMEM on marc handle
    \param cd iconv handle for the OPAC content (not ISO2709 part)
    \param nmem memory for OPACRecord (if NULL, mt NMEM memory is used)
    \param syntax OID for embedded MARC (if NULL, USMARC is used)
    \retval 1 conversion OK
    \retval 0 conversion NOT OK
    \*/
YAZ_EXPORT int yaz_xml_to_opac(yaz_marc_t mt,
                               const char *buf_in, size_t size_in,
                               Z_OPACRecord **dst, yaz_iconv_t cd,
                               NMEM nmem, const Odr_oid *syntax);
#endif

/** \brief flushes records
    \param mt handle
    \param wr WRBUF for output
    \retval 0 OK
    \retval -1 ERROR
*/
YAZ_EXPORT int yaz_marc_write_trailer(yaz_marc_t mt, WRBUF wr);

/** \brief enables record collection output
    \param mt handle
*/
YAZ_EXPORT void yaz_marc_enable_collection(yaz_marc_t mt);

struct json_node;

YAZ_EXPORT int yaz_marc_read_json_node(yaz_marc_t mt, struct json_node *n);

/** \brief check if MARC21 is UTF-8 encoded
    \param charset that is given by user
    \param marc_buf ISO2709 buf
    \param sz ISO2709 size
    \retval 1 is probably UTF-8
    \retval 0 is not UTF-8
*/
YAZ_EXPORT
int yaz_marc_check_marc21_coding(const char *charset,
                                 const char *marc_buf, int sz);

YAZ_EXPORT
int yaz_opac_check_marc21_coding(const char *charset, Z_OPACRecord *r);


size_t yaz_marc_sizeof_char(yaz_marc_t mt, const char *buf);

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

