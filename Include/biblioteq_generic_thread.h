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

#include <yaz/zoom.h>

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

  biblioteq_generic_thread(QObject *);
  ~biblioteq_generic_thread();
  QString getEType(void) const;
  QString getErrorStr(void) const;
  QStringList getList(void) const;
  QStringList getZ3950Results(void) const;
  void msleep(const int);
  void run(void);
  void setFilename(const QString &);
  void setOutputList(const QList<bool> &);
  void setType(const int);
  void setZ3950Name(const QString &);
  void setZ3950SearchString(const QString &);

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
