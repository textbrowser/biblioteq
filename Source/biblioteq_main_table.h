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

#ifndef _BIBLIOTEQ_MAIN_TABLE_H_
#define _BIBLIOTEQ_MAIN_TABLE_H_

#include <QPointer>
#include <QTableWidget>

class biblioteq;

class biblioteq_main_table: public QTableWidget
{
  Q_OBJECT

 public:
  biblioteq_main_table(QWidget *parent);
  QHash<QString, QString> friendlyStates(void) const;

  QModelIndex indexFromItem(const QTableWidgetItem *item) const
  {
#if (QT_VERSION < QT_VERSION_CHECK(5, 10, 0))
    return QTableWidget::indexFromItem(const_cast<QTableWidgetItem *> (item));
#else
    return QTableWidget::indexFromItem(item);
#endif
  }

  QStringList columnNames(void) const;
  bool isColumnHidden(const int index,
		      const QString &type,
		      const QString &username) const;
  bool isColumnHidden(int index) const;
  int columnNumber(const QString &name) const;
  void parseStates(const QHash<QString, QString> &states);
  void recordColumnHidden(const QString &username,
			  const QString &type,
			  const int index,
			  const bool hidden);
  void resetTable(const QString &username,
		  const QString &t,
		  const QString &roles);
  void setColumnNames(const QStringList &list);
  void setQMain(biblioteq *biblioteq);
  void updateToolTips(const int row);

 private:
  QHash<QString, QList<int> > m_hiddenColumns;
  QPointer<biblioteq> m_qmain;
  QVector<QString> m_columnHeaderIndexes;
  void keyPressEvent(QKeyEvent *event);
  void setColumns(const QString &username,
		  const QString &type,
		  const QString &roles);

 signals:
  void deleteKeyPressed(void);
  void enterKeyPressed(void);
};

#endif
