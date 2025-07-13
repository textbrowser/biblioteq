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
  m_qmain = nullptr;
  m_readOnly = false;
}

void biblioteq_hyperlinked_text_edit::keyPressEvent(QKeyEvent *event)
{
  QTextBrowser::keyPressEvent(event);

  if(event == nullptr || isReadOnly())
    return;

  if(event->key() == Qt::AltModifier || event->key() == Qt::Key_Alt)
    QTextBrowser::setReadOnly(true);
}

void biblioteq_hyperlinked_text_edit::keyReleaseEvent(QKeyEvent *event)
{
  QTextBrowser::keyReleaseEvent(event);
  QTextBrowser::setReadOnly(m_readOnly);
}

void biblioteq_hyperlinked_text_edit::setMultipleLinks
(const QString &searchType, const QString &searchField, const QString &str)
{
  if(!m_qmain)
    return;

  QString html("");
  auto const tmpList(str.trimmed().split("\n"));
  int i = 0;

  for(i = 0; i < tmpList.size(); i++)
    {
      if(tmpList[i].trimmed().isEmpty())
	continue;

      html += QString
	("<a href=\"%1?%2?%3\">" + tmpList[i] + "</a>").arg
	(searchType).arg(searchField).arg(tmpList[i].trimmed());

      if(i != tmpList.size() - 1)
	html += "<br>";
    }

  setText(html);

  if(toolTip().trimmed().isEmpty())
    setToolTip(tr("<html>A link may be activated by placing this widget in "
		  "focus and then pressing and holding the Alt key while the "
		  "link is clicked.</html>"));
}

void biblioteq_hyperlinked_text_edit::setQMain(biblioteq *biblioteq)
{
  m_qmain = biblioteq;
}

void biblioteq_hyperlinked_text_edit::setReadOnly(bool state)
{
  QTextBrowser::setReadOnly(state);
  m_readOnly = state;
}

void biblioteq_hyperlinked_text_edit::slotAnchorClicked(const QUrl &url)
{
  if(!m_qmain)
    return;

  auto const tmpList(url.toString().split("?"));

  if(tmpList.size() >= 3)
    {
      auto const searchKey(tmpList[1]);
      auto const searchType(tmpList[0]);
      auto const searchValue(tmpList[2]);

      if(searchType == "book_search")
	m_qmain->bookSearch(searchKey, searchValue);
      else if(searchType == "cd_search")
	m_qmain->cdSearch(searchKey, searchValue);
      else if(searchType == "dvd_search")
	m_qmain->dvdSearch(searchKey, searchValue);
      else if(searchType == "greyliterature_search")
	m_qmain->greyLiteratureSearch(searchKey, searchValue);
      else if(searchType == "journal_search")
	m_qmain->journSearch(searchKey, searchValue);
      else if(searchType == "magazine_search")
	m_qmain->magSearch(searchKey, searchValue);
      else if(searchType == "videogame_search")
	m_qmain->vgSearch(searchKey, searchValue);
    }
}
