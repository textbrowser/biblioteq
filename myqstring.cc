#include "qtbook.h"
#include "myqstring.h"

extern qtbook *qmain;

/*
** -- myqstring() --
*/

myqstring::myqstring(const QString &str):QString(str)
{
}

/*
** -- myqstring() --
*/

myqstring::myqstring(const char *str):QString(str)
{
}

/*
** -- prepConfigString() --
*/

QString myqstring::prepConfigString(const QString &str,
				    const bool ignore_embedded_comments)
{
  QString mystr = str;

  if(mystr.startsWith("#"))
    return mystr.trimmed();
  else if(mystr.contains("#") && !ignore_embedded_comments)
    mystr.resize(mystr.indexOf('#'));

  mystr = mystr.trimmed();
  return mystr;
}

/*
** -- escapeString() --
*/

QString myqstring::escape(const QString &str)
{
  QString mystr = "";

  if(str.contains("'"))
    {
      if(qmain->getDB().driverName() == "QSQLITE")
	{
	  mystr = str;
	  mystr = mystr.replace("'", "''");
	}
      else
	{
	  mystr = QRegExp::escape(str);
	  mystr = mystr.replace("'", QString("\\'"));
	}
    }
  else
    mystr = str;

  return mystr.trimmed();
}
