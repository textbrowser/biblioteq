/*
** -- Qt Includes --
*/

#include <QFileDialog>
#include <QSettings>
#include <QScrollBar>
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

QHash<QString, QString> biblioteq::getOpenLibraryHash(void) const
{
  return m_openLibraryImages;
}

QString biblioteq::publicationDateFormat(const QString &itemType) const
{
  return m_otheroptions->publicationDateFormat(itemType);
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

void biblioteq::slotAllGo(void)
{
  if(!m_db.isOpen())
    return;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  QList<QVariant> values;
  QSqlQuery query(m_db);
  QString searchstr("");
  QString str("");
  QString type("");
  QStringList types;
  bool caseinsensitive = al.caseinsensitive->isChecked();

  types.append("Book");
  types.append("CD");
  types.append("DVD");
  types.append("Grey Literature");
  types.append("Journal");
  types.append("Magazine");
  types.append("Photograph Collection");
  types.append("Video Game");

  while(!types.isEmpty())
    {
      type = types.takeFirst();

      if(type == "Grey Literature")
	str = "SELECT DISTINCT grey_literature.document_title, "
	  "grey_literature.document_id, "
	  "'', grey_literature.document_date, "
	  "'', "
	  "'', "
	  "0.00, '', "
	  "1, "
	  "grey_literature.location, "
	  "0 AS availability, "
	  "0 AS total_reserved, "
	  "grey_literature.job_number, "
	  "grey_literature.type, "
	  "grey_literature.myoid, "
	  "grey_literature.front_cover "
	  "FROM grey_literature "
	  "WHERE ";
      else if(type == "Photograph Collection")
	str = "SELECT DISTINCT photograph_collection.title, "
	  "photograph_collection.id, "
	  "'', '', "
	  "'', "
	  "'', "
	  "0.00, '', "
	  "1, "
	  "photograph_collection.location, "
	  "0 AS availability, "
	  "0 AS total_reserved, "
	  "photograph_collection.accession_number, "
	  "photograph_collection.type, "
	  "photograph_collection.myoid, "
	  "photograph_collection.image_scaled "
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
	     "COUNT(item_borrower_vw.item_oid) AS availability, "
	     "COUNT(item_borrower_vw.item_oid) AS total_reserved, "
	     "%1.accession_number, "
	     "%1.type, ").
	    arg(type.toLower().remove(" "));
	  str += QString("%1.myoid, "
			 "%1.front_cover "
			 "FROM "
			 "%1 LEFT JOIN item_borrower_vw ON "
			 "%1.myoid = "
			 "item_borrower_vw.item_oid "
			 "AND item_borrower_vw.type = '%2' "
			 "WHERE ").arg(type.toLower().remove(" ")).
	    arg(type);
	}

      QString ESCAPE("");
      QString UNACCENT(unaccent());

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
	      QStringList words
		(al.category->toPlainText().trimmed().
		 split(QRegExp("\\s+"), QString::SkipEmptyParts));

	      if(!words.isEmpty())
		{
		  str.append("(");

		  while(!words.isEmpty())
		    {
		      QString word(words.takeFirst());

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
		str.append(UNACCENT + "(LOWER(language)) = " +
			   UNACCENT + "(" + ESCAPE + "'" +
			   biblioteq_myqstring::escape
			   (al.language->currentText().trimmed(),
			    true) +
			   "') AND ");
	      else
		str.append(UNACCENT + "(language) = " + UNACCENT +
			   "(" + ESCAPE + "'" +
			   biblioteq_myqstring::escape
			   (al.language->currentText().trimmed()) +
			   "') AND ");
	    }

	  if(al.monetary_units->currentIndex() != 0)
	    {
	      if(caseinsensitive)
		str.append
		  (UNACCENT + "(LOWER(monetary_units)) = " +
		   UNACCENT + "(" + ESCAPE + "'" +
		   biblioteq_myqstring::
		   escape(al.monetary_units->
			  currentText().trimmed(),
			  true) +
		   "') AND ");
	      else
		str.append
		  (UNACCENT + "(monetary_units) = " + UNACCENT
		   + "(" + ESCAPE + "'" +
		   biblioteq_myqstring::
		   escape(al.monetary_units->
			  currentText().trimmed()) +
		   "') AND ");
	    }

	  if(al.abstract_checkbox->isChecked())
	    {
	      QStringList words
		(al.description->toPlainText().trimmed().
		 split(QRegExp("\\s+"), QString::SkipEmptyParts));

	      if(!words.isEmpty())
		{
		  str.append("(");

		  while(!words.isEmpty())
		    {
		      QString word(words.takeFirst());

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
	      QStringList words
		(al.keyword->toPlainText().trimmed().
		 split(QRegExp("\\s+"), QString::SkipEmptyParts));

	      if(!words.isEmpty())
		{
		  str.append(" AND (");

		  while(!words.isEmpty())
		    {
		      QString word(words.takeFirst());

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

      if(type != "Grey Literature" &&
	 type != "Photograph Collection")
	{
	  if(al.available->isChecked())
	    str.append
	      (QString("HAVING (%1.quantity - "
		       "COUNT(item_borrower_vw.item_oid)) > 0 ").
	       arg(type.toLower().remove(" ")));
	}

      if(type != "Video Game")
	str += "UNION ALL ";
      else
	str += " ";

      searchstr += str;
    }

  query.prepare(searchstr);

  while(!values.isEmpty())
    query.addBindValue(values.takeFirst());

  QApplication::restoreOverrideCursor();
  (void) populateTable(query, "All", NEW_PAGE, POPULATE_SEARCH);
}

void biblioteq::slotGeneralSearchPublicationDateEnabled(bool state)
{
  al.publication_date->setEnabled(state);

  if(!state)
    al.publication_date->setDate(QDate::fromString("2001", "yyyy"));
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

void biblioteq::slotLastWindowClosed(void)
{
  if(ui.actionAutomaticallySaveSettingsOnExit->isChecked())
    slotSaveConfig();

  cleanup();
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

void biblioteq::slotPreviewCanvasBackgroundColor(const QColor &color)
{
  ui.graphicsView->scene()->setBackgroundBrush(color);
}

void biblioteq::slotRefreshAdminList(void)
{
  QScopedPointer<QProgressDialog> progress;

  if(m_admin_diag->isVisible())
    progress.reset(new(std::nothrow) QProgressDialog(m_admin_diag));
  else
    progress.reset(new(std::nothrow) QProgressDialog(this));

  if(!progress)
    return;

  QCheckBox *checkBox = 0;
  QSqlQuery query(m_db);
  QString columnname = "";
  QString str = "";
  QStringList list;
  QTableWidgetItem *item = 0;
  int i = -1;
  int j = 0;

  query.prepare("SELECT username, LOWER(roles) "
		"FROM admin ORDER BY username");
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
#ifndef Q_OS_MAC
  QApplication::processEvents();
#endif
  i = -1;

  while(i++, !progress->wasCanceled() && query.next())
    {
      if(query.isValid())
	{
	  if((item = new(std::nothrow) QTableWidgetItem()) != 0)
	    {
	      str = query.value(0).toString();
	      item->setText(str);
	      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	      str = query.value(1).toString();
	      ab.table->setItem(i, 0, item);

	      for(j = 1; j < ab.table->columnCount(); j++)
		if(query.value(0).toString() == getAdminID() && j > 1)
		  {
		    if((item = new(std::nothrow) QTableWidgetItem()) != 0)
		      {
			item->setFlags(Qt::ItemIsEnabled |
				       Qt::ItemIsSelectable);
			ab.table->setItem(i, j, item);
		      }
		    else
		      addError
			(QString(tr("Memory Error")),
			 QString(tr("Unable to allocate memory for the "
				    "\"item\" object. "
				    "This is a serious problem!")),
			 QString(""), __FILE__, __LINE__);
		  }
		else if((checkBox = new(std::nothrow) QCheckBox()) != 0)
		  {
		    ab.table->setCellWidget(i, j, checkBox);
		    columnname = m_abColumnHeaderIndexes.value(j).toLower();

		    if(str.toLower().contains(columnname))
		      checkBox->setChecked(true);

		    if(query.value(0).toString() == getAdminID())
		      checkBox->setEnabled(false);
		    else
		      connect(checkBox, SIGNAL(stateChanged(int)), this,
			      SLOT(slotAdminCheckBoxClicked(int)));
		  }
		else
		  addError(QString(tr("Memory Error")),
			   QString(tr("Unable to allocate memory for the "
				      "\"checkBox\" object. "
				      "This is a serious problem!")),
			   QString(""), __FILE__, __LINE__);
	    }
	  else
	    addError(QString(tr("Memory Error")),
		     QString(tr("Unable to allocate memory for the "
				"\"item\" object. "
				"This is a serious problem!")),
		     QString(""), __FILE__, __LINE__);
	}

      if(i + 1 <= progress->maximum())
	progress->setValue(i + 1);

      progress->repaint();
#ifndef Q_OS_MAC
      QApplication::processEvents();
#endif
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

void biblioteq::slotRequest(void)
{
  /*
  ** This method is used to either request an item or cancel a request.
  */

  QDate now = QDate::currentDate();
  QModelIndexList list = ui.table->selectionModel()->selectedRows();
  QProgressDialog progress(this);
  QSqlQuery query(m_db);
  QString itemType = "";
  QString oid = "";
  bool error = false;
  bool isRequesting = true;
  int ct = 0;
  int i = 0;
  int numcompleted = 0;

  if(!m_roles.isEmpty())
    isRequesting = false;
  else if(ui.menu_Category->defaultAction() &&
	  ui.menu_Category->defaultAction()->data().
	  toString() == "All Requested")
    isRequesting = false;

  if(isRequesting)
    {
      if(list.isEmpty())
	{
	  QMessageBox::critical
	    (this, tr("BiblioteQ: User Error"),
	     tr("Please select at least one item to place "
		"on request."));
	  return;
	}
    }
  else
    {
      if(list.isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please select at least one request to "
				   "cancel."));
	  return;
	}

      if(list.size() > 0)
	if(QMessageBox::question(this, tr("BiblioteQ: Question"),
				 tr("Are you sure that you wish to "
				    "cancel the selected request(s)?"),
				 QMessageBox::Yes | QMessageBox::No,
				 QMessageBox::No) == QMessageBox::No)
	  {
	    list.clear();
	    return;
	  }
    }

  progress.setCancelButton(0);
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));

  if(isRequesting)
    progress.setLabelText(tr("Requesting the selected item(s)..."));
  else
    progress.setLabelText(tr("Canceling the selected request(s)..."));

  progress.setMaximum(list.size());
  progress.setMinimum(0);
  progress.show();
  progress.repaint();
#ifndef Q_OS_MAC
  QApplication::processEvents();
#endif

  foreach(const QModelIndex &index, list)
    {
      i = index.row();
      ct += 1;

      if(isRequesting)
	oid = biblioteq_misc_functions::getColumnString
	  (ui.table, i, ui.table->columnNumber("MYOID"));
      else
	{
	  oid = biblioteq_misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("REQUESTOID"));

	  if(oid.isEmpty())
	    oid = "-1";
	}

      itemType = biblioteq_misc_functions::getColumnString
	(ui.table, i,
	 ui.table->columnNumber("Type"));

      if(itemType != "Grey Literature" &&
	 itemType != "Photograph Collection")
	{
	  if(isRequesting)
	    {
	      query.prepare("INSERT INTO item_request (item_oid, memberid, "
			    "requestdate, type) VALUES (?, ?, ?, ?)");
	      query.bindValue(0, oid);
	      query.bindValue(1, m_db.userName());
	      query.bindValue(2, now.toString("MM/dd/yyyy"));
	      query.bindValue(3, itemType);
	    }
	  else
	    {
	      query.prepare("DELETE FROM item_request WHERE myoid = ?");
	      query.bindValue(0, oid);
	    }

	  if(!query.exec())
	    {
	      error = true;

	      if(isRequesting)
		addError(QString(tr("Database Error")),
			 QString(tr("Unable to request the item.")),
			 query.lastError().text(), __FILE__, __LINE__);
	      else
		addError(QString(tr("Database Error")),
			 QString(tr("Unable to cancel the request.")),
			 query.lastError().text(), __FILE__, __LINE__);
	    }
	  else
	    {
	      numcompleted += 1;

	      if(!isRequesting)
		deleteItem(oid, itemType);
	    }
	}

      if(i + 1 <= progress.maximum())
	progress.setValue(ct);

      progress.repaint();
#ifndef Q_OS_MAC
      QApplication::processEvents();
#endif

    }

  progress.close();

  /*
  ** Provide some fancy messages.
  */

  if(error && isRequesting)
    QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			  tr("Unable to request some or all of the selected "
			     "items. "
			     "Please verify that you are not attempting to "
			     "request duplicate items."));
  else if(error)
    QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			  tr("Unable to cancel some or all of the selected "
			     "requests."));

  if(!isRequesting && numcompleted > 0)
    slotRefresh();

  list.clear();
}

void biblioteq::slotSaveAdministrators(void)
{
  QCheckBox *checkBox = 0;
  QProgressDialog progress(m_admin_diag);
  QSqlQuery query(m_db);
  QString adminStr = "";
  QString errorstr = "";
  QString str = "";
  QStringList tmplist;
  bool adminCreated = false;
  bool uexists = false;
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

  for(i = 0; i < ab.table->rowCount(); i++)
    {
      if(ab.table->item(i, 0)->text().trimmed().isEmpty())
	continue;

      if(!(qobject_cast<QCheckBox *>
	   (ab.table->cellWidget(i, 1))->isChecked() ||
	   qobject_cast<QCheckBox *>
	   (ab.table->cellWidget(i, 2))->isChecked() ||
	   qobject_cast<QCheckBox *>
	   (ab.table->cellWidget(i, 3))->isChecked() ||
	   qobject_cast<QCheckBox *>
	   (ab.table->cellWidget(i, 4))->isChecked()))
	{
	  tmplist.clear();
	  ab.table->selectRow(i);
	  ab.table->horizontalScrollBar()->setValue(i);
	  QMessageBox::critical
	    (m_admin_diag, tr("BiblioteQ: User Error"),
	     tr("Administrators must belong to at least one category."));
	  return;
	}

      if(!tmplist.contains(ab.table->item(i, 0)->text().toLower().trimmed()))
	tmplist.append(ab.table->item(i, 0)->text().toLower().trimmed());
      else
	{
	  tmplist.clear();
	  ab.table->selectRow(i);
	  ab.table->horizontalScrollBar()->setValue(i);
	  QMessageBox::critical
	    (m_admin_diag, tr("BiblioteQ: User Error"),
	     tr("Duplicate administrator ids are not allowed."));
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
	(m_deletedAdmins[i].toLower(), m_db,
	 biblioteq_misc_functions::DELETE_USER, errorstr);

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
  progress.setCancelButton(0);
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.setLabelText(tr("Saving administrator information..."));
  progress.setMaximum(ab.table->rowCount());
  progress.setMinimum(0);
  progress.show();
  progress.repaint();
#ifndef Q_OS_MAC
  QApplication::processEvents();
#endif

  /*
  ** Add or modify administrators.
  */

  for(i = 0; i < ab.table->rowCount(); i++)
    {
      str = "";
      adminStr = ab.table->item(i, 0)->text().toLower().trimmed();

      if(i + 1 <= progress.maximum())
	progress.setValue(i + 1);

      progress.repaint();
#ifndef Q_OS_MAC
      QApplication::processEvents();
#endif

      if(adminStr.isEmpty())
	continue; // Ignore empty administrator ids.
      else if(adminStr == getAdminID())
	continue; // Ignore current administrator.

      if((qobject_cast<QCheckBox *> (ab.table->cellWidget(i, 1)))->
	 isChecked())
	str = "administrator";
      else
	for(j = 2; j < ab.table->columnCount(); j++)
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
	    (adminStr, m_db,
	     biblioteq_misc_functions::CREATE_USER,
	     errorstr, str);

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
      return;
    }

  QApplication::restoreOverrideCursor();
  m_deletedAdmins.clear();

  if(adminCreated)
    QMessageBox::information
      (m_admin_diag, tr("BiblioteQ: Information"),
       tr("Please notify new administrators that their "
	  "default password has been set "
	  "to tempPass."));

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
}

void biblioteq::slotShowOtherOptions(void)
{
  biblioteq_misc_functions::center(m_otheroptions, this);
  m_otheroptions->showNormal();
  m_otheroptions->activateWindow();
  m_otheroptions->raise();
}
