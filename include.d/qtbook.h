#ifndef _QTBOOK_H_
#define _QTBOOK_H_

/*
** -- C++ Includes --
*/

#include <iostream>
using namespace std;

/*
** -- Qt Includes --
*/

#include <QDir>
#include <QHash>
#include <QCheckBox>
#include <QDateTime>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QClipboard>
#include <QScrollBar>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QFontDialog>
#include <QHeaderView>
#include <QMainWindow>
#include <QMessageBox>
#include <QStringList>
#include <QApplication>
#include <QSqlDatabase>
#include <QDesktopWidget>
#include <QProgressDialog>

/*
** -- Local Includes --
*/

#include "myqstring.h"
#include "qtbook_cd.h"
#include "qtbook_dvd.h"
#include "ui_allinfo.h"
#include "qtbook_book.h"
#include "ui_branch_s.h"
#include "ui_userinfo.h"
#include "ui_errordiag.h"
#include "ui_mainwindow.h"
#include "generic_thread.h"
#include "misc_functions.h"
#include "ui_customquery.h"
#include "qtbook_magazine.h"
#include "ui_authenticate.h"
#include "qtbook_videogame.h"
#include "numeric_table_item.h"
#include "ui_members_browser.h"

class qtbook_cd;
class qtbook_dvd;
class qtbook_book;
class qtbook_journal;
class qtbook_magazine;
class qtbook_videogame;

class qtbook: public QMainWindow
{
  Q_OBJECT

 public:
  static const int EDITABLE = 0;
  static const int VIEW_ONLY = 1;
  static const int CUSTOM_QUERY = 0;
  static const int POPULATE_ALL = 1;
  static const int POPULATE_SEARCH = 2;
  static const int MAINWINDOW_MINWIDTH = 900;
  static const int MAINWINDOW_MINHEIGHT = 700;

  qtbook(void);
  ~qtbook();
  int populateTable(const int, const int, const QString &);
  void addError(const QString &, const QString &, const QString & = "",
		const char * = "", const int = 0);
  void removeCD(qtbook_cd *);
  void showMain(void);
  void removeDVD(qtbook_dvd *);
  void replaceCD(const QString &, qtbook_cd *);
  void removeBook(qtbook_book *);
  void replaceDVD(const QString &, qtbook_dvd *);
  void updateRows(const QString &, const int, const QString &);
  void replaceBook(const QString &, qtbook_book *);
  void removeJournal(qtbook_journal *);
  void removeMagazine(qtbook_magazine *);
  void replaceJournal(const QString &, qtbook_journal *);
  void setGlobalFonts(const QFont &);
  void replaceMagazine(const QString &, qtbook_magazine *);
  void removeVideoGame(qtbook_videogame *);
  void replaceVideoGame(const QString &, qtbook_videogame *);
  void updateItemWindows(void);
  void updateMembersBrowser(void);
  void updateMembersBrowser(const QString &);
  QString getRoles(void);
  QString getAdminID(void);
  QString getGeometryString(void);
  QSqlDatabase getDB(void);
  Ui_mainWindow getUI(void);
  Ui_membersBrowser getBB(void);
  QHash<QString, QString> getLOCHash(void);
  static void quit(void);
  static void quit(const char *, const char *, const int);
  virtual void closeEvent(QCloseEvent *);

 public slots:
  void slotResizeColumns(void);

 private:
  int previousTypeFilter;
  bool supportsTransactions;
  QString roles;
  QString typefilter;
  QHash<QString, QString> LOCHash;
  QHash<QString, QString> selectedBranch;
  QHash<QString, qtbook_cd *> cds;
  QHash<QString, qtbook_dvd *> dvds;
  QHash<QString, qtbook_book *> books;
  QHash<QString, qtbook_journal *> journals;
  QHash<QString, qtbook_magazine *> magazines;
  QHash<QString, qtbook_videogame *> video_games;
  QHash<QString, QHash<QString, QString> > branches;
  QLabel *error_bar_label;
  QLabel *status_bar_label;
  QLabel *connected_bar_label;
  QDialog *auth_diag;
  QDialog *error_diag;
  QDialog *branch_diag;
  QDialog *members_diag;
  QDialog *userinfo_diag;
  QDialog *customquery_diag;
  QMainWindow *all_diag;
  QStringList languages;
  QStringList vg_genres;
  QStringList cd_formats;
  QStringList vg_ratings;
  QStringList dvd_ratings;
  QStringList dvd_regions;
  QStringList cd_locations;
  QStringList vg_locations;
  QStringList vg_platforms;
  QStringList cd_categories;
  QStringList dvd_locations;
  QStringList book_locations;
  QStringList dvd_categories;
  QStringList monetary_units;
  QStringList book_categories;
  QStringList dvd_aspectratios;
  QStringList journal_locations;
  QStringList journal_categories;
  QStringList magazine_locations;
  QStringList magazine_categories;
  Ui_UserInfo userinfo;
  QSqlDatabase db;
  Ui_allDialog al;
  Ui_mainWindow ui;
  Ui_customquery cq;
  Ui_errordialog er;
  Ui_branchSelect br;
  Ui_membersBrowser bb;
  Ui_userAuthenticate au;
  bool isItemBusy(const QString &, const QString &);
  void cleanup(void);
  void lockApp(const bool);
  void cdModify(const int);
  void readConfig(void);
  void authenticate(void);
  void deleteItem(const QString &, const QString &);
  void initialUpdate(void);
  void emptyContainers(void);
  void readGlobalSetup(void);
  void addConfigOptions(void);
  void resetMembersBrowser(void);

 private slots:
  void slotExit(void);
  void slotAbout(void);
  void slotAllGo(void);
  void slotQuery(void);
  void slotReset(void);
  void slotDelete(void);
  void slotModify(void);
  void slotSearch(void);
  void slotRefresh(void);
  void slotCheckout(void);
  void slotCDSearch(void);
  void slotInsertCD(void);
  void slotSaveUser(void);
  void slotSetFonts(void);
  void slotShowGrid(void);
  void slotShowMenu(void);
  void slotShowNext(void);
  void slotShowPrev(void);
  void slotConnectDB(void);
  void slotCopyError(void);
  void slotDVDSearch(void);
  void slotInsertMag(void);
  void slotInsertDVD(void);
  void slotMagSearch(void);
  void slotBookSearch(void);
  void slotDisconnect(void);
  void slotInsertBook(void);
  void slotSaveConfig(void);
  void slotSetColumns(void);
  void slotAddBorrower(void);
  void slotJournSearch(void);
  void slotViewDetails(void);
  void slotReserveCopy(void);
  void slotShowColumns(void);
  void slotInsertJourn(void);
  void slotAuthenticate(void);
  void slotRemoveMember(void);
  void slotCancelAddUser(void);
  void slotPrintReserved(void);
  void slotPrintSelected(void);
  void slotResetErrorLog(void);
  void slotModifyBorrower(void);
  void slotAutoPopOnFilter(void);
  void slotShowCustomQuery(void);
  void slotInsertVideoGame(void);
  void slotShowErrorDialog(void);
  void slotVideoGameSearch(void);
  void slotListOverdueItems(void);
  void slotShowAuthenticate(void);
  void slotShowConnectionDB(void);
  void slotListReservedItems(void);
  void slotUpdateStatusLabel(void);
  void slotExecuteCustomQuery(void);
  void slotShowMembersBrowser(void);
  void slotCloseCustomQueryDialog(void);
  void slotPopulateMembersBrowser(void);
  void slotResizeColumnsAfterSort(void);
  void slotUpdateIndicesAfterSort(int);
};

#endif
