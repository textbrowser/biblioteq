/*
** -- Local Includes --
*/

#include "biblioteq.h"
#include "biblioteq_book.h"
#include "biblioteq_hyperlinked_text_edit.h"

extern biblioteq *qmain;

biblioteq_hyperlinked_text_edit::biblioteq_hyperlinked_text_edit
(QWidget *parent):QTextBrowser(parent)
{
  connect(this, SIGNAL(anchorClicked(const QUrl &)),
	  this, SLOT(slotAnchorClicked(const QUrl &)));
}

void biblioteq_hyperlinked_text_edit::slotAnchorClicked(const QUrl &url)
{
  QString path(url.toString());
  QString searchKey("");
  QString searchType("");
  QString searchValue("");
  QStringList tmplist;

  tmplist = path.split("?");

  if(tmplist.size() >= 3)
    {
      searchType = tmplist[0];
      searchKey = tmplist[1];
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

void biblioteq_hyperlinked_text_edit::setMultipleLinks
(const QString &searchType,
 const QString &searchField,
 const QString &str)
{
  QString html("");
  QStringList tmplist;
  int i = 0;

  tmplist = str.split("\n");

  for(i = 0; i < tmplist.size(); i++)
    {
      html += QString
	("<a href=\"%1?%2?%3\">" + tmplist[i] + "</a>").arg
	(searchType).arg(searchField).arg(tmplist[i].trimmed());

      if(i != tmplist.size() - 1)
	html += "<br>";
    }

  setText(html);
}
