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

#ifndef _BIBLIOTEQ_COPY_EDITOR_BOOK_H_
#define _BIBLIOTEQ_COPY_EDITOR_BOOK_H_

#include <QPointer>

#include "biblioteq_copy_editor.h"
#include "ui_biblioteq_bookcopybrowser.h"

class biblioteq_copy_editor_book: public biblioteq_copy_editor
{
  Q_OBJECT

 public:
  biblioteq_copy_editor_book(QWidget *parent,
			     biblioteq *biblioteq,
			     biblioteq_item *bitemArg,
			     const bool showForLendingArg,
			     const int quantityArg,
			     const QString &ioidArg,
			     QSpinBox *spinboxArg,
			     const QFont &font,
			     const QString &uniqueIdArg);
  ~biblioteq_copy_editor_book();
  void populateCopiesEditor(void);

 private:
  class copy_class
  {
  public:
    copy_class(const QString &condition,
	       const QString &copyid,
	       const QString &copynumber,
	       const QString &itemoid,
	       const QString &originality,
	       const QString &status)
    {
      m_condition = condition;
      m_copyid = copyid;
      m_copynumber = copynumber;
      m_itemoid = itemoid;
      m_originality = originality;
      m_status = status;
    };

  public:
    QString m_condition;
    QString m_copyid;
    QString m_copynumber;
    QString m_itemoid;
    QString m_originality;
    QString m_status;
  };

  enum Columns
    {
      AVAILABILITY = 2,
      BARCODE = 1,
      CONDITION = 4,
      COPY_NUMBER = 7,
      MYOID = 6,
      ORIGINALITY = 3,
      STATUS = 5,
      TITLE = 0
    };

  QList<copy_class *> m_copies;
  QPointer<QWidget> m_parent;
  QPointer<biblioteq> qmain;
  QSpinBox *m_spinbox;
  QString m_ioid;
  QString m_itemType;
  QString m_uniqueIdArg;
  QVector<QString> m_columnHeaderIndexes;
  Ui_bookcopybrowser m_cb;
  biblioteq_item *m_bitem;
  bool m_showForLending;
  int m_quantity;
  QString saveCopies(void);
  void changeEvent(QEvent *event);
  void clearCopiesList(void);
  void closeEvent(QCloseEvent *event);
  void keyPressEvent(QKeyEvent *event);

 private slots:
  void slotDeleteCopy(void);
  void slotSaveCopies(void);
};

#endif
