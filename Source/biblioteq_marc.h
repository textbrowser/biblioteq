/*
** Copyright (c) 2006 - present, Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from BiblioteQ without specific prior written permission.
**
** BIBLIOTEQ IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** BIBLIOTEQ, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _BIBLIOTEQ_MARC_H_
#define _BIBLIOTEQ_MARC_H_

#include <QDate>

class biblioteq_marc
{
 public:
  enum ITEM_TYPE
    {
      BOOK = 0,
      MAGAZINE
    };

  enum PROTOCOL
    {
      SRU = 0,
      Z3950
    };

  enum RECORD_SYNTAX
    {
      MARC21 = 0,
      UNIMARC
    };

  biblioteq_marc(const ITEM_TYPE itemType,
		 const PROTOCOL protocol,
		 const RECORD_SYNTAX recordSyntax);
  biblioteq_marc(void);
  ~biblioteq_marc();

  QDate publicationDate(void) const
  {
    return m_publicationDate;
  }

  QString author(void) const
  {
    return m_author.trimmed();
  }

  QString binding(void) const
  {
    return m_binding.trimmed();
  }

  QString callnum(void) const
  {
    return m_callnum.trimmed();
  }

  QString category(void) const
  {
    return m_category.trimmed();
  }

  QString description(void) const
  {
    return m_description.trimmed();
  }

  QString deweynum(void) const
  {
    return m_deweynum.trimmed();
  }

  QString edition(void) const
  {
    return m_edition.trimmed();
  }

  QString isbn10(void) const
  {
    return m_isbn10.trimmed();
  }

  QString isbn13(void) const
  {
    return m_isbn13.trimmed();
  }

  QString lcnum(void) const
  {
    return m_lcnum.trimmed();
  }

  QString place(void) const
  {
    return m_place.trimmed();
  }

  QString publisher(void) const
  {
    return m_publisher.trimmed();
  }

  QString sru003(void) const
  {
    return m_sru003;
  }

  QString targetAudience(void) const
  {
    return m_targetAudience;
  }

  QString title(void) const
  {
    return m_title.trimmed();
  }

  QString volumeNumber(void) const
  {
    return m_volumeNumber.trimmed();
  }

  void initialize(const ITEM_TYPE itemType,
		  const PROTOCOL protocol,
		  const RECORD_SYNTAX recordSyntax);
  void parse(const QString &data);

 private:
  ITEM_TYPE m_itemType;
  PROTOCOL m_protocol;
  QDate m_publicationDate;
  QString m_author;
  QString m_binding;
  QString m_callnum;
  QString m_category;
  QString m_data;
  QString m_description;
  QString m_deweynum;
  QString m_edition;
  QString m_isbn10;
  QString m_isbn13;
  QString m_lcnum;
  QString m_place;
  QString m_publisher;
  QString m_sru003;
  QString m_targetAudience;
  QString m_title;
  QString m_volumeNumber;
  RECORD_SYNTAX m_recordSyntax;
  void clear(void);
  void parseBookSRUMarc21(void);
  void parseBookSRUUnimarc(void);
  void parseBookZ3950Marc21(void);
  void parseBookZ3950Unimarc(void);
  void parseMagazineZ3950Marc21(void);
  void parseMagazineZ3950Unimarc(void);
  void parseSRU(void);
  void parseZ3950(void);
};

#endif
