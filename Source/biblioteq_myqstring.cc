#include "biblioteq.h"
#include "biblioteq_myqstring.h"

biblioteq_myqstring::biblioteq_myqstring(const QString &str):QString(str)
{
}

biblioteq_myqstring::biblioteq_myqstring(const char *str):QString(str)
{
}

QString biblioteq_myqstring::escape(const QString &str,
				    const bool caseinsensitive)
{
  QString mystr(str);

  mystr = mystr.replace("'", "''");

  if(caseinsensitive)
    mystr = mystr.toLower();

  return mystr.trimmed();
}

QString biblioteq_myqstring::prepConfigString
(const QString &str, const bool ignore_embedded_comments)
{
  QString mystr = str;

  if(mystr.trimmed().startsWith("#"))
    return mystr.trimmed();
  else if(mystr.contains("#") && !ignore_embedded_comments)
    mystr.resize(mystr.indexOf('#'));

  return mystr.trimmed();
}
