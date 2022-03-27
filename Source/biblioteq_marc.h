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

  QString title(void) const
  {
    return m_title.trimmed();
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
  QString m_title;
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
