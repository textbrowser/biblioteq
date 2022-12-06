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

#include "biblioteq.h"
#include "biblioteq_hyperlinked_text_edit.h"

biblioteq_hyperlinked_text_edit::biblioteq_hyperlinked_text_edit
(QWidget *parent):QTextBrowser(parent)
{
  connect(this,
	  SIGNAL(anchorClicked(const QUrl &)),
	  this,
	  SLOT(slotAnchorClicked(const QUrl &)));
  m_readOnly = -1;
  qmain = nullptr;
}

void biblioteq_hyperlinked_text_edit::keyPressEvent(QKeyEvent *event)
{
  QTextBrowser::keyPressEvent(event);

  if(qmain && qmain->getDB().driverName() == "QSQLITE")
    {
      if(m_readOnly == -1)
	isReadOnly() ? m_readOnly = 1 : m_readOnly = 0;

      if(event && event->key() == Qt::Key_Alt)
	setReadOnly(true);
    }
}

void biblioteq_hyperlinked_text_edit::keyReleaseEvent(QKeyEvent *event)
{
  QTextBrowser::keyReleaseEvent(event);

  if(qmain && qmain->getDB().driverName() == "QSQLITE")
    setReadOnly(m_readOnly == 1);
}

void biblioteq_hyperlinked_text_edit::setMultipleLinks
(const QString &searchType,
 const QString &searchField,
 const QString &str)
{
  if(!qmain)
    return;

  QString html("");
  QStringList tmplist;
  int i = 0;

  tmplist = str.trimmed().split("\n");

  for(i = 0; i < tmplist.size(); i++)
    {
      html += QString
	("<a href=\"%1?%2?%3\">" + tmplist[i] + "</a>").arg
	(searchType).arg(searchField).arg(tmplist[i].trimmed());

      if(i != tmplist.size() - 1)
	html += "<br>";
    }

  setText(html);

  if(qmain->getDB().driverName() == "QSQLITE" &&
     toolTip().trimmed().isEmpty())
    setToolTip(tr("<html>A link may be activated by placing this widget in "
		  "focus and then pressing and holding the Alt key while the "
		  "link is clicked.</html>"));
}

void biblioteq_hyperlinked_text_edit::setQMain(biblioteq *biblioteq)
{
  qmain = biblioteq;
}

void biblioteq_hyperlinked_text_edit::slotAnchorClicked(const QUrl &url)
{
  if(!qmain)
    return;

  QString searchKey("");
  QString searchType("");
  QString searchValue("");
  QStringList tmplist;
  auto path(url.toString());

  tmplist = path.split("?");

  if(tmplist.size() >= 3)
    {
      searchKey = tmplist[1];
      searchType = tmplist[0];
      searchValue = tmplist[2];

      if(searchType == "book_search")
	qmain->bookSearch(searchKey, searchValue);
      else if(searchType == "cd_search")
	qmain->cdSearch(searchKey, searchValue);
      else if(searchType == "dvd_search")
	qmain->dvdSearch(searchKey, searchValue);
      else if(searchType == "greyliterature_search")
	qmain->greyLiteratureSearch(searchKey, searchValue);
      else if(searchType == "journal_search")
	qmain->journSearch(searchKey, searchValue);
      else if(searchType == "magazine_search")
	qmain->magSearch(searchKey, searchValue);
      else if(searchType == "videogame_search")
	qmain->vgSearch(searchKey, searchValue);
    }

  tmplist.clear();
}
