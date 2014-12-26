#ifndef _MYQSTRING_H_
#define _MYQSTRING_H_

#include <QRegExp>
#include <QString>

class myqstring: public QString
{
 public:
  myqstring(const char *);
  myqstring(const QString &);
  QString prepConfigString(const QString &, const bool = false);
  static QString escape(const QString &, const bool = false);
};

#endif
