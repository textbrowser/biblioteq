/*
** Copyright (c) 2006 - present, Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from BiblioteQ without specific prior written permission.
**
** BIBLIOTEQ IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** BIBLIOTEQ, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _BIBLIOTEQ_GENERIC_THREAD_H_
#define _BIBLIOTEQ_GENERIC_THREAD_H_

#include <QPointer>
#include <QStringList>
#include <QThread>

class biblioteq;

class biblioteq_generic_thread: public QThread
{
  Q_OBJECT

 public:
  static const int READ_GLOBAL_CONFIG_FILE = 200;
  static const int Z3950_QUERY = 300;

  biblioteq_generic_thread(QObject *parent, biblioteq *biblioteq);
  ~biblioteq_generic_thread();
  QString getEType(void) const;
  QString getErrorStr(void) const;
  QStringList getList(void) const;
  QStringList getZ3950Results(void) const;
  void msleep(const unsigned long);
  void run(void);
  void setFilename(const QString &filename);
  void setOutputList(const QList<bool> &list);
  void setType(const int type);
  void setZ3950Name(const QString &name);
  void setZ3950SearchString(const QString &z3950SearchStr);

 private:
  QList<bool> m_outputListBool;
  QPointer<biblioteq> qmain;
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
