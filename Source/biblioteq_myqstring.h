#ifndef _BIBLIOTEQ_MYQSTRING_H_
#define _BIBLIOTEQ_MYQSTRING_H_

class biblioteq_myqstring: public QString
{
 public:
  biblioteq_myqstring(const QString &str);
  biblioteq_myqstring(const char *str);
  QString prepConfigString(const QString &str,
			   const bool ignore_embedded_comments = false);
  static QString escape(const QString &str,
			const bool caseinsensitive = false);
};

#endif
