#ifndef _QTBOOK_H_
#define _QTBOOK_H_

#define BIBLIOTEQ_VERSION "10.01"

/*
** -- C++ Includes --
*/

#include <iostream>

using namespace std;

/*
** -- Qt Includes --
*/

#include <QHash>
#include <QMainWindow>
#include <QMap>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QStringList>
#include <QtDebug>

/*
** -- Local Includes --
*/

#include "callnum_table_item.h"
#include "dbenumerations.h"
#include "generic_thread.h"
#include "misc_functions.h"
#include "myqstring.h"
#include "numeric_table_item.h"
#include "qtbook_book.h"
#include "qtbook_cd.h"
#include "qtbook_dvd.h"
#include "qtbook_magazine.h"
#include "qtbook_photographcollection.h"
#include "qtbook_videogame.h"
#include "ui_adminsetup.h"
#include "ui_allinfo.h"
#include "ui_branch_s.h"
#include "ui_customquery.h"
#include "ui_errordiag.h"
#include "ui_history.h"
#include "ui_mainwindow.h"
#include "ui_members_browser.h"
#include "ui_password.h"
#include "ui_userinfo.h"

class qtbook: public QMainWindow
{
  Q_OBJECT

 public:
  static QString s_locale;
  static QTranslator *s_appTranslator;
  static QTranslator *s_qtTranslator;

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

    QHash<QString, QString> memberProperties;
    Ui_UserInfo userinfo;

    bool haveMemberChanges(QString &str)
    {
      QStringList list;

      if(memberProperties["membersince"] !=
	 userinfo.membersince->date().toString(Qt::ISODate))
	list << "membersince";

      if(memberProperties["dob"] !=
	 userinfo.dob->date().toString(Qt::ISODate))
	list << "dob";

      if(memberProperties["sex"] != userinfo.sex->currentText())
	list << "sex";

      if(memberProperties["first_name"] != userinfo.firstName->text().
	 trimmed())
	list << "first_name";

      if(memberProperties["middle_init"] != userinfo.middle->text().trimmed())
	list << "middle_init";

      if(memberProperties["last_name"] != userinfo.lastName->text().trimmed())
	list << "last_name";

      if(memberProperties["telephone_num"] !=
	 userinfo.telephoneNumber->text())
	list << "telephone_num";

      if(memberProperties["street"] != userinfo.street->text().trimmed())
	list << "street";

      if(memberProperties["city"] != userinfo.city->text().trimmed())
	list << "city";

      if(memberProperties["state_abbr"] != userinfo.state->currentText())
	list << "state_abbr";

      if(memberProperties["zip"] != userinfo.zip->text())
	list << "zip";

      if(memberProperties["email"] != userinfo.email->text())
	list << "email";

      if(memberProperties["expiration_date"] !=
	 userinfo.expirationdate->date().toString(Qt::ISODate))
	list << "expiration_date";

      if(memberProperties["overdue_fees"] !=
	 QString::number(userinfo.overduefees->value()))
	list << "overdue_fees";

      if(memberProperties["comments"] != userinfo.comments->toPlainText().
	 trimmed())
	list << "comments";

      if(memberProperties["general_registration_number"] !=
	 userinfo.generalregistrationnumber->text().trimmed())
	list << "general_registration_number";

      if(memberProperties["memberclass"] !=
	 userinfo.memberclass->text().trimmed())
	list << "memberclass";

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

  private:
    void changeEvent(QEvent *event)
    {
      if(event)
	switch(event->type())
	  {
	  case QEvent::LanguageChange:
	    {
	      userinfo.retranslateUi(this);
	      break;
	    }
	  default:
	    break;
	  }

      QDialog::changeEvent(event);
    }

    void closeEvent(QCloseEvent *e)
    {
      QString str("");

      if(haveMemberChanges(str))
	if(QMessageBox::question
	   (this, tr("BiblioteQ: Question"),
	    tr("Your changes have not been saved. Continue closing?\n%1").
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

  static const int CUSTOM_QUERY = 0;
  static const int EDITABLE = 0;
  static const int NEW_PAGE = 0;
  static const int NEXT_PAGE = 1;
  static const int POPULATE_ALL = 1;
  static const int POPULATE_SEARCH = 2;
  static const int POPULATE_SEARCH_BASIC = 3;
  static const int PREVIOUS_PAGE = 2;
  static const int VIEW_ONLY = 1;

  qtbook(void);
  ~qtbook();
  QMenu *m_configToolMenu;
  QHash<QString, QString> getAmazonHash(void) const;
  QMainWindow *getMembersBrowser(void) const;
  QMap<QString, QHash<QString, QString> > getSRUMaps(void) const;
  QMap<QString, QHash<QString, QString> > getZ3950Maps(void) const;
  QSqlDatabase getDB(void) const;
  QString getAdminID(void) const;
  QString getPreferredSRUSite(void) const;
  QString getPreferredZ3950Site(void) const;
  QString getRoles(void) const;
  QString getTypeFilterString(void) const;
  QVector<QString> getBBColumnIndexes(void) const;
  Ui_mainWindow getUI(void) const;
  Ui_membersBrowser getBB(void) const;
  int populateTable(const int, const QString &,
		    const QString &, const int = NEW_PAGE);
  static void quit(const char *, const char *, const int);
  static void quit(void);
  void addError(const QString &, const QString &, const QString & = "",
		const char * = "", const int = 0);
  void bookSearch(const QString &, const QString &);
  void cdSearch(const QString &, const QString &);
  void dvdSearch(const QString &, const QString &);
  void journSearch(const QString &, const QString &);
  void magSearch(const QString &, const QString &);
  void pcSearch(const QString &, const QString &);
  void removeBook(qtbook_book *);
  void removeCD(qtbook_cd *);
  void removeDVD(qtbook_dvd *);
  void removeJournal(qtbook_journal *);
  void removeMagazine(qtbook_magazine *);
  void removePhotographCollection(qtbook_photographcollection *);
  void removeVideoGame(qtbook_videogame *);
  void replaceBook(const QString &, qtbook_book *);
  void replaceCD(const QString &, qtbook_cd *);
  void replaceDVD(const QString &, qtbook_dvd *);
  void replaceJournal(const QString &, qtbook_journal *);
  void replaceMagazine(const QString &, qtbook_magazine *);
  void replacePhotographCollection(const QString &,
				   qtbook_photographcollection *);
  void replaceVideoGame(const QString &, qtbook_videogame *);
  void setGlobalFonts(const QFont &);
  void showMain(void);
  void updateItemWindows(void);
  void updateMembersBrowser(const QString &);
  void updateMembersBrowser(void);
  void updateReservationHistoryBrowser(const QString &, const QString &,
				       const QString &, const QString &,
				       const QString &);
  void updateRows(const QString &, const int, const QString &);
  void updateSceneItem(const QString &oid, const QString &type,
		       const QImage &image);
  void vgSearch(const QString &, const QString &);

 public slots:
  void slotDisplaySummary(void);
  void slotResizeColumns(void);

 private:
  QDialog *branch_diag;
  QDialog *pass_diag;
  QHash<QString, QString> amazonImages;
  QHash<QString, QString> selectedBranch;
  QLabel *connected_bar_label;
  QLabel *status_bar_label;
  QMainWindow *admin_diag;
  QMainWindow *all_diag;
  QMainWindow *customquery_diag;
  QMainWindow *error_diag;
  QMainWindow *history_diag;
  QMainWindow *members_diag;
  QMap<QString, QHash<QString, QString> > branches;
  QMap<QString, QHash<QString, QString> > sruMaps;
  QMap<QString, QHash<QString, QString> > z3950Maps;
  QSqlDatabase db;
  QString engUserinfoTitle;
  QString lastCategory;
  QString lastSearchStr;
  QString previousTypeFilter;
  QString roles;
  QStringList deletedAdmins;
  QToolButton *error_bar_label;
  QVector<QString> m_abColumnHeaderIndexes;
  QVector<QString> m_bbColumnHeaderIndexes;
  QVector<QString> m_historyColumnHeaderIndexes;
  Ui_adminBrowser ab;
  Ui_allDialog al;
  Ui_branchSelect br;
  Ui_customquery cq;
  Ui_errordialog er;
  Ui_historyDialog history;
  Ui_mainWindow ui;
  Ui_membersBrowser bb;
  Ui_passSelect pass;
  dbenumerations *db_enumerations;
  int lastSearchType;
  qint64 m_pages;
  qint64 m_queryOffset;
  quint64 m_idCt;
  userinfo_diag_class *userinfo_diag;
  void addConfigOptions(const QString &);
  void adminSetup(void);
  void cdModify(const int);
  void changeEvent(QEvent *);
  void cleanup(void);
  void closeEvent(QCloseEvent *);
  void createSqliteMenuActions(void);
  void deleteItem(const QString &, const QString &);
  void emptyContainers(void);
  void initialUpdate(void);
  void prepareFilter(void);
  void prepareRequestToolButton(const QString &);
  void readConfig(void);
  void readGlobalSetup(void);
  void resetAdminBrowser(void);
  void resetAllSearchWidgets(void);
  void resetMembersBrowser(void);

 private slots:
  void slotAbout(void);
  void slotAddAdmin(void);
  void slotAddBorrower(void);
  void slotAdminCheckBoxClicked(int);
  void slotAllGo(void);
  void slotAutoPopOnFilter(QAction *action);
  void slotBookSearch(void);
  void slotBranchChanged(void);
  void slotCDSearch(void);
  void slotCancelAddUser(void);
  void slotChangeView(bool);
  void slotCheckout(void);
  void slotClearSqliteMenu(bool);
  void slotCloseCustomQueryDialog(void);
  void slotCloseMembersBrowser(void);
  void slotConnectDB(void);
  void slotCopyError(void);
  void slotDVDSearch(void);
  void slotDelete(void);
  void slotDeleteAdmin(void);
  void slotDisconnect(void);
  void slotDisplayNewSqliteDialog(void);
  void slotDuplicate(void);
  void slotExecuteCustomQuery(void);
  void slotExit(void);
  void slotExportAsCSV(void);
  void slotGrantPrivileges(void);
  void slotInsertBook(void);
  void slotInsertCD(void);
  void slotInsertDVD(void);
  void slotInsertJourn(void);
  void slotInsertMag(void);
  void slotInsertPhotograph(void);
  void slotInsertVideoGame(void);
  void slotJournSearch(void);
  void slotLanguageChanged(void);
  void slotListOverdueItems(void);
  void slotListReservedItems(void);
  void slotMagSearch(void);
  void slotModify(void);
  void slotModifyBorrower(void);
  void slotNextPage(void);
  void slotPageClicked(const QString &);
  void slotPhotographSearch(void);
  void slotPopulateMembersBrowser(void);
  void slotPreviousPage(void);
  void slotPrintReservationHistory(void);
  void slotPrintReserved(void);
  void slotPrintView(void);
  void slotQuery(void);
  void slotRefresh(void);
  void slotRefreshAdminList(void);
  void slotRemoveMember(void);
  void slotRequest(void);
  void slotReserveCopy(void);
  void slotReset(void);
  void slotResetAllSearch(void);
  void slotResetErrorLog(void);
  void slotResetGeneralSearch(void);
  void slotResetLoginDialog(void);
  void slotResizeColumnsAfterSort(void);
  void slotRoleChanged(int index);
  void slotSaveAdministrators(void);
  void slotSaveConfig(void);
  void slotSaveDnt(bool state);
  void slotSavePassword(void);
  void slotSaveUser(void);
  void slotSceneSelectionChanged(void);
  void slotSearch(void);
  void slotSearchBasic(void);
  void slotSectionResized(int, int, int);
  void slotSelectDatabaseFile(void);
  void slotSetColumns(void);
  void slotSetFonts(void);
  void slotShowAdminDialog(void);
  void slotShowChangePassword(void);
  void slotShowColumns(void);
  void slotShowConnectionDB(void);
  void slotShowCustomQuery(void);
  void slotShowDbEnumerations(void);
  void slotShowErrorDialog(void);
  void slotShowGrid(void);
  void slotShowHistory(void);
  void slotShowMembersBrowser(void);
  void slotShowMenu(void);
  void slotShowNext(void);
  void slotShowPrev(void);
  void slotSqliteFileSelected(bool);
  void slotUpdateIndicesAfterSort(int);
  void slotVideoGameSearch(void);
  void slotViewDetails(void);
};

#endif
