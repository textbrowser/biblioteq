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

#ifndef _BIBLIOTEQ_STATISTICS_H_
#define _BIBLIOTEQ_STATISTICS_H_

#include <QPointer>

#include "ui_biblioteq_statistics.h"

class biblioteq;
class biblioteq_otheroptions;
class biblioteq_sql_syntax_highlighter;

class biblioteq_statistics: public QMainWindow
{
  Q_OBJECT

 public:
  biblioteq_statistics(biblioteq *parent, biblioteq_otheroptions *other);
  ~biblioteq_statistics();
  void show(QMainWindow *parent, const bool center = true);

 private:
  QPointer<biblioteq_otheroptions> m_otheroptions;
  Ui_statistics m_ui;
  biblioteq *m_qmain;
  biblioteq_sql_syntax_highlighter *m_sqlSyntaxHighlighter;
  void changeEvent(QEvent *event);
  void populateStatistics(void);
  void prepareIcons(void);

 private slots:
  void slotClose(void);
  void slotDelete(void);
  void slotOtherOptionsChanged(void);
  void slotReset(void);
  void slotSave(void);
  void slotSetGlobalFonts(const QFont &font);

 signals:
  void newQuery(void);
};

#endif
