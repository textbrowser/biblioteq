#ifndef _BIBLIOTEQ_MYQSTRING_H_
#define _BIBLIOTEQ_MYQSTRING_H_

#include <QRegExp>
#include <QString>

class biblioteq_myqstring: public QString
{
 public:
  biblioteq_myqstring(const QString &);
  biblioteq_myqstring(const char *);
  QString prepConfigString(const QString &, const bool = false);
  static QString escape(const QString &, const bool = false);
};

#endif
