#ifndef _GENERIC_THREAD_H_
#define _GENERIC_THREAD_H_

/*
** -- Qt Includes --
*/

#include <QDir>
#include <QFile>
#include <QString>
#include <QThread>
#include <QTextStream>

/*
** -- YAZ Includes --
*/

#include <yaz/zoom.h>

/*
** -- Local Includes --
*/

#include "qtbook.h"
#include "myqstring.h"

class generic_thread: public QThread
{
 public:
  static const int READ_USER_CONFIG_FILE = 100;
  static const int WRITE_USER_CONFIG_FILE = 101;
  static const int READ_GLOBAL_CONFIG_FILE = 200;
  static const int QUERY_LIBRARY_OF_CONGRESS = 300;

  generic_thread(void);
  ~generic_thread();
  void run(void);
  void setType(const int);
  void setFilename(const QString &);
  void setOutputList(const QList<bool> &);
  void setLOCSearchString(const QString &);
  QString getEType(void);
  QString getErrorStr(void);
  QStringList getList(void);
  QStringList getLOCResults(void);

 private:
  int type;
  QString eType;
  QString errorStr;
  QString filename;
  QString LOCSearchStr;
  QList<bool> outputListBool;
  QStringList list;
  QStringList LOCResults;
};

#endif
