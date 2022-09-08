#include "biblioteq.h"
#include "biblioteq_misc_functions.h"

#include <QDate>
#include <QDir>
#include <QProgressDialog>

QImage biblioteq_misc_functions::getImage(const QString &oid,
					  const QString &which,
					  const QString &typeArg,
					  const QSqlDatabase &db)
{
  QSqlQuery query(db);
  auto image = QImage();
  auto type(typeArg.toLower());

  if(type == "grey literature" ||
     type == "photograph collection")
    type = type.replace(" ", "_");
  else
    type = type.remove(" ");

  if(type == "book" ||
     type == "cd" ||
     type == "dvd" ||
     type == "grey_literature" ||
     type == "journal" ||
     type == "magazine" ||
     type == "photograph_collection" ||
     type == "videogame")
    {
      if(which == "back_cover" ||
	 which == "front_cover" ||
	 which == "image_scaled")
	{
	  query.prepare(QString("SELECT %1 FROM %2 WHERE myoid = ?").
			arg(which).arg(type));
	  query.bindValue(0, oid);
	}
      else
	return image;
    }
  else
    return image;

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

QList<QPair<QString, QString> > biblioteq_misc_functions::getLocations
(const QSqlDatabase &db, QString &errorstr)
{
  QList<QPair<QString, QString> > locations;
  QSqlQuery query(db);
  QString querystr("");

  errorstr = "";
  querystr = "SELECT type, location FROM locations "
    "WHERE LENGTH(TRIM(type)) > 0 AND "
    "LENGTH(TRIM(location)) > 0 "
    "ORDER BY type, location";

  if(query.exec(querystr))
    while(query.next())
      locations.append(qMakePair(query.value(0).toString().trimmed(),
				 query.value(1).toString().trimmed()));

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return locations;
}

QList<int> biblioteq_misc_functions::selectedRows(QTableWidget *table)
{
  QList<int> rows;

  if(!table)
    return rows;

  auto indexes(table->selectionModel()->selectedRows(0));

  for(int i = 0; i < indexes.size(); i++)
    rows << indexes.at(i).row();

  std::sort(rows.begin(), rows.end());
  return rows;
}

QMap<QString, qint64> biblioteq_misc_functions::getItemsReservedCounts
(const QSqlDatabase &db, const QString &memberid, QString &errorstr)
{
  QMap<QString, qint64> counts;
  QSqlQuery query(db);
  QString querystr = "";

  errorstr = "";
  querystr =
    "SELECT COUNT(myoid) AS numbooks FROM item_borrower WHERE memberid = ? "
    "AND type = 'Book' "
    "UNION ALL "
    "SELECT COUNT(myoid) AS numcds FROM item_borrower WHERE memberid = ? "
    "AND type = 'CD' "
    "UNION ALL "
    "SELECT COUNT(myoid) AS numdvds FROM item_borrower WHERE memberid = ? "
    "AND type = 'DVD' "
    "UNION ALL "
    "SELECT COUNT(myoid) AS numgreyliteratures FROM item_borrower "
    "WHERE memberid = ? AND type = 'Grey Literature' "
    "UNION ALL "
    "SELECT COUNT(myoid) AS numjournals FROM item_borrower WHERE "
    "memberid = ? AND type = 'Journal' "
    "UNION ALL "
    "SELECT COUNT(myoid) AS nummagazines FROM item_borrower WHERE "
    "memberid = ? AND type = 'Magazine' "
    "UNION ALL "
    "SELECT COUNT(myoid) AS numvideogames FROM item_borrower WHERE "
    "memberid = ? AND type = 'Video Game'";
  query.prepare(querystr);
  query.addBindValue(memberid);
  query.addBindValue(memberid);
  query.addBindValue(memberid);
  query.addBindValue(memberid);
  query.addBindValue(memberid);
  query.addBindValue(memberid);
  query.addBindValue(memberid);

  if(query.exec())
    while(query.next())
      {
	auto count = query.value(0).toLongLong();

	if(counts.isEmpty())
	  counts["numbooks"] = count;
	else if(counts.size() == 1)
	  counts["numcds"] = count;
	else if(counts.size() == 2)
	  counts["numdvds"] = count;
	else if(counts.size() == 3)
	  counts["numgreyliteratures"] = count;
	else if(counts.size() == 4)
	  counts["numjournals"] = count;
	else if(counts.size() == 5)
	  counts["nummagazines"] = count;
	else
	  counts["numvideogames"] = count;
      }

  if(counts.isEmpty() || query.lastError().isValid())
    {
      counts["numbooks"] = 0;
      counts["numcds"] = 0;
      counts["numdvds"] = 0;
      counts["numgreyliteratures"] = 0;
      counts["numjournals"] = 0;
      counts["nummagazines"] = 0;
      counts["numtotal"] = 0;
      counts["numvideogames"] = 0;
      errorstr = query.lastError().text();
    }
  else
    counts["numtotal"] = counts.value("numbooks") +
      counts.value("numcds") +
      counts.value("numdvds") +
      counts.value("numgreyliteratures") +
      counts.value("numjournals") +
      counts.value("nummagazines") +
      counts.value("numvideogames");

  return counts;
}

QString biblioteq_misc_functions::accessionNumberAsSpecialText
(const qint64 integer)
{
  return QString::number(integer).rightJustified(20, QChar('0'));
}

QString biblioteq_misc_functions::getAbstractInfo(const QString &oid,
						  const QString &typeArg,
						  const QSqlDatabase &db)
{
  QSqlQuery query(db);
  QString querystr = "";
  QString str = "";
  auto type(typeArg.toLower());

  if(type == "book" ||
     type == "cd" ||
     type == "dvd" ||
     type == "journal" ||
     type == "magazine" ||
     type == "video game")
    {
      type = type.remove(" ");
      querystr = QString("SELECT description FROM %1 WHERE myoid = ?").arg
	(type);
    }
  else if(type == "grey literature")
    {
      type = type.replace(" ", "_");
      querystr = QString("SELECT notes FROM %1 WHERE myoid = ?").arg
	(type);
    }
  else if(type == "photograph collection")
    {
      type = type.replace(" ", "_");
      querystr = QString("SELECT about FROM %1 WHERE myoid = ?").arg
	(type);
    }
  else
    return str;

  query.prepare(querystr);
  query.bindValue(0, oid);

  if(query.exec())
    if(query.next())
      str = query.value(0).toString().trimmed();

  return str;
}

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

  if(itemType.toLower() == "book" ||
     itemType.toLower() == "cd" ||
     itemType.toLower() == "dvd" ||
     itemType.toLower() == "journal" ||
     itemType.toLower() == "magazine" ||
     itemType.toLower() == "video game")
    querystr = QString("SELECT %1.quantity - "
		       "COUNT(item_borrower.item_oid) "
		       "FROM "
		       "%1 LEFT JOIN item_borrower ON "
		       "%1.myoid = item_borrower.item_oid AND "
		       "item_borrower.type = '%2' "
		       "WHERE "
		       "%1.myoid = ? "
		       "GROUP BY %1.quantity, "
		       "%1.myoid").arg(itemType.toLower().remove(" ")).arg
      (itemType);
  else if(itemType.toLower() == "grey literature")
    querystr = "SELECT 1 - COUNT(item_borrower.item_oid) "
      "FROM "
      "grey_literature LEFT JOIN item_borrower ON "
      "grey_literature.myoid = item_borrower.item_oid AND "
      "item_borrower.type = 'Grey Literature' "
      "WHERE "
      "grey_literature.myoid = ? "
      "GROUP BY grey_literature.myoid";
  else
    return str;

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

QString biblioteq_misc_functions::getColumnString(const QTableWidget *table,
						  const int row,
						  const QString &columnName)
{
  if(columnName.isEmpty() || row < 0 || !table)
    return QString("");

  QString str = "";
  QTableWidgetItem *column = nullptr;
  int i = 0;

  if(row >= 0 && row < table->rowCount())
    for(i = 0; i < table->columnCount(); i++)
      {
	column = table->horizontalHeaderItem(i);

	if(column == nullptr || table->item(row, i) == nullptr)
	  continue;

	if(column->text().toLower() == columnName.toLower())
	  {
	    str = table->item(row, i)->text();
	    break;
	  }
      }

  return str;
}

QString biblioteq_misc_functions::getColumnString(const QTableWidget *table,
						  const int row,
						  const int column)
{
  if(column < 0 || row < 0 || !table)
    return QString("");

  QTableWidgetItem *item = nullptr;

  if((item = table->item(row, column)))
    return item->text();
  else
    return QString("");
}

QString biblioteq_misc_functions::getMemberName(const QSqlDatabase &db,
						const QString &memberid,
						QString &errorstr)
{
  QSqlQuery query(db);
  QString querystr = "";
  QString str = "";

  errorstr = "";
  querystr = "SELECT last_name, first_name FROM member WHERE memberid = ?";
  query.prepare(querystr);
  query.bindValue(0, memberid);

  if(query.exec())
    if(query.next())
      str = query.value(0).toString().trimmed() +
	", " +
	query.value(1).toString().trimmed();

  if(query.lastError().isValid())
    {
      errorstr = query.lastError().text();
      str = "Unknown";
    }

  return str;
}

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

  if(itemType == "grey literature" || itemType == "photograph collection")
    itemType = itemType.replace(" ", "_");
  else
    itemType = itemType.remove(" ");

  if(itemType == "book")
    querystr = "SELECT myoid FROM book WHERE "
      "(id = ? AND id IS NOT NULL) OR (isbn13 = ? AND isbn13 IS NOT NULL)";
  else if(itemType == "cd" ||
	  itemType == "dvd" ||
	  itemType == "photograph_collection" ||
	  itemType == "videogame")
    querystr = QString("SELECT myoid FROM %1 WHERE id = ?").arg(itemType);
  else if(itemType == "grey_literature")
    querystr = "SELECT myoid FROM grey_literature WHERE document_id = ?";
  else if(itemType == "journal" || itemType == "magazine")
    querystr = QString("SELECT myoid FROM %1 WHERE id = ? AND "
		       "issuevolume = ? AND issueno = ? AND "
		       "id IS NOT NULL").arg(itemType);
  else
    return oid;

  query.prepare(querystr);

  if(itemType == "book")
    {
      query.addBindValue(id);
      query.addBindValue(id);
    }
  else if(itemType == "journal" || itemType == "magazine")
    {
      auto list(id.split(","));

      for(i = 0; i < list.size(); i++)
	query.bindValue(i, list[i]);
    }
  else
    query.bindValue(0, id);

  if(query.exec())
    if(query.next())
      oid = query.value(0).toString().trimmed();

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return oid;
}

QString biblioteq_misc_functions::getRoles(const QSqlDatabase &db,
					   const QString &username,
					   QString &errorstr)
{
  QSqlQuery query(db);
  QString querystr = "";
  QString roles = "";

  errorstr = "";
  querystr = "SELECT LOWER(roles) FROM admin WHERE LOWER(username) = LOWER(?)";
  query.prepare(querystr);
  query.bindValue(0, username);

  if(query.exec())
    if(query.next())
      {
	roles = query.value(0).toString().trimmed();

	if(roles == "none")
	  roles = "";
      }

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return roles;
}

QString biblioteq_misc_functions::getTotalReserved(const QSqlDatabase &db,
						   const QString &itemTypeArg,
						   const QString &oid)
{
  QSqlQuery query(db);
  QString itemType = "";
  QString querystr = "";
  QString str = "";

  itemType = itemTypeArg.toLower();

  if(itemType == "book" ||
     itemType == "cd" ||
     itemType == "dvd" ||
     itemType == "journal" ||
     itemType == "magazine" ||
     itemType == "video game")
    querystr = QString("SELECT COUNT(item_borrower.item_oid) "
		       "FROM item_borrower WHERE "
		       "item_borrower.item_oid = %1 AND "
		       "item_borrower.type = '%2'").arg(oid).arg(itemTypeArg);
  else if(itemType == "grey literature")
    querystr = QString("SELECT COUNT(item_borrower.item_oid) "
		       "FROM item_borrower WHERE "
		       "item_borrower.item_oid = %1 AND "
		       "item_borrower.type = 'Grey Literature'").arg(oid);
  else
    return str;

  query.prepare(querystr);

  if(query.exec())
    if(query.next())
      str = query.value(0).toString();

  return str;
}

QString biblioteq_misc_functions::imageFormatGuess(const QByteArray &bytes)
{
  QString format("");

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

  return format;
}

QString biblioteq_misc_functions::isbn10to13(const QString &text)
{
  if(QString(text).remove('-').length() != 10)
    return text;

  QList<int> array;
  QString numberstr("");
  QString str("978" + QString(text).remove('-').trimmed().left(9));
  int check = 0;
  int total = 0;

  array << 1 << 3 << 1 << 3 << 1 << 3 << 1 << 3 << 1 << 3 << 1 << 3;

  for(int i = 0; i < array.size(); i++)
    if(i < str.length())
      total += array.at(i) * str.at(i).digitValue();
    else
      break;

  check = 10 - (total % 10);

  if(check == 10)
    check = 0;

  numberstr.setNum(check);
  return str.append(numberstr);
}

QString biblioteq_misc_functions::isbn13to10(const QString &text)
{
  if(QString(text).remove('-').length() != 13)
    return text;

  QString z("");
  auto str(QString(text).remove('-').trimmed().mid(3, 9));
  int total = 0;

  for(int i = 0; i < 9; i++)
    if(i < str.length())
      total += str[i].digitValue() * (10 - i);
    else
      break;

  z = QString::number((11 - (total % 11)) % 11);

  if(z == "10")
    z = "X";

  return str + z;
}

QString biblioteq_misc_functions::linearizeString(const QString &text)
{
  return text.simplified().replace("<br>", " ").simplified().trimmed();
}

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

  if(query.exec(querystr))
    while(query.next())
      types.append(query.value(0).toString().trimmed());

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return types;
}

QStringList biblioteq_misc_functions::getBookTargetAudiences
(const QSqlDatabase &db, QString &errorstr)
{
  QSqlQuery query(db);
  QString querystr("");
  QStringList list;

  errorstr = "";
  querystr = "SELECT target_audience FROM book_target_audiences "
    "WHERE LENGTH(TRIM(target_audience)) > 0 "
    "ORDER BY target_audience";

  if(query.exec(querystr))
    while(query.next())
      list.append(query.value(0).toString().trimmed());

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return list;
}

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

  if(query.exec(querystr))
    while(query.next())
      formats.append(query.value(0).toString().trimmed());

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return formats;
}

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

  if(query.exec(querystr))
    while(query.next())
      aspectratios.append(query.value(0).toString().trimmed());

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return aspectratios;
}

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

  if(query.exec(querystr))
    while(query.next())
      ratings.append(query.value(0).toString().trimmed());

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return ratings;
}

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

  if(query.exec(querystr))
    while(query.next())
      regions.append(query.value(0).toString().trimmed());

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return regions;
}

QStringList biblioteq_misc_functions::getGreyLiteratureTypes
(const QSqlDatabase &db, QString &errorstr)
{
  QSqlQuery query(db);
  QString querystr("");
  QStringList types;

  errorstr = "";
  querystr = "SELECT document_type FROM grey_literature_types "
    "WHERE LENGTH(TRIM(document_type)) > 0 "
    "ORDER BY document_type";

  if(query.exec(querystr))
    while(query.next())
      types.append(query.value(0).toString().trimmed());

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return types;
}

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

  if(query.exec(querystr))
    while(query.next())
      languages.append(query.value(0).toString().trimmed());

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return languages;
}

QStringList biblioteq_misc_functions::getLocations(const QSqlDatabase &db,
						   const QString &type,
						   QString &errorstr)
{
  QSqlQuery query(db);
  QStringList locations;

  errorstr = "";

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
      locations.append(query.value(0).toString().trimmed());

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return locations;
}

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

  if(query.exec(querystr))
    while(query.next())
      {
	if(query.value(0).toString().trimmed() == "CD")
	  map["Music CD"] = query.value(1).toString().trimmed();
	else
	  map[query.value(0).toString().trimmed()] =
	    query.value(1).toString().trimmed();
      }

  if(!map.contains("Book"))
    map["Book"] = "1";

  if(!map.contains("DVD"))
    map["DVD"] = "1";

  if(!map.contains("Grey Literature"))
    map["Grey Literature"] = "1";

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

  if(query.exec(querystr))
    while(query.next())
      monetary_units.append(query.value(0).toString().trimmed());

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return monetary_units;
}

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
	date = QDate::fromString(query.value(4).toString().trimmed(),
				 "MM/dd/yyyy");
	str = QString(QObject::tr("#")) +
	  QString::number(list.size() + 1) + "<br>";
	str += QObject::tr("Barcode: ") +
	  query.value(0).toString().trimmed() + "<br>" +
	  QObject::tr(" Location: ") +
	  query.value(1).toString().trimmed() + "<br>" +
	  QObject::tr("Type: ") + query.value(2).toString().trimmed() +
	  "<br>" +
	  QObject::tr("Title: ") + query.value(3).toString().trimmed() +
	  "<br>" +
	  QObject::tr("Due Date: ") +
	  date.toString(Qt::ISODate);
	list.append(str);
      }

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return list;
}

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

  if(query.exec(querystr))
    while(query.next())
      platforms.append(query.value(0).toString().trimmed());

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return platforms;
}

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

  if(query.exec(querystr))
    while(query.next())
      ratings.append(query.value(0).toString().trimmed());

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return ratings;
}

bool biblioteq_misc_functions::dnt(const QSqlDatabase &db,
				   const QString &memberid,
				   QString &errorstr)
{
  if(db.driverName() == "QSQLITE")
    return false;

  QSqlQuery query(db);
  QString querystr("");
  QStringList types;
  auto dnt = true;

  errorstr = "";
  querystr = "SELECT dnt FROM member_history_dnt WHERE memberid = ?";
  query.prepare(querystr);
  query.bindValue(0, memberid);

  if(query.exec())
    while(query.next())
      dnt = query.value(0).toBool();

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return dnt;
}

bool biblioteq_misc_functions::getMemberMatch(const QString &checksum,
					      const QString &memberid,
					      const QSqlDatabase &db,
					      QString &errorstr)
{
  QSqlQuery query(db);
  QString querystr = "";
  auto exists = false;

  errorstr = "";
  querystr = "SELECT EXISTS(SELECT 1 FROM member "
    "WHERE "
    "dob || sex || first_name || "
    "middle_init || "
    "last_name || street || city || state_abbr || zip = ? "
    "AND memberid != ?)";
  query.prepare(querystr);
  query.bindValue(0, checksum);
  query.bindValue(1, memberid);

  if(query.exec())
    if(query.next())
      exists = query.value(0).toBool();

  if(query.lastError().isValid())
    {
      exists = false;
      errorstr = query.lastError().text();
    }

  return exists;
}

bool biblioteq_misc_functions::hasMemberExpired(const QSqlDatabase &db,
						const QString &memberid,
						QString &errorstr)
{
  QSqlQuery query(db);
  auto expired = true;

  errorstr = "";

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
	    auto date
	      (QDate::
	       fromString(query.value(0).toString().trimmed(), "MM/dd/yyyy"));

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

bool biblioteq_misc_functions::hasUnaccentExtension(const QSqlDatabase &db)
{
  if(db.driverName() == "QSQLITE")
    return false;

  QSqlQuery query(db);

  if(query.exec("SELECT LOWER(extname) FROM pg_extension WHERE "
		"LOWER(extname) = 'unaccent'") && query.next())
    return query.value(0).toString().trimmed() == "unaccent";

  return false;
}

bool biblioteq_misc_functions::isCheckedOut(const QSqlDatabase &db,
					    const QString &oid,
					    const QString &itemTypeArg,
					    QString &errorstr)
{
  QSqlQuery query(db);
  QString itemType = "";
  auto isCheckedOut = false;

  errorstr = "";
  itemType = itemTypeArg;
  query.prepare("SELECT EXISTS(SELECT 1 FROM item_borrower "
		"WHERE item_oid = ? AND type = ?)");
  query.bindValue(0, oid);
  query.bindValue(1, itemType);

  if(query.exec())
    if(query.next())
      isCheckedOut = query.value(0).toBool();

  if(query.lastError().isValid())
    {
      errorstr = query.lastError().text();
      isCheckedOut = false;
    }

  return isCheckedOut;
}

bool biblioteq_misc_functions::isCopyAvailable(const QSqlDatabase &db,
					       const QString &oid,
					       const QString &copyid,
					       const QString &itemTypeArg,
					       QString &errorstr)
{
  QSqlQuery query(db);
  QString itemType = "";
  QString querystr = "";
  auto isAvailable = false;

  errorstr = "";
  itemType = itemTypeArg;

  if(itemType.toLower() == "book" ||
     itemType.toLower() == "cd" ||
     itemType.toLower() == "dvd" ||
     itemType.toLower() == "journal" ||
     itemType.toLower() == "magazine" ||
     itemType.toLower() == "video game")
    querystr = QString
      ("SELECT EXISTS(SELECT 1 FROM %1_copy_info "
       "WHERE copyid = ? AND item_oid = ? "
       "AND copyid NOT IN (SELECT copyid FROM item_borrower "
       "WHERE item_oid = ? AND type = '%2'))").arg(itemType.
						   toLower().remove(" ")).
      arg(itemType);
  else if(itemType.toLower() == "grey literature")
    querystr = "SELECT NOT EXISTS(SELECT 1 FROM item_borrower "
      "WHERE item_oid = ? AND type = 'Grey Literature')";
  else
    return isAvailable;

  query.prepare(querystr);

  if(itemType.toLower() == "grey literature")
    query.addBindValue(oid);
  else
    {
      query.addBindValue(copyid);
      query.addBindValue(oid);
      query.addBindValue(oid);
    }

  if(query.exec())
    if(query.next())
      isAvailable = query.value(0).toBool();

  if(query.lastError().isValid())
    {
      errorstr = query.lastError().text();
      isAvailable = false;
    }

  return isAvailable;
}

bool biblioteq_misc_functions::isCopyCheckedOut(const QSqlDatabase &db,
						const QString &copyid,
						const QString &oid,
						const QString &itemTypeArg,
						QString &errorstr)
{
  QSqlQuery query(db);
  QString itemType = "";
  auto isCheckedOut = false;

  errorstr = "";
  itemType = itemTypeArg;
  query.prepare("SELECT EXISTS(SELECT 1 FROM item_borrower WHERE "
		"copyid = ? AND item_oid = ? AND "
		"type = ?)");
  query.bindValue(0, copyid);
  query.bindValue(1, oid);
  query.bindValue(2, itemType);

  if(query.exec())
    if(query.next())
      isCheckedOut = query.value(0).toBool();

  if(query.lastError().isValid())
    {
      errorstr = query.lastError().text();
      isCheckedOut = false;
    }

  return isCheckedOut;
}

bool biblioteq_misc_functions::isGnome(void)
{
  auto session(qgetenv("DESKTOP_SESSION").toLower().trimmed());

  if(session == "gnome" || session == "ubuntu")
    return true;
  else
    return false;
}

bool biblioteq_misc_functions::isItemAvailable
(const QSqlDatabase &db,
 const QString &id,
 const QString &copyId,
 const QString &t)
{
  QSqlQuery query(db);
  QString querystr("");
  auto type(t.toLower().trimmed());

  if(type == "book")
    {
      if(copyId.trimmed().isEmpty())
	{
	  querystr = "SELECT book.quantity - COUNT(item_borrower.item_oid) "
	    "FROM "
	    "book LEFT JOIN item_borrower ON "
	    "book.myoid = item_borrower.item_oid AND "
	    "item_borrower.type = 'Book' "
	    "WHERE "
	    "book.id = ? OR book.isbn13 = ? "
	    "GROUP BY book.quantity, "
	    "book.myoid";
	  query.prepare(querystr);
	  query.addBindValue(id);
	  query.addBindValue(id);
	}
      else
	{
	  querystr = "SELECT book.quantity - COUNT(item_borrower.item_oid) "
	    "FROM "
	    "book LEFT JOIN item_borrower ON "
	    "book.myoid = item_borrower.item_oid AND "
	    "item_borrower.type = 'Book' "
	    "WHERE "
	    "(book.id = ? OR book.isbn13 = ?) AND item_borrower.copyid = ? "
	    "GROUP BY book.quantity, "
	    "book.myoid";
	  query.prepare(querystr);
	  query.addBindValue(id);
	  query.addBindValue(id);
	  query.addBindValue(copyId);
	}
    }

  if(query.exec())
    {
      if(query.next())
	return query.value(0).toInt() > 0;
      else
	return true;
    }

  return false;
}

bool biblioteq_misc_functions::isRequested(const QSqlDatabase &db,
					   const QString &oid,
					   const QString &itemTypeArg,
					   QString &errorstr)
{
  auto isRequested = false;

  errorstr = "";

  if(db.driverName() == "QSQLITE")
    return isRequested; // Requests are not supported.

  QSqlQuery query(db);
  QString itemType = "";

  itemType = itemTypeArg;
  query.prepare("SELECT EXISTS(SELECT 1 FROM item_request "
		"WHERE item_oid = ? AND type = ?)");
  query.bindValue(0, oid);
  query.bindValue(1, itemType);

  if(query.exec())
    if(query.next())
      isRequested = query.value(0).toBool();

  if(query.lastError().isValid())
    {
      errorstr = query.lastError().text();
      isRequested = false;
    }

  return isRequested;
}

bool biblioteq_misc_functions::userExists(const QString &userid,
					  const QSqlDatabase &db,
					  QString &errorstr)
{
  QSqlQuery query(db);
  auto exists = false;

  errorstr = "";

  if(db.driverName() == "QSQLITE")
    query.prepare("SELECT EXISTS(SELECT 1 FROM member WHERE "
		  "memberid = ?)");
  else
    query.prepare("SELECT EXISTS(SELECT 1 FROM pg_user WHERE "
		  "LOWER(usename) = LOWER(?))");

  query.bindValue(0, userid);

  if(query.exec())
    if(query.next())
      exists = query.value(0).toBool();

  if(query.lastError().isValid())
    {
      exists = false;
      errorstr = query.lastError().text();
    }

  return exists;
}

int biblioteq_misc_functions::getColumnNumber(const QTableWidget *table,
					      const QString &columnName)
{
  if(columnName.isEmpty() || !table)
    return -1;

  QTableWidgetItem *column = nullptr;
  int i = 0;
  int num = -1;

  for(i = 0; i < table->columnCount(); i++)
    {
      column = table->horizontalHeaderItem(i);

      if(column == nullptr)
	continue;

      if(column->text() == columnName ||
	 column->text().toLower() == columnName.toLower())
	{
	  num = i;
	  break;
	}
    }

  return num;
}

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
  query.prepare("SELECT MAX(copy_number) FROM item_borrower "
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

int biblioteq_misc_functions::getMinimumDays(const QSqlDatabase &db,
					     const QString &type,
					     QString &errorstr)
{
  QSqlQuery query(db);
  int minimumdays = 1;

  errorstr = "";
  query.prepare("SELECT days FROM minimum_days WHERE LOWER(type) = ?");
  query.bindValue(0, type.toLower().trimmed());

  if(query.exec())
    if(query.next())
      minimumdays = query.value(0).toInt();

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return minimumdays;
}

int biblioteq_misc_functions::maximumReserved(const QSqlDatabase &db,
					      const QString &memberid,
					      const QString &type)
{
  QSqlQuery query(db);
  QString querystr = "";

  if(type.toLower() == "book")
    querystr = "SELECT maximum_reserved_books FROM member WHERE memberid = ?";

  query.prepare(querystr);
  query.addBindValue(memberid);

  if(query.exec() && query.next())
    return query.value(0).toInt();

  return 0;
}

int biblioteq_misc_functions::sqliteQuerySize
(const QString &querystr,
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
 const QMap<QString, QVariant> &boundValues,
#else
 const QVariantList &boundValues,
#endif
 const QSqlDatabase &db,
 const char *file,
 const int line,
 biblioteq *qmain)
{
  int count = 0;

  if(db.driverName() != "QSQLITE")
    return count; // SQLite only.
  else if(querystr.trimmed().isEmpty())
    return count;

  QSqlQuery query(db);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  auto list(boundValues.values());
#else
  auto list(boundValues);
#endif

  query.prepare(querystr);

  for(int i = 0; i < list.size(); i++)
    query.bindValue(i, list.at(i));

  if(query.exec())
    while(query.next())
      count += 1;

  if(query.lastError().isValid())
    if(qmain)
      qmain->addError
	(QString(QObject::tr("Database Error")),
	 QString(QObject::tr("Unable to determine the query size.")),
	 query.lastError().text(), file, line);

  return count;
}

int biblioteq_misc_functions::sqliteQuerySize(const QString &querystr,
					      const QSqlDatabase &db,
					      const char *file,
					      const int line,
					      biblioteq *qmain)
{
  int count = 0;

  if(db.driverName() != "QSQLITE")
    return count; // SQLite only.
  else if(querystr.trimmed().isEmpty())
    return count;

  QSqlQuery query(db);

  if(query.exec(querystr))
    while(query.next())
      count += 1;

  if(query.lastError().isValid())
    if(qmain)
      qmain->addError
	(QString(QObject::tr("Database Error")),
	 QString(QObject::tr("Unable to determine the query size.")),
	 query.lastError().text(), file, line);

  return count;
}

qint64 biblioteq_misc_functions::bookAccessionNumber(const QSqlDatabase &db)
{
  QSqlQuery query(db);

  if(db.driverName() == "QPSQL")
    query.prepare("SELECT NEXTVAL('book_sequence')");
  else
    {
      if(query.exec("INSERT INTO book_sequence VALUES (NULL)"))
	return query.lastInsertId().toLongLong();
      else
	query.prepare("SELECT value FROM book_sequence");
    }

  if(query.exec() && query.next())
    return query.value(0).toLongLong();

  return -1;
}

qint64 biblioteq_misc_functions::getSqliteUniqueId(const QSqlDatabase &db,
						   QString &errorstr)
{
  qint64 value = -1;

  if(db.driverName() != "QSQLITE")
    return value;

  QSqlQuery query(db);

  errorstr = "";

  if(query.exec("INSERT INTO sequence VALUES (NULL)"))
    {
      auto variant(query.lastInsertId());

      if(variant.isValid())
	{
	  value = variant.toLongLong();

	  /*
	  ** Store only one entry in the table.
	  */

	  query.exec
	    (QString("DELETE FROM sequence WHERE value < %1").arg(value));
	}
      else
	errorstr = "Invalid variant.";
    }
  else if(query.lastError().isValid())
    errorstr = query.lastError().text();
  else
    errorstr = "Query failure.";

  return value;
}

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
  auto exists = false;

  if(action == CREATE_USER)
    {
      /*
      ** Does the user exist?
      */

      exists = userExists(userid, db, errorstr);

      if(!exists)
	{
	  auto str(roles);

	  if(str.contains("administrator") || str.contains("membership"))
	    querystr = QString
	      ("CREATE USER %1 ENCRYPTED PASSWORD 'tempPass' "
	       "CREATEROLE").arg(userid);
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
      auto str(roles);

      if(str.isEmpty() || str == "none")
	str = "biblioteq_patron";
      else
	{
	  str.replace(" ", "_");
	  str.prepend("biblioteq_");
	}

      objectlist << str;

      for(int i = 0; i < objectlist.size(); i++)
	{
	  querystr = QString("REVOKE %1 FROM %2").arg
	    (objectlist.at(i)).arg(userid);

	  if(!query.exec(querystr))
	    break;
	}

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
		  querystr = QString("ALTER USER %1 CREATEROLE").arg(userid);
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
		  querystr = QString("ALTER USER %1 CREATEROLE").arg(userid);
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

void biblioteq_misc_functions::createBookCopy(const QString &idArg,
					      const int copyNumber,
					      const QSqlDatabase &db,
					      QString &errorstr)
{
  QSqlQuery query(db);
  QString id("");
  QString itemOid("");

  /*
  ** Retrieve the item's OID.
  */

  errorstr.clear();
  id = idArg;
  itemOid = getOID(id, "Book", db, errorstr);

  if(!errorstr.isEmpty())
    return;

  if(itemOid.isEmpty())
    {
      errorstr = QObject::tr
	("The method biblioteq_misc_functions::getOID() did not "
	 "discover a valid OID.");
      return;
    }

  if(!itemOid.isEmpty())
    {
      if(db.driverName() != "QSQLITE")
	query.prepare("INSERT INTO book_copy_info "
		      "(item_oid, copy_number, copyid) "
		      "VALUES (?, ?, ?)");
      else
	query.prepare("INSERT INTO book_copy_info "
		      "(item_oid, copy_number, copyid, myoid) "
		      "VALUES (?, ?, ?, ?)");

      query.addBindValue(itemOid);
      query.addBindValue(copyNumber);
      query.addBindValue(id + "-" + QString::number(copyNumber));

      if(db.driverName() == "QSQLITE")
	{
	  auto value = getSqliteUniqueId(db, errorstr);

	  if(errorstr.isEmpty())
	    query.addBindValue(value);
	}

      if(errorstr.isEmpty() && !query.exec())
	errorstr = query.lastError().text();

      if(errorstr.isEmpty())
	{
	  query.prepare
	    ("UPDATE book SET quantity = "
	     "(SELECT COUNT(*) FROM book_copy_info WHERE item_oid = ?) "
	     "WHERE myoid = ?");
	  query.addBindValue(itemOid);
	  query.addBindValue(itemOid);
	  query.exec();
	}
    }
}

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
  auto copies = qBound
    (0, numCopies, static_cast<int> (biblioteq::Limits::QUANTITY));
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
    for(i = 0; i < copies; i++)
      {
	if(db.driverName() != "QSQLITE")
	  {
	    if(itemType == "book" ||
	       itemType == "cd" ||
	       itemType == "dvd" ||
	       itemType == "journal" ||
	       itemType == "magazine" ||
	       itemType == "videogame")
	      query.prepare(QString("INSERT INTO %1_copy_info "
				    "(item_oid, copy_number, "
				    "copyid) "
				    "VALUES (?, "
				    "?, ?)").arg(itemType));
	  }
	else
	  {
	    if(itemType == "book" ||
	       itemType == "cd" ||
	       itemType == "dvd" ||
	       itemType == "journal" ||
	       itemType == "magazine" ||
	       itemType == "videogame")
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
	    auto value = getSqliteUniqueId(db, errorstr);

	    if(errorstr.isEmpty())
	      query.bindValue(3, value);
	    else
	      break;
	  }

	if(errorstr.isEmpty() && !query.exec())
	  {
	    errorstr = query.lastError().text();
	    break;
	  }
      }
}

void biblioteq_misc_functions::exportPhotographs
(const QSqlDatabase &db,
 const QString &collectionOid,
 const QString &destinationPath,
 const QList<QGraphicsItem *> &items,
 QWidget *parent)
{
  QProgressDialog progress(parent);

  progress.setLabelText(QObject::tr("Exporting image(s)..."));
  progress.setMaximum(items.size());
  progress.setMinimum(0);
  progress.setModal(true);
  progress.setWindowTitle(QObject::tr("BiblioteQ: Progress Dialog"));
  progress.show();
  progress.repaint();
  QApplication::processEvents();

  QSqlQuery query(db);

  query.prepare("SELECT image FROM photograph WHERE "
		"collection_oid = ? AND image IS NOT NULL AND myoid = ?");
  query.bindValue(0, collectionOid);

  for(int i = 0; i < items.size(); i++)
    {
      auto item = items.at(i);

      if(!item)
	continue;

      if(i + 1 <= progress.maximum())
	progress.setValue(i + 1);

      query.bindValue(1, item->data(0).toString());
      progress.repaint();
      QApplication::processEvents();

      if(progress.wasCanceled())
	break;

      if(query.exec() && query.next())
	{
	  QImage image;
	  auto bytes
	    (QByteArray::fromBase64(query.value(0).toByteArray()));
	  auto id = QDateTime::currentMSecsSinceEpoch();
	  auto format(imageFormatGuess(bytes));

	  image.loadFromData(bytes, format.toLatin1().constData());

	  if(!image.isNull())
	    image.save
	      (destinationPath + QDir::separator() +
	       QString("%1_%2.%3").arg(id).arg(i + 1).arg(format).toLower(),
	       format.toLatin1().constData(), 100);
	}
    }
}

void biblioteq_misc_functions::exportPhotographs
(const QSqlDatabase &db,
 const QString &collectionOid,
 const int pageOffset,
 const int photographsPerPage,
 const QString &destinationPath,
 QWidget *parent)
{
  QProgressDialog progress(parent);

  progress.setLabelText(QObject::tr("Exporting image(s)..."));
  progress.setMinimum(0);
  progress.setModal(true);
  progress.setWindowTitle(QObject::tr("BiblioteQ: Progress Dialog"));
  progress.show();
  progress.repaint();
  QApplication::processEvents();

  QSqlQuery query(db);

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
	 arg(photographsPerPage).
	 arg(photographsPerPage * (pageOffset - 1)));
      query.bindValue(0, collectionOid);
    }

  if(query.exec())
    {
      if(db.driverName() == "QPSQL")
	progress.setMaximum(query.size());
      else
	progress.setMaximum(0);

      int i = 0;
      int j = -1;
      auto id = QDateTime::currentMSecsSinceEpoch();

      while(query.next())
	{
	  if(db.driverName() == "QPSQL")
	    {
	      j += 1;

	      if(j + 1 <= progress.maximum())
		progress.setValue(j + 1);
	    }

	  progress.repaint();
	  QApplication::processEvents();

	  if(progress.wasCanceled())
	    break;

	  QImage image;
	  auto bytes
	    (QByteArray::fromBase64(query.value(0).toByteArray()));
	  auto format(imageFormatGuess(bytes));

	  image.loadFromData(bytes, format.toLatin1().constData());

	  if(!image.isNull())
	    {
	      i += 1;
	      image.save
		(destinationPath + QDir::separator() +
		 QString("%1_%2.%3").arg(id).arg(i).arg(format).toLower(),
		 format.toLatin1().constData(), 100);
	    }
	}
    }
}

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
  auto str(roles);

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
	  querystr = QString("ALTER USER %1 CREATEROLE").arg(userid);
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
	  querystr = QString("ALTER USER %1 CREATEROLE").arg(userid);
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

void biblioteq_misc_functions::hideAdminFields(QMainWindow *window,
					       const QString &roles)
{
  if(!window)
    return;

  QString str = "";
  auto showWidgets = true;

  if(roles.isEmpty())
    showWidgets = false;
  else if(roles.contains("administrator") ||
	  roles.contains("librarian"))
    showWidgets = true;
  else
    showWidgets = false;

  foreach(auto widget, window->findChildren<QWidget *> ())
    {
      str = widget->objectName().toLower();

      if(str.contains("price") || str.contains("monetary"))
	widget->setVisible(showWidgets);
    }

  foreach(auto widget, window->findChildren<QLabel *> ())
    {
      str = widget->text().toLower();

      if(str.contains("price") || str.contains("monetary"))
	widget->setVisible(showWidgets);
    }

  foreach(auto button, window->findChildren<QToolButton *> ())
    if(button->menu())
      {
	foreach(auto action, button->menu()->findChildren<QAction *> ())
	  {
	    str = action->text().toLower();

	    if(str.contains("price") || str.contains("monetary"))
	      action->setVisible(showWidgets);
	  }
      }
}

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
  auto exists = userExists(userid, db, errorstr);

  if(exists)
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

      for(int i = 0; i < objectlist.size(); i++)
	{
	  querystr = QString("REVOKE %1 FROM %2").arg
	    (objectlist.at(i)).arg(userid);

	  if(!query.exec(querystr))
	    break;
	}

      if(query.lastError().isValid())
	errorstr = query.lastError().text();
      else
	{
	  querystr = QString("ALTER USER %1 NOCREATEROLE").arg(userid);
	  (void) query.exec(querystr);

	  if(query.lastError().isValid())
	    errorstr = query.lastError().text();
	}
    }
}

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

  if(itemType == "book" ||
     itemType == "cd" ||
     itemType == "dvd" ||
     itemType == "journal" ||
     itemType == "magazine" ||
     itemType == "videogame")
    querystr = QString("UPDATE %1 SET quantity = ? WHERE myoid = ?").
      arg(itemType);
  else
    return;

  query.prepare(querystr);
  query.bindValue(0, quantity);
  query.bindValue(1, oid);
  (void) query.exec();

  if(query.lastError().isValid())
    errorstr = query.lastError().text();
}

void biblioteq_misc_functions::setBookRead(const QSqlDatabase &db,
					   const bool state,
					   const quint64 myoid)
{
  if(db.driverName() != "QSQLITE")
    return;

  QSqlQuery query(db);

  query.prepare("UPDATE book SET book_read = ? WHERE myoid = ?");
  query.addBindValue(state ? 1 : 0);
  query.addBindValue(myoid);
  query.exec();
}

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

void biblioteq_misc_functions::sortCombinationBox(QComboBox *comboBox)
{
  if(!comboBox)
    return;

  QStringList list;

  for(int i = 0; i < comboBox->count(); i++)
    list << comboBox->itemText(i);

  std::sort(list.begin(), list.end());
  comboBox->clear();
  comboBox->addItems(list);
}

void biblioteq_misc_functions::updateColumn(QTableWidget *table,
					    const int row,
					    int column,
					    const QString &value)
{
  if(column < 0 || row < 0 || !table || !table->item(row, column))
    return;

  auto sortingEnabled = false;

  if(table->isSortingEnabled())
    sortingEnabled = true;

  if(sortingEnabled)
    table->setSortingEnabled(false);

  table->item(row, column)->setText(value);

  if(sortingEnabled)
    table->setSortingEnabled(true);

  if(qobject_cast<biblioteq_main_table *> (table))
    qobject_cast<biblioteq_main_table *> (table)->updateToolTips(row);
}

void biblioteq_misc_functions::updateColumnColor(QTableWidget *table,
						 const int row,
						 int column,
						 const QColor &color)
{
  if(column < 0 || row < 0 || !table || !table->item(row, column))
    return;

  table->item(row, column)->setBackground(color);
}
