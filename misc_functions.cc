/*
** -- Qt Includes --
*/

#include <QtDebug>

/*
** -- Local Includes --
*/

#include "misc_functions.h"

extern qtbook *qmain;

/*
** As of SQLite 3.5.9, users are not supported.
*/

/*
** -- userCount() --
*/

int misc_functions::userCount(const QString &userid,
			      const QSqlDatabase &db,
			      QString &errorstr)
{
  int count = 0;
  QString querystr = "";
  QSqlQuery query(db);

  errorstr = "";

  if(db.driverName() == "QSQLITE")
    querystr = QString("SELECT COUNT(memberid) FROM member WHERE "
		       "memberid = '%1'").arg(userid);
  else
    querystr = QString("SELECT COUNT(usename) FROM pg_user WHERE "
		       "usename = '%1'").arg(userid);

  if(query.exec(querystr))
    if(query.next())
      count = query.value(0).toInt();

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

QString misc_functions::getAbstractInfo(const QString &oid,
					const QString &typeArg,
					const QSqlDatabase &db)
{
  QString str = "";
  QString type = typeArg.toLower().remove(" ");
  QString querystr = "";
  QSqlQuery query(db);

  querystr = QString("SELECT description FROM %1 WHERE myoid = %2").arg
    (type).arg(oid);

  if(query.exec(querystr))
    if(query.next())
      str = query.value(0).toString();

  return str;
}

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

  querystr = QString("SELECT %1 FROM %2 WHERE myoid = %3").arg
    (which).arg(type).arg(oid);

  if(query.exec(querystr))
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

void misc_functions::grantPrivs(const QString &userid,
				const QString &roles,
				const QSqlDatabase &db,
				QString &errorstr)
{
  int i = 0;
  QString querystr = "";
  QSqlQuery query(db);
  QStringList privlist;
  QStringList objectlist;

  errorstr = "";

  if(db.driverName() == "QSQLITE")
    return; // Users are not supported.

  privlist << "SELECT"
	   << "SELECT";
  objectlist << "admin"
	     << "item_borrower_vw";

  if(roles.contains("administrator") || roles.contains("circulation"))
    {
      privlist << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "DELETE, INSERT, SELECT, UPDATE"
	       << "SELECT, UPDATE, USAGE"
	       << "INSERT, SELECT, UPDATE"
	       << "SELECT, UPDATE, USAGE"
	       << "DELETE, SELECT"
	       << "SELECT, UPDATE, USAGE"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "DELETE, INSERT, SELECT, UPDATE"
	       << "SELECT"
	       << "SELECT";
      objectlist << "book"
		 << "book_copy_info"
		 << "book_copy_info_myoid_seq"
		 << "book_myoid_seq"
		 << "cd"
		 << "cd_copy_info"
		 << "cd_copy_info_myoid_seq"
		 << "cd_myoid_seq"
		 << "cd_songs"
		 << "dvd"
		 << "dvd_copy_info"
		 << "dvd_copy_info_myoid_seq"
		 << "dvd_myoid_seq"
		 << "journal"
		 << "journal_copy_info"
		 << "journal_copy_info_myoid_seq"
		 << "journal_myoid_seq"
		 << "magazine"
		 << "magazine_copy_info"
		 << "magazine_copy_info_myoid_seq"
		 << "magazine_myoid_seq"
		 << "videogame"
		 << "videogame_copy_info"
		 << "videogame_copy_info_myoid_seq"
		 << "videogame_myoid_seq"
		 << "member"
		 << "item_borrower"
		 << "item_borrower_myoid_seq"
		 << "member_history"
		 << "member_history_myoid_seq"
		 << "item_request"
		 << "item_request_myoid_seq"
		 << "locations"
		 << "monetary_units"
		 << "languages"
		 << "cd_formats"
		 << "dvd_ratings"
		 << "dvd_aspect_ratios"
		 << "dvd_regions"
		 << "minimum_days"
		 << "videogame_ratings"
		 << "videogame_platforms";
    }

  if(roles.contains("administrator") || roles.contains("librarian"))
    {
      privlist << "DELETE, INSERT, SELECT, UPDATE"
	       << "DELETE, INSERT, SELECT, UPDATE"
	       << "SELECT, UPDATE, USAGE"
	       << "SELECT, UPDATE, USAGE"
	       << "DELETE, INSERT, SELECT, UPDATE"
	       << "DELETE, INSERT, SELECT, UPDATE"
	       << "SELECT, UPDATE, USAGE"
	       << "SELECT, UPDATE, USAGE"
	       << "DELETE, INSERT, SELECT, UPDATE"
	       << "DELETE, INSERT, SELECT, UPDATE"
	       << "DELETE, INSERT, SELECT, UPDATE"
	       << "SELECT, UPDATE, USAGE"
	       << "SELECT, UPDATE, USAGE"
	       << "DELETE, INSERT, SELECT, UPDATE"
	       << "DELETE, INSERT, SELECT, UPDATE"
	       << "SELECT, UPDATE, USAGE"
	       << "SELECT, UPDATE, USAGE"
	       << "DELETE, INSERT, SELECT, UPDATE"
	       << "DELETE, INSERT, SELECT, UPDATE"
	       << "SELECT, UPDATE, USAGE"
	       << "SELECT, UPDATE, USAGE"
	       << "DELETE, INSERT, SELECT, UPDATE"
	       << "DELETE, INSERT, SELECT, UPDATE"
	       << "SELECT, UPDATE, USAGE"
	       << "SELECT, UPDATE, USAGE"
	       << "SELECT"
	       << "SELECT, UPDATE, USAGE"
	       << "DELETE, INSERT, SELECT, UPDATE"
	       << "DELETE, INSERT, SELECT, UPDATE"
	       << "DELETE, INSERT, SELECT, UPDATE"
	       << "DELETE, INSERT, SELECT, UPDATE"
	       << "DELETE, INSERT, SELECT, UPDATE"
	       << "DELETE, INSERT, SELECT, UPDATE"
	       << "DELETE, INSERT, SELECT, UPDATE"
	       << "DELETE, INSERT, SELECT, UPDATE"
	       << "DELETE, INSERT, SELECT, UPDATE"
	       << "DELETE, INSERT, SELECT, UPDATE";
      objectlist << "book"
		 << "book_copy_info"
		 << "book_copy_info_myoid_seq"
		 << "book_myoid_seq"
		 << "cd"
		 << "cd_copy_info"
		 << "cd_copy_info_myoid_seq"
		 << "cd_myoid_seq"
		 << "cd_songs"
		 << "dvd"
		 << "dvd_copy_info"
		 << "dvd_copy_info_myoid_seq"
		 << "dvd_myoid_seq"
		 << "journal"
		 << "journal_copy_info"
		 << "journal_copy_info_myoid_seq"
		 << "journal_myoid_seq"
		 << "magazine"
		 << "magazine_copy_info"
		 << "magazine_copy_info_myoid_seq"
		 << "magazine_myoid_seq"
		 << "videogame"
		 << "videogame_copy_info"
		 << "videogame_copy_info_myoid_seq"
		 << "videogame_myoid_seq"
		 << "item_request"
		 << "item_request_myoid_seq"
		 << "locations"
		 << "monetary_units"
		 << "languages"
		 << "cd_formats"
		 << "dvd_ratings"
		 << "dvd_aspect_ratios"
		 << "dvd_regions"
		 << "minimum_days"
		 << "videogame_ratings"
		 << "videogame_platforms";
    }

  if(roles.contains("administrator") || roles.contains("membership"))
    {
      privlist << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "DELETE, INSERT, SELECT, UPDATE"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT"
	       << "SELECT";
      objectlist << "book"
		 << "book_copy_info"
		 << "book_copy_info_myoid_seq"
		 << "book_myoid_seq"
		 << "cd"
		 << "cd_copy_info"
		 << "cd_copy_info_myoid_seq"
		 << "cd_myoid_seq"
		 << "cd_songs"
		 << "dvd"
		 << "dvd_copy_info"
		 << "dvd_copy_info_myoid_seq"
		 << "dvd_myoid_seq"
		 << "journal"
		 << "journal_copy_info"
		 << "journal_copy_info_myoid_seq"
		 << "journal_myoid_seq"
		 << "magazine"
		 << "magazine_copy_info"
		 << "magazine_copy_info_myoid_seq"
		 << "magazine_myoid_seq"
		 << "videogame"
		 << "videogame_copy_info"
		 << "videogame_copy_info_myoid_seq"
		 << "videogame_myoid_seq"
		 << "member"
		 << "locations"
		 << "monetary_units"
		 << "languages"
		 << "cd_formats"
		 << "dvd_ratings"
		 << "dvd_aspect_ratios"
		 << "dvd_regions"
		 << "minimum_days"
		 << "videogame_ratings"
		 << "videogame_platforms";
    }

  if(objectlist.size() != privlist.size())
    errorstr = QObject::tr
      ("Application error: objectlist.size() != privlist.size().");
  else
    for(i = 0; i < objectlist.size(); i++)
      {
	querystr = QString("GRANT %1 ON %2 TO %3").arg(privlist[i]).arg
	  (objectlist[i]).arg(userid);

	if(!query.exec(querystr))
	  break;
      }

  privlist.clear();
  objectlist.clear();

  if(!query.lastError().isValid())
    if(roles.contains("administrator") || roles.contains("membership"))
      {
	querystr = QString("ALTER USER %1 CREATEUSER").arg(userid);
	(void) query.exec(querystr);
      }

  if(query.lastError().isValid())
    errorstr = query.lastError().text();
}

/*
** -- revokeAll() --
*/

void misc_functions::revokeAll(const QString &userid,
			       const QSqlDatabase &db,
			       QString &errorstr)
{
  int i = 0;
  int count = 0;
  QString querystr = "";
  QSqlQuery query(db);
  QStringList objectlist;

  errorstr = "";

  if(db.driverName() == "QSQLITE")
    return; // Users are not supported.

  count = userCount(userid, db, errorstr);

  if(count > 0)
    {
      objectlist << "admin"
		 << "book"
		 << "item_borrower"
		 << "item_borrower_myoid_seq"
		 << "item_borrower_vw"
		 << "item_request"
		 << "item_request_myoid_seq"
		 << "book_copy_info"
		 << "book_copy_info_myoid_seq"
		 << "book_myoid_seq"
		 << "cd"
		 << "cd_copy_info"
		 << "cd_copy_info_myoid_seq"
		 << "cd_myoid_seq"
		 << "cd_songs"
		 << "dvd"
		 << "dvd_copy_info"
		 << "dvd_copy_info_myoid_seq"
		 << "dvd_myoid_seq"
		 << "journal"
		 << "journal_copy_info"
		 << "journal_copy_info_myoid_seq"
		 << "journal_myoid_seq"
		 << "magazine"
		 << "magazine_copy_info"
		 << "magazine_copy_info_myoid_seq"
		 << "magazine_myoid_seq"
		 << "member"
		 << "member_history"
		 << "member_history_myoid_seq"
		 << "videogame"
		 << "videogame_copy_info"
		 << "videogame_copy_info_myoid_seq"
		 << "videogame_myoid_seq"
		 << "locations"
		 << "monetary_units"
		 << "languages"
		 << "cd_formats"
		 << "dvd_ratings"
		 << "dvd_aspect_ratios"
		 << "dvd_regions"
		 << "minimum_days"
		 << "videogame_ratings"
		 << "videogame_platforms";

      for(i = 0; i < objectlist.size(); i++)
	{
	  querystr = QString
	    ("REVOKE ALL ON %1 FROM %2").arg
	    (objectlist[i]).arg(userid);

	  if(!query.exec(querystr))
	    break;
	}

      objectlist.clear();

      if(!query.lastError().isValid())
	{
	  querystr = QString("ALTER USER %1 NOCREATEUSER").arg(userid);
	  (void) query.exec(querystr);
	}
    }

  if(query.lastError().isValid())
    errorstr = query.lastError().text();
}

/*
** -- DBAccount() --
*/

void misc_functions::DBAccount(const QString &userid,
			       const QSqlDatabase &db,
			       const int action,
			       QString &errorstr,
			       const QString &roles)
{
  int i = 0;
  int count = 0;
  QString querystr = "";
  QSqlQuery query(db);
  QStringList objectlist;

  errorstr = "";

  if(db.driverName() == "QSQLITE")
    return; // Users are not supported.

  if(action == CREATE_USER)
    {
      /*
      ** Does the user exist?
      */

      count = userCount(userid, db, errorstr);

      if(count == 0)
	{
	  if(roles.contains("administrator") ||
	     roles.contains("membership"))
	    querystr = QString
	      ("CREATE USER %1 ENCRYPTED PASSWORD 'tempPass' "
	       "createuser").arg(userid);
	  else
	    querystr = QString
	      ("CREATE USER %1 ENCRYPTED PASSWORD 'tempPass'").arg
	      (userid);

	  (void) query.exec(querystr);
	}
      else if(!errorstr.isEmpty())
	return;
    }

  if(!query.lastError().isValid())
    {
      if(action == CREATE_USER || action == UPDATE_USER)
	{
	  if(roles.contains("administrator"))
	    objectlist << "admin"
		       << "book"
		       << "item_borrower"
		       << "item_borrower_myoid_seq"
		       << "item_borrower_vw"
		       << "item_request"
		       << "item_request_myoid_seq"
		       << "book_copy_info"
		       << "book_copy_info_myoid_seq"
		       << "book_myoid_seq"
		       << "cd"
		       << "cd_copy_info"
		       << "cd_copy_info_myoid_seq"
		       << "cd_myoid_seq"
		       << "cd_songs"
		       << "dvd"
		       << "dvd_copy_info"
		       << "dvd_copy_info_myoid_seq"
		       << "dvd_myoid_seq"
		       << "journal"
		       << "journal_copy_info"
		       << "journal_copy_info_myoid_seq"
		       << "journal_myoid_seq"
		       << "magazine"
		       << "magazine_copy_info"
		       << "magazine_copy_info_myoid_seq"
		       << "magazine_myoid_seq"
		       << "member"
		       << "member_history"
		       << "member_history_myoid_seq"
		       << "videogame"
		       << "videogame_copy_info"
		       << "videogame_copy_info_myoid_seq"
		       << "videogame_myoid_seq"
		       << "locations"
		       << "monetary_units"
		       << "languages"
		       << "cd_formats"
		       << "dvd_ratings"
		       << "dvd_aspect_ratios"
		       << "dvd_regions"
		       << "minimum_days"
		       << "videogame_ratings"
		       << "videogame_platforms";
	  else if(roles.contains("circulation") ||
		  roles.contains("librarian") ||
		  roles.contains("membership"))
	    {
	      /*
	      ** The method grantPrivs() grants the necessary privileges.
	      */
	    }
	  else // Member.
	    objectlist << "admin"
		       << "book"
		       << "book_myoid_seq"
		       << "book_copy_info"
		       << "book_copy_info_myoid_seq"
		       << "cd"
		       << "cd_myoid_seq"
		       << "cd_songs"
		       << "cd_copy_info"
		       << "cd_copy_info_myoid_seq"
		       << "dvd"
		       << "dvd_myoid_seq"
		       << "dvd_copy_info"
		       << "dvd_copy_info_myoid_seq"
		       << "journal"
		       << "journal_copy_info"
		       << "journal_copy_info_myoid_seq"
		       << "journal_myoid_seq"
		       << "magazine"
		       << "magazine_myoid_seq"
		       << "magazine_copy_info"
		       << "magazine_copy_info_myoid_seq"
		       << "videogame"
		       << "videogame_myoid_seq"
		       << "videogame_copy_info"
		       << "videogame_copy_info_myoid_seq"
		       << "item_borrower_vw"
		       << "member_history"
		       << "member_history_myoid_seq"
		       << "item_request"
		       << "item_request_myoid_seq"
		       << "locations"
		       << "monetary_units"
		       << "languages"
		       << "cd_formats"
		       << "dvd_ratings"
		       << "dvd_aspect_ratios"
		       << "dvd_regions"
		       << "videogame_ratings"
		       << "videogame_platforms";
	}
      else /* Delete the account. */
	objectlist << "admin"
		   << "book"
		   << "item_borrower"
		   << "item_borrower_myoid_seq"
		   << "item_borrower_vw"
		   << "item_request"
		   << "item_request_myoid_seq"
		   << "book_copy_info"
		   << "book_copy_info_myoid_seq"
		   << "book_myoid_seq"
		   << "cd"
		   << "cd_copy_info"
		   << "cd_copy_info_myoid_seq"
		   << "cd_myoid_seq"
		   << "cd_songs"
		   << "dvd"
		   << "dvd_copy_info"
		   << "dvd_copy_info_myoid_seq"
		   << "dvd_myoid_seq"
		   << "journal"
		   << "journal_copy_info"
		   << "journal_copy_info_myoid_seq"
		   << "journal_myoid_seq"
		   << "magazine"
		   << "magazine_copy_info"
		   << "magazine_copy_info_myoid_seq"
		   << "magazine_myoid_seq"
		   << "member"
		   << "member_history"
		   << "member_history_myoid_seq"
		   << "videogame"
		   << "videogame_copy_info"
		   << "videogame_copy_info_myoid_seq"
		   << "videogame_myoid_seq"
		   << "locations"
		   << "monetary_units"
		   << "languages"
		   << "cd_formats"
		   << "dvd_ratings"
		   << "dvd_aspect_ratios"
		   << "dvd_regions"
		   << "minimum_days"
		   << "videogame_ratings"
		   << "videogame_platforms";

      for(i = 0; i < objectlist.size(); i++)
	{
	  if(action == CREATE_USER || action == UPDATE_USER)
	    {
	      if(roles.contains("administrator"))
		{
		  if(objectlist[i].endsWith("_vw"))
		    querystr = QString
		      ("GRANT SELECT ON %1 TO %2").arg
		      (objectlist[i]).arg(userid);
		  else if(objectlist[i] == "item_request")
		    querystr = QString
		      ("GRANT DELETE, SELECT ON %1 TO %2").arg
		      (objectlist[i]).arg(userid);
		  else if(objectlist[i] == "item_request_myoid_seq")
		    querystr = QString
		      ("GRANT SELECT, UPDATE, USAGE ON %1 TO %2").arg
		      (objectlist[i]).arg(userid);
		  else if(objectlist[i].endsWith("_seq"))
		    querystr = QString
		      ("GRANT SELECT, UPDATE, USAGE ON %1 TO %2").arg
		      (objectlist[i]).arg(userid);
		  else
		    querystr = QString
		      ("GRANT DELETE, INSERT, SELECT, UPDATE ON %1 TO %2").arg
		      (objectlist[i]).arg(userid);
		}
	      else if(roles.contains("circulation") ||
		      roles.contains("librarian") ||
		      roles.contains("membership"))
		{
		  /*
		  ** The method grantPrivs() grants the necessary privileges
		  ** to circulation, librarian, and membership administrators.
		  */
		}
	      else if(objectlist[i] == "item_request")
		querystr = QString
		  ("GRANT DELETE, INSERT, SELECT ON %1 TO %2").arg
		  (objectlist[i]).arg(userid);
	      else if(objectlist[i] == "item_request_myoid_seq")
		querystr = QString
		  ("GRANT SELECT, UPDATE, USAGE ON %1 TO %2").arg
		  (objectlist[i]).arg(userid);
	      else if(objectlist[i] == "admin")
		querystr = QString("REVOKE ALL ON %1 FROM %2").arg
		  (objectlist[i]).arg(userid);
	      else
		querystr = QString
		  ("GRANT SELECT ON %1 TO %2").arg
		  (objectlist[i]).arg(userid);
	    }
	  else
	    querystr = QString
	      ("REVOKE ALL ON %1 FROM %2").arg
	      (objectlist[i]).arg(userid);

	  if(!query.exec(querystr))
	    {
	      errorstr = query.lastError().text();
	      break;
	    }
	}

      objectlist.clear();

      if(action == CREATE_USER && !query.lastError().isValid())
	if(roles.contains("circulation") || roles.contains("librarian") ||
	   roles.contains("membership"))
	  {
	    grantPrivs(userid, roles, db, errorstr);

	    if(!errorstr.isEmpty())
	      return;
	  }
    }

  if(action == DELETE_USER)
    if(!query.lastError().isValid())
      (void) query.exec(QString("DROP USER %1").arg(userid));

  if(errorstr.isEmpty() &&
     query.lastError().isValid())
    errorstr = query.lastError().text();
}

/*
** -- savePassword() --
*/

void misc_functions::savePassword(const QString &userid,
				  const QSqlDatabase &db,
				  const QString &password, QString &errorstr)
{
  QString querystr = "";
  QSqlQuery query(db);

  errorstr = "";

  if(db.driverName() == "QSQLITE")
    return; // Users are not supported.

  querystr = QString("ALTER USER %1 ENCRYPTED "
		     "PASSWORD '%2'").arg(userid).arg(password);
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
	  date.toString(Qt::SystemLocaleShortDate);
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
    if(query.next())
      count = query.value(0).toInt();

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
  itemType = itemTypeArg;
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

QString misc_functions::getColumnString(const QTableWidget *table,
					const int row,
					const QString &columnName)
{
  int i = 0;
  QString str = "";
  QTableWidgetItem *column = 0;

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

int misc_functions::getColumnNumber(const QTableWidget *table,
				    const QString &columnName)
{
  int i = 0;
  int num = -1;
  QTableWidgetItem *column = 0;

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

void misc_functions::updateColumn(QTableWidget *table,
				  const int row, const QString &columnName,
				  const QString &value)
{
  int i = 0;
  bool sortingEnabled = false;
  QTableWidgetItem *column = 0;

  if(row >= 0 && row < table->rowCount())
    {
      if(table->isSortingEnabled())
	sortingEnabled = true;

      for(i = 0; i < table->columnCount(); i++)
	{
	  column = table->horizontalHeaderItem(i);

	  if(column == 0)
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
  itemType = itemTypeArg;
  querystr = QString("SELECT COUNT(myoid) FROM item_borrower_vw "
		     "WHERE item_oid = ? AND type = '%1'").arg(itemType);
  query.prepare(querystr);
  query.bindValue(0, oid);

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
  itemType = itemTypeArg;
  querystr = QString("SELECT count(copyid) FROM item_borrower_vw WHERE "
		     "copyid = ? AND item_oid = ? AND "
		     "type = '%1'").arg(itemType);
  query.prepare(querystr);
  query.bindValue(0, copyid);
  query.bindValue(1, oid);

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
  itemType = itemTypeArg;
  querystr = QString("SELECT MAX(copy_number) FROM item_borrower_vw "
		     "WHERE item_oid = ? AND type = '%1'").arg(itemType);
  query.prepare(querystr);
  query.bindValue(0, oid);

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
  itemType = itemTypeArg;
  querystr = QString("SELECT COUNT(myoid) FROM %1_copy_info "
		     "WHERE copyid = ? AND item_oid = ? "
		     "AND copyid NOT IN (SELECT copyid FROM item_borrower_vw "
		     "WHERE item_oid = ? AND type = '%2')").arg
    (itemType.toLower().remove(" ")).arg(itemType);
  query.prepare(querystr);
  query.bindValue(0, copyid);
  query.bindValue(1, oid);
  query.bindValue(2, oid);

  if(query.exec())
    if(query.next())
      if(query.value(0).toInt() >= 1)
	isAvailable = true;

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
    querystr = QString("SELECT myoid FROM %1 WHERE id = ? AND "
		       "issuevolume = ? AND issueno = ?").arg(itemType);
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
    if(query.next())
      oid = query.value(0).toString();

  if(query.lastError().isValid())
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
  QSqlQuery query(db);

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
    qmain->addError
      (QString(QObject::tr("Database Error")),
       QString(QObject::tr("Unable to determine the query size.")),
       query.lastError().text(), file, line);

  return count;
}

/*
** -- center() --
*/

void misc_functions::center(QMainWindow *child, QMainWindow *parent)
{
  int X = 0;
  int Y = 0;
  QPoint p(0, 0);

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

void misc_functions::hideAdminFields(QMainWindow *window, const QString &roles)
{
  bool showWidgets = true;
  QString str = "";

  if(roles.isEmpty())
    showWidgets = false;
  else if(roles.contains("administrator") ||
	  roles.contains("librarian"))
    showWidgets = true;
  else
    showWidgets = false;

  foreach(QWidget *widget, window->findChildren<QWidget *>())
    {
      str = (static_cast<QObject *> (widget))->objectName().toLower();

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

bool misc_functions::isRequested(const QSqlDatabase &db,
				 const QString &oid,
				 const QString &itemTypeArg,
				 QString &errorstr)
{
  bool isRequested = false;
  QString str = "";
  QString itemType = "";
  QString querystr = "";
  QSqlQuery query(db);

  errorstr = "";

  if(db.driverName() == "QSQLITE")
    return isRequested; // Requests are not supported.

  itemType = itemTypeArg;
  querystr = QString("SELECT COUNT(myoid) FROM item_request "
		     "WHERE item_oid = ? AND type = '%1'").arg(itemType);
  query.prepare(querystr);
  query.bindValue(0, oid);

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

QStringList misc_functions::getLocations(const QSqlDatabase &db,
					 const QString &type,
					 QString &errorstr)
{
  QString querystr("");
  QSqlQuery query(db);
  QStringList locations;

  errorstr = "";

  if(type.isEmpty())
    querystr = "SELECT DISTINCT(location) FROM locations "
      "WHERE LENGTH(TRIM(location)) > 0 "
      "ORDER BY location";
  else
    querystr = QString("SELECT location FROM locations WHERE type = '%1' AND "
		       "LENGTH(TRIM(location)) > 0 "
		       "ORDER BY location").arg(type);

  if(query.exec(querystr))
    while(query.next())
      locations.append(query.value(0).toString());

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return locations;
}

/*
** -- getLocations() --
*/

QList<QPair<QString, QString> > misc_functions::getLocations
(const QSqlDatabase &db,
 QString &errorstr)
{
  QString querystr("");
  QSqlQuery query(db);
  QList<QPair<QString, QString> > locations;

  errorstr = "";
  querystr = "SELECT type, location FROM locations "
    "WHERE LENGTH(TRIM(type)) > 0 AND "
    "LENGTH(TRIM(location)) > 0 "
    "ORDER BY type, location";

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

QStringList misc_functions::getMonetaryUnits(const QSqlDatabase &db,
					     QString &errorstr)
{
  QString querystr("");
  QSqlQuery query(db);
  QStringList monetary_units;

  errorstr = "";
  querystr = "SELECT monetary_unit FROM monetary_units "
    "WHERE LENGTH(TRIM(monetary_unit)) > 0 "
    "ORDER BY monetary_unit";

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

QStringList misc_functions::getLanguages(const QSqlDatabase &db,
					 QString &errorstr)
{
  QString querystr("");
  QSqlQuery query(db);
  QStringList languages;

  errorstr = "";
  querystr = "SELECT language FROM languages "
    "WHERE LENGTH(TRIM(language)) > 0 "
    "ORDER BY language";

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

QStringList misc_functions::getCDFormats(const QSqlDatabase &db,
					 QString &errorstr)
{
  QString querystr("");
  QSqlQuery query(db);
  QStringList formats;

  errorstr = "";
  querystr = "SELECT cd_format FROM cd_formats "
    "WHERE LENGTH(TRIM(cd_format)) > 0 "
    "ORDER BY cd_format";

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

QStringList misc_functions::getDVDRatings(const QSqlDatabase &db,
					  QString &errorstr)
{
  QString querystr("");
  QSqlQuery query(db);
  QStringList ratings;

  errorstr = "";
  querystr = "SELECT dvd_rating FROM dvd_ratings "
    "WHERE LENGTH(TRIM(dvd_rating)) > 0 "
    "ORDER BY dvd_rating";

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

QStringList misc_functions::getDVDAspectRatios(const QSqlDatabase &db,
					       QString &errorstr)
{
  QString querystr("");
  QSqlQuery query(db);
  QStringList aspectratios;

  errorstr = "";
  querystr = "SELECT dvd_aspect_ratio FROM dvd_aspect_ratios "
    "WHERE LENGTH(TRIM(dvd_aspect_ratio)) > 0 "
    "ORDER BY dvd_aspect_ratio";

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

QStringList misc_functions::getDVDRegions(const QSqlDatabase &db,
					  QString &errorstr)
{
  QString querystr("");
  QSqlQuery query(db);
  QStringList regions;

  errorstr = "";
  querystr = "SELECT dvd_region FROM dvd_regions "
    "WHERE LENGTH(TRIM(dvd_region)) > 0 "
    "ORDER BY dvd_region";

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

int misc_functions::getMinimumDays(const QSqlDatabase &db,
				   const QString &type,
				   QString &errorstr)
{
  int minimumdays = 1;
  QString querystr("");
  QSqlQuery query(db);

  errorstr = "";
  querystr = QString("SELECT days FROM minimum_days WHERE type = '%1'").
    arg(type);

  if(query.exec(querystr))
    if(query.next())
      minimumdays = query.value(0).toInt();

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return minimumdays;
}

/*
** -- getMinimumDays() --
*/

QStringList misc_functions::getMinimumDays(const QSqlDatabase &db,
					   QString &errorstr)
{
  QString querystr("");
  QSqlQuery query(db);
  QStringList minimumdays;
  QMap<QString, QString> map;

  errorstr = "";
  querystr = "SELECT type, days FROM minimum_days "
    "WHERE LENGTH(TRIM(type)) > 0 "
    "ORDER BY type";

  if(query.exec(querystr))
    while(query.next())
      if(query.value(0).toString() == "CD")
	map["Music CD"] = query.value(1).toString();
      else
	map[query.value(0).toString()] = query.value(1).toString();

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

QStringList misc_functions::getVideoGameRatings(const QSqlDatabase &db,
						QString &errorstr)
{
  QString querystr("");
  QSqlQuery query(db);
  QStringList ratings;

  errorstr = "";
  querystr = "SELECT videogame_rating FROM videogame_ratings "
    "WHERE LENGTH(TRIM(videogame_rating)) > 0 "
    "ORDER BY videogame_rating";

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

QStringList misc_functions::getVideoGamePlatforms(const QSqlDatabase &db,
						  QString &errorstr)
{
  QString querystr("");
  QSqlQuery query(db);
  QStringList platforms;

  errorstr = "";
  querystr = "SELECT videogame_platform FROM videogame_platforms "
    "WHERE LENGTH(TRIM(videogame_platform)) > 0 "
    "ORDER BY videogame_platform";

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

qint64 misc_functions::getSqliteUniqueId(const QSqlDatabase &db,
					 QString &errorstr)
{
  qint64 value = -1;
  QString querystr("");
  QSqlQuery query(db);

  if(db.driverName() != "QSQLITE")
    return value;

  errorstr = "";
  querystr = "INSERT INTO sequence VALUES (NULL)";

  if(query.exec(querystr))
    {
      querystr = "SELECT MAX(value) FROM sequence";

      if(query.exec(querystr))
	if(query.next())
	  value = query.value(0).toInt();
    }

  if(query.lastError().isValid())
    errorstr = query.lastError().text();

  return value;
}

/*
** -- hasMemberExpired() --
*/

bool misc_functions::hasMemberExpired(const QSqlDatabase &db,
				      const QString &memberid,
				      QString &errorstr)
{
  bool expired = true;
  QString querystr("");
  QSqlQuery query(db);

  errorstr = "";

  if(db.driverName() == "QSQLITE")
    querystr = QString("SELECT expiration_date "
		       "FROM member WHERE memberid = '%1'").
      arg(memberid);
  else
    querystr = QString("SELECT TO_DATE(expiration_date, 'mm/dd/yyyy') - "
		       "current_date FROM member WHERE memberid = '%1'").
      arg(memberid);

  if(query.exec(querystr))
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
	    if(query.value(0).toInt() < 0)
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

void misc_functions::updateSQLiteDatabase(const QSqlDatabase &db)
{
  if(db.driverName() != "QSQLITE")
    return;

  QString version(BIBLIOTEQ_VERSION);

  if(version == "6.46")
    {
      QString querystr("");
      QSqlQuery query(db);

      querystr = "ALTER TABLE member ADD expiration_date "
	"VARCHAR(32) NOT NULL DEFAULT '01/01/3000'";
      query.exec(querystr);
    }
  else if(version == "6.51")
    {
      QString querystr("");
      QSqlQuery query(db);
      QStringList tables;

      tables << "book"
	     << "cd"
	     << "dvd"
	     << "journal"
	     << "magazine"
	     << "videogame";

      while(!tables.isEmpty())
	{
	  QString querystr;

	  querystr = QString("ALTER TABLE %1 ADD keyword TEXT").
	    arg(tables.takeFirst());
	  query.exec(querystr);
	}

      querystr = "CREATE TABLE book_backup"
	"("
	"id		 VARCHAR(32) UNIQUE,"
	"myoid		 BIGINT NOT NULL,"
	"title		 TEXT NOT NULL,"
	"edition		 VARCHAR(8) NOT NULL,"
	"author		 TEXT NOT NULL,"
	"pdate		 VARCHAR(32) NOT NULL,"
	"publisher	 TEXT NOT NULL,"
	"place		 TEXT NOT NULL,"
	"category	 TEXT NOT NULL,"
	"price		 NUMERIC(10, 2) NOT NULL DEFAULT 0.00,"
	"description	 TEXT NOT NULL,"
	"language	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',"
	"monetary_units	 VARCHAR(64) NOT NULL DEFAULT 'UNKNOWN',"
	"quantity	 INTEGER NOT NULL DEFAULT 1,"
	"binding_type	 VARCHAR(32) NOT NULL,"
	"location	 TEXT NOT NULL,"
	"isbn13		 VARCHAR(16) UNIQUE,"
	"lccontrolnumber	 VARCHAR(64),"
	"callnumber	 VARCHAR(64),"
	"deweynumber	 VARCHAR(64),"
	"front_cover	 BYTEA,"
	"back_cover	 BYTEA,"
	"marc_tags	 TEXT,"
	"keyword		 TEXT,"
	"type		 VARCHAR(16) NOT NULL DEFAULT 'Book'"
	")";
      query.exec(querystr);
      query.exec("INSERT INTO book_backup SELECT * FROM book");
      query.exec("DROP TABLE book");
      query.exec("ALTER TABLE book_backup RENAME TO book");
    }
}
