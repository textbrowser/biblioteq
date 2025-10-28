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

#include <QActionGroup>
#include <QClipboard>
#include <QFileDialog>
#include <QFontDialog>
#include <QInputDialog>
#include <QPrintPreviewDialog>
#include <QScrollBar>
#include <QSettings>
#include <QSysInfo>
#include <QTranslator>
#include <QtDebug>
#include <QtMath>

#include <iostream>
#include <limits>

#ifdef BIBLIOTEQ_POPPLER_VERSION_DEFINED
#include <poppler-version.h>
#endif

#ifdef BIBLIOTEQ_LINKED_WITH_YAZ
extern "C"
{
#include <yaz/yaz-version.h>
}
#endif

#ifdef Q_OS_ANDROID
#if (QT_VERSION >= QT_VERSION_CHECK(6, 1, 0))
#include <QJniObject>
#endif
#endif

#ifdef Q_OS_MACOS
#include "CocoaInitializer.h"
#endif

#include "biblioteq.h"
#include "biblioteq_bgraphicsscene.h"
#include "biblioteq_custom_query.h"
#include "biblioteq_otheroptions.h"
#include "biblioteq_query_history.h"
#include "biblioteq_sqlite_create_schema.h"
#include "biblioteq_swifty.h"

/*
** -- Global Variables --
*/

QSize biblioteq::s_noImageResize = QSize(126, 187);
QString biblioteq::s_databaseDateFormat = "MM/dd/yyyy";
QString biblioteq::s_databaseTimeFormat = "hh:mm:ss";
QString biblioteq::s_empty = "(Empty)";
QString biblioteq::s_locale = "";
QString biblioteq::s_unknown = "UNKNOWN";
QTranslator *biblioteq::s_appTranslator = nullptr;
QTranslator *biblioteq::s_qtTranslator = nullptr;
qreal biblioteq::PROGRESS_DIALOG_WIDTH_MULTIPLIER = 2.5;

int main(int argc, char *argv[])
{
#if defined(Q_OS_MACOS) || defined(Q_OS_WINDOWS)
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
  QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
#endif
#endif
  QCoreApplication::setApplicationName("BiblioteQ");
  QCoreApplication::setOrganizationName("BiblioteQ");
  QSettings::setDefaultFormat(QSettings::IniFormat);
  QSettings::setPath
    (QSettings::IniFormat, QSettings::UserScope, biblioteq::homePath());
  biblioteq_otheroptions::prepareEnvironmentVariables();

  QApplication qapplication(argc, argv);
  auto font(qapplication.font());

  font.setStyleStrategy
    (QFont::StyleStrategy(QFont::PreferAntialias | QFont::PreferQuality));
  qapplication.setFont(font);

#ifdef Q_OS_MACOS
  /*
  ** Eliminate warnings.
  */

  CocoaInitializer ci;
#endif

  qapplication.setWindowIcon(QIcon(":/book.png"));

  /*
  ** Prepare configuration settings.
  */

  QDir().mkdir(biblioteq::homePath());

  /*
  ** Remove old configuration settings.
  */

  QSettings settings;

  settings.remove("SQLITECustom_header_state");
  settings.remove("automatically_resize_columns");
  settings.remove("column_settings_cleared_v6_51");
  settings.remove("entries_per_page");
  settings.remove("filesPerPage");
  settings.remove("main_window_geometry");
  settings.remove("mainwindowState");
  settings.remove("otheroptions/book_publication_date_format");
  settings.remove("otheroptions/cd_publication_date_format");
  settings.remove("otheroptions/dvd_publication_date_format");
  settings.remove("otheroptions/journal_publication_date_format");
  settings.remove("otheroptions/magazine_publication_date_format");
  settings.remove("otheroptions/photograph_publication_date_format");
  settings.remove("otheroptions/videogame_publication_date_format");
  settings.remove("showTabBar");
  settings.remove("sqlite_db");
  settings.remove("tabbedItemWindows");
  settings.remove("tearOffMenus");
  biblioteq::s_appTranslator = new QTranslator(nullptr);
  biblioteq::s_locale = settings.value("locale").toString();
  biblioteq::s_qtTranslator = new QTranslator(nullptr);

  if(!(biblioteq::s_locale == "ar_JO" ||
       biblioteq::s_locale == "cs_CZ" ||
       biblioteq::s_locale == "de_DE" ||
       biblioteq::s_locale == "el_GR" ||
       biblioteq::s_locale == "en_US" ||
       biblioteq::s_locale == "es_AR" ||
       biblioteq::s_locale == "es_ES" ||
       biblioteq::s_locale == "fr_FR" ||
       biblioteq::s_locale == "hu_HU" ||
       biblioteq::s_locale == "nl_BE" ||
       biblioteq::s_locale == "nl_NL" ||
       biblioteq::s_locale == "pl_PL" ||
       biblioteq::s_locale == "pt_PT" ||
       biblioteq::s_locale == "ro_RO" ||
       biblioteq::s_locale == "ru_RU"))
    biblioteq::s_locale = QLocale::system().name();

  if(biblioteq::
     s_appTranslator->load(":/biblioteq_" + biblioteq::s_locale + ".qm"))
    {
      if(qapplication.installTranslator(biblioteq::s_appTranslator))
	{
	  biblioteq::s_empty = QObject::tr("(Empty)");
	  biblioteq::s_unknown = QObject::tr("UNKNOWN");
	}
      else
	{
	  biblioteq::s_empty = "(Empty)";
	  biblioteq::s_unknown = "UNKNOWN";
	}
    }

  if(biblioteq::
     s_qtTranslator->load(":/qtbase_" + biblioteq::s_locale + ".qm"))
    qapplication.installTranslator(biblioteq::s_qtTranslator);

  biblioteq biblioteq;

  biblioteq.showMain();
  return static_cast<int> (qapplication.exec());
}

biblioteq::biblioteq(void):QMainWindow()
{
  m_swifty = new swifty
    (BIBLIOTEQ_VERSION,
     "#define BIBLIOTEQ_VERSION_LTS",
     QUrl::fromUserInput(BIBLIOTEQ_VERSION_FILE_URL),
     this);
  connect(m_swifty,
	  SIGNAL(different(const QString &)),
	  this,
	  SLOT(slotSwifty(void)));

  QMenu *menu1 = nullptr;

  ui.setupUi(this);
  ui.table->setQMain(this);

  if(menuBar())
    menuBar()->setNativeMenuBar(true);

  m_about = nullptr;
#ifdef Q_OS_ANDROID
  m_admin_diag = new QMainWindow(this);
#else
  m_admin_diag = new QMainWindow();
#endif
  m_allSearchShown = false;
#ifdef Q_OS_ANDROID
  m_all_diag = new QMainWindow(this);
#else
  m_all_diag = new QMainWindow();
#endif
  m_batchActivities = new biblioteq_batch_activities(this);
  m_branch_diag = new QDialog(this);
  m_connected_bar_label = nullptr;
  m_error_bar_label = nullptr;
#ifdef Q_OS_ANDROID
  m_error_diag = new QMainWindow(this);
#else
  m_error_diag = new QMainWindow();
#endif
  m_files = nullptr;
#ifdef Q_OS_ANDROID
  m_history_diag = new QMainWindow(this);
#else
  m_history_diag = new QMainWindow();
  m_history_diag->setWindowFlags
    (Qt::WindowStaysOnTopHint | m_history_diag->windowFlags());
#endif
  m_idCt = 0;
  m_import = new biblioteq_import(this);
  m_lastSearchType = POPULATE_ALL;
  m_magic = new biblioteq_magic(this);
  m_membersWasRefreshed = false;
#ifdef Q_OS_ANDROID
  m_members_diag = new QMainWindow(this);
#else
  m_members_diag = new QMainWindow();
  m_members_diag->setWindowFlags
    (Qt::WindowStaysOnTopHint | m_members_diag->windowFlags());
#endif
  m_menuCategoryActionGroup = new QActionGroup(this);
  m_menuCategoryActionGroup->setExclusive(true);
  m_otherOptions = new biblioteq_otheroptions(this);
  m_overdueItemsTimer.setInterval(5000);
  m_pages = 0;
  m_pass_diag = new QDialog(this);
  m_previousTypeFilter = "";
  m_printPreview = new QTextBrowser(this);
  m_printPreview->setVisible(false);
  m_queryHistory = new biblioteq_query_history(this);
  m_queryOffset = 0;
  m_resizeTimer.setInterval(500);
  m_resizeTimer.setSingleShot(true);
  m_searchQuery = nullptr;
  m_status_bar_label = nullptr;
  userinfo_diag = new userinfo_diag_class(m_members_diag);
  connect(&m_overdueItemsTimer,
	  SIGNAL(timeout(void)),
	  this,
	  SLOT(slotNotifyOfOverdueItems(void)));
  connect(&m_resizeTimer,
	  SIGNAL(timeout(void)),
	  this,
	  SLOT(slotResizeTimeout(void)));
  connect(QCoreApplication::instance(),
	  SIGNAL(lastWindowClosed(void)),
	  this,
	  SLOT(slotLastWindowClosed(void)));
  connect(m_batchActivities,
	  SIGNAL(listMembersReservedItems(const QString &)),
	  this,
	  SLOT(slotListReservedItems(const QString &)));
  connect(m_queryHistory,
	  SIGNAL(executeQuery(const QString &)),
	  this,
	  SLOT(slotExecuteQuery(const QString &)));
  connect(this,
	  SIGNAL(queryCompleted(const QString &)),
	  m_queryHistory,
	  SLOT(slotQueryCompleted(const QString &)));
  menu1 = new QMenu(this);
  connect(menu1->addAction(tr("Reset ID Number")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Title")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Publication Date")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Publisher")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Categories")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Price")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Language")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Monetary Units")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Abstract")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Copies")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Location")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Keywords")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Availability")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Case-Insensitive")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  ui.action_Open_PDF_File->setEnabled(false);
#if defined(BIBLIOTEQ_LINKED_WITH_POPPLER) || \
    defined(BIBLIOTEQ_QT_PDF_SUPPORTED)
  ui.action_Open_PDF_File->setEnabled(true);
#endif
  connect(ui.actionBatchActivitiesBrowser,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowBatchActivitiesBrowser(void)));
  connect(ui.actionCenter_Child_Windows,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotActionToggled(void)));
  connect(ui.actionChangePassword,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowChangePassword(void)));
  connect(ui.actionConnect,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowConnectionDB(void)));
  connect(ui.actionDisconnect,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotDisconnect(void)));
  connect(ui.actionDisplay_Tab_Bar,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotActionToggled(void)));
  connect(ui.actionDownload_Version_Information,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotActionToggled(void)));
  connect(ui.actionImportCSV,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowImport(void)));
  connect(ui.actionMagic,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowMagic(void)));
  connect(ui.actionMusic_CD,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotInsertCD(void)));
  connect(ui.actionOther_Options,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowOtherOptions(void)));
  connect(ui.actionOverdue_Items_Notification,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotActionToggled(void)));
  connect(ui.actionPatron_Reservation_History,
	  SIGNAL(toggled(bool)),
	  this,
	  SLOT(slotSaveDnt(bool)));
  connect(ui.actionQuery_History,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowQueryHistory(void)));
  connect(ui.actionRequests,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotRequest(void)));
  connect(ui.actionSaveSettings,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotSaveConfig()));
  connect(ui.actionSelect_All,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotSelectAll(void)));
  connect(ui.actionStatistics,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowStatistics(void)));
  connect(ui.actionTabbed_Item_Windows,
	  SIGNAL(toggled(bool)),
	  this,
	  SLOT(slotTabbedItemWindows(bool)));
  connect(ui.actionTearOffMenus,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotTearOffMenus(void)));
  connect(ui.action_Book,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotInsertBook(void)));
  connect(ui.action_Contributors,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotContributors(void)));
  connect(ui.action_English,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowDocumentation(void)));
  connect(ui.action_English_Release_Notes,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowReleaseNotes(void)));
  connect(ui.action_French,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowDocumentation(void)));
  connect(ui.action_French_Release_Notes,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowReleaseNotes(void)));
  connect(ui.action_Grey_Literature,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotInsertGreyLiterature(void)));
  connect(ui.action_Online_Documentation,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotOpenOnlineDocumentation(void)));
  connect(ui.action_Open_PDF_File,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotOpenPDFFiles(void)));
  connect(ui.action_Print_Icons_View,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotPrintIconsView(void)));
  connect(ui.action_DVD,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotInsertDVD(void)));
  connect(ui.action_Full_Screen,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotViewFullOrNormalScreen(void)));
  connect(ui.action_Journal,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotInsertJourn(void)));
  connect(ui.action_Magazine,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotInsertMag(void)));
  connect(ui.action_Merge_SQLite_Databases,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotMergeSQLiteDatabases(void)));
  connect(ui.action_Photograph_Collection,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotInsertPhotograph(void)));
  connect(ui.action_Upgrade_SQLite_SchemaAll,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotUpgradeSqliteScheme(void)));
  connect(ui.action_Upgrade_SQLite_SchemaRecent,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotUpgradeSqliteScheme(void)));
  connect(ui.action_VacuumDatabase,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotVacuum(void)));
  connect(ui.action_Video_Game,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotInsertVideoGame(void)));
  connect(ui.disconnectTool,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotDisconnect(void)));
  connect(ui.find,
	  SIGNAL(returnPressed(void)),
	  this,
	  SLOT(slotTableFindNext(void)));
  connect(ui.find,
	  SIGNAL(textChanged(const QString &)),
	  this,
	  SLOT(slotTableFindTextCleared(const QString &)));
  connect(ui.next,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotTableFindNext(void)));
  connect(ui.resetAllSearch,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotResetAllSearch(void)));

  auto scene = new biblioteq_bgraphicsscene(ui.graphicsView);

  connect(scene,
	  SIGNAL(selectionChanged(void)),
	  this,
	  SLOT(slotSceneSelectionChanged(void)));
  ui.graphicsView->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
  ui.graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
  ui.graphicsView->setRubberBandSelectionMode(Qt::IntersectsItemShape);
  ui.graphicsView->setScene(scene);
  ab.setupUi(m_admin_diag);
  al.setupUi(m_all_diag);
  bb.setupUi(m_members_diag);
  br.setupUi(m_branch_diag);
  er.setupUi(m_error_diag);
  history.setupUi(m_history_diag);
  pass.setupUi(m_pass_diag);
  ab.splitter->setStretchFactor(0, 0);
  ab.splitter->setStretchFactor(1, 1);
  al.quantity->setMaximum(static_cast<int> (biblioteq::Limits::QUANTITY));
  bb.menuBar ? bb.menuBar->setNativeMenuBar(true) : (void) 0;
  m_branch_diag->setModal(true);
  m_otherOptions->prepareMembersVisibleColumns(bb.table);
  m_pass_diag->setModal(true);
#ifdef Q_OS_ANDROID
  ui.action_Full_Screen->setEnabled(false);
#endif
  userinfo_diag->setModal(true);
  connect(ab.addButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAddAdmin(void)));
#ifdef Q_OS_ANDROID
  connect(ab.cancelButton,
	  SIGNAL(clicked(void)),
	  m_admin_diag,
	  SLOT(hide(void)));
#else
  connect(ab.cancelButton,
	  SIGNAL(clicked(void)),
	  m_admin_diag,
	  SLOT(close(void)));
#endif
  connect(ab.deleteButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotDeleteAdmin(void)));
  connect(ab.reloadButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRefreshAdminList(void)));
  connect(ab.saveButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSaveAdministrators(void)));
#ifdef Q_OS_ANDROID
  connect(al.cancelButton,
	  SIGNAL(clicked(void)),
	  m_all_diag,
	  SLOT(hide(void)));
#else
  connect(al.cancelButton,
	  SIGNAL(clicked(void)),
	  m_all_diag,
	  SLOT(close(void)));
#endif
  connect(al.caseinsensitive,
	  SIGNAL(toggled(bool)),
	  this,
	  SLOT(slotSaveGeneralSearchCaseSensitivity(bool)));
  connect(al.okButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAllGo(void)));
  connect(al.resetButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(bb.action_Export,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotExportMembersAsCSV(void)));
  connect(bb.action_Set_Membership_Fees,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotSetMembershipFees(void)));
  connect(bb.addButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAddBorrower(void)));
  connect(bb.cancelButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotCloseMembersBrowser(void)));
  connect(bb.checkoutButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotCheckout(void)));
  connect(bb.database_enumerations_browser_label,
	  SIGNAL(linkActivated(const QString &)),
	  this,
	  SLOT(slotShowDbEnumerations(void)));
  connect(bb.deleteButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRemoveMember(void)));
  connect(bb.filter,
	  SIGNAL(returnPressed(void)),
	  this,
	  SLOT(slotPopulateMembersBrowser(void)));
  connect(bb.grantButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotGrantPrivileges(void)));
  connect(bb.historyButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotShowHistory(void)));
  connect(bb.listButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotListReservedItems(void)));
  connect(bb.modifyButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotModifyBorrower(void)));
  connect(bb.overdueButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotListOverdueItems(void)));
  connect(bb.pages,
	  SIGNAL(valueChanged(int)),
	  this,
	  SLOT(slotMembersPagesChanged(int)));
  connect(bb.printButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotPrintReserved(void)));
  connect(bb.reloadButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotPopulateMembersBrowser(void)));
  connect(bb.table->horizontalHeader(),
	  SIGNAL(sectionClicked(int)),
	  this,
	  SLOT(slotResizeColumnsAfterSort(void)));
  connect(br.branch_name,
	  SIGNAL(activated(int)),
	  this,
	  SLOT(slotBranchChanged(void)));
#ifdef Q_OS_ANDROID
  connect(br.cancelButton,
	  SIGNAL(clicked(void)),
	  m_branch_diag,
	  SLOT(hide(void)));
#else
  connect(br.cancelButton,
	  SIGNAL(clicked(void)),
	  m_branch_diag,
	  SLOT(close(void)));
#endif
  connect(br.fileButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSelectDatabaseFile(void)));
  connect(br.okButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotConnectDB(void)));
  connect(br.resetButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotResetLoginDialog(void)));
  connect(br.role,
	  SIGNAL(currentIndexChanged(int)),
	  this,
	  SLOT(slotRoleChanged(int)));
  connect(br.show_password,
	  SIGNAL(toggled(bool)),
	  this,
	  SLOT(slotShowPassword(bool)));
#ifdef Q_OS_ANDROID
  connect(er.cancelButton,
	  SIGNAL(clicked(void)),
	  m_error_diag,
	  SLOT(hide(void)));
#else
  connect(er.cancelButton,
	  SIGNAL(clicked(void)),
	  m_error_diag,
	  SLOT(close(void)));
#endif
  connect(er.copyButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotCopyError(void)));
  connect(er.resetButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotResetErrorLog(void)));
  connect(er.table->horizontalHeader(),
	  SIGNAL(sectionClicked(int)),
	  this,
	  SLOT(slotResizeColumnsAfterSort(void)));
  connect(history.actionExport_As_CSV,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotExportMembersHistoryAsCSV(void)));
#ifdef Q_OS_ANDROID
  connect(history.cancelButton,
	  SIGNAL(clicked(void)),
	  m_history_diag,
	  SLOT(hide(void)));
#else
  connect(history.cancelButton,
	  SIGNAL(clicked(void)),
	  m_history_diag,
	  SLOT(close(void)));
#endif
  connect(history.dnt,
	  SIGNAL(toggled(bool)),
	  this,
	  SLOT(slotSaveDnt(bool)));
  connect(history.nextTool,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotShowNext(void)));
  connect(history.prevTool,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotShowPrev(void)));
  connect(history.printButton,
	  SIGNAL(clicked(void)),
	  history.printButton,
	  SLOT(showMenu(void)));
  connect(history.reloadButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotShowHistory(void)));
  connect(history.table->horizontalHeader(),
	  SIGNAL(sectionClicked(int)),
	  this,
	  SLOT(slotResizeColumnsAfterSort(void)));
  connect(m_otherOptions,
	  SIGNAL(mainWindowCanvasBackgroundColorChanged(const QColor &)),
	  this,
	  SLOT(slotMainWindowCanvasBackgroundColorChanged(const QColor &)));
  connect(m_otherOptions,
	  SIGNAL(mainWindowCanvasBackgroundColorPreview(const QColor &)),
	  this,
	  SLOT(slotPreviewCanvasBackgroundColor(const QColor &)));
  connect(m_otherOptions,
	  SIGNAL(saved(void)),
	  this,
	  SLOT(slotOtherOptionsSaved(void)));
  connect(m_pass_diag,
	  SIGNAL(finished(int)),
	  this,
	  SLOT(slotClosePasswordDialog(void)));
  connect(pass.cancelButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotClosePasswordDialog(void)));
  connect(pass.okButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSavePassword(void)));
  connect(ui.actionAbout,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotAbout(void)));
  connect(ui.actionConfigureAdministratorPrivileges,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowAdminDialog(void)));
  connect(ui.actionDatabase_Enumerations,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowDbEnumerations(void)));
  connect(ui.actionDatabaseSearch,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotSearch(void)));
  connect(ui.actionExport_Current_View,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotExportAsCSV(void)));
  connect(ui.actionExport_View_as_PNG,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotExportAsPNG(void)));
  connect(ui.actionDeleteEntry,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotDelete(void)));
  connect(ui.actionDuplicateEntry,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotDuplicate(void)));
  connect(ui.actionExit,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotExit(void)));
  connect(ui.actionMembersBrowser,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowMembersBrowser(void)));
  connect(ui.actionModifyEntry,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotModify(void)));
  connect(ui.actionRefreshTable,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotRefresh(void)));
  connect(ui.actionReload_biblioteq_conf,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReloadBiblioteqConf(void)));
  connect(ui.actionReservationHistory,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowHistory(void)));
  connect(ui.actionResizeColumns,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotResizeColumns(void)));
  connect(ui.actionSetGlobalFonts,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotSetFonts(void)));
  connect(ui.actionShowErrorDialog,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowErrorDialog(void)));
  connect(ui.actionShowGrid,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowGrid(void)));
  connect(ui.actionViewDetails,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotViewDetails(void)));
  connect(ui.action_Database_Enumerations,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowDbEnumerations(void)));
  connect(ui.action_New_SQLite_Database,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotDisplayNewSqliteDialog(void)));
  connect(ui.configTool,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowMenu(void)));
  connect(ui.connectTool,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowConnectionDB(void)));
  connect(ui.createTool,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowMenu(void)));
  connect(ui.customQueryTool,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowCustomQuery(void)));
  connect(ui.deleteTool,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotDelete(void)));
  connect(ui.detailsTool,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotViewDetails(void)));
  connect(ui.duplicateTool,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotDuplicate(void)));
  connect(ui.exitTool,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotExit(void)));
  connect(ui.filesTool,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowFiles(void)));
  connect(ui.graphicsView->scene(),
	  SIGNAL(deleteKeyPressed(void)),
	  this,
	  SLOT(slotGraphicsSceneDeleteKeyPressed(void)));
  connect(ui.graphicsView->scene(),
	  SIGNAL(enterKeyPressed(void)),
	  this,
	  SLOT(slotGraphicsSceneEnterKeyPressed(void)));
  connect(ui.menuExternal_Applications,
	  SIGNAL(aboutToShow(void)),
	  this,
	  SLOT(slotAboutToShowExternalApplicationsMenu(void)));
  connect(ui.menuItem_Pages,
	  SIGNAL(aboutToShow(void)),
	  this,
	  SLOT(slotAboutToShowItemsPagesMenu(void)));
  connect(ui.menu_Category,
	  SIGNAL(triggered(QAction *)),
	  this,
	  SLOT(slotAutoPopOnFilter(QAction *)));
  connect(ui.modifyTool,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotModify(void)));
  connect(ui.nextPageButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotNextPage(void)));
  connect(ui.pagesLabel,
	  SIGNAL(linkActivated(const QString &)),
	  this,
	  SLOT(slotPageClicked(const QString &)));
  connect(ui.previousPageButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotPreviousPage(void)));
  connect(ui.printTool,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowMenu(void)));
  connect(ui.refreshTool,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotRefresh(void)));
  connect(ui.reserveTool,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReserveCopy(void)));
  connect(ui.search,
	  SIGNAL(returnPressed(void)),
	  this,
	  SLOT(slotSearchBasic(void)));
  connect(ui.searchTool,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowMenu(void)));
  connect(ui.tab,
	  SIGNAL(tabCloseRequested(int)),
	  this,
	  SLOT(slotTabClosed(int)));
  connect(ui.table,
	  SIGNAL(deleteKeyPressed(void)),
	  this,
	  SLOT(slotMainTableDeleteKeyPressed(void)));
  connect(ui.table,
	  SIGNAL(enterKeyPressed(void)),
	  this,
	  SLOT(slotMainTableEnterKeyPressed(void)));
  connect(ui.table,
	  SIGNAL(itemSelectionChanged(void)),
	  this,
	  SLOT(slotDisplaySummary(void)));
  connect(ui.table->horizontalHeader(),
	  SIGNAL(sectionClicked(int)),
	  this,
	  SLOT(slotUpdateIndicesAfterSort(int)));
  connect(ui.table->horizontalHeader(),
	  SIGNAL(sectionPressed(int)),
	  this,
	  SLOT(slotResizeColumnsAfterSort(void)));
  connect(ui.table->horizontalHeader(),
	  SIGNAL(sectionResized(int, int, int)),
	  this,
	  SLOT(slotSectionResized(int, int, int)));
  connect(ui.userTool,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowMembersBrowser(void)));
  connect(userinfo_diag->m_userinfo.cancelButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotCancelAddUser(void)));
  connect(userinfo_diag->m_userinfo.menu,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotShowMenu(void)));
  connect(userinfo_diag->m_userinfo.okButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSaveUser(void)));
  connect(userinfo_diag->m_userinfo.nextTool,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotShowNext(void)));
  connect(userinfo_diag->m_userinfo.prevTool,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotShowPrev(void)));
  ab.table->setContextMenuPolicy(Qt::CustomContextMenu);
  ab.table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  bb.table->setContextMenuPolicy(Qt::CustomContextMenu);
  bb.table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  er.table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  history.table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  ui.table->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(ab.table,
	  SIGNAL(customContextMenuRequested(const QPoint &)),
	  this,
	  SLOT(slotAdminContextMenu(const QPoint &)));
  connect(bb.table,
	  SIGNAL(customContextMenuRequested(const QPoint &)),
	  this,
	  SLOT(slotMembersContextMenu(const QPoint &)));
  connect(ui.table,
	  SIGNAL(customContextMenuRequested(const QPoint &)),
	  this,
	  SLOT(slotContextMenu(const QPoint &)));
  al.publication_date_enabled->setChecked(false);
  al.resetButton->setMenu(menu1);

#ifdef Q_OS_MACOS
  if(!biblioteq_misc_functions::isEnvironmentSet("QT_STYLE_OVERRIDE"))
    foreach(auto tool_button, m_all_diag->findChildren<QToolButton *> ())
      {
#if (QT_VERSION < QT_VERSION_CHECK(5, 10, 0))
	tool_button->setStyleSheet
	  ("QToolButton {border: none; padding-right: 10px}"
	   "QToolButton::menu-button {border: none;}");
#else
	tool_button->setStyleSheet
	  ("QToolButton {border: none; padding-right: 15px}"
	   "QToolButton::menu-button {border: none; width: 15px;}");
#endif
      }
#endif

  ui.actionAutoPopulateOnCreation->setEnabled(false);
  ui.actionBatchActivitiesBrowser->setEnabled(false);
  ui.actionChangePassword->setEnabled(false);
  ui.actionDatabaseSearch->setEnabled(false);
  ui.actionDatabase_Enumerations->setEnabled(false);
  ui.actionDeleteEntry->setEnabled(false);
  ui.actionDisconnect->setEnabled(false);
  ui.actionDuplicateEntry->setEnabled(false);
  ui.actionImportCSV->setEnabled(false);
  ui.actionMagic->setEnabled(false);
  ui.actionMagic->setVisible(false);
  ui.actionMembersBrowser->setEnabled(false);
  ui.actionModifyEntry->setEnabled(false);
  ui.actionPatron_Reservation_History->setChecked(false);
  ui.actionPatron_Reservation_History->setEnabled(false);
  ui.actionPopulate_Administrator_Browser_Table_on_Display->setEnabled(false);
  ui.actionPopulate_Database_Enumerations_Browser_on_Display->setEnabled(false);
  ui.actionPopulate_Members_Browser_Table_on_Display->setEnabled(false);
  ui.actionQuery_History->setEnabled(false);
  ui.actionRefreshTable->setEnabled(false);
  ui.actionRequests->setEnabled(false);
  ui.actionReservationHistory->setEnabled(false);
  ui.actionSelect_All->setEnabled(false);
  ui.actionStatistics->setEnabled(false);
  ui.actionViewDetails->setEnabled(false);
  ui.action_Database_Enumerations->setEnabled(false);
  ui.action_Merge_SQLite_Databases->setEnabled(false);
  ui.action_Upgrade_SQLite_SchemaAll->setEnabled(false);
  ui.action_Upgrade_SQLite_SchemaRecent->setEnabled(false);
  ui.action_VacuumDatabase->setEnabled(false);
  ui.configTool->setEnabled(false);
  ui.createTool->setEnabled(false);
  ui.customQueryTool->setEnabled(false);
  ui.deleteTool->setEnabled(false);
  ui.detailsTool->setEnabled(false);
  ui.disconnectTool->setEnabled(false);
  ui.duplicateTool->setEnabled(false);
  ui.filesTool->setEnabled(false);
  ui.menuEntriesPerPage->setEnabled(false);
  ui.menu_Add_Item->setEnabled(false);
  ui.modifyTool->setEnabled(false);
  ui.nextPageButton->setEnabled(false);
  ui.previousPageButton->setEnabled(false);
  ui.printTool->setEnabled(false);
  ui.refreshTool->setEnabled(false);
  ui.reserveTool->setEnabled(false);
  ui.searchTool->setEnabled(false);
  ui.userTool->setEnabled(false);

  QSettings settings;
  QString typefilter("");

  typefilter = m_lastCategory =
    settings.value("last_category", "All").toString();
  typefilter.replace(" ", "_");
  ui.actionCenter_Child_Windows->setChecked
    (settings.value("center_child_windows", true).toBool());
  ui.actionConfigureAdministratorPrivileges->setEnabled(false);
  ui.actionDisplay_Tab_Bar->setChecked
    (settings.value("show_tab_bar", true).toBool());
  ui.actionOverdue_Items_Notification->setChecked
    (settings.value("overdue_items_notification", false).toBool());
  ui.actionTearOffMenus->setChecked
    (settings.value("tear_off_menus", true).toBool());
  ui.graphicsView->scene()->clear();
  ui.summary->setVisible(false);
  ui.tab->tabBar()->setVisible(ui.actionDisplay_Tab_Bar->isChecked());
  ui.table->resetTable(dbUserName(), m_lastCategory, m_roles);
  ui.table->setIconSize
    (m_otherOptions->showMainTableImages() ? QSize(64, 94) : QSize(0, 0));
  m_previousTypeFilter = m_lastCategory;
  prepareFilter();
  prepareItemPagesMenu();
  prepareTearOffMenus();

  auto found = false;

  for(int i = 0; i < ui.menu_Category->actions().size(); i++)
    if(m_lastCategory ==
       ui.menu_Category->actions().at(i)->data().toString())
      {
	found = true;
	ui.categoryLabel->setText(ui.menu_Category->actions().at(i)->text());
	ui.menu_Category->actions().at(i)->setChecked(true);
	ui.menu_Category->setDefaultAction(ui.menu_Category->actions().at(i));
	break;
      }

  if(!found)
    {
      ui.categoryLabel->setText(tr("All"));

      if(!ui.menu_Category->actions().isEmpty())
	ui.menu_Category->actions().at(0)->setChecked(true);

      ui.menu_Category->setDefaultAction(ui.menu_Category->actions().value(0));
    }

  addConfigOptions(m_lastCategory);
  setUpdatesEnabled(true);
  userinfo_diag->m_userinfo.expirationdate->setMaximumDate(QDate(3000, 1, 1));

  auto group1 = new QActionGroup(this);
  const int end = 21;

  for(int i = 1; i <= end; i++)
    {
      QAction *action = nullptr;

      if(i == end)
	action = group1->addAction(tr("&Unlimited"));
      else
	action = group1->addAction(tr("&%1").arg(5 * i));

      if(!action)
	continue;

      connect(action,
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotRefresh(void)));

      if(i == end)
	action->setData(-1);
      else
	action->setData(5 * i);

      action->setCheckable(true);

      if(i == 1)
	action->setChecked(true);

      ui.menuEntriesPerPage->addAction(action);
    }

  preparePhotographsPerPageMenu();
  prepareReservationHistoryMenu();

  QAction *action = nullptr;
  auto group2 = new QActionGroup(this);

  group2->setExclusive(true);
  group2->setObjectName("view_mode_menu");
  action = group2->addAction(tr("Icons Mode"));
  action->setCheckable(true);
  action->setData(0);
  connect(action,
	  SIGNAL(triggered(bool)),
	  this,
	  SLOT(slotChangeView(bool)));
  ui.menu_View->addAction(action);
  action = group2->addAction(tr("Table Mode"));
  action->setCheckable(true);
  action->setChecked(true);
  action->setData(1);
  connect(action,
	  SIGNAL(triggered(bool)),
	  this,
	  SLOT(slotChangeView(bool)));
  ui.menu_View->addAction(action);

  auto group3 = new QActionGroup(this);

  group3->setExclusive(true);
  (action = group3->addAction(tr("Arabic")))->setCheckable(true);
  action->setData("ar_JO");
  ui.menu_Language->addAction(action);
  (action = group3->addAction(tr("Czech")))->setCheckable(true);
  action->setData("cs_CZ");
  ui.menu_Language->addAction(action);
  (action = group3->addAction(tr("Dutch (Belgium)")))->setCheckable(true);
  action->setData("nl_BE");
  ui.menu_Language->addAction(action);
  (action = group3->addAction(tr("Dutch (Netherlands)")))->setCheckable(true);
  action->setData("nl_NL");
  ui.menu_Language->addAction(action);
  (action = group3->addAction(tr("English")))->setCheckable(true);
  action->setData("en_US");
  ui.menu_Language->addAction(action);
  (action = group3->addAction(tr("French")))->setCheckable(true);
  action->setData("fr_FR");
  ui.menu_Language->addAction(action);
  (action = group3->addAction(tr("German")))->setCheckable(true);
  action->setData("de_DE");
  ui.menu_Language->addAction(action);
  (action = group3->addAction(tr("Greek")))->setCheckable(true);
  action->setData("el_GR");
  ui.menu_Language->addAction(action);
  (action = group3->addAction(tr("Hebrew")))->setCheckable(true);
  action->setData("he_IL");
  ui.menu_Language->addAction(action);
  (action = group3->addAction(tr("Hungarian")))->setCheckable(true);
  action->setData("hu_HU");
  ui.menu_Language->addAction(action);
  (action = group3->addAction(tr("Polish")))->setCheckable(true);
  action->setData("pl_PL");
  ui.menu_Language->addAction(action);
  (action = group3->addAction(tr("Portuguese")))->setCheckable(true);
  action->setData("pt_PT");
  ui.menu_Language->addAction(action);
  (action = group3->addAction(tr("Romanian")))->setCheckable(true);
  action->setData("ro_RO");
  ui.menu_Language->addAction(action);  
  (action = group3->addAction(tr("Russian")))->setCheckable(true);
  action->setData("ru_RU");
  ui.menu_Language->addAction(action);
  (action = group3->addAction(tr("Spanish")))->setCheckable(true);
  action->setData("es_ES");
  ui.menu_Language->addAction(action);
  (action = group3->addAction(tr("Spanish (Argentina)")))->setCheckable(true);
  action->setData("es_AR");
  ui.menu_Language->addAction(action);

  foreach(auto action, ui.menu_Language->actions())
    {
      if(action->data().toString() == s_locale)
	action->setChecked(true);

      connect(action,
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotLanguageChanged(void)));
    }

  ui.menuExternal_Applications->setStyleSheet("QMenu {menu-scrollable: 1;}");
  ui.menuPreferredSRUSite->setStyleSheet("QMenu {menu-scrollable: 1;}");
  ui.menuPreferredZ3950Server->setStyleSheet("QMenu {menu-scrollable: 1;}");
  ui.menu_Custom_Query->setStyleSheet("QMenu {menu-scrollable: 1;}");
  ui.menu_Language->setStyleSheet("QMenu {menu-scrollable: 1;}");
  populateFavorites();

  auto validator1 = new QRegularExpressionValidator
    (QRegularExpression("\\w+"), this);

  userinfo_diag->m_userinfo.memberid->setValidator(validator1);

  /*
  ** Highlight userinfo_diag->userinfo required widgets.
  */

  biblioteq_misc_functions::highlightWidget
    (userinfo_diag->m_userinfo.city,
     m_otherOptions->membersMandatoryFieldColor());
  biblioteq_misc_functions::highlightWidget
    (userinfo_diag->m_userinfo.firstName,
     m_otherOptions->membersMandatoryFieldColor());
  biblioteq_misc_functions::highlightWidget
    (userinfo_diag->m_userinfo.lastName,
     m_otherOptions->membersMandatoryFieldColor());
  biblioteq_misc_functions::highlightWidget
    (userinfo_diag->m_userinfo.street,
     m_otherOptions->membersMandatoryFieldColor());
  biblioteq_misc_functions::highlightWidget
    (userinfo_diag->m_userinfo.zip,
     m_otherOptions->membersMandatoryFieldColor());
  ui.actionOverdue_Items_Notification->isChecked() ?
    m_overdueItemsTimer.start() : m_overdueItemsTimer.stop();
  ui.splitter->restoreState
    (settings.value("main_splitter_state").toByteArray());
  ui.splitter->setCollapsible(1, false);
  ui.splitter->setStretchFactor(0, 0);
  ui.splitter->setStretchFactor(1, 1);
}

biblioteq::~biblioteq()
{
  m_aboutTimer.stop();
  m_overdueItemsTimer.stop();
  m_resizeTimer.stop();
}

QHash<QString, QString> biblioteq::getAmazonHash(void) const
{
  return m_amazonImages;
}

QHash<QString, QString> biblioteq::getSRUHash(const QString &name) const
{
  QMapIterator<QString, QHash<QString, QString> > it(m_sruMaps);

  while(it.hasNext())
    {
      it.next();

      if(QString(it.key()).remove("&") == QString(name).remove("&"))
	return it.value();
    }

  return QHash<QString, QString> ();
}

QHash<QString, QString> biblioteq::getZ3950Hash(const QString &name) const
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QMapIterator<QString, QHash<QString, QString> > it(m_z3950Maps);
#else
  QMultiMapIterator<QString, QHash<QString, QString> > it(m_z3950Maps);
#endif

  while(it.hasNext())
    {
      it.next();

      if(QString(it.key()).remove("&") == QString(name).remove("&"))
	return it.value();
    }

  return QHash<QString, QString> ();
}

QMainWindow *biblioteq::getMembersBrowser(void) const
{
  return m_members_diag;
}

QSqlDatabase biblioteq::getDB(void) const
{
  return m_db;
}

QString biblioteq::getAdminID(void) const
{
  return dbUserName();
}

QString biblioteq::getPreferredSRUSite(void) const
{
  for(int i = 0; i < ui.menuPreferredSRUSite->actions().size(); i++)
    if(ui.menuPreferredSRUSite->actions().at(i)->isChecked())
      return ui.menuPreferredSRUSite->actions().at(i)->text();

  return "";
}

QString biblioteq::getPreferredZ3950Site(void) const
{
  for(int i = 0; i < ui.menuPreferredZ3950Server->actions().size(); i++)
    if(ui.menuPreferredZ3950Server->actions().at(i)->isChecked())
      return ui.menuPreferredZ3950Server->actions().at(i)->text();

  return "";
}

QString biblioteq::getRoles(void) const
{
  /*
  ** If m_roles is empty, the user is a guest or a patron.
  */

  return m_roles;
}

QString biblioteq::getTypeFilterString(void) const
{
  if(ui.menu_Category->defaultAction())
    return ui.menu_Category->defaultAction()->data().toString();
  else
    return "All";
}

Ui_mainWindow biblioteq::getUI(void) const
{
  return ui;
}

Ui_membersBrowser biblioteq::getBB(void) const
{
  return bb;
}

QVector<QString> biblioteq::getBBColumnIndexes(void) const
{
  return m_bbColumnHeaderIndexes;
}

void biblioteq::addConfigOptions(const QString &typefilter)
{
  createConfigToolMenu();
  m_configToolMenu->clear();

  for(int i = 0; i < ui.table->columnCount(); i++)
    {
      if(typefilter != "Custom")
	{
	  if(typefilter != "All" &&
	     typefilter != "All Available" &&
	     typefilter != "All Overdue" &&
	     typefilter != "All Requested" &&
	     typefilter != "All Reserved")
	    {
	      if(ui.table->columnNames().value(i) == "MYOID" ||
		 ui.table->columnNames().value(i) == "Type")
		continue;
	    }
	  else if(ui.table->columnNames().value(i) == "MYOID" ||
		  ui.table->columnNames().value(i) == "REQUESTOID")
	    continue;
	}

      auto action = new QAction
	(ui.table->horizontalHeaderItem(i)->text(), ui.configTool);

      action->setCheckable(true);
      action->setChecked
	(!ui.table->isColumnHidden(i, typefilter, dbUserName()));
      action->setData(typefilter);
      connect(action,
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotSetColumns(void)));
      m_configToolMenu->addAction(action);
    }
}

void biblioteq::addError(const QString &type,
			 const QString &summary,
			 const QString &error,
			 const char *file,
			 const int line)
{
  if(error.trimmed().isEmpty())
    return;

  auto const now = QDateTime::currentDateTime();

  er.table->setRowCount(er.table->rowCount() + 1);
  er.table->setSortingEnabled(false);

  for(int i = 0; i < er.table->columnCount(); i++)
    {
      auto item = new QTableWidgetItem();

      if(i == static_cast<int> (ErrorDialogColumns::EVENT_TIME))
	item->setText(now.toString("yyyy/MM/dd hh:mm:ss"));
      else if(i == static_cast<int> (ErrorDialogColumns::EVENT_TYPE))
	item->setText(type.trimmed());
      else if(i == static_cast<int> (ErrorDialogColumns::FILE))
	{
	  if(file)
	    item->setText(file);
	}
      else if(i == static_cast<int> (ErrorDialogColumns::FULL_DESCRIPTION))
	{
	  if(error.simplified().isEmpty())
	    item->setText(summary);
	  else
	    item->setText(error.simplified());
	}
      else if(i == static_cast<int> (ErrorDialogColumns::SUMMARY))
	item->setText(summary.trimmed());
      else
	item->setText(QString::number(line));

      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
      er.table->setItem(er.table->rowCount() - 1, i, item);
    }

  for(int i = 0; i < er.table->columnCount() - 1; i++)
    er.table->resizeColumnToContents(i);

  er.table->setSortingEnabled(true);

  if(m_error_bar_label != nullptr)
    {
      m_error_bar_label->setToolTip(tr("Non-Empty Error Log"));
      prepareStatusBarIcons();
    }
}

void biblioteq::adminSetup(void)
{
  bb.addButton->setEnabled(true);
  bb.checkoutButton->setEnabled(true);
  bb.deleteButton->setEnabled(true);
  bb.grantButton->setEnabled(true);
  bb.historyButton->setEnabled(true);
  bb.listButton->setEnabled(true);
  bb.modifyButton->setEnabled(true);
  bb.overdueButton->setEnabled(true);
  bb.printButton->setEnabled(true);

  if(m_db.driverName() == "QSQLITE")
    ui.actionChangePassword->setEnabled(false);
  else
    ui.actionChangePassword->setEnabled(true);

  ui.actionViewDetails->setEnabled(true);
  ui.action_VacuumDatabase->setEnabled(true);
  ui.detailsTool->setEnabled(true);

  if(m_status_bar_label != nullptr)
    {
      if(m_roles.contains("administrator"))
	m_status_bar_label->setToolTip(tr("Administrator Mode"));
      else if(m_roles == "circulation")
	m_status_bar_label->setToolTip(tr("Circulation Mode"));
      else if(m_roles == "librarian")
	m_status_bar_label->setToolTip(tr("Librarian Mode"));
      else if(m_roles == "membership")
	m_status_bar_label->setToolTip(tr("Membership Mode"));
      else
	m_status_bar_label->setToolTip(tr("Privileged Mode"));

      prepareStatusBarIcons();
    }

  if(m_roles.contains("administrator") || m_roles.contains("librarian"))
    {
      ui.graphicsView->scene()->disconnect(SIGNAL(itemDoubleClicked(void)));
      ui.table->disconnect(SIGNAL(itemDoubleClicked(QTableWidgetItem *)));
      disconnect(ui.graphicsView->scene(),
		 SIGNAL(itemDoubleClicked(void)),
		 this,
		 SLOT(slotModify(void)));
      disconnect(ui.table,
		 SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
		 this,
		 SLOT(slotModify(void)));
      connect(ui.graphicsView->scene(),
	      SIGNAL(itemDoubleClicked(void)),
	      this,
	      SLOT(slotModify(void)));
      connect(ui.table,
	      SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
	      this,
	      SLOT(slotModify(void)));
      updateItemWindows();
    }

  if(m_roles.contains("administrator") || m_roles.contains("librarian"))
    {
      ui.actionDeleteEntry->setEnabled(true);
      ui.actionDuplicateEntry->setEnabled(true);
      ui.actionImportCSV->setEnabled(true);
      ui.actionMagic->setEnabled(true);
      ui.actionModifyEntry->setEnabled(true);
      ui.actionViewDetails->setEnabled(false);
      ui.createTool->setEnabled(true);
      ui.deleteTool->setEnabled(true);
      ui.detailsTool->setEnabled(false);
      ui.duplicateTool->setEnabled(true);
      ui.menu_Add_Item->setEnabled(true);
      ui.modifyTool->setEnabled(true);
    }

  if(m_roles.contains("administrator") ||
     m_roles.contains("circulation") ||
     m_roles.contains("membership"))
    {
      ui.actionMembersBrowser->setEnabled(true);
      ui.userTool->setEnabled(true);
    }

  if(m_roles.contains("administrator") || m_roles.contains("circulation"))
    {
      ui.actionBatchActivitiesBrowser->setEnabled(true);
      ui.reserveTool->setEnabled(true);
    }

  if(m_roles.contains("administrator") || m_roles.contains("librarian"))
    ui.actionAutoPopulateOnCreation->setEnabled(true);

  ui.actionPopulate_Members_Browser_Table_on_Display->setEnabled
    (m_roles.contains("administrator"));

  if(m_db.driverName() != "QSQLITE")
    {
      ui.actionConfigureAdministratorPrivileges->setEnabled
	(m_roles.contains("administrator"));
      ui.actionDatabase_Enumerations->setEnabled
	(m_roles.contains("administrator"));
      ui.actionPopulate_Administrator_Browser_Table_on_Display->setEnabled
	(m_roles.contains("administrator"));
      ui.actionPopulate_Database_Enumerations_Browser_on_Display->setEnabled
	(m_roles.contains("administrator"));
      ui.action_Database_Enumerations->setEnabled
	(ui.actionDatabase_Enumerations->isEnabled());
    }
  else
    {
      bb.grantButton->setEnabled(false);
      ui.actionDatabase_Enumerations->setEnabled(true);
      ui.actionPopulate_Database_Enumerations_Browser_on_Display->setEnabled
	(true);
      ui.action_Database_Enumerations->setEnabled(true);
    }

  ui.actionRequests->setData
    (static_cast<int> (RequestActionItems::INACTIVE));
  ui.actionRequests->setToolTip(tr("Item Requests (PostgreSQL)"));

  /*
  ** Hide certain fields in the Members Browser.
  */

  if(m_roles == "membership")
    {
      bb.checkoutButton->setEnabled(false);
      bb.historyButton->setEnabled(false);
      bb.listButton->setEnabled(false);
      bb.overdueButton->setEnabled(false);
      bb.printButton->setEnabled(false);
    }

  if(m_roles == "circulation" || m_roles == "librarian")
    {
      bb.addButton->setEnabled(false);
      bb.deleteButton->setEnabled(false);
      bb.grantButton->setEnabled(false);
      bb.modifyButton->setEnabled(false);

      if(m_roles == "librarian")
	{
	  bb.checkoutButton->setEnabled(false);
	  ui.actionDatabase_Enumerations->setEnabled(true);
	  ui.actionPopulate_Database_Enumerations_Browser_on_Display->
	    setEnabled(true);
	  ui.action_Database_Enumerations->setEnabled(true);
	}
    }
  else
    {
      disconnect(bb.table,
		 SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
		 this,
		 SLOT(slotModifyBorrower(void)));
      connect(bb.table,
	      SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
	      this,
	      SLOT(slotModifyBorrower(void)));
      bb.addButton->setEnabled(true);

      if(m_db.driverName() != "QSQLITE")
	bb.grantButton->setEnabled(true);

      bb.deleteButton->setEnabled(true);
      bb.modifyButton->setEnabled(true);
    }

  resetAdminBrowser();
}

void biblioteq::changeEvent(QEvent *event)
{
  if(event)
    switch(event->type())
      {
      case QEvent::LanguageChange:
	{
	  ab.retranslateUi(m_admin_diag);
	  al.retranslateUi(m_all_diag);
	  bb.retranslateUi(m_members_diag);
	  br.retranslateUi(m_branch_diag);
	  er.retranslateUi(m_error_diag);
	  history.retranslateUi(m_history_diag);
	  m_findList.clear();
	  pass.retranslateUi(m_pass_diag);
	  s_empty = tr("(Empty)");
	  s_unknown = tr("UNKNOWN");
	  ui.retranslateUi(this);
	  ui.graphicsView->horizontalScrollBar()->setValue(0);
	  ui.graphicsView->resetTransform();
	  ui.graphicsView->scene()->clear();
	  ui.graphicsView->verticalScrollBar()->setValue(0);
	  ui.itemsCountLabel->setText(tr("0 Results"));
	  ui.nextPageButton->setEnabled(false);
	  ui.pagesLabel->setText(tr("1"));
	  ui.previousPageButton->setEnabled(false);
	  ui.table->resetTable
	    (dbUserName(),
	     ui.menu_Category->defaultAction() ?
	     ui.menu_Category->defaultAction()->data().toString() : "All",
	     m_roles);
	  addConfigOptions(m_lastCategory);
	  prepareFilter();
	  QMessageBox::information
	    (this,
	     tr("BiblioteQ: Information"),
	     tr("You may have selected a new language. Please restart "
		"BiblioteQ after saving your settings."));
	  QApplication::processEvents();
	  break;
	}
      default:
	{
	  break;
	}
      }

  QMainWindow::changeEvent(event);
}

void biblioteq::cleanup(void)
{
  if(m_db.isOpen())
    m_db.close();
}

void biblioteq::closeEvent(QCloseEvent *event)
{
  if(event)
    {
      std::unique_lock<std::mutex> lock{m_mutex, std::defer_lock};

      if(!lock.try_lock())
	{
	  event->ignore();
	  return;
	}
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!emptyContainers())
    {
      QApplication::restoreOverrideCursor();
      event ? event->ignore() : (void) 0;
      return;
    }
  else
    QApplication::restoreOverrideCursor();

  slotExit();
}

void biblioteq::createSqliteMenuActions(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  QSettings settings;
  QStringList dups;
  auto const allKeys(settings.allKeys());

  ui.menu_Recent_SQLite_Files->clear();

  for(int i = 0; i < allKeys.size(); i++)
    {
      if(!allKeys[i].startsWith("sqlite_db_"))
	continue;

      auto const str(settings.value(allKeys[i], "").toString().trimmed());

      if(str.isEmpty())
	{
	  settings.remove(allKeys[i]);
	  continue;
	}

      QFileInfo const fileInfo(str);

      if(!dups.contains(str) && fileInfo.isReadable() && fileInfo.isWritable())
	dups.append(str);
      else
	settings.remove(allKeys[i]);
    }

  std::sort(dups.begin(), dups.end());

  foreach(auto const &str, dups)
    {
      QFileInfo const fileInfo(str);
      auto action = new QAction
	(fileInfo.absoluteFilePath(), ui.menu_Recent_SQLite_Files);

      action->setData(fileInfo.absoluteFilePath());
      connect(action,
	      SIGNAL(triggered(bool)),
	      this,
	      SLOT(slotSqliteFileSelected(bool)));
      ui.menu_Recent_SQLite_Files->addAction(action);
    }

  auto action = new QAction(tr("&Clear Menu"), ui.menu_Recent_SQLite_Files);

  connect(action,
	  SIGNAL(triggered(bool)),
	  this,
	  SLOT(slotClearSqliteMenu(bool)));

  if(!ui.menu_Recent_SQLite_Files->actions().isEmpty())
    ui.menu_Recent_SQLite_Files->addSeparator();

  ui.menu_Recent_SQLite_Files->addAction(action);
  QApplication::restoreOverrideCursor();
}

void biblioteq::initialUpdate(void)
{
  /*
  ** Read the configuration file.
  */

  readConfigurationFile();

  /*
  ** Act upon the contents of the settings file.
  */

  slotShowGrid();
}

void biblioteq::prepareFilter(void)
{
  QStringList tmplist1;
  QStringList tmplist2;

  if(m_db.driverName() == "QSQLITE")
    {
      tmplist1 << "All"
	       << "All Available"
	       << "All Overdue"
	       << "All Reserved"
	       << "Books"
	       << "DVDs"
	       << "Grey Literature"
	       << "Journals"
	       << "Magazines"
	       << "Music CDs"
	       << "Photograph Collections"
	       << "Video Games";
      tmplist2 << tr("All")
	       << tr("All Available")
	       << tr("All Overdue")
	       << tr("All Reserved")
	       << tr("Books")
	       << tr("DVDs")
	       << tr("Grey Literature")
	       << tr("Journals")
	       << tr("Magazines")
	       << tr("Music CDs")
	       << tr("Photograph Collections")
	       << tr("Video Games");
    }
  else if(m_roles.contains("administrator") || m_roles.contains("circulation"))
    {
      tmplist1 << "All"
	       << "All Available"
	       << "All Overdue"
	       << "All Requested"
	       << "All Reserved"
	       << "Books"
	       << "DVDs"
	       << "Grey Literature"
	       << "Journals"
	       << "Magazines"
	       << "Music CDs"
	       << "Photograph Collections"
	       << "Video Games";
      tmplist2 << tr("All")
	       << tr("All Available")
	       << tr("All Overdue")
	       << tr("All Requested")
	       << tr("All Reserved")
	       << tr("Books")
	       << tr("DVDs")
	       << tr("Grey Literature")
	       << tr("Journals")
	       << tr("Magazines")
	       << tr("Music CDs")
	       << tr("Photograph Collections")
	       << tr("Video Games");
    }
  else
    {
      if(dbUserName() == BIBLIOTEQ_GUEST_ACCOUNT ||
	 m_roles == "librarian" ||
	 m_roles == "membership")
	{
	  tmplist1 << "All"
		   << "All Available"
		   << "Books"
		   << "DVDs"
		   << "Grey Literature"
		   << "Journals"
		   << "Magazines"
		   << "Music CDs"
		   << "Photograph Collections"
		   << "Video Games";
	  tmplist2 << tr("All")
		   << tr("All Available")
		   << tr("Books")
		   << tr("DVDs")
		   << tr("Grey Literature")
		   << tr("Journals")
		   << tr("Magazines")
		   << tr("Music CDs")
		   << tr("Photograph Collections")
		   << tr("Video Games");
	}
      else
	{
	  tmplist1 << "All"
		   << "All Available"
		   << "All Overdue"
		   << "All Requested"
		   << "All Reserved"
		   << "Books"
		   << "DVDs"
		   << "Grey Literature"
		   << "Journals"
		   << "Magazines"
		   << "Music CDs"
		   << "Photograph Collections"
		   << "Video Games";
	  tmplist2 << tr("All")
		   << tr("All Available")
		   << tr("All Overdue")
		   << tr("All Requested")
		   << tr("All Reserved")
		   << tr("Books")
		   << tr("DVDs")
		   << tr("Grey Literature")
		   << tr("Journals")
		   << tr("Magazines")
		   << tr("Music CDs")
		   << tr("Photograph Collections")
		   << tr("Video Games");
	}
    }

  disconnect(ui.menu_Category,
	     SIGNAL(triggered(QAction *)),
	     this,
	     SLOT(slotAutoPopOnFilter(QAction *)));
  ui.menu_Category->clear();

  for(int i = 0; i < m_menuCategoryActionGroup->actions().size(); i++)
    m_menuCategoryActionGroup->removeAction
      (m_menuCategoryActionGroup->actions().at(i));

  for(int i = 0; i < tmplist1.size(); i++)
    {
      auto action = ui.menu_Category->addAction(tmplist2[i]);

      if(action)
	{
	  action->setCheckable(true);
	  action->setData(tmplist1[i]);
	  m_menuCategoryActionGroup->addAction(action);
	}
    }

  connect(ui.menu_Category,
	  SIGNAL(triggered(QAction *)),
	  this,
	  SLOT(slotAutoPopOnFilter(QAction *)));
}

void biblioteq::prepareRequestToolButton(const QString &typefilter)
{
  ui.actionRequests->setData
    (static_cast<int> (RequestActionItems::INACTIVE));

  if(m_db.driverName() == "QPSQL" && m_db.isOpen())
    {
      if(dbUserName() == BIBLIOTEQ_GUEST_ACCOUNT)
	{
	  ui.actionRequests->setEnabled(false);
	  ui.actionRequests->setIcon(QIcon(":/32x32/request.png"));
	  ui.actionRequests->setToolTip(tr("Item Requests"));
	}
      else if((m_roles == "administrator" ||
	       m_roles == "circulation") &&
	      typefilter == "All Requested")
	{
	  ui.actionRequests->setData
	    (static_cast<int> (RequestActionItems::CANCEL_REQUESTED));
	  ui.actionRequests->setEnabled(true);
	  ui.actionRequests->setIcon(QIcon(":/32x32/remove_request.png"));
	  ui.actionRequests->setToolTip(tr("Cancel Selected Request(s)"));
	  }
      else if(m_roles.isEmpty() &&
	      (typefilter == "All" ||
	       typefilter == "All Available" ||
	       typefilter == "Grey Literature" ||
	       typefilter == "Books" ||
	       typefilter == "DVDs" ||
	       typefilter == "Journals" ||
	       typefilter == "Magazines" ||
	       typefilter == "Music CDs" ||
	       typefilter == "Video Games"))
	{
	  ui.actionRequests->setData
	    (static_cast<int> (RequestActionItems::REQUEST_SELECTED));
	  ui.actionRequests->setEnabled(true);
	  ui.actionRequests->setIcon(QIcon(":/32x32/request.png"));
	  ui.actionRequests->setToolTip(tr("Request Selected Item(s)"));
	}
      else if(m_roles.isEmpty() && typefilter == "All Requested")
	{
	  ui.actionRequests->setData
	    (static_cast<int> (RequestActionItems::CANCEL_REQUESTED));
	  ui.actionRequests->setEnabled(true);
	  ui.actionRequests->setIcon(QIcon(":/32x32/remove_request.png"));
	  ui.actionRequests->setToolTip(tr("Cancel Selected Request(s)"));
	}
      else if((m_roles == "administrator" || m_roles == "circulation") &&
	      typefilter == "All Reserved")
	{
	  ui.actionRequests->setData
	    (static_cast<int> (RequestActionItems::RETURN_RESERVED));
	  ui.actionRequests->setEnabled(true);
	  ui.actionRequests->setIcon(QIcon(":/32x32/remove_request.png"));
	  ui.actionRequests->setToolTip(tr("Return Selected Item(s)"));
	}
      else
	{
	  ui.actionRequests->setEnabled(false);
	  ui.actionRequests->setIcon(QIcon(":/32x32/request.png"));
	  ui.actionRequests->setToolTip(tr("Item Requests"));
	}
    }

  if(m_db.driverName() == "QSQLITE")
    {
      if((m_roles == "administrator" || m_roles == "circulation") &&
	 typefilter == "All Reserved")
	{
	  ui.actionRequests->setData
	    (static_cast<int> (RequestActionItems::RETURN_RESERVED));
	  ui.actionRequests->setEnabled(true);
	  ui.actionRequests->setIcon(QIcon(":/32x32/remove_request.png"));
	  ui.actionRequests->setToolTip(tr("Return Selected Item(s)"));
	}
      else
	{
	  ui.actionRequests->setEnabled(false);
	  ui.actionRequests->setIcon(QIcon(":/32x32/request.png"));
	  ui.actionRequests->setToolTip(tr("Item Requests (PostgreSQL)"));
	}
    }
}

void biblioteq::quit(const char *msg, const char *file, const int line)
{
  if(file && msg && qstrnlen(msg, std::numeric_limits<uint>::max()) > 0)
    qDebug() << tr(msg)
	     << tr(" in file ")
	     << file
	     << tr(", line ")
	     << line
	     << tr(".");
  else if(file)
    qDebug() << tr("An unknown error occurred in file ")
	     << file
	     << tr(", line ")
	     << line
	     << tr(".");
  else
    qDebug() << tr("An unknown error occurred in an unknown file.");

  exit(EXIT_FAILURE);
#ifdef Q_OS_ANDROID
#if (QT_VERSION >= QT_VERSION_CHECK(6, 1, 0))
  auto activity = QJniObject(QNativeInterface::QAndroidApplication::context());

  activity.callMethod<void> ("finishAndRemoveTask");
#endif
#endif
}

void biblioteq::quit(void)
{
  QCoreApplication::quit();
#ifdef Q_OS_ANDROID
#if (QT_VERSION >= QT_VERSION_CHECK(6, 1, 0))
  auto activity = QJniObject(QNativeInterface::QAndroidApplication::context());

  activity.callMethod<void> ("finishAndRemoveTask");
#endif
#endif
}

void biblioteq::removeBook(biblioteq_book *book)
{
  if(!book)
    return;

  std::unique_lock<std::mutex> lock{m_mutex, std::defer_lock};

  if(!lock.try_lock())
    return;

  book->deleteLater();
}

void biblioteq::removeCD(biblioteq_cd *cd)
{
  if(!cd)
    return;

  std::unique_lock<std::mutex> lock{m_mutex, std::defer_lock};

  if(!lock.try_lock())
    return;

  cd->deleteLater();
}

void biblioteq::removeDVD(biblioteq_dvd *dvd)
{
  if(!dvd)
    return;

  std::unique_lock<std::mutex> lock{m_mutex, std::defer_lock};

  if(!lock.try_lock())
    return;

  dvd->deleteLater();
}

void biblioteq::removeGreyLiterature(biblioteq_grey_literature *gl)
{
  if(!gl)
    return;

  std::unique_lock<std::mutex> lock{m_mutex, std::defer_lock};

  if(!lock.try_lock())
    return;

  gl->deleteLater();
}

void biblioteq::removeJournal(biblioteq_journal *journal)
{
  if(!journal)
    return;

  std::unique_lock<std::mutex> lock{m_mutex, std::defer_lock};

  if(!lock.try_lock())
    return;

  journal->deleteLater();
}

void biblioteq::removeMagazine(biblioteq_magazine *magazine)
{
  if(!magazine)
    return;

  std::unique_lock<std::mutex> lock{m_mutex, std::defer_lock};

  if(!lock.try_lock())
    return;

  magazine->deleteLater();
}

void biblioteq::removePhotographCollection(biblioteq_photographcollection *pc)
{
  if(!pc)
    return;

  std::unique_lock<std::mutex> lock{m_mutex, std::defer_lock};

  if(!lock.try_lock())
    return;

  pc->deleteLater();
}

void biblioteq::removeVideoGame(biblioteq_videogame *videogame)
{
  if(!videogame)
    return;

  std::unique_lock<std::mutex> lock{m_mutex, std::defer_lock};

  if(!lock.try_lock())
    return;

  videogame->deleteLater();
}

void biblioteq::replaceBook(const QString &id, biblioteq_book *book)
{
  Q_UNUSED(book);
  Q_UNUSED(id);
}

void biblioteq::replaceCD(const QString &id, biblioteq_cd *cd)
{
  Q_UNUSED(cd);
  Q_UNUSED(id);
}

void biblioteq::replaceDVD(const QString &id, biblioteq_dvd *dvd)
{
  Q_UNUSED(dvd);
  Q_UNUSED(id);
}

void biblioteq::replaceGreyLiterature(const QString &id,
				      biblioteq_grey_literature *gl)
{
  Q_UNUSED(gl);
  Q_UNUSED(id);
}

void biblioteq::replaceJournal(const QString &id, biblioteq_journal *journal)
{
  Q_UNUSED(id);
  Q_UNUSED(journal);
}

void biblioteq::replaceMagazine(const QString &id,
				biblioteq_magazine *magazine)
{
  Q_UNUSED(id);
  Q_UNUSED(magazine);
}

void biblioteq::replacePhotographCollection
(const QString &id, biblioteq_photographcollection *photograph)
{
  Q_UNUSED(id);
  Q_UNUSED(photograph);
}

void biblioteq::replaceVideoGame(const QString &id,
				 biblioteq_videogame *videogame)
{
  Q_UNUSED(id);
  Q_UNUSED(videogame);
}

void biblioteq::resetAdminBrowser(void)
{
  QStringList list;

  ab.table->horizontalScrollBar()->setValue(0);
  ab.table->scrollToTop();
  ab.table->setColumnCount(0);
  ab.table->setCurrentItem(nullptr);
  ab.table->setRowCount(0);
  list.append(tr("ID"));
  list.append(tr("Administrator"));
  list.append(tr("Circulation"));
  list.append(tr("Librarian"));
  list.append(tr("Membership"));
  ab.table->setColumnCount(list.size());
  ab.table->setHorizontalHeaderLabels(list);
  m_abColumnHeaderIndexes.clear();
  m_abColumnHeaderIndexes.append("ID");
  m_abColumnHeaderIndexes.append("Administrator");
  m_abColumnHeaderIndexes.append("Circulation");
  m_abColumnHeaderIndexes.append("Librarian");
  m_abColumnHeaderIndexes.append("Membership");
}

void biblioteq::resetMembersBrowser(void)
{
  QStringList list;

  bb.table->horizontalScrollBar()->setValue(0);
  bb.table->scrollToTop();
  bb.table->setColumnCount(0);
  bb.table->setCurrentItem(nullptr);
  bb.table->setRowCount(0);
  m_bbColumnHeaderIndexes.clear();
  list.append(tr("Member ID"));
  list.append(tr("First Name"));
  list.append(tr("Last Name"));
  list.append(tr("E-Mail Address"));
  list.append(tr("Telephone Number"));
  list.append(tr("Home Address"));
  list.append(tr("Member Since"));
  list.append(tr("Expiration Date"));
  list.append(tr("Membership Fees"));
  list.append(tr("Overdue Fees"));
  list.append(tr("Books Reserved"));
  list.append(tr("CDs Reserved"));
  list.append(tr("DVDs Reserved"));
  list.append(tr("Grey Literatures Reserved"));
  list.append(tr("Journals Reserved"));
  list.append(tr("Magazines Reserved"));
  list.append(tr("Video Games Reserved"));
  list.append(tr("Total Reserved"));
  m_bbColumnHeaderIndexes.append("Member ID");
  m_bbColumnHeaderIndexes.append("First Name");
  m_bbColumnHeaderIndexes.append("Last Name");
  m_bbColumnHeaderIndexes.append("E-Mail Address");
  m_bbColumnHeaderIndexes.append("Telephone Number");
  m_bbColumnHeaderIndexes.append("Home Address");
  m_bbColumnHeaderIndexes.append("Member Since");
  m_bbColumnHeaderIndexes.append("Expiration Date");
  m_bbColumnHeaderIndexes.append("Membership Fees");
  m_bbColumnHeaderIndexes.append("Overdue Fees");
  m_bbColumnHeaderIndexes.append("Books Reserved");
  m_bbColumnHeaderIndexes.append("CDs Reserved");
  m_bbColumnHeaderIndexes.append("DVDs Reserved");
  m_bbColumnHeaderIndexes.append("Grey Literatures Reserved");
  m_bbColumnHeaderIndexes.append("Journals Reserved");
  m_bbColumnHeaderIndexes.append("Magazines Reserved");
  m_bbColumnHeaderIndexes.append("Video Games Reserved");
  m_bbColumnHeaderIndexes.append("Total Reserved");
  bb.table->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  bb.table->setColumnCount(list.size());
  bb.table->setHorizontalHeaderLabels(list);

  for(int i = 0; i < bb.table->columnCount(); i++)
    {
      auto item = bb.table->horizontalHeaderItem(i);

      if(item)
	bb.table->setColumnHidden
	  (i, !m_otherOptions->isMembersColumnVisible(item->text()));

      if(i < bb.table->columnCount() - 1)
	bb.table->resizeColumnToContents(i);
    }
}

void biblioteq::setGlobalFonts(const QFont &font)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  QApplication::setFont(font);

  foreach(auto widget, QApplication::allWidgets())
    if(br.driver_warning == widget)
      continue;
    else
      {
	auto f(font);

	f.setBold(widget->font().bold());
	widget->setFont(f);
	widget->update();
      }

  auto mb = menuBar();

  if(mb)
    {
      mb->setFont(font);

      foreach(auto menu, mb->findChildren<QMenu *> ())
	{
	  foreach(auto action, menu->actions())
	    action->setFont(font);
	}

      mb->update();
    }

  emit fontChanged(font);
  QApplication::restoreOverrideCursor();
}

void biblioteq::showMain(void)
{
  if(statusBar())
    statusBar()->setStyleSheet("QStatusBar::item{border: 0px;}");

  if(m_connected_bar_label)
    m_connected_bar_label->deleteLater();

  if(m_error_bar_label)
    m_error_bar_label->deleteLater();

  if(m_status_bar_label)
    m_status_bar_label->deleteLater();

  if(statusBar())
    {
      m_connected_bar_label = new QLabel();
      m_connected_bar_label->setToolTip(tr("Disconnected"));
      m_status_bar_label = new QLabel();
      m_status_bar_label->setToolTip(tr("Standard User Mode"));
      statusBar()->addPermanentWidget(m_connected_bar_label);
      statusBar()->addPermanentWidget(m_status_bar_label);
      m_error_bar_label = new QToolButton();
      disconnect(m_error_bar_label,
		 SIGNAL(clicked(void)),
		 this,
		 SLOT(slotShowErrorDialog(void)));
      connect(m_error_bar_label,
	      SIGNAL(clicked(void)),
	      this,
	      SLOT(slotShowErrorDialog(void)));
      m_error_bar_label->setAutoRaise(true);
      m_error_bar_label->setToolTip(tr("Empty Error Log"));
      statusBar()->addPermanentWidget(m_error_bar_label);
      prepareStatusBarIcons();
    }

#ifdef Q_OS_MACOS
  if(!biblioteq_misc_functions::isEnvironmentSet("QT_STYLE_OVERRIDE"))
    {
      if(m_error_bar_label)
	m_error_bar_label->setStyleSheet
	  ("QToolButton {border: none;}"
	   "QToolButton::menu-button {border: none;}");
    }
#endif

  ui.itemsCountLabel->setText(tr("0 Results"));

  QSettings settings;

  bb.pages->setValue(settings.value("membersPerPage", 500).toInt());

  if(settings.contains("mainwindow_state"))
    restoreState(settings.value("mainwindow_state").toByteArray());

  readGlobalConfiguration();

  /*
  ** Perform additional user interface duties.
  */

  auto group1 = new QActionGroup(this);
  auto list(m_sruMaps.keys());

  for(int i = 0; i < list.size(); i++)
    {
      auto action = group1->addAction(list.at(i));

      if(!action)
	continue;

      action->setCheckable(true);

      if(i == 0)
	action->setChecked(true);

      ui.menuPreferredSRUSite->addAction(action);
    }

  if(ui.menuPreferredSRUSite->actions().isEmpty())
    {
      group1->deleteLater();
      ui.menuPreferredSRUSite->addAction(tr("None"));
    }

  auto group2 = new QActionGroup(this);

  list = m_z3950Maps.keys();

  for(int i = 0; i < list.size(); i++)
    {
      auto action = group2->addAction(list.at(i));

      if(!action)
	continue;

      action->setCheckable(true);

      if(i == 0)
	action->setChecked(true);

      ui.menuPreferredZ3950Server->addAction(action);
    }

  if(ui.menuPreferredZ3950Server->actions().isEmpty())
    {
      group2->deleteLater();
      ui.menuPreferredZ3950Server->addAction(tr("None"));
    }

#ifndef BIBLIOTEQ_LINKED_WITH_YAZ
  ui.menuPreferredZ3950Server->setEnabled(false);
#endif

  /*
  ** Initial update.
  */

  initialUpdate();
  prepareIcons();
  show();
  setGlobalFonts(QApplication::font());
  slotResizeColumns();
  QApplication::processEvents();

#if defined(Q_OS_ANDROID)
  QFileInfo const fileInfo("assets:/biblioteq.conf");
#elif defined(Q_OS_MACOS)
  QFileInfo const fileInfo
    (QCoreApplication::applicationDirPath() + "/../../../biblioteq.conf");
#elif defined(Q_OS_OS2)
  QFileInfo const fileInfo(qgetenv("unixroot") + "/usr/local/biblioteq.conf");
#elif defined(Q_OS_WINDOWS)
  QFileInfo const fileInfo(QCoreApplication::applicationDirPath() +
			   QDir::separator() +
			   "biblioteq.conf");
#else
  QFileInfo const fileInfo(BIBLIOTEQ_CONFIGURATION_FILE);
#endif

  if(!fileInfo.isReadable())
    {
      QMessageBox::warning
	(this,
	 tr("BiblioteQ: Warning"),
	 tr("BiblioteQ was not able to discover the biblioteq.conf "
	    "file. Default values will be assumed. The expected absolute "
	    "path of biblioteq.conf is %1.").arg(fileInfo.absolutePath()));
      QApplication::processEvents();
    }

  if(!(QSqlDatabase::isDriverAvailable("QPSQL") ||
       QSqlDatabase::isDriverAvailable("QSQLITE")))
    {
      QFileInfo const fileInfo("qt.conf");
      QString str("");

      if(fileInfo.isReadable() && fileInfo.size() > 0)
	str = tr("Please verify that the PostgreSQL driver or the "
		 "SQLite driver is installed. "
		 "The file qt.conf is present in BiblioteQ's "
		 "current working directory. Perhaps a plugin conflict "
		 "exists. Please resolve!");
      else
	str = tr("Please verify that the PostgreSQL driver or the "
		 "SQLite driver is installed.");

      QMessageBox::critical(this, tr("BiblioteQ: Error"), str);
      QApplication::processEvents();
    }
  else
    {
      auto const list(QApplication::arguments());
      auto openDatabase = false;

      for(int i = 0; i < list.size(); i++)
	if(list.at(i) == "--help")
	  std::cout << "BiblioteQ:" << std::endl
		    << " [--help]" << std::endl
		    << " [--open-postgresql-database]" << std::endl
		    << " [--open-sqlite-database]" << std::endl
		    << " [--open-sqlite-database-index]" << std::endl
		    << " [--prepare-csv-import-file]" << std::endl
		    << " [--special-executable]" << std::endl
		    << " [--special-executable-icon]" << std::endl;
	else if(list.at(i) == "--open-postgresql-database")
	  {
	    i += 1;

	    if(i >= list.size())
	      continue;

	    if(!openDatabase)
	      {
		auto const index = br.branch_name->findText(list.at(i));

		if(index >= 0)
		  {
		    br.branch_name->setCurrentIndex(index);
		    openDatabase = true;
		    slotConnectDB();
		    break;
		  }
	      }
	  }
	else if(list.at(i) == "--open-sqlite-database")
	  {
	    i += 1;

	    if(i >= list.size())
	      continue;

	    if(!openDatabase)
	      {
		br.filename->setText(QFileInfo(list.at(i)).absoluteFilePath());

		for(int j = 0; j < br.branch_name->count(); j++)
		  if(m_branches.contains(br.branch_name->itemText(j)))
		    if(m_branches[br.branch_name->itemText(j)].
		       value("database_type") == "sqlite")
		      {
			br.branch_name->setCurrentIndex(j);
			openDatabase = true;
			slotConnectDB();
			break;
		      }
	      }
	  }
	else if(list.at(i) == "--open-sqlite-database-index")
	  {
	    i += 1;

	    if(i >= list.size())
	      continue;

	    if(!openDatabase)
	      {
		auto const index = list.at(i).toInt();

		for(int j = 0;
		    j < ui.menu_Recent_SQLite_Files->actions().size() - 2;
		    j++)
		  if(index == j + 1)
		    {
		      openDatabase = true;
		      ui.menu_Recent_SQLite_Files->actions().at(j)->trigger();
		      break;
		    }
	      }
	  }
	else if(list.at(i) == "--prepare-csv-import-file")
	  {
	    i += 1;

	    if(i >= list.size())
	      continue;

	    m_import->setImportFile
	      (QFileInfo(list.at(i).trimmed()).absoluteFilePath());
	  }
	else if(list.at(i) == "--special-executable")
	  {
	    i += 1;

	    if(i >= list.size())
	      continue;

	    m_specialExecutables
	      [QFileInfo(list.at(i).trimmed()).absoluteFilePath()] = 0;
	  }
	else if(list.at(i) == "--special-executable-icon")
	  {
	    i += 1;

	    if(i - 2 < 0 || i >= list.size())
	      continue;

	    QFileInfo const fileInfo(list.at(i - 2).trimmed());

	    if(m_specialExecutables.contains(fileInfo.absoluteFilePath()))
	      m_specialExecutablesIcons[fileInfo.absoluteFilePath()] =
		QFileInfo(list.at(i).trimmed()).absoluteFilePath();
	  }
	else if(list.at(i).endsWith(".sqlite", Qt::CaseInsensitive))
	  {
	    if(!openDatabase)
	      {
		br.filename->setText(QFileInfo(list.at(i)).absoluteFilePath());

		for(int j = 0; j < br.branch_name->count(); j++)
		  if(m_branches.contains(br.branch_name->itemText(j)))
		    if(m_branches[br.branch_name->itemText(j)].
		       value("database_type") == "sqlite")
		      {
			br.branch_name->setCurrentIndex(j);
			openDatabase = true;
			slotConnectDB();
			break;
		      }
	      }
	  }
    }

  QTimer::singleShot(1500, this, &biblioteq::slotDelayedPreparation);
  ui.actionDownload_Version_Information->isChecked() ?
    QTimer::singleShot(1500, m_swifty, &swifty::slot_download) : (void) 0;
}

void biblioteq::slotAbout(void)
{
  if(!m_about)
    {
      connect(&m_aboutTimer,
	      &QTimer::timeout,
	      this,
	      &biblioteq::slotAnimateAbout);
      m_about = new QMessageBox(this);
    }

  if(!m_aboutTimer.isActive())
    {
      m_aboutColors.first = QColor(0, 87, 184);
      m_aboutColors.first = m_aboutColors.first.darker(10000);
      m_aboutColors.second = QColor(254, 221, 0);
      m_aboutColors.second = m_aboutColors.second.darker(10000);
      m_aboutTimer.start(150);
    }

  QString qversion("");
  auto const tmp = qVersion();

  if(tmp)
    qversion = tmp;

  qversion = qversion.trimmed();

  if(qversion.isEmpty())
    qversion = "unknown";

  m_about->setFont(QApplication::font());
  m_about->setIconPixmap
    (QPixmap(":/book.png").scaled(QSize(128, 128),
				  Qt::KeepAspectRatio,
				  Qt::SmoothTransformation));
  m_about->setStandardButtons(QMessageBox::Close);
  m_about->setText
    (tr("<html>BiblioteQ Local Version %1<br>"
	"BiblioteQ Official Version %9<br>"
	"Architecture %4.<br>"
	"Compiled on %2, %3.<br>"
	"Made with love by textbrower.<br>"
	"Software for and from the margins.<br>"
	"Faenza icons.<br>"
#ifdef BIBLIOTEQ_POPPLER_VERSION_DEFINED
	"Poppler version %5.<br>"
#else
	"%5<br>"
#endif
	"Qt version %6 (runtime %7).<br>"
	"YAZ version %8.<br><br>"
	"Please visit <a href=\"https://biblioteq.sourceforge.io\">"
	"https://biblioteq.sourceforge.io</a> or "
	"<a href=\"https://textbrowser.github.io/biblioteq/\">"
	"https://textbrowser.github.io/biblioteq</a> "
	"for project information.</html>").
     arg(BIBLIOTEQ_VERSION).
     arg(__DATE__).
     arg(__TIME__).
     arg(QSysInfo::currentCpuArchitecture()).
#ifdef BIBLIOTEQ_POPPLER_VERSION_DEFINED
     arg(POPPLER_VERSION).
#else
     arg(tr("Poppler version is not available.")).
#endif
     arg(QT_VERSION_STR).
     arg(qversion).
#ifdef BIBLIOTEQ_LINKED_WITH_YAZ
     arg(YAZ_VERSION).
#else
     arg(tr("is not available")).
#endif
     arg(BIBLIOTEQ_VERSION_LTS));
  m_about->button(QMessageBox::Close)->setShortcut(tr("Ctrl+W"));
  m_about->setTextFormat(Qt::RichText);
  m_about->setWindowIcon(windowIcon());
  m_about->setWindowModality(Qt::NonModal);
  m_about->setWindowTitle(tr("BiblioteQ: About"));
  m_about->showNormal();
  m_about->activateWindow();
  m_about->raise();
  QApplication::processEvents();
}

void biblioteq::slotAddAdmin(void)
{
  ab.table->setRowCount(ab.table->rowCount() + 1);

  for(int i = 0; i < ab.table->columnCount(); i++)
    if(i == static_cast<int> (AdminSetupColumns::ID))
      {
	auto item = new QTableWidgetItem();

	item->setFlags(item->flags() | Qt::ItemIsEditable);
	ab.table->setItem(ab.table->rowCount() - 1, i, item);
      }
    else
      {
	auto checkBox = new QCheckBox();

	ab.table->setCellWidget(ab.table->rowCount() - 1, i, checkBox);
	connect(checkBox,
		SIGNAL(stateChanged(int)),
		this,
		SLOT(slotAdminCheckBoxClicked(int)));
      }
}

void biblioteq::slotAddBorrower(void)
{
  auto const now = QDate::currentDate();
  auto const nowTime = QDateTime::currentDateTime();

  biblioteq_misc_functions::highlightWidget
    (userinfo_diag->m_userinfo.memberid,
     m_otherOptions->membersMandatoryFieldColor());
  userinfo_diag->m_memberProperties.clear();
  userinfo_diag->m_userinfo.city->clear();
  userinfo_diag->m_userinfo.comments->clear();
  userinfo_diag->m_userinfo.dob->setDate(now.addYears(-25));
  userinfo_diag->m_userinfo.email->clear();
  userinfo_diag->m_userinfo.expirationdate->setDate
    (QDate::fromString("01/01/3000", s_databaseDateFormat));
  userinfo_diag->m_userinfo.firstName->clear();
  userinfo_diag->m_userinfo.generalregistrationnumber->clear();
  userinfo_diag->m_userinfo.lastName->clear();
  userinfo_diag->m_userinfo.maximum_reserved_books->setValue(0);
  userinfo_diag->m_userinfo.memberclass->clear();
  userinfo_diag->m_userinfo.memberid->setReadOnly(false);
  userinfo_diag->m_userinfo.memberid->setText
    ("m" + nowTime.toString("yyyyMMddhhmmss"));
  userinfo_diag->m_userinfo.membershipfees->setValue(0.00);
  userinfo_diag->m_userinfo.membersince->setDate(now);
  userinfo_diag->m_userinfo.membersince->setMaximumDate(now);
  userinfo_diag->m_userinfo.middle->clear();
  userinfo_diag->m_userinfo.overduefees->setValue(0.00);
  userinfo_diag->m_userinfo.sex->setCurrentIndex(2); // Private
  userinfo_diag->m_userinfo.state->setCurrentIndex(0);
  userinfo_diag->m_userinfo.street->clear();
  userinfo_diag->m_userinfo.telephoneNumber->clear();
  userinfo_diag->m_userinfo.zip->setCursorPosition(0);
  userinfo_diag->m_userinfo.zip->setText("N/A");
  userinfo_diag->m_memberProperties["dob"] =
    userinfo_diag->m_userinfo.dob->date().toString(Qt::ISODate);
  userinfo_diag->m_memberProperties["expiration_date"] =
    userinfo_diag->m_userinfo.expirationdate->date().toString(Qt::ISODate);
  userinfo_diag->m_memberProperties["maximum_reserved_books"] =
    userinfo_diag->m_userinfo.maximum_reserved_books->text();
  userinfo_diag->m_memberProperties["membership_fees"] =
    userinfo_diag->m_userinfo.membershipfees->text();
  userinfo_diag->m_memberProperties["membersince"] =
    userinfo_diag->m_userinfo.membersince->date().toString(Qt::ISODate);
  userinfo_diag->m_memberProperties["overdue_fees"] =
    userinfo_diag->m_userinfo.overduefees->text();
  userinfo_diag->m_memberProperties["sex"] =
    userinfo_diag->m_userinfo.sex->currentText();
  userinfo_diag->m_memberProperties["state_abbr"] =
    userinfo_diag->m_userinfo.state->currentText();
  userinfo_diag->m_memberProperties["telephone_num"] =
    userinfo_diag->m_userinfo.telephoneNumber->text();
  userinfo_diag->m_memberProperties["zip"] = userinfo_diag->m_userinfo.zip->
    text();
  userinfo_diag->setWindowTitle(tr("BiblioteQ: Create New Member"));
  m_engUserinfoTitle = "Create New Member";
  userinfo_diag->m_userinfo.memberid->selectAll();
  userinfo_diag->m_userinfo.memberid->setFocus();
  userinfo_diag->m_userinfo.nextTool->setVisible(false);
  userinfo_diag->m_userinfo.prevTool->setVisible(false);
  userinfo_diag->m_userinfo.scrollArea->horizontalScrollBar()->setValue(0);
  userinfo_diag->m_userinfo.scrollArea->verticalScrollBar()->setValue(0);
  userinfo_diag->m_userinfo.tabWidget->setCurrentIndex(0);
  userinfo_diag->updateGeometry();
#ifdef Q_OS_ANDROID
  userinfo_diag->showMaximized();
#else
  userinfo_diag->resize
    (userinfo_diag->width(),
     qMax(-100 + m_members_diag->height(),
	  userinfo_diag->sizeHint().height()));
  biblioteq_misc_functions::center(userinfo_diag, m_members_diag);
  userinfo_diag->show();
#endif
}

void biblioteq::slotAdminCheckBoxClicked(int state)
{
  Q_UNUSED(state);

  auto box = qobject_cast<QCheckBox *> (sender());
  int column = -1;
  int row = -1;

  for(int i = 0; i < ab.table->rowCount(); i++)
    for(int j = static_cast<int> (AdminSetupColumns::ADMINISTRATOR);
	j < ab.table->columnCount();
	j++)
      if(ab.table->cellWidget(i, j) == box)
	{
	  row = i;
	  column = j;
	  break;
	}

  if(row > -1)
    {
      if(column == static_cast<int> (AdminSetupColumns::ADMINISTRATOR))
	{
	  for(int i = static_cast<int> (AdminSetupColumns::CIRCULATION);
	      i < ab.table->columnCount();
	      i++)
	    if(box->isChecked())
	      (qobject_cast<QCheckBox *> (ab.table->cellWidget(row, i)))->
		setChecked(false);
	}
      else
	if(box->isChecked())
	  (qobject_cast<QCheckBox *> (ab.table->cellWidget(row, 1)))->
	    setChecked(false);
    }
}

void biblioteq::slotAutoPopOnFilter(QAction *action)
{
  if(!action)
    return;

  disconnect(ui.menu_Category,
	     SIGNAL(triggered(QAction *)),
	     this,
	     SLOT(slotAutoPopOnFilter(QAction *)));
  action->setChecked(true);
  ui.menu_Category->setDefaultAction(action);
  connect(ui.menu_Category,
	  SIGNAL(triggered(QAction *)),
	  this,
	  SLOT(slotAutoPopOnFilter(QAction *)));
  ui.categoryLabel->setText(action->text());
  prepareRequestToolButton(action->data().toString());

  QSettings settings;

  m_lastCategory = getTypeFilterString();
  settings.setValue("last_category", m_lastCategory);

  /*
  ** Populate the main table only if we're connected to a database.
  */

  if(m_db.isOpen())
    slotRefresh();
  else
    {
      m_findList.clear();
      ui.graphicsView->horizontalScrollBar()->setValue(0);
      ui.graphicsView->resetTransform();
      ui.graphicsView->scene()->clear();
      ui.graphicsView->verticalScrollBar()->setValue(0);
      ui.itemsCountLabel->setText(tr("0 Results"));
      ui.nextPageButton->setEnabled(false);
      ui.pagesLabel->setText(tr("1"));
      ui.previousPageButton->setEnabled(false);
      ui.table->resetTable(dbUserName(), action->data().toString(), "");
    }
}

void biblioteq::slotBranchChanged(void)
{
  std::unique_lock<std::mutex> lock{m_mutex, std::defer_lock};

  if(!lock.try_lock())
    return;
  else
    lock.unlock();

  auto const tmphash(m_branches[br.branch_name->currentText()]);

  if(tmphash.value("database_type") == "sqlite")
    {
      br.driver_warning->setVisible
	(!QSqlDatabase::isDriverAvailable("QSQLITE"));
      br.fileButton->setFocus();
      br.stackedWidget->setCurrentIndex(0);
    }
  else
    {
      br.driver_warning->setVisible(!QSqlDatabase::isDriverAvailable("QPSQL"));
      br.stackedWidget->setCurrentIndex(1);
      br.userid->selectAll();
    }

  m_branch_diag->update();
#ifdef Q_OS_ANDROID
  m_branch_diag->showMaximized();
#else
  m_branch_diag->resize
    (m_branch_diag->width(), m_branch_diag->minimumSize().height());
  m_branch_diag->show();
#endif
}

void biblioteq::slotCancelAddUser(void)
{
  if(userinfo_diag->isVisible())
#ifdef Q_OS_ANDROID
    userinfo_diag->hide();
#else
    userinfo_diag->close();
#endif
}

void biblioteq::slotChangeView(bool checked)
{
  Q_UNUSED(checked);

  auto action = qobject_cast<QAction *> (sender());

  if(action)
    {
      ui.stackedWidget->setCurrentIndex(action->data().toInt());

      if(ui.stackedWidget->currentIndex() == 0)
	ui.table->setSelectionMode(QAbstractItemView::MultiSelection);
      else
	ui.table->setSelectionMode(QAbstractItemView::ExtendedSelection);

      QSettings settings;

      settings.setValue("view_mode_index", action->data().toInt());
    }
}

void biblioteq::slotClearSqliteMenu(bool state)
{
  Q_UNUSED(state);
  br.filename->clear();
  ui.menu_Recent_SQLite_Files->clear();

  QSettings settings;
  auto const allKeys(settings.allKeys());

  for(int i = 0; i < allKeys.size(); i++)
    if(allKeys[i].startsWith("sqlite_db_"))
      settings.remove(allKeys[i]);

  createSqliteMenuActions();
}

void biblioteq::slotCloseMembersBrowser(void)
{
  /*
  ** Changes? If so, warn the user.
  */

  /*
  ** Also closes the Reservation History Browser.
  */

#ifdef Q_OS_ANDROID
  m_history_diag->hide();
  m_members_diag->hide();
#else
  m_history_diag->close();
  m_members_diag->close();
#endif
}

void biblioteq::slotClosePasswordDialog(void)
{
  m_pass_diag->setVisible(false);
  pass.password->setText(QString(1024, '0'));
  pass.password->clear();
  pass.userid->clear();
}

void biblioteq::slotCopyError(void)
{
  auto const list(er.table->selectionModel()->selectedRows());

  if(list.isEmpty())
    {
      QMessageBox::critical(m_error_diag,
			    tr("BiblioteQ: User Error"),
			    tr("To copy the contents of the Error Log "
			       "into the clipboard buffer, you must first "
			       "select at least one entry."));
      QApplication::processEvents();
      return;
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  QString text = "";
  auto clipboard = QApplication::clipboard();

  foreach(auto const &index, list)
    {
      auto const i = index.row();

      for(int j = 0; j < er.table->columnCount(); j++)
	{
	  text += er.table->item(i, j)->text();

	  if(er.table->columnCount() - 1 != j)
	    text += ",";
	}

      text = text.trimmed();
      text += "\n";
    }

  if(!text.isEmpty())
    clipboard->setText(text);

  QApplication::restoreOverrideCursor();
}

void biblioteq::slotDelete(void)
{
  if(!m_db.isOpen())
    return;

  std::unique_lock<std::mutex> lock{m_mutex, std::defer_lock};

  if(!lock.try_lock())
    return;

  auto const list(ui.table->selectionModel()->selectedRows());

  if(list.isEmpty())
    {
      QMessageBox::critical(this,
			    tr("BiblioteQ: User Error"),
			    tr("Please select an item to delete."));
      QApplication::processEvents();
      return;
    }

  QSqlQuery query(m_db);
  QString errorstr = "";
  auto const col = ui.table->columnNumber("MYOID");
  auto error = false;
  auto isCheckedOut = false;
  auto isRequested = false;
  int numdeleted = 0;

  foreach(auto const &index, list)
    {
      auto const i = index.row();

      if(ui.table->item(i, col) == nullptr)
	continue;

      auto const itemType = biblioteq_misc_functions::getColumnString
	(ui.table, i, ui.table->columnNumber("Type"));
      auto const oid = biblioteq_misc_functions::getColumnString
	(ui.table, i, ui.table->columnNumber("MYOID"));

      if(itemType.isEmpty() || oid.isEmpty())
	{
	  addError(tr("Error"),
		   tr("The main table does not contain enough "
		      "information for item deletion."),
		   tr("The main table does not contain enough "
		      "information for item deletion."),
		   __FILE__,
		   __LINE__);
	  QMessageBox::critical(this,
				tr("BiblioteQ: Error"),
				tr("The main table does not contain enough "
				   "information for item deletion."));
	  QApplication::processEvents();
	  return;
	}

      QApplication::setOverrideCursor(Qt::WaitCursor);
      isCheckedOut = biblioteq_misc_functions::isCheckedOut
	(m_db, oid, itemType, errorstr);
      QApplication::restoreOverrideCursor();

      if(!errorstr.isEmpty())
	{
	  addError(tr("Database Error"),
		   tr("Unable to determine if the item has been reserved."),
		   errorstr,
		   __FILE__,
		   __LINE__);
	  QMessageBox::critical(this,
				tr("BiblioteQ: Database Error"),
				tr("Unable to determine if the item has "
				   "been reserved."));
	  QApplication::processEvents();
	  return;
	}

      if(isCheckedOut)
	{
	  QMessageBox::critical(this,
				tr("BiblioteQ: User Error"),
				tr("Reserved items may not be deleted."));
	  QApplication::processEvents();
	  return;
	}

      QApplication::setOverrideCursor(Qt::WaitCursor);
      isRequested = biblioteq_misc_functions::isRequested
	(m_db, oid, itemType, errorstr);
      QApplication::restoreOverrideCursor();

      if(!errorstr.isEmpty())
	{
	  addError(tr("Database Error"),
		   tr("Unable to determine if the item has been requested."),
		   errorstr,
		   __FILE__,
		   __LINE__);
	  QMessageBox::critical(this,
				tr("BiblioteQ: Database Error"),
				tr("Unable to determine if the item has "
				   "been requested."));
	  QApplication::processEvents();
	  return;
	}

      if(isRequested)
	{
	  QMessageBox::critical(this,
				tr("BiblioteQ: User Error"),
				tr("Requested items may not be deleted."));
	  QApplication::processEvents();
	  return;
	}
    }

  if(!list.isEmpty())
    {
      if(QMessageBox::question(this,
			       tr("BiblioteQ: Question"),
			       tr("Are you sure that you wish to permanently "
				  "delete the selected item(s)?"),
			       QMessageBox::No | QMessageBox::Yes,
			       QMessageBox::No) == QMessageBox::No)
	{
	  QApplication::processEvents();
	  return;
	}
      else
	QApplication::processEvents();
    }

  QProgressDialog progress(this);

  progress.setCancelButton(nullptr);
  progress.setLabelText(tr("Deleting the selected item(s)..."));
  progress.setMaximum(list.size());
  progress.setMinimum(0);
  progress.setMinimumWidth
    (qCeil(PROGRESS_DIALOG_WIDTH_MULTIPLIER * progress.sizeHint().width()));
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.show();
  progress.repaint();
  QApplication::processEvents();

  foreach(auto const &index, list)
    {
      auto const i = index.row();

      if(i + 1 <= progress.maximum())
	progress.setValue(i + 1);

      progress.repaint();
      QApplication::processEvents();

      if(ui.table->item(i, col) == nullptr)
	continue;

      auto const str = ui.table->item(i, col)->text();
      auto itemType = biblioteq_misc_functions::getColumnString
	(ui.table, i, ui.table->columnNumber("Type")).toLower();

      if(itemType == "grey literature" || itemType == "photograph collection")
	itemType = itemType.replace(" ", "_");
      else
	itemType = itemType.remove(" ");

      if(itemType == "book" ||
	 itemType == "cd" ||
	 itemType == "dvd" ||
	 itemType == "grey_literature" ||
	 itemType == "journal" ||
	 itemType == "magazine" ||
	 itemType == "photograph_collection" ||
	 itemType == "videogame")
	query.prepare(QString("DELETE FROM %1 WHERE myoid = ?").arg(itemType));

      query.bindValue(0, str);

      if(!query.exec())
	{
	  error = true;
	  addError(tr("Database Error"),
		   tr("Unable to delete the item."),
		   query.lastError().text(),
		   __FILE__,
		   __LINE__);
	}
      else
	{
	  deleteItem(str, itemType);
	  numdeleted += 1;

	  /*
	  ** SQL errors are ignored.
	  */

	  if(itemType == "book" ||
	     itemType == "grey_literature" ||
	     itemType == "journal" ||
	     itemType == "magazine")
	    {
	      if(itemType != "grey_literature")
		{
		  query.prepare
		    (QString("DELETE FROM %1_copy_info WHERE item_oid = ?").
		     arg(itemType));
		  query.bindValue(0, str);
		  query.exec();
		}

	      query.prepare
		(QString("DELETE FROM %1_files WHERE item_oid = ?").
		 arg(itemType));
	      query.bindValue(0, str);
	      query.exec();
	    }
	  else if(itemType == "cd")
	    {
	      query.prepare("DELETE FROM cd_copy_info WHERE item_oid = ?");
	      query.bindValue(0, str);
	      query.exec();
	      query.prepare("DELETE FROM cd_songs WHERE item_oid = ?");
	      query.bindValue(0, str);
	      query.exec();
	    }
	  else if(itemType == "dvd")
	    {
	      query.prepare("DELETE FROM dvd_copy_info WHERE item_oid = ?");
	      query.bindValue(0, str);
	      query.exec();
	    }
	  else if(itemType == "photograph_collection")
	    {
	      query.prepare("DELETE FROM photograph WHERE collection_oid = ?");
	      query.bindValue(0, str);
	      query.exec();
	    }
	  else if(itemType == "videogame")
	    {
	      query.prepare
		("DELETE FROM videogame_copy_info WHERE item_oid = ?");
	      query.bindValue(0, str);
	      query.exec();
	    }
	}
    }

  progress.close();

  /*
  ** Provide some fancy messages.
  */

  if(error)
    {
      QMessageBox::critical
	(this,
	 tr("BiblioteQ: Database Error"),
	 tr("Unable to delete all or some of the selected items."));
      QApplication::processEvents();
    }

  if(numdeleted > 0)
    slotRefresh();
}

void biblioteq::slotDeleteAdmin(void)
{
  auto const row = ab.table->currentRow();

  if(row < 0)
    {
      QMessageBox::critical
	(m_admin_diag,
	 tr("BiblioteQ: User Error"),
	 tr("To delete an entry, you must first select it."));
      QApplication::processEvents();
      return;
    }

  auto const str = ab.table->item
    (row, static_cast<int> (AdminSetupColumns::ID))->text().toLower().trimmed();

  if((ab.table->item(row, static_cast<int> (AdminSetupColumns::ID))->flags() &
      Qt::ItemIsEditable) == 0 &&
     str == getAdminID())
    {
      QMessageBox::critical
	(m_admin_diag,
	 tr("BiblioteQ: User Error"),
	 tr("As an administrator, you may not delete your account."));
      QApplication::processEvents();
      return;
    }
  else
    {
      if(!m_deletedAdmins.contains(str) && !str.isEmpty())
	m_deletedAdmins.append(str);

      ab.table->removeRow(row);
#ifdef Q_OS_MACOS
      ab.table->hide();
      ab.table->show();
#endif
    }
}

void biblioteq::slotDisplayNewSqliteDialog(void)
{
  QFileDialog dialog(this);
  QString errorstr("");
  auto error = false;

  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setDefaultSuffix("sqlite");
  dialog.setDirectory(QDir::homePath());
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setNameFilter("SQLite Database (*.sqlite)");
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowTitle(tr("BiblioteQ: New SQLite Database"));
#ifdef Q_OS_ANDROID
  dialog.showMaximized();
#endif
  dialog.exec();
  dialog.close();
  QApplication::processEvents();

  if(dialog.result() == QDialog::Accepted)
    {
      repaint();
      QApplication::processEvents();
      QApplication::setOverrideCursor(Qt::WaitCursor);

      QString connectionName("create_sqlite_database");

      {
	auto db = QSqlDatabase::addDatabase("QSQLITE", connectionName);

	QFile::remove(dialog.selectedFiles().value(0));
	db.setDatabaseName(dialog.selectedFiles().value(0));

	if(db.open())
	  {
	    QSqlQuery query(db);
	    auto const list
	      (QString(sqlite_create_schema_text).split("CREATE",
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
							Qt::SkipEmptyParts
#else
							QString::SkipEmptyParts
#endif
							));

	    foreach(auto const &string, list)
	      if(!query.exec("CREATE " + string))
		{
		  error = true;
		  errorstr = query.lastError().text();
		  break;
		}
	  }
	else
	  {
	    error = true;
	    errorstr = tr("db.open() (%1) error").arg
	      (dialog.selectedFiles().value(0));
	  }

	db.close();
      }

      QSqlDatabase::removeDatabase(connectionName);
      QApplication::restoreOverrideCursor();

      if(!error)
	{
	  /*
	  ** The user may not wish to open the new database, so let's not
	  ** connect automatically.
	  */

	  if(m_db.isOpen())
	    {
	      /*
	      ** Attempt to locate an SQLite branch.
	      */

	      auto found = false;

	      for(int i = 0; i < br.branch_name->count(); i++)
		{
		  if(m_branches.contains(br.branch_name->itemText(i)))
		    if(m_branches[br.branch_name->itemText(i)].
		       value("database_type") == "sqlite")
		      {
			br.branch_name->setCurrentIndex(i);
			found = true;
			break;
		      }
		}

	      if(found)
		{
		  if(QMessageBox::
		     question(this,
			      tr("BiblioteQ: Question"),
			      tr("It appears that you are already "
				 "connected to a database. Do you "
				 "wish to terminate the current connection "
				 "and connect to the new SQLite database?"),
			      QMessageBox::No | QMessageBox::Yes,
			      QMessageBox::No) == QMessageBox::Yes)
		    {
		      QApplication::processEvents();
		      br.filename->setText(dialog.selectedFiles().value(0));
		      slotConnectDB();
		    }
		  else
		    QApplication::processEvents();
		}
	    }
	  else
	    {
	      /*
	      ** Attempt to locate an SQLite branch.
	      */

	      auto found = false;

	      for(int i = 0; i < br.branch_name->count(); i++)
		{
		  if(m_branches.contains(br.branch_name->itemText(i)))
		    if(m_branches[br.branch_name->itemText(i)].
		       value("database_type") == "sqlite")
		      {
			br.branch_name->setCurrentIndex(i);
			found = true;
			break;
		      }
		}

	      if(found)
		{
		  br.filename->setText(dialog.selectedFiles().value(0));
		  slotConnectDB();
		}
	    }
	}
      else
	{
	  QMessageBox::critical
	    (this,
	     tr("BiblioteQ: Database Error"),
	     tr("An error (%1) occurred while attempting "
		"to create the specified SQLite database.").arg(errorstr));
	  QApplication::processEvents();
	}
    }
}

void biblioteq::slotDuplicate(void)
{
  if(!m_db.isOpen())
    return;

  std::unique_lock<std::mutex> lock{m_mutex, std::defer_lock};

  if(!lock.try_lock())
    return;

  auto list(ui.table->selectionModel()->selectedRows());

  if(list.isEmpty())
    {
      QMessageBox::critical
	(this,
	 tr("BiblioteQ: User Error"),
	 tr("Please select at least one item to duplicate."));
      QApplication::processEvents();
      return;
    }
  else if(list.size() >= MAXIMUM_DEVICES_CONFIRMATION)
    {
      if(QMessageBox::
	 question(this,
		  tr("BiblioteQ: Question"),
		  tr("Are you sure that you wish to duplicate the "
		     "%1 selected item(s)? BiblioteQ will terminate if "
		     "it's unable to acquire resources.").arg(list.size()),
		  QMessageBox::No | QMessageBox::Yes,
		  QMessageBox::No) == QMessageBox::No)
	{
	  QApplication::processEvents();
	  return;
	}

      QApplication::processEvents();
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  auto error = false;
  biblioteq_book *book = nullptr;
  biblioteq_cd *cd = nullptr;
  biblioteq_dvd *dvd = nullptr;
  biblioteq_grey_literature *gl = nullptr;
  biblioteq_journal *journal = nullptr;
  biblioteq_magazine *magazine = nullptr;
  biblioteq_main_table *table = ui.table;
  biblioteq_photographcollection *photograph = nullptr;
  biblioteq_videogame *videogame = nullptr;
  int c = 1;

  std::stable_sort
    (list.begin(), list.end(), biblioteq_misc_functions::sortByRow);

  foreach(auto const &index, list)
    {
      m_idCt += 1;

      auto const i = index.row();
      auto const id = QString("duplicate_%1").arg(m_idCt);
      auto const oid = biblioteq_misc_functions::getColumnString
	(table, i, table->columnNumber("MYOID"));
      auto const type = biblioteq_misc_functions::getColumnString
	(table, i, table->columnNumber("Type")).toLower();

      showStatusBarMessage
	(tr("Duplicating item %1 of %2.").arg(c++).arg(list.size()), 5000);

      if(type == "book")
	{
	  book = new biblioteq_book(this, oid, index);
	  addItemWindowToTab(book);
	  book->duplicate(id, EDITABLE);
	  connect(this,
		  SIGNAL(databaseEnumerationsCommitted(void)),
		  book,
		  SLOT(slotDatabaseEnumerationsCommitted(void)));
	}
      else if(type == "cd")
	{
	  cd = new biblioteq_cd(this, oid, index);
	  addItemWindowToTab(cd);
	  cd->duplicate(id, EDITABLE);
	  connect(this,
		  SIGNAL(databaseEnumerationsCommitted(void)),
		  cd,
		  SLOT(slotDatabaseEnumerationsCommitted(void)));
	}
      else if(type == "dvd")
	{
	  dvd = new biblioteq_dvd(this, oid, index);
	  addItemWindowToTab(dvd);
	  dvd->duplicate(id, EDITABLE);
	  connect(this,
		  SIGNAL(databaseEnumerationsCommitted(void)),
		  dvd,
		  SLOT(slotDatabaseEnumerationsCommitted(void)));
	}
      else if(type == "grey literature")
	{
	  gl = new biblioteq_grey_literature(this, oid, index);
	  addItemWindowToTab(gl);
	  gl->duplicate(id, EDITABLE);
	  connect(this,
		  SIGNAL(databaseEnumerationsCommitted(void)),
		  gl,
		  SLOT(slotDatabaseEnumerationsCommitted(void)));
	}
      else if(type == "journal")
	{
	  journal = new biblioteq_journal(this, oid, index);
	  addItemWindowToTab(journal);
	  journal->duplicate(id, EDITABLE);
	  connect(this,
		  SIGNAL(databaseEnumerationsCommitted(void)),
		  journal,
		  SLOT(slotDatabaseEnumerationsCommitted(void)));
	}
      else if(type == "magazine")
	{
	  magazine = new biblioteq_magazine(this, oid, index, "magazine");
	  addItemWindowToTab(magazine);
	  magazine->duplicate(id, EDITABLE);
	  connect(this,
		  SIGNAL(databaseEnumerationsCommitted(void)),
		  magazine,
		  SLOT(slotDatabaseEnumerationsCommitted(void)));
	}
      else if(type == "photograph collection")
	{
	  photograph = new biblioteq_photographcollection(this, oid, index);
	  addItemWindowToTab(photograph);
	  photograph->duplicate(id, EDITABLE);
	  connect(this,
		  SIGNAL(databaseEnumerationsCommitted(void)),
		  photograph,
		  SLOT(slotDatabaseEnumerationsCommitted(void)));
	}
      else if(type == "video game")
	{
	  videogame = new biblioteq_videogame(this, oid, index);
	  addItemWindowToTab(videogame);
	  videogame->duplicate(id, EDITABLE);
	  connect(this,
		  SIGNAL(databaseEnumerationsCommitted(void)),
		  videogame,
		  SLOT(slotDatabaseEnumerationsCommitted(void)));
	}
      else
	{
	  error = true;
	  break;
	}
    }

  QApplication::restoreOverrideCursor();

  if(error)
    {
      QMessageBox::critical
	(this,
	 tr("BiblioteQ: Error"),
	 tr("Unable to determine the selected item's type."));
      QApplication::processEvents();
    }
}

void biblioteq::slotExit(void)
{
  std::unique_lock<std::mutex> lock{m_mutex, std::defer_lock};

  if(!lock.try_lock())
    return;
  else
    lock.unlock();

  if(sender()) // A person requested an exit.
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);

      if(!emptyContainers())
	{
	  QApplication::restoreOverrideCursor();
	  return;
	}
      else
	QApplication::restoreOverrideCursor();
    }

  QSettings().setValue("mainwindow_state", saveState());
  m_admin_diag->deleteLater();
  m_all_diag->deleteLater();
  m_error_diag->deleteLater();
  m_history_diag->deleteLater();
  m_members_diag->deleteLater();
  slotLastWindowClosed();
  quit();
}

void biblioteq::slotExportAsCSV(void)
{
  exportAsCSV(ui.table, this, tr("BiblioteQ: Export Table View As CSV"));
}

void biblioteq::slotGrantPrivileges(void)
{
  QProgressDialog progress(m_members_diag);
  QString errorstr("");
  QTableWidgetItem *item = nullptr;
  auto error = false;

  progress.setCancelButton(nullptr);
  progress.setLabelText(tr("Granting privileges..."));
  progress.setMaximum(bb.table->rowCount());
  progress.setMinimum(0);
  progress.setMinimumWidth
    (qCeil(PROGRESS_DIALOG_WIDTH_MULTIPLIER * progress.sizeHint().width()));
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.show();
  progress.repaint();
  QApplication::processEvents();

  for(int i = 0; i < bb.table->rowCount(); i++)
    {
      if((item = bb.table->item(i, 0)))
	{
	  biblioteq_misc_functions::DBAccount
	    (item->text(),
	     m_db,
	     biblioteq_misc_functions::UPDATE_USER,
	     errorstr);

	  if(!errorstr.isEmpty())
	    {
	      addError
		(tr("Database Error"),
		 tr("An error occurred while attempting to "
		    "update the database account for %1.").arg(item->text()),
		 errorstr,
		 __FILE__,
		 __LINE__);
	      error = true;
	    }
	}

      if(i + 1 <= progress.maximum())
	progress.setValue(i + 1);

      progress.repaint();
      QApplication::processEvents();
    }

  progress.close();

  if(error)
    {
      QMessageBox::critical(m_members_diag,
			    tr("BiblioteQ: Database Error"),
			    tr("Unable to grant privileges to all of "
			       "the members."));
      QApplication::processEvents();
    }
}

void biblioteq::slotInsertBook(void)
{
  std::unique_lock<std::mutex> lock{m_mutex, std::defer_lock};

  if(!lock.try_lock())
    return;

  auto ok = false;
  auto const integer = QInputDialog::getInt
    (this, tr("BiblioteQ"), tr("Create Books"), 1, 1, 10, 1, &ok);

  if(!ok)
    return;

  for(int i = 0; i < integer; i++)
    {
      m_idCt += 1;

      auto book = new biblioteq_book
	(this, QString("insert_%1").arg(m_idCt), QModelIndex());

      addItemWindowToTab(book);
      book->insert();
      connect(this,
	      SIGNAL(databaseEnumerationsCommitted(void)),
	      book,
	      SLOT(slotDatabaseEnumerationsCommitted(void)));
    }
}

void biblioteq::slotInsertCD(void)
{
  QString id("");
  biblioteq_cd *cd = nullptr;

  m_idCt += 1;
  id = QString("insert_%1").arg(m_idCt);
  cd = new biblioteq_cd(this, id, QModelIndex());
  addItemWindowToTab(cd);
  cd->insert();
  connect(this,
	  SIGNAL(databaseEnumerationsCommitted(void)),
	  cd,
	  SLOT(slotDatabaseEnumerationsCommitted(void)));
}

void biblioteq::slotInsertDVD(void)
{
  QString id("");
  biblioteq_dvd *dvd = nullptr;

  m_idCt += 1;
  id = QString("insert_%1").arg(m_idCt);
  dvd = new biblioteq_dvd(this, id, QModelIndex());
  addItemWindowToTab(dvd);
  dvd->insert();
  connect(this,
	  SIGNAL(databaseEnumerationsCommitted(void)),
	  dvd,
	  SLOT(slotDatabaseEnumerationsCommitted(void)));
}

void biblioteq::slotInsertJourn(void)
{
  QString id("");
  biblioteq_journal *journal = nullptr;

  m_idCt += 1;
  id = QString("insert_%1").arg(m_idCt);
  journal = new biblioteq_journal(this, id, QModelIndex());
  addItemWindowToTab(journal);
  journal->insert();
  connect(this,
	  SIGNAL(databaseEnumerationsCommitted(void)),
	  journal,
	  SLOT(slotDatabaseEnumerationsCommitted(void)));
}

void biblioteq::slotInsertMag(void)
{
  QString id("");
  biblioteq_magazine *magazine = nullptr;

  m_idCt += 1;
  id = QString("insert_%1").arg(m_idCt);
  magazine = new biblioteq_magazine(this, id, QModelIndex(), "magazine");
  addItemWindowToTab(magazine);
  magazine->insert();
  connect(this,
	  SIGNAL(databaseEnumerationsCommitted(void)),
	  magazine,
	  SLOT(slotDatabaseEnumerationsCommitted(void)));
}

void biblioteq::slotInsertPhotograph(void)
{
  QString id("");
  biblioteq_photographcollection *photograph = nullptr;

  m_idCt += 1;
  id = QString("insert_%1").arg(m_idCt);
  photograph = new biblioteq_photographcollection(this, id, QModelIndex());
  addItemWindowToTab(photograph);
  photograph->insert();
  connect(this,
	  SIGNAL(databaseEnumerationsCommitted(void)),
	  photograph,
	  SLOT(slotDatabaseEnumerationsCommitted(void)));
}

void biblioteq::slotInsertVideoGame(void)
{
  QString id("");
  biblioteq_videogame *videogame = nullptr;

  m_idCt += 1;
  id = QString("insert_%1").arg(m_idCt);
  videogame = new biblioteq_videogame(this, id, QModelIndex());
  addItemWindowToTab(videogame);
  videogame->insert();
  connect(this,
	  SIGNAL(databaseEnumerationsCommitted(void)),
	  videogame,
	  SLOT(slotDatabaseEnumerationsCommitted(void)));
}

void biblioteq::slotLanguageChanged(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(action && action->isChecked())
    {
      QApplication::removeTranslator(s_appTranslator);
      QApplication::removeTranslator(s_qtTranslator);
      s_locale = action->data().toString();

      if(s_appTranslator->load(":/biblioteq_" + s_locale + ".qm"))
	QApplication::installTranslator(s_appTranslator);

      if(s_qtTranslator->load(":/qtbase_" + s_locale + ".qm"))
	QApplication::installTranslator(s_qtTranslator);
    }
}

void biblioteq::slotListOverdueItems(void)
{
  QString memberid("");

  if(bb.overdueButton == sender() && m_members_diag->isVisible())
    memberid = biblioteq_misc_functions::getColumnString
      (bb.table,
       bb.table->currentRow(),
       m_bbColumnHeaderIndexes.indexOf("Member ID"));
  else if(m_roles.isEmpty())
    memberid = dbUserName();

  (void) populateTable(POPULATE_ALL, "All Overdue", memberid);

  if(bb.overdueButton == sender())
    {
#ifdef Q_OS_ANDROID
      m_members_diag->showMaximized();
#else
      m_members_diag->showNormal();
#endif
      m_members_diag->activateWindow();
      m_members_diag->raise();
    }
}

void biblioteq::slotListReservedItems(const QString &id)
{
  (void) populateTable(POPULATE_ALL, "All Reserved", id);
}

void biblioteq::slotListReservedItems(void)
{
  auto const row = bb.table->currentRow();

  if(row < 0)
    {
      QMessageBox::critical(m_members_diag,
			    tr("BiblioteQ: User Error"),
			    tr("In order to list a member's reserved items, "
			       "you must first select the member."));
      QApplication::processEvents();
      return;
    }

  auto const memberid
    (biblioteq_misc_functions::
     getColumnString(bb.table,
		     row,
		     m_bbColumnHeaderIndexes.indexOf("Member ID")));

  (void) populateTable(POPULATE_ALL, "All Reserved", memberid);
#ifdef Q_OS_ANDROID
  m_members_diag->showMaximized();
#else
  m_members_diag->showNormal();
#endif
  m_members_diag->activateWindow();
  m_members_diag->raise();
}

void biblioteq::slotModify(void)
{
  if(!m_db.isOpen())
    return;

  std::unique_lock<std::mutex> lock{m_mutex, std::defer_lock};

  if(!lock.try_lock())
    return;

  auto list(ui.table->selectionModel()->selectedRows());

  if(list.isEmpty())
    {
      QMessageBox::critical
	(this,
	 tr("BiblioteQ: User Error"),
	 tr("Please select at least one item to modify."));
      QApplication::processEvents();
      return;
    }
  else if(list.size() >= MAXIMUM_DEVICES_CONFIRMATION)
    {
      if(QMessageBox::
	 question(this,
		  tr("BiblioteQ: Question"),
		  tr("Are you sure that you wish to modify the %1 "
		     "selected item(s)? BiblioteQ will terminate "
		     "if it's unable to acquire resources.").arg(list.size()),
		  QMessageBox::No | QMessageBox::Yes,
		  QMessageBox::No) == QMessageBox::No)
	{
	  QApplication::processEvents();
	  return;
	}

      QApplication::processEvents();
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  auto error = false;
  auto table = ui.table;
  biblioteq_book *book = nullptr;
  biblioteq_cd *cd = nullptr;
  biblioteq_dvd *dvd = nullptr;
  biblioteq_grey_literature *gl = nullptr;
  biblioteq_journal *journal = nullptr;
  biblioteq_magazine *magazine = nullptr;
  biblioteq_photographcollection *photograph = nullptr;
  biblioteq_videogame *videogame = nullptr;
  int c = 1;

  std::stable_sort
    (list.begin(), list.end(), biblioteq_misc_functions::sortByRow);

  foreach(auto const &index, list)
    {
      auto const i = index.row();
      auto const oid = biblioteq_misc_functions::getColumnString
	(table, i, table->columnNumber("MYOID"));
      auto const type = biblioteq_misc_functions::getColumnString
	(table, i, table->columnNumber("Type")).toLower();

      book = nullptr;
      cd = nullptr;
      dvd = nullptr;
      gl = nullptr;
      journal = nullptr;
      magazine = nullptr;
      photograph = nullptr;
      showStatusBarMessage
	(tr("Retrieving item %1 of %2.").arg(c++).arg(list.size()), 5000);
      videogame = nullptr;

      if(type == "book")
	{
	  auto created = true;

	  foreach(auto w, QApplication::topLevelWidgets())
	    {
	      auto b = qobject_cast<biblioteq_book *> (w);

	      if(b && b->getID() == oid)
		{
		  book = b;
		  created = false;
		  break;
		}
	    }

	  if(!book)
	    book = dynamic_cast<biblioteq_book *> (findItemInTab(oid));

	  if(!book)
	    book = new biblioteq_book(this, oid, index);
	  else
	    created = false;

	  addItemWindowToTab(book);

	  if(created)
	    {
	      book->modify(EDITABLE);
	      connect(this,
		      SIGNAL(databaseEnumerationsCommitted(void)),
		      book,
		      SLOT(slotDatabaseEnumerationsCommitted(void)));
	    }
	}
      else if(type == "cd")
	{
	  auto created = true;

	  foreach(auto w, QApplication::topLevelWidgets())
	    {
	      auto c = qobject_cast<biblioteq_cd *> (w);

	      if(c && c->getID() == oid)
		{
		  cd = c;
		  created = false;
		  break;
		}
	    }

	  if(!cd)
	    cd = dynamic_cast<biblioteq_cd *> (findItemInTab(oid));

	  if(!cd)
	    cd = new biblioteq_cd(this, oid, index);
	  else
	    created = false;

	  addItemWindowToTab(cd);

	  if(created)
	    {
	      cd->modify(EDITABLE);
	      connect(this,
		      SIGNAL(databaseEnumerationsCommitted(void)),
		      cd,
		      SLOT(slotDatabaseEnumerationsCommitted(void)));
	    }
	}
      else if(type == "dvd")
	{
	  auto created = true;

	  foreach(auto w, QApplication::topLevelWidgets())
	    {
	      auto d = qobject_cast<biblioteq_dvd *> (w);

	      if(d && d->getID() == oid)
		{
		  created = false;
		  dvd = d;
		  break;
		}
	    }

	  if(!dvd)
	    dvd = dynamic_cast<biblioteq_dvd *> (findItemInTab(oid));

	  if(!dvd)
	    dvd = new biblioteq_dvd(this, oid, index);
	  else
	    created = false;

	  addItemWindowToTab(dvd);

	  if(created)
	    {
	      dvd->modify(EDITABLE);
	      connect(this,
		      SIGNAL(databaseEnumerationsCommitted(void)),
		      dvd,
		      SLOT(slotDatabaseEnumerationsCommitted(void)));
	    }
	}
      else if(type == "grey literature")
	{
	  auto created = true;

	  foreach(auto w, QApplication::topLevelWidgets())
	    {
	      auto g = qobject_cast<biblioteq_grey_literature *> (w);

	      if(g && g->getID() == oid)
		{
		  created = false;
		  gl = g;
		  break;
		}
	    }

	  if(!gl)
	    gl = dynamic_cast<biblioteq_grey_literature *>
	      (findItemInTab(oid));

	  if(!gl)
	    gl = new biblioteq_grey_literature(this, oid, index);
	  else
	    created = false;

	  addItemWindowToTab(gl);

	  if(created)
	    {
	      gl->modify(EDITABLE);
	      connect(this,
		      SIGNAL(databaseEnumerationsCommitted(void)),
		      gl,
		      SLOT(slotDatabaseEnumerationsCommitted(void)));
	    }
	}
      else if(type == "journal")
	{
	  auto created = true;

	  foreach(auto w, QApplication::topLevelWidgets())
	    {
	      auto j = qobject_cast<biblioteq_journal *> (w);

	      if(j && j->getID() == oid)
		{
		  created = false;
		  journal = j;
		  break;
		}
	    }

	  if(!journal)
	    journal = dynamic_cast<biblioteq_journal *> (findItemInTab(oid));

	  if(!journal)
	    journal = new biblioteq_journal(this, oid, index);
	  else
	    created = false;

	  addItemWindowToTab(journal);

	  if(created)
	    {
	      journal->modify(EDITABLE);
	      connect(this,
		      SIGNAL(databaseEnumerationsCommitted(void)),
		      journal,
		      SLOT(slotDatabaseEnumerationsCommitted(void)));
	    }
	}
      else if(type == "magazine")
	{
	  auto created = true;

	  foreach(auto w, QApplication::topLevelWidgets())
	    {
	      auto m = qobject_cast<biblioteq_magazine *> (w);

	      /*
	      ** The class biblioteq_journal inherits biblioteq_magazine.
	      */

	      if(!qobject_cast<biblioteq_journal *> (w))
		if(m && m->getID() == oid)
		  {
		    created = false;
		    magazine = m;
		    break;
		  }
	    }

	  if(!magazine)
	    magazine = dynamic_cast<biblioteq_magazine *> (findItemInTab(oid));

	  if(!magazine)
	    magazine = new biblioteq_magazine(this, oid, index, "magazine");
	  else
	    created = false;

	  addItemWindowToTab(magazine);

	  if(created)
	    {
	      magazine->modify(EDITABLE);
	      connect(this,
		      SIGNAL(databaseEnumerationsCommitted(void)),
		      magazine,
		      SLOT(slotDatabaseEnumerationsCommitted(void)));
	    }
	}
      else if(type == "photograph collection")
	{
	  auto created = true;

	  foreach(auto w, QApplication::topLevelWidgets())
	    {
	      auto p = qobject_cast<biblioteq_photographcollection *> (w);

	      if(p && p->getID() == oid)
		{
		  created = false;
		  photograph = p;
		  break;
		}
	    }

	  if(!photograph)
	    photograph = dynamic_cast<biblioteq_photographcollection *>
	      (findItemInTab(oid));

	  if(!photograph)
	    photograph = new biblioteq_photographcollection(this, oid, index);
	  else
	    created = false;

	  addItemWindowToTab(photograph);

	  if(created)
	    {
	      photograph->modify(EDITABLE);
	      connect(this,
		      SIGNAL(databaseEnumerationsCommitted(void)),
		      photograph,
		      SLOT(slotDatabaseEnumerationsCommitted(void)));
	    }
	}
      else if(type == "video game")
	{
	  auto created = true;

	  foreach(auto w, QApplication::topLevelWidgets())
	    {
	      auto v = qobject_cast<biblioteq_videogame *> (w);

	      if(v && v->getID() == oid)
		{
		  created = false;
		  videogame = v;
		  break;
		}
	    }

	  if(!videogame)
	    videogame = dynamic_cast<biblioteq_videogame *>
	      (findItemInTab(oid));

	  if(!videogame)
	    videogame = new biblioteq_videogame(this, oid, index);
	  else
	    created = false;

	  addItemWindowToTab(videogame);

	  if(created)
	    {
	      videogame->modify(EDITABLE);
	      connect(this,
		      SIGNAL(databaseEnumerationsCommitted(void)),
		      videogame,
		      SLOT(slotDatabaseEnumerationsCommitted(void)));
	    }
	}
      else
	{
	  error = true;
	  break;
	}
    }

  QApplication::restoreOverrideCursor();

  if(error)
    {
      QMessageBox::critical
	(this,
	 tr("BiblioteQ: Error"),
	 tr("Unable to determine the selected item's type."));
      QApplication::processEvents();
    }
}

void biblioteq::slotNextPage(void)
{
  if(m_db.isOpen())
    {
      if(m_lastSearchStr == "Item Search Query")
	(void) populateTable
	  (m_searchQuery, m_previousTypeFilter, NEXT_PAGE, m_lastSearchType);
      else
	(void) populateTable
	  (m_lastSearchType, m_previousTypeFilter, m_lastSearchStr, NEXT_PAGE);
    }
}

void biblioteq::slotPageClicked(const QString &link)
{
  if(m_db.isOpen())
    {
      if(m_lastSearchStr == "Item Search Query")
	(void) populateTable(m_searchQuery,
			     m_previousTypeFilter,
			     -link.toInt(),
			     m_lastSearchType);
      else
	(void) populateTable(m_lastSearchType,
			     m_previousTypeFilter,
			     m_lastSearchStr,
			     -link.toInt());
    }
}

void biblioteq::slotPreviousPage(void)
{
  if(m_db.isOpen())
    {
      if(m_lastSearchStr == "Item Search Query")
	(void) populateTable(m_searchQuery,
			     m_previousTypeFilter,
			     PREVIOUS_PAGE,
			     m_lastSearchType);
      else
	(void) populateTable(m_lastSearchType,
			     m_previousTypeFilter,
			     m_lastSearchStr,
			     PREVIOUS_PAGE);
    }
}

void biblioteq::slotPrintPreview(QPrinter *printer)
{
  if(printer)
    m_printPreview->print(printer);
}

void biblioteq::slotPrintReservationHistory(void)
{
  if(history.table->rowCount() == 0)
    {
      if(m_members_diag->isVisible())
	QMessageBox::critical
	  (m_history_diag,
	   tr("BiblioteQ: User Error"),
	   tr("The selected member does not yet have a "
	      "reservation history to print."));
      else
	QMessageBox::critical
	  (m_history_diag,
	   tr("BiblioteQ: User Error"),
	   tr("You do not yet have a reservation history to print."));

      QApplication::processEvents();
      return;
    }

  history.printButton->repaint();

  QPrinter printer;
  QScopedPointer<QPrintDialog> dialog
    (new QPrintDialog(&printer, m_history_diag));
  QTextDocument document;

  printer.setColorMode(QPrinter::GrayScale);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
  printer.setPageSize(QPageSize(QPageSize::Letter));
#else
  printer.setPageSize(QPrinter::Letter);
#endif

  if(dialog->exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      document.setHtml(reservationHistoryHtml());
      document.print(&printer);
    }

  QApplication::processEvents();
}

void biblioteq::slotPrintReservationHistoryPreview(void)
{
  if(history.table->rowCount() == 0)
    {
      if(m_members_diag->isVisible())
	QMessageBox::critical
	  (m_history_diag,
	   tr("BiblioteQ: User Error"),
	   tr("The selected member does not yet have a "
	      "reservation history to print."));
      else
	QMessageBox::critical
	  (m_history_diag,
	   tr("BiblioteQ: User Error"),
	   tr("You do not yet have a reservation history to print."));

      QApplication::processEvents();
      return;
    }

  history.printButton->repaint();

  QPrinter printer;
  QScopedPointer<QPrintPreviewDialog> printDialog
    (new QPrintPreviewDialog(&printer, this));

  printDialog->setWindowModality(Qt::ApplicationModal);
  printer.setColorMode(QPrinter::GrayScale);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
  printer.setPageOrientation(QPageLayout::Landscape);
  printer.setPageSize(QPageSize(QPageSize::Letter));
#else
  printer.setOrientation(QPrinter::Landscape);
  printer.setPageSize(QPrinter::Letter);
#endif
  connect(printDialog.data(),
	  SIGNAL(paintRequested(QPrinter *)),
	  this,
	  SLOT(slotPrintPreview(QPrinter *)));
  QApplication::setOverrideCursor(Qt::WaitCursor);
  m_printPreview->setHtml(reservationHistoryHtml());
#ifdef Q_OS_ANDROID
  printDialog->showMaximized();
#else
  printDialog->show();
#endif
  QApplication::restoreOverrideCursor();

  if(printDialog->exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      m_printPreview->print(&printer);
    }

  QApplication::processEvents();
}

void biblioteq::slotPrintReserved(void)
{
  auto const row = bb.table->currentRow();

  if(row < 0)
    {
      QMessageBox::critical
	(m_members_diag,
	 tr("BiblioteQ: User Error"),
	 tr("In order to print a member's reserved items, "
	    "you must first select the member."));
      QApplication::processEvents();
      return;
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  auto const itemsReserved = biblioteq_misc_functions::getColumnString
    (bb.table,
     row,
     m_bbColumnHeaderIndexes.indexOf("Books Reserved")).toInt() +
    biblioteq_misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("CDs Reserved")).toInt() +
    biblioteq_misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("DVDs Reserved")).toInt() +
    biblioteq_misc_functions::getColumnString
    (bb.table,
     row,
     m_bbColumnHeaderIndexes.
     indexOf("Grey Literatures Reserved")).toInt() +
    biblioteq_misc_functions::getColumnString
    (bb.table,
     row,
     m_bbColumnHeaderIndexes.indexOf("Journals Reserved")).toInt() +
    biblioteq_misc_functions::getColumnString
    (bb.table,
     row,
     m_bbColumnHeaderIndexes.indexOf("Magazines Reserved")).toInt() +
    biblioteq_misc_functions::getColumnString
    (bb.table,
     row,
     m_bbColumnHeaderIndexes.indexOf("Video Games Reserved")).toInt();
  auto const memberid = biblioteq_misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("Member ID"));

  QApplication::restoreOverrideCursor();

  if(itemsReserved < 1)
    {
      QMessageBox::critical(m_members_diag,
			    tr("BiblioteQ: User Error"),
			    tr("The member (%1) that you selected "
			       "does not have reserved items.").arg(memberid));
      QApplication::processEvents();
      return;
    }

  bb.printButton->repaint();

  QMap<QString, QString> memberinfo;
  QPrinter printer;
  QScopedPointer<QPrintDialog> dialog
    (new QPrintDialog(&printer, m_members_diag));
  QString errorstr = "";
  QString str = "";
  QTextDocument document;

  memberinfo["firstname"] = biblioteq_misc_functions::getColumnString
    (bb.table,
     row,
     m_bbColumnHeaderIndexes.indexOf("First Name"));
  memberinfo["lastname"] = biblioteq_misc_functions::getColumnString
    (bb.table,
     row,
     m_bbColumnHeaderIndexes.indexOf("Last Name"));
  QApplication::setOverrideCursor(Qt::WaitCursor);

  const auto itemsList = biblioteq_misc_functions::getReservedItems
    (memberid, m_db, errorstr);

  QApplication::restoreOverrideCursor();

  if(errorstr.isEmpty())
    {
      str = "<html>\n";
      str += tr("Reserved Items for ") +
	memberinfo.value("lastname") +
	tr(", ") +
	memberinfo.value("firstname") +
	"<br><br>";

      for(int i = 0; i < itemsList.size(); i++)
	str += itemsList[i] + "<br><br>";

      str = str.mid(0, str.length() - 8);
      str += "</html>";
      printer.setColorMode(QPrinter::GrayScale);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
      printer.setPageSize(QPageSize(QPageSize::Letter));
#else
      printer.setPageSize(QPrinter::Letter);
#endif

      if(dialog->exec() == QDialog::Accepted)
	{
	  QApplication::processEvents();
	  document.setHtml(str);
	  document.print(&printer);
	}

      QApplication::processEvents();
    }
  else
    {
      addError(tr("Database Error"),
	       tr("Unable to determine the reserved items for "
		  "the selected member."),
	       errorstr,
	       __FILE__,
	       __LINE__);
      QMessageBox::critical(m_members_diag,
			    tr("BiblioteQ: Database Error"),
			    tr("Unable to determine the reserved items for "
			       "the selected member."));
      QApplication::processEvents();
    }
}

void biblioteq::slotPrintView(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(action && action->menu())
    action->menu()->repaint();

  QPrinter printer;
  QScopedPointer<QPrintDialog> dialog(new QPrintDialog(&printer, this));
  QTextDocument document;
  auto const html(viewHtml());

  printer.setColorMode(QPrinter::GrayScale);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
  printer.setPageOrientation(QPageLayout::Landscape);
  printer.setPageSize(QPageSize(QPageSize::Letter));
#else
  printer.setOrientation(QPrinter::Landscape);
  printer.setPageSize(QPrinter::Letter);
#endif

  if(dialog->exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      document.setHtml(html);
      document.print(&printer);
    }

  QApplication::processEvents();
}

void biblioteq::slotPrintViewPreview(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(action && action->menu())
    action->menu()->repaint();

  QPrinter printer;
  QScopedPointer<QPrintPreviewDialog> printDialog
    (new QPrintPreviewDialog(&printer, this));

  printDialog->setWindowModality(Qt::ApplicationModal);
  printer.setColorMode(QPrinter::GrayScale);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
  printer.setPageOrientation(QPageLayout::Landscape);
  printer.setPageSize(QPageSize(QPageSize::Letter));
#else
  printer.setOrientation(QPrinter::Landscape);
  printer.setPageSize(QPrinter::Letter);
#endif
  connect(printDialog.data(),
	  SIGNAL(paintRequested(QPrinter *)),
	  this,
	  SLOT(slotPrintPreview(QPrinter *)));
  QApplication::setOverrideCursor(Qt::WaitCursor);
  m_printPreview->setHtml(viewHtml());
#ifdef Q_OS_ANDROID
  printDialog->showMaximized();
#else
  printDialog->show();
#endif
  QApplication::restoreOverrideCursor();

  if(printDialog->exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      m_printPreview->print(&printer);
    }

  QApplication::processEvents();
}

void biblioteq::slotRefresh(void)
{
  if(m_db.isOpen())
    {
      QApplication::processEvents();

      QString str("%");
      auto const data
	(ui.menu_Category->defaultAction() ?
	 ui.menu_Category->defaultAction()->data().toString() : "All");

      if(data == "All Overdue" && m_roles.isEmpty())
	str = dbUserName();
      else if(data == "All Requested" && m_roles.isEmpty())
	str = dbUserName();
      else if(data == "All Reserved" && m_roles.isEmpty())
	str = dbUserName();

      (void) populateTable(POPULATE_ALL, data, str.trimmed());
    }
}

void biblioteq::slotReserveCopy(void)
{
  auto const row = ui.table->currentRow();

  if(row < 0)
    {
      if(m_members_diag->isVisible())
	QMessageBox::critical
	  (m_members_diag,
	   tr("BiblioteQ: User Error"),
	   tr("In order to reserve an item, you must first select it."));
      else
	QMessageBox::critical
	  (this,
	   tr("BiblioteQ: User Error"),
	   tr("In order to reserve an item, you must first select it."));

      QApplication::processEvents();
      return;
    }

  auto const type = biblioteq_misc_functions::getColumnString
    (ui.table, row, ui.table->columnNumber("Type"));

  if(type == "Photograph Collection")
    {
      if(m_members_diag->isVisible())
	QMessageBox::critical(m_members_diag,
			      tr("BiblioteQ: User Error"),
			      tr("Photographs may not be reserved."));
      else
	QMessageBox::critical(this,
			      tr("BiblioteQ: User Error"),
			      tr("Photographs may not be reserved."));

      QApplication::processEvents();
      return;
    }

  auto const oid = biblioteq_misc_functions::getColumnString
    (ui.table, row, ui.table->columnNumber("MYOID"));

  QApplication::setOverrideCursor(Qt::WaitCursor);

  QString errorstr = "";
  auto const availability = biblioteq_misc_functions::getAvailability
    (oid, m_db, type, errorstr).toInt();

  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    {
      addError
	(tr("Database Error"),
	 tr("Unable to determine the availability of the selected item."),
	errorstr,
	__FILE__,
	__LINE__);

      if(m_members_diag->isVisible())
	QMessageBox::critical
	  (m_members_diag,
	   tr("BiblioteQ: Database Error"),
	   tr("Unable to determine the availability of the selected item."));
      else
	QMessageBox::critical
	  (this,
	   tr("BiblioteQ: Database Error"),
	   tr("Unable to determine the availability of the selected item."));

      QApplication::processEvents();
      return;
    }

  if(availability < 1)
    {
      if(m_members_diag->isVisible())
	QMessageBox::critical
	  (m_members_diag,
	   tr("BiblioteQ: User Error"),
	   tr("It appears that the item you selected "
	      "is not available for reservation."));
      else
	QMessageBox::critical
	  (this,
	   tr("BiblioteQ: User Error"),
	   tr("It appears that the item you selected "
	      "is not available for reservation."));

      QApplication::processEvents();
      return;
    }

  slotShowMembersBrowser();

  if(!ui.actionPopulate_Members_Browser_Table_on_Display->isChecked())
    {
      if(!m_membersWasRefreshed)
	if(QMessageBox::question(m_members_diag,
				 tr("BiblioteQ: Question"),
				 tr("Would you like to retrieve the list of "
				    "members?"),
				 QMessageBox::No | QMessageBox::Yes,
				 QMessageBox::No) == QMessageBox::Yes)
	  {
	    QApplication::processEvents();
	    slotPopulateMembersBrowser();
	  }

      QApplication::processEvents();
    }

  auto const memberId
    (biblioteq_misc_functions::getColumnString(ui.table,
					       ui.table->currentRow(),
					       "Member ID"));

  if(memberId.isEmpty())
    {
      if(bb.table->currentRow() < 0)
	bb.table->selectRow(0);
    }
  else
    {
      auto const column = biblioteq_misc_functions::getColumnNumber
	(bb.table, "Member ID");

      if(column > -1)
	{
	  QApplication::setOverrideCursor(Qt::WaitCursor);

	  for(int i = 0; i < bb.table->rowCount(); i++)
	    if(bb.table->item(i, column) &&
	       bb.table->item(i, column)->text() == memberId)
	      {
		bb.table->selectRow(i);
		break;
	      }

	  QApplication::restoreOverrideCursor();
	}
    }
}

void biblioteq::slotReset(void)
{
  if(m_all_diag->isVisible())
    {
      auto action = qobject_cast<QAction *> (sender());

      if(action != nullptr)
	{
	  auto const actions(al.resetButton->menu()->actions());

	  if(actions.size() < 14)
	    {
	      // Error.
	    }
	  else if(action == actions[0])
	    {
	      al.idnumber->clear();
	      al.idnumber->setFocus();
	    }
	  else if(action == actions[1])
	    {
	      al.title->clear();
	      al.title->setFocus();
	    }
	  else if(action == actions[2])
	    {
	      al.publication_date->setDate(QDate::fromString("2001", "yyyy"));
	      al.publication_date->setFocus();
	      al.publication_date_enabled->setChecked(false);
	    }
	  else if(action == actions[3])
	    {
	      al.publisher->clear();
	      al.publisher->setFocus();
	    }
	  else if(action == actions[4])
	    {
	      al.categories_checkbox->setChecked(false);
	      al.category->clear();
	      al.category->setFocus();
	    }
	  else if(action == actions[5])
	    {
	      al.price->setFocus();
	      al.price->setValue(-0.01);
	    }
	  else if(action == actions[6])
	    {
	      al.language->setCurrentIndex(0);
	      al.language->setFocus();
	    }
	  else if(action == actions[7])
	    {
	      al.monetary_units->setCurrentIndex(0);
	      al.monetary_units->setFocus();
	    }
	  else if(action == actions[8])
	    {
	      al.abstract_checkbox->setChecked(false);
	      al.description->clear();
	      al.description->setFocus();
	    }
	  else if(action == actions[9])
	    {
	      al.quantity->setFocus();
	      al.quantity->setValue(0);
	    }
	  else if(action == actions[10])
	    {
	      al.location->setCurrentIndex(0);
	      al.location->setFocus();
	    }
	  else if(action == actions[11])
	    {
	      al.keyword->clear();
	      al.keyword->setFocus();
	      al.keywords_checkbox->setChecked(false);
	    }
	  else if(action == actions[12])
	    {
	      al.available->setChecked(false);
	      al.available->setFocus();
	    }
	  else if(action == actions[13])
	    {
	      al.caseinsensitive->setChecked(false);
	      al.caseinsensitive->setFocus();
	    }
	}
      else
	{
	  al.abstract_checkbox->setChecked(false);
	  al.available->setChecked(false);
	  al.caseinsensitive->setChecked(false);
	  al.categories_checkbox->setChecked(false);
	  al.category->clear();
	  al.description->clear();
	  al.idnumber->clear();
	  al.idnumber->setFocus();
	  al.keyword->clear();
	  al.keywords_checkbox->setChecked(false);
	  al.language->setCurrentIndex(0);
	  al.location->setCurrentIndex(0);
	  al.monetary_units->setCurrentIndex(0);
	  al.price->setMinimum(-0.01);
	  al.price->setValue(-0.01);
	  al.publication_date->setDate(QDate::fromString("2001", "yyyy"));
	  al.publication_date_enabled->setChecked(false);
	  al.publisher->clear();
	  al.quantity->setMinimum(0);
	  al.quantity->setValue(0);
	  al.title->clear();
	}
    }
}

void biblioteq::slotResetErrorLog(void)
{
  QStringList list;

  list.append(tr("Event Time"));
  list.append(tr("Event Type"));
  list.append(tr("Summary"));
  list.append(tr("Full Description"));
  list.append(tr("File"));
  list.append(tr("Line Number"));
  er.table->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  er.table->horizontalScrollBar()->setValue(0);
  er.table->scrollToTop();
  er.table->setColumnCount(0);
  er.table->setColumnCount(list.size());
  er.table->setCurrentItem(nullptr);
  er.table->setHorizontalHeaderLabels(list);
  er.table->setRowCount(0);

  for(int i = 0; i < er.table->columnCount() - 1; i++)
    er.table->resizeColumnToContents(i);

  if(m_error_bar_label != nullptr)
    {
      m_error_bar_label->setToolTip(tr("Empty Error Log"));
      prepareStatusBarIcons();
    }
}

void biblioteq::slotResetLoginDialog(void)
{
  br.filename->clear();
  br.password->setText(BIBLIOTEQ_GUEST_ACCOUNT);
  br.role->setCurrentIndex(1);
  br.show_password->setChecked(false);
  br.userid->setText(BIBLIOTEQ_GUEST_ACCOUNT);

  QSettings settings;
  auto const index = br.branch_name->findText
    (settings.value("previous_branch_name").toString());

  if(index >= 0)
    br.branch_name->setCurrentIndex(index);
  else
    br.branch_name->setCurrentIndex(0);

  slotBranchChanged();
}

void biblioteq::slotResizeColumns(void)
{
  if(!sender())
    if(!ui.actionAutomatically_Resize_Column_Widths->isChecked())
      return;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  for(int i = 0; i < ui.table->columnCount() - 1; i++)
    ui.table->resizeColumnToContents(i);

  ui.table->horizontalHeader()->setStretchLastSection(true);
  QApplication::restoreOverrideCursor();
}

void biblioteq::slotResizeColumnsAfterSort(void)
{
  QObject *parent = nullptr;
  auto object = qobject_cast<QObject *> (sender());

  if(object != nullptr && object->parent() != nullptr)
    {
      if(!ui.actionAutomatically_Resize_Column_Widths->isChecked() &&
	 object->parent() == ui.table)
	return;

      QApplication::setOverrideCursor(Qt::WaitCursor);
      parent = object->parent();
      (qobject_cast<QTableWidget *> (parent))->resizeColumnsToContents();
      (qobject_cast<QTableWidget *> (parent))->horizontalHeader()->
	setStretchLastSection(true);
      QApplication::restoreOverrideCursor();
    }
}

void biblioteq::slotSavePassword(void)
{
  QString errorstr = "";

  if(pass.password->text().length() < 8 ||
     pass.passwordAgain->text().length() < 8)
    {
      QMessageBox::critical
	(m_pass_diag,
	 tr("BiblioteQ: User Error"),
	 tr("The password must contain at least eight characters."));
      QApplication::processEvents();
      pass.password->selectAll();
      pass.password->setFocus();
      return;
    }
  else if(pass.password->text() != pass.passwordAgain->text())
    {
      QMessageBox::critical
	(m_pass_diag,
	 tr("BiblioteQ: User Error"),
	 tr("The passwords do not match. Please try again."));
      QApplication::processEvents();
      pass.password->selectAll();
      pass.password->setFocus();
      return;
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);
  biblioteq_misc_functions::savePassword
    (pass.userid->text(), m_db, pass.password->text(), errorstr);

  if(m_roles.isEmpty())
    biblioteq_misc_functions::setRole(m_db, errorstr, "patron");
  else
    biblioteq_misc_functions::setRole(m_db, errorstr, m_roles);

  QApplication::restoreOverrideCursor();
  pass.password->setText(QString(1024, '0'));
  pass.password->clear();
  pass.passwordAgain->setText(QString(1024, '0'));
  pass.passwordAgain->clear();

  if(!errorstr.isEmpty())
    {
      addError(tr("Database Error"),
	       tr("Unable to save the new password."),
	       errorstr,
	       __FILE__,
	       __LINE__);
      QMessageBox::critical(m_pass_diag,
			    tr("BiblioteQ: Database Error"),
			    tr("Unable to save the new password."));
      QApplication::processEvents();
    }
  else
#ifdef Q_OS_ANDROID
    m_pass_diag->hide();
#else
    m_pass_diag->close();
#endif
}

void biblioteq::slotSearch(void)
{
  if(!m_db.isOpen())
    return;
  else if(!al.resetButton->isEnabled())
    {
#ifdef Q_OS_ANDROID
      m_all_diag->showMaximized();
#else
      if(!m_all_diag->isVisible())
	m_all_diag->updateGeometry();

      biblioteq_misc_functions::center(m_all_diag, this);
      m_all_diag->showNormal();
#endif
      m_all_diag->activateWindow();
      m_all_diag->raise();
      return;
    }

  QString errorstr("");

  if(m_allSearchShown)
    goto done_label;

  /*
  ** Hide certain fields if we're a regular user.
  */

  biblioteq_misc_functions::hideAdminFields(m_all_diag, m_roles);
  al.abstract_checkbox->setChecked(false);
  al.available->setChecked(false);
  al.categories_checkbox->setChecked(false);
  al.category->clear();
  al.description->clear();
  al.idnumber->clear();
  al.keyword->clear();
  al.keywords_checkbox->setChecked(false);
  al.language->clear();
  al.location->clear();
  al.monetary_units->clear();
  al.photograph_reminder_label->setVisible(true); /*
						  ** Hidden by
						  ** hideAdminFields().
						  */
  al.price->setMinimum(-0.01);
  al.price->setValue(-0.01);
  al.publication_date->setDate(QDate::fromString("2001", "yyyy"));
  al.publication_date_enabled->setChecked(false);
  al.publisher->clear();
  al.quantity->setMinimum(0);
  al.quantity->setValue(0);
  al.title->clear();

  /*
  ** Populate combination boxes.
  */

  QApplication::setOverrideCursor(Qt::WaitCursor);
  al.language->addItems
    (biblioteq_misc_functions::getLanguages(m_db, errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    addError(tr("Database Error"),
	     tr("Unable to retrieve the languages."),
	     errorstr,
	     __FILE__,
	     __LINE__);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  al.monetary_units->addItems
    (biblioteq_misc_functions::getMonetaryUnits(m_db, errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    addError(tr("Database Error"),
	     tr("Unable to retrieve the monetary units."),
	     errorstr,
	     __FILE__,
	     __LINE__);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  al.location->addItems
    (biblioteq_misc_functions::getLocations(m_db, "", errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    addError
      (tr("Database Error"),
       tr("Unable to retrieve the locations."),
       errorstr,
       __FILE__,
       __LINE__);

  al.idnumber->setFocus();
  al.language->addItem(s_unknown);
  al.language->insertItem(0, tr("Any"));
  al.language->setCurrentIndex(0);
  al.location->addItem(s_unknown);
  al.location->insertItem(0, tr("Any"));
  al.location->setCurrentIndex(0);
  al.monetary_units->addItem(s_unknown);
  al.monetary_units->insertItem(0, tr("Any"));
  al.monetary_units->setCurrentIndex(0);
 done_label:
  m_allSearchShown = true;

  if(!m_all_diag->isVisible())
    m_all_diag->updateGeometry();

#ifdef Q_OS_ANDROID
  m_all_diag->showMaximized();
#else
  auto static resized = false;

  if(!resized)
    m_all_diag->resize
      (qRound(0.95 * size().width()), qRound(0.95 * size().height()));

  resized = true;

  if(!m_all_diag->isVisible())
    biblioteq_misc_functions::center(m_all_diag, this);

  m_all_diag->showNormal();
#endif
  m_all_diag->activateWindow();
  m_all_diag->raise();
}

void biblioteq::slotSectionResized(int logicalIndex, int oldSize, int newSize)
{
  Q_UNUSED(logicalIndex);
  Q_UNUSED(newSize);
  Q_UNUSED(oldSize);
}

void biblioteq::slotSelectDatabaseFile(void)
{
  QFileDialog dialog(m_branch_diag);

  dialog.setDirectory(QDir::homePath());
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter("SQLite Database (*.sqlite)");
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowTitle(tr("BiblioteQ: SQLite Database Selection"));
#ifdef Q_OS_ANDROID
  dialog.showMaximized();
#endif
  dialog.exec();
  QApplication::processEvents();

  if(dialog.result() == QDialog::Accepted)
    br.filename->setText(dialog.selectedFiles().value(0));
}

void biblioteq::slotSetColumns(void)
{
  createConfigToolMenu();

  for(int i = 0; i < m_configToolMenu->actions().size(); i++)
    {
      ui.table->setColumnHidden
	(i, !m_configToolMenu->actions().at(i)->isChecked());
      ui.table->recordColumnHidden
	(dbUserName(),
	 m_configToolMenu->actions().at(i)->data().toString(),
	 i,
	 !m_configToolMenu->actions().at(i)->isChecked());
    }
}

void biblioteq::slotSetFonts(void)
{
  QFontDialog dialog(this);

  dialog.setCurrentFont(QApplication::font());
  dialog.setWindowTitle(tr("BiblioteQ: Select Global Font"));

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      setGlobalFonts(dialog.selectedFont());
    }

  QApplication::processEvents();
}

void biblioteq::slotShowAdminDialog(void)
{
#ifdef Q_OS_ANDROID
  m_admin_diag->showMaximized();
#else
  auto static resized = false;

  if(!resized)
    m_admin_diag->resize
      (qRound(0.95 * size().width()), qRound(0.95 * size().height()));

  resized = true;
  biblioteq_misc_functions::center(m_admin_diag, this);
  m_admin_diag->showNormal();
#endif
  m_admin_diag->activateWindow();
  m_admin_diag->raise();

  if(ui.actionPopulate_Administrator_Browser_Table_on_Display->isChecked())
    slotRefreshAdminList();
}

void biblioteq::slotShowChangePassword(void)
{
  pass.userid->setText(dbUserName());
  pass.password->setText(QString(1024, '0'));
  pass.password->clear();
  pass.passwordAgain->setText(QString(1024, '0'));
  pass.passwordAgain->clear();
  pass.password->setFocus();
#ifdef Q_OS_ANDROID
  m_pass_diag->showMaximized();
#else
  m_pass_diag->show();
#endif
}

void biblioteq::slotShowConnectionDB(void)
{
  slotBranchChanged();
}

void biblioteq::slotShowCustomQuery(void)
{
  auto dialog = new biblioteq_custom_query(m_otherOptions, this);

  connect(dialog,
	  SIGNAL(linkActivated(const QString &)),
	  m_otherOptions,
	  SLOT(slotShowCustomQuery(void)));
  connect(this,
	  SIGNAL(otherOptionsSaved(void)),
	  dialog,
	  SLOT(slotPrepareIcons(void)));
#ifdef Q_OS_ANDROID
  dialog->showMaximized();
#else
  dialog->resize
    (qRound(0.50 * size().width()), qRound(0.95 * size().height()));
  biblioteq_misc_functions::center(dialog, this);
  dialog->show();
#endif
}

void biblioteq::slotShowDbEnumerations(void)
{
  if(!db_enumerations)
    {
      db_enumerations = new biblioteq_dbenumerations(this);
      connect(db_enumerations,
	      SIGNAL(committed(void)),
	      this,
	      SIGNAL(databaseEnumerationsCommitted(void)));
    }

  db_enumerations->show
    (this,
     ui.actionPopulate_Database_Enumerations_Browser_on_Display->isChecked());

  if(bb.database_enumerations_browser_label == sender())
    db_enumerations->setPage
      (biblioteq_dbenumerations::Page::ReservationMinimumDays);
}

void biblioteq::slotShowErrorDialog(void)
{
  er.table->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);

  for(int i = 0; i < er.table->columnCount() - 1; i++)
    er.table->resizeColumnToContents(i);

#ifdef Q_OS_ANDROID
  m_error_diag->showMaximized();
#else
  auto static resized = false;

  if(!resized)
    m_error_diag->resize
      (qRound(0.95 * size().width()), qRound(0.95 * size().height()));

  resized = true;
  biblioteq_misc_functions::center(m_error_diag, this);
  m_error_diag->showNormal();
#endif
  m_error_diag->activateWindow();
  m_error_diag->raise();
}

void biblioteq::slotShowGrid(void)
{
  ui.table->setShowGrid(ui.actionShowGrid->isChecked());
}

void biblioteq::slotShowMembersBrowser(void)
{
  if(!m_db.isOpen())
    return;

  bb.filter->clear();
  bb.filter->setFocus();
  bb.filterBox->setCheckState(Qt::Checked);
  bb.filtertype->setCurrentIndex(0);

  for(int i = 0; i < bb.table->columnCount(); i++)
    {
      auto item = bb.table->horizontalHeaderItem(i);

      if(item)
	bb.table->setColumnHidden
	  (i, !m_otherOptions->isMembersColumnVisible(item->text()));

      if(i < bb.table->columnCount() - 1)
	bb.table->resizeColumnToContents(i);
    }

#ifdef Q_OS_ANDROID
  m_members_diag->showMaximized();
#else
  auto static resized = false;

  if(!resized)
    m_members_diag->resize
      (qRound(0.95 * size().width()), qRound(0.95 * size().height()));

  resized = true;
  biblioteq_misc_functions::center(m_members_diag, this);
  m_members_diag->showNormal();
#endif
  m_members_diag->activateWindow();
  m_members_diag->raise();

  if(ui.actionPopulate_Members_Browser_Table_on_Display->isChecked())
    slotPopulateMembersBrowser();
}

void biblioteq::slotShowMenu(void)
{
  QPoint point;
  auto widget = widgetForAction(qobject_cast<QAction *> (sender()));

  if(widget)
    point = widget->mapToGlobal(widget->rect().bottomRight() - QPoint(5, 5));
  else
    point = QCursor::pos();

  if(sender() == ui.configTool)
    {
      createConfigToolMenu();

      auto const typefilter = ui.menu_Category->defaultAction() ?
	ui.menu_Category->defaultAction()->data().toString() : "All";

      addConfigOptions(typefilter);
      m_configToolMenu->exec(point);
    }
  else if(sender() == ui.createTool)
    {
      QMenu menu(this);

      connect(menu.addAction(tr("Add &Book...")),
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotInsertBook(void)));
      connect(menu.addAction(tr("Add &DVD...")),
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotInsertDVD(void)));
      connect(menu.addAction(tr("Add &Grey Literature...")),
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotInsertGreyLiterature(void)));
      connect(menu.addAction(tr("Add &Journal...")),
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotInsertJourn(void)));
      connect(menu.addAction(tr("Add &Magazine...")),
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotInsertMag(void)));
      connect(menu.addAction(tr("Add Music &CD...")),
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotInsertCD(void)));
      connect(menu.addAction(tr("Add &Photograph Collection...")),
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotInsertPhotograph(void)));
      connect(menu.addAction(tr("Add &Video Game...")),
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotInsertVideoGame(void)));
      menu.exec(point);
    }
  else if(sender() == ui.printTool)
    {
      QMenu menu(this);

      connect(menu.addAction(tr("Print...")),
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotPrintView(void)));
      connect(menu.addAction(tr("Print Preview...")),
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotPrintViewPreview(void)));
      menu.exec(point);
    }
  else if(sender() == ui.searchTool)
    {
      QMenu menu(this);

      connect(menu.addAction(tr("General &Search...")),
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotSearch(void)));
      menu.addSeparator();
      connect(menu.addAction(tr("&Book Search...")),
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotBookSearch(void)));
      connect(menu.addAction(tr("&DVD Search...")),
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotDVDSearch(void)));
      connect(menu.addAction(tr("&Grey Literature Search...")),
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotGreyLiteratureSearch(void)));
      connect(menu.addAction(tr("&Journal Search...")),
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotJournSearch(void)));
      connect(menu.addAction(tr("&Magazine Search...")),
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotMagSearch(void)));
      connect(menu.addAction(tr("Music &CD Search...")),
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotCDSearch(void)));
      connect
	(menu.addAction(tr("&Photograph Collection Search...")),
	 SIGNAL(triggered(void)),
	 this,
	 SLOT(slotPhotographSearch(void)));
      connect
	(menu.addAction(tr("&Video Game Search...")),
	 SIGNAL(triggered(void)),
	 this,
	 SLOT(slotVideoGameSearch(void)));
      menu.exec(point);
    }
  else if(sender() == userinfo_diag->m_userinfo.menu)
    {
      QMenu menu(this);

      connect(menu.addAction(tr("Generate Letter")),
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotGenerateAndCopyMemberLetter(void)));
      connect(menu.addAction(tr("Launch E-Mail Software...")),
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotLaunchEmailSoftware(void)));
      menu.exec(point);
    }
}

void biblioteq::slotShowNext(void)
{
  auto row = bb.table->currentRow();

  if(row == (bb.table->rowCount() - 1))
    row = 0;
  else
    row += 1;

  if(m_history_diag->isVisible())
    {
      bb.table->clearSelection();
      bb.table->selectRow(row);
      slotShowHistory();
    }
  else
    {
      QString str("");

      if(userinfo_diag->haveMemberChanges(str))
	{
	  if(QMessageBox::
	     question(userinfo_diag,
		      tr("BiblioteQ: Question"),
		      tr("Your changes have not been saved. Continue?"
			 "\n%1").arg(str.trimmed()),
		      QMessageBox::No | QMessageBox::Yes,
		      QMessageBox::No) == QMessageBox::No)
	    {
	      QApplication::processEvents();
	      return;
	    }

	  QApplication::processEvents();
	}

      bb.table->clearSelection();
      bb.table->selectRow(row);
      slotModifyBorrower();
    }
}

void biblioteq::slotShowPrev(void)
{
  auto row = bb.table->currentRow();

  if(row == 0)
    row = bb.table->rowCount() - 1;
  else
    row -= 1;

  if(row < 0)
    row = 0;

  if(m_history_diag->isVisible())
    {
      bb.table->clearSelection();
      bb.table->selectRow(row);
      slotShowHistory();
    }
  else
    {
      QString str("");

      if(userinfo_diag->haveMemberChanges(str))
	{
	  if(QMessageBox::
	     question(userinfo_diag,
		      tr("BiblioteQ: Question"),
		      tr("Your changes have not been saved. Continue?"
			 "\n%1").arg(str.trimmed()),
		      QMessageBox::No | QMessageBox::Yes,
		      QMessageBox::No) == QMessageBox::No)
	    {
	      QApplication::processEvents();
	      return;
	    }

	  QApplication::processEvents();
	}

      bb.table->clearSelection();
      bb.table->selectRow(row);
      slotModifyBorrower();
    }
}

void biblioteq::slotSqliteFileSelected(bool state)
{
  Q_UNUSED(state);

  auto action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  br.filename->setText(action->data().toString());
  br.filename->setCursorPosition(0);

  for(int i = 0; i < br.branch_name->count(); i++)
    {
      if(m_branches.contains(br.branch_name->itemText(i)))
	if(m_branches[br.branch_name->itemText(i)].value("database_type") ==
	   "sqlite")
	  {
	    br.branch_name->setCurrentIndex(i);
	    break;
	  }
    }

  slotConnectDB();
}

void biblioteq::slotUpdateIndicesAfterSort(int column)
{
  auto order = Qt::AscendingOrder;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(ui.table->horizontalHeader()->sortIndicatorOrder() != Qt::AscendingOrder)
    order = Qt::DescendingOrder;
  else
    order = Qt::AscendingOrder;

  ui.table->horizontalHeader()->setSortIndicator(column, order);
  ui.table->sortByColumn(column, order);
  QApplication::restoreOverrideCursor();
}

void biblioteq::slotViewDetails(void)
{
  std::unique_lock<std::mutex> lock{m_mutex, std::defer_lock};

  if(!lock.try_lock())
    return;

  auto list(ui.table->selectionModel()->selectedRows());

  if(list.isEmpty())
    {
      QMessageBox::critical(this,
			    tr("BiblioteQ: User Error"),
			    tr("Please select at least one item to view."));
      QApplication::processEvents();
      return;
    }
  else if(list.size() >= MAXIMUM_DEVICES_CONFIRMATION)
    {
      if(QMessageBox::
	 question(this,
		  tr("BiblioteQ: Question"),
		  tr("Are you sure that you wish to view the %1 "
		     "selected item(s)? BiblioteQ will terminate if "
		     "it's unable to acquire resources.").arg(list.size()),
		  QMessageBox::No | QMessageBox::Yes,
		  QMessageBox::No) == QMessageBox::No)
	{
	  QApplication::processEvents();
	  return;
	}

      QApplication::processEvents();
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  auto error = false;
  auto table = ui.table;
  biblioteq_book *book = nullptr;
  biblioteq_cd *cd = nullptr;
  biblioteq_dvd *dvd = nullptr;
  biblioteq_grey_literature *gl = nullptr;
  biblioteq_journal *journal = nullptr;
  biblioteq_magazine *magazine = nullptr;
  biblioteq_photographcollection *photograph = nullptr;
  biblioteq_videogame *videogame = nullptr;
  int c = 1;

  std::stable_sort
    (list.begin(), list.end(), biblioteq_misc_functions::sortByRow);

  foreach(auto const &index, list)
    {
      auto const i = index.row();
      auto const oid = biblioteq_misc_functions::getColumnString
	(table, i, table->columnNumber("MYOID"));
      auto const type = biblioteq_misc_functions::getColumnString
	(table, i, table->columnNumber("Type"));

      book = nullptr;
      cd = nullptr;
      dvd = nullptr;
      gl = nullptr;
      journal = nullptr;
      magazine = nullptr;
      photograph = nullptr;
      showStatusBarMessage
	(tr("Retrieving item %1 of %2.").arg(c++).arg(list.size()), 5000);
      videogame = nullptr;

      if(type.toLower() == "book")
	{
	  auto created = true;

	  foreach(auto w, QApplication::topLevelWidgets())
	    {
	      auto b = qobject_cast<biblioteq_book *> (w);

	      if(b && b->getID() == oid)
		{
		  book = b;
		  created = false;
		  break;
		}
	    }

	  if(!book)
	    book = dynamic_cast<biblioteq_book *> (findItemInTab(oid));

	  if(!book)
	    book = new biblioteq_book(this, oid, index);
	  else
	    created = false;

	  addItemWindowToTab(book);

	  if(created)
	    {
	      book->modify(VIEW_ONLY);
	      connect(this,
		      SIGNAL(databaseEnumerationsCommitted(void)),
		      book,
		      SLOT(slotDatabaseEnumerationsCommitted(void)));
	    }
	}
      else if(type.toLower() == "cd")
	{
	  auto created = true;

	  foreach(auto w, QApplication::topLevelWidgets())
	    {
	      auto c = qobject_cast<biblioteq_cd *> (w);

	      if(c && c->getID() == oid)
		{
		  cd = c;
		  created = false;
		  break;
		}
	    }

	  if(!cd)
	    cd = dynamic_cast<biblioteq_cd *> (findItemInTab(oid));

	  if(!cd)
	    cd = new biblioteq_cd(this, oid, index);
	  else
	    created = false;

	  addItemWindowToTab(cd);

	  if(created)
	    {
	      cd->modify(VIEW_ONLY);
	      connect(this,
		      SIGNAL(databaseEnumerationsCommitted(void)),
		      cd,
		      SLOT(slotDatabaseEnumerationsCommitted(void)));
	    }
	}
      else if(type.toLower() == "dvd")
	{
	  auto created = true;

	  foreach(auto w, QApplication::topLevelWidgets())
	    {
	      auto d = qobject_cast<biblioteq_dvd *> (w);

	      if(d && d->getID() == oid)
		{
		  created = false;
		  dvd = d;
		  break;
		}
	    }

	  if(!dvd)
	    dvd = dynamic_cast<biblioteq_dvd *> (findItemInTab(oid));

	  if(!dvd)
	    dvd = new biblioteq_dvd(this, oid, index);
	  else
	    created = false;

	  addItemWindowToTab(dvd);

	  if(created)
	    {
	      dvd->modify(VIEW_ONLY);
	      connect(this,
		      SIGNAL(databaseEnumerationsCommitted(void)),
		      dvd,
		      SLOT(slotDatabaseEnumerationsCommitted(void)));
	    }
	}
      else if(type.toLower() == "grey literature")
	{
	  auto created = true;

	  foreach(auto w, QApplication::topLevelWidgets())
	    {
	      auto g = qobject_cast<biblioteq_grey_literature *> (w);

	      if(g && g->getID() == oid)
		{
		  created = false;
		  gl = g;
		  break;
		}
	    }

	  if(!gl)
	    gl = dynamic_cast<biblioteq_grey_literature *>
	      (findItemInTab(oid));

	  if(!gl)
	    gl = new biblioteq_grey_literature(this, oid, index);
	  else
	    created = false;

	  addItemWindowToTab(gl);

	  if(created)
	    {
	      gl->modify(VIEW_ONLY);
	      connect(this,
		      SIGNAL(databaseEnumerationsCommitted(void)),
		      gl,
		      SLOT(slotDatabaseEnumerationsCommitted(void)));
	    }
	}
      else if(type.toLower() == "journal")
	{
	  auto created = true;

	  foreach(auto w, QApplication::topLevelWidgets())
	    {
	      auto j = qobject_cast<biblioteq_journal *> (w);

	      if(j && j->getID() == oid)
		{
		  created = false;
		  journal = j;
		  break;
		}
	    }

	  if(!journal)
	    journal = dynamic_cast<biblioteq_journal *> (findItemInTab(oid));

	  if(!journal)
	    journal = new biblioteq_journal(this, oid, index);
	  else
	    created = false;

	  addItemWindowToTab(journal);

	  if(created)
	    {
	      journal->modify(VIEW_ONLY);
	      connect(this,
		      SIGNAL(databaseEnumerationsCommitted(void)),
		      journal,
		      SLOT(slotDatabaseEnumerationsCommitted(void)));
	    }
	}
      else if(type.toLower() == "magazine")
	{
	  auto created = true;

	  foreach(auto w, QApplication::topLevelWidgets())
	    {
	      auto m = qobject_cast<biblioteq_magazine *> (w);

	      /*
	      ** The class biblioteq_journal inherits biblioteq_magazine.
	      */

	      if(!qobject_cast<biblioteq_journal *> (w))
		if(m && m->getID() == oid)
		  {
		    created = false;
		    magazine = m;
		    break;
		  }
	    }

	  if(!magazine)
	    magazine = dynamic_cast<biblioteq_magazine *> (findItemInTab(oid));

	  if(!magazine)
	    magazine = new biblioteq_magazine(this, oid, index, "magazine");
	  else
	    created = false;

	  addItemWindowToTab(magazine);

	  if(created)
	    {
	      magazine->modify(VIEW_ONLY);
	      connect(this,
		      SIGNAL(databaseEnumerationsCommitted(void)),
		      magazine,
		      SLOT(slotDatabaseEnumerationsCommitted(void)));
	    }
	}
      else if(type.toLower() == "photograph collection")
	{
	  auto created = true;

	  foreach(auto w, QApplication::topLevelWidgets())
	    {
	      auto p = qobject_cast<biblioteq_photographcollection *> (w);

	      if(p && p->getID() == oid)
		{
		  created = false;
		  photograph = p;
		  break;
		}
	    }

	  if(!photograph)
	    photograph = dynamic_cast<biblioteq_photographcollection *>
	      (findItemInTab(oid));

	  if(!photograph)
	    photograph = new biblioteq_photographcollection(this, oid, index);
	  else
	    created = false;

	  addItemWindowToTab(photograph);

	  if(created)
	    {
	      photograph->modify(VIEW_ONLY);
	      connect(this,
		      SIGNAL(databaseEnumerationsCommitted(void)),
		      photograph,
		      SLOT(slotDatabaseEnumerationsCommitted(void)));
	    }
	}
      else if(type.toLower() == "video game")
	{
	  auto created = true;

	  foreach(auto w, QApplication::topLevelWidgets())
	    {
	      auto v = qobject_cast<biblioteq_videogame *> (w);

	      if(v && v->getID() == oid)
		{
		  created = false;
		  videogame = v;
		  break;
		}
	    }

	  if(!videogame)
	    videogame = dynamic_cast<biblioteq_videogame *>
	      (findItemInTab(oid));

	  if(!videogame)
	    videogame = new biblioteq_videogame(this, oid, index);
	  else
	    created = false;

	  addItemWindowToTab(videogame);

	  if(created)
	    {
	      videogame->modify(VIEW_ONLY);
	      connect(this,
		      SIGNAL(databaseEnumerationsCommitted(void)),
		      videogame,
		      SLOT(slotDatabaseEnumerationsCommitted(void)));
	    }
	}
      else
	{
	  error = true;
	  break;
	}
    }

  QApplication::restoreOverrideCursor();

  if(error)
    {
      QMessageBox::critical
	(this,
	 tr("BiblioteQ: Error"),
	 tr("Unable to determine the selected item's type."));
      QApplication::processEvents();
    }
}

void biblioteq::updateItemWindows(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  for(int i = 1; i < ui.tab->count(); i++)
    {
      auto w = dynamic_cast<biblioteq_item *> (ui.tab->widget(i));

      if(!w)
	continue;

      auto book = dynamic_cast<biblioteq_book *> (w);
      auto cd = dynamic_cast<biblioteq_cd *> (w);
      auto dvd = dynamic_cast<biblioteq_dvd *> (w);
      auto gl = dynamic_cast<biblioteq_grey_literature *> (w);
      auto journal = dynamic_cast<biblioteq_journal *> (w);
      auto magazine = dynamic_cast<biblioteq_magazine *> (w);
      auto photograph = dynamic_cast<biblioteq_photographcollection *> (w);
      auto videogame = dynamic_cast<biblioteq_videogame *> (w);

      if(book)
	book->updateWindow(EDITABLE);

      if(cd)
	cd->updateWindow(EDITABLE);

      if(dvd)
	dvd->updateWindow(EDITABLE);

      if(gl)
	gl->updateWindow(EDITABLE);

      if(journal)
	journal->updateWindow(EDITABLE);

      if(!dynamic_cast<biblioteq_journal *> (w))
	if(magazine)
	  magazine->updateWindow(EDITABLE);

      if(photograph)
	photograph->updateWindow(EDITABLE);

      if(videogame)
	videogame->updateWindow(EDITABLE);
    }

  foreach(auto w, QApplication::topLevelWidgets())
    {
      auto book = qobject_cast<biblioteq_book *> (w);
      auto cd = qobject_cast<biblioteq_cd *> (w);
      auto dvd = qobject_cast<biblioteq_dvd *> (w);
      auto gl = qobject_cast<biblioteq_grey_literature *> (w);
      auto journal = qobject_cast<biblioteq_journal *> (w);
      auto magazine = qobject_cast<biblioteq_magazine *> (w);
      auto photograph = qobject_cast<biblioteq_photographcollection *> (w);
      auto videogame = qobject_cast<biblioteq_videogame *> (w);

      if(book)
	book->updateWindow(EDITABLE);

      if(cd)
	cd->updateWindow(EDITABLE);

      if(dvd)
	dvd->updateWindow(EDITABLE);

      if(gl)
	gl->updateWindow(EDITABLE);

      if(journal)
	journal->updateWindow(EDITABLE);

      if(!qobject_cast<biblioteq_journal *> (w))
	if(magazine)
	  magazine->updateWindow(EDITABLE);

      if(photograph)
	photograph->updateWindow(EDITABLE);

      if(videogame)
	videogame->updateWindow(EDITABLE);
    }

  QApplication::restoreOverrideCursor();
}

void biblioteq::updateRows
(const QString &oid, const QTableWidgetItem *item, const QString &it)
{
  auto const index = ui.table->indexFromItem(item);
  auto const itemType(it.toLower().remove(" ").trimmed());

  if(itemType == "book")
    {
      auto item = dynamic_cast<biblioteq_book *> (findItemInTab(oid));

      if(item)
	item->updateRow(index);
      else
	foreach(auto w, QApplication::topLevelWidgets())
	  {
	    auto book = qobject_cast<biblioteq_book *> (w);

	    if(book && book->getID() == oid)
	      {
		book->updateRow(index);
		break;
	      }
	  }
    }
  else if(itemType == "cd")
    {
      auto item = dynamic_cast<biblioteq_cd *> (findItemInTab(oid));

      if(item)
	item->updateRow(index);
      else
	foreach(auto w, QApplication::topLevelWidgets())
	  {
	    auto cd = qobject_cast<biblioteq_cd *> (w);

	    if(cd && cd->getID() == oid)
	      {
		cd->updateRow(index);
		break;
	      }
	  }
    }
  else if(itemType == "dvd")
    {
      auto item = dynamic_cast<biblioteq_dvd *> (findItemInTab(oid));

      if(item)
	item->updateRow(index);
      else
	foreach(auto w, QApplication::topLevelWidgets())
	  {
	    auto dvd = qobject_cast<biblioteq_dvd *> (w);

	    if(dvd && dvd->getID() == oid)
	      {
		dvd->updateRow(index);
		break;
	      }
	  }
    }
  else if(itemType == "greyliterature")
    {
      auto item = dynamic_cast<biblioteq_grey_literature *>
	(findItemInTab(oid));

      if(item)
	item->updateRow(index);
      else
	foreach(auto w, QApplication::topLevelWidgets())
	  {
	    auto gl = qobject_cast<biblioteq_grey_literature *> (w);

	    if(gl && gl->getID() == oid)
	      {
		gl->updateRow(index);
		break;
	      }
	  }
    }
  else if(itemType == "journal")
    {
      auto item = dynamic_cast<biblioteq_journal *> (findItemInTab(oid));

      if(item)
	item->updateRow(index);
      else
	foreach(auto w, QApplication::topLevelWidgets())
	  {
	    auto journal = qobject_cast<biblioteq_journal *> (w);

	    if(journal && journal->getID() == oid)
	      {
		journal->updateRow(index);
		break;
	      }
	  }
    }
  else if(itemType == "magazine")
    {
      auto item = dynamic_cast<biblioteq_magazine *> (findItemInTab(oid));

      if(item)
	item->updateRow(index);
      else
	foreach(auto w, QApplication::topLevelWidgets())
	  {
	    auto magazine = qobject_cast<biblioteq_magazine *> (w);

	    /*
	    ** The class biblioteq_journal inherits biblioteq_magazine.
	    */

	    if(!qobject_cast<biblioteq_journal *> (w))
	      if(magazine && magazine->getID() == oid)
		{
		  magazine->updateRow(index);
		  break;
		}
	  }
    }
  else if(itemType == "photographcollection")
    {
      auto item = dynamic_cast<biblioteq_photographcollection *>
	(findItemInTab(oid));

      if(item)
	item->updateRow(index);
      else
	foreach(auto w, QApplication::topLevelWidgets())
	  {
	    auto photograph = qobject_cast<biblioteq_photographcollection *>
	      (w);

	    if(photograph && photograph->getID() == oid)
	      {
		photograph->updateRow(index);
		break;
	      }
	  }
    }
  else if(itemType == "videogame")
    {
      auto item = dynamic_cast<biblioteq_videogame *> (findItemInTab(oid));

      if(item)
	item->updateRow(index);
      else
	foreach(auto w, QApplication::topLevelWidgets())
	  {
	    auto videogame = qobject_cast<biblioteq_videogame *> (w);

	    if(videogame && videogame->getID() == oid)
	      {
		videogame->updateRow(index);
		break;
	      }
	  }
    }
}

void biblioteq::updateMembersBrowser(const QString &memberid)
{
  /*
  ** Called from the Borrowers Editor when an item has been updated.
  */

  QApplication::setOverrideCursor(Qt::WaitCursor);

  QString errorstr = "";
  auto const counts = biblioteq_misc_functions::getItemsReservedCounts
    (m_db, memberid, errorstr);

  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    addError(tr("Database Error"),
	     tr("Unable to retrieve the number of reserved items "
		"of the selected member."),
	     errorstr,
	     __FILE__,
	     __LINE__);
  else
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);

      for(int i = 0; i < bb.table->rowCount(); i++)
	{
	  auto const str = biblioteq_misc_functions::getColumnString
	    (bb.table, i, m_bbColumnHeaderIndexes.indexOf("Member ID"));

	  if(memberid == str)
	    {
	      biblioteq_misc_functions::updateColumn
		(bb.table,
		 i,
		 m_bbColumnHeaderIndexes.indexOf("Books Reserved"),
		 QString::number(counts.value("numbooks")));
	      biblioteq_misc_functions::updateColumn
		(bb.table,
		 i,
		 m_bbColumnHeaderIndexes.indexOf("CDs Reserved"),
		 QString::number(counts.value("numcds")));
	      biblioteq_misc_functions::updateColumn
		(bb.table,
		 i,
		 m_bbColumnHeaderIndexes.indexOf("DVDs Reserved"),
		 QString::number(counts.value("numdvds")));
	      biblioteq_misc_functions::updateColumn
		(bb.table,
		 i,
		 m_bbColumnHeaderIndexes.indexOf("Grey Literatures Reserved"),
		 QString::number(counts.value("numgreyliteratures")));
	      biblioteq_misc_functions::updateColumn
		(bb.table,
		 i,
		 m_bbColumnHeaderIndexes.indexOf("Journals Reserved"),
		 QString::number(counts.value("numjournals")));
	      biblioteq_misc_functions::updateColumn
		(bb.table,
		 i,
		 m_bbColumnHeaderIndexes.indexOf("Magazines Reserved"),
		 QString::number(counts.value("nummagazines")));
	      biblioteq_misc_functions::updateColumn
		(bb.table,
		 i,
		 m_bbColumnHeaderIndexes.indexOf("Total Reserved"),
		 QString::number(counts.value("numtotal")));
	      biblioteq_misc_functions::updateColumn
		(bb.table,
		 i,
		 m_bbColumnHeaderIndexes.indexOf("Video Games Reserved"),
		 QString::number(counts.value("numvideogames")));
	      break;
	    }
	}

      QApplication::restoreOverrideCursor();
    }
}

void biblioteq::updateMembersBrowser(void)
{
  int row = 0;

  /*
  ** Called from the Copy Editor when an item has been reserved.
  */

  if((row = bb.table->currentRow()) < 0)
    return;

  auto const memberid = biblioteq_misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("Member ID"));

  QApplication::setOverrideCursor(Qt::WaitCursor);

  QString errorstr = "";
  auto const counts = biblioteq_misc_functions::getItemsReservedCounts
    (m_db, memberid, errorstr);

  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    addError(tr("Database Error"),
	     tr("Unable to determine the number of reserved items "
		"for the selected member."),
	     errorstr,
	     __FILE__,
	     __LINE__);
  else
    {
      biblioteq_misc_functions::updateColumn
	(bb.table,
	 row,
	 m_bbColumnHeaderIndexes.indexOf("Books Reserved"),
	 QString::number(counts.value("numbooks")));
      biblioteq_misc_functions::updateColumn
	(bb.table,
	 row,
	 m_bbColumnHeaderIndexes.indexOf("CDs Reserved"),
	 QString::number(counts.value("numcds")));
      biblioteq_misc_functions::updateColumn
	(bb.table,
	 row,
	 m_bbColumnHeaderIndexes.indexOf("DVDs Reserved"),
	 QString::number(counts.value("numdvds")));
      biblioteq_misc_functions::updateColumn
	(bb.table,
	 row,
	 m_bbColumnHeaderIndexes.indexOf("Grey Literatures Reserved"),
	 QString::number(counts.value("numgreyliteratures")));
      biblioteq_misc_functions::updateColumn
	(bb.table,
	 row,
	 m_bbColumnHeaderIndexes.indexOf("Journals Reserved"),
	 QString::number(counts.value("numjournals")));
      biblioteq_misc_functions::updateColumn
	(bb.table,
	 row,
	 m_bbColumnHeaderIndexes.indexOf("Magazines Reserved"),
	 QString::number(counts.value("nummagazines")));
      biblioteq_misc_functions::updateColumn
	(bb.table,
	 row,
	 m_bbColumnHeaderIndexes.indexOf("Total Reserved"),
	 QString::number(counts.value("numtotal")));
      biblioteq_misc_functions::updateColumn
	(bb.table,
	 row,
	 m_bbColumnHeaderIndexes.indexOf("Video Games Reserved"),
	 QString::number(counts.value("numvideogames")));

      if(m_history_diag->isVisible())
	slotShowHistory();
    }
}

void biblioteq::updateReservationHistoryBrowser(const QString &memberid,
						const QString &ioid,
						const QString &copyid,
						const QString &itemType,
						const QString &returnedDate)
{
  /*
  ** Called from the Borrowers Editor when an item has been updated.
  */

  if(biblioteq_misc_functions::
     getColumnString(history.table,
		     0,
		     m_historyColumnHeaderIndexes.
		     indexOf("Member ID")) == memberid &&
     history.table->rowCount() > 0 &&
     m_history_diag->isVisible())
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);

      for(int i = 0; i < history.table->rowCount(); i++)
	{
	  auto const barcode = biblioteq_misc_functions::getColumnString
	    (history.table,
	     i,
	     m_historyColumnHeaderIndexes.indexOf("Barcode"));
	  auto const myoid = biblioteq_misc_functions::getColumnString
	    (history.table,
	     i,
	     m_historyColumnHeaderIndexes.indexOf("MYOID"));
	  auto const type = biblioteq_misc_functions::getColumnString
	    (history.table,
	     i,
	     m_historyColumnHeaderIndexes.indexOf("Type")).
	    toLower().remove(" ");

	  if(barcode == copyid && ioid == myoid && itemType == type)
	    {
	      auto const date
		(QDate::fromString(returnedDate, s_databaseDateFormat));

	      biblioteq_misc_functions::updateColumn
		(history.table,
		 i,
		 m_historyColumnHeaderIndexes.indexOf("Returned Date"),
		 date.toString(Qt::ISODate));
	      break;
	    }
	}

      QApplication::restoreOverrideCursor();
    }
}

void biblioteq::updateSceneItem(const QString &oid,
				const QString &type,
				const QImage &image)
{
  QGraphicsPixmapItem *item = nullptr;
  auto const items(ui.graphicsView->scene()->items());

  for(int i = 0; i < items.size(); i++)
    if((item = qgraphicsitem_cast<QGraphicsPixmapItem *> (items.at(i))))
      if(item->data(0).toString() == oid && item->data(1).toString() == type)
	{
	  auto l_image(image);

	  if(!l_image.isNull())
	    l_image = l_image.scaled
	      (s_noImageResize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	  auto const pixmap(QPixmap::fromImage(l_image));

	  if(!pixmap.isNull())
	    item->setPixmap(pixmap);
	  else
	    {
	      QImage l_image(":/missing_image.png");

	      if(!l_image.isNull())
		l_image = l_image.scaled
		  (s_noImageResize,
		   Qt::KeepAspectRatio,
		   Qt::SmoothTransformation);

	      item->setPixmap(QPixmap::fromImage(l_image));
	    }

	  break;
	}
}
