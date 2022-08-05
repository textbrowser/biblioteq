#ifndef _BIBLIOTEQ_MISC_FUNCTIONS_H_
#define _BIBLIOTEQ_MISC_FUNCTIONS_H_

#include <QGraphicsItem>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTableWidget>

class QMainWindow;
class biblioteq;

class biblioteq_misc_functions
{
 public:
  static const int CREATE_USER = 100;
  static const int DELETE_USER = 200;
  static const int UPDATE_USER = 300;
  static QImage getImage(const QString &,
			 const QString &,
			 const QString &,
			 const QSqlDatabase &);
  static QList<QPair<QString, QString> > getLocations(const QSqlDatabase &,
						      QString &);
  static QList<int> selectedRows(QTableWidget *);
  static QMap<QString, qint64> getItemsReservedCounts(const QSqlDatabase &,
						      const QString &,
						      QString &);
  static QString accessionNumberAsSpecialText(const qint64 integer);
  static QString getAbstractInfo(const QString &,
				 const QString &,
				 const QSqlDatabase &);
  static QString getAvailability(const QString &,
				 const QSqlDatabase &,
				 const QString &,
				 QString &);
  static QString getColumnString(const QTableWidget *,
				 const int,
				 const QString &);
  static QString getColumnString(const QTableWidget *, const int, const int);
  static QString getMemberName(const QSqlDatabase &,
			       const QString &,
			       QString &);
  static QString getOID(const QString &,
			const QString &,
			const QSqlDatabase &,
			QString &);
  static QString getRoles(const QSqlDatabase &, const QString &, QString &);
  static QString getTotalReserved(const QSqlDatabase &,
				  const QString &,
				  const QString &);
  static QString imageFormatGuess(const QByteArray &bytes);
  static QString isbn10to13(const QString &text);
  static QString isbn13to10(const QString &text);
  static QString linearizeString(const QString &text);
  static QStringList getBookBindingTypes(const QSqlDatabase &, QString &);
  static QStringList getBookTargetAudiences(const QSqlDatabase &, QString &);
  static QStringList getCDFormats(const QSqlDatabase &, QString &);
  static QStringList getDVDAspectRatios(const QSqlDatabase &, QString &);
  static QStringList getDVDRatings(const QSqlDatabase &, QString &);
  static QStringList getDVDRegions(const QSqlDatabase &, QString &);
  static QStringList getGreyLiteratureTypes(const QSqlDatabase &db,
					    QString &errorstr);
  static QStringList getLanguages(const QSqlDatabase &, QString &);
  static QStringList getLocations(const QSqlDatabase &,
				  const QString &,
				  QString &);
  static QStringList getMinimumDays(const QSqlDatabase &, QString &);
  static QStringList getMonetaryUnits(const QSqlDatabase &, QString &);
  static QStringList getReservedItems(const QString &,
				      const QSqlDatabase &,
				      QString &);
  static QStringList getVideoGamePlatforms(const QSqlDatabase &, QString &);
  static QStringList getVideoGameRatings(const QSqlDatabase &, QString &);
  static bool dnt(const QSqlDatabase &, const QString &, QString &);
  static bool getMemberMatch(const QString &,
			     const QString &,
			     const QSqlDatabase &,
			     QString &);
  static bool hasMemberExpired(const QSqlDatabase &,
			       const QString &,
			       QString &);
  static bool hasUnaccentExtension(const QSqlDatabase &);

  static bool isBookRead(const QSqlDatabase &db, const quint64 myoid)
  {
    if(db.driverName() != "QSQLITE")
      return false;

    QSqlQuery query(db);

    query.setForwardOnly(true);
    query.prepare("SELECT book_read FROM book WHERE myoid = ?");
    query.addBindValue(myoid);

    if(query.exec() && query.next())
      return query.value(0).toBool();

    return false;
  }

  static bool isCheckedOut(const QSqlDatabase &,
			   const QString &,
			   const QString &,
			   QString &);
  static bool isCopyAvailable(const QSqlDatabase &,
			      const QString &,
			      const QString &,
			      const QString &,
			      QString &);
  static bool isCopyCheckedOut(const QSqlDatabase &,
			       const QString &,
			       const QString &,
			       const QString &,
			       QString &);
  static bool isGnome(void);
  static bool isItemAvailable
    (const QSqlDatabase &db, const QString &id, const QString &t);
  static bool isRequested(const QSqlDatabase &,
			  const QString &,
			  const QString &,
			  QString &);
  static bool userExists(const QString &, const QSqlDatabase &, QString &);
  static int getColumnNumber(const QTableWidget *, const QString &);
  static int getMaxCopyNumber(const QSqlDatabase &,
			      const QString &,
			      const QString &,
			      QString &);
  static int getMinimumDays(const QSqlDatabase &, const QString &, QString &);
  static int maximumReserved(const QSqlDatabase &,
			     const QString &,
			     const QString &);
  static int sqliteQuerySize(const QString &,
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
			     const QMap<QString, QVariant> &,
#else
			     const QVariantList &,
#endif
			     const QSqlDatabase &,
			     const char *,
			     const int,
			     biblioteq *);
  static int sqliteQuerySize(const QString &,
			     const QSqlDatabase &,
			     const char *,
			     const int,
			     biblioteq *);
  static qint64 bookAccessionNumber(const QSqlDatabase &);
  static qint64 getSqliteUniqueId(const QSqlDatabase &, QString &);
  static void DBAccount(const QString &,
			const QSqlDatabase &,
			const int,
			QString &,
			const QString & = "");
  static void center(QWidget *, QMainWindow *);
  static void createBookCopy(const QString &idArg,
			     const int copyNumber,
			     const QSqlDatabase &db,
			     QString &errorstr);
  static void createInitialCopies(QString const &,
				  const int,
				  const QSqlDatabase &,
				  const QString &,
				  QString &);
  static void exportPhotographs(const QSqlDatabase &,
				const QString &,
				const QString &,
				const QList<QGraphicsItem *> &,
				QWidget *);
  static void exportPhotographs(const QSqlDatabase &,
				const QString &,
				const int,
				const int,
				const QString &,
				QWidget *);
  static void grantPrivs(const QString &,
			 const QString &,
			 const QSqlDatabase &,
			 QString &);
  static void hideAdminFields(QMainWindow *, const QString &);
  static void highlightWidget(QWidget *, const QColor &);
  static void revokeAll(const QString &, const QSqlDatabase &, QString &);
  static void savePassword(const QString &,
			   const QSqlDatabase &,
			   const QString &,
			   QString &);
  static void saveQuantity(const QSqlDatabase &,
			   const QString &,
			   const int,
			   const QString &,
			   QString &);
  static void setBookRead(const QSqlDatabase &, const bool, const quint64);
  static void setRole(const QSqlDatabase &, QString &, const QString &);
  static void updateColumn(QTableWidget *,
			   const int,
			   const int,
			   const QString &);
  static void updateColumnColor(QTableWidget *,
				const int,
				const int,
				const QColor &);

 private:
  biblioteq_misc_functions(void);
  ~biblioteq_misc_functions();
};

#endif
