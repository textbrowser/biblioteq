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
#include "ui_history.h"
#include "qtbook_book.h"
#include "ui_branch_s.h"
#include "ui_password.h"
#include "ui_userinfo.h"
#include "ui_errordiag.h"
#include "ui_adminsetup.h"
#include "ui_mainwindow.h"
#include "generic_thread.h"
#include "misc_functions.h"
#include "ui_customquery.h"
#include "qtbook_magazine.h"
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

  qtbook(void);
  ~qtbook();
  int populateTable(const int, const QString &, const QString &);
  void addError(const QString &, const QString &, const QString & = "",
		const char * = "", const int = 0);
  void removeCD(qtbook_cd *);
  void showMain(void);
  void cdSearch(const QString &, const QString &);
  void vgSearch(const QString &, const QString &);
  void dvdSearch(const QString &, const QString &);
  void magSearch(const QString &, const QString &);
  void removeDVD(qtbook_dvd *);
  void replaceCD(const QString &, qtbook_cd *);
  void closeEvent(QCloseEvent *);
  void removeBook(qtbook_book *);
  void replaceDVD(const QString &, qtbook_dvd *);
  void bookSearch(const QString &, const QString &);
  void updateRows(const QString &, const int, const QString &);
  void journSearch(const QString &, const QString &);
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
  void updateReservationHistoryBrowser(const QString &, const QString &,
				       const QString &, const QString &,
				       const QString &);
  QString getRoles(void);
  QString getAdminID(void);
  QString sqlitefile(void);
  QString getGeometryString(void);
  QMainWindow *getMembersBrowser(void);
  QSqlDatabase getDB(void);
  Ui_mainWindow getUI(void);
  Ui_membersBrowser getBB(void);
  QHash<QString, QString> getLOCHash(void);
  static void quit(void);
  static void quit(const char *, const char *, const int);

 public slots:
  void slotResizeColumns(void);
  void slotDisplaySummary(void);

 private:
  QString roles;
  QString previousTypeFilter;
  QStringList deletedAdmins;
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
  QDialog *pass_diag;
  QDialog *branch_diag;
  QDialog *userinfo_diag;
  QMainWindow *all_diag;
  QMainWindow *admin_diag;
  QMainWindow *error_diag;
  QMainWindow *history_diag;
  QMainWindow *members_diag;
  QMainWindow *customquery_diag;
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
  Ui_passSelect pass;
  Ui_customquery cq;
  Ui_errordialog er;
  Ui_adminBrowser ab;
  Ui_branchSelect br;
  Ui_historyDialog history;
  Ui_membersBrowser bb;
  bool isItemBusy(const QString &, const QString &);
  void cleanup(void);
  void lockApp(const bool);
  void cdModify(const int);
  void adminSetup(void);
  void readConfig(void);
  void deleteItem(const QString &, const QString &);
  void initialUpdate(void);
  void prepareFilter(void);
  void emptyContainers(void);
  void readGlobalSetup(void);
  void addConfigOptions(const QString &);
  void resetAdminBrowser(void);
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
  void slotRequest(void);
  void slotAddAdmin(void);
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
  void slotDeleteAdmin(void);
  void slotJournSearch(void);
  void slotViewDetails(void);
  void slotReserveCopy(void);
  void slotShowColumns(void);
  void slotShowHistory(void);
  void slotInsertJourn(void);
  void slotRemoveMember(void);
  void slotSavePassword(void);
  void slotCancelAddUser(void);
  void slotBranchChanged(void);
  void slotPrintReserved(void);
  void slotPrintSelected(void);
  void slotResetErrorLog(void);
  void slotModifyBorrower(void);
  void slotAutoPopOnFilter(void);
  void slotShowCustomQuery(void);
  void slotInsertVideoGame(void);
  void slotShowAdminDialog(void);
  void slotShowErrorDialog(void);
  void slotVideoGameSearch(void);
  void slotListOverdueItems(void);
  void slotRefreshAdminList(void);
  void slotResetLoginDialog(void);
  void slotShowConnectionDB(void);
  void slotListReservedItems(void);
  void slotExecuteCustomQuery(void);
  void slotSaveAdministrators(void);
  void slotSelectDatabaseFile(void);
  void slotShowChangePassword(void);
  void slotShowMembersBrowser(void);
  void slotCloseMembersBrowser(void);
  void slotAdminCheckBoxClicked(int);
  void slotCloseCustomQueryDialog(void);
  void slotPopulateMembersBrowser(void);
  void slotResizeColumnsAfterSort(void);
  void slotUpdateIndicesAfterSort(int);
  void slotPrintReservationHistory(void);
};

#endif
