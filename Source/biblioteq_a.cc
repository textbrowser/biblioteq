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

#include <QClipboard>
#include <QFileDialog>
#include <QFontDialog>
#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
#include <QMacStyle>
#endif
#endif
#include <QScrollBar>
#include <QSettings>
#include <QTranslator>
#include <QtDebug>

#include <limits>

#ifdef Q_OS_MAC
#if QT_VERSION >= 0x050000
#include "CocoaInitializer.h"
#endif
#endif

#ifdef BIBLIOTEQ_POPPLER_VERSION_DEFINED
#include <poppler-version.h>
#endif

extern "C"
{
#include <math.h>
#ifdef Q_OS_WIN32
#include <sqlite3/sqlite3.h>
#else
#include <sqlite3.h>
#endif
#include <yaz/yaz-version.h>
}

#include "biblioteq.h"
#include "biblioteq_architecture.h"
#include "biblioteq_bgraphicsscene.h"
#include "biblioteq_otheroptions.h"
#include "biblioteq_sqlite_create_schema.h"

/*
** -- Global Variables --
*/

QString biblioteq::s_locale = "";
QTranslator *biblioteq::s_appTranslator = 0;
QTranslator *biblioteq::s_qtTranslator = 0;

int main(int argc, char *argv[])
{
#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  QMacStyle *style = new(std::nothrow) QMacStyle();

  if(style)
    QApplication::setStyle(style);
#endif
#endif

  QApplication qapplication(argc, argv);
  QFont font(qapplication.font());

#if QT_VERSION >= 0x050700
  qapplication.setAttribute(Qt::AA_DontUseNativeDialogs);
#endif
  font.setStyleStrategy
    (QFont::StyleStrategy(QFont::PreferAntialias | QFont::PreferQuality));
  qapplication.setFont(font);

#ifdef Q_OS_MAC
#if QT_VERSION >= 0x050000
  /*
  ** Eliminate warnings.
  */

  CocoaInitializer ci;
#endif
#endif

  qapplication.setWindowIcon(QIcon(":/book.png"));

  /*
  ** Prepare configuration settings.
  */

  QCoreApplication::setOrganizationName("BiblioteQ");
  QCoreApplication::setOrganizationDomain("biblioteq.sourceforge.net");
  QCoreApplication::setApplicationName("BiblioteQ");
  QSettings::setPath(QSettings::IniFormat,
		     QSettings::UserScope,
		     biblioteq::homePath());
  QSettings::setDefaultFormat(QSettings::IniFormat);
  QDir().mkdir(biblioteq::homePath());

  /*
  ** Remove old configuration settings.
  */

  QSettings settings;

  settings.remove("sqlite_db");
  settings.remove("entries_per_page");
  settings.remove("automatically_resize_columns");

  if(!settings.contains("column_settings_cleared_v6_51"))
    {
      settings.setValue("column_settings_cleared_v6_51", true);

      for(int i = settings.allKeys().size() - 1; i >= 0; i--)
	if(settings.allKeys().at(i).contains("_header_state"))
	  settings.remove(settings.allKeys().at(i));
    }

  if((biblioteq::s_qtTranslator = new(std::nothrow) QTranslator(0)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  if((biblioteq::s_appTranslator = new(std::nothrow) QTranslator(0)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  biblioteq::s_locale = settings.value("locale").toString();

  if(!(biblioteq::s_locale == "ar_JO" ||
       biblioteq::s_locale == "cs_CZ" ||
       biblioteq::s_locale == "de_DE" ||
       biblioteq::s_locale == "el_GR" ||
       biblioteq::s_locale == "en_US" ||
       biblioteq::s_locale == "fr_FR" ||
       biblioteq::s_locale == "hu_HU" ||
       biblioteq::s_locale == "nl_BE" ||
       biblioteq::s_locale == "nl_NL" ||
       biblioteq::s_locale == "ru_RU"))
    biblioteq::s_locale = QLocale::system().name();

#ifdef Q_OS_MAC
  biblioteq::s_qtTranslator->load
    ("qt_" + biblioteq::s_locale,
     QCoreApplication::applicationDirPath() + "/../../../Translations");
#else
  biblioteq::s_qtTranslator->load("qt_" + biblioteq::s_locale, "Translations");
#endif
  qapplication.installTranslator(biblioteq::s_qtTranslator);
#ifdef Q_OS_MAC
  biblioteq::s_appTranslator->load
    ("biblioteq_" + biblioteq::s_locale,
     QCoreApplication::applicationDirPath() + "/../../../Translations");
#else
  biblioteq::s_appTranslator->load("biblioteq_" + biblioteq::s_locale,
				   "Translations");
#endif
  qapplication.installTranslator(biblioteq::s_appTranslator);

  biblioteq biblioteq;

  biblioteq.showMain();

  /*
  ** Enter an endless loop.
  */

  return qapplication.exec();
}

biblioteq::biblioteq(void):QMainWindow()
{
#if QT_VERSION < 0x050000
#ifdef Q_OS_MAC
  if(QSysInfo::MacintoshVersion > QSysInfo::MV_10_6)
    {
      QNetworkAccessManager manager;
      QNetworkReply *reply = 0;

      /*
      ** We need to perform this before a PostgreSQL database connection is
      ** established. Otherwise, the connection may be severed later.
      ** The anomaly occurs on OS X 10.7.4 and Qt 4.8.6.
      ** It may also occur on OS X 10.7.5 and Qt 4.8.7.
      */

      reply = manager.get
	(QNetworkRequest(QUrl::fromUserInput("http://0.0.0.0")));
      reply->deleteLater();
    }
#endif
#endif

  QMenu *menu1 = 0;

  ui.setupUi(this);
  ui.table->setQMain(this);

  if(menuBar())
    menuBar()->setNativeMenuBar(true);

  m_connected_bar_label = 0;
  m_error_bar_label = 0;
  m_idCt = 0;
  m_lastSearchType = POPULATE_ALL;
  m_previousTypeFilter = "";
  m_status_bar_label = 0;

  if((m_branch_diag = new(std::nothrow) QDialog(this)) == 0)
    quit("Memory allocation failure", __FILE__, __LINE__);

  if((m_menuCategoryActionGroup = new(std::nothrow) QActionGroup(this)) == 0)
    quit("Memory allocation failure", __FILE__, __LINE__);
  else
    m_menuCategoryActionGroup->setExclusive(true);

  if((m_otheroptions = new(std::nothrow) biblioteq_otheroptions(this)) == 0)
    quit("Memory allocation failure", __FILE__, __LINE__);

  if((m_pass_diag = new(std::nothrow) QDialog(this)) == 0)
    quit("Memory allocation failure", __FILE__, __LINE__);

  if((m_all_diag = new(std::nothrow) QMainWindow()) == 0)
    quit("Memory allocation failure", __FILE__, __LINE__);

  if((m_admin_diag = new(std::nothrow) QMainWindow()) == 0)
    quit("Memory allocation failure", __FILE__, __LINE__);

  if((m_members_diag = new(std::nothrow) QMainWindow()) == 0)
    quit("Memory allocation failure", __FILE__, __LINE__);

  if((m_history_diag = new(std::nothrow) QMainWindow()) == 0)
    quit("Memory allocation failure", __FILE__, __LINE__);

  if((m_customquery_diag = new(std::nothrow) QMainWindow()) == 0)
    quit("Memory allocation failure", __FILE__, __LINE__);

  if((userinfo_diag =
      new(std::nothrow) userinfo_diag_class(m_members_diag)) == 0)
    quit("Memory allocation failure", __FILE__, __LINE__);

  if((m_error_diag = new(std::nothrow) QMainWindow()) == 0)
    quit("Memory allocation failure", __FILE__, __LINE__);

  if((db_enumerations = new(std::nothrow) biblioteq_dbenumerations(this)) == 0)
    quit("Memory allocation failure", __FILE__, __LINE__);

  if((m_configToolMenu = new(std::nothrow) QMenu(this)) == 0)
    quit("Memory allocation failure", __FILE__, __LINE__);

  if((m_import = new(std::nothrow) biblioteq_import(this)) == 0)
    quit("Memory allocation failure", __FILE__, __LINE__);

  if((menu1 = new(std::nothrow) QMenu(this)) == 0)
    quit("Memory allocation failure", __FILE__, __LINE__);

  m_configToolMenu->setTearOffEnabled(true);
  m_configToolMenu->setWindowIcon(QIcon(":/book.png"));
  m_configToolMenu->setWindowTitle(tr("BiblioteQ"));
  connect(QCoreApplication::instance(),
	  SIGNAL(lastWindowClosed(void)),
	  this,
	  SLOT(slotLastWindowClosed(void)));
  connect(menu1->addAction(tr("Reset ID Number")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Title")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Publication Date")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Publisher")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Categories")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Price")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Language")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Monetary Units")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Abstract")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Copies")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Location")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Keywords")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Availability")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Case-Insensitive")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
#ifndef BIBLIOTEQ_LINKED_WITH_POPPLER
  ui.action_Open_PDF_File->setEnabled(false);
#endif
  connect(ui.action_Book,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotInsertBook(void)));
  connect(ui.actionGrey_Literature,
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
	  SLOT(slotOpenPDFFile(void)));
  connect(ui.actionImportCSV,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowImport(void)));
  connect(ui.actionOther_Options,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowOtherOptions(void)));
  connect(ui.actionMusic_CD,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotInsertCD(void)));
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
  connect(ui.action_Photograph_Collection,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotInsertPhotograph(void)));
  connect(ui.action_Upgrade_SQLite_Schema,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotUpgradeSqliteScheme(void)));
  connect(ui.action_Video_Game,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotInsertVideoGame(void)));
  connect(ui.resetAllSearch,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotResetAllSearch(void)));

  biblioteq_bgraphicsscene *scene = 0;

  if((scene = new(std::nothrow)
      biblioteq_bgraphicsscene(ui.graphicsView)) == 0)
    quit("Memory allocation failure", __FILE__, __LINE__);

  connect(scene,
	  SIGNAL(selectionChanged(void)),
	  this,
	  SLOT(slotSceneSelectionChanged(void)));
  ui.graphicsView->setScene(scene);
  ui.graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
  ui.graphicsView->setRubberBandSelectionMode(Qt::IntersectsItemShape);
  bb.setupUi(m_members_diag);
  history.setupUi(m_history_diag);
  br.setupUi(m_branch_diag);
  pass.setupUi(m_pass_diag);
  al.setupUi(m_all_diag);
  cq.setupUi(m_customquery_diag);
  er.setupUi(m_error_diag);
  ab.setupUi(m_admin_diag);
  ab.splitter->setStretchFactor(0, 0);
  ab.splitter->setStretchFactor(1, 1);
#ifdef Q_OS_MAC
  ui.actionSetGlobalFonts->setVisible(false);
#endif
  m_pass_diag->setModal(true);
  userinfo_diag->setModal(true);

  /*
  ** Please read https://stackoverflow.com/questions/201323/how-to-validate-an-email-address-using-a-regular-expression.
  */

  QRegExp rx
    ("^[-!#$%&'*+/0-9=?A-Z^_a-z{|}~](\\.?[-!#$%&'*+/0-9=?A-Z^_a-z{|}~])*"
     "@[a-zA-Z](-?[a-zA-Z0-9])*(\\.[a-zA-Z](-?[a-zA-Z0-9])*)+$",
     Qt::CaseInsensitive);

  rx.setPatternSyntax(QRegExp::RegExp);
  userinfo_diag->m_userinfo.email->setValidator(new QRegExpValidator(rx, this));
  m_branch_diag->setModal(true);
  connect(ui.table->horizontalHeader(), SIGNAL(sectionPressed(int)),
	  this, SLOT(slotResizeColumnsAfterSort(void)));
  connect(ui.table->horizontalHeader(), SIGNAL(sectionClicked(int)),
	  this, SLOT(slotUpdateIndicesAfterSort(int)));
  connect(ui.table->horizontalHeader(), SIGNAL(sectionResized(int, int, int)),
	  this, SLOT(slotSectionResized(int, int, int)));
  connect(er.table->horizontalHeader(), SIGNAL(sectionClicked(int)),
	  this, SLOT(slotResizeColumnsAfterSort(void)));
  connect(er.copyButton, SIGNAL(clicked(void)), this,
	  SLOT(slotCopyError(void)));
  connect(bb.action_Export,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotExportMembersAsCSV(void)));
  connect(bb.table->horizontalHeader(), SIGNAL(sectionClicked(int)),
	  this, SLOT(slotResizeColumnsAfterSort(void)));
  connect(history.table->horizontalHeader(), SIGNAL(sectionClicked(int)),
	  this, SLOT(slotResizeColumnsAfterSort(void)));
  connect(ui.table, SIGNAL(itemSelectionChanged(void)), this,
	  SLOT(slotDisplaySummary(void)));
  connect(bb.modifyButton, SIGNAL(clicked(void)), this,
	  SLOT(slotModifyBorrower(void)));
  connect(bb.historyButton, SIGNAL(clicked(void)), this,
	  SLOT(slotShowHistory(void)));
  connect(history.reloadButton, SIGNAL(clicked(void)), this,
	  SLOT(slotShowHistory(void)));
  connect(history.printButton, SIGNAL(clicked(void)), this,
	  SLOT(slotPrintReservationHistory(void)));
  connect(al.okButton, SIGNAL(clicked(void)), this, SLOT(slotAllGo(void)));
  connect(ui.exitTool, SIGNAL(triggered(void)), this, SLOT(slotExit(void)));
  connect(ui.actionExit, SIGNAL(triggered(void)), this,
	  SLOT(slotExit(void)));
  connect(ui.actionSetGlobalFonts, SIGNAL(triggered(void)), this,
	  SLOT(slotSetFonts(void)));
  connect(ui.deleteTool, SIGNAL(triggered(void)), this,
	  SLOT(slotDelete(void)));
  connect(ui.duplicateTool, SIGNAL(triggered(void)), this,
	  SLOT(slotDuplicate(void)));
  connect(ui.actionDeleteEntry, SIGNAL(triggered(void)), this,
	  SLOT(slotDelete(void)));
  connect(ui.actionDuplicateEntry, SIGNAL(triggered(void)), this,
	  SLOT(slotDuplicate(void)));
  connect(ui.refreshTool, SIGNAL(triggered(void)), this,
	  SLOT(slotRefresh(void)));
  connect(ui.actionRefreshTable, SIGNAL(triggered(void)), this,
	  SLOT(slotRefresh(void)));
  connect(ui.actionReload_biblioteq_conf, SIGNAL(triggered(void)), this,
	  SLOT(slotReloadBiblioteqConf(void)));
  connect(ui.menu_Category, SIGNAL(triggered(QAction *)), this,
	  SLOT(slotAutoPopOnFilter(QAction *)));
  connect(ui.modifyTool, SIGNAL(triggered(void)), this,
	  SLOT(slotModify(void)));
  connect(ui.actionModifyEntry, SIGNAL(triggered(void)), this,
	  SLOT(slotModify(void)));
  connect(ui.actionShowErrorDialog, SIGNAL(triggered(void)), this,
	  SLOT(slotShowErrorDialog(void)));
  connect(ui.actionAbout, SIGNAL(triggered(void)), this,
	  SLOT(slotAbout(void)));
  connect(ui.actionShowGrid, SIGNAL(triggered(void)), this,
	  SLOT(slotShowGrid(void)));
  connect(ui.actionResizeColumns, SIGNAL(triggered(void)), this,
	  SLOT(slotResizeColumns(void)));
  connect(ui.userTool, SIGNAL(triggered(void)), this,
	  SLOT(slotShowMembersBrowser(void)));
  connect(ui.reserveTool, SIGNAL(triggered(void)), this,
	  SLOT(slotReserveCopy(void)));
  connect(ui.actionMembersBrowser, SIGNAL(triggered(void)), this,
	  SLOT(slotShowMembersBrowser(void)));
  connect(userinfo_diag->m_userinfo.allow_any_email, SIGNAL(toggled(bool)),
	  this, SLOT(slotAllowAnyUserEmail(bool)));
  connect(userinfo_diag->m_userinfo.okButton, SIGNAL(clicked(void)), this,
	  SLOT(slotSaveUser(void)));
  connect(ui.actionChangePassword, SIGNAL(triggered(void)), this,
	  SLOT(slotShowChangePassword(void)));
  connect(ui.actionSaveSettings, SIGNAL(triggered(void)), this,
	  SLOT(slotSaveConfig()));
  connect(ui.actionRequests, SIGNAL(triggered(void)), this,
	  SLOT(slotRequest(void)));
  connect(ui.connectTool, SIGNAL(triggered(void)), this,
	  SLOT(slotShowConnectionDB(void)));
  connect(ui.actionConnect, SIGNAL(triggered(void)), this,
	  SLOT(slotShowConnectionDB(void)));
  connect(ui.disconnectTool, SIGNAL(triggered(void)), this,
	  SLOT(slotDisconnect(void)));
  connect(ui.actionDisconnect, SIGNAL(triggered(void)), this,
	  SLOT(slotDisconnect(void)));
  connect(userinfo_diag->m_userinfo.nextTool, SIGNAL(clicked(void)), this,
	  SLOT(slotShowNext(void)));
  connect(userinfo_diag->m_userinfo.prevTool, SIGNAL(clicked(void)), this,
	  SLOT(slotShowPrev(void)));
  connect(history.nextTool, SIGNAL(clicked(void)), this,
	  SLOT(slotShowNext(void)));
  connect(history.prevTool, SIGNAL(clicked(void)), this,
	  SLOT(slotShowPrev(void)));
  connect(history.cancelButton, SIGNAL(clicked(void)),
	  m_history_diag, SLOT(close(void)));
  connect(history.dnt, SIGNAL(toggled(bool)),
	  this, SLOT(slotSaveDnt(bool)));
  connect(br.okButton, SIGNAL(clicked(void)), this,
	  SLOT(slotConnectDB(void)));
  connect(br.branch_name, SIGNAL(activated(int)), this,
	  SLOT(slotBranchChanged(void)));
  connect(br.role, SIGNAL(currentIndexChanged(int)), this,
	  SLOT(slotRoleChanged(int)));
  connect(bb.printButton, SIGNAL(clicked(void)), this,
	  SLOT(slotPrintReserved(void)));
  connect(bb.addButton, SIGNAL(clicked(void)), this,
	  SLOT(slotAddBorrower(void)));
  connect(bb.reloadButton, SIGNAL(clicked(void)), this,
	  SLOT(slotPopulateMembersBrowser(void)));
  connect(bb.deleteButton, SIGNAL(clicked(void)), this,
	  SLOT(slotRemoveMember(void)));
  connect(userinfo_diag->m_userinfo.cancelButton, SIGNAL(clicked(void)), this,
	  SLOT(slotCancelAddUser(void)));
  connect(bb.cancelButton, SIGNAL(clicked(void)), this,
	  SLOT(slotCloseMembersBrowser(void)));
  connect(bb.checkoutButton, SIGNAL(clicked(void)), this,
	  SLOT(slotCheckout(void)));
  connect(bb.listButton, SIGNAL(clicked(void)), this,
	  SLOT(slotListReservedItems(void)));
  connect(bb.overdueButton, SIGNAL(clicked(void)), this,
	  SLOT(slotListOverdueItems(void)));
  connect(al.resetButton, SIGNAL(clicked(void)), this,
	  SLOT(slotSearch(void)));
  connect(al.cancelButton, SIGNAL(clicked(void)),
	  m_all_diag, SLOT(close(void)));
  connect(ui.actionReservationHistory, SIGNAL(triggered(void)), this,
	  SLOT(slotShowHistory(void)));
  connect(ui.searchTool, SIGNAL(triggered(void)), this,
	  SLOT(slotShowMenu(void)));
  connect(ui.customQueryTool, SIGNAL(triggered(void)), this,
	  SLOT(slotShowCustomQuery(void)));
  connect(ui.actionDatabaseSearch, SIGNAL(triggered(void)), this,
	  SLOT(slotSearch(void)));
  connect(ui.actionViewDetails, SIGNAL(triggered(void)), this,
	  SLOT(slotViewDetails(void)));
  connect(ui.detailsTool, SIGNAL(triggered(void)), this,
	  SLOT(slotViewDetails(void)));
  connect(ui.createTool, SIGNAL(triggered(void)), this,
	  SLOT(slotShowMenu(void)));
  connect(ui.search, SIGNAL(returnPressed(void)), this,
	  SLOT(slotSearchBasic(void)));
  connect(er.resetButton, SIGNAL(clicked(void)), this,
	  SLOT(slotResetErrorLog(void)));
  connect(er.cancelButton, SIGNAL(clicked(void)),
	  m_error_diag, SLOT(close(void)));
  connect(bb.filter, SIGNAL(returnPressed(void)), this,
	  SLOT(slotPopulateMembersBrowser(void)));
  connect(bb.grantButton, SIGNAL(clicked(void)), this,
	  SLOT(slotGrantPrivileges(void)));
  connect(ui.configTool, SIGNAL(triggered(void)), this,
	  SLOT(slotShowMenu(void)));
  connect(ui.printTool, SIGNAL(triggered(void)), this,
	  SLOT(slotPrintView(void)));
  connect(ui.previousPageButton, SIGNAL(clicked(void)), this,
	  SLOT(slotPreviousPage(void)));
  connect(ui.nextPageButton, SIGNAL(clicked(void)), this,
	  SLOT(slotNextPage(void)));
  connect(ui.pagesLabel, SIGNAL(linkActivated(const QString &)),
	  this, SLOT(slotPageClicked(const QString &)));
  connect(cq.close_pb, SIGNAL(clicked(void)), this,
	  SLOT(slotCloseCustomQueryDialog(void)));
  connect(cq.execute_pb, SIGNAL(clicked(void)), this,
	  SLOT(slotExecuteCustomQuery(void)));
  connect(cq.refresh_pb, SIGNAL(clicked(void)), this,
	  SLOT(slotRefreshCustomQuery(void)));
  connect(pass.okButton, SIGNAL(clicked(void)), this,
	  SLOT(slotSavePassword(void)));
  connect(pass.cancelButton, SIGNAL(clicked(void)), this,
	  SLOT(slotClosePasswordDialog(void)));
  connect(m_otheroptions,
	  SIGNAL(mainWindowCanvasBackgroundColorChanged(const QColor &)),
	  this,
	  SLOT(slotMainWindowCanvasBackgroundColorChanged(const QColor &)));
  connect(m_otheroptions,
	  SIGNAL(mainWindowCanvasBackgroundColorPreview(const QColor &)),
	  this,
	  SLOT(slotPreviewCanvasBackgroundColor(const QColor &)));
  connect(m_otheroptions,
	  SIGNAL(saved(void)),
	  this,
	  SLOT(slotOtherOptionsSaved(void)));
  connect(m_pass_diag, SIGNAL(finished(int)), this,
	  SLOT(slotClosePasswordDialog(void)));
  connect(br.resetButton, SIGNAL(clicked(void)), this,
	  SLOT(slotResetLoginDialog(void)));
  connect(br.fileButton, SIGNAL(clicked(void)), this,
	  SLOT(slotSelectDatabaseFile(void)));
  connect(br.cancelButton, SIGNAL(clicked(void)),
	  m_branch_diag, SLOT(close(void)));
  connect(ui.actionConfigureAdministratorPrivileges,
	  SIGNAL(triggered(void)), this, SLOT(slotShowAdminDialog(void)));
  connect(ab.reloadButton, SIGNAL(clicked(void)), this,
	  SLOT(slotRefreshAdminList(void)));
  connect(ab.addButton, SIGNAL(clicked(void)), this,
	  SLOT(slotAddAdmin(void)));
  connect(ab.deleteButton, SIGNAL(clicked(void)), this,
	  SLOT(slotDeleteAdmin(void)));
  connect(ab.saveButton, SIGNAL(clicked(void)), this,
	  SLOT(slotSaveAdministrators(void)));
  connect(ab.cancelButton, SIGNAL(clicked(void)),
	  m_admin_diag, SLOT(close(void)));
  connect(ui.action_New_SQLite_Database,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotDisplayNewSqliteDialog(void)));
  connect(ui.actionDatabase_Enumerations,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowDbEnumerations(void)));
  connect(ui.actionExport_Current_View,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotExportAsCSV(void)));
#if QT_VERSION >= 0x050000
  ab.table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  bb.table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  er.table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  history.table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
  ab.table->verticalHeader()->setResizeMode(QHeaderView::Fixed);
  bb.table->verticalHeader()->setResizeMode(QHeaderView::Fixed);
  er.table->verticalHeader()->setResizeMode(QHeaderView::Fixed);
  history.table->verticalHeader()->setResizeMode(QHeaderView::Fixed);
#endif
  al.publication_date_enabled->setChecked(false);
  al.resetButton->setMenu(menu1);

#ifdef Q_OS_MAC
  foreach(QToolButton *tool_button, m_all_diag->findChildren<QToolButton *> ())
#if (QT_VERSION < QT_VERSION_CHECK(5, 10, 0))
    tool_button->setStyleSheet
    ("QToolButton {border: none; padding-right: 10px}"
     "QToolButton::menu-button {border: none;}");
#else
    tool_button->setStyleSheet
      ("QToolButton {border: none; padding-right: 15px}"
       "QToolButton::menu-button {border: none; width: 15px;}");
#endif
#endif

  ui.actionImportCSV->setEnabled(false);
  ui.previousPageButton->setEnabled(false);
  ui.nextPageButton->setEnabled(false);
  ui.actionRequests->setEnabled(false);
  ui.actionReservationHistory->setEnabled(false);
  ui.actionChangePassword->setEnabled(false);
  ui.printTool->setEnabled(false);
  ui.deleteTool->setEnabled(false);
  ui.menu_Add_Item->setEnabled(false);
  ui.actionDeleteEntry->setEnabled(false);
  ui.actionDuplicateEntry->setEnabled(false);
  ui.createTool->setEnabled(false);
  ui.duplicateTool->setEnabled(false);
  ui.modifyTool->setEnabled(false);
  ui.detailsTool->setEnabled(false);
  ui.actionModifyEntry->setEnabled(false);
  ui.searchTool->setEnabled(false);
  ui.customQueryTool->setEnabled(false);
  ui.actionDatabaseSearch->setEnabled(false);
  ui.actionViewDetails->setEnabled(false);
  ui.refreshTool->setEnabled(false);
  ui.actionRefreshTable->setEnabled(false);
  ui.disconnectTool->setEnabled(false);
  ui.actionDisconnect->setEnabled(false);
  ui.userTool->setEnabled(false);
  ui.reserveTool->setEnabled(false);
  ui.actionMembersBrowser->setEnabled(false);
  ui.configTool->setEnabled(false);
  ui.actionAutoPopulateOnCreation->setEnabled(false);
  ui.actionPopulate_Administrator_Browser_Table_on_Display->setEnabled(false);
  ui.actionPopulate_Members_Browser_Table_on_Display->setEnabled(false);
  ui.actionDatabase_Enumerations->setEnabled(false);
  ui.actionPopulate_Database_Enumerations_Browser_on_Display->setEnabled
    (false);
  ui.action_Upgrade_SQLite_Schema->setEnabled(false);

  QString typefilter("");
  QSettings settings;

  typefilter = m_lastCategory =
    settings.value("last_category", "All").toString();
  typefilter.replace(" ", "_");
  ui.actionConfigureAdministratorPrivileges->setEnabled(false);
  ui.graphicsView->scene()->clear();
  ui.summary->setVisible(false);
  ui.table->resetTable(dbUserName(), m_lastCategory, m_roles);
  ui.table->setIconSize(QSize(64, 94));
  m_previousTypeFilter = m_lastCategory;
  prepareFilter();

  bool found = false;

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

  QActionGroup *group1 = 0;
  int end = 21;

  if((group1 = new(std::nothrow) QActionGroup(this)) == 0)
    quit("Memory allocation failure", __FILE__, __LINE__);

  for(int i = 1; i <= end; i++)
    {
      QAction *action = 0;

      if(i == end)
	action = group1->addAction
	  (QString(tr("&Unlimited Entries per Page")));
      else
	action = group1->addAction(QString(tr("&%1")).arg(5 * i));

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

  QAction *action = 0;
  QActionGroup *group2 = 0;

  if((group2 = new(std::nothrow) QActionGroup(this)) == 0)
    quit("Memory allocation failure", __FILE__, __LINE__);

  group2->setObjectName("ViewModeMenu");
  group2->setExclusive(true);
  (action = group2->addAction(tr("Icons Mode")))->setCheckable(true);
  action->setData(0);
  connect(action,
	  SIGNAL(triggered(bool)),
	  this,
	  SLOT(slotChangeView(bool)));
  ui.menu_View->addAction(action);
  (action = group2->addAction(tr("Table Mode")))->setCheckable(true);
  action->setData(1);
  action->setChecked(true);
  connect(action,
	  SIGNAL(triggered(bool)),
	  this,
	  SLOT(slotChangeView(bool)));
  ui.menu_View->addAction(action);

  QActionGroup *group3 = 0;

  if((group3 = new(std::nothrow) QActionGroup(this)) == 0)
    quit("Memory allocation failure", __FILE__, __LINE__);

  group3->setExclusive(true);
  (action = group3->addAction(tr("&Arabic")))->setCheckable(true);
  action->setData("ar_JO");
  ui.menu_Language->addAction(action);
  (action = group3->addAction(tr("&Czech")))->setCheckable(true);
  action->setData("cs_CZ");
  ui.menu_Language->addAction(action);
  (action = group3->addAction(tr("Dutch (&Belgium)")))->setCheckable(true);
  action->setData("nl_BE");
  ui.menu_Language->addAction(action);
  (action = group3->addAction(tr("Dutch (&Netherlands)")))->setCheckable(true);
  action->setData("nl_NL");
  ui.menu_Language->addAction(action);
  (action = group3->addAction(tr("&English")))->setCheckable(true);
  action->setData("en_US");
  ui.menu_Language->addAction(action);
  (action = group3->addAction(tr("&French")))->setCheckable(true);
  action->setData("fr_FR");
  ui.menu_Language->addAction(action);
  (action = group3->addAction(tr("&German")))->setCheckable(true);
  action->setData("de_DE");
  ui.menu_Language->addAction(action);
  (action = group3->addAction(tr("G&reek")))->setCheckable(true);
  action->setData("el_GR");
  ui.menu_Language->addAction(action);
  (action = group3->addAction(tr("&Hungarian")))->setCheckable(true);
  action->setData("hu_HU");
  ui.menu_Language->addAction(action);
  (action = group3->addAction(tr("&Russian")))->setCheckable(true);
  action->setData("ru_RU");
  ui.menu_Language->addAction(action);

  foreach(QAction *action, ui.menu_Language->actions())
    {
      if(s_locale == action->data().toString())
	action->setChecked(true);

      connect(action,
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotLanguageChanged(void)));
    }

  QRegExp rx1("\\w+");
  QValidator *validator1 = 0;

  if((validator1 = new(std::nothrow) QRegExpValidator(rx1, this)) == 0)
    quit("Memory allocation failure", __FILE__, __LINE__);

  userinfo_diag->m_userinfo.memberid->setValidator(validator1);

  /*
  ** Highlight userinfo_diag->userinfo required widgets.
  */

  biblioteq_misc_functions::highlightWidget(userinfo_diag->m_userinfo.firstName,
					    QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget(userinfo_diag->m_userinfo.lastName,
					    QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget(userinfo_diag->m_userinfo.street,
					    QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget(userinfo_diag->m_userinfo.city,
					    QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget(userinfo_diag->m_userinfo.zip,
					    QColor(255, 248, 220));
  ui.splitter->restoreState
    (settings.value("main_splitter_state").toByteArray());
  ui.splitter->setCollapsible(1, false);
  ui.splitter->setStretchFactor(0, 0);
  ui.splitter->setStretchFactor(1, 1);
}

biblioteq::~biblioteq()
{
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
  QMapIterator<QString, QHash<QString, QString> > it(m_z3950Maps);

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
    if(ui.menuPreferredSRUSite->actions()[i]->isChecked())
      return ui.menuPreferredSRUSite->actions()[i]->text();

  return "";
}

QString biblioteq::getPreferredZ3950Site(void) const
{
  for(int i = 0; i < ui.menuPreferredZ3950Server->actions().size(); i++)
    if(ui.menuPreferredZ3950Server->actions()[i]->isChecked())
      return ui.menuPreferredZ3950Server->actions()[i]->text();

  return "";
}

QString biblioteq::getRoles(void) const
{
  /*
  ** Empty roles suggest that the user is a patron.
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
  int i = 0;
  QAction *action = 0;

  /*
  ** Delete existing actions, if any.
  */

  m_configToolMenu->clear();

  for(i = 0; i < ui.table->columnCount(); i++)
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

      if((action = new(std::nothrow) QAction(ui.table->
					     horizontalHeaderItem(i)->text(),
					     ui.configTool)) == 0)
	continue;

      action->setCheckable(true);
      action->setChecked(!ui.table->isColumnHidden(i));
      m_configToolMenu->addAction(action);
      connect(action, SIGNAL(triggered(void)), this,
	      SLOT(slotSetColumns(void)));
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

  QDateTime now = QDateTime::currentDateTime();
  QString str = "";
  QTableWidgetItem *item = 0;
  int i = 0;

  if(m_error_bar_label != 0)
    {
      m_error_bar_label->setIcon(QIcon(":/16x16/log.png"));
      m_error_bar_label->setToolTip(tr("Error Log Active"));
    }

  er.table->setSortingEnabled(false);
  er.table->setRowCount(er.table->rowCount() + 1);

  for(i = 0; i < 6; i++)
    if((item = new(std::nothrow) QTableWidgetItem()) != 0)
      {
	if(i == 0)
	  item->setText(now.toString("yyyy/MM/dd hh:mm:ss"));
	else if(i == 1)
	  item->setText(type.trimmed());
	else if(i == 2)
	  item->setText(summary.trimmed());
	else if(i == 3)
	  {
	    if(error.simplified().isEmpty())
	      item->setText(summary);
	    else
	      item->setText(error.simplified());
	  }
	else if(i == 4)
	  {
	    if(file)
	      item->setText(file);
	  }
	else
	  {
	    str.setNum(line);
	    item->setText(str);
	  }

	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	er.table->setItem(er.table->rowCount() - 1, i, item);
      }
    else
      qDebug() << tr("Memory failure in addError()!");

  for(int i = 0; i < er.table->columnCount() - 1; i++)
    er.table->resizeColumnToContents(i);

  er.table->setSortingEnabled(true);
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

  ui.detailsTool->setEnabled(true);
  ui.actionViewDetails->setEnabled(true);

  if(m_status_bar_label != 0)
    {
      m_status_bar_label->setPixmap(QPixmap(":/16x16/unlock.png"));

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
    }

  if(m_roles.contains("administrator") || m_roles.contains("librarian"))
    {
      ui.table->disconnect(SIGNAL(itemDoubleClicked(QTableWidgetItem *)));
      connect(ui.table, SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
	      this, SLOT(slotModify(void)));
      ui.graphicsView->scene()->disconnect(SIGNAL(itemDoubleClicked(void)));
      connect(ui.graphicsView->scene(), SIGNAL(itemDoubleClicked(void)),
	      this, SLOT(slotModify(void)));
      updateItemWindows();
    }

  if(m_roles.contains("administrator") || m_roles.contains("librarian"))
    ui.deleteTool->setEnabled(true);

  if(m_roles.contains("administrator") || m_roles.contains("librarian"))
    {
      ui.actionDeleteEntry->setEnabled(true);
      ui.actionDuplicateEntry->setEnabled(true);
      ui.actionImportCSV->setEnabled(true);
      ui.createTool->setEnabled(true);
      ui.duplicateTool->setEnabled(true);
      ui.menu_Add_Item->setEnabled(true);
      ui.modifyTool->setEnabled(true);
    }

  if(m_roles.contains("administrator") || m_roles.contains("librarian"))
    {
      ui.detailsTool->setEnabled(false);
      ui.actionViewDetails->setEnabled(false);
    }

  if(m_roles.contains("administrator") || m_roles.contains("librarian"))
    ui.actionModifyEntry->setEnabled(true);

  if(m_roles.contains("administrator") || m_roles.contains("circulation") ||
     m_roles.contains("membership"))
    {
      ui.userTool->setEnabled(true);
      ui.actionMembersBrowser->setEnabled(true);
    }

  if(m_roles.contains("administrator") || m_roles.contains("circulation"))
    ui.reserveTool->setEnabled(true);

  if(m_roles.contains("administrator") || m_roles.contains("librarian"))
    ui.actionAutoPopulateOnCreation->setEnabled(true);

  ui.actionPopulate_Members_Browser_Table_on_Display->setEnabled
    (m_roles.contains("administrator"));

  if(m_db.driverName() != "QSQLITE")
    {
      ui.actionConfigureAdministratorPrivileges->setEnabled
	(m_roles.contains("administrator"));
      ui.actionPopulate_Administrator_Browser_Table_on_Display->setEnabled
	(m_roles.contains("administrator"));
      ui.actionDatabase_Enumerations->setEnabled
	(m_roles.contains("administrator"));
      ui.actionPopulate_Database_Enumerations_Browser_on_Display->setEnabled
	(m_roles.contains("administrator"));
    }
  else
    {
      bb.grantButton->setEnabled(false);
      ui.actionDatabase_Enumerations->setEnabled(true);
      ui.actionPopulate_Database_Enumerations_Browser_on_Display->setEnabled
	(true);
    }

  ui.actionRequests->setToolTip(tr("Item Requests"));

  /*
  ** Hide certain fields in the Members Browser.
  */

  if(m_roles == "membership")
    {
      bb.historyButton->setEnabled(false);
      bb.listButton->setEnabled(false);
      bb.printButton->setEnabled(false);
      bb.checkoutButton->setEnabled(false);
      bb.overdueButton->setEnabled(false);
    }

  if(m_roles == "circulation" || m_roles == "librarian")
    {
      bb.addButton->setEnabled(false);
      bb.grantButton->setEnabled(false);
      bb.deleteButton->setEnabled(false);
      bb.modifyButton->setEnabled(false);

      if(m_roles == "librarian")
	{
	  ui.actionDatabase_Enumerations->setEnabled(true);
	  ui.actionPopulate_Database_Enumerations_Browser_on_Display->
	    setEnabled(true);
	}
    }
  else
    {
      disconnect(bb.table, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this,
		 SLOT(slotModifyBorrower(void)));
      connect(bb.table, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this,
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
	  cq.retranslateUi(m_customquery_diag);
	  er.retranslateUi(m_error_diag);
	  history.retranslateUi(m_history_diag);
	  pass.retranslateUi(m_pass_diag);
	  ui.retranslateUi(this);
	  ui.graphicsView->scene()->clear();
	  ui.graphicsView->resetTransform();
	  ui.graphicsView->horizontalScrollBar()->setValue(0);
	  ui.graphicsView->verticalScrollBar()->setValue(0);
	  ui.nextPageButton->setEnabled(false);
	  ui.pagesLabel->setText("1");
	  ui.previousPageButton->setEnabled(false);
	  ui.table->resetTable
	    (dbUserName(),
	     ui.menu_Category->defaultAction() ?
	     ui.menu_Category->defaultAction()->data().toString() :
	     "All",
	     m_roles);
	  ui.itemsCountLabel->setText(tr("0 Results"));
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
	break;
      }

  QMainWindow::changeEvent(event);
}

void biblioteq::cleanup(void)
{
  if(m_db.isOpen())
    m_db.close();
}

void biblioteq::closeEvent(QCloseEvent *e)
{
  slotExit();
  Q_UNUSED(e);
}

void biblioteq::createSqliteMenuActions(void)
{
  QSettings settings;
  QStringList allKeys(settings.allKeys());
  QStringList dups;

  ui.menu_Recent_SQLite_Files->clear();

  for(int i = 0; i < allKeys.size(); i++)
    {
      if(!allKeys[i].startsWith("sqlite_db_"))
	continue;

      QString str(settings.value(allKeys[i], "").toString().trimmed());

      if(str.isEmpty())
	{
	  settings.remove(allKeys[i]);
	  continue;
	}

      QFileInfo fileInfo(str);

      if(!dups.contains(str) && fileInfo.isReadable() && fileInfo.isWritable())
	dups.append(str);
      else
	{
	  settings.remove(allKeys[i]);
	  continue;
	}

      QAction *action = new(std::nothrow) QAction
	(str, ui.menu_Recent_SQLite_Files);

      if(!action)
	continue;

      action->setData(str);
      connect(action, SIGNAL(triggered(bool)), this,
	      SLOT(slotSqliteFileSelected(bool)));
      ui.menu_Recent_SQLite_Files->addAction(action);
    }

  dups.clear();
  allKeys.clear();

  QAction *action = new(std::nothrow) QAction(tr("&Clear Menu"),
					      ui.menu_Recent_SQLite_Files);

  if(action)
    {
      connect(action, SIGNAL(triggered(bool)), this,
	      SLOT(slotClearSqliteMenu(bool)));

      if(ui.menu_Recent_SQLite_Files->actions().size() > 0)
	ui.menu_Recent_SQLite_Files->addSeparator();

      ui.menu_Recent_SQLite_Files->addAction(action);
    }
}

void biblioteq::initialUpdate(void)
{
  /*
  ** Read the configuration file.
  */

  readConfig();

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
  else if(m_roles.contains("administrator") ||
	  m_roles.contains("circulation"))
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
      if(dbUserName() == "xbook_guest" ||
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

  disconnect(ui.menu_Category, SIGNAL(triggered(QAction *)), this,
	     SLOT(slotAutoPopOnFilter(QAction *)));
  ui.menu_Category->clear();

  for(int i = 0; i < m_menuCategoryActionGroup->actions().size(); i++)
    m_menuCategoryActionGroup->removeAction
      (m_menuCategoryActionGroup->actions().at(i));

  for(int i = 0; i < tmplist1.size(); i++)
    {
      QAction *action = ui.menu_Category->addAction(tmplist2[i]);

      if(action)
	{
	  action->setCheckable(true);
	  action->setData(tmplist1[i]);
	  m_menuCategoryActionGroup->addAction(action);
	}
    }

  connect(ui.menu_Category, SIGNAL(triggered(QAction *)), this,
	  SLOT(slotAutoPopOnFilter(QAction *)));
  tmplist1.clear();
  tmplist2.clear();
}

void biblioteq::prepareRequestToolButton(const QString &typefilter)
{
  if(m_db.driverName() != "QSQLITE")
    if(m_db.isOpen())
      {
	if(dbUserName() == "xbook_guest")
	  {
	    ui.actionRequests->setToolTip(tr("Item Requests"));
	    ui.actionRequests->setIcon(QIcon(":/32x32/request.png"));
	    ui.actionRequests->setEnabled(false);
	  }
	else if((m_roles == "administrator" || m_roles == "circulation") &&
		typefilter == "All Requested")
	  {
	    ui.actionRequests->setEnabled(true);
	    ui.actionRequests->setToolTip(tr("Cancel Selected Request(s)"));
	    ui.actionRequests->setIcon
	      (QIcon(":/32x32/remove_request.png"));
	  }
	else if(m_roles.isEmpty() && (typefilter == "All" ||
				      typefilter == "All Available" ||
				      typefilter == "Books" ||
				      typefilter == "DVDs" ||
				      typefilter == "Journals" ||
				      typefilter == "Magazines" ||
				      typefilter == "Music CDs" ||
				      typefilter == "Video Games"))
	  {
	    ui.actionRequests->setToolTip(tr("Request Selected Item(s)"));
	    ui.actionRequests->setIcon(QIcon(":/32x32/request.png"));
	    ui.actionRequests->setEnabled(true);
	  }
	else if(m_roles.isEmpty() && typefilter == "All Requested")
	  {
	    ui.actionRequests->setToolTip(tr("Cancel Selected Request(s)"));
	    ui.actionRequests->setIcon
	      (QIcon(":/32x32/remove_request.png"));
	    ui.actionRequests->setEnabled(true);
	  }
	else
	  {
	    ui.actionRequests->setToolTip(tr("Item Requests"));
	    ui.actionRequests->setIcon(QIcon(":/32x32/request.png"));
	    ui.actionRequests->setEnabled(false);
	  }
      }
}

void biblioteq::quit(const char *msg, const char *file, const int line)
{
  if(msg != 0 && qstrnlen(msg, std::numeric_limits<uint>::max()) > 0)
    qDebug() << tr(msg)
	     << tr(" in file ")
	     << file << tr(", line ") << line
	     << tr(".");
  else
    qDebug() << tr("An unknown error occurred in file ")
	     << file << tr(", line ")
	     << line << tr(".");

  exit(EXIT_FAILURE);
}

void biblioteq::quit(void)
{
  QCoreApplication::quit();
}

void biblioteq::removeBook(biblioteq_book *book)
{
  if(book)
    book->deleteLater();
}

void biblioteq::removeCD(biblioteq_cd *cd)
{
  if(cd)
    cd->deleteLater();
}

void biblioteq::removeDVD(biblioteq_dvd *dvd)
{
  if(dvd)
    dvd->deleteLater();
}

void biblioteq::removeGreyLiterature(biblioteq_grey_literature *gl)
{
  if(gl)
    gl->deleteLater();
}

void biblioteq::removeJournal(biblioteq_journal *journal)
{
  if(journal)
    journal->deleteLater();
}

void biblioteq::removeMagazine(biblioteq_magazine *magazine)
{
  if(magazine)
    magazine->deleteLater();
}

void biblioteq::removePhotographCollection(biblioteq_photographcollection *pc)
{
  if(pc)
    pc->deleteLater();
}

void biblioteq::removeVideoGame(biblioteq_videogame *videogame)
{
  if(videogame)
    videogame->deleteLater();
}

void biblioteq::replaceBook(const QString &id, biblioteq_book *book)
{
  Q_UNUSED(id);
  Q_UNUSED(book);
}

void biblioteq::replaceCD(const QString &id, biblioteq_cd *cd)
{
  Q_UNUSED(id);
  Q_UNUSED(cd);
}

void biblioteq::replaceDVD(const QString &id, biblioteq_dvd *dvd)
{
  Q_UNUSED(id);
  Q_UNUSED(dvd);
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

  ab.table->setCurrentItem(0);
  ab.table->setColumnCount(0);
  ab.table->setRowCount(0);
  ab.table->scrollToTop();
  ab.table->horizontalScrollBar()->setValue(0);
  list.clear();
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
  list.clear();
}

void biblioteq::resetMembersBrowser(void)
{
  QStringList list;

  bb.table->setCurrentItem(0);
  bb.table->setColumnCount(0);
  bb.table->setRowCount(0);
  bb.table->scrollToTop();
  bb.table->horizontalScrollBar()->setValue(0);
  m_bbColumnHeaderIndexes.clear();
  list.append(tr("Member ID"));
  list.append(tr("First Name"));
  list.append(tr("Last Name"));
  list.append(tr("E-Mail Address"));
  list.append(tr("Telephone Number"));
  list.append(tr("Home Address"));
  list.append(tr("Member Since"));
  list.append(tr("Expiration Date"));
  list.append(tr("Books Reserved"));
  list.append(tr("CDs Reserved"));
  list.append(tr("DVDs Reserved"));
  list.append(tr("Journals Reserved"));
  list.append(tr("Magazines Reserved"));
  list.append(tr("Video Games Reserved"));
  m_bbColumnHeaderIndexes.append("Member ID");
  m_bbColumnHeaderIndexes.append("First Name");
  m_bbColumnHeaderIndexes.append("Last Name");
  m_bbColumnHeaderIndexes.append("E-Mail Address");
  m_bbColumnHeaderIndexes.append("Telephone Number");
  m_bbColumnHeaderIndexes.append("Home Address");
  m_bbColumnHeaderIndexes.append("Member Since");
  m_bbColumnHeaderIndexes.append("Expiration Date");
  m_bbColumnHeaderIndexes.append("Books Reserved");
  m_bbColumnHeaderIndexes.append("CDs Reserved");
  m_bbColumnHeaderIndexes.append("DVDs Reserved");
  m_bbColumnHeaderIndexes.append("Journals Reserved");
  m_bbColumnHeaderIndexes.append("Magazines Reserved");
  m_bbColumnHeaderIndexes.append("Video Games Reserved");
  bb.table->setColumnCount(list.size());
  bb.table->setHorizontalHeaderLabels(list);
  list.clear();
  bb.table->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);

  for(int i = 0; i < bb.table->columnCount() - 1; i++)
    bb.table->resizeColumnToContents(i);
}

void biblioteq::setGlobalFonts(const QFont &font)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  QApplication::setFont(font);

  foreach(QWidget *widget, QApplication::allWidgets())
    {
      widget->setFont(font);

      if(!(qobject_cast<QDialog *> (widget) ||
	   qobject_cast<QMainWindow *> (widget)))
	widget->adjustSize();

      widget->update();
    }

  QMenuBar *mb = menuBar();

  if(mb)
    {
      mb->setFont(font);

      foreach(QMenu *menu, mb->findChildren<QMenu *> ())
	foreach(QAction *action, menu->actions())
	  action->setFont(font);

      mb->update();
    }

  QApplication::restoreOverrideCursor();
}

void biblioteq::showMain(void)
{
  statusBar()->setStyleSheet("QStatusBar::item{border: 0px;}");

  if(m_connected_bar_label)
    m_connected_bar_label->deleteLater();

  if(m_error_bar_label)
    m_error_bar_label->deleteLater();

  if(m_status_bar_label)
    m_status_bar_label->deleteLater();

  if((m_connected_bar_label = new(std::nothrow) QLabel()) != 0)
    {
      m_connected_bar_label->setPixmap
	(QPixmap(":/16x16/disconnected.png"));
      m_connected_bar_label->setToolTip(tr("Disconnected"));
      statusBar()->addPermanentWidget(m_connected_bar_label);
    }

  if((m_status_bar_label = new(std::nothrow) QLabel()) != 0)
    {
      m_status_bar_label->setPixmap(QPixmap(":/16x16/lock.png"));
      m_status_bar_label->setToolTip(tr("Standard User Mode"));
      statusBar()->addPermanentWidget(m_status_bar_label);
    }

  if((m_error_bar_label = new(std::nothrow) QToolButton()) != 0)
    {
      disconnect(m_error_bar_label,
		 SIGNAL(clicked(void)),
		 this,
		 SLOT(slotShowErrorDialog(void)));
      connect(m_error_bar_label,
	      SIGNAL(clicked(void)),
	      this,
	      SLOT(slotShowErrorDialog(void)));
      m_error_bar_label->setAutoRaise(true);
      m_error_bar_label->setIcon(QIcon(":/16x16/ok.png"));
      m_error_bar_label->setToolTip(tr("Empty Error Log"));
      statusBar()->addPermanentWidget(m_error_bar_label);
#ifdef Q_OS_MAC
      m_error_bar_label->setStyleSheet
	("QToolButton {border: none;}"
	 "QToolButton::menu-button {border: none;}");
#endif
    }

  ui.itemsCountLabel->setText(tr("0 Results"));

  QSettings settings;

  if(settings.contains("mainwindowState"))
    restoreState(settings.value("mainwindowState").toByteArray());

  readGlobalSetup();

  /*
  ** Perform additional user interface duties.
  */

  QActionGroup *group1 = 0;

  if((group1 = new(std::nothrow) QActionGroup(this)) == 0)
    quit("Memory allocation failure", __FILE__, __LINE__);

  QStringList list(m_sruMaps.keys());

  for(int i = 0; i < list.size(); i++)
    {
      QAction *action = group1->addAction(list.at(i));

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

  QActionGroup *group2 = 0;

  if((group2 = new(std::nothrow) QActionGroup(this)) == 0)
    quit("Memory allocation failure", __FILE__, __LINE__);

  list = m_z3950Maps.keys();

  for(int i = 0; i < list.size(); i++)
    {
      QAction *action = group2->addAction(list.at(i));

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

  /*
  ** Initial update.
  */

  initialUpdate();
  show();
#ifndef Q_OS_MAC
  setGlobalFonts(QApplication::font());
#endif
  slotResizeColumns();

#ifdef Q_OS_MAC
  QFileInfo fileInfo
    (QCoreApplication::applicationDirPath() + "/../../../biblioteq.conf");
#elif defined(Q_OS_OS2) || defined(Q_OS_WIN32)
  QFileInfo fileInfo(QCoreApplication::applicationDirPath() +
		     QDir::separator() +
		     "biblioteq.conf");
#else
  QFileInfo fileInfo(BIBLIOTEQ_CONFIGFILE);
#endif

  if(!fileInfo.isReadable())
    {
      QMessageBox::warning
	(this, tr("BiblioteQ: Warning"),
	 tr("BiblioteQ was not able to discover the biblioteq.conf "
	    "file. Default values will be assumed. The expected absolute "
	    "path of biblioteq.conf is %1.").arg(fileInfo.absolutePath()));
      QApplication::processEvents();
    }

  if(!(QSqlDatabase::isDriverAvailable("QPSQL") ||
       QSqlDatabase::isDriverAvailable("QSQLITE")))
    {
      QFileInfo fileInfo("qt.conf");
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
}

void biblioteq::slotAbout(void)
{
  QMessageBox mb(this);
  QString qversion("");
  const char *tmp = qVersion();

  if(tmp)
    qversion = tmp;

  qversion = qversion.trimmed();

  if(qversion.isEmpty())
    qversion = "unknown";

#ifndef Q_OS_MAC
  mb.setFont(QApplication::font());
#endif
  mb.setWindowIcon(windowIcon());
  mb.setWindowTitle(tr("BiblioteQ: About"));
  mb.setTextFormat(Qt::RichText);
  mb.setText
    (tr("<html>BiblioteQ Version %1<br>"
	"Architecture %4.<br>"
	"Compiled on %2, %3.<br>"
	"Copyright (c) 2005 - present, Petya Stormware.<br>"
	"Faenza icons.<br>"
#ifdef BIBLIOTEQ_POPPLER_VERSION_DEFINED
	"Poppler version %5.<br>"
#else
	"%5"
#endif
	"Qt version %6 (runtime %7).<br>"
	"YAZ version %8."
	"<hr>"
	"Please visit <a href=\"https://biblioteq.sourceforge.io\">"
	"https://biblioteq.sourceforge.io</a> or "
	"<a href=\"https://textbrowser.github.io/biblioteq/\">"
	"https://textbrowser.github.io/biblioteq</a> "
	"for project information.</html>").
     arg(BIBLIOTEQ_VERSION).
     arg(__DATE__).
     arg(__TIME__).
     arg(BIBLIOTEQ_ARCHITECTURE_STR).
#ifdef BIBLIOTEQ_POPPLER_VERSION_DEFINED
     arg(POPPLER_VERSION).
#else
     arg("Poppler library is not supported.").
#endif
     arg(QT_VERSION_STR).
     arg(qversion).
     arg(YAZ_VERSION));
  mb.setStandardButtons(QMessageBox::Ok);
  mb.setIconPixmap
    (QPixmap(":/book.png").scaled(QSize(128, 128),
				  Qt::KeepAspectRatio,
				  Qt::SmoothTransformation));
  mb.exec();
  QApplication::processEvents();
}

void biblioteq::slotAddAdmin(void)
{
  QCheckBox *checkBox = 0;
  QTableWidgetItem *item = 0;
  int i = 0;

  ab.table->setRowCount(ab.table->rowCount() + 1);

  for(i = 0; i < ab.table->columnCount(); i++)
    if(i == 0)
      {
	if((item = new(std::nothrow) QTableWidgetItem()) != 0)
	  {
	    item->setFlags(item->flags() | Qt::ItemIsEditable);
	    ab.table->setItem(ab.table->rowCount() - 1, 0, item);
	  }
	else
	  addError(QString(tr("Memory Error")),
		   QString(tr("Unable to allocate memory for the "
			      "\"item\" object. "
			      "This is a serious problem!")),
		   QString(""), __FILE__, __LINE__);
      }
    else
      {
	if((checkBox = new(std::nothrow) QCheckBox()) == 0)
	  addError(QString(tr("Memory Error")),
		   QString(tr("Unable to allocate memory for the "
			      "\"checkBox\" object. "
			      "This is a serious problem!")),
		   QString(""), __FILE__, __LINE__);
	else
	  {
	    ab.table->setCellWidget(ab.table->rowCount() - 1, i, checkBox);
	    connect(checkBox, SIGNAL(stateChanged(int)), this,
		    SLOT(slotAdminCheckBoxClicked(int)));
	  }
      }
}

void biblioteq::slotAddBorrower(void)
{
  QDate now = QDate::currentDate();
  QDateTime nowTime = QDateTime::currentDateTime();

  biblioteq_misc_functions::highlightWidget(userinfo_diag->m_userinfo.memberid,
					    QColor(255, 248, 220));
  userinfo_diag->m_memberProperties.clear();
  userinfo_diag->m_userinfo.memberid->setReadOnly(false);
  userinfo_diag->m_userinfo.memberid->setText
    ("m" + nowTime.toString("yyyyMMddhhmmss"));
  userinfo_diag->m_userinfo.membersince->setDate(now);
  userinfo_diag->m_userinfo.membersince->setMaximumDate(now);
  userinfo_diag->m_userinfo.firstName->clear();
  userinfo_diag->m_userinfo.lastName->clear();
  userinfo_diag->m_userinfo.middle->clear();
  userinfo_diag->m_userinfo.dob->setDate(now.addYears(-25));
  userinfo_diag->m_userinfo.sex->setCurrentIndex(2); // Private
  userinfo_diag->m_userinfo.street->clear();
  userinfo_diag->m_userinfo.city->clear();
  userinfo_diag->m_userinfo.state->setCurrentIndex(0);
  userinfo_diag->m_userinfo.zip->setText("N/A");
  userinfo_diag->m_userinfo.zip->setCursorPosition(0);
  userinfo_diag->m_userinfo.telephoneNumber->clear();
  userinfo_diag->m_userinfo.email->clear();
  userinfo_diag->m_userinfo.expirationdate->setDate
    (QDate::fromString("01/01/3000", "MM/dd/yyyy"));
  userinfo_diag->m_userinfo.overduefees->setValue(0.00);
  userinfo_diag->m_userinfo.comments->clear();
  userinfo_diag->m_userinfo.memberclass->clear();
  userinfo_diag->m_userinfo.generalregistrationnumber->clear();
  userinfo_diag->m_memberProperties["membersince"] =
    userinfo_diag->m_userinfo.membersince->date().toString
    (Qt::ISODate);
  userinfo_diag->m_memberProperties["dob"] =
    userinfo_diag->m_userinfo.dob->date().toString
    (Qt::ISODate);
  userinfo_diag->m_memberProperties["sex"] =
    userinfo_diag->m_userinfo.sex->currentText();
  userinfo_diag->m_memberProperties["state_abbr"] =
    userinfo_diag->m_userinfo.state->currentText();
  userinfo_diag->m_memberProperties["zip"] = userinfo_diag->m_userinfo.zip->
    text();
  userinfo_diag->m_memberProperties["telephone_num"] =
    userinfo_diag->m_userinfo.telephoneNumber->text();
  userinfo_diag->m_memberProperties["expiration_date"] =
    userinfo_diag->m_userinfo.expirationdate->date().toString
    (Qt::ISODate);
  userinfo_diag->m_memberProperties["overdue_fees"] =
    userinfo_diag->m_userinfo.overduefees->text();
  userinfo_diag->setWindowTitle(tr("BiblioteQ: Create New Member"));
  m_engUserinfoTitle = "Create New Member";
  userinfo_diag->m_userinfo.prevTool->setVisible(false);
  userinfo_diag->m_userinfo.nextTool->setVisible(false);
  userinfo_diag->m_userinfo.memberid->selectAll();
  userinfo_diag->m_userinfo.tabWidget->setCurrentIndex(0);
  userinfo_diag->m_userinfo.memberid->setFocus();
  userinfo_diag->updateGeometry();
  biblioteq_misc_functions::center(userinfo_diag, m_members_diag);
  userinfo_diag->show();
}

void biblioteq::slotAdminCheckBoxClicked(int state)
{
  QCheckBox *box = qobject_cast<QCheckBox *> (sender());
  int column = -1;
  int i = 0;
  int j = 0;
  int row = -1;

  (void) state;

  for(i = 0; i < ab.table->rowCount(); i++)
    for(j = 1; j < ab.table->columnCount(); j++)
      if(ab.table->cellWidget(i, j) == box)
	{
	  row = i;
	  column = j;
	  break;
	}

  if(row > -1)
    {
      if(column == 1)
	{
	  for(i = 2; i < ab.table->columnCount(); i++)
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

  disconnect(ui.menu_Category, SIGNAL(triggered(QAction *)), this,
	     SLOT(slotAutoPopOnFilter(QAction *)));
  action->setChecked(true);
  ui.menu_Category->setDefaultAction(action);
  connect(ui.menu_Category, SIGNAL(triggered(QAction *)), this,
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
      QString typefilter("");

      typefilter = action->data().toString();
      ui.graphicsView->scene()->clear();
      ui.graphicsView->resetTransform();
      ui.graphicsView->verticalScrollBar()->setValue(0);
      ui.graphicsView->horizontalScrollBar()->setValue(0);
      ui.nextPageButton->setEnabled(false);
      ui.pagesLabel->setText("1");
      ui.previousPageButton->setEnabled(false);
      ui.table->resetTable(dbUserName(), typefilter, "");
      ui.itemsCountLabel->setText(tr("0 Results"));
    }
}

void biblioteq::slotBranchChanged(void)
{
  QHash<QString, QString> tmphash;

  tmphash = m_branches[br.branch_name->currentText()];

  if(tmphash.value("database_type") == "sqlite")
    {
      br.stackedWidget->setCurrentIndex(0);
      br.fileButton->setFocus();
    }
  else
    {
      br.stackedWidget->setCurrentIndex(1);
      br.userid->selectAll();
    }

  tmphash.clear();
  m_branch_diag->update();
  m_branch_diag->resize(m_branch_diag->width(),
			m_branch_diag->minimumSize().height());
  m_branch_diag->show();
}

void biblioteq::slotCancelAddUser(void)
{
  if(userinfo_diag->isVisible())
    userinfo_diag->close();
}

void biblioteq::slotChangeView(bool checked)
{
  Q_UNUSED(checked);

  QAction *action = qobject_cast<QAction *> (sender());

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
  QStringList allKeys(settings.allKeys());

  for(int i = 0; i < allKeys.size(); i++)
    if(allKeys[i].startsWith("sqlite_db_"))
      settings.remove(allKeys[i]);

  allKeys.clear();
  createSqliteMenuActions();
}

void biblioteq::slotCloseCustomQueryDialog(void)
{
  m_customquery_diag->close();
}

void biblioteq::slotCloseMembersBrowser(void)
{
  /*
  ** Changes? If so, warn the user.
  */

  /*
  ** Also closes the Reservation History Browser.
  */

  m_history_diag->close();
  m_members_diag->close();
}

void biblioteq::slotClosePasswordDialog(void)
{
  m_pass_diag->setVisible(false);
  pass.password->clear();
  pass.userid->clear();
}

void biblioteq::slotCopyError(void)
{
  int i = 0;
  int j = 0;
  QString text = "";
  QClipboard *clipboard = QApplication::clipboard();
  QModelIndexList list = er.table->selectionModel()->selectedRows();

  if(list.isEmpty())
    {
      QMessageBox::critical(m_error_diag, tr("BiblioteQ: User Error"),
			    tr("To copy the contents of the Error "
			       "Log into "
			       "the clipboard buffer, you must first "
			       "select at least one entry."));
      QApplication::processEvents();
      return;
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  foreach(const QModelIndex &index, list)
    {
      i = index.row();

      for(j = 0; j < er.table->columnCount(); j++)
	text += er.table->item(i, j)->text() + ",";

      text = text.trimmed();
      text += "\n";
    }

  if(!text.isEmpty())
    clipboard->setText(text);

  list.clear();
  QApplication::restoreOverrideCursor();
}

void biblioteq::slotDelete(void)
{
  if(!m_db.isOpen())
    return;

  QModelIndexList list = ui.table->selectionModel()->selectedRows();
  QProgressDialog progress(this);
  QSqlQuery query(m_db);
  QString errorstr = "";
  QString itemType = "";
  QString oid = "";
  QString str = "";
  QString title = "";
  bool error = false;
  bool isCheckedOut = false;
  bool isRequested = false;
  int col = -1;
  int i = 0;
  int numdeleted = 0;

  if(list.isEmpty())
    {
      QMessageBox::critical(this, tr("BiblioteQ: User Error"),
			    tr("Please select an item to delete."));
      QApplication::processEvents();
      return;
    }

  col = ui.table->columnNumber("MYOID");

  foreach(const QModelIndex &index, list)
    {
      i = index.row();

      if(ui.table->item(i, col) == 0)
	continue;

      oid = biblioteq_misc_functions::getColumnString
	(ui.table, i, ui.table->columnNumber("MYOID"));
      itemType = biblioteq_misc_functions::getColumnString
	(ui.table, i, ui.table->columnNumber("Type"));

      if(oid.isEmpty() || itemType.isEmpty())
	{
	  addError(QString(tr("Error")),
		   QString(tr("The main table does not contain enough "
			      "information for item deletion.")),
		   QString(tr("The main table does not contain enough "
			      "information for item deletion.")),
		   __FILE__, __LINE__);
	  QMessageBox::critical(this, tr("BiblioteQ: Error"),
				tr("The main table does not contain enough "
				   "information for item deletion."));
	  QApplication::processEvents();
	  list.clear();
	  return;
	}

      QApplication::setOverrideCursor(Qt::WaitCursor);
      isCheckedOut = biblioteq_misc_functions::isCheckedOut(m_db, oid, itemType,
							    errorstr);
      QApplication::restoreOverrideCursor();

      if(!errorstr.isEmpty())
	{
	  addError(QString(tr("Database Error")),
		   QString(tr("Unable to determine if the item has been "
			      "reserved.")),
		   errorstr, __FILE__, __LINE__);
	  QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
				tr("Unable to determine if the item has "
				   "been reserved."));
	  QApplication::processEvents();
	  list.clear();
	  return;
	}

      if(isCheckedOut)
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Reserved items may not be deleted."));
	  QApplication::processEvents();
	  list.clear();
	  return;
	}

      QApplication::setOverrideCursor(Qt::WaitCursor);
      isRequested = biblioteq_misc_functions::isRequested
	(m_db, oid, itemType, errorstr);
      QApplication::restoreOverrideCursor();

      if(!errorstr.isEmpty())
	{
	  addError(QString(tr("Database Error")),
		   QString(tr("Unable to determine if the item has been "
			      "requested.")),
		   errorstr, __FILE__, __LINE__);
	  QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
				tr("Unable to determine if the item has "
				   "been requested."));
	  QApplication::processEvents();
	  list.clear();
	  return;
	}

      if(isRequested)
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Requested items may not be deleted."));
	  QApplication::processEvents();
	  list.clear();
	  return;
	}
    }

  if(list.size() > 0)
    {
      if(QMessageBox::question(this, tr("BiblioteQ: Question"),
			       tr("Are you sure that you wish to permanently "
				  "delete the selected item(s)?"),
			       QMessageBox::Yes | QMessageBox::No,
			       QMessageBox::No) == QMessageBox::No)
	{
	  QApplication::processEvents();
	  list.clear();
	  return;
	}
      else
	QApplication::processEvents();
    }

  progress.setCancelButton(0);
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.setLabelText(tr("Deleting the selected item(s)..."));
  progress.setMaximum(list.size());
  progress.setMinimum(0);
  progress.show();
  progress.repaint();
  QApplication::processEvents();

  foreach(const QModelIndex &index, list)
    {
      i = index.row();

      if(i + 1 <= progress.maximum())
	progress.setValue(i + 1);

      progress.repaint();
      QApplication::processEvents();

      if(ui.table->item(i, col) == 0)
	continue;

      str = ui.table->item(i, col)->text();
      itemType = biblioteq_misc_functions::getColumnString
	(ui.table, i, ui.table->columnNumber("Type")).
	toLower();

      if(itemType == "grey literature" || itemType == "photograph collection")
	itemType = itemType.replace(" ", "_");
      else
	itemType = itemType.remove(" ");

      if(itemType == "book" || itemType == "cd" || itemType == "dvd" ||
	 itemType == "grey_literature" ||
	 itemType == "journal" || itemType == "magazine" ||
	 itemType == "photograph_collection" || itemType == "videogame")
	query.prepare(QString("DELETE FROM %1 WHERE myoid = ?").
		      arg(itemType));

      query.bindValue(0, str);

      if(!query.exec())
	{
	  error = true;
	  addError(QString(tr("Database Error")),
		   QString(tr("Unable to delete the item.")),
		   query.lastError().text(), __FILE__, __LINE__);
	}
      else
	{
	  deleteItem(str, itemType);
	  numdeleted += 1;
	}
    }

  progress.close();

  /*
  ** Provide some fancy messages.
  */

  if(error)
    {
      QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			    tr("Unable to delete all or some of the selected "
			       "items."));
      QApplication::processEvents();
    }

  if(numdeleted > 0)
    slotRefresh();

  list.clear();
}

void biblioteq::slotDeleteAdmin(void)
{
  QString str = "";
  int row = ab.table->currentRow();

  if(row < 0)
    {
      QMessageBox::critical
	(m_admin_diag, tr("BiblioteQ: User Error"),
	 tr("To delete an entry, you must first select it."));
      QApplication::processEvents();
      return;
    }

  str = ab.table->item(row, 0)->text().toLower().trimmed();

  if((ab.table->item(row, 0)->flags() & Qt::ItemIsEditable) == 0 &&
     str == getAdminID())
    {
      QMessageBox::critical(m_admin_diag, tr("BiblioteQ: User Error"),
			    tr("As an administrator, you may not delete "
			       "your account."));
      QApplication::processEvents();
      return;
    }
  else
    {
      if(!str.isEmpty() && !m_deletedAdmins.contains(str))
	m_deletedAdmins.append(str);

      ab.table->removeRow(row);
#ifdef Q_OS_MAC
      ab.table->hide();
      ab.table->show();
#endif
    }
}

void biblioteq::slotDisplayNewSqliteDialog(void)
{
  QFileDialog dialog(this);
  bool error = true;

  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setDirectory(QDir::homePath());
  dialog.setNameFilter("SQLite Database (*.sqlite)");
  dialog.setDefaultSuffix("sqlite");
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowTitle(tr("BiblioteQ: New SQLite Database"));
  dialog.exec();
  QApplication::processEvents();

  if(dialog.result() == QDialog::Accepted)
    {
      repaint();
      QApplication::processEvents();

      int rc = 0;
      sqlite3 *ppDb = 0;

      QApplication::setOverrideCursor(Qt::WaitCursor);
      QFile::remove(dialog.selectedFiles().value(0));
      rc = sqlite3_open_v2(dialog.selectedFiles().value(0).toUtf8(),
			   &ppDb,
			   SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE,
			   0);

      if(rc == SQLITE_OK)
	{
	  char *errorstr = 0;

	  if(sqlite3_exec(ppDb,
			  sqlite_create_schema_text,
			  0,
			  0,
			  &errorstr) == SQLITE_OK)
	    error = false;
	  else
	    addError(tr("Database Error"),
		     "Unable to create the specified SQLite database.",
		     errorstr, __FILE__, __LINE__);

	  sqlite3_free(errorstr);
	}
      else
	addError(tr("Database Error"),
		 tr("Unable to create the specified SQLite database."),
		 "sqlite3_open_v2() failure.", __FILE__, __LINE__);

      sqlite3_close(ppDb);
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

	      bool found = false;

	      for(int i = 0; i < br.branch_name->count(); i++)
		{
		  if(m_branches.contains(br.branch_name->itemText(i)))
		    if(m_branches[br.branch_name->itemText(i)].
		       value("database_type") == "sqlite")
		      {
			found = true;
			br.branch_name->setCurrentIndex(i);
			break;
		      }
		}

	      if(found)
		{
		  if(QMessageBox::question
		     (this,
		      tr("BiblioteQ: Question"),
		      tr("It appears that you are already "
			 "connected to a database. Do you "
			 "want to terminate the current connection "
			 "and connect to the new SQLite database?"),
		      QMessageBox::Yes | QMessageBox::No,
		      QMessageBox::No) == QMessageBox::Yes)
		    {
		      QApplication::processEvents();
		      slotDisconnect();
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

	      bool found = false;

	      for(int i = 0; i < br.branch_name->count(); i++)
		{
		  if(m_branches.contains(br.branch_name->itemText(i)))
		    if(m_branches[br.branch_name->itemText(i)].
		       value("database_type") == "sqlite")
		      {
			found = true;
			br.branch_name->setCurrentIndex(i);
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
	    (this, tr("BiblioteQ: Database Error"),
	     tr("An error occurred while attempting "
		"to create the specified SQLite database."));
	  QApplication::processEvents();
	}
    }
}

void biblioteq::slotDuplicate(void)
{
  if(!m_db.isOpen())
    return;

  QModelIndexList list = ui.table->selectionModel()->selectedRows();
  QString oid = "";
  QString type = "";
  biblioteq_book *book = 0;
  biblioteq_cd *cd = 0;
  biblioteq_dvd *dvd = 0;
  biblioteq_grey_literature *gl = 0;
  biblioteq_journal *journal = 0;
  biblioteq_magazine *magazine = 0;
  biblioteq_main_table *table = ui.table;
  biblioteq_photographcollection *photograph = 0;
  biblioteq_videogame *video_game = 0;
  bool error = false;
  int i = 0;

  if(list.isEmpty())
    {
      QMessageBox::critical(this, tr("BiblioteQ: User Error"),
			    tr("Please select at least one item to "
			       "duplicate."));
      QApplication::processEvents();
      return;
    }
  else if(list.size() >= 5)
    {
      if(QMessageBox::question(this, tr("BiblioteQ: Question"),
			       tr("Are you sure that you wish to duplicate "
				  "the ") +
			       QString::number(list.size()) +
			       tr(" selected items? BiblioteQ will exit if "
				  "it's unable "
				  "to acquire resources."),
			       QMessageBox::Yes | QMessageBox::No,
			       QMessageBox::No) == QMessageBox::No)
	{
	  QApplication::processEvents();
	  list.clear();
	  return;
	}

      QApplication::processEvents();
    }

  QString id("");

  QApplication::setOverrideCursor(Qt::WaitCursor);
  std::stable_sort(list.begin(), list.end());

  foreach(const QModelIndex &index, list)
    {
      i = index.row();
      oid = biblioteq_misc_functions::getColumnString
	(table, i, table->columnNumber("MYOID"));
      type = biblioteq_misc_functions::getColumnString
	(table, i, table->columnNumber("Type"));
      m_idCt += 1;
      id = QString("duplicate_%1").arg(m_idCt);

      if(type.toLower() == "book")
	{
	  book = new(std::nothrow) biblioteq_book(this, oid, i);

	  if(book)
	    book->duplicate(id, EDITABLE);
	}
      else if(type.toLower() == "cd")
	{
	  cd = new(std::nothrow) biblioteq_cd(this, oid, i);

	  if(cd)
	    cd->duplicate(id, EDITABLE);
	}
      else if(type.toLower() == "dvd")
	{
	  dvd = new(std::nothrow) biblioteq_dvd(this, oid, i);

	  if(dvd)
	    dvd->duplicate(id, EDITABLE);
	}
      else if(type.toLower() == "grey literature")
	{
	  gl = new(std::nothrow) biblioteq_grey_literature(this, oid, i);

	  if(gl)
	    gl->duplicate(id, EDITABLE);
	}
      else if(type.toLower() == "journal")
	{
	  journal = new(std::nothrow) biblioteq_journal(this, oid, i);

	  if(journal)
	    journal->duplicate(id, EDITABLE);
	}
      else if(type.toLower() == "magazine")
	{
	  magazine = new(std::nothrow) biblioteq_magazine(this,
							  oid, i,
							  "magazine");

	  if(magazine)
	    magazine->duplicate(id, EDITABLE);
	}
      else if(type.toLower() == "photograph collection")
	{
	  photograph = new(std::nothrow) biblioteq_photographcollection
	    (this, oid, i);

	  if(photograph)
	    photograph->duplicate(id, EDITABLE);
	}
      else if(type.toLower() == "video game")
	{
	  video_game = new(std::nothrow) biblioteq_videogame(this,
							     oid, i);

	  if(video_game)
	    video_game->duplicate(id, EDITABLE);
	}
      else
	{
	  error = true;
	  break;
	}
    }

  list.clear();
  QApplication::restoreOverrideCursor();

  if(error)
    {
      QMessageBox::critical(this, tr("BiblioteQ: Error"),
			    tr("Unable to determine the selected item's "
			       "type."));
      QApplication::processEvents();
    }
}

void biblioteq::slotExecuteCustomQuery(void)
{
  QString querystr = "";

  querystr = cq.query_te->toPlainText().trimmed();

  if(querystr.isEmpty())
    {
      QMessageBox::critical(m_customquery_diag, tr("BiblioteQ: User Error"),
			    tr("Please provide a valid SQL statement."));
      QApplication::processEvents();
      return;
    }

  if(querystr.toLower().contains("alter ") ||
     querystr.toLower().contains("cluster ") ||
     querystr.toLower().contains("create " ) ||
     querystr.toLower().contains("delete ") ||
     querystr.toLower().contains("drop ") ||
     querystr.toLower().contains("grant ") ||
     querystr.toLower().contains("insert ") ||
     querystr.toLower().contains("lock ") ||
     querystr.toLower().contains("revoke ") ||
     querystr.toLower().contains("truncate ") ||
     querystr.toLower().contains("update "))
    {
      QMessageBox::critical(m_customquery_diag, tr("BiblioteQ: User Error"),
			    tr("Please provide a non-destructive SQL "
			       "statement."));
      QApplication::processEvents();
      return;
    }

  populateTable(CUSTOM_QUERY, "Custom", querystr);
}

void biblioteq::slotExit(void)
{
  QSettings settings;

  settings.setValue("mainwindowState", saveState());
  settings.sync();
  slotLastWindowClosed();
  quit();
}

void biblioteq::slotExportAsCSV(void)
{
  exportAsCSV(ui.table, tr("BiblioteQ: Export Table View as CSV"));
}

void biblioteq::slotGrantPrivileges(void)
{
  QProgressDialog progress(m_members_diag);
  QString errorstr("");
  QTableWidgetItem *item = 0;
  bool error = false;

  progress.setCancelButton(0);
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.setLabelText(tr("Granting privileges..."));
  progress.setMaximum(bb.table->rowCount());
  progress.setMinimum(0);
  progress.show();
  progress.repaint();
  QApplication::processEvents();

  for(int i = 0; i < bb.table->rowCount(); i++)
    {
      if((item = bb.table->item(i, 0)))
	{
	  biblioteq_misc_functions::DBAccount
	    (item->text(),
	     m_db, biblioteq_misc_functions::UPDATE_USER,
	     errorstr);

	  if(!errorstr.isEmpty())
	    {
	      error = true;
	      addError(QString(tr("Database Error")),
		       QString(tr("An error occurred while attempting to "
				  "update the database account "
				  "for ")) +
		       item->text() +
		       QString(tr(".")),
		       errorstr, __FILE__, __LINE__);
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
      QMessageBox::critical(m_members_diag, tr("BiblioteQ: Database Error"),
			    tr("Unable to grant privileges to all of "
			       "the members."));
      QApplication::processEvents();
    }
}

void biblioteq::slotInsertBook(void)
{
  QString id("");
  biblioteq_book *book = 0;

  m_idCt += 1;
  id = QString("insert_%1").arg(m_idCt);
  book = new(std::nothrow) biblioteq_book(this, id, -1);

  if(book)
    book->insert();
}

void biblioteq::slotInsertCD(void)
{
  QString id("");
  biblioteq_cd *cd = 0;

  m_idCt += 1;
  id = QString("insert_%1").arg(m_idCt);
  cd = new(std::nothrow) biblioteq_cd(this, id, -1);

  if(cd)
    cd->insert();
}

void biblioteq::slotInsertDVD(void)
{
  QString id("");
  biblioteq_dvd *dvd = 0;

  m_idCt += 1;
  id = QString("insert_%1").arg(m_idCt);
  dvd = new(std::nothrow) biblioteq_dvd(this, id, -1);

  if(dvd)
    dvd->insert();
}

void biblioteq::slotInsertJourn(void)
{
  QString id("");
  biblioteq_journal *journal = 0;

  m_idCt += 1;
  id = QString("insert_%1").arg(m_idCt);
  journal = new(std::nothrow) biblioteq_journal(this, id, -1);

  if(journal)
    journal->insert();
}

void biblioteq::slotInsertMag(void)
{
  QString id("");
  biblioteq_magazine *magazine = 0;

  m_idCt += 1;
  id = QString("insert_%1").arg(m_idCt);
  magazine = new(std::nothrow) biblioteq_magazine(this, id, -1, "magazine");

  if(magazine)
    magazine->insert();
}

void biblioteq::slotInsertPhotograph(void)
{
  QString id("");
  biblioteq_photographcollection *photograph = 0;

  m_idCt += 1;
  id = QString("insert_%1").arg(m_idCt);
  photograph = new(std::nothrow) biblioteq_photographcollection(this, id, -1);

  if(photograph)
    photograph->insert();
}

void biblioteq::slotInsertVideoGame(void)
{
  QString id("");
  biblioteq_videogame *videogame = 0;

  m_idCt += 1;
  id = QString("insert_%1").arg(m_idCt);
  videogame = new(std::nothrow) biblioteq_videogame(this, id, -1);

  if(videogame)
    videogame->insert();
}

void biblioteq::slotLanguageChanged(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  if(action && action->isChecked())
    {
      s_locale = action->data().toString();
      QApplication::removeTranslator(s_qtTranslator);
      QApplication::removeTranslator(s_appTranslator);
#ifdef Q_OS_MAC
      s_qtTranslator->load
	("qt_" + s_locale,
	 QCoreApplication::applicationDirPath() + "/../../../Translations");
#else
      s_qtTranslator->load("qt_" + s_locale, "Translations");
#endif
      QApplication::installTranslator(s_qtTranslator);
#ifdef Q_OS_MAC
      s_appTranslator->load
	("biblioteq_" + s_locale,
	 QCoreApplication::applicationDirPath() + "/../../../Translations");
#else
      s_appTranslator->load("biblioteq_" + s_locale, "Translations");
#endif
      QApplication::installTranslator(s_appTranslator);
    }
}

void biblioteq::slotListOverdueItems(void)
{
  QString memberid = "";
  int row = bb.table->currentRow();

  if(m_members_diag->isVisible())
    memberid = biblioteq_misc_functions::getColumnString
      (bb.table, row,
       m_bbColumnHeaderIndexes.indexOf("Member ID"));
  else if(m_roles.isEmpty())
    memberid = dbUserName();

  (void) populateTable(POPULATE_ALL, "All Overdue", memberid);
  m_members_diag->showNormal();
  m_members_diag->activateWindow();
  m_members_diag->raise();
}

void biblioteq::slotListReservedItems(void)
{
  QString memberid = "";
  int row = bb.table->currentRow();

  if(row < 0)
    {
      QMessageBox::critical(m_members_diag, tr("BiblioteQ: User Error"),
			    tr("In order to list a member's reserved items, "
			       "you must first select the member."));
      QApplication::processEvents();
      return;
    }

  memberid = biblioteq_misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("Member ID"));
  (void) populateTable(POPULATE_ALL, "All Reserved", memberid);
  m_members_diag->showNormal();
  m_members_diag->activateWindow();
  m_members_diag->raise();
}

void biblioteq::slotModify(void)
{
  if(!m_db.isOpen())
    return;

  QModelIndexList list = ui.table->selectionModel()->selectedRows();
  QString oid = "";
  QString type = "";
  biblioteq_book *book = 0;
  biblioteq_cd *cd = 0;
  biblioteq_dvd *dvd = 0;
  biblioteq_grey_literature *gl = 0;
  biblioteq_journal *journal = 0;
  biblioteq_magazine *magazine = 0;
  biblioteq_main_table *table = ui.table;
  biblioteq_photographcollection *photograph = 0;
  biblioteq_videogame *videogame = 0;
  bool error = false;
  int i = 0;

  if(list.isEmpty())
    {
      QMessageBox::critical
	(this, tr("BiblioteQ: User Error"),
	 tr("Please select at least one item to modify."));
      QApplication::processEvents();
      return;
    }
  else if(list.size() >= 5)
    {
      if(QMessageBox::question
	 (this, tr("BiblioteQ: Question"),
	  tr("Are you sure that you wish to modify the ") +
	  QString::number(list.size()) +
	  tr(" selected items? BiblioteQ will exit if it's unable "
	     "to acquire resources."),
	  QMessageBox::Yes | QMessageBox::No,
	  QMessageBox::No) == QMessageBox::No)
	{
	  QApplication::processEvents();
	  list.clear();
	  return;
	}

      QApplication::processEvents();
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);
  std::stable_sort(list.begin(), list.end());

  foreach(const QModelIndex &index, list)
    {
      i = index.row();
      oid = biblioteq_misc_functions::getColumnString
	(table, i, table->columnNumber("MYOID"));
      type = biblioteq_misc_functions::getColumnString
	(table, i, table->columnNumber("Type"));
      book = 0;
      cd = 0;
      dvd = 0;
      gl = 0;
      journal = 0;
      magazine = 0;
      photograph = 0;
      videogame = 0;

      if(type.toLower() == "book")
	{
	  foreach(QWidget *w, QApplication::topLevelWidgets())
	    {
	      biblioteq_book *b = qobject_cast<biblioteq_book *> (w);

	      if(b && b->getID() == oid)
		{
		  book = b;
		  break;
		}
	    }

	  if(!book)
	    book = new(std::nothrow) biblioteq_book(this, oid, i);

	  if(book)
	    book->modify(EDITABLE);
	}
      else if(type.toLower() == "cd")
	{
	  foreach(QWidget *w, QApplication::topLevelWidgets())
	    {
	      biblioteq_cd *c = qobject_cast<biblioteq_cd *> (w);

	      if(c && c->getID() == oid)
		{
		  cd = c;
		  break;
		}
	    }

	  if(!cd)
	    cd = new(std::nothrow) biblioteq_cd(this, oid, i);

	  if(cd)
	    cd->modify(EDITABLE);
	}
      else if(type.toLower() == "dvd")
	{
	  foreach(QWidget *w, QApplication::topLevelWidgets())
	    {
	      biblioteq_dvd *d = qobject_cast<biblioteq_dvd *> (w);

	      if(d && d->getID() == oid)
		{
		  dvd = d;
		  break;
		}
	    }

	  if(!dvd)
	    dvd = new(std::nothrow) biblioteq_dvd(this, oid, i);

	  if(dvd)
	    dvd->modify(EDITABLE);
	}
      else if(type.toLower() == "grey literature")
	{
	  foreach(QWidget *w, QApplication::topLevelWidgets())
	    {
	      biblioteq_grey_literature *g =
		qobject_cast<biblioteq_grey_literature *> (w);

	      if(g && g->getID() == oid)
		{
		  gl = g;
		  break;
		}
	    }

	  if(!gl)
	    gl = new(std::nothrow) biblioteq_grey_literature(this, oid, i);

	  if(gl)
	    gl->modify(EDITABLE);
	}
      else if(type.toLower() == "journal")
	{
	  foreach(QWidget *w, QApplication::topLevelWidgets())
	    {
	      biblioteq_journal *j = qobject_cast<biblioteq_journal *> (w);

	      if(j && j->getID() == oid)
		{
		  journal = j;
		  break;
		}
	    }

	  if(!journal)
	    journal = new(std::nothrow) biblioteq_journal(this, oid, i);

	  if(journal)
	    journal->modify(EDITABLE);
	}
      else if(type.toLower() == "magazine")
	{
	  foreach(QWidget *w, QApplication::topLevelWidgets())
	    {
	      biblioteq_magazine *m = qobject_cast<biblioteq_magazine *> (w);

	      /*
	      ** The class biblioteq_journal inherits biblioteq_magazine.
	      */

	      if(!qobject_cast<biblioteq_journal *> (w))
		if(m && m->getID() == oid)
		  {
		    magazine = m;
		    break;
		  }
	    }

	  if(!magazine)
	    magazine = new(std::nothrow) biblioteq_magazine
	      (this,
	       oid, i, "magazine");

	  if(magazine)
	    magazine->modify(EDITABLE);
	}
      else if(type.toLower() == "photograph collection")
	{
	  foreach(QWidget *w, QApplication::topLevelWidgets())
	    {
	      biblioteq_photographcollection *p =
		qobject_cast<biblioteq_photographcollection *> (w);

	      if(p && p->getID() == oid)
		{
		  photograph = p;
		  break;
		}
	    }

	  if(!photograph)
	    photograph = new(std::nothrow) biblioteq_photographcollection
	      (this, oid, i);

	  if(photograph)
	    photograph->modify(EDITABLE);
	}
      else if(type.toLower() == "video game")
	{
	  foreach(QWidget *w, QApplication::topLevelWidgets())
	    {
	      biblioteq_videogame *v = qobject_cast<biblioteq_videogame *> (w);

	      if(v && v->getID() == oid)
		{
		  videogame = v;
		  break;
		}
	    }

	  if(!videogame)
	    videogame = new(std::nothrow) biblioteq_videogame(this, oid, i);

	  if(videogame)
	    videogame->modify(EDITABLE);
	}
      else
	{
	  error = true;
	  break;
	}
    }

  list.clear();
  QApplication::restoreOverrideCursor();

  if(error)
    {
      QMessageBox::critical(this, tr("BiblioteQ: Error"),
			    tr("Unable to determine the selected item's "
			       "type."));
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

void biblioteq::slotPrintReservationHistory(void)
{
  QPrinter printer;
  QPrintDialog dialog(&printer, m_history_diag);
  QString html = "<html>";
  QTextDocument document;

  if(history.table->rowCount() == 0)
    {
      if(m_members_diag->isVisible())
	QMessageBox::critical(m_history_diag, tr("BiblioteQ: User Error"),
			      tr("The selected member does not yet have a "
				 "reservation history to print."));
      else
	QMessageBox::critical(m_history_diag, tr("BiblioteQ: User Error"),
			      tr("You do not yet have a reservation history "
				 "to print."));

      QApplication::processEvents();
      return;
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);
  html = tr("Reservation History") + "<br><br>";
  html += "<table border=1>";
  html += "<tr>";

  for(int i = 0; i < history.table->columnCount(); i++)
    if(!history.table->isColumnHidden(i))
      html += "<th>" + history.table->horizontalHeaderItem(i)->text() +
	"</th>";

  html += "</tr>";

  for(int i = 0; i < history.table->rowCount(); i++)
    {
      html += "<tr>";

      for(int j = 0; j < history.table->columnCount(); j++)
	if(!history.table->isColumnHidden(j))
	  html += "<td>" + history.table->item(i, j)->text() + "</td>";

      html += "</tr>";
    }

  html += "</table>";
  html += "</html>";
  QApplication::restoreOverrideCursor();
  printer.setPageSize(QPrinter::Letter);
  printer.setColorMode(QPrinter::GrayScale);

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      document.setHtml(html);
      document.print(&printer);
    }

  QApplication::processEvents();
}

void biblioteq::slotPrintReserved(void)
{
  QMap<QString, QString> memberinfo;
  QPrinter printer;
  QPrintDialog dialog(&printer, m_members_diag);
  QString errorstr = "";
  QString memberid = "";
  QString str = "";
  QStringList itemsList;
  QTextDocument document;
  int itemsReserved = 0;
  int row = bb.table->currentRow();

  if(row < 0)
    {
      QMessageBox::critical
	(m_members_diag, tr("BiblioteQ: User Error"),
	 tr("In order to print a member's reserved items, "
	    "you must first select the member."));
      QApplication::processEvents();
      return;
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);
  itemsReserved = biblioteq_misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("Books Reserved")).
    toInt() +
    biblioteq_misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("CDs Reserved")).toInt() +
    biblioteq_misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("DVDs Reserved")).toInt() +
    biblioteq_misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("Journals Reserved")).
    toInt() +
    biblioteq_misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("Magazines Reserved")).
    toInt() +
    biblioteq_misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("Video Games Reserved")).
    toInt();
  QApplication::restoreOverrideCursor();

  if(itemsReserved < 1)
    {
      QMessageBox::critical(m_members_diag, tr("BiblioteQ: User Error"),
			    tr("The member that you selected does not have "
			       "any reserved items."));
      QApplication::processEvents();
      return;
    }

  memberid = biblioteq_misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("Member ID"));
  memberinfo["firstname"] = biblioteq_misc_functions::getColumnString
    (bb.table, row,
     m_bbColumnHeaderIndexes.indexOf("First Name"));
  memberinfo["lastname"] = biblioteq_misc_functions::getColumnString
    (bb.table, row,
     m_bbColumnHeaderIndexes.indexOf("Last Name"));
  QApplication::setOverrideCursor(Qt::WaitCursor);
  itemsList = biblioteq_misc_functions::getReservedItems
    (memberid, m_db, errorstr);
  QApplication::restoreOverrideCursor();

  if(errorstr.isEmpty())
    {
      str = "<html>\n";
      str += tr("Reserved Items for ") + memberinfo.value("lastname") +
	tr(", ") + memberinfo.value("firstname") + "<br><br>";

      for(int i = 0; i < itemsList.size(); i++)
	str += itemsList[i] + "<br><br>";

      str = str.mid(0, str.length() - 8);
      str += "</html>";
      printer.setPageSize(QPrinter::Letter);
      printer.setColorMode(QPrinter::GrayScale);

      if(dialog.exec() == QDialog::Accepted)
	{
	  QApplication::processEvents();
	  document.setHtml(str);
	  document.print(&printer);
	}

      QApplication::processEvents();
    }
  else
    {
      addError(QString(tr("Database Error")),
	       QString(tr("Unable to determine the reserved items for "
			  "the selected member.")),
	       errorstr, __FILE__, __LINE__);
      QMessageBox::critical(m_members_diag, tr("BiblioteQ: Database Error"),
			    tr("Unable to determine the reserved items for "
			       "the selected member."));
      QApplication::processEvents();
    }

  itemsList.clear();
  memberinfo.clear();
}

void biblioteq::slotPrintView(void)
{
  QPrinter printer;
  QPrintDialog dialog(&printer, this);
  QString html = "<html>";
  QTextDocument document;

  QApplication::setOverrideCursor(Qt::WaitCursor);
  html += "<table border=1>";
  html += "<tr>";

  for(int i = 0; i < ui.table->columnCount(); i++)
    if(!ui.table->isColumnHidden(i))
      html += "<th>" + ui.table->horizontalHeaderItem(i)->text() +
	"</th>";

  html += "</tr>";
  
  for(int i = 0; i < ui.table->rowCount(); i++)
    {
      html += "<tr>";

      for(int j = 0; j < ui.table->columnCount(); j++)
	if(!ui.table->isColumnHidden(j))
	  html += "<td>" + ui.table->item(i, j)->text() + "</td>";

      html += "</tr>";
    }

  html += "</table>";
  html += "</html>";
  QApplication::restoreOverrideCursor();
  printer.setPaperSize(QPrinter::Letter);
  printer.setColorMode(QPrinter::GrayScale);
  printer.setOrientation(QPrinter::Landscape);

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      document.setHtml(html);
      document.print(&printer);
    }

  QApplication::processEvents();
}

void biblioteq::slotRefresh(void)
{
  if(m_db.isOpen())
    {
      QApplication::processEvents();

      QString str = "";
      QVariant data(ui.menu_Category->defaultAction() ?
		    ui.menu_Category->defaultAction()->data().
		    toString() : "All");

      if(data.toString() == "All Overdue" && m_roles.isEmpty())
	str = dbUserName();
      else if(data.toString() == "All Requested" && m_roles.isEmpty())
	str = dbUserName();
      else if(data.toString() == "All Reserved" && m_roles.isEmpty())
	str = dbUserName();
      else if(data.toString() == "All Reserved")
	str = "%";

      (void) populateTable(POPULATE_ALL, data.toString(), str.trimmed());
    }
}

void biblioteq::slotReserveCopy(void)
{
  QString errorstr = "";
  QString oid = "";
  QString type = "";
  int availability = 0;
  int row = ui.table->currentRow();

  if(row < 0)
    {
      if(m_members_diag->isVisible())
	QMessageBox::critical(m_members_diag, tr("BiblioteQ: User Error"),
			      tr("In order to reserve an item, you must "
				 "first select it."));
      else
	QMessageBox::critical(this, tr("BiblioteQ: User Error"),
			      tr("In order to reserve an item, you must "
				 "first select it."));

      QApplication::processEvents();
      return;
    }

  type = biblioteq_misc_functions::getColumnString
    (ui.table, row, ui.table->columnNumber("Type"));

  if(type == "Grey Literature")
    {
      if(m_members_diag->isVisible())
	QMessageBox::critical(m_members_diag, tr("BiblioteQ: User Error"),
			      tr("Grey literature may not be reserved."));
      else
	QMessageBox::critical(this, tr("BiblioteQ: User Error"),
			      tr("Grey literature may not be reserved."));

      QApplication::processEvents();
      return;
    }
  else if(type == "Photograph Collection")
    {
      if(m_members_diag->isVisible())
	QMessageBox::critical(m_members_diag, tr("BiblioteQ: User Error"),
			      tr("Photographs may not be reserved."));
      else
	QMessageBox::critical(this, tr("BiblioteQ: User Error"),
			      tr("Photographs may not be reserved."));

      QApplication::processEvents();
      return;
    }

  oid = biblioteq_misc_functions::getColumnString
    (ui.table, row, ui.table->columnNumber("MYOID"));
  QApplication::setOverrideCursor(Qt::WaitCursor);
  availability = biblioteq_misc_functions::getAvailability
    (oid, m_db, type, errorstr).toInt();
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    {
      addError(QString(tr("Database Error")),
	       QString(tr("Unable to determine the availability of "
			  "the selected item.")),
	       errorstr, __FILE__, __LINE__);

      if(m_members_diag->isVisible())
	QMessageBox::critical(m_members_diag, tr("BiblioteQ: Database Error"),
			      tr("Unable to determine the availability of "
				 "the selected item."));
      else
	QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			      tr("Unable to determine the availability of "
				 "the selected item."));

      QApplication::processEvents();
      return;
    }

  if(availability < 1)
    {
      if(m_members_diag->isVisible())
	QMessageBox::critical(m_members_diag, tr("BiblioteQ: User Error"),
			      tr("It appears that the item you selected "
				 "is not available for reservation."));
      else
	QMessageBox::critical(this, tr("BiblioteQ: User Error"),
			      tr("It appears that the item you selected "
				 "is not available for reservation."));

      QApplication::processEvents();
      return;
    }

  slotShowMembersBrowser();

  if(!ui.actionPopulate_Members_Browser_Table_on_Display->isChecked())
    {
      if(QMessageBox::question(m_members_diag, tr("BiblioteQ: Question"),
			       tr("Would you like to retrieve the list of "
				  "members?"),
			       QMessageBox::Yes | QMessageBox::No,
			       QMessageBox::No) == QMessageBox::Yes)
	{
	  QApplication::processEvents();
	  slotPopulateMembersBrowser();
	}

      QApplication::processEvents();
    }

  if(bb.table->currentRow() < 0)
    bb.table->selectRow(0);
}

void biblioteq::slotReset(void)
{
  if(m_all_diag->isVisible())
    {
      QAction *action = qobject_cast<QAction *> (sender());

      if(action != 0)
	{
	  QList<QAction *> actions = al.resetButton->menu()->actions();

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
	      al.price->setValue(-0.01);
	      al.price->setFocus();
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
	      al.quantity->setValue(0);
	      al.quantity->setFocus();
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

	  actions.clear();
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
  er.table->setCurrentItem(0);
  er.table->setColumnCount(0);
  er.table->setRowCount(0);
  er.table->setColumnCount(0);
  er.table->scrollToTop();
  er.table->horizontalScrollBar()->setValue(0);
  er.table->setColumnCount(list.size());
  er.table->setHorizontalHeaderLabels(list);
  list.clear();
  er.table->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);

  for(int i = 0; i < er.table->columnCount() - 1; i++)
    er.table->resizeColumnToContents(i);

  if(m_error_bar_label != 0)
    {
      m_error_bar_label->setIcon(QIcon(":/16x16/ok.png"));
      m_error_bar_label->setToolTip(tr("Empty Error Log"));
    }
}

void biblioteq::slotResetLoginDialog(void)
{
  br.filename->clear();
  br.role->setCurrentIndex(1);
  br.password->setText("xbook_guest");
  br.userid->setText("xbook_guest");

  int index = 0;
  QSettings settings;

  index = br.branch_name->findText(settings.value("previous_branch_name").
				   toString());

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
  QObject *object = qobject_cast<QObject *> (sender());
  QObject *parent = 0;

  if(object != 0 && object->parent() != 0)
    {
      if(object->parent() == ui.table)
	if(!ui.actionAutomatically_Resize_Column_Widths->isChecked())
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

  if(pass.password->text().length() < 8)
    {
      QMessageBox::critical
	(m_pass_diag, tr("BiblioteQ: User Error"),
	 tr("The password must be at least eight characters "
	    "long."));
      QApplication::processEvents();
      pass.password->selectAll();
      pass.password->setFocus();
      return;
    }
  else if(pass.password->text() != pass.passwordAgain->text())
    {
      QMessageBox::critical
	(m_pass_diag, tr("BiblioteQ: User Error"),
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
  pass.password->clear();
  pass.passwordAgain->clear();

  if(!errorstr.isEmpty())
    {
      addError(QString(tr("Database Error")),
	       QString(tr("Unable to save the new password.")),
	       errorstr, __FILE__, __LINE__);
      QMessageBox::critical(m_pass_diag, tr("BiblioteQ: Database Error"),
			    tr("Unable to save the new password."));
      QApplication::processEvents();
    }
  else
    m_pass_diag->close();
}

void biblioteq::slotSearch(void)
{
  if(!m_db.isOpen())
    return;
  else if(!al.resetButton->isEnabled())
    {
      if(!m_all_diag->isVisible())
	m_all_diag->updateGeometry();

      biblioteq_misc_functions::center(m_all_diag, this);
      m_all_diag->showNormal();
      m_all_diag->activateWindow();
      m_all_diag->raise();
      return;
    }

  /*
  ** Hide certain fields if we're a regular user.
  */

  biblioteq_misc_functions::hideAdminFields(m_all_diag, m_roles);
  al.idnumber->clear();
  al.title->clear();
  al.publisher->clear();
  al.categories_checkbox->setChecked(false);
  al.category->clear();
  al.publication_date->setDate(QDate::fromString("2001", "yyyy"));
  al.publication_date_enabled->setChecked(false);
  al.price->setMinimum(-0.01);
  al.price->setValue(-0.01);
  al.quantity->setMinimum(0);
  al.quantity->setValue(0);
  al.abstract_checkbox->setChecked(false);
  al.description->clear();
  al.language->clear();
  al.monetary_units->clear();
  al.location->clear();
  al.keyword->clear();
  al.keywords_checkbox->setChecked(false);
  al.available->setChecked(false);
  al.caseinsensitive->setChecked(false);
  al.photograph_reminder_label->setVisible(true); /*
						  ** Hidden by
						  ** hideAdminFields().
						  */

  /*
  ** Populate combination boxes.
  */

  QString errorstr("");

  QApplication::setOverrideCursor(Qt::WaitCursor);
  al.language->addItems
    (biblioteq_misc_functions::getLanguages(m_db,
					    errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the languages.")),
       errorstr, __FILE__, __LINE__);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  al.monetary_units->addItems
    (biblioteq_misc_functions::getMonetaryUnits(m_db,
						errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the monetary units.")),
       errorstr, __FILE__, __LINE__);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  al.location->addItems
    (biblioteq_misc_functions::getLocations(m_db,
					    "",
					    errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the locations.")),
       errorstr, __FILE__, __LINE__);

  al.language->insertItem(0, tr("Any"));
  al.monetary_units->insertItem(0, tr("Any"));
  al.location->insertItem(0, tr("Any"));
  al.language->addItem(tr("UNKNOWN"));
  al.monetary_units->addItem(tr("UNKNOWN"));
  al.location->addItem(tr("UNKNOWN"));
  al.location->setCurrentIndex(0);
  al.language->setCurrentIndex(0);
  al.monetary_units->setCurrentIndex(0);
  al.idnumber->setFocus();

  if(!m_all_diag->isVisible())
    m_all_diag->updateGeometry();

  static bool resized = false;

  if(!resized)
    m_all_diag->resize(qRound(0.85 * size().width()),
		       qRound(0.85 * size().height()));

  resized = true;

  if(!m_all_diag->isVisible())
    biblioteq_misc_functions::center(m_all_diag, this);

  m_all_diag->showNormal();
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

  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setDirectory(QDir::homePath());
  dialog.setNameFilter("SQLite Database (*.sqlite)");
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowTitle(tr("BiblioteQ: SQLite Database Selection"));
  dialog.exec();
  QApplication::processEvents();

  if(dialog.result() == QDialog::Accepted)
    br.filename->setText(dialog.selectedFiles().value(0));
}

void biblioteq::slotSetColumns(void)
{
  QString typefilter = ui.menu_Category->defaultAction() ?
    ui.menu_Category->defaultAction()->data().toString() : "All";

  for(int i = 0; i < m_configToolMenu->actions().size(); i++)
    {
      ui.table->setColumnHidden
	(i, !m_configToolMenu->actions().at(i)->isChecked());
      ui.table->recordColumnHidden
	(dbUserName(),
	 typefilter, i, !m_configToolMenu->actions().at(i)->
	 isChecked());
    }
}

void biblioteq::slotSetFonts(void)
{
  QFontDialog dialog(this);

  dialog.setOption(QFontDialog::DontUseNativeDialog);
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
  static bool resized = false;

  if(!resized)
    m_admin_diag->resize(qRound(0.85 * size().width()),
			 qRound(0.85 * size().height()));

  resized = true;
  biblioteq_misc_functions::center(m_admin_diag, this);
  m_admin_diag->showNormal();
  m_admin_diag->activateWindow();
  m_admin_diag->raise();

  if(ui.actionPopulate_Administrator_Browser_Table_on_Display->isChecked())
    slotRefreshAdminList();
}

void biblioteq::slotShowChangePassword(void)
{
  pass.userid->setText(dbUserName());
  pass.password->clear();
  pass.passwordAgain->clear();
  pass.password->setFocus();
  m_pass_diag->show();
}

void biblioteq::slotShowConnectionDB(void)
{
  if(m_db.isOpen())
    return;

  slotBranchChanged();
}

void biblioteq::slotShowCustomQuery(void)
{
  if(cq.tables_t->columnCount() == 0)
    slotRefreshCustomQuery();

  static bool resized = false;

  if(!resized)
    m_customquery_diag->resize(qRound(0.85 * size().width()),
			       qRound(0.85 * size().height()));

  resized = true;
  biblioteq_misc_functions::center(m_customquery_diag, this);
  m_customquery_diag->showNormal();
  m_customquery_diag->activateWindow();
  m_customquery_diag->raise();
}

void biblioteq::slotShowDbEnumerations(void)
{
  db_enumerations->show
    (this,
     ui.actionPopulate_Database_Enumerations_Browser_on_Display->
     isChecked());
}

void biblioteq::slotShowErrorDialog(void)
{
  er.table->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);

  for(int i = 0; i < er.table->columnCount() - 1; i++)
    er.table->resizeColumnToContents(i);

  static bool resized = false;

  if(!resized)
    m_error_diag->resize(qRound(0.85 * size().width()),
			 qRound(0.85 * size().height()));

  resized = true;
  biblioteq_misc_functions::center(m_error_diag, this);
  m_error_diag->showNormal();
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
  bb.filterBox->setCheckState(Qt::Checked);
  bb.filtertype->setCurrentIndex(0);
  bb.filter->setFocus();

  for(int i = 0; i < bb.table->columnCount() - 1; i++)
    bb.table->resizeColumnToContents(i);

  static bool resized = false;

  if(!resized)
    m_members_diag->resize(qRound(0.85 * size().width()),
			   qRound(0.85 * size().height()));

  resized = true;
  biblioteq_misc_functions::center(m_members_diag, this);
  m_members_diag->showNormal();
  m_members_diag->activateWindow();
  m_members_diag->raise();

  if(ui.actionPopulate_Members_Browser_Table_on_Display->isChecked())
    slotPopulateMembersBrowser();
}

void biblioteq::slotShowMenu(void)
{
  if(sender() == ui.configTool)
    m_configToolMenu->exec(QCursor::pos());
  else if(sender() == ui.createTool)
    {
      QMenu menu(this);

      connect(menu.addAction(tr("Add &Book...")),
	      SIGNAL(triggered(void)), this, SLOT(slotInsertBook(void)));
      // menu.addAction(tr("Add &Cassette Tape..."));
      connect(menu.addAction(tr("Add &DVD...")),
	      SIGNAL(triggered(void)), this, SLOT(slotInsertDVD(void)));
      connect(menu.addAction(tr("Add &Grey Literature...")),
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotInsertGreyLiterature(void)));
      connect(menu.addAction(tr("Add &Journal...")),
	      SIGNAL(triggered(void)), this, SLOT(slotInsertJourn(void)));
      connect(menu.addAction(tr("Add &Magazine...")),
	      SIGNAL(triggered(void)), this, SLOT(slotInsertMag(void)));
      connect(menu.addAction(tr("Add Music &CD...")),
	      SIGNAL(triggered(void)), this, SLOT(slotInsertCD(void)));
      // menu.addAction(tr("Add &Newspaper..."));
      connect(menu.addAction(tr("Add &Photograph Collection...")),
	      SIGNAL(triggered(void)), this, SLOT(slotInsertPhotograph(void)));
      connect(menu.addAction(tr("Add &Video Game...")),
	      SIGNAL(triggered(void)), this, SLOT(slotInsertVideoGame(void)));
      // menu.addAction(tr("Add &VHS..."));
      // menu.addAction(tr("Add &Vinyl Record..."));
      menu.exec(QCursor::pos());
    }
  else if(sender() == ui.searchTool)
    {
      QMenu menu(this);

      connect(menu.addAction(tr("General &Search...")),
	      SIGNAL(triggered(void)), this, SLOT(slotSearch(void)));
      menu.addSeparator();
      connect(menu.addAction(tr("&Book Search...")),
	      SIGNAL(triggered(void)), this, SLOT(slotBookSearch(void)));
      connect(menu.addAction(tr("&DVD Search...")),
	      SIGNAL(triggered(void)), this, SLOT(slotDVDSearch(void)));
      connect(menu.addAction(tr("&Grey Literature Search...")),
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotGreyLiteratureSearch(void)));
      connect(menu.addAction(tr("&Journal Search...")),
	      SIGNAL(triggered(void)), this, SLOT(slotJournSearch(void)));
      connect(menu.addAction(tr("&Magazine Search...")),
	      SIGNAL(triggered(void)), this, SLOT(slotMagSearch(void)));
      connect(menu.addAction(tr("Music &CD Search...")),
	      SIGNAL(triggered(void)), this, SLOT(slotCDSearch(void)));
      connect
	(menu.addAction(tr("&Photograph Collection Search...")),
	 SIGNAL(triggered(void)), this, SLOT(slotPhotographSearch(void)));
      connect
	(menu.addAction(tr("&Video Game Search...")),
	 SIGNAL(triggered(void)), this, SLOT(slotVideoGameSearch(void)));
      menu.exec(QCursor::pos());
    }
}

void biblioteq::slotShowNext(void)
{
  QTableWidget *table = 0;
  int row = -1;

  table = bb.table;
  row = table->currentRow();

  if(row == (table->rowCount() - 1))
    row = 0;
  else
    row += 1;

  if(m_history_diag->isVisible())
    {
      table->clearSelection();
      table->selectRow(row);
      slotShowHistory();
    }
  else
    {
      QString str("");

      if(userinfo_diag->haveMemberChanges(str))
	{
	  if(QMessageBox::question
	     (userinfo_diag, tr("BiblioteQ: Question"),
	      tr("Your changes have not been saved. Continue?\n%1").arg(str),
	      QMessageBox::Yes | QMessageBox::No,
	      QMessageBox::No) == QMessageBox::No)
	    {
	      QApplication::processEvents();
	      return;
	    }

	  QApplication::processEvents();
	}

      table->clearSelection();
      table->selectRow(row);
      slotModifyBorrower();
    }
}

void biblioteq::slotShowPrev(void)
{
  QTableWidget *table = 0;
  int row = -1;

  table = bb.table;
  row = table->currentRow();

  if(row == 0)
    row = table->rowCount() - 1;
  else
    row -= 1;

  if(row < 0)
    row = 0;

  if(m_history_diag->isVisible())
    {
      table->clearSelection();
      table->selectRow(row);
      slotShowHistory();
    }
  else
    {
      QString str("");

      if(userinfo_diag->haveMemberChanges(str))
	{
	  if(QMessageBox::question
	     (userinfo_diag, tr("BiblioteQ: Question"),
	      tr("Your changes have not been saved. Continue?\n%1").arg(str),
	      QMessageBox::Yes | QMessageBox::No,
	      QMessageBox::No) == QMessageBox::No)
	    {
	      QApplication::processEvents();
	      return;
	    }

	  QApplication::processEvents();
	}

      table->clearSelection();
      table->selectRow(row);
      slotModifyBorrower();
    }
}

void biblioteq::slotSqliteFileSelected(bool state)
{
  Q_UNUSED(state);

  QAction *action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  slotDisconnect();
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
  QString itemType = "";
  QString oid = "";
  Qt::SortOrder order;
  int i = 0;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(ui.table->horizontalHeader()->sortIndicatorOrder() !=
     Qt::AscendingOrder)
    order = Qt::DescendingOrder;
  else
    order = Qt::AscendingOrder;

  ui.table->horizontalHeader()->setSortIndicator(column, order);
  ui.table->sortByColumn(column, order);

  for(i = 0; i < ui.table->rowCount(); i++)
    {
      oid = biblioteq_misc_functions::getColumnString
	(ui.table, i, ui.table->columnNumber("MYOID"));
      itemType = biblioteq_misc_functions::getColumnString
	(ui.table, i, ui.table->columnNumber("Type"));
      itemType = itemType.toLower().remove(" ");
      updateRows(oid, i, itemType);
    }

  QApplication::restoreOverrideCursor();
}

void biblioteq::slotViewDetails(void)
{
  QModelIndexList list = ui.table->selectionModel()->selectedRows();
  QString oid = "";
  QString type = "";
  biblioteq_book *book = 0;
  biblioteq_cd *cd = 0;
  biblioteq_dvd *dvd = 0;
  biblioteq_grey_literature *gl = 0;
  biblioteq_journal *journal = 0;
  biblioteq_magazine *magazine = 0;
  biblioteq_main_table *table = ui.table;
  biblioteq_photographcollection *photograph = 0;
  biblioteq_videogame *videogame = 0;
  bool error = false;
  int i = 0;

  if(list.isEmpty())
    {
      QMessageBox::critical(this, tr("BiblioteQ: User Error"),
			    tr("Please select at least one item to view."));
      QApplication::processEvents();
      return;
    }
  else if(list.size() >= 5)
    {
      if(QMessageBox::question(this, tr("BiblioteQ: Question"),
			       tr("Are you sure that you wish to view the ") +
			       QString::number(list.size()) +
			       tr(" selected items? BiblioteQ will exit if "
				  "it's unable "
				  "to acquire resources."),
			       QMessageBox::Yes | QMessageBox::No,
			       QMessageBox::No) == QMessageBox::No)
	{
	  QApplication::processEvents();
	  list.clear();
	  return;
	}

      QApplication::processEvents();
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);
  std::stable_sort(list.begin(), list.end());

  foreach(const QModelIndex &index, list)
    {
      i = index.row();
      oid = biblioteq_misc_functions::getColumnString
	(table, i, table->columnNumber("MYOID"));
      type = biblioteq_misc_functions::getColumnString
	(table, i, table->columnNumber("Type"));
      book = 0;
      cd = 0;
      dvd = 0;
      gl = 0;
      journal = 0;
      magazine = 0;
      photograph = 0;
      videogame = 0;

      if(type.toLower() == "book")
	{
	  foreach(QWidget *w, QApplication::topLevelWidgets())
	    {
	      biblioteq_book *b = qobject_cast<biblioteq_book *> (w);

	      if(b && b->getID() == oid)
		{
		  book = b;
		  break;
		}
	    }

	  if(!book)
	    book = new(std::nothrow) biblioteq_book(this, oid, i);

	  if(book)
	    book->modify(VIEW_ONLY);
	}
      else if(type.toLower() == "cd")
	{
	  foreach(QWidget *w, QApplication::topLevelWidgets())
	    {
	      biblioteq_cd *c = qobject_cast<biblioteq_cd *> (w);

	      if(c && c->getID() == oid)
		{
		  cd = c;
		  break;
		}
	    }

	  if(!cd)
	    cd = new(std::nothrow) biblioteq_cd(this, oid, i);

	  if(cd)
	    cd->modify(VIEW_ONLY);
	}
      else if(type.toLower() == "dvd")
	{
	  foreach(QWidget *w, QApplication::topLevelWidgets())
	    {
	      biblioteq_dvd *d = qobject_cast<biblioteq_dvd *> (w);

	      if(d && d->getID() == oid)
		{
		  dvd = d;
		  break;
		}
	    }

	  if(!dvd)
	    dvd = new(std::nothrow) biblioteq_dvd(this, oid, i);

	  if(dvd)
	    dvd->modify(VIEW_ONLY);
	}
      else if(type.toLower() == "grey literature")
	{
	  foreach(QWidget *w, QApplication::topLevelWidgets())
	    {
	      biblioteq_grey_literature *g =
		qobject_cast<biblioteq_grey_literature *> (w);

	      if(g && g->getID() == oid)
		{
		  gl = g;
		  break;
		}
	    }

	  if(!gl)
	    gl = new(std::nothrow) biblioteq_grey_literature(this, oid, i);

	  if(gl)
	    gl->modify(VIEW_ONLY);
	}
      else if(type.toLower() == "journal")
	{
	  foreach(QWidget *w, QApplication::topLevelWidgets())
	    {
	      biblioteq_journal *j = qobject_cast<biblioteq_journal *> (w);

	      if(j && j->getID() == oid)
		{
		  journal = j;
		  break;
		}
	    }

	  if(!journal)
	    journal = new(std::nothrow) biblioteq_journal(this, oid, i);

	  if(journal)
	    journal->modify(VIEW_ONLY);
	}
      else if(type.toLower() == "magazine")
	{
	  foreach(QWidget *w, QApplication::topLevelWidgets())
	    {
	      biblioteq_magazine *m = qobject_cast<biblioteq_magazine *> (w);

	      /*
	      ** The class biblioteq_journal inherits biblioteq_magazine.
	      */

	      if(!qobject_cast<biblioteq_journal *> (w))
		if(m && m->getID() == oid)
		  {
		    magazine = m;
		    break;
		  }
	    }

	  if(!magazine)
	    magazine = new(std::nothrow) biblioteq_magazine
	      (this, oid, i, "magazine");

	  if(magazine)
	    magazine->modify(VIEW_ONLY);
	}
      else if(type.toLower() == "photograph collection")
	{
	  foreach(QWidget *w, QApplication::topLevelWidgets())
	    {
	      biblioteq_photographcollection *p =
		qobject_cast<biblioteq_photographcollection *> (w);

	      if(p && p->getID() == oid)
		{
		  photograph = p;
		  break;
		}
	    }

	  if(!photograph)
	    photograph = new(std::nothrow) biblioteq_photographcollection
	      (this, oid, i);

	  if(photograph)
	    photograph->modify(VIEW_ONLY);
	}
      else if(type.toLower() == "video game")
	{
	  foreach(QWidget *w, QApplication::topLevelWidgets())
	    {
	      biblioteq_videogame *v = qobject_cast<biblioteq_videogame *> (w);

	      if(v && v->getID() == oid)
		{
		  videogame = v;
		  break;
		}
	    }

	  if(!videogame)
	    videogame = new(std::nothrow) biblioteq_videogame
	      (this, oid, i);

	  if(videogame)
	    videogame->modify(VIEW_ONLY);
	}
      else
	{
	  error = true;
	  break;
	}
    }

  list.clear();
  QApplication::restoreOverrideCursor();

  if(error)
    {
      QMessageBox::critical(this, tr("BiblioteQ: Error"),
			    tr("Unable to determine the selected item's "
			       "type."));
      QApplication::processEvents();
    }
}

void biblioteq::updateItemWindows(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  foreach(QWidget *w, QApplication::topLevelWidgets())
    {
      biblioteq_book *book = qobject_cast<biblioteq_book *> (w);
      biblioteq_cd *cd = qobject_cast<biblioteq_cd *> (w);
      biblioteq_dvd *dvd = qobject_cast<biblioteq_dvd *> (w);
      biblioteq_grey_literature *gl = qobject_cast
	<biblioteq_grey_literature *> (w);
      biblioteq_journal *journal = qobject_cast<biblioteq_journal *> (w);
      biblioteq_magazine *magazine = qobject_cast<biblioteq_magazine *> (w);
      biblioteq_photographcollection *photograph =
	qobject_cast<biblioteq_photographcollection *> (w);
      biblioteq_videogame *videogame = qobject_cast<biblioteq_videogame *> (w);

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

void biblioteq::updateRows(const QString &oid,
			   const int row,
			   const QString &itemType)
{
  if(itemType == "book")
    {
      foreach(QWidget *w, QApplication::topLevelWidgets())
	{
	  biblioteq_book *book = qobject_cast<biblioteq_book *> (w);

	  if(book && book->getID() == oid)
	    {
	      book->updateRow(row);
	      break;
	    }
	}
    }
  else if(itemType == "cd")
    {
      foreach(QWidget *w, QApplication::topLevelWidgets())
	{
	  biblioteq_cd *cd = qobject_cast<biblioteq_cd *> (w);

	  if(cd && cd->getID() == oid)
	    {
	      cd->updateRow(row);
	      break;
	    }
	}
    }
  else if(itemType == "dvd")
    {
      foreach(QWidget *w, QApplication::topLevelWidgets())
	{
	  biblioteq_dvd *dvd = qobject_cast<biblioteq_dvd *> (w);

	  if(dvd && dvd->getID() == oid)
	    {
	      dvd->updateRow(row);
	      break;
	    }
	}
    }
  else if(itemType == "greyliterature")
    {
      foreach(QWidget *w, QApplication::topLevelWidgets())
	{
	  biblioteq_grey_literature *gl =
	    qobject_cast<biblioteq_grey_literature *> (w);

	  if(gl && gl->getID() == oid)
	    {
	      gl->updateRow(row);
	      break;
	    }
	}
    }
  else if(itemType == "journal")
    {
      foreach(QWidget *w, QApplication::topLevelWidgets())
	{
	  biblioteq_journal *journal = qobject_cast<biblioteq_journal *> (w);

	  if(journal && journal->getID() == oid)
	    {
	      journal->updateRow(row);
	      break;
	    }
	}
    }
  else if(itemType == "magazine")
    {
      foreach(QWidget *w, QApplication::topLevelWidgets())
	{
	  biblioteq_magazine *magazine =
	    qobject_cast<biblioteq_magazine *> (w);

	  /*
	  ** The class biblioteq_journal inherits biblioteq_magazine.
	  */

	  if(!qobject_cast<biblioteq_journal *> (w))
	    if(magazine && magazine->getID() == oid)
	      {
		magazine->updateRow(row);
		break;
	      }
	}
    }
  else if(itemType == "photographcollection")
    {
      foreach(QWidget *w, QApplication::topLevelWidgets())
	{
	  biblioteq_photographcollection *photograph =
	    qobject_cast<biblioteq_photographcollection *> (w);

	  if(photograph && photograph->getID() == oid)
	    {
	      photograph->updateRow(row);
	      break;
	    }
	}
    }
  else if(itemType == "videogame")
    {
      foreach(QWidget *w, QApplication::topLevelWidgets())
	{
	  biblioteq_videogame *videogame =
	    qobject_cast<biblioteq_videogame *> (w);

	  if(videogame && videogame->getID() == oid)
	    {
	      videogame->updateRow(row);
	      break;
	    }
	}
    }
}

void biblioteq::updateMembersBrowser(const QString &memberid)
{
  QMap<QString, QString> counts;
  QString errorstr = "";
  QString str = "";
  int i = 0;

  /*
  ** Called from the Borrowers Editor when an item has been updated.
  */

  QApplication::setOverrideCursor(Qt::WaitCursor);
  counts = biblioteq_misc_functions::getItemsReservedCounts
    (m_db, memberid, errorstr);
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    addError(QString(tr("Database Error")),
	     QString(tr("Unable to retrieve the number of reserved items "
			"of the selected member.")),
	     errorstr, __FILE__, __LINE__);
  else
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);

      for(i = 0; i < bb.table->rowCount(); i++)
	{
	  str = biblioteq_misc_functions::getColumnString
	    (bb.table, i, m_bbColumnHeaderIndexes.indexOf("Member ID"));

	  if(str == memberid)
	    {
	      biblioteq_misc_functions::updateColumn
		(bb.table, i,
		 m_bbColumnHeaderIndexes.indexOf("Books Reserved"),
		 counts.value("numbooks"));
	      biblioteq_misc_functions::updateColumn
		(bb.table, i, m_bbColumnHeaderIndexes.indexOf("CDs Reserved"),
		 counts.value("numcds"));
	      biblioteq_misc_functions::updateColumn
		(bb.table, i, m_bbColumnHeaderIndexes.indexOf("DVDs Reserved"),
		 counts.value("numdvds"));
	      biblioteq_misc_functions::updateColumn
		(bb.table, i,
		 m_bbColumnHeaderIndexes.indexOf("Journals Reserved"),
		 counts.value("numjournals"));
	      biblioteq_misc_functions::updateColumn
		(bb.table, i,
		 m_bbColumnHeaderIndexes.indexOf("Magazines Reserved"),
		 counts.value("nummagazines"));
	      biblioteq_misc_functions::updateColumn
		(bb.table, i,
		 m_bbColumnHeaderIndexes.indexOf("Video Games Reserved"),
		 counts.value("numvideogames"));
	      break;
	    }
	}

      counts.clear();
      QApplication::restoreOverrideCursor();
    }
}

void biblioteq::updateMembersBrowser(void)
{
  QMap<QString, QString> counts;
  QString errorstr = "";
  QString memberid = "";
  int row = 0;

  /*
  ** Called from the Copy Editor when an item has been reserved.
  */

  if((row = bb.table->currentRow()) < 0)
    return;

  memberid = biblioteq_misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("Member ID"));
  QApplication::setOverrideCursor(Qt::WaitCursor);
  counts = biblioteq_misc_functions::getItemsReservedCounts
    (m_db, memberid, errorstr);
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    addError(QString(tr("Database Error")),
	     QString(tr("Unable to determine the number of reserved items "
			"for the selected member.")),
	     errorstr, __FILE__, __LINE__);
  else
    {
      biblioteq_misc_functions::updateColumn
	(bb.table, row, m_bbColumnHeaderIndexes.indexOf("Books Reserved"),
	 counts.value("numbooks"));
      biblioteq_misc_functions::updateColumn
	(bb.table, row, m_bbColumnHeaderIndexes.indexOf("CDs Reserved"),
	 counts.value("numcds"));
      biblioteq_misc_functions::updateColumn
	(bb.table, row, m_bbColumnHeaderIndexes.indexOf("DVDs Reserved"),
	 counts.value("numdvds"));
      biblioteq_misc_functions::updateColumn
	(bb.table, row, m_bbColumnHeaderIndexes.indexOf("Journals Reserved"),
	 counts.value("numjournals"));
      biblioteq_misc_functions::updateColumn
	(bb.table, row, m_bbColumnHeaderIndexes.indexOf("Magazines Reserved"),
	 counts.value("nummagazines"));
      biblioteq_misc_functions::updateColumn
	(bb.table, row,
	 m_bbColumnHeaderIndexes.indexOf("Video Games Reserved"),
	 counts.value("numvideogames"));
      counts.clear();

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
  QString value1 = "";
  QString value2 = "";
  QString value3 = "";
  int i = 0;

  /*
  ** Called from the Borrowers Editor when an item has been updated.
  */

  if(m_history_diag->isVisible())
    if(history.table->rowCount() > 0 &&
       biblioteq_misc_functions::getColumnString
       (history.table, 0,
	m_historyColumnHeaderIndexes.
	indexOf("Member ID")) ==
       memberid)
      {
	QApplication::setOverrideCursor(Qt::WaitCursor);

	for(i = 0; i < history.table->rowCount(); i++)
	  {
	    value1 = biblioteq_misc_functions::getColumnString
	      (history.table, i,
	       m_historyColumnHeaderIndexes.indexOf("MYOID"));
	    value2 = biblioteq_misc_functions::getColumnString
	      (history.table, i,
	       m_historyColumnHeaderIndexes.indexOf("Barcode"));
	    value3 = biblioteq_misc_functions::getColumnString
	      (history.table, i,
	       m_historyColumnHeaderIndexes.indexOf("Type")).
	      toLower().remove(" ");

	    if(value1 == ioid && value2 == copyid && value3 == itemType)
	      {
		QDate date(QDate::fromString(returnedDate, "MM/dd/yyyy"));

		biblioteq_misc_functions::updateColumn
		  (history.table, i,
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
  QGraphicsPixmapItem *item = 0;
  QList<QGraphicsItem *> items(ui.graphicsView->scene()->items());

  for(int i = 0; i < items.size(); i++)
    if((item = qgraphicsitem_cast<QGraphicsPixmapItem *> (items.at(i))))
      if(oid == item->data(0).toString() && type == item->data(1).toString())
	{
	  QImage l_image(image);

	  if(!l_image.isNull())
	    l_image = l_image.scaled
	      (126, 187, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	  QPixmap pixmap(QPixmap::fromImage(l_image));

	  if(!pixmap.isNull())
	    item->setPixmap(pixmap);
	  else
	    {
	      QImage l_image(":/no_image.png");

	      if(!l_image.isNull())
		l_image = l_image.scaled
		  (126, 187, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	      item->setPixmap(QPixmap::fromImage(l_image));
	    }

	  break;
	}
}
