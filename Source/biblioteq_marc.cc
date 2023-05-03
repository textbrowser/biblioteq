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

#include <QStringList>
#include <QXmlStreamReader>

#include "biblioteq.h"
#include "biblioteq_marc.h"

biblioteq_marc::biblioteq_marc(const ITEM_TYPE itemType,
			       const PROTOCOL protocol,
			       const RECORD_SYNTAX recordSyntax)
{
  m_itemType = itemType;
  m_protocol = protocol;
  m_recordSyntax = recordSyntax;
}

biblioteq_marc::biblioteq_marc(void)
{
}

biblioteq_marc::~biblioteq_marc()
{
}

void biblioteq_marc::clear(void)
{
  m_author.clear();
  m_binding.clear();
  m_callnum.clear();
  m_category.clear();
  m_description.clear();
  m_deweynum.clear();
  m_edition.clear();
  m_isbn10.clear();
  m_isbn13.clear();
  m_lcnum.clear();
  m_place.clear();
  m_publicationDate = QDate();
  m_publisher.clear();
  m_sru003.clear();
  m_targetAudience.clear();
  m_title.clear();
  m_volumeNumber.clear();
}

void biblioteq_marc::initialize(const ITEM_TYPE itemType,
				const PROTOCOL protocol,
				const RECORD_SYNTAX recordSyntax)
{
  m_itemType = itemType;
  m_protocol = protocol;
  m_recordSyntax = recordSyntax;
}

void biblioteq_marc::parse(const QString &data)
{
  m_data = data;

  if(m_protocol == SRU)
    parseSRU();
  else
    parseZ3950();
}

void biblioteq_marc::parseBookSRUMarc21(void)
{
  clear();

  QXmlStreamReader reader(m_data);

  while(!reader.atEnd())
    if(reader.readNextStartElement())
      {
	if(reader.name().toString().toLower().trimmed() == "datafield")
	  {
	    auto tag
	      (reader.attributes().value("tag").toString().toLower().trimmed());

	    if(tag == "010")
	      {
		/*
		** $a - LC control number (NR)
		** $b - NUCMC control number (R)
		** $z - Canceled/invalid LC control number (R)
		** $8 - Field link and sequence number (R)
		*/

		QString str("");

		while(reader.readNextStartElement())
		  if(reader.name().toString().toLower().trimmed() == "subfield")
		    {
		      if(reader.attributes().value("code").
			 toString().toLower().trimmed() == "a")
			{
			  str.append(reader.readElementText());
			  break;
			}
		      else
			reader.skipCurrentElement();
		    }
		  else
		    break;

		m_lcnum = str.trimmed();
	      }
	    else if(tag == "020")
	      {
		/*
		** $a - International Standard Book Number (NR)
		** $c - Terms of availability (NR)
		** $z - Canceled/invalid ISBN (R)
		** $6 - Linkage (NR)
		** $8 - Field link and sequence number (R)
		*/

		QString str("");

		while(reader.readNextStartElement())
		  if(reader.name().toString().toLower().trimmed() == "subfield")
		    str.append(reader.readElementText());
		  else
		    break;

		if(str.contains("br.", Qt::CaseInsensitive) ||
		   str.contains("pbk", Qt::CaseInsensitive))
		  m_binding = QObject::tr("paperback");
		else if(str.contains("cart.", Qt::CaseInsensitive) ||
			str.contains("hardback", Qt::CaseInsensitive) ||
			str.contains("hardcover", Qt::CaseInsensitive) ||
			str.contains("hbk", Qt::CaseInsensitive) ||
			str.contains("rel.", Qt::CaseInsensitive))
		  m_binding = QObject::tr("hardcover");
		else
		  m_binding = biblioteq::s_unknown;

		if(str.contains(" ") && str.indexOf(" ") == 10)
		  {
		    str = str.mid(0, 10).trimmed();
		    m_isbn10 = str;
		  }
		else if(str.contains(" ") &&
			str.indexOf(" ") == 13)
		  {
		    str = str.mid(0, 13).trimmed();
		    m_isbn13 = str;
		  }
		else if(str.length() == 10)
		  m_isbn10 = str.trimmed();
	      }
	    else if(tag == "050")
	      {
		/*
		** $a - Classification number (R)
		** $b - Item number (NR)
		** $3 - Materials specified (NR)
		** $6 - Linkage (NR)
		** $8 - Field link and sequence number (R)
		*/

		QString str("");

		while(reader.readNextStartElement())
		  if(reader.name().toString().toLower().trimmed() == "subfield")
		    {
		      if(reader.attributes().value("code").
			 toString().toLower().trimmed() == "a" ||
			 reader.attributes().value("code").
			 toString().toLower().trimmed() == "b")
			str.append(reader.readElementText());
		      else
			reader.skipCurrentElement();
		    }
		  else
		    break;

		m_callnum = str.trimmed();
	      }
	    else if(tag == "082")
	      {
		/*
		** $a - Classification number (R)
		** $b - Item number (NR)
		** $m - Standard or optional designation (NR)
		** $q - Assigning agency (NR)
		** $2 - Edition number (NR)
		** $6 - Linkage (NR)
		** $8 - Field link and sequence number (R)
		*/

		QString str("");

		while(reader.readNextStartElement())
		  if(reader.name().toString().toLower().trimmed() == "subfield")
		    {
		      if(reader.attributes().value("code").
			 toString().toLower().trimmed() == "a" ||
			 reader.attributes().value("code").
			 toString().toLower().trimmed() == "b" ||
			 reader.attributes().value("code").
			 toString().toLower().trimmed() == "m" ||
			 reader.attributes().value("code").
			 toString().toLower().trimmed() == "q")
			str.append(reader.readElementText());
		      else
			reader.skipCurrentElement();
		    }
		  else
		    break;

		m_deweynum = str.trimmed();
	      }
	    else if(tag == "100" || tag == "700")
	      {
		/*
		** $a - Personal name (NR)
		** $b - Numeration (NR)
		** $c - Titles and words associated with a name (R)
		** $d - Dates associated with a name (NR)
		** $e - Relator term (R)
		** $f - Date of a work (NR)
		** $g - Miscellaneous information (NR)
		** $j - Attribution qualifier (R)
		** $k - Form subheading (R)
		** $l - Language of a work (NR)
		** $n - Number of part/section of a work (R)
		** $p - Name of part/section of a work (R)
		** $q - Fuller form of name (NR)
		** $t - Title of a work (NR)
		** $u - Affiliation (NR)
		** $0 - Authority record control number (R)
		** $4 - Relator code (R)
		** $6 - Linkage (NR)
		** $8 - Field link and sequence number (R)
		*/

		QString str("");

		while(reader.readNextStartElement())
		  if(reader.name().toString().toLower().trimmed() == "subfield")
		    {
		      if(reader.attributes().value("code").
			 toString().toLower().trimmed() == "a")
			{
			  str.append(reader.readElementText());
			  break;
			}
		      else
			reader.skipCurrentElement();
		    }
		  else
		    break;

		if(str.endsWith(","))
		  str = str.mid(0, str.length() - 1).trimmed();
		else
		  str = str.trimmed();

		if(!m_author.contains(str))
		  {
		    if(tag == "100")
		      m_author = str;
		    else if(!m_author.isEmpty())
		      m_author = m_author + "\n" + str;
		    else
		      m_author = str;
		  }
	      }
	    else if(tag == "245")
	      {
		/*
		** $a - Title (NR)
		** $b - Remainder of title (NR)
		** $c - Statement of responsibility, etc. (NR)
		** $f - Inclusive dates (NR)
		** $g - Bulk dates (NR)
		** $h - Medium (NR)
		** $k - Form (R)
		** $n - Number of part/section of a work (R)
		** $p - Name of part/section of a work (R)
		** $s - Version (NR)
		** $6 - Linkage (NR)
		** $8 - Field link and sequence number (R)
		*/

		QString str;

		while(reader.readNextStartElement())
		  if(reader.name().toString().toLower().trimmed() == "subfield")
		    {
		      if(reader.attributes().value("code").
			 toString().toLower().trimmed() == "a" ||
			 reader.attributes().value("code").
			 toString().toLower().trimmed() == "b")
			str.append(reader.readElementText());
		      else
			reader.skipCurrentElement();
		    }
		  else
		    break;

		if(str.lastIndexOf('/') > -1)
		  str = str.mid(0, str.lastIndexOf('/')).trimmed();
		else
		  str = str.trimmed();

		m_title = str;
	      }
	    else if(tag == "250")
	      {
		/*
		** $a - Edition statement (NR)
		** $b - Remainder of edition statement (NR)
		** $6 - Linkage (NR)
		** $8 - Field link and sequence number (R)
		*/

		QString str("");

		while(reader.readNextStartElement())
		  if(reader.name().toString().toLower().trimmed() == "subfield")
		    {
		      if(reader.attributes().value("code").
			 toString().toLower().trimmed() == "a")
			{
			  str.append(reader.readElementText());
			  break;
			}
		      else
			reader.skipCurrentElement();
		    }
		  else
		    break;

		if(str.indexOf(" ") > -1)
		  str = str.mid(0, str.indexOf(" ")).trimmed();
		else
		  str = str.trimmed();

		int i = 0;

		for(i = 0; i < str.size(); i++)
		  if(!str.at(i).isDigit())
		    break;

		str = str.mid(0, i).trimmed();
		m_edition = str;
	      }
	    else if(tag == "260")
	      {
		/*
		** $a - Place of publication, distribution,
		**      etc. (R)
		** $b - Name of publisher, distributor, etc. (R)
		** $c - Date of publication, distribution, etc. (R)
		** $e - Place of manufacture (R)
		** $f - Manufacturer (R)
		** $g - Date of manufacture (R)
		** $3 - Materials specified (NR)
		** $6 - Linkage (NR)
		** $8 - Field link and sequence number (R)
		*/

		QString date("");
		QString place("");
		QString publisher("");

		while(reader.readNextStartElement())
		  if(reader.name().toString().toLower().trimmed() == "subfield")
		    {
		      if(reader.attributes().value("code").
			 toString().toLower().trimmed() == "a")
			{
			  auto p(reader.readElementText().trimmed());

			  if(!p.isEmpty())
			    if(!p.at(p.length() - 1).isLetter())
			      p = p.mid(0, p.length() - 1);

			  place.append(p.trimmed()).append('\n');
			}
		      else if(reader.attributes().value("code").
			      toString().toLower().trimmed() == "b")
			publisher = reader.readElementText();
		      else if(reader.attributes().value("code").
			      toString().toLower().trimmed() == "c")
			{
			  date = reader.readElementText().toLower();

			  for(int i = date.length() - 1; i >= 0; i--)
			    if(!date.at(i).isNumber())
			      date.remove(i, 1);
			}
		      else
			reader.skipCurrentElement();
		    }
		  else
		    break;

		m_place = place.trimmed();
		m_publicationDate = QDate::fromString
		  ("01/01/" + date.mid(0, 4), biblioteq::s_databaseDateFormat);

		if(publisher.endsWith(","))
		  publisher = publisher.mid
		    (0, publisher.length() - 1).trimmed();

		m_publisher = publisher;
	      }
	    else if(tag == "300")
	      {
		/*
		** $a - Extent (R)
		** $b - Other physical details (NR)
		** $c - Dimensions (R)
		** $e - Accompanying material (NR)
		** $f - Type of unit (R)
		** $g - Size of unit (R)
		** $3 - Materials specified (NR)
		** $6 - Linkage (NR)
		** $8 - Field link and sequence number (R)
		*/

		QString str("");

		while(reader.readNextStartElement())
		  if(reader.name().toString().toLower().trimmed() == "subfield")
		    str.append(reader.readElementText());
		  else
		    break;

		m_description = str.trimmed();
	      }
	    else if(tag == "491")
	      {
		/*
		** $v - Volume Number
		*/

		QString str("");

		while(reader.readNextStartElement())
		  if(reader.name().toString().toLower().trimmed() == "subfield")
		    {
		      if(reader.attributes().value("code").
			 toString().toLower().trimmed() == "v")
			{
			  str.append(reader.readElementText());
			  break;
			}
		      else
			reader.skipCurrentElement();
		    }
		  else
		    break;

		m_volumeNumber = str.trimmed();
	      }
	    else if(tag == "521")
	      {
		/*
		** $a - Target audience note (R)
		** $b - Source (NR)
		** $2 - Source (NR) (This is a peculiarity.)
		** $3 - Materials specified (NR)
		** $6 - Linkage (NR)
		** $8 - Field link and sequence number (R)
		*/

		QString str("");

		while(reader.readNextStartElement())
		  if(reader.name().toString().toLower().trimmed() == "subfield")
		    str.append(reader.readElementText());
		  else
		    break;

		if(m_targetAudience.isEmpty())
		  m_targetAudience = str.trimmed();
	      }
	    else if(tag == "650")
	      {
		/*
		** $a - Topical term or geographic name entry element (NR)
		** $b - Topical term following geographic name
		**      entry element (NR)
		** $c - Location of event (NR)
		** $d - Active dates (NR)
		** $e - Relator term (R)
		** $4 - Relator code (R)
		** $v - Form subdivision (R)
		** $x - General subdivision (R)
		** $y - Chronological subdivision (R)
		** $z - Geographic subdivision (R)
		** $0 - Authority record control number (R)
		** $2 - Source of heading or term (NR)
		** $3 - Materials specified (NR)
		** $6 - Linkage (NR)
		** $8 - Field link and sequence number (R)
		*/

		QString str("");

		while(reader.readNextStartElement())
		  if(reader.name().toString().toLower().trimmed() == "subfield")
		    {
		      if(reader.attributes().value("code").
			 toString().toLower().trimmed() == "a")
			{
			  str.append(reader.readElementText());
			  break;
			}
		      else
			reader.skipCurrentElement();
		    }
		  else
		    break;

		str = str.trimmed();

		if(!str.isEmpty())
		  {
		    if(!str[str.length() - 1].isPunct())
		      str += ".";

		    if(!m_category.contains(str))
		      {
			if(!m_category.isEmpty())
			  m_category =
			    m_category + "\n" + str;
			else
			  m_category = str;
		      }
		  }
	      }
	  }
	else if(reader.name().toString().toLower().trimmed() == "controlfield")
	  {
	    auto tag
	      (reader.attributes().value("tag").toString().toLower().trimmed());

	    if(tag == "003")
	      {
		QString str("");

		str.append(reader.readElementText());

		auto index = str.indexOf("ark:/");

		if(index >= 0)
		  m_sru003 = str.mid(index);
	      }
	  }
      }
}

void biblioteq_marc::parseBookSRUUnimarc(void)
{
  parseBookSRUMarc21();
}

void biblioteq_marc::parseBookZ3950Marc21(void)
{
  clear();

  auto list(m_data.split("\n"));

  for(int i = 0; i < list.size(); i++)
    {
      auto str = list[i];

      if(str.startsWith("010 "))
	{
	  str = str.mid(4);

	  /*
	  ** $a - LC control number (NR)
	  ** $b - NUCMC control number (R)
	  ** $z - Canceled/invalid LC control number (R)
	  ** $8 - Field link and sequence number (R)
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  QStringList subfields;

	  subfields << "$b"
		    << "$z"
		    << "$8";

	  for(int i = 0; i < subfields.size(); i++)
	    if(str.contains(subfields.at(i)))
	      str = str.mid(0, str.indexOf(subfields.at(i))).trimmed();

	  m_lcnum = str.trimmed();
	}
      else if(str.startsWith("020 "))
	{
	  str = str.mid(4);

	  /*
	  ** $a - International Standard Book Number (NR)
	  ** $c - Terms of availability (NR)
	  ** $z - Canceled/invalid ISBN (R)
	  ** $6 - Linkage (NR)
	  ** $8 - Field link and sequence number (R)
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  if(str.contains("br.", Qt::CaseInsensitive) ||
	     str.contains("pbk", Qt::CaseInsensitive))
	    m_binding = QObject::tr("paperback");
	  else if(str.contains("cart.", Qt::CaseInsensitive) ||
		  str.contains("hardback", Qt::CaseInsensitive) ||
		  str.contains("hardcover", Qt::CaseInsensitive) ||
		  str.contains("hbk", Qt::CaseInsensitive) ||
		  str.contains("rel.", Qt::CaseInsensitive))
	    m_binding = QObject::tr("hardcover");
	  else
	    m_binding = biblioteq::s_unknown;

	  if(str.contains(" ") && str.indexOf(" ") == 10)
	    m_isbn10 = str.mid(0, 10).trimmed();
	  else if(str.contains(" ") && str.indexOf(" ") == 13)
	    m_isbn13 = str.mid(0, 13).trimmed();
	  else if(str.length() == 10)
	    m_isbn10 = str.trimmed();
	}
      else if(str.startsWith("050 "))
	{
	  str = str.mid(4);

	  /*
	  ** $a - Classification number (R)
	  ** $b - Item number (NR)
	  ** $3 - Materials specified (NR)
	  ** $6 - Linkage (NR)
	  ** $8 - Field link and sequence number (R)
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  str = str.remove(" $b").trimmed();

	  QStringList subfields;

	  subfields << "$3"
		    << "$6"
		    << "$8";

	  for(int i = 0; i < subfields.size(); i++)
	    if(str.contains(subfields.at(i)))
	      str = str.mid(0, str.indexOf(subfields.at(i))).trimmed();

	  m_callnum = str.trimmed();
	}
      else if(str.startsWith("082 "))
	{
	  str = str.mid(4);

	  /*
	  ** $a - Classification number (R)
	  ** $b - Item number (NR)
	  ** $m - Standard or optional designation (NR)
	  ** $q - Assigning agency (NR)
	  ** $2 - Edition number (NR)
	  ** $6 - Linkage (NR)
	  ** $8 - Field link and sequence number (R)
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  str = str.remove(" $b").remove
	    (" $m").remove(" $q").trimmed();

	  QStringList subfields;

	  subfields << "$2"
		    << "$6"
		    << "$8";

	  for(int i = 0; i < subfields.size(); i++)
	    if(str.contains(subfields.at(i)))
	      str = str.mid(0, str.indexOf(subfields.at(i))).trimmed();

	  m_deweynum = str.trimmed();
	}
      else if(str.startsWith("100 ") || str.startsWith("700 "))
	{
	  str = str.mid(4);

	  /*
	  ** $a - Personal name (NR)
	  ** $b - Numeration (NR)
	  ** $c - Titles and words associated with a name (R)
	  ** $d - Dates associated with a name (NR)
	  ** $e - Relator term (R)
	  ** $f - Date of a work (NR)
	  ** $g - Miscellaneous information (NR)
	  ** $j - Attribution qualifier (R)
	  ** $k - Form subheading (R)
	  ** $l - Language of a work (NR)
	  ** $n - Number of part/section of a work (R)
	  ** $p - Name of part/section of a work (R)
	  ** $q - Fuller form of name (NR)
	  ** $t - Title of a work (NR)
	  ** $u - Affiliation (NR)
	  ** $0 - Authority record control number (R)
	  ** $4 - Relator code (R)
	  ** $6 - Linkage (NR)
	  ** $8 - Field link and sequence number (R)
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  QStringList subfields;

	  subfields << "$b"
		    << "$c"
		    << "$d"
		    << "$e"
		    << "$f"
		    << "$g"
		    << "$j"
		    << "$k"
		    << "$l"
		    << "$n"
		    << "$p"
		    << "$q"
		    << "$t"
		    << "$u"
		    << "$0"
		    << "$4"
		    << "$6"
		    << "$8";

	  for(int j = 0; j < subfields.size(); j++)
	    if(str.contains(subfields.at(j)))
	      str = str.mid(0, str.indexOf(subfields.at(j))).trimmed();

	  if(str.endsWith(","))
	    str = str.mid(0, str.length() - 1).trimmed();
	  else
	    str = str.trimmed();

	  if(!m_author.contains(str))
	    {
	      if(list[i].startsWith("100 "))
		m_author = str;
	      else if(!m_author.isEmpty())
		m_author = m_author + "\n" + str;
	      else
		m_author = str;
	    }
	}
      else if(str.startsWith("245 "))
	{
	  str = str.mid(4);

	  /*
	  ** $a - Title (NR)
	  ** $b - Remainder of title (NR)
	  ** $c - Statement of responsibility, etc. (NR)
	  ** $f - Inclusive dates (NR)
	  ** $g - Bulk dates (NR)
	  ** $h - Medium (NR)
	  ** $k - Form (R)
	  ** $n - Number of part/section of a work (R)
	  ** $p - Name of part/section of a work (R)
	  ** $s - Version (NR)
	  ** $6 - Linkage (NR)
	  ** $8 - Field link and sequence number (R)
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  str = str.remove(" $b").trimmed();

	  QStringList subfields;

	  subfields << "$c"
		    << "$f"
		    << "$g"
		    << "$h"
		    << "$k"
		    << "$n"
		    << "$p"
		    << "$s"
		    << "$6"
		    << "$8";

	  for(int i = 0; i < subfields.size(); i++)
	    if(str.contains(subfields.at(i)))
	      str = str.mid(0, str.indexOf(subfields.at(i))).trimmed();

	  if(str.lastIndexOf('/') > -1)
	    str = str.mid(0, str.lastIndexOf('/')).trimmed();
	  else
	    str = str.trimmed();

	  m_title = str;
	}
      else if(str.startsWith("250 "))
	{
	  str = str.mid(4);

	  /*
	  ** $a - Edition statement (NR)
	  ** $b - Remainder of edition statement (NR)
	  ** $6 - Linkage (NR)
	  ** $8 - Field link and sequence number (R)
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  QStringList subfields;

	  subfields << "$b"
		    << "$6"
		    << "$8";

	  for(int i = 0; i < subfields.size(); i++)
	    if(str.contains(subfields.at(i)))
	      str = str.mid(0, str.indexOf(subfields.at(i))).trimmed();

	  if(str.indexOf(" ") > -1)
	    str = str.mid(0, str.indexOf(" ")).trimmed();

	  str = str.remove("d").remove("h").remove("n").
	    remove("r").remove("s").remove("t").trimmed();
	  m_edition = str;
	}
      else if(str.startsWith("260 ") || str.startsWith("264 "))
	{
	  str = str.mid(4);

	  /*
	  ** $a - Place of publication, distribution, etc. (R)
	  ** $b - Name of publisher, distributor, etc. (R)
	  ** $c - Date of publication, distribution, etc. (R)
	  ** $e - Place of manufacture (R) (260)
	  ** $f - Manufacturer (R) (260)
	  ** $g - Date of manufacture (R) (260)
	  ** $3 - Materials specified (NR)
	  ** $6 - Linkage (NR)
	  ** $8 - Field link and sequence number (R)
	  */

	  QString tmpstr = "";

	  if(str.indexOf("$a") > -1)
	    tmpstr = str.mid(str.indexOf("$a") + 2).trimmed();
	  else
	    tmpstr = str;

	  QStringList subfields;

	  if(list[i].startsWith("260 "))
	    subfields << "$b"
		      << "$c"
		      << "$e"
		      << "$f"
		      << "$g"
		      << "$3"
		      << "$6"
		      << "$8";
	  else
	    subfields << "$b"
		      << "$c"
		      << "$3"
		      << "$6"
		      << "$8";

	  for(int i = 0; i < subfields.size(); i++)
	    if(tmpstr.contains(subfields.at(i)))
	      tmpstr = tmpstr.mid(0, tmpstr.indexOf(subfields.at(i))).trimmed();

	  auto tmplist(tmpstr.split("$a"));

	  for(int j = 0; j < tmplist.size(); j++)
	    {
	      tmpstr = tmplist.at(j).trimmed();

	      if(tmpstr.lastIndexOf(" ") > -1)
		tmpstr = tmpstr.mid(0, tmpstr.lastIndexOf(" ")).
		  trimmed();

	      if(tmpstr.isEmpty())
		continue;

	      if(!tmpstr[0].isLetterOrNumber())
		tmpstr = tmpstr.mid(1).trimmed();

	      if(tmpstr.isEmpty())
		continue;

	      if(!tmpstr[tmpstr.length() - 1].isLetter())
		tmpstr = tmpstr.remove(tmpstr.length() - 1, 1).
		  trimmed();

	      if(m_place.isEmpty())
		m_place = tmpstr;
	      else
		m_place = m_place + "\n" + tmpstr;
	    }

	  if(str.indexOf("$c") > -1 &&
	     str.mid(str.indexOf("$c") + 2, 4).toLower().
	     contains("c"))
	    m_publicationDate = QDate::fromString
	      ("01/01/" +
	       str.mid(str.indexOf("$c") + 4, 4),
	       biblioteq::s_databaseDateFormat);
	  else if(str.indexOf("$c") > -1)
	    m_publicationDate = QDate::fromString
	      ("01/01/" +
	       str.mid(str.indexOf("$c") + 3, 4),
	       biblioteq::s_databaseDateFormat);

	  if(str.contains("$b"))
	    str = str.mid(str.indexOf("$b") + 2).trimmed();

	  if(str.contains("$a"))
	    {
	      str = str.mid(0, str.indexOf("$a")).trimmed();

	      if(str.lastIndexOf(" ") > -1)
		str = str.mid(0, str.lastIndexOf(" ")).trimmed();
	    }
	  else if(str.indexOf("$c") > -1)
	    str = str.mid(0, str.indexOf("$c")).trimmed();

	  if(str.contains("$b "))
	    str.remove("$b ");
	  else if(str.contains("$b"))
	    str.remove("$b");

	  if(str.endsWith(","))
	    str = str.mid(0, str.length() - 1).trimmed();
	  else
	    str = str.trimmed();

	  m_publisher = str;
	}
      else if(str.startsWith("300 "))
	{
	  str = str.mid(4);

	  /*
	  ** $a - Extent (R)
	  ** $b - Other physical details (NR)
	  ** $c - Dimensions (R)
	  ** $e - Accompanying material (NR)
	  ** $f - Type of unit (R)
	  ** $g - Size of unit (R)
	  ** $3 - Materials specified (NR)
	  ** $6 - Linkage (NR)
	  ** $8 - Field link and sequence number (R)
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  str = str.remove(" $b").trimmed();
	  str = str.remove(" $c").trimmed();
	  str = str.remove(" $e").trimmed();
	  str = str.remove(" $f").trimmed();
	  str = str.remove(" $g").trimmed();
	  str = str.remove(" $3").trimmed();
	  str = str.remove(" $6").trimmed();
	  str = str.remove(" $8").trimmed();
	  m_description = str;
	}
      else if(str.startsWith("490 "))
	{
	  /*
	  ** $v - Volume Number
	  */

	  str = str.mid(4);

	  if(str.indexOf("$v") > -1)
	    {
	      str = str.mid(str.indexOf("$v") + 2).trimmed();

	      if(str.indexOf("$") > -1)
		str = str.mid(0, str.indexOf("$"));

	      m_volumeNumber = str.trimmed();
	    }
	  else
	    m_volumeNumber.clear();
	}
      else if(str.startsWith("521 "))
	{
	  str = str.mid(4);

	  /*
	  ** $a - Target audience note (R)
	  ** $b - Source (NR)
	  ** $2 - Source (NR) (This is a peculiarity.)
	  ** $3 - Materials specified (NR)
	  ** $6 - Linkage (NR)
	  ** $8 - Field link and sequence number (R)
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  QStringList subfields;

	  subfields << "$b"
		    << "$2"
		    << "$3"
		    << "$6"
		    << "$8";

	  for(int i = 0; i < subfields.size(); i++)
	    if(str.contains(subfields.at(i)))
	      str = str.mid(0, str.indexOf(subfields.at(i))).trimmed();

	  if(m_targetAudience.isEmpty())
	    m_targetAudience = str.trimmed();
	}
      else if(str.startsWith("650 "))
	{
	  str = str.mid(4);

	  /*
	  ** $a - Topical term or geographic name entry element (NR)
	  ** $b - Topical term following geographic name entry
	  **      element (NR)
	  ** $c - Location of event (NR)
	  ** $d - Active dates (NR)
	  ** $e - Relator term (R)
	  ** $4 - Relator code (R)
	  ** $v - Form subdivision (R)
	  ** $x - General subdivision (R)
	  ** $y - Chronological subdivision (R)
	  ** $z - Geographic subdivision (R)
	  ** $0 - Authority record control number (R)
	  ** $2 - Source of heading or term (NR)
	  ** $3 - Materials specified (NR)
	  ** $6 - Linkage (NR)
	  ** $8 - Field link and sequence number (R)
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  QStringList subfields;

	  subfields << "$b"
		    << "$c"
		    << "$d"
		    << "$e"
		    << "$4"
		    << "$v"
		    << "$x"
		    << "$y"
		    << "$z"
		    << "$0"
		    << "$2"
		    << "$3"
		    << "$6"
		    << "$8";

	  for(int i = 0; i < subfields.size(); i++)
	    if(str.contains(subfields.at(i)))
	      str = str.mid(0, str.indexOf(subfields.at(i))).trimmed();

	  str = str.trimmed();

	  if(!str.isEmpty())
	    {
	      if(!str[str.length() - 1].isPunct())
		str += ".";

	      if(!m_category.contains(str))
		{
		  if(!m_category.isEmpty())
		    m_category = m_category + "\n" + str;
		  else
		    m_category = str;
		}
	    }
	}
    }
}

void biblioteq_marc::parseBookZ3950Unimarc(void)
{
  clear();

  auto list(m_data.split("\n"));

  for(int i = 0; i < list.size(); i++)
    {
      auto str = list[i];

      if(str.startsWith("010 "))
	{
	  str = str.mid(4);

	  /*
	  ** $a - Number (ISMN)
	  ** $b - Qualification
	  ** $d - Terms of Availability and/or Price
	  ** $z - Erroneous ISMN
	  ** $6 - Interfield Linking Data
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  if(str.contains("br.", Qt::CaseInsensitive) ||
	     str.contains("pbk", Qt::CaseInsensitive))
	    m_binding = QObject::tr("paperback");
	  else if(str.contains("cart.", Qt::CaseInsensitive) ||
		  str.contains("hardback", Qt::CaseInsensitive) ||
		  str.contains("hardcover", Qt::CaseInsensitive) ||
		  str.contains("hbk", Qt::CaseInsensitive) ||
		  str.contains("rel.", Qt::CaseInsensitive))
	    m_binding = QObject::tr("hardcover");
	  else
	    m_binding = biblioteq::s_unknown;

	  str = str.remove("#").remove("M-").remove("-");

	  if(str.indexOf("$b") > -1)
	    str = str.mid(0, str.indexOf("$b"));
	  else if(str.indexOf("$d") > -1)
	    str = str.mid(0, str.indexOf("$d"));
	  else if(str.indexOf("$z") > -1)
	    str = str.mid(0, str.indexOf("$z"));

	  str = str.trimmed();

	  if(str.length() == 10)
	    m_isbn10 = str;
	  else if(str.length() == 13)
	    m_isbn13 = str;
	}
      else if(str.startsWith("200 "))
	{
	  str = str.mid(4);

	  /*
	  ** $a - Title Proper
	  ** $b - General Material Designation
	  ** $c - Title Proper by Another Author
	  ** $d - Parallel Title Proper
	  ** $e - Other Title Information
	  ** $f - First Statement of Responsibility
	  ** $g - Subsequent Statement of Responsibility
	  ** $h - Number of a Part
	  ** $i - Name of a Part
	  ** $v - Volume Designation
	  ** $z - Language of Parallel Title Proper
	  ** $5 - Institution to Which Field Applies
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  str = str.remove(" $b").trimmed();

	  auto author(str);

	  while(author.indexOf("$f") > -1)
	    {
	      author = author.mid(author.indexOf("$f") + 2);

	      if(author.indexOf("$") > -1)
		author = author.mid(0, author.indexOf("$")).trimmed();
	      else
		author = author.trimmed();

	      if(!m_author.isEmpty())
		m_author = m_author + "\n" + author;
	      else
		m_author = author;
	    }

	  QStringList subfields;

	  subfields << "$c"
		    << "$d"
		    << "$e"
		    << "$f"
		    << "$g"
		    << "$h"
		    << "$i"
		    << "$v"
		    << "$z"
		    << "$5";

	  for(int i = 0; i < subfields.size(); i++)
	    if(str.contains(subfields.at(i)))
	      str = str.mid(0, str.indexOf(subfields.at(i))).trimmed();

	  if(str.lastIndexOf('/') > -1)
	    str = str.mid(0, str.lastIndexOf('/')).trimmed();
	  else
	    str = str.trimmed();

	  m_title = str;
	}
      else if(str.startsWith("205 "))
	{
	  str = str.mid(4);

	  /*
	  ** $a - Edition Statement
	  ** $b - Issue Statement
	  ** $d - Parallel Edition Statement
	  ** $f - Statement of Responsibility Relating to Edition
	  ** $g - Subsequent Statement of Responsibility
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  QStringList subfields;

	  subfields << "$b"
		    << "$d"
		    << "$f"
		    << "$g";

	  for(int i = 0; i < subfields.size(); i++)
	    if(str.contains(subfields.at(i)))
	      str = str.mid(0, str.indexOf(subfields.at(i))).trimmed();

	  if(str.indexOf(" ") > -1)
	    str = str.mid(0, str.indexOf(" ")).trimmed();

	  str = str.remove(".").remove("d").remove("h").remove("n").
	    remove("r").remove("s").remove("t").trimmed();
	  m_edition = str;
	}
      else if(str.startsWith("210 "))
	{
	  str = str.mid(4);

	  /*
	  ** $a - Place of Publication, Distribution, etc.
	  ** $b - Address of Publisher, Distributor, etc.
	  ** $c - Name of Publisher, Distributor, etc.
	  ** $d - Date of Publication, Distribution, etc.
	  ** $e - Place of Manufacture
	  ** $f - Address of Manufacturer
	  ** $g - Name of Manufacturer
	  ** $h - Date of Manufacture
	  */

	  QString tmpstr = "";

	  if(str.indexOf("$a") > -1)
	    tmpstr = str.mid(str.indexOf("$a") + 2).trimmed();
	  else
	    tmpstr = str;

	  QStringList subfields;

	  subfields << "$b"
		    << "$c"
		    << "$e"
		    << "$f"
		    << "$g"
		    << "$h";

	  for(int i = 0; i < subfields.size(); i++)
	    if(tmpstr.contains(subfields.at(i)))
	      tmpstr = tmpstr.mid(0, tmpstr.indexOf(subfields.at(i))).trimmed();

	  auto tmplist(tmpstr.split("$a"));

	  for(int j = 0; j < tmplist.size(); j++)
	    {
	      tmpstr = tmplist.at(j).trimmed();

	      if(tmpstr.lastIndexOf(" ") > -1)
		tmpstr = tmpstr.mid(0, tmpstr.lastIndexOf(" ")).
		  trimmed();

	      if(tmpstr.isEmpty())
		continue;

	      if(!tmpstr[0].isLetterOrNumber())
		tmpstr = tmpstr.mid(1).trimmed();

	      if(tmpstr.isEmpty())
		continue;

	      if(!tmpstr[tmpstr.length() - 1].isLetter())
		tmpstr = tmpstr.remove(tmpstr.length() - 1, 1).
		  trimmed();

	      if(m_place.isEmpty())
		m_place = tmpstr;
	      else
		m_place = m_place + "\n" + tmpstr;
	    }

	  if(str.indexOf("$d") > -1 &&
	     str.mid(str.indexOf("$d") + 2, 4).toLower().
	     contains("c"))
	    m_publicationDate = QDate::fromString
	      ("01/01/" +
	       str.mid(str.indexOf("$d") + 4, 4),
	       biblioteq::s_databaseDateFormat);
	  else if(str.indexOf("$d") > -1)
	    m_publicationDate = QDate::fromString
	      ("01/01/" +
	       str.mid(str.indexOf("$d") + 3, 4),
	       biblioteq::s_databaseDateFormat);

	  if(str.contains("$c"))
	    str = str.mid(str.indexOf("$c") + 2).trimmed();

	  if(str.indexOf("$") > -1)
	    str = str.mid(0, str.indexOf("$")).trimmed();

	  if(str.endsWith(","))
	    str = str.mid(0, str.length() - 1).trimmed();
	  else
	    str = str.trimmed();

	  m_publisher = str;
	}
      else if(str.startsWith("215 "))
	{
	  str = str.mid(4);

	  /*
	  ** $a - Specific Material Designation and Extent of Item
	  ** $c - Other Physical Details
	  ** $d - Dimensions
	  ** $e - Accompanying Material
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  str = str.remove(" $c").trimmed();
	  str = str.remove(" $d").trimmed();
	  str = str.remove(" $e").trimmed();
	  m_description = str;
	}
      else if(str.startsWith("225 ") || str.startsWith("461 "))
	{
	  /*
	  ** $v - Volume Number
	  */

	  str = str.mid(4);

	  if(str.indexOf("$v") > -1)
	    {
	      str = str.mid(str.indexOf("$v") + 2).trimmed();

	      if(str.indexOf("$") > -1)
		str = str.mid(0, str.indexOf("$"));

	      m_volumeNumber = str.trimmed();
	    }
	  else
	    m_volumeNumber.clear();
	}
      else if(str.startsWith("333 "))
	{
	  str = str.mid(4);

	  /*
	  ** $a - Text of Note
	  ** $k - Unknown (French?)
	  ** $2 - Unknown (French?)
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  QStringList subfields;

	  subfields << "$k"
		    << "$2";

	  for(int i = 0; i < subfields.size(); i++)
	    if(str.contains(subfields.at(i)))
	      str = str.mid(0, str.indexOf(subfields.at(i))).trimmed();

	  str = str.trimmed();

	  if(m_targetAudience.isEmpty())
	    m_targetAudience = str;
	}
      else if(str.startsWith("606 "))
	{
	  str = str.mid(4);

	  /*
	  ** $a - Entry Element
	  ** $j - Form Subdivision
	  ** $x - Topical Subdivision
	  ** $y - Geographical Subdivision
	  ** $z - Chronological Subdivision
	  ** $2 - System Code
	  ** $3 - Authority Record Number
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  QStringList subfields;

	  subfields << "$j"
		    << "$x"
		    << "$y"
		    << "$z"
		    << "$2"
		    << "$3";

	  for(int i = 0; i < subfields.size(); i++)
	    if(str.contains(subfields.at(i)))
	      str = str.mid(0, str.indexOf(subfields.at(i))).trimmed();

	  str = str.trimmed();

	  if(!str.isEmpty())
	    {
	      if(!str[str.length() - 1].isPunct())
		str += ".";

	      if(!m_category.contains(str))
		{
		  if(!m_category.isEmpty())
		    m_category = m_category + "\n" + str;
		  else
		    m_category = str;
		}
	    }
	}
    }
}

void biblioteq_marc::parseMagazineZ3950Marc21(void)
{
  /*
  ** Not available.
  */
}

void biblioteq_marc::parseMagazineZ3950Unimarc(void)
{
  clear();

  auto list(m_data.split("\n"));

  for(int i = 0; i < list.size(); i++)
    {
      auto str = list[i];

      if(str.startsWith("200 "))
	{
	  str = str.mid(4);

	  /*
	  ** $a - Title Proper
	  ** $b - General Material Designation
	  ** $c - Title Proper by Another Author
	  ** $d - Parallel Title Proper
	  ** $e - Other Title Information
	  ** $f - First Statement of Responsibility
	  ** $g - Subsequent Statement of Responsibility
	  ** $h - Number of a Part
	  ** $i - Name of a Part
	  ** $v - Volume Designation
	  ** $z - Language of Parallel Title Proper
	  ** $5 - Institution to Which Field Applies
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  QStringList subfields;

	  subfields << "$c"
		    << "$d"
		    << "$e"
		    << "$f"
		    << "$g"
		    << "$h"
		    << "$i"
		    << "$v"
		    << "$z"
		    << "$5";

	  for(int i = 0; i < subfields.size(); i++)
	    if(str.contains(subfields.at(i)))
	      str = str.mid(0, str.indexOf(subfields.at(i))).trimmed();

	  if(str.lastIndexOf('/') > -1)
	    str = str.mid(0, str.lastIndexOf('/')).trimmed();
	  else
	    str = str.trimmed();

	  m_title = str;
	}
      else if(str.startsWith("210 "))
	{
	  str = str.mid(4);

	  /*
	  ** $a - Place of Publication, Distribution, etc.
	  ** $b - Address of Publisher, Distributor, etc.
	  ** $c - Name of Publisher, Distributor, etc.
	  ** $d - Date of Publication, Distribution, etc.
	  ** $e - Place of Manufacture
	  ** $f - Address of Manufacturer
	  ** $g - Name of Manufacturer
	  ** $h - Date of Manufacture
	  */

	  QString tmpstr = "";

	  if(str.indexOf("$a") > -1)
	    tmpstr = str.mid(str.indexOf("$a") + 2).trimmed();
	  else
	    tmpstr = str;

	  QStringList subfields;

	  subfields << "$b"
		    << "$c"
		    << "$e"
		    << "$f"
		    << "$g"
		    << "$h";

	  for(int i = 0; i < subfields.size(); i++)
	    if(tmpstr.contains(subfields.at(i)))
	      tmpstr = tmpstr.mid(0, tmpstr.indexOf(subfields.at(i))).trimmed();

	  auto tmplist(tmpstr.split("$a"));

	  for(int j = 0; j < tmplist.size(); j++)
	    {
	      tmpstr = tmplist.at(j).trimmed();

	      if(tmpstr.lastIndexOf(" ") > -1)
		tmpstr = tmpstr.mid(0, tmpstr.lastIndexOf(" ")).
		  trimmed();

	      if(tmpstr.isEmpty())
		continue;

	      if(!tmpstr[0].isLetterOrNumber())
		tmpstr = tmpstr.mid(1).trimmed();

	      if(tmpstr.isEmpty())
		continue;

	      if(!tmpstr[tmpstr.length() - 1].isLetter())
		tmpstr = tmpstr.remove(tmpstr.length() - 1, 1).
		  trimmed();

	      if(m_place.isEmpty())
		m_place = tmpstr;
	      else
		m_place = m_place + "\n" + tmpstr;
	    }

	  if(str.indexOf("$d") > -1 &&
	     str.mid(str.indexOf("$d") + 2, 4).toLower().
	     contains("c"))
	    m_publicationDate = QDate::fromString
	      ("01/01/" +
	       str.mid(str.indexOf("$d") + 4, 4),
	       biblioteq::s_databaseDateFormat);
	  else if(str.indexOf("$d") > -1)
	    m_publicationDate = QDate::fromString
	      ("01/01/" +
	       str.mid(str.indexOf("$d") + 3, 4),
	       biblioteq::s_databaseDateFormat);

	  if(str.contains("$c"))
	    str = str.mid(str.indexOf("$c") + 2).trimmed();

	  if(str.indexOf("$") > -1)
	    str = str.mid(0, str.indexOf("$")).trimmed();

	  if(str.endsWith(","))
	    str = str.mid(0, str.length() - 1).trimmed();
	  else
	    str = str.trimmed();

	  m_publisher = str;
	}
      else if(str.startsWith("215 "))
	{
	  str = str.mid(4);

	  /*
	  ** $a - Specific Material Designation and Extent of Item
	  ** $c - Other Physical Details
	  ** $d - Dimensions
	  ** $e - Accompanying Material
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  str = str.remove(" $c").trimmed();
	  str = str.remove(" $d").trimmed();
	  str = str.remove(" $e").trimmed();
	  m_description = str;
	}
      else if(str.startsWith("606 "))
	{
	  str = str.mid(4);

	  /*
	  ** $a - Entry Element
	  ** $j - Form Subdivision
	  ** $x - Topical Subdivision
	  ** $y - Geographical Subdivision
	  ** $z - Chronological Subdivision
	  ** $2 - System Code
	  ** $3 - Authority Record Number
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  QStringList subfields;

	  subfields << "$j"
		    << "$x"
		    << "$y"
		    << "$z"
		    << "$2"
		    << "$3";

	  for(int i = 0; i < subfields.size(); i++)
	    if(str.contains(subfields.at(i)))
	      str = str.mid(0, str.indexOf(subfields.at(i))).trimmed();

	  str = str.trimmed();

	  if(!str.isEmpty())
	    {
	      if(!str[str.length() - 1].isPunct())
		str += ".";

	      if(!m_category.contains(str))
		{
		  if(!m_category.isEmpty())
		    m_category = m_category + "\n" + str;
		  else
		    m_category = str;
		}
	    }
	}
    }
}

void biblioteq_marc::parseSRU(void)
{
  if(m_itemType == BOOK)
    {
      if(m_recordSyntax == MARC21)
	parseBookSRUMarc21();
      else
	parseBookSRUUnimarc();
    }
}

void biblioteq_marc::parseZ3950(void)
{
  if(m_itemType == BOOK)
    {
      if(m_recordSyntax == MARC21)
	parseBookZ3950Marc21();
      else
	parseBookZ3950Unimarc();
    }
  else
    {
      if(m_recordSyntax == MARC21)
	parseMagazineZ3950Marc21();
      else
	parseMagazineZ3950Unimarc();
    }
}
