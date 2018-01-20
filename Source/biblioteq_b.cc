/*
** -- Qt Includes --
*/

#include <QSettings>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QtDebug>

/*
** -- C++ Includes --
*/

#include <limits>

/*
** -- Local Includes --
*/

#include "biblioteq.h"
#include "biblioteq_graphicsitempixmap.h"
#include "biblioteq_otheroptions.h"
#include "ui_biblioteq_generalmessagediag.h"

int biblioteq::populateTable(const int search_type_arg,
			     const QString &typefilter,
			     const QString &searchstrArg,
			     const int pagingType)
{
  ui.itemsCountLabel->setText(tr("0 Results"));

  QDate now(QDate::currentDate());
  QProgressDialog progress(this);
  QString itemType = "";
  QString searchstr = "";
  QString str = "";
  QString type = "";
  QStringList tmplist; // Used for custom queries.
  QStringList types;
  QTableWidgetItem *item = 0;
  int i = -1;
  int search_type = search_type_arg;

#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  progress.setAttribute(Qt::WA_MacMetalStyle, BIBLIOTEQ_WA_MACMETALSTYLE);
#endif
#endif

  QString limitStr("");
  QString offsetStr("");
  int limit = 0;
  qint64 offset = m_queryOffset;

  if(search_type != POPULATE_SEARCH_BASIC)
    resetAllSearchWidgets();

  for(int ii = 0; ii < ui.menuEntriesPerPage->actions().size(); ii++)
    if(ui.menuEntriesPerPage->actions()[ii]->isChecked())
      {
	limit = ui.menuEntriesPerPage->actions()[ii]->data().toInt();
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
	(0, 0,
	 5 * 150,
	 limit / 5 * 200 + 15);
    }

  /*
  ** The order of the fields in the select statements should match
  ** the original column order.
  */

  switch(search_type)
    {
    case CUSTOM_QUERY:
      {
	searchstr = searchstrArg;

	if(searchstr.lastIndexOf("LIMIT") != -1)
	  searchstr.remove(searchstr.lastIndexOf("LIMIT"),
			   searchstr.length());

	searchstr += limitStr + offsetStr;
	break;
      }
    case POPULATE_ALL:
      {
	if(typefilter == "All" ||
	   typefilter == "All Available")
	  {
	    QString checkAvailability("");

	    if(typefilter == "All Available")
	      checkAvailability =
		" HAVING (quantity - COUNT(item_borrower_vw.item_oid)) > 0 ";

	    searchstr = QString
	      ("SELECT DISTINCT book.title, "
	       "book.id, "
	       "book.publisher, book.pdate, "
	       "book.category, "
	       "book.language, "
	       "book.price, book.monetary_units, "
	       "book.quantity, "
	       "book.location, "
	       "book.quantity - COUNT(item_borrower_vw.item_oid) "
	       "AS availability, "
	       "COUNT(item_borrower_vw.item_oid) AS total_reserved, "
	       "book.accession_number, "
	       "book.type, "
	       "book.myoid, "
	       "book.front_cover "
	       "FROM "
	       "book LEFT JOIN item_borrower_vw ON "
	       "book.myoid = item_borrower_vw.item_oid "
	       "AND item_borrower_vw.type = 'Book' "
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
	       "cd.quantity - COUNT(item_borrower_vw.item_oid) "
	       "AS availability, "
	       "COUNT(item_borrower_vw.item_oid) AS total_reserved, "
	       "cd.accession_number, "
	       "cd.type, "
	       "cd.myoid, "
	       "cd.front_cover "
	       "FROM "
	       "cd LEFT JOIN item_borrower_vw ON "
	       "cd.myoid = item_borrower_vw.item_oid "
	       "AND item_borrower_vw.type = 'CD' "
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
	       "dvd.quantity - COUNT(item_borrower_vw.item_oid) AS "
	       "availability, "
	       "COUNT(item_borrower_vw.item_oid) AS total_reserved, "
	       "dvd.accession_number, "
	       "dvd.type, "
	       "dvd.myoid, "
	       "dvd.front_cover "
	       "FROM "
	       "dvd LEFT JOIN item_borrower_vw ON "
	       "dvd.myoid = item_borrower_vw.item_oid "
	       "AND item_borrower_vw.type = 'DVD' "
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
	       "SELECT DISTINCT journal.title, "
	       "journal.id, "
	       "journal.publisher, journal.pdate, "
	       "journal.category, "
	       "journal.language, "
	       "journal.price, journal.monetary_units, "
	       "journal.quantity, "
	       "journal.location, "
	       "journal.quantity - COUNT(item_borrower_vw.item_oid) AS "
	       "availability, "
	       "COUNT(item_borrower_vw.item_oid) AS total_reserved, "
	       "journal.accession_number, "
	       "journal.type, "
	       "journal.myoid, "
	       "journal.front_cover "
	       "FROM "
	       "journal LEFT JOIN item_borrower_vw ON "
	       "journal.myoid = item_borrower_vw.item_oid "
	       "AND item_borrower_vw.type = journal.type "
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
	       "magazine.quantity - COUNT(item_borrower_vw.item_oid) AS "
	       "availability, "
	       "COUNT(item_borrower_vw.item_oid) AS total_reserved, "
	       "magazine.accession_number, "
	       "magazine.type, "
	       "magazine.myoid, "
	       "magazine.front_cover "
	       "FROM "
	       "magazine LEFT JOIN item_borrower_vw ON "
	       "magazine.myoid = item_borrower_vw.item_oid "
	       "AND item_borrower_vw.type = magazine.type "
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
	       "GROUP BY "
	       "photograph_collection.title, "
	       "photograph_collection.id, "
	       "photograph_collection.location, "
	       "photograph_collection.accession_number, "
	       "photograph_collection.type, "
	       "photograph_collection.myoid, "
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
	       "videogame.quantity - COUNT(item_borrower_vw.item_oid) "
	       "AS "
	       "availability, "
	       "COUNT(item_borrower_vw.item_oid) AS total_reserved, "
	       "videogame.accession_number, "
	       "videogame.type, "
	       "videogame.myoid, "
	       "videogame.front_cover "
	       "FROM "
	       "videogame LEFT JOIN item_borrower_vw ON "
	       "videogame.myoid = item_borrower_vw.item_oid "
	       "AND item_borrower_vw.type = 'Video Game' "
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
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "book.title, "
				 "book.id, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.quantity - "
				 "COUNT(item_borrower_vw.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower_vw.item_oid) AS "
				 "total_reserved, "
				 "book.accession_number, "
				 "book.type, "
				 "book.myoid, "
				 "book.front_cover "
				 "FROM "
				 "book LEFT JOIN item_borrower_vw ON "
				 "book.myoid = item_borrower_vw.item_oid "
				 "AND item_borrower_vw.type = 'Book' "
				 "WHERE "
				 "item_borrower_vw.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append
		  ("SUBSTR(item_borrower_vw.duedate, 7, 4) || '/' || "
		   "SUBSTR(item_borrower_vw.duedate, 1, 2) || '/' || "
		   "SUBSTR(item_borrower_vw.duedate, 4, 2) < '");
		searchstr.append(now.toString("yyyy/MM/dd"));
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "book.title, "
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
				 "book.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "cd.title, "
				 "cd.id, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.quantity - "
				 "COUNT(item_borrower_vw.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower_vw.item_oid) AS "
				 "total_reserved, "
				 "cd.accession_number, "
				 "cd.type, "
				 "cd.myoid, "
				 "cd.front_cover "
				 "FROM "
				 "cd LEFT JOIN item_borrower_vw ON "
				 "cd.myoid = item_borrower_vw.item_oid "
				 "AND item_borrower_vw.type = 'CD' "
				 "WHERE "
				 "item_borrower_vw.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append
		  ("SUBSTR(item_borrower_vw.duedate, 7, 4) || '/' || "
		   "SUBSTR(item_borrower_vw.duedate, 1, 2) || '/' || "
		   "SUBSTR(item_borrower_vw.duedate, 4, 2) < '");
		searchstr.append(now.toString("yyyy/MM/dd"));
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "cd.title, "
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
				 "cd.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "dvd.title, "
				 "dvd.id, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.quantity - "
				 "COUNT(item_borrower_vw.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower_vw.item_oid) AS "
				 "total_reserved, "
				 "dvd.accession_number, "
				 "dvd.type, "
				 "dvd.myoid, "
				 "dvd.front_cover "
				 "FROM "
				 "dvd LEFT JOIN item_borrower_vw ON "
				 "dvd.myoid = item_borrower_vw.item_oid "
				 "AND item_borrower_vw.type = 'DVD' "
				 "WHERE "
				 "item_borrower_vw.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append
		  ("SUBSTR(item_borrower_vw.duedate, 7, 4) || '/' || "
		   "SUBSTR(item_borrower_vw.duedate, 1, 2) || '/' || "
		   "SUBSTR(item_borrower_vw.duedate, 4, 2) < '");
		searchstr.append(now.toString("yyyy/MM/dd"));
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "dvd.title, "
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
				 "dvd.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "journal.title, "
				 "journal.id, "
				 "journal.publisher, journal.pdate, "
				 "journal.category, "
				 "journal.language, "
				 "journal.price, journal.monetary_units, "
				 "journal.quantity, "
				 "journal.location, "
				 "journal.quantity - "
				 "COUNT(item_borrower_vw.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower_vw.item_oid) AS "
				 "total_reserved, "
				 "journal.accession_number, "
				 "journal.type, "
				 "journal.myoid, "
				 "journal.front_cover "
				 "FROM "
				 "journal LEFT JOIN item_borrower_vw ON "
				 "journal.myoid = item_borrower_vw.item_oid "
				 "AND item_borrower_vw.type = journal.type "
				 "WHERE "
				 "item_borrower_vw.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append
		  ("SUBSTR(item_borrower_vw.duedate, 7, 4) || '/' || "
		   "SUBSTR(item_borrower_vw.duedate, 1, 2) || '/' || "
		   "SUBSTR(item_borrower_vw.duedate, 4, 2) < '");
		searchstr.append(now.toString("yyyy/MM/dd"));
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "journal.title, "
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
				 "journal.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.quantity - "
				 "COUNT(item_borrower_vw.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower_vw.item_oid) AS "
				 "total_reserved, "
				 "magazine.accession_number, "
				 "magazine.type, "
				 "magazine.myoid, "
				 "magazine.front_cover "
				 "FROM "
				 "magazine LEFT JOIN item_borrower_vw ON "
				 "magazine.myoid = item_borrower_vw.item_oid "
				 "AND item_borrower_vw.type = magazine.type "
				 "WHERE "
				 "item_borrower_vw.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append
		  ("SUBSTR(item_borrower_vw.duedate, 7, 4) || '/' || "
		   "SUBSTR(item_borrower_vw.duedate, 1, 2) || '/' || "
		   "SUBSTR(item_borrower_vw.duedate, 4, 2) < '");
		searchstr.append(now.toString("yyyy/MM/dd"));
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "magazine.title, "
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
				 "magazine.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "videogame.title, "
				 "videogame.id, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.quantity - "
				 "COUNT(item_borrower_vw.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower_vw.item_oid) AS "
				 "total_reserved, "
				 "videogame.accession_number, "
				 "videogame.type, "
				 "videogame.myoid, "
				 "videogame.front_cover "
				 "FROM "
				 "videogame LEFT JOIN item_borrower_vw ON "
				 "videogame.myoid = "
				 "item_borrower_vw.item_oid "
				 "AND item_borrower_vw.type = 'Video Game' "
				 "WHERE "
				 "item_borrower_vw.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append
		  ("SUBSTR(item_borrower_vw.duedate, 7, 4) || '/' || "
		   "SUBSTR(item_borrower_vw.duedate, 1, 2) || '/' || "
		   "SUBSTR(item_borrower_vw.duedate, 4, 2) < '");
		searchstr.append(now.toString("yyyy/MM/dd"));
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "videogame.title, "
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
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "book.title, "
				 "book.id, "
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
				 "book.myoid, "
				 "book.front_cover "
				 "FROM "
				 "member, "
				 "book LEFT JOIN item_borrower ON "
				 "book.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = 'Book' "
				 "WHERE "
				 "member.memberid LIKE '%");
		searchstr.append(searchstrArg);
		searchstr.append("%' AND ");
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
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "book.title, "
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
				 "book.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "cd.title, "
				 "cd.id, "
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
				 "cd.myoid, "
				 "cd.front_cover "
				 "FROM "
				 "member, "
				 "cd LEFT JOIN item_borrower ON "
				 "cd.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = 'CD' "
				 "WHERE "
				 "member.memberid LIKE '%");
		searchstr.append(searchstrArg);
		searchstr.append("%' AND ");
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
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "cd.title, "
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
				 "cd.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "dvd.title, "
				 "dvd.id, "
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
				 "dvd.myoid, "
				 "dvd.front_cover "
				 "FROM "
				 "member, "
				 "dvd LEFT JOIN item_borrower ON "
				 "dvd.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = 'DVD' "
				 "WHERE "
				 "member.memberid LIKE '%");
		searchstr.append(searchstrArg);
		searchstr.append("%' AND ");
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
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "dvd.title, "
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
				 "dvd.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "journal.title, "
				 "journal.id, "
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
				 "journal.myoid, "
				 "journal.front_cover "
				 "FROM "
				 "member, "
				 "journal LEFT JOIN item_borrower ON "
				 "journal.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = journal.type "
				 "WHERE "
				 "member.memberid LIKE '%");
		searchstr.append(searchstrArg);
		searchstr.append("%' AND ");
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
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "journal.title, "
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
				 "journal.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "magazine.title, "
				 "magazine.id, "
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
				 "magazine.myoid, "
				 "magazine.front_cover "
				 "FROM "
				 "member, "
				 "magazine LEFT JOIN item_borrower ON "
				 "magazine.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = magazine.type "
				 "WHERE "
				 "member.memberid LIKE '%");
		searchstr.append(searchstrArg);
		searchstr.append("%' AND ");
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
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "magazine.title, "
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
				 "magazine.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "videogame.title, "
				 "videogame.id, "
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
				 "videogame.myoid, "
				 "videogame.front_cover "
				 "FROM "
				 "member, "
				 "videogame LEFT JOIN item_borrower ON "
				 "videogame.myoid = "
				 "item_borrower.item_oid "
				 "AND item_borrower.type = 'Video Game' "
				 "WHERE "
				 "member.memberid LIKE '%");
		searchstr.append(searchstrArg);
		searchstr.append("%' AND ");
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
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "videogame.title, "
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
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.accession_number, "
				 "book.type, "
				 "book.myoid, "
				 "item_request.myoid AS requestoid, "
				 "book.front_cover "
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
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.accession_number, "
				 "cd.type, "
				 "cd.myoid, "
				 "item_request.myoid AS requestoid, "
				 "cd.front_cover "
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
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.accession_number, "
				 "dvd.type, "
				 "dvd.myoid, "
				 "item_request.myoid AS requestoid, "
				 "dvd.front_cover "
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
				 "journal.title, "
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
				 "item_request.myoid AS requestoid, "
				 "journal.front_cover "
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
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.accession_number, "
				 "magazine.type, "
				 "magazine.myoid, "
				 "item_request.myoid AS requestoid, "
				 "magazine.front_cover "
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
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.accession_number, "
				 "videogame.type, "
				 "videogame.myoid, "
				 "item_request.myoid AS requestoid, "
				 "videogame.front_cover "
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
				 "item_request.requestdate, "
				 "book.title, "
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
				 "item_request.myoid AS requestoid, "
				 "book.front_cover "
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
				 "item_request.requestdate, "
				 "book.title, "
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
				 "item_request.myoid, "
				 "book.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_request.requestdate, "
				 "cd.title, "
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
				 "item_request.myoid AS requestoid, "
				 "cd.front_cover "
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
				 "item_request.requestdate, "
				 "cd.title, "
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
				 "item_request.myoid, "
				 "cd.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_request.requestdate, "
				 "dvd.title, "
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
				 "item_request.myoid AS requestoid, "
				 "dvd.front_cover "
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
				 "item_request.requestdate, "
				 "dvd.title, "
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
				 "item_request.myoid, "
				 "dvd.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_request.requestdate, "
				 "journal.title, "
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
				 "item_request.myoid AS requestoid, "
				 "journal.front_cover "
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
				 "item_request.requestdate, "
				 "journal.title, "
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
				 "item_request.myoid, "
				 "journal.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_request.requestdate, "
				 "magazine.title, "
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
				 "item_request.myoid AS requestoid, "
				 "magazine.front_cover "
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
				 "item_request.requestdate, "
				 "magazine.title, "
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
				 "item_request.myoid, "
				 "magazine.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_request.requestdate, "
				 "videogame.title, "
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
				 "item_request.myoid AS requestoid, "
				 "videogame.front_cover "
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
				 "item_request.requestdate, "
				 "videogame.title, "
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
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "book.title, "
				 "book.id, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.quantity - "
				 "COUNT(item_borrower_vw.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower_vw.item_oid) AS "
				 "total_reserved, "
				 "book.accession_number, "
				 "book.type, "
				 "book.myoid, "
				 "book.front_cover "
				 "FROM "
				 "book LEFT JOIN item_borrower_vw ON "
				 "book.myoid = item_borrower_vw.item_oid "
				 "AND item_borrower_vw.type = 'Book' "
				 "WHERE "
				 "item_borrower_vw.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "book.title, "
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
				 "book.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "cd.title, "
				 "cd.id, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.quantity - "
				 "COUNT(item_borrower_vw.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower_vw.item_oid) AS "
				 "total_reserved, "
				 "cd.accession_number, "
				 "cd.type, "
				 "cd.myoid, "
				 "cd.front_cover "
				 "FROM "
				 "cd LEFT JOIN item_borrower_vw ON "
				 "cd.myoid = item_borrower_vw.item_oid "
				 "AND item_borrower_vw.type = 'CD' "
				 "WHERE "
				 "item_borrower_vw.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "cd.title, "
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
				 "cd.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "dvd.title, "
				 "dvd.id, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.quantity - "
				 "COUNT(item_borrower_vw.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower_vw.item_oid) AS "
				 "total_reserved, "
				 "dvd.accession_number, "
				 "dvd.type, "
				 "dvd.myoid, "
				 "dvd.front_cover "
				 "FROM "
				 "dvd LEFT JOIN item_borrower_vw ON "
				 "dvd.myoid = item_borrower_vw.item_oid "
				 "AND item_borrower_vw.type = 'DVD' "
				 "WHERE "
				 "item_borrower_vw.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "dvd.title, "
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
				 "dvd.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "journal.title, "
				 "journal.id, "
				 "journal.publisher, journal.pdate, "
				 "journal.category, "
				 "journal.language, "
				 "journal.price, journal.monetary_units, "
				 "journal.quantity, "
				 "journal.location, "
				 "journal.quantity - "
				 "COUNT(item_borrower_vw.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower_vw.item_oid) AS "
				 "total_reserved, "
				 "journal.accession_number, "
				 "journal.type, "
				 "journal.myoid, "
				 "journal.front_cover "
				 "FROM "
				 "journal LEFT JOIN item_borrower_vw ON "
				 "journal.myoid = "
				 "item_borrower_vw.item_oid "
				 "AND item_borrower_vw.type = journal.type "
				 "WHERE "
				 "item_borrower_vw.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "journal.title, "
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
				 "journal.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.quantity - "
				 "COUNT(item_borrower_vw.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower_vw.item_oid) AS "
				 "total_reserved, "
				 "magazine.accession_number, "
				 "magazine.type, "
				 "magazine.myoid, "
				 "magazine.front_cover "
				 "FROM "
				 "magazine LEFT JOIN item_borrower_vw ON "
				 "magazine.myoid = "
				 "item_borrower_vw.item_oid "
				 "AND item_borrower_vw.type = magazine.type "
				 "WHERE "
				 "item_borrower_vw.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "magazine.title, "
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
				 "magazine.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "videogame.title, "
				 "videogame.id, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.quantity - "
				 "COUNT(item_borrower_vw.item_oid) "
				 "AS availability, "
				 "COUNT(item_borrower_vw.item_oid) AS "
				 "total_reserved, "
				 "videogame.accession_number, "
				 "videogame.type, "
				 "videogame.myoid, "
				 "videogame.front_cover "
				 "FROM "
				 "videogame LEFT JOIN item_borrower_vw ON "
				 "videogame.myoid = "
				 "item_borrower_vw.item_oid "
				 "AND item_borrower_vw.type = 'Video Game' "
				 "WHERE "
				 "item_borrower_vw.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "videogame.title, "
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
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "book.title, "
				 "book.id, "
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
				 "book.myoid, "
				 "book.front_cover "
				 "FROM "
				 "member, "
				 "book LEFT JOIN item_borrower ON "
				 "book.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = 'Book' "
				 "WHERE "
				 "member.memberid LIKE '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append("item_borrower.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "book.title, "
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
				 "book.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "cd.title, "
				 "cd.id, "
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
				 "cd.myoid, "
				 "cd.front_cover "
				 "FROM "
				 "member, "
				 "cd LEFT JOIN item_borrower ON "
				 "cd.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = 'CD' "
				 "WHERE "
				 "member.memberid LIKE '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append("item_borrower.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "cd.title, "
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
				 "cd.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "dvd.title, "
				 "dvd.id, "
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
				 "dvd.myoid, "
				 "dvd.front_cover "
				 "FROM "
				 "member, "
				 "dvd LEFT JOIN item_borrower ON "
				 "dvd.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = 'DVD' "
				 "WHERE "
				 "member.memberid LIKE '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append("item_borrower.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "dvd.title, "
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
				 "dvd.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "journal.title, "
				 "journal.id, "
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
				 "journal.myoid, "
				 "journal.front_cover "
				 "FROM "
				 "member, "
				 "journal LEFT JOIN item_borrower ON "
				 "journal.myoid = "
				 "item_borrower.item_oid "
				 "AND item_borrower.type = journal.type "
				 "WHERE "
				 "member.memberid LIKE '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append("item_borrower.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "journal.title, "
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
				 "journal.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "magazine.title, "
				 "magazine.id, "
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
				 "magazine.myoid, "
				 "magazine.front_cover "
				 "FROM "
				 "member, "
				 "magazine LEFT JOIN item_borrower ON "
				 "magazine.myoid = "
				 "item_borrower.item_oid "
				 "AND item_borrower.type = magazine.type "
				 "WHERE "
				 "member.memberid LIKE '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append("item_borrower.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "magazine.title, "
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
				 "magazine.front_cover ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "videogame.title, "
				 "videogame.id, "
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
				 "videogame.myoid, "
				 "videogame.front_cover "
				 "FROM "
				 "member, "
				 "videogame LEFT JOIN item_borrower ON "
				 "videogame.myoid = "
				 "item_borrower.item_oid "
				 "AND item_borrower.type = 'Video Game' "
				 "WHERE "
				 "member.memberid LIKE '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append("item_borrower.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "videogame.title, "
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
	      "COUNT(item_borrower_vw.item_oid) "
	      "AS availability, "
	      "COUNT(item_borrower_vw.item_oid) AS total_reserved, "
	      "videogame.accession_number, "
	      "videogame.type, "
	      "videogame.myoid, "
	      "videogame.front_cover "
	      "FROM "
	      "videogame LEFT JOIN item_borrower_vw ON "
	      "videogame.myoid = item_borrower_vw.item_oid "
	      "AND item_borrower_vw.type = 'Video Game' "
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
	      "COUNT(item_borrower_vw.item_oid) "
	      "AS availability, "
	      "COUNT(item_borrower_vw.item_oid) AS total_reserved, "
	      "book.originality, "
	      "book.condition, "
	      "book.accession_number, "
	      "book.type, "
	      "book.myoid, "
	      "book.front_cover "
	      "FROM "
	      "book LEFT JOIN item_borrower_vw ON "
	      "book.myoid = item_borrower_vw.item_oid "
	      "AND item_borrower_vw.type = 'Book' "
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
	      "COUNT(item_borrower_vw.item_oid) "
	      "AS availability, "
	      "COUNT(item_borrower_vw.item_oid) AS total_reserved, "
	      "dvd.accession_number, "
	      "dvd.type, "
	      "dvd.myoid, "
	      "dvd.front_cover "
	      "FROM "
	      "dvd LEFT JOIN item_borrower_vw ON "
	      "dvd.myoid = item_borrower_vw.item_oid "
	      "AND item_borrower_vw.type = 'DVD' "
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
	      "cd.quantity - COUNT(item_borrower_vw.item_oid) AS "
	      "availability, "
	      "COUNT(item_borrower_vw.item_oid) AS total_reserved, "
	      "cd.accession_number, "
	      "cd.type, "
	      "cd.myoid, "
	      "cd.front_cover "
	      "FROM "
	      "cd LEFT JOIN item_borrower_vw ON "
	      "cd.myoid = item_borrower_vw.item_oid "
	      "AND item_borrower_vw.type = 'CD' "
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
	      "photograph_collection.myoid, "
	      "photograph_collection.image_scaled "
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
				"COUNT(item_borrower_vw.item_oid) AS "
				"availability, "
				"COUNT(item_borrower_vw.item_oid) AS "
				"total_reserved, "
				"%1.accession_number, "
				"%1.type, "
				"%1.myoid, "
				"%1.front_cover "
				"FROM "
				"%1 LEFT JOIN item_borrower_vw ON "
				"%1.myoid = "
				"item_borrower_vw.item_oid "
				"AND item_borrower_vw.type = %1.type "
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
	    bool caseinsensitive = al.caseinsensitive->isChecked();

	    types.append("Book");
	    types.append("CD");
	    types.append("DVD");
	    types.append("Grey Literature");
	    types.append("Journal");
	    types.append("Magazine");
	    types.append("Photograph Collection");
	    types.append("Video Game");
	    searchstr = "";

	    while(!types.isEmpty())
	      {
		type = types.takeFirst();

		if(type == "Photograph Collection")
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

		QString E("");

		if(m_db.driverName() != "QSQLITE")
		  E = "E";

		if(caseinsensitive)
		  str.append
		    ("(id IS NULL OR LOWER(id) LIKE " + E + "'%" +
		     biblioteq_myqstring::
		     escape(al.idnumber->text().trimmed(), true) +
		     "%' ");
		else
		  str.append
		    ("(id IS NULL OR id LIKE " + E + "'%" +
		     biblioteq_myqstring::
		     escape(al.idnumber->text().trimmed()) +
		     "%' ");

		if(type == "Book")
		  {
		    if(caseinsensitive)
		      str.append("OR LOWER(isbn13) LIKE " + E + "'%" +
				 biblioteq_myqstring::
				 escape(al.idnumber->text().
					trimmed(), true) + "%')");
		    else
		      str.append("OR isbn13 LIKE " + E + "'%" +
				 biblioteq_myqstring::
				 escape(al.idnumber->text().
					trimmed()) + "%')");
		  }
		else
		  str.append(")");

		str.append(" AND ");

		if(caseinsensitive)
		  str.append
		    ("LOWER(title) LIKE " + E + "'%" +
		     biblioteq_myqstring::
		     escape(al.title->text().trimmed(), true) +
		     "%'");
		else
		  str.append("title LIKE " + E + "'%" +
			     biblioteq_myqstring::
			     escape(al.title->text().trimmed()) +
			     "%'");

		if(type != "Photograph Collection")
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
				  str.append
				    ("LOWER(category) LIKE " + E + "'%" +
				     biblioteq_myqstring::escape
				     (word.trimmed(),
				      true) +
				     "%'" + (words.isEmpty() ? "" : " OR "));
				else
				  str.append
				    ("category LIKE " + E + "'%" +
				     biblioteq_myqstring::escape
				     (word.trimmed()) +
				     "%'" + (words.isEmpty() ? "" : " OR "));
			      }

			    str.append(") AND ");
			  }
		      }
		    else
		      {
			if(caseinsensitive)
			  str.append("LOWER(category) LIKE " + E + "'%" +
				     biblioteq_myqstring::escape
				     (al.category->toPlainText().trimmed(),
				      true) +
				     "%' AND ");
			else
			  str.append("category LIKE " + E + "'%" +
				     biblioteq_myqstring::escape
				     (al.category->toPlainText().trimmed()) +
				     "%' AND ");
		      }

		    if(caseinsensitive)
		      str.append
			("LOWER(publisher) LIKE " + E + "'%" +
			 biblioteq_myqstring::
			 escape(al.publisher->text().trimmed(),
				true) +
			 "%' AND ");
		    else
		      str.append
			("publisher LIKE " + E + "'%" +
			 biblioteq_myqstring::
			 escape(al.publisher->text().trimmed()) +
			 "%' AND ");

		    if(al.price->value() > -0.01)
		      {
			str.append("price = ");
			str.append(QString::number(al.price->value()));
			str.append(" AND ");
		      }

		    if(al.language->currentIndex() != 0)
		      {
			if(caseinsensitive)
			  str.append("LOWER(language) = " + E + "'" +
				     biblioteq_myqstring::escape
				     (al.language->currentText().trimmed(),
				      true) +
				     "' AND ");
			else
			  str.append("language = " + E + "'" +
				     biblioteq_myqstring::escape
				     (al.language->currentText().trimmed()) +
				     "' AND ");
		      }

		    if(al.monetary_units->currentIndex() != 0)
		      {
			if(caseinsensitive)
			  str.append
			    ("LOWER(monetary_units) = " + E + "'" +
			     biblioteq_myqstring::
			     escape(al.monetary_units->
				    currentText().trimmed(),
				    true) +
			     "' AND ");
			else
			  str.append
			    ("monetary_units = " + E + "'" +
			     biblioteq_myqstring::
			     escape(al.monetary_units->
				    currentText().trimmed()) +
			     "' AND ");
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
				  str.append
				    ("LOWER(description) LIKE " + E + "'%" +
				     biblioteq_myqstring::escape
				     (word.trimmed(),
				      true) +
				     "%'" + (words.isEmpty() ? "" : " OR "));
				else
				  str.append
				    ("description LIKE " + E + "'%" +
				     biblioteq_myqstring::escape
				     (word.trimmed()) +
				     "%'" + (words.isEmpty() ? "" : " OR "));
			      }

			    str.append(") ");
			  }
		      }
		    else
		      {
			if(caseinsensitive)
			  str.append
			    ("LOWER(description) LIKE " + E + "'%" +
			     biblioteq_myqstring::
			     escape(al.description->
				    toPlainText().trimmed(),
				    true) + "%' ");
			else
			  str.append
			    ("description LIKE " + E + "'%" +
			     biblioteq_myqstring::
			     escape(al.description->
				    toPlainText().trimmed()) + "%' ");
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
				  str.append
				    ("COALESCE(LOWER(keyword), '') LIKE " +
				     E + "'%" +
				     biblioteq_myqstring::escape
				     (word.trimmed(),
				      true) +
				     "%'" + (words.isEmpty() ? "" : " OR "));
				else
				  str.append
				    ("COALESCE(keyword, '') LIKE " +
				     E + "'%" +
				     biblioteq_myqstring::escape
				     (word.trimmed()) +
				     "%'" + (words.isEmpty() ? "" : " OR "));
			      }

			    str.append(") ");
			  }
		      }
		    else
		      {
			if(caseinsensitive)
			  str.append("AND COALESCE(LOWER(keyword), '') LIKE " +
				     E + "'%" +
				     biblioteq_myqstring::escape
				     (al.keyword->toPlainText().trimmed(),
				      true) +
				     "%' ");
			else
			  str.append("AND COALESCE(keyword, '') LIKE " +
				     E + "'%" +
				     biblioteq_myqstring::escape
				     (al.keyword->toPlainText().trimmed()) +
				     "%' ");
		      }

		    if(al.quantity->value() != 0)
		      str.append("AND quantity = " +
				 al.quantity->text() + " ");

		    if(al.location->currentIndex() != 0)
		      {
			if(caseinsensitive)
			  str.append
			    ("AND LOWER(location) = " + E + "'" +
			     biblioteq_myqstring::escape
			     (al.location->currentText().trimmed(),
			      true) + "' ");
			else
			  str.append
			    ("AND location = " + E + "'" +
			     biblioteq_myqstring::escape
			     (al.location->currentText().trimmed()) + "' ");
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

		if(type != "Photograph Collection")
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

	    searchstr += "ORDER BY 1 ";
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

	break;
      }
    case POPULATE_SEARCH_BASIC:
      {
	types.append("Book");
	types.append("CD");
	types.append("DVD");
	types.append("Grey Literature");
	types.append("Journal");
	types.append("Magazine");
	types.append("Photograph Collection");
	types.append("Video Game");
	searchstr = "";

	while(!types.isEmpty())
	  {
	    type = types.takeFirst();

	    if(type == "Photograph Collection")
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

	    QString E("");

	    if(m_db.driverName() != "QSQLITE")
	      E = "E";

	    if(ui.searchType->currentIndex() == 0) // Category
	      {
		if(type != "Photograph Collection")
		  {
		    if(ui.case_insensitive->isChecked())
		      str.append("LOWER(category) LIKE " +
				 E + "'%" +
				 biblioteq_myqstring::escape
				 (searchstrArg.toLower().trimmed()) +
				 "%' ");
		    else
		      str.append("category LIKE " +
				 E + "'%" +
				 biblioteq_myqstring::escape
				 (searchstrArg.trimmed()) +
				 "%' ");
		  }
		else
		  {
		    if(ui.case_insensitive->isChecked())
		      str.append("COALESCE(LOWER(about), '') LIKE " +
				 E + "'%" +
				 biblioteq_myqstring::escape
				 (searchstrArg.toLower().trimmed()) +
				 "%' ");
		    else
		      str.append("COALESCE(about, '') LIKE " +
				 E + "'%" +
				 biblioteq_myqstring::escape
				 (searchstrArg.trimmed()) +
				 "%' ");
		  }
	      }
	    else if(ui.searchType->currentIndex() == 1) // ID
	      {
		if(ui.case_insensitive->isChecked())
		  str.append
		    ("(LOWER(id) LIKE " + E + "'%" +
		     biblioteq_myqstring::
		     escape(searchstrArg.toLower().trimmed()) +
		     "%' ");
		else
		  str.append
		    ("(id LIKE " + E + "'%" +
		     biblioteq_myqstring::escape(searchstrArg.trimmed()) +
		     "%' ");

		if(type == "Book")
		  {
		    if(ui.case_insensitive->isChecked())
		      str.append
			("OR LOWER(isbn13) LIKE " + E + "'%" +
			 biblioteq_myqstring::escape(searchstrArg.toLower().
						     trimmed()) + "%') ");
		    else
		      str.append
			("OR isbn13 LIKE " + E + "'%" +
			 biblioteq_myqstring::escape(searchstrArg.
						     trimmed()) + "%') ");
		  }
		else
		  str.append(") ");
	      }
	    else if(ui.searchType->currentIndex() == 2) // Keyword
	      {
		if(type != "Photograph Collection")
		  {
		    if(ui.case_insensitive->isChecked())
		      str.append("COALESCE(LOWER(keyword), '') LIKE " +
				 E + "'%" +
				 biblioteq_myqstring::escape
				 (searchstrArg.toLower().trimmed()) +
				 "%' ");
		    else
		      str.append("COALESCE(keyword, '') LIKE " +
				 E + "'%" +
				 biblioteq_myqstring::escape
				 (searchstrArg.trimmed()) +
				 "%' ");
		  }
		else
		  {
		    if(ui.case_insensitive->isChecked())
		      str.append("COALESCE(LOWER(about), '') LIKE " +
				 E + "'%" +
				 biblioteq_myqstring::escape
				 (searchstrArg.toLower().trimmed()) +
				 "%' ");
		    else
		      str.append("COALESCE(about, '') LIKE " +
				 E + "'%" +
				 biblioteq_myqstring::escape
				 (searchstrArg.trimmed()) +
				 "%' ");
		  }
	      }
	    else // Title
	      {
		if(ui.case_insensitive->isChecked())
		  str.append("LOWER(title) LIKE " + E + "'%" +
			     biblioteq_myqstring::
			     escape(searchstrArg.toLower().trimmed()) +
			     "%' ");
		else
		  str.append("title LIKE " + E + "'%" +
			     biblioteq_myqstring::
			     escape(searchstrArg.trimmed()) +
			     "%' ");
	      }

	    if(type != "Photograph Collection")
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

	searchstr += "ORDER BY 1 ";
	searchstr += limitStr + offsetStr;
	break;
      }
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);
  ui.itemsCountLabel->setText(QString(tr("%1 Result(s)")).
			      arg(ui.table->rowCount()));

  QSqlQuery query(m_db);

  query.setForwardOnly(true);

  if(!query.exec(searchstr))
    {
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

      addError(QString(tr("Database Error")),
	       QString(tr("Unable to retrieve the data required for "
			  "populating the main views.")),
	       query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			    tr("Unable to retrieve the data required for "
			       "populating the main views."));
      return 1;
    }

  QApplication::restoreOverrideCursor();
  prepareRequestToolButton(typefilter);

  bool found = false;

  for(int ii = 0; ii < ui.menu_Category->actions().size(); ii++)
    if(typefilter ==
       ui.menu_Category->actions().at(ii)->data().toString())
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

  if(search_type != CUSTOM_QUERY)
    ui.table->resetTable(m_db.userName(), typefilter, m_roles);
  else
    ui.table->resetTable(m_db.userName(), "", m_roles);

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
#ifndef Q_OS_MAC
  progress.repaint();
  QApplication::processEvents();
#endif

  biblioteq_graphicsitempixmap *pixmapItem = 0;
  int iconTableColumnIdx = 0;
  int iconTableRowIdx = 0;

  /*
  ** Adjust the dimensions of the graphics scene if pagination
  ** is effectively disabled.
  */

  if(limit == -1)
    {
      int size = biblioteq_misc_functions::sqliteQuerySize
	(searchstr, m_db, __FILE__, __LINE__);

      if(size > 0 && (size / 250 <= std::numeric_limits<int>::max()))
	ui.graphicsView->setSceneRect(0, 0,
				      5 * 150,
				      size * 250 + 15);
      else
	ui.graphicsView->setSceneRect(0, 0,
				      5 * 150,
				      std::numeric_limits<int>::max());
    }

  if(limit != -1 && m_db.driver()->hasFeature(QSqlDriver::QuerySize))
    progress.setMaximum(query.size());

  i = -1;

  while(i++, !progress.wasCanceled() && query.next())
    {
      pixmapItem = 0;

      if(query.isValid())
	for(int j = 0; j < query.record().count(); j++)
	  {
	    item = 0;

	    if(!query.record().fieldName(j).endsWith("front_cover") &&
	       !query.record().fieldName(j).endsWith("image_scaled"))
	      {
		if(query.record().fieldName(j).contains("date") ||
		   query.record().fieldName(j).contains("membersince"))
		  {
		    QDate date(QDate::fromString(query.value(j).toString(),
						 "MM/dd/yyyy"));

		    str = date.toString(Qt::ISODate);
		  }
		else
		  str = query.value(j).toString();
	      }

	    if(search_type == CUSTOM_QUERY)
	      if(!tmplist.contains(query.record().fieldName(j)))
		{
		  tmplist.append(query.record().fieldName(j));
		  ui.table->setColumnCount(tmplist.size());
		}

	    if(query.record().fieldName(j).endsWith("issue") ||
	       query.record().fieldName(j).endsWith("price") ||
	       query.record().fieldName(j).endsWith("volume") ||
	       query.record().fieldName(j).endsWith("quantity") ||
	       query.record().fieldName(j).endsWith("issueno") ||
	       query.record().fieldName(j).endsWith("issuevolume") ||
	       query.record().fieldName(j).endsWith("cddiskcount") ||
	       query.record().fieldName(j).endsWith("dvddiskcount") ||
	       query.record().fieldName(j).endsWith("availability") ||
	       query.record().fieldName(j).endsWith("total_reserved") ||
	       query.record().fieldName(j).endsWith("photograph_count"))
	      {
		if(query.record().fieldName(j).endsWith("price"))
		  {
		    item = new(std::nothrow) biblioteq_numeric_table_item
		      (query.value(j).toDouble());
		    str = QString::number(query.value(j).toDouble(), 'f', 2);
		  }
		else
		  item = new(std::nothrow) biblioteq_numeric_table_item
		    (query.value(j).toInt());
	      }
	    else if(query.record().fieldName(j).endsWith("callnumber"))
	      {
		str = query.value(j).toString();
		item = new(std::nothrow) biblioteq_callnum_table_item(str);
	      }
	    else if(query.record().fieldName(j).endsWith("front_cover") ||
		    query.record().fieldName(j).endsWith("image_scaled"))
	      {
		QImage image;

		image.loadFromData
		  (QByteArray::fromBase64(query.value(j).
					  toByteArray()));

		if(image.isNull())
		  image.loadFromData(query.value(j).toByteArray());

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

		    pixmapItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
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
		    ui.table->setRowCount(ui.table->rowCount() + 1);
		    ui.itemsCountLabel->setText(QString(tr("%1 Result(s)")).
						arg(ui.table->rowCount()));
		  }

		ui.table->setItem(i, j, item);

		if(query.record().fieldName(j).endsWith("type"))
		  {
		    itemType = str;
		    itemType = itemType.toLower().remove(" ");
		  }

		if(query.record().fieldName(j).endsWith("myoid"))
		  updateRows(str, i, itemType);
	      }
	    else if(!query.record().fieldName(j).endsWith("front_cover") &&
		    !query.record().fieldName(j).endsWith("image_scaled"))
	      addError(QString(tr("Memory Error")),
		       QString(tr("Unable to allocate "
				  "memory for the \"item\" "
				  "object. "
				  "This is a serious "
				  "problem!")), QString(""),
		       __FILE__, __LINE__);
	  }

      if(query.isValid())
	if(pixmapItem)
	  for(int ii = 0; ii < query.record().count(); ii++)
	    {
	      if(query.record().fieldName(ii).endsWith("myoid"))
		pixmapItem->setData(0, query.value(ii));
	      else if(query.record().fieldName(ii).endsWith("type"))
		pixmapItem->setData(1, query.value(ii));
	    }

      if(i + 1 <= progress.maximum())
	progress.setValue(i + 1);

#ifndef Q_OS_MAC
      progress.repaint();
      QApplication::processEvents();
#endif
    }

  if(limit != -1 && !m_db.driver()->hasFeature(QSqlDriver::QuerySize))
    progress.setValue(limit);

  bool wasCanceled = progress.wasCanceled(); /*
					     ** QProgressDialog::close()!
					     */

  progress.close();
  ui.table->setSortingEnabled(true);

  if(search_type == CUSTOM_QUERY)
    {
      if(tmplist.isEmpty())
	for(int ii = 0; ii < query.record().count(); ii++)
	  if(!tmplist.contains(query.record().fieldName(ii)))
	    {
	      tmplist.append(query.record().fieldName(ii));
	      ui.table->setColumnCount(tmplist.size());
	    }

      ui.table->setColumnCount(tmplist.size());
      ui.table->setHorizontalHeaderLabels(tmplist);
      ui.table->setColumnNames(tmplist);
      tmplist.clear();
      addConfigOptions("Custom");
    }
  else if(search_type == POPULATE_SEARCH)
    {
      if(typefilter == "All")
	if(ui.table->rowCount() == 0)
	  {
	    foreach(QWidget *widget, m_all_diag->findChildren<QWidget *> ())
	      widget->setEnabled(true);

	    al.reset->setVisible(false);
	  }
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

void biblioteq::slotSearchBasic(void)
{
  if(!m_db.isOpen())
    return;

  ui.case_insensitive->setEnabled(false);
  ui.search->setEnabled(false);
  ui.searchType->setEnabled(false);
  (void) populateTable
    (POPULATE_SEARCH_BASIC, "All", ui.search->text().trimmed());
}

void biblioteq::slotResetAllSearch(void)
{
  ui.graphicsView->scene()->clear();
  ui.graphicsView->resetTransform();
  ui.graphicsView->verticalScrollBar()->setValue(0);
  ui.graphicsView->horizontalScrollBar()->setValue(0);
  ui.itemsCountLabel->setText(tr("0 Results"));
  ui.nextPageButton->setEnabled(false);
  ui.pagesLabel->setText("1");
  ui.previousPageButton->setEnabled(false);
  ui.table->resetTable(m_db.userName(), m_lastCategory, m_roles);
  ui.table->scrollToTop();
  ui.table->horizontalScrollBar()->setValue(0);
  ui.table->clearSelection();
  ui.table->setCurrentItem(0);
  resetAllSearchWidgets();
  slotDisplaySummary();
}

void biblioteq::resetAllSearchWidgets(void)
{
  ui.case_insensitive->setChecked(false);
  ui.case_insensitive->setEnabled(true);
  ui.search->clear();
  ui.search->setEnabled(true);
  ui.searchType->setCurrentIndex(0);
  ui.searchType->setEnabled(true);
}

void biblioteq::slotResetGeneralSearch(void)
{
  /*
  ** Do not clear field values.
  */

  ui.graphicsView->scene()->clear();
  ui.graphicsView->resetTransform();
  ui.graphicsView->verticalScrollBar()->setValue(0);
  ui.graphicsView->horizontalScrollBar()->setValue(0);
  ui.itemsCountLabel->setText(tr("0 Results"));
  ui.nextPageButton->setEnabled(false);
  ui.pagesLabel->setText("1");
  ui.previousPageButton->setEnabled(false);
  ui.table->resetTable(m_db.userName(), m_lastCategory, m_roles);
  ui.table->scrollToTop();
  ui.table->horizontalScrollBar()->setValue(0);
  ui.table->clearSelection();
  ui.table->setCurrentItem(0);
  slotDisplaySummary();

  foreach(QWidget *widget, m_all_diag->findChildren<QWidget *> ())
    widget->setEnabled(true);

  al.publication_date_enabled->setChecked(false);
  al.reset->setVisible(false);
  al.idnumber->setFocus();
}

void biblioteq::slotRoleChanged(int index)
{
  if(index == 1)
    {
      br.password->setEnabled(false);
      br.password->setText("xbook_guest");
      br.userid->setEnabled(false);
      br.userid->setText("xbook_guest");
    }
  else
    {
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
  query.bindValue(1, m_db.userName());
  query.exec();

  if(!query.lastError().text().toLower().contains("duplicate"))
    addError(QString(tr("Database Error")),
	     QString(tr("Unable to insert into member_history_dnt for "
			"member %1.").arg(m_db.userName())),
	     query.lastError().text(), __FILE__, __LINE__);

  query.prepare("UPDATE member_history_dnt "
		"SET dnt = ? "
		"WHERE memberid = ?");
  query.bindValue(0, QVariant(state).toInt());
  query.bindValue(1, m_db.userName());
  query.exec();

  if(query.lastError().isValid())
    addError(QString(tr("Database Error")),
	     QString(tr("Unable to update member_history_dnt for "
			"member %1.").arg(m_db.userName())),
	     query.lastError().text(), __FILE__, __LINE__);

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
    return;

#ifndef Q_OS_MAC
  repaint();
  QApplication::processEvents();
#endif
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
  list.append("ALTER TABLE member ADD comments TEXT, "
	      "ADD general_registration_number TEXT, ADD memberclass TEXT");
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
  list.append("INSERT INTO member_temporary SELECT * FROM member");
  list.append("DROP TABLE member");
  list.append("ALTER TABLE member_temporary RENAME TO member");
  list.append("CREATE TABLE journal_temporary "
	      "("
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
  list.append("INSERT INTO journal_temporary SELECT * FROM journal");
  list.append("DROP TABLE journal");
  list.append("ALTER TABLE journal_temporary RENAME TO journal");
  list.append("CREATE TABLE magazine_temporary"
	      "("
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
  list.append("INSERT INTO magazine_temporary SELECT * FROM magazine");
  list.append("DROP TABLE magazine");
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
  list.append("CREATE TABLE IF NOT EXISTS grey_literature_images "
	      "("
	      "description TEXT,"
	      "image BYTEA NOT NULL,"
	      "item_oid BIGINT NOT NULL,"
	      "myoid BIGINT UNIQUE,"
	      "FOREIGN KEY(item_oid) REFERENCES grey_literature(myoid) ON "
	      "DELETE CASCADE,"
	      "PRIMARY KEY(item_oid, myoid)"
	      ")");

  QString errors("");
  int ct = 0;

  for(int i = 0; i < list.size(); i++)
    {
      QSqlQuery query(m_db);

      if(!query.exec(list.at(i)))
	{
	  errors.append(QString("Error %1: %2.\n").
			arg(ct).arg(query.lastError().text()));
	  ct += 1;
	}
    }

  QApplication::restoreOverrideCursor();

  if(!errors.isEmpty())
    {
      QDialog dialog(this);
      Ui_generalmessagediag ui;

      ui.setupUi(&dialog);
      ui.text->setPlainText(errors);
      dialog.exec();
    }
  else
    QMessageBox::information
      (this,
       tr("BiblioteQ: Information"),
       tr("The database %1 was upgraded successfully.").
       arg(m_db.databaseName()));
}

void biblioteq::preparePhotographsPerPageMenu(void)
{
  QActionGroup *group = 0;

  if((group = new(std::nothrow) QActionGroup(this)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  ui.menuPhotographs_per_Page->clear();

  QSettings settings;
  int integer = settings.value("photographs_per_page", 25).toInt();

  if(!(integer == -1 || (integer >= 25 && integer <= 100)))
    integer = 25;

  for(int i = 1; i <= 5; i++)
    {
      QAction *action = 0;

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

void biblioteq::slotPhotographsPerPageChanged(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  QSettings settings;

  settings.setValue("photographs_per_page", action->data().toInt());
}

QVariant biblioteq::setting(const QString &name) const
{
  if(name == "automatically_resize_column_widths")
    return ui.actionAutomatically_Resize_Column_Widths->isChecked();
  else if(name == "photographs_per_page")
    {
      foreach(QAction *action, ui.menuPhotographs_per_Page->actions())
	if(action->isChecked())
	  return action->data().toInt();

      return 25;
    }
  else
    return QVariant();
}

QString biblioteq::homePath(void)
{
#ifdef Q_OS_WIN32
  return QDir::currentPath() + QDir::separator() + ".biblioteq";
#else
  return QDir::homePath() + QDir::separator() + ".biblioteq";
#endif
}

bool biblioteq::isGuest(void) const
{
  if(m_db.driverName() == "QSQLITE")
    return false;
  else if(m_db.userName() == "xbook_guest")
    return true;
  else
    return false;
}

void biblioteq::slotReloadBiblioteqConf(void)
{
  m_otheroptions->prepareSettings();
  readGlobalSetup();
}
