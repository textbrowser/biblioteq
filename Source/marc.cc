#include <QObject>
#include <QStringList>
#include <QXmlStreamReader>

#include "marc.h"

marc::marc(const ITEM_TYPE itemType, const PROTOCOL protocol,
	   const RECORD_SYNTAX recordSyntax)
{
  m_itemType = itemType;
  m_protocol = protocol;
  m_recordSyntax = recordSyntax;
}

marc::marc(void)
{
}

marc::~marc()
{
}

void marc::clear(void)
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
  m_title.clear();
}

void marc::initialize(const ITEM_TYPE itemType, const PROTOCOL protocol,
		      const RECORD_SYNTAX recordSyntax)
{
  m_itemType = itemType;
  m_protocol = protocol;
  m_recordSyntax = recordSyntax;
}

void marc::parseBookSRUMarc21(void)
{
  clear();

  QXmlStreamReader reader(m_data);

  while(!reader.atEnd())
    if(reader.readNextStartElement())
      {
	if(reader.name().toString().toLower().
	   trimmed() == "datafield")
	  {
	    QString tag(reader.attributes().value("tag").
			toString().toLower().trimmed());

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
		  if(reader.name().toString().toLower().
		     trimmed() == "subfield")
		    {
		      if(reader.attributes().value("code").
			 toString().toLower().trimmed() == "a")
			{
			  str.append(reader.readElementText());
			  break;
			}
		    }
		  else
		    break;

		str = str.trimmed();
		m_lcnum = str;
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
		  if(reader.name().toString().toLower().
		     trimmed() == "subfield")
		    str.append(reader.readElementText());
		  else
		    break;

		if(str.toLower().contains("hardcover"))
		  m_binding = QObject::tr("hardcover");
		else if(str.toLower().contains("pbk."))
		  m_binding = QObject::tr("paperback");
		else
		  m_binding = QObject::tr("UNKNOWN");

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
		  m_isbn10 = str;
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
		  if(reader.name().toString().toLower().
		     trimmed() == "subfield")
		    {
		      if(reader.attributes().value("code").
			 toString().toLower().trimmed() == "a" ||
			 reader.attributes().value("code").
			 toString().toLower().trimmed() == "b")
			str.append(reader.readElementText());
		    }
		  else
		    break;

		m_callnum = str;
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
		  if(reader.name().toString().toLower().
		     trimmed() == "subfield")
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
		    }
		  else
		    break;

		m_deweynum = str;
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
		  if(reader.name().toString().toLower().
		     trimmed() == "subfield")
		    {
		      if(reader.attributes().value("code").
			 toString().toLower().trimmed() == "a")
			{
			  str.append(reader.readElementText());
			  break;
			}
		    }
		  else
		    break;

		if(str.endsWith(","))
		  str = str.mid(0, str.length() - 1).trimmed();

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
		  if(reader.name().toString().toLower().
		     trimmed() == "subfield")
		    {
		      if(reader.attributes().value("code").
			 toString().toLower().trimmed() == "a" ||
			 reader.attributes().value("code").
			 toString().toLower().trimmed() == "b")
			str.append(reader.readElementText());
		    }
		  else
		    break;

		if(str.lastIndexOf('/') > -1)
		  str = str.mid
		    (0, str.lastIndexOf('/')).trimmed();

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
		  if(reader.name().toString().toLower().
		     trimmed() == "subfield")
		    {
		      if(reader.attributes().value("code").
			 toString().toLower().trimmed() == "a")
			{
			  str.append(reader.readElementText());
			  break;
			}
		    }
		  else
		    break;

		if(str.indexOf(" ") > -1)
		  str = str.mid(0, str.indexOf(" ")).trimmed();

		int i = 0;

		for(i = 0; i < str.size(); i++)
		  if(!str.at(i).isDigit())
		    break;

		str = str.mid(0, i);
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
		  if(reader.name().toString().toLower().
		     trimmed() == "subfield")
		    {
		      if(reader.attributes().value("code").
			 toString().toLower().trimmed() == "a")
			place = reader.readElementText();
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
		    }
		  else
		    break;

		m_publicationDate = QDate::fromString
		  ("01/01/" + date.mid(0, 4), "MM/dd/yyyy");

		if(place.lastIndexOf(" ") > -1)
		  place = place.mid(0, place.lastIndexOf(" ")).
		    trimmed();

		if(!place.isEmpty())
		  if(!place[place.length() - 1].isLetter())
		    place = place.remove(place.length() - 1, 1).
		      trimmed();

		m_place = place;

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
		  if(reader.name().toString().toLower().
		     trimmed() == "subfield")
		    str.append(reader.readElementText());
		  else
		    break;

		m_description = str;
	      }
	    else if(tag == "650")
	      {
		/*
		** $a - Topical term or geographic name entry
		**      element (NR)
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
		  if(reader.name().toString().toLower().
		     trimmed() == "subfield")
		    {
		      if(reader.attributes().value("code").
			 toString().toLower().trimmed() == "a")
			{
			  str.append(reader.readElementText());
			  break;
			}
		    }
		  else
		    break;

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
      }
}

void marc::parseBookZ3950Marc21(void)
{
  clear();

  QStringList list(m_data.split("\n"));

  for(int i = 0; i < list.size(); i++)
    {
      QString str = list[i];

      if(str.startsWith("010 "))
	{
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

	  while(!subfields.isEmpty())
	    if(str.contains(subfields.first()))
	      str = str.mid
		(0, str.indexOf(subfields.takeFirst())).trimmed();
	    else
	      subfields.takeFirst();

	  m_lcnum = str;
	}
      else if(str.startsWith("020 "))
	{
	  /*
	  ** $a - International Standard Book Number (NR)
	  ** $c - Terms of availability (NR)
	  ** $z - Canceled/invalid ISBN (R)
	  ** $6 - Linkage (NR)
	  ** $8 - Field link and sequence number (R)
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  if(str.toLower().contains("hardcover"))
	    m_binding = QObject::tr("hardcover");
	  else if(str.toLower().contains("pbk."))
	    m_binding = QObject::tr("paperback");
	  else
	    m_binding = QObject::tr("UNKNOWN");

	  if(str.contains(" ") && str.indexOf(" ") == 10)
	    m_isbn10 = str.mid(0, 10).trimmed();
	  else if(str.contains(" ") && str.indexOf(" ") == 13)
	    m_isbn13 = str.mid(0, 13).trimmed();
	  else if(str.length() == 10)
	    m_isbn10 = str;
	}
      else if(str.startsWith("050 "))
	{
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

	  while(!subfields.isEmpty())
	    if(str.contains(subfields.first()))
	      str = str.mid
		(0, str.indexOf(subfields.takeFirst())).trimmed();
	    else
	      subfields.takeFirst();

	  m_callnum = str;
	}
      else if(str.startsWith("082 "))
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

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  str = str.remove(" $b").remove
	    (" $m").remove(" $q").trimmed();

	  QStringList subfields;

	  subfields << "$2"
		    << "$6"
		    << "$8";

	  while(!subfields.isEmpty())
	    if(str.contains(subfields.first()))
	      str = str.mid
		(0, str.indexOf(subfields.takeFirst())).trimmed();
	    else
	      subfields.takeFirst();

	  m_deweynum = str;
	}
      else if(str.startsWith("100 ") ||
	      str.startsWith("700 "))
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

	  while(!subfields.isEmpty())
	    if(str.contains(subfields.first()))
	      str = str.mid
		(0, str.indexOf(subfields.takeFirst())).trimmed();
	    else
	      subfields.takeFirst();

	  if(str.endsWith(","))
	    str = str.mid(0, str.length() - 1).trimmed();

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

	  while(!subfields.isEmpty())
	    if(str.contains(subfields.first()))
	      str = str.mid
		(0, str.indexOf(subfields.takeFirst())).trimmed();
	    else
	      subfields.takeFirst();

	  if(str.lastIndexOf('/') > -1)
	    str = str.mid(0, str.lastIndexOf('/')).trimmed();

	  m_title = str;
	}
      else if(str.startsWith("250 "))
	{
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

	  while(!subfields.isEmpty())
	    if(str.contains(subfields.first()))
	      str = str.mid
		(0, str.indexOf(subfields.takeFirst())).trimmed();
	    else
	      subfields.takeFirst();

	  if(str.indexOf(" ") > -1)
	    str = str.mid(0, str.indexOf(" ")).trimmed();

	  str = str.remove("d").remove("h").remove("n").
	    remove("r").remove("s").remove("t").trimmed();
	  m_edition = str;
	}
      else if(str.startsWith("260 ") ||
	      str.startsWith("264 "))
	{
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

	  if(str.startsWith("260 "))
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

	  while(!subfields.isEmpty())
	    if(tmpstr.contains(subfields.first()))
	      tmpstr = tmpstr.mid
		(0, tmpstr.indexOf(subfields.takeFirst())).
		trimmed();
	    else
	      subfields.takeFirst();

	  QStringList tmplist(tmpstr.split("$a"));

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
	       "MM/dd/yyyy");
	  else if(str.indexOf("$c") > -1)
	    m_publicationDate = QDate::fromString
	      ("01/01/" +
	       str.mid(str.indexOf("$c") + 3, 4),
	       "MM/dd/yyyy");

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

	  m_publisher = str;
	}
      else if(str.startsWith("300 "))
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
      else if(str.startsWith("650 "))
	{
	  /*
	  ** $a - Topical term or geographic name entry
	  **      element (NR)
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

	  while(!subfields.isEmpty())
	    if(str.contains(subfields.first()))
	      str = str.mid
		(0, str.indexOf(subfields.takeFirst())).trimmed();
	    else
	      subfields.takeFirst();

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

void marc::parseMagazineZ3950Marc21(void)
{
}

void marc::parseSRU(void)
{
  if(m_itemType == BOOK)
    {
      if(m_recordSyntax == MARC21)
	parseBookSRUMarc21();
    }
}

void marc::parseZ3950(void)
{
  if(m_itemType == BOOK)
    {
      if(m_recordSyntax == MARC21)
	parseBookZ3950Marc21();
    }
  else
    {
      if(m_recordSyntax == MARC21)
	parseMagazineZ3950Marc21();
    }
}

void marc::setData(const QString &data)
{
  m_data = data;

  if(m_protocol == SRU)
    parseSRU();
  else
    parseZ3950();
}
