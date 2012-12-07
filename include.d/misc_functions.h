#ifndef _MISC_FUNCTIONS_H_
#define _MISC_FUNCTIONS_H_

/*
** -- C++ Includes --
*/

#include <iostream>
using namespace std;

/*
** -- Qt Includes --
*/

#include <QPair>
#include <QString>
#include <QVariant>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QTableWidget>
#include <QTableWidgetItem>

class QMainWindow;

class misc_functions
{
 public:
  static const int CREATE_USER = 100;
  static const int DELETE_USER = 200;
  static const int UPDATE_USER = 300;
  static int userCount(const QString &, const QSqlDatabase &, QString &);
  static int getMinimumDays(const QSqlDatabase &,
			    const QString &,
			    QString &);
  static int getColumnNumber(const QTableWidget *, const QString &);
  static int sqliteQuerySize(const QString &, const QSqlDatabase &,
			     const char *, const int);
  static int getMaxCopyNumber(const QSqlDatabase &, const QString &,
			      const QString &, QString &);
  static int getMemberMatchCount(const QString &, const QString &,
				 const QSqlDatabase &,
				 QString &);
  static bool isRequested(const QSqlDatabase &, const QString &,
			  const QString &, QString &);
  static bool isCheckedOut(const QSqlDatabase &, const QString &,
			   const QString &, QString &);
  static bool isCopyAvailable(const QSqlDatabase &, const QString &,
			      const QString &, const QString &, QString &);
  static bool hasMemberExpired(const QSqlDatabase &db,
			       const QString &memberid,
			       QString &errorstr);
  static bool isCopyCheckedOut(const QSqlDatabase &, const QString &,
			       const QString &, const QString &, QString &);
  static void center(QMainWindow *, QMainWindow *);
  static void setRole(const QSqlDatabase &,
		      QString &,
		      const QString &);
  static void DBAccount(const QString &, const QSqlDatabase &,
			const int, QString &,
			const QString & = "");
  static void revokeAll(const QString &, const QSqlDatabase &,
			QString &);
  static void grantPrivs(const QString &, const QString &,
			 const QSqlDatabase &, QString &);
  static void savePassword(const QString &, const QSqlDatabase &,
			   const QString &, QString &, const QString &);
  static void saveQuantity(const QSqlDatabase &, const QString &,
			   const int, const QString &, QString &);
  static void updateColumn(QTableWidget *, const int, const int,
			   const QString &);
  static void hideAdminFields(QMainWindow *, const QString &);
  static void highlightWidget(QWidget *, const QColor &);
  static void createInitialCopies(QString const &, const int,
				  const QSqlDatabase &,
				  const QString &, QString &);
  static void updateSQLiteDatabase(const QSqlDatabase &);
  static qint64 getSqliteUniqueId(const QSqlDatabase &,
				  QString &);
  static QImage getImage(const QString &, const QString &, const QString &,
			 const QSqlDatabase &);
  static QString getOID(const QString &, const QString &,
			const QSqlDatabase &, QString &);
  static QString getRoles(const QSqlDatabase &, const QString &, QString &);
  static QString getMemberName(const QSqlDatabase &,
			       const QString &,
			       QString &);
  static QString getAbstractInfo(const QString &,
				 const QString &,
				 const QSqlDatabase &);
  static QString getColumnString(const QTableWidget *,
				 const int,
				 const int);
  static QString getColumnString(const QTableWidget *, const int,
				 const QString &);
  static QString getAvailability(const QString &, const QSqlDatabase &,
				 const QString &, QString &);
  static QStringList getCDFormats(const QSqlDatabase &,
				  QString &);
  static QStringList getLanguages(const QSqlDatabase &,
				  QString &);
  static QStringList getLocations(const QSqlDatabase &,
				  const QString &,
				  QString &);
  static QStringList getDVDRatings(const QSqlDatabase &,
				   QString &);
  static QStringList getDVDRegions(const QSqlDatabase &,
				   QString &);
  static QStringList getMinimumDays(const QSqlDatabase &,
				    QString &);
  static QStringList getMonetaryUnits(const QSqlDatabase &,
				      QString &);
  static QStringList getReservedItems(const QString &,
				      const QSqlDatabase &,
				      QString &);
  static QStringList getDVDAspectRatios(const QSqlDatabase &,
					QString &);
  static QStringList getVideoGameRatings(const QSqlDatabase &,
					 QString &);
  static QStringList getVideoGamePlatforms(const QSqlDatabase &,
					   QString &);
  static QMap<QString, QString> getItemsReservedCounts(const QSqlDatabase &,
						       const QString &,
						       QString &);
  static QList<QPair<QString, QString> > getLocations(const QSqlDatabase &,
						      QString &);
};

#endif
