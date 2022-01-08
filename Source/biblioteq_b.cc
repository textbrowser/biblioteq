#include "biblioteq.h"
#include "biblioteq_graphicsitempixmap.h"
#include "biblioteq_otheroptions.h"
#include "ui_biblioteq_generalmessagediag.h"

#include <QActionGroup>
#include <QDir>
#include <QScrollBar>
#include <QSettings>
#include <QSqlDriver>
#include <QSqlField>
#include <QSqlRecord>
#include <QStandardPaths>

#include <limits>

QString biblioteq::homePath(void)
{
#ifdef BIBLIOTEQ_NON_PORTABLE
  return QStandardPaths::writableLocation
    (QStandardPaths::StandardLocation::AppConfigLocation);
#elif defined(Q_OS_WIN)
  return QDir::currentPath() + QDir::separator() + ".biblioteq";
#else
  return QDir::homePath() + QDir::separator() + ".biblioteq";
#endif
}

QString biblioteq::unaccent(void) const
{
  return m_unaccent;
}

QVariant biblioteq::setting(const QString &name) const
{
  if(name == "automatically_resize_column_widths")
    return ui.actionAutomatically_Resize_Column_Widths->isChecked();
  else if(name == "photographs_per_page")
    {
      foreach(auto action, ui.menuPhotographs_per_Page->actions())
	if(action->isChecked())
	  return action->data().toInt();

      return 25;
    }
  else
    return QVariant();
}

bool biblioteq::isGuest(void) const
{
  if(m_db.driverName() == "QSQLITE")
    return false;
  else if(dbUserName() == BIBLIOTEQ_GUEST_ACCOUNT)
    return true;
  else
    return false;
}

bool biblioteq::isPatron(void) const
{
  if(m_db.driverName() == "QSQLITE")
    return true; // Administrator and patron.
  else if(dbUserName() == BIBLIOTEQ_GUEST_ACCOUNT)
    return false;
  else if(m_roles.isEmpty())
    return true;
  else
    return false;
}

int biblioteq::pageLimit(void) const
{
  int limit = 25;

  for(int i = 0; i < ui.menuEntriesPerPage->actions().size(); i++)
    if(ui.menuEntriesPerPage->actions().at(i)->isChecked())
      {
	limit = ui.menuEntriesPerPage->actions().at(i)->data().toInt();
	break;
      }

  return limit;
}

int biblioteq::populateTable(const int search_type_arg,
			     const QString &typefilter,
			     const QString &searchstrArg,
			     const int pagingType)
{
  ui.itemsCountLabel->setText(tr("0 Results"));

  QScopedPointer<QProgressDialog> progress;
  QString limitStr("");
  QString offsetStr("");
  QString searchstr = "";
  QString str = "";
  QString type = "";
  QStringList tmplist; // Used for custom queries.
  QTableWidgetItem *item = nullptr;
  auto now(QDate::currentDate());
  auto offset = m_queryOffset;
  auto search_type = search_type_arg;
  int i = -1;
  int limit = 0;

  for(int ii = 0; ii < ui.menuEntriesPerPage->actions().size(); ii++)
    if(ui.menuEntriesPerPage->actions().at(ii)->isChecked())
      {
	limit = ui.menuEntriesPerPage->actions().at(ii)->data().toInt();
	break;
      }

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
	      ** A specific page was selected from
	      ** ui.pagesLabel.
	      */

	      offset = 0;

	      for(int ii = 1; ii < qAbs(pagingType); ii++)
		offset += limit;
	    }
	}
      else
	offset = 0;

      limitStr = QString(" LIMIT %1 ").arg(limit);
      offsetStr = QString(" OFFSET %1 ").arg(offset);
      ui.graphicsView->setSceneRect
	(0.0,
	 0.0,
	 5.0 * 130.0,
	 static_cast<qreal> (limit / 5.0 * 200.0) + 200.0);
    }

  QString bookFrontCover("'' AS front_cover ");
  QString cdFrontCover("'' AS front_cover ");
  QString dvdFrontCover("'' AS front_cover ");
  QString greyLiteratureFrontCover("'' AS front_cover ");
  QString journalFrontCover("'' AS front_cover ");
  QString magazineFrontCover("'' AS front_cover ");
  QString photographCollectionFrontCover("'' AS image_scaled ");
  QString videoGameFrontCover("'' AS front_cover ");

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

  if(m_otheroptions->showMainTableProgressDialogs())
    {
      auto closeButton = new QPushButton(tr("Interrupt"));

      closeButton->setShortcut(QKeySequence(Qt::Key_F8));
      progress.reset(new QProgressDialog(this));
      progress->hide();
      progress->setCancelButton(closeButton);
    }

  /*
  ** The order of the fields in the select statements should match
  ** the original column order.
  */

  ui.configTool->setEnabled(true);
  ui.configTool->setToolTip("");

  switch(search_type)
    {
    case CUSTOM_QUERY:
      {
	if(m_configToolMenu)
	  m_configToolMenu->deleteLater();

	ui.configTool->setEnabled(false);
	ui.configTool->setToolTip(tr("Disabled for custom queries."));
	searchstr = searchstrArg;

	if(searchstr.lastIndexOf("LIMIT") != -1)
	  searchstr.remove(searchstr.lastIndexOf("LIMIT"),
			   searchstr.length());

	searchstr += limitStr + offsetStr;
	break;
      }
    case POPULATE_ALL:
      {
	if(typefilter == "All" || typefilter == "All Available")
	  {
	    QString checkAvailability("");

	    if(typefilter == "All Available")
	      checkAvailability =
		" HAVING (quantity - COUNT(item_borrower.item_oid)) > 0 ";

	    searchstr = QString
	      ("SELECT DISTINCT book.title, "
	       "book.id, "
	       "book.publisher, book.pdate, "
	       "book.category, "
	       "book.language, "
	       "book.price, book.monetary_units, "
	       "book.quantity, "
	       "book.location, "
	       "book.quantity - COUNT(item_borrower.item_oid) "
	       "AS availability, "
	       "COUNT(item_borrower.item_oid) AS total_reserved, "
	       "book.accession_number, "
	       "book.type, "
	       "book.myoid, " +
	       bookFrontCover +
	       "FROM "
	       "book LEFT JOIN item_borrower ON "
	       "book.myoid = item_borrower.item_oid "
	       "AND item_borrower.type = 'Book' "
	       "GROUP BY book.title, "
	       "book.id, "
	       "book.publisher, book.pdate, "
	       "book.category, "
	       "book.language, "
	       "book.price, book.monetary_units, "
	       "book.quantity, "
	       "book.location, "
	       "book.accession_number, "
	       "book.type, "
	       "book.myoid, "
	       "book.front_cover "
	       " %1 "
	       "UNION "
	       "SELECT DISTINCT cd.title, "
	       "cd.id, "
	       "cd.recording_label, cd.rdate, "
	       "cd.category, "
	       "cd.language, "
	       "cd.price, cd.monetary_units, "
	       "cd.quantity, "
	       "cd.location, "
	       "cd.quantity - COUNT(item_borrower.item_oid) "
	       "AS availability, "
	       "COUNT(item_borrower.item_oid) AS total_reserved, "
	       "cd.accession_number, "
	       "cd.type, "
	       "cd.myoid, " +
	       cdFrontCover +
	       "FROM "
	       "cd LEFT JOIN item_borrower ON "
	       "cd.myoid = item_borrower.item_oid "
	       "AND item_borrower.type = 'CD' "
	       "GROUP BY cd.title, "
	       "cd.id, "
	       "cd.recording_label, cd.rdate, "
	       "cd.category, "
	       "cd.language, "
	       "cd.price, cd.monetary_units, "
	       "cd.quantity, "
	       "cd.location, "
	       "cd.accession_number, "
	       "cd.type, "
	       "cd.myoid, "
	       "cd.front_cover "
	       " %1 "
	       "UNION "
	       "SELECT DISTINCT dvd.title, "
	       "dvd.id, "
	       "dvd.studio, dvd.rdate, "
	       "dvd.category, "
	       "dvd.language, "
	       "dvd.price, dvd.monetary_units, "
	       "dvd.quantity, "
	       "dvd.location, "
	       "dvd.quantity - COUNT(item_borrower.item_oid) AS "
	       "availability, "
	       "COUNT(item_borrower.item_oid) AS total_reserved, "
	       "dvd.accession_number, "
	       "dvd.type, "
	       "dvd.myoid, " +
	       dvdFrontCover +
	       "FROM "
	       "dvd LEFT JOIN item_borrower ON "
	       "dvd.myoid = item_borrower.item_oid "
	       "AND item_borrower.type = 'DVD' "
	       "GROUP BY dvd.title, "
	       "dvd.id, "
	       "dvd.studio, dvd.rdate, "
	       "dvd.category, "
	       "dvd.language, "
	       "dvd.price, dvd.monetary_units, "
	       "dvd.quantity, "
	       "dvd.location, "
	       "dvd.accession_number, "
	       "dvd.type, "
	       "dvd.myoid, "
	       "dvd.front_cover "
	       " %1 "
	       "UNION "
	       "SELECT DISTINCT grey_literature.document_title, "
	       "grey_literature.document_id, "
	       "grey_literature.author, "
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
	       "grey_literature.myoid = item_borrower.item_oid "
	       "AND item_borrower.type = 'Grey Literature' "
	       "GROUP BY "
	       "grey_literature.document_title, "
	       "grey_literature.document_id, "
	       "grey_literature.author, "
	       "grey_literature.document_date, "
	       "grey_literature.quantity, "
	       "grey_literature.location, "
	       "grey_literature.job_number, "
	       "grey_literature.type, "
	       "grey_literature.myoid, "
	       "grey_literature.front_cover "
	       "%1 "
	       "UNION "
	       "SELECT DISTINCT journal.title, "
	       "journal.id, "
	       "journal.publisher, journal.pdate, "
	       "journal.category, "
	       "journal.language, "
	       "journal.price, journal.monetary_units, "
	       "journal.quantity, "
	       "journal.location, "
	       "journal.quantity - COUNT(item_borrower.item_oid) AS "
	       "availability, "
	       "COUNT(item_borrower.item_oid) AS total_reserved, "
	       "journal.accession_number, "
	       "journal.type, "
	       "journal.myoid, " +
	       journalFrontCover +
	       "FROM "
	       "journal LEFT JOIN item_borrower ON "
	       "journal.myoid = item_borrower.item_oid "
	       "AND item_borrower.type = journal.type "
	       "GROUP BY journal.title, "
	       "journal.id, "
	       "journal.publisher, journal.pdate, "
	       "journal.category, "
	       "journal.language, "
	       "journal.price, journal.monetary_units, "
	       "journal.quantity, "
	       "journal.location, "
	       "journal.accession_number, "
	       "journal.type, "
	       "journal.myoid, "
	       "journal.front_cover "
	       " %1 "
	       "UNION "
	       "SELECT DISTINCT magazine.title, "
	       "magazine.id, "
	       "magazine.publisher, magazine.pdate, "
	       "magazine.category, "
	       "magazine.language, "
	       "magazine.price, magazine.monetary_units, "
	       "magazine.quantity, "
	       "magazine.location, "
	       "magazine.quantity - COUNT(item_borrower.item_oid) AS "
	       "availability, "
	       "COUNT(item_borrower.item_oid) AS total_reserved, "
	       "magazine.accession_number, "
	       "magazine.type, "
	       "magazine.myoid, " +
	       magazineFrontCover +
	       "FROM "
	       "magazine LEFT JOIN item_borrower ON "
	       "magazine.myoid = item_borrower.item_oid "
	       "AND item_borrower.type = magazine.type "
	       "GROUP BY magazine.title, "
	       "magazine.id, "
	       "magazine.publisher, magazine.pdate, "
	       "magazine.category, "
	       "magazine.language, "
	       "magazine.price, magazine.monetary_units, "
	       "magazine.quantity, "
	       "magazine.location, "
	       "magazine.accession_number, "
	       "magazine.type, "
	       "magazine.myoid, "
	       "magazine.front_cover "
	       " %1 "
	       "UNION "
	       "SELECT DISTINCT photograph_collection.title, "
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
	       "GROUP BY "
	       "photograph_collection.title, "
	       "photograph_collection.id, "
	       "photograph_collection.location, "
	       "photograph_collection.accession_number, "
	       "photograph_collection.type, "
	       "photograph_collection.myoid, " +
	       "photograph_collection.image_scaled "
	       "UNION "
	       "SELECT DISTINCT videogame.title, "
	       "videogame.id, "
	       "videogame.publisher, videogame.rdate, "
	       "videogame.genre, "
	       "videogame.language, "
	       "videogame.price, videogame.monetary_units, "
	       "videogame.quantity, "
	       "videogame.location, "
	       "videogame.quantity - COUNT(item_borrower.item_oid) "
	       "AS availability, "
	       "COUNT(item_borrower.item_oid) AS total_reserved, "
	       "videogame.accession_number, "
	       "videogame.type, "
	       "videogame.myoid, " +
	       videoGameFrontCover +
	       "FROM "
	       "videogame LEFT JOIN item_borrower ON "
	       "videogame.myoid = item_borrower.item_oid "
	       "AND item_borrower.type = 'Video Game' "
	       "GROUP BY videogame.title, "
	       "videogame.id, "
	       "videogame.publisher, videogame.rdate, "
	       "videogame.genre, "
	       "videogame.language, "
	       "videogame.price, videogame.monetary_units, "
	       "videogame.quantity, "
	       "videogame.location, "
	       "videogame.accession_number, "
	       "videogame.type, "
	       "videogame.myoid, "
	       "videogame.front_cover "
	       " %1 "
	       "ORDER BY 1").arg(checkAvailability) +
	      limitStr + offsetStr;
	  }
	else if(typefilter == "All Overdue")
	  {
	    searchstr = "";

	    if(m_roles.isEmpty())
	      {
		searchstr.append("SELECT DISTINCT "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "book.title, "
				 "book.id, "
				 "book.callnumber, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "book.accession_number, "
				 "book.type, "
				 "book.myoid, " +
				 bookFrontCover +
				 "FROM "
				 "book LEFT JOIN item_borrower ON "
				 "book.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = 'Book' "
				 "WHERE "
				 "item_borrower.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append
		  ("SUBSTR(item_borrower.duedate, 7, 4) || '/' || "
		   "SUBSTR(item_borrower.duedate, 1, 2) || '/' || "
		   "SUBSTR(item_borrower.duedate, 4, 2) < '");
		searchstr.append(now.toString("yyyy/MM/dd"));
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "book.title, "
				 "book.id, "
				 "book.callnumber, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.accession_number, "
				 "book.type, "
				 "book.myoid, "
				 "book.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "cd.title, "
				 "cd.id, "
				 "'' AS callnumber, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "cd.accession_number, "
				 "cd.type, "
				 "cd.myoid, " +
				 cdFrontCover +
				 "FROM "
				 "cd LEFT JOIN item_borrower ON "
				 "cd.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = 'CD' "
				 "WHERE "
				 "item_borrower.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append
		  ("SUBSTR(item_borrower.duedate, 7, 4) || '/' || "
		   "SUBSTR(item_borrower.duedate, 1, 2) || '/' || "
		   "SUBSTR(item_borrower.duedate, 4, 2) < '");
		searchstr.append(now.toString("yyyy/MM/dd"));
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "cd.title, "
				 "cd.id, "
				 "callnumber, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.accession_number, "
				 "cd.type, "
				 "cd.myoid, "
				 "cd.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "dvd.title, "
				 "dvd.id, "
				 "'' AS callnumber, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "dvd.accession_number, "
				 "dvd.type, "
				 "dvd.myoid, " +
				 dvdFrontCover +
				 "FROM "
				 "dvd LEFT JOIN item_borrower ON "
				 "dvd.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = 'DVD' "
				 "WHERE "
				 "item_borrower.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append
		  ("SUBSTR(item_borrower.duedate, 7, 4) || '/' || "
		   "SUBSTR(item_borrower.duedate, 1, 2) || '/' || "
		   "SUBSTR(item_borrower.duedate, 4, 2) < '");
		searchstr.append(now.toString("yyyy/MM/dd"));
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "dvd.title, "
				 "dvd.id, "
				 "callnumber, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.accession_number, "
				 "dvd.type, "
				 "dvd.myoid, "
				 "dvd.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "grey_literature.document_title, "
				 "grey_literature.document_id, "
				 "'', "
				 "grey_literature.author, "
				 "grey_literature.document_date, "
				 "grey_literature.document_type, "
				 "'', "
				 "0.00, "
				 "'', "
				 "grey_literature.quantity, "
				 "grey_literature.location, "
				 "1 - COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "grey_literature.job_number, "
				 "grey_literature.type, "
				 "grey_literature.myoid, " +
				 greyLiteratureFrontCover +
				 "FROM "
				 "grey_literature LEFT JOIN item_borrower ON "
				 "grey_literature.myoid = "
				 "item_borrower.item_oid "
				 "AND item_borrower.type = 'Grey Literature' "
				 "WHERE "
				 "item_borrower.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append
		  ("SUBSTR(item_borrower.duedate, 7, 4) || '/' || "
		   "SUBSTR(item_borrower.duedate, 1, 2) || '/' || "
		   "SUBSTR(item_borrower.duedate, 4, 2) < '");
		searchstr.append(now.toString("yyyy/MM/dd"));
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "grey_literature.document_title, "
				 "grey_literature.document_id, "
				 "grey_literature.author, "
				 "grey_literature.document_date, "
				 "grey_literature.document_type, "
				 "grey_literature.quantity, "
				 "grey_literature.location, "
				 "grey_literature.job_number, "
				 "grey_literature.type, "
				 "grey_literature.myoid, "
				 "grey_literature.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "journal.title, "
				 "journal.id, "
				 "journal.callnumber, "
				 "journal.publisher, journal.pdate, "
				 "journal.category, "
				 "journal.language, "
				 "journal.price, journal.monetary_units, "
				 "journal.quantity, "
				 "journal.location, "
				 "journal.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "journal.accession_number, "
				 "journal.type, "
				 "journal.myoid, " +
				 journalFrontCover +
				 "FROM "
				 "journal LEFT JOIN item_borrower ON "
				 "journal.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = journal.type "
				 "WHERE "
				 "item_borrower.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append
		  ("SUBSTR(item_borrower.duedate, 7, 4) || '/' || "
		   "SUBSTR(item_borrower.duedate, 1, 2) || '/' || "
		   "SUBSTR(item_borrower.duedate, 4, 2) < '");
		searchstr.append(now.toString("yyyy/MM/dd"));
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "journal.title, "
				 "journal.id, "
				 "journal.callnumber, "
				 "journal.publisher, journal.pdate, "
				 "journal.category, "
				 "journal.language, "
				 "journal.price, journal.monetary_units, "
				 "journal.quantity, "
				 "journal.location, "
				 "journal.accession_number, "
				 "journal.type, "
				 "journal.myoid, "
				 "journal.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.callnumber, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "magazine.accession_number, "
				 "magazine.type, "
				 "magazine.myoid, " +
				 magazineFrontCover +
				 "FROM "
				 "magazine LEFT JOIN item_borrower ON "
				 "magazine.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = magazine.type "
				 "WHERE "
				 "item_borrower.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append
		  ("SUBSTR(item_borrower.duedate, 7, 4) || '/' || "
		   "SUBSTR(item_borrower.duedate, 1, 2) || '/' || "
		   "SUBSTR(item_borrower.duedate, 4, 2) < '");
		searchstr.append(now.toString("yyyy/MM/dd"));
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.callnumber, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.accession_number, "
				 "magazine.type, "
				 "magazine.myoid, "
				 "magazine.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "videogame.title, "
				 "videogame.id, "
				 "'' AS callnumber, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "videogame.accession_number, "
				 "videogame.type, "
				 "videogame.myoid, " +
				 videoGameFrontCover +
				 "FROM "
				 "videogame LEFT JOIN item_borrower ON "
				 "videogame.myoid = "
				 "item_borrower.item_oid "
				 "AND item_borrower.type = 'Video Game' "
				 "WHERE "
				 "item_borrower.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append
		  ("SUBSTR(item_borrower.duedate, 7, 4) || '/' || "
		   "SUBSTR(item_borrower.duedate, 1, 2) || '/' || "
		   "SUBSTR(item_borrower.duedate, 4, 2) < '");
		searchstr.append(now.toString("yyyy/MM/dd"));
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "videogame.title, "
				 "videogame.id, "
				 "callnumber, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.accession_number, "
				 "videogame.type, "
				 "videogame.myoid, "
				 "videogame.front_cover ");
		searchstr.append("ORDER BY 1");
		searchstr.append(limitStr + offsetStr);
	      }
	    else // !m_roles.isEmpty()
	      {
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "book.title, "
				 "book.id, "
				 "book.callnumber, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "book.accession_number, "
				 "book.type, "
				 "book.myoid, " +
				 bookFrontCover +
				 "FROM "
				 "member, "
				 "book LEFT JOIN item_borrower ON "
				 "book.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = 'Book' "
				 "WHERE "
				 "LOWER(member.memberid) LIKE LOWER('");
		searchstr.append(searchstrArg.trimmed().isEmpty() ?
				 "%" : searchstrArg.trimmed());
		searchstr.append("') AND ");
		searchstr.append
		  ("SUBSTR(item_borrower.duedate, 7, 4) || '/' || "
		   "SUBSTR(item_borrower.duedate, 1, 2) || '/' || "
		   "SUBSTR(item_borrower.duedate, 4, 2) < '");
		searchstr.append(now.toString("yyyy/MM/dd"));
		searchstr.append("' AND ");
		searchstr.append("item_borrower.memberid = member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "book.title, "
				 "book.id, "
				 "book.callnumber, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.accession_number, "
				 "book.type, "
				 "book.myoid, "
				 "book.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "cd.title, "
				 "cd.id, "
				 "'' AS callnumber, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "cd.accession_number, "
				 "cd.type, "
				 "cd.myoid, " +
				 cdFrontCover +
				 "FROM "
				 "member, "
				 "cd LEFT JOIN item_borrower ON "
				 "cd.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = 'CD' "
				 "WHERE "
				 "LOWER(member.memberid) LIKE LOWER('");
		searchstr.append(searchstrArg.trimmed().isEmpty() ?
				 "%" : searchstrArg.trimmed());
		searchstr.append("') AND ");
		searchstr.append
		  ("SUBSTR(item_borrower.duedate, 7, 4) || '/' || "
		   "SUBSTR(item_borrower.duedate, 1, 2) || '/' || "
		   "SUBSTR(item_borrower.duedate, 4, 2) < '");
		searchstr.append(now.toString("yyyy/MM/dd"));
		searchstr.append("' AND ");
		searchstr.append("item_borrower.memberid = member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "cd.title, "
				 "cd.id, "
				 "callnumber, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.accession_number, "
				 "cd.type, "
				 "cd.myoid, "
				 "cd.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "dvd.title, "
				 "dvd.id, "
				 "'' AS callnumber, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "dvd.accession_number, "
				 "dvd.type, "
				 "dvd.myoid, " +
				 dvdFrontCover +
				 "FROM "
				 "member, "
				 "dvd LEFT JOIN item_borrower ON "
				 "dvd.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = 'DVD' "
				 "WHERE "
				 "LOWER(member.memberid) LIKE LOWER('");
		searchstr.append(searchstrArg.trimmed().isEmpty() ?
				 "%" : searchstrArg.trimmed());
		searchstr.append("') AND ");
		searchstr.append
		  ("SUBSTR(item_borrower.duedate, 7, 4) || '/' || "
		   "SUBSTR(item_borrower.duedate, 1, 2) || '/' || "
		   "SUBSTR(item_borrower.duedate, 4, 2) < '");
		searchstr.append(now.toString("yyyy/MM/dd"));
		searchstr.append("' AND ");
		searchstr.append("item_borrower.memberid = member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "dvd.title, "
				 "dvd.id, "
				 "callnumber, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.accession_number, "
				 "dvd.type, "
				 "dvd.myoid, "
				 "dvd.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "grey_literature.document_title, "
				 "grey_literature.document_id, "
				 "'', "
				 "grey_literature.author, "
				 "grey_literature.document_date, "
				 "grey_literature.document_type, "
				 "'', "
				 "0.00, "
				 "'', "
				 "grey_literature.quantity, "
				 "grey_literature.location, "
				 "1 - COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "grey_literature.job_number, "
				 "grey_literature.type, "
				 "grey_literature.myoid, " +
				 greyLiteratureFrontCover +
				 "FROM "
				 "member, "
				 "grey_literature LEFT JOIN item_borrower ON "
				 "grey_literature.myoid = "
				 "item_borrower.item_oid "
				 "AND item_borrower.type = 'Grey Literature' "
				 "WHERE "
				 "LOWER(member.memberid) LIKE LOWER('");
		searchstr.append(searchstrArg.trimmed().isEmpty() ?
				 "%" : searchstrArg.trimmed());
		searchstr.append("') AND ");
		searchstr.append
		  ("SUBSTR(item_borrower.duedate, 7, 4) || '/' || "
		   "SUBSTR(item_borrower.duedate, 1, 2) || '/' || "
		   "SUBSTR(item_borrower.duedate, 4, 2) < '");
		searchstr.append(now.toString("yyyy/MM/dd"));
		searchstr.append("' AND ");
		searchstr.append("item_borrower.memberid = member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "grey_literature.document_title, "
				 "grey_literature.document_id, "
				 "grey_literature.author, "
				 "grey_literature.document_date, "
				 "grey_literature.document_type, "
				 "grey_literature.quantity, "
				 "grey_literature.location, "
				 "grey_literature.job_number, "
				 "grey_literature.type, "
				 "grey_literature.myoid, "
				 "grey_literature.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "journal.title, "
				 "journal.id, "
				 "journal.callnumber, "
				 "journal.publisher, journal.pdate, "
				 "journal.category, "
				 "journal.language, "
				 "journal.price, journal.monetary_units, "
				 "journal.quantity, "
				 "journal.location, "
				 "journal.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "journal.accession_number, "
				 "journal.type, "
				 "journal.myoid, " +
				 journalFrontCover +
				 "FROM "
				 "member, "
				 "journal LEFT JOIN item_borrower ON "
				 "journal.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = journal.type "
				 "WHERE "
				 "LOWER(member.memberid) LIKE LOWER('");
		searchstr.append(searchstrArg.trimmed().isEmpty() ?
				 "%" : searchstrArg.trimmed());
		searchstr.append("') AND ");
		searchstr.append
		  ("SUBSTR(item_borrower.duedate, 7, 4) || '/' || "
		   "SUBSTR(item_borrower.duedate, 1, 2) || '/' || "
		   "SUBSTR(item_borrower.duedate, 4, 2) < '");
		searchstr.append(now.toString("yyyy/MM/dd"));
		searchstr.append("' AND ");
		searchstr.append("item_borrower.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "journal.title, "
				 "journal.id, "
				 "journal.callnumber, "
				 "journal.publisher, journal.pdate, "
				 "journal.category, "
				 "journal.language, "
				 "journal.price, journal.monetary_units, "
				 "journal.quantity, "
				 "journal.location, "
				 "journal.accession_number, "
				 "journal.type, "
				 "journal.myoid, "
				 "journal.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.callnumber, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "magazine.accession_number, "
				 "magazine.type, "
				 "magazine.myoid, " +
				 magazineFrontCover +
				 "FROM "
				 "member, "
				 "magazine LEFT JOIN item_borrower ON "
				 "magazine.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = magazine.type "
				 "WHERE "
				 "LOWER(member.memberid) LIKE LOWER('");
		searchstr.append(searchstrArg.trimmed().isEmpty() ?
				 "%" : searchstrArg.trimmed());
		searchstr.append("') AND ");
		searchstr.append
		  ("SUBSTR(item_borrower.duedate, 7, 4) || '/' || "
		   "SUBSTR(item_borrower.duedate, 1, 2) || '/' || "
		   "SUBSTR(item_borrower.duedate, 4, 2) < '");
		searchstr.append(now.toString("yyyy/MM/dd"));
		searchstr.append("' AND ");
		searchstr.append("item_borrower.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.callnumber, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.accession_number, "
				 "magazine.type, "
				 "magazine.myoid, "
				 "magazine.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "videogame.title, "
				 "videogame.id, "
				 "'' AS callnumber, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "videogame.accession_number, "
				 "videogame.type, "
				 "videogame.myoid, " +
				 videoGameFrontCover +
				 "FROM "
				 "member, "
				 "videogame LEFT JOIN item_borrower ON "
				 "videogame.myoid = "
				 "item_borrower.item_oid "
				 "AND item_borrower.type = 'Video Game' "
				 "WHERE "
				 "LOWER(member.memberid) LIKE LOWER('");
		searchstr.append(searchstrArg.trimmed().isEmpty() ?
				 "%" : searchstrArg.trimmed());
		searchstr.append("') AND ");
		searchstr.append
		  ("SUBSTR(item_borrower.duedate, 7, 4) || '/' || "
		   "SUBSTR(item_borrower.duedate, 1, 2) || '/' || "
		   "SUBSTR(item_borrower.duedate, 4, 2) < '");
		searchstr.append(now.toString("yyyy/MM/dd"));
		searchstr.append("' AND ");
		searchstr.append("item_borrower.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "videogame.title, "
				 "videogame.id, "
				 "callnumber, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.accession_number, "
				 "videogame.type, "
				 "videogame.myoid, "
				 "videogame.front_cover ");
		searchstr.append("ORDER BY 1");
		searchstr.append(limitStr + offsetStr);
	      }
	  }
	else if(typefilter == "All Requested")
	  {
	    searchstr = "";

	    if(m_roles.isEmpty())
	      {
		searchstr.append("SELECT DISTINCT "
				 "item_request.requestdate, "
				 "book.title, "
				 "book.id, "
				 "book.callnumber, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.accession_number, "
				 "book.type, "
				 "book.myoid, "
				 "item_request.myoid AS requestoid, " +
				 bookFrontCover +
				 "FROM "
				 "book LEFT JOIN item_request ON "
				 "book.myoid = item_request.item_oid "
				 "AND item_request.type = 'Book' "
				 "WHERE "
				 "item_request.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_request.requestdate, "
				 "book.title, "
				 "book.id, "
				 "book.callnumber, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.accession_number, "
				 "book.type, "
				 "book.myoid, "
				 "item_request.myoid, "
				 "book.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_request.requestdate, "
				 "cd.title, "
				 "cd.id, "
				 "'' AS callnumber, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.accession_number, "
				 "cd.type, "
				 "cd.myoid, "
				 "item_request.myoid AS requestoid, " +
				 cdFrontCover +
				 "FROM "
				 "cd LEFT JOIN item_request ON "
				 "cd.myoid = item_request.item_oid "
				 "AND item_request.type = 'CD' "
				 "WHERE "
				 "item_request.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_request.requestdate, "
				 "cd.title, "
				 "cd.id, "
				 "callnumber, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.accession_number, "
				 "cd.type, "
				 "cd.myoid, "
				 "item_request.myoid, "
				 "cd.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_request.requestdate, "
				 "dvd.title, "
				 "dvd.id, "
				 "'' AS callnumber, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.accession_number, "
				 "dvd.type, "
				 "dvd.myoid, "
				 "item_request.myoid AS requestoid, " +
				 dvdFrontCover +
				 "FROM "
				 "dvd LEFT JOIN item_request ON "
				 "dvd.myoid = item_request.item_oid "
				 "AND item_request.type = 'DVD' "
				 "WHERE "
				 "item_request.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_request.requestdate, "
				 "dvd.title, "
				 "dvd.id, "
				 "callnumber, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.accession_number, "
				 "dvd.type, "
				 "dvd.myoid, "
				 "item_request.myoid, "
				 "dvd.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_request.requestdate, "
				 "grey_literature.document_title, "
				 "grey_literature.document_id, "
				 "'', "
				 "grey_literature.author, "
				 "grey_literature.document_date, "
				 "grey_literature.document_type, "
				 "'', "
				 "0.00, "
				 "'', "
				 "grey_literature.quantity, "
				 "grey_literature.location, "
				 "grey_literature.job_number, "
				 "grey_literature.type, "
				 "grey_literature.myoid, "
				 "item_request.myoid AS requestoid, " +
				 greyLiteratureFrontCover +
				 "FROM "
				 "grey_literature LEFT JOIN item_request ON "
				 "grey_literature.myoid = "
				 "item_request.item_oid "
				 "AND item_request.type = 'Grey Literature' "
				 "WHERE "
				 "item_request.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_request.requestdate, "
				 "grey_literature.document_title, "
				 "grey_literature.document_id, "
				 "grey_literature.author, "
				 "grey_literature.document_date, "
				 "grey_literature.document_type, "
				 "grey_literature.quantity, "
				 "grey_literature.location, "
				 "grey_literature.job_number, "
				 "grey_literature.type, "
				 "grey_literature.myoid, "
				 "item_request.myoid, "
				 "grey_literature.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_request.requestdate, "
				 "journal.title, "
				 "journal.id, "
				 "journal.callnumber, "
				 "journal.publisher, journal.pdate, "
				 "journal.category, "
				 "journal.language, "
				 "journal.price, journal.monetary_units, "
				 "journal.quantity, "
				 "journal.location, "
				 "journal.accession_number, "
				 "journal.type, "
				 "journal.myoid, "
				 "item_request.myoid AS requestoid, " +
				 journalFrontCover +
				 "FROM "
				 "journal LEFT JOIN item_request ON "
				 "journal.myoid = "
				 "item_request.item_oid "
				 "AND item_request.type = journal.type "
				 "WHERE "
				 "item_request.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_request.requestdate, "
				 "journal.title, "
				 "journal.id, "
				 "journal.callnumber, "
				 "journal.publisher, journal.pdate, "
				 "journal.category, "
				 "journal.language, "
				 "journal.price, journal.monetary_units, "
				 "journal.quantity, "
				 "journal.location, "
				 "journal.accession_number, "
				 "journal.type, "
				 "journal.myoid, "
				 "item_request.myoid, "
				 "journal.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_request.requestdate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.callnumber, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.accession_number, "
				 "magazine.type, "
				 "magazine.myoid, "
				 "item_request.myoid AS requestoid, " +
				 magazineFrontCover +
				 "FROM "
				 "magazine LEFT JOIN item_request ON "
				 "magazine.myoid = "
				 "item_request.item_oid "
				 "AND item_request.type = magazine.type "
				 "WHERE "
				 "item_request.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_request.requestdate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.callnumber, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.accession_number, "
				 "magazine.type, "
				 "magazine.myoid, "
				 "item_request.myoid, "
				 "magazine.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_request.requestdate, "
				 "videogame.title, "
				 "videogame.id, "
				 "'' AS callnumber, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.accession_number, "
				 "videogame.type, "
				 "videogame.myoid, "
				 "item_request.myoid AS requestoid, " +
				 videoGameFrontCover +
				 "FROM "
				 "videogame LEFT JOIN item_request ON "
				 "videogame.myoid = "
				 "item_request.item_oid "
				 "AND item_request.type = 'Video Game' "
				 "WHERE "
				 "item_request.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_request.requestdate, "
				 "videogame.title, "
				 "videogame.id, "
				 "callnumber, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.accession_number, "
				 "videogame.type, "
				 "videogame.myoid, "
				 "item_request.myoid, "
				 "videogame.front_cover ");
		searchstr.append("ORDER BY 1");
		searchstr.append(limitStr + offsetStr);
	      }
	    else // !m_roles.isEmpty()
	      {
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_request.requestdate, "
				 "book.title, "
				 "book.id, "
				 "book.callnumber, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.accession_number, "
				 "book.type, "
				 "book.myoid, "
				 "item_request.myoid AS requestoid, " +
				 bookFrontCover +
				 "FROM "
				 "member, "
				 "book LEFT JOIN item_request ON "
				 "book.myoid = item_request.item_oid "
				 "AND item_request.type = 'Book' "
				 "WHERE ");
		searchstr.append("item_request.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_request.requestdate, "
				 "book.title, "
				 "book.id, "
				 "book.callnumber, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.accession_number, "
				 "book.type, "
				 "book.myoid, "
				 "item_request.myoid, "
				 "book.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_request.requestdate, "
				 "cd.title, "
				 "cd.id, "
				 "'' AS callnumber, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.accession_number, "
				 "cd.type, "
				 "cd.myoid, "
				 "item_request.myoid AS requestoid, " +
				 cdFrontCover +
				 "FROM "
				 "member, "
				 "cd LEFT JOIN item_request ON "
				 "cd.myoid = item_request.item_oid "
				 "AND item_request.type = 'CD' "
				 "WHERE ");
		searchstr.append("item_request.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_request.requestdate, "
				 "cd.title, "
				 "cd.id, "
				 "callnumber, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.accession_number, "
				 "cd.type, "
				 "cd.myoid, "
				 "item_request.myoid, "
				 "cd.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_request.requestdate, "
				 "dvd.title, "
				 "dvd.id, "
				 "'' AS callnumber, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.accession_number, "
				 "dvd.type, "
				 "dvd.myoid, "
				 "item_request.myoid AS requestoid, " +
				 dvdFrontCover +
				 "FROM "
				 "member, "
				 "dvd LEFT JOIN item_request ON "
				 "dvd.myoid = item_request.item_oid "
				 "AND item_request.type = 'DVD' "
				 "WHERE ");
		searchstr.append("item_request.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_request.requestdate, "
				 "dvd.title, "
				 "dvd.id, "
				 "callnumber, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.accession_number, "
				 "dvd.type, "
				 "dvd.myoid, "
				 "item_request.myoid, "
				 "dvd.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_request.requestdate, "
				 "grey_literature.document_title, "
				 "grey_literature.document_id, "
				 "'', "
				 "grey_literature.author, "
				 "grey_literature.document_date, "
				 "grey_literature.document_type, "
				 "'', "
				 "0.00, "
				 "'', "
				 "grey_literature.quantity, "
				 "grey_literature.location, "
				 "grey_literature.job_number, "
				 "grey_literature.type, "
				 "grey_literature.myoid, "
				 "item_request.myoid AS requestoid, " +
				 greyLiteratureFrontCover +
				 "FROM "
				 "member, "
				 "grey_literature LEFT JOIN item_request ON "
				 "grey_literature.myoid = "
				 "item_request.item_oid "
				 "AND item_request.type = 'Grey Literature'  "
				 "WHERE ");
		searchstr.append("item_request.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_request.requestdate, "
				 "grey_literature.document_title, "
				 "grey_literature.document_id, "
				 "grey_literature.author, "
				 "grey_literature.document_date, "
				 "grey_literature.document_type, "
				 "grey_literature.quantity, "
				 "grey_literature.location, "
				 "grey_literature.job_number, "
				 "grey_literature.type, "
				 "grey_literature.myoid, "
				 "item_request.myoid, "
				 "grey_literature.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_request.requestdate, "
				 "journal.title, "
				 "journal.id, "
				 "journal.callnumber, "
				 "journal.publisher, journal.pdate, "
				 "journal.category, "
				 "journal.language, "
				 "journal.price, journal.monetary_units, "
				 "journal.quantity, "
				 "journal.location, "
				 "journal.accession_number, "
				 "journal.type, "
				 "journal.myoid, "
				 "item_request.myoid AS requestoid, " +
				 journalFrontCover +
				 "FROM "
				 "member, "
				 "journal LEFT JOIN item_request ON "
				 "journal.myoid = "
				 "item_request.item_oid "
				 "AND item_request.type = journal.type "
				 "WHERE ");
		searchstr.append("item_request.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_request.requestdate, "
				 "journal.title, "
				 "journal.id, "
				 "journal.callnumber, "
				 "journal.publisher, journal.pdate, "
				 "journal.category, "
				 "journal.language, "
				 "journal.price, journal.monetary_units, "
				 "journal.quantity, "
				 "journal.location, "
				 "journal.accession_number, "
				 "journal.type, "
				 "journal.myoid, "
				 "item_request.myoid, "
				 "journal.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_request.requestdate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.callnumber, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.accession_number, "
				 "magazine.type, "
				 "magazine.myoid, "
				 "item_request.myoid AS requestoid, " +
				 magazineFrontCover +
				 "FROM "
				 "member, "
				 "magazine LEFT JOIN item_request ON "
				 "magazine.myoid = "
				 "item_request.item_oid "
				 "AND item_request.type = magazine.type "
				 "WHERE ");
		searchstr.append("item_request.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_request.requestdate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.callnumber, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.accession_number, "
				 "magazine.type, "
				 "magazine.myoid, "
				 "item_request.myoid, "
				 "magazine.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_request.requestdate, "
				 "videogame.title, "
				 "videogame.id, "
				 "'' AS callnumber, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.accession_number, "
				 "videogame.type, "
				 "videogame.myoid, "
				 "item_request.myoid AS requestoid, " +
				 videoGameFrontCover +
				 "FROM "
				 "member, "
				 "videogame LEFT JOIN item_request ON "
				 "videogame.myoid = "
				 "item_request.item_oid "
				 "AND item_request.type = 'Video Game' "
				 "WHERE ");
		searchstr.append("item_request.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_request.requestdate, "
				 "videogame.title, "
				 "videogame.id, "
				 "callnumber, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.accession_number, "
				 "videogame.type, "
				 "videogame.myoid, "
				 "item_request.myoid, "
				 "videogame.front_cover ");
		searchstr.append("ORDER BY 1");
		searchstr.append(limitStr + offsetStr);
	      }
	  }
	else if(typefilter == "All Reserved")
	  {
	    searchstr = "";

	    if(m_roles.isEmpty())
	      {
		searchstr.append("SELECT DISTINCT "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "book.title, "
				 "book.id, "
				 "book.callnumber, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "book.accession_number, "
				 "book.type, "
				 "book.myoid, " +
				 bookFrontCover +
				 "FROM "
				 "book LEFT JOIN item_borrower ON "
				 "book.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = 'Book' "
				 "WHERE "
				 "item_borrower.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "book.title, "
				 "book.id, "
				 "book.callnumber, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.accession_number, "
				 "book.type, "
				 "book.myoid, "
				 "book.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "cd.title, "
				 "cd.id, "
				 "'' AS callnumber, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "cd.accession_number, "
				 "cd.type, "
				 "cd.myoid, " +
				 cdFrontCover +
				 "FROM "
				 "cd LEFT JOIN item_borrower ON "
				 "cd.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = 'CD' "
				 "WHERE "
				 "item_borrower.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "cd.title, "
				 "cd.id, "
				 "callnumber, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.accession_number, "
				 "cd.type, "
				 "cd.myoid, "
				 "cd.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "dvd.title, "
				 "dvd.id, "
				 "'' AS callnumber, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "dvd.accession_number, "
				 "dvd.type, "
				 "dvd.myoid, " +
				 dvdFrontCover +
				 "FROM "
				 "dvd LEFT JOIN item_borrower ON "
				 "dvd.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = 'DVD' "
				 "WHERE "
				 "item_borrower.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "dvd.title, "
				 "dvd.id, "
				 "callnumber, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.accession_number, "
				 "dvd.type, "
				 "dvd.myoid, "
				 "dvd.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "grey_literature.document_title, "
				 "grey_literature.document_id, "
				 "'', "
				 "grey_literature.author, "
				 "grey_literature.document_date, "
				 "grey_literature.document_type, "
				 "'', "
				 "0.00, "
				 "'', "
				 "grey_literature.quantity, "
				 "grey_literature.location, "
				 "1 - COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "grey_literature.job_number, "
				 "grey_literature.type, "
				 "grey_literature.myoid, " +
				 greyLiteratureFrontCover +
				 "FROM "
				 "grey_literature LEFT JOIN item_borrower ON "
				 "grey_literature.myoid = "
				 "item_borrower.item_oid "
				 "AND item_borrower.type = 'Grey Literature' "
				 "WHERE "
				 "item_borrower.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "grey_literature.document_title, "
				 "grey_literature.document_id, "
				 "grey_literature.author, "
				 "grey_literature.document_date, "
				 "grey_literature.document_type, "
				 "grey_literature.quantity, "
				 "grey_literature.location, "
				 "grey_literature.job_number, "
				 "grey_literature.type, "
				 "grey_literature.myoid, "
				 "grey_literature.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "journal.title, "
				 "journal.id, "
				 "journal.callnumber, "
				 "journal.publisher, journal.pdate, "
				 "journal.category, "
				 "journal.language, "
				 "journal.price, journal.monetary_units, "
				 "journal.quantity, "
				 "journal.location, "
				 "journal.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "journal.accession_number, "
				 "journal.type, "
				 "journal.myoid, " +
				 journalFrontCover +
				 "FROM "
				 "journal LEFT JOIN item_borrower ON "
				 "journal.myoid = "
				 "item_borrower.item_oid "
				 "AND item_borrower.type = journal.type "
				 "WHERE "
				 "item_borrower.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "journal.title, "
				 "journal.id, "
				 "journal.callnumber, "
				 "journal.publisher, journal.pdate, "
				 "journal.category, "
				 "journal.language, "
				 "journal.price, journal.monetary_units, "
				 "journal.quantity, "
				 "journal.location, "
				 "journal.accession_number, "
				 "journal.type, "
				 "journal.myoid, "
				 "journal.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.callnumber, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "magazine.accession_number, "
				 "magazine.type, "
				 "magazine.myoid, " +
				 magazineFrontCover +
				 "FROM "
				 "magazine LEFT JOIN item_borrower ON "
				 "magazine.myoid = "
				 "item_borrower.item_oid "
				 "AND item_borrower.type = magazine.type "
				 "WHERE "
				 "item_borrower.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.callnumber, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.accession_number, "
				 "magazine.type, "
				 "magazine.myoid, "
				 "magazine.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "videogame.title, "
				 "videogame.id, "
				 "'' AS callnumber, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "videogame.accession_number, "
				 "videogame.type, "
				 "videogame.myoid, " +
				 videoGameFrontCover +
				 "FROM "
				 "videogame LEFT JOIN item_borrower ON "
				 "videogame.myoid = "
				 "item_borrower.item_oid "
				 "AND item_borrower.type = 'Video Game' "
				 "WHERE "
				 "item_borrower.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "videogame.title, "
				 "videogame.id, "
				 "callnumber, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.accession_number, "
				 "videogame.type, "
				 "videogame.myoid, "
				 "videogame.front_cover ");
		searchstr.append("ORDER BY 1");
		searchstr.append(limitStr + offsetStr);
	      }
	    else // !m_roles.isEmpty()
	      {
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "book.title, "
				 "book.id, "
				 "book.callnumber, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "book.accession_number, "
				 "book.type, "
				 "book.myoid, " +
				 bookFrontCover +
				 "FROM "
				 "member, "
				 "book LEFT JOIN item_borrower ON "
				 "book.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = 'Book' "
				 "WHERE "
				 "LOWER(member.memberid) LIKE LOWER('");
		searchstr.append(searchstrArg.trimmed().isEmpty() ?
				 "%" : searchstrArg.trimmed());
		searchstr.append("') AND ");
		searchstr.append("item_borrower.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "book.title, "
				 "book.id, "
				 "book.callnumber, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.accession_number, "
				 "book.type, "
				 "book.myoid, "
				 "book.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "cd.title, "
				 "cd.id, "
				 "'' AS callnumber, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "cd.accession_number, "
				 "cd.type, "
				 "cd.myoid, " +
				 cdFrontCover +
				 "FROM "
				 "member, "
				 "cd LEFT JOIN item_borrower ON "
				 "cd.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = 'CD' "
				 "WHERE "
				 "LOWER(member.memberid) LIKE LOWER('");
		searchstr.append(searchstrArg.trimmed().isEmpty() ?
				 "%" : searchstrArg.trimmed());
		searchstr.append("') AND ");
		searchstr.append("item_borrower.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "cd.title, "
				 "cd.id, "
				 "callnumber, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.accession_number, "
				 "cd.type, "
				 "cd.myoid, "
				 "cd.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "dvd.title, "
				 "dvd.id, "
				 "'' AS callnumber, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "dvd.accession_number, "
				 "dvd.type, "
				 "dvd.myoid, " +
				 dvdFrontCover +
				 "FROM "
				 "member, "
				 "dvd LEFT JOIN item_borrower ON "
				 "dvd.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = 'DVD' "
				 "WHERE "
				 "LOWER(member.memberid) LIKE LOWER('");
		searchstr.append(searchstrArg.trimmed().isEmpty() ?
				 "%" : searchstrArg.trimmed());
		searchstr.append("') AND ");
		searchstr.append("item_borrower.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "dvd.title, "
				 "dvd.id, "
				 "callnumber, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.accession_number, "
				 "dvd.type, "
				 "dvd.myoid, "
				 "dvd.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "grey_literature.document_title, "
				 "grey_literature.document_id, "
				 "'', "
				 "grey_literature.author, "
				 "grey_literature.document_date, "
				 "grey_literature.document_type, "
				 "'', "
				 "0.00, "
				 "'', "
				 "grey_literature.quantity, "
				 "grey_literature.location, "
				 "1 - COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "grey_literature.job_number, "
				 "grey_literature.type, "
				 "grey_literature.myoid, " +
				 greyLiteratureFrontCover +
				 "FROM "
				 "member, "
				 "grey_literature LEFT JOIN item_borrower ON "
				 "grey_literature.myoid = "
				 "item_borrower.item_oid "
				 "AND item_borrower.type = 'Grey Literature' "
				 "WHERE "
				 "LOWER(member.memberid) LIKE LOWER('");
		searchstr.append(searchstrArg.trimmed().isEmpty() ?
				 "%" : searchstrArg.trimmed());
		searchstr.append("') AND ");
		searchstr.append("item_borrower.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "grey_literature.document_title, "
				 "grey_literature.document_id, "
				 "grey_literature.author, "
				 "grey_literature.document_date, "
				 "grey_literature.document_type, "
				 "grey_literature.quantity, "
				 "grey_literature.location, "
				 "grey_literature.job_number, "
				 "grey_literature.type, "
				 "grey_literature.myoid, "
				 "grey_literature.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "journal.title, "
				 "journal.id, "
				 "journal.callnumber, "
				 "journal.publisher, journal.pdate, "
				 "journal.category, "
				 "journal.language, "
				 "journal.price, journal.monetary_units, "
				 "journal.quantity, "
				 "journal.location, "
				 "journal.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "journal.accession_number, "
				 "journal.type, "
				 "journal.myoid, " +
				 journalFrontCover +
				 "FROM "
				 "member, "
				 "journal LEFT JOIN item_borrower ON "
				 "journal.myoid = "
				 "item_borrower.item_oid "
				 "AND item_borrower.type = journal.type "
				 "WHERE "
				 "LOWER(member.memberid) LIKE LOWER('");
		searchstr.append(searchstrArg.trimmed().isEmpty() ?
				 "%" : searchstrArg.trimmed());
		searchstr.append("') AND ");
		searchstr.append("item_borrower.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "journal.title, "
				 "journal.id, "
				 "journal.callnumber, "
				 "journal.publisher, journal.pdate, "
				 "journal.category, "
				 "journal.language, "
				 "journal.price, journal.monetary_units, "
				 "journal.quantity, "
				 "journal.location, "
				 "journal.accession_number, "
				 "journal.type, "
				 "journal.myoid, "
				 "journal.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.callnumber, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "magazine.accession_number, "
				 "magazine.type, "
				 "magazine.myoid, " +
				 magazineFrontCover +
				 "FROM "
				 "member, "
				 "magazine LEFT JOIN item_borrower ON "
				 "magazine.myoid = "
				 "item_borrower.item_oid "
				 "AND item_borrower.type = magazine.type "
				 "WHERE "
				 "LOWER(member.memberid) LIKE LOWER('");
		searchstr.append(searchstrArg.trimmed().isEmpty() ?
				 "%" : searchstrArg.trimmed());
		searchstr.append("') AND ");
		searchstr.append("item_borrower.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.callnumber, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.accession_number, "
				 "magazine.type, "
				 "magazine.myoid, "
				 "magazine.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "videogame.title, "
				 "videogame.id, "
				 "'' AS callnumber, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower.item_oid) AS "
				 "total_reserved, "
				 "videogame.accession_number, "
				 "videogame.type, "
				 "videogame.myoid, " +
				 videoGameFrontCover +
				 "FROM "
				 "member, "
				 "videogame LEFT JOIN item_borrower ON "
				 "videogame.myoid = "
				 "item_borrower.item_oid "
				 "AND item_borrower.type = 'Video Game' "
				 "WHERE "
				 "LOWER(member.memberid) LIKE LOWER('");
		searchstr.append(searchstrArg.trimmed().isEmpty() ?
				 "%" : searchstrArg.trimmed());
		searchstr.append("') AND ");
		searchstr.append("item_borrower.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "member.telephone_num, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "videogame.title, "
				 "videogame.id, "
				 "callnumber, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.accession_number, "
				 "videogame.type, "
				 "videogame.myoid, "
				 "videogame.front_cover ");
		searchstr.append("ORDER BY 1");
		searchstr.append(limitStr + offsetStr);
	      }
	  }
	else if(typefilter == "Video Games")
	  {
	    searchstr = "SELECT DISTINCT videogame.title, "
	      "videogame.vgrating, "
	      "videogame.vgplatform, "
	      "videogame.vgmode, "
	      "videogame.publisher, "
	      "videogame.rdate, "
	      "videogame.place, "
	      "videogame.genre, "
	      "videogame.language, "
	      "videogame.id, "
	      "videogame.price, "
	      "videogame.monetary_units, "
	      "videogame.quantity, "
	      "videogame.location, "
	      "videogame.quantity - "
	      "COUNT(item_borrower.item_oid) "
	      "AS availability, "
	      "COUNT(item_borrower.item_oid) AS total_reserved, "
	      "videogame.accession_number, "
	      "videogame.type, "
	      "videogame.myoid, " +
	      videoGameFrontCover +
	      "FROM "
	      "videogame LEFT JOIN item_borrower ON "
	      "videogame.myoid = item_borrower.item_oid "
	      "AND item_borrower.type = 'Video Game' "
	      "GROUP BY "
	      "videogame.title, "
	      "videogame.vgrating, "
	      "videogame.vgplatform, "
	      "videogame.vgmode, "
	      "videogame.publisher, "
	      "videogame.rdate, "
	      "videogame.place, "
	      "videogame.genre, "
	      "videogame.language, "
	      "videogame.id, "
	      "videogame.price, "
	      "videogame.monetary_units, "
	      "videogame.quantity, "
	      "videogame.location, "
	      "videogame.accession_number, "
	      "videogame.type, "
	      "videogame.myoid, "
	      "videogame.front_cover "
	      "ORDER BY "
	      "videogame.title" +
	      limitStr + offsetStr;
	  }
	else if(typefilter == "Books")
	  {
	    searchstr = "SELECT DISTINCT book.title, "
	      "book.author, "
	      "book.publisher, book.pdate, book.place, "
	      "book.edition, "
	      "book.category, book.language, "
	      "book.id, "
	      "book.price, book.monetary_units, "
	      "book.quantity, "
	      "book.binding_type, "
	      "book.location, "
	      "book.isbn13, "
	      "book.lccontrolnumber, "
	      "book.callnumber, "
	      "book.deweynumber, "
	      "book.quantity - "
	      "COUNT(item_borrower.item_oid) "
	      "AS availability, "
	      "COUNT(item_borrower.item_oid) AS total_reserved, "
	      "book.originality, "
	      "book.condition, "
	      "book.accession_number, "
	      "book.type, "
	      "book.myoid, " +
	      bookFrontCover +
	      "FROM "
	      "book LEFT JOIN item_borrower ON "
	      "book.myoid = item_borrower.item_oid "
	      "AND item_borrower.type = 'Book' "
	      "GROUP BY "
	      "book.title, "
	      "book.author, "
	      "book.publisher, book.pdate, book.place, "
	      "book.edition, "
	      "book.category, book.language, "
	      "book.id, "
	      "book.price, book.monetary_units, "
	      "book.quantity, "
	      "book.binding_type, "
	      "book.location, "
	      "book.isbn13, "
	      "book.lccontrolnumber, "
	      "book.callnumber, "
	      "book.deweynumber, "
	      "book.originality, "
	      "book.condition, "
	      "book.accession_number, "
	      "book.type, "
	      "book.myoid, "
	      "book.front_cover "
	      "ORDER BY "
	      "book.title" +
	      limitStr + offsetStr;
	  }
	else if(typefilter == "DVDs")
	  {
	    searchstr = "SELECT DISTINCT dvd.title, "
	      "dvd.dvdformat, "
	      "dvd.studio, "
	      "dvd.rdate, "
	      "dvd.dvddiskcount, "
	      "dvd.dvdruntime, "
	      "dvd.category, "
	      "dvd.language, "
	      "dvd.id, "
	      "dvd.price, "
	      "dvd.monetary_units, "
	      "dvd.quantity, "
	      "dvd.location, "
	      "dvd.dvdrating, "
	      "dvd.dvdregion, "
	      "dvd.dvdaspectratio, "
	      "dvd.quantity - "
	      "COUNT(item_borrower.item_oid) "
	      "AS availability, "
	      "COUNT(item_borrower.item_oid) AS total_reserved, "
	      "dvd.accession_number, "
	      "dvd.type, "
	      "dvd.myoid, " +
	      dvdFrontCover +
	      "FROM "
	      "dvd LEFT JOIN item_borrower ON "
	      "dvd.myoid = item_borrower.item_oid "
	      "AND item_borrower.type = 'DVD' "
	      "GROUP BY "
	      "dvd.title, "
	      "dvd.dvdformat, "
	      "dvd.studio, "
	      "dvd.rdate, "
	      "dvd.dvddiskcount, "
	      "dvd.dvdruntime, "
	      "dvd.category, "
	      "dvd.language, "
	      "dvd.id, "
	      "dvd.price, "
	      "dvd.monetary_units, "
	      "dvd.quantity, "
	      "dvd.location, "
	      "dvd.dvdrating, "
	      "dvd.dvdregion, "
	      "dvd.dvdaspectratio, "
	      "dvd.accession_number, "
	      "dvd.type, "
	      "dvd.myoid, "
	      "dvd.front_cover "
	      "ORDER BY "
	      "dvd.title" +
	      limitStr + offsetStr;
	  }
	else if(typefilter == "Grey Literature")
	  {
	    searchstr = "SELECT DISTINCT grey_literature.author, "
	      "grey_literature.client, "
	      "grey_literature.document_code_a, "
	      "grey_literature.document_code_b, "
	      "grey_literature.document_date, "
	      "grey_literature.document_id, "
	      "grey_literature.document_status, "
	      "grey_literature.document_title, "
	      "grey_literature.document_type, "
	      "grey_literature.job_number, "
	      "grey_literature.location, "
	      "(SELECT COUNT(myoid) FROM grey_literature_files "
	      "WHERE grey_literature_files.item_oid = grey_literature.myoid) "
	      "AS file_count, "
	      "1 - COUNT(item_borrower.item_oid) AS availability, "
	      "COUNT(item_borrower.item_oid) AS total_reserved, "
	      "grey_literature.type, "
	      "grey_literature.myoid, " +
	      greyLiteratureFrontCover +
	      "FROM "
	      "grey_literature LEFT JOIN item_borrower ON "
	      "grey_literature.myoid = item_borrower.item_oid "
	      "AND item_borrower.type = 'Grey Literature' "
	      "GROUP BY "
	      "grey_literature.author, "
	      "grey_literature.client, "
	      "grey_literature.document_code_a, "
	      "grey_literature.document_code_b, "
	      "grey_literature.document_date, "
	      "grey_literature.document_id, "
	      "grey_literature.document_status, "
	      "grey_literature.document_title, "
	      "grey_literature.document_type, "
	      "grey_literature.job_number, "
	      "grey_literature.location, "
	      "grey_literature.type, "
	      "grey_literature.myoid, "
	      "grey_literature.front_cover "
	      "ORDER BY "
	      "grey_literature.author" +
	      limitStr + offsetStr;
	  }
	else if(typefilter == "Music CDs")
	  {
	    searchstr = "SELECT DISTINCT cd.title, "
	      "cd.artist, "
	      "cd.cdformat, "
	      "cd.recording_label, "
	      "cd.rdate, "
	      "cd.cddiskcount, "
	      "cd.cdruntime, "
	      "cd.category, "
	      "cd.language, "
	      "cd.id, "
	      "cd.price, "
	      "cd.monetary_units, "
	      "cd.quantity, "
	      "cd.location, "
	      "cd.cdaudio, "
	      "cd.cdrecording, "
	      "cd.quantity - COUNT(item_borrower.item_oid) AS "
	      "availability, "
	      "COUNT(item_borrower.item_oid) AS total_reserved, "
	      "cd.accession_number, "
	      "cd.type, "
	      "cd.myoid, " +
	      cdFrontCover +
	      "FROM "
	      "cd LEFT JOIN item_borrower ON "
	      "cd.myoid = item_borrower.item_oid "
	      "AND item_borrower.type = 'CD' "
	      "GROUP BY "
	      "cd.title, "
	      "cd.artist, "
	      "cd.cdformat, "
	      "cd.recording_label, "
	      "cd.rdate, "
	      "cd.cddiskcount, "
	      "cd.cdruntime, "
	      "cd.category, "
	      "cd.language, "
	      "cd.id, "
	      "cd.price, "
	      "cd.monetary_units, "
	      "cd.quantity, "
	      "cd.location, "
	      "cd.cdaudio, "
	      "cd.cdrecording, "
	      "cd.accession_number, "
	      "cd.type, "
	      "cd.myoid, "
	      "cd.front_cover "
	      "ORDER BY "
	      "cd.title" +
	      limitStr + offsetStr;
	  }
	else if(typefilter == "Photograph Collections")
	  {
	    searchstr = "SELECT DISTINCT photograph_collection.title, "
	      "photograph_collection.id, "
	      "photograph_collection.location, "
	      "COUNT(photograph.myoid) AS photograph_count, "
	      "photograph_collection.about, "
	      "photograph_collection.accession_number, "
	      "photograph_collection.type, "
	      "photograph_collection.myoid, " +
	      photographCollectionFrontCover +
	      "FROM "
	      "photograph_collection "
	      "LEFT JOIN photograph "
	      "ON photograph_collection.myoid = photograph.collection_oid "
	      "GROUP BY "
	      "photograph_collection.title, "
	      "photograph_collection.id, "
	      "photograph_collection.location, "
	      "photograph_collection.about, "
	      "photograph_collection.accession_number, "
	      "photograph_collection.type, "
	      "photograph_collection.myoid, "
	      "photograph_collection.image_scaled "
	      "ORDER BY "
	      "photograph_collection.title" +
	      limitStr + offsetStr;
	  }
	else if(typefilter == "Journals" || typefilter == "Magazines")
	  {
	    if(typefilter == "Journals")
	      type = "Journal";
	    else
	      type = "Magazine";

	    QString frontCover("'' AS front_cover ");

	    if(m_otheroptions->showMainTableImages())
	      {
		if(type == "Journal")
		  frontCover = "journal.front_cover ";
		else
		  frontCover = "magazine.front_cover ";
	      }

	    searchstr = QString("SELECT DISTINCT %1.title, "
				"%1.publisher, %1.pdate, "
				"%1.place, "
				"%1.issuevolume, %1.issueno, "
				"%1.category, %1.language, "
				"%1.id, "
				"%1.price, %1.monetary_units, "
				"%1.quantity, "
				"%1.location, "
				"%1.lccontrolnumber, "
				"%1.callnumber, "
				"%1.deweynumber, "
				"%1.quantity - "
				"COUNT(item_borrower.item_oid) AS "
				"availability, "
				"COUNT(item_borrower.item_oid) AS "
				"total_reserved, "
				"%1.accession_number, "
				"%1.type, "
				"%1.myoid, " +
				frontCover +
				"FROM "
				"%1 LEFT JOIN item_borrower ON "
				"%1.myoid = "
				"item_borrower.item_oid "
				"AND item_borrower.type = %1.type "
				"WHERE "
				"%1.type = '%1' "
				"GROUP BY "
				"%1.title, "
				"%1.publisher, %1.pdate, "
				"%1.place, "
				"%1.issuevolume, %1.issueno, "
				"%1.category, %1.language, "
				"%1.id, "
				"%1.price, %1.monetary_units, "
				"%1.quantity, "
				"%1.location, "
				"%1.lccontrolnumber, "
				"%1.callnumber, "
				"%1.deweynumber, "
				"%1.accession_number, "
				"%1.type, "
				"%1.myoid, "
				"%1.front_cover "
				"ORDER BY "
				"%1.title").arg(type);
	    searchstr += limitStr + offsetStr;
	  }

	break;
      }
    case POPULATE_SEARCH: default:
      {
	if(typefilter == "All")
	  {
	    searchstr = searchstrArg;

	    if(searchstr.lastIndexOf("LIMIT") != -1)
	      searchstr.remove(searchstr.lastIndexOf("LIMIT"),
			       searchstr.length());

	    searchstr += limitStr + offsetStr;
	  }
	else if(typefilter == "Books")
	  {
	    if(!searchstr.contains("ORDER BY"))
	      {
		searchstr.append(searchstrArg);
		searchstr.append("GROUP BY book.title, "
				 "book.author, "
				 "book.publisher, book.pdate, book.place, "
				 "book.edition, "
				 "book.category, book.language, "
				 "book.id, "
				 "book.price, "
				 "book.monetary_units, "
				 "book.quantity, "
				 "book.binding_type, "
				 "book.location, "
				 "book.isbn13, "
				 "book.lccontrolnumber, "
				 "book.callnumber, "
				 "book.deweynumber, "
				 "book.originality, "
				 "book.condition, "
				 "book.accession_number, "
				 "book.type, "
				 "book.myoid, "
				 "book.front_cover "
				 "ORDER BY book.title");
	      }

	    if(searchstr.lastIndexOf("LIMIT") != -1)
	      searchstr.remove(searchstr.lastIndexOf("LIMIT"),
			       searchstr.length());

	    searchstr += limitStr + offsetStr;
	  }
	else if(typefilter == "DVDs")
	  {
	    if(!searchstr.contains("ORDER BY"))
	      {
		searchstr.append(searchstrArg);
		searchstr.append("GROUP BY "
				 "dvd.title, "
				 "dvd.dvdformat, "
				 "dvd.studio, "
				 "dvd.rdate, "
				 "dvd.dvddiskcount, "
				 "dvd.dvdruntime, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.id, "
				 "dvd.price, "
				 "dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.dvdrating, "
				 "dvd.dvdregion, "
				 "dvd.dvdaspectratio, "
				 "dvd.accession_number, "
				 "dvd.type, "
				 "dvd.myoid, "
				 "dvd.front_cover "
				 "ORDER BY "
				 "dvd.title");
	      }

	    if(searchstr.lastIndexOf("LIMIT") != -1)
	      searchstr.remove(searchstr.lastIndexOf("LIMIT"),
			       searchstr.length());

	    searchstr += limitStr + offsetStr;
	  }
	else if(typefilter == "Grey Literature")
	  {
	    if(!searchstr.contains("ORDER BY"))
	      {
		searchstr.append(searchstrArg);
		searchstr.append("GROUP BY grey_literature.document_title, "
				 "grey_literature.document_id, "
				 "grey_literature.location, "
				 "grey_literature.notes, "
				 "grey_literature.job_number, "
				 "grey_literature.type, "
				 "grey_literature.myoid, "
				 "grey_literature.front_cover "
				 "ORDER BY grey_literature.document_title");
	      }

	    if(searchstr.lastIndexOf("LIMIT") != -1)
	      searchstr.remove
		(searchstr.lastIndexOf("LIMIT"), searchstr.length());

	    searchstr += limitStr + offsetStr;
	  }
	else if(typefilter == "Journals")
	  {
	    if(!searchstr.contains("ORDER BY"))
	      {
		searchstr.append(searchstrArg);
		searchstr.append("GROUP BY journal.title, "
				 "journal.publisher, journal.pdate, "
				 "journal.place, "
				 "journal.issuevolume, "
				 "journal.issueno, "
				 "journal.category, journal.language, "
				 "journal.id, "
				 "journal.price, "
				 "journal.monetary_units, "
				 "journal.quantity, "
				 "journal.location, "
				 "journal.lccontrolnumber, "
				 "journal.callnumber, "
				 "journal.deweynumber, "
				 "journal.accession_number, "
				 "journal.type, "
				 "journal.myoid, "
				 "journal.front_cover "
				 "ORDER BY journal.title, "
				 "journal.issuevolume, journal.issueno");
	      }

	    if(searchstr.lastIndexOf("LIMIT") != -1)
	      searchstr.remove(searchstr.lastIndexOf("LIMIT"),
			       searchstr.length());

	    searchstr += limitStr + offsetStr;
	  }
	else if(typefilter == "Magazines")
	  {
	    if(!searchstr.contains("ORDER BY"))
	      {
		searchstr.append(searchstrArg);
		searchstr.append("GROUP BY magazine.title, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.place, "
				 "magazine.issuevolume, "
				 "magazine.issueno, "
				 "magazine.category, magazine.language, "
				 "magazine.id, "
				 "magazine.price, "
				 "magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.lccontrolnumber, "
				 "magazine.callnumber, "
				 "magazine.deweynumber, "
				 "magazine.accession_number, "
				 "magazine.type, "
				 "magazine.myoid, "
				 "magazine.front_cover "
				 "ORDER BY magazine.title, "
				 "magazine.issuevolume, magazine.issueno");
	      }

	    if(searchstr.lastIndexOf("LIMIT") != -1)
	      searchstr.remove(searchstr.lastIndexOf("LIMIT"),
			       searchstr.length());

	    searchstr += limitStr + offsetStr;
	  }
	else if(typefilter == "Music CDs")
	  {
	    if(!searchstr.contains("ORDER BY"))
	      {
		searchstr.append(searchstrArg);
		searchstr.append("GROUP BY "
				 "cd.title, "
				 "cd.artist, "
				 "cd.cdformat, "
				 "cd.recording_label, "
				 "cd.rdate, "
				 "cd.cddiskcount, "
				 "cd.cdruntime, "
				 "cd.category, "
				 "cd.language, "
				 "cd.id, "
				 "cd.price, "
				 "cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.cdaudio, "
				 "cd.cdrecording, "
				 "cd.accession_number, "
				 "cd.type, "
				 "cd.myoid, "
				 "cd.front_cover "
				 "ORDER BY "
				 "cd.title");
	      }

	    if(searchstr.lastIndexOf("LIMIT") != -1)
	      searchstr.remove(searchstr.lastIndexOf("LIMIT"),
			       searchstr.length());

	    searchstr += limitStr + offsetStr;
	  }
	else if(typefilter == "Photograph Collections")
	  {
	    if(!searchstr.contains("ORDER BY"))
	      {
		searchstr.append(searchstrArg);
		searchstr.append("GROUP BY photograph_collection.title, "
				 "photograph_collection.id, "
				 "photograph_collection.location, "
				 "photograph_collection.about, "
				 "photograph_collection.accession_number, "
				 "photograph_collection.type, "
				 "photograph_collection.myoid, "
				 "photograph_collection.image_scaled "
				 "ORDER BY photograph_collection.title");
	      }

	    if(searchstr.lastIndexOf("LIMIT") != -1)
	      searchstr.remove(searchstr.lastIndexOf("LIMIT"),
			       searchstr.length());

	    searchstr += limitStr + offsetStr;
	  }
	else if(typefilter == "Video Games")
	  {
	    if(!searchstr.contains("ORDER BY"))
	      {
		searchstr.append(searchstrArg);
		searchstr.append("GROUP BY "
				 "videogame.title, "
				 "videogame.vgrating, "
				 "videogame.vgplatform, "
				 "videogame.vgmode, "
				 "videogame.publisher, "
				 "videogame.rdate, "
				 "videogame.place, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.id, "
				 "videogame.price, "
				 "videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.accession_number, "
				 "videogame.type, "
				 "videogame.myoid, "
				 "videogame.front_cover "
				 "ORDER BY "
				 "videogame.title");
	      }

	    if(searchstr.lastIndexOf("LIMIT") != -1)
	      searchstr.remove(searchstr.lastIndexOf("LIMIT"),
			       searchstr.length());

	    searchstr += limitStr + offsetStr;
	  }

	break;
      }
    case POPULATE_SEARCH_BASIC:
      {
	searchstr = searchstrArg;

	if(searchstr.lastIndexOf("LIMIT") != -1)
	  searchstr.remove(searchstr.lastIndexOf("LIMIT"),
			   searchstr.length());

	searchstr += limitStr + offsetStr;
	break;
      }
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(ui.table->rowCount() == 0)
    ui.itemsCountLabel->setText(tr("0 Results"));
  else
    ui.itemsCountLabel->setText
      (QString(tr("%1 Result(s)")).arg(ui.table->rowCount()));

  QSqlQuery query(m_db);

  if(limit == -1)
    query.setForwardOnly(true);

  if(!query.exec(searchstr))
    {
      if(progress)
	/*
	** The progress dialog should be invisible as we have not shown it.
	*/

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

      addError(QString(tr("Database Error")),
	       QString(tr("Unable to retrieve the data required for "
			  "populating the main views.")),
	       query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			    tr("Unable to retrieve the data required for "
			       "populating the main views."));
      QApplication::processEvents();
      return 1;
    }

  prepareRequestToolButton(typefilter);

  auto found = false;

  for(int ii = 0; ii < ui.menu_Category->actions().size(); ii++)
    if(typefilter ==
       ui.menu_Category->actions().at(ii)->data().toString())
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

  if(search_type != CUSTOM_QUERY)
    ui.table->resetTable(dbUserName(), typefilter, m_roles);
  else
    ui.table->resetTable(dbUserName(), "Custom", m_roles);

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

  m_queryOffset = offset;

  if(search_type == POPULATE_SEARCH_BASIC)
    m_lastSearchStr = searchstrArg;
  else if(typefilter != "All Overdue" &&
	  typefilter != "All Requested" &&
	  typefilter != "All Reserved")
    m_lastSearchStr = searchstr;
  else
    m_lastSearchStr = searchstrArg;

  m_lastSearchType = search_type;
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

  auto setRowCount = true;
  int iconTableColumnIdx = 0;
  int iconTableRowIdx = 0;

  /*
  ** Adjust the dimensions of the graphics scene if pagination
  ** is effectively disabled.
  */

  if(limit == -1)
    {
      auto size = biblioteq_misc_functions::sqliteQuerySize
	(searchstr, m_db, __FILE__, __LINE__, this);

      if(size > 0)
	ui.graphicsView->setSceneRect(0.0,
				      0.0,
				      5.0 * 130.0,
				      (size / 5.0) * 200.0 + 200.0);

      if(size >= 0)
	{
	  setRowCount = false;
	  ui.table->setRowCount(size);
	}
    }

  if(limit != -1 &&
     m_db.driver()->hasFeature(QSqlDriver::QuerySize) &&
     progress)
    progress->setMaximum(query.size());

  QSettings settings;
  QStringList columnNames;
  auto showToolTips = settings.value("show_maintable_tooltips", false).toBool();

  if(search_type == CUSTOM_QUERY)
    {
      auto record(query.record());

      for(int ii = 0; ii < record.count(); ii++)
	if(!columnNames.contains(record.fieldName(ii)))
	  columnNames.append(record.fieldName(ii));
    }
  else
    for(int ii = 0; ii < ui.table->columnCount(); ii++)
      columnNames.append(ui.table->horizontalHeaderItem(ii)->text());

  QString dateFormat("");
  int totalRows = 0;

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

  if(m_db.driverName() == "QPSQL")
    {
      setRowCount = false;
      ui.table->setRowCount(query.size());
    }

  i = -1;

  QFontMetrics fontMetrics(ui.table->font());
  auto availabilityColors = this->availabilityColors();
  auto booksAccessionNumberIndex = m_otheroptions->booksAccessionNumberIndex();
  auto showMainTableImages = m_otheroptions->showMainTableImages();

  while(i++, query.next())
    {
      if(progress && progress->wasCanceled())
	break;

      QString itemType = "";
      biblioteq_graphicsitempixmap *pixmapItem = nullptr;
      biblioteq_numeric_table_item *availabilityItem = nullptr;
      quint64 myoid = 0;

      if(query.isValid())
	{
	  QString tooltip("");
	  QTableWidgetItem *first = nullptr;
	  auto record(query.record());

	  if(showToolTips)
	    {
	      tooltip = "<html>";

	      for(int j = 0; j < record.count(); j++)
		{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
		  if(QMetaType::Type(record.field(j).metaType().id()) ==
		     QMetaType::QByteArray ||
#else
		  if(record.field(j).type() == QVariant::ByteArray ||
#endif
		     record.fieldName(j).contains("cover") ||
		     record.fieldName(j).contains("image"))
		    continue;

		  QString columnName("");

		  if(m_db.driverName() == "QSQLITE" &&
		     search_type != CUSTOM_QUERY)
		    columnName = columnNames.value(j + 1);
		  else
		    columnName = columnNames.value(j);

		  if(columnName.isEmpty())
		    columnName = "N/A";

		  tooltip.append("<b>");
		  tooltip.append(columnName);
		  tooltip.append(":</b> ");

		  if(record.field(j).tableName() == "book" &&
		     (record.fieldName(j) == "id" ||
		      record.fieldName(j) == "isbn13"))
		    {
		      auto str(query.value(j).toString().trimmed());

		      if(record.fieldName(j) == "id")
			str = m_otheroptions->isbn10DisplayFormat(str);
		      else
			str = m_otheroptions->isbn13DisplayFormat(str);

		      tooltip.append(str);
		    }
		  else
		    tooltip.append
		      (query.value(j).
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
		      auto date
			(QDate::
			 fromString(query.value(j).toString(), "MM/dd/yyyy"));

		      if(dateFormat.isEmpty())
			str = date.toString(Qt::ISODate);
		      else
			str = date.toString(dateFormat);

		      if(str.isEmpty())
			str = query.value(j).toString().trimmed();
		    }
		  else
		    str = query.value(j).toString().trimmed();
		}

	      if(search_type == CUSTOM_QUERY)
		if(!tmplist.contains(fieldName))
		  {
		    tmplist.append(fieldName);
		    ui.table->setColumnCount(tmplist.size());
		  }

	      if(record.field(j).tableName() == "book" &&
		 (fieldName == "id" ||
		  fieldName == "isbn13"))
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
			(query.value(j).toDouble());
		      str = QString::number(query.value(j).toDouble(), 'f', 2);
		    }
		  else
		    {
		      item = new biblioteq_numeric_table_item
			(query.value(j).toInt());

		      if(availabilityColors &&
			 fieldName.endsWith("availability"))
			availabilityItem =
			  dynamic_cast<biblioteq_numeric_table_item *> (item);
		    }
		}
	      else if(fieldName.endsWith("callnumber"))
		{
		  str = query.value(j).toString().trimmed();
		  item = new biblioteq_callnum_table_item(str);
		}
	      else if(fieldName.endsWith("front_cover") ||
		      fieldName.endsWith("image_scaled"))
		{
		  QImage image;

		  if(showMainTableImages)
		    {
		      if(!query.isNull(j))
			{
			  image.loadFromData
			    (QByteArray::fromBase64(query.value(j).
						    toByteArray()));

			  if(image.isNull())
			    image.loadFromData(query.value(j).toByteArray());
			}
		    }

		  if(image.isNull())
		    image = QImage(":/no_image.png");

		  /*
		  ** The size of no_image.png is 126x187.
		  */

		  if(!image.isNull())
		    image = image.scaled
		      (126, 187, Qt::KeepAspectRatio, Qt::SmoothTransformation);

		  pixmapItem = new biblioteq_graphicsitempixmap
		    (QPixmap::fromImage(image), nullptr);

		  if(iconTableRowIdx == 0)
		    pixmapItem->setPos(140 * iconTableColumnIdx, 15);
		  else
		    pixmapItem->setPos(140 * iconTableColumnIdx,
				       200 * iconTableRowIdx + 15);

		  pixmapItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
		  ui.graphicsView->scene()->addItem(pixmapItem);
		  iconTableColumnIdx += 1;

		  if(iconTableColumnIdx >= 5)
		    {
		      iconTableRowIdx += 1;
		      iconTableColumnIdx = 0;
		    }
		}
	      else
		item = new QTableWidgetItem();

	      if(item != nullptr)
		{
		  item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		  item->setText
		    (str.simplified().replace("<br>", " ").simplified().
		     trimmed());

		  if(j == 0)
		    {
		      first = item;

		      if(setRowCount)
			ui.table->setRowCount(ui.table->rowCount() + 1);
		    }

		  if(!tooltip.isEmpty())
		    item->setToolTip(tooltip);

		  if(m_db.driverName() == "QSQLITE" &&
		     search_type != CUSTOM_QUERY)
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

	  if(m_db.driverName() == "QSQLITE" && search_type != CUSTOM_QUERY)
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

	  totalRows += 1;
	}

      if(query.isValid())
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
    }

  ui.table->setRowCount(totalRows);
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

  if(search_type == CUSTOM_QUERY)
    {
      if(tmplist.isEmpty())
	{
	  auto record(query.record());

	  for(int ii = 0; ii < record.count(); ii++)
	    if(!tmplist.contains(record.fieldName(ii)))
	      tmplist.append(record.fieldName(ii));
	}

      ui.table->setColumnCount(tmplist.size());
      ui.table->setColumnNames(tmplist);
      ui.table->setHorizontalHeaderLabels(tmplist);
      tmplist.clear();
      addConfigOptions("Custom");
    }
  else if(search_type == POPULATE_SEARCH_BASIC)
    {
      if(ui.table->rowCount() == 0)
	{
	  ui.case_insensitive->setEnabled(true);
	  ui.search->setEnabled(true);
	  ui.searchType->setEnabled(true);
	}
    }

  if(search_type != CUSTOM_QUERY)
    addConfigOptions(typefilter);

  if(ui.actionAutomatically_Resize_Column_Widths->isChecked())
    slotResizeColumns();

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

void biblioteq::prepareContextMenus()
{
  if(m_menu)
    m_menu->clear();
  else
    m_menu = new QMenu(this);

  if(m_roles.contains("administrator") ||
     m_roles.contains("librarian"))
    {
      if(getTypeFilterString() == "All Requested" &&
	 !m_roles.contains("librarian"))
	m_menu->addAction(tr("Cancel Requested Request(s)"),
			  this,
			  SLOT(slotRequest(void)));

      m_menu->addAction(tr("Delete Selected Item(s)"),
			this,
			SLOT(slotDelete(void)));
      m_menu->addAction(tr("Duplicate Selected Item(s)..."),
			this,
			SLOT(slotDuplicate(void)));
      m_menu->addAction(tr("Modify Selected Item(s)..."),
			this,
			SLOT(slotModify(void)));
      m_menu->addAction(tr("Print Current View..."),
			this,
			SLOT(slotPrintView(void)));

      if(getTypeFilterString() != "All Requested" &&
	 !m_roles.contains("librarian"))
	m_menu->addAction(tr("Reserve Selected Item..."),
			  this,
			  SLOT(slotReserveCopy(void)));
    }
  else if(m_roles.contains("circulation"))
    {
      if(getTypeFilterString() == "All Requested")
	m_menu->addAction(tr("Cancel Requested Request(s)"),
			  this,
			  SLOT(slotRequest(void)));

      m_menu->addAction(tr("Print Current View..."),
			this,
			SLOT(slotPrintView(void)));

      if(getTypeFilterString() != "All Requested")
	m_menu->addAction(tr("Reserve Selected Item..."),
			  this,
			  SLOT(slotReserveCopy(void)));

      m_menu->addAction(tr("View Selected Item(s)..."),
			this,
			SLOT(slotViewDetails(void)));
    }
  else if(m_roles.contains("membership"))
    {
      m_menu->addAction(tr("Print Current View..."),
			this,
			SLOT(slotPrintView(void)));
      m_menu->addAction(tr("View Selected Item(s)..."),
			this,
			SLOT(slotViewDetails(void)));
    }
  else
    {
      if(!isGuest())
	{
	  if(getTypeFilterString() == "All Requested")
	    {
	      m_menu->addAction(tr("Print Current View..."),
				this,
				SLOT(slotPrintView(void)));
	      m_menu->addAction(tr("Cancel Requested Request(s)"),
				this,
				SLOT(slotRequest(void)));
	    }
	  else
	    {
	      m_menu->addAction(tr("Print Current View..."),
				this,
				SLOT(slotPrintView(void)));
	      m_menu->addAction(tr("Request Selected Item(s)"),
				this,
				SLOT(slotRequest(void)));
	    }
	}

      m_menu->addAction(tr("View Selected Item(s)..."),
			this,
			SLOT(slotViewDetails(void)));
    }
}

void biblioteq::preparePhotographsPerPageMenu(void)
{
  auto group = new QActionGroup(this);

  ui.menuPhotographs_per_Page->clear();

  QSettings settings;
  auto integer = settings.value("photographs_per_page", 25).toInt();

  if(!(integer == -1 || (integer >= 25 && integer <= 100)))
    integer = 25;

  for(int i = 1; i <= 5; i++)
    {
      QAction *action = nullptr;

      if(i == 5)
	action = group->addAction(tr("&Unlimited"));
      else
	action = group->addAction(QString(tr("&%1")).arg(25 * i));

      if(!action)
	continue;

      connect(action,
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotPhotographsPerPageChanged(void)));

      if(i == 5)
	action->setData(-1);
      else
	action->setData(25 * i);

      action->setCheckable(true);

      if(action->data().toInt() == integer)
	action->setChecked(true);

      ui.menuPhotographs_per_Page->addAction(action);
    }
}

void biblioteq::setSummaryImages(const QImage &back, const QImage &front)
{
  if(ui.itemSummary->width() < 0 || ui.table->currentRow() < 0)
    {
      ui.backImage->clear();
      ui.backImage->setVisible(false);
      ui.frontImage->clear();
      ui.frontImage->setVisible(false);
      return;
    }

  /*
  ** The size of no_image.png is 126x187.
  */

  auto b(back);
  auto f(front);

  if(b.isNull())
    b = QImage(":/no_image.png");

  if(!b.isNull())
    b = b.scaled(126, 187, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  if(!b.isNull())
    {
      ui.backImage->setPixmap(QPixmap::fromImage(b));
      ui.backImage->setVisible(true);
    }
  else
    ui.backImage->clear();

  if(f.isNull())
    f = QImage(":/no_image.png");

  if(!f.isNull())
    f = f.scaled(126, 187, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  if(!f.isNull())
    {
      ui.frontImage->setPixmap(QPixmap::fromImage(f));
      ui.frontImage->setVisible(true);
    }
  else
    ui.frontImage->clear();
}

void biblioteq::slotAllowAnyUserEmail(bool state)
{
  userinfo_diag->m_userinfo.email->setValidator(nullptr);

  if(!state)
    {
      QRegularExpression rx
	("^[-!#$%&'*+/0-9=?A-Z^_a-z{|}~](\\.?[-!#$%&'*+/0-9=?A-Z^_a-z{|}~])*"
	 "@[a-zA-Z](-?[a-zA-Z0-9])*(\\.[a-zA-Z](-?[a-zA-Z0-9])*)+$",
	 QRegularExpression::CaseInsensitiveOption);

      userinfo_diag->m_userinfo.email->setValidator
	(new QRegularExpressionValidator(rx, this));
    }
}

void biblioteq::slotBookSearch(void)
{
  biblioteq_book *book = nullptr;

  foreach(auto w, QApplication::topLevelWidgets())
    {
      auto b = qobject_cast<biblioteq_book *> (w);

      if(b && b->getID() == "search")
	{
	  book = b;
	  break;
	}
    }

  if(!book)
    {
      book = new biblioteq_book(this, "search", QModelIndex());
      book->search();
    }

#ifdef Q_OS_ANDROID
  book->showMaximized();
#else
  book->showNormal();
#endif
  book->activateWindow();
  book->raise();
}

void biblioteq::slotCDSearch(void)
{
  biblioteq_cd *cd = nullptr;

  foreach(auto w, QApplication::topLevelWidgets())
    {
      auto c = qobject_cast<biblioteq_cd *> (w);

      if(c && c->getID() == "search")
	{
	  cd = c;
	  break;
	}
    }

  if(!cd)
    {
      cd = new biblioteq_cd(this, "search", QModelIndex());
      cd->search();
    }

#ifdef Q_OS_ANDROID
  cd->showMaximized();
#else
  cd->showNormal();
#endif
  cd->activateWindow();
  cd->raise();
}

void biblioteq::slotContextMenu(const QPoint &point)
{
  prepareContextMenus();
  m_menu->exec(ui.table->mapToGlobal(point));
}

void biblioteq::slotDVDSearch(void)
{
  biblioteq_dvd *dvd = nullptr;

  foreach(auto w, QApplication::topLevelWidgets())
    {
      auto d = qobject_cast<biblioteq_dvd *> (w);

      if(d && d->getID() == "search")
	{
	  dvd = d;
	  break;
	}
    }

  if(!dvd)
    {
      dvd = new biblioteq_dvd(this, "search", QModelIndex());
      dvd->search();
    }

#ifdef Q_OS_ANDROID
  dvd->showMaximized();
#else
  dvd->showNormal();
#endif
  dvd->activateWindow();
  dvd->raise();
}

void biblioteq::slotJournSearch(void)
{
  biblioteq_journal *journal = nullptr;

  foreach(auto w, QApplication::topLevelWidgets())
    {
      auto j = qobject_cast<biblioteq_journal *> (w);

      if(j && j->getID() == "search")
	{
	  journal = j;
	  break;
	}
    }

  if(!journal)
    {
      journal = new biblioteq_journal(this, "search", QModelIndex());
      journal->search();
    }

#ifdef Q_OS_ANDROID
  journal->showMaximized();
#else
  journal->showNormal();
#endif
  journal->activateWindow();
  journal->raise();
}

void biblioteq::slotMagSearch(void)
{
  biblioteq_magazine *magazine = nullptr;

  foreach(auto w, QApplication::topLevelWidgets())
    {
      auto m = qobject_cast<biblioteq_magazine *> (w);

      /*
      ** The class biblioteq_journal inherits biblioteq_magazine.
      */

      if(!qobject_cast<biblioteq_journal *> (w))
	if(m && m->getID() == "search")
	  {
	    magazine = m;
	    break;
	  }
    }

  if(!magazine)
    {
      magazine = new biblioteq_magazine
	(this, "search", QModelIndex(), "magazine");
      magazine->search();
    }

#ifdef Q_OS_ANDROID
  magazine->showMaximized();
#else
  magazine->showNormal();
#endif
  magazine->activateWindow();
  magazine->raise();
}

void biblioteq::slotPhotographSearch(void)
{
  biblioteq_photographcollection *photograph = nullptr;

  foreach(auto w, QApplication::topLevelWidgets())
    {
      auto p = qobject_cast<biblioteq_photographcollection *> (w);

      if(p && p->getID() == "search")
	{
	  photograph = p;
	  break;
	}
    }

  if(!photograph)
    {
      photograph = new biblioteq_photographcollection
	(this, "search", QModelIndex());
      photograph->search();
    }

#ifdef Q_OS_ANDROID
  photograph->showMaximized();
#else
  photograph->showNormal();
#endif
  photograph->activateWindow();
  photograph->raise();
}

void biblioteq::slotPhotographsPerPageChanged(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  QSettings settings;

  settings.setValue("photographs_per_page", action->data().toInt());
}

void biblioteq::slotReloadBiblioteqConf(void)
{
  m_otheroptions->prepareSettings();
  readGlobalSetup();
}

void biblioteq::slotResetAllSearch(void)
{
  ui.case_insensitive->setChecked(false);
  ui.case_insensitive->setEnabled(true);
  ui.search->clear();
  ui.search->setEnabled(true);
  ui.searchType->setCurrentIndex(0);
  ui.searchType->setEnabled(true);
}

void biblioteq::slotRoleChanged(int index)
{
  if(index == 1)
    {
      br.password->setEnabled(false);
      br.password->setText(BIBLIOTEQ_GUEST_ACCOUNT);
      br.userid->setEnabled(false);
      br.userid->setText(BIBLIOTEQ_GUEST_ACCOUNT);
    }
  else
    {
      br.password->setText(QString(1024, '0'));
      br.password->clear();
      br.password->setEnabled(true);
      br.userid->clear();
      br.userid->setEnabled(true);
    }

  br.userid->setFocus();
}

void biblioteq::slotSaveDnt(bool state)
{
  QSqlQuery query(m_db);

  query.prepare("INSERT INTO member_history_dnt "
		"(dnt, memberid) "
		"VALUES (?, ?)");
  query.bindValue(0, QVariant(state).toInt());
  query.bindValue(1, dbUserName());
  query.exec();

  if(!(query.lastError().text().toLower().contains("duplicate") ||
       query.lastError().text().toLower().contains("unique")))
    addError(QString(tr("Database Error")),
	     QString(tr("Unable to insert into member_history_dnt for "
			"member %1.").arg(dbUserName())),
	     query.lastError().text(), __FILE__, __LINE__);

  query.prepare("UPDATE member_history_dnt "
		"SET dnt = ? "
		"WHERE memberid = ?");
  query.bindValue(0, QVariant(state).toInt());
  query.bindValue(1, dbUserName());
  query.exec();

  if(query.lastError().isValid())
    addError(QString(tr("Database Error")),
	     QString(tr("Unable to update member_history_dnt for "
			"member %1.").arg(dbUserName())),
	     query.lastError().text(), __FILE__, __LINE__);

}

void biblioteq::slotSearchBasic(void)
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
  auto text(ui.search->text().trimmed());

  types.append("Book");
  types.append("CD");
  types.append("DVD");
  types.append("Grey Literature");
  types.append("Journal");
  types.append("Magazine");
  types.append("Photograph Collection");
  types.append("Video Game");

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

  for(int i = 0; i < types.size(); i++)
    {
      type = types.at(i);

      if(type == "Grey Literature")
	str = "SELECT DISTINCT grey_literature.document_title, "
	  "grey_literature.document_id, "
	  "'', "
	  "'', "
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

      QString E("");

      if(m_db.driverName() != "QSQLITE")
	E = "E";

      switch(ui.searchType->currentIndex())
	{
	case ACCESSION_NUMBER_GENERIC_SEARCH_TYPE:
	  {
	    if(type != "Grey Literature")
	      {
		if(ui.case_insensitive->isChecked())
		  {
		    str.append("COALESCE(LOWER(accession_number), '') LIKE " +
			       E + "'%' || ? || '%' ");
		    values.append
		      (biblioteq_myqstring::escape(text.toLower().trimmed()));
		  }
		else
		  {
		    str.append("COALESCE(accession_number, '') LIKE " +
			       E + "'%' || ? || '%' ");
		    values.append(biblioteq_myqstring::escape(text.trimmed()));
		  }
	      }
	    else if(ui.case_insensitive->isChecked())
	      {
		str.append
		  ("LOWER(document_id) LIKE " + E + "'%' || ? || '%' ");
		values.append
		  (biblioteq_myqstring::escape(text.toLower().trimmed()));
	      }
	    else
	      {
		str.append("document_id LIKE " + E + "'%' || ? || '%' ");
		values.append(biblioteq_myqstring::escape(text.trimmed()));
	      }

	    break;
	  }
	case CATEGORY_GENERIC_SEARCH_TYPE:
	  {
	    if(type != "Grey Literature" && type != "Photograph Collection")
	      {
		if(ui.case_insensitive->isChecked())
		  {
		    str.append
		      ("LOWER(category) LIKE " + E + "'%' || ? || '%' ");
		    values.append
		      (biblioteq_myqstring::escape(text.toLower().trimmed()));
		  }
		else
		  {
		    str.append("category LIKE " + E + "'%' || ? || '%' ");
		    values.append(biblioteq_myqstring::escape(text.trimmed()));
		  }
	      }
	    else if(type == "Grey Literature")
	      {
		if(ui.case_insensitive->isChecked())
		  {
		    str.append("COALESCE(LOWER(notes), '') LIKE " +
			       E + "'%' || ? || '%' ");
		    values.append
		      (biblioteq_myqstring::escape(text.toLower().trimmed()));
		  }
		else
		  {
		    str.append
		      ("COALESCE(notes, '') LIKE " + E + "'%' || ? || '%' ");
		    values.append
		      (biblioteq_myqstring::escape(text.trimmed()));
		  }
	      }
	    else if(type == "Photograph Collection")
	      {
		if(ui.case_insensitive->isChecked())
		  {
		    str.append("COALESCE(LOWER(about), '') LIKE " +
			       E + "'%' || ? || '%' ");
		    values.append
		      (biblioteq_myqstring::escape(text.toLower().trimmed()));
		  }
		else
		  {
		    str.append
		      ("COALESCE(about, '') LIKE " + E + "'%' || ? || '%' ");
		    values.append(biblioteq_myqstring::escape(text.trimmed()));
		  }
	      }

	    break;
	  }
	case ID_GENERIC_SEARCH_TYPE:
	  {
	    if(type == "Grey Literature")
	      {
		if(ui.case_insensitive->isChecked())
		  {
		    str.append
		      ("(LOWER(document_id) LIKE " + E + "'%' || ? || '%' ");
		    values.append
		      (biblioteq_myqstring::escape(text.toLower().trimmed()));
		  }
		else
		  {
		    str.append("(document_id LIKE " + E + "'%' || ? || '%' ");
		    values.append(biblioteq_myqstring::escape(text.trimmed()));
		  }
	      }
	    else if(ui.case_insensitive->isChecked())
	      {
		str.append("(LOWER(id) LIKE " + E + "'%' || ? || '%' ");
		values.append
		  (biblioteq_myqstring::escape(text.toLower().trimmed()));
	      }
	    else
	      {
		str.append("(id LIKE " + E + "'%' || ? || '%' ");
		values.append(biblioteq_myqstring::escape(text.trimmed()));
	      }

	    if(type == "Book")
	      {
		if(ui.case_insensitive->isChecked())
		  {
		    str.append
		      ("OR LOWER(isbn13) LIKE " + E + "'%' || ? || '%' ");
		    values.append
		      (biblioteq_myqstring::escape(text.toLower().trimmed()));
		  }
		else
		  {
		    str.append("OR isbn13 LIKE " + E + "'%' || ? || '%' ");
		    values.append(biblioteq_myqstring::escape(text.trimmed()));
		  }
	      }

	    str.append(") ");
	    break;
	  }
	case KEYWORD_GENERIC_SEARCH_TYPE:
	  {
	    if(type != "Grey Literature" && type != "Photograph Collection")
	      {
		if(ui.case_insensitive->isChecked())
		  {
		    str.append("COALESCE(LOWER(keyword), '') LIKE " +
			       E + "'%' || ? || '%' ");
		    values.append
		      (biblioteq_myqstring::escape(text.toLower().trimmed()));
		  }
		else
		  {
		    str.append("COALESCE(keyword, '') LIKE " +
			       E + "'%' || ? || '%' ");
		    values.append(biblioteq_myqstring::escape(text.trimmed()));
		  }
	      }
	    else if(type == "Grey Literature")
	      {
		if(ui.case_insensitive->isChecked())
		  {
		    str.append("COALESCE(LOWER(notes), '') LIKE " +
			       E + "'%' || ? || '%' ");
		    values.append
		      (biblioteq_myqstring::escape(text.toLower().trimmed()));
		  }
		else
		  {
		    str.append
		      ("COALESCE(notes, '') LIKE " + E + "'%' || ? || '%' ");
		    values.append(biblioteq_myqstring::escape(text.trimmed()));
		  }
	      }
	    else if(type == "Photograph Collection")
	      {
		if(ui.case_insensitive->isChecked())
		  {
		    str.append("COALESCE(LOWER(about), '') LIKE " +
			       E + "'%' || ? || '%' ");
		    values.append
		      (biblioteq_myqstring::escape(text.toLower().trimmed()));
		  }
		else
		  {
		    str.append
		      ("COALESCE(about, '') LIKE " + E + "'%' || ? || '%' ");
		    values.append(biblioteq_myqstring::escape(text.trimmed()));
		  }
	      }

	    break;
	  }
	default:
	  {
	    if(type == "Grey Literature")
	      {
		if(ui.case_insensitive->isChecked())
		  {
		    str.append("LOWER(document_title) LIKE " +
			       E + "'%' || ? || '%' ");
		    values.append
		      (biblioteq_myqstring::escape(text.toLower().trimmed()));
		  }
		else
		  {
		    str.append("document_title LIKE " + E + "'%' || ? || '%' ");
		    values.append(biblioteq_myqstring::escape(text.trimmed()));
		  }
	      }
	    else if(ui.case_insensitive->isChecked())
	      {
		str.append("LOWER(title) LIKE " + E + "'%' || ? || '%' ");
		values.append
		  (biblioteq_myqstring::escape(text.toLower().trimmed()));
	      }
	    else
	      {
		str.append("title LIKE " + E + "'%' || ? || '%' ");
		values.append(biblioteq_myqstring::escape(text.trimmed()));
	      }
	  }
	}

      if(type != "Grey Literature" &&
	 type != "Photograph Collection")
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
		       ).arg(type.toLower().remove(" "));
      else if(type == "Grey Literature")
	str += "GROUP BY "
	  "grey_literature.document_title, "
	  "grey_literature.document_id, "
	  "grey_literature.location, "
	  "grey_literature.job_number, "
	  "grey_literature.type, "
	  "grey_literature.myoid, "
	  "grey_literature.front_cover ";
      else
	str += "GROUP BY "
	  "photograph_collection.title, "
	  "photograph_collection.id, "
	  "photograph_collection.location, "
	  "photograph_collection.accession_number, "
	  "photograph_collection.type, "
	  "photograph_collection.myoid, "
	  "photograph_collection.image_scaled ";

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

      if(type != "Video Game")
	str += "UNION ALL ";
      else
	str += " ";

      searchstr += str;
    }

  query.prepare(searchstr);

  for(int i = 0; i < values.size(); i++)
    query.addBindValue(values.at(i));

  QApplication::restoreOverrideCursor();
  (void) populateTable(query, "All", NEW_PAGE, POPULATE_SEARCH_BASIC);
}

void biblioteq::slotUpgradeSqliteScheme(void)
{
  if(m_db.driverName() != "QSQLITE")
    return;

  if(QMessageBox::question(this, tr("BiblioteQ: Question"),
			   tr("You are about to upgrade the legacy "
			      "SQLite database %1. "
			      "Please verify that you have made a "
			      "copy of this database. "
			      "Are you sure that you wish to continue?").
			   arg(m_db.databaseName()),
			   QMessageBox::Yes | QMessageBox::No,
			   QMessageBox::No) == QMessageBox::No)
    {
      QApplication::processEvents();
      return;
    }

  repaint();
  QApplication::processEvents();
  QApplication::setOverrideCursor(Qt::WaitCursor);

  QStringList list;

  list.append("CREATE TABLE IF NOT EXISTS book_files"
	      "("
	      "description	TEXT,"
	      "file		BYTEA NOT NULL,"
	      "file_digest	TEXT NOT NULL,"
	      "file_name        TEXT NOT NULL,"
	      "item_oid	BIGINT NOT NULL,"
	      "myoid		BIGSERIAL NOT NULL,"
	      "FOREIGN KEY(item_oid) REFERENCES book(myoid) ON DELETE CASCADE,"
	      "PRIMARY KEY(file_digest, item_oid)"
	      ")");
  list.append("CREATE TABLE IF NOT EXISTS journal_files"
	      "("
	      "description	TEXT,"
	      "file		BYTEA NOT NULL,"
	      "file_digest	TEXT NOT NULL,"
	      "file_name        TEXT NOT NULL,"
	      "item_oid	BIGINT NOT NULL,"
	      "myoid		BIGSERIAL NOT NULL,"
	      "FOREIGN KEY(item_oid) REFERENCES journal(myoid) ON DELETE "
	      "CASCADE,"
	      "PRIMARY KEY(file_digest, item_oid)"
	      ")");
  list.append("CREATE TABLE IF NOT EXISTS locations "
	      "("
	      "location TEXT NOT NULL,"
	      "type VARCHAR(16),"
	      "PRIMARY KEY(location, type))");
  list.append("CREATE TABLE IF NOT EXISTS magazine_files"
	      "("
	      "description	TEXT,"
	      "file		BYTEA NOT NULL,"
	      "file_digest	TEXT NOT NULL,"
	      "file_name        TEXT NOT NULL,"
	      "item_oid	BIGINT NOT NULL,"
	      "myoid		BIGSERIAL NOT NULL,"
	      "FOREIGN KEY(item_oid) REFERENCES magazine(myoid) ON DELETE "
	      "CASCADE,"
	      "PRIMARY KEY(file_digest, item_oid)"
	      ")");
  list.append("CREATE TABLE IF NOT EXISTS monetary_units "
	      "("
	      "monetary_unit TEXT NOT NULL PRIMARY KEY)");
  list.append("CREATE TABLE IF NOT EXISTS languages "
	      "("
	      "language TEXT NOT NULL PRIMARY KEY)");
  list.append("CREATE TABLE IF NOT EXISTS cd_formats "
	      "("
	      "cd_format	TEXT NOT NULL PRIMARY KEY)");
  list.append("CREATE TABLE IF NOT EXISTS dvd_ratings "
	      "("
	      "dvd_rating TEXT NOT NULL PRIMARY KEY)");
  list.append("CREATE TABLE IF NOT EXISTS dvd_aspect_ratios "
	      "("
	      "dvd_aspect_ratio TEXT NOT NULL PRIMARY KEY)");
  list.append("CREATE TABLE IF NOT EXISTS dvd_regions "
	      "("
	      "dvd_region TEXT NOT NULL PRIMARY KEY)");
  list.append("CREATE TABLE IF NOT EXISTS minimum_days "
	      "("
	      "days INTEGER NOT NULL,"
	      "type VARCHAR(16) NOT NULL PRIMARY KEY)");
  list.append("CREATE TABLE IF NOT EXISTS videogame_ratings"
	      "("
	      "videogame_rating TEXT NOT NULL PRIMARY KEY)");
  list.append("CREATE TABLE IF NOT EXISTS videogame_platforms "
	      "("
	      "videogame_platform TEXT NOT NULL PRIMARY KEY)");
  list.append("ALTER TABLE book ADD marc_tags TEXT");
  list.append("ALTER TABLE journal ADD marc_tags TEXT");
  list.append("ALTER TABLE magazine ADD marc_tags TEXT");
  list.append("ALTER TABLE member ADD expiration_date VARCHAR(32) "
	      "NOT NULL DEFAULT '01/0/3000'");
  list.append("ALTER TABLE book ADD keyword TEXT");
  list.append("ALTER TABLE cd ADD keyword TEXT");
  list.append("ALTER TABLE dvd ADD keyword TEXT");
  list.append("ALTER TABLE journal ADD keyword TEXT");
  list.append("ALTER TABLE magazine ADD keyword TEXT");
  list.append("ALTER TABLE videogame ADD keyword TEXT");
  list.append("ALTER TABLE member ADD overdue_fees NUMERIC(10, 2) "
	      "NOT NULL DEFAULT 0.00");
  list.append("ALTER TABLE member ADD comments TEXT");
  list.append("ALTER TABLE member ADD general_registration_number TEXT");
  list.append("ALTER TABLE member ADD memberclass TEXT");
  list.append("CREATE TABLE IF NOT EXISTS photograph_collection "
	      "("
	      "id  TEXT PRIMARY KEY NOT NULL,"
	      "myoid BIGINT UNIQUE,"
	      "title TEXT NOT NULL,"
	      "location TEXT NOT NULL,"
	      "about TEXT,"
	      "notes TEXT,"
	      "image BYTEA,"
	      "image_scaled BYTEA,"
	      "type VARCHAR(32) NOT NULL DEFAULT 'Photograph Collection')");
  list.append("CREATE TABLE IF NOT EXISTS photograph "
	      "("
	      "id TEXT NOT NULL,"
	      "myoid BIGINT UNIQUE,"
	      "collection_oid BIGINT NOT NULL,"
	      "title TEXT NOT NULL,"
	      "creators TEXT NOT NULL,"
	      "pdate VARCHAR(32) NOT NULL,"
	      "quantity INTEGER NOT NULL DEFAULT 1,"
	      "medium TEXT NOT NULL,"
	      "reproduction_number TEXT NOT NULL,"
	      "copyright TEXT NOT NULL,"
	      "callnumber VARCHAR(64),"
	      "other_number TEXT,"
	      "notes TEXT,"
	      "subjects TEXT,"
	      "format TEXT,"
	      "image BYTEA,"
	      "image_scaled BYTEA,"
	      "PRIMARY KEY(id, collection_oid),"
	      "FOREIGN KEY(collection_oid) REFERENCES "
	      "photograph_collection(myoid) ON DELETE CASCADE)");
  list.append("ALTER TABLE cd_songs ADD artist TEXT "
	      "NOT NULL DEFAULT 'UNKNOWN'");
  list.append("ALTER TABLE cd_songs ADD composer TEXT "
	      "NOT NULL DEFAULT 'UNKNOWN'");
  list.append("ALTER TABLE book ADD condition TEXT");
  list.append("ALTER TABLE book ADD originality TEXT");
  list.append("ALTER TABLE book_copy_info ADD condition TEXT");
  list.append("ALTER TABLE book_copy_info ADD originality TEXT");
  list.append("CREATE TABLE IF NOT EXISTS book_binding_types "
	      "("
	      "binding_type TEXT NOT NULL PRIMARY KEY)");
  list.append("CREATE TABLE member_temporary "
	      "("
	      "memberid VARCHAR(16) NOT NULL PRIMARY KEY DEFAULT 1,"
	      "membersince VARCHAR(32) NOT NULL,"
	      "dob VARCHAR(32) NOT NULL,"
	      "sex VARCHAR(32) NOT NULL DEFAULT 'Female',"
	      "first_name VARCHAR(128) NOT NULL,"
	      "middle_init VARCHAR(1),"
	      "last_name VARCHAR(128) NOT NULL,"
	      "telephone_num VARCHAR(32),"
	      "street VARCHAR(256) NOT NULL,"
	      "city VARCHAR(256) NOT NULL,"
	      "state_abbr VARCHAR(16) NOT NULL DEFAULT 'N/A',"
	      "zip VARCHAR(16) NOT NULL DEFAULT 'N/A',"
	      "email VARCHAR(128),"
	      "expiration_date VARCHAR(32) NOT NULL,"
	      "overdue_fees NUMERIC(10, 2) NOT NULL DEFAULT 0.00,"
	      "comments TEXT,"
	      "general_registration_number TEXT,"
	      "memberclass TEXT)");
  list.append("INSERT INTO member_temporary SELECT "
	      "memberid, "
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
	      "memberclass FROM member");
  list.append("DROP TABLE IF EXISTS member");
  list.append("ALTER TABLE member_temporary RENAME TO member");
  list.append("CREATE TABLE journal_temporary "
	      "("
	      "accession_number TEXT, "
	      "id VARCHAR(32),"
	      "myoid BIGINT NOT NULL,"
	      "title TEXT NOT NULL,"
	      "pdate VARCHAR(32) NOT NULL,"
	      "publisher TEXT NOT NULL,"
	      "place TEXT NOT NULL,"
	      "category TEXT NOT NULL,"
	      "price NUMERIC(10, 2) NOT NULL DEFAULT 0.00,"
	      "description TEXT NOT NULL,"
	      "language VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',"
	      "monetary_units VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',"
	      "quantity INTEGER NOT NULL DEFAULT 1,"
	      "location TEXT NOT NULL,"
	      "issuevolume INTEGER NOT NULL DEFAULT 0,"
	      "issueno INTEGER NOT NULL DEFAULT 0,"
	      "lccontrolnumber VARCHAR(64),"
	      "callnumber VARCHAR(64),"
	      "deweynumber VARCHAR(64),"
	      "front_cover BYTEA,"
	      "back_cover BYTEA,"
	      "marc_tags TEXT,"
	      "keyword TEXT,"
	      "type VARCHAR(16) NOT NULL DEFAULT 'Journal',"
	      "UNIQUE (id, issueno, issuevolume))");
  list.append("INSERT INTO journal_temporary SELECT "
	      "accession_number, "
	      "id, "
	      "myoid, "
	      "title, "
	      "pdate, "
	      "publisher, "
	      "place, "
	      "category, "
	      "price, "
	      "description, "
	      "language, "
	      "monetary_units, "
	      "quantity, "
	      "location, "
	      "issuevolume, "
	      "issueno, "
	      "lccontrolnumber, "
	      "callnumber, "
	      "deweynumber, "
	      "front_cover, "
	      "back_cover, "
	      "marc_tags, "
	      "keyword, "
	      "type FROM journal");
  list.append("DROP TABLE IF EXISTS journal");
  list.append("ALTER TABLE journal_temporary RENAME TO journal");
  list.append("CREATE TABLE magazine_temporary"
	      "("
	      "accession_number TEXT, "
	      "id VARCHAR(32),"
	      "myoid BIGINT NOT NULL,"
	      "title TEXT NOT NULL,"
	      "pdate VARCHAR(32) NOT NULL,"
	      "publisher TEXT NOT NULL,"
	      "place TEXT NOT NULL,"
	      "category TEXT NOT NULL,"
	      "price NUMERIC(10, 2) NOT NULL DEFAULT 0.00,"
	      "description TEXT NOT NULL,"
	      "language VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',"
	      "monetary_units VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',"
	      "quantity INTEGER NOT NULL DEFAULT 1,"
	      "location TEXT NOT NULL,"
	      "issuevolume INTEGER NOT NULL DEFAULT 0,"
	      "issueno INTEGER NOT NULL DEFAULT 0,"
	      "lccontrolnumber VARCHAR(64),"
	      "callnumber VARCHAR(64),"
	      "deweynumber VARCHAR(64),"
	      "front_cover BYTEA,"
	      "back_cover BYTEA,"
	      "marc_tags TEXT,"
	      "keyword TEXT,"
	      "type VARCHAR(16) NOT NULL DEFAULT 'Magazine',"
	      "UNIQUE (id, issueno, issuevolume))");
  list.append("INSERT INTO magazine_temporary SELECT "
	      "accession_number, "
	      "id, "
	      "myoid, "
	      "title, "
	      "pdate, "
	      "publisher, "
	      "place, "
	      "category, "
	      "price, "
	      "description, "
	      "language, "
	      "monetary_units, "
	      "quantity, "
	      "location, "
	      "issuevolume, "
	      "issueno, "
	      "lccontrolnumber, "
	      "callnumber, "
	      "deweynumber, "
	      "front_cover, "
	      "back_cover, "
	      "marc_tags, "
	      "keyword, "
	      "type FROM magazine");
  list.append("DROP TABLE IF EXISTS magazine");
  list.append("ALTER TABLE magazine_temporary RENAME TO magazine");
  list.append("CREATE TABLE IF NOT EXISTS grey_literature "
	      "("
	      "author TEXT NOT NULL,"
	      "client TEXT,"
	      "document_code_a TEXT NOT NULL,"
	      "document_code_b TEXT NOT NULL,"
	      "document_date TEXT NOT NULL,"
	      "document_id TEXT NOT NULL PRIMARY KEY,"
	      "document_status TEXT,"
	      "document_title TEXT NOT NULL,"
	      "document_type TEXT NOT NULL,"
	      "front_cover BYTEA,"
	      "job_number TEXT NOT NULL,"
	      "location TEXT,"
	      "myoid BIGINT UNIQUE,"
	      "notes TEXT,"
	      "type VARCHAR(16) NOT NULL DEFAULT 'Grey Literature')");
  list.append("ALTER TABLE book ADD accession_number TEXT");
  list.append("ALTER TABLE cd ADD accession_number TEXT");
  list.append("ALTER TABLE dvd ADD accession_number TEXT");
  list.append("ALTER TABLE journal ADD accession_number TEXT");
  list.append("ALTER TABLE magazine ADD accession_number TEXT");
  list.append("ALTER TABLE photograph ADD accession_number TEXT");
  list.append("ALTER TABLE photograph_collection ADD accession_number TEXT");
  list.append("ALTER TABLE videogame ADD accession_number TEXT");
  list.append("CREATE TABLE IF NOT EXISTS grey_literature_files "
	      "("
	      "description TEXT,"
	      "file BYTEA NOT NULL,"
	      "file_digest TEXT NOT NULL,"
	      "file_name TEXT NOT NULL,"
	      "item_oid BIGINT NOT NULL,"
	      "myoid BIGINT NOT NULL,"
	      "FOREIGN KEY(item_oid) REFERENCES grey_literature(myoid) ON "
	      "DELETE CASCADE,"
	      "PRIMARY KEY(file_digest, item_oid)"
	      ")");
  list.append("CREATE TABLE IF NOT EXISTS grey_literature_types	"
	      "("
	      "document_type     TEXT NOT NULL PRIMARY KEY"
	      ")");
  list.append("DROP VIEW IF EXISTS item_borrower_vw");
  list.append("CREATE TABLE IF NOT EXISTS book_sequence "
	      "("
	      "value            INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT"
	      ")");
  list.append("ALTER TABLE book_copy_info ADD status TEXT");
  list.append("ALTER TABLE cd_copy_info ADD status TEXT");
  list.append("ALTER TABLE dvd_copy_info ADD status TEXT");
  list.append("ALTER TABLE journal_copy_info ADD status TEXT");
  list.append("ALTER TABLE magazine_copy_info ADD status TEXT");
  list.append("ALTER TABLE videogame_copy_info ADD status TEXT");
  list.append("ALTER TABLE book ADD url");
  list.append
    ("ALTER TABLE grey_literature ADD quantity INTEGER NOT NULL DEFAULT 1");
  list.append
    ("CREATE TRIGGER grey_literature_purge_trigger AFTER DELETE ON "
     "grey_literature "
     "FOR EACH row "
     "BEGIN "
     "DELETE FROM item_borrower WHERE item_oid = old.myoid; "
     "DELETE FROM member_history WHERE item_oid = old.myoid AND "
     "type = old.type; "
     "END;");
  list.append("ALTER TABLE book ADD book_read INTEGER DEFAULT 0");
  list.append("ALTER TABLE member ADD maximum_reserved_books "
	      "INTEGER NOT NULL DEFAULT 0");

  QString errors("<html>");
  int ct = 1;

  errors.append(tr("Executing %1 statements.<br><br>").arg(list.size()));

  for(int i = 0; i < list.size(); i++)
    {
      QSqlQuery query(m_db);

      errors.append(QString::number(i + 1));
      errors.append(". ");
      errors.append(list.at(i));
      errors.append("<br><br>");

      if(!query.exec(list.at(i)))
	{
	  errors.append
	    (tr("<font color='red'>Error %1: %2. Statement: %3.</font>").
	     arg(ct).arg(query.lastError().text().toLower()).
	     arg(list.at(i)));
	  ct += 1;
	}
      else
	errors.append
	  ("<font color='green'>Statement concluded correctly!</font>");

      if(i < list.size() - 1)
	errors.append("<br><br>");
      else
	errors.append("<br>");
    }

  errors.append("</html>");

  if(!errors.isEmpty())
    {
      QDialog dialog(this);
      Ui_generalmessagediag ui;

      ui.setupUi(&dialog);
      ui.text->setText(errors);
      connect(ui.cancelButton,
	      SIGNAL(clicked(void)),
	      &dialog,
	      SLOT(close(void)));
      dialog.setWindowTitle(tr("BiblioteQ: Upgrade SQLite Schema Results"));
      QApplication::restoreOverrideCursor();
      dialog.exec();
    }
  else
    {
      QApplication::restoreOverrideCursor();
      QMessageBox::information
	(this,
	 tr("BiblioteQ: Information"),
	 tr("The database %1 was upgraded successfully.").
	 arg(m_db.databaseName()));
    }

  QApplication::processEvents();
}

void biblioteq::slotVideoGameSearch(void)
{
  biblioteq_videogame *videogame = nullptr;

  foreach(auto w, QApplication::topLevelWidgets())
    {
      auto v = qobject_cast<biblioteq_videogame *> (w);

      if(v && v->getID() == "search")
	{
	  videogame = v;
	  break;
	}
    }

  if(!videogame)
    {
      videogame = new biblioteq_videogame(this, "search", QModelIndex());
      videogame->search();
    }

#ifdef Q_OS_ANDROID
  videogame->showMaximized();
#else
  videogame->showNormal();
#endif
  videogame->activateWindow();
  videogame->raise();
}

void biblioteq::slotViewFullOrNormalScreen(void)
{
  if(windowState() == Qt::WindowFullScreen)
    {
      showNormal();
      ui.action_Full_Screen->setText(tr("&Full Screen"));
    }
  else if(windowState() == Qt::WindowNoState)
    {
      showFullScreen();
      ui.action_Full_Screen->setText(tr("&Normal Screen"));
    }
}
