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

QString myqstring::escape(const QString &str,
			  const bool caseinsensitive)
{
  QString mystr(str);

  mystr = mystr.replace("'", "''");

  if(caseinsensitive)
    mystr = mystr.toLower();

  return mystr.trimmed();
}
