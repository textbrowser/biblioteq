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

#ifndef _BIBLIOTEQ_BORROWERS_EDITOR_H_
#define _BIBLIOTEQ_BORROWERS_EDITOR_H_

#include <QPointer>

#include "ui_biblioteq_borrowers.h"

class biblioteq;
class biblioteq_item;

class biblioteq_borrowers_editor: public QDialog
{
  Q_OBJECT

 public:
  biblioteq_borrowers_editor(QWidget *parent,
			     biblioteq *biblioteq,
			     biblioteq_item *bitemArg,
			     const int quantityArg,
			     const QString &ioidArg,
			     const QString &uniqueidArg,
			     const QFont &font,
			     const QString &itemTypeArg,
			     const int stateArg);
  ~biblioteq_borrowers_editor();
  void showUsers(void);

 private:
  QPointer<biblioteq> qmain;
  QString m_ioid;
  QString m_itemType;
  QVector<QString> m_columnHeaderIndexes;
  Ui_checkedOutDialog m_bd;
  biblioteq_item *m_bitem;
  int m_quantity;
  int m_state;
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void setGlobalFonts(const QFont &font);

 private slots:
  void slotCloseCurrentBorrowers(void);
  void slotEraseBorrower(void);
  void slotSave(void);
};

#endif
