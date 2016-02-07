#ifndef _BIBLIOTEQ_MISC_FUNCTIONS_H_
#define _BIBLIOTEQ_MISC_FUNCTIONS_H_

/*
** -- C++ Includes --
*/

#include <iostream>

using namespace std;

/*
** -- Qt Includes --
*/

#include <QPair>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVariant>

class QMainWindow;

class biblioteq_misc_functions
{
 public:
  static const int CREATE_USER = 100;
  static const int DELETE_USER = 200;
  static const int UPDATE_USER = 300;
  static QImage getImage(const QString &, const QString &,
			 const QString &, const QSqlDatabase &);
  static QList<QPair<QString, QString> > getLocations
    (const QSqlDatabase &, QString &);
  static QMap<QString, QString> getItemsReservedCounts
    (const QSqlDatabase &, const QString &, QString &);
  static QString getAbstractInfo(const QString &, const QString &,
				 const QSqlDatabase &);
  static QString getAvailability(const QString &, const QSqlDatabase &,
				 const QString &, QString &);
  static QString getColumnString(const QTableWidget *, const int,
				 const QString &);
  static QString getColumnString(const QTableWidget *, const int, const int);
  static QString getMemberName(const QSqlDatabase &,
			       const QString &, QString &);
  static QString getOID(const QString &, const QString &,
			const QSqlDatabase &, QString &);
  static QString getRoles(const QSqlDatabase &, const QString &, QString &);
  static QString imageFormatGuess(const QByteArray &bytes);
  static QStringList getBookBindingTypes(const QSqlDatabase &, QString &);
  static QStringList getCDFormats(const QSqlDatabase &, QString &);
  static QStringList getDVDAspectRatios(const QSqlDatabase &, QString &);
  static QStringList getDVDRatings(const QSqlDatabase &, QString &);
  static QStringList getDVDRegions(const QSqlDatabase &, QString &);
  static QStringList getLanguages(const QSqlDatabase &, QString &);
  static QStringList getLocations(const QSqlDatabase &, const QString &,
				  QString &);
  static QStringList getMinimumDays(const QSqlDatabase &, QString &);
  static QStringList getMonetaryUnits(const QSqlDatabase &, QString &);
  static QStringList getReservedItems(const QString &, const QSqlDatabase &,
				      QString &);
  static QStringList getVideoGamePlatforms(const QSqlDatabase &, QString &);
  static QStringList getVideoGameRatings(const QSqlDatabase &, QString &);
  static bool dnt(const QSqlDatabase &, const QString &, QString &);
  static bool hasMemberExpired(const QSqlDatabase &db,
			       const QString &memberid, QString &errorstr);
  static bool isCheckedOut(const QSqlDatabase &,
			   const QString &, const QString &, QString &);
  static bool isCopyAvailable(const QSqlDatabase &, const QString &,
			      const QString &, const QString &, QString &);
  static bool isCopyCheckedOut(const QSqlDatabase &, const QString &,
			       const QString &, const QString &, QString &);
  static bool isGnome(void);
  static bool isRequested(const QSqlDatabase &, const QString &,
			  const QString &, QString &);
  static int getColumnNumber(const QTableWidget *, const QString &);
  static int getMaxCopyNumber(const QSqlDatabase &, const QString &,
			      const QString &, QString &);
  static int getMinimumDays(const QSqlDatabase &, const QString &, QString &);
  static int sqliteQuerySize(const QString &, const QMap<QString, QVariant> &,
			     const QSqlDatabase &, const char *, const int);
  static int sqliteQuerySize(const QString &, const QSqlDatabase &,
			     const char *, const int);
  static qint64 getMemberMatchCount(const QString &, const QString &,
				    const QSqlDatabase &, QString &);
  static qint64 getSqliteUniqueId(const QSqlDatabase &, QString &);
  static qint64 userCount(const QString &, const QSqlDatabase &, QString &);
  static void DBAccount(const QString &, const QSqlDatabase &,
			const int, QString &, const QString & = "");
  static void center(QWidget *, QMainWindow *);
  static void createInitialCopies(QString const &, const int,
				  const QSqlDatabase &, const QString &,
				  QString &);
  static void exportPhotographs(const QSqlDatabase &, const QString &,
				const int, const QString &,
				QWidget *parent);
  static void grantPrivs(const QString &, const QString &,
			 const QSqlDatabase &, QString &);
  static void hideAdminFields(QMainWindow *, const QString &);
  static void highlightWidget(QWidget *, const QColor &);
  static void revokeAll(const QString &, const QSqlDatabase &, QString &);
  static void savePassword(const QString &, const QSqlDatabase &,
			   const QString &, QString &);
  static void saveQuantity(const QSqlDatabase &, const QString &,
			   const int, const QString &, QString &);
  static void setRole(const QSqlDatabase &, QString &, const QString &);
  static void updateColumn(QTableWidget *, const int, const int,
			   const QString &);
  static void updateSQLiteDatabase(const QSqlDatabase &);
};

#endif
