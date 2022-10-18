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

#ifndef _BIBLIOTEQ_PDFREADER_H_
#define _BIBLIOTEQ_PDFREADER_H_

#include "ui_biblioteq_pdfreader.h"

#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
#include <poppler-qt5.h>
#endif

class QPrinter;

class biblioteq_pdfreader: public QMainWindow
{
  Q_OBJECT

 public:
  biblioteq_pdfreader(QWidget *parent);
  ~biblioteq_pdfreader();
  void load(const QByteArray &data, const QString &fileName);
  void load(const QString &fileName);

 private:
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
  Poppler::Document *m_document;
#endif
  QRectF m_searchLocation;
  QString m_fileName;
  Ui_pdfreader m_ui;
  bool event(QEvent *event);
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void prepareContents(void);
  void resizeEvent(QResizeEvent *event);

 private slots:
  void setGlobalFonts(const QFont &font);
  void slotChangePageViewSize(int value);
  void slotClose(void);
  void slotContentsDoubleClicked(QListWidgetItem *item);
  void slotPrint(void);
  void slotPrintPreview(QPrinter *printer);
  void slotPrintPreview(void);
  void slotSaveAs(void);
  void slotSearchNext(void);
  void slotSearchPrevious(void);
  void slotShowContents(bool state);
  void slotShowPage(int value, const QRectF &location = QRectF());
  void slotSliderTriggerAction(int action);

 public slots:
  void showNormal(void);
};

#endif
