/*
** Copyright (c) 2006 - present, Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from BiblioteQ without specific prior written permission.
**
** BIBLIOTEQ IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** BIBLIOTEQ, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _BIBLIOTEQ_H_
#define _BIBLIOTEQ_H_

#define BIBLIOTEQ_GUEST_ACCOUNT "xbook_guest"
#define BIBLIOTEQ_VERSION "2023.02.25"

#include "biblioteq_batch_activities.h"
#include "biblioteq_book.h"
#include "biblioteq_callnum_table_item.h"
#include "biblioteq_cd.h"
#include "biblioteq_dbenumerations.h"
#include "biblioteq_dvd.h"
#include "biblioteq_generic_thread.h"
#include "biblioteq_grey_literature.h"
#include "biblioteq_import.h"
#include "biblioteq_magazine.h"
#include "biblioteq_misc_functions.h"
#include "biblioteq_myqstring.h"
#include "biblioteq_numeric_table_item.h"
#include "biblioteq_photographcollection.h"
#include "biblioteq_videogame.h"
#include "ui_biblioteq_adminsetup.h"
#include "ui_biblioteq_allinfo.h"
#include "ui_biblioteq_branch_s.h"
#include "ui_biblioteq_customquery.h"
#include "ui_biblioteq_errordiag.h"
#include "ui_biblioteq_history.h"
#include "ui_biblioteq_mainwindow.h"
#include "ui_biblioteq_members_browser.h"
#include "ui_biblioteq_password.h"
#include "ui_biblioteq_userinfo.h"

#include <QMessageBox>
#include <QTimer>

class biblioteq_documentationwindow;
class biblioteq_files;
class biblioteq_otheroptions;
class biblioteq_sql_syntax_highlighter;
class biblioteq_sqlite_merge_databases;
class woody_collapse_expand_tool_button;

class userinfo_diag_class: public QDialog
{
  Q_OBJECT

 public:
  userinfo_diag_class(QMainWindow *parent):QDialog(parent)
  {
    m_userinfo.setupUi(this);
  }

  ~userinfo_diag_class()
  {
  }

  QHash<QString, QString> m_memberProperties;
  Ui_UserInfo m_userinfo;

  bool haveMemberChanges(QString &str)
  {
    QStringList list;

    if(m_memberProperties.value("city") != m_userinfo.city->text().trimmed())
      list << "city";

    if(m_memberProperties.value("comments") !=
       m_userinfo.comments->toPlainText().
       trimmed())
      list << "comments";

    if(m_memberProperties.value("dob") !=
       m_userinfo.dob->date().toString(Qt::ISODate))
      list << "dob";

    if(m_memberProperties.value("email") != m_userinfo.email->text())
      list << "email";

    if(m_memberProperties.value("expiration_date") !=
       m_userinfo.expirationdate->date().toString(Qt::ISODate))
      list << "expiration_date";

    if(m_memberProperties.value("first_name") != m_userinfo.firstName->text().
       trimmed())
      list << "first_name";

    if(m_memberProperties.value("general_registration_number") !=
       m_userinfo.generalregistrationnumber->text().trimmed())
      list << "general_registration_number";

    if(m_memberProperties.value("last_name") != m_userinfo.lastName->text().
       trimmed())
      list << "last_name";

    if(m_memberProperties.value("maximum_reserved_books") !=
       m_userinfo.maximum_reserved_books->text())
      list << "maximum_reserved_books";

    if(m_memberProperties.value("memberclass") !=
       m_userinfo.memberclass->text().trimmed())
      list << "memberclass";

    if(m_memberProperties.value("membership_fees") !=
       m_userinfo.membershipfees->text())
      list << "membership_fees";

    if(m_memberProperties.value("membersince") !=
       m_userinfo.membersince->date().toString(Qt::ISODate))
      list << "membersince";

    if(m_memberProperties.value("middle_init") != m_userinfo.middle->text().
       trimmed())
      list << "middle_init";

    if(m_memberProperties.value("overdue_fees") !=
       m_userinfo.overduefees->text())
      list << "overdue_fees";

    if(m_memberProperties.value("sex") != m_userinfo.sex->currentText())
      list << "sex";

    if(m_memberProperties.value("state_abbr") !=
       m_userinfo.state->currentText())
      list << "state_abbr";

    if(m_memberProperties.value("street") !=
       m_userinfo.street->text().trimmed())
      list << "street";

    if(m_memberProperties.value("telephone_num") !=
       m_userinfo.telephoneNumber->text())
      list << "telephone_num";

    if(m_memberProperties.value("zip") != m_userinfo.zip->text())
      list << "zip";

    std::sort(list.begin(), list.end());

    for(int i = 0; i < list.size(); i++)
      str += list.at(i) + ", ";

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
	    m_userinfo.retranslateUi(this);
	    break;
	  }
	default:
	  {
	    break;
	  }
	}

    QDialog::changeEvent(event);
  }

 private slots:
  void done(int result)
  {
    QString str("");

    if(haveMemberChanges(str))
      if(QMessageBox::
	 question(this,
		  tr("BiblioteQ: Question"),
		  tr("Your changes have not been saved. Continue "
		     "closing?\n%1").arg(str),
		  QMessageBox::No | QMessageBox::Yes,
		  QMessageBox::No) == QMessageBox::No)
	{
	  QApplication::processEvents();
	  return;
	}

    QDialog::done(result);
  }
};

class biblioteq: public QMainWindow
{
  Q_OBJECT

 public:
  enum class Limits
    {
      FAVORITES_LENGTH = 1024,
      QUANTITY = 1000 // Copies per item.
    };

  static QString s_databaseDateFormat;
  static QString s_locale;
  static QString s_unknown;
  static QTranslator *s_appTranslator;
  static QTranslator *s_qtTranslator;
  static const int CUSTOM_QUERY = 0;
  static const int EDITABLE = 0;
  static const int MAXIMUM_DEVICES_CONFIRMATION = 5;
  static const int NEW_PAGE = 0;
  static const int NEXT_PAGE = 1;
  static const int POPULATE_ALL = 1;
  static const int POPULATE_SEARCH = 2;
  static const int POPULATE_SEARCH_BASIC = 3;
  static const int PREVIOUS_PAGE = 2;
  static const int VIEW_ONLY = 1;
  biblioteq(void);
  ~biblioteq();
  QHash<QAction *, QPointer<biblioteq_documentationwindow> > m_documentation;
  QHash<QAction *, QPointer<biblioteq_documentationwindow> > m_releaseNotes;
  QPointer<QMenu> m_configToolMenu;
  QPointer<biblioteq_documentationwindow> m_contributors;
  QString m_unaccent;
  QColor availabilityColor(const QString &itemType) const;
  QColor itemMandatoryFieldColor(void) const;
  QColor itemQueryResultColor(void) const;
  QHash<QString, QString> getAmazonHash(void) const;
  QHash<QString, QString> getOpenLibraryImagesHash(void) const;
  QHash<QString, QString> getOpenLibraryItemsHash(void) const;
  QHash<QString, QString> getSRUHash(const QString &name) const;
  QHash<QString, QString> getZ3950Hash(const QString &name) const;
  QHash<QString, QString> otherImagesHash(void) const;
  QMainWindow *getMembersBrowser(void) const;
  QSqlDatabase getDB(void) const;
  QString formattedISBN10(const QString &str) const;
  QString formattedISBN13(const QString &str) const;
  QString getAdminID(void) const;
  QString getPreferredSRUSite(void) const;
  QString getPreferredZ3950Site(void) const;
  QString getRoles(void) const;
  QString getTypeFilterString(void) const;
  QString publicationDateFormat(const QString &itemType) const;
  QString unaccent(void) const;
  QStringList getSRUNames(void) const;
  QStringList getZ3950Names(void) const;
  QVariant setting(const QString &name) const;
  QVector<QString> getBBColumnIndexes(void) const;
  Ui_mainWindow getUI(void) const;
  Ui_membersBrowser getBB(void) const;
  bool availabilityColors(void) const;
  bool canAccessDatabaseEnumerations(void) const;
  bool isGuest(void) const;
  bool isPatron(void) const;
  bool showBookReadStatus(void) const;
  bool showMainTableImages(void) const;
  int pageLimit(void) const;
  int populateTable(QSqlQuery *query,
		    const QString &typefilter,
		    const int pagingType,
		    const int searchType);
  int populateTable(const int search_type_arg,
		    const QString &typefilter,
		    const QString &searchstrArg,
		    const int pagingType = NEW_PAGE);
  static QString homePath(void);
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
  void greyLiteratureSearch(const QString &field, const QString &value);
  void journSearch(const QString &field, const QString &value);
  void magSearch(const QString &field, const QString &value);
  void pcSearch(const QString &field, const QString &value);
  void removeBook(biblioteq_book *book);
  void removeCD(biblioteq_cd *cd);
  void removeDVD(biblioteq_dvd *dvd);
  void removeGreyLiterature(biblioteq_grey_literature *gl);
  void removeJournal(biblioteq_journal *journal);
  void removeMagazine(biblioteq_magazine *magazine);
  void removePhotographCollection(biblioteq_photographcollection *pc);
  void removeVideoGame(biblioteq_videogame *videogame);
  void replaceBook(const QString &id, biblioteq_book *book);
  void replaceCD(const QString &id, biblioteq_cd *cd);
  void replaceDVD(const QString &id, biblioteq_dvd *dvd);
  void replaceGreyLiterature(const QString &id, biblioteq_grey_literature *gl);
  void replaceJournal(const QString &id, biblioteq_journal *journal);
  void replaceMagazine(const QString &id, biblioteq_magazine *magazine);
  void replacePhotographCollection(const QString &id,
				   biblioteq_photographcollection *photograph);
  void replaceVideoGame(const QString &id, biblioteq_videogame *videogame);
  void setGlobalFonts(const QFont &font);
  void setSummaryImages(const QImage &back, const QImage &front);
  void showMain(void);
  void updateItemWindows(void);
  void updateMembersBrowser(const QString &memberid);
  void updateMembersBrowser(void);
  void updateReservationHistoryBrowser(const QString &memberid,
				       const QString &ioid,
				       const QString &copyid,
				       const QString &itemType,
				       const QString &returnedDate);
  void updateRows
    (const QString &oid, const QTableWidgetItem *item, const QString &it);
  void updateSceneItem(const QString &oid,
		       const QString &type,
		       const QImage &image);
  void vgSearch(const QString &field, const QString &value);

 public slots:
  void slotDisplaySummary(void);
  void slotResizeColumns(void);

 private:
  enum AdminSetupColumns
    {
      ADMINISTRATOR = 1,
      CIRCULATION = 2,
      ID = 0,
      LIBRARIAN = 3,
      MEMBERSHIP = 4
    };

  enum ErrorDialogColumns
    {
      EVENT_TIME = 0,
      EVENT_TYPE = 1,
      FILE = 4,
      FULL_DESCRIPTION = 3,
      LINE_NUMBER = 5,
      SUMMARY = 2
    };

  enum GenericSearchTypes
    {
      ACCESSION_NUMBER_GENERIC_SEARCH_TYPE = 0,
      CATEGORY_GENERIC_SEARCH_TYPE = 1,
      ID_GENERIC_SEARCH_TYPE = 2,
      KEYWORD_GENERIC_SEARCH_TYPE = 3,
      TITLE_GENERIC_SEARCH_TYPE = 4
    };

  enum HistoryColumns
    {
      BARCODE = 5,
      DUE_DATE = 8,
      FIRST_NAME = 1,
      ID_NUMBER = 4,
      LAST_NAME = 2,
      LENDER = 10,
      MEMBER_ID = 0,
      MYOID = 11,
      RESERVATION_DATE = 7,
      RETURNED_DATE = 9,
      TITLE = 3,
      TYPE = 6
    };

  enum RequestActionItems
    {
      CANCEL_REQUESTED = 0,
      INACTIVE,
      REQUEST_SELECTED,
      RETURN_RESERVED
    };

  QActionGroup *m_menuCategoryActionGroup;
  QDialog *m_branch_diag;
  QDialog *m_pass_diag;
  QHash<QString, QString> m_amazonImages;
  QHash<QString, QString> m_openLibraryImages;
  QHash<QString, QString> m_openLibraryItems;
  QHash<QString, QString> m_otherImages;
  QHash<QString, QString> m_selectedBranch;
  QLabel *m_connected_bar_label;
  QLabel *m_status_bar_label;
  QList<QTableWidgetItem *> m_findList;
  QMainWindow *m_admin_diag;
  QMainWindow *m_all_diag;
  QMainWindow *m_customquery_diag;
  QMainWindow *m_error_diag;
  QMainWindow *m_history_diag;
  QMainWindow *m_members_diag;
  QMap<QString, QHash<QString, QString> > m_branches;
  QMap<QString, QHash<QString, QString> > m_sruMaps;
  QMessageBox *m_about;
  QMultiMap<QString, QHash<QString, QString> > m_z3950Maps;
  QPair<QColor, QColor> m_aboutColors;
  QPointer<QMenu> m_menu;
  QPointer<biblioteq_dbenumerations> db_enumerations;
  QPointer<biblioteq_sqlite_merge_databases> m_sqliteMergeDatabases;
  QSqlDatabase m_db;
  QSqlQuery *m_searchQuery;
  QString m_engUserinfoTitle;
  QString m_lastCategory;
  QString m_lastSearchStr;
  QString m_previousTypeFilter;
  QString m_roles;
  QStringList m_deletedAdmins;
  QTextBrowser *m_printPreview;
  QTimer m_aboutTimer;
  QToolButton *m_error_bar_label;
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
  biblioteq_batch_activities *m_batchActivities;
  biblioteq_files *m_files;
  biblioteq_import *m_import;
  biblioteq_otheroptions *m_otheroptions;
  biblioteq_sql_syntax_highlighter *m_sqlSyntaxHighlighter;
  bool m_allSearchShown;
  bool m_membersWasRefreshed;
  int m_lastSearchType;
  qint64 m_pages;
  qint64 m_queryOffset;
  quint64 m_idCt;
  userinfo_diag_class *userinfo_diag;
  woody_collapse_expand_tool_button *m_woody;
  QString dbUserName(void) const;
  QString reservationHistoryHtml(void) const;
  QString viewHtml(void) const;
  QWidget *widgetForAction(QAction *action) const;
  bool emptyContainers(void);
  bool isCurrentItemAPhotograph(void) const;
  void addConfigOptions(const QString &typefilter);
  void adminSetup(void);
  void changeEvent(QEvent *event);
  void cleanup(void);
  void closeEvent(QCloseEvent *event);
  void createConfigToolMenu(void);
  void createSqliteMenuActions(void);
  void deleteItem(const QString &oid, const QString &itemType);
  void executeCustomQuery(QWidget *widget, const QString &text);
  void exportAsCSV(QTableWidget *table, QWidget *parent, const QString &title);
  void initialUpdate(void);
  void populateFavorites(void);
  void prepareContextMenus(void);
  void prepareFilter(void);
  void preparePhotographsPerPageMenu(void);
  void prepareRequestToolButton(const QString &typefilter);
  void prepareReservationHistoryMenu(void);
  void prepareTearOffMenus(void);
  void prepareUpgradeNotification(void);
  void readConfig(void);
  void readGlobalSetup(void);
  void resetAdminBrowser(void);
  void resetMembersBrowser(void);

 private slots:
  void slotAbout(void);
  void slotAddAdmin(void);
  void slotAddBorrower(void);
  void slotAdminCheckBoxClicked(int state);
  void slotAdminContextMenu(const QPoint &point);
  void slotAllGo(void);
  void slotAllowAnyUserEmail(bool state);
  void slotAnimateAbout(void);
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
  void slotContextMenu(const QPoint &point);
  void slotContributors(void);
  void slotCopyError(void);
  void slotCustomQuery(void);
  void slotDVDSearch(void);
  void slotDelete(void);
  void slotDeleteAdmin(void);
  void slotDeleteFavoriteQuery(void);
  void slotDisconnect(void);
  void slotDisplayNewSqliteDialog(void);
  void slotDuplicate(void);
  void slotExecuteCustomQuery(void);
  void slotExit(void);
  void slotExportAsCSV(void);
  void slotExportAsPNG(void);
  void slotExportMembersAsCSV(void);
  void slotExportMembersHistoryAsCSV(void);
  void slotGeneralSearchPublicationDateEnabled(bool state);
  void slotGenerateAndCopyMemberLetter(void);
  void slotGrantPrivileges(void);
  void slotGraphicsSceneDeleteKeyPressed(void);
  void slotGraphicsSceneEnterKeyPressed(void);
  void slotGreyLiteratureSearch(void);
  void slotInsertBook(void);
  void slotInsertCD(void);
  void slotInsertDVD(void);
  void slotInsertGreyLiterature(void);
  void slotInsertJourn(void);
  void slotInsertMag(void);
  void slotInsertPhotograph(void);
  void slotInsertVideoGame(void);
  void slotItemChanged(QTableWidgetItem *item);
  void slotJournSearch(void);
  void slotLanguageChanged(void);
  void slotLastWindowClosed(void);
  void slotLaunchEmailSoftware(void);
  void slotListOverdueItems(void);
  void slotListReservedItems(const QString &);
  void slotListReservedItems(void);
  void slotLoadFavorite(void);
  void slotMagSearch(void);
  void slotMainTableDeleteKeyPressed(void);
  void slotMainTableEnterKeyPressed(void);
  void slotMainWindowCanvasBackgroundColorChanged(const QColor &color);
  void slotMembersContextMenu(const QPoint &point);
  void slotMembersPagesChanged(int value);
  void slotMergeSQLiteDatabases(void);
  void slotModify(void);
  void slotModifyBorrower(void);
  void slotNextPage(void);
  void slotOpenOnlineDocumentation(void);
  void slotOpenPDFFiles(void);
  void slotOtherOptionsSaved(void);
  void slotPageClicked(const QString &link);
  void slotPhotographSearch(void);
  void slotPhotographsPerPageChanged(void);
  void slotPopulateMembersBrowser(void);
  void slotPreviewCanvasBackgroundColor(const QColor &color);
  void slotPreviousPage(void);
  void slotPrintIconsView(void);
  void slotPrintPreview(QPrinter *printer);
  void slotPrintReservationHistory(void);
  void slotPrintReservationHistoryPreview(void);
  void slotPrintReserved(void);
  void slotPrintView(void);
  void slotPrintViewPreview(void);
  void slotRefresh(void);
  void slotRefreshAdminList(void);
  void slotRefreshCustomQuery(void);
  void slotReloadBiblioteqConf(void);
  void slotRemoveMember(void);
  void slotRenameFavoriteQuery(void);
  void slotRequest(void);
  void slotReserveCopy(void);
  void slotReset(void);
  void slotResetAllSearch(void);
  void slotResetErrorLog(void);
  void slotResetLoginDialog(void);
  void slotResizeColumnsAfterSort(void);
  void slotRoleChanged(int index);
  void slotSaveAdministrators(void);
  void slotSaveConfig(void);
  void slotSaveCustomQuery(void);
  void slotSaveDnt(bool state);
  void slotSaveGeneralSearchCaseSensitivity(bool state);
  void slotSavePassword(void);
  void slotSaveUser(void);
  void slotSceneSelectionChanged(void);
  void slotSearch(void);
  void slotSearchBasic(void);
  void slotSectionResized(int logicalIndex, int oldSize, int newSize);
  void slotSelectDatabaseFile(void);
  void slotSetColumns(void);
  void slotSetFonts(void);
  void slotSetMembershipFees(void);
  void slotShowAdminDialog(void);
  void slotShowBatchActivitiesBrowser(void);
  void slotShowChangePassword(void);
  void slotShowConnectionDB(void);
  void slotShowCustomQuery(void);
  void slotShowDbEnumerations(void);
  void slotShowDocumentation(void);
  void slotShowErrorDialog(void);
  void slotShowFiles(void);
  void slotShowGrid(void);
  void slotShowHistory(void);
  void slotShowImport(void);
  void slotShowMembersBrowser(void);
  void slotShowMenu(void);
  void slotShowNext(void);
  void slotShowOtherOptions(void);
  void slotShowPassword(bool state);
  void slotShowPrev(void);
  void slotShowReleaseNotes(void);
  void slotSqliteFileSelected(bool state);
  void slotTableFindNext(void);
  void slotTableFindTextCleared(const QString &text);
  void slotTearOffMenus(void);
  void slotUpdateIndicesAfterSort(int column);
  void slotUpgradeSqliteScheme(void);
  void slotVacuum(void);
  void slotVideoGameSearch(void);
  void slotViewDetails(void);
  void slotViewFullOrNormalScreen(void);

 signals:
  void fontChanged(const QFont &font);
};

#endif
