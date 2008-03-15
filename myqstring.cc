#include "myqstring.h"

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
    return mystr;
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

  mystr = QRegExp::escape(str);
  mystr = mystr.replace('\'', QString("\\'"));
  return mystr;
}
