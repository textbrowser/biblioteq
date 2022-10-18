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

#ifndef _BIBLIOTEQ_ITEM_H_
#define _BIBLIOTEQ_ITEM_H_

#include <QFont>
#include <QKeyEvent>
#include <QMainWindow>
#include <QPointer>
#include <QPrintDialog>
#include <QPrinter>
#include <QProgressDialog>

class QPersistentModelIndex;
class biblioteq;

class biblioteq_item_working_dialog: public QProgressDialog
{
  Q_OBJECT

 public:
  biblioteq_item_working_dialog(QMainWindow *parent):QProgressDialog(parent)
  {
  }

  ~biblioteq_item_working_dialog()
  {
  }

 protected:
  void closeEvent(QCloseEvent *event)
  {
    if(event)
      event->ignore();
  }

  void keyPressEvent(QKeyEvent *event)
  {
    if(event && event->key() == Qt::Key_Escape)
      event->ignore();
    else
      QProgressDialog::keyPressEvent(event);
  }
};

class biblioteq_item
{
 public:
  biblioteq_item(const QModelIndex &index);
  biblioteq_item(void);
  virtual ~biblioteq_item();
  QString getID(void) const;
  int getOldQ(void) const;
  int getRow(void) const;
  void setOldQ(const int q);
  void updateQuantity(const int q);
  void updateRow(const QModelIndex &index);

 protected:
  QColor m_queryHighlightColor;
  QColor m_requiredHighlightColor;
  QMap<QString, QImage> m_imageValues;
  QMap<QString, QString> m_widgetValues;
  QPersistentModelIndex *m_index;
  QPointer<QMainWindow> m_parentWid;
  QString m_html;
  QString m_oid;
  biblioteq *qmain;
  bool m_isQueryEnabled;
  int m_oldq;
  bool hasDataChanged(QMainWindow *window) const;
  void print(QWidget *parent);
  void setQMain(QMainWindow *window);
  void setReadOnlyFields(QMainWindow *window, const bool state);
  void storeData(QMainWindow *window);
  void updateFont(const QFont &font, QWidget *window);
};

#endif
