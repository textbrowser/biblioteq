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

#ifndef _BIBLIOTEQ_CUSTOM_QUERY_H_
#define _BIBLIOTEQ_CUSTOM_QUERY_H_

#include <QPointer>

#include "ui_biblioteq_customquery.h"

class biblioteq;
class biblioteq_otheroptions;
class biblioteq_sql_syntax_highlighter;
class woody_collapse_expand_tool_button;

class biblioteq_custom_query: public QMainWindow
{
  Q_OBJECT

 public:
  biblioteq_custom_query(biblioteq_otheroptions *other, biblioteq *parent);
  ~biblioteq_custom_query();

 private:
  Ui_customquery cq;
  QPointer<biblioteq> m_parent;
  QPointer<biblioteq_otheroptions> m_otheroptions;
  biblioteq_sql_syntax_highlighter *m_sqlSyntaxHighlighter;
  woody_collapse_expand_tool_button *m_woody;
  void changeEvent(QEvent *event);
  void closeEvent(QCloseEvent *event);

 private slots:
  void slotCustomQueryFavorite(bool state);
  void slotDeleteFavoriteQuery(void);
  void slotExecuteCustomQuery(void);
  void slotLoadFavorite(void);
  void slotNewFavorite(void);
  void slotOtherOptionsChanged(void);
  void slotPopulateFavorites(void);
  void slotRefreshCustomQuery(void);
  void slotRenameFavoriteQuery(void);
  void slotSaveCustomQuery(void);

 signals:
  void favoritesChanged(void);
  void newFavorite(void);
};

#endif
