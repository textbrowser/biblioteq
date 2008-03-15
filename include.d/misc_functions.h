#ifndef _MISC_FUNCTIONS_H_
#define _MISC_FUNCTIONS_H_

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

class misc_functions
{
 public:
  static int getColumnNumber(const QTableWidget *, const QString &);
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
  static void saveQuantity(const QSqlDatabase &, const QString &,
			   const int, const QString &, QString &);
  static void updateColumn(QTableWidget *, const int, const QString &,
			   const QString &);
  static void createInitialCopies(QString const &, const int,
				  const QSqlDatabase &,
				  const QString &, QString &);
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
