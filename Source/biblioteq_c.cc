#include "biblioteq.h"
#include "biblioteq_copy_editor.h"
#include "biblioteq_files.h"
#include "biblioteq_graphicsitempixmap.h"
#include "biblioteq_otheroptions.h"
#include "biblioteq_pdfreader.h"

#include <QActionGroup>
#include <QDesktopServices>
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

QColor biblioteq::availabilityColor(const QString &itemType) const
{
  return m_otheroptions->availabilityColor(itemType);
}

QHash<QString, QString> biblioteq::getOpenLibraryImagesHash(void) const
{
  return m_openLibraryImages;
}

QHash<QString, QString> biblioteq::getOpenLibraryItemsHash(void) const
{
  return m_openLibraryItems;
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
  return m_otheroptions->publicationDateFormat(itemType);
}

QString biblioteq::reservationHistoryHtml(void) const
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  QString firstname("");
  QString information("");
  QString lastname("");
  QString memberid("");
  QString html("<html>");
  static QString endl("<br>");

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
	      if(j == HistoryColumns::MEMBER_ID)
		memberid = history.table->item(i, j)->text();
	      else if(j == HistoryColumns::FIRST_NAME)
		firstname = history.table->item(i, j)->text();
	      else if(j == HistoryColumns::LAST_NAME)
		lastname = history.table->item(i, j)->text();
	      else
		break;
	  }

	information += history.table->item(i, HistoryColumns::TITLE)->text();
	information += endl;
	information += history.table->item
	  (i, HistoryColumns::ID_NUMBER)->text();
	information += endl;
	information += history.table->item(i, HistoryColumns::TYPE)->text();
	information += endl;
	information += tr("Reservation Date: ");
	information += history.table->item
	  (i, HistoryColumns::RESERVATION_DATE)->text();
	information += endl;
	information += tr("Due Date: ");
	information += history.table->item
	  (i, HistoryColumns::DUE_DATE)->text();
	information += endl;
	information += tr("Returned Date: ");
	information += history.table->item
	  (i, HistoryColumns::RETURNED_DATE)->text();

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
      html += "<th>" + ui.table->horizontalHeaderItem(i)->text() +
	"</th>";

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

int biblioteq::populateTable(const QSqlQuery &query,
			     const QString &typefilter,
			     const int pagingType,
			     const int searchType)
{
  if(pagingType == NEW_PAGE)
    {
      if(m_searchQuery.isActive())
	m_searchQuery.clear();

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
      m_searchQuery = query;
#else
      m_searchQuery = query;
#endif
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

  if(m_otheroptions->showMainTableProgressDialogs())
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
  auto columns = m_otheroptions->iconsViewColumnCount();
  auto limit = pageLimit();
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
	 limit / (static_cast<qreal> (columns)) * 200.0 + 200.0);
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(ui.table->rowCount() == 0)
    ui.itemsCountLabel->setText(tr("0 Results"));
  else
    ui.itemsCountLabel->setText
      (QString(tr("%1 Result(s)")).arg(ui.table->rowCount()));

  auto ok = true;

  if(pagingType == NEW_PAGE)
    ok = m_searchQuery.exec();
  else if(pagingType == NEXT_PAGE || pagingType == PREVIOUS_PAGE)
    ok = m_searchQuery.seek(static_cast<int> (offset));
  else if(pagingType < 0)
    ok = m_searchQuery.seek(limit * qAbs(pagingType + 1));

  if(m_searchQuery.lastError().isValid() || !ok)
    {
      if(progress)
	progress->close();

      QApplication::processEvents();
      QApplication::restoreOverrideCursor();

      if(!m_previousTypeFilter.isEmpty())
	for(int ii = 0; ii < ui.menu_Category->actions().size();
	    ii++)
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

      if(m_searchQuery.lastError().isValid())
	addError(QString(tr("Database Error")),
		 QString(tr("Unable to retrieve the data required for "
			    "populating the main views.")),
		 m_searchQuery.lastError().text(), __FILE__, __LINE__);

      QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			    tr("Unable to retrieve the data required for "
			       "populating the main views."));
      QApplication::processEvents();
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

  if(pagingType >= 0 &&
     pagingType != PREVIOUS_PAGE &&
     currentPage > m_pages)
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
  ui.table->setCurrentItem(nullptr);
  slotDisplaySummary();
  ui.graphicsView->scene()->clear();
  ui.graphicsView->resetTransform();
  ui.graphicsView->verticalScrollBar()->setValue(0);
  ui.graphicsView->horizontalScrollBar()->setValue(0);
  ui.table->setSortingEnabled(false);

  if(progress)
    {
      QApplication::restoreOverrideCursor();
      progress->setLabelText(tr("Populating the views..."));

      if(limit == -1)
	progress->setMaximum(0);
      else
	progress->setMaximum(limit);

      progress->setMinimum(0);
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
      m_searchQuery.seek(0);

      int size = 0;

      while(m_searchQuery.next())
	size += 1;

      if(size > 0)
	ui.graphicsView->setSceneRect
	  (0.0,
	   0.0,
	   150.0 * static_cast<qreal> (columns),
	   (size / static_cast<qreal> (columns)) * 200.0 + 200.0);

      if(progress && size >= 0)
	progress->setMaximum(size);

      m_searchQuery.seek(static_cast<int> (offset));
    }

  if(limit != -1 &&
     m_db.driver()->hasFeature(QSqlDriver::QuerySize) &&
     progress)
    progress->setMaximum(qMin(limit, m_searchQuery.size()));

  QFontMetrics fontMetrics(ui.table->font());
  QSettings settings;
  QString dateFormat("");
  auto availabilityColors = this->availabilityColors();
  auto booksAccessionNumberIndex = m_otheroptions->booksAccessionNumberIndex();
  auto columnNames(ui.table->columnNames());
  auto showBookReadStatus = m_db.driverName() == "QSQLITE" &&
    m_otheroptions->showBookReadStatus() &&
    typefilter == "Books";
  auto showMainTableImages = m_otheroptions->showMainTableImages();
  auto showToolTips = settings.value("show_maintable_tooltips", false).toBool();

  if(typefilter == "Books" ||
     typefilter == "DVDs" ||
     typefilter == "Grey Literature" ||
     typefilter == "Journals" ||
     typefilter == "Magazines" ||
     typefilter == "Music CDs" ||
     typefilter == "Photograph Collections" ||
     typefilter == "Video Games")
    dateFormat = publicationDateFormat
      (QString(typefilter).remove(' ').toLower());

  i = -1;

  while(i++, true)
    {
      if(i == limit)
	break;

      if(progress && progress->wasCanceled())
	break;

      if(m_searchQuery.at() == QSql::BeforeFirstRow)
	if(!m_searchQuery.next())
	  break;

      biblioteq_graphicsitempixmap *pixmapItem = nullptr;
      biblioteq_numeric_table_item *availabilityItem = nullptr;
      quint64 myoid = 0;

      if(m_searchQuery.isValid())
	{
	  QString tooltip("");
	  QTableWidgetItem *first = nullptr;
	  auto record(m_searchQuery.record());

	  if(showToolTips)
	    {
	      tooltip = "<html>";

	      for(int j = 0; j < record.count(); j++)
		{
		  auto fieldName(record.fieldName(j));

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

		  if(record.field(j).tableName() == "book" &&
		     (fieldName == "id" || fieldName == "isbn13"))
		    {
		      auto str(m_searchQuery.value(j).toString().trimmed());

		      if(fieldName == "id")
			str = m_otheroptions->isbn10DisplayFormat(str);
		      else
			str = m_otheroptions->isbn13DisplayFormat(str);

		      tooltip.append(str);
		    }
		  else
		    tooltip.append
		      (m_searchQuery.value(j).
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

	      auto fieldName(record.fieldName(j));

	      if(!fieldName.endsWith("front_cover") &&
		 !fieldName.endsWith("image_scaled"))
		{
		  if(fieldName.contains("date") ||
		     fieldName.contains("membersince"))
		    {
		      auto date(QDate::fromString(m_searchQuery.value(j).
						  toString().trimmed(),
						  "MM/dd/yyyy"));

		      if(dateFormat.isEmpty())
			str = date.toString(Qt::ISODate);
		      else
			str = date.toString(dateFormat);

		      if(str.isEmpty())
			str = m_searchQuery.value(j).toString().trimmed();
		    }
		  else
		    str = m_searchQuery.value(j).toString().trimmed();
		}

	      if(record.field(j).tableName() == "book" &&
		 (fieldName == "id" || fieldName == "isbn13"))
		{
		  if(fieldName == "id")
		    str = m_otheroptions->isbn10DisplayFormat(str);
		  else
		    str = m_otheroptions->isbn13DisplayFormat(str);

		  item = new QTableWidgetItem(str);
		}
	      else if(fieldName.endsWith("accession_number"))
		{
		  if(typefilter == "Books")
		    {
		      if(booksAccessionNumberIndex == 0)
			item = new biblioteq_numeric_table_item
			  (query.value(j).toInt());
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
			(m_searchQuery.value(j).toDouble());
		      str = QString::number
			(m_searchQuery.value(j).toDouble(), 'f', 2);
		    }
		  else
		    {
		      item = new biblioteq_numeric_table_item
			(m_searchQuery.value(j).toInt());

		      if(availabilityColors &&
			 fieldName.endsWith("availability"))
			availabilityItem = dynamic_cast
			  <biblioteq_numeric_table_item *> (item);
		    }
		}
	      else if(fieldName.endsWith("callnumber"))
		{
		  str = m_searchQuery.value(j).toString().trimmed();
		  item = new biblioteq_callnum_table_item(str);
		}
	      else if(fieldName.endsWith("front_cover") ||
		      fieldName.endsWith("image_scaled"))
		{
		  QImage image;

		  if(showMainTableImages)
		    {
		      if(!m_searchQuery.isNull(j))
			{
			  image.loadFromData
			    (QByteArray::fromBase64(m_searchQuery.value(j).
						    toByteArray()));

			  if(image.isNull())
			    image.loadFromData
			      (m_searchQuery.value(j).toByteArray());
			}
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

		  pixmapItem = new biblioteq_graphicsitempixmap
		    (QPixmap::fromImage(image), nullptr);

		  if(iconTableRowIdx == 0)
		    pixmapItem->setPos(140.0 * iconTableColumnIdx, 15.0);
		  else
		    pixmapItem->setPos(140.0 * iconTableColumnIdx,
				       200.0 * iconTableRowIdx + 15.0);

		  pixmapItem->setFlag
		    (QGraphicsItem::ItemIsSelectable, true);
		  ui.graphicsView->scene()->addItem(pixmapItem);
		  iconTableColumnIdx += 1;

		  if(columns <= iconTableColumnIdx)
		    {
		      iconTableRowIdx += 1;
		      iconTableColumnIdx = 0;
		    }
		}
	      else
		item = new QTableWidgetItem();

	      if(item != nullptr)
		{
		  item->setText
		    (str.simplified().replace("<br>", " ").
		     simplified().trimmed());
		  item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

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

		  if(fieldName.endsWith("type"))
		    {
		      itemType = str;
		      itemType = itemType.toLower().remove(" ");
		    }

		  if(fieldName.endsWith("myoid"))
		    {
		      myoid = query.value(j).toULongLong();
		      updateRows(str, item, itemType);
		    }
		}
	    }

	  if(availabilityItem && availabilityItem->value() > 0.0)
	    availabilityItem->setBackground(availabilityColor(itemType));

	  if(first && showMainTableImages)
	    {
	      if(pixmapItem)
		first->setIcon(pixmapItem->pixmap());
	      else
		first->setIcon(QIcon(":/no_image.png"));

	      ui.table->setRowHeight
		(i, qMax(fontMetrics.height() + 10,
			 ui.table->iconSize().height()));
	    }

	  if(showBookReadStatus)
	    {
	      /*
	      ** Was the book read?
	      */

	      auto item = new QTableWidgetItem();

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

      if(m_searchQuery.isValid())
	if(pixmapItem)
	  {
	    pixmapItem->setData(0, myoid);
	    pixmapItem->setData(1, itemType);
	  }

      if(progress)
	{
	  if(i + 1 <= progress->maximum())
	    progress->setValue(i + 1);

	  progress->repaint();
	  QApplication::processEvents();
	}

      if(m_searchQuery.at() != QSql::BeforeFirstRow)
	if(!m_searchQuery.next())
	  break;
    }

  ui.itemsCountLabel->setText
    (QString(tr("%1 Result(s)")).arg(ui.table->rowCount()));

  if(limit != -1 &&
     !m_db.driver()->hasFeature(QSqlDriver::QuerySize) &&
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

#ifdef Q_OS_MACOS
  ui.table->hide();
  ui.table->show();
#endif
  connect(ui.table,
	  SIGNAL(itemChanged(QTableWidgetItem *)),
	  this,
	  SLOT(slotItemChanged(QTableWidgetItem *)));
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
      m_configToolMenu->setTearOffEnabled(true);
      m_configToolMenu->setWindowIcon(QIcon(":/book.png"));
      m_configToolMenu->setWindowTitle(tr("BiblioteQ"));
#endif
    }
}

void biblioteq::deleteItem(const QString &oid, const QString &itemType)
{
  if(itemType == "book")
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
  else if(itemType == "cd")
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
  else if(itemType == "dvd")
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
  else if(itemType == "grey_literature")
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
  else if(itemType == "journal")
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
  else if(itemType == "magazine")
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
  else if(itemType == "photograph_collection")
    {
      foreach(auto w, QApplication::topLevelWidgets())
	{
	  auto photograph = qobject_cast<biblioteq_photographcollection *> (w);

	  if(photograph && photograph->getID() == oid)
	    {
	      removePhotographCollection(photograph);
	      break;
	    }
	}
    }
  else if(itemType == "videogame")
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

void biblioteq::dvdSearch(const QString &field, const QString &value)
{
  auto dvd = new biblioteq_dvd(this, "", QModelIndex());

  dvd->search(field, value);
  dvd->deleteLater();
}

void biblioteq::exportAsCSV(biblioteq_main_table *table, const QString &title)
{
  if(!table)
    return;

  QFileDialog dialog(this);

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

		if(table->columnNames().value(i) == "Read Status")
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

		    if(!table->item(i, j))
		      {
			str += ",";
			continue;
		      }

		    if(table->columnNames().value(j) == "Read Status")
		      /*
		      ** We cannot export the Read Status column because
		      ** it is not supported by PostgreSQL.
		      */

		      continue;

		    auto cleaned(table->item(i, j)->text());

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

void biblioteq::readConfig(void)
{
  QFont font;
  QSettings settings;

  al.caseinsensitive->setChecked
    (settings.value("generalSearchCaseSensitivity", false).toBool());
  ui.actionAutoPopulateOnCreation->setChecked
    (settings.value("automatically_populate_on_create", false).toBool());
  ui.actionAutomatically_Resize_Column_Widths->setChecked
    (settings.value("automatically_resize_column_widths", false).toBool());

  if(biblioteq_misc_functions::isGnome())
    m_members_diag->setGeometry
      (settings.value("members_window_geometry").toRect());
  else
    m_members_diag->restoreGeometry
      (settings.value("members_window_geometry").toByteArray());

  ui.actionPopulateOnStart->setChecked
    (settings.value("populate_table_on_connect", false).toBool());
  ui.actionResetErrorLogOnDisconnect->setChecked
    (settings.value("reset_error_log_on_disconnect", false).toBool());
  ui.actionShowGrid->setChecked
    (settings.value("show_table_grid", false).toBool());

  if(settings.contains("main_window_geometry"))
    {
      ui.actionPreserveGeometry->setChecked(true);

      if(biblioteq_misc_functions::isGnome())
	setGeometry(settings.value("main_window_geometry").toRect());
      else
	restoreGeometry(settings.value("main_window_geometry").toByteArray());
    }
  else
    ui.actionPreserveGeometry->setChecked(false);

#ifndef Q_OS_MACOS
  font = QApplication::font();

  if(settings.contains("global_font"))
    if(!font.fromString(settings.value("global_font", "").toString()))
      font = QApplication::font();

  QApplication::setFont(font);
#endif
  ui.actionAutomaticallySaveSettingsOnExit->setChecked
    (settings.value("save_settings_on_exit", true).toBool());
  ui.actionPopulate_Members_Browser_Table_on_Display->setChecked
    (settings.value("automatically_populate_members_"
		    "list_on_display", false).toBool());
  ui.actionPopulate_Administrator_Browser_Table_on_Display->setChecked
    (settings.value("automatically_populate_admin_list_on_display",
		    false).toBool());
  ui.actionPopulate_Database_Enumerations_Browser_on_Display->setChecked
    (settings.value("automatically_populate_enum_list_on_display",
		    false).toBool());

  QHash<QString, QString> states;

  for(int i = 0; i < settings.allKeys().size(); i++)
    if(settings.allKeys().at(i).contains("_header_state"))
      states[settings.allKeys().at(i)] =
	settings.value(settings.allKeys().at(i)).toString();

  ui.table->parseStates(states);
  states.clear();

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

  auto index = br.branch_name->findText
    (settings.value("previous_branch_name", "").toString());

  if(index >= 0)
    br.branch_name->setCurrentIndex(index);
  else
    br.branch_name->setCurrentIndex(0);

  auto viewModeIndex = settings.value("view_mode_index", 1).toInt();

  if(viewModeIndex < 0 || viewModeIndex > 1)
    viewModeIndex = 1;

  auto ag = findChild<QActionGroup *> ("ViewModeMenu");

  if(ag && ag->actions().size() > viewModeIndex)
    ag->actions().at(viewModeIndex)->setChecked(true);

  ui.stackedWidget->setCurrentIndex(viewModeIndex);

  if(ui.stackedWidget->currentIndex() == 0)
    ui.table->setSelectionMode(QAbstractItemView::MultiSelection);
  else
    ui.table->setSelectionMode(QAbstractItemView::ExtendedSelection);

  QColor color(settings.value("mainwindow_canvas_background_color").
	       toString().trimmed());

  if(!color.isValid())
    color = Qt::white;

  ui.graphicsView->scene()->setBackgroundBrush(color);
  slotResizeColumns();
  createSqliteMenuActions();
}

void biblioteq::readGlobalSetup(void)
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
#elif defined(Q_OS_WIN)
  QSettings settings(QCoreApplication::applicationDirPath() +
		     QDir::separator() +
		     "biblioteq.conf",
		     QSettings::IniFormat);
#else
  QSettings settings(BIBLIOTEQ_CONFIGFILE, QSettings::IniFormat);
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
	  m_amazonImages["back_proxy_port"] = settings.value("proxy_port", "").
	    toString().trimmed();
	  m_amazonImages["back_proxy_type"] = settings.value("proxy_type", "").
	    toString().trimmed();
	  m_amazonImages["back_proxy_username"] = settings.value
	    ("proxy_username", "").toString().trimmed();
	  m_amazonImages["back_proxy_password"] = settings.value
	    ("proxy_password", "").toString().trimmed();
	}
      else if(settings.group() == "Amazon Front Cover Images")
	{
	  m_amazonImages["front_cover_host"] = settings.value("host", "").
	    toString().trimmed();
	  m_amazonImages["front_cover_path"] = settings.value("path", "").
	    toString().trimmed().remove('"');
	  m_amazonImages["front_proxy_host"] = settings.value("proxy_host", "").
	    toString().trimmed();
	  m_amazonImages["front_proxy_port"] = settings.value("proxy_port", "").
	    toString().trimmed();
	  m_amazonImages["front_proxy_type"] = settings.value("proxy_type", "").
	    toString().trimmed();
	  m_amazonImages["front_proxy_username"] = settings.value
	    ("proxy_username", "").toString().trimmed();
	  m_amazonImages["front_proxy_password"] = settings.value
	    ("proxy_password", "").toString().trimmed();
	}
      else if(settings.group().startsWith("Branch"))
	{
	  if(!settings.value("database_name", "").
	     toString().trimmed().isEmpty())
	    {
	      QHash<QString, QString> hash;

	      hash["branch_name"] = settings.value("database_name", "").
		toString().trimmed();
	      hash["connection_options"] = settings.value
		("connection_options", "").toString().trimmed();
	      hash["hostname"] = settings.value("hostname", "").
		toString().trimmed();
	      hash["database_type"] = settings.value("database_type", "").
		toString().trimmed();
	      hash["port"] = settings.value("port", "").toString().trimmed();
	      hash["ssl_enabled"] = settings.value("ssl_enabled", "").
		toString().trimmed();
	      m_branches[settings.value("database_name", "").
			 toString().trimmed()] = hash;
	    }
	}
      else if(settings.group().startsWith("Open Library"))
	{
	  if(settings.group() == "Open Library")
	    {
	      m_openLibraryItems["url_isbn"] =
		settings.value("url_isbn").toString().trimmed();
	    }
	  else
	    {
	      m_openLibraryImages["back_url"] =
		settings.value("back_url", "").toString().trimmed();
	      m_openLibraryImages["front_url"] =
		settings.value("front_url", "").toString().trimmed();
	      m_openLibraryImages["proxy_host"] = settings.value
		("proxy_host", "").toString().trimmed();
	      m_openLibraryImages["proxy_port"] = settings.value
		("proxy_port", "").toString().trimmed();
	      m_openLibraryImages["proxy_type"] = settings.value
		("proxy_type", "").toString().trimmed();
	      m_openLibraryImages["proxy_username"] = settings.value
		("proxy_username", "").toString().trimmed();
	      m_openLibraryImages["proxy_password"] = settings.value
		("proxy_password", "").toString().trimmed();
	    }
	}
      else if(settings.group().startsWith("SRU"))
	{
	  if(!settings.value("name", "").toString().trimmed().isEmpty())
	    {
	      QHash<QString, QString> hash;

	      hash["Name"] = settings.value("name", "").toString().trimmed();
	      hash["proxy_host"] = settings.value("proxy_host", "").
		toString().trimmed();
	      hash["proxy_port"] = settings.value("proxy_port", "").
		toString().trimmed();
	      hash["proxy_type"] = settings.value("proxy_type", "").
		toString().trimmed();
	      hash["proxy_username"] = settings.value
		("proxy_username", "").toString().trimmed();
	      hash["proxy_password"] = settings.value
		("proxy_password", "").toString().trimmed();
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
	      hash["Port"] = settings.value("port", "").toString().trimmed();
	      hash["Database"] = settings.value("database_name", "").
		toString().trimmed();
	      hash["Format"] = settings.value("format", "marc8,utf-8").
		toString().trimmed().remove('"');
	      hash["RecordSyntax"] = settings.value
		("record_syntax", "MARC21").toString().trimmed();
	      hash["Userid"] = settings.value("username", "").
		toString().trimmed();
	      hash["Password"] = settings.value("password", "").
		toString().trimmed();
	      hash["proxy_host"] = settings.value("proxy_host", "").
		toString().trimmed();
	      hash["proxy_port"] = settings.value("proxy_port", "").
		toString().trimmed();
	      hash["timeout"] = settings.value("timeout", "30").
		toString().trimmed();
	      m_z3950Maps.insert
		(settings.value("name", "Z39.50 Site").toString().trimmed(),
		 hash);
	    }
	}

      settings.endGroup();
    }

  br.branch_name->clear();

  auto list(m_branches.keys());

  for(int i = 0; i < list.size(); i++)
    br.branch_name->addItem(list.at(i));

  if(br.branch_name->count() == 0)
    {
      QHash<QString, QString> hash;

      hash["branch_name"] = "local_db";
      hash["hostname"] = "127.0.0.1";
      hash["database_type"] = "sqlite";
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
  QSqlQuery query(m_db);
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
  auto caseinsensitive = al.caseinsensitive->isChecked();

  if(m_otheroptions->showMainTableImages())
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
      auto UNACCENT(unaccent());

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
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
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
		      auto word(words.takeFirst());

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
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
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
		      auto word(words.takeFirst());

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
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
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
		      auto word(words.takeFirst());

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
	    str.append("AND quantity = " +
		       al.quantity->text() + " ");

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
			 "%1.front_cover "
			 ).arg
	    (type.toLower().remove(" "));
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
    query.exec("PRAGMA case_sensitive_like = TRUE");

  query.prepare(searchstr);

  for(int i = 0; i < values.size(); i++)
    query.addBindValue(values.at(i));

  QApplication::restoreOverrideCursor();
  (void) populateTable(query, "All", NEW_PAGE, POPULATE_SEARCH);
}

void biblioteq::slotCheckout(void)
{
  QString errorstr = "";
  QString itemid = "";
  QString oid = "";
  QString type = "";
  auto row1 = bb.table->currentRow();
  auto row2 = ui.table->currentIndex();
  biblioteq_copy_editor *copyeditor = nullptr;
  biblioteq_item *item = nullptr;
  int quantity = 0;

  type = biblioteq_misc_functions::getColumnString
    (ui.table, row2.row(), ui.table->columnNumber("Type"));

  if(type == "Photograph Collection")
    {
      QMessageBox::critical(m_members_diag, tr("BiblioteQ: User Error"),
			    tr("Photographs may not be reserved."));
      QApplication::processEvents();
      return;
    }

  if(row1 > -1)
    {
      /*
      ** Has the member's membership expired?
      */

      auto expired = true;
      auto memberid = biblioteq_misc_functions::getColumnString
	(bb.table, row1, m_bbColumnHeaderIndexes.indexOf("Member ID"));

      QApplication::setOverrideCursor(Qt::WaitCursor);
      expired = biblioteq_misc_functions::hasMemberExpired
	(m_db, memberid, errorstr);
      QApplication::restoreOverrideCursor();

      if(!errorstr.isEmpty())
	addError(QString(tr("Database Error")),
		 QString(tr("Unable to determine if the membership of "
			    "the selected member has expired.")),
		 errorstr, __FILE__, __LINE__);

      if(expired || !errorstr.isEmpty())
	{
	  QMessageBox::critical(m_members_diag, tr("BiblioteQ: User Error"),
				tr("It appears that the selected member's "
				   "membership has expired."));
	  QApplication::processEvents();
	  return;
	}
    }

  if(row2.isValid())
    {
      /*
      ** Is the item available?
      */

      oid = biblioteq_misc_functions::getColumnString
	(ui.table, row2.row(), ui.table->columnNumber("MYOID"));
      QApplication::setOverrideCursor(Qt::WaitCursor);

      auto availability = biblioteq_misc_functions::getAvailability
	(oid, m_db, type, errorstr).toInt();

      QApplication::restoreOverrideCursor();

      if(!errorstr.isEmpty())
	addError(QString(tr("Database Error")),
		 QString(tr("Unable to determine the availability of "
			    "the selected item.")),
		 errorstr, __FILE__, __LINE__);

      if(availability < 1 || !errorstr.isEmpty())
	{
	  QMessageBox::critical(m_members_diag, tr("BiblioteQ: User Error"),
				tr("It appears that the item that you "
				   "selected "
				   "is not available for reservation."));
	  QApplication::processEvents();
	  return;
	}
    }

  if(row1 < 0 || !row2.isValid())
    {
      QMessageBox::critical(m_members_diag, tr("BiblioteQ: User Error"),
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
      else if(type.toLower() == "dvd")
	itemid = biblioteq_misc_functions::getColumnString
	  (ui.table, row2.row(), ui.table->columnNumber("UPC"));
      else if(type.toLower() == "grey literature")
	{
	  itemid = biblioteq_misc_functions::getColumnString
	    (ui.table, row2.row(), ui.table->columnNumber("Document ID"));
	  quantity = 1;
	}
      else if(type.toLower() == "journal" ||
	      type.toLower() == "magazine")
	itemid = biblioteq_misc_functions::getColumnString
	  (ui.table, row2.row(), ui.table->columnNumber("ISSN"));
      else if(type.toLower() == "cd")
	itemid = biblioteq_misc_functions::getColumnString
	  (ui.table, row2.row(), ui.table->columnNumber("Catalog Number"));
      else if(type.toLower() == "video game")
	itemid = biblioteq_misc_functions::getColumnString
	  (ui.table, row2.row(), ui.table->columnNumber("UPC"));
      else
	{
	  QMessageBox::critical
	    (m_members_diag, tr("BiblioteQ: User Error"),
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
  slotDisconnect();

  auto tmphash(m_branches[br.branch_name->currentText()]);

  if(tmphash.value("database_type") == "sqlite")
    {
      QFileInfo fileInfo(br.filename->text());

      if(!fileInfo.exists() || !fileInfo.isReadable() || !fileInfo.isWritable())
	{
	  QWidget *parent = this;

	  if(m_branch_diag->isVisible())
	    parent = m_branch_diag;

	  QMessageBox::critical
	    (parent, tr("BiblioteQ: User Error"),
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

  foreach(const auto &driver, QSqlDatabase::drivers())
    drivers += driver + ", ";

  if(drivers.endsWith(", "))
    drivers = drivers.mid(0, drivers.length() - 2);

  if(drivers.isEmpty())
    drivers = "N/A";

  foreach(const auto &path, QApplication::libraryPaths())
    if(path.toLower().contains("plugin"))
      {
	plugins = path;
	break;
      }

  if(plugins.isEmpty())
    plugins = "N/A";

  if(!QSqlDatabase::isDriverAvailable(str))
    {
      tmphash.clear();

      QFileInfo fileInfo("qt.conf");
      QString str("");

      if(fileInfo.isReadable() && fileInfo.size() > 0)
	str = tr("\nThe file qt.conf is present in BiblioteQ's "
		 "current working directory. Perhaps a conflict "
		 "exists.");

      QMessageBox::critical
	(m_branch_diag, tr("BiblioteQ: Database Error"),
	 tr("The selected branch's database type does not "
	    "have a driver associated with it.") + "\n" +
	 tr("The following drivers are available: ") +
	 drivers + tr(".") + "\n" +
	 tr("In addition, Qt expects plugins to exist "
	    "in: ") + plugins + tr(".") + str + "\n" +
	 tr("Please contact your administrator."));
      QApplication::processEvents();
      return;
    }

  m_db = QSqlDatabase::addDatabase(str, "Default");

  if(tmphash.value("database_type") == "sqlite")
    m_db.setDatabaseName(br.filename->text());
  else
    {
      m_db.setHostName(tmphash.value("hostname"));
      m_db.setDatabaseName(br.branch_name->currentText());
      m_db.setPort(tmphash.value("port").toInt());
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
      addError(QString(tr("Database Error")),
	       QString(tr("Unable to open a database connection "
			  "with the provided information.")),
	       m_db.lastError().text(),
	       __FILE__, __LINE__);
      QMessageBox::critical
	(m_branch_diag, tr("BiblioteQ: Database Error"),
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
	    (QString(tr("Database Error")),
	     QString(tr("The current database driver that you're using "
			"does not support transactions. "
			"Please upgrade your database and/or driver.")),
	     m_db.lastError().text(),
	     __FILE__, __LINE__);
	  QMessageBox::critical
	    (m_branch_diag, tr("BiblioteQ: Database Error"),
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
	    (m_db, br.userid->text().trimmed(), errorstr).toLower();
	  m_unaccent = biblioteq_misc_functions::hasUnaccentExtension(m_db) ?
	    "unaccent" : "";
	  QApplication::restoreOverrideCursor();

	  if(errorstr.isEmpty())
	    {
	      if(br.role->currentIndex() == 0 && m_roles.isEmpty())
		{
		  error = true;
		  QMessageBox::critical
		    (m_branch_diag, tr("BiblioteQ: User Error"),
		     QString(tr("It appears that the user ")) +
		     br.userid->text().trimmed() +
		     QString(tr(" does not have "
				"administrator privileges.")));
		  QApplication::processEvents();
		}
	      else if(br.role->currentIndex() != 0 && !m_roles.isEmpty())
		{
		  error = true;
		  QMessageBox::critical
		    (m_branch_diag, tr("BiblioteQ: User Error"),
		     tr("It appears that you are attempting to assume an "
			"administrator role in a non-administrator mode."));
		  QApplication::processEvents();
		}
	      else
		{
		  if(br.role->currentIndex() == 0) // Administrator
		    biblioteq_misc_functions::setRole
		      (m_db, errorstr, m_roles);
		  else if(br.role->currentIndex() == 1) // Guest
		    biblioteq_misc_functions::setRole
		      (m_db, errorstr, "guest");
		  else
		    biblioteq_misc_functions::setRole
		      (m_db, errorstr, "patron");

		  if(!errorstr.isEmpty())
		    {
		      error = true;
		      addError(QString(tr("Database Error")),
			       QString(tr("Unable to set "
					  "the role for ")) +
			       br.userid->text().trimmed() +
			       tr("."),
			       errorstr,
			       __FILE__, __LINE__);
		      QMessageBox::critical
			(m_branch_diag, tr("BiblioteQ: Database Error"),
			 QString(tr("Unable to set the role "
				    "for ")) +
			 br.userid->text().trimmed() +
			 tr("."));
		      QApplication::processEvents();
		    }
		}
	    }
	  else if(br.role->currentIndex() == 0) // Administrator
	    {
	      error = true;
	      addError(QString(tr("Database Error")),
		       QString(tr("Unable to determine the roles of ")) +
		       br.userid->text().trimmed() +
		       tr("."),
		       errorstr,
		       __FILE__, __LINE__);
	      QMessageBox::critical
		(m_branch_diag, tr("BiblioteQ: Database Error"),
		 QString(tr("Unable to determine the roles of ")) +
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
		  addError(QString(tr("Database Error")),
			   tr("Unable to set a guest role."),
			   errorstr,
			   __FILE__, __LINE__);
		  QMessageBox::critical
		    (m_branch_diag, tr("BiblioteQ: Database Error"),
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
		  addError(QString(tr("Database Error")),
			   QString(tr("Unable to set the role for ")) +
			   br.userid->text().trimmed() +
			   tr("."),
			   errorstr,
			   __FILE__, __LINE__);
		  QMessageBox::critical
		    (m_branch_diag, tr("BiblioteQ: Database Error"),
		     QString(tr("Unable to set the role for ")) +
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

  tmphash.clear();

  if(error)
    {
      m_db = QSqlDatabase();
      QSqlDatabase::removeDatabase("Default");
      return;
    }
  else
#ifdef Q_OS_ANDROID
    m_branch_diag->hide();
#else
    m_branch_diag->close();
#endif

  /*
  ** We've connected successfully. Let's initialize other containers and
  ** widgets.
  */

  QSettings settings;

  settings.setValue("previous_branch_name", br.branch_name->currentText());
  m_selectedBranch = m_branches[br.branch_name->currentText()];

  if(m_connected_bar_label != nullptr)
    {
      m_connected_bar_label->setPixmap(QPixmap(":/16x16/connected.png"));
      m_connected_bar_label->setToolTip(tr("Connected"));
    }

  ui.actionDatabaseSearch->setEnabled(true);
  ui.actionDisconnect->setEnabled(true);
  ui.actionRefreshTable->setEnabled(true);
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
      ui.action_Merge_SQLite_Databases->setEnabled(true);
      ui.action_Upgrade_SQLite_Schema->setEnabled(true);
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
	  (ui.menuEntriesPerPage->actions().size() - 1)->setEnabled(false);

      ui.actionChangePassword->setEnabled(true);
      disconnect(ui.table, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this,
		 SLOT(slotViewDetails(void)));
      disconnect(ui.graphicsView->scene(),
		 SIGNAL(itemDoubleClicked(void)),
		 this,
		 SLOT(slotViewDetails(void)));
      connect(ui.table, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this,
	      SLOT(slotViewDetails(void)));
      connect(ui.graphicsView->scene(), SIGNAL(itemDoubleClicked(void)), this,
	      SLOT(slotViewDetails(void)));

      /*
      ** Set the window's title.
      */

      setWindowTitle(tr("BiblioteQ: ") + m_selectedBranch.
		     value("branch_name") +
		     QString(" (%1)").arg(dbUserName()));
    }

  prepareFilter();

  if(br.role->currentIndex() == 0 || m_db.driverName() == "QSQLITE")
    {
      if(m_db.driverName() == "QSQLITE")
	{
	  /*
	  ** Add the database's information to the pulldown menu.
	  */

	  auto actions = ui.menu_Recent_SQLite_Files->actions();
	  auto exists = false;

	  for(int i = 0; i < actions.size(); i++)
	    if(actions[i]->data().toString() == br.filename->text())
	      {
		exists = true;
		break;
	      }

	  actions.clear();

	  if(!exists)
	    {
	      QSettings settings;
	      auto allKeys(settings.allKeys());
	      int index = 1;

	      for(int i = 0; i < allKeys.size(); i++)
		if(allKeys[i].startsWith("sqlite_db_"))
		  index += 1;

	      allKeys.clear();
	      settings.setValue(QString("sqlite_db_%1").arg(index),
				br.filename->text());
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
      ui.actionRequests->setData(RequestActionItems::REQUEST_SELECTED);
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

  if(ui.actionPopulateOnStart->isChecked())
    slotRefresh();

  prepareContextMenus();
}

void biblioteq::slotDisconnect(void)
{
  if(db_enumerations &&
     db_enumerations->isVisible() &&
     !db_enumerations->close())
    return;

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

  if(m_files)
    m_files->reset();

  m_membersWasRefreshed = false;
  m_roles = "";
  m_pages = 0;
  m_queryOffset = 0;

  if(m_searchQuery.isActive())
    m_searchQuery.clear();

  userinfo_diag->m_memberProperties.clear();
#ifdef Q_OS_ANDROID
  m_admin_diag->hide();
  m_all_diag->hide();
  m_customquery_diag->hide();
  m_history_diag->hide();
  m_import->hide();
  m_members_diag->hide();
#else
  m_admin_diag->close();
  m_all_diag->close();
  m_customquery_diag->close();
  m_history_diag->close();
  m_import->close();
  m_members_diag->close();
#endif
  m_unaccent.clear();
  cq.tables_t->clear();
  cq.tables_t->setColumnCount(0);
  cq.tables_t->scrollToTop();
  cq.tables_t->horizontalScrollBar()->setValue(0);
  cq.tables_t->clearSelection();
  cq.tables_t->setCurrentItem(nullptr);
  cq.query_te->clear();

  if(db_enumerations)
    db_enumerations->clear();

  resetAdminBrowser();
  resetMembersBrowser();
  ui.actionAutoPopulateOnCreation->setEnabled(false);
  ui.actionChangePassword->setEnabled(false);
  ui.actionDatabaseSearch->setEnabled(false);
  ui.actionDeleteEntry->setEnabled(false);
  ui.actionDisconnect->setEnabled(false);
  ui.actionDuplicateEntry->setEnabled(false);
  ui.actionImportCSV->setEnabled(false);
  ui.actionMembersBrowser->setEnabled(false);
  ui.actionModifyEntry->setEnabled(false);
  ui.actionRefreshTable->setEnabled(false);
  ui.actionReservationHistory->setEnabled(false);
  ui.actionViewDetails->setEnabled(false);
  ui.action_Merge_SQLite_Databases->setEnabled(false);
  ui.action_Upgrade_SQLite_Schema->setEnabled(false);
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
  ui.actionRequests->setData(RequestActionItems::INACTIVE);
  ui.actionRequests->setEnabled(false);
  ui.actionRequests->setIcon(QIcon(":/32x32/request.png"));
  ui.actionRequests->setToolTip(tr("Item Requests (PostgreSQL)"));
  ui.action_Database_Enumerations->setEnabled(false);
  ui.graphicsView->scene()->clear();
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

  if(m_connected_bar_label != nullptr)
    {
      m_connected_bar_label->setPixmap
	(QPixmap(":/16x16/disconnected.png"));
      m_connected_bar_label->setToolTip(tr("Disconnected"));
    }

  if(m_status_bar_label != nullptr)
    {
      m_status_bar_label->setPixmap(QPixmap(":/16x16/lock.png"));
      m_status_bar_label->setToolTip(tr("Standard User Mode"));
    }

  if(ui.actionResetErrorLogOnDisconnect->isChecked())
    slotResetErrorLog();

  ui.graphicsView->scene()->clear();
  ui.graphicsView->resetTransform();
  ui.graphicsView->verticalScrollBar()->setValue(0);
  ui.graphicsView->horizontalScrollBar()->setValue(0);
  ui.nextPageButton->setEnabled(false);
  ui.pagesLabel->setText(tr("1"));
  ui.previousPageButton->setEnabled(false);
  ui.table->resetTable(dbUserName(), m_previousTypeFilter, m_roles);
  ui.itemsCountLabel->setText(tr("0 Results"));
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

  if(QSqlDatabase::contains("Default"))
    QSqlDatabase::removeDatabase("Default");

  setWindowTitle(tr("BiblioteQ"));
}

void biblioteq::slotDisplaySummary(void)
{
  QImage backImage;
  QImage frontImage;
  QString oid = "";
  QString summary = "";
  QString tmpstr = "";
  QString type = "";
  int i = 0;

  /*
  ** Display a preview.
  */

  if(ui.itemSummary->width() > 0 && ui.table->currentRow() > -1)
    {
      i = ui.table->currentRow();
      oid = biblioteq_misc_functions::getColumnString
	(ui.table, i, ui.table->columnNumber("MYOID"));

      if(ui.stackedWidget->currentIndex() == 1)
	{
	  /*
	  ** This method is also called by slotSceneSelectionChanged().
	  */

	  QPainterPath painterPath;
	  auto items(ui.graphicsView->scene()->items());
	  auto tableItems(ui.table->selectedItems());

	  for(int ii = 0; ii < tableItems.size(); ii++)
	    {
	      auto oid = biblioteq_misc_functions::getColumnString
		(ui.table,
		 tableItems.at(ii)->row(),
		 ui.table->columnNumber("MYOID"));
	      auto type =  biblioteq_misc_functions::getColumnString
		(ui.table,
		 tableItems.at(ii)->row(),
		 ui.table->columnNumber("Type")).remove(' ').toLower();

	      for(int jj = 0; jj < items.size(); jj++)
		if(oid == items.at(jj)->data(0).toString() &&
		   type == items.at(jj)->data(1).toString())
		  {
		    QRectF rect;

		    rect.setTopLeft(items.at(jj)->scenePos());
		    rect.setWidth(126);
		    rect.setHeight(187);
		    painterPath.addRect(rect);
		  }
		else
		  items.at(jj)->setSelected(false);
	    }

	  items.clear();
	  ui.graphicsView->scene()->setSelectionArea(painterPath);
	}

      type = biblioteq_misc_functions::getColumnString
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
	    summary += "<br>" + QString(tr("%1 File(s)")).arg(tmpstr);

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
	    summary += "<br>" + QString(tr("%1 Photograph(s)")).arg(tmpstr);

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
      ** The size of no_image.png is 126x187.
      */

      if(frontImage.isNull())
	frontImage = QImage(":/no_image.png");

      if(!frontImage.isNull())
	frontImage = frontImage.scaled
	  (126, 187, Qt::KeepAspectRatio, Qt::SmoothTransformation);

      if(type != "Grey Literature" &&
	 type != "Photograph Collection")
	{
	  if(backImage.isNull())
	    backImage = QImage(":/no_image.png");

	  if(!backImage.isNull())
	    backImage = backImage.scaled
	      (126, 187, Qt::KeepAspectRatio, Qt::SmoothTransformation);
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
	      ui.backImage->setVisible(true);
	      ui.backImage->setPixmap(QPixmap::fromImage(backImage));
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

	  foreach(const auto &f, QStringList()
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

void biblioteq::slotGraphicsSceneEnterKeyPressed(void)
{
  slotMainTableEnterKeyPressed();
}

void biblioteq::slotGreyLiteratureSearch(void)
{
  biblioteq_grey_literature *gl = nullptr;

  foreach(auto w, QApplication::topLevelWidgets())
    {
      auto g = qobject_cast<biblioteq_grey_literature *> (w);

      if(g && g->getID() == "search")
	{
	  gl = g;
	  break;
	}
    }

  if(!gl)
    {
      gl = new biblioteq_grey_literature(this, "search", QModelIndex());
      gl->search();
    }

#ifdef Q_OS_ANDROID
  gl->showMaximized();
#else
  gl->showNormal();
#endif
  gl->activateWindow();
  gl->raise();
}

void biblioteq::slotInsertGreyLiterature(void)
{
  QString id("");
  biblioteq_grey_literature *gl = nullptr;

  m_idCt += 1;
  id = QString("insert_%1").arg(m_idCt);
  gl = new biblioteq_grey_literature(this, id, QModelIndex());
  gl->insert();
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
		   toString().trimmed());

      if(!color.isValid())
	color = Qt::white;

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
  QSqlQuery query(m_db);
  QString fieldname = "";
  QString str = "";
  QVariant var;
  auto row = bb.table->currentRow();
  int i = 0;

  if(row < 0)
    {
      QMessageBox::critical(m_members_diag, tr("BiblioteQ: User Error"),
			    tr("Please select a member to modify."));
      QApplication::processEvents();
      return;
    }

  str = biblioteq_misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("Member ID"));
  query.setForwardOnly(true);
  query.prepare("SELECT * FROM member WHERE memberid = ?");
  query.bindValue(0, str);
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!query.exec() || !query.next())
    {
      QApplication::restoreOverrideCursor();
      addError(QString(tr("Database Error")),
	       QString(tr("Unable to retrieve the selected member's "
			  "information.")),
	       query.lastError().text(),
	       __FILE__, __LINE__);

      if(userinfo_diag->isVisible())
	QMessageBox::critical(userinfo_diag, tr("BiblioteQ: Database Error"),
			      tr("Unable to retrieve the selected member's "
				 "information."));
      else
	QMessageBox::critical(m_members_diag, tr("BiblioteQ: Database Error"),
			      tr("Unable to retrieve the selected member's "
				 "information."));

      QApplication::processEvents();
      return;
    }
  else
    {
      QApplication::restoreOverrideCursor();

      auto record(query.record());

      for(i = 0; i < record.count(); i++)
	{
	  str = query.value(i).toString().trimmed();
	  var = record.field(i).value();
	  fieldname = record.fieldName(i);

	  if(fieldname == "memberid")
	    userinfo_diag->m_userinfo.memberid->setText
	      (var.toString().trimmed());
	  else if(fieldname == "membersince")
	    userinfo_diag->m_userinfo.membersince->setDate
	      (QDate::fromString(var.toString().trimmed(), "MM/dd/yyyy"));
	  else if(fieldname == "dob")
	    userinfo_diag->m_userinfo.dob->setDate
	      (QDate::fromString(var.toString().trimmed(), "MM/dd/yyyy"));
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
	  else if(fieldname == "first_name")
	    userinfo_diag->m_userinfo.firstName->setText
	      (var.toString().trimmed());
	  else if(fieldname == "middle_init")
	    userinfo_diag->m_userinfo.middle->setText
	      (var.toString().trimmed());
	  else if(fieldname == "last_name")
	    userinfo_diag->m_userinfo.lastName->setText
	      (var.toString().trimmed());
	  else if(fieldname == "telephone_num")
	    userinfo_diag->m_userinfo.telephoneNumber->setText
	      (var.toString().trimmed());
	  else if(fieldname == "street")
	    userinfo_diag->m_userinfo.street->setText
	      (var.toString().trimmed());
	  else if(fieldname == "city")
	    userinfo_diag->m_userinfo.city->setText(var.toString().trimmed());
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
	  else if(fieldname == "zip")
	    userinfo_diag->m_userinfo.zip->setText(var.toString().trimmed());
	  else if(fieldname == "email")
	    userinfo_diag->m_userinfo.email->setText(var.toString().trimmed());
	  else if(fieldname == "expiration_date")
	    userinfo_diag->m_userinfo.expirationdate->setDate
	      (QDate::fromString(var.toString().trimmed(), "MM/dd/yyyy"));
	  else if(fieldname == "overdue_fees")
	    userinfo_diag->m_userinfo.overduefees->setValue(var.toDouble());
	  else if(fieldname == "comments")
	    userinfo_diag->m_userinfo.comments->setPlainText
	      (var.toString().trimmed());
	  else if(fieldname == "general_registration_number")
	    userinfo_diag->m_userinfo.generalregistrationnumber->setText
	      (var.toString().trimmed());
	  else if(fieldname == "memberclass")
	    userinfo_diag->m_userinfo.memberclass->setText
	      (var.toString().trimmed());
	  else if(fieldname == "maximum_reserved_books")
	    userinfo_diag->m_userinfo.maximum_reserved_books->setValue
	      (var.toInt());

	  if(fieldname.contains("dob") ||
	     fieldname.contains("date") ||
	     fieldname.contains("membersince"))
	    userinfo_diag->m_memberProperties[fieldname] =
	      QDate::fromString(var.toString().trimmed(), "MM/dd/yyyy").
	      toString(Qt::ISODate);
	  else if(fieldname == "overdue_fees")
	    userinfo_diag->m_memberProperties[fieldname] =
	      userinfo_diag->m_userinfo.overduefees->text();
	  else if(fieldname == "maximum_reserved_books")
	    userinfo_diag->m_memberProperties[fieldname] =
	      userinfo_diag->m_userinfo.maximum_reserved_books->text();
	  else
	    userinfo_diag->m_memberProperties[fieldname] =
	      var.toString().trimmed();
	}

      foreach(auto textfield, userinfo_diag->findChildren<QLineEdit *> ())
	textfield->setCursorPosition(0);
    }

  userinfo_diag->m_userinfo.memberid->setReadOnly(true);
  userinfo_diag->m_userinfo.prevTool->setVisible(true);
  userinfo_diag->m_userinfo.nextTool->setVisible(true);
  userinfo_diag->setWindowTitle(tr("BiblioteQ: Modify Member"));
  m_engUserinfoTitle = "Modify Member";
  userinfo_diag->m_userinfo.membersince->setMaximumDate(QDate::currentDate());

  if(!userinfo_diag->isVisible())
    userinfo_diag->m_userinfo.tabWidget->setCurrentIndex(0);

  userinfo_diag->m_userinfo.membersince->setFocus();
  userinfo_diag->m_userinfo.memberid->setPalette
    (userinfo_diag->m_userinfo.telephoneNumber->palette());
  userinfo_diag->updateGeometry();
  userinfo_diag->show();
}

void biblioteq::slotOpenOnlineDocumentation(void)
{
  QDesktopServices::openUrl
    (QUrl("https://github.com/textbrowser/biblioteq/"
	  "blob/master/Documentation/BiblioteQ.pdf"));
}

void biblioteq::slotOpenPDFFiles(void)
{
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
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
		    QMessageBox::Yes | QMessageBox::No,
		    QMessageBox::No) == QMessageBox::No)
	  {
	    QApplication::processEvents();
	    return;
	  }

      QApplication::setOverrideCursor(Qt::WaitCursor);

      for(int i = 0; i < dialog.selectedFiles().size(); i++)
	{
	  auto reader = new biblioteq_pdfreader(this);

	  reader->load(dialog.selectedFiles().at(i));
	  biblioteq_misc_functions::center(reader, this);
	  reader->show();
	}

      QApplication::restoreOverrideCursor();
    }

  QApplication::processEvents();
#endif
}

void biblioteq::slotOtherOptionsSaved(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  foreach(auto widget, QApplication::topLevelWidgets())
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

  if(m_otheroptions->showMainTableImages())
    ui.table->setIconSize(QSize(64, 94));
  else
    ui.table->setIconSize(QSize(0, 0));

  QFontMetrics fontMetrics(ui.table->font());

  for(int i = 0; i < ui.table->rowCount(); i++)
    ui.table->setRowHeight
      (i, qMax(fontMetrics.height() + 10, ui.table->iconSize().height()));

  QApplication::restoreOverrideCursor();
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

	if(bb.filtertype->currentIndex() == 0) // Member ID
	  {
	    str.append("LOWER(memberid) LIKE " + E + "'%' || ");
	    str.append("LOWER(?)");
	  }
	else
	  {
	    str.append("LOWER(last_name) LIKE " + E + "'%' || ");
	    str.append("LOWER(?)");
	  }

	str.append("|| '%' ");
	query.prepare(str);
	query.bindValue
	  (0, biblioteq_myqstring::escape(bb.filter->text().trimmed()));
      }
    else
      query.prepare(str);

    if(query.exec() && query.next())
      {
	auto pages = qCeil
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

      if(bb.filtertype->currentIndex() == 0) // Member ID
	{
	  str.append("LOWER(member.memberid) LIKE " + E + "'%' || ");
	  str.append("LOWER(?)");
	}
      else
	{
	  str.append("LOWER(member.last_name) LIKE " + E + "'%' || ");
	  str.append("LOWER(?)");
	}

      str.append("|| '%' ");
    }

  str.append("GROUP BY "
	     "member.memberid, "
	     "member.first_name, "
	     "member.last_name, "
	     "member.email, "
	     "member.telephone_num, "
	     "address, "
	     "member.membersince, "
	     "member.expiration_date ");
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
      addError(QString(tr("Database Error")),
	       QString(tr("Unable to retrieve member data for table "
			  "populating.")),
	       query.lastError().text(),
	       __FILE__, __LINE__);
      QMessageBox::critical(m_members_diag, tr("BiblioteQ: Database Error"),
			    tr("Unable to retrieve member data for "
			       "table populating."));
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
	  auto record(query.record());
	  qint64 total = 0;

	  for(j = 0; j < record.count(); j++)
	    {
	      if(record.fieldName(j).contains("date") ||
		 record.fieldName(j).contains("membersince"))
		{
		  auto date(QDate::fromString(query.value(j).toString().
					       trimmed(),
					       "MM/dd/yyyy"));

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
	      item->setText(str);
	      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
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
  QStringList list;
  QTableWidgetItem *item = nullptr;
  int i = -1;
  int j = 0;

  query.prepare("SELECT username, LOWER(roles) FROM admin ORDER BY username");
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!query.exec())
    {
      QApplication::restoreOverrideCursor();
      addError(QString(tr("Database Error")),
	       QString(tr("Unable to retrieve administrator data for table "
			  "populating.")),
	       query.lastError().text(),
	       __FILE__, __LINE__);
      QMessageBox::critical(m_admin_diag, tr("BiblioteQ: Database Error"),
			    tr("Unable to retrieve administrator "
			       "data for table populating."));
      QApplication::processEvents();
      return;
    }

  QApplication::restoreOverrideCursor();
  resetAdminBrowser();
  ab.table->setRowCount(query.size());
  progress->setModal(true);
  progress->setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress->setLabelText(tr("Populating the table..."));
  progress->setMaximum(query.size());
  progress->setMinimum(0);
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
	  item->setText(str);
	  item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	  str = query.value(1).toString().trimmed();
	  ab.table->setItem(i, AdminSetupColumns::ID, item);

	  for(j = AdminSetupColumns::ADMINISTRATOR;
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

		if(str.toLower().contains(columnname))
		  checkBox->setChecked(true);

		if(query.value(0).toString().trimmed() == getAdminID())
		  checkBox->setEnabled(false);
		else
		  connect(checkBox, SIGNAL(stateChanged(int)), this,
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

void biblioteq::slotRefreshCustomQuery(void)
{
  if(!m_db.isOpen())
    return;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  QSqlField field;
  QSqlRecord rec;
  QStringList list;
  QTreeWidgetItem *item1 = nullptr;
  QTreeWidgetItem *item2 = nullptr;
  int i = 0;
  int j = 0;

  cq.tables_t->clear();

  if(m_db.driverName() == "QSQLITE")
    list << "book"
	 << "book_binding_types"
	 << "book_copy_info"
	 << "book_files"
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
	 << "grey_literature_files"
	 << "grey_literature_types"
	 << "item_borrower"
	 << "journal"
	 << "journal_copy_info"
	 << "journal_files"
	 << "languages"
	 << "locations"
	 << "magazine"
	 << "magazine_copy_info"
	 << "magazine_files"
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
	 << "book_files"
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
	 << "grey_literature_files"
	 << "grey_literature_types"
	 << "item_borrower"
	 << "item_request"
	 << "journal"
	 << "journal_copy_info"
	 << "journal_files"
	 << "languages"
	 << "locations"
	 << "magazine"
	 << "magazine_copy_info"
	 << "magazine_files"
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
    {
      item1 = new QTreeWidgetItem(cq.tables_t);
      item1->setText(0, list[i]);
      rec = m_db.record(list[i]);

      for(j = 0; j < rec.count(); j++)
	{
	  item2 = new QTreeWidgetItem(item1);
	  field = rec.field(rec.fieldName(j));
	  item2->setText(1, rec.fieldName(j));
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	  item2->setText(2, QMetaType(rec.field(j).metaType().id()).name());
#else
	  item2->setText(2, QVariant::typeToName(field.type()));
#endif

	  if(field.requiredStatus() == QSqlField::Required)
	    item2->setText(3, tr("No"));
	  else
	    item2->setText(3, "");
	}
    }

  for(i = 0; i < cq.tables_t->columnCount() - 1; i++)
    cq.tables_t->resizeColumnToContents(i);

  cq.tables_t->setSortingEnabled(true);
  cq.tables_t->sortByColumn(0, Qt::AscendingOrder);
  QApplication::restoreOverrideCursor();
}

void biblioteq::slotRemoveMember(void)
{
  QMap<QString, qint64> counts;
  QSqlQuery query(m_db);
  QString errorstr = "";
  QString memberid = "";
  auto row = bb.table->currentRow();

  if(row < 0)
    {
      QMessageBox::critical(m_members_diag, tr("BiblioteQ: User Error"),
			    tr("Please select a member to delete."));
      QApplication::processEvents();
      return;
    }

  memberid = biblioteq_misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("Member ID"));
  QApplication::setOverrideCursor(Qt::WaitCursor);
  counts = biblioteq_misc_functions::getItemsReservedCounts
    (m_db, memberid, errorstr);
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    {
      addError(QString(tr("Database Error")),
	       QString(tr("Unable to determine the number of items that "
			  "are reserved by the selected member.")),
	       errorstr, __FILE__, __LINE__);
      QMessageBox::critical
	(m_members_diag, tr("BiblioteQ: Database Error"),
	 tr("Unable to determine the number of items that "
	    "are reserved by the selected member."));
      QApplication::processEvents();
      return;
    }

  if(counts.value("numtotal") != 0)
    {
      QMessageBox::critical
	(m_members_diag, tr("BiblioteQ: User Error"),
	 tr("You may not remove a member that has reserved "
	    "items."));
      QApplication::processEvents();
      return;
    }

  if(QMessageBox::question(m_members_diag, tr("BiblioteQ: Question"),
			   tr("Are you sure that you wish to delete the "
			      "selected member (%1)?").arg(memberid),
			   QMessageBox::Yes | QMessageBox::No,
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
	(QString(tr("Database Error")),
	 QString(tr("Unable to create a database transaction.")),
	 m_db.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical
	(m_members_diag, tr("BiblioteQ: Database Error"),
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
	  (QString(tr("Database Error")), QString(tr("Rollback failure.")),
	   m_db.lastError().text(), __FILE__, __LINE__);

      QApplication::restoreOverrideCursor();
      addError(QString(tr("Database Error")),
	       QString(tr("Unable to remove the selected member.")),
	       query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(m_members_diag, tr("BiblioteQ: Database Error"),
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
	    (QString(tr("Database Error")),
	     QString(tr("Unable to remove the patron account ")) +
	     memberid + tr("."),
	     errorstr, __FILE__, __LINE__);

	  if(!m_db.rollback())
	    addError
	      (QString(tr("Database Error")),
	       QString(tr("Rollback failure.")),
	       m_db.lastError().text(), __FILE__, __LINE__);

	  QApplication::restoreOverrideCursor();
	  QMessageBox::critical
	    (m_members_diag,
	     tr("BiblioteQ: Database Error"),
	     QString(tr("Unable to remove the patron account ")) +
	     memberid + tr("."));
	  QApplication::processEvents();
	}
      else
	{
	  if(!m_db.commit())
	    {
	      addError
		(QString(tr("Database Error")),
		 QString(tr("Unable to commit the current database "
			    "transaction.")),
		 m_db.lastError().text(), __FILE__,
		 __LINE__);
	      m_db.rollback();
	      QApplication::restoreOverrideCursor();
	      QMessageBox::critical(m_members_diag,
				    tr("BiblioteQ: Database Error"),
				    tr("Unable to commit the current "
				       "database transaction."));
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
  ** This method is used for canceling request(s), requesting item(s), or
  ** returning selected item(s).
  */

  auto list(ui.table->selectionModel()->selectedRows());
  auto task = RequestActionItems(ui.actionRequests->data().toInt());

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
  auto error = false;
  auto now(QDate::currentDate());
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
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.show();
  progress.repaint();
  QApplication::processEvents();

  foreach(const auto &index, list)
    {
      QSqlQuery query(m_db);

      ct += 1;
      i = index.row();
      itemType = biblioteq_misc_functions::getColumnString
	(ui.table, i, ui.table->columnNumber("Type"));

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
	      query.addBindValue(now.toString("MM/dd/yyyy"));
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
		  query.addBindValue(now.toString("MM/dd/yyyy"));
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
		numcompleted += 1;

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
    if(task == RequestActionItems::CANCEL_REQUESTED ||
       task == RequestActionItems::RETURN_RESERVED)
      slotRefresh();
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
      if(ab.table->item(i, AdminSetupColumns::ID)->text().trimmed().isEmpty())
	continue;

      if(!(qobject_cast<QCheckBox *>
	   (ab.table->cellWidget(i, AdminSetupColumns::ADMINISTRATOR))->
	   isChecked() ||
	   qobject_cast<QCheckBox *>
	   (ab.table->cellWidget(i, AdminSetupColumns::CIRCULATION))->
	   isChecked() ||
	   qobject_cast<QCheckBox *>
	   (ab.table->cellWidget(i, AdminSetupColumns::LIBRARIAN))->
	   isChecked() ||
	   qobject_cast<QCheckBox *>
	   (ab.table->cellWidget(i, AdminSetupColumns::MEMBERSHIP))->
	   isChecked()))
	{
	  tmplist.clear();
	  ab.table->selectRow(i);
	  ab.table->horizontalScrollBar()->setValue(i);
	  QMessageBox::critical
	    (m_admin_diag, tr("BiblioteQ: User Error"),
	     tr("Administrators must belong to at least one category."));
	  QApplication::processEvents();
	  return;
	}

      if(!tmplist.contains(ab.table->item(i, AdminSetupColumns::ID)->
			   text().toLower().trimmed()))
	tmplist.append(ab.table->item(i, AdminSetupColumns::ID)->
		       text().toLower().trimmed());
      else
	{
	  tmplist.clear();
	  ab.table->selectRow(i);
	  ab.table->horizontalScrollBar()->setValue(i);
	  QMessageBox::critical
	    (m_admin_diag, tr("BiblioteQ: User Error"),
	     tr("Duplicate administrator ids are not allowed."));
	  QApplication::processEvents();
	  return;
	}
    }

  tmplist.clear();
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!m_db.transaction())
    {
      QApplication::restoreOverrideCursor();
      addError
	(QString(tr("Database Error")),
	 QString(tr("Unable to create a database transaction.")),
	 m_db.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical
	(m_admin_diag, tr("BiblioteQ: Database Error"),
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
	  addError(QString(tr("Database Error")),
		   QString(tr("An error occurred while attempting to "
			      "remove ")) + m_deletedAdmins[i].toLower() +
		   QString(tr(".")),
		   query.lastError().text(), __FILE__, __LINE__);
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
	    (QString(tr("Database Error")),
	     QString(tr("An error occurred while attempting to "
			"remove the database account ")) +
	     m_deletedAdmins[i].toLower() + QString(tr(".")),
	     errorstr, __FILE__, __LINE__);
	  goto db_rollback;
	}
    }

  QApplication::restoreOverrideCursor();
  progress.setCancelButton(nullptr);
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.setLabelText(tr("Saving administrator information..."));
  progress.setMaximum(ab.table->rowCount());
  progress.setMinimum(0);
  progress.show();
  progress.repaint();
  QApplication::processEvents();

  /*
  ** Add or modify administrators.
  */

  for(i = 0; i < ab.table->rowCount(); i++)
    {
      str = "";
      adminStr = ab.table->item(i, AdminSetupColumns::ID)->
	text().toLower().trimmed();

      if(i + 1 <= progress.maximum())
	progress.setValue(i + 1);

      progress.repaint();
      QApplication::processEvents();

      if(adminStr.isEmpty())
	continue; // Ignore empty administrator ids.
      else if(adminStr == getAdminID())
	continue; // Ignore current administrator.

      if((qobject_cast<QCheckBox *> (ab.table->cellWidget(i,
							  AdminSetupColumns::
							  ADMINISTRATOR)))->
	 isChecked())
	str = "administrator";
      else
	for(j = AdminSetupColumns::CIRCULATION;
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
	    (QString(tr("Database Error")),
	     QString(tr("The function biblioteq_misc_functions::"
			"userExists() failed "
			"for ")) + adminStr + QString(tr(".")),
	     errorstr, __FILE__, __LINE__);
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
	    (QString(tr("Database Error")),
	     QString(tr("Unable to create or update the administrator entry "
			"for ")) + adminStr + QString(tr(".")),
	     query.lastError().text(), __FILE__, __LINE__);
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
		(QString(tr("Database Error")),
		 QString(tr("An error occurred while attempting to "
			    "create a database account for ")) + adminStr +
		 QString(tr(".")),
		 errorstr, __FILE__, __LINE__);
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
		(QString(tr("Database Error")),
		 QString(tr("An error occurred while attempting to "
			    "revoke privileges from ")) + adminStr +
		 QString(tr(".")),
		 errorstr, __FILE__, __LINE__);
	      goto db_rollback;
	    }

	  biblioteq_misc_functions::grantPrivs(adminStr, str, m_db, errorstr);

	  if(!errorstr.isEmpty())
	    {
	      progress.close();
	      addError
		(QString(tr("Database Error")),
		 QString(tr("An error occurred while attempting to "
			    "grant privileges to ")) + adminStr +
		 QString(tr(".")),
		 errorstr, __FILE__, __LINE__);
	      goto db_rollback;
	    }
	}
    }

  progress.close();
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!m_db.commit())
    {
      addError
	(QString(tr("Database Error")),
	 QString(tr("Unable to commit the current database "
		    "transaction.")),
	 m_db.lastError().text(), __FILE__,
	 __LINE__);
      m_db.rollback();
      QApplication::restoreOverrideCursor();
      QMessageBox::critical(m_admin_diag,
			    tr("BiblioteQ: Database Error"),
			    tr("Unable to commit the current "
			       "database transaction."));
      QApplication::processEvents();
      return;
    }

  QApplication::restoreOverrideCursor();
  m_deletedAdmins.clear();

  if(adminCreated)
    {
      QMessageBox::information
	(m_admin_diag, tr("BiblioteQ: Information"),
	 tr("Please notify new administrators that their "
	    "default password has been set "
	    "to tempPass."));
      QApplication::processEvents();
    }

  slotRefreshAdminList();
  return;

 db_rollback:

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!m_db.rollback())
    addError(QString(tr("Database Error")), QString(tr("Rollback failure.")),
	     m_db.lastError().text(), __FILE__, __LINE__);

  QApplication::restoreOverrideCursor();
  QMessageBox::critical(m_admin_diag, tr("BiblioteQ: Database Error"),
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

  if(!isFullScreen())
    {
      if(biblioteq_misc_functions::isGnome())
	settings.setValue
	  ("members_window_geometry", m_members_diag->geometry());
      else
	settings.setValue
	  ("members_window_geometry", m_members_diag->saveGeometry());
    }

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
	{
	  if(biblioteq_misc_functions::isGnome())
	    settings.setValue("main_window_geometry", geometry());
	  else
	    settings.setValue("main_window_geometry", saveGeometry());
	}
    }
  else
    settings.remove("main_window_geometry");

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

  settings.sync();
}

void biblioteq::slotSaveUser(void)
{
  QSqlQuery query(m_db);
  QString checksum = "";
  QString errorstr = "";
  QString str = "";
  auto exists = false;
  auto row = bb.table->currentRow();
  int i = 0;

  str = userinfo_diag->m_userinfo.memberid->text().trimmed();
  userinfo_diag->m_userinfo.memberid->setText(str);
  str = userinfo_diag->m_userinfo.firstName->text().trimmed();
  userinfo_diag->m_userinfo.firstName->setText(str);
  str = userinfo_diag->m_userinfo.lastName->text().trimmed();
  userinfo_diag->m_userinfo.lastName->setText(str);
  str = userinfo_diag->m_userinfo.middle->text().trimmed();
  userinfo_diag->m_userinfo.middle->setText(str);
  str = userinfo_diag->m_userinfo.street->text().trimmed();
  userinfo_diag->m_userinfo.street->setText(str);
  str = userinfo_diag->m_userinfo.city->text().trimmed();
  userinfo_diag->m_userinfo.city->setText(str);
  str = userinfo_diag->m_userinfo.zip->text().trimmed();
  userinfo_diag->m_userinfo.zip->setText(str);
  str = userinfo_diag->m_userinfo.telephoneNumber->text().trimmed();
  userinfo_diag->m_userinfo.telephoneNumber->setText(str);
  str = userinfo_diag->m_userinfo.email->text().trimmed();
  userinfo_diag->m_userinfo.email->setText(str);

  if(m_engUserinfoTitle.contains("New"))
    {
      if(userinfo_diag->m_userinfo.memberid->text().length() < 5)
	{
	  QMessageBox::critical(userinfo_diag, tr("BiblioteQ: User Error"),
				tr("The Member ID must be at least five "
				   "characters long."));
	  QApplication::processEvents();
	  userinfo_diag->m_userinfo.memberid->setFocus();
	  return;
	}

      QApplication::setOverrideCursor(Qt::WaitCursor);

      auto uexists = biblioteq_misc_functions::userExists
	(userinfo_diag->m_userinfo.memberid->text(), m_db, errorstr);

      QApplication::restoreOverrideCursor();

      if(uexists)
	{
	  QMessageBox::critical
	    (userinfo_diag, tr("BiblioteQ: User Error"),
	     QString(tr("The Member ID ")) +
	     userinfo_diag->m_userinfo.memberid->text() +
	     QString(tr(" already exists.")));
	  QApplication::processEvents();
	  userinfo_diag->m_userinfo.memberid->setFocus();
	  return;
	}
    }

  if(userinfo_diag->m_userinfo.firstName->text().isEmpty())
    {
      QMessageBox::critical(userinfo_diag, tr("BiblioteQ: User Error"),
			    tr("Please provide a valid First Name."));
      QApplication::processEvents();
      userinfo_diag->m_userinfo.firstName->setFocus();
      return;
    }

  if(userinfo_diag->m_userinfo.lastName->text().isEmpty())
    {
      QMessageBox::critical(userinfo_diag, tr("BiblioteQ: User Error"),
			    tr("Please provide a valid Last Name."));
      QApplication::processEvents();
      userinfo_diag->m_userinfo.lastName->setFocus();
      return;
    }

  if(userinfo_diag->m_userinfo.street->text().isEmpty())
    {
      QMessageBox::critical(userinfo_diag, tr("BiblioteQ: User Error"),
			    tr("Please provide a valid Street."));
      QApplication::processEvents();
      userinfo_diag->m_userinfo.street->setFocus();
      return;
    }

  if(userinfo_diag->m_userinfo.city->text().isEmpty())
    {
      QMessageBox::critical(userinfo_diag, tr("BiblioteQ: User Error"),
			    tr("Please provide a valid City."));
      QApplication::processEvents();
      userinfo_diag->m_userinfo.city->setFocus();
      return;
    }

  if(userinfo_diag->m_userinfo.zip->text().isEmpty())
    {
      QMessageBox::critical(userinfo_diag, tr("BiblioteQ: User Error"),
			    tr("Please provide a ZIP Code."));
      QApplication::processEvents();
      userinfo_diag->m_userinfo.zip->setFocus();
      return;
    }

  checksum.append(userinfo_diag->m_userinfo.dob->date().
		  toString("MM/dd/yyyy"));
  checksum.append(userinfo_diag->m_userinfo.sex->currentText());
  checksum.append(userinfo_diag->m_userinfo.firstName->text());
  checksum.append(userinfo_diag->m_userinfo.middle->text());
  checksum.append(userinfo_diag->m_userinfo.lastName->text());
  checksum.append(userinfo_diag->m_userinfo.street->text());
  checksum.append(userinfo_diag->m_userinfo.city->text());
  checksum.append(userinfo_diag->m_userinfo.state->currentText());
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
      addError(QString(tr("Database Error")),
	       QString(tr("Unable to determine the uniqueness of the "
			  "proposed member.")),
	       errorstr, __FILE__, __LINE__);
      QMessageBox::critical(userinfo_diag, tr("BiblioteQ: Database Error"),
			    tr("Unable to determine the uniqueness of "
			       "the proposed member."));
      QApplication::processEvents();
      return;
    }

  if(exists)
    {
      QMessageBox::critical(userinfo_diag, tr("BiblioteQ: User Error"),
			    tr("An identical member already exists."));
      QApplication::processEvents();
      return;
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!m_db.transaction())
    {
      QApplication::restoreOverrideCursor();
      addError
	(QString(tr("Database Error")),
	 QString(tr("Unable to create a database transaction.")),
	 m_db.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical
	(userinfo_diag, tr("BiblioteQ: Database Error"),
	 tr("Unable to create a database transaction."));
      QApplication::processEvents();
      return;
    }

  QApplication::restoreOverrideCursor();

  if(m_engUserinfoTitle.contains("New"))
    {
      query.prepare("INSERT INTO member "
		    "(memberid, membersince, dob, sex, "
		    "first_name, middle_init, last_name, "
		    "telephone_num, street, city, "
		    "state_abbr, zip, email, expiration_date, overdue_fees, "
		    "comments, general_registration_number, memberclass, "
		    "maximum_reserved_books) "
		    "VALUES "
		    "(?, ?, ?, ?, "
		    "?, ?, ?, ?, "
		    "?, ?, ?, ?, ?, ?, "
		    "?, ?, ?, ?, ?)");
      query.bindValue(0, userinfo_diag->m_userinfo.memberid->text().trimmed());
      query.bindValue(1, userinfo_diag->m_userinfo.membersince->
		      date().toString("MM/dd/yyyy"));
      query.bindValue(2, userinfo_diag->m_userinfo.dob->date().
		      toString("MM/dd/yyyy"));
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
		      date().toString("MM/dd/yyyy"));
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
		    "maximum_reserved_books = ? "
		    "WHERE memberid = ?");
      query.bindValue(0, userinfo_diag->m_userinfo.membersince->date().
		      toString("MM/dd/yyyy"));
      query.bindValue(1, userinfo_diag->m_userinfo.dob->date().
		      toString("MM/dd/yyyy"));
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
		      date().toString("MM/dd/yyyy"));
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
	(18, userinfo_diag->m_userinfo.memberid->text().trimmed());
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!query.exec())
    {
      if(!m_db.rollback())
	addError
	  (QString(tr("Database Error")), QString(tr("Rollback failure.")),
	   m_db.lastError().text(), __FILE__, __LINE__);

      QApplication::restoreOverrideCursor();
      addError(QString(tr("Database Error")),
	       QString(tr("Unable to save the member's information.")),
	       query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(userinfo_diag, tr("BiblioteQ: Database Error"),
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
		  (QString(tr("Database Error")),
		   QString(tr("Rollback failure.")),
		   m_db.lastError().text(), __FILE__, __LINE__);

	      QApplication::restoreOverrideCursor();
	      addError
		(QString(tr("Database Error")),
		 QString(tr("An error occurred while attempting to "
			    "create a database account "
			    "for the new member.")),
		 errorstr, __FILE__, __LINE__);
	      QMessageBox::critical
		(userinfo_diag,
		 tr("BiblioteQ: Database Error"),
		 tr("An error occurred while attempting to "
		    "create a database account "
		    "for the new member."));
	      QApplication::processEvents();
	      return;
	    }
	  else
	    {
	      if(!m_db.commit())
		{
		  addError
		    (QString(tr("Database Error")),
		     QString(tr("Unable to commit the current database "
				"transaction.")),
		     m_db.lastError().text(), __FILE__,
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
		  (QString(tr("Database Error")),
		   QString(tr("Rollback failure.")),
		   m_db.lastError().text(), __FILE__, __LINE__);

	      QApplication::restoreOverrideCursor();
	      addError(QString(tr("Database Error")),
		       QString(tr("An error occurred while attempting to "
				  "update the database account "
				  "for ")) +
		       userinfo_diag->m_userinfo.memberid->text() +
		       QString(tr(".")),
		       errorstr, __FILE__, __LINE__);
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
		    (QString(tr("Database Error")),
		     QString(tr("Unable to commit the current database "
				"transaction.")),
		     m_db.lastError().text(), __FILE__,
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
      userinfo_diag->m_memberProperties["membersince"] =
	userinfo_diag->m_userinfo.membersince->date().toString
	(Qt::ISODate);
      userinfo_diag->m_memberProperties["dob"] =
	userinfo_diag->m_userinfo.dob->date().toString
	(Qt::ISODate);
      userinfo_diag->m_memberProperties["sex"] =
	userinfo_diag->m_userinfo.sex->currentText();
      userinfo_diag->m_memberProperties["first_name"] =
	userinfo_diag->m_userinfo.firstName->text().trimmed();
      userinfo_diag->m_memberProperties["middle_init"] =
	userinfo_diag->m_userinfo.middle->text().trimmed();
      userinfo_diag->m_memberProperties["last_name"] =
	userinfo_diag->m_userinfo.lastName->text().trimmed();
      userinfo_diag->m_memberProperties["telephone_num"] =
	userinfo_diag->m_userinfo.telephoneNumber->text();
      userinfo_diag->m_memberProperties["street"] =
	userinfo_diag->m_userinfo.street->text().trimmed();
      userinfo_diag->m_memberProperties["city"] =
	userinfo_diag->m_userinfo.city->text().trimmed();
      userinfo_diag->m_memberProperties["state_abbr"] =
	userinfo_diag->m_userinfo.state->currentText();
      userinfo_diag->m_memberProperties["zip"] =
	userinfo_diag->m_userinfo.zip->text();
      userinfo_diag->m_memberProperties["email"] =
	userinfo_diag->m_userinfo.email->text().trimmed();
      userinfo_diag->m_memberProperties["expiration_date"] =
	userinfo_diag->m_userinfo.expirationdate->date().toString
	(Qt::ISODate);
      userinfo_diag->m_memberProperties["overdue_fees"] =
	userinfo_diag->m_userinfo.overduefees->text();
      userinfo_diag->m_memberProperties["comments"] =
	userinfo_diag->m_userinfo.comments->toPlainText().trimmed();
      userinfo_diag->m_memberProperties["general_registration_number"] =
	userinfo_diag->m_userinfo.generalregistrationnumber->text().trimmed();
      userinfo_diag->m_memberProperties["memberclass"] =
	userinfo_diag->m_userinfo.memberclass->text().trimmed();
      userinfo_diag->m_memberProperties["maximum_reserved_books"] =
	userinfo_diag->m_userinfo.maximum_reserved_books->text();

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
		    "default password has been set "
		    "to tempPass."));
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

  auto items(ui.graphicsView->scene()->selectedItems());

  if(!items.isEmpty())
    {
      QGraphicsItem *item = nullptr;
      QStringList oids;
      QStringList types;

      for(int i = 0; i < items.size(); i++)
	if((item = items.at(i)))
	  {
	    oids.append(item->data(0).toString());
	    types.append(item->data(1).toString());
	  }

      auto column1 = ui.table->columnNumber("MYOID");
      auto column2 = ui.table->columnNumber("Type");

      for(int i = 0; i < ui.table->rowCount(); i++)
	if(ui.table->item(i, column1) &&
	   oids.contains(ui.table->item(i, column1)->text()) &&
	   ui.table->item(i, column2) &&
	   types.contains(ui.table->
			  item(i, column2)->text().remove(' ').toLower()))
	  ui.table->selectRow(i);

      oids.clear();
      types.clear();
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
  auto row = bb.table->currentRow();
  int i = -1;
  int j = 0;

  if(m_db.driverName() == "QPSQL" && m_roles.isEmpty())
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);

      auto dnt = biblioteq_misc_functions::dnt(m_db, dbUserName(), errorstr);

      if(errorstr.isEmpty())
	{
	  history.dnt->setChecked(dnt);
	  history.dnt->setEnabled(true);
	  history.dnt->setToolTip("");
	}
      else
	{
	  history.dnt->setEnabled(false);
	  history.dnt->setToolTip
	    (tr("The option is not available because an error "
		"occurred while attempting to retrieve its value."));
	}

      QApplication::restoreOverrideCursor();
    }
  else
    {
      history.dnt->setChecked(true);
      history.dnt->setEnabled(false);

      if(m_db.driverName() == "QPSQL")
	history.dnt->setToolTip
	  (tr("The option is only available for patrons."));
      else
	history.dnt->setToolTip
	  (tr("The option is not available for SQLite databases as "
	      "such databases do not support actual patrons."));
    }

  if(m_members_diag->isVisible())
    if(row < 0)
      {
	QMessageBox::critical
	  (m_members_diag, tr("BiblioteQ: User Error"),
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
      (bb.table, row, m_bbColumnHeaderIndexes.indexOf("Member ID"));
  else
    memberid = dbUserName();

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
	(QString(tr("Database Error")),
	 QString(tr("Unable to retrieve reservation history data for table "
		    "populating.")),
	 query.lastError().text(),
	 __FILE__, __LINE__);

      if(m_history_diag->isVisible())
	QMessageBox::critical(m_history_diag, tr("BiblioteQ: Database Error"),
			      tr("Unable to retrieve reservation "
				 "history data for table populating."));
      else if(m_members_diag->isVisible())
	QMessageBox::critical(m_members_diag, tr("BiblioteQ: Database Error"),
			      tr("Unable to retrieve reservation "
				 "history data for table populating."));
      else
	QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			      tr("Unable to retrieve reservation "
				 "history data for table populating."));

      QApplication::processEvents();
      return;
    }

  QApplication::restoreOverrideCursor();
  history.table->setCurrentItem(nullptr);
  history.table->setColumnCount(0);
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
  history.table->setHorizontalHeaderLabels(list);
  history.table->setColumnHidden(history.table->columnCount() - 1, true);
  list.clear();
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

  history.table->scrollToTop();
  history.table->horizontalScrollBar()->setValue(0);
  progress->setModal(true);
  progress->setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress->setLabelText(tr("Populating the table..."));
  progress->setMinimum(0);
  progress->setMaximum(history.table->rowCount());
  progress->show();
  progress->repaint();
  QApplication::processEvents();
  i = -1;

  while(i++, !progress->wasCanceled() && query.next())
    {
      if(query.isValid())
	{
	  auto record(query.record());

	  for(j = 0; j < record.count(); j++)
	    {
	      if(record.fieldName(j).contains("date"))
		{
		  auto date(QDate::fromString(query.value(j).toString().
					       trimmed(),
					       "MM/dd/yyyy"));

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
  history.table->setRowCount(i); // Support cancellation.
  history.table->setSortingEnabled(true);
  history.table->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);

  for(int i = 0; i < history.table->columnCount() - 1; i++)
    history.table->resizeColumnToContents(i);

  history.nextTool->setVisible(!m_roles.isEmpty());
  history.prevTool->setVisible(!m_roles.isEmpty());

  if(m_members_diag->isVisible())
    {
      static auto resized = false;

      if(!resized)
	{
	  m_history_diag->resize
	    (qRound(0.85 * m_members_diag->size().width()),
	     qRound(0.85 * m_members_diag->size().height()));
	  biblioteq_misc_functions::center(m_history_diag, m_members_diag);
	}

      resized = true;
    }
  else
    {
      static auto resized = false;

      if(!resized)
	{
	  m_history_diag->resize(qRound(0.85 * size().width()),
				 qRound(0.85 * size().height()));
	  biblioteq_misc_functions::center(m_history_diag, this);
	}

      resized = true;
    }

  m_history_diag->showNormal();
  m_history_diag->activateWindow();
  m_history_diag->raise();
}

void biblioteq::slotShowImport(void)
{
  m_import->show(this);
}

void biblioteq::slotShowOtherOptions(void)
{
  biblioteq_misc_functions::center(m_otheroptions, this);
  m_otheroptions->showNormal();
  m_otheroptions->activateWindow();
  m_otheroptions->raise();
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
			   QMessageBox::Yes | QMessageBox::No,
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
  QApplication::restoreOverrideCursor();
}

void biblioteq::vgSearch(const QString &field, const QString &value)
{
  auto videogame = new biblioteq_videogame(this, "", QModelIndex());

  videogame->search(field, value);
  videogame->deleteLater();
}
