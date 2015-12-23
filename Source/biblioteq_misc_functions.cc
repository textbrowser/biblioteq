/*
** -- Qt Includes --
*/

#include <QDate>
#include <QSqlField>
#include <QSqlIndex>
#include <QtDebug>

/*
** -- Local Includes --
*/

#include "biblioteq.h"
#include "biblioteq_misc_functions.h"

extern biblioteq *qmain;

/*
** As of SQLite 3.5.9, users are not supported.
*/

/*
** -- userCount() --
*/

qint64 biblioteq_misc_functions::userCount(const QString &userid,
					   const QSqlDatabase &db,
					   QString &errorstr)
{
  QSqlQuery query(db);
  qint64 count = 0;

  errorstr = "";
  query.setForwardOnly(true);

  if(db.driverName() == "QSQLITE")
    query.prepare("SELECT COUNT(memberid) FROM member WHERE "
		  "memberid = ?");
  else
    query.prepare("SELECT COUNT(usename) FROM pg_user WHERE "
		  "LOWER(usename) = LOWER(?)");

  query.bindValue(0, userid);

  if(query.exec())
    if(query.next())
      count = query.value(0).toLongLong();

  if(query.lastError().isValid())
    {
      count = -1;
      errorstr = query.lastError().text();
    }

  return count;
}

/*
** -- getAbstractInfo() --
*/

QString biblioteq_misc_functions::getAbstractInfo(const QString &oid,
						  const QString &typeArg,
						  const QSqlDatabase &db)
{
  QSqlQuery query(db);
  QString querystr = "";
  QString str = "";
  QString type(typeArg.toLower());

  if(type == "photograph collection")
    {
      type = type.replace(" ", "_");
      querystr = QString("SELECT about FROM %1 WHERE myoid = ?").arg
	(type);
    }
  else if(type == "book" || type == "cd" || type == "dvd" ||
	  type == "journal" || type == "magazine" || type == "video game")
    {
      type = type.remove(" ");
      querystr = QString("SELECT description FROM %1 WHERE myoid = ?").arg
	(type);
    }

  query.setForwardOnly(true);
  query.prepare(querystr);
  query.bindValue(0, oid);

  if(query.exec())
    if(query.next())
      str = query.value(0).toString();

  return str;
}

/*
** -- getImage() --
*/

QImage biblioteq_misc_functions::getImage(const QString &oid,
					  const QString &which,
					  const QString &typeArg,
					  const QSqlDatabase &db)
{
  QImage image = QImage();
  QSqlQuery query(db);
  QString type(typeArg.toLower());

  if(type == "photograph collection")
    type = type.replace(" ", "_");
  else
    type = type.remove(" ");

  if(type == "book" || type == "cd" || type == "dvd" || type == "journal" ||
     type == "magazine" || type == "photograph_collection" ||
     type == "videogame")
    if(which == "back_cover" || which == "front_cover" ||
       which == "image_scaled")
      {
	query.setForwardOnly(true);
	query.prepare(QString("SELECT %1 FROM %2 WHERE myoid = ?").
		      arg(which).arg(type));
	query.bindValue(0, oid);
      }

  if(query.exec())
    if(query.next())
      {
	image.loadFromData
	  (QByteArray::fromBase64(query.value(0).toByteArray()));

	if(image.isNull())
	  image.loadFromData(query.value(0).toByteArray());
      }

  return image;
}

/*
** -- grantPrivs() --
*/

void biblioteq_misc_functions::grantPrivs(const QString &userid,
					  const QString &roles,
					  const QSqlDatabase &db,
					  QString &errorstr)
{
  errorstr = "";

  if(db.driverName() == "QSQLITE")
    return; // Users are not supported.

  QSqlQuery query(db);
  QString querystr = "";
  QString str(roles);

  if(str.isEmpty() || str == "none")
    str = "biblioteq_patron";
  else
    {
      str.replace(" ", "_");
      str.prepend("biblioteq_");
    }

  if(str.contains("administrator"))
    {
      (void) query.exec(QString("GRANT biblioteq_administrator "
				"TO %1 WITH ADMIN OPTION").
			arg(userid));

      if(!query.lastError().isValid())
	{
	  querystr = QString("ALTER USER %1 CREATEUSER").arg(userid);
	  (void) query.exec(querystr);
	}

      goto done_label;
    }

  if(str.contains("membership"))
    {
      (void) query.exec(QString("GRANT %1 "
				"TO %2 WITH ADMIN OPTION").
			arg(str).arg(userid));

      if(!query.lastError().isValid())
	{
	  querystr = QString("ALTER USER %1 CREATEUSER").arg(userid);
	  (void) query.exec(querystr);
	}

      goto done_label;
    }

  (void) query.exec(QString("GRANT %1 "
			    "TO %2").arg(str).arg(userid));

 done_label:

  if(query.lastError().isValid())
    errorstr = query.lastError().text();
}

/*
** -- revokeAll() --
*/

void biblioteq_misc_functions::revokeAll(const QString &userid,
					 const QSqlDatabase &db,
					 QString &errorstr)
{
  errorstr = "";

  if(db.driverName() == "QSQLITE")
    return; // Users are not supported.

  QSqlQuery query(db);
  QString querystr = "";
  QStringList objectlist;
  qint64 count = 0;

  count = userCount(userid, db, errorstr);

  if(count > 0)
    {
      objectlist << "biblioteq_administrator"
		 << "biblioteq_circulation"
		 << "biblioteq_circulation_librarian"
		 << "biblioteq_circulation_librarian_membership"
		 << "biblioteq_circulation_membership"
		 << "biblioteq_librarian"
		 << "biblioteq_librarian_membership"
		 << "biblioteq_membership"
		 << "biblioteq_patron";

      while(!objectlist.isEmpty())
	{
	  querystr = QString("REVOKE %1 FROM %2").arg
	    (objectlist.takeFirst()).arg(userid);

	  if(!query.exec(querystr))
	    break;
	}

      objectlist.clear();

      if(query.lastError().isValid())
	errorstr = query.lastError().text();
      else
	{
	  querystr = QString("ALTER USER %1 NOCREATEUSER").arg(userid);
	  (void) query.exec(querystr);

	  if(query.lastError().isValid())
	    errorstr = query.lastError().text();
	}
    }
}

/*
** -- DBAccount() --
*/

void biblioteq_misc_functions::DBAccount(const QString &userid,
					 const QSqlDatabase &db,
					 const int action,
					 QString &errorstr,
					 const QString &roles)
{
  errorstr = "";

  if(db.driverName() == "QSQLITE")
    return; // Users are not supported.

  QSqlQuery query(db);
  QString querystr = "";
  QStringList objectlist;
  qint64 count = 0;

  if(action == CREATE_USER)
    {
      /*
      ** Does the user exist?
      */

      count = userCount(userid, db, errorstr);

      if(count == 0)
	{
	  QString str(roles);

	  if(str.contains("administrator") || str.contains("membership"))
	    querystr = QString
	      ("CREATE USER %1 ENCRYPTED PASSWORD 'tempPass' "
	       "CREATEUSER").arg(userid);
	  else
	    querystr = QString
	      ("CREATE USER %1 ENCRYPTED PASSWORD 'tempPass'").arg(userid);

	  (void) query.exec(querystr);

	  if(query.lastError().isValid())
	    errorstr = query.lastError().text();

	  if(!errorstr.isEmpty())
	    return;

	  if(str.isEmpty() || str == "none")
	    str = "biblioteq_patron";
	  else
	    {
	      str.replace(" ", "_");
	      str.prepend("biblioteq_");
	    }

	  if(str.contains("administrator"))
	    {
	      (void) query.exec(QString("GRANT biblioteq_administrator "
					"TO %1 WITH ADMIN OPTION").
				arg(userid));
	      goto done_label;
	    }

	  if(str.contains("membership"))
	    {
	      (void) query.exec(QString("GRANT %1 "
					"TO %2 WITH ADMIN OPTION").
				arg(str).arg(userid));
	      goto done_label;
	    }

	  (void) query.exec(QString("GRANT %1 "
				    "TO %2").arg(str).arg(userid));

	done_label:

	  if(query.lastError().isValid())
	    errorstr = query.lastError().text();

	  return;
	}
      else if(!errorstr.isEmpty())
	return;
    }

  if(action == UPDATE_USER)
    {
      QString str(roles);

      if(str.isEmpty() || str == "none")
	str = "biblioteq_patron";
      else
	{
	  str.replace(" ", "_");
	  str.prepend("biblioteq_");
	}

      objectlist << str;

      while(!objectlist.isEmpty())
	{
	  querystr = QString("REVOKE %1 FROM %2").arg
	    (objectlist.takeFirst()).arg(userid);

	  if(!query.exec(querystr))
	    break;
	}

      objectlist.clear();

      if(query.lastError().isValid())
	errorstr = query.lastError().text();

      if(errorstr.isEmpty())
	{
	  if(str.contains("administrator"))
	    {
	      (void) query.exec(QString("GRANT biblioteq_administrator "
					"TO %1 WITH ADMIN OPTION").
				arg(userid));

	      if(!query.lastError().isValid())
		{
		  querystr = QString("ALTER USER %1 CREATEUSER").arg(userid);
		  (void) query.exec(querystr);
		}
	    }
	  else if(str.contains("membership"))
	    {
	      (void) query.exec(QString("GRANT %1 "
					"TO %2 WITH ADMIN OPTION").
				arg(str).arg(userid));

	      if(!query.lastError().isValid())
		{
		  querystr = QString("ALTER USER %1 CREATEUSER").arg(userid);
		  (void) query.exec(querystr);
		}
	    }
	  else
	    (void) query.exec(QString("GRANT %1 "
				      "TO %2").arg(str).arg(userid));

	  if(query.lastError().isValid())
	    errorstr = query.lastError().text();
	}
    }

  if(action == DELETE_USER || action == UPDATE_USER)
    {
      if(action == DELETE_USER)
	{
	  (void) query.exec(QString("DROP USER IF EXISTS %1").arg(userid));

	  if(errorstr.isEmpty() && query.lastError().isValid())
	    errorstr = query.lastError().text();
	}
    }
}

/*
** -- savePassword() --
*/

void biblioteq_misc_functions::savePassword(const QString &userid,
					    const QSqlDatabase &db,
					    const QString &password,
					    QString &errorstr)
{
  errorstr = "";

  if(db.driverName() == "QSQLITE")
    return; // Users are not supported.

  QSqlQuery query(db);
  QString querystr = "";

  query.exec("SET ROLE NONE");
  querystr = QString("ALTER USER %1 WITH ENCRYPTED "
		     "PASSWORD '%2'").arg(userid).arg(password);

  if(!query.exec(querystr))
    errorstr = query.lastError().text();
}

/*
** -- getReservedItems() --
*/

QStringList biblioteq_misc_functions::getReservedItems(const QString &memberid,
						       const QSqlDatabase &db,
						       QString &errorstr)
{
  QSqlQuery query(db);
  QString querystr = "";
  QString str = "";
  QStringList list;

  errorstr = "";
  querystr =
    "SELECT "
    "item_borrower.copyid, "
    "book.location, "
    "book.type, "
    "book.title, "
    "item_borrower.duedate "
    "FROM "
    "book, "
    "item_borrower "
    "WHERE "
    "item_borrower.item_oid = book.myoid AND "
    "item_borrower.type = 'Book' AND "
    "item_borrower.memberid = ? "
    "UNION ALL "
    "SELECT "
    "item_borrower.copyid, "
    "cd.location, "
    "cd.type, "
    "cd.title, "
    "item_borrower.duedate "
    "FROM "
    "cd, "
    "item_borrower "
    "WHERE "
    "item_borrower.item_oid = cd.myoid AND "
    "item_borrower.type = 'CD' AND "
    "item_borrower.memberid = ? "
    "UNION ALL "
    "SELECT "
    "item_borrower.copyid, "
    "dvd.location, "
    "dvd.type, "
    "dvd.title, "
    "item_borrower.duedate "
    "FROM "
    "dvd, "
    "item_borrower "
    "WHERE "
    "item_borrower.item_oid = dvd.myoid AND "
    "item_borrower.type = 'DVD' AND "
    "item_borrower.memberid = ? "
    "UNION ALL "
    "SELECT "
    "item_borrower.copyid, "
    "journal.location, "
    "journal.type, "
    "journal.title, "
    "item_borrower.duedate "
    "FROM "
    "journal, "
    "item_borrower "
    "WHERE "
    "item_borrower.item_oid = journal.myoid AND "
    "item_borrower.type = 'Journal' AND "
    "item_borrower.memberid = ? "
    "UNION ALL "
    "SELECT "
    "item_borrower.copyid, "
    "magazine.location, "
    "magazine.type, "
    "magazine.title, "
    "item_borrower.duedate "
    "FROM "
    "magazine, "
    "item_borrower "
    "WHERE "
    "item_borrower.item_oid = magazine.myoid AND "
    "item_borrower.type = 'Magazine' AND "
    "item_borrower.memberid = ? "
    "UNION ALL "
    "SELECT "
    "item_borrower.copyid, "
    "videogame.location, "
    "videogame.type, "
    "videogame.title, "
    "item_borrower.duedate "
    "FROM "
    "videogame, "
    "item_borrower "
    "WHERE "
    "item_borrower.item_oid = videogame.myoid AND "
    "item_borrower.type = 'Video Game' AND "
    "item_borrower.memberid = ? ";
  query.setForwardOnly(true);
  query.prepare(querystr);
  query.bindValue(0, memberid);
  query.bindValue(1, memberid);
  query.bindValue(2, memberid);
  query.bindValue(3, memberid);
  query.bindValue(4, memberid);
  query.bindValue(5, memberid);

  QDate date;

  if(query.exec())
    while(query.next())
      {
	date = QDate::fromString(query.value(4).toString(),
				 "MM/dd/yyyy");
	str = QString(QObject::tr("#")) +
	  QString::number(list.size() + 1) + "<br>";
	str += QObject::tr("Barcode: ") +
	  query.value(0).toString() + "<br>" +
	  QObject::tr(" Location: ") +
	  query.value(1).toString() + "<br>" +
	  QObject::tr("Type: ") + query.value(2).toString() + "<br>" +
	  QObject::tr("Title: ") + query.value(3).toString() + "<br>" +
	  QObject::tr("Due Date: ") +
	  date.toString(Qt::ISODate);
	list.append(str);
      }

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return list;
}

/*
** -- getMemberMatchCount() --
*/

qint64 biblioteq_misc_functions::getMemberMatchCount(const QString &checksum,
						     const QString &memberid,
						     const QSqlDatabase &db,
						     QString &errorstr)
{
  QSqlQuery query(db);
  QString querystr = "";
  qint64 count = 0;

  errorstr = "";
  querystr = "SELECT COUNT(memberid) FROM member "
    "WHERE "
    "dob || sex || first_name || "
    "middle_init || "
    "last_name || street || city || state_abbr || zip = ? "
    "AND memberid != ?";
  query.setForwardOnly(true);
  query.prepare(querystr);
  query.bindValue(0, checksum);
  query.bindValue(1, memberid);

  if(query.exec())
    if(query.next())
      count = query.value(0).toLongLong();

  if(query.lastError().isValid())
    {
      count = -1;
      errorstr = query.lastError().text();
    }

  return count;
}

/*
** -- getAvailability() --
*/

QString biblioteq_misc_functions::getAvailability(const QString &oid,
						  const QSqlDatabase &db,
						  const QString &itemTypeArg,
						  QString &errorstr)
{
  QSqlQuery query(db);
  QString itemType = "";
  QString querystr = "";
  QString str = "";

  errorstr = "";
  itemType = itemTypeArg;

  if(itemType.toLower() == "book" || itemType.toLower() == "cd" ||
     itemType.toLower() == "dvd" || itemType.toLower() == "journal" ||
     itemType.toLower() == "magazine" || itemType.toLower() == "video game")
    querystr = QString("SELECT %1.quantity - "
		       "COUNT(item_borrower_vw.item_oid) "
		       "FROM "
		       "%1 LEFT JOIN item_borrower_vw ON "
		       "%1.myoid = item_borrower_vw.item_oid AND "
		       "item_borrower_vw.type = '%2' "
		       "WHERE "
		       "%1.myoid = ? "
		       "GROUP BY %1.quantity, "
		       "%1.myoid").arg(itemType.toLower().remove(" ")).arg
      (itemType);

  query.setForwardOnly(true);
  query.prepare(querystr);
  query.bindValue(0, oid);

  if(query.exec())
    if(query.next())
      str = query.value(0).toString();

  if(query.lastError().isValid())
    {
      str = "0";
      errorstr = query.lastError().text();
    }
  else if(str.isEmpty())
    {
      str = "0";
      errorstr = QObject::tr("NULL availability value.");
    }

  return str;
}

/*
** -- getColumnString() --
*/

QString biblioteq_misc_functions::getColumnString(const QTableWidget *table,
						  const int row,
						  const int column)
{
  if(column < 0 || row < 0 || !table)
    return QString("");

  QTableWidgetItem *item = 0;

  if((item = table->item(row, column)))
    return item->text();
  else
    return QString("");
}

/*
** -- getColumnString() --
*/

QString biblioteq_misc_functions::getColumnString(const QTableWidget *table,
						  const int row,
						  const QString &columnName)
{
  if(columnName.isEmpty() || row < 0 || !table)
    return QString("");

  QString str = "";
  QTableWidgetItem *column = 0;
  int i = 0;

  if(row >= 0 && row < table->rowCount())
    for(i = 0; i < table->columnCount(); i++)
      {
	column = table->horizontalHeaderItem(i);

	if(column == 0 || table->item(row, i) == 0)
	  continue;

	if(column->text().toLower() == columnName.toLower())
	  {
	    str = table->item(row, i)->text();
	    break;
	  }
      }

  return str;
}

/*
** -- getColumnNumber() --
*/

int biblioteq_misc_functions::getColumnNumber(const QTableWidget *table,
					      const QString &columnName)
{
  if(columnName.isEmpty() || !table)
    return -1;

  QTableWidgetItem *column = 0;
  int i = 0;
  int num = -1;

  for(i = 0; i < table->columnCount(); i++)
    {
      column = table->horizontalHeaderItem(i);

      if(column == 0)
	continue;

      if(column->text().toLower() == columnName.toLower())
	{
	  num = i;
	  break;
	}
    }

  return num;
}

/*
** -- updateColumn() --
*/

void biblioteq_misc_functions::updateColumn(QTableWidget *table,
					    const int row, int column,
					    const QString &value)
{
  if(column < 0 || row < 0 || !table || !table->item(row, column))
    return;

  bool sortingEnabled = false;

  if(table->isSortingEnabled())
    sortingEnabled = true;

  if(sortingEnabled)
    table->setSortingEnabled(false);

  table->item(row, column)->setText(value);

  if(sortingEnabled)
    table->setSortingEnabled(true);
}

/*
** -- isCheckedOut() --
*/

bool biblioteq_misc_functions::isCheckedOut(const QSqlDatabase &db,
					    const QString &oid,
					    const QString &itemTypeArg,
					    QString &errorstr)
{
  QSqlQuery query(db);
  QString itemType = "";
  QString str = "";
  bool isCheckedOut = false;

  errorstr = "";
  itemType = itemTypeArg;
  query.setForwardOnly(true);
  query.prepare("SELECT COUNT(myoid) FROM item_borrower_vw "
		"WHERE item_oid = ? AND type = ?");
  query.bindValue(0, oid);
  query.bindValue(1, itemType);

  if(query.exec())
    {
      if(query.next())
	str = query.value(0).toString();

      if(str == "0")
	isCheckedOut = false;
      else
	isCheckedOut = true;
    }

  if(query.lastError().isValid())
    {
      errorstr = query.lastError().text();
      isCheckedOut = false;
    }

  return isCheckedOut;
}

/*
** -- isCopyCheckedOut() --
*/

bool biblioteq_misc_functions::isCopyCheckedOut(const QSqlDatabase &db,
						const QString &copyid,
						const QString &oid,
						const QString &itemTypeArg,
						QString &errorstr)
{
  QSqlQuery query(db);
  QString itemType = "";
  QString str = "";
  bool isCheckedOut = false;

  errorstr = "";
  itemType = itemTypeArg;
  query.setForwardOnly(true);
  query.prepare("SELECT count(copyid) FROM item_borrower_vw WHERE "
		"copyid = ? AND item_oid = ? AND "
		"type = ?");
  query.bindValue(0, copyid);
  query.bindValue(1, oid);
  query.bindValue(2, itemType);

  if(query.exec())
    {
      if(query.next())
	str = query.value(0).toString();

      if(str == "0")
	isCheckedOut = false;
      else
	isCheckedOut = true;
    }

  if(query.lastError().isValid())
    {
      errorstr = query.lastError().text();
      isCheckedOut = false;
    }

  return isCheckedOut;
}

/*
** -- saveQuantity() --
*/

void biblioteq_misc_functions::saveQuantity(const QSqlDatabase &db,
					    const QString &oid,
					    const int quantity,
					    const QString &itemTypeArg,
					    QString &errorstr)
{
  QSqlQuery query(db);
  QString itemType = "";
  QString querystr = "";

  errorstr = "";
  itemType = itemTypeArg.toLower().remove(" ");

  if(itemType == "book" || itemType == "cd" || itemType == "dvd" ||
     itemType == "journal" || itemType == "magazine" ||
     itemType == "videogame")
    querystr = QString("UPDATE %1 SET quantity = ? WHERE "
		       "myoid = ?").arg(itemType);

  query.prepare(querystr);
  query.bindValue(0, quantity);
  query.bindValue(1, oid);
  (void) query.exec();

  if(query.lastError().isValid())
    errorstr = query.lastError().text();
}

/*
** -- getMaxCopyNumber() --
*/

int biblioteq_misc_functions::getMaxCopyNumber(const QSqlDatabase &db,
					       const QString &oid,
					       const QString &itemTypeArg,
					       QString &errorstr)
{
  QSqlQuery query(db);
  QString itemType = "";
  int copy_number = -1;

  errorstr = "";
  itemType = itemTypeArg;
  query.setForwardOnly(true);
  query.prepare("SELECT MAX(copy_number) FROM item_borrower_vw "
		"WHERE item_oid = ? AND type = ?");
  query.bindValue(0, oid);
  query.bindValue(1, itemType);

  if(query.exec())
    if(query.next())
      copy_number = query.value(0).toInt();

  if(query.lastError().isValid())
    {
      copy_number = -1;
      errorstr = query.lastError().text();
    }

  return copy_number;
}

/*
** -- isCopyAvailable() --
*/

bool biblioteq_misc_functions::isCopyAvailable(const QSqlDatabase &db,
					       const QString &oid,
					       const QString &copyid,
					       const QString &itemTypeArg,
					       QString &errorstr)
{
  QSqlQuery query(db);
  QString itemType = "";
  QString querystr = "";
  bool isAvailable = false;

  errorstr = "";
  itemType = itemTypeArg;

  if(itemType.toLower() == "book" || itemType.toLower() == "cd" ||
     itemType.toLower() == "dvd" || itemType.toLower() == "journal" ||
     itemType.toLower() == "magazine" || itemType.toLower() == "video game")
    querystr = QString("SELECT COUNT(myoid) FROM %1_copy_info "
		       "WHERE copyid = ? AND item_oid = ? "
		       "AND copyid NOT IN (SELECT copyid FROM item_borrower_vw "
		       "WHERE item_oid = ? AND type = '%2')").arg
      (itemType.toLower().remove(" ")).arg(itemType);

  query.setForwardOnly(true);
  query.prepare(querystr);
  query.bindValue(0, copyid);
  query.bindValue(1, oid);
  query.bindValue(2, oid);

  if(query.exec())
    if(query.next())
      if(query.value(0).toLongLong() >= 1)
	isAvailable = true;

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return isAvailable;
}

/*
** -- getItemsReservedCounts() --
*/

QMap<QString, QString> biblioteq_misc_functions::getItemsReservedCounts
(const QSqlDatabase &db,
 const QString &memberid,
 QString &errorstr)
{
  QMap<QString, QString> counts;
  QSqlQuery query(db);
  QString querystr = "";
  QString str = "";

  errorstr = "";
  querystr =
    "SELECT COUNT(myoid) AS numbooks FROM item_borrower_vw WHERE memberid = ? "
    "AND type = 'Book' "
    "UNION ALL "
    "SELECT COUNT(myoid) AS numcds FROM item_borrower_vw WHERE memberid = ? "
    "AND type = 'CD' "
    "UNION ALL "
    "SELECT COUNT(myoid) AS numdvds FROM item_borrower_vw WHERE memberid = ? "
    "AND type = 'DVD' "
    "UNION ALL "
    "SELECT COUNT(myoid) AS numjournals FROM item_borrower_vw WHERE "
    "memberid = ? AND type = 'Journal' "
    "UNION ALL "
    "SELECT COUNT(myoid) AS nummagazines FROM item_borrower_vw WHERE "
    "memberid = ? AND type = 'Magazine' "
    "UNION ALL "
    "SELECT COUNT(myoid) AS numvideogames FROM item_borrower_vw WHERE "
    "memberid = ? AND type = 'Video Game'";
  query.setForwardOnly(true);
  query.prepare(querystr);
  query.bindValue(0, memberid);
  query.bindValue(1, memberid);
  query.bindValue(2, memberid);
  query.bindValue(3, memberid);
  query.bindValue(4, memberid);
  query.bindValue(5, memberid);

  if(query.exec())
    while(query.next())
      {
	str = query.value(0).toString();

	if(str == "0")
	  str = "";

	if(counts.size() == 0)
	  counts["numbooks"] = str;
	else if(counts.size() == 1)
	  counts["numcds"] = str;
	else if(counts.size() == 2)
	  counts["numdvds"] = str;
	else if(counts.size() == 3)
	  counts["numjournals"] = str;
	else if(counts.size() == 4)
	  counts["nummagazines"] = str;
	else
	  counts["numvideogames"] = str;
      }

  if(query.lastError().isValid())
    errorstr = query.lastError().text();
  else if(counts.isEmpty())
    {
      counts["numbooks"] = "";
      counts["numcds"] = "";
      counts["numdvds"] = "";
      counts["numjournals"] = "";
      counts["nummagazines"] = "";
      counts["numvideogames"] = "";
    }

  return counts;
}

/*
** -- getRoles() --
*/

QString biblioteq_misc_functions::getRoles(const QSqlDatabase &db,
					   const QString &username,
					   QString &errorstr)
{
  QSqlQuery query(db);
  QString querystr = "";
  QString roles = "";

  errorstr = "";
  querystr = "SELECT LOWER(roles) FROM admin WHERE "
    "LOWER(username) = LOWER(?)";
  query.setForwardOnly(true);
  query.prepare(querystr);
  query.bindValue(0, username);

  if(query.exec())
    if(query.next())
      {
	roles = query.value(0).toString();

	if(roles == "none")
	  roles = "";
      }

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return roles;
}

/*
** -- getOID() --
*/

QString biblioteq_misc_functions::getOID(const QString &idArg,
					 const QString &itemTypeArg,
					 const QSqlDatabase &db,
					 QString &errorstr)
{
  QSqlQuery query(db);
  QString id = "";
  QString itemType = "";
  QString oid = "";
  QString querystr = "";
  int i = 0;

  id = idArg;
  itemType = itemTypeArg.toLower();

  if(itemType == "photograph collection")
    itemType = itemType.replace(" ", "_");
  else
    itemType = itemType.remove(" ");

  if(itemType == "journal" || itemType == "magazine")
    querystr = QString("SELECT myoid FROM %1 WHERE id = ? AND "
		       "issuevolume = ? AND issueno = ? AND "
		       "id IS NOT NULL").arg(itemType);
  else if(itemType == "book")
    querystr = QString("SELECT myoid FROM %1 WHERE id = ? AND "
		       "id IS NOT NULL").arg(itemType);
  else if(itemType == "cd" || itemType == "dvd" ||
	  itemType == "photograph_collection" || itemType == "videogame")
    querystr = QString("SELECT myoid FROM %1 WHERE id = ?").arg(itemType);

  query.setForwardOnly(true);
  query.prepare(querystr);

  if(itemType == "journal" || itemType == "magazine")
    {
      QStringList list = id.split(",");

      for(i = 0; i < list.size(); i++)
	query.bindValue(i, list[i]);
    }
  else
    query.bindValue(0, id);

  if(query.exec())
    if(query.next())
      oid = query.value(0).toString();

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return oid;
}

/*
** -- createInitialCopies() --
*/

void biblioteq_misc_functions::createInitialCopies(const QString &idArg,
						   const int numCopies,
						   const QSqlDatabase &db,
						   const QString &itemTypeArg,
						   QString &errorstr)
{
  QSqlQuery query(db);
  QString id = "";
  QString itemType = "";
  QString itemoid = "";
  int i = 0;

  /*
  ** Retrieve the item's OID. Use the OID to create initial copies.
  */

  errorstr = "";
  id = idArg;
  itemType = itemTypeArg.toLower().remove(" ");
  itemoid = getOID(id, itemType, db, errorstr);

  if(!errorstr.isEmpty())
    return;

  if(itemType == "journal" || itemType == "magazine")
    {
      if(itemoid.isEmpty())
	/*
	** If the id from getOID() is empty, createInitialCopies() was called
	** with an oid.
	*/

	id = itemoid = id.split(",").value(0);
      else
	id = id.split(",").value(0);
    }
  else if(itemType == "book")
    {
      if(itemoid.isEmpty())
	/*
	** If the id from getOID() is empty, createInitialCopies() was called
	** with an oid.
	*/

	itemoid = id;
    }

  if(!itemoid.isEmpty())
    for(i = 0; i < numCopies; i++)
      {
	if(db.driverName() != "QSQLITE")
	  {
	    if(itemType == "book" || itemType == "cd" ||
	       itemType == "dvd" || itemType == "journal" ||
	       itemType == "magazine" || itemType == "videogame")
	      query.prepare(QString("INSERT INTO %1_copy_info "
				    "(item_oid, copy_number, "
				    "copyid) "
				    "VALUES (?, "
				    "?, ?)").arg(itemType));
	  }
	else
	  {
	    if(itemType == "book" || itemType == "cd" ||
	       itemType == "dvd" || itemType == "journal" ||
	       itemType == "magazine" || itemType == "videogame")
	      query.prepare(QString("INSERT INTO %1_copy_info "
				    "(item_oid, copy_number, "
				    "copyid, myoid) "
				    "VALUES (?, "
				    "?, ?, ?)").arg(itemType));
	  }

	query.bindValue(0, itemoid);
	query.bindValue(1, i + 1);
	query.bindValue(2, id + "-" + QString::number(i + 1));

	if(db.driverName() == "QSQLITE")
	  {
	    qint64 value = getSqliteUniqueId(db, errorstr);

	    if(errorstr.isEmpty())
	      query.bindValue(3, value);
	    else
	      break;
	  }

	if(!query.exec())
	  {
	    errorstr = query.lastError().text();
	    break;
	  }
      }
}

/*
** -- getMemberName() --
*/

QString biblioteq_misc_functions::getMemberName(const QSqlDatabase &db,
						const QString &memberid,
						QString &errorstr)
{
  QSqlQuery query(db);
  QString querystr = "";
  QString str = "";

  errorstr = "";
  querystr = "SELECT last_name, first_name FROM member WHERE memberid = ?";
  query.setForwardOnly(true);
  query.prepare(querystr);
  query.bindValue(0, memberid);

  if(query.exec())
    if(query.next())
      str = query.value(0).toString() + ", " + query.value(1).toString();

  if(query.lastError().isValid())
    {
      errorstr = query.lastError().text();
      str = "Unknown";
    }

  return str;
}

/*
** -- highlightWidget() --
*/

void biblioteq_misc_functions::highlightWidget(QWidget *widget,
					       const QColor &color)
{
  if(!widget)
    return;

  QPalette pal;

  pal = widget->palette();
  pal.setColor(widget->backgroundRole(), color);
  widget->setPalette(pal);
}

/*
** -- sqliteQuerySize() --
*/

int biblioteq_misc_functions::sqliteQuerySize(const QString &querystr,
					      const QSqlDatabase &db,
					      const char *file,
					      const int line)
{
  int count = 0;

  if(db.driverName() != "QSQLITE")
    return count; // SQLite only.

  QSqlQuery query(db);

  if(query.exec(querystr))
    while(query.next())
      count += 1;

  if(query.lastError().isValid())
    qmain->addError
      (QString(QObject::tr("Database Error")),
       QString(QObject::tr("Unable to determine the query size.")),
       query.lastError().text(), file, line);

  return count;
}

/*
** -- sqliteQuerySize() --
*/

int biblioteq_misc_functions::sqliteQuerySize
(const QString &querystr,
 const QMap<QString, QVariant> &boundValues,
 const QSqlDatabase &db,
 const char *file, const int line)
{
  int count = 0;

  if(db.driverName() != "QSQLITE")
    return count; // SQLite only.

  QList<QVariant> list = boundValues.values();
  QSqlQuery query(db);

  query.prepare(querystr);

  for(int i = 0; i < list.size(); i++)
    query.bindValue(i, list.at(i));

  if(query.exec())
    while(query.next())
      count += 1;

  if(query.lastError().isValid())
    qmain->addError
      (QString(QObject::tr("Database Error")),
       QString(QObject::tr("Unable to determine the query size.")),
       query.lastError().text(), file, line);

  return count;
}

/*
** -- center() --
*/

void biblioteq_misc_functions::center(QWidget *child, QMainWindow *parent)
{
  if(!child || !parent)
    return;

  QPoint p(0, 0);
  int X = 0;
  int Y = 0;

  p = parent->pos();

  if(parent->width() >= child->width())
    X = p.x() + (parent->width() - child->width()) / 2;
  else
    X = p.x() - (child->width() - parent->width()) / 2;

  if(parent->height() >= child->height())
    Y = p.y() + (parent->height() - child->height()) / 2;
  else
    Y = p.y() - (child->height() - parent->height()) / 2;

  child->move(X, Y);
}

/*
** -- hideAdminFields() --
*/

void biblioteq_misc_functions::hideAdminFields(QMainWindow *window,
					       const QString &roles)
{
  if(!window)
    return;

  QString str = "";
  bool showWidgets = true;

  if(roles.isEmpty())
    showWidgets = false;
  else if(roles.contains("administrator") ||
	  roles.contains("librarian"))
    showWidgets = true;
  else
    showWidgets = false;

  foreach(QWidget *widget, window->findChildren<QWidget *>())
    {
      str = widget->objectName().toLower();

      if(str.contains("price") || str.contains("monetary"))
	widget->setVisible(showWidgets);
    }

  foreach(QLabel *widget, window->findChildren<QLabel *>())
    {
      str = widget->text().toLower();

      if(str.contains("price") || str.contains("monetary"))
	widget->setVisible(showWidgets);
    }

  foreach(QToolButton *button, window->findChildren<QToolButton *>())
    if(button->menu())
      foreach(QAction *action, button->menu()->findChildren<QAction *>())
	{
	  str = action->text().toLower();

	  if(str.contains("price") || str.contains("monetary"))
	    action->setVisible(showWidgets);
	}
}

/*
** -- isRequested() --
*/

bool biblioteq_misc_functions::isRequested(const QSqlDatabase &db,
					   const QString &oid,
					   const QString &itemTypeArg,
					   QString &errorstr)
{
  bool isRequested = false;

  errorstr = "";

  if(db.driverName() == "QSQLITE")
    return isRequested; // Requests are not supported.

  QSqlQuery query(db);
  QString itemType = "";
  QString str = "";

  itemType = itemTypeArg;
  query.setForwardOnly(true);
  query.prepare("SELECT COUNT(myoid) FROM item_request "
		"WHERE item_oid = ? AND type = ?");
  query.bindValue(0, oid);
  query.bindValue(1, itemType);

  if(query.exec())
    {
      if(query.next())
	str = query.value(0).toString();

      if(str == "0")
	isRequested = false;
      else
	isRequested = true;
    }

  if(query.lastError().isValid())
    {
      errorstr = query.lastError().text();
      isRequested = false;
    }

  return isRequested;
}

/*
** -- getLocations() --
*/

QStringList biblioteq_misc_functions::getLocations(const QSqlDatabase &db,
						   const QString &type,
						   QString &errorstr)
{
  QSqlQuery query(db);
  QStringList locations;

  errorstr = "";
  query.setForwardOnly(true);

  if(type.isEmpty())
    query.prepare("SELECT DISTINCT(location) FROM locations "
		  "WHERE LENGTH(TRIM(location)) > 0 "
		  "ORDER BY location");
  else
    {
      query.prepare("SELECT location FROM locations WHERE type = ? AND "
		    "LENGTH(TRIM(location)) > 0 "
		    "ORDER BY location");
      query.bindValue(0, type);
    }

  if(query.exec())
    while(query.next())
      locations.append(query.value(0).toString());

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return locations;
}

/*
** -- getLocations() --
*/

QList<QPair<QString, QString> > biblioteq_misc_functions::getLocations
(const QSqlDatabase &db,
 QString &errorstr)
{
  QList<QPair<QString, QString> > locations;
  QSqlQuery query(db);
  QString querystr("");

  errorstr = "";
  querystr = "SELECT type, location FROM locations "
    "WHERE LENGTH(TRIM(type)) > 0 AND "
    "LENGTH(TRIM(location)) > 0 "
    "ORDER BY type, location";
  query.setForwardOnly(true);

  if(query.exec(querystr))
    while(query.next())
      locations.append(qMakePair(query.value(0).toString(),
				 query.value(1).toString()));

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return locations;
}

/*
** -- getMonetaryUnits() --
*/

QStringList biblioteq_misc_functions::getMonetaryUnits(const QSqlDatabase &db,
						       QString &errorstr)
{
  QSqlQuery query(db);
  QString querystr("");
  QStringList monetary_units;

  errorstr = "";
  querystr = "SELECT monetary_unit FROM monetary_units "
    "WHERE LENGTH(TRIM(monetary_unit)) > 0 "
    "ORDER BY monetary_unit";
  query.setForwardOnly(true);

  if(query.exec(querystr))
    while(query.next())
      monetary_units.append(query.value(0).toString());

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return monetary_units;
}

/*
** -- getLanguages() --
*/

QStringList biblioteq_misc_functions::getLanguages(const QSqlDatabase &db,
						   QString &errorstr)
{
  QSqlQuery query(db);
  QString querystr("");
  QStringList languages;

  errorstr = "";
  querystr = "SELECT language FROM languages "
    "WHERE LENGTH(TRIM(language)) > 0 "
    "ORDER BY language";
  query.setForwardOnly(true);

  if(query.exec(querystr))
    while(query.next())
      languages.append(query.value(0).toString());

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return languages;
}

/*
** -- getCDFormats() --
*/

QStringList biblioteq_misc_functions::getCDFormats(const QSqlDatabase &db,
						   QString &errorstr)
{
  QSqlQuery query(db);
  QString querystr("");
  QStringList formats;

  errorstr = "";
  querystr = "SELECT cd_format FROM cd_formats "
    "WHERE LENGTH(TRIM(cd_format)) > 0 "
    "ORDER BY cd_format";
  query.setForwardOnly(true);

  if(query.exec(querystr))
    while(query.next())
      formats.append(query.value(0).toString());

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return formats;
}

/*
** -- get DVDRatings() --
*/

QStringList biblioteq_misc_functions::getDVDRatings(const QSqlDatabase &db,
						    QString &errorstr)
{
  QSqlQuery query(db);
  QString querystr("");
  QStringList ratings;

  errorstr = "";
  querystr = "SELECT dvd_rating FROM dvd_ratings "
    "WHERE LENGTH(TRIM(dvd_rating)) > 0 "
    "ORDER BY dvd_rating";
  query.setForwardOnly(true);

  if(query.exec(querystr))
    while(query.next())
      ratings.append(query.value(0).toString());

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return ratings;
}

/*
** -- getDVDAspectRatios() --
*/

QStringList biblioteq_misc_functions::getDVDAspectRatios
(const QSqlDatabase &db, QString &errorstr)
{
  QSqlQuery query(db);
  QString querystr("");
  QStringList aspectratios;

  errorstr = "";
  querystr = "SELECT dvd_aspect_ratio FROM dvd_aspect_ratios "
    "WHERE LENGTH(TRIM(dvd_aspect_ratio)) > 0 "
    "ORDER BY dvd_aspect_ratio";
  query.setForwardOnly(true);

  if(query.exec(querystr))
    while(query.next())
      aspectratios.append(query.value(0).toString());

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return aspectratios;
}

/*
** -- getDVDRegions() --
*/

QStringList biblioteq_misc_functions::getDVDRegions(const QSqlDatabase &db,
						    QString &errorstr)
{
  QSqlQuery query(db);
  QString querystr("");
  QStringList regions;

  errorstr = "";
  querystr = "SELECT dvd_region FROM dvd_regions "
    "WHERE LENGTH(TRIM(dvd_region)) > 0 "
    "ORDER BY dvd_region";
  query.setForwardOnly(true);

  if(query.exec(querystr))
    while(query.next())
      regions.append(query.value(0).toString());

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return regions;
}

/*
** -- getMinimumDays() --
*/

int biblioteq_misc_functions::getMinimumDays(const QSqlDatabase &db,
					     const QString &type,
					     QString &errorstr)
{
  QSqlQuery query(db);
  int minimumdays = 1;

  errorstr = "";
  query.setForwardOnly(true);
  query.prepare("SELECT days FROM minimum_days WHERE type = ?");
  query.bindValue(0, type);

  if(query.exec())
    if(query.next())
      minimumdays = query.value(0).toInt();

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return minimumdays;
}

/*
** -- getMinimumDays() --
*/

QStringList biblioteq_misc_functions::getMinimumDays(const QSqlDatabase &db,
						     QString &errorstr)
{
  QMap<QString, QString> map;
  QSqlQuery query(db);
  QString querystr("");
  QStringList minimumdays;

  errorstr = "";
  querystr = "SELECT type, days FROM minimum_days "
    "WHERE LENGTH(TRIM(type)) > 0 "
    "ORDER BY type";
  query.setForwardOnly(true);

  if(query.exec(querystr))
    while(query.next())
      {
	if(query.value(0).toString() == "CD")
	  map["Music CD"] = query.value(1).toString();
	else
	  map[query.value(0).toString()] = query.value(1).toString();
      }

  if(!map.contains("Book"))
    map["Book"] = "1";

  if(!map.contains("DVD"))
    map["DVD"] = "1";

  if(!map.contains("Journal"))
    map["Journal"] = "1";

  if(!map.contains("Magazine"))
    map["Magazine"] = "1";

  if(!map.contains("Music CD"))
    map["Music CD"] = "1";

  if(!map.contains("Video Game"))
    map["Video Game"] = "1";

  for(int i = 0; i < map.values().size(); i++)
    minimumdays.append(map.values().at(i));

  map.clear();
  return minimumdays;
}

/*
** -- getVideoGameRatings() --
*/

QStringList biblioteq_misc_functions::getVideoGameRatings
(const QSqlDatabase &db, QString &errorstr)
{
  QSqlQuery query(db);
  QString querystr("");
  QStringList ratings;

  errorstr = "";
  querystr = "SELECT videogame_rating FROM videogame_ratings "
    "WHERE LENGTH(TRIM(videogame_rating)) > 0 "
    "ORDER BY videogame_rating";
  query.setForwardOnly(true);

  if(query.exec(querystr))
    while(query.next())
      ratings.append(query.value(0).toString());

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return ratings;
}

/*
** -- getVideoGamePlatforms() --
*/

QStringList biblioteq_misc_functions::getVideoGamePlatforms
(const QSqlDatabase &db, QString &errorstr)
{
  QSqlQuery query(db);
  QString querystr("");
  QStringList platforms;

  errorstr = "";
  querystr = "SELECT videogame_platform FROM videogame_platforms "
    "WHERE LENGTH(TRIM(videogame_platform)) > 0 "
    "ORDER BY videogame_platform";
  query.setForwardOnly(true);

  if(query.exec(querystr))
    while(query.next())
      platforms.append(query.value(0).toString());

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return platforms;
}

/*
** -- getSqliteUniqueId() --
*/

qint64 biblioteq_misc_functions::getSqliteUniqueId(const QSqlDatabase &db,
						   QString &errorstr)
{
  qint64 value = -1;

  if(db.driverName() != "QSQLITE")
    return value;

  QSqlQuery query(db);
  QString querystr("");

  errorstr = "";
  querystr = "INSERT INTO sequence VALUES (NULL)";
  query.setForwardOnly(true);

  if(query.exec(querystr))
    {
      querystr = "SELECT MAX(value) FROM sequence";

      if(query.exec(querystr))
	if(query.next())
	  value = query.value(0).toLongLong();
    }

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return value;
}

/*
** -- hasMemberExpired() --
*/

bool biblioteq_misc_functions::hasMemberExpired(const QSqlDatabase &db,
						const QString &memberid,
						QString &errorstr)
{
  QSqlQuery query(db);
  bool expired = true;

  errorstr = "";
  query.setForwardOnly(true);

  if(db.driverName() == "QSQLITE")
    query.prepare("SELECT expiration_date "
		  "FROM member WHERE memberid = ?");
  else
    query.prepare("SELECT TO_DATE(expiration_date, 'mm/dd/yyyy') - "
		  "current_date FROM member WHERE memberid = ?");

  query.bindValue(0, memberid);

  if(query.exec())
    if(query.next())
      {
	if(db.driverName() == "QSQLITE")
	  {
	    QDate date(QDate::fromString(query.value(0).toString(),
					 "MM/dd/yyyy"));

	    if(date.daysTo(QDate::currentDate()) > 0)
	      expired = true;
	    else
	      expired = false;
	  }
	else
	  {
	    if(query.value(0).toLongLong() < 0)
	      expired = true;
	    else
	      expired = false;
	  }
      }

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return expired;
}

/*
** -- updateSQLiteDatabase() --
*/

void biblioteq_misc_functions::updateSQLiteDatabase(const QSqlDatabase &db)
{
  if(db.driverName() != "QSQLITE")
    return;

  QString version(BIBLIOTEQ_VERSION);

  if(version >= "6.63")
    {
      QSqlQuery query(db);

      query.exec("ALTER TABLE cd_songs ADD artist TEXT "
		 "NOT NULL DEFAULT 'UNKNOWN'");
      query.exec("ALTER TABLE cd_songs ADD composer TEXT "
		 "NOT NULL DEFAULT 'UNKNOWN'");
    }

  if(version >= "6.66")
    {
      QSqlQuery query(db);

      query.exec("ALTER TABLE book ADD condition TEXT");
      query.exec("ALTER TABLE book ADD originality TEXT");
      query.exec("ALTER TABLE book_copy_info ADD condition TEXT");
      query.exec("ALTER TABLE book_copy_info ADD originality TEXT");
      query.exec("CREATE TABLE book_binding_types "
		 "( "
		 "binding_type	TEXT NOT NULL PRIMARY KEY "
		 ")");
    }

  if(version >= "6.70")
    {
      QSqlField field;
      QSqlRecord record(db.record("member"));

      field = record.field("zip");

      if(field.defaultValue() != "'N/A'")
	{
	  QSqlQuery query(db);

	  query.exec
	    ("CREATE TABLE member_temporary "
	     "("
	     "memberid	 VARCHAR(16) NOT NULL PRIMARY KEY DEFAULT 1,"
	     "membersince	 VARCHAR(32) NOT NULL,"
	     "dob		 VARCHAR(32) NOT NULL,"
	     "sex		 VARCHAR(32) NOT NULL DEFAULT 'Private',"
	     "first_name	 VARCHAR(128) NOT NULL,"
	     "middle_init	 VARCHAR(1),"
	     "last_name	 VARCHAR(128) NOT NULL,"
	     "telephone_num	 VARCHAR(32),"
	     "street		 VARCHAR(256) NOT NULL,"
	     "city		 VARCHAR(256) NOT NULL,"
	     "state_abbr	 VARCHAR(16) NOT NULL DEFAULT 'N/A',"
	     "zip		 VARCHAR(16) NOT NULL DEFAULT 'N/A',"
	     "email		 VARCHAR(128),"
	     "expiration_date	 VARCHAR(32) NOT NULL,"
	     "overdue_fees	 NUMERIC(10, 2) NOT NULL DEFAULT 0.00,"
	     "comments	 TEXT,"
	     "general_registration_number	 TEXT,"
	     "memberclass	 TEXT"
	     ")");
	  query.exec("INSERT INTO member_temporary SELECT * FROM member");
	  query.exec("DROP TABLE member");
	  query.exec("ALTER TABLE member_temporary RENAME TO member");
	}
    }

  if(version >= "6.71")
    {
      QSqlField field;
      QSqlRecord record(db.record("journal"));

      field = record.field("id");

      if(field.requiredStatus() == QSqlField::Required)
	{
	  QSqlQuery query(db);

	  query.exec
	    ("CREATE TABLE journal_temporary "
	     "( "
	     "id		 VARCHAR(32), "
	     "myoid		 BIGINT NOT NULL, "
	     "title		 TEXT NOT NULL,	"
	     "pdate		 VARCHAR(32) NOT NULL, "
	     "publisher	 TEXT NOT NULL, "
	     "place		 TEXT NOT NULL, "
	     "category	 TEXT NOT NULL, "
	     "price		 NUMERIC(10, 2) NOT NULL DEFAULT 0.00, "
	     "description	 TEXT NOT NULL, "
	     "language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN', "
	     "monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN', "
	     "quantity	 INTEGER NOT NULL DEFAULT 1, "
	     "location	 TEXT NOT NULL, "
	     "issuevolume	 INTEGER NOT NULL DEFAULT 0, "
	     "issueno		 INTEGER NOT NULL DEFAULT 0, "
	     "lccontrolnumber	 VARCHAR(64), "
	     "callnumber	 VARCHAR(64), "
	     "deweynumber	 VARCHAR(64), "
	     "front_cover	 BYTEA, "
	     "back_cover	 BYTEA, "
	     "marc_tags      TEXT, "
	     "keyword        TEXT, "
	     "type		 VARCHAR(16) NOT NULL DEFAULT 'Journal', "
	     "UNIQUE (id, issueno, issuevolume) "
	     ");");
	  query.exec("INSERT INTO journal_temporary SELECT * FROM journal");
	  query.exec("DROP TABLE journal");
	  query.exec("ALTER TABLE journal_temporary RENAME TO journal");
	  query.exec
	    ("CREATE TABLE magazine_temporary "
	     "( "
	     "id		 VARCHAR(32), "
	     "myoid		 BIGINT NOT NULL, "
	     "title		 TEXT NOT NULL,	"
	     "pdate		 VARCHAR(32) NOT NULL, "
	     "publisher	 TEXT NOT NULL, "
	     "place		 TEXT NOT NULL, "
	     "category	 TEXT NOT NULL, "
	     "price		 NUMERIC(10, 2) NOT NULL DEFAULT 0.00, "
	     "description	 TEXT NOT NULL, "
	     "language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN', "
	     "monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN', "
	     "quantity	 INTEGER NOT NULL DEFAULT 1, "
	     "location	 TEXT NOT NULL, "
	     "issuevolume	 INTEGER NOT NULL DEFAULT 0, "
	     "issueno		 INTEGER NOT NULL DEFAULT 0, "
	     "lccontrolnumber	 VARCHAR(64), "
	     "callnumber	 VARCHAR(64), "
	     "deweynumber	 VARCHAR(64), "
	     "front_cover	 BYTEA, "
	     "back_cover	 BYTEA, "
	     "marc_tags      TEXT, "
	     "keyword        TEXT, "
	     "type		 VARCHAR(16) NOT NULL DEFAULT 'Magazine', "
	     "UNIQUE (id, issueno, issuevolume) "
	     ");");
	  query.exec("INSERT INTO magazine_temporary SELECT * FROM magazine");
	  query.exec("DROP TABLE magazine");
	  query.exec("ALTER TABLE magazine_temporary RENAME TO magazine");
	}
    }
}

/*
** -- setRole() --
*/

void biblioteq_misc_functions::setRole(const QSqlDatabase &db,
				       QString &errorstr,
				       const QString &roles)
{
  errorstr = "";

  if(db.driverName() == "QSQLITE")
    return; // Users are not supported.

  QSqlQuery query(db);

  if(!roles.isEmpty())
    {
      if(roles.contains("administrator"))
	query.exec("SET ROLE biblioteq_administrator");
      else
	{
	  if(roles.contains("circulation") &&
	     roles.contains("librarian") &&
	     roles.contains("membership"))
	    query.exec("SET ROLE biblioteq_circulation_librarian_membership");
	  else if(roles.contains("circulation") && roles.contains("librarian"))
	    query.exec("SET ROLE biblioteq_circulation_librarian");
	  else if(roles.contains("circulation") &&
		  roles.contains("membership"))
	    query.exec("SET ROLE biblioteq_circulation_membership");
	  else if(roles.contains("librarian") && roles.contains("membership"))
	    query.exec("SET ROLE biblioteq_librarian_membership");
	  else if(roles.contains("circulation"))
	    query.exec("SET ROLE biblioteq_circulation");
	  else if(roles.contains("librarian"))
	    query.exec("SET ROLE biblioteq_librarian");
	  else if(roles.contains("membership"))
	    query.exec("SET ROLE biblioteq_membership");
	  else if(roles.contains("patron"))
	    query.exec("SET ROLE biblioteq_patron");
	  else
	    query.exec("SET ROLE biblioteq_guest");
	}
    }
  else
    query.exec("SET ROLE biblioteq_guest");

  if(query.lastError().isValid())
    errorstr = query.lastError().text();
}

/*
** -- isGnome() --
*/

bool biblioteq_misc_functions::isGnome(void)
{
  QByteArray session(qgetenv("DESKTOP_SESSION").toLower().trimmed());

  if(session == "gnome" || session == "ubuntu")
    return true;
  else
    return false;
}

/*
** -- exportPhotographs() --
*/

void biblioteq_misc_functions::exportPhotographs
(const QSqlDatabase &db,
 const QString &collectionOid,
 const int pageOffset,
 const QString &destinationPath)
{
  QSqlQuery query(db);

  query.setForwardOnly(true);

  if(pageOffset <= 0)
    {
      query.prepare("SELECT image FROM photograph WHERE "
		    "collection_oid = ? AND image IS NOT NULL");
      query.bindValue(0, collectionOid);
    }
  else
    {
      query.prepare
	(QString("SELECT image FROM photograph WHERE "
		 "collection_oid = ? AND image IS NOT NULL "
		 "LIMIT %1 "
		 "OFFSET %2").
	 arg(biblioteq_photographcollection::PHOTOGRAPHS_PER_PAGE).
	 arg(biblioteq_photographcollection::
	     PHOTOGRAPHS_PER_PAGE * (pageOffset -
				     1)));
      query.bindValue(0, collectionOid);
    }

  if(query.exec())
    {
      int i = 0;
#if QT_VERSION >= 0x040700
      qint64 id = QDateTime::currentMSecsSinceEpoch();
#else
      QDateTime dateTime(QDateTime::currentDateTime());
      qint64 id = static_cast<qint64> (dateTime.toTime_t());
#endif

      while(query.next())
	{
	  QByteArray bytes
	    (QByteArray::fromBase64(query.value(0).toByteArray()));
	  QImage image;
	  const char *format = 0;

	  if(bytes.size() >= 4 &&
	     tolower(bytes[1]) == 'p' &&
	     tolower(bytes[2]) == 'n' &&
	     tolower(bytes[3]) == 'g')
	    format = "PNG";
	  else if(bytes.size() >= 10 &&
		  tolower(bytes[6]) == 'j' && tolower(bytes[7]) == 'f' &&
		  tolower(bytes[8]) == 'i' && tolower(bytes[9]) == 'f')
	    format = "JPG";
	  else if(bytes.size() >= 2 &&
		  tolower(bytes[0]) == 'b' &&
		  tolower(bytes[1]) == 'm')
	    format = "BMP";
	  else // Guess!
	    format = "JPG";

	  image.loadFromData(bytes, format);

	  if(!image.isNull())
	    {
	      i += 1;
	      image.save
		(destinationPath + QDir::separator() +
		 QString("%1_%2.%3").arg(id).arg(i).arg(format).toLower(),
		 format, 100);
	    }
	}
    }
}

/*
** -- getBookBindingTypes() --
*/

QStringList biblioteq_misc_functions::getBookBindingTypes
(const QSqlDatabase &db, QString &errorstr)
{
  QSqlQuery query(db);
  QString querystr("");
  QStringList types;

  errorstr = "";
  querystr = "SELECT binding_type FROM book_binding_types "
    "WHERE LENGTH(TRIM(binding_type)) > 0 "
    "ORDER BY binding_type";
  query.setForwardOnly(true);

  if(query.exec(querystr))
    while(query.next())
      types.append(query.value(0).toString());

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return types;
}

/*
** -- dnt() --
*/

bool biblioteq_misc_functions::dnt(const QSqlDatabase &db,
				   const QString &memberid,
				   QString &errorstr)
{
  if(db.driverName() == "QSQLITE")
    return true;

  QSqlQuery query(db);
  QString querystr("");
  QStringList types;
  bool dnt = true;

  errorstr = "";
  querystr = "SELECT dnt FROM member_history_dnt WHERE memberid = ?";
  query.setForwardOnly(true);
  query.prepare(querystr);
  query.bindValue(0, memberid);

  if(query.exec())
    while(query.next())
      dnt = query.value(0).toBool();

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return dnt;
}
