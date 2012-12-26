#ifndef _GENERIC_THREAD_H_
#define _GENERIC_THREAD_H_

/*
** -- Qt Includes --
*/

#include <QFile>
#include <QString>
#include <QThread>
#include <QStringList>
#include <QTextStream>
#include <QWaitCondition>

/*
** -- YAZ Includes --
*/

#include <yaz/zoom.h>

/*
** -- Local Includes --
*/

#include "myqstring.h"

class generic_thread: public QThread
{
  Q_OBJECT

 public:
  static const int READ_GLOBAL_CONFIG_FILE = 200;
  static const int SRU_QUERY = 250;
  static const int Z3950_QUERY = 300;

  generic_thread(QObject *);
  ~generic_thread();
  void run(void);
  void start(void);
  void msleep(const int);
  void setType(const int);
  void setSRUName(const QString &);
  void setFilename(const QString &);
  void setZ3950Name(const QString &);
  void setOutputList(const QList<bool> &);
  void setSRUSearchString(const QString &);
  void setZ3950SearchString(const QString &);
  QString getEType(void) const;
  QString getErrorStr(void) const;
  QByteArray getSRUResults(void) const;
  QStringList getList(void) const;
  QStringList getZ3950Results(void) const;

 private:
  int m_type;
  QString m_eType;
  QString m_errorStr;
  QString m_filename;
  QString m_sruName;
  QString m_z3950Name;
  QString m_sruSearchStr;
  QString m_z3950SearchStr;
  QByteArray m_sruResults;
  QList<bool> m_outputListBool;
  QStringList m_list;
  QStringList m_z3950Results;
  QWaitCondition m_sruCondition;

 private slots:
  void slotReadyRead(void);
  void slotDownloadFinished(void);
};

#endif
