#ifndef _BIBLIOTEQ_GENERIC_THREAD_H_
#define _BIBLIOTEQ_GENERIC_THREAD_H_

/*
** -- Qt Includes --
*/

#include <QFile>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QThread>
#include <QWaitCondition>

/*
** -- YAZ Includes --
*/

#ifdef Q_OS_WIN32
#include <yaz.d/zoom.h>
#else
#include <yaz/zoom.h>
#endif

/*
** -- Local Includes --
*/

#include "biblioteq_myqstring.h"

class biblioteq_generic_thread: public QThread
{
  Q_OBJECT

 public:
  static const int READ_GLOBAL_CONFIG_FILE = 200;
  static const int Z3950_QUERY = 300;

  biblioteq_generic_thread(QObject *parent);
  ~biblioteq_generic_thread();
  QString getEType(void) const;
  QString getErrorStr(void) const;
  QStringList getList(void) const;
  QStringList getZ3950Results(void) const;
  void msleep(const int msecs);
  void run(void);
  void setFilename(const QString &filename);
  void setOutputList(const QList<bool> &list);
  void setType(const int type);
  void setZ3950Name(const QString &name);
  void setZ3950SearchString(const QString &z3950SearchStr);

 private:
  QList<bool> m_outputListBool;
  QString m_eType;
  QString m_errorStr;
  QString m_filename;
  QString m_z3950Name;
  QString m_z3950SearchStr;
  QStringList m_list;
  QStringList m_z3950Results;
  int m_type;
};

#endif
