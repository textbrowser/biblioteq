/*
** -- Qt Includes --
*/

#include <QSettings>
#include <QSqlDriver>
#include <QSqlField>
#include <QSqlRecord>

#include <limits>

/*
** -- Local Includes --
*/

#include "biblioteq.h"
#include "biblioteq_book.h"
#include "biblioteq_graphicsitempixmap.h"
#include "biblioteq_grey_literature.h"
#include "biblioteq_otheroptions.h"
#include "biblioteq_pdfreader.h"

void biblioteq::slotShowOtherOptions(void)
{
  biblioteq_misc_functions::center(m_otheroptions, this);
  m_otheroptions->showNormal();
  m_otheroptions->activateWindow();
  m_otheroptions->raise();
}

void biblioteq::slotPreviewCanvasBackgroundColor(const QColor &color)
{
  ui.graphicsView->scene()->setBackgroundBrush(color);
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
		   toString().trimmed());

      if(!color.isValid())
	color = Qt::white;

      ui.graphicsView->scene()->setBackgroundBrush(color);
    }
}

QString biblioteq::publicationDateFormat(const QString &itemType) const
{
  return m_otheroptions->publicationDateFormat(itemType);
}

void biblioteq::slotOtherOptionsSaved(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  foreach(QWidget *widget, QApplication::topLevelWidgets())
    if(qobject_cast<biblioteq_book *> (widget))
      qobject_cast<biblioteq_book *> (widget)->setPublicationDateFormat
	(m_otheroptions->publicationDateFormat("books"));
    else if(qobject_cast<biblioteq_cd *> (widget))
      qobject_cast<biblioteq_cd *> (widget)->setPublicationDateFormat
	(m_otheroptions->publicationDateFormat("musiccds"));
    else if(qobject_cast<biblioteq_dvd *> (widget))
      qobject_cast<biblioteq_dvd *> (widget)->setPublicationDateFormat
	(m_otheroptions->publicationDateFormat("dvds"));
    else if(qobject_cast<biblioteq_grey_literature *> (widget))
      qobject_cast<biblioteq_grey_literature *> (widget)->
	setPublicationDateFormat(m_otheroptions->
				 publicationDateFormat("greyliterature"));
    else if(qobject_cast<biblioteq_journal *> (widget))
      qobject_cast<biblioteq_journal *> (widget)->setPublicationDateFormat
	(m_otheroptions->publicationDateFormat("journals"));
    else if(qobject_cast<biblioteq_magazine *> (widget))
      qobject_cast<biblioteq_magazine *> (widget)->setPublicationDateFormat
	(m_otheroptions->publicationDateFormat("magazines"));
    else if(qobject_cast<biblioteq_photographcollection *> (widget))
      qobject_cast<biblioteq_photographcollection *> (widget)->
	setPublicationDateFormat
	(m_otheroptions->publicationDateFormat("photographcollections"));
    else if(qobject_cast<biblioteq_videogame *> (widget))
      qobject_cast<biblioteq_videogame *> (widget)->setPublicationDateFormat
	(m_otheroptions->publicationDateFormat("videogames"));

  QApplication::restoreOverrideCursor();
}

void biblioteq::slotOpenPDFFile(void)
{
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
  QFileDialog dialog(this);

  dialog.setDirectory(QDir::homePath());
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter("PDF File (*.pdf)");
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowTitle(tr("BiblioteQ: Open PDF File"));

  if(dialog.exec() == QDialog::Accepted)
    {
#ifndef Q_OS_MAC
      QApplication::processEvents();
#endif

      biblioteq_pdfreader *reader = new(std::nothrow) biblioteq_pdfreader(0);

      if(reader)
	{
	  QApplication::setOverrideCursor(Qt::WaitCursor);
	  reader->load(dialog.selectedFiles().value(0));
	  biblioteq_misc_functions::center(reader, this);
	  reader->show();
	  QApplication::restoreOverrideCursor();
	}
    }
#endif
}

void biblioteq::slotGeneralSearchPublicationDateEnabled(bool state)
{
  al.publication_date->setEnabled(state);

  if(!state)
    al.publication_date->setDate(QDate::fromString("2001", "yyyy"));
}

void biblioteq::slotInsertGreyLiterature(void)
{
  QString id("");
  biblioteq_grey_literature *gl = 0;

  m_idCt += 1;
  id = QString("insert_%1").arg(m_idCt);
  gl = new(std::nothrow) biblioteq_grey_literature(this, id, -1);

  if(gl)
    gl->insert();
}

void biblioteq::slotGreyLiteratureSearch(void)
{
  biblioteq_grey_literature *gl = 0;

  foreach(QWidget *w, QApplication::topLevelWidgets())
    {
      biblioteq_grey_literature *g = qobject_cast
	<biblioteq_grey_literature *> (w);

      if(g && g->getID() == "search")
	{
	  gl = g;
	  break;
	}
    }

  if(!gl)
    {
      gl = new(std::nothrow) biblioteq_grey_literature(this, "search", -1);

      if(gl)
	gl->search();
    }

  if(gl)
    {
      gl->showNormal();
      gl->activateWindow();
      gl->raise();
    }
}

void biblioteq::greyLiteratureSearch
(const QString &field, const QString &value)
{
  biblioteq_grey_literature *gl =
    new(std::nothrow) biblioteq_grey_literature(this, "", -1);

  if(gl)
    {
      gl->search(field, value);
      gl->deleteLater();
    }
}

void biblioteq::slotRefreshCustomQuery(void)
{
  if(!m_db.isOpen())
    return;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  QSqlField field;
  QSqlRecord rec;
  QStringList list;
  QTreeWidgetItem *item1 = 0;
  QTreeWidgetItem *item2 = 0;
  int i = 0;
  int j = 0;

  cq.tables_t->clear();

  if(m_db.driverName() == "QSQLITE")
    list << "book"
	 << "book_binding_types"
	 << "book_copy_info"
	 << "cd"
	 << "cd_copy_info"
	 << "cd_formats"
	 << "cd_songs"
	 << "dvd"
	 << "dvd_aspect_ratios"
	 << "dvd_copy_info"
	 << "dvd_ratings"
	 << "dvd_regions"
	 << "grey_literature"
	 << "grey_literature_types"
	 << "item_borrower"
	 << "item_borrower_vw"
	 << "journal"
	 << "journal_copy_info"
	 << "languages"
	 << "locations"
	 << "magazine"
	 << "magazine_copy_info"
	 << "member"
	 << "member_history"
	 << "minimum_days"
	 << "monetary_units"
	 << "photograph"
	 << "photograph_collection"
	 << "videogame"
	 << "videogame_copy_info"
	 << "videogame_platforms"
	 << "videogame_ratings";
  else
    list << "admin"
	 << "book"
	 << "book_binding_types"
	 << "book_copy_info"
	 << "cd"
	 << "cd_copy_info"
	 << "cd_formats"
	 << "cd_songs"
	 << "dvd"
	 << "dvd_aspect_ratios"
	 << "dvd_copy_info"
	 << "dvd_ratings"
	 << "dvd_regions"
	 << "grey_literature"
	 << "grey_literature_types"
	 << "item_borrower"
	 << "item_borrower_vw"
	 << "item_request"
	 << "journal"
	 << "journal_copy_info"
	 << "languages"
	 << "locations"
	 << "magazine"
	 << "magazine_copy_info"
	 << "member"
	 << "member_history"
	 << "minimum_days"
	 << "monetary_units"
	 << "photograph"
	 << "photograph_collection"
	 << "videogame"
	 << "videogame_copy_info"
	 << "videogame_platforms"
	 << "videogame_ratings";

  list.sort();
  cq.tables_t->setSortingEnabled(false);
  cq.tables_t->setColumnCount(3);
  cq.tables_t->setHeaderLabels(QStringList()
			       << tr("Table Name")
			       << tr("Column")
			       << tr("Column Type")
			       << tr("NULL"));

  for(i = 0; i < list.size(); i++)
    if((item1 = new(std::nothrow) QTreeWidgetItem(cq.tables_t)) != 0)
      {
	item1->setText(0, list[i]);
	rec = m_db.record(list[i]);

	for(j = 0; j < rec.count(); j++)
	  {
	    if((item2 = new(std::nothrow) QTreeWidgetItem(item1)) == 0)
	      {
		addError(QString(tr("Memory Error")),
			 QString(tr("Unable to allocate "
				    "memory for the \"item2\" "
				    "object. "
				    "This is a serious "
				    "problem!")), QString(""),
			 __FILE__, __LINE__);
		continue;
	      }

	    field = rec.field(rec.fieldName(j));
	    item2->setText(1, rec.fieldName(j));
	    item2->setText(2, QVariant::typeToName(field.type()));

	    if(field.requiredStatus() == QSqlField::Required)
	      item2->setText(3, tr("No"));
	    else
	      item2->setText(3, "");
	  }
      }
    else
      addError(QString(tr("Memory Error")),
	       QString(tr("Unable to allocate "
			  "memory for the \"item1\" "
			  "object. "
			  "This is a serious "
			  "problem!")), QString(""),
	       __FILE__, __LINE__);

  for(i = 0; i < cq.tables_t->columnCount() - 1; i++)
    cq.tables_t->resizeColumnToContents(i);

  cq.tables_t->setSortingEnabled(true);
  cq.tables_t->sortByColumn(0, Qt::AscendingOrder);
  QApplication::restoreOverrideCursor();
}

QStringList biblioteq::getSRUNames(void) const
{
  return m_sruMaps.keys();
}

QStringList biblioteq::getZ3950Names(void) const
{
  return m_z3950Maps.keys();
}

int biblioteq::populateTable(const QSqlQuery &query,
			     const QString &typefilter,
			     const int pagingType,
			     const int searchType)
{
  if(pagingType == NEW_PAGE)
    {
      if(m_searchQuery.isActive())
	m_searchQuery.clear();

      m_searchQuery = query;
    }

  m_lastSearchStr = "Item Search Query";
  ui.itemsCountLabel->setText(tr("0 Results"));

  QProgressDialog progress(this);
  QTableWidgetItem *item = 0;
  QString itemType("");
  QString str("");
  int i = -1;
  int limit = pageLimit();
  qint64 offset = m_queryOffset;

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

      ui.graphicsView->setSceneRect(0, 0, 5 * 150, limit / 5 * 200 + 15);
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(ui.table->rowCount() == 0)
    ui.itemsCountLabel->setText(tr("0 Results"));
  else
    ui.itemsCountLabel->setText
      (QString(tr("%1 Result(s)")).arg(ui.table->rowCount()));

  bool ok = true;

  if(pagingType == NEW_PAGE)
    ok = m_searchQuery.exec();
  else if(pagingType == NEXT_PAGE || pagingType == PREVIOUS_PAGE)
    ok = m_searchQuery.seek(offset);
  else if(pagingType < 0)
    ok = m_searchQuery.seek(limit * qAbs(pagingType + 1));

  if(m_searchQuery.lastError().isValid() || !ok)
    {
      progress.close();
#ifndef Q_OS_MAC
      QApplication::processEvents();
#endif
      QApplication::restoreOverrideCursor();

      if(!m_previousTypeFilter.isEmpty())
	for(int ii = 0; ii < ui.menu_Category->actions().size();
	    ii++)
	  if(m_previousTypeFilter ==
	     ui.menu_Category->actions().at(ii)->data().toString())
	    {
	      ui.menu_Category->setDefaultAction
		(ui.menu_Category->actions().at(ii));
	      ui.categoryLabel->setText
		(ui.menu_Category->actions().at(ii)->text());
	      break;
	    }

      if(m_searchQuery.lastError().isValid())
	addError(QString(tr("Database Error")),
		 QString(tr("Unable to retrieve the data required for "
			    "populating the main views.")),
		 m_searchQuery.lastError().text(), __FILE__, __LINE__);

      QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			    tr("Unable to retrieve the data required for "
			       "populating the main views."));
      return 1;
    }

  QApplication::restoreOverrideCursor();
  prepareRequestToolButton(typefilter);

  bool found = false;

  for(int ii = 0; ii < ui.menu_Category->actions().size(); ii++)
    if(typefilter == ui.menu_Category->actions().at(ii)->data().toString())
      {
	found = true;
	m_previousTypeFilter = typefilter;
	ui.menu_Category->setDefaultAction(ui.menu_Category->actions().at(ii));
	ui.categoryLabel->setText(ui.menu_Category->actions().at(ii)->text());
	break;
      }

  if(typefilter.isEmpty())
    {
      ui.menu_Category->setDefaultAction(ui.menu_Category->actions().value(0));
      ui.categoryLabel->setText(tr("All"));
    }
  else if(!found)
    {
      ui.menu_Category->setDefaultAction(ui.menu_Category->actions().value(0));
      ui.categoryLabel->setText(tr("All"));
    }

  ui.table->resetTable(m_db.userName(), typefilter, m_roles);

  qint64 currentPage = 0;

  if(limit <= 0)
    currentPage = 1;
  else
    currentPage = offset / limit + 1;

  if(pagingType == NEW_PAGE)
    m_pages = 0;

  if(pagingType >= 0 &&
     pagingType != PREVIOUS_PAGE &&
     currentPage > m_pages)
    m_pages += 1;

  if(limit == -1)
    m_pages = 1;

  if(m_pages == 1)
    ui.pagesLabel->setText("1");
  else if(m_pages >= 2 && m_pages <= 10)
    {
      QString str("");

      for(qint64 ii = 1; ii <= m_pages; ii++)
	if(ii == currentPage)
	  str += QString(tr(" %1 ")).arg(currentPage);
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
	  str += QString(tr(" %1 ")).arg(ii);
	else if(ii <= m_pages - 1)
	  str += QString(" <a href=\"%1\">" + tr("%1") + "</a> ").arg(ii);

      if(currentPage == m_pages)
	str += QString(tr(" ... %1 ")).arg(currentPage);
      else
	str += QString(" ... <a href=\"%1\">" + tr("%1") + "</a> ").
	  arg(m_pages);

      str = str.trimmed();
      ui.pagesLabel->setText(str);
    }

  m_lastSearchType = searchType;
  ui.table->scrollToTop();
  ui.table->horizontalScrollBar()->setValue(0);
  ui.table->clearSelection();
  ui.table->setCurrentItem(0);
  slotDisplaySummary();
  ui.graphicsView->scene()->clear();
  ui.graphicsView->resetTransform();
  ui.graphicsView->verticalScrollBar()->setValue(0);
  ui.graphicsView->horizontalScrollBar()->setValue(0);
  ui.table->setSortingEnabled(false);
  progress.setLabelText(tr("Populating the views..."));

  if(limit == -1)
    progress.setMaximum(0);
  else
    progress.setMaximum(limit);

  progress.setMinimum(0);
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  raise();
  progress.show();
  progress.update();
  progress.repaint();
#ifndef Q_OS_MAC
  QApplication::processEvents();
#endif

  int iconTableColumnIdx = 0;
  int iconTableRowIdx = 0;

  /*
  ** Adjust the dimensions of the graphics scene if pagination
  ** is effectively disabled.
  */

  if(limit == -1)
    {
      m_searchQuery.seek(0);

      int size = 0;

      while(m_searchQuery.next())
	size += 1;

      if(size > 0 && (size / 250 <= std::numeric_limits<int>::max()))
	ui.graphicsView->setSceneRect(0, 0, 5 * 150, size * 250 + 15);
      else
	ui.graphicsView->setSceneRect
	  (0, 0, 5 * 150, std::numeric_limits<int>::max());

      m_searchQuery.seek(offset);
    }

  if(limit != -1 && m_db.driver()->hasFeature(QSqlDriver::QuerySize))
    progress.setMaximum(qMin(limit, m_searchQuery.size()));

  QSettings settings;
  QStringList columnNames(ui.table->columnNames());
  bool showToolTips = settings.value("show_maintable_tooltips", false).toBool();

  i = -1;

  while(i++, !progress.wasCanceled())
    {
      if(i == limit)
	break;

      if(m_searchQuery.at() == QSql::BeforeFirstRow)
	if(!m_searchQuery.next())
	  break;

      biblioteq_graphicsitempixmap *pixmapItem = 0;

      if(m_searchQuery.isValid())
	{
	  QSqlRecord record(m_searchQuery.record());
	  QString tooltip("");
	  QTableWidgetItem *first = 0;

	  if(showToolTips)
	    {
	      tooltip = "<html>";

	      for(int j = 0; j < record.count(); j++)
		{
		  if(record.field(j).type() == QVariant::ByteArray)
		    continue;

		  QString columnName(columnNames.value(j));

		  if(columnName.isEmpty())
		    columnName = "N/A";

		  tooltip.append("<b>");
		  tooltip.append(columnName);
		  tooltip.append(":</b> ");
		  tooltip.append(m_searchQuery.value(j).toString().trimmed());
		  tooltip.append("<br>");
		}

	      if(tooltip.endsWith("<br>"))
		tooltip = tooltip.mid(0, tooltip.length() - 4);

	      tooltip.append("</html>");
	    }

	  for(int j = 0; j < record.count(); j++)
	    {
	      item = 0;

	      if(!record.fieldName(j).endsWith("front_cover") &&
		 !record.fieldName(j).endsWith("image_scaled"))
		{
		  if(record.fieldName(j).contains("date") ||
		     record.fieldName(j).contains("membersince"))
		    {
		      QDate date(QDate::fromString(m_searchQuery.value(j).
						   toString(), "MM/dd/yyyy"));

		      str = date.toString(Qt::ISODate);

		      if(str.isEmpty())
			str = m_searchQuery.value(j).toString().trimmed();
		    }
		  else
		    str = m_searchQuery.value(j).toString();
		}

	      if(record.fieldName(j).endsWith("availability") ||
		 record.fieldName(j).endsWith("cddiskcount") ||
		 record.fieldName(j).endsWith("dvddiskcount") ||
		 record.fieldName(j).endsWith("file_count") ||
		 record.fieldName(j).endsWith("issue") ||
		 record.fieldName(j).endsWith("issueno") ||
		 record.fieldName(j).endsWith("issuevolume") ||
		 record.fieldName(j).endsWith("photograph_count") ||
		 record.fieldName(j).endsWith("price") ||
		 record.fieldName(j).endsWith("quantity") ||
		 record.fieldName(j).endsWith("total_reserved") ||
		 record.fieldName(j).endsWith("volume"))
		{
		  if(record.fieldName(j).endsWith("price"))
		    {
		      item = new(std::nothrow) biblioteq_numeric_table_item
			(m_searchQuery.value(j).toDouble());
		      str = QString::number
			(m_searchQuery.value(j).toDouble(), 'f', 2);
		    }
		  else
		    item = new(std::nothrow) biblioteq_numeric_table_item
		      (m_searchQuery.value(j).toInt());
		}
	      else if(record.fieldName(j).endsWith("callnumber"))
		{
		  str = m_searchQuery.value(j).toString();
		  item = new(std::nothrow) biblioteq_callnum_table_item(str);
		}
	      else if(record.fieldName(j).endsWith("front_cover") ||
		      record.fieldName(j).endsWith("image_scaled"))
		{
		  QImage image;

		  if(!m_searchQuery.isNull(j))
		    {
		      image.loadFromData
			(QByteArray::fromBase64(m_searchQuery.value(j).
						toByteArray()));

		      if(image.isNull())
			image.loadFromData
			  (m_searchQuery.value(j).toByteArray());
		    }

		  if(image.isNull())
		    image = QImage(":/no_image.png");

		  /*
		  ** The size of no_image.png is 126x187.
		  */

		  if(!image.isNull())
		    image = image.scaled
		      (126, 187, Qt::KeepAspectRatio,
		       Qt::SmoothTransformation);

		  pixmapItem = new(std::nothrow) biblioteq_graphicsitempixmap
		    (QPixmap::fromImage(image), 0);

		  if(pixmapItem)
		    {
		      if(iconTableRowIdx == 0)
			pixmapItem->setPos(140 * iconTableColumnIdx, 15);
		      else
			pixmapItem->setPos(140 * iconTableColumnIdx,
					   200 * iconTableRowIdx + 15);

		      pixmapItem->setFlag
			(QGraphicsItem::ItemIsSelectable, true);
		      ui.graphicsView->scene()->addItem(pixmapItem);
		    }

		  iconTableColumnIdx += 1;

		  if(iconTableColumnIdx >= 5)
		    {
		      iconTableRowIdx += 1;
		      iconTableColumnIdx = 0;
		    }
		}
	      else
		item = new(std::nothrow) QTableWidgetItem();

	      if(item != 0)
		{
		  item->setText(str);
		  item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

		  if(j == 0)
		    {
		      first = item;
		      ui.table->setRowCount(ui.table->rowCount() + 1);
		      ui.itemsCountLabel->setText(QString(tr("%1 Result(s)")).
						  arg(ui.table->rowCount()));
		    }

		  item->setToolTip(tooltip);
		  ui.table->setItem(i, j, item);

		  if(record.fieldName(j).endsWith("type"))
		    {
		      itemType = str;
		      itemType = itemType.toLower().remove(" ");
		    }

		  if(record.fieldName(j).endsWith("myoid"))
		    updateRows(str, i, itemType);
		}
	      else if(!record.fieldName(j).endsWith("front_cover") &&
		      !record.fieldName(j).endsWith("image_scaled"))
		addError(QString(tr("Memory Error")),
			 QString(tr("Unable to allocate "
				    "memory for the \"item\" "
				    "object. "
				    "This is a serious "
				    "problem!")), QString(""),
			 __FILE__, __LINE__);
	    }

	  if(first)
	    {
	      if(pixmapItem)
		first->setIcon(pixmapItem->pixmap());
	      else
		first->setIcon(QIcon(":/no_image.png"));

	      ui.table->setRowHeight(i, ui.table->iconSize().height());
	    }
	}

      if(m_searchQuery.isValid())
	if(pixmapItem)
	  {
	    QSqlRecord record(m_searchQuery.record());

	    for(int ii = 0; ii < record.count(); ii++)
	      {
		if(record.fieldName(ii).endsWith("myoid"))
		  pixmapItem->setData(0, m_searchQuery.value(ii));
		else if(record.fieldName(ii).endsWith("type"))
		  pixmapItem->setData(1, m_searchQuery.value(ii));
	      }
	  }

      if(i + 1 <= progress.maximum())
	progress.setValue(i + 1);

      progress.repaint();
#ifndef Q_OS_MAC
      QApplication::processEvents();
#endif

      if(m_searchQuery.at() != QSql::BeforeFirstRow)
	if(!m_searchQuery.next())
	  break;
    }

  if(limit != -1 && !m_db.driver()->hasFeature(QSqlDriver::QuerySize))
    progress.setValue(limit);

  bool wasCanceled = progress.wasCanceled(); /*
					     ** QProgressDialog::close()!
					     */

  progress.close();
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
    ui.itemsCountLabel->setText(QString(tr("%1 Result(s)")).
				arg(ui.table->rowCount()));

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

#ifdef Q_OS_MAC
  ui.table->hide();
  ui.table->show();
#endif
  return 0;
}

QHash<QString, QString> biblioteq::getOpenLibraryHash(void) const
{
  return m_openLibraryImages;
}
