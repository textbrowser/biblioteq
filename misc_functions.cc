/*
** -- Local Includes --
*/

#include "misc_functions.h"

extern qtbook *qmain;

/*
** -- getImage() --
*/

QImage misc_functions::getImage(const QString &oid,
				const QString &which,
				const QString &typeArg,
				const QSqlDatabase &db)
{
  QImage image = QImage();
  QString type = typeArg.toLower().remove(" ");
  QString querystr = "";
  QSqlQuery query(db);

  if(type.toLower() == "journal")
    querystr = QString("SELECT %1, %1_fmt FROM %2 WHERE myoid = %3").arg
      (which).arg("magazine").arg(oid);
  else
    querystr = QString("SELECT %1, %1_fmt FROM %2 WHERE myoid = %3").arg
      (which).arg(type).arg(oid);

  if(query.exec(querystr))
    if(query.next())
      image.loadFromData(query.value(0).toByteArray(),
			 query.value(1).toString().toAscii());

  return image;
}

/*
** -- createOrDeleteDBAccount() --
*/

void misc_functions::createOrDeleteDBAccount(const QString &memberid,
					     const QSqlDatabase &db,
					     const int action,
					     QString &errorstr)
{
  int i = 0;
  QString querystr = "";
  QSqlQuery query(db);
  QStringList objectllist;

  errorstr = "";

  if(db.driverName() == "QSQLITE")
    return; // Users are not supported.

  if(action == CREATE_USER)
    {
      querystr = QString
	("CREATE USER %1 ENCRYPTED PASSWORD 'tempPass'").arg(memberid);
      (void) query.exec(querystr);
    }

  if(!query.lastError().isValid())
    {
      objectllist << "admin"
		  << "book"
		  << "public.book_myoid_seq"
		  << "book_copy_info"
		  << "public.book_copy_info_myoid_seq"
		  << "cd"
		  << "public.cd_myoid_seq"
		  << "cd_songs"
		  << "cd_copy_info"
		  << "public.cd_copy_info_myoid_seq"
		  << "dvd"
		  << "public.dvd_myoid_seq"
		  << "dvd_copy_info"
		  << "public.dvd_copy_info_myoid_seq"
		  << "magazine"
		  << "public.magazine_myoid_seq"
		  << "magazine_copy_info"
		  << "public.magazine_copy_info_myoid_seq"
		  << "videogame"
		  << "public.videogame_myoid_seq"
		  << "videogame_copy_info"
		  << "public.videogame_copy_info_myoid_seq"
		  << "book_borrower_vw"
		  << "cd_borrower_vw"
		  << "dvd_borrower_vw"
		  << "magazine_borrower_vw"
		  << "videogame_borrower_vw"
		  << "member_history";

      for(i = 0; i < objectllist.size(); i++)
	{
	  if(action == CREATE_USER)
	    querystr = QString
	      ("GRANT SELECT ON %1 TO %2").arg
	      (objectllist[i]).arg(memberid);
	  else
	    querystr = QString
	      ("REVOKE ALL ON %1 FROM %2").arg
	      (objectllist[i]).arg(memberid);

	  if(!query.exec(querystr))
	    break;
	}

      objectllist.clear();
    }

  if(action == DELETE_USER)
    if(!query.lastError().isValid())
      {
	query.prepare(QString("DROP USER %1").arg(memberid));
	(void) query.exec();
      }

  if(query.lastError().isValid())
    errorstr = query.lastError().text();
}

/*
** -- savePassword() --
*/

void misc_functions::savePassword(const QString &memberid,
				  const QSqlDatabase &db,
				  const QString &password, QString &errorstr)
{
  QString querystr = "";
  QSqlQuery query(db);

  errorstr = "";
  querystr = "";

  if(db.driverName() == "QSQLITE")
    return; // Users are not supported.

  querystr = QString("ALTER USER %1 ENCRYPTED "
		     "PASSWORD '%2'").arg(memberid).arg(password);
  (void) query.exec(querystr);

  if(query.lastError().isValid())
    errorstr = query.lastError().text();
}

/*
** -- getReservedItems() --
*/

QStringList misc_functions::getReservedItems(const QString &memberid,
					     const QSqlDatabase &db,
					     QString &errorstr)
{
  QString str = "";
  QString querystr = "";
  QSqlQuery query(db);
  QStringList list;

  errorstr = "";
  querystr =
    "SELECT "
    "book_borrower.copyid, "
    "book.location, "
    "book.type, "
    "book.title, "
    "book_borrower.duedate "
    "FROM "
    "book, "
    "book_borrower "
    "WHERE "
    "book_borrower.item_oid = book.myoid AND "
    "book_borrower.memberid = ? "
    "UNION ALL "
    "SELECT "
    "cd_borrower.copyid, "
    "cd.location, "
    "cd.type, "
    "cd.title, "
    "cd_borrower.duedate "
    "FROM "
    "cd, "
    "cd_borrower "
    "WHERE "
    "cd_borrower.item_oid = cd.myoid AND "
    "cd_borrower.memberid = ? "
    "UNION ALL "
    "SELECT "
    "dvd_borrower.copyid, "
    "dvd.location, "
    "dvd.type, "
    "dvd.title, "
    "dvd_borrower.duedate "
    "FROM "
    "dvd, "
    "dvd_borrower "
    "WHERE "
    "dvd_borrower.item_oid = dvd.myoid AND "
    "dvd_borrower.memberid = ? "
    "UNION ALL "
    "SELECT "
    "magazine_borrower.copyid, "
    "magazine.location, "
    "magazine.type, "
    "magazine.title, "
    "magazine_borrower.duedate "
    "FROM "
    "magazine, "
    "magazine_borrower "
    "WHERE "
    "magazine_borrower.item_oid = magazine.myoid AND "
    "magazine_borrower.memberid = ? "
    "UNION ALL "
    "SELECT "
    "videogame_borrower.copyid, "
    "videogame.location, "
    "videogame.type, "
    "videogame.title, "
    "videogame_borrower.duedate "
    "FROM "
    "videogame, "
    "videogame_borrower "
    "WHERE "
    "videogame_borrower.item_oid = videogame.myoid AND "
    "videogame_borrower.memberid = ? ";
  query.prepare(querystr);
  query.bindValue(0, memberid);
  query.bindValue(1, memberid);
  query.bindValue(2, memberid);
  query.bindValue(3, memberid);
  query.bindValue(4, memberid);

  if(query.exec())
    while(query.next())
      {
	str = QString("#%1<br>").arg(list.size() + 1);
	str += QString("Barcode: %1 Location: %2<br>"
		       "Type: %3<br>"
		       "Title: %4<br>"
		       "Due Date: %5").arg(query.value(0).toString()).
	  arg(query.value(1).toString()).
	  arg(query.value(2).toString()).
	  arg(query.value(3).toString()).
	  arg(query.value(4).toString());
	list.append(str);
      }

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return list;
}

/*
** -- getMemberMatchCount() --
*/

int misc_functions::getMemberMatchCount(const QString &checksum,
					const QString &memberid,
					const QSqlDatabase &db,
					QString &errorstr)
{
  int count = 0;
  QString querystr = "";
  QSqlQuery query(db);

  errorstr = "";
  querystr = "SELECT COUNT(memberid) FROM member "
    "WHERE "
    "LOWER(dob || sex || first_name || "
    "middle_init || "
    "last_name || street || city || state_abbr || zip) = ? "
    "AND memberid != ?";
  query.prepare(querystr);
  query.bindValue(0, checksum.toLower());
  query.bindValue(1, memberid);

  if(query.exec())
    {
      (void) query.next();
      count = query.value(0).toInt();
    }

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

QString misc_functions::getAvailability(const QString &oid,
					const QSqlDatabase &db,
					const QString &itemTypeArg,
					QString &errorstr)
{
  QString str = "";
  QString itemType = "";
  QString querystr = "";
  QSqlQuery query(db);

  errorstr = "";
  itemType = itemTypeArg.toLower().remove(" ");

  if(itemType == "journal")
    itemType = "magazine";

  querystr = QString("SELECT %1.quantity - "
		     "COUNT(%1_borrower_vw.item_oid) "
		     "FROM "
		     "%1 LEFT JOIN %1_borrower_vw ON "
		     "%1.myoid = %1_borrower_vw.item_oid WHERE "
		     "%1.myoid = ? "
		     "GROUP BY %1.quantity, "
		     "%1.myoid").arg(itemType);
  query.prepare(querystr);
  query.bindValue(0, oid);

  if(query.exec())
    {
      (void) query.next();
      str = query.value(0).toString();
    }

  if(query.lastError().isValid())
    {
      str = "0";
      errorstr = query.lastError().text();
    }
  else if(str.isEmpty())
    {
      str = "0";
      errorstr = "NULL availability value.";
    }

  return str;
}

/*
** -- getColumnString() --
*/

QString misc_functions::getColumnString(const QTableWidget *table,
					const int row,
					const QString &columnName)
{
  int i = 0;
  QString str = "";
  QTableWidgetItem *column = NULL;

  if(row >= 0 && row < table->rowCount())
    for(i = 0; i < table->columnCount(); i++)
      {
	column = table->horizontalHeaderItem(i);

	if(column == NULL || table->item(row, i) == NULL)
	  continue;

	if(column->text().toLower() == columnName.toLower())
	  {
	    str = table->item(row, i)->text();
	    break;
	  }
	else if(columnName.toLower() == "oid" &&
		column->text().toLower() == "myoid")
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

int misc_functions::getColumnNumber(const QTableWidget *table,
				    const QString &columnName)
{
  int i = 0;
  int num = -1;
  QTableWidgetItem *column = NULL;

  for(i = 0; i < table->columnCount(); i++)
    {
      column = table->horizontalHeaderItem(i);

      if(column == NULL)
	continue;

      if(column->text().toLower() == columnName.toLower())
	{
	  num = i;
	  break;
	}
      else if(columnName.toLower() == "oid" &&
	      column->text().toLower() == "myoid")
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

void misc_functions::updateColumn(QTableWidget *table,
				  const int row, const QString &columnName,
				  const QString &value)
{
  int i = 0;
  bool sortingEnabled = false;
  QTableWidgetItem *column = NULL;

  if(row >= 0 && row < table->rowCount())
    {
      if(table->isSortingEnabled())
	sortingEnabled = true;

      for(i = 0; i < table->columnCount(); i++)
	{
	  column = table->horizontalHeaderItem(i);

	  if(column == NULL)
	    continue;

	  if(column->text().toLower() == columnName.toLower())
	    {
	      if(sortingEnabled)
		table->setSortingEnabled(false);

	      table->item(row, i)->setText(value);

	      if(sortingEnabled)
		table->setSortingEnabled(true);

	      break;
	    }
	}
    }
}

/*
** -- isCheckedOut() --
*/

bool misc_functions::isCheckedOut(const QSqlDatabase &db,
				  const QString &oid,
				  const QString &itemTypeArg,
				  QString &errorstr)
{
  bool isCheckedOut = false;
  QString str = "";
  QString itemType = "";
  QString querystr = "";
  QSqlQuery query(db);

  errorstr = "";
  itemType = itemTypeArg.toLower().remove(" ");

  if(itemType == "journal")
    itemType = "magazine";

  querystr = QString("SELECT COUNT(myoid) FROM %1_borrower "
		     "WHERE item_oid = ?").arg(itemType);
  query.prepare(querystr);
  query.bindValue(0, oid);

  if(query.exec())
    {
      (void) query.next();
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

bool misc_functions::isCopyCheckedOut(const QSqlDatabase &db,
				      const QString &copyid,
				      const QString &oid,
				      const QString &itemTypeArg,
				      QString &errorstr)
{
  bool isCheckedOut = false;
  QString str = "";
  QString itemType = "";
  QString querystr = "";
  QSqlQuery query(db);

  errorstr = "";
  itemType = itemTypeArg.toLower().remove(" ");

  if(itemType == "journal")
    itemType = "magazine";

  querystr = QString("SELECT count(copyid) FROM %1_borrower WHERE "
		     "copyid = ? AND item_oid = ?").arg(itemType);
  query.prepare(querystr);
  query.bindValue(0, copyid);
  query.bindValue(1, oid);

  if(query.exec())
    {
      (void) query.next();
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

void misc_functions::saveQuantity(const QSqlDatabase &db, const QString &oid,
				  const int quantity,
				  const QString &itemTypeArg,
				  QString &errorstr)
{
  QString itemType = "";
  QString querystr = "";
  QSqlQuery query(db);

  errorstr = "";
  itemType = itemTypeArg.toLower().remove(" ");

  if(itemType == "journal")
    itemType = "magazine";

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

int misc_functions::getMaxCopyNumber(const QSqlDatabase &db,
				     const QString &oid,
				     const QString &itemTypeArg,
				     QString &errorstr)
{
  int copy_number = -1;
  QString itemType = "";
  QString querystr = "";
  QSqlQuery query(db);

  errorstr = "";
  itemType = itemTypeArg.toLower().remove(" ");

  if(itemType == "journal")
    itemType = "magazine";

  querystr = QString("SELECT MAX(copy_number) FROM %1_borrower "
		     "WHERE item_oid = ?").arg(itemType);
  query.prepare(querystr);
  query.bindValue(0, oid);

  if(query.exec())
    {
      (void) query.next();
      copy_number = query.value(0).toInt();
    }

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

bool misc_functions::isCopyAvailable(const QSqlDatabase &db,
				     const QString &oid,
				     const QString &copyid,
				     const QString &itemTypeArg,
				     QString &errorstr)
{
  bool isAvailable = false;
  QString itemType = "";
  QString querystr = "";
  QSqlQuery query(db);

  errorstr = "";
  itemType = itemTypeArg.toLower().remove(" ");

  if(itemType == "journal")
    itemType = "magazine";

  querystr = QString("SELECT COUNT(myoid) FROM %1_copy_info "
		     "WHERE copyid = ? AND item_oid = ? "
		     "AND copyid NOT IN (SELECT copyid FROM %1_borrower "
		     "WHERE item_oid = ?)").arg(itemType);
  query.prepare(querystr);
  query.bindValue(0, copyid);
  query.bindValue(1, oid);
  query.bindValue(2, oid);

  if(query.exec())
    {
      (void) query.next();

      if(query.value(0).toInt() >= 1)
	isAvailable = true;
    }

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return isAvailable;
}

/*
** -- getItemsReservedCounts() --
*/

QMap<QString, QString> misc_functions::getItemsReservedCounts
(const QSqlDatabase &db,
 const QString &memberid,
 QString &errorstr)
{
  QString str = "";
  QString querystr = "";
  QSqlQuery query(db);
  QMap<QString, QString> counts;

  errorstr = "";
  querystr =
    "SELECT COUNT(myoid) AS numbooks FROM book_borrower WHERE memberid = ? "
    "UNION ALL "
    "SELECT COUNT(myoid) AS numcds FROM cd_borrower WHERE memberid = ? "
    "UNION ALL "
    "SELECT COUNT(myoid) AS numdvds FROM dvd_borrower WHERE memberid = ? "
    "UNION ALL "
    "SELECT COUNT(myoid) AS numjournals FROM magazine_borrower WHERE "
    "memberid = ? AND "
    "item_oid IN (SELECT myoid FROM magazine WHERE type = 'Journal') "
    "UNION ALL "
    "SELECT COUNT(myoid) AS nummagazines FROM magazine_borrower WHERE "
    "memberid = ? AND "
    "item_oid IN (SELECT myoid FROM magazine WHERE type = 'Magazine') "
    "UNION ALL "
    "SELECT COUNT(myoid) AS numvideogames FROM videogame_borrower WHERE "
    "memberid = ?";
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

QString misc_functions::getRoles(const QSqlDatabase &db,
				 const QString &username, QString &errorstr)
{
  QString roles = "";
  QString querystr = "";
  QSqlQuery query(db);

  errorstr = "";
  querystr = "SELECT roles FROM admin WHERE username = ?";
  query.prepare(querystr);
  query.bindValue(0, username);

  if(query.exec())
    if(query.next())
      roles = query.value(0).toString();

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return roles;
}

/*
** -- getOID() --
*/

QString misc_functions::getOID(const QString &idArg,
			       const QString &itemTypeArg,
			       const QSqlDatabase &db,
			       QString &errorstr)
{
  int i = 0;
  QString id = "";
  QString oid = "";
  QString itemType = "";
  QString querystr = "";
  QSqlQuery query(db);

  id = idArg;
  itemType = itemTypeArg.toLower().remove(" ");

  if(itemType == "journal" || itemType == "magazine")
    querystr = "SELECT myoid FROM magazine WHERE id = ? AND "
      "mag_volume = ? AND mag_no = ?";
  else
    querystr = QString("SELECT myoid FROM %1 WHERE id = ?").arg(itemType);

  query.prepare(querystr);

  if(itemType == "journal" || itemType == "magazine")
    {
      QStringList list = id.split(",");

      for(i = 0; i < list.size(); i++)
	query.bindValue(i, list[i]);

      id = list[0];
      list.clear();
    }
  else
    query.bindValue(0, id);

  if(query.exec())
    {
      (void) query.next();
      oid = query.value(0).toString();
    }
  else
    errorstr = query.lastError().text();

  return oid;
}

/*
** -- createInitialCopies() --
*/

void misc_functions::createInitialCopies(const QString &idArg,
					 const int numCopies,
					 const QSqlDatabase &db,
					 const QString &itemTypeArg,
					 QString &errorstr)
{
  int i = 0;
  QString id = "";
  QString itemoid = "";
  QString itemType = "";
  QString querystr = "";
  QSqlQuery query(db);

  /*
  ** Retrieve the item's OID. Use the OID to create initial copies.
  */

  errorstr = "";
  id = idArg;
  itemType = itemTypeArg.toLower().remove(" ");

  if(itemType == "journal")
    itemType = "magazine";

  itemoid = getOID(id, itemType, db, errorstr);

  if(itemType == "magazine")
    id = id.split(",")[0];

  if(!itemoid.isEmpty())
    for(i = 0; i < numCopies; i++)
      {
	if(db.driverName() != "QSQLITE")
	  query.prepare(QString("INSERT INTO %1_copy_info "
				"(item_oid, copy_number, "
				"copyid) "
				"VALUES (?, "
				"?, ?)").arg(itemType));
	else
	  query.prepare(QString("INSERT INTO %1_copy_info "
				"(item_oid, copy_number, "
				"copyid, myoid) "
				"VALUES (?, "
				"?, ?, ?)").arg(itemType));

	query.bindValue(0, itemoid);
	query.bindValue(1, i + 1);
	query.bindValue(2, id + "-" + QString::number(i + 1));

	if(db.driverName() == "QSQLITE")
	  query.bindValue(3, itemoid + QString::number(i + 1));

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

QString misc_functions::getMemberName(const QSqlDatabase &db,
				      const QString &memberid,
				      QString &errorstr)
{
  QString str = "";
  QString querystr = "";
  QSqlQuery query(db);

  errorstr = "";
  querystr = "SELECT last_name, first_name FROM member WHERE memberid = ?";
  query.prepare(querystr);
  query.bindValue(0, memberid);

  if(query.exec())
    {
      (void) query.next();
      str = query.value(0).toString() + ", " + query.value(1).toString();
    }

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

void misc_functions::highlightWidget(QWidget *widget,
				     const QColor &color)
{
  QPalette pal;

  pal = widget->palette();
  pal.setColor(widget->backgroundRole(), color);
  widget->setPalette(pal);
}

/*
** -- sqliteQuerySize() --
*/

int misc_functions::sqliteQuerySize(const QString &querystr,
				    const QSqlDatabase &db,
				    const char *file, const int line)
{
  int count = 0;
  QSqlQuery query(db);

  if(db.driverName() != "QSQLITE")
    return count; // SQLite only.

  if(query.exec(querystr))
    while(query.next())
      count += 1;

  if(query.lastError().isValid())
    qmain->addError(QString("Database Error"),
		    QString("Unable to determine the query size."),
		    query.lastError().text(), file, line);

  return count;
}
