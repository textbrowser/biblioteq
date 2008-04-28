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

#include <QString>
#include <QVariant>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QTableWidget>
#include <QTableWidgetItem>

/*
** -- Local Includes --
*/

#include "qtbook.h"

class misc_functions
{
 public:
  static const int CREATE_USER = 100;
  static const int DELETE_USER = 200;
  static int userCount(const QString &, const QSqlDatabase &, QString &);
  static int getColumnNumber(const QTableWidget *, const QString &);
  static int sqliteQuerySize(const QString &, const QSqlDatabase &,
			     const char *, const int);
  static int getMaxCopyNumber(const QSqlDatabase &, const QString &,
			      const QString &, QString &);
  static int getMemberMatchCount(const QString &, const QString &,
				 const QSqlDatabase &, QString &);
  static bool isCheckedOut(const QSqlDatabase &, const QString &,
			   const QString &, QString &);
  static bool isCopyAvailable(const QSqlDatabase &, const QString &,
			      const QString &, const QString &, QString &);
  static bool isCopyCheckedOut(const QSqlDatabase &, const QString &,
			       const QString &, const QString &, QString &);
  static void revokeAll(const QString &, const QSqlDatabase &,
			QString &);
  static void savePassword(const QString &, const QSqlDatabase &,
			   const QString &, QString &);
  static void saveQuantity(const QSqlDatabase &, const QString &,
			   const int, const QString &, QString &);
  static void updateColumn(QTableWidget *, const int, const QString &,
			   const QString &);
  static void highlightWidget(QWidget *, const QColor &);
  static void createInitialCopies(QString const &, const int,
				  const QSqlDatabase &,
				  const QString &, QString &);
  static void createOrDeleteDBAccount(const QString &, const QSqlDatabase &,
				      const int, QString &,
				      const QString & = "");
  static QImage getImage(const QString &, const QString &, const QString &,
			 const QSqlDatabase &);
  static QString getOID(const QString &, const QString &,
			const QSqlDatabase &, QString &);
  static QString getRoles(const QSqlDatabase &, const QString &, QString &);
  static QString getMemberName(const QSqlDatabase &,
			       const QString &,
			       QString &);
  static QString getColumnString(const QTableWidget *, const int,
				 const QString &);
  static QString getAvailability(const QString &, const QSqlDatabase &,
				 const QString &, QString &);
  static QStringList getReservedItems(const QString &,
				      const QSqlDatabase &,
				      QString &);
  static QMap<QString, QString> getItemsReservedCounts(const QSqlDatabase &,
						       const QString &,
						       QString &);
};

#endif
