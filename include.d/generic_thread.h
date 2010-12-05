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
  Q_OBJECT

 public:
  static const int READ_GLOBAL_CONFIG_FILE = 200;
  static const int Z3950_QUERY = 300;

  generic_thread(void);
  ~generic_thread();
  void run(void);
  void msleep(const int);
  void setType(const int);
  void setFilename(const QString &);
  void setZ3950Name(const QString &);
  void setOutputList(const QList<bool> &);
  void setZ3950SearchString(const QString &);
  QString getEType(void) const;
  QString getErrorStr(void) const;
  QStringList getList(void) const;
  QStringList getZ3950Results(void) const;

 private:
  int type;
  QString eType;
  QString errorStr;
  QString filename;
  QString z3950Name;
  QString z3950SearchStr;
  QList<bool> outputListBool;
  QStringList list;
  QStringList z3950Results;
};

#endif
