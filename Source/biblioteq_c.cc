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
#include "biblioteq_boolean_table_item.h"
#include "biblioteq_copy_editor.h"
#include "biblioteq_custom_query.h"
#include "biblioteq_documentationwindow.h"
#include "biblioteq_files.h"
#include "biblioteq_graphicsitempixmap.h"
#include "biblioteq_otheroptions.h"
#include "biblioteq_pdfreader.h"
#include "biblioteq_query_history.h"
#include "biblioteq_sql_syntax_highlighter.h"
#include "biblioteq_sqlite_merge_databases.h"
#include "biblioteq_statistics.h"
#include "biblioteq_woody.h"
#include "ui_biblioteq_generalmessagediag.h"

#include <QActionGroup>
#include <QDesktopServices>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QScrollBar>
#include <QSettings>
#include <QSqlDriver>
#include <QSqlField>
#include <QSqlRecord>
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#include <QTextCodec>
#endif
#include <QTextStream>
#include <QtMath>

#include <limits>

#ifdef BIBLIOTEQ_SQLITE3_INCLUDE_FILE_EXISTS
extern "C"
{
#include <sqlite3.h>
}

static void regexp(sqlite3_context *context, int argc, sqlite3_value **argv)
{
  if(argc != 2 || context == nullptr)
    /*
    ** We require two values.
    */

    return;

  auto expression = reinterpret_cast<const char *>
    (sqlite3_value_text(argv[0]));
  auto text = reinterpret_cast<const char *> (sqlite3_value_text(argv[1]));

  if(!expression || !text)
    /*
    ** We require two values.
    */

    return;

  QRegularExpression const r(expression);

  sqlite3_result_int
    (context, QRegularExpressionMatch(r.match(text)).hasMatch());
}
#endif

QColor biblioteq::availabilityColor(const QString &itemType) const
{
  return m_otherOptions->availabilityColor(itemType);
}

QHash<QString, QString> biblioteq::getOpenLibraryImagesHash(void) const
{
  return m_openLibraryImages;
}

QHash<QString, QString> biblioteq::getOpenLibraryItemsHash(void) const
{
  return m_openLibraryItems;
}

QString biblioteq::dateFormat(const QString &itemType) const
{
  return m_otherOptions->dateFormat(itemType);
}

QString biblioteq::dbUserName(void) const
{
  if(m_db.driverName() != "QSQLITE")
    return m_db.userName();
  else
    return "SQLITE";
}

QString biblioteq::publicationDateFormat(const QString &itemType) const
{
  return dateFormat(itemType);
}

QString biblioteq::reservationHistoryHtml(void) const
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  QString firstname("");
  QString html("<html>");
  QString information("");
  QString lastname("");
  QString memberid("");
  static QString const endl("<br>");

  for(int i = 0; i < history.table->rowCount(); i++)
    if(m_roles.isEmpty())
      {
	if(i == 0)
	  memberid = history.table->item(i, 0)->text();

	information += history.table->item(i, 1)->text(); // Title
	information += endl;
	information += history.table->item(i, 2)->text(); // ID Number
	information += endl;
	information += history.table->item(i, 4)->text(); // Type
	information += endl;
	information += tr("Reservation Date: ");
	information += history.table->item(i, 5)->text(); // Reservation Date
	information += endl;
	information += tr("Due Date: ");
	information += history.table->item(i, 6)->text(); // Due Date
	information += endl;
	information += tr("Returned Date: ");
	information += history.table->item(i, 7)->text(); // Returned Date

	if(history.table->rowCount() - 1 != i)
	  {
	    information += endl;
	    information += endl;
	  }
      }
    else
      {
	if(i == 0)
	  {
	    for(int j = 0; j < history.table->columnCount(); j++)
	      if(j == static_cast<int> (HistoryColumns::FIRST_NAME))
		firstname = history.table->item(i, j)->text();
	      else if(j == static_cast<int> (HistoryColumns::LAST_NAME))
		lastname = history.table->item(i, j)->text();
	      else if(j == static_cast<int> (HistoryColumns::MEMBER_ID))
		memberid = history.table->item(i, j)->text();
	      else
		break;
	  }

	information += history.table->item
	  (i, static_cast<int> (HistoryColumns::TITLE))->text();
	information += endl;
	information += history.table->item
	  (i, static_cast<int> (HistoryColumns::ID_NUMBER))->text();
	information += endl;
	information += history.table->item
	  (i, static_cast<int> (HistoryColumns::TYPE))->text();
	information += endl;
	information += tr("Reservation Date: ");
	information += history.table->item
	  (i, static_cast<int> (HistoryColumns::RESERVATION_DATE))->text();
	information += endl;
	information += tr("Due Date: ");
	information += history.table->item
	  (i, static_cast<int> (HistoryColumns::DUE_DATE))->text();
	information += endl;
	information += tr("Returned Date: ");
	information += history.table->item
	  (i, static_cast<int> (HistoryColumns::RETURNED_DATE))->text();

	if(history.table->rowCount() - 1 != i)
	  {
	    information += endl;
	    information += endl;
	  }
      }

  if(m_roles.isEmpty())
    {
      html += memberid;
      html += endl;
      html += endl;
      html += information.trimmed();
    }
  else
    {
      html += memberid;
      html += endl;
      html += lastname;
      html += ", ";
      html += firstname;
      html += endl;
      html += endl;
      html += information.trimmed();
    }

  html += "</html>";
  QApplication::restoreOverrideCursor();
  return html;
}

QString biblioteq::viewHtml(void) const
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  QString html = "<html>";

  html += "<table border=1>";
  html += "<tr>";

  for(int i = 0; i < ui.table->columnCount(); i++)
    if(!ui.table->isColumnHidden(i))
      html += "<th>" + ui.table->horizontalHeaderItem(i)->text() + "</th>";

  html += "</tr>";

  for(int i = 0; i < ui.table->rowCount(); i++)
    {
      html += "<tr>";

      for(int j = 0; j < ui.table->columnCount(); j++)
	if(!ui.table->isColumnHidden(j))
	  html += "<td>" + ui.table->item(i, j)->text() + "</td>";

      html += "</tr>";
    }

  html += "</table>";
  html += "</html>";
  QApplication::restoreOverrideCursor();
  return html;
}

QStringList biblioteq::getSRUNames(void) const
{
  return m_sruMaps.keys();
}

QStringList biblioteq::getZ3950Names(void) const
{
  return m_z3950Maps.keys();
}

QWidget *biblioteq::widgetForAction(QAction *action) const
{
  if(!action)
    return nullptr;

  if(action == ui.configTool)
    return ui.toolBar_5->widgetForAction(action);
  else if(action == ui.createTool)
    return ui.toolBar_2->widgetForAction(action);
  else if(action == ui.printTool)
    return ui.toolBar_2->widgetForAction(action);
  else if(action == ui.searchTool)
    return ui.toolBar_4->widgetForAction(action);

  return nullptr;
}

bool biblioteq::availabilityColors(void) const
{
  QSettings settings;

  return settings.value("otheroptions/availability_colors", false).toBool();
}

bool biblioteq::emptyContainers(void)
{
  for(int i = 1; i < ui.tab->count(); i++)
    {
      auto w = dynamic_cast<biblioteq_item *> (ui.tab->widget(i));

      if(!w)
	continue;

      auto book = dynamic_cast<biblioteq_book *> (w);
      auto cd = dynamic_cast<biblioteq_cd *> (w);
      auto dvd = dynamic_cast<biblioteq_dvd *> (w);
      auto gl = dynamic_cast<biblioteq_grey_literature *> (w);
      auto journal = dynamic_cast<biblioteq_journal *> (w);
      auto magazine = dynamic_cast<biblioteq_magazine *> (w);
      auto photograph = dynamic_cast<biblioteq_photographcollection *> (w);
      auto videogame = dynamic_cast<biblioteq_videogame *> (w);

      if(book)
	{
	  if(!book->close())
	    return false;
	  else
	    book->deleteLater();
	}

      if(cd)
	{
	  if(!cd->close())
	    return false;
	  else
	    cd->deleteLater();
	}

      if(dvd)
	{
	  if(!dvd->close())
	    return false;
	  else
	    dvd->deleteLater();
	}

      if(gl)
	{
	  if(!gl->close())
	    return false;
	  else
	    gl->deleteLater();
	}

      if(journal)
	{
	  if(!journal->close())
	    return false;
	  else
	    journal->deleteLater();
	}

      if(!dynamic_cast<biblioteq_journal *> (w))
	if(magazine)
	  {
	    if(!magazine->close())
	      return false;
	    else
	      magazine->deleteLater();
	  }

      if(photograph)
	{
	  if(!photograph->close())
	    return false;
	  else
	    photograph->deleteLater();
	}

      if(videogame)
	{
	  if(!videogame->close())
	    return false;
	  else
	    videogame->deleteLater();
	}
    }

  foreach(auto w, QApplication::topLevelWidgets())
    {
      auto book = qobject_cast<biblioteq_book *> (w);
      auto cd = qobject_cast<biblioteq_cd *> (w);
      auto dvd = qobject_cast<biblioteq_dvd *> (w);
      auto gl = qobject_cast<biblioteq_grey_literature *> (w);
      auto journal = qobject_cast<biblioteq_journal *> (w);
      auto magazine = qobject_cast<biblioteq_magazine *> (w);
      auto photograph = qobject_cast<biblioteq_photographcollection *> (w);
      auto videogame = qobject_cast<biblioteq_videogame *> (w);

      if(book)
	{
	  if(book->isVisible() && !book->close())
	    return false;
	  else
	    book->deleteLater();
	}

      if(cd)
	{
	  if(cd->isVisible() && !cd->close())
	    return false;
	  else
	    cd->deleteLater();
	}

      if(dvd)
	{
	  if(dvd->isVisible() && !dvd->close())
	    return false;
	  else
	    dvd->deleteLater();
	}

      if(gl)
	{
	  if(gl->isVisible() && !gl->close())
	    return false;
	  else
	    gl->deleteLater();
	}

      if(journal)
	{
	  if(journal->isVisible() && !journal->close())
	    return false;
	  else
	    journal->deleteLater();
	}

      if(!qobject_cast<biblioteq_journal *> (w))
	if(magazine)
	  {
	    if(magazine->isVisible() && !magazine->close())
	      return false;
	    else
	      magazine->deleteLater();
	  }

      if(photograph)
	{
	  if(photograph->isVisible() && !photograph->close())
	    return false;
	  else
	    photograph->deleteLater();
	}

      if(videogame)
	{
	  if(videogame->isVisible() && !videogame->close())
	    return false;
	  else
	    videogame->deleteLater();
	}
    }

  return true;
}

int biblioteq::populateTable(QSqlQuery *query,
			     const QString &typefilter,
			     const int pagingType,
			     const int searchType)
{
  QElapsedTimer elapsed;

  elapsed.start();

  if(m_searchQuery != query)
    {
      delete m_searchQuery;
      m_searchQuery = query;
    }

  if(!m_searchQuery)
    return 1;

  if(pagingType == NEW_PAGE)
    {
      if(m_searchQuery->isActive())
	m_searchQuery->clear();
    }

  if(m_configToolMenu)
    m_configToolMenu->close();

  m_lastSearchStr = "Item Search Query";

  if(searchType == CUSTOM_QUERY)
    {
      if(m_configToolMenu)
	m_configToolMenu->deleteLater();

      ui.configTool->setEnabled(false);
      ui.configTool->setToolTip(tr("Disabled for custom queries."));
    }
  else
    {
      ui.configTool->setEnabled(true);
      ui.configTool->setToolTip("");
    }

  ui.itemsCountLabel->setText(tr("0 Results"));

  QScopedPointer<QProgressDialog> progress;

  if(m_otherOptions->showMainTableProgressDialogs())
    {
      auto closeButton = new QPushButton(tr("Interrupt"));

      closeButton->setShortcut(QKeySequence(Qt::Key_F8));
      progress.reset(new QProgressDialog(this));
      progress->hide();
      progress->setCancelButton(closeButton);
    }

  QTableWidgetItem *item = nullptr;
  QString itemType("");
  QString str("");
  auto const columns = imageModeColumns();
  auto const limit = pageLimit();
  auto offset = m_queryOffset;
  int i = -1;

  if(limit != -1)
    {
      if(pagingType != NEW_PAGE)
	{
	  if(pagingType == PREVIOUS_PAGE)
	    {
	      offset -= limit;

	      if(offset < 0)
		offset = 0;
	    }
	  else if(pagingType == NEXT_PAGE)
	    offset += limit;
	  else
	    {
	      /*
	      ** A specific page was selected from ui.pagesLabel.
	      */

	      offset = 0;

	      for(int ii = 1; ii < qAbs(pagingType); ii++)
		offset += limit;
	    }
	}
      else
	offset = 0;

      ui.graphicsView->setSceneRect
	(0.0,
	 0.0,
	 150.0 * static_cast<qreal> (columns),
	 (limit / static_cast<qreal> (columns)) * 200.0 + 200.0);
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(ui.table->rowCount() == 0)
    ui.itemsCountLabel->setText(tr("0 Results"));
  else
    ui.itemsCountLabel->setText
      (tr("%1 Result(s)").arg(ui.table->rowCount()));

  auto ok = true;

  if(pagingType == NEW_PAGE)
    ok = m_searchQuery->exec();
  else if(pagingType == NEXT_PAGE || pagingType == PREVIOUS_PAGE)
    ok = m_searchQuery->seek(static_cast<int> (offset));
  else if(pagingType < 0)
    ok = m_searchQuery->seek(limit * qAbs(pagingType + 1));

  if(!ok || m_searchQuery->lastError().isValid())
    {
      if(progress)
	progress->close();

      QApplication::processEvents();
      QApplication::restoreOverrideCursor();

      if(!m_previousTypeFilter.isEmpty())
	for(int ii = 0; ii < ui.menu_Category->actions().size(); ii++)
	  if(m_previousTypeFilter ==
	     ui.menu_Category->actions().at(ii)->data().toString())
	    {
	      ui.categoryLabel->setText
		(ui.menu_Category->actions().at(ii)->text());
	      ui.menu_Category->actions().at(ii)->setChecked(true);
	      ui.menu_Category->setDefaultAction
		(ui.menu_Category->actions().at(ii));
	      break;
	    }

      if(m_searchQuery->lastError().isValid())
	addError(tr("Database Error"),
		 tr("Unable to retrieve the data required for "
		    "populating the main views."),
		 m_searchQuery->lastError().text(),
		 __FILE__,
		 __LINE__);

      QMessageBox::critical
	(this,
	 tr("BiblioteQ: Database Error"),
	 tr("Unable to retrieve the data required for "
	    "populating the main views."));
      QApplication::processEvents();
      delete m_searchQuery;
      m_searchQuery = nullptr;
      ui.graphicsView->setSceneRect
	(ui.graphicsView->scene()->itemsBoundingRect());
      return 1;
    }

  prepareRequestToolButton(typefilter);

  auto found = false;

  for(int ii = 0; ii < ui.menu_Category->actions().size(); ii++)
    if(typefilter == ui.menu_Category->actions().at(ii)->data().toString())
      {
	found = true;
	m_previousTypeFilter = typefilter;
	ui.categoryLabel->setText(ui.menu_Category->actions().at(ii)->text());
	ui.menu_Category->actions().at(ii)->setChecked(true);
	ui.menu_Category->setDefaultAction(ui.menu_Category->actions().at(ii));
	break;
      }

  if(typefilter.isEmpty())
    {
      ui.categoryLabel->setText(tr("All"));

      if(!ui.menu_Category->actions().isEmpty())
	ui.menu_Category->actions().at(0)->setChecked(true);

      ui.menu_Category->setDefaultAction(ui.menu_Category->actions().value(0));
    }
  else if(!found)
    {
      ui.categoryLabel->setText(tr("All"));

      if(!ui.menu_Category->actions().isEmpty())
	ui.menu_Category->actions().at(0)->setChecked(true);

      ui.menu_Category->setDefaultAction(ui.menu_Category->actions().value(0));
    }

  disconnect(ui.table,
	     SIGNAL(itemChanged(QTableWidgetItem *)),
	     this,
	     SLOT(slotItemChanged(QTableWidgetItem *)));
  ui.table->resetTable(dbUserName(), typefilter, m_roles);

  qint64 currentPage = 0;

  if(limit <= 0)
    currentPage = 1;
  else
    currentPage = offset / limit + 1;

  if(pagingType == NEW_PAGE)
    m_pages = 0;

  if(currentPage > m_pages &&
     pagingType != PREVIOUS_PAGE &&
     pagingType >= 0)
    m_pages += 1;

  if(limit == -1)
    m_pages = 1;

  if(m_pages == 1)
    ui.pagesLabel->setText(tr("1"));
  else if(m_pages >= 2 && m_pages <= 10)
    {
      QString str("");

      for(qint64 ii = 1; ii <= m_pages; ii++)
	if(ii == currentPage)
	  str += tr(" %1 ").arg(currentPage);
	else
	  str += QString(" <a href=\"%1\">" + tr("%1") + "</a> ").arg(ii);

      str = str.trimmed();
      ui.pagesLabel->setText(str);
    }
  else
    {
      QString str("");
      qint64 start = 2;

      if(currentPage == 1)
	str += tr(" 1 ... ");
      else
	str += " <a href=\"1\">" + tr("1") + "</a>" + tr(" ... ");

      if(currentPage != 1)
	while(!(start <= currentPage && currentPage <= start + 6))
	  start += 7;

      for(qint64 ii = start; ii <= start + 6; ii++)
	if(ii == currentPage && ii <= m_pages - 1)
	  str += tr(" %1 ").arg(ii);
	else if(ii <= m_pages - 1)
	  str += QString(" <a href=\"%1\">" + tr("%1") + "</a> ").arg(ii);

      if(currentPage == m_pages)
	str += tr(" ... %1 ").arg(currentPage);
      else
	str += QString(" ... <a href=\"%1\">" + tr("%1") + "</a> ").
	  arg(m_pages);

      str = str.trimmed();
      ui.pagesLabel->setText(str);
    }

  m_lastSearchType = searchType;
  ui.table->clearSelection();
  ui.table->horizontalScrollBar()->setValue(0);
  ui.table->scrollToTop();
  ui.table->setCurrentItem(nullptr);
  slotDisplaySummary();
  ui.graphicsView->horizontalScrollBar()->setValue(0);
  ui.graphicsView->resetTransform();
  ui.graphicsView->scene()->clear();
  ui.graphicsView->verticalScrollBar()->setValue(0);
  ui.table->setSortingEnabled(false);

  if(progress)
    {
      QApplication::restoreOverrideCursor();
      progress->setLabelText(tr("Populating the views..."));
      progress->setMaximum(limit == -1 ? 0 : limit);
      progress->setMinimum(0);
      progress->setMinimumWidth
	(qCeil(PROGRESS_DIALOG_WIDTH_MULTIPLIER *
	       progress->sizeHint().width()));
      progress->setModal(true);
      progress->setWindowTitle(tr("BiblioteQ: Progress Dialog"));
      raise();
      progress->show();
      progress->update();
      progress->repaint();
      QApplication::processEvents();
    }

  int iconTableColumnIdx = 0;
  int iconTableRowIdx = 0;

  /*
  ** Adjust the dimensions of the graphics scene if pagination is disabled.
  */

  if(limit == -1)
    {
      m_searchQuery->seek(0);

      int size = 0;

      while(m_searchQuery->next())
	size += 1;

      if(size > 0)
	ui.graphicsView->setSceneRect
	  (0.0,
	   0.0,
	   150.0 * static_cast<qreal> (columns),
	   (size / static_cast<qreal> (columns)) * 200.0 + 200.0);

      if(progress && size >= 0)
	progress->setMaximum(size);

      m_searchQuery->seek(static_cast<int> (offset));
    }

  if(limit != -1 &&
     m_db.driver()->hasFeature(QSqlDriver::QuerySize) &&
     progress)
    progress->setMaximum(qMin(limit, m_searchQuery->size()));

  QFontMetrics const fontMetrics(ui.table->font());
  QHash<QString, QString> dateFormats;
  QLocale locale;
  QMap<QByteArray, QImage> images;
  QSettings settings;
  auto const availabilityColors = this->availabilityColors();
  auto const booksAccessionNumberIndex =
    m_otherOptions->booksAccessionNumberIndex();
  auto const columnNames(ui.table->columnNames());
  auto const showBookReadStatus = m_db.driverName() == "QSQLITE" &&
    m_otherOptions->showBookReadStatus() &&
    typefilter == "Books";
  auto const showMainTableImages = m_otherOptions->showMainTableImages();
  auto const showToolTips = settings.value
    ("show_maintable_tooltips", false).toBool();

  i = -1;

  while(i++, true)
    {
      if(i == limit)
	break;

      if(progress && progress->wasCanceled())
	break;

      if(m_searchQuery->at() == QSql::BeforeFirstRow)
	if(!m_searchQuery->next())
	  break;

      biblioteq_graphicsitempixmap *pixmapItem = nullptr;
      biblioteq_numeric_table_item *availabilityItem = nullptr;
      quint64 myoid = 0;

      if(m_searchQuery->isValid())
	{
	  QString tooltip("");
	  QTableWidgetItem *first = nullptr;
	  auto const record(m_searchQuery->record());

	  itemType = record.field("type").value().
	    toString().remove(' ').toLower().trimmed();

	  if(!dateFormats.contains(itemType))
	    dateFormats[itemType] = dateFormat(itemType);

	  if(showToolTips)
	    {
	      tooltip = "<html>";

	      for(int j = 0; j < record.count(); j++)
		{
		  auto const fieldName(record.fieldName(j));

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
		  if(QMetaType::Type(record.field(j).metaType().id()) ==
		     QMetaType::QByteArray ||
#else
		  if(record.field(j).type() == QVariant::ByteArray ||
#endif
		     fieldName.contains("cover") ||
		     fieldName.contains("image"))
		    continue;

		  QString columnName("");

		  if(showBookReadStatus)
		    columnName = columnNames.value(j + 1);
		  else
		    columnName = columnNames.value(j);

		  if(columnName.isEmpty())
		    columnName = "N/A";

		  tooltip.append("<b>");
		  tooltip.append(columnName);
		  tooltip.append(":</b> ");

#if QT_VERSION > 0x050501
		  if(record.field(j).tableName() == "book" &&
		     (fieldName == "id" || fieldName == "isbn13"))
#else
		  if(fieldName == "id" || fieldName == "isbn13")
#endif
		    {
		      auto str(m_searchQuery->value(j).toString().trimmed());

		      if(fieldName == "id")
			str = m_otherOptions->isbn10DisplayFormat(str);
		      else
			str = m_otherOptions->isbn13DisplayFormat(str);

		      tooltip.append(str);
		    }
		  else
		    tooltip.append
		      (m_searchQuery->value(j).
		       toString().simplified().replace("<br>", " ").
		       simplified().trimmed());

		  tooltip.append("<br>");
		}

	      if(tooltip.endsWith("<br>"))
		tooltip = tooltip.mid(0, tooltip.length() - 4);

	      tooltip.append("</html>");
	    }

	  for(int j = 0; j < record.count(); j++)
	    {
	      item = nullptr;

	      auto const fieldName(record.fieldName(j));

	      if(!fieldName.endsWith("front_cover") &&
		 !fieldName.endsWith("image_scaled"))
		{
		  if(fieldName.contains("date") ||
		     fieldName.contains("membersince"))
		    {
		      auto const date
			(QDate::fromString(m_searchQuery->value(j).
					   toString().trimmed(),
					   s_databaseDateFormat));

		      if(dateFormats.value(itemType).isEmpty())
			str = date.toString(Qt::ISODate);
		      else
			str = date.toString(dateFormats.value(itemType));

		      if(str.isEmpty())
			str = m_searchQuery->value(j).toString().trimmed();
		    }
		  else
		    str = m_searchQuery->value(j).toString().trimmed();
		}

#if QT_VERSION > 0x050501
	      if((fieldName == "id" || fieldName == "isbn13") &&
		 record.field(j).tableName() == "book")
#else
	      if(fieldName == "id" || fieldName == "isbn13")
#endif
		{
		  if(fieldName == "id")
		    str = m_otherOptions->isbn10DisplayFormat(str);
		  else
		    str = m_otherOptions->isbn13DisplayFormat(str);

		  item = new QTableWidgetItem(str);
		}
	      else if(fieldName.endsWith("accession_number"))
		{
		  if(typefilter == "Books")
		    {
		      if(booksAccessionNumberIndex == 0)
			item = new biblioteq_numeric_table_item
			  (m_searchQuery->value(j).toInt());
		      else
			item = new QTableWidgetItem();
		    }
		  else
		    item = new QTableWidgetItem();
		}
	      else if(fieldName.endsWith("availability") ||
		      fieldName.endsWith("cddiskcount") ||
		      fieldName.endsWith("dvddiskcount") ||
		      fieldName.endsWith("file_count") ||
		      fieldName.endsWith("issue") ||
		      fieldName.endsWith("issueno") ||
		      fieldName.endsWith("issuevolume") ||
		      fieldName.endsWith("photograph_count") ||
		      fieldName.endsWith("price") ||
		      fieldName.endsWith("quantity") ||
		      fieldName.endsWith("total_reserved") ||
		      fieldName.endsWith("volume"))
		{
		  if(fieldName.endsWith("price"))
		    {
		      item = new biblioteq_numeric_table_item
			(m_searchQuery->value(j).toDouble());
		      str = locale.toString
			(m_searchQuery->value(j).toDouble());
		    }
		  else
		    {
		      item = new biblioteq_numeric_table_item
			(m_searchQuery->value(j).toInt());

		      if(availabilityColors &&
			 fieldName.endsWith("availability"))
			availabilityItem = dynamic_cast
			  <biblioteq_numeric_table_item *> (item);
		    }
		}
	      else if(fieldName.endsWith("callnumber"))
		{
		  str = m_searchQuery->value(j).toString().trimmed();
		  item = new biblioteq_callnum_table_item(str);
		}
	      else if(fieldName.endsWith("duedate"))
		{
		  item = new QTableWidgetItem
		    (m_searchQuery->value(j).toString().trimmed());

		  auto const duedate
		    (QDate::
		     fromString(m_searchQuery->value(j).toString().trimmed(),
				s_databaseDateFormat));

		  if(QDate::currentDate() > duedate)
		    item->setBackground
		      (m_otherOptions->overdueColor(itemType));
		}
	      else if(fieldName.endsWith("front_cover") ||
		      fieldName.endsWith("image_scaled"))
		{
		  QImage image;

		  if(showMainTableImages)
		    {
		      if(!m_searchQuery->isNull(j))
			{
			  auto bytes
			    (QByteArray::fromBase64(m_searchQuery->value(j).
						    toByteArray()));

			  if(images.contains(bytes))
			    image = images.value(bytes);
			  else
			    {
			      image.loadFromData(bytes);

			      if(!image.isNull())
				images[bytes] = image;
			    }

			  if(image.isNull())
			    {
			      bytes = m_searchQuery->value(j).toByteArray();

			      if(images.contains(bytes))
				image = images.value(bytes);
			      else
				{
				  image.loadFromData(bytes);

				  if(!image.isNull())
				    images[bytes] = image;
				}
			    }
			}
		    }

		  if(image.isNull())
		    {
		      if(images.contains(QByteArray()))
			image = images.value(QByteArray());
		      else
			{
			  image = QImage(":/missing_image.png");
			  images[QByteArray()] = image;
			}
		    }

		  /*
		  ** The size of missing_image.png is AxB.
		  */

		  if(!image.isNull())
		    image = image.scaled
		      (s_noImageResize,
		       Qt::KeepAspectRatio,
		       Qt::SmoothTransformation);

		  pixmapItem = new biblioteq_graphicsitempixmap
		    (QPixmap::fromImage(image), nullptr);

		  if(iconTableRowIdx == 0)
		    pixmapItem->setPos(140.0 * iconTableColumnIdx, 15.0);
		  else
		    pixmapItem->setPos(140.0 * iconTableColumnIdx,
				       200.0 * iconTableRowIdx + 15.0);

		  iconTableColumnIdx += 1;
		  pixmapItem->setFlag
		    (QGraphicsItem::ItemIsSelectable, true);
		  pixmapItem->setToolTip(tooltip);
		  ui.graphicsView->scene()->addItem(pixmapItem);

		  if(columns <= iconTableColumnIdx)
		    {
		      iconTableColumnIdx = 0;
		      iconTableRowIdx += 1;
		    }
		}
	      else
		item = new QTableWidgetItem();

	      if(item != nullptr)
		{
		  item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		  item->setText
		    (str.simplified().replace("<br>", " ").
		     simplified().trimmed());

		  if(j == 0)
		    {
		      first = item;
		      ui.table->setRowCount(ui.table->rowCount() + 1);
		    }

		  if(!tooltip.isEmpty())
		    item->setToolTip(tooltip);

		  if(showBookReadStatus)
		    ui.table->setItem(i, j + 1, item);
		  else
		    ui.table->setItem(i, j, item);

		  if(fieldName.endsWith("myoid"))
		    {
		      myoid = m_searchQuery->value(j).toULongLong();
		      updateRows(str, item, itemType);
		    }
		}
	    }

	  if(availabilityItem && availabilityItem->value().toInt() > 0)
	    availabilityItem->setBackground(availabilityColor(itemType));

	  if(first && showMainTableImages)
	    {
	      if(pixmapItem)
		first->setIcon(pixmapItem->pixmap());
	      else
		first->setIcon(QIcon(":/missing_image.png"));

	      ui.table->setRowHeight
		(i,
		 qMax(fontMetrics.height() + 10,
		      ui.table->iconSize().height()));
	    }

	  if(showBookReadStatus)
	    {
	      /*
	      ** Was the book read?
	      */

	      auto item = new biblioteq_boolean_table_item();

	      if(itemType == "book")
		{
		  item->setCheckState
		    (biblioteq_misc_functions::isBookRead(m_db, myoid) ?
		     Qt::Checked : Qt::Unchecked);
		  item->setData(Qt::UserRole, myoid);
		  item->setFlags(Qt::ItemIsEnabled |
				 Qt::ItemIsSelectable |
				 Qt::ItemIsUserCheckable);
		}
	      else
		item->setFlags(Qt::ItemIsSelectable);

	      if(!tooltip.isEmpty())
		item->setToolTip(tooltip);

	      ui.table->setItem(i, 0, item);
	    }
	}

      if(m_searchQuery->isValid())
	if(pixmapItem)
	  {
	    pixmapItem->setData(0, myoid);
	    pixmapItem->setData(1, itemType);
	    pixmapItem->setData(2, i);
	  }

      if(progress)
	{
	  if(i + 1 <= progress->maximum())
	    progress->setValue(i + 1);

	  progress->repaint();
	  QApplication::processEvents();
	}

      if(m_searchQuery->at() != QSql::BeforeFirstRow)
	if(!m_searchQuery->next())
	  break;
    }

  ui.itemsCountLabel->setText
    (tr("%1 Result(s)").arg(ui.table->rowCount()));

  if(!m_db.driver()->hasFeature(QSqlDriver::QuerySize) &&
     limit != -1 &&
     progress)
    progress->setValue(limit);

  auto wasCanceled = false;

  if(progress)
    {
      progress->wasCanceled(); // QProgressDialog::close()!
      progress->close();
    }

  ui.table->setSortingEnabled(true);

  if(searchType == POPULATE_SEARCH_BASIC)
    if(ui.table->rowCount() == 0)
      {
	ui.case_insensitive->setEnabled(true);
	ui.search->setEnabled(true);
	ui.searchType->setEnabled(true);
      }

  addConfigOptions(typefilter);

  if(ui.actionAutomatically_Resize_Column_Widths->isChecked())
    slotResizeColumns();

  m_queryOffset = offset;
  ui.previousPageButton->setEnabled(m_queryOffset > 0);

  if(ui.table->rowCount() == 0)
    ui.itemsCountLabel->setText(tr("0 Results"));
  else
    ui.itemsCountLabel->setText(tr("%1 Result(s)").arg(ui.table->rowCount()));

  if(limit == -1)
    ui.nextPageButton->setEnabled(false);
  else if(ui.table->rowCount() < limit)
    {
      if(wasCanceled)
	/*
	** Allow viewing of the next potential page if the user
	** canceled the query.
	*/

	ui.nextPageButton->setEnabled(true);
      else
	ui.nextPageButton->setEnabled(false);
    }
  else
    ui.nextPageButton->setEnabled(true);

#ifdef Q_OS_MACOS
  ui.table->hide();
  ui.table->show();
#endif
  connect(ui.table,
	  SIGNAL(itemChanged(QTableWidgetItem *)),
	  this,
	  SLOT(slotItemChanged(QTableWidgetItem *)));
  m_findList.clear();

  if(statusBar())
    statusBar()->showMessage
      (tr("Query completed in %1 second(s).").
       arg(qAbs(static_cast<double> (elapsed.elapsed())) / 1000.0),
       5000);

  ui.graphicsView->setSceneRect(ui.graphicsView->scene()->itemsBoundingRect());
  emit queryCompleted(biblioteq_misc_functions::queryString(m_searchQuery));
  QApplication::restoreOverrideCursor();
  return 0;
}

void biblioteq::bookSearch(const QString &field, const QString &value)
{
  auto book = new biblioteq_book(this, "", QModelIndex());

  book->search(field, value);
  book->deleteLater();
}

void biblioteq::cdSearch(const QString &field, const QString &value)
{
  auto cd = new biblioteq_cd(this, "", QModelIndex());

  cd->search(field, value);
  cd->deleteLater();
}

void biblioteq::createConfigToolMenu(void)
{
  if(!m_configToolMenu)
    {
      m_configToolMenu = new QMenu(this);
#ifndef Q_OS_ANDROID
      m_configToolMenu->setTearOffEnabled(ui.actionTearOffMenus->isChecked());
      m_configToolMenu->setWindowIcon(QIcon(":/book.png"));
      m_configToolMenu->setWindowTitle(tr("BiblioteQ"));
#endif
    }
}

void biblioteq::deleteItem(const QString &oid, const QString &itemType)
{
  if(itemType == "book")
    {
      auto item = dynamic_cast<biblioteq_book *> (findItemInTab(oid));

      if(item)
	removeBook(item);
      else
	{
	  foreach(auto w, QApplication::topLevelWidgets())
	    {
	      auto book = qobject_cast<biblioteq_book *> (w);

	      if(book && book->getID() == oid)
		{
		  removeBook(book);
		  break;
		}
	    }
	}
    }
  else if(itemType == "cd")
    {
      auto item = dynamic_cast<biblioteq_cd *> (findItemInTab(oid));

      if(item)
	removeCD(item);
      else
	{
	  foreach(auto w, QApplication::topLevelWidgets())
	    {
	      auto cd = qobject_cast<biblioteq_cd *> (w);

	      if(cd && cd->getID() == oid)
		{
		  removeCD(cd);
		  break;
		}
	    }
	}
    }
  else if(itemType == "dvd")
    {
      auto item = dynamic_cast<biblioteq_dvd *> (findItemInTab(oid));

      if(item)
	removeDVD(item);
      else
	{
	  foreach(auto w, QApplication::topLevelWidgets())
	    {
	      auto dvd = qobject_cast<biblioteq_dvd *> (w);

	      if(dvd && dvd->getID() == oid)
		{
		  removeDVD(dvd);
		  break;
		}
	    }
	}
    }
  else if(itemType == "grey_literature")
    {
      auto item = dynamic_cast<biblioteq_grey_literature *>
	(findItemInTab(oid));

      if(item)
	removeGreyLiterature(item);
      else
	{
	  foreach(auto w, QApplication::topLevelWidgets())
	    {
	      auto gl = qobject_cast<biblioteq_grey_literature *> (w);

	      if(gl && gl->getID() == oid)
		{
		  removeGreyLiterature(gl);
		  break;
		}
	    }
	}
    }
  else if(itemType == "journal")
    {
      auto item = dynamic_cast<biblioteq_journal *> (findItemInTab(oid));

      if(item)
	removeJournal(item);
      else
	{
	  foreach(auto w, QApplication::topLevelWidgets())
	    {
	      auto journal = qobject_cast<biblioteq_journal *> (w);

	      if(journal && journal->getID() == oid)
		{
		  removeJournal(journal);
		  break;
		}
	    }
	}
    }
  else if(itemType == "magazine")
    {
      auto item = dynamic_cast<biblioteq_magazine *> (findItemInTab(oid));

      if(item)
	removeMagazine(item);
      else
	{
	  foreach(auto w, QApplication::topLevelWidgets())
	    {
	      auto magazine = qobject_cast<biblioteq_magazine *> (w);

	      /*
	      ** The class biblioteq_journal inherits biblioteq_magazine.
	      */

	      if(!qobject_cast<biblioteq_journal *> (w))
		if(magazine && magazine->getID() == oid)
		  {
		    removeMagazine(magazine);
		    break;
		  }
	    }
	}
    }
  else if(itemType == "photograph_collection")
    {
      auto item = dynamic_cast<biblioteq_photographcollection *>
	(findItemInTab(oid));

      if(item)
	removePhotographCollection(item);
      else
	{
	  foreach(auto w, QApplication::topLevelWidgets())
	    {
	      auto photograph = qobject_cast<biblioteq_photographcollection *>
		(w);

	      if(photograph && photograph->getID() == oid)
		{
		  removePhotographCollection(photograph);
		  break;
		}
	    }
	}
    }
  else if(itemType == "videogame")
    {
      auto item = dynamic_cast<biblioteq_videogame *>
	(findItemInTab(oid));

      if(item)
	removeVideoGame(item);
      else
	{
	  foreach(auto w, QApplication::topLevelWidgets())
	    {
	      auto videogame = qobject_cast<biblioteq_videogame *> (w);

	      if(videogame && videogame->getID() == oid)
		{
		  removeVideoGame(videogame);
		  break;
		}
	    }
	}
    }
}

void biblioteq::dvdSearch(const QString &field, const QString &value)
{
  auto dvd = new biblioteq_dvd(this, "", QModelIndex());

  dvd->search(field, value);
  dvd->deleteLater();
}

void biblioteq::exportAsCSV
(QTableWidget *table, QWidget *parent, const QString &title)
{
  if(!table)
    return;

  QFileDialog dialog(parent);

  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setDefaultSuffix("csv");
  dialog.setDirectory(QDir::homePath());
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setNameFilter(tr("CSV (*.csv)"));
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowTitle(title);
  dialog.exec();
  QApplication::processEvents();

  if(dialog.result() == QDialog::Accepted)
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);

      QFile file(dialog.selectedFiles().value(0));

      if(file.open(QIODevice::Text |
		   QIODevice::Truncate |
		   QIODevice::WriteOnly))
	{
	  QString str("");
	  QTextStream stream(&file);

	  for(int i = 0; i < table->columnCount(); i++)
	    if(!table->isColumnHidden(i))
	      {
		if(!table->horizontalHeaderItem(i))
		  continue;

		if(table->horizontalHeaderItem(i)->text() == tr("Read Status"))
		  /*
		  ** We cannot export the Read Status column because
		  ** it is not supported by PostgreSQL.
		  */

		  continue;

		if(table->horizontalHeaderItem(i)->text().contains(","))
		  str += QString("\"%1\",").arg
		    (table->horizontalHeaderItem(i)->text());
		else
		  str += QString("%1,").arg
		    (table->horizontalHeaderItem(i)->text());
	      }

	  if(str.endsWith(","))
	    str = str.mid(0, str.length() - 1);

	  if(!str.isEmpty())
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
	    stream << str << Qt::endl;
#else
	    stream << str << endl;
#endif

	  for(int i = 0; i < table->rowCount(); i++)
	    {
	      str = "";

	      for(int j = 0; j < table->columnCount(); j++)
		if(!table->isColumnHidden(j))
		  {
		    /*
		    ** A lot of awful things will occur if
		    ** !table->item(i, j) is true!
		    */

		    if(!table->horizontalHeaderItem(j) || !table->item(i, j))
		      {
			str += ",";
			continue;
		      }

		    if(table->horizontalHeaderItem(j)->text() ==
		       tr("Read Status"))
		      /*
		      ** We cannot export the Read Status column because
		      ** it is not supported by PostgreSQL.
		      */

		      continue;

		    auto cleaned(table->item(i, j)->text());

		    cleaned.replace('\n', "<br>");
		    cleaned.replace("\r\n", "<br>");
		    cleaned = cleaned.trimmed();

		    if(cleaned.contains(","))
		      str += QString("\"%1\",").arg(cleaned);
		    else
		      str += QString("%1,").arg(cleaned);
		  }

	      if(str.endsWith(","))
		str = str.mid(0, str.length() - 1);

	      if(!str.isEmpty())
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
		stream << str << Qt::endl;
#else
	        stream << str << endl;
#endif
	    }

	  file.close();
	}

      QApplication::restoreOverrideCursor();
    }
}

void biblioteq::greyLiteratureSearch(const QString &field, const QString &value)
{
  auto gl = new biblioteq_grey_literature(this, "", QModelIndex());

  gl->search(field, value);
  gl->deleteLater();
}

void biblioteq::journSearch(const QString &field, const QString &value)
{
  auto journal = new biblioteq_journal(this, "", QModelIndex());

  journal->search(field, value);
  journal->deleteLater();
}

void biblioteq::magSearch(const QString &field, const QString &value)
{
  auto magazine = new biblioteq_magazine(this, "", QModelIndex(), "magazine");

  magazine->search(field, value);
  magazine->deleteLater();
}

void biblioteq::pcSearch(const QString &field, const QString &value)
{
  auto photograph = new biblioteq_photographcollection(this, "", QModelIndex());

  photograph->search(field, value);
  photograph->deleteLater();
}

void biblioteq::prepareReservationHistoryMenu(void)
{
  if(!history.printButton->actions().isEmpty())
    return;

  auto action = new QAction(tr("Print..."), this);

  connect(action,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotPrintReservationHistory(void)));
  history.printButton->addAction(action);
  action = new QAction(tr("Print Preview..."), this);
  connect(action,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotPrintReservationHistoryPreview(void)));
  history.printButton->addAction(action);
}

void biblioteq::readConfigurationFile(void)
{
  QFont font;
  QSettings settings;

  al.caseinsensitive->setChecked
    (settings.value("generalSearchCaseSensitivity", false).toBool());
#ifndef Q_OS_ANDROID
  m_members_diag->restoreGeometry
    (settings.value("members_window_geometry").toByteArray());
#endif
  ui.actionAutoPopulateOnCreation->setChecked
    (settings.value("automatically_populate_on_create", false).toBool());
  ui.actionAutomatically_Resize_Column_Widths->setChecked
    (settings.value("automatically_resize_column_widths", false).toBool());
  ui.actionDownload_Version_Information->setChecked
    (settings.value("download_version_information", false).toBool());
  ui.actionPopulateOnStart->setChecked
    (settings.value("populate_table_on_connect", false).toBool());
  ui.actionResetErrorLogOnDisconnect->setChecked
    (settings.value("reset_error_log_on_disconnect", false).toBool());
  ui.actionShowGrid->setChecked
    (settings.value("show_table_grid", false).toBool());

  if(settings.contains("mainwindow_geometry"))
    {
      restoreGeometry(settings.value("mainwindow_geometry").toByteArray());
      ui.actionPreserveGeometry->setChecked(true);
    }
  else
    ui.actionPreserveGeometry->setChecked(false);

  font = QApplication::font();

  if(!settings.value("global_font", "").toString().trimmed().isEmpty())
    if(!font.fromString(settings.value("global_font", "").toString().trimmed()))
      font = QApplication::font();

  QApplication::setFont(font);
  ui.actionAutomaticallySaveSettingsOnExit->setChecked
    (settings.value("save_settings_on_exit", true).toBool());
  ui.actionPopulate_Administrator_Browser_Table_on_Display->setChecked
    (settings.value("automatically_populate_admin_list_on_display", true).
     toBool());
  ui.actionPopulate_Database_Enumerations_Browser_on_Display->setChecked
    (settings.value("automatically_populate_enum_list_on_display", true).
     toBool());
  ui.actionPopulate_Members_Browser_Table_on_Display->setChecked
    (settings.value("automatically_populate_members_list_on_display", true).
     toBool());
  ui.actionTabbed_Item_Windows->setChecked
    (settings.value("tabbed_item_windows", true).toBool());

  QHash<QString, QString> states;

  for(int i = 0; i < settings.allKeys().size(); i++)
    if(settings.allKeys().at(i).contains("_header_state"))
      states[settings.allKeys().at(i)] =
	settings.value(settings.allKeys().at(i)).toString();

  ui.table->parseStates(states);

  auto found = false;

  for(int i = 0; i < ui.menuPreferredSRUSite->actions().size(); i++)
    if(QString(settings.value("preferred_sru_site").toString()).
       remove("&").trimmed() ==
       QString(ui.menuPreferredSRUSite->actions().at(i)->text()).remove("&"))
      {
	found = true;
	ui.menuPreferredSRUSite->actions().at(i)->setChecked(true);
	break;
      }

  if(!found && !ui.menuPreferredSRUSite->actions().isEmpty())
    ui.menuPreferredSRUSite->actions().at(0)->setChecked(true);

  found = false;

  for(int i = 0; i < ui.menuPreferredZ3950Server->actions().size(); i++)
    if(QString(settings.value("preferred_z3950_site").toString()).
       remove("&").trimmed() ==
       QString(ui.menuPreferredZ3950Server->actions().at(i)->text()).
       remove("&"))
      {
	found = true;
	ui.menuPreferredZ3950Server->actions().at(i)->setChecked(true);
	break;
      }

  if(!found && !ui.menuPreferredZ3950Server->actions().isEmpty())
    ui.menuPreferredZ3950Server->actions().at(0)->setChecked(true);

  auto const index = br.branch_name->findText
    (settings.value("previous_branch_name", "").toString());

  if(index >= 0)
    br.branch_name->setCurrentIndex(index);
  else
    br.branch_name->setCurrentIndex(0);

  auto viewModeIndex = settings.value("view_mode_index", 1).toInt();

  if(viewModeIndex < 0 || viewModeIndex > 1)
    viewModeIndex = 1;

  auto ag = findChild<QActionGroup *> ("view_mode_menu");

  if(ag && ag->actions().size() > viewModeIndex)
    ag->actions().at(viewModeIndex)->setChecked(true);

  ui.stackedWidget->setCurrentIndex(viewModeIndex);

  if(ui.stackedWidget->currentIndex() == 0)
    ui.table->setSelectionMode(QAbstractItemView::MultiSelection);
  else
    ui.table->setSelectionMode(QAbstractItemView::ExtendedSelection);

  QColor color
    (settings.value("mainwindow_canvas_background_color").
     toString().remove('&').trimmed());

  if(!color.isValid())
    color = QColor(Qt::white);

  ui.graphicsView->scene()->setBackgroundBrush(color);
  createSqliteMenuActions();
  slotResizeColumns();
}

void biblioteq::readGlobalConfiguration(void)
{
#if defined(Q_OS_ANDROID)
  QSettings settings("assets:/biblioteq.conf", QSettings::IniFormat);
#elif defined(Q_OS_MACOS)
  QSettings settings
    (QCoreApplication::applicationDirPath() + "/../../../biblioteq.conf",
     QSettings::IniFormat);
#elif defined(Q_OS_OS2)
  QSettings settings
    (qgetenv("unixroot") + "/usr/local/biblioteq.conf", QSettings::IniFormat);
#elif defined(Q_OS_WINDOWS)
  QSettings settings(QCoreApplication::applicationDirPath() +
		     QDir::separator() +
		     "biblioteq.conf",
		     QSettings::IniFormat);
#else
  QSettings settings(BIBLIOTEQ_CONFIGURATION_FILE, QSettings::IniFormat);
#endif

  m_amazonImages.clear();
  m_branches.clear();
  m_openLibraryImages.clear();
  m_openLibraryItems.clear();
  m_sruMaps.clear();
  m_z3950Maps.clear();
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
#endif

  for(int i = 0; i < settings.childGroups().size(); i++)
    {
      settings.beginGroup(settings.childGroups().at(i));

      if(settings.group() == "Amazon Back Cover Images")
	{
	  m_amazonImages["back_cover_host"] = settings.value("host", "").
	    toString().trimmed();
	  m_amazonImages["back_cover_path"] = settings.value("path", "").
	    toString().trimmed().remove('"');
	  m_amazonImages["back_proxy_host"] = settings.value("proxy_host", "").
	    toString().trimmed();
	  m_amazonImages["back_proxy_password"] = settings.value
	    ("proxy_password", "").toString().trimmed();
	  m_amazonImages["back_proxy_port"] = settings.value("proxy_port", "").
	    toString().trimmed();
	  m_amazonImages["back_proxy_type"] = settings.value("proxy_type", "").
	    toString().trimmed();
	  m_amazonImages["back_proxy_username"] = settings.value
	    ("proxy_username", "").toString().trimmed();
	}
      else if(settings.group() == "Amazon Front Cover Images")
	{
	  m_amazonImages["front_cover_host"] = settings.value("host", "").
	    toString().trimmed();
	  m_amazonImages["front_cover_path"] = settings.value("path", "").
	    toString().trimmed().remove('"');
	  m_amazonImages["front_proxy_host"] = settings.value("proxy_host", "").
	    toString().trimmed();
	  m_amazonImages["front_proxy_password"] = settings.value
	    ("proxy_password", "").toString().trimmed();
	  m_amazonImages["front_proxy_port"] = settings.value("proxy_port", "").
	    toString().trimmed();
	  m_amazonImages["front_proxy_type"] = settings.value("proxy_type", "").
	    toString().trimmed();
	  m_amazonImages["front_proxy_username"] = settings.value
	    ("proxy_username", "").toString().trimmed();
	}
      else if(settings.group().startsWith("Branch"))
	{
	  if(!settings.value("database_name",
			     "").toString().trimmed().isEmpty())
	    {
	      QHash<QString, QString> hash;

	      hash["branch_name"] = settings.value("database_name", "").
		toString().trimmed();
	      hash["connection_options"] = settings.value
		("connection_options", "").toString().trimmed();
	      hash["database_type"] = settings.value("database_type", "").
		toString().trimmed();
	      hash["hostname"] = settings.value("hostname", "").
		toString().trimmed();
	      hash["port"] = settings.value("port", "").toString().trimmed();
	      hash["ssl_enabled"] = settings.value("ssl_enabled", "").
		toString().trimmed();
	      m_branches
		[settings.value("database_name", "").toString().trimmed()] =
		hash;
	    }
	}
      else if(settings.group().startsWith("Open Library"))
	{
	  if(settings.group() == "Open Library")
	    m_openLibraryItems["url_isbn"] =
	      settings.value("url_isbn").toString().trimmed();
	  else
	    {
	      m_openLibraryImages["back_url"] =
		settings.value("back_url", "").toString().trimmed();
	      m_openLibraryImages["front_url"] =
		settings.value("front_url", "").toString().trimmed();
	      m_openLibraryImages["proxy_host"] = settings.value
		("proxy_host", "").toString().trimmed();
	      m_openLibraryImages["proxy_password"] = settings.value
		("proxy_password", "").toString().trimmed();
	      m_openLibraryImages["proxy_port"] = settings.value
		("proxy_port", "").toString().trimmed();
	      m_openLibraryImages["proxy_type"] = settings.value
		("proxy_type", "").toString().trimmed();
	      m_openLibraryImages["proxy_username"] = settings.value
		("proxy_username", "").toString().trimmed();
	    }
	}
      else if(settings.group().startsWith("Other Cover Images"))
	{
	  m_otherImages["back_url"] =
	    settings.value("back_url", "").toString().trimmed();
	  m_otherImages["front_url"] =
	    settings.value("front_url", "").toString().trimmed();
	}
      else if(settings.group().startsWith("SRU"))
	{
	  if(!settings.value("name", "").toString().trimmed().isEmpty())
	    {
	      QHash<QString, QString> hash;

	      hash["Name"] = settings.value("name", "").toString().trimmed();
	      hash["proxy_host"] = settings.value("proxy_host", "").
		toString().trimmed();
	      hash["proxy_password"] = settings.value
		("proxy_password", "").toString().trimmed();
	      hash["proxy_port"] = settings.value("proxy_port", "").
		toString().trimmed();
	      hash["proxy_type"] = settings.value("proxy_type", "").
		toString().trimmed();
	      hash["proxy_username"] = settings.value
		("proxy_username", "").toString().trimmed();
	      hash["url_isbn"] = settings.value
		("url_isbn", "").toString().trimmed().remove('"');
	      hash["url_issn"] = settings.value
		("url_issn", "").toString().trimmed().remove('"');
	      m_sruMaps[settings.value("name", "").toString().trimmed()] = hash;
	    }
	}
      else if(settings.group().startsWith("Z39.50"))
	{
	  if(!settings.value("name", "").toString().trimmed().isEmpty())
	    {
	      QHash<QString, QString> hash;

	      hash["Name"] = settings.value("name", "Z39.50 Site").
		toString().trimmed();
	      hash["Address"] = settings.value("hostname", "").
		toString().trimmed();
	      hash["Database"] = settings.value("database_name", "").
		toString().trimmed();
	      hash["Format"] = settings.value("format", "marc8,utf-8").
		toString().trimmed().remove('"');
	      hash["Password"] = settings.value("password", "").
		toString().trimmed();
	      hash["Port"] = settings.value("port", "").toString().trimmed();
	      hash["RecordSyntax"] = settings.value
		("record_syntax", "MARC21").toString().trimmed();
	      hash["Userid"] = settings.value("username", "").
		toString().trimmed();
	      hash["proxy_host"] = settings.value("proxy_host", "").
		toString().trimmed();
	      hash["proxy_port"] = settings.value("proxy_port", "").
		toString().trimmed();
	      hash["timeout"] = settings.value("timeout", "30").
		toString().trimmed();

	      for(int j = 0; j < settings.allKeys().size(); j++)
		if(settings.allKeys().at(j).length() > 4 &&
		   settings.allKeys().at(j).startsWith("yaz_"))
		  hash[settings.allKeys().at(j)] = settings.value
		    (settings.allKeys().at(j)).toString().trimmed();

	      m_z3950Maps.insert
		(settings.value("name", "Z39.50 Site").toString().trimmed(),
		 hash);
	    }
	}

      settings.endGroup();
    }

  br.branch_name->clear();

  auto const list(m_branches.keys());

  for(int i = 0; i < list.size(); i++)
    br.branch_name->addItem(list.at(i));

  if(br.branch_name->count() == 0)
    {
      QHash<QString, QString> hash;

      hash["branch_name"] = "sqlite";
      hash["database_type"] = "sqlite";
      hash["hostname"] = "127.0.0.1";
      hash["port"] = "-1";
      hash["ssl_enabled"] = "false";

      if(!m_branches.contains(hash.value("branch_name")))
	m_branches[hash.value("branch_name")] = hash;

      br.branch_name->addItem(hash.value("branch_name"));
    }

  if(m_sruMaps.isEmpty())
    {
      QHash<QString, QString> hash;

      hash["Name"] = "Library of Congress";
      hash["url_isbn"] = "https://www.loc.gov/z39voy?operation=searchRetrieve&"
	"version=1.1&query=bath.isbn=%1 or bath.isbn=%2&"
	"recordSchema=marcxml&startRecord=1&maximumRecords=1";
      hash["url_issn"] = "https://www.loc.gov/z39voy?operation="
	"searchRetrieve&version=1.1&query=bath.issn=%1&"
	"recordSchema=marcxml&startRecord=1&maximumRecords=100";
      m_sruMaps["Library of Congress"] = hash;
    }

  if(m_z3950Maps.isEmpty())
    {
      QHash<QString, QString> hash;

      hash["Name"] = "Library of Congress";
      hash["Address"] = "lx2.loc.gov";
      hash["Port"] = "210";
      hash["Database"] = "LCDB";
      hash["Format"] = "marc8,utf-8";
      hash["RecordSyntax"] = "MARC21";
      hash["timeout"] = "30";
      m_z3950Maps.insert("Library of Congress", hash);
    }
}

void biblioteq::slotAdminContextMenu(const QPoint &point)
{
  QMenu menu(m_admin_diag);

  menu.addAction(tr("Add Administrator"),
		 this,
		 SLOT(slotAddAdmin(void)));
  menu.addAction(tr("Delete Selected Administrator"),
		 this,
		 SLOT(slotDeleteAdmin(void)));
  menu.addSeparator();
  menu.addAction(tr("Refresh Table"),
		 this,
		 SLOT(slotRefreshAdminList(void)));
  menu.exec(ab.table->mapToGlobal(point));
}

void biblioteq::slotAllGo(void)
{
  if(!m_db.isOpen())
    return;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  QList<QVariant> values;
  QString bookFrontCover("'' AS front_cover ");
  QString cdFrontCover("'' AS front_cover ");
  QString dvdFrontCover("'' AS front_cover ");
  QString greyLiteratureFrontCover("'' AS front_cover ");
  QString journalFrontCover("'' AS front_cover ");
  QString magazineFrontCover("'' AS front_cover ");
  QString photographCollectionFrontCover("'' AS image_scaled ");
  QString searchstr("");
  QString str("");
  QString type("");
  QString videoGameFrontCover("'' AS front_cover ");
  QStringList types;
  auto const caseinsensitive = al.caseinsensitive->isChecked();
  auto query = new QSqlQuery(m_db);

  if(m_otherOptions->showMainTableImages())
    {
      bookFrontCover = "book.front_cover ";
      cdFrontCover = "cd.front_cover ";
      dvdFrontCover = "dvd.front_cover ";
      greyLiteratureFrontCover = "grey_literature.front_cover ";
      journalFrontCover = "journal.front_cover ";
      magazineFrontCover = "magazine.front_cover ";
      photographCollectionFrontCover = "photograph_collection.image_scaled ";
      videoGameFrontCover = "videogame.front_cover ";
    }

  types.append("Book");
  types.append("CD");
  types.append("DVD");
  types.append("Grey Literature");
  types.append("Journal");
  types.append("Magazine");
  types.append("Photograph Collection");
  types.append("Video Game");

  for(int i = 0; i < types.size(); i++)
    {
      type = types.at(i);

      if(type == "Grey Literature")
	str = "SELECT DISTINCT grey_literature.document_title, "
	  "grey_literature.document_id, "
	  "'', "
	  "grey_literature.document_date, "
	  "'', "
	  "'', "
	  "0.00, "
	  "'', "
	  "grey_literature.quantity, "
	  "grey_literature.location, "
	  "1 - COUNT(item_borrower.item_oid) AS availability, "
	  "COUNT(item_borrower.item_oid) AS total_reserved, "
	  "grey_literature.job_number, "
	  "grey_literature.type, "
	  "grey_literature.myoid, " +
	  greyLiteratureFrontCover +
	  "FROM "
	  "grey_literature LEFT JOIN item_borrower ON "
	  "grey_literature.myoid = "
	  "item_borrower.item_oid "
	  "AND item_borrower.type = 'Grey Literature' "
	  "WHERE ";
      else if(type == "Photograph Collection")
	str = "SELECT DISTINCT photograph_collection.title, "
	  "photograph_collection.id, "
	  "'', "
	  "'', "
	  "'', "
	  "'', "
	  "0.00, "
	  "'', "
	  "1 AS quantity, "
	  "photograph_collection.location, "
	  "0 AS availability, "
	  "0 AS total_reserved, "
	  "photograph_collection.accession_number, "
	  "photograph_collection.type, "
	  "photograph_collection.myoid, " +
	  photographCollectionFrontCover +
	  "FROM photograph_collection "
	  "WHERE ";
      else
	{
	  str = QString
	    ("SELECT DISTINCT %1.title, "
	     "%1.id, "
	     "%1.publisher, %1.pdate, "
	     "%1.category, "
	     "%1.language, "
	     "%1.price, %1.monetary_units, "
	     "%1.quantity, "
	     "%1.location, "
	     "%1.quantity - "
	     "COUNT(item_borrower.item_oid) AS availability, "
	     "COUNT(item_borrower.item_oid) AS total_reserved, "
	     "%1.accession_number, "
	     "%1.type, "
	     "%1.myoid, ").arg(type.toLower().remove(" "));

	  if(type == "Book")
	    str.append(bookFrontCover);
	  else if(type == "CD")
	    str.append(cdFrontCover);
	  else if(type == "DVD")
	    str.append(dvdFrontCover);
	  else if(type == "Journal")
	    str.append(journalFrontCover);
	  else if(type == "Magazine")
	    str.append(magazineFrontCover);
	  else
	    str.append(videoGameFrontCover);

	  str += QString("FROM "
			 "%1 LEFT JOIN item_borrower ON "
			 "%1.myoid = "
			 "item_borrower.item_oid "
			 "AND item_borrower.type = '%2' "
			 "WHERE ").arg(type.toLower().remove(" ")).arg(type);
	}

      QString ESCAPE("");
      auto const UNACCENT(unaccent());

      if(m_db.driverName() != "QSQLITE")
	ESCAPE = "E";

      QString idField("id");

      if(type == "Grey Literature")
	idField = "document_id";

      if(caseinsensitive)
	{
	  str.append
	    (QString("(%1 IS NULL OR " + UNACCENT +
		     "(LOWER(%1)) LIKE " + UNACCENT + "(" +
		     ESCAPE + "'%' || ? || '%') ").arg(idField));
	  values.append
	    (biblioteq_myqstring::escape(al.idnumber->text().trimmed(), true));
	}
      else
	{
	  str.append
	    (QString("(%1 IS NULL OR " + UNACCENT + "(%1) LIKE " +
		     UNACCENT + "(" +
		     ESCAPE + "'%' || ? || '%') ").arg(idField));
	  values.append
	    (biblioteq_myqstring::escape(al.idnumber->text().trimmed()));
	}

      if(type == "Book")
	{
	  if(caseinsensitive)
	    {
	      str.append
		("OR LOWER(isbn13) LIKE " + ESCAPE + "'%' || ? || '%')");
	      values.append
		(biblioteq_myqstring::
		 escape(al.idnumber->text().trimmed(), true));
	    }
	  else
	    {
	      str.append("OR isbn13 LIKE " + ESCAPE + "'%' || ? || '%')");
	      values.append
		(biblioteq_myqstring::escape(al.idnumber->text().trimmed()));
	    }
	}
      else
	str.append(")");

      str.append(" AND ");

      QString titleField("title");

      if(type == "Grey Literature")
	titleField = "document_title";

      if(caseinsensitive)
	{
	  str.append
	    (QString(UNACCENT + "(LOWER(%1)) LIKE " + UNACCENT + "(" +
		     ESCAPE + "'%' || ? || '%')").arg(titleField));
	  values.append
	    (biblioteq_myqstring::escape(al.title->text().trimmed(), true));
	}
      else
	{
	  str.append(QString(UNACCENT + "(%1) LIKE " + UNACCENT + "(" +
			     ESCAPE + "'%' || ? || '%')").arg(titleField));
	  values.append
	    (biblioteq_myqstring::escape(al.title->text().trimmed()));
	}

      if(type != "Grey Literature" &&
	 type != "Photograph Collection")
	{
	  str.append(" AND ");

	  if(al.publication_date_enabled->isChecked())
	    str.append("SUBSTR(pdate, 7) = '" +
		       al.publication_date->date().
		       toString("yyyy") + "' AND ");

	  if(al.categories_checkbox->isChecked())
	    {
	      auto words
		(al.category->toPlainText().trimmed().
		 split(QRegularExpression("\\s+"),
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
		       Qt::SkipEmptyParts
#else
		       QString::SkipEmptyParts
#endif
		       ));

	      if(!words.isEmpty())
		{
		  str.append("(");

		  while(!words.isEmpty())
		    {
		      auto const word(words.takeFirst());

		      if(caseinsensitive)
			{
			  str.append
			    (UNACCENT +
			     "(LOWER(category)) LIKE " +
			     UNACCENT + "(" + ESCAPE + "'%' || ? || '%')" +
			     (words.isEmpty() ? "" : " OR "));
			  values.append
			    (biblioteq_myqstring::escape(word.trimmed(), true));
			}
		      else
			{
			  str.append
			    (UNACCENT +
			     "(category) LIKE " + UNACCENT + "(" +
			     ESCAPE + "'%' || ? || '%')" +
			     (words.isEmpty() ? "" : " OR "));
			  values.append
			    (biblioteq_myqstring::escape(word.trimmed()));
			}
		    }

		  str.append(") AND ");
		}
	    }
	  else
	    {
	      if(caseinsensitive)
		{
		  str.append(UNACCENT + "(LOWER(category)) LIKE " +
			     UNACCENT + "(" + ESCAPE + "'%' || ? || '%') AND ");
		  values.append
		    (biblioteq_myqstring::
		     escape(al.category->toPlainText().trimmed(), true));
		}
	      else
		{
		  str.append(UNACCENT + "(category) LIKE " +
			     UNACCENT + "(" + ESCAPE + "'%' || ? || '%') AND ");
		  values.append
		    (biblioteq_myqstring::
		     escape(al.category->toPlainText().trimmed()));
		}
	    }

	  if(caseinsensitive)
	    {
	      str.append
		(UNACCENT + "(LOWER(publisher)) LIKE " + UNACCENT +
		 "(" + ESCAPE + "'%' || ? || '%') AND ");
	      values.append
		(biblioteq_myqstring::
		 escape(al.publisher->text().trimmed(), true));
	    }
	  else
	    {
	      str.append
		(UNACCENT + "(publisher) LIKE " + UNACCENT + "(" +
		 ESCAPE + "'%' || ? || '%') AND ");
	      values.append
		(biblioteq_myqstring::escape(al.publisher->text().trimmed()));
	    }

	  if(al.price->value() > -0.01)
	    {
	      str.append("price = ");
	      str.append(QString::number(al.price->value()));
	      str.append(" AND ");
	    }

	  if(al.language->currentIndex() != 0)
	    {
	      if(caseinsensitive)
		{
		  str.append
		    (UNACCENT + "(LOWER(language)) = " + UNACCENT + "(?) AND ");
		  values.append
		    (biblioteq_myqstring::
		     escape(al.language->currentText().trimmed(), true));
		}
	      else
		{
		  str.append
		    (UNACCENT + "(language) = " + UNACCENT + "(?) AND ");
		  values.append
		    (biblioteq_myqstring::
		     escape(al.language->currentText().trimmed()));
		}
	    }

	  if(al.monetary_units->currentIndex() != 0)
	    {
	      if(caseinsensitive)
		{
		  str.append
		    (UNACCENT + "(LOWER(monetary_units)) = " +
		     UNACCENT + "(?) AND ");
		  values.append
		    (biblioteq_myqstring::
		     escape(al.monetary_units->currentText().trimmed(), true));
		}
	      else
		{
		  str.append
		    (UNACCENT + "(monetary_units) = " + UNACCENT + "(?) AND ");
		  values.append
		    (biblioteq_myqstring::
		     escape(al.monetary_units->currentText().trimmed()));
		}
	    }

	  if(al.abstract_checkbox->isChecked())
	    {
	      auto words
		(al.description->toPlainText().trimmed().
		 split(QRegularExpression("\\s+"),
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
		       Qt::SkipEmptyParts
#else
		       QString::SkipEmptyParts
#endif
		       ));

	      if(!words.isEmpty())
		{
		  str.append("(");

		  while(!words.isEmpty())
		    {
		      auto const word(words.takeFirst());

		      if(caseinsensitive)
			{
			  str.append
			    (UNACCENT + "(LOWER(description)) LIKE " +
			     UNACCENT + "(" + ESCAPE
			     + "'%' || ? || '%')" +
			     (words.isEmpty() ? "" : " OR "));
			  values.append
			    (biblioteq_myqstring::escape(word.trimmed(), true));
			}
		      else
			{
			  str.append
			    (UNACCENT + "(description) LIKE " +
			     UNACCENT + "(" + ESCAPE + "'%' || ? || '%')" +
			     (words.isEmpty() ? "" : " OR "));
			  values.append
			    (biblioteq_myqstring::escape(word.trimmed()));
			}
		    }

		  str.append(") ");
		}
	      else
		{
		  if(caseinsensitive)
		    {
		      str.append
			(UNACCENT + "(LOWER(description)) LIKE " +
			 UNACCENT + "(" + ESCAPE + "'%' || ? || '%') ");
		      values.append
			(biblioteq_myqstring::
			 escape(al.description->toPlainText().trimmed(), true));
		    }
		  else
		    {
		      str.append
			(UNACCENT + "(description) LIKE " +
			 UNACCENT + "(" + ESCAPE + "'%' || ? || '%') ");
		      values.append
			(biblioteq_myqstring::
			 escape(al.description->toPlainText().trimmed()));
		    }
		}
	    }
	  else
	    {
	      if(caseinsensitive)
		{
		  str.append
		    (UNACCENT + "(LOWER(description)) LIKE " +
		     UNACCENT + "(" + ESCAPE + "'%' || ? || '%') ");
		  values.append
		    (biblioteq_myqstring::
		     escape(al.description->toPlainText().trimmed(), true));
		}
	      else
		{
		  str.append
		    (UNACCENT + "(description) LIKE " + UNACCENT +
		     "(" + ESCAPE + "'%' || ? || '%') ");
		  values.append
		    (biblioteq_myqstring::
		     escape(al.description->toPlainText().trimmed()));
		}
	    }

	  if(al.keywords_checkbox->isChecked())
	    {
	      auto words
		(al.keyword->toPlainText().trimmed().
		 split(QRegularExpression("\\s+"),
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
		       Qt::SkipEmptyParts
#else
		       QString::SkipEmptyParts
#endif
		       ));

	      if(!words.isEmpty())
		{
		  str.append(" AND (");

		  while(!words.isEmpty())
		    {
		      auto const word(words.takeFirst());

		      if(caseinsensitive)
			{
			  str.append
			    (UNACCENT +
			     "(COALESCE(LOWER(keyword), '')) LIKE " +
			     UNACCENT + "(" + ESCAPE + "'%' || ? || '%')" +
			     (words.isEmpty() ? "" : " OR "));
			  values.append
			    (biblioteq_myqstring::escape(word.trimmed(), true));
			}
		      else
			{
			  str.append
			    (UNACCENT +
			     "(COALESCE(keyword, '')) LIKE " +
			     UNACCENT + "(" + ESCAPE + "'%' || ? || '%')" +
			     (words.isEmpty() ? "" : " OR "));
			  values.append
			    (biblioteq_myqstring::escape(word.trimmed()));
			}
		    }

		  str.append(") ");
		}
	    }
	  else
	    {
	      if(caseinsensitive)
		{
		  str.append("AND " + UNACCENT +
			     "(COALESCE(LOWER(keyword), '')) LIKE " +
			     UNACCENT + "(" + ESCAPE + "'%' || ? || '%') ");
		  values.append
		    (biblioteq_myqstring::escape(al.keyword->toPlainText().
						 trimmed(), true));
		}
	      else
		{
		  str.append("AND " + UNACCENT +
			     "(COALESCE(keyword, '')) LIKE " +
			     UNACCENT + "(" + ESCAPE + "'%' || ? || '%') ");
		  values.append
		    (biblioteq_myqstring::escape(al.keyword->toPlainText().
						 trimmed()));
		}
	    }

	  if(al.quantity->value() != 0)
	    str.append("AND quantity = " + al.quantity->text() + " ");

	  if(al.location->currentIndex() != 0)
	    {
	      if(caseinsensitive)
		str.append
		  ("AND " + UNACCENT + "(LOWER(location)) = " +
		   UNACCENT + "(" + ESCAPE + "'" +
		   biblioteq_myqstring::escape
		   (al.location->currentText().trimmed(),
		    true) + "') ");
	      else
		str.append
		  ("AND " + UNACCENT + "(location) = " +
		   UNACCENT + "(" + ESCAPE + "'" +
		   biblioteq_myqstring::escape
		   (al.location->currentText().trimmed()) + "') ");
	    }

	  str += " ";
	  str += QString("GROUP BY "
			 "%1.title, "
			 "%1.id, "
			 "%1.publisher, %1.pdate, "
			 "%1.category, "
			 "%1.language, "
			 "%1.price, "
			 "%1.monetary_units, "
			 "%1.quantity, "
			 "%1.location, "
			 "%1.keyword, "
			 "%1.accession_number, "
			 "%1.type, "
			 "%1.myoid, "
			 "%1.front_cover ").arg(type.toLower().remove(" "));
	}
      else if(type == "Grey Literature")
	{
	  if(al.location->currentIndex() != 0)
	    {
	      if(caseinsensitive)
		str.append
		  ("AND " + UNACCENT + "(LOWER(location)) = " +
		   UNACCENT + "(" + ESCAPE + "'" +
		   biblioteq_myqstring::escape
		   (al.location->currentText().trimmed(),
		    true) + "') ");
	      else
		str.append
		  ("AND " + UNACCENT + "(location) = " +
		   UNACCENT + "(" + ESCAPE + "'" +
		   biblioteq_myqstring::escape
		   (al.location->currentText().trimmed()) + "') ");
	    }

	  str += " ";
	  str += "GROUP BY grey_literature.document_title, "
	    "grey_literature.document_id, "
	    "grey_literature.document_date, "
	    "grey_literature.location, "
	    "grey_literature.job_number, "
	    "grey_literature.type, "
	    "grey_literature.myoid, "
	    "grey_literature.front_cover ";
	}
      else if(type == "Photograph Collection")
	{
	  if(al.location->currentIndex() != 0)
	    {
	      if(caseinsensitive)
		str.append
		  ("AND " + UNACCENT + "(LOWER(location)) = " +
		   UNACCENT + "(" + ESCAPE + "'" +
		   biblioteq_myqstring::escape
		   (al.location->currentText().trimmed(),
		    true) + "') ");
	      else
		str.append
		  ("AND " + UNACCENT + "(location) = " +
		   UNACCENT + "(" + ESCAPE + "'" +
		   biblioteq_myqstring::escape
		   (al.location->currentText().trimmed()) + "') ");
	    }

	  str += " ";
	  str += "GROUP BY "
	    "photograph_collection.title, "
	    "photograph_collection.id, "
	    "photograph_collection.location, "
	    "photograph_collection.accession_number, "
	    "photograph_collection.type, "
	    "photograph_collection.myoid, "
	    "photograph_collection.image_scaled ";
	}

      if(type == "CD")
	{
	  str = str.replace("pdate", "rdate");
	  str = str.replace("publisher", "recording_label");
	}
      else if(type == "DVD")
	{
	  str = str.replace("pdate", "rdate");
	  str = str.replace("publisher", "studio");
	}
      else if(type == "Video Game")
	{
	  str = str.replace("pdate", "rdate");
	  str = str.replace("category", "genre");
	}

      if(type != "Photograph Collection")
	{
	  if(al.available->isChecked())
	    {
	      if(type == "Grey Literature")
		str.append
		  ("HAVING (quantity - COUNT(item_borrower.item_oid)) > 0 ");
	      else
		str.append
		  (QString("HAVING (%1.quantity - "
			   "COUNT(item_borrower.item_oid)) > 0 ").
		   arg(type.toLower().remove(" ")));
	    }
	}

      if(type != "Video Game")
	str += "UNION ALL ";
      else
	str += " ";

      searchstr += str;
    }

  if(m_db.driverName() == "QSQLITE")
    query->exec("PRAGMA case_sensitive_like = TRUE");

  query->prepare(searchstr);

  for(int i = 0; i < values.size(); i++)
    query->addBindValue(values.at(i));

  QApplication::restoreOverrideCursor();
  (void) populateTable(query, "All", NEW_PAGE, POPULATE_SEARCH);
}

void biblioteq::slotCheckout(void)
{
  QString errorstr = "";
  QString itemid = "";
  QString oid = "";
  QString type = "";
  auto const row1 = bb.table->currentRow();
  auto const row2 = ui.table->currentIndex();
  biblioteq_copy_editor *copyeditor = nullptr;
  biblioteq_item *item = nullptr;
  int quantity = 0;

  type = biblioteq_misc_functions::getColumnString
    (ui.table, row2.row(), ui.table->columnNumber("Type"));

  if(type == "Photograph Collection")
    {
      QMessageBox::critical(m_members_diag,
			    tr("BiblioteQ: User Error"),
			    tr("Photographs may not be reserved."));
      QApplication::processEvents();
      return;
    }

  if(row1 > -1)
    {
      /*
      ** Has the member's membership expired?
      */

      auto const memberid = biblioteq_misc_functions::getColumnString
	(bb.table, row1, m_bbColumnHeaderIndexes.indexOf("Member ID"));
      auto expired = true;

      QApplication::setOverrideCursor(Qt::WaitCursor);
      expired = biblioteq_misc_functions::hasMemberExpired
	(m_db, memberid, errorstr);
      QApplication::restoreOverrideCursor();

      if(!errorstr.isEmpty())
	addError(tr("Database Error"),
		 tr("Unable to determine if the membership of "
		    "the selected member has expired."),
		 errorstr,
		 __FILE__,
		 __LINE__);

      if(!errorstr.isEmpty() || expired)
	{
	  QMessageBox::critical
	    (m_members_diag,
	     tr("BiblioteQ: User Error"),
	     tr("It appears that the selected member's "
		"membership has expired."));
	  QApplication::processEvents();
	  return;
	}

      QApplication::setOverrideCursor(Qt::WaitCursor);

      auto const maximumReserved = biblioteq_misc_functions::maximumReserved
	(m_db, memberid, "book"); // Only books offer maximums.

      if(maximumReserved > 0)
	{
	  auto const totalReserved = biblioteq_misc_functions::
	    getItemsReservedCounts(m_db, memberid, errorstr).value("numbooks");

	  if(maximumReserved <= totalReserved)
	    {
	      QApplication::restoreOverrideCursor();
	      QMessageBox::critical
		(m_members_diag,
		 tr("BiblioteQ: User Error"),
		 tr("Too many (%1) items reserved (%2).").
		 arg(maximumReserved).arg(totalReserved));
	      QApplication::processEvents();
	      return;
	    }
	}

      QApplication::restoreOverrideCursor();
    }

  if(row2.isValid())
    {
      /*
      ** Is the item available?
      */

      oid = biblioteq_misc_functions::getColumnString
	(ui.table, row2.row(), ui.table->columnNumber("MYOID"));
      QApplication::setOverrideCursor(Qt::WaitCursor);

      auto const availability = biblioteq_misc_functions::getAvailability
	(oid, m_db, type, errorstr).toInt();

      QApplication::restoreOverrideCursor();

      if(!errorstr.isEmpty())
	addError(tr("Database Error"),
		 tr("Unable to determine the availability of "
		    "the selected item."),
		 errorstr,
		 __FILE__,
		 __LINE__);

      if(!errorstr.isEmpty() || availability < 1)
	{
	  QMessageBox::critical
	    (m_members_diag,
	     tr("BiblioteQ: User Error"),
	     tr("It appears that the item that you selected "
		"is not available for reservation."));
	  QApplication::processEvents();
	  return;
	}
    }

  if(!row2.isValid() || row1 < 0)
    {
      QMessageBox::critical(m_members_diag,
			    tr("BiblioteQ: User Error"),
			    tr("Please select a member and an item "
			       "to continue with the reservation process."));
      QApplication::processEvents();
      return;
    }
  else
    {
      item = new biblioteq_item(row2);
      quantity = biblioteq_misc_functions::getColumnString
	(ui.table, row2.row(), ui.table->columnNumber("Quantity")).toInt();

      if(type.toLower() == "book")
	{
	  itemid = biblioteq_misc_functions::getColumnString
	    (ui.table, row2.row(), ui.table->columnNumber("ISBN-10"));

	  if(itemid.isEmpty())
	    itemid = biblioteq_misc_functions::getColumnString
	      (ui.table, row2.row(), ui.table->columnNumber("ISBN-13"));
	}
      else if(type.toLower() == "cd")
	itemid = biblioteq_misc_functions::getColumnString
	  (ui.table, row2.row(), ui.table->columnNumber("Catalog Number"));
      else if(type.toLower() == "dvd")
	itemid = biblioteq_misc_functions::getColumnString
	  (ui.table, row2.row(), ui.table->columnNumber("UPC"));
      else if(type.toLower() == "grey literature")
	{
	  itemid = biblioteq_misc_functions::getColumnString
	    (ui.table, row2.row(), ui.table->columnNumber("Document ID"));
	  quantity = 1;
	}
      else if(type.toLower() == "journal" || type.toLower() == "magazine")
	itemid = biblioteq_misc_functions::getColumnString
	  (ui.table, row2.row(), ui.table->columnNumber("ISSN"));
      else if(type.toLower() == "video game")
	itemid = biblioteq_misc_functions::getColumnString
	  (ui.table, row2.row(), ui.table->columnNumber("UPC"));
      else
	{
	  QMessageBox::critical
	    (m_members_diag,
	     tr("BiblioteQ: User Error"),
	     tr("Unable to determine the selected item's type."));
	  QApplication::processEvents();
	  delete item;
	  return;
	}

      if(itemid.isEmpty())
	itemid = biblioteq_misc_functions::getColumnString
	  (ui.table, row2.row(), ui.table->columnNumber("ID Number"));

      /*
      ** Custom search?
      */

      if(itemid.isEmpty())
	itemid = biblioteq_misc_functions::getColumnString
	  (ui.table, row2.row(), "id");

      copyeditor = new biblioteq_copy_editor(m_members_diag,
					     this,
					     item,
					     true,
					     quantity,
					     oid,
					     nullptr,
					     font(),
					     type,
					     itemid,
					     bb.speedy->isChecked());
      copyeditor->populateCopiesEditor();
      copyeditor->exec();
      QApplication::processEvents();
      delete item;
    }
}

void biblioteq::slotConnectDB(void)
{
  std::unique_lock<std::mutex> lock{m_mutex, std::defer_lock};

  if(!lock.try_lock())
    return;
  else
    lock.unlock();

  QWidget *parent = this;
  auto const tmphash(m_branches[br.branch_name->currentText()]);

  if(m_branch_diag->isVisible())
    parent = m_branch_diag;

  if(tmphash.value("database_type") == "sqlite")
    {
      QFileInfo const fileInfo(br.filename->text());

      if(!fileInfo.exists() || !fileInfo.isReadable() || !fileInfo.isWritable())
	{
	  QMessageBox::critical
	    (parent,
	     tr("BiblioteQ: User Error"),
	     tr("The selected SQLite file is not accessible. Please "
		"verify that the file exists, is readable, and is writable."));
	  QApplication::processEvents();
	  return;
	}
    }

  QString drivers = "";
  QString errorstr = "";
  QString plugins = "";
  QString str = "";
  auto error = false;

  /*
  ** Configure some database attributes.
  */

  br.userid->setFocus();

  if(tmphash.value("database_type") == "postgresql")
    str = "QPSQL";
  else if(tmphash.value("database_type") == "sqlite")
    str = "QSQLITE";

  foreach(auto const &driver, QSqlDatabase::drivers())
    drivers += driver + ", ";

  if(drivers.endsWith(", "))
    drivers = drivers.mid(0, drivers.length() - 2);

  if(drivers.isEmpty())
    drivers = "N/A";

  foreach(auto const &path, QApplication::libraryPaths())
    if(path.contains("plugin", Qt::CaseInsensitive))
      {
	plugins = path;
	break;
      }

  if(plugins.isEmpty())
    plugins = "N/A";

  if(!QSqlDatabase::isDriverAvailable(str))
    {
      QFileInfo const fileInfo("qt.conf");
      QString str("");

      if(fileInfo.isReadable() && fileInfo.size() > 0)
	str = tr("\nThe file qt.conf is present in BiblioteQ's "
		 "current working directory. Perhaps a conflict "
		 "exists.");

      QMessageBox::critical
	(parent,
	 tr("BiblioteQ: Database Error"),
	 tr("The selected branch's database type does not "
	    "have a driver associated with it.") +
	 "\n" +
	 tr("The following drivers are available: ") +
	 drivers +
	 tr(".") +
	 "\n" +
	 tr("In addition, Qt expects plugins to exist "
	    "in: ") +
	 plugins +
	 tr(".") +
	 str +
	 "\n" +
	 tr("Please contact your administrator."));
      QApplication::processEvents();
      return;
    }

  slotDisconnect();
  m_db = QSqlDatabase::addDatabase(str, "Default");

  if(tmphash.value("database_type") == "sqlite")
    m_db.setDatabaseName(br.filename->text());
  else
    {
      m_db.setDatabaseName(br.branch_name->currentText());
      m_db.setHostName(tmphash.value("hostname"));
      m_db.setPort(static_cast<int> (tmphash.value("port").toUShort()));
    }

  if(tmphash.value("database_type") != "sqlite")
    {
      auto str(tmphash.value("connection_options"));

      if(tmphash.value("ssl_enabled") == "true")
	str.append(";requiressl=1");

      m_db.setConnectOptions(str);
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(tmphash.value("database_type") == "sqlite")
    (void) m_db.open();
  else
    {
      (void) m_db.open(br.userid->text().trimmed(), br.password->text());

      if(br.role->currentIndex() != 1)
	{
	  br.password->setText(QString(1024, '0'));
	  br.password->clear();
	}
    }

  QApplication::restoreOverrideCursor();

  if(!m_db.isOpen())
    {
      error = true;
      addError(tr("Database Error"),
	       tr("Unable to open a database connection "
		  "with the provided information."),
	       m_db.lastError().text(),
	       __FILE__,
	       __LINE__);
      QMessageBox::critical
	(parent,
	 tr("BiblioteQ: Database Error"),
	 tr("Unable to open a database "
	    "connection with the provided information. Please "
	    "review the Error Log."));
      QApplication::processEvents();
    }
  else
    {
      if(!m_db.driver()->hasFeature(QSqlDriver::Transactions))
	{
	  error = true;
	  addError
	    (tr("Database Error"),
	     tr("The current database driver that you're using "
		"does not support transactions. "
		"Please upgrade your database and/or driver."),
	     m_db.lastError().text(),
	     __FILE__,
	     __LINE__);
	  QMessageBox::critical
	    (parent,
	     tr("BiblioteQ: Database Error"),
	     tr("The current database driver that you're using "
		"does not support transactions. "
		"Please upgrade your database and/or driver."));
	  QApplication::processEvents();
	}
    }

  if(tmphash.value("database_type") != "sqlite")
    {
      if(!error)
	{
	  QApplication::setOverrideCursor(Qt::WaitCursor);
	  m_roles = biblioteq_misc_functions::getRoles
	    (m_db, br.userid->text().trimmed(), errorstr).toLower().trimmed();
	  m_unaccent = biblioteq_misc_functions::hasUnaccentExtension(m_db) ?
	    "unaccent" : "";
	  QApplication::restoreOverrideCursor();

	  if(errorstr.isEmpty())
	    {
	      if(br.role->currentIndex() == 0 && m_roles.isEmpty())
		{
		  error = true;
		  QMessageBox::critical
		    (parent,
		     tr("BiblioteQ: User Error"),
		     tr("It appears that the account ") +
		     br.userid->text().trimmed() +
		     tr(" does not have administrator privileges."));
		  QApplication::processEvents();
		}
	      else if(!m_roles.isEmpty() && br.role->currentIndex() != 0)
		{
		  error = true;
		  QMessageBox::critical
		    (parent,
		     tr("BiblioteQ: User Error"),
		     tr("It appears that you are attempting to assume an "
			"administrator role in a non-administrator mode."));
		  QApplication::processEvents();
		}
	      else
		{
		  if(br.role->currentIndex() == 0) // Administrator
		    biblioteq_misc_functions::setRole(m_db, errorstr, m_roles);
		  else if(br.role->currentIndex() == 1) // Guest
		    biblioteq_misc_functions::setRole(m_db, errorstr, "guest");
		  else
		    biblioteq_misc_functions::setRole
		      (m_db, errorstr, "patron");

		  if(!errorstr.isEmpty())
		    {
		      error = true;
		      addError(tr("Database Error"),
			       tr("Unable to set the role for ") +
			       br.userid->text().trimmed() +
			       tr("."),
			       errorstr,
			       __FILE__,
			       __LINE__);
		      QMessageBox::critical
			(parent,
			 tr("BiblioteQ: Database Error"),
			 tr("Unable to set the role for ") +
			 br.userid->text().trimmed() +
			 tr("."));
		      QApplication::processEvents();
		    }
		}
	    }
	  else if(br.role->currentIndex() == 0) // Administrator
	    {
	      error = true;
	      addError(tr("Database Error"),
		       tr("Unable to determine the roles of ") +
		       br.userid->text().trimmed() +
		       tr("."),
		       errorstr,
		       __FILE__,
		       __LINE__);
	      QMessageBox::critical
		(parent,
		 tr("BiblioteQ: Database Error"),
		 tr("Unable to determine the roles of ") +
		 br.userid->text().trimmed() +
		 tr("."));
	      QApplication::processEvents();
	    }
	  else if(br.role->currentIndex() == 1) // Guest
	    {
	      QSqlQuery query(m_db);

	      if(!query.exec("SET ROLE biblioteq_guest"))
		{
		  error = true;
		  addError(tr("Database Error"),
			   tr("Unable to set a guest role."),
			   errorstr,
			   __FILE__,
			   __LINE__);
		  QMessageBox::critical
		    (parent,
		     tr("BiblioteQ: Database Error"),
		     tr("Unable to set a guest role."));
		  QApplication::processEvents();
		}
	    }
	  else
	    {
	      QSqlQuery query(m_db);

	      if(!query.exec("SET ROLE biblioteq_patron"))
		{
		  error = true;
		  addError(tr("Database Error"),
			   tr("Unable to set the role for ") +
			   br.userid->text().trimmed() +
			   tr("."),
			   errorstr,
			   __FILE__,
			   __LINE__);
		  QMessageBox::critical
		    (parent,
		     tr("BiblioteQ: Database Error"),
		     tr("Unable to set the role for ") +
		     br.userid->text().trimmed() +
		     tr("."));
		  QApplication::processEvents();
		}
	    }
	}
    }
  else
    {
      if(!error)
	m_roles = "administrator";
    }

  if(error)
    {
      m_db = QSqlDatabase();
      QSqlDatabase::removeDatabase("Default");
      return;
    }
  else
    {
#ifdef Q_OS_ANDROID
      m_branch_diag->hide();
#else
      m_branch_diag->close();
#endif
    }

  /*
  ** We've connected successfully. Let's initialize other containers and
  ** widgets.
  */

#ifdef BIBLIOTEQ_SQLITE3_INCLUDE_FILE_EXISTS
  if(m_db.driver()->handle().isValid() && m_db.driverName() == "QSQLITE")
    {
      auto handle = *static_cast<sqlite3 **> (m_db.driver()->handle().data());
      auto ok = true;

      if(!handle)
	{
	  addError
	    (tr("SQLite Run-Time Loadable Extension"),
	     tr("The SQLite database handle is not valid."),
	     tr("The SQLite database handle is not valid."),
	     __FILE__,
	     __LINE__);
	  ok = false;
	}

      if(ok && sqlite3_create_function_v2(handle,
					  "REGEXP",
					  2,
					  SQLITE_UTF8,
					  nullptr,
					  regexp,
					  nullptr,
					  nullptr,
					  nullptr) != SQLITE_OK)
	addError
	  (tr("SQLite Create Function"),
	   tr("The function sqlite3_create_function_v2(REGEXP) failed."),
	   tr("The function sqlite3_create_function_v2(REGEXP) failed."),
	   __FILE__,
	   __LINE__);

      if(ok && sqlite3_enable_load_extension(handle, 1) != SQLITE_OK)
	{
	  addError
	    (tr("SQLite Run-Time Loadable Extension"),
	     tr("The function sqlite3_enable_load_extension() failed."),
	     tr("The function sqlite3_enable_load_extension() failed."),
	     __FILE__,
	     __LINE__);
	  ok = false;
	}

      if(ok)
	{
	  auto list
	    (QSettings().
	     value("otheroptions/sqlite_runtime_loadable_extensions").
	     toString().trimmed().split('\n'));

	  list.removeDuplicates();

	  foreach(auto const &i, list)
	    if(!i.trimmed().isEmpty())
	      {
		QFileInfo const fileInfo(i);

		if(fileInfo.isReadable())
		  {
		    char *error = nullptr;

		    if(sqlite3_load_extension(handle,
					      i.toUtf8().constData(),
					      nullptr,
					      &error) == SQLITE_OK)
		      addError(tr("SQLite Run-Time Loadable Extension"),
			       tr("The file %1 was loaded properly.").arg(i),
			       tr("The file %1 was loaded properly.").arg(i),
			       __FILE__,
			       __LINE__);
		    else
		      addError
			(tr("SQLite Run-Time Loadable Extension"),
			 tr("The file %1 was not loaded.").arg(i),
			 error ?
			 error : tr("Error with sqlite3_load_extension()."),
			 __FILE__,
			 __LINE__);

		    sqlite3_free(error);
		  }
		else
		  addError(tr("SQLite Run-Time Loadable Extension"),
			   tr("The file %1 is not readable.").arg(i),
			   tr("The file %1 is not readable.").arg(i),
			   __FILE__,
			   __LINE__);
	      }
	}
    }
#endif

  QSettings settings;

  settings.setValue("previous_branch_name", br.branch_name->currentText());
  m_selectedBranch = m_branches[br.branch_name->currentText()];

  if(m_connected_bar_label != nullptr)
    {
      m_connected_bar_label->setToolTip(tr("Connected"));
      prepareStatusBarIcons();
    }

  m_otherOptions->sqliteConnected();
  ui.actionDatabaseSearch->setEnabled(true);
  ui.actionDisconnect->setEnabled(true);
  ui.actionQuery_History->setEnabled(true);
  ui.actionRefreshTable->setEnabled(true);
  ui.actionSelect_All->setEnabled(true);
  ui.actionStatistics->setEnabled(true);
  ui.actionViewDetails->setEnabled(true);
  ui.configTool->setEnabled(true);
  ui.customQueryTool->setEnabled(true);
  ui.detailsTool->setEnabled(true);
  ui.disconnectTool->setEnabled(true);
  ui.filesTool->setEnabled(true);
  ui.printTool->setEnabled(true);
  ui.refreshTool->setEnabled(true);
  ui.searchTool->setEnabled(true);

  if(m_db.driverName() == "QSQLITE")
    {
      ui.actionChangePassword->setEnabled(false);
      ui.actionImportCSV->setEnabled(true);
      ui.actionPatron_Reservation_History->setChecked
	(!settings.value("dnt").toBool());
      ui.actionPatron_Reservation_History->setEnabled(true);
      ui.action_Merge_SQLite_Databases->setEnabled(true);
      ui.action_Upgrade_SQLite_SchemaAll->setEnabled(true);
      ui.action_Upgrade_SQLite_SchemaRecent->setEnabled(true);
      ui.action_VacuumDatabase->setEnabled(true);
      ui.menuEntriesPerPage->setEnabled(true);

      if(!ui.menuEntriesPerPage->actions().isEmpty())
	ui.menuEntriesPerPage->actions().at
	  (ui.menuEntriesPerPage->actions().size() - 1)->setEnabled(true);

      /*
      ** Set the window's title.
      */

      if(!m_roles.isEmpty())
	setWindowTitle
	  (tr("BiblioteQ: %1 (%2)").
	   arg(QFileInfo(br.filename->text()).fileName()).
	   arg(m_roles));
      else
	setWindowTitle
	  (tr("BiblioteQ: %1 (%2)").
	   arg(QFileInfo(br.filename->text()).fileName()).
	   arg("missing roles"));
    }
  else
    {
      ui.menuEntriesPerPage->setEnabled(true);

      if(!ui.menuEntriesPerPage->actions().isEmpty())
	ui.menuEntriesPerPage->actions().at
	  (ui.menuEntriesPerPage->actions().size() - 1)->setEnabled(true);

      ui.actionChangePassword->setEnabled(true);
      disconnect(ui.graphicsView->scene(),
		 SIGNAL(itemDoubleClicked(void)),
		 this,
		 SLOT(slotViewDetails(void)));
      disconnect(ui.table,
		 SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
		 this,
		 SLOT(slotViewDetails(void)));
      connect(ui.graphicsView->scene(),
	      SIGNAL(itemDoubleClicked(void)),
	      this,
	      SLOT(slotViewDetails(void)));
      connect(ui.table,
	      SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
	      this,
	      SLOT(slotViewDetails(void)));

      /*
      ** Set the window's title.
      */

      if(m_roles.isEmpty())
	setWindowTitle
	  (tr("BiblioteQ: %1 (%2)").
	   arg(m_db.databaseName()).arg(dbUserName()));
      else
      	setWindowTitle
	  (tr("BiblioteQ: %1 (%2) (%3)").
	   arg(m_db.databaseName()).arg(dbUserName()).arg(m_roles));
    }

  prepareFilter();

  if(br.role->currentIndex() == 0 || m_db.driverName() == "QSQLITE")
    {
      if(m_db.driverName() == "QSQLITE")
	{
	  /*
	  ** Add the database's information to the pulldown menu.
	  */

	  auto const actions = ui.menu_Recent_SQLite_Files->actions();
	  auto exists = false;

	  for(int i = 0; i < actions.size(); i++)
	    if(actions[i]->data().toString() == br.filename->text())
	      {
		exists = true;
		break;
	      }

	  if(!exists)
	    {
	      QSettings settings;
	      auto const allKeys(settings.allKeys());
	      int index = 1;

	      for(int i = 0; i < allKeys.size(); i++)
		if(allKeys[i].startsWith("sqlite_db_"))
		  index += 1;

	      settings.setValue
		(QString("sqlite_db_%1").arg(index), br.filename->text());
	      createSqliteMenuActions();
	    }
	}

      adminSetup();
    }
  else if(br.role->currentIndex() == 1)
    {
      /*
      ** Guest.
      */

      ui.actionChangePassword->setEnabled(false);
    }
  else
    {
      /*
      ** Patron.
      */

      ui.actionChangePassword->setEnabled(true);
      ui.actionPatron_Reservation_History->setChecked
	(!biblioteq_misc_functions::dnt(m_db, dbUserName(), errorstr));
      ui.actionRequests->setData
	(static_cast<int> (RequestActionItems::REQUEST_SELECTED));
      ui.actionRequests->setEnabled(true);
      ui.actionRequests->setToolTip(tr("Request Selected Item(s)"));
      ui.actionReservationHistory->setEnabled(true);
    }

  auto found = false;

  if(m_db.driverName() == "QSQLITE")
    {
      for(int i = 0; i < ui.menuEntriesPerPage->actions().size(); i++)
	if(ui.menuEntriesPerPage->actions().at(i)->data().toInt() ==
	   settings.value("sqlite_entries_per_page").toInt())
	  {
	    found = true;
	    ui.menuEntriesPerPage->actions().at(i)->setChecked(true);
	    break;
	  }
    }
  else
    {
      for(int i = 0; i < ui.menuEntriesPerPage->actions().size(); i++)
	if(ui.menuEntriesPerPage->actions().at(i)->data().toInt() ==
	   settings.value("postgresql_entries_per_page").toInt())
	  {
	    found = true;
	    ui.menuEntriesPerPage->actions().at(i)->setChecked(true);
	    break;
	  }
    }

  if(!found && !ui.menuEntriesPerPage->actions().isEmpty())
    ui.menuEntriesPerPage->actions().at(0)->setChecked(true);

  found = false;

  for(int i = 0; i < ui.menu_Category->actions().size(); i++)
    if(m_lastCategory ==
       ui.menu_Category->actions().at(i)->data().toString())
      {
	found = true;
	ui.categoryLabel->setText(ui.menu_Category->actions().at(i)->text());
	ui.menu_Category->actions().at(i)->setChecked(true);
	ui.menu_Category->setDefaultAction(ui.menu_Category->actions().at(i));
	break;
      }

  if(!found)
    {
      ui.categoryLabel->setText(tr("All"));

      if(!ui.menu_Category->actions().isEmpty())
	ui.menu_Category->actions().at(0)->setChecked(true);

      ui.menu_Category->setDefaultAction(ui.menu_Category->actions().value(0));
    }

  bb.database_enumerations_browser_label->setVisible
    (canAccessDatabaseEnumerations());

  if(ui.actionPopulateOnStart->isChecked())
    slotRefresh();

  canAccessOverdueItems() && ui.actionOverdue_Items_Notification->isChecked() ?
    m_overdueItemsTimer.start() : m_overdueItemsTimer.stop();
  prepareContextMenus();
  prepareUpgradeNotification();
}

void biblioteq::slotDisconnect(void)
{
  if(db_enumerations &&
     db_enumerations->isVisible() &&
     !db_enumerations->close())
    return;

  std::unique_lock<std::mutex> lock{m_mutex, std::defer_lock};

  if(!lock.try_lock())
    return;
  else
    lock.unlock();

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!emptyContainers())
    {
      QApplication::restoreOverrideCursor();
      return;
    }
  else
    QApplication::restoreOverrideCursor();

  br.show_password->setChecked(false);
  m_allSearchShown = false;
  m_attachedWindows.clear();
  m_batchActivities->reset();
  m_files ? m_files->reset() : (void) 0;
  m_import->reset();
  m_membersWasRefreshed = false;
  m_overdueItemsTimer.stop();
  m_pages = 0;
  m_queryHistory->reset();
  m_queryOffset = 0;
  m_roles = "";
  delete m_searchQuery;
  m_searchQuery = nullptr;
  m_sqliteMergeDatabases ? m_sqliteMergeDatabases->reset() : (void) 0;
  userinfo_diag->m_memberProperties.clear();
#ifdef Q_OS_ANDROID
  m_admin_diag->hide();
  m_all_diag->hide();
  m_batchActivities->hide();
  m_history_diag->hide();
  m_import->hide();
  m_members_diag->hide();
  m_queryHistory->hide();
  m_sqliteMergeDatabases ? (void) m_sqliteMergeDatabases->hide() : (void) 0;
#else
  m_admin_diag->close();
  m_all_diag->close();
  m_batchActivities->close();
  m_history_diag->close();
  m_import->close();
  m_members_diag->close();
  m_queryHistory->close();
  m_sqliteMergeDatabases ? (void) m_sqliteMergeDatabases->close() : (void) 0;
#endif

  foreach(auto dialog, findChildren<biblioteq_custom_query *> ())
    if(dialog)
      dialog->deleteLater();

  foreach(auto window, findChildren<biblioteq_statistics *> ())
    if(window)
      window->deleteLater();

  m_unaccent.clear();

  if(db_enumerations)
    db_enumerations->clear();

  resetAdminBrowser();
  resetMembersBrowser();
  ui.actionAutoPopulateOnCreation->setEnabled(false);
  ui.actionBatchActivitiesBrowser->setEnabled(false);
  ui.actionChangePassword->setEnabled(false);
  ui.actionDatabaseSearch->setEnabled(false);
  ui.actionDeleteEntry->setEnabled(false);
  ui.actionDisconnect->setEnabled(false);
  ui.actionDuplicateEntry->setEnabled(false);
  ui.actionImportCSV->setEnabled(false);
  ui.actionMagic->setEnabled(false);
  ui.actionMembersBrowser->setEnabled(false);
  ui.actionModifyEntry->setEnabled(false);
  ui.actionPatron_Reservation_History->setEnabled(false);
  ui.actionQuery_History->setEnabled(false);
  ui.actionRefreshTable->setEnabled(false);
  ui.actionReservationHistory->setEnabled(false);
  ui.actionSelect_All->setEnabled(false);
  ui.actionStatistics->setEnabled(false);
  ui.actionViewDetails->setEnabled(false);
  ui.action_Merge_SQLite_Databases->setEnabled(false);
  ui.action_Upgrade_SQLite_SchemaAll->setEnabled(false);
  ui.action_Upgrade_SQLite_SchemaRecent->setEnabled(false);
  ui.action_VacuumDatabase->setEnabled(false);
  ui.configTool->setEnabled(false);
  ui.createTool->setEnabled(false);
  ui.customQueryTool->setEnabled(false);
  ui.deleteTool->setEnabled(false);
  ui.detailsTool->setEnabled(false);
  ui.disconnectTool->setEnabled(false);
  ui.duplicateTool->setEnabled(false);
  ui.filesTool->setEnabled(false);
  ui.menuEntriesPerPage->setEnabled(false);
  ui.menu_Add_Item->setEnabled(false);
  ui.modifyTool->setEnabled(false);
  ui.nextPageButton->setEnabled(false);
  ui.pagesLabel->setText(tr("1"));
  ui.previousPageButton->setEnabled(false);
  ui.printTool->setEnabled(false);
  ui.refreshTool->setEnabled(false);
  ui.reserveTool->setEnabled(false);
  ui.searchTool->setEnabled(false);
  ui.userTool->setEnabled(false);

  if(!ui.menuEntriesPerPage->actions().isEmpty())
    ui.menuEntriesPerPage->actions().at
      (ui.menuEntriesPerPage->actions().size() - 1)->setEnabled(true);

  ui.actionPopulate_Administrator_Browser_Table_on_Display->setEnabled(false);
  ui.actionPopulate_Database_Enumerations_Browser_on_Display->setEnabled(false);
  ui.actionPopulate_Members_Browser_Table_on_Display->setEnabled(false);
  ui.actionConfigureAdministratorPrivileges->setEnabled(false);
  ui.actionDatabase_Enumerations->setEnabled(false);
  ui.actionRequests->setData
    (static_cast<int> (RequestActionItems::INACTIVE));
  ui.actionRequests->setEnabled(false);
  ui.actionRequests->setIcon(QIcon(":/32x32/request.png"));
  ui.actionRequests->setToolTip(tr("Item Requests (PostgreSQL)"));
  ui.action_Database_Enumerations->setEnabled(false);
  ui.graphicsView->scene()->clear();
  ui.graphicsView->setSceneRect(0.0, 0.0, 1.0, 1.0);
  bb.table->disconnect(SIGNAL(itemDoubleClicked(QTableWidgetItem *)));
  ui.table->disconnect(SIGNAL(itemDoubleClicked(QTableWidgetItem *)));
  ui.graphicsView->scene()->disconnect(SIGNAL(itemDoubleClicked(void)));
  slotResetAllSearch();

  if(m_db.isOpen())
    {
      QSettings settings;

      if(m_db.driverName() == "QSQLITE")
	{
	  for(int i = 0; i < ui.menuEntriesPerPage->actions().size(); i++)
	    if(ui.menuEntriesPerPage->actions().at(i)->isChecked())
	      {
		settings.setValue
		  ("sqlite_entries_per_page",
		   ui.menuEntriesPerPage->actions().at(i)->data().toInt());
		break;
	      }
	}
      else
	{
	  for(int i = 0; i < ui.menuEntriesPerPage->actions().size(); i++)
	    if(ui.menuEntriesPerPage->actions().at(i)->isChecked())
	      {
		settings.setValue
		  ("postgresql_entries_per_page",
		   ui.menuEntriesPerPage->actions().at(i)->data().toInt());
		break;
	      }
	}
    }

  if(!ui.menuEntriesPerPage->actions().isEmpty())
    ui.menuEntriesPerPage->actions().at(0)->setChecked(true);

  if(ui.actionResetErrorLogOnDisconnect->isChecked())
    slotResetErrorLog();

  m_findList.clear();
  ui.graphicsView->horizontalScrollBar()->setValue(0);
  ui.graphicsView->resetTransform();
  ui.graphicsView->scene()->clear();
  ui.graphicsView->verticalScrollBar()->setValue(0);
  ui.itemsCountLabel->setText(tr("0 Results"));
  ui.nextPageButton->setEnabled(false);
  ui.pagesLabel->setText(tr("1"));
  ui.previousPageButton->setEnabled(false);
  ui.table->resetTable(dbUserName(), m_previousTypeFilter, m_roles);
  prepareFilter();

  for(int i = 0; i < ui.menu_Category->actions().size(); i++)
    if(m_previousTypeFilter ==
       ui.menu_Category->actions().at(i)->data().toString())
      {
	ui.categoryLabel->setText(ui.menu_Category->actions().at(i)->text());
	ui.menu_Category->actions().at(i)->setChecked(true);
	ui.menu_Category->setDefaultAction(ui.menu_Category->actions().at(i));
	break;
      }

  addConfigOptions(m_previousTypeFilter);
  slotDisplaySummary();
  m_deletedAdmins.clear();
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(m_db.isOpen())
    m_db.close();

  QApplication::restoreOverrideCursor();
  m_db = QSqlDatabase();

  if(m_connected_bar_label != nullptr)
    {
      m_connected_bar_label->setToolTip(tr("Disconnected"));
      prepareStatusBarIcons();
    }

  if(m_status_bar_label != nullptr)
    {
      m_status_bar_label->setToolTip(tr("Standard User Mode"));
      prepareStatusBarIcons();
    }

  if(QSqlDatabase::contains("Default"))
    QSqlDatabase::removeDatabase("Default");

  setWindowTitle(tr("BiblioteQ"));
}

void biblioteq::slotDisplaySummary(void)
{
  if(ui.stackedWidget->currentIndex() == 1) // Table Mode
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);

      QPainterPath painterPath;
      auto const items(ui.graphicsView->scene()->items());
      auto const tableItems(ui.table->selectionModel()->selectedRows());

      for(int ii = 0; ii < tableItems.size(); ii++)
	{
	  auto const oid = biblioteq_misc_functions::getColumnString
	    (ui.table,
	     tableItems.at(ii).row(),
	     ui.table->columnNumber("MYOID"));
	  auto const type =  biblioteq_misc_functions::getColumnString
	    (ui.table,
	     tableItems.at(ii).row(),
	     ui.table->columnNumber("Type")).remove(' ').toLower();

	  for(int jj = 0; jj < items.size(); jj++)
	    if(oid == items.at(jj)->data(0).toString() &&
	       type == items.at(jj)->data(1).toString())
	      painterPath.addRect
		(items.at(jj)->
		 mapRectToScene(items.at(jj)->boundingRect()));
	    else
	      items.at(jj)->setSelected(false);
	}

      ui.graphicsView->scene()->setSelectionArea(painterPath);
      QApplication::restoreOverrideCursor();
    }

  /*
  ** Display a preview.
  */

  if(ui.itemSummary->width() > 0 && ui.table->currentRow() > -1)
    {
      QImage backImage;
      QImage frontImage;
      QString summary("");
      QString tmpstr("");
      auto const i = ui.table->currentRow();
      auto const oid = biblioteq_misc_functions::getColumnString
	(ui.table, i, ui.table->columnNumber("MYOID"));
      auto const type = biblioteq_misc_functions::getColumnString
	(ui.table, i, ui.table->columnNumber("Type"));

      summary = "<html>";

      if(type == "Book")
	{
	  summary += "<b>" +
	    biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Title")) +
	    "</b>";
	  summary += "<br>";
	  tmpstr = biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("ISBN-10"));

	  if(tmpstr.isEmpty())
	    tmpstr = biblioteq_misc_functions::getColumnString
	      (ui.table, i, ui.table->columnNumber("ID Number"));

	  if(tmpstr.isEmpty())
	    tmpstr = "<br>";

	  summary += tmpstr;
	  summary += "<br>";
	  summary += biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Publication Date"));
	  summary += "<br>";
	  summary += biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Publisher"));
	  summary += "<br>";
	  summary += biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Place of Publication"));
	  summary += "<br>";
	}
      else if(type == "CD")
	{
	  summary += "<b>" +
	    biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Title")) +
	    "</b>";
	  summary += "<br>";
	  tmpstr = biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Catalog Number"));

	  if(tmpstr.isEmpty())
	    tmpstr = biblioteq_misc_functions::getColumnString
	      (ui.table, i, ui.table->columnNumber("ID Number"));

	  if(tmpstr.isEmpty())
	    tmpstr = "<br>";

	  summary += tmpstr;
	  summary += "<br>";
	  tmpstr = biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Publication Date"));

	  if(tmpstr.isEmpty())
	    tmpstr = biblioteq_misc_functions::getColumnString
	      (ui.table, i, ui.table->columnNumber("Release Date"));

	  if(tmpstr.isEmpty())
	    tmpstr = "<br>";

	  summary += tmpstr;
	  summary += "<br>";
	  tmpstr = biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Publisher"));

	  if(tmpstr.isEmpty())
	    tmpstr = biblioteq_misc_functions::getColumnString
	      (ui.table, i, ui.table->columnNumber("Recording Label"));

	  if(tmpstr.isEmpty())
	    tmpstr = "<br>";

	  summary += tmpstr;
	  summary += "<br>";
	}
      else if(type == "DVD")
	{
	  summary += "<b>" +
	    biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Title")) +
	    "</b>";
	  summary += "<br>";
	  tmpstr = biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("UPC"));

	  if(tmpstr.isEmpty())
	    tmpstr = biblioteq_misc_functions::getColumnString
	      (ui.table, i, ui.table->columnNumber("ID Number"));

	  if(tmpstr.isEmpty())
	    tmpstr = "<br>";

	  summary += tmpstr;
	  summary += "<br>";
	  tmpstr = biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Publication Date"));

	  if(tmpstr.isEmpty())
	    tmpstr = biblioteq_misc_functions::getColumnString
	      (ui.table, i, ui.table->columnNumber("Release Date"));

	  if(tmpstr.isEmpty())
	    tmpstr = "<br>";

	  summary += tmpstr;
	  summary += "<br>";
	  tmpstr = biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Publisher"));

	  if(tmpstr.isEmpty())
	    tmpstr = biblioteq_misc_functions::getColumnString
	      (ui.table, i, ui.table->columnNumber("Studio"));

	  if(tmpstr.isEmpty())
	    tmpstr = "<br>";

	  summary += tmpstr;
	  summary += "<br>";
	}
      else if(type == "Grey Literature")
	{
	  summary += "<b>" +
	    biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Title")) +
	    "</b>";
	  summary += "<br>";
	  tmpstr = biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("ID"));

	  if(tmpstr.isEmpty())
	    tmpstr = biblioteq_misc_functions::getColumnString
	      (ui.table, i, ui.table->columnNumber("ID Number"));

	  if(tmpstr.isEmpty())
	    tmpstr = "<br>";

	  summary += tmpstr;
	  tmpstr = biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("File Count"));

	  if(!tmpstr.isEmpty())
	    summary += "<br>" + tr("%1 File(s)").arg(tmpstr);

	  summary += "<br>";
	}
      else if(type == "Journal" || type == "Magazine")
	{
	  summary += "<b>" +
	    biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Title")) +
	    "</b>";
	  summary += "<br>";
	  tmpstr = biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("ISSN"));

	  if(tmpstr.isEmpty())
	    tmpstr = biblioteq_misc_functions::getColumnString
	      (ui.table, i, ui.table->columnNumber("ID Number"));
	  else
	    {
	      tmpstr += QString(" Vol. %1, No. %2").
		arg(biblioteq_misc_functions::
		    getColumnString(ui.table,
				    i,
				    ui.table->columnNumber("Volume"))).
		arg(biblioteq_misc_functions::
		    getColumnString(ui.table,
				    i,
				    ui.table->columnNumber("Issue")));
	    }

	  if(tmpstr.isEmpty())
	    tmpstr = "<br>";

	  summary += tmpstr;
	  summary += "<br>";
	  summary += biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Publication Date"));
	  summary += "<br>";
	  summary += biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Publisher"));
	  summary += "<br>";
	  summary += biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Place of Publication"));
	  summary += "<br>";
	}
      else if(type == "Photograph Collection")
	{
	  summary += "<b>" +
	    biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Title")) +
	    "</b>";
	  summary += "<br>";
	  tmpstr = biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("ID"));

	  if(tmpstr.isEmpty())
	    tmpstr = biblioteq_misc_functions::getColumnString
	      (ui.table, i, ui.table->columnNumber("ID Number"));

	  if(tmpstr.isEmpty())
	    tmpstr = "<br>";

	  summary += tmpstr;
	  tmpstr = biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Photograph Count"));

	  if(!tmpstr.isEmpty())
	    summary += "<br>" + tr("%1 Photograph(s)").arg(tmpstr);

	  summary += "<br>";
	}
      else if(type == "Video Game")
	{
	  summary += "<b>" +
	    biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Title")) +
	    "</b>";
	  summary += "<br>";
	  tmpstr = biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("UPC"));

	  if(tmpstr.isEmpty())
	    tmpstr = biblioteq_misc_functions::getColumnString
	      (ui.table, i, ui.table->columnNumber("ID Number"));

	  if(tmpstr.isEmpty())
	    tmpstr = "<br>";

	  summary += tmpstr;
	  summary += "<br>";
	  tmpstr = biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Publication Date"));

	  if(tmpstr.isEmpty())
	    tmpstr = biblioteq_misc_functions::getColumnString
	      (ui.table, i, ui.table->columnNumber("Release Date"));

	  if(tmpstr.isEmpty())
	    tmpstr = "<br>";

	  summary += tmpstr;
	  summary += "<br>";
	  summary += biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Publisher"));
	  summary += "<br>";
	}

      summary += biblioteq_misc_functions::getAbstractInfo(oid, type, m_db);
      summary += "<br>";

      if(type != "Photograph Collection")
	{
	  tmpstr = biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Availability"));

	  if(!tmpstr.isEmpty())
	    {
	      if(tmpstr.toInt() > 0)
		summary += tr("Available") + "<br>";
	      else
		summary += tr("Unavailable") + "<br>";
	    }
	}

      summary += biblioteq_misc_functions::getColumnString
	(ui.table, i, ui.table->columnNumber("Location"));

      while(summary.contains("<br><br>"))
	summary.replace("<br><br>", "<br>");

      if(type == "Book")
	if(biblioteq_misc_functions::isBookRead(m_db, oid.toULongLong()))
	  {
	    summary += "<br>";
	    summary += tr("<b>Read</b>");
	  }

      summary += "</html>";
      ui.summary->setText(summary);
      ui.summary->setVisible(true);
      QApplication::setOverrideCursor(Qt::WaitCursor);

      if(type != "Grey Literature" &&
	 type != "Photograph Collection")
	frontImage = biblioteq_misc_functions::getImage
	  (oid, "front_cover", type,
	   m_db);
      else
	frontImage = biblioteq_misc_functions::getImage
	  (oid, "image_scaled", type,
	   m_db);

      if(type != "Grey Literature" &&
	 type != "Photograph Collection")
	backImage = biblioteq_misc_functions::getImage
	  (oid, "back_cover", type, m_db);

      QApplication::restoreOverrideCursor();

      /*
      ** The size of missing_image.png is AxB.
      */

      if(frontImage.isNull())
	frontImage = QImage(":/missing_image.png");

      if(!frontImage.isNull())
	frontImage = frontImage.scaled
	  (s_noImageResize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

      if(type != "Grey Literature" &&
	 type != "Photograph Collection")
	{
	  if(backImage.isNull())
	    backImage = QImage(":/missing_image.png");

	  if(!backImage.isNull())
	    backImage = backImage.scaled
	      (s_noImageResize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}

      if(!frontImage.isNull())
	{
	  ui.frontImage->setVisible(true);
	  ui.frontImage->setPixmap(QPixmap::fromImage(frontImage));
	}
      else
	ui.frontImage->clear();

      if(type != "Grey Literature" &&
	 type != "Photograph Collection")
	{
	  if(!backImage.isNull())
	    {
	      ui.backImage->setPixmap(QPixmap::fromImage(backImage));
	      ui.backImage->setVisible(true);
	    }
	  else
	    ui.backImage->clear();
	}
      else
	ui.backImage->clear();
    }
  else
    {
      /*
      ** Clear the scene.
      */

      ui.backImage->clear();
      ui.backImage->setVisible(false);
      ui.frontImage->clear();
      ui.frontImage->setVisible(false);
      ui.summary->clear();
      ui.summary->setVisible(false);
    }
}

void biblioteq::slotExportMembersAsCSV(void)
{
  QFileDialog dialog(m_members_diag);

  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setDefaultSuffix("csv");
  dialog.setDirectory(QDir::homePath());
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setNameFilter(tr("CSV (*.csv)"));
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowTitle(tr("BiblioteQ: Export Patrons As CSV"));
  dialog.exec();
  QApplication::processEvents();

  if(dialog.result() == QDialog::Accepted)
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);

      QFile file(dialog.selectedFiles().value(0));

      if(file.open(QIODevice::Text |
		   QIODevice::Truncate |
		   QIODevice::WriteOnly))
	{
	  QString str("");
	  QTextStream stream(&file);

	  foreach(auto const &f, QStringList()
		  << tr("City")
		  << tr("Comments")
		  << tr("Date of Birth")
		  << tr("E-Mail Address")
		  << tr("Expiration Date")
		  << tr("First Name")
		  << tr("General Registration Number")
		  << tr("Last Name")
		  << tr("Maximum Reserved Books")
		  << tr("Member Class")
		  << tr("Member ID")
		  << tr("Member Since")
		  << tr("Membership Fees")
		  << tr("Middle Initial")
		  << tr("Overdue Fees")
		  << tr("Sex")
		  << tr("State")
		  << tr("Street")
		  << tr("Telephone Number")
		  << tr("ZIP"))
	    str += QString("%1,").arg(f);

	  if(str.endsWith(","))
	    str = str.mid(0, str.length() - 1);

	  if(!str.isEmpty())
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
	    stream << str << Qt::endl;
#else
	    stream << str << endl;
#endif

	  QSqlQuery query(m_db);

	  if(query.exec("SELECT city, "
			"comments, "
			"dob, "
			"email, "
			"expiration_date, "
			"first_name, "
			"general_registration_number, "
			"last_name, "
			"maximum_reserved_books, "
			"memberclass, "
			"memberid, "
			"membersince, "
			"membership_fees, "
			"middle_init, "
			"overdue_fees, "
			"sex, "
			"state_abbr, "
			"street, "
			"telephone_num, "
			"zip "
			"FROM member"))
	    while(query.next())
	      {
		str = "";

		for(int i = 0; i < query.record().count(); i++)
		  {
		    auto cleaned(query.value(i).toString());

		    cleaned.replace("\n", " ");
		    cleaned.replace("\r\n", " ");
		    cleaned = cleaned.trimmed();

		    if(cleaned.contains(","))
		      str += QString("\"%1\",").arg(cleaned);
		    else
		      str += QString("%1,").arg(cleaned);
		  }

		if(str.endsWith(","))
		  str = str.mid(0, str.length() - 1);

		if(!str.isEmpty())
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
		  stream << str << Qt::endl;
#else
	          stream << str << endl;
#endif
	      }

	  file.close();
	}

      QApplication::restoreOverrideCursor();
    }
}

void biblioteq::slotGeneralSearchPublicationDateEnabled(bool state)
{
  al.publication_date->setEnabled(state);

  if(!state)
    al.publication_date->setDate(QDate::fromString("2001", "yyyy"));
}

void biblioteq::slotGraphicsSceneDeleteKeyPressed(void)
{
  slotMainTableDeleteKeyPressed();
}

void biblioteq::slotGraphicsSceneEnterKeyPressed(void)
{
  slotMainTableEnterKeyPressed();
}

void biblioteq::slotGreyLiteratureSearch(void)
{
  auto gl = dynamic_cast<biblioteq_grey_literature *>
    (findItemInTab("search-grey-literature"));

  if(!gl)
    {
      foreach(auto w, QApplication::topLevelWidgets())
	{
	  auto g = qobject_cast<biblioteq_grey_literature *> (w);

	  if(g && g->getID() == "search-grey-literature")
	    {
	      gl = g;
	      break;
	    }
	}
    }

  if(!gl)
    {
      gl = new biblioteq_grey_literature
	(this, "search-grey-literature", QModelIndex());
      addItemWindowToTab(gl);
      gl->search();
      connect(this,
	      SIGNAL(databaseEnumerationsCommitted(void)),
	      gl,
	      SLOT(slotDatabaseEnumerationsCommitted(void)));
    }
  else
    addItemWindowToTab(gl);
}

void biblioteq::slotInsertGreyLiterature(void)
{
  biblioteq_grey_literature *gl = nullptr;

  m_idCt += 1;
  gl = new biblioteq_grey_literature
    (this, QString("insert_%1").arg(m_idCt), QModelIndex());
  addItemWindowToTab(gl);
  gl->insert();
  connect(this,
	  SIGNAL(databaseEnumerationsCommitted(void)),
	  gl,
	  SLOT(slotDatabaseEnumerationsCommitted(void)));
}

void biblioteq::slotItemChanged(QTableWidgetItem *item)
{
  if(!item || !(Qt::ItemIsUserCheckable & item->flags()))
    return;

  QApplication::setOverrideCursor(Qt::WaitCursor);
  biblioteq_misc_functions::setBookRead
    (m_db,
     item->checkState() == Qt::Checked,
     item->data(Qt::UserRole).toULongLong());
  QApplication::restoreOverrideCursor();
  slotDisplaySummary();
}

void biblioteq::slotLastWindowClosed(void)
{
  if(ui.actionAutomaticallySaveSettingsOnExit->isChecked())
    slotSaveConfig();

  cleanup();
}

void biblioteq::slotMainTableDeleteKeyPressed(void)
{
  if(m_roles.contains("administrator") || m_roles.contains("librarian"))
    slotDelete();
}

void biblioteq::slotMainTableEnterKeyPressed(void)
{
  if(m_roles.contains("administrator") || m_roles.contains("librarian"))
    slotModify();
  else
    slotViewDetails();
}

void biblioteq::slotMainWindowCanvasBackgroundColorChanged(const QColor &color)
{
  QSettings settings;

  if(color.isValid())
    {
      settings.setValue("mainwindow_canvas_background_color", color.name());
      ui.graphicsView->scene()->setBackgroundBrush(color);
    }
  else
    {
      QColor color(settings.value("mainwindow_canvas_background_color").
		   toString().remove('&').trimmed());

      if(!color.isValid())
	color = QColor(Qt::white);

      ui.graphicsView->scene()->setBackgroundBrush(color);
    }
}

void biblioteq::slotMembersContextMenu(const QPoint &point)
{
  QMenu menu(m_members_diag);

  menu.addAction(tr("Delete Selected Member"),
		 this,
		 SLOT(slotRemoveMember(void)));
  menu.addAction(tr("Modify Selected Member..."),
		 this,
		 SLOT(slotModifyBorrower(void)));
  menu.addSeparator();
  menu.addAction(tr("List Selected Member's Reserved Items"),
		 this,
		 SLOT(slotListReservedItems(void)));
  menu.addAction(tr("Print Selected Member's Reserved Items..."),
		 this,
		 SLOT(slotPrintReserved(void)));
  menu.addAction(tr("Show Selected Member's Reservation History..."),
		 this,
		 SLOT(slotShowHistory(void)));
  menu.addSeparator();
  menu.addAction(tr("Refresh Table"),
		 this,
		 SLOT(slotPopulateMembersBrowser(void)));
  menu.addSeparator();
  menu.addAction(tr("Reserve Selected Item..."),
		 this,
		 SLOT(slotCheckout(void)))->setEnabled
    (!isCurrentItemAPhotograph());
  menu.exec(bb.table->mapToGlobal(point));
}

void biblioteq::slotModifyBorrower(void)
{
  auto const row = bb.table->currentRow();

  if(row < 0)
    {
      QMessageBox::critical
	(m_members_diag,
	 tr("BiblioteQ: User Error"),
	 tr("Please select a member to modify."));
      QApplication::processEvents();
      return;
    }

  QSqlQuery query(m_db);
  QString str("");
  int i = 0;

  str = biblioteq_misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("Member ID"));
  query.setForwardOnly(true);
  query.prepare("SELECT * FROM member WHERE memberid = ?");
  query.bindValue(0, str);
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!query.exec() || !query.next())
    {
      QApplication::restoreOverrideCursor();
      addError(tr("Database Error"),
	       tr("Unable to retrieve the selected member's information."),
	       query.lastError().text(),
	       __FILE__,
	       __LINE__);

      if(userinfo_diag->isVisible())
	QMessageBox::critical
	  (userinfo_diag,
	   tr("BiblioteQ: Database Error"),
	   tr("Unable to retrieve the selected member's information."));
      else
	QMessageBox::critical
	  (m_members_diag,
	   tr("BiblioteQ: Database Error"),
	   tr("Unable to retrieve the selected member's information."));

      QApplication::processEvents();
      return;
    }
  else
    {
      QApplication::restoreOverrideCursor();

      auto const record(query.record());

      for(i = 0; i < record.count(); i++)
	{
	  auto const fieldname = record.fieldName(i);
	  auto const var = record.field(i).value();

	  if(fieldname == "city")
	    userinfo_diag->m_userinfo.city->setText(var.toString().trimmed());
	  else if(fieldname == "comments")
	    userinfo_diag->m_userinfo.comments->setPlainText
	      (var.toString().trimmed());
	  else if(fieldname == "dob")
	    userinfo_diag->m_userinfo.dob->setDate
	      (QDate::fromString(var.toString().trimmed(),
				 s_databaseDateFormat));
	  else if(fieldname == "email")
	    userinfo_diag->m_userinfo.email->setText(var.toString().trimmed());
	  else if(fieldname == "expiration_date")
	    userinfo_diag->m_userinfo.expirationdate->setDate
	      (QDate::fromString(var.toString().trimmed(),
				 s_databaseDateFormat));
	  else if(fieldname == "first_name")
	    userinfo_diag->m_userinfo.firstName->setText
	      (var.toString().trimmed());
	  else if(fieldname == "general_registration_number")
	    userinfo_diag->m_userinfo.generalregistrationnumber->setText
	      (var.toString().trimmed());
	  else if(fieldname == "last_name")
	    userinfo_diag->m_userinfo.lastName->setText
	      (var.toString().trimmed());
	  else if(fieldname == "maximum_reserved_books")
	    userinfo_diag->m_userinfo.maximum_reserved_books->setValue
	      (var.toInt());
	  else if(fieldname == "maximum_reserved_item_type")
	    {
	      auto const str(query.value(i).toString().toLower().trimmed());

	      if(str == "cd")
		userinfo_diag->m_userinfo.maximum_reserved_cds->setValue
		  (record.field("maximum_reserved_item_value").value().toInt());
	      else if(str == "dvd")
		userinfo_diag->m_userinfo.maximum_reserved_dvds->setValue
		  (record.field("maximum_reserved_item_value").value().toInt());
	      else if(str == "grey_literature")
		userinfo_diag->m_userinfo.maximum_reserved_grey_literatures->
		  setValue(record.field("maximum_reserved_item_value").
			   value().toInt());
	      else if(str == "journal")
		userinfo_diag->m_userinfo.maximum_reserved_journals->
		  setValue(record.field("maximum_reserved_item_value").
			   value().toInt());
	      else if(str == "magazine")
		userinfo_diag->m_userinfo.maximum_reserved_magazines->
		  setValue(record.field("maximum_reserved_item_value").
			   value().toInt());
	      else if(str == "video_game")
		userinfo_diag->m_userinfo.maximum_reserved_video_games->
		  setValue(record.field("maximum_reserved_item_value").
			   value().toInt());
	    }
	  else if(fieldname == "memberclass")
	    userinfo_diag->m_userinfo.memberclass->setText
	      (var.toString().trimmed());
	  else if(fieldname == "memberid")
	    userinfo_diag->m_userinfo.memberid->setText
	      (var.toString().trimmed());
	  else if(fieldname == "membership_fees")
	    userinfo_diag->m_userinfo.membershipfees->setValue(var.toDouble());
	  else if(fieldname == "membersince")
	    userinfo_diag->m_userinfo.membersince->setDate
	      (QDate::fromString(var.toString().trimmed(),
				 s_databaseDateFormat));
	  else if(fieldname == "middle_init")
	    userinfo_diag->m_userinfo.middle->setText
	      (var.toString().trimmed());
	  else if(fieldname == "overdue_fees")
	    userinfo_diag->m_userinfo.overduefees->setValue(var.toDouble());
	  else if(fieldname == "sex")
	    {
	      if(userinfo_diag->m_userinfo.sex->
		 findText(var.toString().trimmed()) > -1)
		userinfo_diag->m_userinfo.sex->setCurrentIndex
		  (userinfo_diag->m_userinfo.sex->
		   findText(var.toString().trimmed()));
	      else
		userinfo_diag->m_userinfo.sex->setCurrentIndex(2); // Private
	    }
	  else if(fieldname == "state_abbr")
	    {
	      if(userinfo_diag->m_userinfo.state->
		 findText(var.toString().trimmed()) == -1)
		userinfo_diag->m_userinfo.state->setCurrentIndex(0);
	      else
		userinfo_diag->m_userinfo.state->setCurrentIndex
		  (userinfo_diag->m_userinfo.state->
		   findText(var.toString().trimmed()));
	    }
	  else if(fieldname == "street")
	    userinfo_diag->m_userinfo.street->setText
	      (var.toString().trimmed());
	  else if(fieldname == "telephone_num")
	    userinfo_diag->m_userinfo.telephoneNumber->setText
	      (var.toString().trimmed());
	  else if(fieldname == "zip")
	    userinfo_diag->m_userinfo.zip->setText(var.toString().trimmed());

	  if(fieldname.contains("date") ||
	     fieldname.contains("dob") ||
	     fieldname.contains("membersince"))
	    userinfo_diag->m_memberProperties[fieldname] =
	      QDate::fromString(var.toString().trimmed(), s_databaseDateFormat).
	      toString(Qt::ISODate);
	  else if(fieldname == "maximum_reserved_books")
	    userinfo_diag->m_memberProperties[fieldname] =
	      userinfo_diag->m_userinfo.maximum_reserved_books->text();
	  else if(fieldname == "membership_fees")
	    userinfo_diag->m_memberProperties[fieldname] =
	      userinfo_diag->m_userinfo.membershipfees->text();
	  else if(fieldname == "overdue_fees")
	    userinfo_diag->m_memberProperties[fieldname] =
	      userinfo_diag->m_userinfo.overduefees->text();
	  else
	    userinfo_diag->m_memberProperties[fieldname] =
	      var.toString().trimmed();
	}

      foreach(auto textfield, userinfo_diag->findChildren<QLineEdit *> ())
	textfield->setCursorPosition(0);
    }

  m_engUserinfoTitle = "Modify Member";
  userinfo_diag->m_userinfo.memberid->setPalette
    (userinfo_diag->m_userinfo.telephoneNumber->palette());
  userinfo_diag->m_userinfo.memberid->setReadOnly(true);
  userinfo_diag->m_userinfo.membersince->setFocus();
  userinfo_diag->m_userinfo.membersince->setMaximumDate(QDate::currentDate());
  userinfo_diag->m_userinfo.nextTool->setVisible(true);
  userinfo_diag->m_userinfo.prevTool->setVisible(true);
  userinfo_diag->m_userinfo.scrollArea->horizontalScrollBar()->setValue(0);
  userinfo_diag->m_userinfo.scrollArea->verticalScrollBar()->setValue(0);

  if(!userinfo_diag->isVisible())
    userinfo_diag->m_userinfo.tabWidget->setCurrentIndex(0);

  userinfo_diag->setWindowTitle(tr("BiblioteQ: Modify Member"));
  userinfo_diag->updateGeometry();
#ifdef Q_OS_ANDROID
  userinfo_diag->showMaximized();
#else
  userinfo_diag->resize
    (userinfo_diag->width(), qMax(-100 + m_members_diag->height(),
				  userinfo_diag->sizeHint().height()));
  biblioteq_misc_functions::center(userinfo_diag, m_members_diag);
  userinfo_diag->show();
#endif
}

void biblioteq::slotOpenOnlineDocumentation(void)
{
  QDesktopServices::openUrl
    (QUrl("https://github.com/textbrowser/biblioteq/"
	  "blob/master/Documentation/BiblioteQ.pdf"));
}

void biblioteq::slotOpenPDFFiles(void)
{
#if defined(BIBLIOTEQ_LINKED_WITH_POPPLER) || \
    defined(BIBLIOTEQ_QT_PDF_SUPPORTED)
  QFileDialog dialog(this);

  dialog.setDirectory(QDir::homePath());
  dialog.setFileMode(QFileDialog::ExistingFiles);
  dialog.setNameFilter("PDF (*.pdf)");
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowTitle(tr("BiblioteQ: Open PDF File(s)"));

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();

      if(dialog.selectedFiles().size() >= MAXIMUM_DEVICES_CONFIRMATION)
	if(QMessageBox::
	   question(this,
		    tr("BiblioteQ: Question"),
		    tr("Are you sure that you wish to open %1 PDF files?").
		    arg(dialog.selectedFiles().size()),
		    QMessageBox::No | QMessageBox::Yes,
		    QMessageBox::No) == QMessageBox::No)
	  {
	    QApplication::processEvents();
	    return;
	  }

      QApplication::setOverrideCursor(Qt::WaitCursor);

      for(int i = 0; i < dialog.selectedFiles().size(); i++)
	{
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
	  auto reader = new biblioteq_pdfreader(this);

	  reader->load(dialog.selectedFiles().at(i));
#else
	  auto reader = new biblioteq_documentationwindow
	    (QUrl::fromLocalFile(dialog.selectedFiles().at(i)), this);
#endif

	  biblioteq_misc_functions::center(reader, this);
#ifdef Q_OS_ANDROID
	  reader->showMaximized();
#else
	  reader->show();
#endif
	}

      QApplication::restoreOverrideCursor();
    }

  QApplication::processEvents();
#endif
}

void biblioteq::slotOtherOptionsSaved(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  biblioteq_misc_functions::highlightWidget
    (userinfo_diag->m_userinfo.city,
     m_otherOptions->membersMandatoryFieldColor());
  biblioteq_misc_functions::highlightWidget
    (userinfo_diag->m_userinfo.firstName,
     m_otherOptions->membersMandatoryFieldColor());
  biblioteq_misc_functions::highlightWidget
    (userinfo_diag->m_userinfo.lastName,
     m_otherOptions->membersMandatoryFieldColor());
  biblioteq_misc_functions::highlightWidget
    (userinfo_diag->m_userinfo.memberid,
     m_otherOptions->membersMandatoryFieldColor());
  biblioteq_misc_functions::highlightWidget
    (userinfo_diag->m_userinfo.street,
     m_otherOptions->membersMandatoryFieldColor());
  biblioteq_misc_functions::highlightWidget
    (userinfo_diag->m_userinfo.zip,
     m_otherOptions->membersMandatoryFieldColor());

  for(int i = 1; i < ui.tab->count(); i++)
    {
      auto widget = ui.tab->widget(i);

      if(dynamic_cast<biblioteq_book *> (widget))
	dynamic_cast<biblioteq_book *> (widget)->setPublicationDateFormat
	  (m_otherOptions->dateFormat("books"));
      else if(dynamic_cast<biblioteq_cd *> (widget))
	dynamic_cast<biblioteq_cd *> (widget)->setPublicationDateFormat
	  (m_otherOptions->dateFormat("musiccds"));
      else if(dynamic_cast<biblioteq_dvd *> (widget))
	dynamic_cast<biblioteq_dvd *> (widget)->setPublicationDateFormat
	  (m_otherOptions->dateFormat("dvds"));
      else if(dynamic_cast<biblioteq_grey_literature *> (widget))
	dynamic_cast<biblioteq_grey_literature *> (widget)->
	  setPublicationDateFormat
	  (m_otherOptions->dateFormat("greyliterature"));
      else if(dynamic_cast<biblioteq_journal *> (widget))
	dynamic_cast<biblioteq_journal *> (widget)->setPublicationDateFormat
	  (m_otherOptions->dateFormat("journals"));
      else if(dynamic_cast<biblioteq_magazine *> (widget))
	dynamic_cast<biblioteq_magazine *> (widget)->setPublicationDateFormat
	  (m_otherOptions->dateFormat("magazines"));
      else if(dynamic_cast<biblioteq_photographcollection *> (widget))
	dynamic_cast<biblioteq_photographcollection *> (widget)->
	  setPublicationDateFormat
	  (m_otherOptions->dateFormat("photographcollections"));
      else if(dynamic_cast<biblioteq_videogame *> (widget))
	dynamic_cast<biblioteq_videogame *> (widget)->setPublicationDateFormat
	  (m_otherOptions->dateFormat("videogames"));
    }

  foreach(auto widget, QApplication::topLevelWidgets())
    if(qobject_cast<biblioteq_book *> (widget))
      qobject_cast<biblioteq_book *> (widget)->setPublicationDateFormat
	(m_otherOptions->dateFormat("books"));
    else if(qobject_cast<biblioteq_cd *> (widget))
      qobject_cast<biblioteq_cd *> (widget)->setPublicationDateFormat
	(m_otherOptions->dateFormat("musiccds"));
    else if(qobject_cast<biblioteq_dvd *> (widget))
      qobject_cast<biblioteq_dvd *> (widget)->setPublicationDateFormat
	(m_otherOptions->dateFormat("dvds"));
    else if(qobject_cast<biblioteq_grey_literature *> (widget))
      qobject_cast<biblioteq_grey_literature *> (widget)->
	setPublicationDateFormat(m_otherOptions->dateFormat("greyliterature"));
    else if(qobject_cast<biblioteq_journal *> (widget))
      qobject_cast<biblioteq_journal *> (widget)->setPublicationDateFormat
	(m_otherOptions->dateFormat("journals"));
    else if(qobject_cast<biblioteq_magazine *> (widget))
      qobject_cast<biblioteq_magazine *> (widget)->setPublicationDateFormat
	(m_otherOptions->dateFormat("magazines"));
    else if(qobject_cast<biblioteq_photographcollection *> (widget))
      qobject_cast<biblioteq_photographcollection *> (widget)->
	setPublicationDateFormat
	(m_otherOptions->dateFormat("photographcollections"));
    else if(qobject_cast<biblioteq_videogame *> (widget))
      qobject_cast<biblioteq_videogame *> (widget)->setPublicationDateFormat
	(m_otherOptions->dateFormat("videogames"));

  for(int i = 0; i < bb.table->columnCount(); i++)
    {
      auto item = bb.table->horizontalHeaderItem(i);

      if(item)
	bb.table->setColumnHidden
	  (i, !m_otherOptions->isMembersColumnVisible(item->text()));
    }

  if(m_otherOptions->showMainTableImages())
    ui.table->setIconSize(QSize(64, 94));
  else
    ui.table->setIconSize(QSize(0, 0));

  QFontMetrics const fontMetrics(ui.table->font());

  for(int i = 0; i < ui.table->rowCount(); i++)
    ui.table->setRowHeight
      (i, qMax(fontMetrics.height() + 10, ui.table->iconSize().height()));

  {
    QHashIterator<QAction *, QPointer<biblioteq_documentationwindow> > it
      (m_documentation);

    while(it.hasNext())
      {
	it.next();
	it.value() ? it.value()->prepareIcons() : (void) 0;
      }
  }

  {
    QHashIterator<QAction *, QPointer<biblioteq_documentationwindow> > it
      (m_releaseNotes);

    while(it.hasNext())
      {
	it.next();
	it.value() ? it.value()->prepareIcons() : (void) 0;
      }
  }

  db_enumerations ? db_enumerations->prepareIcons() : (void) 0;
  m_batchActivities->prepareIcons();
  m_contributors ? m_contributors->prepareIcons() : (void) 0;
  m_files ? m_files->prepareIcons() : (void) 0;
  m_import->prepareIcons();
  m_queryHistory ? m_queryHistory->prepareIcons() : (void) 0;
  m_sqliteMergeDatabases ? m_sqliteMergeDatabases->prepareIcons() : (void) 0;
  prepareCustomQueryFavoriteShortcut();
  prepareIcons();
  prepareStatusBarIcons();
  ui.table->allowUtf8Printable
    (QSettings().value("otheroptions/only_utf8_printable_text", false).
     toBool());
  ui.table->prepareConnections();
  QApplication::restoreOverrideCursor();
  emit otherOptionsSaved();
}

void biblioteq::slotPopulateMembersBrowser(void)
{
  QScopedPointer<QProgressDialog> progress;

  if(m_members_diag->isVisible())
    progress.reset(new QProgressDialog(m_members_diag));
  else
    progress.reset(new QProgressDialog(this));

  QSqlQuery query(m_db);
  QString str = "";
  QTableWidgetItem *item = nullptr;
  auto page = bb.page->currentIndex();
  int i = -1;
  int j = 0;

  {
    /*
    ** Retrieve the number of members.
    */

    QSqlQuery query(m_db);

    disconnect(bb.page,
	       SIGNAL(currentIndexChanged(int)),
	       this,
	       SLOT(slotPopulateMembersBrowser(void)));
    bb.page->clear();
    query.setForwardOnly(true);
    str = "SELECT COUNT(*) FROM member ";

    if(bb.filterBox->isChecked())
      {
	str.append("WHERE ");

	QString E("");

	if(m_db.driverName() != "QSQLITE")
	  E = "E";

	if(bb.filtertype->currentIndex() == 0)
	  str.append("LOWER(last_name) LIKE " + E + "'%' || LOWER(?) || '%'");
	else
	  str.append("LOWER(memberid) LIKE " + E + "'%' || LOWER(?) || '%'");

	query.prepare(str);
	query.bindValue
	  (0, biblioteq_myqstring::escape(bb.filter->text().trimmed()));
      }
    else
      query.prepare(str);

    if(query.exec() && query.next())
      {
	auto const pages = qCeil
	  (query.value(0).toDouble() / static_cast<double> (bb.pages->value()));

	for(int i = 0; i < pages; i++)
	  bb.page->addItem(QString::number(i + 1));
      }

    if(bb.page->count() == 0)
      bb.page->addItem("1");

    page = qBound(0, page, bb.page->count() - 1);
    bb.page->setCurrentIndex(page);
    connect(bb.page,
	    SIGNAL(currentIndexChanged(int)),
	    this,
	    SLOT(slotPopulateMembersBrowser(void)));
  }

  str = "SELECT member.memberid, "
    "member.first_name, "
    "member.last_name, "
    "member.email, "
    "member.telephone_num, "
    "member.street || ' ' || "
    "member.city || ' ' || "
    "member.state_abbr || ' ' || "
    "member.zip AS address, "
    "member.membersince, "
    "member.expiration_date, "
    "member.membership_fees, "
    "member.overdue_fees, "
    "COUNT(DISTINCT ib1.myoid) AS number_reserved_books, "
    "COUNT(DISTINCT ib2.myoid) AS number_reserved_cds, "
    "COUNT(DISTINCT ib3.myoid) AS number_reserved_dvds, "
    "COUNT(DISTINCT ib4.myoid) AS number_reserved_greyliteratures, "
    "COUNT(DISTINCT ib5.myoid) AS number_reserved_journals, "
    "COUNT(DISTINCT ib6.myoid) AS number_reserved_magazines, "
    "COUNT(DISTINCT ib7.myoid) AS number_reserved_videogames, "
    "0 AS number_reserved_total "
    "FROM member member "
    "LEFT JOIN item_borrower ib1 ON "
    "member.memberid = ib1.memberid AND ib1.type = 'Book' "
    "LEFT JOIN item_borrower ib2 ON "
    "member.memberid = ib2.memberid AND ib2.type = 'CD' "
    "LEFT JOIN item_borrower ib3 ON "
    "member.memberid = ib3.memberid AND ib3.type = 'DVD' "
    "LEFT JOIN item_borrower ib4 ON "
    "member.memberid = ib4.memberid AND ib4.type = 'Grey Literature' "
    "LEFT JOIN item_borrower ib5 ON "
    "member.memberid = ib5.memberid AND ib5.type = 'Journal' "
    "LEFT JOIN item_borrower ib6 ON "
    "member.memberid = ib6.memberid AND ib6.type = 'Magazine' "
    "LEFT JOIN item_borrower ib7 ON "
    "member.memberid = ib7.memberid AND ib7.type = 'Video Game' ";

  if(bb.filterBox->isChecked())
    {
      str.append("WHERE ");

      QString E("");

      if(m_db.driverName() != "QSQLITE")
	E = "E";

      if(bb.filtertype->currentIndex() == 0)
	str.append
	  ("LOWER(member.last_name) LIKE " + E + "'%' || LOWER(?) || '%' ");
      else
	str.append
	  ("LOWER(member.memberid) LIKE " + E + "'%' || LOWER(?) || '%' ");
    }

  str.append(" ");
  str.append("GROUP BY "
	     "member.memberid, "
	     "member.first_name, "
	     "member.last_name, "
	     "member.email, "
	     "member.telephone_num, "
	     "address, "
	     "member.membersince, "
	     "member.expiration_date, "
	     "member.membership_fees, "
	     "member.overdue_fees ");
  str.append("ORDER BY member.memberid ");
  str.append(QString("LIMIT %1 OFFSET %2").
	     arg(bb.pages->value()).
	     arg(bb.page->currentIndex() * bb.pages->value()));
  query.prepare(str);

  if(bb.filterBox->isChecked())
    query.bindValue
      (0, biblioteq_myqstring::escape(bb.filter->text().trimmed()));

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!query.exec())
    {
      QApplication::restoreOverrideCursor();
      addError(tr("Database Error"),
	       tr("Unable to retrieve member data for table populating."),
	       query.lastError().text(),
	       __FILE__,
	       __LINE__);
      QMessageBox::critical
	(m_members_diag,
	 tr("BiblioteQ: Database Error"),
	 tr("Unable to retrieve member data for table populating."));
      QApplication::processEvents();
      return;
    }

  QApplication::restoreOverrideCursor();
  resetMembersBrowser();
  bb.table->setSortingEnabled(false);

  if(m_db.driverName() != "QSQLITE")
    bb.table->setRowCount(query.size());
  else
    bb.table->setRowCount
      (biblioteq_misc_functions::sqliteQuerySize(query.lastQuery(),
						 query.boundValues(),
						 m_db,
						 __FILE__,
						 __LINE__,
						 this));

  progress->setLabelText(tr("Populating the table..."));
  progress->setMinimum(0);
  progress->setMinimumWidth
    (qCeil(PROGRESS_DIALOG_WIDTH_MULTIPLIER * progress->sizeHint().width()));
  progress->setModal(true);
  progress->setWindowTitle(tr("BiblioteQ: Progress Dialog"));

  if(m_db.driverName() == "QSQLITE")
    progress->setMaximum
      (biblioteq_misc_functions::sqliteQuerySize(query.lastQuery(),
						 query.boundValues(),
						 m_db,
						 __FILE__,
						 __LINE__,
						 this));
  else
    progress->setMaximum(query.size());

  progress->show();
  progress->repaint();
  QApplication::processEvents();
  i = -1;

  while(i++, !progress->wasCanceled() && query.next())
    {
      if(query.isValid())
	{
	  auto const record(query.record());
	  qint64 total = 0;

	  for(j = 0; j < record.count(); j++)
	    {
	      if(record.fieldName(j).contains("date") ||
		 record.fieldName(j).contains("membersince"))
		{
		  auto const date
		    (QDate::fromString(query.value(j).toString().
				       trimmed(),
				       s_databaseDateFormat));

		  str = date.toString(Qt::ISODate);
		}
	      else
		{
		  str = query.value(j).toString().trimmed();

		  if(record.fieldName(j).startsWith("number_reserved_"))
		    total += str.toLongLong();
		}

	      if(j == record.count() - 1)
		str = QString::number(total);

	      item = new QTableWidgetItem();
	      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	      item->setText(str);
	      bb.table->setItem(i, j, item);
	    }
	}

      if(i + 1 <= progress->maximum())
	progress->setValue(i + 1);

      progress->repaint();
      QApplication::processEvents();
    }

  progress->close();
  bb.table->setSortingEnabled(true);
  bb.table->setRowCount(i); // Support cancellation.

  for(int i = 0; i < bb.table->columnCount() - 1; i++)
    bb.table->resizeColumnToContents(i);

#ifdef Q_OS_MACOS
  bb.table->hide();
  bb.table->show();
#endif
  m_membersWasRefreshed = true;
}

void biblioteq::slotPreviewCanvasBackgroundColor(const QColor &color)
{
  ui.graphicsView->scene()->setBackgroundBrush(color);
}

void biblioteq::slotRefreshAdminList(void)
{
  QScopedPointer<QProgressDialog> progress;

  if(m_admin_diag->isVisible())
    progress.reset(new QProgressDialog(m_admin_diag));
  else
    progress.reset(new QProgressDialog(this));

  if(!progress)
    return;
  else
    progress->hide();

  QCheckBox *checkBox = nullptr;
  QSqlQuery query(m_db);
  QString columnname = "";
  QString str = "";
  QTableWidgetItem *item = nullptr;
  int i = -1;
  int j = 0;

  query.prepare("SELECT username, LOWER(roles) FROM admin ORDER BY username");
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!query.exec())
    {
      QApplication::restoreOverrideCursor();
      addError
	(tr("Database Error"),
	 tr("Unable to retrieve administrator data for table populating."),
	 query.lastError().text(),
	 __FILE__,
	 __LINE__);
      QMessageBox::critical
	(m_admin_diag,
	 tr("BiblioteQ: Database Error"),
	 tr("Unable to retrieve administrator data for table populating."));
      QApplication::processEvents();
      return;
    }

  QApplication::restoreOverrideCursor();
  resetAdminBrowser();
  ab.table->setRowCount(query.size());
  progress->setLabelText(tr("Populating the table..."));
  progress->setMaximum(query.size());
  progress->setMinimum(0);
  progress->setMinimumWidth
    (qCeil(PROGRESS_DIALOG_WIDTH_MULTIPLIER * progress->sizeHint().width()));
  progress->setModal(true);
  progress->setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress->show();
  progress->repaint();
  QApplication::processEvents();
  i = -1;

  while(i++, !progress->wasCanceled() && query.next())
    {
      if(query.isValid())
	{
	  item = new QTableWidgetItem();
	  str = query.value(0).toString().trimmed();
	  item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	  item->setText(str);
	  str = query.value(1).toString().trimmed();
	  ab.table->setItem(i, static_cast<int> (AdminSetupColumns::ID), item);

	  for(j = static_cast<int> (AdminSetupColumns::ADMINISTRATOR);
	      j < ab.table->columnCount();
	      j++)
	    if(query.value(0).toString().trimmed() == getAdminID() &&
	       j > 1)
	      {
		item = new QTableWidgetItem();
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		ab.table->setItem(i, j, item);
	      }
	    else
	      {
		checkBox = new QCheckBox();
		ab.table->setCellWidget(i, j, checkBox);
		columnname = m_abColumnHeaderIndexes.value(j).toLower();

		if(str.contains(columnname, Qt::CaseInsensitive))
		  checkBox->setChecked(true);

		if(getAdminID() == query.value(0).toString().trimmed())
		  checkBox->setEnabled(false);
		else
		  connect(checkBox,
			  SIGNAL(stateChanged(int)),
			  this,
			  SLOT(slotAdminCheckBoxClicked(int)));
	      }
	}

      if(i + 1 <= progress->maximum())
	progress->setValue(i + 1);

      progress->repaint();
      QApplication::processEvents();
    }

  progress->close();
  ab.table->setRowCount(i); // Support cancellation.

  for(int i = 0; i < ab.table->columnCount() - 1; i++)
    ab.table->resizeColumnToContents(i);

  m_deletedAdmins.clear();
}

void biblioteq::slotRemoveMember(void)
{
  auto const row = bb.table->currentRow();

  if(row < 0)
    {
      QMessageBox::critical(m_members_diag,
			    tr("BiblioteQ: User Error"),
			    tr("Please select a member to delete."));
      QApplication::processEvents();
      return;
    }

  QMap<QString, qint64> counts;
  QSqlQuery query(m_db);
  QString errorstr = "";
  QString memberid = "";

  memberid = biblioteq_misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("Member ID"));
  QApplication::setOverrideCursor(Qt::WaitCursor);
  counts = biblioteq_misc_functions::getItemsReservedCounts
    (m_db, memberid, errorstr);
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    {
      addError(tr("Database Error"),
	       tr("Unable to determine the number of items that "
		  "are reserved by the selected member."),
	       errorstr,
	       __FILE__,
	       __LINE__);
      QMessageBox::critical
	(m_members_diag,
	 tr("BiblioteQ: Database Error"),
	 tr("Unable to determine the number of items that "
	    "are reserved by the selected member."));
      QApplication::processEvents();
      return;
    }

  if(counts.value("numtotal") != 0)
    {
      QMessageBox::critical
	(m_members_diag,
	 tr("BiblioteQ: User Error"),
	 tr("You may not remove a member that has reserved items."));
      QApplication::processEvents();
      return;
    }

  if(QMessageBox::question(m_members_diag,
			   tr("BiblioteQ: Question"),
			   tr("Are you sure that you wish to delete the "
			      "selected member (%1)?").arg(memberid),
			   QMessageBox::No | QMessageBox::Yes,
			   QMessageBox::No) == QMessageBox::No)
    {
      QApplication::processEvents();
      return;
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!m_db.transaction())
    {
      QApplication::restoreOverrideCursor();
      addError
	(tr("Database Error"),
	 tr("Unable to create a database transaction."),
	 m_db.lastError().text(),
	 __FILE__,
	 __LINE__);
      QMessageBox::critical
	(m_members_diag,
	 tr("BiblioteQ: Database Error"),
	 tr("Unable to create a database transaction."));
      QApplication::processEvents();
      return;
    }

  QApplication::restoreOverrideCursor();
  query.prepare("DELETE FROM member WHERE memberid = ?");
  query.bindValue(0, memberid);
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!query.exec())
    {
      if(!m_db.rollback())
	addError
	  (tr("Database Error"),
	   tr("Rollback failure."),
	   m_db.lastError().text(),
	   __FILE__,
	   __LINE__);

      QApplication::restoreOverrideCursor();
      addError(tr("Database Error"),
	       tr("Unable to remove the selected member."),
	       query.lastError().text(),
	       __FILE__,
	       __LINE__);
      QMessageBox::critical
	(m_members_diag,
	 tr("BiblioteQ: Database Error"),
	 tr("Unable to remove the selected member."));
      QApplication::processEvents();
    }
  else
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);
      query.prepare("DELETE FROM member_history WHERE memberid = ?");
      query.addBindValue(memberid);
      query.exec();
      QApplication::restoreOverrideCursor();
      biblioteq_misc_functions::DBAccount
	(memberid, m_db, biblioteq_misc_functions::DELETE_USER, errorstr);

      if(!errorstr.isEmpty())
	{
	  addError
	    (tr("Database Error"),
	     tr("Unable to remove the patron account ") +
	     memberid +
	     tr("."),
	     errorstr,
	     __FILE__,
	     __LINE__);

	  if(!m_db.rollback())
	    addError(tr("Database Error"),
		     tr("Rollback failure."),
		     m_db.lastError().text(),
		     __FILE__,
		     __LINE__);

	  QApplication::restoreOverrideCursor();
	  QMessageBox::critical
	    (m_members_diag,
	     tr("BiblioteQ: Database Error"),
	     tr("Unable to remove the patron account ") +
	     memberid +
	     tr("."));
	  QApplication::processEvents();
	}
      else
	{
	  if(!m_db.commit())
	    {
	      addError
		(tr("Database Error"),
		 tr("Unable to commit the current database transaction."),
		 m_db.lastError().text(),
		 __FILE__,
		 __LINE__);
	      m_db.rollback();
	      QApplication::restoreOverrideCursor();
	      QMessageBox::critical
		(m_members_diag,
		 tr("BiblioteQ: Database Error"),
		 tr("Unable to commit the current database transaction."));
	      QApplication::processEvents();
	      return;
	    }
	}

      QApplication::restoreOverrideCursor();
      slotPopulateMembersBrowser();
    }
}

void biblioteq::slotRequest(void)
{
  /*
  ** This method is used for canceling request(s), requesting item(s), and
  ** returning selected item(s).
  */

  auto const list(ui.table->selectionModel()->selectedRows());
  auto const task = static_cast<RequestActionItems>
    (ui.actionRequests->data().toInt());

  if(task == RequestActionItems::CANCEL_REQUESTED)
    {
      if(list.isEmpty())
	{
	  QMessageBox::critical
	    (this,
	     tr("BiblioteQ: User Error"),
	     tr("Please select at least one reservation request to cancel."));
	  QApplication::processEvents();
	  return;
	}

      if(list.size() > 0)
	{
	  if(QMessageBox::question(this,
				   tr("BiblioteQ: Question"),
				   tr("Are you sure that you wish to "
				      "cancel the selected reservation "
				      "request(s)?"),
				   QMessageBox::No | QMessageBox::Yes,
				   QMessageBox::No) == QMessageBox::No)
	    {
	      QApplication::processEvents();
	      return;
	    }

	  QApplication::processEvents();
	}
    }
  else if(task == RequestActionItems::REQUEST_SELECTED)
    {
      if(list.isEmpty())
	{
	  QMessageBox::critical
	    (this,
	     tr("BiblioteQ: User Error"),
	     tr("Please select at least one item to place on request."));
	  QApplication::processEvents();
	  return;
	}
    }
  else if(task == RequestActionItems::RETURN_RESERVED)
    {
      if(list.isEmpty())
	{
	  QMessageBox::critical
	    (this,
	     tr("BiblioteQ: User Error"),
	     tr("Please select at least one item to return."));
	  QApplication::processEvents();
	  return;
	}
    }
  else
    return;

  QProgressDialog progress(this);
  QString itemType("");
  QString oid("");
  QString str("<html>");
  QString title("");
  auto const now(QDateTime::currentDateTime());
  auto error = false;
  int ct = 0;
  int i = 0;
  int numcompleted = 0;

  progress.setCancelButton(nullptr);

  if(task == RequestActionItems::CANCEL_REQUESTED)
    progress.setLabelText
      (tr("Canceling the selected reservation request(s)..."));
  else if(task == RequestActionItems::REQUEST_SELECTED)
    progress.setLabelText(tr("Requesting the selected item(s)..."));
  else
    progress.setLabelText(tr("Returning the selected item(s)..."));

  progress.setMaximum(list.size());
  progress.setMinimum(0);
  progress.setMinimumWidth
    (qCeil(PROGRESS_DIALOG_WIDTH_MULTIPLIER * progress.sizeHint().width()));
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.show();
  progress.repaint();
  QApplication::processEvents();

  foreach(auto const &index, list)
    {
      QSqlQuery query(m_db);

      ct += 1;
      i = index.row();
      itemType = biblioteq_misc_functions::getColumnString
	(ui.table, i, ui.table->columnNumber("Type")).trimmed();
      title = biblioteq_misc_functions::getColumnString
	(ui.table, i, ui.table->columnNumber("Title")).trimmed();
      title = title.isEmpty() ? tr("(unknown title)") : title;

      if(task == RequestActionItems::CANCEL_REQUESTED)
	{
	  oid = biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("REQUESTOID"));

	  if(oid.isEmpty())
	    oid = "-1";
	}
      else
	oid = biblioteq_misc_functions::getColumnString
	  (ui.table, i, ui.table->columnNumber("MYOID"));

      if(itemType != "Photograph Collection")
	{
	  if(task == RequestActionItems::CANCEL_REQUESTED)
	    {
	      query.prepare("DELETE FROM item_request WHERE myoid = ?");
	      query.addBindValue(oid);
	    }
	  else if(task == RequestActionItems::REQUEST_SELECTED)
	    {
	      query.prepare("INSERT INTO item_request "
			    "(item_oid, memberid, requestdate, type) "
			    "VALUES (?, ?, ?, ?)");
	      query.addBindValue(oid);
	      query.addBindValue(dbUserName());
	      query.addBindValue
		(now.toString(s_databaseDateFormat +
			      " " +
			      s_databaseTimeFormat));
	      query.addBindValue(itemType);
	    }
	  else
	    {
	      QString copyid("");

	      copyid = biblioteq_misc_functions::getColumnString
		(ui.table, i, ui.table->columnNumber("Barcode"));
	      query.prepare
		("DELETE FROM item_borrower WHERE "
		 "copyid = ? AND item_oid = ? AND type = ?");
	      query.addBindValue(copyid);
	      query.addBindValue(oid);
	      query.addBindValue(itemType);

	      if(query.exec())
		{
		  QString memberid("");

		  memberid = biblioteq_misc_functions::getColumnString
		    (ui.table, i, ui.table->columnNumber("Member ID"));
		  query.prepare("UPDATE member_history SET returned_date = ? "
				"WHERE copyid = ? AND "
				"item_oid = ? AND "
				"memberid = ?");
		  query.addBindValue(now.toString(s_databaseDateFormat));
		  query.addBindValue(copyid);
		  query.addBindValue(oid);
		  query.addBindValue(memberid);
		  query.exec();
		}

	      if(query.lastError().isValid())
		{
		  addError(tr("Database Error"),
			   tr("Unable to return the item."),
			   query.lastError().text(),
			   __FILE__,
			   __LINE__);
		  error = true;
		}
	      else
		{
		  QString errorstr("");
		  QString member("");
		  auto const identifier
		    (biblioteq_misc_functions::
		     getColumnString(ui.table,
				     i,
				     ui.table->columnNumber("ID Number")));

		  numcompleted += 1;

		  if(biblioteq_misc_functions::isRequested(m_db,
							   oid,
							   itemType,
							   errorstr) ||
		     biblioteq_misc_functions::sqliteReturnReminder(member,
								    m_db,
								    identifier,
								    itemType))
		    str += biblioteq_misc_functions::prettySQLiteReminder
		      (member, title) + "<br>";
		}

	      goto progress_label;
	    }

	  if(!query.exec())
	    {
	      error = true;

	      if(task == RequestActionItems::CANCEL_REQUESTED)
		addError(tr("Database Error"),
			 tr("Unable to cancel the reservation request."),
			 query.lastError().text(),
			 __FILE__,
			 __LINE__);
	      else if(task == RequestActionItems::REQUEST_SELECTED)
		addError(tr("Database Error"),
			 tr("Unable to request the item."),
			 query.lastError().text(),
			 __FILE__,
			 __LINE__);
	    }
	  else
	    numcompleted += 1;
	}

    progress_label:

      if(i + 1 <= progress.maximum())
	progress.setValue(ct);

      progress.repaint();
      QApplication::processEvents();
    }

  progress.close();

  if(error)
    {
      if(task == RequestActionItems::CANCEL_REQUESTED)
	QMessageBox::critical
	  (this,
	   tr("BiblioteQ: Database Error"),
	   tr("Unable to cancel some or all of the selected reservation "
	      "requests."));
      else if(task == RequestActionItems::REQUEST_SELECTED)
	QMessageBox::critical
	  (this,
	   tr("BiblioteQ: Database Error"),
	   tr("Unable to request some or all of the selected items. "
	      "Please verify that you are not attempting to request "
	      "duplicate items."));
      else
      	QMessageBox::critical
	  (this,
	   tr("BiblioteQ: Database Error"),
	   tr("Unable to return some or all of the selected items."));
    }

  QApplication::processEvents();

  if(numcompleted > 0)
    {
      str += "</html>";

      if(str != "<html></html>")
	{
	  QDialog dialog(this);
	  Ui_generalmessagediag ui;

	  ui.setupUi(&dialog);
	  ui.hideForThisSession->setVisible(false);
	  ui.text->setHtml(str);
	  connect(ui.cancelButton,
		  SIGNAL(clicked(void)),
		  &dialog,
		  SLOT(close(void)));
	  dialog.resize(500, 500);
	  dialog.setWindowTitle(tr("BiblioteQ: Information"));
	  dialog.exec();
	  QApplication::processEvents();
	}

      if(task == RequestActionItems::CANCEL_REQUESTED ||
	 task == RequestActionItems::RETURN_RESERVED)
	slotRefresh();
    }
}

void biblioteq::slotSaveAdministrators(void)
{
  QCheckBox *checkBox = nullptr;
  QProgressDialog progress(m_admin_diag);
  QSqlQuery query(m_db);
  QString adminStr = "";
  QString errorstr = "";
  QString str = "";
  QStringList tmplist;
  auto adminCreated = false;
  auto uexists = false;
  int i = 0;
  int j = 0;

  /*
  ** 1. Prohibit duplicate administrator ids and administrators
  **    without privileges.
  ** 2. Create a database transaction.
  ** 3. Delete required entries from the admin table.
  ** 4. Remove all deleted database accounts.
  ** 5. Create new entries in the admin table.
  ** 6. Create new database accounts with correct privileges.
  ** 7. Commit or rollback the current database transaction.
  */

  ab.saveButton->setFocus();
  progress.hide();

  for(i = 0; i < ab.table->rowCount(); i++)
    {
      if(ab.table->item(i, static_cast<int> (AdminSetupColumns::ID))->
	 text().trimmed().isEmpty())
	continue;

      if(!(qobject_cast<QCheckBox *>
	   (ab.table->cellWidget
	    (i, static_cast<int> (AdminSetupColumns::ADMINISTRATOR)))->
	   isChecked() ||
	   qobject_cast<QCheckBox *>
	   (ab.table->cellWidget
	    (i, static_cast<int> (AdminSetupColumns::CIRCULATION)))->
	   isChecked() ||
	   qobject_cast<QCheckBox *>
	   (ab.table->cellWidget
	    (i, static_cast<int> (AdminSetupColumns::LIBRARIAN)))->
	   isChecked() ||
	   qobject_cast<QCheckBox *>
	   (ab.table->cellWidget
	    (i, static_cast<int> (AdminSetupColumns::MEMBERSHIP)))->
	   isChecked()))
	{
	  ab.table->selectRow(i);
	  ab.table->horizontalScrollBar()->setValue(i);
	  QMessageBox::critical
	    (m_admin_diag,
	     tr("BiblioteQ: User Error"),
	     tr("Administrators must belong to at least one category."));
	  QApplication::processEvents();
	  return;
	}

      if(!tmplist.contains
	 (ab.table->item(i, static_cast<int> (AdminSetupColumns::ID))->
	  text().toLower().trimmed()))
	tmplist.append
	  (ab.table->item(i, static_cast<int> (AdminSetupColumns::ID))->
	   text().toLower().trimmed());
      else
	{
	  ab.table->selectRow(i);
	  ab.table->horizontalScrollBar()->setValue(i);
	  QMessageBox::critical
	    (m_admin_diag,
	     tr("BiblioteQ: User Error"),
	     tr("Duplicate administrator ids are not allowed."));
	  QApplication::processEvents();
	  return;
	}
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!m_db.transaction())
    {
      QApplication::restoreOverrideCursor();
      addError
	(tr("Database Error"),
	 tr("Unable to create a database transaction."),
	 m_db.lastError().text(),
	 __FILE__,
	 __LINE__);
      QMessageBox::critical
	(m_admin_diag,
	 tr("BiblioteQ: Database Error"),
	 tr("Unable to create a database transaction."));
      QApplication::processEvents();
      return;
    }

  /*
  ** Remove database accounts.
  */

  for(i = 0; i < m_deletedAdmins.size(); i++)
    {
      query.prepare("DELETE FROM admin WHERE LOWER(username) = LOWER(?)");
      query.bindValue(0, m_deletedAdmins[i].toLower());

      if(!query.exec())
	{
	  QApplication::restoreOverrideCursor();
	  addError(tr("Database Error"),
		   tr("An error occurred while attempting to remove ") +
		   m_deletedAdmins[i].toLower() +
		   tr("."),
		   query.lastError().text(),
		   __FILE__,
		   __LINE__);
	  goto db_rollback;
	}

      biblioteq_misc_functions::DBAccount
	(m_deletedAdmins[i].toLower(),
	 m_db,
	 biblioteq_misc_functions::DELETE_USER,
	 errorstr);

      if(!errorstr.isEmpty())
	{
	  QApplication::restoreOverrideCursor();
	  addError
	    (tr("Database Error"),
	     tr("An error occurred while attempting to "
		"remove the database account ") +
	     m_deletedAdmins[i].toLower() +
	     tr("."),
	     errorstr,
	     __FILE__,
	     __LINE__);
	  goto db_rollback;
	}
    }

  QApplication::restoreOverrideCursor();
  progress.setCancelButton(nullptr);
  progress.setLabelText(tr("Saving administrator information..."));
  progress.setMaximum(ab.table->rowCount());
  progress.setMinimum(0);
  progress.setMinimumWidth
    (qCeil(PROGRESS_DIALOG_WIDTH_MULTIPLIER * progress.sizeHint().width()));
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.show();
  progress.repaint();
  QApplication::processEvents();

  /*
  ** Add or modify administrators.
  */

  for(i = 0; i < ab.table->rowCount(); i++)
    {
      str = "";
      adminStr = ab.table->item(i, static_cast<int> (AdminSetupColumns::ID))->
	text().toLower().trimmed();

      if(i + 1 <= progress.maximum())
	progress.setValue(i + 1);

      progress.repaint();
      QApplication::processEvents();

      if(adminStr.isEmpty())
	continue; // Ignore empty administrator ids.
      else if(adminStr == getAdminID())
	continue; // Ignore current administrator.

      if((qobject_cast<QCheckBox *>
	  (ab.table->cellWidget
	   (i, static_cast<int> (AdminSetupColumns::ADMINISTRATOR))))->
	 isChecked())
	str = "administrator";
      else
	for(j = static_cast<int> (AdminSetupColumns::CIRCULATION);
	    j < ab.table->columnCount();
	    j++)
	  {
	    checkBox = qobject_cast<QCheckBox *> (ab.table->cellWidget(i, j));

	    if(checkBox->isChecked())
	      str += m_abColumnHeaderIndexes.value(j).toLower() +
		" ";
	  }

      str = str.trimmed();

      if(str.isEmpty())
	str = "none";

      uexists = biblioteq_misc_functions::userExists(adminStr, m_db, errorstr);

      if(!errorstr.isEmpty())
	{
	  progress.close();
	  addError
	    (tr("Database Error"),
	     tr("The function biblioteq_misc_functions::userExists() failed "
		"for ") +
	     adminStr +
	     tr("."),
	     errorstr,
	     __FILE__,
	     __LINE__);
	  goto db_rollback;
	}

      if(!uexists)
	{
	  query.prepare("INSERT INTO admin (username, roles) "
			"VALUES (LOWER(?), LOWER(?))");
	  query.bindValue(0, adminStr);
	  query.bindValue(1, str);
	}
      else
	{
	  query.prepare
	    ("UPDATE admin SET roles = LOWER(?), username = LOWER(?) WHERE "
	     "LOWER(username) = LOWER(?)");
	  query.bindValue(0, str);
	  query.bindValue(1, adminStr);
	  query.bindValue(2, adminStr);
	}

      if(!query.exec())
	{
	  progress.close();
	  addError
	    (tr("Database Error"),
	     tr("Unable to create or update the administrator entry for ") +
	     adminStr +
	     tr("."),
	     query.lastError().text(),
	     __FILE__,
	     __LINE__);
	  goto db_rollback;
	}

      if(!uexists)
	{
	  biblioteq_misc_functions::DBAccount
	    (adminStr,
	     m_db,
	     biblioteq_misc_functions::CREATE_USER,
	     errorstr,
	     str);

	  if(!errorstr.isEmpty())
	    {
	      progress.close();
	      addError
		(tr("Database Error"),
		 tr("An error occurred while attempting to "
		    "create a database account for ") +
		 adminStr +
		 tr("."),
		 errorstr,
		 __FILE__,
		 __LINE__);
	      goto db_rollback;
	    }

	  adminCreated = true;
	}
      else
	{
	  biblioteq_misc_functions::revokeAll(adminStr, m_db, errorstr);

	  if(!errorstr.isEmpty())
	    {
	      progress.close();
	      addError
		(tr("Database Error"),
		 tr("An error occurred while attempting to "
		    "revoke privileges from ") +
		 adminStr +
		 tr("."),
		 errorstr,
		 __FILE__,
		 __LINE__);
	      goto db_rollback;
	    }

	  biblioteq_misc_functions::grantPrivs(adminStr, str, m_db, errorstr);

	  if(!errorstr.isEmpty())
	    {
	      progress.close();
	      addError
		(tr("Database Error"),
		 tr("An error occurred while attempting to "
		    "grant privileges to ") +
		 adminStr +
		 tr("."),
		 errorstr,
		 __FILE__,
		 __LINE__);
	      goto db_rollback;
	    }
	}
    }

  progress.close();
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!m_db.commit())
    {
      addError
	(tr("Database Error"),
	 tr("Unable to commit the current database transaction."),
	 m_db.lastError().text(),
	 __FILE__,
	 __LINE__);
      m_db.rollback();
      QApplication::restoreOverrideCursor();
      QMessageBox::critical
	(m_admin_diag,
	 tr("BiblioteQ: Database Error"),
	 tr("Unable to commit the current database transaction."));
      QApplication::processEvents();
      return;
    }

  QApplication::restoreOverrideCursor();
  m_deletedAdmins.clear();

  if(adminCreated)
    {
      QMessageBox::information
	(m_admin_diag,
	 tr("BiblioteQ: Information"),
	 tr("Please notify new administrators that their "
	    "default password has been set to tempPass."));
      QApplication::processEvents();
    }

  slotRefreshAdminList();
  return;

 db_rollback:

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!m_db.rollback())
    addError(tr("Database Error"),
	     tr("Rollback failure."),
	     m_db.lastError().text(),
	     __FILE__,
	     __LINE__);

  QApplication::restoreOverrideCursor();
  QMessageBox::critical
    (m_admin_diag,
     tr("BiblioteQ: Database Error"),
     tr("An error occurred while attempting to save "
	"the administrator information."));
  QApplication::processEvents();
}

void biblioteq::slotSaveConfig(void)
{
  QSettings settings;

  settings.setValue
    ("automatically_populate_admin_list_on_display",
     ui.actionPopulate_Administrator_Browser_Table_on_Display->isChecked());
  settings.setValue
    ("automatically_populate_enum_list_on_display",
     ui.actionPopulate_Database_Enumerations_Browser_on_Display->isChecked());
  settings.setValue
    ("automatically_populate_members_list_on_display",
     ui.actionPopulate_Members_Browser_Table_on_Display->isChecked());
  settings.setValue("automatically_populate_on_create",
		    ui.actionAutoPopulateOnCreation->isChecked());
  settings.setValue("automatically_resize_column_widths",
		    ui.actionAutomatically_Resize_Column_Widths->isChecked());
  settings.setValue("global_font", font().toString());
  settings.setValue("last_category", getTypeFilterString());
  settings.setValue("locale", s_locale);
  settings.setValue("main_splitter_state", ui.splitter->saveState());
#ifndef Q_OS_ANDROID
  settings.setValue("members_window_geometry", m_members_diag->saveGeometry());
#endif
  settings.setValue("populate_table_on_connect",
		    ui.actionPopulateOnStart->isChecked());
  settings.setValue("reset_error_log_on_disconnect",
		    ui.actionResetErrorLogOnDisconnect->isChecked());
  settings.setValue("save_settings_on_exit",
		    ui.actionAutomaticallySaveSettingsOnExit->isChecked());
  settings.setValue("show_table_grid", ui.actionShowGrid->isChecked());

  if(ui.actionPreserveGeometry->isChecked())
    {
      if(!isFullScreen())
	settings.setValue("mainwindow_geometry", saveGeometry());
    }
  else
    settings.remove("mainwindow_geometry");

  if(m_db.isOpen())
    {
      if(m_db.driverName() == "QSQLITE")
	{
	  for(int i = 0; i < ui.menuEntriesPerPage->actions().size(); i++)
	    if(ui.menuEntriesPerPage->actions().at(i)->isChecked())
	      {
		settings.setValue
		  ("sqlite_entries_per_page",
		   ui.menuEntriesPerPage->actions().at(i)->data().toInt());
		break;
	      }
	}
      else
	{
	  for(int i = 0; i < ui.menuEntriesPerPage->actions().size(); i++)
	    if(ui.menuEntriesPerPage->actions().at(i)->isChecked())
	      {
		settings.setValue
		  ("postgresql_entries_per_page",
		   ui.menuEntriesPerPage->actions().at(i)->data().toInt());
		break;
	      }
	}
    }

  for(int i = 0; i < ui.menuPreferredSRUSite->actions().size(); i++)
    if(ui.menuPreferredSRUSite->actions().at(i)->isChecked())
      {
	settings.setValue
	  ("preferred_sru_site",
	   ui.menuPreferredSRUSite->actions().at(i)->text().trimmed());
	break;
      }

  for(int i = 0; i < ui.menuPreferredZ3950Server->actions().size(); i++)
    if(ui.menuPreferredZ3950Server->actions().at(i)->isChecked())
      {
	settings.setValue
	  ("preferred_z3950_site",
	   ui.menuPreferredZ3950Server->actions().at(i)->text().trimmed());
	break;
      }

  for(int i = 0; i < ui.table->friendlyStates().keys().size(); i++)
    settings.setValue
      (ui.table->friendlyStates().keys().at(i),
       ui.table->
       friendlyStates().value(ui.table->friendlyStates().keys().at(i)));

  settings.setValue("tear_off_menus", ui.actionTearOffMenus->isChecked());
}

void biblioteq::slotSaveUser(void)
{
  QSqlQuery query(m_db);
  QString checksum = "";
  QString errorstr = "";
  QString str = "";
  auto const row = bb.table->currentRow();
  auto exists = false;
  int i = 0;

  str = userinfo_diag->m_userinfo.city->text().trimmed();
  userinfo_diag->m_userinfo.city->setText(str);
  str = userinfo_diag->m_userinfo.email->text().trimmed();
  userinfo_diag->m_userinfo.email->setText(str);
  str = userinfo_diag->m_userinfo.firstName->text().trimmed();
  userinfo_diag->m_userinfo.firstName->setText(str);
  str = userinfo_diag->m_userinfo.lastName->text().trimmed();
  userinfo_diag->m_userinfo.lastName->setText(str);
  str = userinfo_diag->m_userinfo.memberid->text().trimmed();
  userinfo_diag->m_userinfo.memberid->setText(str);
  str = userinfo_diag->m_userinfo.middle->text().trimmed();
  userinfo_diag->m_userinfo.middle->setText(str);
  str = userinfo_diag->m_userinfo.street->text().trimmed();
  userinfo_diag->m_userinfo.street->setText(str);
  str = userinfo_diag->m_userinfo.telephoneNumber->text().trimmed();
  userinfo_diag->m_userinfo.telephoneNumber->setText(str);
  str = userinfo_diag->m_userinfo.zip->text().trimmed();
  userinfo_diag->m_userinfo.zip->setText(str);

  if(m_engUserinfoTitle.contains("New"))
    {
      if(userinfo_diag->m_userinfo.memberid->text().length() < 5)
	{
	  QMessageBox::critical
	    (userinfo_diag,
	     tr("BiblioteQ: User Error"),
	     tr("The Member ID must contain at least five characters."));
	  QApplication::processEvents();
	  userinfo_diag->m_userinfo.memberid->setFocus();
	  return;
	}

      QApplication::setOverrideCursor(Qt::WaitCursor);

      auto const uexists = biblioteq_misc_functions::userExists
	(userinfo_diag->m_userinfo.memberid->text(), m_db, errorstr);

      QApplication::restoreOverrideCursor();

      if(uexists)
	{
	  QMessageBox::critical
	    (userinfo_diag,
	     tr("BiblioteQ: User Error"),
	     tr("The Member ID ") +
	     userinfo_diag->m_userinfo.memberid->text() +
	     tr(" already exists."));
	  QApplication::processEvents();
	  userinfo_diag->m_userinfo.memberid->setFocus();
	  return;
	}
    }

  if(userinfo_diag->m_userinfo.city->text().isEmpty())
    {
      QMessageBox::critical(userinfo_diag,
			    tr("BiblioteQ: User Error"),
			    tr("Please provide a valid City."));
      QApplication::processEvents();
      userinfo_diag->m_userinfo.city->setFocus();
      return;
    }

  if(userinfo_diag->m_userinfo.firstName->text().isEmpty())
    {
      QMessageBox::critical(userinfo_diag,
			    tr("BiblioteQ: User Error"),
			    tr("Please provide a valid First Name."));
      QApplication::processEvents();
      userinfo_diag->m_userinfo.firstName->setFocus();
      return;
    }

  if(userinfo_diag->m_userinfo.lastName->text().isEmpty())
    {
      QMessageBox::critical(userinfo_diag,
			    tr("BiblioteQ: User Error"),
			    tr("Please provide a valid Last Name."));
      QApplication::processEvents();
      userinfo_diag->m_userinfo.lastName->setFocus();
      return;
    }

  if(userinfo_diag->m_userinfo.street->text().isEmpty())
    {
      QMessageBox::critical(userinfo_diag,
			    tr("BiblioteQ: User Error"),
			    tr("Please provide a valid Street."));
      QApplication::processEvents();
      userinfo_diag->m_userinfo.street->setFocus();
      return;
    }

  if(userinfo_diag->m_userinfo.zip->text().isEmpty())
    {
      QMessageBox::critical(userinfo_diag,
			    tr("BiblioteQ: User Error"),
			    tr("Please provide a ZIP Code."));
      QApplication::processEvents();
      userinfo_diag->m_userinfo.zip->setFocus();
      return;
    }

  checksum.append(userinfo_diag->m_userinfo.city->text());
  checksum.append
    (userinfo_diag->m_userinfo.dob->date().toString(s_databaseDateFormat));
  checksum.append(userinfo_diag->m_userinfo.firstName->text());
  checksum.append(userinfo_diag->m_userinfo.lastName->text());
  checksum.append(userinfo_diag->m_userinfo.middle->text());
  checksum.append(userinfo_diag->m_userinfo.sex->currentText());
  checksum.append(userinfo_diag->m_userinfo.state->currentText());
  checksum.append(userinfo_diag->m_userinfo.street->text());
  checksum.append(userinfo_diag->m_userinfo.zip->text());
  QApplication::setOverrideCursor(Qt::WaitCursor);
  exists = biblioteq_misc_functions::getMemberMatch
    (checksum,
     userinfo_diag->m_userinfo.memberid->text(),
     m_db,
     errorstr);
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    {
      addError(tr("Database Error"),
	       tr("Unable to determine the uniqueness of the "
		  "proposed member."),
	       errorstr,
	       __FILE__,
	       __LINE__);
      QMessageBox::critical
	(userinfo_diag,
	 tr("BiblioteQ: Database Error"),
	 tr("Unable to determine the uniqueness of the proposed member."));
      QApplication::processEvents();
      return;
    }

  if(exists)
    {
      QMessageBox::critical(userinfo_diag,
			    tr("BiblioteQ: User Error"),
			    tr("An identical member already exists."));
      QApplication::processEvents();
      return;
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!m_db.transaction())
    {
      QApplication::restoreOverrideCursor();
      addError
	(tr("Database Error"),
	 tr("Unable to create a database transaction."),
	 m_db.lastError().text(),
	 __FILE__,
	 __LINE__);
      QMessageBox::critical
	(userinfo_diag,
	 tr("BiblioteQ: Database Error"),
	 tr("Unable to create a database transaction."));
      QApplication::processEvents();
      return;
    }

  QApplication::restoreOverrideCursor();

  if(m_engUserinfoTitle.contains("New"))
    {
      query.prepare("INSERT INTO member "
		    "(memberid, "
		    "membersince, "
		    "dob, "
		    "sex, "
		    "first_name, "
		    "middle_init, "
		    "last_name, "
		    "telephone_num, "
		    "street, "
		    "city, "
		    "state_abbr, "
		    "zip, "
		    "email, "
		    "expiration_date, "
		    "overdue_fees, "
		    "comments, "
		    "general_registration_number, "
		    "memberclass, "
		    "maximum_reserved_books, "
		    "membership_fees) "
		    "VALUES "
		    "(?, "
		    "?, "
		    "?, "
		    "?, "
		    "?, "
		    "?, "
		    "?, "
		    "?, "
		    "?, "
		    "?, "
		    "?, "
		    "?, "
		    "?, "
		    "?, "
		    "?, "
		    "?, "
		    "?, "
		    "?, "
		    "?, "
		    "?)");
      query.bindValue(0, userinfo_diag->m_userinfo.memberid->text().trimmed());
      query.bindValue(1, userinfo_diag->m_userinfo.membersince->
		      date().toString(s_databaseDateFormat));
      query.bindValue(2, userinfo_diag->m_userinfo.dob->date().
		      toString(s_databaseDateFormat));
      query.bindValue(3, userinfo_diag->m_userinfo.sex->currentText());
      query.bindValue(4, userinfo_diag->m_userinfo.firstName->text().trimmed());
      query.bindValue(5, userinfo_diag->m_userinfo.middle->text().trimmed());
      query.bindValue(6, userinfo_diag->m_userinfo.lastName->text().trimmed());
      query.bindValue(7, userinfo_diag->m_userinfo.telephoneNumber->text());
      query.bindValue(8, userinfo_diag->m_userinfo.street->text().trimmed());
      query.bindValue(9, userinfo_diag->m_userinfo.city->text().trimmed());
      query.bindValue(10, userinfo_diag->m_userinfo.state->currentText());
      query.bindValue(11, userinfo_diag->m_userinfo.zip->text());
      query.bindValue(12, userinfo_diag->m_userinfo.email->text().trimmed());
      query.bindValue(13, userinfo_diag->m_userinfo.expirationdate->
		      date().toString(s_databaseDateFormat));
      query.bindValue(14, userinfo_diag->m_userinfo.overduefees->value());
      query.bindValue
	(15, userinfo_diag->m_userinfo.comments->toPlainText().trimmed());
      query.bindValue
	(16, userinfo_diag->m_userinfo.generalregistrationnumber->text().
	 trimmed());
      query.bindValue(17, userinfo_diag->m_userinfo.memberclass->text().
		      trimmed());
      query.bindValue
	(18, userinfo_diag->m_userinfo.maximum_reserved_books->value());
      query.bindValue
	(19, userinfo_diag->m_userinfo.membershipfees->value());
    }
  else
    {
      query.prepare("UPDATE member SET "
		    "membersince = ?, "
		    "dob = ?, "
		    "sex = ?, "
		    "first_name = ?, "
		    "middle_init = ?, "
		    "last_name = ?, "
		    "telephone_num = ?, "
		    "street = ?, "
		    "city = ?, "
		    "state_abbr = ?, "
		    "zip = ?, "
		    "email = ?, "
		    "expiration_date = ?, "
		    "overdue_fees = ?, "
		    "comments = ?, "
		    "general_registration_number = ?, "
		    "memberclass = ?, "
		    "maximum_reserved_books = ?, "
		    "membership_fees = ? "
		    "WHERE memberid = ?");
      query.bindValue(0, userinfo_diag->m_userinfo.membersince->date().
		      toString(s_databaseDateFormat));
      query.bindValue(1, userinfo_diag->m_userinfo.dob->date().
		      toString(s_databaseDateFormat));
      query.bindValue(2, userinfo_diag->m_userinfo.sex->currentText());
      query.bindValue
	(3, userinfo_diag->m_userinfo.firstName->text().trimmed());
      query.bindValue(4, userinfo_diag->m_userinfo.middle->text().trimmed());
      query.bindValue(5, userinfo_diag->m_userinfo.lastName->text().trimmed());
      query.bindValue(6, userinfo_diag->m_userinfo.telephoneNumber->text());
      query.bindValue(7, userinfo_diag->m_userinfo.street->text().trimmed());
      query.bindValue(8, userinfo_diag->m_userinfo.city->text().trimmed());
      query.bindValue(9, userinfo_diag->m_userinfo.state->currentText());
      query.bindValue(10, userinfo_diag->m_userinfo.zip->text());
      query.bindValue(11, userinfo_diag->m_userinfo.email->text().trimmed());
      query.bindValue(12, userinfo_diag->m_userinfo.expirationdate->
		      date().toString(s_databaseDateFormat));
      query.bindValue(13, userinfo_diag->m_userinfo.overduefees->value());
      query.bindValue(14, userinfo_diag->m_userinfo.comments->toPlainText().
		      trimmed());
      query.bindValue(15, userinfo_diag->m_userinfo.generalregistrationnumber->
		      text().trimmed());
      query.bindValue(16, userinfo_diag->m_userinfo.memberclass->text().
		      trimmed());
      query.bindValue
	(17, userinfo_diag->m_userinfo.maximum_reserved_books->value());
      query.bindValue
	(18, userinfo_diag->m_userinfo.membershipfees->value());
      query.bindValue
	(19, userinfo_diag->m_userinfo.memberid->text().trimmed());
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!query.exec())
    {
      if(!m_db.rollback())
	addError
	  (tr("Database Error"),
	   tr("Rollback failure."),
	   m_db.lastError().text(),
	   __FILE__,
	   __LINE__);

      QApplication::restoreOverrideCursor();
      addError(tr("Database Error"),
	       tr("Unable to save the member's information."),
	       query.lastError().text(),
	       __FILE__,
	       __LINE__);
      QMessageBox::critical(userinfo_diag,
			    tr("BiblioteQ: Database Error"),
			    tr("Unable to save the member's information."));
      QApplication::processEvents();
    }
  else
    {
      if(m_engUserinfoTitle.contains("New"))
	{
	  /*
	  ** Create a database account for the new member.
	  */

	  biblioteq_misc_functions::DBAccount
	    (userinfo_diag->m_userinfo.memberid->text(),
	     m_db,
	     biblioteq_misc_functions::CREATE_USER,
	     errorstr);

	  if(!errorstr.isEmpty())
	    {
	      if(!m_db.rollback())
		addError
		  (tr("Database Error"),
		   tr("Rollback failure."),
		   m_db.lastError().text(),
		   __FILE__,
		   __LINE__);

	      QApplication::restoreOverrideCursor();
	      addError
		(tr("Database Error"),
		 tr("An error occurred while attempting to "
		    "create a database account for the new member."),
		 errorstr,
		 __FILE__,
		 __LINE__);
	      QMessageBox::critical
		(userinfo_diag,
		 tr("BiblioteQ: Database Error"),
		 tr("An error occurred while attempting to "
		    "create a database account for the new member."));
	      QApplication::processEvents();
	      return;
	    }
	  else
	    {
	      if(!m_db.commit())
		{
		  addError
		    (tr("Database Error"),
		     tr("Unable to commit the current database transaction."),
		     m_db.lastError().text(),
		     __FILE__,
		     __LINE__);
		  m_db.rollback();
		  QApplication::restoreOverrideCursor();
		  QMessageBox::critical(userinfo_diag,
					tr("BiblioteQ: Database Error"),
					tr("Unable to commit the current "
					   "database transaction."));
		  QApplication::processEvents();
		  return;
		}
	    }
	}
      else
	{
	  /*
	  ** Update privileges.
	  */

	  biblioteq_misc_functions::DBAccount
	    (userinfo_diag->m_userinfo.memberid->text(),
	     m_db,
	     biblioteq_misc_functions::UPDATE_USER,
	     errorstr);

	  if(errorstr.trimmed().contains("not exist"))
	    /*
	    ** Attempt to create the account.
	    */

	    biblioteq_misc_functions::DBAccount
	      (userinfo_diag->m_userinfo.memberid->text(),
	       m_db,
	       biblioteq_misc_functions::CREATE_USER,
	       errorstr);

	  if(!errorstr.isEmpty())
	    {
	      if(!m_db.rollback())
		addError
		  (tr("Database Error"),
		   tr("Rollback failure."),
		   m_db.lastError().text(),
		   __FILE__,
		   __LINE__);

	      QApplication::restoreOverrideCursor();
	      addError(tr("Database Error"),
		       tr("An error occurred while attempting to "
			  "update the database account for ") +
		       userinfo_diag->m_userinfo.memberid->text() +
		       tr("."),
		       errorstr,
		       __FILE__,
		       __LINE__);
	      QMessageBox::critical
		(userinfo_diag,
		 tr("BiblioteQ: Database Error"),
		 tr("An error occurred while attempting "
		    "to update the database account %1.").
		 arg(userinfo_diag->m_userinfo.memberid->text()));
	      QApplication::processEvents();
	      return;
	    }
	  else
	    {
	      if(!m_db.commit())
		{
		  addError
		    (tr("Database Error"),
		     tr("Unable to commit the current database transaction."),
		     m_db.lastError().text(),
		     __FILE__,
		     __LINE__);
		  m_db.rollback();
		  QApplication::restoreOverrideCursor();
		  QMessageBox::critical(userinfo_diag,
					tr("BiblioteQ: Database Error"),
					tr("Unable to commit the current "
					   "database transaction."));
		  QApplication::processEvents();
		  return;
		}
	    }
	}

      QApplication::restoreOverrideCursor();
      userinfo_diag->m_userinfo.okButton->animate(2500);
      userinfo_diag->m_memberProperties["city"] =
	userinfo_diag->m_userinfo.city->text().trimmed();
      userinfo_diag->m_memberProperties["comments"] =
	userinfo_diag->m_userinfo.comments->toPlainText().trimmed();
      userinfo_diag->m_memberProperties["dob"] =
	userinfo_diag->m_userinfo.dob->date().toString(Qt::ISODate);
      userinfo_diag->m_memberProperties["email"] =
	userinfo_diag->m_userinfo.email->text().trimmed();
      userinfo_diag->m_memberProperties["expiration_date"] =
	userinfo_diag->m_userinfo.expirationdate->date().toString(Qt::ISODate);
      userinfo_diag->m_memberProperties["first_name"] =
	userinfo_diag->m_userinfo.firstName->text().trimmed();
      userinfo_diag->m_memberProperties["general_registration_number"] =
	userinfo_diag->m_userinfo.generalregistrationnumber->text().trimmed();
      userinfo_diag->m_memberProperties["last_name"] =
	userinfo_diag->m_userinfo.lastName->text().trimmed();
      userinfo_diag->m_memberProperties["maximum_reserved_books"] =
	userinfo_diag->m_userinfo.maximum_reserved_books->text();
      userinfo_diag->m_memberProperties["memberclass"] =
	userinfo_diag->m_userinfo.memberclass->text().trimmed();
      userinfo_diag->m_memberProperties["membership_fees"] =
	userinfo_diag->m_userinfo.membershipfees->text();
      userinfo_diag->m_memberProperties["membersince"] =
	userinfo_diag->m_userinfo.membersince->date().toString(Qt::ISODate);
      userinfo_diag->m_memberProperties["middle_init"] =
	userinfo_diag->m_userinfo.middle->text().trimmed();
      userinfo_diag->m_memberProperties["overdue_fees"] =
	userinfo_diag->m_userinfo.overduefees->text();
      userinfo_diag->m_memberProperties["sex"] =
	userinfo_diag->m_userinfo.sex->currentText();
      userinfo_diag->m_memberProperties["state_abbr"] =
	userinfo_diag->m_userinfo.state->currentText();
      userinfo_diag->m_memberProperties["street"] =
	userinfo_diag->m_userinfo.street->text().trimmed();
      userinfo_diag->m_memberProperties["telephone_num"] =
	userinfo_diag->m_userinfo.telephoneNumber->text();
      userinfo_diag->m_memberProperties["zip"] =
	userinfo_diag->m_userinfo.zip->text();

      if(m_engUserinfoTitle.contains("Modify"))
	{
	  bb.table->setSortingEnabled(false);

	  for(i = 0; i < m_bbColumnHeaderIndexes.size(); i++)
	    {
	      if(!bb.table->item(row, i))
		continue;

	      if(m_bbColumnHeaderIndexes.at(i) == "E-Mail Address")
		bb.table->item(row, i)->setText
		  (userinfo_diag->m_userinfo.email->text());
	      else if(m_bbColumnHeaderIndexes.at(i) == "Expiration Date")
		bb.table->item(row, i)->setText
		  (userinfo_diag->m_userinfo.expirationdate->
		   date().toString(Qt::ISODate));
	      else if(m_bbColumnHeaderIndexes.at(i) == "First Name")
		bb.table->item(row, i)->setText
		  (userinfo_diag->m_userinfo.firstName->text());
	      else if(m_bbColumnHeaderIndexes.at(i) == "Home Address")
		bb.table->item(row, i)->setText
		  ((userinfo_diag->m_userinfo.street->text() + " " +
		    userinfo_diag->m_userinfo.city->text() + " " +
		    userinfo_diag->m_userinfo.state->currentText() + " " +
		    userinfo_diag->m_userinfo.zip->text()).trimmed());
	      else if(m_bbColumnHeaderIndexes.at(i) == "Last Name")
		bb.table->item(row, i)->setText
		  (userinfo_diag->m_userinfo.lastName->text());
	      else if(m_bbColumnHeaderIndexes.at(i) == "Member Since")
		bb.table->item(row, i)->setText
		  (userinfo_diag->m_userinfo.membersince->date().
		   toString(Qt::ISODate));
	      else if(m_bbColumnHeaderIndexes.at(i) == "Membership Fees")
		bb.table->item(row, i)->setText
		  (userinfo_diag->m_userinfo.membershipfees->text());
	      else if(m_bbColumnHeaderIndexes.at(i) == "Overdue Fees")
		bb.table->item(row, i)->setText
		  (userinfo_diag->m_userinfo.overduefees->text());
	      else if(m_bbColumnHeaderIndexes.at(i) == "Telephone Number")
		bb.table->item(row, i)->setText
		  (userinfo_diag->m_userinfo.telephoneNumber->text());
	    }

	  bb.table->setSortingEnabled(true);
	}
      else
	{
#ifdef Q_OS_ANDROID
	  userinfo_diag->hide();
#else
	  userinfo_diag->close();
#endif

	  if(m_db.driverName() != "QSQLITE")
	    {
	      QMessageBox::information
		(m_members_diag,
		 tr("BiblioteQ: Information"),
		 tr("Please notify the new member that their "
		    "default password has been set to tempPass."));
	      QApplication::processEvents();
	    }

	  slotPopulateMembersBrowser();
	}
    }
}

void biblioteq::slotSceneSelectionChanged(void)
{
  if(ui.stackedWidget->currentIndex() != 0)
    return;

  ui.table->clearSelection();
  ui.table->setCurrentCell(-1, -1);
  slotDisplaySummary();

  auto const items(ui.graphicsView->scene()->selectedItems());

  if(!items.isEmpty())
    {
      QGraphicsItem *item = nullptr;
      QMap<QString, char> oids;
      QMap<QString, char> types;

      for(int i = 0; i < items.size(); i++)
	if((item = items.at(i)))
	  {
	    oids[item->data(0).toString()] = 0;
	    types[item->data(1).toString()] = 0;
	  }

      auto const column1 = ui.table->columnNumber("MYOID");
      auto const column2 = ui.table->columnNumber("Type");

      for(int i = 0; i < ui.table->rowCount(); i++)
	if(ui.table->item(i, column1) && ui.table->item(i, column2))
	  if(oids.
	     contains(ui.table->item(i, column1)->text()) &&
	     types.contains
	     (ui.table->item(i, column2)->text().remove(' ').toLower()))
	    ui.table->selectRow(i);
    }
}

void biblioteq::slotShowHistory(void)
{
  QScopedPointer<QProgressDialog> progress;

  if(m_history_diag->isVisible())
    progress.reset(new QProgressDialog(m_history_diag));
  else if(m_members_diag->isVisible())
    progress.reset(new QProgressDialog(m_members_diag));
  else
    progress.reset(new QProgressDialog(this));

  QSqlQuery query(m_db);
  QString errorstr("");
  QString memberid("");
  QString querystr = "";
  QString str = "";
  QStringList list;
  QTableWidgetItem *item = nullptr;
  auto const row = bb.table->currentRow();
  int i = -1;
  int j = 0;

  if(m_db.driverName() == "QPSQL" && m_roles.isEmpty())
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);

      auto const dnt = biblioteq_misc_functions::dnt
	(m_db, dbUserName(), errorstr);

      disconnect(history.dnt,
		 SIGNAL(toggled(bool)),
		 this,
		 SLOT(slotSaveDnt(bool)));

      if(errorstr.isEmpty())
	{
	  history.dnt->setChecked(dnt);
	  history.dnt->setEnabled(true);
	  history.dnt->setToolTip("");
	}
      else
	{
	  history.dnt->setChecked(true);
	  history.dnt->setEnabled(true);
	  history.dnt->setToolTip
	    (tr("An error occurred with biblioteq_misc_functions::dnt()."));
	}

      connect(history.dnt,
	      SIGNAL(toggled(bool)),
	      this,
	      SLOT(slotSaveDnt(bool)));
      QApplication::restoreOverrideCursor();
    }
  else
    {
      disconnect(history.dnt,
		 SIGNAL(toggled(bool)),
		 this,
		 SLOT(slotSaveDnt(bool)));

      if(m_db.driverName() == "QPSQL")
	{
	  history.dnt->setChecked(true);
	  history.dnt->setEnabled(false);
	  history.dnt->setToolTip
	    (tr("The option is only available for PostgreSQL patrons."));
	}
      else
	{
	  history.dnt->setChecked
	    (biblioteq_misc_functions::dnt(m_db, dbUserName(), errorstr));
	  history.dnt->setEnabled(true);
	  history.dnt->setToolTip("");
	}

      connect(history.dnt,
	      SIGNAL(toggled(bool)),
	      this,
	      SLOT(slotSaveDnt(bool)));
    }

  if(m_members_diag->isVisible())
    if(row < 0)
      {
	QMessageBox::critical
	  (m_members_diag,
	   tr("BiblioteQ: User Error"),
	   tr("In order to display a member's reservation "
	      "history, you must first select the member."));
	QApplication::processEvents();
	return;
      }

  list << "book"
       << "cd"
       << "dvd"
       << "grey_literature"
       << "journal"
       << "magazine"
       << "videogame";

  if(!m_roles.isEmpty())
    memberid = biblioteq_misc_functions::getColumnString
      (bb.table, row, m_bbColumnHeaderIndexes.indexOf("Member ID")).trimmed();
  else
    memberid = dbUserName().trimmed();

  if(!m_roles.isEmpty())
    for(i = 0; i < list.size(); i++)
      {
	if(list[i] != "book")
	  {
	    if(list[i] == "grey_literature")
	      querystr += "SELECT "
		"history.memberid, "
		"member.first_name, "
		"member.last_name, "
		"grey_literature.document_title, "
		"grey_literature.document_id, "
		"history.copyid, "
		"grey_literature.type, "
		"history.reserved_date, "
		"history.duedate, "
		"history.returned_date, "
		"history.reserved_by, "
		"grey_literature.myoid "
		"FROM member_history history, "
		"grey_literature grey_literature, "
		"member member "
		"WHERE history.memberid = member.memberid AND "
		"grey_literature.myoid = history.item_oid AND "
		"member.memberid = ? AND grey_literature.type = "
		"history.type ";
	    else
	      querystr += QString
		("SELECT "
		 "history.memberid, "
		 "member.first_name, "
		 "member.last_name, "
		 "%1.title, "
		 "%1.id, "
		 "history.copyid, "
		 "%1.type, "
		 "history.reserved_date, "
		 "history.duedate, "
		 "history.returned_date, "
		 "history.reserved_by, "
		 "%1.myoid "
		 "FROM member_history history, "
		 "%1 %1, "
		 "member member "
		 "WHERE history.memberid = member.memberid AND "
		 "%1.myoid = history.item_oid AND "
		 "member.memberid = ? AND %1.type = "
		 "history.type ").arg(list[i]);
	  }
	else
	  {
	    if(m_db.driverName() != "QSQLITE")
	      querystr += QString
		("SELECT "
		 "history.memberid, "
		 "member.first_name, "
		 "member.last_name, "
		 "book.title, "
		 "book.id, "
		 "history.copyid, "
		 "book.type, "
		 "history.reserved_date, "
		 "history.duedate, "
		 "history.returned_date, "
		 "history.reserved_by, "
		 "book.myoid "
		 "FROM member_history history, "
		 "book book, "
		 "member member "
		 "WHERE history.memberid = member.memberid AND "
		 "book.myoid = history.item_oid AND "
		 "member.memberid = ? AND book.type = "
		 "history.type ");
	    else
	      querystr += QString
		("SELECT "
		 "history.memberid, "
		 "member.first_name, "
		 "member.last_name, "
		 "book.title, "
		 "book.id, "
		 "history.copyid, "
		 "book.type, "
		 "history.reserved_date, "
		 "history.duedate, "
		 "history.returned_date, "
		 "history.reserved_by, "
		 "book.myoid "
		 "FROM member_history history, "
		 "book book, "
		 "member member "
		 "WHERE history.memberid = member.memberid AND "
		 "book.myoid = history.item_oid AND "
		 "member.memberid = ? AND book.type = "
		 "history.type ");
	  }

	if(i != list.size() - 1)
	  querystr += "UNION ALL ";
      }
  else
    for(i = 0; i < list.size(); i++)
      {
	if(list[i] != "book")
	  {
	    if(list[i] == "grey_literature")
	      querystr += "SELECT "
		"history.memberid, "
		"grey_literature.document_title, "
		"grey_literature.document_id, "
		"history.copyid, "
		"grey_literature.type, "
		"history.reserved_date, "
		"history.duedate, "
		"history.returned_date, "
		"history.reserved_by, "
		"grey_literature.myoid "
		"FROM member_history history, "
		"grey_literature grey_literature "
		"WHERE history.memberid = ? AND "
		"grey_literature.myoid = history.item_oid AND "
		"grey_literature.type = history.type ";
	    else
	      querystr += QString
		("SELECT "
		 "history.memberid, "
		 "%1.title, "
		 "%1.id, "
		 "history.copyid, "
		 "%1.type, "
		 "history.reserved_date, "
		 "history.duedate, "
		 "history.returned_date, "
		 "history.reserved_by, "
		 "%1.myoid "
		 "FROM member_history history, "
		 "%1 %1 "
		 "WHERE history.memberid = ? AND "
		 "%1.myoid = history.item_oid AND %1.type = "
		 "history.type ").arg(list[i]);
	  }
	else
	  {
	    if(m_db.driverName() != "QSQLITE")
	      querystr += QString
		("SELECT "
		 "history.memberid, "
		 "book.title, "
		 "book.id, "
		 "history.copyid, "
		 "book.type, "
		 "history.reserved_date, "
		 "history.duedate, "
		 "history.returned_date, "
		 "history.reserved_by, "
		 "book.myoid "
		 "FROM member_history history, "
		 "book book "
		 "WHERE history.memberid = ? AND "
		 "book.myoid = history.item_oid AND book.type = "
		 "history.type ");
	    else
	      querystr += QString
		("SELECT "
		 "history.memberid, "
		 "book.title, "
		 "book.id, "
		 "history.copyid, "
		 "book.type, "
		 "history.reserved_date, "
		 "history.duedate, "
		 "history.returned_date, "
		 "history.reserved_by, "
		 "book.myoid "
		 "FROM member_history history, "
		 "book book "
		 "WHERE history.memberid = ? AND "
		 "book.myoid = history.item_oid AND book.type = "
		 "history.type ");
	  }

	if(i != list.size() - 1)
	  querystr += "UNION ALL ";
      }

  querystr.append("ORDER BY 1");
  query.prepare(querystr);

  /*
  ** The number of bound values should equal the size of list.
  */

  for(int i = 0; i < list.size(); i++)
    query.addBindValue(memberid);

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!query.exec())
    {
      progress->close();
      QApplication::restoreOverrideCursor();
      addError
	(tr("Database Error"),
	 tr("Unable to retrieve reservation history data for table "
	    "populating."),
	 query.lastError().text(),
	 __FILE__,
	 __LINE__);

      if(m_history_diag->isVisible())
	QMessageBox::critical
	  (m_history_diag,
	   tr("BiblioteQ: Database Error"),
	   tr("Unable to retrieve reservation "
	      "history data for table populating."));
      else if(m_members_diag->isVisible())
	QMessageBox::critical
	  (m_members_diag,
	   tr("BiblioteQ: Database Error"),
	   tr("Unable to retrieve reservation "
	      "history data for table populating."));
      else
	QMessageBox::critical
	  (this,
	   tr("BiblioteQ: Database Error"),
	   tr("Unable to retrieve reservation "
	      "history data for table populating."));

      QApplication::processEvents();
      return;
    }

  QApplication::restoreOverrideCursor();
  history.table->setColumnCount(0);
  history.table->setCurrentItem(nullptr);
  history.table->setRowCount(0);
  list.clear();
  list.append(tr("Member ID"));

  if(!m_roles.isEmpty())
    {
      list.append(tr("First Name"));
      list.append(tr("Last Name"));
    }

  list.append(tr("Title"));
  list.append(tr("ID Number"));
  list.append(tr("Barcode"));
  list.append(tr("Type"));
  list.append(tr("Reservation Date"));
  list.append(tr("Original Due Date"));
  list.append(tr("Returned Date"));
  list.append(tr("Lender"));
  list.append("MYOID");
  m_historyColumnHeaderIndexes.clear();
  m_historyColumnHeaderIndexes.append("Title");
  m_historyColumnHeaderIndexes.append("ID Number");
  m_historyColumnHeaderIndexes.append("Barcode");
  m_historyColumnHeaderIndexes.append("Type");
  m_historyColumnHeaderIndexes.append("Reservation Date");
  m_historyColumnHeaderIndexes.append("Original Due Date");
  m_historyColumnHeaderIndexes.append("Returned Date");
  m_historyColumnHeaderIndexes.append("Lender");
  m_historyColumnHeaderIndexes.append("MYOID");
  history.table->setColumnCount(list.size());
  history.table->setColumnHidden(history.table->columnCount() - 1, true);
  history.table->setHorizontalHeaderLabels(list);
  history.table->setSortingEnabled(false);

  if(m_db.driverName() != "QSQLITE")
    history.table->setRowCount(query.size());
  else
    history.table->setRowCount
      (biblioteq_misc_functions::sqliteQuerySize(query.lastQuery(),
						 query.boundValues(),
						 m_db,
						 __FILE__,
						 __LINE__,
						 this));

  history.table->horizontalScrollBar()->setValue(0);
  history.table->scrollToTop();
  progress->setLabelText(tr("Populating the table..."));
  progress->setMaximum(history.table->rowCount());
  progress->setMinimum(0);
  progress->setMinimumWidth
    (qCeil(PROGRESS_DIALOG_WIDTH_MULTIPLIER * progress->sizeHint().width()));
  progress->setModal(true);
  progress->setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress->show();
  progress->repaint();
  QApplication::processEvents();
  i = -1;

  while(i++, !progress->wasCanceled() && query.next())
    {
      if(query.isValid())
	{
	  auto const record(query.record());

	  for(j = 0; j < record.count(); j++)
	    {
	      if(record.fieldName(j).contains("date"))
		{
		  auto const date
		    (QDate::fromString(query.value(j).toString().trimmed(),
				       s_databaseDateFormat));

		  str = date.toString(Qt::ISODate);
		}
	      else
		str = query.value(j).toString().trimmed();

	      item = new QTableWidgetItem();
	      item->setText(str);
	      history.table->setItem(i, j, item);
	    }
	}

      if(i + 1 <= progress->maximum())
	progress->setValue(i + 1);

      progress->repaint();
      QApplication::processEvents();
    }

  progress->close();
  history.table->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  history.table->setRowCount(i); // Support cancellation.
  history.table->setSortingEnabled(true);

  for(int i = 0; i < history.table->columnCount() - 1; i++)
    history.table->resizeColumnToContents(i);

  history.nextTool->setVisible(!m_roles.isEmpty());
  history.prevTool->setVisible(!m_roles.isEmpty());

#ifndef Q_OS_ANDROID
  if(m_members_diag->isVisible())
    {
      auto static resized = false;

      if(!resized)
	{
	  m_history_diag->resize
	    (qRound(0.95 * m_members_diag->size().width()),
	     qRound(0.95 * m_members_diag->size().height()));
	  biblioteq_misc_functions::center(m_history_diag, m_members_diag);
	}

      resized = true;
    }
  else
    {
      auto static resized = false;

      if(!resized)
	{
	  m_history_diag->resize
	    (qRound(0.95 * size().width()), qRound(0.95 * size().height()));
	  biblioteq_misc_functions::center(m_history_diag, this);
	}

      resized = true;
    }
#endif

  m_history_diag->setWindowTitle
    (memberid.isEmpty() ?
     tr("BiblioteQ: Member's Reservation History") :
     tr("BiblioteQ: Member's Reservation History (%1)").arg(memberid));
#ifdef Q_OS_ANDROID
  m_history_diag->showMaximized();
#else
  m_history_diag->showNormal();
#endif
  m_history_diag->activateWindow();
  m_history_diag->raise();
}

void biblioteq::slotShowImport(void)
{
  m_import->show(this);
}

void biblioteq::slotShowOtherOptions(void)
{
#ifdef Q_OS_ANDROID
  m_otherOptions->showMaximized();
#else
  biblioteq_misc_functions::center(m_otherOptions, this, false);
  m_otherOptions->showNormal();
#endif
  m_otherOptions->activateWindow();
  m_otherOptions->raise();
}

void biblioteq::slotShowPassword(bool state)
{
  if(state)
    br.password->setEchoMode(QLineEdit::Normal);
  else
    br.password->setEchoMode(QLineEdit::Password);
}

void biblioteq::slotVacuum(void)
{
  if(QMessageBox::question(this,
			   tr("BiblioteQ: Question"),
			   tr("Vacuuming a database may require a "
			      "significant amount of time to complete. "
			      "Continue?"),
			   QMessageBox::No | QMessageBox::Yes,
			   QMessageBox::No) == QMessageBox::No)
    {
      QApplication::processEvents();
      return;
    }

  QApplication::processEvents();

  QProgressDialog progress(this);

  progress.setCancelButton(nullptr);
  progress.setMaximum(0);
  progress.setMinimum(0);
  progress.setMinimumWidth
    (qCeil(PROGRESS_DIALOG_WIDTH_MULTIPLIER * progress.sizeHint().width()));
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Vacuuming Database"));
  progress.show();
  progress.repaint();
  QApplication::processEvents();

  if(statusBar())
    {
      statusBar()->showMessage
	(tr("Vacuuming the database. Please be patient."));
      statusBar()->repaint();
      statusBar()->update();
    }

  QSqlQuery query(m_db);

  if(m_db.driverName() == "QSQLITE")
    {
      query.exec("DELETE FROM book_copy_info WHERE item_oid NOT IN "
		 "(SELECT myoid FROM book)");
      progress.setValue(0);
      query.exec("DELETE FROM book_files WHERE item_oid NOT IN "
		 "(SELECT myoid FROM book)");
      progress.setValue(0);
      query.exec("DELETE FROM cd_copy_info WHERE item_oid NOT IN "
		 "(SELECT myoid FROM cd)");
      progress.setValue(0);
      query.exec("DELETE FROM cd_songs WHERE item_oid NOT IN "
		 "(SELECT myoid FROM cd)");
      progress.setValue(0);
      query.exec("DELETE FROM dvd_copy_info WHERE item_oid NOT IN "
		 "(SELECT myoid FROM dvd)");
      progress.setValue(0);
      query.exec("DELETE FROM grey_literature_files WHERE item_oid NOT IN "
		 "(SELECT myoid FROM grey_literature)");
      progress.setValue(0);
      query.exec("DELETE FROM journal_copy_info WHERE item_oid NOT IN "
		 "(SELECT myoid FROM journal)");
      progress.setValue(0);
      query.exec("DELETE FROM journal_files WHERE item_oid NOT IN "
		 "(SELECT myoid FROM journal)");
      progress.setValue(0);
      query.exec("DELETE FROM magazine_copy_info WHERE item_oid NOT IN "
		 "(SELECT myoid FROM magazine)");
      progress.setValue(0);
      query.exec("DELETE FROM magazine_files WHERE item_oid NOT IN "
		 "(SELECT myoid FROM magazine)");
      progress.setValue(0);
      query.exec("DELETE FROM photograph WHERE collection_oid NOT IN "
		 "(SELECT myoid FROM photograph_collection)");
      progress.setValue(0);
      query.exec("DELETE FROM videogame_copy_info WHERE item_oid NOT IN "
		 "(SELECT myoid FROM videogame)");
      progress.setValue(0);
    }

  query.exec("VACUUM");
  progress.setValue(0);

  if(statusBar())
    statusBar()->clearMessage();

  progress.close();
  QApplication::processEvents();
}

void biblioteq::vgSearch(const QString &field, const QString &value)
{
  auto videogame = new biblioteq_videogame(this, "", QModelIndex());

  videogame->search(field, value);
  videogame->deleteLater();
}
