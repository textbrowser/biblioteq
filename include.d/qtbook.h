#ifndef _QTBOOK_H_
#define _QTBOOK_H_

#define BIBLIOTEQ_VERSION "6.52"

/*
** -- C++ Includes --
*/

#include <iostream>
using namespace std;

/*
** -- Qt Includes --
*/

#include <QDir>
#include <QMap>
#include <QHash>
#include <QtDebug>
#include <QCheckBox>
#include <QDateTime>
#include <QSettings>
#include <QSqlError>
#include <QSqlField>
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
#include "dbenumerations.h"
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
  class userinfo_diag_class: public QDialog
  {
  public:
    userinfo_diag_class(QMainWindow *parent):QDialog(parent)
    {
      userinfo.setupUi(this);
    }

    ~userinfo_diag_class()
    {
    }

    Ui_UserInfo userinfo;
    QHash<QString, QString> memberProperties;

    bool haveMemberChanges(QString &str)
    {
      QStringList list;

      if(memberProperties["membersince"] !=
	 userinfo.membersince->date().toString
	 (Qt::SystemLocaleShortDate))
	list << "membersince";

      if(memberProperties["dob"] !=
	 userinfo.dob->date().toString(Qt::SystemLocaleShortDate))
	list << "dob";

      if(memberProperties["sex"] != userinfo.sex->currentText())
	list << "sex";

      if(memberProperties["first_name"] != userinfo.firstName->text())
	list << "first_name";

      if(memberProperties["middle_init"] != userinfo.middle->text())
	list << "middle_init";

      if(memberProperties["last_name"] != userinfo.lastName->text())
	list << "last_name";

      if(memberProperties["telephone_num"] !=
	 userinfo.telephoneNumber->text())
	list << "telephone_num";

      if(memberProperties["street"] != userinfo.street->text())
	list << "street";

      if(memberProperties["city"] != userinfo.city->text())
	list << "city";

      if(memberProperties["state_abbr"] != userinfo.state->currentText())
	list << "state_abbr";

      if(memberProperties["zip"] != userinfo.zip->text())
	list << "zip";

      if(memberProperties["email"] != userinfo.email->text())
	list << "email";

      if(memberProperties["expiration_date"] !=
	 userinfo.expirationdate->date().toString
	 (Qt::SystemLocaleShortDate))
	list << "expiration_date";

      while(!list.isEmpty())
	str += list.takeFirst() + ", ";

      if(!str.isEmpty())
	{
	  str.prepend("(");
	  str = str.mid(0, str.length() - 2);
	  str.append(")");
	}

      return !str.isEmpty();
    }

  protected:
    void closeEvent(QCloseEvent *e)
    {
      QString str("");

      if(haveMemberChanges(str))
	if(QMessageBox::question
	   (this, tr("BiblioteQ: Question"),
	    tr("You have unsaved data. Continue closing?\n%1").
	    arg(str),
	    QMessageBox::Yes | QMessageBox::No,
	    QMessageBox::No) == QMessageBox::No)
	  {
	    if(e)
	      e->ignore();

	    return;
	  }

      QDialog::closeEvent(e);
    }
  };

  static const int EDITABLE = 0;
  static const int NEW_PAGE = 0;
  static const int NEXT_PAGE = 1;
  static const int VIEW_ONLY = 1;
  static const int CUSTOM_QUERY = 0;
  static const int POPULATE_ALL = 1;
  static const int PREVIOUS_PAGE = 2;
  static const int POPULATE_SEARCH = 2;

  qtbook(void);
  ~qtbook();
  int populateTable(const int, const QString &, const QString &,
		    const int = NEW_PAGE);
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
  QString getRoles(void) const;
  QString getAdminID(void) const;
  QString getTypeFilterString(void) const;
  QString getPreferredZ3950Site(void) const;
  QMainWindow *getMembersBrowser(void) const;
  QSqlDatabase getDB(void) const;
  Ui_mainWindow getUI(void) const;
  Ui_membersBrowser getBB(void) const;
  QHash<QString, QString> getAmazonHash(void) const;
  QMap<QString, QHash<QString, QString> > getZ3950Maps(void) const;
  static void quit(void);
  static void quit(const char *, const char *, const int);

 public slots:
  void slotResizeColumns(void);
  void slotDisplaySummary(void);

 private:
  int m_pages;
  int m_queryOffset;
  int lastSearchType;
  QString roles;
  QString lastCategory;
  QString lastSearchStr;
  QString engUserinfoTitle;
  QString previousTypeFilter;
  QStringList deletedAdmins;
  QHash<QString, QString> amazonImages;
  QHash<QString, QString> selectedBranch;
  QHash<QString, qtbook_cd *> cds;
  QHash<QString, qtbook_dvd *> dvds;
  QHash<QString, qtbook_book *> books;
  QHash<QString, qtbook_journal *> journals;
  QHash<QString, qtbook_magazine *> magazines;
  QHash<QString, qtbook_videogame *> video_games;
  QMap<QString, QHash<QString, QString> > branches;
  QMap<QString, QHash<QString, QString> > z3950Maps;
  QLabel *error_bar_label;
  QLabel *status_bar_label;
  QLabel *connected_bar_label;
  QDialog *pass_diag;
  QDialog *branch_diag;
  QMainWindow *all_diag;
  QMainWindow *admin_diag;
  QMainWindow *error_diag;
  QMainWindow *history_diag;
  QMainWindow *members_diag;
  QMainWindow *customquery_diag;
  QSqlDatabase db;
  Ui_allDialog al;
  Ui_mainWindow ui;
  Ui_passSelect pass;
  dbenumerations *db_enumerations;
  Ui_customquery cq;
  Ui_errordialog er;
  Ui_adminBrowser ab;
  Ui_branchSelect br;
  Ui_historyDialog history;
  Ui_membersBrowser bb;
  userinfo_diag_class *userinfo_diag;
  void cleanup(void);
  void lockApp(const bool);
  void cdModify(const int);
  void adminSetup(void);
  void readConfig(void);
  void deleteItem(const QString &, const QString &);
  void initialUpdate(void);
  void prepareFilter(void);
  void emptyContainers(void);
  void readGlobalSetup(QString &);
  void addConfigOptions(const QString &);
  void resetAdminBrowser(void);
  void resetMembersBrowser(void);
  void createSqliteMenuActions(void);
  void prepareRequestToolButton(const QString &);

 protected:
  void closeEvent(QCloseEvent *);

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
  void slotNextPage(void);
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
  void slotPrintView(void);
  void slotBookSearch(void);
  void slotChangeView(bool);
  void slotDisconnect(void);
  void slotInsertBook(void);
  void slotSaveConfig(void);
  void slotSetColumns(void);
  void slotAddBorrower(void);
  void slotDeleteAdmin(void);
  void slotExportAsCSV(void);
  void slotJournSearch(void);
  void slotPageClicked(const QString &);
  void slotViewDetails(void);
  void slotReserveCopy(void);
  void slotShowColumns(void);
  void slotShowHistory(void);
  void slotInsertJourn(void);
  void slotPreviousPage(void);
  void slotRemoveMember(void);
  void slotSavePassword(void);
  void slotCancelAddUser(void);
  void slotBranchChanged(void);
  void slotPrintReserved(void);
  void slotResetErrorLog(void);
  void slotModifyBorrower(void);
  void slotSectionResized(int, int, int);
  void slotAutoPopOnFilter(void);
  void slotClearSqliteMenu(bool);
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
  void slotShowDbEnumerations(void);
  void slotShowMembersBrowser(void);
  void slotSqliteFileSelected(bool);
  void slotCloseMembersBrowser(void);
  void slotAdminCheckBoxClicked(int);
  void slotCloseCustomQueryDialog(void);
  void slotDisplayNewSqliteDialog(void);
  void slotPopulateMembersBrowser(void);
  void slotResizeColumnsAfterSort(void);
  void slotUpdateIndicesAfterSort(int);
  void slotPrintReservationHistory(void);
};

#endif
