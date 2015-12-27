#ifndef _BIBLIOTEQ_H_
#define _BIBLIOTEQ_H_

#define BIBLIOTEQ_VERSION "2016.02.01"

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

#include "biblioteq_book.h"
#include "biblioteq_callnum_table_item.h"
#include "biblioteq_cd.h"
#include "biblioteq_dbenumerations.h"
#include "biblioteq_dvd.h"
#include "biblioteq_generic_thread.h"
#include "biblioteq_magazine.h"
#include "biblioteq_misc_functions.h"
#include "biblioteq_myqstring.h"
#include "biblioteq_numeric_table_item.h"
#include "biblioteq_photographcollection.h"
#include "biblioteq_videogame.h"
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

class userinfo_diag_class: public QDialog
{
  Q_OBJECT

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

  void closeEvent(QCloseEvent *event)
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
	  if(event)
	    event->ignore();

	  return;
	}

    QDialog::closeEvent(event);
  }
};

class biblioteq: public QMainWindow
{
  Q_OBJECT

 public:
  static QString s_locale;
  static QTranslator *s_appTranslator;
  static QTranslator *s_qtTranslator;
  static const int CUSTOM_QUERY = 0;
  static const int EDITABLE = 0;
  static const int NEW_PAGE = 0;
  static const int NEXT_PAGE = 1;
  static const int POPULATE_ALL = 1;
  static const int POPULATE_SEARCH = 2;
  static const int POPULATE_SEARCH_BASIC = 3;
  static const int PREVIOUS_PAGE = 2;
  static const int VIEW_ONLY = 1;
  biblioteq(void);
  ~biblioteq();
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
  int populateTable(const int search_type_arg,
		    const QString &typefilter,
		    const QString &searchstrArg,
		    const int pagingType= NEW_PAGE);
  static void quit(const char *msg, const char *file, const int line);
  static void quit(void);
  void addError(const QString &type,
		const QString &summary,
		const QString &error = "",
		const char *file = "",
		const int line = 0);
  void bookSearch(const QString &field, const QString &value);
  void cdSearch(const QString &field, const QString &value);
  void dvdSearch(const QString &field, const QString &value);
  void journSearch(const QString &field, const QString &value);
  void magSearch(const QString &field, const QString &value);
  void pcSearch(const QString &field, const QString &value);
  void removeBook(biblioteq_book *book);
  void removeCD(biblioteq_cd *cd);
  void removeDVD(biblioteq_dvd *dvd);
  void removeJournal(biblioteq_journal *journal);
  void removeMagazine(biblioteq_magazine *magazine);
  void removePhotographCollection(biblioteq_photographcollection *pc);
  void removeVideoGame(biblioteq_videogame *videogame);
  void replaceBook(const QString &id, biblioteq_book *book);
  void replaceCD(const QString &id, biblioteq_cd *cd);
  void replaceDVD(const QString &id, biblioteq_dvd *dvd);
  void replaceJournal(const QString &id, biblioteq_journal *journal);
  void replaceMagazine(const QString &id, biblioteq_magazine *magazine);
  void replacePhotographCollection(const QString &id,
				   biblioteq_photographcollection *pc);
  void replaceVideoGame(const QString &id, biblioteq_videogame *videogame);
  void setGlobalFonts(const QFont &font);
  void showMain(void);
  void updateItemWindows(void);
  void updateMembersBrowser(const QString &memberid);
  void updateMembersBrowser(void);
  void updateReservationHistoryBrowser(const QString &memberid,
				       const QString &ioid,
				       const QString &copyid,
				       const QString &itemType,
				       const QString &returnedDate);
  void updateRows(const QString &oid,
		  const int row,
		  const QString &itemType);
  void updateSceneItem(const QString &oid,
		       const QString &type,
		       const QImage &image);
  void vgSearch(const QString &field, const QString &value);

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
  biblioteq_dbenumerations *db_enumerations;
  int lastSearchType;
  qint64 m_pages;
  qint64 m_queryOffset;
  quint64 m_idCt;
  userinfo_diag_class *userinfo_diag;
  void addConfigOptions(const QString &typefilter);
  void adminSetup(void);
  void changeEvent(QEvent *event);
  void cleanup(void);
  void closeEvent(QCloseEvent *event);
  void createSqliteMenuActions(void);
  void deleteItem(const QString &oid, const QString &itemType);
  void emptyContainers(void);
  void initialUpdate(void);
  void prepareFilter(void);
  void prepareRequestToolButton(const QString &typefilter);
  void readConfig(void);
  void readGlobalSetup(void);
  void resetAdminBrowser(void);
  void resetAllSearchWidgets(void);
  void resetMembersBrowser(void);

 private slots:
  void slotAbout(void);
  void slotAddAdmin(void);
  void slotAddBorrower(void);
  void slotAdminCheckBoxClicked(int state);
  void slotAllGo(void);
  void slotAutoPopOnFilter(QAction *action);
  void slotBookSearch(void);
  void slotBranchChanged(void);
  void slotCDSearch(void);
  void slotCancelAddUser(void);
  void slotChangeView(bool checked);
  void slotCheckout(void);
  void slotClearSqliteMenu(bool state);
  void slotCloseCustomQueryDialog(void);
  void slotCloseMembersBrowser(void);
  void slotClosePasswordDialog(void);
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
  void slotPageClicked(const QString &link);
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
  void slotSectionResized(int logicalIndex, int oldSize, int newSize);
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
  void slotSqliteFileSelected(bool state);
  void slotUpdateIndicesAfterSort(int column);
  void slotVideoGameSearch(void);
  void slotViewDetails(void);
};

#endif
