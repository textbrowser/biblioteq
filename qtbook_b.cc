/*
** -- Qt Includes --
*/

#include <QtDebug>
#include <QSqlRecord>

/*
** -- Local Includes --
*/

#include "qtbook.h"

/*
** -- Global Variables --
*/

extern QApplication *qapp;

/*
** -- populateTable() --
*/

int qtbook::populateTable(const int search_type_arg,
			  const QString &typefilter,
			  const QString &searchstrArg, const int pagingType)
{
  int i = -1;
  int j = 0;
  int search_type = search_type_arg;
  QDate now = QDate::currentDate();
  QString str = "";
  QString type = "";
  QString itemType = "";
  QString searchstr = "";
  QStringList types;
  QStringList tmplist; // Used for custom queries.
  QProgressDialog progress(this);
  QTableWidgetItem *item = 0;

#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  progress.setAttribute(Qt::WA_MacMetalStyle, true);
#endif
#endif

  int limit = 0;
  int offset = m_queryOffset;
  QString limitStr("");
  QString offsetStr("");

  for(i = 0; i < ui.menuEntriesPerPage->actions().size(); i++)
    if(ui.menuEntriesPerPage->actions()[i]->isChecked())
      {
	limit = ui.menuEntriesPerPage->actions()[i]->data().toInt();
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
	 limit / 5 * 200);
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
	       "photograph_collection.type, "
	       "photograph_collection.myoid, "
	       "photograph_collection.image_scaled "
	       "FROM photograph_collection "
	       "GROUP BY "
	       "photograph_collection.title, "
	       "photograph_collection.id, "
	       "photograph_collection.location, "
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

	    if(roles.isEmpty())
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
				 "videogame.type, "
				 "videogame.myoid, "
				 "videogame.front_cover ");
		searchstr.append("ORDER BY 1");
		searchstr.append(limitStr + offsetStr);
	      }
	    else // !roles.isEmpty()
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

	    if(roles.isEmpty())
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
				 "book.type, "
				 "book.myoid, "
				 "item_request.myoid, "
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
				 "cd.type, "
				 "cd.myoid, "
				 "item_request.myoid, "
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
				 "dvd.type, "
				 "dvd.myoid, "
				 "item_request.myoid, "
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
				 "journal.type, "
				 "journal.myoid, "
				 "item_request.myoid, "
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
				 "magazine.type, "
				 "magazine.myoid, "
				 "item_request.myoid, "
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
				 "videogame.type, "
				 "videogame.myoid, "
				 "item_request.myoid, "
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
				 "videogame.type, "
				 "videogame.myoid, "
				 "item_request.myoid, "
				 "videogame.front_cover ");
		searchstr.append("ORDER BY 1");
		searchstr.append(limitStr + offsetStr);
	      }
	    else // !roles.isEmpty()
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
				 "book.type, "
				 "book.myoid, "
				 "item_request.myoid, "
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
				 "cd.type, "
				 "cd.myoid, "
				 "item_request.myoid, "
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
				 "dvd.type, "
				 "dvd.myoid, "
				 "item_request.myoid, "
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
				 "journal.type, "
				 "journal.myoid, "
				 "item_request.myoid, "
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
				 "magazine.type, "
				 "magazine.myoid, "
				 "item_request.myoid, "
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
				 "videogame.type, "
				 "videogame.myoid, "
				 "item_request.myoid, "
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

	    if(roles.isEmpty())
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
				 "videogame.type, "
				 "videogame.myoid, "
				 "videogame.front_cover ");
		searchstr.append("ORDER BY 1");
		searchstr.append(limitStr + offsetStr);
	      }
	    else // !roles.isEmpty()
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
	    types.append("Book");
	    types.append("CD");
	    types.append("DVD");
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

		if(db.driverName() != "QSQLITE")
		  E = "E";

		str.append("(id IS NULL OR id LIKE " + E + "'%" +
			   myqstring::escape(al.idnumber->text().trimmed()) +
			   "%' ");

		if(type == "Book")
		  str.append("OR isbn13 LIKE " + E + "'%" +
			     myqstring::escape(al.idnumber->text().
					       trimmed()) + "%')");
		else
		  str.append(")");

		str.append(" AND ");
		str.append("title LIKE " + E + "'%" +
			   myqstring::escape(al.title->text().trimmed()) +
			   "%'");

		if(type != "Photograph Collection")
		  {
		    str.append(" AND ");

		    if(al.publication_date->date().toString
		       ("MM/yyyy") != "01/7999")
		      str.append
			("SUBSTR(pdate, 1, 3) || SUBSTR(pdate, 7) = '" +
			 al.publication_date->date().toString
			 ("MM/yyyy") + "' AND ");

		    str.append("category LIKE " + E + "'%" +
			       myqstring::escape
			       (al.category->toPlainText().trimmed()) +
			       "%' AND ");
		    str.append
		      ("publisher LIKE " + E + "'%" +
		       myqstring::escape(al.publisher->text().trimmed()) +
		       "%' AND ");

		    if(al.price->value() > -0.01)
		      {
			str.append("price = ");
			str.append(QString::number(al.price->value()));
			str.append(" AND ");
		      }

		    if(al.language->currentIndex() != 0)
		      str.append("language = " + E + "'" +
				 myqstring::escape
				 (al.language->currentText().trimmed()) +
				 "' AND ");

		    if(al.monetary_units->currentIndex() != 0)
		      str.append
			("monetary_units = " + E + "'" +
			 myqstring::escape(al.monetary_units->
					   currentText().trimmed()) +
			 "' AND ");

		    str.append
		      ("description LIKE " + E + "'%" +
		       myqstring::escape(al.description->
					 toPlainText().trimmed()) + "%' ");
		    str.append("AND COALESCE(keyword, '') LIKE " + E + "'%" +
			       myqstring::escape
			       (al.keyword->toPlainText().trimmed()) +
			       "%' ");

		    if(al.quantity->value() != 0)
		      str.append("AND quantity = " +
				 al.quantity->text() + " ");

		    if(al.location->currentIndex() != 0)
		      str.append("AND location = " + E + "'" +
				 myqstring::escape
				 (al.location->currentText().trimmed()) + "' ");

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
    }

  qapp->setOverrideCursor(Qt::WaitCursor);
  ui.itemsCountLabel->setText(QString(tr("%1 Result(s)")).
			      arg(ui.table->rowCount()));

  QSqlQuery query(db);

  if(!query.exec(searchstr))
    {
      qapp->restoreOverrideCursor();

      if(!previousTypeFilter.isEmpty())
	ui.typefilter->setCurrentIndex
	  (ui.typefilter->findData(QVariant(previousTypeFilter)));

      addError(QString(tr("Database Error")),
	       QString(tr("Unable to retrieve the data required for "
			  "populating the main table.")),
	       query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			    tr("Unable to retrieve the data required for "
			       "populating the main table."));
      return 1;
    }

  qapp->restoreOverrideCursor();
  prepareRequestToolButton(typefilter);

  if(ui.typefilter->findData(QVariant(typefilter)) > -1)
    previousTypeFilter = typefilter;

  if(typefilter.isEmpty())
    ui.typefilter->setCurrentIndex(0);
  else if(ui.typefilter->findData(QVariant(typefilter)) > -1)
    ui.typefilter->setCurrentIndex
      (ui.typefilter->findData(QVariant(typefilter)));
  else
    ui.typefilter->setCurrentIndex(0);

  if(search_type != CUSTOM_QUERY)
    ui.table->resetTable(db.userName(), typefilter, roles);
  else
    ui.table->resetTable(db.userName(), "", roles);

  int currentPage = offset / limit + 1;

  if(limit == -1)
    currentPage = 1;

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

      for(int ii = 1; ii <= m_pages; ii++)
	if(ii == currentPage)
	  str += QString(tr(" %1 ")).arg(currentPage);
	else
	  str += QString(" <a href=\"%1\">" + tr("%1") + "</a> ").arg(ii);

      str = str.trimmed();
      ui.pagesLabel->setText(str);
    }
  else
    {
      int start = 2;
      QString str("");

      if(currentPage == 1)
	str += tr(" 1 ... ");
      else
	str += " <a href=\"1\">" + tr("1") + "</a>" + tr(" ... ");

      if(currentPage != 1)
	while(!(start <= currentPage && currentPage <= start + 6))
	  start += 7;

      for(int ii = start; ii <= start + 6; ii++)
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

  if(typefilter != "All Overdue" &&
     typefilter != "All Requested" &&
     typefilter != "All Reserved")
    lastSearchStr = searchstr;
  else
    lastSearchStr = searchstrArg;

  lastSearchType = search_type;
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
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.setLabelText(tr("Populating the table..."));

  if(limit == -1)
    {
      progress.setMaximum(0);
      progress.setMinimum(0);
    }
  else
    {
      progress.setMinimum(0);
      progress.setMaximum(limit);
    }

  raise();
  progress.show();
  progress.update();

  int iconTableRowIdx = 0;
  int iconTableColumnIdx = 0;
  QGraphicsPixmapItem *pixmapItem = 0;

  /*
  ** Adjust the dimensions of the graphics scene if pagination
  ** is effectively disabled.
  */

  if(limit == -1)
    {
      int size = misc_functions::sqliteQuerySize
	(searchstr, db, __FILE__, __LINE__);

      if(size > 0 && (size / 250 <= INT_MAX))
	ui.graphicsView->setSceneRect(0, 0, 5 * 150,
				      size * 250);
      else
	ui.graphicsView->setSceneRect(0, 0, 5 * 150, INT_MAX);
    }

  i = -1;

  while(i++, !progress.wasCanceled() && query.next())
    {
      pixmapItem = 0;

      if(query.isValid())
	for(j = 0; j < query.record().count(); j++)
	  {
	    item = 0;

	    if(query.record().fieldName(j) != "front_cover" &&
	       query.record().fieldName(j) != "image_scaled")
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

	    if(query.record().fieldName(j) == "issue" ||
	       query.record().fieldName(j) == "price" ||
	       query.record().fieldName(j) == "volume" ||
	       query.record().fieldName(j) == "quantity" ||
	       query.record().fieldName(j) == "issueno" ||
	       query.record().fieldName(j) == "issuevolume" ||
	       query.record().fieldName(j) == "cddiskcount" ||
	       query.record().fieldName(j) == "dvddiskcount" ||
	       query.record().fieldName(j) == "availability" ||
	       query.record().fieldName(j) == "total_reserved" ||
	       query.record().fieldName(j) == "photograph_count")
	      {
		if(query.record().fieldName(j) == "price")
		  {
		    item = new(std::nothrow) numeric_table_item
		      (query.value(j).toDouble());
		    str = QString::number(query.value(j).toDouble(), 'f', 2);
		  }
		else
		  item = new(std::nothrow) numeric_table_item
		    (query.value(j).toInt());
	      }
	    else if(query.record().fieldName(j) == "callnumber")
	      {
		str = query.value(j).toString();
		item = new(std::nothrow) callnum_table_item(str);
	      }
	    else if(query.record().fieldName(j) == "front_cover" ||
		    query.record().fieldName(j) == "image_scaled")
	      {
		QImage image;

		image.loadFromData
		  (QByteArray::fromBase64(query.value(j).
					  toByteArray()));

		if(image.isNull())
		  image.loadFromData(query.value(j).toByteArray());

		if(image.isNull())
		  image = QImage("icons.d/no_image.png");

		/*
		** The size of no_image.png is 126x187.
		*/

		if(!image.isNull())
		  image = image.scaled
		    (126, 187, Qt::KeepAspectRatio,
		     Qt::SmoothTransformation);

		pixmapItem = ui.graphicsView->scene()->addPixmap
		  (QPixmap().fromImage(image));
		pixmapItem->setPos(150 * iconTableColumnIdx,
				   200 * iconTableRowIdx);
		pixmapItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
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
		  ui.table->setRowCount(ui.table->rowCount() + 1);

		ui.table->setItem(i, j, item);

		if(query.record().fieldName(j) == "type")
		  {
		    itemType = str;
		    itemType = itemType.toLower().remove(" ");
		  }

		if(query.record().fieldName(j) == "myoid")
		  updateRows(str, i, itemType);
	      }
	    else if(query.record().fieldName(j) != "front_cover" &&
		    query.record().fieldName(j) != "image_scaled")
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
	      if(query.record().fieldName(ii) == "myoid")
		pixmapItem->setData(0, query.value(ii));
	      else if(query.record().fieldName(ii) == "type")
		pixmapItem->setData(1, query.value(ii));
	    }

      if(i + 1 <= progress.maximum())
	progress.setValue(i + 1);

      progress.update();
#ifndef Q_OS_MAC
      qapp->processEvents();
#endif
    }

  progress.hide();
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

  if(search_type != CUSTOM_QUERY)
    addConfigOptions(typefilter);

  slotResizeColumns();
  ui.previousPageButton->setEnabled(m_queryOffset > 0);
  ui.itemsCountLabel->setText(QString(tr("%1 Result(s)")).
			      arg(ui.table->rowCount()));

  if(limit == -1)
    ui.nextPageButton->setEnabled(false);
  else if(ui.table->rowCount() < limit)
    ui.nextPageButton->setEnabled(false);
  else
    ui.nextPageButton->setEnabled(true);

#ifdef Q_OS_MAC
  ui.table->hide();
  ui.table->show();
#endif
  return 0;
}
