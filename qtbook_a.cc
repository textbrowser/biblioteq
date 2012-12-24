/*
** Copyright (c) 2006, 2007, 2008, 2009, 2010, 2011, 2012 Alexis Megas
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

/*
** -- Qt Includes --
*/

#include <QtDebug>
#ifdef Q_OS_MAC
#include <QMacStyle>
#endif
#include <QSettings>
#include <QSqlField>
#include <QClipboard>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QFontDialog>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDesktopWidget>

/*
** -- C Includes --
*/

extern "C"
{
#include <math.h>
#ifdef Q_OS_WIN32
#include <sqlite3/sqlite3.h>
#else
#include <sqlite3.h>
#endif
}

/*
** -- Local Includes --
*/

#include "qtbook.h"
#include "bgraphicsscene.h"
#include "sqlite_create_schema.h"

/*
** -- Local Variables --
*/

qtbook *qmain = 0;

/*
** -- Global Variables --
*/

QString qtbook::s_locale = "";
QTranslator *qtbook::s_qtTranslator = 0;
QTranslator *qtbook::s_appTranslator = 0;
QApplication *qapp = 0;

/*
** -- main() --
*/

int main(int argc, char *argv[])
{
  /*
  ** Prepare configuration settings.
  */

  QCoreApplication::setOrganizationName("BiblioteQ");
  QCoreApplication::setOrganizationDomain("biblioteq.sourceforge.net");
  QCoreApplication::setApplicationName("BiblioteQ");

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

#ifdef Q_OS_MAC
  QApplication::setStyle(new QMacStyle());
#endif

  /*
  ** Create the user interface.
  */

  if((qapp = new(std::nothrow) QApplication(argc, argv)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((qtbook::s_qtTranslator = new(std::nothrow) QTranslator(0)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((qtbook::s_appTranslator = new(std::nothrow) QTranslator(0)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  qtbook::s_locale = settings.value("locale").toString();

  if(!(qtbook::s_locale == "cs_CZ" ||
       qtbook::s_locale == "de_DE" ||
       qtbook::s_locale == "el_GR" ||
       qtbook::s_locale == "fr_FR" ||
       qtbook::s_locale == "nl_BE" ||
       qtbook::s_locale == "nl_NL"))
    qtbook::s_locale = QLocale::system().name();

  qtbook::s_qtTranslator->load("qt_" + qtbook::s_locale, "translations.d");
  qapp->installTranslator(qtbook::s_qtTranslator);
  qtbook::s_appTranslator->load("biblioteq_" + qtbook::s_locale,
				"translations.d");
  qapp->installTranslator(qtbook::s_appTranslator);
  qapp->connect(qapp, SIGNAL(lastWindowClosed()), qapp, SLOT(quit(void)));

  if((qmain = new(std::nothrow) qtbook()) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  qmain->showMain();

  /*
  ** Enter an endless loop.
  */

  qapp->exec();

  /*
  ** Return.
  */

  return EXIT_SUCCESS;
}

/*
** -- quit() --
*/

void qtbook::quit(void)
{
  if(qmain != 0)
    {
      if(qmain->ui.actionAutomaticallySaveSettingsOnExit->isChecked())
	qmain->slotSaveConfig();

      qmain->cleanup();
    }

  if(qapp != 0)
    qapp->quit();

  qDebug() << tr("BiblioteQ has exited.");
  exit(EXIT_SUCCESS);
}

/*
** -- cleanup () --
*/

void qtbook::cleanup(void)
{
  if(qapp != 0 && isVisible())
    qapp->setOverrideCursor(Qt::WaitCursor);

  if(db.isOpen())
    db.close();

  if(qapp != 0 && isVisible())
    qapp->restoreOverrideCursor();
}

/*
** -- quit() --
*/

void qtbook::quit(const char *msg, const char *file, const int line)
{
  if(msg != 0 && strlen(msg) > 0)
    qDebug() << tr(msg)
	     << tr(" in file ")
	     << file << tr(", line ") << line
	     << tr(".");
  else
    qDebug() << tr("An unknown error occurred in file ")
	     << file << tr(", line ")
	     << line << tr(".");

  if(qmain != 0)
    qmain->cleanup();

  exit(EXIT_SUCCESS);
}

/*
** -- qtbook() --
*/

qtbook::qtbook(void):QMainWindow()
{
  QMenu *menu1 = 0;
  QMenu *menu2 = 0;
  QMenu *menu3 = 0;
  QMenu *menu4 = 0;

  m_idCt = 0;
  previousTypeFilter = "";

  if((branch_diag = new(std::nothrow) QDialog(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((pass_diag = new(std::nothrow) QDialog(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((all_diag = new(std::nothrow) QMainWindow(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((admin_diag = new(std::nothrow) QMainWindow(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((members_diag = new(std::nothrow) QMainWindow()) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((history_diag = new(std::nothrow) QMainWindow()) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((customquery_diag = new(std::nothrow) QMainWindow(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((userinfo_diag = new(std::nothrow) userinfo_diag_class
      (members_diag)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((error_diag = new(std::nothrow) QMainWindow(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((db_enumerations = new(std::nothrow) dbenumerations()) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((menu1 = new(std::nothrow) QMenu(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((menu2 = new(std::nothrow) QMenu(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((menu3 = new(std::nothrow) QMenu(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((menu4 = new(std::nothrow) QMenu(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  connect(menu1->addAction(tr("Add &Book")),
	  SIGNAL(triggered(void)), this, SLOT(slotInsertBook(void)));
  // menu1->addAction(tr("Add &Cassette Tape"));
  connect(menu1->addAction(tr("Add &DVD")),
	  SIGNAL(triggered(void)), this, SLOT(slotInsertDVD(void)));
  connect(menu1->addAction(tr("Add &Journal")),
	  SIGNAL(triggered(void)), this, SLOT(slotInsertJourn(void)));
  connect(menu1->addAction(tr("Add &Magazine")),
	  SIGNAL(triggered(void)), this, SLOT(slotInsertMag(void)));
  connect(menu1->addAction(tr("Add Music &CD")),
	  SIGNAL(triggered(void)), this, SLOT(slotInsertCD(void)));
  // menu1->addAction(tr("Add &Newspaper"));
  connect(menu1->addAction(tr("Add &Photograph Collection")),
	  SIGNAL(triggered(void)), this, SLOT(slotInsertPhotograph(void)));
  connect(menu1->addAction(tr("Add &Video Game")),
	  SIGNAL(triggered(void)), this, SLOT(slotInsertVideoGame(void)));
  // menu1->addAction(tr("Add &VHS"));
  // menu1->addAction(tr("Add &Vinyl Record"));
  connect(menu2->addAction(tr("&General Search")),
	  SIGNAL(triggered(void)), this, SLOT(slotSearch(void)));
  menu2->addSeparator();
  connect(menu2->addAction(tr("&Book Search")),
	  SIGNAL(triggered(void)), this, SLOT(slotBookSearch(void)));
  connect(menu2->addAction(tr("&DVD Search")),
	  SIGNAL(triggered(void)), this, SLOT(slotDVDSearch(void)));
  connect(menu2->addAction(tr("&Journal Search")),
	  SIGNAL(triggered(void)), this, SLOT(slotJournSearch(void)));
  connect(menu2->addAction(tr("&Magazine Search")),
	  SIGNAL(triggered(void)), this, SLOT(slotMagSearch(void)));
  connect(menu2->addAction(tr("&Music CD Search")),
	  SIGNAL(triggered(void)), this, SLOT(slotCDSearch(void)));
  connect(menu2->addAction(tr("&Photograph Collection Search")),
	  SIGNAL(triggered(void)), this, SLOT(slotPhotographSearch(void)));
  connect(menu2->addAction(tr("&Video Game Search")),
	  SIGNAL(triggered(void)), this, SLOT(slotVideoGameSearch(void)));
  menu3->setTearOffEnabled(true);
  menu3->setWindowIcon(QIcon("icons.d/book.png"));
  menu3->setWindowTitle(tr("BiblioteQ"));
  connect(menu4->addAction(tr("Reset &ID Number")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu4->addAction(tr("Reset &Title")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu4->addAction(tr("Reset &Publication Date")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu4->addAction(tr("Reset &Publisher")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu4->addAction(tr("Reset &Categories")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu4->addAction(tr("Reset &Price")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu4->addAction(tr("Reset &Language")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu4->addAction(tr("Reset &Monetary Units")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu4->addAction(tr("Reset &Abstract")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu4->addAction(tr("Reset &Copies")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu4->addAction(tr("Reset &Location")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu4->addAction(tr("Reset &Keywords")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu4->addAction(tr("Reset &Availability")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  ui.setupUi(this);
#ifdef Q_OS_MAC
  setAttribute(Qt::WA_MacMetalStyle, true);
#endif
  connect(ui.action_Book,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotInsertBook(void)));
  connect(ui.actionMusic_CD,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotInsertCD(void)));
  connect(ui.action_DVD,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotInsertDVD(void)));
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
  connect(ui.action_Video_Game,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotInsertVideoGame(void)));

  bgraphicsscene *scene = 0;

  if((scene = new(std::nothrow) bgraphicsscene(ui.graphicsView)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  connect(scene,
	  SIGNAL(selectionChanged(void)),
	  this,
	  SLOT(slotSceneSelectionChanged(void)));
  ui.graphicsView->setScene(scene);
  ui.graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
  ui.graphicsView->setRubberBandSelectionMode(Qt::IntersectsItemShape);
  bb.setupUi(members_diag);
  history.setupUi(history_diag);
  br.setupUi(branch_diag);
  pass.setupUi(pass_diag);
  al.setupUi(all_diag);
  cq.setupUi(customquery_diag);
  er.setupUi(error_diag);
  ab.setupUi(admin_diag);
  ab.splitter->setStretchFactor(0, 0);
  ab.splitter->setStretchFactor(1, 1);
#ifdef Q_OS_MAC
  members_diag->setAttribute(Qt::WA_MacMetalStyle, true);
  history_diag->setAttribute(Qt::WA_MacMetalStyle, true);
  branch_diag->setAttribute(Qt::WA_MacMetalStyle, true);
  pass_diag->setAttribute(Qt::WA_MacMetalStyle, true);
  all_diag->setAttribute(Qt::WA_MacMetalStyle, true);
  customquery_diag->setAttribute(Qt::WA_MacMetalStyle, true);
  error_diag->setAttribute(Qt::WA_MacMetalStyle, true);
  admin_diag->setAttribute(Qt::WA_MacMetalStyle, true);
  ui.actionSetGlobalFonts->setVisible(false);
#endif
  pass_diag->setModal(true);
  userinfo_diag->setModal(true);
  branch_diag->setModal(true);
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
  connect(ui.typefilter, SIGNAL(activated(int)), this,
	  SLOT(slotAutoPopOnFilter(void)));
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
  connect(userinfo_diag->userinfo.okButton, SIGNAL(clicked(void)), this,
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
  connect(userinfo_diag->userinfo.nextTool, SIGNAL(clicked(void)), this,
	  SLOT(slotShowNext(void)));
  connect(userinfo_diag->userinfo.prevTool, SIGNAL(clicked(void)), this,
	  SLOT(slotShowPrev(void)));
  connect(history.nextTool, SIGNAL(clicked(void)), this,
	  SLOT(slotShowNext(void)));
  connect(history.prevTool, SIGNAL(clicked(void)), this,
	  SLOT(slotShowPrev(void)));
  connect(history.cancelButton, SIGNAL(clicked(void)),
	  history_diag, SLOT(close(void)));
  connect(br.okButton, SIGNAL(clicked(void)), this,
	  SLOT(slotConnectDB(void)));
  connect(br.branch_name, SIGNAL(activated(int)), this,
	  SLOT(slotBranchChanged(void)));
  connect(bb.printButton, SIGNAL(clicked(void)), this,
	  SLOT(slotPrintReserved(void)));
  connect(bb.addButton, SIGNAL(clicked(void)), this,
	  SLOT(slotAddBorrower(void)));
  connect(bb.reloadButton, SIGNAL(clicked(void)), this,
	  SLOT(slotPopulateMembersBrowser(void)));
  connect(bb.deleteButton, SIGNAL(clicked(void)), this,
	  SLOT(slotRemoveMember(void)));
  connect(userinfo_diag->userinfo.cancelButton, SIGNAL(clicked(void)), this,
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
	  all_diag, SLOT(close(void)));
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
  connect(er.resetButton, SIGNAL(clicked(void)), this,
	  SLOT(slotResetErrorLog(void)));
  connect(er.cancelButton, SIGNAL(clicked(void)),
	  error_diag, SLOT(close(void)));
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
  connect(pass.okButton, SIGNAL(clicked(void)), this,
	  SLOT(slotSavePassword(void)));
  connect(pass.cancelButton, SIGNAL(clicked(void)), pass_diag,
	  SLOT(close(void)));
  connect(br.resetButton, SIGNAL(clicked(void)), this,
	  SLOT(slotResetLoginDialog(void)));
  connect(br.fileButton, SIGNAL(clicked(void)), this,
	  SLOT(slotSelectDatabaseFile(void)));
  connect(br.cancelButton, SIGNAL(clicked(void)),
	  branch_diag, SLOT(close(void)));
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
	  admin_diag, SLOT(close(void)));
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
  ab.table->verticalHeader()->setResizeMode(QHeaderView::Fixed);
  bb.table->verticalHeader()->setResizeMode(QHeaderView::Fixed);
  er.table->verticalHeader()->setResizeMode(QHeaderView::Fixed);
  history.table->verticalHeader()->setResizeMode(QHeaderView::Fixed);
  ui.createTool->setMenu(menu1);
  ui.searchTool->setMenu(menu2);
  ui.configTool->setMenu(menu3);
  al.resetButton->setMenu(menu4);
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

  QString typefilter("");
  QSettings settings;

  typefilter = lastCategory =
    settings.value("last_category", "All").toString();
  typefilter.replace(" ", "_");
  ui.table->resetTable(db.userName(), lastCategory, roles);
  ui.summary->setVisible(false);
  ui.actionConfigureAdministratorPrivileges->setEnabled(false);
  previousTypeFilter = lastCategory;
  prepareFilter();

  if(ui.typefilter->findData(QVariant(lastCategory)) > -1)
    ui.typefilter->setCurrentIndex
      (ui.typefilter->findData(QVariant(lastCategory)));

  addConfigOptions(lastCategory);
  setUpdatesEnabled(true);
  userinfo_diag->userinfo.telephoneNumber->setInputMask("999-999-9999");
  userinfo_diag->userinfo.zip->setInputMask("99999");
  userinfo_diag->userinfo.expirationdate->setMaximumDate
    (QDate(3000, 1, 1));

  QActionGroup *group1 = 0;

  if((group1 = new(std::nothrow) QActionGroup(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  for(int i = 1; i <= 5; i++)
    {
      QAction *action = 0;

      if(i == 5)
	action = group1->addAction
	  (QString(tr("&Unlimited Entries per Page")));
      else
	action = group1->addAction(QString(tr("&%1")).arg(25 * i));

      if(!action)
	continue;

      connect(action,
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotRefresh(void)));

      if(i == 5)
	action->setData(-1);
      else
	action->setData(25 * i);

      action->setCheckable(true);

      if(i == 1)
	action->setChecked(true);

      ui.menuEntriesPerPage->addAction(action);
    }

  QAction *action = 0;
  QActionGroup *group2 = 0;

  if((group2 = new(std::nothrow) QActionGroup(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

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
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  group3->setExclusive(true);
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
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  userinfo_diag->userinfo.memberid->setValidator(validator1);

  /*
  ** Highlight userinfo_diag->userinfo required widgets.
  */

  misc_functions::highlightWidget(userinfo_diag->userinfo.firstName,
				  QColor(255, 248, 220));
  misc_functions::highlightWidget(userinfo_diag->userinfo.lastName,
				  QColor(255, 248, 220));
  misc_functions::highlightWidget(userinfo_diag->userinfo.street,
				  QColor(255, 248, 220));
  misc_functions::highlightWidget(userinfo_diag->userinfo.city,
				  QColor(255, 248, 220));
  misc_functions::highlightWidget(userinfo_diag->userinfo.zip,
				  QColor(255, 248, 220));
  ui.splitter->setCollapsible(1, false);
  ui.splitter->setStretchFactor(0, 0);
  ui.splitter->setStretchFactor(1, 1);
}

/*
** -- addConfigOptions() --
*/

void qtbook::addConfigOptions(const QString &typefilter)
{
  int i = 0;
  QAction *action = 0;

  /*
  ** Delete existing actions, if any.
  */

  ui.configTool->menu()->clear();

  for(i = 0; i < ui.table->columnCount(); i++)
    {
      if(typefilter != "All" &&
	 typefilter != "All Available" &&
	 typefilter != "All Overdue" &&
	 typefilter != "All Requested" &&
	 typefilter != "All Reserved")
	{
	  if(ui.table->columnNames().at(i) == "MYOID" ||
	     ui.table->columnNames().at(i) == "Type")
	    continue;
	}
      else if(ui.table->columnNames().at(i) == "MYOID" ||
	      ui.table->columnNames().at(i) == "REQUESTOID")
	continue;

      if((action = new(std::nothrow) QAction(ui.table->
					     horizontalHeaderItem(i)->text(),
					     ui.configTool)) == 0)
	continue;

      action->setCheckable(true);
      action->setChecked(!ui.table->isColumnHidden(i));
      ui.configTool->menu()->addAction(action);
      connect(action, SIGNAL(triggered(void)), this,
	      SLOT(slotSetColumns(void)));
    }
}

/*
** -- setColumns() --
*/

void qtbook::slotSetColumns(void)
{
  int i = 0;
  QString typefilter = ui.typefilter->itemData
    (ui.typefilter->currentIndex()).toString();

  for(i = 0; i < ui.configTool->menu()->actions().size(); i++)
    {
      ui.table->setColumnHidden
	(i, !ui.configTool->menu()->actions().at(i)->isChecked());
      ui.table->recordColumnHidden
	(db.userName(),
	 typefilter, i, !ui.configTool->menu()->actions().at(i)->
	 isChecked());
    }
}

/*
** -- getRoles() --
*/

QString qtbook::getRoles(void) const
{
  /*
  ** Empty roles suggest that the user is a patron.
  */

  return roles;
}

/*
** -- adminSetup() --
*/

void qtbook::adminSetup(void)
{
  bb.addButton->setEnabled(true);
  bb.grantButton->setEnabled(true);
  bb.deleteButton->setEnabled(true);
  bb.modifyButton->setEnabled(true);
  bb.historyButton->setEnabled(true);
  bb.listButton->setEnabled(true);
  bb.printButton->setEnabled(true);
  bb.checkoutButton->setEnabled(true);
  bb.overdueButton->setEnabled(true);
  ui.detailsTool->setEnabled(true);
  ui.actionViewDetails->setEnabled(true);

  if(status_bar_label != 0)
    {
      status_bar_label->setPixmap(QPixmap("icons.d/16x16/unlock.png"));

      if(roles.contains("administrator"))
	status_bar_label->setToolTip(tr("Administrator Mode"));
      else if(roles == "circulation")
	status_bar_label->setToolTip(tr("Circulation Mode"));
      else if(roles == "librarian")
	status_bar_label->setToolTip(tr("Librarian Mode"));
      else if(roles == "membership")
	status_bar_label->setToolTip(tr("Membership Mode"));
      else
	status_bar_label->setToolTip(tr("Privileged Mode"));
    }

  if(roles.contains("administrator") || roles.contains("librarian"))
    {
      ui.table->disconnect(SIGNAL(itemDoubleClicked(QTableWidgetItem *)));
      connect(ui.table, SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
	      this, SLOT(slotModify(void)));
      ui.graphicsView->scene()->disconnect(SIGNAL(itemDoubleClicked(void)));
      connect(ui.graphicsView->scene(), SIGNAL(itemDoubleClicked(void)),
	      this, SLOT(slotModify(void)));
      updateItemWindows();
    }

  if(roles.contains("administrator") || roles.contains("librarian"))
    ui.deleteTool->setEnabled(true);

  if(roles.contains("administrator") || roles.contains("librarian"))
    {
      ui.menu_Add_Item->setEnabled(true);
      ui.actionDeleteEntry->setEnabled(true);
      ui.actionDuplicateEntry->setEnabled(true);
      ui.createTool->setEnabled(true);
      ui.modifyTool->setEnabled(true);
      ui.duplicateTool->setEnabled(true);
    }

  if(roles.contains("administrator") || roles.contains("librarian"))
    {
      ui.detailsTool->setEnabled(false);
      ui.actionViewDetails->setEnabled(false);
    }

  if(roles.contains("administrator") || roles.contains("librarian"))
    ui.actionModifyEntry->setEnabled(true);

  if(roles.contains("administrator") || roles.contains("circulation") ||
     roles.contains("membership"))
    {
      ui.userTool->setEnabled(true);
      ui.actionMembersBrowser->setEnabled(true);
    }

  if(roles.contains("administrator") || roles.contains("circulation"))
    ui.reserveTool->setEnabled(true);

  if(roles.contains("administrator") || roles.contains("librarian"))
    ui.actionAutoPopulateOnCreation->setEnabled(true);

  ui.actionPopulate_Members_Browser_Table_on_Display->setEnabled
    (roles.contains("administrator"));

  if(db.driverName() != "QSQLITE")
    {
      ui.actionConfigureAdministratorPrivileges->setEnabled
	(roles.contains("administrator"));
      ui.actionPopulate_Administrator_Browser_Table_on_Display->setEnabled
	(roles.contains("administrator"));
      ui.actionDatabase_Enumerations->setEnabled
	(roles.contains("administrator"));
      ui.actionPopulate_Database_Enumerations_Browser_on_Display->setEnabled
	(roles.contains("administrator"));
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

  if(roles == "membership")
    {
      bb.historyButton->setEnabled(false);
      bb.listButton->setEnabled(false);
      bb.printButton->setEnabled(false);
      bb.checkoutButton->setEnabled(false);
      bb.overdueButton->setEnabled(false);
    }

  if(roles == "circulation" || roles == "librarian")
    {
      bb.addButton->setEnabled(false);
      bb.grantButton->setEnabled(false);
      bb.deleteButton->setEnabled(false);
      bb.modifyButton->setEnabled(false);

      if(roles == "librarian")
	{
	  ui.actionDatabase_Enumerations->setEnabled(true);
	  ui.actionPopulate_Database_Enumerations_Browser_on_Display->
	    setEnabled(true);
	}
    }
  else
    {
      connect(bb.table, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this,
	      SLOT(slotModifyBorrower(void)));
      bb.addButton->setEnabled(true);

      if(db.driverName() != "QSQLITE")
	bb.grantButton->setEnabled(true);

      bb.deleteButton->setEnabled(true);
      bb.modifyButton->setEnabled(true);
    }
}

/*
** -- showMain() --
*/

void qtbook::showMain(void)
{
  if((connected_bar_label = new(std::nothrow) QLabel()) != 0)
    {
      connected_bar_label->setPixmap
	(QPixmap("icons.d/16x16/disconnected.png"));
      connected_bar_label->setToolTip(tr("Disconnected"));
      statusBar()->addPermanentWidget(connected_bar_label);
    }

  if((status_bar_label = new(std::nothrow) QLabel()) != 0)
    {
      status_bar_label->setPixmap(QPixmap("icons.d/16x16/lock.png"));
      status_bar_label->setToolTip(tr("Standard User Mode"));
      statusBar()->addPermanentWidget(status_bar_label);
    }

  if((error_bar_label = new(std::nothrow) QLabel()) != 0)
    {
      error_bar_label->setPixmap(QPixmap("icons.d/16x16/ok.png"));
      error_bar_label->setToolTip(tr("Empty Error Log"));
      statusBar()->addPermanentWidget(error_bar_label);
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
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  for(int i = 0; i < getSRUMaps().size(); i++)
    {
      QAction *action = group1->addAction
	(getSRUMaps().values()[i]["Name"]);

      if(!action)
	continue;

      action->setCheckable(true);

      if(i == 0)
	action->setChecked(true);

      ui.menuPreferredSRUSite->addAction(action);
    }

  if(ui.menuPreferredSRUSite->actions().isEmpty())
    ui.menuPreferredSRUSite->addAction(tr("None"));

  QActionGroup *group2 = 0;

  if((group2 = new(std::nothrow) QActionGroup(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  for(int i = 0; i < getZ3950Maps().size(); i++)
    {
      QAction *action = group2->addAction
	(getZ3950Maps().values()[i]["Name"]);

      if(!action)
	continue;

      action->setCheckable(true);

      if(i == 0)
	action->setChecked(true);

      ui.menuPreferredZ3950Server->addAction(action);
    }

  if(ui.menuPreferredZ3950Server->actions().isEmpty())
    ui.menuPreferredZ3950Server->addAction(tr("None"));

  /*
  ** Initial update.
  */

  initialUpdate();
  show();
#ifndef Q_OS_MAC
  setGlobalFonts(qapp->font());
#endif
  slotResizeColumns();
}

/*
** -- ~qtbook() --
*/

qtbook::~qtbook()
{
}

/*
** -- slotExit() --
*/

void qtbook::slotExit(void)
{
  QSettings settings;

  settings.setValue("mainwindowState", saveState());
  qtbook::quit();
}

/*
** -- slotAbout() --
*/

void qtbook::slotAbout(void)
{
  QMessageBox mb(this);

#ifndef Q_OS_MAC
  mb.setFont(qapp->font());
#endif
  mb.setWindowTitle(tr("BiblioteQ: About"));
  mb.setTextFormat(Qt::RichText);
  mb.setText
    (QString("<html>BiblioteQ Version %1<br>"
	     "Copyright (c) 2006 - 2012 Elephants and Tigers.<br>"
	     "Icons copyright (c) Matthieu James.<br>"
	     "Library icon copyright (c) Jonas Rask Design.<br>"
	     "Qt version %2."
	     "<hr>"
	     "Please visit <a href=\"http://biblioteq.sourceforge.net\">"
	     "http://biblioteq.sourceforge.net</a> for "
	     "project information.<br>"
	     "For release notes, please visit "
	     "<a href=\"http://biblioteq.sourceforge.net/news.html\">"
	     "http://biblioteq.sourceforge.net/news.html</a>.<br></html>").
     arg(BIBLIOTEQ_VERSION).
     arg(QT_VERSION_STR));
  mb.setStandardButtons(QMessageBox::Ok);
  mb.setIconPixmap(QPixmap("./icons.d/book.png"));
  mb.exec();
}

/*
** -- slotSearch() --
*/

void qtbook::slotSearch(void)
{
  if(!db.isOpen())
    return;

  /*
  ** Hide certain fields if we're a regular user.
  */

  misc_functions::hideAdminFields(all_diag, roles);
  al.idnumber->clear();
  al.title->clear();
  al.publisher->clear();
  al.category->clear();
  al.publication_date->setDate(QDate::fromString("01/7999",
						 "MM/yyyy"));
  al.price->setMinimum(-0.01);
  al.price->setValue(-0.01);
  al.quantity->setMinimum(0);
  al.quantity->setValue(0);
  al.description->clear();
  al.language->clear();
  al.monetary_units->clear();
  al.location->clear();
  al.keyword->clear();
  al.available->setChecked(false);

  /*
  ** Populate combination boxes.
  */

  QString errorstr("");

  qapp->setOverrideCursor(Qt::WaitCursor);
  al.language->addItems
    (misc_functions::getLanguages(db,
				  errorstr));
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the languages.")),
       errorstr, __FILE__, __LINE__);

  qapp->setOverrideCursor(Qt::WaitCursor);
  al.monetary_units->addItems
    (misc_functions::getMonetaryUnits(db,
				      errorstr));
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the monetary units.")),
       errorstr, __FILE__, __LINE__);

  qapp->setOverrideCursor(Qt::WaitCursor);
  al.location->addItems
    (misc_functions::getLocations(db,
				  "",
				  errorstr));
  qapp->restoreOverrideCursor();

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

  if(!all_diag->isVisible())
    all_diag->updateGeometry();

  static bool resized = false;

  if(!resized)
    all_diag->resize(0.75 * size().width(),
		     0.75 * size().height());

  resized = true;
  misc_functions::center(all_diag, this);
  all_diag->raise();
  all_diag->show();
}

/*
** -- slotShowGrid() --
*/

void qtbook::slotShowGrid(void)
{
  ui.table->setShowGrid(ui.actionShowGrid->isChecked());
}

/*
** -- slotModify() --
*/

void qtbook::slotModify(void)
{
  if(!db.isOpen())
    return;

  int i = 0;
  bool error = false;
  QString oid = "";
  QString type = "";
  qtbook_cd *cd = 0;
  qtbook_dvd *dvd = 0;
  QModelIndex index;
  main_table *table = ui.table;
  qtbook_book *book = 0;
  qtbook_journal *journal = 0;
  qtbook_magazine *magazine = 0;
  qtbook_videogame *videogame = 0;
  qtbook_photographcollection *photograph = 0;
  QModelIndexList list = table->selectionModel()->selectedRows();

  if(list.isEmpty())
    {
      QMessageBox::critical
	(this, tr("BiblioteQ: User Error"),
	 tr("Please select at least one item to modify."));
      return;
    }
  else if(list.size() >= 5)
    if(QMessageBox::question
       (this, tr("BiblioteQ: Question"),
	tr("Are you sure that you wish to modify the ") +
	QString::number(list.size()) +
	tr(" selected items?"),
	QMessageBox::Yes | QMessageBox::No,
	QMessageBox::No) == QMessageBox::No)
      {
	list.clear();
	return;
      }

  qStableSort(list.begin(), list.end());

  foreach(index, list)
    {
      i = index.row();
      oid = misc_functions::getColumnString
	(table, i, table->columnNumber("MYOID"));
      type = misc_functions::getColumnString
	(table, i, table->columnNumber("Type"));
      cd = 0;
      dvd = 0;
      book = 0;
      journal = 0;
      magazine = 0;
      videogame = 0;
      photograph = 0;

      if(type.toLower() == "cd")
	{
	  foreach(QWidget *w, qapp->topLevelWidgets())
	    {
	      qtbook_cd *c = qobject_cast<qtbook_cd *> (w);

	      if(c && c->getID() == oid)
		{
		  cd = c;
		  break;
		}
	    }

	  if(!cd)
	    cd = new(std::nothrow) qtbook_cd(this, oid, i);

	  if(cd)
	    cd->modify(EDITABLE);
	}
      else if(type.toLower() == "dvd")
	{
	  foreach(QWidget *w, qapp->topLevelWidgets())
	    {
	      qtbook_dvd *d = qobject_cast<qtbook_dvd *> (w);

	      if(d && d->getID() == oid)
		{
		  dvd = d;
		  break;
		}
	    }

	  if(!dvd)
	    dvd = new(std::nothrow) qtbook_dvd(this, oid, i);

	  if(dvd)
	    dvd->modify(EDITABLE);
	}
      else if(type.toLower() == "book")
	{
	  foreach(QWidget *w, qapp->topLevelWidgets())
	    {
	      qtbook_book *b = qobject_cast<qtbook_book *> (w);

	      if(b && b->getID() == oid)
		{
		  book = b;
		  break;
		}
	    }

	  if(!book)
	    book = new(std::nothrow) qtbook_book(this, oid, i);

	  if(book)
	    book->modify(EDITABLE);
	}
      else if(type.toLower() == "journal")
	{
	  foreach(QWidget *w, qapp->topLevelWidgets())
	    {
	      qtbook_journal *j = qobject_cast<qtbook_journal *> (w);

	      if(j && j->getID() == oid)
		{
		  journal = j;
		  break;
		}
	    }

	  if(!journal)
	    journal = new(std::nothrow) qtbook_journal(this, oid, i);

	  if(journal)
	    journal->modify(EDITABLE);
	}
      else if(type.toLower() == "magazine")
	{
	  foreach(QWidget *w, qapp->topLevelWidgets())
	    {
	      qtbook_magazine *m = qobject_cast<qtbook_magazine *> (w);

	      /*
	      ** The class qtbook_journal inherits qtbook_magazine.
	      */

	      if(!qobject_cast<qtbook_journal *> (w))
		if(m && m->getID() == oid)
		  {
		    magazine = m;
		    break;
		  }
	    }

	  if(!magazine)
	    magazine = new(std::nothrow) qtbook_magazine
	      (this,
	       oid, i, "magazine");

	  if(magazine)
	    magazine->modify(EDITABLE);
	}
      else if(type.toLower() == "photograph collection")
	{
	  foreach(QWidget *w, qapp->topLevelWidgets())
	    {
	      qtbook_photographcollection *p =
		qobject_cast<qtbook_photographcollection *> (w);

	      if(p && p->getID() == oid)
		{
		  photograph = p;
		  break;
		}
	    }

	  if(!photograph)
	    photograph = new(std::nothrow) qtbook_photographcollection
	      (this, oid, i);

	  if(photograph)
	    photograph->modify(EDITABLE);
	}
      else if(type.toLower() == "video game")
	{
	  foreach(QWidget *w, qapp->topLevelWidgets())
	    {
	      qtbook_videogame *v = qobject_cast<qtbook_videogame *> (w);

	      if(v && v->getID() == oid)
		{
		  videogame = v;
		  break;
		}
	    }

	  if(!videogame)
	    videogame = new(std::nothrow) qtbook_videogame(this, oid, i);

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

  if(error)
    QMessageBox::critical(this, tr("BiblioteQ: Error"),
			  tr("Unable to determine the selected item's "
			     "type."));
}

/*
** -- slotViewDetails() --
*/

void qtbook::slotViewDetails(void)
{
  int i = 0;
  bool error = false;
  QString oid = "";
  QString type = "";
  qtbook_cd *cd = 0;
  qtbook_dvd *dvd = 0;
  QModelIndex index;
  qtbook_book *book = 0;
  main_table *table = ui.table;
  qtbook_journal *journal = 0;
  qtbook_magazine *magazine = 0;
  QModelIndexList list = table->selectionModel()->selectedRows();
  qtbook_videogame *videogame = 0;
  qtbook_photographcollection *photograph = 0;

  if(list.isEmpty())
    {
      QMessageBox::critical(this, tr("BiblioteQ: User Error"),
			    tr("Please select at least one item to view."));
      return;
    }
  else if(list.size() >= 5)
    if(QMessageBox::question(this, tr("BiblioteQ: Question"),
			     tr("Are you sure that you wish to view the ") +
			     QString::number(list.size()) +
			     tr(" selected items?"),
			     QMessageBox::Yes | QMessageBox::No,
			     QMessageBox::No) == QMessageBox::No)
      {
	list.clear();
	return;
      }

  qStableSort(list.begin(), list.end());

  foreach(index, list)
    {
      i = index.row();
      oid = misc_functions::getColumnString
	(table, i, table->columnNumber("MYOID"));
      type = misc_functions::getColumnString
	(table, i, table->columnNumber("Type"));
      cd = 0;
      dvd = 0;
      book = 0;
      journal = 0;
      magazine = 0;
      videogame = 0;
      photograph = 0;

      if(type.toLower() == "cd")
	{
	  foreach(QWidget *w, qapp->topLevelWidgets())
	    {
	      qtbook_cd *c = qobject_cast<qtbook_cd *> (w);

	      if(c && c->getID() == oid)
		{
		  cd = c;
		  break;
		}
	    }

	  if(!cd)
	    cd = new(std::nothrow) qtbook_cd(this, oid, i);

	  if(cd)
	    cd->modify(VIEW_ONLY);
	}
      else if(type.toLower() == "dvd")
	{
	  foreach(QWidget *w, qapp->topLevelWidgets())
	    {
	      qtbook_dvd *d = qobject_cast<qtbook_dvd *> (w);

	      if(d && d->getID() == oid)
		{
		  dvd = d;
		  break;
		}
	    }

	  if(!dvd)
	    dvd = new(std::nothrow) qtbook_dvd(this, oid, i);

	  if(dvd)
	    dvd->modify(VIEW_ONLY);
	}
      else if(type.toLower() == "book")
	{
	  foreach(QWidget *w, qapp->topLevelWidgets())
	    {
	      qtbook_book *b = qobject_cast<qtbook_book *> (w);

	      if(b && b->getID() == oid)
		{
		  book = b;
		  break;
		}
	    }

	  if(!book)
	    book = new(std::nothrow) qtbook_book(this, oid, i);

	  if(book)
	    book->modify(VIEW_ONLY);
	}
      else if(type.toLower() == "journal")
	{
	  foreach(QWidget *w, qapp->topLevelWidgets())
	    {
	      qtbook_journal *j = qobject_cast<qtbook_journal *> (w);

	      if(j && j->getID() == oid)
		{
		  journal = j;
		  break;
		}
	    }

	  if(!journal)
	    journal = new(std::nothrow) qtbook_journal(this, oid, i);

	  if(journal)
	    journal->modify(VIEW_ONLY);
	}
      else if(type.toLower() == "magazine")
	{
	  foreach(QWidget *w, qapp->topLevelWidgets())
	    {
	      qtbook_magazine *m = qobject_cast<qtbook_magazine *> (w);

	      /*
	      ** The class qtbook_journal inherits qtbook_magazine.
	      */

	      if(!qobject_cast<qtbook_journal *> (w))
		if(m && m->getID() == oid)
		  {
		    magazine = m;
		    break;
		  }
	    }

	  if(!magazine)
	    magazine = new(std::nothrow) qtbook_magazine
	      (this, oid, i, "magazine");

	  if(magazine)
	    magazine->modify(VIEW_ONLY);
	}
      else if(type.toLower() == "photograph collection")
	{
	  foreach(QWidget *w, qapp->topLevelWidgets())
	    {
	      qtbook_photographcollection *p =
		qobject_cast<qtbook_photographcollection *> (w);

	      if(p && p->getID() == oid)
		{
		  photograph = p;
		  break;
		}
	    }

	  if(!photograph)
	    photograph = new(std::nothrow) qtbook_photographcollection
	      (this, oid, i);

	  if(photograph)
	    photograph->modify(VIEW_ONLY);
	}
      else if(type.toLower() == "video game")
	{
	  foreach(QWidget *w, qapp->topLevelWidgets())
	    {
	      qtbook_videogame *v = qobject_cast<qtbook_videogame *> (w);

	      if(v && v->getID() == oid)
		{
		  videogame = v;
		  break;
		}
	    }

	  if(!videogame)
	    videogame = new(std::nothrow) qtbook_videogame
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

  if(error)
    QMessageBox::critical(this, tr("BiblioteQ: Error"),
			  tr("Unable to determine the selected item's "
			     "type."));
}

/*
** -- slotDelete() --
*/

void qtbook::slotDelete(void)
{
  if(!db.isOpen())
    return;

  int i = 0;
  int col = -1;
  int numdeleted = 0;
  bool error = false;
  bool isRequested = false;
  bool isCheckedOut = false;
  QString oid = "";
  QString str = "";
  QString title = "";
  QString errorstr = "";
  QString itemType = "";
  QSqlQuery query(db);
  QModelIndex index;
  QProgressDialog progress(this);
  QModelIndexList list = ui.table->selectionModel()->selectedRows();

#ifdef Q_OS_MAC
  progress.setAttribute(Qt::WA_MacMetalStyle, true);
#endif

  if(list.isEmpty())
    {
      QMessageBox::critical(this, tr("BiblioteQ: User Error"),
			    tr("Please select an item to delete."));
      return;
    }

  col = ui.table->columnNumber("MYOID");

  foreach(index, list)
    {
      i = index.row();

      if(ui.table->item(i, col) == 0)
	continue;

      oid = misc_functions::getColumnString
	(ui.table, i, ui.table->columnNumber("MYOID"));
      itemType = misc_functions::getColumnString
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
	  list.clear();
	  return;
	}

      qapp->setOverrideCursor(Qt::WaitCursor);
      isCheckedOut = misc_functions::isCheckedOut(db, oid, itemType,
						  errorstr);
      qapp->restoreOverrideCursor();

      if(!errorstr.isEmpty())
	{
	  addError(QString(tr("Database Error")),
		   QString(tr("Unable to determine if the item has been "
			      "reserved.")),
		   errorstr, __FILE__, __LINE__);
	  QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
				tr("Unable to determine if the item has "
				   "been reserved."));
	  list.clear();
	  return;
	}

      if(isCheckedOut)
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Reserved items may not be deleted."));
	  list.clear();
	  return;
	}

      qapp->setOverrideCursor(Qt::WaitCursor);
      isRequested = misc_functions::isRequested(db, oid, itemType, errorstr);
      qapp->restoreOverrideCursor();

      if(!errorstr.isEmpty())
	{
	  addError(QString(tr("Database Error")),
		   QString(tr("Unable to determine if the item has been "
			      "requested.")),
		   errorstr, __FILE__, __LINE__);
	  QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
				tr("Unable to determine if the item has "
				   "been requested."));
	  list.clear();
	  return;
	}

      if(isRequested)
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Requested items may not be deleted."));
	  list.clear();
	  return;
	}
    }

  if(list.size() > 0)
    if(QMessageBox::question(this, tr("BiblioteQ: Question"),
			     tr("Are you sure that you wish to permanently "
				"delete the selected item(s)?"),
			     QMessageBox::Yes | QMessageBox::No,
			     QMessageBox::No) == QMessageBox::No)
      {
	list.clear();
	return;
      }

  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.setLabelText(tr("Deleting the selected item(s)..."));
  progress.setMaximum(list.size());
  progress.setMinimum(0);
  progress.show();
  progress.update();

  foreach(index, list)
    {
      i = index.row();

      if(i + 1 <= progress.maximum())
	progress.setValue(i + 1);

      if(ui.table->item(i, col) == 0)
	continue;

      str = ui.table->item(i, col)->text();
      itemType = misc_functions::getColumnString
	(ui.table, i, ui.table->columnNumber("Type")).
	toLower();

      if(itemType != "photograph collection")
	itemType = itemType.remove(" ");
      else
	itemType = itemType.replace(" ", "_");

      query.prepare(QString("DELETE FROM %1 WHERE myoid = ?").arg(itemType));
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

      progress.update();
#ifndef Q_OS_MAC
      qapp->processEvents();
#endif
    }

  progress.hide();

  /*
  ** Provide some fancy messages.
  */

  if(error)
    QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			  tr("Unable to delete all or some of the selected "
			     "items."));

  if(numdeleted > 0)
    slotRefresh();

  list.clear();
}

/*
** -- closeEvent() --
*/

void qtbook::closeEvent(QCloseEvent *e)
{
  slotExit();
  QMainWindow::closeEvent(e);
}

/*
** -- slotRefresh() --
*/

void qtbook::slotRefresh(void)
{
  if(db.isOpen())
    {
      QString str = "";
      QVariant data(ui.typefilter->itemData(ui.typefilter->currentIndex()));

      if(data.toString() == "All Overdue" && roles.isEmpty())
	str = db.userName();
      else if(data.toString() == "All Requested" && roles.isEmpty())
	str = db.userName();
      else if(data.toString() == "All Reserved" && roles.isEmpty())
	str = db.userName();
      else if(data.toString() == "All Reserved")
	str = "%";

      (void) populateTable(POPULATE_ALL, data.toString(), str.trimmed());
    }
}

/*
** -- slotResizeColumnsAfterSort() --
*/

void qtbook::slotResizeColumnsAfterSort(void)
{
  QObject *object = qobject_cast<QObject *> (sender());
  QObject *parent = 0;

  if(object != 0 && object->parent() != 0)
    {
      qapp->setOverrideCursor(Qt::WaitCursor);
      parent = object->parent();
      (static_cast<QTableWidget *> (parent))->resizeColumnsToContents();
      (static_cast<QTableWidget *> (parent))->horizontalHeader()->
	setStretchLastSection(true);
      qapp->restoreOverrideCursor();
    }
}

/*
** -- slotUpdateIndicesAfterSort() --
*/

void qtbook::slotUpdateIndicesAfterSort(int column)
{
  int i = 0;
  QString oid = "";
  QString itemType = "";
  Qt::SortOrder order;

  qapp->setOverrideCursor(Qt::WaitCursor);

  if(ui.table->horizontalHeader()->sortIndicatorOrder() !=
     Qt::AscendingOrder)
    order = Qt::DescendingOrder;
  else
    order = Qt::AscendingOrder;

  ui.table->horizontalHeader()->setSortIndicator(column, order);
  ui.table->sortByColumn(column, order);

  for(i = 0; i < ui.table->rowCount(); i++)
    {
      oid = misc_functions::getColumnString
	(ui.table, i, ui.table->columnNumber("MYOID"));
      itemType = misc_functions::getColumnString
	(ui.table, i, ui.table->columnNumber("Type"));
      itemType = itemType.toLower().remove(" ");
      updateRows(oid, i, itemType);
    }

  qapp->restoreOverrideCursor();
}

/*
** -- slotResizeColumns() --
*/

void qtbook::slotResizeColumns(void)
{
  qapp->setOverrideCursor(Qt::WaitCursor);
  ui.table->resizeColumnsToContents();
  ui.table->horizontalHeader()->setStretchLastSection(true);
  qapp->restoreOverrideCursor();
}

/*
** -- slotAllGo() --
*/

void qtbook::slotAllGo(void)
{
  (void) populateTable(POPULATE_SEARCH, "All", QString(""));
}

/*
** -- slotQuery() --
*/

void qtbook::slotQuery(void)
{
}

/*
** -- slotAddBorrower() --
*/

void qtbook::slotAddBorrower(void)
{
  QDate now = QDate::currentDate();
  QDateTime nowTime = QDateTime::currentDateTime();

  misc_functions::highlightWidget(userinfo_diag->userinfo.memberid,
				  QColor(255, 248, 220));
  userinfo_diag->memberProperties.clear();
  userinfo_diag->userinfo.memberid->setReadOnly(false);
  userinfo_diag->userinfo.memberid->setText
    ("m" + nowTime.toString("yyyyMMddhhmmss"));
  userinfo_diag->userinfo.membersince->setDate(now);
  userinfo_diag->userinfo.membersince->setMaximumDate(now);
  userinfo_diag->userinfo.firstName->clear();
  userinfo_diag->userinfo.lastName->clear();
  userinfo_diag->userinfo.middle->clear();
  userinfo_diag->userinfo.dob->setDate(now.addYears(-25));
  userinfo_diag->userinfo.sex->setCurrentIndex(0);
  userinfo_diag->userinfo.street->clear();
  userinfo_diag->userinfo.city->clear();
  userinfo_diag->userinfo.state->setCurrentIndex(0);
  userinfo_diag->userinfo.zip->setText("00000");
  userinfo_diag->userinfo.zip->setCursorPosition(0);
  userinfo_diag->userinfo.telephoneNumber->clear();
  userinfo_diag->userinfo.email->clear();
  userinfo_diag->userinfo.expirationdate->setDate
    (QDate::fromString("01/01/3000", "MM/dd/yyyy"));
  userinfo_diag->userinfo.overduefees->setValue(0.00);
  userinfo_diag->userinfo.comments->clear();
  userinfo_diag->userinfo.memberclass->clear();
  userinfo_diag->userinfo.generalregistrationnumber->clear();
  userinfo_diag->memberProperties["membersince"] =
    userinfo_diag->userinfo.membersince->date().toString
    (Qt::ISODate);
  userinfo_diag->memberProperties["dob"] =
    userinfo_diag->userinfo.dob->date().toString
    (Qt::ISODate);
  userinfo_diag->memberProperties["sex"] =
    userinfo_diag->userinfo.sex->currentText();
  userinfo_diag->memberProperties["state_abbr"] =
    userinfo_diag->userinfo.state->currentText();
  userinfo_diag->memberProperties["zip"] = userinfo_diag->userinfo.zip->
    text();
  userinfo_diag->memberProperties["telephone_num"] =
    userinfo_diag->userinfo.telephoneNumber->text();
  userinfo_diag->memberProperties["expiration_date"] =
    userinfo_diag->userinfo.expirationdate->date().toString
    (Qt::ISODate);
  userinfo_diag->memberProperties["overdue_fees"] =
    QString::number(userinfo_diag->userinfo.overduefees->value());
  userinfo_diag->setWindowTitle(tr("BiblioteQ: Create New Member"));
  engUserinfoTitle = "Create New Member";
  userinfo_diag->userinfo.prevTool->setVisible(false);
  userinfo_diag->userinfo.nextTool->setVisible(false);
  userinfo_diag->userinfo.memberid->selectAll();
  userinfo_diag->userinfo.tabWidget->setCurrentIndex(0);
  userinfo_diag->userinfo.memberid->setFocus();
  userinfo_diag->updateGeometry();
  misc_functions::center(userinfo_diag, members_diag);
  userinfo_diag->show();
}

/*
** -- slotSaveUser() --
*/

void qtbook::slotSaveUser(void)
{
  int i = 0;
  int row = bb.table->currentRow();
  int count = -1;
  QString str = "";
  QString checksum = "";
  QString errorstr = "";
  QSqlQuery query(db);

  str = userinfo_diag->userinfo.memberid->text().trimmed();
  userinfo_diag->userinfo.memberid->setText(str);
  str = userinfo_diag->userinfo.firstName->text().trimmed();
  userinfo_diag->userinfo.firstName->setText(str);
  str = userinfo_diag->userinfo.lastName->text().trimmed();
  userinfo_diag->userinfo.lastName->setText(str);
  str = userinfo_diag->userinfo.middle->text().trimmed();
  userinfo_diag->userinfo.middle->setText(str);
  str = userinfo_diag->userinfo.street->text().trimmed();
  userinfo_diag->userinfo.street->setText(str);
  str = userinfo_diag->userinfo.city->text().trimmed();
  userinfo_diag->userinfo.city->setText(str);
  str = userinfo_diag->userinfo.zip->text().trimmed();
  userinfo_diag->userinfo.zip->setText(str);
  str = userinfo_diag->userinfo.telephoneNumber->text().trimmed();
  userinfo_diag->userinfo.telephoneNumber->setText(str);
  str = userinfo_diag->userinfo.email->text().trimmed();
  userinfo_diag->userinfo.email->setText(str);

  if(engUserinfoTitle.contains("New"))
    {
      if(userinfo_diag->userinfo.memberid->text().length() <= 5)
	{
	  QMessageBox::critical(userinfo_diag, tr("BiblioteQ: User Error"),
				tr("The Member ID must be at least five "
				   "characters long."));
	  userinfo_diag->userinfo.memberid->setFocus();
	  return;
	}

      qapp->setOverrideCursor(Qt::WaitCursor);
      int ucount = misc_functions::userCount
	(userinfo_diag->userinfo.memberid->text(), db, errorstr);
      qapp->restoreOverrideCursor();

      if(ucount > 0)
	{
	  QMessageBox::critical
	    (userinfo_diag, tr("BiblioteQ: User Error"),
	     QString(tr("The Member ID ")) +
	     userinfo_diag->userinfo.memberid->text() +
	     QString(tr(" already exists.")));
	  userinfo_diag->userinfo.memberid->setFocus();
	  return;
	}
    }

  if(userinfo_diag->userinfo.firstName->text().isEmpty())
    {
      QMessageBox::critical(userinfo_diag, tr("BiblioteQ: User Error"),
			    tr("Please provide a valid First Name."));
      userinfo_diag->userinfo.firstName->setFocus();
      return;
    }

  if(userinfo_diag->userinfo.lastName->text().isEmpty())
    {
      QMessageBox::critical(userinfo_diag, tr("BiblioteQ: User Error"),
			    tr("Please provide a valid Last Name."));
      userinfo_diag->userinfo.lastName->setFocus();
      return;
    }

  if(userinfo_diag->userinfo.street->text().isEmpty())
    {
      QMessageBox::critical(userinfo_diag, tr("BiblioteQ: User Error"),
			    tr("Please provide a valid Street."));
      userinfo_diag->userinfo.street->setFocus();
      return;
    }

  if(userinfo_diag->userinfo.city->text().isEmpty())
    {
      QMessageBox::critical(userinfo_diag, tr("BiblioteQ: User Error"),
			    tr("Please provide a valid City."));
      userinfo_diag->userinfo.city->setFocus();
      return;
    }

  if(userinfo_diag->userinfo.zip->text().length() != 5)
    {
      QMessageBox::critical(userinfo_diag, tr("BiblioteQ: User Error"),
			    tr("Please provide a valid ZIP Code."));
      userinfo_diag->userinfo.zip->setFocus();
      return;
    }

  checksum.append(userinfo_diag->userinfo.dob->date().
		  toString("MM/dd/yyyy"));
  checksum.append(userinfo_diag->userinfo.sex->currentText());
  checksum.append(userinfo_diag->userinfo.firstName->text());
  checksum.append(userinfo_diag->userinfo.middle->text());
  checksum.append(userinfo_diag->userinfo.lastName->text());
  checksum.append(userinfo_diag->userinfo.street->text());
  checksum.append(userinfo_diag->userinfo.city->text());
  checksum.append(userinfo_diag->userinfo.state->currentText());
  checksum.append(userinfo_diag->userinfo.zip->text());
  qapp->setOverrideCursor(Qt::WaitCursor);
  count = misc_functions::getMemberMatchCount
    (checksum,
     userinfo_diag->userinfo.memberid->text(),
     db,
     errorstr);
  qapp->restoreOverrideCursor();

  if(count < 0)
    {
      addError(QString(tr("Database Error")),
	       QString(tr("Unable to determine the uniqueness of the "
			  "proposed member.")),
	       errorstr, __FILE__, __LINE__);
      QMessageBox::critical(userinfo_diag, tr("BiblioteQ: Database Error"),
			    tr("Unable to determine the uniqueness of "
			       "the proposed member."));
      return;
    }

  if(count != 0)
    {
      QMessageBox::critical(userinfo_diag, tr("BiblioteQ: User Error"),
			    tr("An identical member already exists."));
      return;
    }

  if(engUserinfoTitle.contains("New"))
    {
      qapp->setOverrideCursor(Qt::WaitCursor);

      if(!db.transaction())
	{
	  qapp->restoreOverrideCursor();
	  addError
	    (QString(tr("Database Error")),
	     QString(tr("Unable to create a database transaction.")),
	     db.lastError().text(), __FILE__, __LINE__);
	  QMessageBox::critical
	    (userinfo_diag, tr("BiblioteQ: Database Error"),
	     tr("Unable to create a database transaction."));
	  return;
	}

      qapp->restoreOverrideCursor();
      query.prepare("INSERT INTO member "
		    "(memberid, membersince, dob, sex, "
		    "first_name, middle_init, last_name, "
		    "telephone_num, street, city, "
		    "state_abbr, zip, email, expiration_date, overdue_fees, "
		    "comments, general_registration_number, memberclass) "
		    "VALUES "
		    "(?, ?, ?, ?, "
		    "?, ?, ?, "
		    "?, ?, ?, ?, ?, ?, "
		    "?, ?, ?, ?, ?)");
      query.bindValue(0, userinfo_diag->userinfo.memberid->text().trimmed());
      query.bindValue(1, userinfo_diag->userinfo.membersince->
		      date().toString("MM/dd/yyyy"));
      query.bindValue(2, userinfo_diag->userinfo.dob->date().
		      toString("MM/dd/yyyy"));
      query.bindValue(3, userinfo_diag->userinfo.sex->currentText());
      query.bindValue(4, userinfo_diag->userinfo.firstName->text().trimmed());
      query.bindValue(5, userinfo_diag->userinfo.middle->text().trimmed());
      query.bindValue(6, userinfo_diag->userinfo.lastName->text().trimmed());
      query.bindValue(7, userinfo_diag->userinfo.telephoneNumber->text());
      query.bindValue(8, userinfo_diag->userinfo.street->text().trimmed());
      query.bindValue(9, userinfo_diag->userinfo.city->text().trimmed());
      query.bindValue(10, userinfo_diag->userinfo.state->currentText());
      query.bindValue(11, userinfo_diag->userinfo.zip->text());
      query.bindValue(12, userinfo_diag->userinfo.email->text().trimmed());
      query.bindValue(13, userinfo_diag->userinfo.expirationdate->
		      date().toString("MM/dd/yyyy"));
      query.bindValue(14, userinfo_diag->userinfo.overduefees->value());
      query.bindValue
	(15, userinfo_diag->userinfo.comments->toPlainText().trimmed());
      query.bindValue
	(16, userinfo_diag->userinfo.generalregistrationnumber->text().
	 trimmed());
      query.bindValue(17, userinfo_diag->userinfo.memberclass->text().
		      trimmed());
    }
  else
    {
      query.prepare("UPDATE member SET "
		    "membersince = ?, "
		    "dob = ?, "
		    "sex = ?, "
		    "first_name = ?, "
		    "middle_init = ?, "
		    "last_name = ?, "
		    "telephone_num = ?, "
		    "street = ?, "
		    "city = ?, "
		    "state_abbr = ?, "
		    "zip = ?, "
		    "email = ?, "
		    "expiration_date = ?, "
		    "overdue_fees = ?, "
		    "comments = ?, "
		    "general_registration_number = ?, "
		    "memberclass = ? "
		    "WHERE memberid = ?");
      query.bindValue(0, userinfo_diag->userinfo.membersince->date().
		      toString("MM/dd/yyyy"));
      query.bindValue(1, userinfo_diag->userinfo.dob->date().
		      toString("MM/dd/yyyy"));
      query.bindValue(2, userinfo_diag->userinfo.sex->currentText());
      query.bindValue(3, userinfo_diag->userinfo.firstName->text().trimmed());
      query.bindValue(4, userinfo_diag->userinfo.middle->text().trimmed());
      query.bindValue(5, userinfo_diag->userinfo.lastName->text().trimmed());
      query.bindValue(6, userinfo_diag->userinfo.telephoneNumber->text());
      query.bindValue(7, userinfo_diag->userinfo.street->text().trimmed());
      query.bindValue(8, userinfo_diag->userinfo.city->text().trimmed());
      query.bindValue(9, userinfo_diag->userinfo.state->currentText());
      query.bindValue(10, userinfo_diag->userinfo.zip->text());
      query.bindValue(11, userinfo_diag->userinfo.email->text().trimmed());
      query.bindValue(12, userinfo_diag->userinfo.expirationdate->
		      date().toString("MM/dd/yyyy"));
      query.bindValue(13, userinfo_diag->userinfo.overduefees->value());
      query.bindValue(14, userinfo_diag->userinfo.comments->toPlainText().
		      trimmed());
      query.bindValue(15, userinfo_diag->userinfo.generalregistrationnumber->
		      text().trimmed());
      query.bindValue(16, userinfo_diag->userinfo.memberclass->text().
		      trimmed());
      query.bindValue(17, userinfo_diag->userinfo.memberid->text().trimmed());
    }

  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec())
    {
      if(engUserinfoTitle.contains("New"))
	if(!db.rollback())
	  addError
	    (QString(tr("Database Error")), QString(tr("Rollback failure.")),
	     db.lastError().text(), __FILE__, __LINE__);

      qapp->restoreOverrideCursor();
      addError(QString(tr("Database Error")),
	       QString(tr("Unable to save the member's information.")),
	       query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(userinfo_diag, tr("BiblioteQ: Database Error"),
			    tr("Unable to save the member's information."));
    }
  else
    {
      if(engUserinfoTitle.contains("New"))
	{
	  /*
	  ** Create a database account for the new member.
	  */

	  misc_functions::DBAccount(userinfo_diag->userinfo.memberid->text(),
				    db, misc_functions::CREATE_USER,
				    errorstr);

	  if(!errorstr.isEmpty())
	    {
	      if(!db.rollback())
		addError
		  (QString(tr("Database Error")),
		   QString(tr("Rollback failure.")),
		   db.lastError().text(), __FILE__, __LINE__);

	      qapp->restoreOverrideCursor();
	      addError
		(QString(tr("Database Error")),
		 QString(tr("An error occurred while attempting to "
			    "create a database account "
			    "for the new member.")),
		 errorstr, __FILE__, __LINE__);
	      QMessageBox::critical
		(userinfo_diag,
		 tr("BiblioteQ: Database Error"),
		 tr("An error occurred while attempting to "
		    "create a database account "
		    "for the new member."));
	      return;
	    }
	  else
	    {
	      if(!db.commit())
		{
		  addError
		    (QString(tr("Database Error")),
		     QString(tr("Unable to commit the current database "
				"transaction.")),
		     db.lastError().text(), __FILE__,
		     __LINE__);
		  db.rollback();
		  qapp->restoreOverrideCursor();
		  QMessageBox::critical(userinfo_diag,
					tr("BiblioteQ: Database Error"),
					tr("Unable to commit the current "
					   "database transaction."));
		  return;
		}
	    }
	}
      else
	{
	  /*
	  ** Update privileges.
	  */

	  misc_functions::DBAccount(userinfo_diag->userinfo.memberid->text(),
				    db, misc_functions::UPDATE_USER,
				    errorstr);

	  if(!errorstr.isEmpty())
	    addError(QString(tr("Database Error")),
		     QString(tr("An error occurred while attempting to "
				"update the database account "
				"for ")) +
		     userinfo_diag->userinfo.memberid->text() +
		     QString(tr(".")),
		     errorstr, __FILE__, __LINE__);
	}

      qapp->restoreOverrideCursor();
      userinfo_diag->memberProperties["membersince"] =
	userinfo_diag->userinfo.membersince->date().toString
	(Qt::ISODate);
      userinfo_diag->memberProperties["dob"] =
	userinfo_diag->userinfo.dob->date().toString
	(Qt::ISODate);
      userinfo_diag->memberProperties["sex"] =
	userinfo_diag->userinfo.sex->currentText();
      userinfo_diag->memberProperties["first_name"] =
	userinfo_diag->userinfo.firstName->text().trimmed();
      userinfo_diag->memberProperties["middle_init"] =
	userinfo_diag->userinfo.middle->text().trimmed();
      userinfo_diag->memberProperties["last_name"] =
	userinfo_diag->userinfo.lastName->text().trimmed();
      userinfo_diag->memberProperties["telephone_num"] =
	userinfo_diag->userinfo.telephoneNumber->text();
      userinfo_diag->memberProperties["street"] =
	userinfo_diag->userinfo.street->text().trimmed();
      userinfo_diag->memberProperties["city"] =
	userinfo_diag->userinfo.city->text().trimmed();
      userinfo_diag->memberProperties["state_abbr"] =
	userinfo_diag->userinfo.state->currentText();
      userinfo_diag->memberProperties["zip"] =
	userinfo_diag->userinfo.zip->text();
      userinfo_diag->memberProperties["email"] =
	userinfo_diag->userinfo.email->text().trimmed();
      userinfo_diag->memberProperties["expiration_date"] =
	userinfo_diag->userinfo.expirationdate->date().toString
	(Qt::ISODate);
      userinfo_diag->memberProperties["overdue_fees"] =
	QString::number(userinfo_diag->userinfo.overduefees->value());
      userinfo_diag->memberProperties["comments"] =
	userinfo_diag->userinfo.comments->toPlainText().trimmed();
      userinfo_diag->memberProperties["general_registration_number"] =
	userinfo_diag->userinfo.generalregistrationnumber->text().trimmed();
      userinfo_diag->memberProperties["memberclass"] =
	userinfo_diag->userinfo.memberclass->text().trimmed();

      if(engUserinfoTitle.contains("Modify"))
	{
	  bb.table->setSortingEnabled(false);

	  for(i = 0; i < m_bbColumnHeaderIndexes.count(); i++)
	    {
	      if(m_bbColumnHeaderIndexes.at(i) == "First Name")
		bb.table->item(row, i)->setText
		  (userinfo_diag->userinfo.firstName->text());
	      else if(m_bbColumnHeaderIndexes.at(i) == "Last Name")
		bb.table->item(row, i)->setText
		  (userinfo_diag->userinfo.lastName->text());
	      else if(m_bbColumnHeaderIndexes.at(i) == "Member Since")
		bb.table->item(row, i)->setText
		  (userinfo_diag->userinfo.membersince->date().
		   toString(Qt::ISODate));
	      else if(m_bbColumnHeaderIndexes.at(i) == "Expiration Date")
		bb.table->item(row, i)->setText
		  (userinfo_diag->userinfo.expirationdate->
		   date().toString(Qt::ISODate));
	    }

	  bb.table->setSortingEnabled(true);
	}
      else
	{
	  userinfo_diag->close();

	  if(db.driverName() != "QSQLITE")
	    QMessageBox::information
	      (members_diag,
	       tr("BiblioteQ: Information"),
	       tr("Please notify the new member that their "
		  "default password has been set "
		  "to tempPass."));

	  slotPopulateMembersBrowser();
	}
    }
}

/*
** -- readGlobalSetup() --
*/

void qtbook::readGlobalSetup(void)
{
#ifdef Q_OS_WIN32
  QSettings settings("biblioteq.conf", QSettings::IniFormat);
#else
  QSettings settings(CONFIGFILE, QSettings::IniFormat);
#endif

  for(int i = 0; i < settings.childGroups().size(); i++)
    {
      settings.beginGroup(settings.childGroups().at(i));

      if(settings.group() == "Amazon Back Cover Images")
	{
	  amazonImages["back_cover_host"] = settings.value("host", "").
	    toString().trimmed();
	  amazonImages["back_cover_path"] = settings.value("path", "").
	    toString().trimmed();
	  amazonImages["back_proxy_host"] = settings.value("proxy_host", "").
	    toString().trimmed();
	  amazonImages["back_proxy_port"] = settings.value("proxy_port", "").
	    toString().trimmed();
	  amazonImages["back_proxy_type"] = settings.value("proxy_type", "").
	    toString().trimmed();
	  amazonImages["back_proxy_username"] = settings.value
	    ("proxy_username", "").toString().trimmed();
	  amazonImages["back_proxy_password"] = settings.value
	    ("proxy_password", "").toString().trimmed();
	}
      else if(settings.group() == "Amazon Front Cover Images")
	{
	  amazonImages["front_cover_host"] = settings.value("host", "").
	    toString().trimmed();
	  amazonImages["front_cover_path"] = settings.value("path", "").
	    toString().trimmed();
	  amazonImages["front_proxy_host"] = settings.value("proxy_host", "").
	    toString().trimmed();
	  amazonImages["front_proxy_port"] = settings.value("proxy_port", "").
	    toString().trimmed();
	  amazonImages["front_proxy_type"] = settings.value("proxy_type", "").
	    toString().trimmed();
	  amazonImages["front_proxy_username"] = settings.value
	    ("proxy_username", "").toString().trimmed();
	  amazonImages["front_proxy_password"] = settings.value
	    ("proxy_password", "").toString().trimmed();
	}
      else if(settings.group().startsWith("Branch"))
	{
	  if(!settings.value("database_name", "").
	     toString().trimmed().isEmpty())
	    {
	      QHash<QString, QString> hash;

	      hash["branch_name"] = settings.value("database_name", "").
		toString().trimmed();
	      hash["hostname"] = settings.value("hostname", "").
		toString().trimmed();
	      hash["database_type"] = settings.value("database_type", "").
		toString().trimmed();
	      hash["port"] = settings.value("port", "").toString().trimmed();
	      hash["ssl_enabled"] = settings.value("ssl_enabled", "").
		toString().trimmed();
	      branches[settings.value("database_name", "").
		       toString().trimmed()] = hash;
	    }
	}
      else if(settings.group().startsWith("SRU"))
	{
	  if(!settings.value("name", "").toString().trimmed().isEmpty())
	    {
	      QHash<QString, QString> hash;

	      hash["Name"] = settings.value("name", "").toString().trimmed();
	      hash["URL"] = settings.value("URL", "").toString().trimmed();
	      sruMaps[settings.value("name", "").toString().trimmed()] = hash;
	    }
	}
      else if(settings.group().startsWith("Z39.50"))
	{
	  if(!settings.value("name", "").toString().trimmed().isEmpty())
	    {
	      QHash<QString, QString> hash;

	      hash["Name"] = settings.value("name", "").toString().trimmed();
	      hash["Address"] = settings.value("hostname", "").
		toString().trimmed();
	      hash["Port"] = settings.value("port", "").toString().trimmed();
	      hash["Database"] = settings.value("database_name", "").
		toString().trimmed();
	      hash["Format"] = settings.value("format", "").
		toString().trimmed();
	      hash["Userid"] = settings.value("username", "").
		toString().trimmed();
	      hash["Password"] = settings.value("password", "").
		toString().trimmed();
	      hash["proxy_host"] = settings.value("proxy_host", "").
		toString().trimmed();
	      hash["proxy_port"] = settings.value("proxy_port", "").
		toString().trimmed();
	      z3950Maps[settings.value("name", "").toString().trimmed()] =
		hash;
	    }
	}

      settings.endGroup();
    }

  QStringList list(branches.keys());

  for(int i = 0; i < list.size(); i++)
    br.branch_name->addItem(list.at(i));

  if(br.branch_name->count() == 0)
    {
      QHash<QString, QString> hash;

      hash["branch_name"] = "local_db";
      hash["hostname"] = "127.0.0.1";
      hash["database_type"] = "sqlite";
      hash["port"] = "-1";
      hash["ssl_enabled"] = "false";

      if(!branches.contains(hash["branch_name"]))
	branches[hash["branch_name"]] = hash;

      br.branch_name->addItem(hash["branch_name"]);
    }

  if(sruMaps.isEmpty())
    {
      QHash<QString, QString> hash;

      hash["Name"] = "Library of Congress";
      hash["URL"] = "http://www.loc.gov/z39voy?operation=searchRetrieve&"
	"version=1.1&query=bath.isbn=%1&recordSchema=marcxml&"
	"startRecord=1&maximumRecords=1";
      sruMaps["Library of Congress"] = hash;
    }

  if(z3950Maps.isEmpty())
    {
      QHash<QString, QString> hash;

      hash["Name"] = "Library of Congress";
      hash["Address"] = "z3950.loc.gov";
      hash["Port"] = "7090";
      hash["Database"] = "Voyager";
      hash["Format"] = "marc8,utf-8";
      z3950Maps["Library of Congress"] = hash;
    }
}

/*
** -- readConfig() --
*/

void qtbook::readConfig(void)
{
  QFont font;
  QSettings settings;

  ui.actionShowGrid->setChecked(settings.value("show_table_grid",
					       false).toBool());
  ui.actionPopulateOnStart->setChecked
    (settings.value("populate_table_on_connect", false).toBool());
  ui.actionResetErrorLogOnDisconnect->setChecked
    (settings.value("reset_error_log_on_disconnect", false).toBool());
  ui.actionAutoPopulateOnCreation->setChecked
    (settings.value("automatically_populate_on_create", false).toBool());

  if(settings.contains("main_window_geometry"))
    {
      ui.actionPreserveGeometry->setChecked(true);
      restoreGeometry(settings.value("main_window_geometry").toByteArray());
    }
  else
    ui.actionPreserveGeometry->setChecked(false);

#ifndef Q_OS_MAC
  font = qapp->font();

  if(settings.contains("global_font"))
    font.fromString(settings.value("global_font", "").toString());

  qapp->setFont(font);
#endif
  ui.actionAutomaticallySaveSettingsOnExit->setChecked
    (settings.value("save_settings_on_exit", false).toBool());
  ui.actionPopulate_Members_Browser_Table_on_Display->setChecked
    (settings.value("automatically_populate_members_"
		    "list_on_display", false).toBool());
  ui.actionPopulate_Administrator_Browser_Table_on_Display->setChecked
    (settings.value("automatically_populate_admin_list_on_display",
		    false).toBool());
  ui.actionPopulate_Database_Enumerations_Browser_on_Display->setChecked
    (settings.value("automatically_populate_enum_list_on_display",
		    false).toBool());

  QHash<QString, QString> states;

  for(int i = 0; i < settings.allKeys().size(); i++)
    if(settings.allKeys().at(i).contains("_header_state"))
      states[settings.allKeys().at(i)] =
	settings.value(settings.allKeys().at(i)).toString();

  ui.table->parseStates(states);
  states.clear();

  bool found = false;

  for(int i = 0; i < ui.menuPreferredSRUSite->actions().size(); i++)
    if(settings.value("preferred_sru_site").toString().trimmed() ==
       ui.menuPreferredSRUSite->actions()[i]->text())
      {
	found = true;
	ui.menuPreferredSRUSite->actions()[i]->setChecked(true);
	break;
      }

  if(!found && !ui.menuPreferredSRUSite->actions().isEmpty())
    ui.menuPreferredSRUSite->actions()[0]->setChecked(true);

  found = false;

  for(int i = 0; i < ui.menuPreferredZ3950Server->actions().size(); i++)
    if(settings.value("preferred_z3950_site").toString().trimmed() ==
       ui.menuPreferredZ3950Server->actions()[i]->text())
      {
	found = true;
	ui.menuPreferredZ3950Server->actions()[i]->setChecked(true);
	break;
      }

  if(!found && !ui.menuPreferredZ3950Server->actions().isEmpty())
    ui.menuPreferredZ3950Server->actions()[0]->setChecked(true);

  int index = br.branch_name->findText
    (settings.value("previous_branch_name", "").toString());

  if(index >= 0)
    br.branch_name->setCurrentIndex(index);
  else
    br.branch_name->setCurrentIndex(0);

  int viewModeIndex = settings.value("view_mode_index", 1).toInt();

  if(viewModeIndex < 0 || viewModeIndex > 1)
    viewModeIndex = 1;

  if(!findChild<QActionGroup *> ("ViewModeMenu")->actions().isEmpty())
    findChild<QActionGroup *> ("ViewModeMenu")->
      actions().at(viewModeIndex)->setChecked(true);

  ui.stackedWidget->setCurrentIndex(viewModeIndex);

  if(ui.stackedWidget->currentIndex() == 0)
    ui.table->setSelectionMode(QAbstractItemView::MultiSelection);
  else
    ui.table->setSelectionMode(QAbstractItemView::ExtendedSelection);

  slotResizeColumns();
  createSqliteMenuActions();
}

/*
** -- slotRemoveMember() --
*/

void qtbook::slotRemoveMember(void)
{
  int row = bb.table->currentRow();
  int totalReserved;
  QString errorstr = "";
  QString memberid = "";
  QSqlQuery query(db);
  QMap<QString, QString> counts;

  if(row < 0)
    {
      QMessageBox::critical(members_diag, tr("BiblioteQ: User Error"),
			    tr("Please select a member to delete."));
      return;
    }

  memberid = misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("Member ID"));
  qapp->setOverrideCursor(Qt::WaitCursor);
  counts = misc_functions::getItemsReservedCounts(db, memberid, errorstr);
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    {
      addError(QString(tr("Database Error")),
	       QString(tr("Unable to determine the number of items that "
			  "are reserved by the selected member.")),
	       errorstr, __FILE__, __LINE__);
      QMessageBox::critical
	(members_diag, tr("BiblioteQ: Database Error"),
	 tr("Unable to determine the number of items that "
	    "are reserved by the selected member."));
      return;
    }

  totalReserved = counts["numbooks"].toInt() + counts["numcds"].toInt() +
    counts["numdvds"].toInt() + counts["numjournals"].toInt() +
    counts["nummagazines"].toInt() + counts["numvideogames"].toInt();
  counts.clear();

  if(totalReserved != 0)
    {
      QMessageBox::critical
	(members_diag, tr("BiblioteQ: User Error"),
	 tr("You may not remove a member that has reserved "
	    "items."));
      return;
    }

  if(QMessageBox::question(members_diag, tr("BiblioteQ: Question"),
			   tr("Are you sure that you wish to delete the "
			      "selected member?"),
			   QMessageBox::Yes | QMessageBox::No,
			   QMessageBox::No) == QMessageBox::No)
    return;

  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!db.transaction())
    {
      qapp->restoreOverrideCursor();
      addError
	(QString(tr("Database Error")),
	 QString(tr("Unable to create a database transaction.")),
	 db.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical
	(members_diag, tr("BiblioteQ: Database Error"),
	 tr("Unable to create a database transaction."));
      return;
    }

  qapp->restoreOverrideCursor();
  query.prepare(QString("DELETE FROM member WHERE "
			"memberid = ?"));
  query.bindValue(0, memberid);
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec())
    {
      if(!db.rollback())
	addError
	  (QString(tr("Database Error")), QString(tr("Rollback failure.")),
	   db.lastError().text(), __FILE__, __LINE__);

      qapp->restoreOverrideCursor();
      addError(QString(tr("Database Error")),
	       QString(tr("Unable to remove the selected member.")),
	       query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(members_diag, tr("BiblioteQ: Database Error"),
			    tr("Unable to remove the selected member."));
    }
  else
    {
      misc_functions::DBAccount(memberid, db, misc_functions::DELETE_USER,
				errorstr);

      if(!errorstr.isEmpty())
	{
	  addError
	    (QString(tr("Database Error")),
	     QString(tr("Unable to remove the patron account ")) +
	     memberid + tr("."),
	     errorstr, __FILE__, __LINE__);

	  if(!db.rollback())
	    addError
	      (QString(tr("Database Error")),
	       QString(tr("Rollback failure.")),
	       db.lastError().text(), __FILE__, __LINE__);

	  qapp->restoreOverrideCursor();
	  QMessageBox::critical
	    (members_diag,
	     tr("BiblioteQ: Database Error"),
	     QString(tr("Unable to remove the patron account ")) +
	     memberid + tr("."));
	}
      else
	{
	  if(!db.commit())
	    {
	      addError
		(QString(tr("Database Error")),
		 QString(tr("Unable to commit the current database "
			    "transaction.")),
		 db.lastError().text(), __FILE__,
		 __LINE__);
	      db.rollback();
	      qapp->restoreOverrideCursor();
	      QMessageBox::critical(members_diag,
				    tr("BiblioteQ: Database Error"),
				    tr("Unable to commit the current "
				       "database transaction."));
	      return;
	    }
	}

      qapp->restoreOverrideCursor();
      slotPopulateMembersBrowser();
    }
}

/*
** -- slotSaveConfig() --
*/

void qtbook::slotSaveConfig(void)
{
  QSettings settings;

  settings.setValue("show_table_grid", ui.actionShowGrid->isChecked());
  settings.setValue("populate_table_on_connect",
		    ui.actionPopulateOnStart->isChecked());
  settings.setValue("reset_error_log_on_disconnect",
		    ui.actionResetErrorLogOnDisconnect->isChecked());
  settings.setValue("automatically_populate_on_create",
		    ui.actionAutoPopulateOnCreation->isChecked());
  settings.setValue("save_settings_on_exit",
		    ui.actionAutomaticallySaveSettingsOnExit->isChecked());
  settings.setValue
    ("automatically_populate_members_list_on_display",
     ui.actionPopulate_Members_Browser_Table_on_Display->isChecked());
  settings.setValue
    ("automatically_populate_admin_list_on_display",
     ui.actionPopulate_Administrator_Browser_Table_on_Display->isChecked());
  settings.setValue
    ("automatically_populate_enum_list_on_display",
     ui.actionPopulate_Database_Enumerations_Browser_on_Display->
     isChecked());
  settings.setValue("global_font", font().toString());
  settings.setValue("last_category", getTypeFilterString());
  settings.setValue("locale", s_locale);

  if(ui.actionPreserveGeometry->isChecked())
    settings.setValue("main_window_geometry", saveGeometry());
  else
    settings.remove("main_window_geometry");

  if(db.isOpen())
    {
      if(db.driverName() == "QSQLITE")
	{
	  for(int i = 0; i < ui.menuEntriesPerPage->actions().size(); i++)
	    if(ui.menuEntriesPerPage->actions()[i]->isChecked())
	      {
		settings.setValue
		  ("sqlite_entries_per_page",
		   ui.menuEntriesPerPage->actions()[i]->data().toInt());
		break;
	      }
	}
      else
	{
	  for(int i = 0; i < ui.menuEntriesPerPage->actions().size(); i++)
	    if(ui.menuEntriesPerPage->actions()[i]->isChecked())
	      {
		settings.setValue
		  ("postgresql_entries_per_page",
		   ui.menuEntriesPerPage->actions()[i]->data().toInt());
		break;
	      }
	}
    }

  for(int i = 0; i < ui.menuPreferredSRUSite->actions().size(); i++)
    if(ui.menuPreferredSRUSite->actions()[i]->isChecked())
      {
	settings.setValue
	  ("preferred_sru_site",
	   ui.menuPreferredSRUSite->actions()[i]->text().trimmed());
	break;
      }

  for(int i = 0; i < ui.menuPreferredZ3950Server->actions().size(); i++)
    if(ui.menuPreferredZ3950Server->actions()[i]->isChecked())
      {
	settings.setValue
	  ("preferred_z3950_site",
	   ui.menuPreferredZ3950Server->actions()[i]->text().trimmed());
	break;
      }

  for(int i = 0; i < ui.table->friendlyStates().keys().size(); i++)
    settings.setValue
      (ui.table->friendlyStates().keys().at(i),
       ui.table->friendlyStates()[ui.table->friendlyStates().keys().at(i)]);
}

/*
** -- slotShowColumns() --
*/

void qtbook::slotShowColumns(void)
{
  int i = 0;

  qapp->setOverrideCursor(Qt::WaitCursor);

  for(i = 0; i < ui.table->columnNames().count(); i++)
    if(ui.table->columnNames().at(i) == "Publisher")
      ui.table->setColumnHidden(i, false);

  qapp->restoreOverrideCursor();
}

/*
** -- slotSceneSelectionChanged()
*/

void qtbook::slotSceneSelectionChanged(void)
{
  if(ui.stackedWidget->currentIndex() != 0)
    return;

  ui.table->clearSelection();
  ui.table->setCurrentCell(-1, -1);
  slotDisplaySummary();

  QList<QGraphicsItem *> items(ui.graphicsView->scene()->selectedItems());

  if(!items.isEmpty())
    {
      QStringList oids;
      QGraphicsItem *item = 0;

      while(!items.isEmpty())
	if((item = items.takeFirst()))
	  oids.append(item->data(0).toString());

      int column = ui.table->columnNumber("MYOID");

      for(int i = 0; i < ui.table->rowCount(); i++)
	if(ui.table->item(i, column) &&
	   oids.contains(ui.table->item(i, column)->text()))
	  ui.table->selectRow(i);

      oids.clear();
    }
}

/*
** -- slotDisplaySummary() --
*/

void qtbook::slotDisplaySummary(void)
{
  int i = 0;
  QImage backImage;
  QImage frontImage;
  QString oid = "";
  QString type = "";
  QString tmpstr = "";
  QString summary = "";

  /*
  ** Display a preview.
  */

  if(ui.itemSummary->width() > 0 && ui.table->currentRow() > -1)
    {
      i = ui.table->currentRow();
      oid = misc_functions::getColumnString
	(ui.table, i, ui.table->columnNumber("MYOID"));

      if(ui.stackedWidget->currentIndex() == 1)
	{
	  /*
	  ** This method is also called by slotSceneSelectionChanged().
	  */

	  QPainterPath painterPath;
	  QList<QGraphicsItem *> items(ui.graphicsView->scene()->items());
	  QList<QTableWidgetItem *> tableItems(ui.table->selectedItems());

	  for(int ii = 0; ii < tableItems.size(); ii++)
	    {
	      QString oid = misc_functions::getColumnString
		(ui.table, tableItems.at(ii)->row(),
		 ui.table->columnNumber("MYOID"));

	      for(int jj = 0; jj < items.size(); jj++)
		if(oid == items.at(jj)->data(0).toString())
		  {
		    QRectF rect;

		    rect.setTopLeft(items.at(jj)->scenePos());
		    rect.setWidth(126);
		    rect.setHeight(187);
		    painterPath.addRect(rect);
		  }
		else
		  items.at(jj)->setSelected(false);
	    }

	  items.clear();
	  ui.graphicsView->scene()->setSelectionArea(painterPath);
	}

      type = misc_functions::getColumnString
	(ui.table, i, ui.table->columnNumber("Type"));
      summary = "<html>";

      if(type == "Book")
	{
	  summary += "<b>" +
	    misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Title")) +
	    "</b>";
	  summary += "<br>";
	  tmpstr = misc_functions::getColumnString
	    (ui.table, i,
	     ui.table->columnNumber("ISBN-10"));

	  if(tmpstr.isEmpty())
	    tmpstr = misc_functions::getColumnString
	      (ui.table, i,
	       ui.table->columnNumber("ID Number"));

	  if(tmpstr.isEmpty())
	    tmpstr = "<br>";

	  summary += tmpstr;
	  summary += "<br>";
	  summary += misc_functions::getColumnString
	    (ui.table, i,
	     ui.table->columnNumber("Publication Date"));
	  summary += "<br>";
	  summary += misc_functions::getColumnString
	    (ui.table, i,
	     ui.table->columnNumber("Publisher"));
	  summary += "<br>";
	  summary += misc_functions::getColumnString
	    (ui.table, i,
	     ui.table->columnNumber("Place of Publication"));
	  summary += "<br>";
	}
      else if(type == "CD")
	{
	  summary += "<b>" +
	    misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Title")) +
	    "</b>";
	  summary += "<br>";
	  tmpstr = misc_functions::getColumnString
	    (ui.table, i,
	     ui.table->columnNumber("Catalog Number"));

	  if(tmpstr.isEmpty())
	    tmpstr = misc_functions::getColumnString
	      (ui.table, i,
	       ui.table->columnNumber("ID Number"));

	  if(tmpstr.isEmpty())
	    tmpstr = "<br>";

	  summary += tmpstr;
	  summary += "<br>";
	  tmpstr = misc_functions::getColumnString
	    (ui.table, i,
	     ui.table->columnNumber("Publication Date"));

	  if(tmpstr.isEmpty())
	    tmpstr = misc_functions::getColumnString
	      (ui.table, i,
	       ui.table->columnNumber("Release Date"));

	  if(tmpstr.isEmpty())
	    tmpstr = "<br>";

	  summary += tmpstr;
	  summary += "<br>";
	  tmpstr = misc_functions::getColumnString
	    (ui.table, i,
	     ui.table->columnNumber("Publisher"));

	  if(tmpstr.isEmpty())
	    tmpstr = misc_functions::getColumnString
	      (ui.table, i,
	       ui.table->columnNumber("Recording Label"));

	  if(tmpstr.isEmpty())
	    tmpstr = "<br>";

	  summary += tmpstr;
	  summary += "<br>";
	}
      else if(type == "DVD")
	{
	  summary += "<b>" +
	    misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Title")) +
	    "</b>";
	  summary += "<br>";
	  tmpstr = misc_functions::getColumnString
	    (ui.table, i,
	     ui.table->columnNumber("UPC"));

	  if(tmpstr.isEmpty())
	    tmpstr = misc_functions::getColumnString
	      (ui.table, i,
	       ui.table->columnNumber("ID Number"));

	  if(tmpstr.isEmpty())
	    tmpstr = "<br>";

	  summary += tmpstr;
	  summary += "<br>";
	  tmpstr = misc_functions::getColumnString
	    (ui.table, i,
	     ui.table->columnNumber("Publication Date"));

	  if(tmpstr.isEmpty())
	    tmpstr = misc_functions::getColumnString
	      (ui.table, i,
	       ui.table->columnNumber("Release Date"));

	  if(tmpstr.isEmpty())
	    tmpstr = "<br>";

	  summary += tmpstr;
	  summary += "<br>";
	  tmpstr = misc_functions::getColumnString
	    (ui.table, i,
	     ui.table->columnNumber("Publisher"));

	  if(tmpstr.isEmpty())
	    tmpstr = misc_functions::getColumnString
	      (ui.table, i,
	       ui.table->columnNumber("Studio"));

	  if(tmpstr.isEmpty())
	    tmpstr = "<br>";

	  summary += tmpstr;
	  summary += "<br>";
	}
      else if(type == "Journal" || type == "Magazine")
	{
	  summary += "<b>" +
	    misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Title")) +
	    "</b>";
	  summary += "<br>";
	  tmpstr = misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("ISSN"));

	  if(tmpstr.isEmpty())
	    tmpstr = misc_functions::getColumnString
	      (ui.table, i,
	       ui.table->columnNumber("ID Number"));

	  if(tmpstr.isEmpty())
	    tmpstr = "<br>";

	  summary += tmpstr;
	  summary += "<br>";
	  summary += misc_functions::getColumnString
	    (ui.table, i,
	     ui.table->columnNumber("Publication Date"));
	  summary += "<br>";
	  summary += misc_functions::getColumnString
	    (ui.table, i,
	     ui.table->columnNumber("Publisher"));
	  summary += "<br>";
	  summary += misc_functions::getColumnString
	    (ui.table, i,
	     ui.table->columnNumber("Place of Publication"));
	  summary += "<br>";
	}
      else if(type == "Photograph Collection")
	{
	  summary += "<b>" +
	    misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Title")) +
	    "</b>";
	  summary += "<br>";
	  tmpstr = misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("ID"));

	  if(tmpstr.isEmpty())
	    tmpstr = misc_functions::getColumnString
	      (ui.table, i,
	       ui.table->columnNumber("ID Number"));

	  if(tmpstr.isEmpty())
	    tmpstr = "<br>";

	  summary += tmpstr;
	  summary += "<br>";
	}
      else if(type == "Video Game")
	{
	  summary += "<b>" +
	    misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("Title")) +
	    "</b>";
	  summary += "<br>";
	  tmpstr = misc_functions::getColumnString
	    (ui.table, i,
	     ui.table->columnNumber("UPC"));

	  if(tmpstr.isEmpty())
	    tmpstr = misc_functions::getColumnString
	      (ui.table, i,
	       ui.table->columnNumber("ID Number"));

	  if(tmpstr.isEmpty())
	    tmpstr = "<br>";

	  summary += tmpstr;
	  summary += "<br>";
	  tmpstr = misc_functions::getColumnString
	    (ui.table, i,
	     ui.table->columnNumber("Publication Date"));

	  if(tmpstr.isEmpty())
	    tmpstr = misc_functions::getColumnString
	      (ui.table, i,
	       ui.table->columnNumber("Release Date"));

	  if(tmpstr.isEmpty())
	    tmpstr = "<br>";

	  summary += tmpstr;
	  summary += "<br>";
	  summary += misc_functions::getColumnString
	    (ui.table, i,
	     ui.table->columnNumber("Publisher"));
	  summary += "<br>";
	}

      summary = summary.remove("<br><br>");
      summary += misc_functions::getAbstractInfo(oid, type, db);
      summary += "<br>";

      if(type != "Photograph Collection")
	{
	  tmpstr = misc_functions::getColumnString
	    (ui.table, i,
	     ui.table->columnNumber("Availability"));

	  if(!tmpstr.isEmpty())
	    {
	      if(tmpstr.toInt() > 0)
		summary += tr("Available") + "<br>";
	      else
		summary += tr("Unavailable") + "<br>";
	    }
	}

      summary += misc_functions::getColumnString
	(ui.table, i,
	 ui.table->columnNumber("Location"));
      summary += "</html>";
      ui.summary->setText(summary);
      ui.summary->setVisible(true);
      qapp->setOverrideCursor(Qt::WaitCursor);

      if(type != "Photograph Collection")
	frontImage = misc_functions::getImage(oid, "front_cover", type,
					      db);
      else
	frontImage = misc_functions::getImage(oid, "image_scaled", type,
					      db);

      if(type != "Photograph Collection")
	backImage = misc_functions::getImage(oid, "back_cover", type,
					     db);

      qapp->restoreOverrideCursor();

      /*
      ** The size of no_image.png is 126x187.
      */

      if(frontImage.isNull())
	frontImage = QImage("icons.d/no_image.png");

      if(!frontImage.isNull())
	frontImage = frontImage.scaled
	  (126, 187, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

      if(type != "Photograph Collection")
	{
	  if(backImage.isNull())
	    backImage = QImage("icons.d/no_image.png");

	  if(!backImage.isNull())
	    backImage = backImage.scaled
	      (126, 187, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	}

      if(!frontImage.isNull())
	{
	  ui.frontImage->setVisible(true);
	  ui.frontImage->setPixmap(QPixmap().fromImage(frontImage));
	}
      else
	ui.frontImage->clear();

      if(type != "Photograph Collection")
	{
	  if(!backImage.isNull())
	    {
	      ui.backImage->setVisible(true);
	      ui.backImage->setPixmap(QPixmap().fromImage(backImage));
	    }
	  else
	    ui.backImage->clear();
	}
      else
	ui.backImage->clear();
    }
  else
    {
      /*
      ** Clear the scene.
      */

      ui.summary->setVisible(false);
      ui.summary->clear();
      ui.frontImage->setVisible(false);
      ui.frontImage->clear();
      ui.backImage->setVisible(false);
      ui.backImage->clear();
    }
}

/*
** -- slotShowNext() --
*/

void qtbook::slotShowNext(void)
{
  int row = -1;
  QTableWidget *table = 0;

  table = bb.table;
  row = table->currentRow();

  if(row == (table->rowCount() - 1))
    row = 0;
  else
    row += 1;

  if(history_diag->isVisible())
    {
      table->clearSelection();
      table->selectRow(row);
      slotShowHistory();
    }
  else
    {
      QString str("");

      if(userinfo_diag->haveMemberChanges(str))
	if(QMessageBox::question
	   (userinfo_diag, tr("BiblioteQ: Question"),
	    tr("You have unsaved data. Continue?\n%1").arg(str),
	    QMessageBox::Yes | QMessageBox::No,
	    QMessageBox::No) == QMessageBox::No)
	  return;

      table->clearSelection();
      table->selectRow(row);
      slotModifyBorrower();
    }
}

/*
** -- slotShowPrev() --
*/

void qtbook::slotShowPrev(void)
{
  int row = -1;
  QTableWidget *table = 0;

  table = bb.table;
  row = table->currentRow();

  if(row == 0)
    row = table->rowCount() - 1;
  else
    row -= 1;

  if(history_diag->isVisible())
    {
      table->clearSelection();
      table->selectRow(row);
      slotShowHistory();
    }
  else
    {
      QString str("");

      if(userinfo_diag->haveMemberChanges(str))
	if(QMessageBox::question
	   (userinfo_diag, tr("BiblioteQ: Question"),
	    tr("You have unsaved data. Continue?\n%1").arg(str),
	    QMessageBox::Yes | QMessageBox::No,
	    QMessageBox::No) == QMessageBox::No)
	  return;

      table->clearSelection();
      table->selectRow(row);
      slotModifyBorrower();
    }
}

/*
** -- slotShowConnectionDB() --
*/

void qtbook::slotShowConnectionDB(void)
{
  if(db.isOpen())
    return;

  slotBranchChanged();
}

/*
** -- slotConnectDB() --
*/

void qtbook::slotConnectDB(void)
{
  /*
  ** Prevent duplicate connections.
  */

  if(db.isOpen())
    return;

  bool error = false;
  QString str = "";
  QString drivers = "";
  QString plugins = "";
  QString errorstr = "";
  QHash<QString, QString> tmphash;

  /*
  ** Configure some database attributes.
  */

  br.userid->setFocus();
  tmphash = branches[br.branch_name->currentText()];

  if(tmphash["database_type"] == "postgresql")
    str = "QPSQL";
  else if(tmphash["database_type"] == "sqlite")
    str = "QSQLITE";
  else
    str = "QODBC";

  foreach(QString driver, QSqlDatabase::drivers())
    drivers += driver + ", ";

  if(drivers.endsWith(", "))
    drivers = drivers.mid(0, drivers.length() - 2);

  if(drivers.isEmpty())
    drivers = "N/A";

  foreach(QString path, qapp->libraryPaths())
    if(path.toLower().contains("plugin"))
      {
	plugins = path;
	break;
      }

  if(plugins.isEmpty())
    plugins = "N/A";

  if(!QSqlDatabase::isDriverAvailable(str))
    {
      tmphash.clear();
      QMessageBox::critical
	(branch_diag, tr("BiblioteQ: Database Error"),
	 tr("The selected branch's database type does not "
	    "have a driver associated with it.") + "\n" +
	 tr("The following drivers are available: ") +
	 drivers + tr(".") + "\n" +
	 tr("In addition, Qt expects plugins to exist "
	    "in: ") + plugins + tr(".") + "\n" +
	 tr("Please contact your administrator."));
      return;
    }

  db = QSqlDatabase::addDatabase(str, "Default");

  if(tmphash["database_type"] == "sqlite")
    db.setDatabaseName(br.filename->text());
  else
    {
      db.setHostName(tmphash["hostname"]);
      db.setDatabaseName(br.branch_name->currentText());
      db.setUserName(br.userid->text().trimmed());
      db.setPassword(br.password->text());
      db.setPort(tmphash["port"].toInt());
    }

  if(tmphash["database_type"] != "sqlite")
    if(tmphash["ssl_enabled"] == "true")
      db.setConnectOptions("requiressl=1");

  qapp->setOverrideCursor(Qt::WaitCursor);
  (void) db.open();
  qapp->restoreOverrideCursor();

  if(!db.isOpen())
    {
      error = true;
      addError(QString(tr("Database Error")),
	       QString(tr("Unable to open a database connection "
			  "with the provided information.")),
	       db.lastError().text(),
	       __FILE__, __LINE__);
      QMessageBox::critical(branch_diag, tr("BiblioteQ: Database Error"),
			    tr("Unable to open a database "
			       "connection with the provided information."));
    }
  else
    {
      if(!db.driver()->hasFeature(QSqlDriver::Transactions))
	{
	  error = true;
	  addError
	    (QString(tr("Database Error")),
	     QString(tr("The current database driver that you're using "
			"does not support transactions. "
			"Please upgradie your database and/or driver.")),
	     db.lastError().text(),
	     __FILE__, __LINE__);
	  QMessageBox::critical
	    (branch_diag, tr("BiblioteQ: Database Error"),
	     tr("The current database driver that you're using "
		"does not support transactions. "
		"Please upgrade your database and/or driver."));
	}
    }

  if(tmphash["database_type"] != "sqlite")
    {
      if(!error)
	{
	  qapp->setOverrideCursor(Qt::WaitCursor);
	  roles = misc_functions::getRoles
	    (db, br.userid->text().trimmed(), errorstr).toLower();
	  qapp->restoreOverrideCursor();

	  if(errorstr.isEmpty())
	    {
	      if(br.adminCheck->isChecked() && roles.isEmpty())
		{
		  error = true;
		  QMessageBox::critical
		    (branch_diag, tr("BiblioteQ: User Error"),
		     QString(tr("It appears that the user ")) +
		     br.userid->text().trimmed() +
		     QString(tr(" does not have "
				"administrator privileges.")));
		}
	      else if(!br.adminCheck->isChecked() && !roles.isEmpty())
		{
		  error = true;
		  QMessageBox::critical
		    (branch_diag, tr("BiblioteQ: User Error"),
		     tr("It appears that you are attempting to assume an "
			"administrator role in a non-administrator mode."));
		}
	      else
		{
		  misc_functions::setRole(db, errorstr, roles);

		  if(!errorstr.isEmpty())
		    {
		      error = true;
		      addError(QString(tr("Database Error")),
			       QString(tr("Unable to set "
					  "the role for ")) +
			       br.userid->text().trimmed() +
			       tr("."),
			       errorstr,
			       __FILE__, __LINE__);
		      QMessageBox::critical
			(branch_diag, tr("BiblioteQ: Database Error"),
			 QString(tr("Unable to set the role "
				    "for ")) +
			 br.userid->text().trimmed() +
			 tr("."));
		    }
		}
	    }
	  else if(br.adminCheck->isChecked())
	    {
	      error = true;
	      addError(QString(tr("Database Error")),
		       QString(tr("Unable to determine the "
				  "roles of ")) +
		       br.userid->text().trimmed() +
		       tr("."),
		       errorstr,
		       __FILE__, __LINE__);
	      QMessageBox::critical
		(branch_diag, tr("BiblioteQ: Database Error"),
		 QString(tr("Unable to determine the "
			    "roles of ")) +
		 br.userid->text().trimmed() +
		 tr("."));
	    }
	  else
	    {
	      QSqlQuery query(db);

	      if(!query.exec("SET ROLE biblioteq_patron"))
		{
		  error = true;
		  addError(QString(tr("Database Error")),
			   QString(tr("Unable to set "
				      "the role for ")) +
			   br.userid->text().trimmed() +
			   tr("."),
			   errorstr,
			   __FILE__, __LINE__);
		  QMessageBox::critical
		    (branch_diag, tr("BiblioteQ: Database Error"),
		     QString(tr("Unable to set the role "
				"for ")) +
		     br.userid->text().trimmed() +
		     tr("."));
		}
	    }
	}
    }
  else
    {
      db.exec("PRAGMA foreign_keys = ON");

      if(!error)
	roles = "administrator";
    }

  tmphash.clear();

  if(error)
    {
      db = QSqlDatabase();
      QSqlDatabase::removeDatabase("Default");
      return;
    }
  else
    branch_diag->close();

  /*
  ** We've connected successfully. Let's initialize
  ** other containers and widgets.
  */

  QSettings settings;

  settings.setValue("previous_branch_name",
		    br.branch_name->currentText());
  misc_functions::updateSQLiteDatabase(db);
  selectedBranch = branches[br.branch_name->currentText()];

  if(connected_bar_label != 0)
    {
      connected_bar_label->setPixmap(QPixmap("icons.d/16x16/connected.png"));
      connected_bar_label->setToolTip(tr("Connected"));
    }

  ui.printTool->setEnabled(true);
  ui.detailsTool->setEnabled(true);
  ui.searchTool->setEnabled(true);
  ui.customQueryTool->setEnabled(true);
  ui.actionDatabaseSearch->setEnabled(true);
  ui.actionViewDetails->setEnabled(true);
  ui.refreshTool->setEnabled(true);
  ui.actionRefreshTable->setEnabled(true);
  ui.disconnectTool->setEnabled(true);
  ui.actionDisconnect->setEnabled(true);
  ui.configTool->setEnabled(true);
  ui.connectTool->setEnabled(false);
  ui.actionConnect->setEnabled(false);

  if(db.driverName() == "QSQLITE")
    {
      ui.actionChangePassword->setEnabled(false);
      ui.menuEntriesPerPage->setEnabled(true);
      ui.menuEntriesPerPage->actions()[4]->setEnabled(true);

      /*
      ** Set the window's title.
      */

      setWindowTitle(tr("BiblioteQ: ") +
		     QFileInfo(br.filename->text()).fileName());
    }
  else
    {
      ui.menuEntriesPerPage->setEnabled(true);
      ui.menuEntriesPerPage->actions()[4]->setEnabled(false);
      ui.actionChangePassword->setEnabled(true);
      connect(ui.table, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this,
	      SLOT(slotViewDetails(void)));
      connect(ui.graphicsView->scene(), SIGNAL(itemDoubleClicked(void)), this,
	      SLOT(slotViewDetails(void)));

      /*
      ** Set the window's title.
      */

      setWindowTitle(tr("BiblioteQ: ") + selectedBranch["branch_name"] +
		     QString(" (%1)").arg(br.userid->text().trimmed()));
    }

  prepareFilter();

  if(br.adminCheck->isChecked() || db.driverName() == "QSQLITE")
    {
      if(db.driverName() == "QSQLITE")
	{
	  /*
	  ** Add the database's information to the pulldown menu.
	  */

	  bool exists = false;
	  QList<QAction *> actions = ui.menu_Recent_SQLite_Files->actions();

	  for(int i = 0; i < actions.size(); i++)
	    if(actions[i]->data().toString() == br.filename->text())
	      {
		exists = true;
		break;
	      }

	  actions.clear();

	  if(!exists)
	    {
	      int index = 1;
	      QSettings settings;
	      QStringList allKeys(settings.allKeys());

	      for(int i = 0; i < allKeys.size(); i++)
		if(allKeys[i].startsWith("sqlite_db_"))
		  index += 1;

	      allKeys.clear();
	      settings.setValue(QString("sqlite_db_%1").arg(index),
				br.filename->text());
	      createSqliteMenuActions();
	    }
	}

      adminSetup();
    }
  else
    {
      /*
      ** Patron.
      */

      ui.actionRequests->setToolTip(tr("Request Selected Item(s)"));
      ui.actionRequests->setEnabled(true);
      ui.actionReservationHistory->setEnabled(true);
    }

  bool found = false;

  if(db.driverName() == "QSQLITE")
    {
      for(int i = 0; i < ui.menuEntriesPerPage->actions().size(); i++)
	if(ui.menuEntriesPerPage->actions()[i]->data().toInt() ==
	   settings.value("sqlite_entries_per_page").toInt())
	  {
	    found = true;
	    ui.menuEntriesPerPage->actions()[i]->setChecked(true);
	    break;
	  }
    }
  else
    {
      for(int i = 0; i < ui.menuEntriesPerPage->actions().size(); i++)
	if(ui.menuEntriesPerPage->actions()[i]->data().toInt() ==
	   settings.value("postgresql_entries_per_page").toInt())
	  {
	    found = true;
	    ui.menuEntriesPerPage->actions()[i]->setChecked(true);
	    break;
	  }
    }

  if(!found && !ui.menuEntriesPerPage->actions().isEmpty())
    ui.menuEntriesPerPage->actions()[0]->setChecked(true);

  found = false;

  if(ui.typefilter->findData(QVariant(lastCategory)) > -1)
    ui.typefilter->setCurrentIndex
      (ui.typefilter->findData(QVariant(lastCategory)));

  if(ui.actionPopulateOnStart->isChecked())
    slotRefresh();
}

/*
** -- slotDisconnect() --
*/

void qtbook::slotDisconnect(void)
{
  roles = "";
  m_pages = 0;
  m_queryOffset = 0;
  userinfo_diag->memberProperties.clear();
  all_diag->close();
  members_diag->close();
  history_diag->close();
  customquery_diag->close();
  cq.tables_t->clear();
  cq.tables_t->setColumnCount(0);
  cq.tables_t->scrollToTop();
  cq.tables_t->horizontalScrollBar()->setValue(0);
  cq.tables_t->clearSelection();
  cq.tables_t->setCurrentItem(0);
  cq.query_te->clear();
  admin_diag->close();
  db_enumerations->clear();
  db_enumerations->close();
  resetAdminBrowser();
  resetMembersBrowser();
  ui.pagesLabel->setText("1");
  ui.previousPageButton->setEnabled(false);
  ui.nextPageButton->setEnabled(false);
  ui.actionReservationHistory->setEnabled(false);
  ui.printTool->setEnabled(false);
  ui.actionChangePassword->setEnabled(false);
  ui.deleteTool->setEnabled(false);
  ui.menu_Add_Item->setEnabled(false);
  ui.actionDeleteEntry->setEnabled(false);
  ui.actionDuplicateEntry->setEnabled(false);
  ui.createTool->setEnabled(false);
  ui.duplicateTool->setEnabled(false);
  ui.modifyTool->setEnabled(false);
  ui.actionModifyEntry->setEnabled(false);
  ui.searchTool->setEnabled(false);
  ui.customQueryTool->setEnabled(false);
  ui.detailsTool->setEnabled(false);
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
  ui.connectTool->setEnabled(true);
  ui.actionConnect->setEnabled(true);
  ui.actionAutoPopulateOnCreation->setEnabled(false);
  ui.menuEntriesPerPage->setEnabled(true);
  ui.menuEntriesPerPage->actions()[4]->setEnabled(true);
  ui.actionPopulate_Administrator_Browser_Table_on_Display->setEnabled
    (false);
  ui.actionPopulate_Database_Enumerations_Browser_on_Display->setEnabled
    (false);
  ui.actionPopulate_Members_Browser_Table_on_Display->setEnabled(false);
  ui.actionConfigureAdministratorPrivileges->setEnabled(false);
  ui.actionDatabase_Enumerations->setEnabled(false);
  ui.actionRequests->setEnabled(false);
  ui.actionRequests->setToolTip(tr("Item Requests"));
  ui.actionRequests->setIcon(QIcon("icons.d/32x32/request.png"));
  ui.graphicsView->scene()->clear();
  bb.table->disconnect(SIGNAL(itemDoubleClicked(QTableWidgetItem *)));
  ui.table->disconnect(SIGNAL(itemDoubleClicked(QTableWidgetItem *)));
  ui.graphicsView->scene()->disconnect(SIGNAL(itemDoubleClicked(void)));

  if(db.isOpen())
    {
      QSettings settings;

      if(db.driverName() == "QSQLITE")
	{
	  for(int i = 0; i < ui.menuEntriesPerPage->actions().size(); i++)
	    if(ui.menuEntriesPerPage->actions()[i]->isChecked())
	      {
		settings.setValue
		  ("sqlite_entries_per_page",
		   ui.menuEntriesPerPage->actions()[i]->data().toInt());
		break;
	      }
	}
      else
	{
	  for(int i = 0; i < ui.menuEntriesPerPage->actions().size(); i++)
	    if(ui.menuEntriesPerPage->actions()[i]->isChecked())
	      {
		settings.setValue
		  ("postgresql_entries_per_page",
		   ui.menuEntriesPerPage->actions()[i]->data().toInt());
		break;
	      }
	}
    }

  ui.menuEntriesPerPage->actions()[0]->setChecked(true);

  if(connected_bar_label != 0)
    {
      connected_bar_label->setPixmap
	(QPixmap("icons.d/16x16/disconnected.png"));
      connected_bar_label->setToolTip(tr("Disconnected"));
    }

  if(status_bar_label != 0)
    {
      status_bar_label->setPixmap(QPixmap("icons.d/16x16/lock.png"));
      status_bar_label->setToolTip(tr("Standard User Mode"));
    }

  if(ui.actionResetErrorLogOnDisconnect->isChecked())
    slotResetErrorLog();

  ui.table->resetTable(db.userName(), previousTypeFilter, roles);
  ui.itemsCountLabel->setText(tr("0 Results"));
  prepareFilter();

  if(ui.typefilter->findData(QVariant(previousTypeFilter)) > -1)
    ui.typefilter->setCurrentIndex
      (ui.typefilter->findData(QVariant(previousTypeFilter)));

  addConfigOptions(previousTypeFilter);
  slotDisplaySummary();
  emptyContainers();
  deletedAdmins.clear();
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(db.isOpen())
    db.close();

  qapp->restoreOverrideCursor();
  db = QSqlDatabase();

  if(QSqlDatabase::contains("Default"))
    QSqlDatabase::removeDatabase("Default");

  setWindowTitle(tr("BiblioteQ"));
}

/*
** -- initialUpdate() --
*/

void qtbook::initialUpdate(void)
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

/*
** -- resetAdminBrowser() --
*/

void qtbook::resetAdminBrowser(void)
{
  QStringList list;

  ab.table->clear();
  ab.table->setCurrentItem(0);
  ab.table->setColumnCount(0);
  ab.table->setRowCount(0);
  ab.table->scrollToTop();
  ab.table->horizontalScrollBar()->setValue(0);
  ab.table->setSortingEnabled(false);
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

/*
** -- resetMembersBrowser() --
*/

void qtbook::resetMembersBrowser(void)
{
  QStringList list;

  bb.table->clear();
  bb.table->setCurrentItem(0);
  bb.table->setColumnCount(0);
  bb.table->setRowCount(0);
  bb.table->scrollToTop();
  bb.table->horizontalScrollBar()->setValue(0);
  m_bbColumnHeaderIndexes.clear();
  list.append(tr("Member ID"));
  list.append(tr("First Name"));
  list.append(tr("Last Name"));
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
  bb.table->horizontalHeader()->resizeSections
    (QHeaderView::ResizeToContents);
}

/*
** -- getBBColumnIndexes() --
*/

QVector<QString> qtbook::getBBColumnIndexes(void) const
{
  return m_bbColumnHeaderIndexes;
}

/*
** -- slotShowMembersBrowser() --
*/

void qtbook::slotShowMembersBrowser(void)
{
  if(!db.isOpen())
    return;

  bb.filter->clear();
  bb.filterBox->setCheckState(Qt::Checked);
  bb.filtertype->setCurrentIndex(0);
  bb.filter->setFocus();
  bb.table->horizontalHeader()->resizeSections
    (QHeaderView::ResizeToContents);

  static bool resized = false;

  if(!resized)
    members_diag->resize(0.75 * size().width(),
			 0.75 * size().height());

  resized = true;
  misc_functions::center(members_diag, this);
  members_diag->raise();
  members_diag->show();

  if(ui.actionPopulate_Members_Browser_Table_on_Display->isChecked())
    slotPopulateMembersBrowser();
}

/*
** -- slotPopulateMembersBrowser() --
*/

void qtbook::slotPopulateMembersBrowser(void)
{
  int i = -1;
  int j = 0;
  QString str = "";
  QSqlQuery query(db);
  QProgressDialog progress(members_diag);
  QTableWidgetItem *item = 0;

#ifdef Q_OS_MAC
  progress.setAttribute(Qt::WA_MacMetalStyle, true);
#endif
  str = "SELECT member.memberid, "
    "member.first_name, "
    "member.last_name, "
    "member.membersince, "
    "member.expiration_date, "
    "COUNT(DISTINCT ib1.myoid) AS numbooks, "
    "COUNT(DISTINCT ib2.myoid) AS numcds, "
    "COUNT(DISTINCT ib3.myoid) AS numdvds, "
    "COUNT(DISTINCT ib4.myoid) AS numjournals, "
    "COUNT(DISTINCT ib5.myoid) AS nummagazines, "
    "COUNT(DISTINCT ib6.myoid) AS numvideogames "
    "FROM member member "
    "LEFT JOIN item_borrower_vw ib1 ON "
    "member.memberid = ib1.memberid AND ib1.type = 'Book' "
    "LEFT JOIN item_borrower_vw ib2 ON "
    "member.memberid = ib2.memberid AND ib2.type = 'CD' "
    "LEFT JOIN item_borrower_vw ib3 ON "
    "member.memberid = ib3.memberid AND ib3.type = 'DVD' "
    "LEFT JOIN item_borrower_vw ib4 ON "
    "member.memberid = ib4.memberid AND ib4.type = 'Journal' "
    "LEFT JOIN item_borrower_vw ib5 ON "
    "member.memberid = ib5.memberid AND ib5.type = 'Magazine' "
    "LEFT JOIN item_borrower_vw ib6 ON "
    "member.memberid = ib6.memberid AND ib6.type = 'Video Game' ";

  if(bb.filterBox->isChecked())
    {
      str.append("WHERE ");

      QString E("");

      if(db.driverName() != "QSQLITE")
	E = "E";

      if(bb.filtertype->currentText() == "Member ID")
	str.append("member.memberid LIKE " + E + "'%");
      else
	str.append("member.last_name LIKE " + E + "'%");

      str.append(myqstring::escape(bb.filter->text().trimmed()));
      str.append("%' ");
    }

  str.append("GROUP BY "
	     "member.memberid, "
	     "member.first_name, "
	     "member.last_name, "
	     "member.membersince, "
	     "member.expiration_date ");
  str.append("ORDER BY member.memberid");
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec(str))
    {
      qapp->restoreOverrideCursor();
      addError(QString(tr("Database Error")),
	       QString(tr("Unable to retrieve member data for table "
			  "populating.")),
	       query.lastError().text(),
	       __FILE__, __LINE__);
      QMessageBox::critical(members_diag, tr("BiblioteQ: Database Error"),
			    tr("Unable to retrieve member data for "
			       "table populating."));
      return;
    }

  qapp->restoreOverrideCursor();
  resetMembersBrowser();
  bb.table->setSortingEnabled(false);

  if(db.driverName() != "QSQLITE")
    bb.table->setRowCount(query.size());
  else
    bb.table->setRowCount
      (misc_functions::sqliteQuerySize(str, db, __FILE__, __LINE__));

  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.setLabelText(tr("Populating the table..."));
  progress.setMinimum(0);

  if(db.driverName() == "QSQLITE")
    progress.setMaximum
      (misc_functions::sqliteQuerySize(str, db, __FILE__, __LINE__));
  else
    progress.setMaximum(query.size());

  progress.show();
  progress.update();
  i = -1;

  while(i++, !progress.wasCanceled() && query.next())
    {
      if(query.isValid())
	for(j = 0; j < query.record().count(); j++)
	  {
	    if(query.record().fieldName(j).contains("date") ||
	       query.record().fieldName(j).contains("membersince"))
	      {
		QDate date(QDate::fromString(query.value(j).toString(),
					     "MM/dd/yyyy"));

		str = date.toString(Qt::ISODate);
	      }
	    else
	      str = query.value(j).toString();

	    if(str == "0")
	      str = "";

	    if((item = new(std::nothrow) QTableWidgetItem()) != 0)
	      {
		item->setText(str);
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		bb.table->setItem(i, j, item);
	      }
	    else
	      addError(QString(tr("Memory Error")),
		       QString(tr("Unable to allocate memory for the "
				  "\"item\" object. "
				  "This is a serious problem!")),
		       QString(""), __FILE__, __LINE__);
	  }

      if(i + 1 <= progress.maximum())
	progress.setValue(i + 1);

      progress.update();
#ifndef Q_OS_MAC
      qapp->processEvents();
#endif
    }

  progress.hide();
  bb.table->setSortingEnabled(true);
  bb.table->setRowCount(i);
  bb.table->horizontalHeader()->resizeSections
    (QHeaderView::ResizeToContents);
#ifdef Q_OS_MAC
  bb.table->hide();
  bb.table->show();
#endif
}

/*
** -- slotGrantPrivileges() --
*/

void qtbook::slotGrantPrivileges(void)
{
  bool error = false;
  QString errorstr("");
  QProgressDialog progress(members_diag);
  QTableWidgetItem *item = 0;

#ifdef Q_OS_MAC
  progress.setAttribute(Qt::WA_MacMetalStyle, true);
#endif
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.setLabelText(tr("Granting privileges..."));
  progress.setMaximum(bb.table->rowCount());
  progress.setMinimum(0);
  progress.show();
  progress.update();

  for(int i = 0; i < bb.table->rowCount(); i++)
    {
      if((item = bb.table->item(i, 0)))
	{
	  misc_functions::DBAccount(item->text(),
				    db, misc_functions::UPDATE_USER,
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

      progress.update();
#ifndef Q_OS_MAC
      qapp->processEvents();
#endif
    }

  progress.hide();

  if(error)
    QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			  tr("Unable to grant privileges to all of "
			     "the members."));
}

/*
** -- updateMembersBrowser() --
*/

void qtbook::updateMembersBrowser(void)
{
  int row = 0;
  QString errorstr = "";
  QString memberid = "";
  QMap<QString, QString> counts;

  /*
  ** Called from the Copy Editor when an item has been reserved.
  */

  if((row = bb.table->currentRow()) < 0)
    return;

  memberid = misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("Member ID"));
  qapp->setOverrideCursor(Qt::WaitCursor);
  counts = misc_functions::getItemsReservedCounts(db, memberid, errorstr);
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    addError(QString(tr("Database Error")),
	     QString(tr("Unable to determine the number of reserved items "
			"for the selected member.")),
	     errorstr, __FILE__, __LINE__);
  else
    {
      misc_functions::updateColumn
	(bb.table, row, m_bbColumnHeaderIndexes.indexOf("Books Reserved"),
	 counts["numbooks"]);
      misc_functions::updateColumn
	(bb.table, row, m_bbColumnHeaderIndexes.indexOf("CDs Reserved"),
	 counts["numcds"]);
      misc_functions::updateColumn
	(bb.table, row, m_bbColumnHeaderIndexes.indexOf("DVDs Reserved"),
	 counts["numdvds"]);
      misc_functions::updateColumn
	(bb.table, row, m_bbColumnHeaderIndexes.indexOf("Journals Reserved"),
	 counts["numjournals"]);
      misc_functions::updateColumn
	(bb.table, row, m_bbColumnHeaderIndexes.indexOf("Magazines Reserved"),
	 counts["nummagazines"]);
      misc_functions::updateColumn
	(bb.table, row,
	 m_bbColumnHeaderIndexes.indexOf("Video Games Reserved"),
	 counts["numvideogames"]);
      counts.clear();

      if(history_diag->isVisible())
	slotShowHistory();
    }
}

/*
** -- updateMembersBrowser() --
*/

void qtbook::updateMembersBrowser(const QString &memberid)
{
  int i = 0;
  QString str = "";
  QString errorstr = "";
  QMap<QString, QString> counts;

  /*
  ** Called from the Borrowers Editor when an item has been updated.
  */

  qapp->setOverrideCursor(Qt::WaitCursor);
  counts = misc_functions::getItemsReservedCounts(db, memberid, errorstr);
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    addError(QString(tr("Database Error")),
	     QString(tr("Unable to retrieve the number of reserved items "
			"of the selected member.")),
	     errorstr, __FILE__, __LINE__);
  else
    {
      qapp->setOverrideCursor(Qt::WaitCursor);

      for(i = 0; i < bb.table->rowCount(); i++)
	{
	  str = misc_functions::getColumnString
	    (bb.table, i, m_bbColumnHeaderIndexes.indexOf("Member ID"));

	  if(str == memberid)
	    {
	      misc_functions::updateColumn
		(bb.table, i,
		 m_bbColumnHeaderIndexes.indexOf("Books Reserved"),
		 counts["numbooks"]);
	      misc_functions::updateColumn
		(bb.table, i, m_bbColumnHeaderIndexes.indexOf("CDs Reserved"),
		 counts["numcds"]);
	      misc_functions::updateColumn
		(bb.table, i, m_bbColumnHeaderIndexes.indexOf("DVDs Reserved"),
		 counts["numdvds"]);
	      misc_functions::updateColumn
		(bb.table, i,
		 m_bbColumnHeaderIndexes.indexOf("Journals Reserved"),
		 counts["numjournals"]);
	      misc_functions::updateColumn
		(bb.table, i,
		 m_bbColumnHeaderIndexes.indexOf("Magazines Reserved"),
		 counts["nummagazines"]);
	      misc_functions::updateColumn
		(bb.table, i,
		 m_bbColumnHeaderIndexes.indexOf("Video Games Reserved"),
		 counts["numvideogames"]);
	      break;
	    }
	}

      counts.clear();
      qapp->restoreOverrideCursor();
    }
}

/*
** -- slotModifyBorrower() --
*/

void qtbook::slotModifyBorrower(void)
{
  int i = 0;
  int row = bb.table->currentRow();
  QString str = "";
  QString fieldname = "";
  QString searchstr = "";
  QVariant var;
  QSqlQuery query(db);

  if(row < 0)
    {
      QMessageBox::critical(members_diag, tr("BiblioteQ: User Error"),
			    tr("Please select a member to modify."));
      return;
    }

  str = misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("Member ID"));
  searchstr = "SELECT * FROM member WHERE memberid = '";
  searchstr.append(str);
  searchstr.append("'");
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec(searchstr) || !query.next())
    {
      qapp->restoreOverrideCursor();
      addError(QString(tr("Database Error")),
	       QString(tr("Unable to retrieve the selected member's "
			  "information.")),
	       query.lastError().text(),
	       __FILE__, __LINE__);

      if(userinfo_diag->isVisible())
	QMessageBox::critical(userinfo_diag, tr("BiblioteQ: Database Error"),
			      tr("Unable to retrieve the selected member's "
				 "information."));
      else
	QMessageBox::critical(members_diag, tr("BiblioteQ: Database Error"),
			      tr("Unable to retrieve the selected member's "
				 "information."));

      return;
    }
  else
    {
      qapp->restoreOverrideCursor();

      for(i = 0; i < query.record().count(); i++)
	{
	  str = query.value(i).toString();
	  var = query.record().field(i).value();
	  fieldname = query.record().fieldName(i);

	  if(fieldname == "memberid")
	    userinfo_diag->userinfo.memberid->setText(var.toString());
	  else if(fieldname == "membersince")
	    userinfo_diag->userinfo.membersince->setDate
	      (QDate::fromString(var.toString(), "MM/dd/yyyy"));
	  else if(fieldname == "dob")
	    userinfo_diag->userinfo.dob->setDate
	      (QDate::fromString(var.toString(), "MM/dd/yyyy"));
	  else if(fieldname == "sex")
	    {
	      if(userinfo_diag->userinfo.sex->findText(var.toString()) > -1)
		userinfo_diag->userinfo.sex->setCurrentIndex
		  (userinfo_diag->userinfo.sex->findText(var.toString()));
	      else
		userinfo_diag->userinfo.sex->setCurrentIndex(0);
	    }
	  else if(fieldname == "first_name")
	    userinfo_diag->userinfo.firstName->setText(var.toString());
	  else if(fieldname == "middle_init")
	    userinfo_diag->userinfo.middle->setText(var.toString());
	  else if(fieldname == "last_name")
	    userinfo_diag->userinfo.lastName->setText(var.toString());
	  else if(fieldname == "telephone_num")
	    userinfo_diag->userinfo.telephoneNumber->setText(var.toString());
	  else if(fieldname == "street")
	    userinfo_diag->userinfo.street->setText(var.toString());
	  else if(fieldname == "city")
	    userinfo_diag->userinfo.city->setText(var.toString());
	  else if(fieldname == "state_abbr")
	    {
	      if(userinfo_diag->userinfo.state->
		 findText(var.toString()) == -1)
		userinfo_diag->userinfo.state->setCurrentIndex(0);
	      else
		userinfo_diag->userinfo.state->setCurrentIndex
		  (userinfo_diag->userinfo.state->findText(var.toString()));
	    }
	  else if(fieldname == "zip")
	    userinfo_diag->userinfo.zip->setText(var.toString());
	  else if(fieldname == "email")
	    userinfo_diag->userinfo.email->setText(var.toString());
	  else if(fieldname == "expiration_date")
	    userinfo_diag->userinfo.expirationdate->setDate
	      (QDate::fromString(var.toString(), "MM/dd/yyyy"));
	  else if(fieldname == "overdue_fees")
	    userinfo_diag->userinfo.overduefees->setValue(var.toDouble());
	  else if(fieldname == "comments")
	    userinfo_diag->userinfo.comments->setPlainText(var.toString());
	  else if(fieldname == "general_registration_number")
	    userinfo_diag->userinfo.generalregistrationnumber->setText
	      (var.toString());
	  else if(fieldname == "memberclass")
	    userinfo_diag->userinfo.memberclass->setText(var.toString());

	  if(fieldname.contains("dob") ||
	     fieldname.contains("date") ||
	     fieldname.contains("membersince"))
	    userinfo_diag->memberProperties[fieldname] =
	      QDate::fromString(var.toString(), "MM/dd/yyyy").
	      toString(Qt::ISODate);
	  else
	    userinfo_diag->memberProperties[fieldname] = var.toString();
	}

      foreach(QLineEdit *textfield,
	      userinfo_diag->findChildren<QLineEdit *>())
	textfield->setCursorPosition(0);
    }

  userinfo_diag->userinfo.memberid->setReadOnly(true);
  userinfo_diag->userinfo.prevTool->setVisible(true);
  userinfo_diag->userinfo.nextTool->setVisible(true);
  userinfo_diag->setWindowTitle(tr("BiblioteQ: Modify Member"));
  engUserinfoTitle = "Modify Member";
  userinfo_diag->userinfo.membersince->setMaximumDate(QDate::currentDate());
  userinfo_diag->userinfo.tabWidget->setCurrentIndex(0);
  userinfo_diag->userinfo.membersince->setFocus();
  userinfo_diag->userinfo.memberid->setPalette
    (userinfo_diag->userinfo.telephoneNumber->palette());
  userinfo_diag->updateGeometry();
  userinfo_diag->show();
}

/*
** -- slotCancelAddUser() --
*/

void qtbook::slotCancelAddUser(void)
{
  if(userinfo_diag->isVisible())
    userinfo_diag->close();
}

/*
** -- slotCheckout() --
*/

void qtbook::slotCheckout(void)
{
  int row1 = bb.table->currentRow();
  int row2 = ui.table->currentRow();
  int quantity = 0;
  int availability = 0;
  QString oid = "";
  QString type = "";
  QString itemid = "";
  QString errorstr = "";
  copy_editor *copyeditor = 0;
  qtbook_item *item = 0;

  if(row1 > -1)
    {
      /*
      ** Has the member's membership expired?
      */

      bool expired = true;
      QString memberid =
	misc_functions::getColumnString
	(bb.table, row1, m_bbColumnHeaderIndexes.indexOf("Member ID"));

      qapp->setOverrideCursor(Qt::WaitCursor);
      expired = misc_functions::hasMemberExpired(db, memberid, errorstr);
      qapp->restoreOverrideCursor();

      if(!errorstr.isEmpty())
	addError(QString(tr("Database Error")),
		 QString(tr("Unable to determine if the membership of "
			    "the selected member has expired.")),
		 errorstr, __FILE__, __LINE__);

      if(expired || !errorstr.isEmpty())
	{
	  QMessageBox::critical(members_diag, tr("BiblioteQ: User Error"),
				tr("It appears that the selected member's "
				   "membership has expired."));
	  return;
	}
    }

  if(row2 > -1)
    {
      /*
      ** Is the item available?
      */

      oid = misc_functions::getColumnString
	(ui.table, row2, ui.table->columnNumber("MYOID"));
      type = misc_functions::getColumnString
	(ui.table, row2, ui.table->columnNumber("Type"));
      qapp->setOverrideCursor(Qt::WaitCursor);
      availability = misc_functions::getAvailability
	(oid, db, type, errorstr).toInt();
      qapp->restoreOverrideCursor();

      if(!errorstr.isEmpty())
	addError(QString(tr("Database Error")),
		 QString(tr("Unable to determine the availability of "
			    "the selected item.")),
		 errorstr, __FILE__, __LINE__);

      if(availability < 1 || !errorstr.isEmpty())
	{
	  QMessageBox::critical(members_diag, tr("BiblioteQ: User Error"),
				tr("It appears that the item that you "
				   "selected "
				   "is not available for reservation."));
	  return;
	}
    }

  if(row1 < 0 || row2 < 0)
    {
      QMessageBox::critical(members_diag, tr("BiblioteQ: User Error"),
			    tr("Please select a member and an item "
			       "to continue with the reservation process."));
      return;
    }
  else
    {
      if((item = new(std::nothrow) qtbook_item(row2)) != 0)
	{
	  quantity = misc_functions::getColumnString
	    (ui.table, row2,
	     ui.table->columnNumber("Quantity")).toInt();

	  if(type.toLower() == "book")
	    {
	      itemid = misc_functions::getColumnString
		(ui.table, row2,
		 ui.table->columnNumber("ISBN-10"));

	      if(itemid.isEmpty())
		itemid = misc_functions::getColumnString
		  (ui.table, row2,
		   ui.table->columnNumber("ISBN-13"));
	    }
	  else if(type.toLower() == "dvd")
	    itemid = misc_functions::getColumnString
	      (ui.table, row2,
	       ui.table->columnNumber("UPC"));
	  else if(type.toLower() == "journal" || type.toLower() ==
		  "magazine")
	    itemid = misc_functions::getColumnString
	      (ui.table, row2,
	       ui.table->columnNumber("ISSN"));
	  else if(type.toLower() == "cd")
	    itemid = misc_functions::getColumnString
	      (ui.table, row2,
	       ui.table->columnNumber("Catalog Number"));
	  else if(type.toLower() == "video game")
	    itemid = misc_functions::getColumnString
	      (ui.table, row2,
	       ui.table->columnNumber("UPC"));

	  if(itemid.isEmpty())
	    itemid = misc_functions::getColumnString
	      (ui.table, row2,
	       ui.table->columnNumber("ID Number"));

	  /*
	  ** Custom search?
	  */

	  if(itemid.isEmpty())
	    itemid = misc_functions::getColumnString
	      (ui.table, row2, "id");

	  if((copyeditor = new(std::nothrow)
	      copy_editor(members_diag, item,
			  true,
			  quantity, oid,
			  static_cast<QSpinBox *> (0),
			  font(), type, itemid)) != 0)
	    copyeditor->populateCopiesEditor();
	}
    }
}

/*
** -- prepareRequestToolButton() --
*/

void qtbook::prepareRequestToolButton(const QString &typefilter)
{
  if(db.driverName() != "QSQLITE")
    if(db.isOpen())
      {
	if((roles == "administrator" || roles == "circulation") &&
	   typefilter == "All Requested")
	  {
	    ui.actionRequests->setEnabled(true);
	    ui.actionRequests->setToolTip(tr("Cancel Selected Request(s)"));
	    ui.actionRequests->setIcon
	      (QIcon("icons.d/32x32/remove_request.png"));
	  }
	else if(roles.isEmpty() && (typefilter == "All" ||
				    typefilter == "All Available" ||
				    typefilter == "Books" ||
				    typefilter == "DVDs" ||
				    typefilter == "Journals" ||
				    typefilter == "Magazines" ||
				    typefilter == "Music CDs" ||
				    typefilter == "Video Games"))
	  {
	    ui.actionRequests->setToolTip(tr("Request Selected Item(s)"));
	    ui.actionRequests->setIcon(QIcon("icons.d/32x32/request.png"));
	    ui.actionRequests->setEnabled(true);
	  }
	else if(roles.isEmpty() && typefilter == "All Requested")
	  {
	    ui.actionRequests->setToolTip(tr("Cancel Selected Request(s)"));
	    ui.actionRequests->setIcon
	      (QIcon("icons.d/32x32/remove_request.png"));
	    ui.actionRequests->setEnabled(true);
	  }
	else
	  {
	    ui.actionRequests->setToolTip(tr("Item Requests"));
	    ui.actionRequests->setIcon(QIcon("icons.d/32x32/request.png"));
	    ui.actionRequests->setEnabled(false);
	  }
      }
}

/*
** -- slotAutoPopOnFilter() --
*/

void qtbook::slotAutoPopOnFilter(void)
{
  prepareRequestToolButton
    (ui.typefilter->itemData(ui.typefilter->currentIndex()).toString());

  /*
  ** Populate the main table only if we're connected to a database.
  */

  if(db.isOpen())
    slotRefresh();
  else
    {
      QString typefilter("");

      typefilter = ui.typefilter->itemData(ui.typefilter->currentIndex()).
	toString();
      ui.table->resetTable(db.userName(), typefilter, "");
    }
}

/*
** -- slotReset() --
*/

void qtbook::slotReset(void)
{
  if(all_diag->isVisible())
    {
      QAction *action = qobject_cast<QAction *> (sender());

      if(action != 0)
	{
	  QList<QAction *> actions = al.resetButton->menu()->actions();

	  if(action == actions[0])
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
	      al.publication_date->setDate
		(QDate::fromString("01/7999", "MM/yyyy"));
	      al.publication_date->setFocus();
	    }
	  else if(action == actions[3])
	    {
	      al.publisher->clear();
	      al.publisher->setFocus();
	    }
	  else if(action == actions[4])
	    {
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
	    }
	  else if(action == actions[12])
	    {
	      al.available->setChecked(false);
	      al.available->setFocus();
	    }

	  actions.clear();
	}
    }
}

/*
** -- slotShowErrorDialog() --
*/

void qtbook::slotShowErrorDialog(void)
{
  er.table->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  er.table->resizeColumnsToContents();

  static bool resized = false;

  if(!resized)
    error_diag->resize(0.75 * size().width(),
		       0.75 * size().height());

  resized = true;
  misc_functions::center(error_diag, this);
  error_diag->raise();
  error_diag->show();
}

/*
** -- addError() --
*/

void qtbook::addError(const QString &type, const QString &summary,
		      const QString &error, const char *file,
		      const int line)
{
  int i = 0;
  QString str = "";
  QDateTime now = QDateTime::currentDateTime();
  QTableWidgetItem *item = 0;

  if(error_bar_label != 0)
    {
      error_bar_label->setPixmap(QPixmap("icons.d/16x16/log.png"));
      error_bar_label->setToolTip(tr("Error Log Active"));
    }

  er.table->setSortingEnabled(false);
  er.table->setRowCount(er.table->rowCount() + 1);

  for(i = 0; i < 6; i++)
    if((item = new(std::nothrow) QTableWidgetItem()) != 0)
      {
	if(i == 0)
	  item->setText(now.toString("yyyy/MM/dd hh:mm:ss"));
	else if(i == 1)
	  item->setText(type);
	else if(i == 2)
	  item->setText(summary);
	else if(i == 3)
	  {
	    if(error.simplified().isEmpty())
	      item->setText(summary);
	    else
	      item->setText(error.simplified());
	  }
	else if(i == 4)
	  item->setText(file);
	else
	  {
	    str.setNum(line);
	    item->setText(str);
	  }

	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	er.table->setItem(er.table->rowCount() - 1, i, item);
	er.table->resizeColumnToContents(i);
      }
    else
      qDebug() << tr("Memory failure in addError()!");

  er.table->resizeColumnsToContents();
  er.table->setSortingEnabled(true);
}

/*
** -- resetErrorLog() --
*/

void qtbook::slotResetErrorLog(void)
{
  QStringList list;

  list.append(tr("Event Time"));
  list.append(tr("Event Type"));
  list.append(tr("Summary"));
  list.append(tr("Full Description"));
  list.append(tr("File"));
  list.append(tr("Line Number"));
  er.table->clear();
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
  er.table->resizeColumnsToContents();

  if(error_bar_label != 0)
    {
      error_bar_label->setPixmap(QPixmap("icons.d/16x16/ok.png"));
      error_bar_label->setToolTip(tr("Empty Error Log"));
    }
}

/*
** -- getBB() --
*/

Ui_membersBrowser qtbook::getBB(void) const
{
  return bb;
}

/*
** -- getUI() --
*/

Ui_mainWindow qtbook::getUI(void) const
{
  return ui;
}

/*
** -- getDB() --
*/

QSqlDatabase qtbook::getDB(void) const
{
  return db;
}

/*
** -- removeCD() --
*/

void qtbook::removeCD(qtbook_cd *cd)
{
  if(cd)
    cd->deleteLater();
}

/*
** -- replaceCD() --
*/

void qtbook::replaceCD(const QString &id, qtbook_cd *cd)
{
  Q_UNUSED(id);
  Q_UNUSED(cd);
}

/*
** -- removeDVD() --
*/

void qtbook::removeDVD(qtbook_dvd *dvd)
{
  if(dvd)
    dvd->deleteLater();
}

/*
** -- replaceDVD() --
*/

void qtbook::replaceDVD(const QString &id, qtbook_dvd *dvd)
{
  Q_UNUSED(id);
  Q_UNUSED(dvd);
}

/*
** -- removeBook() --
*/

void qtbook::removeBook(qtbook_book *book)
{
  if(book)
    book->deleteLater();
}

/*
** -- replaceBook() --
*/

void qtbook::replaceBook(const QString &id, qtbook_book *book)
{
  Q_UNUSED(id);
  Q_UNUSED(book);
}

/*
** -- removeJournal() --
*/

void qtbook::removeJournal(qtbook_journal *journal)
{
  if(journal)
    journal->deleteLater();
}

/*
** -- removeMagazine() --
*/

void qtbook::removeMagazine(qtbook_magazine *magazine)
{
  if(magazine)
    magazine->deleteLater();
}

/*
** -- replaceJournal() --
*/

void qtbook::replaceJournal(const QString &id, qtbook_journal *journal)
{
  Q_UNUSED(id);
  Q_UNUSED(journal);
}

/*
** -- replaceMagazine() --
*/

void qtbook::replaceMagazine(const QString &id, qtbook_magazine *magazine)
{
  Q_UNUSED(id);
  Q_UNUSED(magazine);
}

/*
** -- removePhotographCollection() --
*/

void qtbook::removePhotographCollection(qtbook_photographcollection *pc)
{
  if(pc)
    pc->deleteLater();
}

/*
** -- removeVideoGame() --
*/

void qtbook::removeVideoGame(qtbook_videogame *videogame)
{
  if(videogame)
    videogame->deleteLater();
}

/*
** -- replaceVideoGame() --
*/

void qtbook::replaceVideoGame(const QString &id,
			      qtbook_videogame *videogame)
{
  Q_UNUSED(id);
  Q_UNUSED(videogame);
}

/*
** -- replacePhotographCollection() --
*/

void qtbook::replacePhotographCollection
(const QString &id,
 qtbook_photographcollection *photograph)
{
  Q_UNUSED(id);
  Q_UNUSED(photograph);
}

/*
** -- updateItemWindows() --
*/

void qtbook::updateItemWindows(void)
{
  qapp->setOverrideCursor(Qt::WaitCursor);

  foreach(QWidget *w, qapp->topLevelWidgets())
    {
      qtbook_cd *cd = qobject_cast<qtbook_cd *> (w);
      qtbook_dvd *dvd = qobject_cast<qtbook_dvd *> (w);
      qtbook_book *book = qobject_cast<qtbook_book *> (w);
      qtbook_journal *journal = qobject_cast<qtbook_journal *> (w);
      qtbook_magazine *magazine = qobject_cast<qtbook_magazine *> (w);
      qtbook_videogame *videogame = qobject_cast<qtbook_videogame *> (w);

      if(cd)
	cd->updateWindow(EDITABLE);

      if(dvd)
	dvd->updateWindow(EDITABLE);

      if(book)
	book->updateWindow(EDITABLE);

      if(journal)
	journal->updateWindow(EDITABLE);

      if(!qobject_cast<qtbook_journal *> (w))
	if(magazine)
	  magazine->updateWindow(EDITABLE);

      if(videogame)
	videogame->updateWindow(EDITABLE);
    }

  qapp->restoreOverrideCursor();
}

/*
** -- emptyContainers() --
*/

void qtbook::emptyContainers(void)
{
  qapp->setOverrideCursor(Qt::WaitCursor);

  foreach(QWidget *w, qapp->topLevelWidgets())
    {
      qtbook_cd *cd = qobject_cast<qtbook_cd *> (w);
      qtbook_dvd *dvd = qobject_cast<qtbook_dvd *> (w);
      qtbook_book *book = qobject_cast<qtbook_book *> (w);
      qtbook_journal *journal = qobject_cast<qtbook_journal *> (w);
      qtbook_magazine *magazine = qobject_cast<qtbook_magazine *> (w);
      qtbook_videogame *videogame = qobject_cast<qtbook_videogame *> (w);
      qtbook_photographcollection *photograph =
	qobject_cast<qtbook_photographcollection *> (w);

      if(cd)
	cd->deleteLater();

      if(dvd)
	dvd->deleteLater();

      if(book)
	book->deleteLater();

      if(journal)
	journal->deleteLater();

      if(!qobject_cast<qtbook_journal *> (w))
	if(magazine)
	  magazine->deleteLater();

      if(videogame)
	videogame->deleteLater();

      if(photograph)
	photograph->deleteLater();
    }

  qapp->restoreOverrideCursor();
}

/*
** -- getAdminID() --
*/

QString qtbook::getAdminID(void) const
{
  if(db.driverName() != "QSQLITE")
    return db.userName();
  else
    return "N/A";
}

/*
** -- slotInsertCD() --
*/

void qtbook::slotInsertCD(void)
{
  QString id("");
  qtbook_cd *cd = 0;

  m_idCt += 1;
  id = QString("insert_%1").arg(m_idCt);
  cd = new(std::nothrow) qtbook_cd(this, id, -1);

  if(cd)
    cd->insert();
}

/*
** -- slotInsertDVD() --
*/

void qtbook::slotInsertDVD(void)
{
  QString id("");
  qtbook_dvd *dvd = 0;

  m_idCt += 1;
  id = QString("insert_%1").arg(m_idCt);
  dvd = new(std::nothrow) qtbook_dvd(this, id, -1);

  if(dvd)
    dvd->insert();
}

/*
** -- slotInsertBook() --
*/

void qtbook::slotInsertBook(void)
{
  QString id("");
  qtbook_book *book = 0;

  m_idCt += 1;
  id = QString("insert_%1").arg(m_idCt);
  book = new(std::nothrow) qtbook_book(this, id, -1);

  if(book)
    book->insert();
}

/*
** -- slotInsertJourn() --
*/

void qtbook::slotInsertJourn(void)
{
  QString id("");
  qtbook_journal *journal = 0;

  m_idCt += 1;
  id = QString("insert_%1").arg(m_idCt);
  journal = new(std::nothrow) qtbook_journal(this, id, -1);

  if(journal)
    journal->insert();
}

/*
** -- slotInsertMag() --
*/

void qtbook::slotInsertMag(void)
{
  QString id("");
  qtbook_magazine *magazine = 0;

  m_idCt += 1;
  id = QString("insert_%1").arg(m_idCt);
  magazine = new(std::nothrow) qtbook_magazine(this, id, -1,
					       "magazine");

  if(magazine)
    magazine->insert();
}

/*
** -- slotInsertPhotograph() --
*/

void qtbook::slotInsertPhotograph(void)
{
  QString id("");
  qtbook_photographcollection *photograph = 0;

  m_idCt += 1;
  id = QString("insert_%1").arg(m_idCt);
  photograph = new(std::nothrow) qtbook_photographcollection(this, id, -1);

  if(photograph)
    photograph->insert();
}

/*
** -- slotInsertVideoGame() --
*/

void qtbook::slotInsertVideoGame(void)
{
  QString id("");
  qtbook_videogame *videogame = 0;

  m_idCt += 1;
  id = QString("insert_%1").arg(m_idCt);
  videogame = new(std::nothrow) qtbook_videogame(this, id, -1);

  if(videogame)
    videogame->insert();
}

/*
** -- deleteItem() --
*/

void qtbook::deleteItem(const QString &oid, const QString &itemType)
{
  if(itemType == "cd")
    {
      foreach(QWidget *w, qapp->topLevelWidgets())
	{
	  qtbook_cd *cd = qobject_cast<qtbook_cd *> (w);

	  if(cd && cd->getID() == oid)
	    {
	      removeCD(cd);
	      break;
	    }
	}
    }
  else if(itemType == "dvd")
    {
      foreach(QWidget *w, qapp->topLevelWidgets())
	{
	  qtbook_dvd *dvd = qobject_cast<qtbook_dvd *> (w);

	  if(dvd && dvd->getID() == oid)
	    {
	      removeDVD(dvd);
	      break;
	    }
	}
    }
  else if(itemType == "book")
    {
      foreach(QWidget *w, qapp->topLevelWidgets())
	{
	  qtbook_book *book = qobject_cast<qtbook_book *> (w);

	  if(book && book->getID() == oid)
	    {
	      removeBook(book);
	      break;
	    }
	}
    }
  else if(itemType == "journal")
    {
      foreach(QWidget *w, qapp->topLevelWidgets())
	{
	  qtbook_journal *journal = qobject_cast<qtbook_journal *> (w);

	  if(journal && journal->getID() == oid)
	    {
	      removeJournal(journal);
	      break;
	    }
	}
    }
  else if(itemType == "magazine")
    {
      foreach(QWidget *w, qapp->topLevelWidgets())
	{
	  qtbook_magazine *magazine = qobject_cast<qtbook_magazine *> (w);

	  /*
	  ** The class qtbook_journal inherits qtbook_magazine.
	  */

	  if(!qobject_cast<qtbook_journal *> (w))
	    if(magazine && magazine->getID() == oid)
	      {
		removeMagazine(magazine);
		break;
	      }
	}
    }
  else if(itemType == "photographcollection")
    {
      foreach(QWidget *w, qapp->topLevelWidgets())
	{
	  qtbook_photographcollection *photograph =
	    qobject_cast<qtbook_photographcollection *> (w);

	  if(photograph && photograph->getID() == oid)
	    {
	      removePhotographCollection(photograph);
	      break;
	    }
	}
    }
  else if(itemType == "videogame")
    {
      foreach(QWidget *w, qapp->topLevelWidgets())
	{
	  qtbook_videogame *videogame = qobject_cast<qtbook_videogame *> (w);

	  if(videogame && videogame->getID() == oid)
	    {
	      removeVideoGame(videogame);
	      break;
	    }
	}
    }
}

/*
** -- bookSearch() --
*/

void qtbook::bookSearch(const QString &field, const QString &value)
{
  qtbook_book *book = new(std::nothrow) qtbook_book(this, "", -1);

  if(book)
    {
      book->search(field, value);
      book->deleteLater();
    }
}

/*
** -- slotBookSearch() --
*/

void qtbook::slotBookSearch(void)
{
  qtbook_book *book = 0;

  foreach(QWidget *w, qapp->topLevelWidgets())
    {
      qtbook_book *b = qobject_cast<qtbook_book *> (w);

      if(b && b->getID() == "search")
	{
	  book = b;
	  break;
	}
    }

  if(!book)
    {
      book = new(std::nothrow) qtbook_book(this, "search", -1);
      book->raise();
      book->search();
    }
  else
    book->raise();
}

/*
** -- cdSearch() --
*/

void qtbook::cdSearch(const QString &field, const QString &value)
{
  qtbook_cd *cd = new(std::nothrow) qtbook_cd(this, "", -1);

  if(cd)
    {
      cd->search(field, value);
      cd->deleteLater();
    }
}

/*
** -- slotCDSearch() --
*/

void qtbook::slotCDSearch(void)
{
  qtbook_cd *cd = 0;

  foreach(QWidget *w, qapp->topLevelWidgets())
    {
      qtbook_cd *c = qobject_cast<qtbook_cd *> (w);

      if(c && c->getID() == "search")
	{
	  cd = c;
	  break;
	}
    }

  if(!cd)
    {
      cd = new(std::nothrow) qtbook_cd(this, "search", -1);
      cd->raise();
      cd->search();
    }
  else
    cd->raise();
}

/*
** -- dvdSearch() --
*/

void qtbook::dvdSearch(const QString &field, const QString &value)
{
  qtbook_dvd *dvd = new(std::nothrow) qtbook_dvd(this, "", -1);

  if(dvd)
    {
      dvd->search(field, value);
      dvd->deleteLater();
    }
}

/*
** -- slotDVDSearch() --
*/

void qtbook::slotDVDSearch(void)
{
  qtbook_dvd *dvd = 0;

  foreach(QWidget *w, qapp->topLevelWidgets())
    {
      qtbook_dvd *d = qobject_cast<qtbook_dvd *> (w);

      if(d && d->getID() == "search")
	{
	  dvd = d;
	  break;
	}
    }

  if(!dvd)
    {
      dvd = new(std::nothrow) qtbook_dvd(this, "search", -1);
      dvd->raise();
      dvd->search();
    }
  else
    dvd->raise();
}

/*
** -- journSearch() --
*/

void qtbook::journSearch(const QString &field, const QString &value)
{
  qtbook_journal *journal = new(std::nothrow) qtbook_journal(this, "", -1);

  if(journal)
    {
      journal->search(field, value);
      journal->deleteLater();
    }
}

/*
** -- slotJournSearch() --
*/

void qtbook::slotJournSearch(void)
{
  qtbook_journal *journal = 0;

  foreach(QWidget *w, qapp->topLevelWidgets())
    {
      qtbook_journal *j = qobject_cast<qtbook_journal *> (w);

      if(j && j->getID() == "search")
	{
	  journal = j;
	  break;
	}
    }

  if(!journal)
    {
      journal = new(std::nothrow) qtbook_journal(this, "search", -1);
      journal->raise();
      journal->search();
    }
  else
    journal->raise();
}

/*
** -- magSearch() --
*/

void qtbook::magSearch(const QString &field, const QString &value)
{
  qtbook_magazine *magazine = new(std::nothrow) qtbook_magazine
    (this, "", -1, "magazine");

  if(magazine)
    {
      magazine->search(field, value);
      magazine->deleteLater();
    }
}

/*
** -- slotMagSearch() --
*/

void qtbook::slotMagSearch(void)
{
  qtbook_magazine *magazine = 0;

  foreach(QWidget *w, qapp->topLevelWidgets())
    {
      qtbook_magazine *m = qobject_cast<qtbook_magazine *> (w);

      /*
      ** The class qtbook_journal inherits qtbook_magazine.
      */

      if(!qobject_cast<qtbook_journal *> (w))
	if(m && m->getID() == "search")
	  {
	    magazine = m;
	    break;
	  }
    }

  if(!magazine)
    {
      magazine = new(std::nothrow) qtbook_magazine
	(this, "search", -1, "magazine");
      magazine->raise();
      magazine->search();
    }
  else
    magazine->raise();
}

/*
** -- pcSearch() --
*/

void qtbook::pcSearch(const QString &field, const QString &value)
{
  qtbook_photographcollection *photograph =
    new(std::nothrow) qtbook_photographcollection(this, "", -1);

  if(photograph)
    {
      photograph->search(field, value);
      photograph->deleteLater();
    }
}

/*
** -- slotPhotographSearch() --
*/

void qtbook::slotPhotographSearch(void)
{
  qtbook_photographcollection *photograph = 0;

  foreach(QWidget *w, qapp->topLevelWidgets())
    {
      qtbook_photographcollection *p =
	qobject_cast<qtbook_photographcollection *> (w);

      if(p && p->getID() == "search")
	{
	  photograph = p;
	  break;
	}
    }

  if(!photograph)
    {
      photograph = new(std::nothrow) qtbook_photographcollection
	(this, "search", -1);
      photograph->raise();
      photograph->search();
    }
  else
    photograph->raise();
}

/*
** -- vgSearch() --
*/

void qtbook::vgSearch(const QString &field, const QString &value)
{
  qtbook_videogame *videogame = new(std::nothrow) qtbook_videogame
    (this, "", -1);

  if(videogame)
    {
      videogame->search(field, value);
      videogame->deleteLater();
    }
}

/*
** -- slotVideoGameSearch() --
*/

void qtbook::slotVideoGameSearch(void)
{
  qtbook_videogame *videogame = 0;

  foreach(QWidget *w, qapp->topLevelWidgets())
    {
      qtbook_videogame *v = qobject_cast<qtbook_videogame *> (w);

      if(v && v->getID() == "search")
	{
	  videogame = v;
	  break;
	}
    }

  if(!videogame)
    {
      videogame = new(std::nothrow) qtbook_videogame
	(this, "search", -1);
      videogame->raise();
      videogame->search();
    }
  else
    videogame->raise();
}

/*
** -- updateRows() --
*/

void qtbook::updateRows(const QString &oid, const int row,
			const QString &itemType)
{
  if(itemType == "cd")
    {
      foreach(QWidget *w, qapp->topLevelWidgets())
	{
	  qtbook_cd *cd = qobject_cast<qtbook_cd *> (w);

	  if(cd && cd->getID() == oid)
	    {
	      cd->updateRow(row);
	      break;
	    }
	}
    }
  else if(itemType == "dvd")
    {
      foreach(QWidget *w, qapp->topLevelWidgets())
	{
	  qtbook_dvd *dvd = qobject_cast<qtbook_dvd *> (w);

	  if(dvd && dvd->getID() == oid)
	    {
	      dvd->updateRow(row);
	      break;
	    }
	}
    }
  else if(itemType == "book")
    {
      foreach(QWidget *w, qapp->topLevelWidgets())
	{
	  qtbook_book *book = qobject_cast<qtbook_book *> (w);

	  if(book && book->getID() == oid)
	    {
	      book->updateRow(row);
	      break;
	    }
	}
    }
  else if(itemType == "journal")
    {
      foreach(QWidget *w, qapp->topLevelWidgets())
	{
	  qtbook_journal *journal = qobject_cast<qtbook_journal *> (w);

	  if(journal && journal->getID() == oid)
	    {
	      journal->updateRow(row);
	      break;
	    }
	}
    }
  else if(itemType == "magazine")
    {
      foreach(QWidget *w, qapp->topLevelWidgets())
	{
	  qtbook_magazine *magazine = qobject_cast<qtbook_magazine *> (w);

	  /*
	  ** The class qtbook_journal inherits qtbook_magazine.
	  */

	  if(!qobject_cast<qtbook_journal *> (w))
	    if(magazine && magazine->getID() == oid)
	      {
		magazine->updateRow(row);
		break;
	      }
	}
    }
  else if(itemType == "photographcollection")
    {
      foreach(QWidget *w, qapp->topLevelWidgets())
	{
	  qtbook_photographcollection *photograph =
	    qobject_cast<qtbook_photographcollection *> (w);

	  if(photograph && photograph->getID() == oid)
	    {
	      photograph->updateRow(row);
	      break;
	    }
	}
    }
  else if(itemType == "videogame")
    {
      foreach(QWidget *w, qapp->topLevelWidgets())
	{
	  qtbook_videogame *videogame = qobject_cast<qtbook_videogame *> (w);

	  if(videogame && videogame->getID() == oid)
	    {
	      videogame->updateRow(row);
	      break;
	    }
	}
    }
}

/*
** -- slotCloseMembersBrowser() --
*/

void qtbook::slotCloseMembersBrowser(void)
{
  /*
  ** Changes? If so, warn the user.
  */

  /*
  ** Also closes the Reservation History Browser.
  */

  history_diag->close();
  members_diag->close();
}

/*
** -- slotListReservedItems() --
*/

void qtbook::slotListReservedItems(void)
{
  int row = bb.table->currentRow();
  QString memberid = "";

  if(row < 0)
    {
      QMessageBox::critical(members_diag, tr("BiblioteQ: User Error"),
			    tr("In order to list a member's reserved items, "
			       "you must first select the member."));
      return;
    }

  memberid = misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("Member ID"));
  (void) populateTable(POPULATE_ALL, "All Reserved", memberid);
  members_diag->raise();
}

/*
** -- slotListOverdueItems() --
*/

void qtbook::slotListOverdueItems(void)
{
  int row = bb.table->currentRow();
  QString memberid = "";

  if(members_diag->isVisible())
    memberid = misc_functions::getColumnString
      (bb.table, row,
       m_bbColumnHeaderIndexes.indexOf("Member ID"));
  else if(roles.isEmpty())
    memberid = db.userName();

  (void) populateTable(POPULATE_ALL, "All Overdue", memberid);
  members_diag->raise();
}

/*
** -- slotReserveCopy() --
*/

void qtbook::slotReserveCopy(void)
{
  int row = ui.table->currentRow();
  int availability = 0;
  QString oid = "";
  QString type = "";
  QString errorstr = "";

  if(row < 0)
    {
      if(members_diag->isVisible())
	QMessageBox::critical(members_diag, tr("BiblioteQ: User Error"),
			      tr("In order to reserve an item, you must "
				 "first select it."));
      else
	QMessageBox::critical(this, tr("BiblioteQ: User Error"),
			      tr("In order to reserve an item, you must "
				 "first select it."));

      return;
    }

  oid = misc_functions::getColumnString
    (ui.table, row, ui.table->columnNumber("MYOID"));
  type = misc_functions::getColumnString
    (ui.table, row, ui.table->columnNumber("Type"));
  qapp->setOverrideCursor(Qt::WaitCursor);
  availability = misc_functions::getAvailability
    (oid, db, type, errorstr).toInt();
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    {
      addError(QString(tr("Database Error")),
	       QString(tr("Unable to determine the availability of "
			  "the selected item.")),
	       errorstr, __FILE__, __LINE__);
      QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			    tr("Unable to determine the availability of "
			       "the selected item."));
      return;
    }

  if(availability < 1)
    {
      if(members_diag->isVisible())
	QMessageBox::critical(members_diag, tr("BiblioteQ: User Error"),
			      tr("It appears that the item you selected "
				 "is not available for reservation."));
      else
	QMessageBox::critical(this, tr("BiblioteQ: User Error"),
			      tr("It appears that the item you selected "
				 "is not available for reservation."));

      return;
    }

  slotShowMembersBrowser();

  if(!ui.actionPopulate_Members_Browser_Table_on_Display->isChecked())
    if(QMessageBox::question(members_diag, tr("BiblioteQ: Question"),
			     tr("Would you like to retrieve the list of "
				"members?"),
			     QMessageBox::Yes | QMessageBox::No,
			     QMessageBox::No) == QMessageBox::Yes)
      slotPopulateMembersBrowser();

  if(bb.table->currentRow() < 0)
    bb.table->selectRow(0);
}

/*
** -- getZ3950Maps() --
*/

QMap<QString, QHash<QString, QString> > qtbook::getZ3950Maps(void) const
{
  return z3950Maps;
}

/*
** -- getAmazonHash() --
*/

QHash<QString, QString> qtbook::getAmazonHash(void) const
{
  return amazonImages;
}

/*
** -- slotShowMenu() --
*/

void qtbook::slotShowMenu(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  action->menu()->exec(QCursor::pos());
}

/*
** -- slotSetFonts() --
*/

void qtbook::slotSetFonts(void)
{
  QFontDialog dialog(this);

#ifdef Q_OS_MAC
  dialog.setAttribute(Qt::WA_MacMetalStyle, true);
#endif
  dialog.setOption(QFontDialog::DontUseNativeDialog);
  dialog.setCurrentFont(QFont());
  dialog.setWindowTitle(tr("BiblioteQ: Select Global Font"));

  if(dialog.exec() == QDialog::Accepted)
    setGlobalFonts(dialog.selectedFont());
}

/*
** -- setGlobalFonts() --
*/

void qtbook::setGlobalFonts(const QFont &font)
{
  qapp->setOverrideCursor(Qt::WaitCursor);
  qapp->setFont(font);

  foreach(QWidget *widget, qapp->allWidgets())
    {
      widget->setFont(font);

      if(!(qobject_cast<QDialog *> (widget) ||
	   qobject_cast<QMainWindow *> (widget)))
	widget->adjustSize();
    }

  menuBar()->setFont(font);

  foreach(QMenu *menu, menuBar()->findChildren<QMenu *>())
    foreach(QAction *action, menu->actions())
      action->setFont(font);

  qapp->restoreOverrideCursor();
}

/*
** -- slotShowCustomQuery() --
*/

void qtbook::slotShowCustomQuery(void)
{
  int i = 0;
  int j = 0;
  QSqlField field;
  QSqlRecord rec;
  QStringList list;
  QTreeWidgetItem *item1 = 0;
  QTreeWidgetItem *item2 = 0;

  if(cq.tables_t->columnCount() == 0)
    {
      qapp->setOverrideCursor(Qt::WaitCursor);

      if(db.driverName() == "QSQLITE")
	list << "book"
	     << "book_copy_info"
	     << "cd"
	     << "cd_copy_info"
	     << "cd_formats"
	     << "cd_songs"
	     << "dvd"
	     << "dvd_aspect_ratios"
	     << "dvd_copy_info"
	     << "dvd_ratings"
	     << "dvd_regions"
	     << "item_borrower"
	     << "item_borrower_vw"
	     << "journal"
	     << "journal_copy_info"
	     << "languages"
	     << "locations"
	     << "magazine"
	     << "magazine_copy_info"
	     << "member"
	     << "member_history"
	     << "minimum_days"
	     << "monetary_units"
	     << "photograph"
	     << "photograph_collection"
	     << "videogame"
	     << "videogame_copy_info"
	     << "videogame_platforms"
	     << "videogame_ratings";
      else if(roles.contains("administrator"))
	list << "admin"
	     << "book"
	     << "book_copy_info"
	     << "cd"
	     << "cd_copy_info"
	     << "cd_formats"
	     << "cd_songs"
	     << "dvd"
	     << "dvd_aspect_ratios"
	     << "dvd_copy_info"
	     << "dvd_ratings"
	     << "dvd_regions"
	     << "item_borrower"
	     << "item_borrower_vw"
	     << "item_request"
	     << "journal"
	     << "journal_copy_info"
	     << "languages"
	     << "locations"
	     << "magazine"
	     << "magazine_copy_info"
	     << "member"
	     << "member_history"
	     << "minimum_days"
	     << "monetary_units"
	     << "photograph"
	     << "photograph_collection"
	     << "videogame"
	     << "videogame_copy_info"
	     << "videogame_platforms"
	     << "videogame_ratings";
      else if(roles.contains("circulation"))
	list << "admin"
	     << "book"
	     << "book_copy_info"
	     << "cd"
	     << "cd_copy_info"
	     << "cd_formats"
	     << "cd_songs"
	     << "dvd"
	     << "dvd_aspect_ratios"
	     << "dvd_copy_info"
	     << "dvd_ratings"
	     << "dvd_regions"
	     << "item_borrower"
	     << "item_borrower_vw"
	     << "item_request"
	     << "journal"
	     << "journal_copy_info"
	     << "languages"
	     << "locations"
	     << "magazine"
	     << "magazine_copy_info"
	     << "member"
	     << "member_history"
	     << "minimum_days"
	     << "monetary_units"
	     << "photograph"
	     << "photograph_collection"
	     << "videogame"
	     << "videogame_copy_info"
	     << "videogame_platforms"
	     << "videogame_ratings";
      else if(roles.contains("librarian"))
	list << "admin"
	     << "book"
	     << "book_copy_info"
	     << "cd"
	     << "cd_copy_info"
	     << "cd_formats"
	     << "cd_songs"
	     << "dvd"
	     << "dvd_aspect_ratios"
	     << "dvd_copy_info"
	     << "dvd_ratings"
	     << "dvd_regions"
	     << "item_borrower_vw"
	     << "item_request"
	     << "journal"
	     << "journal_copy_info"
	     << "languages"
	     << "locations"
	     << "magazine"
	     << "magazine_copy_info"
	     << "minimum_days"
	     << "monetary_units"
	     << "photograph"
	     << "photograph_collection"
	     << "videogame"
	     << "videogame_copy_info"
	     << "videogame_platforms"
	     << "videogame_ratings";
      else if(roles.contains("membership"))
	list << "admin"
	     << "book"
	     << "book_copy_info"
	     << "cd"
	     << "cd_copy_info"
	     << "cd_formats"
	     << "cd_songs"
	     << "dvd"
	     << "dvd_aspect_ratios"
	     << "dvd_copy_info"
	     << "dvd_ratings"
	     << "dvd_regions"
	     << "item_borrower_vw"
	     << "journal"
	     << "journal_copy_info"
	     << "languages"
	     << "locations"
	     << "magazine"
	     << "magazine_copy_info"
	     << "member"
	     << "minimum_days"
	     << "monetary_units"
	     << "photograph"
	     << "photograph_collection"
	     << "videogame"
	     << "videogame_copy_info"
	     << "videogame_platforms"
	     << "videogame_ratings";
      else
	list << "book"
	     << "book_copy_info"
	     << "cd"
	     << "cd_copy_info"
	     << "cd_songs"
	     << "dvd"
	     << "dvd_copy_info"
	     << "journal"
	     << "journal_copy_info"
	     << "magazine"
	     << "magazine_copy_info"
	     << "photograph"
	     << "photograph_collection"
	     << "videogame"
	     << "videogame_copy_info";

      list.sort();
      cq.tables_t->setColumnCount(3);
      cq.tables_t->setHeaderLabels(QStringList()
				   << tr("Table Name")
				   << tr("Column")
				   << tr("Column Type")
				   << tr("NULL"));


      for(i = 0; i < list.size(); i++)
	if((item1 = new(std::nothrow) QTreeWidgetItem(cq.tables_t)) != 0)
	  {
	    item1->setText(0, list[i]);
	    rec = db.record(list[i]);

	    for(j = 0; j < rec.count(); j++)
	      {
		if((item2 = new(std::nothrow) QTreeWidgetItem(item1)) == 0)
		  {
		    addError(QString(tr("Memory Error")),
			     QString(tr("Unable to allocate "
					"memory for the \"item2\" "
					"object. "
					"This is a serious "
					"problem!")), QString(""),
			     __FILE__, __LINE__);
		    continue;
		  }

		field = rec.field(rec.fieldName(j));
		item2->setText(1, rec.fieldName(j));
		item2->setText(2, QVariant::typeToName(field.type()));

		if(field.requiredStatus() == QSqlField::Required)
		  item2->setText(3, tr("No"));
		else
		  item2->setText(3, "");
	      }
	  }
	else
	  addError(QString(tr("Memory Error")),
		   QString(tr("Unable to allocate "
			      "memory for the \"item1\" "
			      "object. "
			      "This is a serious "
			      "problem!")), QString(""),
		   __FILE__, __LINE__);

      for(i = 0; i < cq.tables_t->columnCount(); i++)
	{
	  cq.tables_t->header()->setResizeMode
	    (i, QHeaderView::ResizeToContents);
	  cq.tables_t->resizeColumnToContents(i);
	}

      qapp->restoreOverrideCursor();
    }

  static bool resized = false;

  if(!resized)
    customquery_diag->resize(0.75 * size().width(),
			     0.75 * size().height());

  resized = true;
  misc_functions::center(customquery_diag, this);
  customquery_diag->raise();
  customquery_diag->show();
}

/*
** -- slotCloseCustomQueryDialog() --
*/

void qtbook::slotCloseCustomQueryDialog(void)
{
  customquery_diag->close();
}

/*
** -- slotExecuteCustomQuery() --
*/

void qtbook::slotExecuteCustomQuery(void)
{
  QString querystr = "";

  querystr = cq.query_te->toPlainText().trimmed();

  if(querystr.isEmpty())
    {
      QMessageBox::critical(customquery_diag, tr("BiblioteQ: User Error"),
			    tr("Please provide a valid SQL statement."));
      return;
    }

  if(querystr.toLower().contains("alter") ||
     querystr.toLower().contains("create") ||
     querystr.toLower().contains("delete") ||
     querystr.toLower().contains("drop") ||
     querystr.toLower().contains("insert") ||
     querystr.toLower().contains("update"))
    {
      QMessageBox::critical(customquery_diag, tr("BiblioteQ: User Error"),
			    tr("Please provide a non-destructive SQL "
			       "statement."));
      return;
    }

  if(populateTable(CUSTOM_QUERY, "Custom", querystr) == 0)
    customquery_diag->close();
  else
    {
      misc_functions::center(customquery_diag, this);
      customquery_diag->raise();
      customquery_diag->show();
    }
}

/*
** -- slotPrintView() --
*/

void qtbook::slotPrintView(void)
{
  QString html = "<html>";
  QPrinter printer;
  QPrintDialog dialog(&printer, this);
  QTextDocument document;

#ifdef Q_OS_MAC
  dialog.setAttribute(Qt::WA_MacMetalStyle, true);
#endif

  qapp->setOverrideCursor(Qt::WaitCursor);
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
  qapp->restoreOverrideCursor();
  printer.setPaperSize(QPrinter::Letter);
  printer.setColorMode(QPrinter::GrayScale);
  printer.setOrientation(QPrinter::Landscape);

  if(dialog.exec() == QDialog::Accepted)
    {
      document.setHtml(html);
      document.print(&printer);
    }
}

/*
** -- slotPrintReserved() --
*/

void qtbook::slotPrintReserved(void)
{
  int row = bb.table->currentRow();
  int itemsReserved = 0;
  QString str = "";
  QString errorstr = "";
  QString memberid = "";
  QPrinter printer;
  QStringList itemsList;
  QPrintDialog dialog(&printer, members_diag);
  QTextDocument document;
  QMap<QString, QString> memberinfo;

#ifdef Q_OS_MAC
  dialog.setAttribute(Qt::WA_MacMetalStyle, true);
#endif

  if(row < 0)
    {
      QMessageBox::critical
	(members_diag, tr("BiblioteQ: User Error"),
	 tr("In order to print a member's reserved items, "
	    "you must first select the member."));
      return;
    }

  qapp->setOverrideCursor(Qt::WaitCursor);
  itemsReserved = misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("Books Reserved")).
    toInt() +
    misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("CDs Reserved")).toInt() +
    misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("DVDs Reserved")).toInt() +
    misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("Journals Reserved")).
    toInt() +
    misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("Magazines Reserved")).
    toInt() +
    misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("Video Games Reserved")).
    toInt();
  qapp->restoreOverrideCursor();

  if(itemsReserved < 1)
    {
      QMessageBox::critical(members_diag, tr("BiblioteQ: User Error"),
			    tr("The member that you selected does not have "
			       "any reserved items."));
      return;
    }

  memberid = misc_functions::getColumnString
    (bb.table, row, m_bbColumnHeaderIndexes.indexOf("Member ID"));
  memberinfo["firstname"] = misc_functions::getColumnString
    (bb.table, row,
     m_bbColumnHeaderIndexes.indexOf("First Name"));
  memberinfo["lastname"] = misc_functions::getColumnString
    (bb.table, row,
     m_bbColumnHeaderIndexes.indexOf("Last Name"));
  qapp->setOverrideCursor(Qt::WaitCursor);
  itemsList = misc_functions::getReservedItems(memberid, db, errorstr);
  qapp->restoreOverrideCursor();

  if(errorstr.isEmpty())
    {
      str = "<html>\n";
      str += tr("Reserved Items for ") + memberinfo["lastname"] +
	tr(", ") + memberinfo["firstname"] + "<br><br>";

      for(int i = 0; i < itemsList.size(); i++)
	str += itemsList[i] + "<br><br>";

      str = str.mid(0, str.length() - 8);
      str += "</html>";
      printer.setPageSize(QPrinter::Letter);
      printer.setColorMode(QPrinter::GrayScale);

      if(dialog.exec() == QDialog::Accepted)
	{
	  document.setHtml(str);
	  document.print(&printer);
	}
    }
  else
    {
      addError(QString(tr("Database Error")),
	       QString(tr("Unable to determine the reserved items for "
			  "the selected member.")),
	       errorstr, __FILE__, __LINE__);
      QMessageBox::critical(members_diag, tr("BiblioteQ: Database Error"),
			    tr("Unable to determine the reserved items for "
			       "the selected member."));
    }

  itemsList.clear();
  memberinfo.clear();
}

/*
** -- slotCopyError() --
*/

void qtbook::slotCopyError(void)
{
  int i = 0;
  int j = 0;
  QString text = "";
  QClipboard *clipboard = qapp->clipboard();
  QModelIndex index;
  QModelIndexList list = er.table->selectionModel()->selectedRows();

  if(list.isEmpty())
    {
      QMessageBox::critical(error_diag, tr("BiblioteQ: User Error"),
			    tr("To copy the contents of the Error "
			       "Log into "
			       "the clipboard buffer, you must first "
			       "select at least one entry."));
      return;
    }

  qapp->setOverrideCursor(Qt::WaitCursor);

  foreach(index, list)
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
  qapp->restoreOverrideCursor();
}

/*
** -- slotShowHistory() --
*/

void qtbook::slotShowHistory(void)
{
  int i = -1;
  int j = 0;
  int row = bb.table->currentRow();
  QString str = "";
  QString querystr = "";
  QSqlQuery query(db);
  QStringList list;
  QProgressDialog progress(history_diag);
  QTableWidgetItem *item = 0;

#ifdef Q_OS_MAC
  progress.setAttribute(Qt::WA_MacMetalStyle, true);
#endif

  if(members_diag->isVisible())
    if(row < 0)
      {
	QMessageBox::critical
	  (members_diag, tr("BiblioteQ: User Error"),
	   tr("In order to display a member's reservation "
	      "history, you must first select the member."));
	return;
      }

  list << "cd" << "dvd" << "book" << "journal" << "magazine" << "videogame";

  if(!roles.isEmpty())
    for(i = 0; i < list.size(); i++)
      {
	if(list[i] != "book")
	  querystr += QString("SELECT "
			      "history.memberid, "
			      "member.first_name, "
			      "member.last_name, "
			      "%1.title, "
			      "%1.id, "
			      "history.copyid, "
			      "%1.type, "
			      "history.reserved_date, "
			      "history.duedate, "
			      "history.returned_date, "
			      "history.reserved_by, "
			      "%1.myoid "
			      "FROM member_history history, "
			      "%1 %1, "
			      "member member "
			      "WHERE history.memberid = member.memberid AND "
			      "%1.myoid = history.item_oid AND "
			      "member.memberid = '%2' AND %1.type = "
			      "history.type ").arg(list[i]).arg
	    (misc_functions::getColumnString(bb.table, row,
					     m_bbColumnHeaderIndexes.
					     indexOf("Member ID")));
	else
	  {
	    if(db.driverName() != "QSQLITE")
	      querystr += QString
		("SELECT "
		 "history.memberid, "
		 "member.first_name, "
		 "member.last_name, "
		 "book.title, "
		 "book.id, "
		 "history.copyid, "
		 "book.type, "
		 "history.reserved_date, "
		 "history.duedate, "
		 "history.returned_date, "
		 "history.reserved_by, "
		 "book.myoid "
		 "FROM member_history history, "
		 "book book, "
		 "member member "
		 "WHERE history.memberid = member.memberid AND "
		 "book.myoid = history.item_oid AND "
		 "member.memberid = '%1' AND book.type = "
		 "history.type ").arg
		(misc_functions::getColumnString(bb.table, row,
						 m_bbColumnHeaderIndexes.
						 indexOf("Member ID")));
	    else
	      querystr += QString
		("SELECT "
		 "history.memberid, "
		 "member.first_name, "
		 "member.last_name, "
		 "book.title, "
		 "book.id, "
		 "history.copyid, "
		 "book.type, "
		 "history.reserved_date, "
		 "history.duedate, "
		 "history.returned_date, "
		 "history.reserved_by, "
		 "book.myoid "
		 "FROM member_history history, "
		 "book book, "
		 "member member "
		 "WHERE history.memberid = member.memberid AND "
		 "book.myoid = history.item_oid AND "
		 "member.memberid = '%1' AND book.type = "
		 "history.type ").arg
		(misc_functions::getColumnString(bb.table, row,
						 m_bbColumnHeaderIndexes.
						 indexOf("Member ID")));
	  }

	if(i != list.size() - 1)
	  querystr += "UNION ALL ";
      }
  else
    for(i = 0; i < list.size(); i++)
      {
	if(list[i] != "book")
	  querystr += QString("SELECT "
			      "history.memberid, "
			      "%1.title, "
			      "%1.id, "
			      "history.copyid, "
			      "%1.type, "
			      "history.reserved_date, "
			      "history.duedate, "
			      "history.returned_date, "
			      "history.reserved_by, "
			      "%1.myoid "
			      "FROM member_history history, "
			      "%1 %1 "
			      "WHERE history.memberid = '%2' AND "
			      "%1.myoid = history.item_oid AND %1.type = "
			      "history.type ").arg(list[i]).arg
	    (db.userName());
	else
	  {
	    if(db.driverName() != "QSQLITE")
	      querystr += QString
		("SELECT "
		 "history.memberid, "
		 "book.title, "
		 "book.id, "
		 "history.copyid, "
		 "book.type, "
		 "history.reserved_date, "
		 "history.duedate, "
		 "history.returned_date, "
		 "history.reserved_by, "
		 "book.myoid "
		 "FROM member_history history, "
		 "book book "
		 "WHERE history.memberid = '%1' AND "
		 "book.myoid = history.item_oid AND book.type = "
		 "history.type ").arg(db.userName());
	    else
	      querystr += QString
		("SELECT "
		 "history.memberid, "
		 "book.title, "
		 "book.id, "
		 "history.copyid, "
		 "book.type, "
		 "history.reserved_date, "
		 "history.duedate, "
		 "history.returned_date, "
		 "history.reserved_by, "
		 "book.myoid "
		 "FROM member_history history, "
		 "book book "
		 "WHERE history.memberid = '%1' AND "
		 "book.myoid = history.item_oid AND book.type = "
		 "history.type ").arg(db.userName());
	  }

	if(i != list.size() - 1)
	  querystr += "UNION ALL ";
      }

  querystr.append("ORDER BY 1");
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec(querystr))
    {
      qapp->restoreOverrideCursor();
      addError
	(QString(tr("Database Error")),
	 QString(tr("Unable to retrieve reservation history data for table "
		    "populating.")),
	 query.lastError().text(),
	 __FILE__, __LINE__);

      if(history_diag->isVisible())
	QMessageBox::critical(history_diag, tr("BiblioteQ: Database Error"),
			      tr("Unable to retrieve reservation "
				 "history data for table populating."));
      else if(members_diag->isVisible())
	QMessageBox::critical(members_diag, tr("BiblioteQ: Database Error"),
			      tr("Unable to retrieve reservation "
				 "history data for table populating."));
      else
	QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			      tr("Unable to retrieve reservation "
				 "history data for table populating."));

      return;
    }

  qapp->restoreOverrideCursor();
  history.table->clear();
  history.table->setCurrentItem(0);
  history.table->setColumnCount(0);
  history.table->setRowCount(0);
  list.clear();
  list.append(tr("Member ID"));

  if(!roles.isEmpty())
    {
      list.append(tr("First Name"));
      list.append(tr("Last Name"));
    }

  list.append(tr("Title"));
  list.append(tr("ID Number"));
  list.append(tr("Barcode"));
  list.append(tr("Type"));
  list.append(tr("Reservation Date"));
  list.append(tr("Original Due Date"));
  list.append(tr("Returned Date"));
  list.append(tr("Lender"));
  list.append("MYOID");
  m_historyColumnHeaderIndexes.clear();
  m_historyColumnHeaderIndexes.append("Title");
  m_historyColumnHeaderIndexes.append("ID Number");
  m_historyColumnHeaderIndexes.append("Barcode");
  m_historyColumnHeaderIndexes.append("Type");
  m_historyColumnHeaderIndexes.append("Reservation Date");
  m_historyColumnHeaderIndexes.append("Original Due Date");
  m_historyColumnHeaderIndexes.append("Returned Date");
  m_historyColumnHeaderIndexes.append("Lender");
  m_historyColumnHeaderIndexes.append("MYOID");
  history.table->setColumnCount(list.size());
  history.table->setHorizontalHeaderLabels(list);
  history.table->setColumnHidden(history.table->columnCount() - 1, true);
  list.clear();
  history.table->setSortingEnabled(false);

  if(db.driverName() != "QSQLITE")
    history.table->setRowCount(query.size());
  else
    history.table->setRowCount
      (misc_functions::sqliteQuerySize(querystr, db,
				       __FILE__, __LINE__));

  history.table->scrollToTop();
  history.table->horizontalScrollBar()->setValue(0);
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.setLabelText(tr("Populating the table..."));
  progress.setMinimum(0);

  if(db.driverName() == "QSQLITE")
    progress.setMaximum
      (misc_functions::sqliteQuerySize(querystr, db,
				       __FILE__, __LINE__));
  else
    progress.setMaximum(query.size());

  progress.show();
  progress.update();
  i = -1;

  while(i++, !progress.wasCanceled() && query.next())
    {
      if(query.isValid())
	for(j = 0; j < query.record().count(); j++)
	  {
	    if(query.record().fieldName(j).contains("date"))
	      {
		QDate date(QDate::fromString(query.value(j).toString(),
					     "MM/dd/yyyy"));

		str = date.toString(Qt::ISODate);
	      }
	    else
	      str = query.value(j).toString();

	    if((item = new(std::nothrow) QTableWidgetItem()) != 0)
	      {
		item->setText(str);
		history.table->setItem(i, j, item);
	      }
	    else
	      addError(QString(tr("Memory Error")),
		       QString(tr("Unable to allocate memory for the "
				  "\"item\" object. "
				  "This is a serious problem!")),
		       QString(""), __FILE__, __LINE__);
	  }

      if(i + 1 <= progress.maximum())
	progress.setValue(i + 1);

      progress.update();
#ifndef Q_OS_MAC
      qapp->processEvents();
#endif
    }

  progress.hide();
  history.table->setRowCount(i);
  history.table->setSortingEnabled(true);
  history.table->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  history.table->horizontalHeader()->resizeSections
    (QHeaderView::ResizeToContents);
  history.nextTool->setVisible(!roles.isEmpty());
  history.prevTool->setVisible(!roles.isEmpty());

  if(members_diag->isVisible())
    {
      static bool resized = false;

      if(!resized)
	history_diag->resize(0.75 * members_diag->size().width(),
			     0.75 * members_diag->size().height());

      resized = true;
      misc_functions::center(history_diag, members_diag);
    }
  else
    {
      static bool resized = false;

      if(!resized)
	history_diag->resize(0.75 * size().width(),
			     0.75 * size().height());

      resized = true;
      misc_functions::center(history_diag, this);
    }

  history_diag->raise();
  history_diag->show();
}

/*
** -- getMembersBrowser() --
*/

QMainWindow *qtbook::getMembersBrowser(void) const
{
  return members_diag;
}

/*
** -- slotPrintReservationHistory() --
*/

void qtbook::slotPrintReservationHistory(void)
{
  QString html = "<html>";
  QPrinter printer;
  QPrintDialog dialog(&printer, history_diag);
  QTextDocument document;

#ifdef Q_OS_MAC
  dialog.setAttribute(Qt::WA_MacMetalStyle, true);
#endif

  if(history.table->rowCount() == 0)
    {
      if(members_diag->isVisible())
	QMessageBox::critical(history_diag, tr("BiblioteQ: User Error"),
			      tr("The selected member does not yet have a "
				 "reservation history to print."));
      else
	QMessageBox::critical(history_diag, tr("BiblioteQ: User Error"),
			      tr("You do not yet have a reservation history "
				 "to print."));

      return;
    }

  qapp->setOverrideCursor(Qt::WaitCursor);
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
  qapp->restoreOverrideCursor();
  printer.setPageSize(QPrinter::Letter);
  printer.setColorMode(QPrinter::GrayScale);

  if(dialog.exec() == QDialog::Accepted)
    {
      document.setHtml(html);
      document.print(&printer);
    }
}

/*
** -- slotBranchChanged() --
*/

void qtbook::slotBranchChanged(void)
{
  QHash<QString, QString> tmphash;

  tmphash = branches[br.branch_name->currentText()];

  if(tmphash["database_type"] == "sqlite")
    {
      br.stackedWidget->setCurrentIndex(0);
      br.fileButton->setFocus();
    }
  else
    {
      br.stackedWidget->setCurrentIndex(1);
      br.userid->setFocus();
    }

  tmphash.clear();
  branch_diag->update();
  branch_diag->resize(branch_diag->width(),
		      branch_diag->minimumSize().height());
  branch_diag->show();
}

/*
** -- updateReservationHistoryBrowser() --
*/

void qtbook::updateReservationHistoryBrowser(const QString &memberid,
					     const QString &ioid,
					     const QString &copyid,
					     const QString &itemType,
					     const QString &returnedDate)
{
  int i = 0;
  QString value1 = "";
  QString value2 = "";
  QString value3 = "";

  /*
  ** Called from the Borrowers Editor when an item has been updated.
  */

  if(history_diag->isVisible())
    if(history.table->rowCount() > 0 &&
       misc_functions::getColumnString(history.table, 0,
				       m_historyColumnHeaderIndexes.
				       indexOf("Member ID")) ==
       memberid)
      {
	qapp->setOverrideCursor(Qt::WaitCursor);

	for(i = 0; i < history.table->rowCount(); i++)
	  {
	    value1 = misc_functions::getColumnString
	      (history.table, i,
	       m_historyColumnHeaderIndexes.indexOf("MYOID"));
	    value2 = misc_functions::getColumnString
	      (history.table, i,
	       m_historyColumnHeaderIndexes.indexOf("Barcode"));
	    value3 = misc_functions::getColumnString
	      (history.table, i,
	       m_historyColumnHeaderIndexes.indexOf("Type")).
	      toLower().remove(" ");

	    if(value1 == ioid && value2 == copyid && value3 == itemType)
	      {
		QDate date(QDate::fromString(returnedDate,
					     "MM/dd/yyyy"));

		misc_functions::updateColumn
		  (history.table, i,
		   m_historyColumnHeaderIndexes.indexOf("Returned Date"),
		   date.toString(Qt::ISODate));
		break;
	      }
	  }

	qapp->restoreOverrideCursor();
      }
}

/*
** -- slotShowChangePassword() --
*/

void qtbook::slotShowChangePassword(void)
{
  pass.userid->setText(db.userName());
  pass.currentpassword->clear();
  pass.password->clear();
  pass.passwordAgain->clear();
  pass.currentpassword->setFocus();
  pass_diag->show();
}

/*
** -- slotSavePassword() --
*/

void qtbook::slotSavePassword(void)
{
  QString errorstr = "";

  if(pass.currentpassword->text() != br.password->text())
    {
      QMessageBox::critical(pass_diag, tr("BiblioteQ: User Error"),
			    tr("The current password is incorrect."));
      pass.currentpassword->setFocus();
      return;
    }
  else if(pass.password->text().length() < 8)
    {
      QMessageBox::critical
	(pass_diag, tr("BiblioteQ: User Error"),
	 tr("The password must be at least eight characters "
	    "long."));
      pass.password->setFocus();
      return;
    }
  else if(pass.password->text() != pass.passwordAgain->text())
    {
      QMessageBox::critical
	(pass_diag, tr("BiblioteQ: User Error"),
	 tr("The passwords do not match. Please try again."));
      pass.password->setFocus();
      return;
    }

  qapp->setOverrideCursor(Qt::WaitCursor);
  misc_functions::savePassword(pass.userid->text(), db,
			       pass.password->text(), errorstr, roles);
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    {
      addError(QString(tr("Database Error")),
	       QString(tr("Unable to save the new password.")),
	       errorstr, __FILE__, __LINE__);
      QMessageBox::critical(pass_diag, tr("BiblioteQ: Database Error"),
			    tr("Unable to save the new password."));
    }
  else
    {
      /*
      ** Update the password field with the new password.
      */

      br.password->setText(pass.password->text());
      pass_diag->close();
    }
}

/*
** -- slotResetLoginDialog() --
*/

void qtbook::slotResetLoginDialog(void)
{
  br.userid->clear();
  br.password->clear();
  br.filename->clear();
  br.adminCheck->setChecked(false);

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

/*
** -- slotSelectDatabaseFile() --
*/

void qtbook::slotSelectDatabaseFile(void)
{
  QFileDialog dialog(branch_diag);

#ifdef Q_OS_MAC
  dialog.setAttribute(Qt::WA_MacMetalStyle, true);
#endif
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setDirectory(QDir::homePath());
  dialog.setNameFilter("SQLite Database (*.sqlite)");
  dialog.setWindowTitle(tr("BiblioteQ: SQLite Database Selection"));
  dialog.exec();

  if(dialog.result() == QDialog::Accepted)
    br.filename->setText(dialog.selectedFiles().at(0));
}

/*
** -- slotShowAdminDialog() --
*/

void qtbook::slotShowAdminDialog(void)
{
  static bool resized = false;

  if(!resized)
    admin_diag->resize(0.85 * size().width(),
		       0.85 * size().height());

  resized = true;
  misc_functions::center(admin_diag, this);
  admin_diag->raise();
  admin_diag->show();

  if(ui.actionPopulate_Administrator_Browser_Table_on_Display->isChecked())
    slotRefreshAdminList();
}

/*
** -- slotAddAdmin() --
*/

void qtbook::slotAddAdmin(void)
{
  int i = 0;
  QCheckBox *checkBox = 0;
  QTableWidgetItem *item = 0;

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

/*
** -- slotDeleteAdmin() --
*/

void qtbook::slotDeleteAdmin(void)
{
  int row = ab.table->currentRow();
  QString str = "";

  if(row < 0)
    {
      QMessageBox::critical
	(admin_diag, tr("BiblioteQ: User Error"),
	 tr("To delete an entry, you must first select it."));
      return;
    }

  str = ab.table->item(row, 0)->text().trimmed();

  if((ab.table->item(row, 0)->flags() & Qt::ItemIsEditable) == 0 &&
     str == getAdminID())
    {
      QMessageBox::critical(admin_diag, tr("BiblioteQ: User Error"),
			    tr("As an administrator, you may not delete "
			       "your account."));
      return;
    }
  else
    {
      if(!str.isEmpty() && !deletedAdmins.contains(str))
	deletedAdmins.append(str);

      ab.table->removeRow(row);
#ifdef Q_OS_MAC
      ab.table->hide();
      ab.table->show();
#endif
    }
}

/*
** -- slotAdminCheckBoxClicked() --
*/

void qtbook::slotAdminCheckBoxClicked(int state)
{
  int i = 0;
  int j = 0;
  int row = -1;
  int column = -1;
  QCheckBox *box = qobject_cast<QCheckBox *> (sender());

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
	      (static_cast<QCheckBox *> (ab.table->cellWidget(row, i)))->
		setChecked(false);
	}
      else
	if(box->isChecked())
	  (static_cast<QCheckBox *> (ab.table->cellWidget(row, 1)))->
	    setChecked(false);
    }
}

/*
** -- slotRefreshAdminList() --
*/

void qtbook::slotRefreshAdminList(void)
{
  int i = -1;
  int j = 0;
  QString str = "";
  QString querystr = "";
  QString columnname = "";
  QCheckBox *checkBox = 0;
  QSqlQuery query(db);
  QStringList list;
  QProgressDialog progress(admin_diag);
  QTableWidgetItem *item = 0;

#ifdef Q_OS_MAC
  progress.setAttribute(Qt::WA_MacMetalStyle, true);
#endif
  querystr = "SELECT username, roles FROM admin ORDER BY username";
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec(querystr))
    {
      qapp->restoreOverrideCursor();
      addError(QString(tr("Database Error")),
	       QString(tr("Unable to retrieve administrator data for table "
			  "populating.")),
	       query.lastError().text(),
	       __FILE__, __LINE__);
      QMessageBox::critical(admin_diag, tr("BiblioteQ: Database Error"),
			    tr("Unable to retrieve administrator "
			       "data for table populating."));
      return;
    }

  qapp->restoreOverrideCursor();
  resetAdminBrowser();
  ab.table->setRowCount(query.size());
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.setLabelText(tr("Populating the table..."));
  progress.setMaximum(query.size());
  progress.setMinimum(0);
  progress.show();
  progress.update();
  i = -1;

  while(i++, !progress.wasCanceled() && query.next())
    {
      if(query.isValid())
	{
	  if((item = new(std::nothrow) QTableWidgetItem()) != 0)
	    {
	      str = query.value(0).toString();
	      item->setText(str);
	      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	      str = query.value(1).toString();
	      ab.table->setItem(i, 0, item);

	      for(j = 1; j < ab.table->columnCount(); j++)
		if(query.value(0).toString() == getAdminID() && j > 1)
		  {
		    if((item = new(std::nothrow) QTableWidgetItem()) != 0)
		      {
			item->setFlags(Qt::ItemIsEnabled |
				       Qt::ItemIsSelectable);
			ab.table->setItem(i, j, item);
		      }
		    else
		      addError
			(QString(tr("Memory Error")),
			 QString(tr("Unable to allocate memory for the "
				    "\"item\" object. "
				    "This is a serious problem!")),
			 QString(""), __FILE__, __LINE__);
		  }
		else if((checkBox = new(std::nothrow) QCheckBox()) != 0)
		  {
		    ab.table->setCellWidget(i, j, checkBox);
		    columnname = m_abColumnHeaderIndexes.value(j).toLower();

		    if(str.toLower().contains(columnname))
		      checkBox->setChecked(true);

		    if(query.value(0).toString() == getAdminID())
		      checkBox->setEnabled(false);
		    else
		      connect(checkBox, SIGNAL(stateChanged(int)), this,
			      SLOT(slotAdminCheckBoxClicked(int)));
		  }
		else
		  addError(QString(tr("Memory Error")),
			   QString(tr("Unable to allocate memory for the "
				      "\"checkBox\" object. "
				      "This is a serious problem!")),
			   QString(""), __FILE__, __LINE__);
	    }
	  else
	    addError(QString(tr("Memory Error")),
		     QString(tr("Unable to allocate memory for the "
				"\"item\" object. "
				"This is a serious problem!")),
		     QString(""), __FILE__, __LINE__);
	}

      if(i + 1 <= progress.maximum())
	progress.setValue(i + 1);

      progress.update();
#ifndef Q_OS_MAC
      qapp->processEvents();
#endif
    }

  progress.hide();
  ab.table->setRowCount(i);
  ab.table->horizontalHeader()->resizeSections
    (QHeaderView::ResizeToContents);
  deletedAdmins.clear();
}

/*
** -- slotSaveAdministrators() --
*/

void qtbook::slotSaveAdministrators(void)
{
  int i = 0;
  int j = 0;
  int ucount = 0;
  bool adminCreated = false;
  QString str = "";
  QString adminStr = "";
  QString errorstr = "";
  QString querystr = "";
  QCheckBox *checkBox = 0;
  QSqlQuery query(db);
  QStringList tmplist;
  QProgressDialog progress(admin_diag);

#ifdef Q_OS_MAC
  progress.setAttribute(Qt::WA_MacMetalStyle, true);
#endif
  /*
  ** 1. Prohibit duplicate administrator ids and administrators
  **    without privileges.
  ** 2. Create a database transaction.
  ** 3. Delete required entries from the admin table.
  ** 4. Remove all deleted database accounts.
  ** 5. Create new entries in the admin table.
  ** 6. Create new database accounts with correct privileges.
  ** 7. Commit or rollback the current database transaction.
  */

  ab.saveButton->setFocus();

  for(i = 0; i < ab.table->rowCount(); i++)
    {
      if(ab.table->item(i, 0)->text().trimmed().isEmpty())
	continue;

      if(!(static_cast<QCheckBox *>
	   (ab.table->cellWidget(i, 1))->isChecked() ||
	   static_cast<QCheckBox *>
	   (ab.table->cellWidget(i, 2))->isChecked() ||
	   static_cast<QCheckBox *>
	   (ab.table->cellWidget(i, 3))->isChecked() ||
	   static_cast<QCheckBox *>
	   (ab.table->cellWidget(i, 4))->isChecked()))
	{
	  tmplist.clear();
	  ab.table->selectRow(i);
	  ab.table->horizontalScrollBar()->setValue(i);
	  QMessageBox::critical
	    (admin_diag, tr("BiblioteQ: User Error"),
	     tr("Administrators must belong to at least one category."));
	  return;
	}

      if(!tmplist.contains(ab.table->item(i, 0)->text().trimmed()))
	tmplist.append(ab.table->item(i, 0)->text().trimmed());
      else
	{
	  tmplist.clear();
	  ab.table->selectRow(i);
	  ab.table->horizontalScrollBar()->setValue(i);
	  QMessageBox::critical
	    (admin_diag, tr("BiblioteQ: User Error"),
	     tr("Duplicate administrator ids are not allowed."));
	  return;
	}
    }

  tmplist.clear();
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!db.transaction())
    {
      qapp->restoreOverrideCursor();
      addError
	(QString(tr("Database Error")),
	 QString(tr("Unable to create a database transaction.")),
	 db.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical
	(admin_diag, tr("BiblioteQ: Database Error"),
	 tr("Unable to create a database transaction."));
      return;
    }

  /*
  ** Remove database accounts.
  */

  for(i = 0; i < deletedAdmins.size(); i++)
    {
      querystr = QString("DELETE FROM admin WHERE username = '%1'").arg
	(deletedAdmins[i]);

      if(!query.exec(querystr))
	{
	  qapp->restoreOverrideCursor();
	  addError(QString(tr("Database Error")),
		   QString(tr("An error occurred while attempting to "
			      "remove ")) + deletedAdmins[i] +
		   QString(tr(".")),
		   query.lastError().text(), __FILE__, __LINE__);
	  goto db_rollback;
	}

      misc_functions::DBAccount(deletedAdmins[i], db,
				misc_functions::DELETE_USER, errorstr);

      if(!errorstr.isEmpty())
	{
	  qapp->restoreOverrideCursor();
	  addError
	    (QString(tr("Database Error")),
	     QString(tr("An error occurred while attempting to "
			"remove the database account ")) +
	     deletedAdmins[i] + QString(tr(".")),
	     errorstr, __FILE__, __LINE__);
	  goto db_rollback;
	}
    }

  qapp->restoreOverrideCursor();
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.setLabelText(tr("Saving administrator information..."));
  progress.setMaximum(ab.table->rowCount());
  progress.setMinimum(0);
  progress.show();
  progress.update();

  /*
  ** Add or modify administrators.
  */

  for(i = 0; i < ab.table->rowCount(); i++)
    {
      str = "";
      adminStr = ab.table->item(i, 0)->text().trimmed();

      if(i + 1 <= progress.maximum())
	progress.setValue(i + 1);

      if(adminStr.isEmpty())
	continue; // Ignore empty administrator ids.
      else if(adminStr == getAdminID())
	continue; // Ignore current administrator.

      if((static_cast<QCheckBox *> (ab.table->cellWidget(i, 1)))->
	 isChecked())
	str = "administrator";
      else
	for(j = 2; j < ab.table->columnCount(); j++)
	  {
	    checkBox = static_cast<QCheckBox *> (ab.table->cellWidget(i, j));

	    if(checkBox->isChecked())
	      str += m_abColumnHeaderIndexes.value(j).toLower() +
		" ";
	  }

      str = str.trimmed();

      if(str.isEmpty())
	str = "none";

      ucount = misc_functions::userCount(adminStr, db, errorstr);

      if(!errorstr.isEmpty())
	{
	  progress.hide();
	  addError
	    (QString(tr("Database Error")),
	     QString(tr("The function misc_functions::userCount() failed "
			"for ")) + adminStr + QString(tr(".")),
	     errorstr, __FILE__, __LINE__);
	  goto db_rollback;
	}

      if(ucount == 0)
	{
	  query.prepare("INSERT INTO admin (username, roles) "
			"VALUES (?, ?)");
	  query.bindValue(0, adminStr);
	  query.bindValue(1, str);
	}
      else
	{
	  query.prepare("UPDATE admin SET roles = ? WHERE "
			"username = ?");
	  query.bindValue(0, str);
	  query.bindValue(1, adminStr);
	}

      if(!query.exec())
	{
	  progress.hide();
	  addError
	    (QString(tr("Database Error")),
	     QString(tr("Unable to create or update the administrator entry "
			"for ")) + adminStr + QString(tr(".")),
	     query.lastError().text(), __FILE__, __LINE__);
	  goto db_rollback;
	}

      if(ucount == 0)
	{
	  misc_functions::DBAccount(adminStr, db,
				    misc_functions::CREATE_USER,
				    errorstr, str);

	  if(!errorstr.isEmpty())
	    {
	      progress.hide();
	      addError
		(QString(tr("Database Error")),
		 QString(tr("An error occurred while attempting to "
			    "create a database account for ")) + adminStr +
		 QString(tr(".")),
		 errorstr, __FILE__, __LINE__);
	      goto db_rollback;
	    }

	  adminCreated = true;
	}
      else
	{
	  misc_functions::revokeAll(adminStr, db, errorstr);

	  if(!errorstr.isEmpty())
	    {
	      progress.hide();
	      addError
		(QString(tr("Database Error")),
		 QString(tr("An error occurred while attempting to "
			    "revoke privileges from ")) + adminStr +
		 QString(tr(".")),
		 errorstr, __FILE__, __LINE__);
	      goto db_rollback;
	    }

	  misc_functions::grantPrivs(adminStr, str, db, errorstr);

	  if(!errorstr.isEmpty())
	    {
	      progress.hide();
	      addError
		(QString(tr("Database Error")),
		 QString(tr("An error occurred while attempting to "
			    "grant privileges to ")) + adminStr +
		 QString(tr(".")),
		 errorstr, __FILE__, __LINE__);
	      goto db_rollback;
	    }
	}

      progress.update();
#ifndef Q_OS_MAC
      qapp->processEvents();
#endif
    }

  progress.hide();
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!db.commit())
    {
      addError
	(QString(tr("Database Error")),
	 QString(tr("Unable to commit the current database "
		    "transaction.")),
	 db.lastError().text(), __FILE__,
	 __LINE__);
      db.rollback();
      qapp->restoreOverrideCursor();
      QMessageBox::critical(admin_diag,
			    tr("BiblioteQ: Database Error"),
			    tr("Unable to commit the current "
			       "database transaction."));
      return;
    }

  qapp->restoreOverrideCursor();
  deletedAdmins.clear();

  if(adminCreated)
    QMessageBox::information
      (admin_diag, tr("BiblioteQ: Information"),
       tr("Please notify new administrators that their "
	  "default password has been set "
	  "to tempPass."));

  slotRefreshAdminList();
  return;

 db_rollback:

  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!db.rollback())
    addError(QString(tr("Database Error")), QString(tr("Rollback failure.")),
	     db.lastError().text(), __FILE__, __LINE__);

  qapp->restoreOverrideCursor();
  QMessageBox::critical(admin_diag, tr("BiblioteQ: Database Error"),
			tr("An error occurred while attempting to save "
			   "the administrator information."));
}

/*
** -- slotRequest --
*/

void qtbook::slotRequest(void)
{
  /*
  ** This method is used to either request an item or cancel a request.
  */

  int i = 0;
  int ct = 0;
  int numcompleted = 0;
  bool error = false;
  bool isRequesting = true;
  QDate now = QDate::currentDate();
  QString oid = "";
  QString itemType = "";
  QString querystr = "";
  QSqlQuery query(db);
  QModelIndex index;
  QProgressDialog progress(this);
  QModelIndexList list = ui.table->selectionModel()->selectedRows();

#ifdef Q_OS_MAC
  progress.setAttribute(Qt::WA_MacMetalStyle, true);
#endif

  if(!roles.isEmpty())
    isRequesting = false;
  else if(ui.typefilter->itemData
	  (ui.typefilter->currentIndex()).toString() == "All Requested")
    isRequesting = false;

  if(isRequesting)
    {
      if(list.isEmpty())
	{
	  QMessageBox::critical
	    (this, tr("BiblioteQ: User Error"),
	     tr("Please select at least one item to place "
		"on request."));
	  return;
	}
    }
  else
    {
      if(list.isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please select at least one request to "
				   "cancel."));
	  return;
	}

      if(list.size() > 0)
	if(QMessageBox::question(this, tr("BiblioteQ: Question"),
				 tr("Are you sure that you wish to "
				    "cancel the selected request(s)?"),
				 QMessageBox::Yes | QMessageBox::No,
				 QMessageBox::No) == QMessageBox::No)
	  {
	    list.clear();
	    return;
	  }
    }

  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));

  if(isRequesting)
    progress.setLabelText(tr("Requesting the selected item(s)..."));
  else
    progress.setLabelText(tr("Cancelling the selected request(s)..."));

  progress.setMaximum(list.size());
  progress.setMinimum(0);
  progress.show();
  progress.update();

  foreach(index, list)
    {
      i = index.row();
      ct += 1;

      if(isRequesting)
	oid = misc_functions::getColumnString
	  (ui.table, i, ui.table->columnNumber("MYOID"));
      else
	{
	  oid = misc_functions::getColumnString
	    (ui.table, i, ui.table->columnNumber("REQUESTOID"));

	  if(oid.isEmpty())
	    oid = "-1";
	}

      if(isRequesting)
	{
	  itemType = misc_functions::getColumnString
	    (ui.table, i,
	     ui.table->columnNumber("Type"));
	  querystr = "INSERT INTO item_request (item_oid, memberid, "
	    "requestdate, type) VALUES (?, ?, ?, ?)";
	  query.prepare(querystr);
	  query.bindValue(0, oid);
	  query.bindValue(1, db.userName());
	  query.bindValue(2, now.toString("MM/dd/yyyy"));
	  query.bindValue(3, itemType);
	}
      else
	{
	  querystr = "DELETE FROM item_request WHERE myoid = ?";
	  query.prepare(querystr);
	  query.bindValue(0, oid);
	}

      if(!query.exec())
	{
	  error = true;

	  if(isRequesting)
	    addError(QString(tr("Database Error")),
		     QString(tr("Unable to request the item.")),
		     query.lastError().text(), __FILE__, __LINE__);
	  else
	    addError(QString(tr("Database Error")),
		     QString(tr("Unable to cancel the request.")),
		     query.lastError().text(), __FILE__, __LINE__);
	}
      else
	{
	  numcompleted += 1;

	  if(!isRequesting)
	    deleteItem(oid, itemType);
	}

      if(i + 1 <= progress.maximum())
	progress.setValue(ct);

      progress.update();
#ifndef Q_OS_MAC
      qapp->processEvents();
#endif
    }

  progress.hide();

  /*
  ** Provide some fancy messages.
  */

  if(error && isRequesting)
    QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			  tr("Unable to request some or all of the selected "
			     "items. "
			     "Please verify that you are not attempting to "
			     "request duplicate items."));
  else if(error)
    QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			  tr("Unable to cancel some or all of the selected "
			     "requests."));

  if(!isRequesting && numcompleted > 0)
    slotRefresh();

  list.clear();
}

/*
** -- prepareFilter() --
*/

void qtbook::prepareFilter(void)
{
  QStringList tmplist1;
  QStringList tmplist2;

  if(db.driverName() == "QSQLITE")
    {
      tmplist1 << "All"
	       << "All Available"
	       << "All Overdue"
	       << "All Reserved"
	       << "Books"
	       << "DVDs"
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
	       << tr("Journals")
	       << tr("Magazines")
	       << tr("Music CDs")
	       << tr("Photograph Collections")
	       << tr("Video Games");
    }
  else if(roles.isEmpty() ||
	  roles.contains("administrator") ||
	  roles.contains("circulation"))
    {
      tmplist1 << "All"
	       << "All Available"
	       << "All Overdue"
	       << "All Requested"
	       << "All Reserved"
	       << "Books"
	       << "DVDs"
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
	       << "Books"
	       << "DVDs"
	       << "Journals"
	       << "Magazines"
	       << "Music CDs"
	       << "Photograph Collections"
	       << "Video Games";
      tmplist2 << tr("All")
	       << tr("All Available")
	       << tr("Books")
	       << tr("DVDs")
	       << tr("Journals")
	       << tr("Magazines")
	       << tr("Music CDs")
	       << tr("Photograph Collections")
	       << tr("Video Games");
    }

  ui.typefilter->clear();

  for(int i = 0; i < tmplist1.size(); i++)
    ui.typefilter->addItem(tmplist2[i],
			   QVariant(tmplist1[i]));

  tmplist1.clear();
  tmplist2.clear();
}

/*
** -- slotSqliteFileSelected() --
*/

void qtbook::slotSqliteFileSelected(bool state)
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
      if(branches.contains(br.branch_name->itemText(i)))
	if(branches[br.branch_name->itemText(i)]["database_type"] ==
	   "sqlite")
	  {
	    br.branch_name->setCurrentIndex(i);
	    break;
	  }
    }

  slotConnectDB();
}

/*
** -- slotClearSqliteMenu() --
*/

void qtbook::slotClearSqliteMenu(bool state)
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

/*
** -- createSqliteMenuActions() --
*/

void qtbook::createSqliteMenuActions(void)
{
  QSettings settings;
  QStringList dups;
  QStringList allKeys(settings.allKeys());

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

      if(!dups.contains(str) && QFileInfo(str).isReadable() &&
	 QFileInfo(str).isWritable())
	dups.append(str);
      else
	{
	  settings.remove(allKeys[i]);
	  continue;
	}

      QAction *action = new(std::nothrow) QAction(str, this);

      if(!action)
	continue;

      action->setData(str);
      connect(action, SIGNAL(triggered(bool)), this,
	      SLOT(slotSqliteFileSelected(bool)));
      ui.menu_Recent_SQLite_Files->addAction(action);
    }

  dups.clear();
  allKeys.clear();

  QAction *action = new(std::nothrow) QAction(tr("&Clear Menu"), this);

  if(action)
    {
      connect(action, SIGNAL(triggered(bool)), this,
	      SLOT(slotClearSqliteMenu(bool)));

      if(ui.menu_Recent_SQLite_Files->actions().size() > 0)
	ui.menu_Recent_SQLite_Files->addSeparator();

      ui.menu_Recent_SQLite_Files->addAction(action);
    }
}

/*
** -- slotPreviousPage() --
*/

void qtbook::slotPreviousPage(void)
{
  if(db.isOpen())
    (void) populateTable(lastSearchType, previousTypeFilter,
			 lastSearchStr, PREVIOUS_PAGE);
}

/*
** -- slotNextPage() --
*/

void qtbook::slotNextPage(void)
{
  if(db.isOpen())
    (void) populateTable(lastSearchType, previousTypeFilter,
			 lastSearchStr, NEXT_PAGE);
}

/*
** -- slotPageClicked() --
*/

void qtbook::slotPageClicked(const QString &link)
{
  if(db.isOpen())
    (void) populateTable(lastSearchType, previousTypeFilter,
			 lastSearchStr, -link.toInt());
}

/*
** -- getPreferredSRUSite() --
*/

QString qtbook::getPreferredSRUSite(void) const
{
  for(int i = 0; i < ui.menuPreferredSRUSite->actions().size(); i++)
    if(ui.menuPreferredSRUSite->actions()[i]->isChecked())
      return ui.menuPreferredSRUSite->actions()[i]->text();

  return "";
}

/*
** -- getPreferredZ3950Site() --
*/

QString qtbook::getPreferredZ3950Site(void) const
{
  for(int i = 0; i < ui.menuPreferredZ3950Server->actions().size(); i++)
    if(ui.menuPreferredZ3950Server->actions()[i]->isChecked())
      return ui.menuPreferredZ3950Server->actions()[i]->text();

  return "";
}

/*
** -- getTypeFilterString() --
*/

QString qtbook::getTypeFilterString(void) const
{
  return ui.typefilter->itemData(ui.typefilter->currentIndex()).toString();
}

/*
** -- slotDisplayNewSqliteDialog() --
*/

void qtbook::slotDisplayNewSqliteDialog(void)
{
  bool error = true;
  QFileDialog dialog(this);

#ifdef Q_OS_MAC
  dialog.setAttribute(Qt::WA_MacMetalStyle, true);
#endif
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setDirectory(QDir::homePath());
  dialog.setNameFilter("SQLite Database (*.sqlite)");
  dialog.setDefaultSuffix("sqlite");
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setWindowTitle(tr("BiblioteQ: New SQLite Database"));
  dialog.exec();

  if(dialog.result() == QDialog::Accepted)
    {
      int rc = 0;
      sqlite3 *ppDb = 0;

      qapp->setOverrideCursor(Qt::WaitCursor);
      QFile::remove(dialog.selectedFiles().at(0));
      rc = sqlite3_open_v2(dialog.selectedFiles().at(0).toUtf8(),
			   &ppDb,
			   SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
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
      qapp->restoreOverrideCursor();

      if(!error)
	{
	  /*
	  ** The user may not wish to open the new database, so let's not
	  ** connect automatically.
	  */

	  if(db.isOpen())
	    {
	      /*
	      ** Attempt to locate an SQLite branch.
	      */

	      bool found = false;

	      for(int i = 0; i < br.branch_name->count(); i++)
		{
		  if(branches.contains(br.branch_name->itemText(i)))
		    if(branches[br.branch_name->itemText(i)]
		       ["database_type"] == "sqlite")
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
		      slotDisconnect();
		      br.filename->setText(dialog.selectedFiles().at(0));
		      slotConnectDB();
		    }
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
		  if(branches.contains(br.branch_name->itemText(i)))
		    if(branches[br.branch_name->itemText(i)]
		       ["database_type"] == "sqlite")
		      {
			found = true;
			br.branch_name->setCurrentIndex(i);
			break;
		      }
		}

	      if(found)
		{
		  br.filename->setText(dialog.selectedFiles().at(0));
		  slotConnectDB();
		}
	    }
	}
    }
}

/*
** -- slotShowDbEnumerations() --
*/

void qtbook::slotShowDbEnumerations(void)
{
  db_enumerations->show
    (this,
     ui.actionPopulate_Database_Enumerations_Browser_on_Display->
     isChecked());
}

/*
** -- slotChangeView() --
*/

void qtbook::slotChangeView(bool checked)
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

/*
** -- slotExportAsCSV(void) --
*/

void qtbook::slotExportAsCSV(void)
{
  QFileDialog dialog(this);

#ifdef Q_OS_MAC
  dialog.setAttribute(Qt::WA_MacMetalStyle, true);
#endif
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setDirectory(QDir::homePath());
  dialog.setNameFilter(tr("CSV (*.csv)"));
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setWindowTitle(tr("BiblioteQ: Export Table View as CSV"));
  dialog.setDefaultSuffix("csv");
  dialog.exec();

  if(dialog.result() == QDialog::Accepted)
    {
      qapp->setOverrideCursor(Qt::WaitCursor);

      QFile file(dialog.selectedFiles().at(0));

      if(file.open(QIODevice::WriteOnly | QIODevice::Truncate |
		   QIODevice::Text))
	{
	  QString str("");
	  QTextStream stream(&file);

	  for(int i = 0; i < ui.table->columnCount(); i++)
	    if(!ui.table->isColumnHidden(i))
	      {
		if(ui.table->horizontalHeaderItem(i)->text().contains(","))
		  str += QString("\"%1\",").arg
		    (ui.table->horizontalHeaderItem(i)->text());
		else
		  str += QString("%1,").arg
		    (ui.table->horizontalHeaderItem(i)->text());
	      }

	  if(str.endsWith(","))
	    str = str.mid(0, str.length() - 1);

	  if(!str.isEmpty())
	    stream << str << endl;

	  for(int i = 0; i < ui.table->rowCount(); i++)
	    {
	      str = "";

	      for(int j = 0; j < ui.table->columnCount(); j++)
		if(!ui.table->isColumnHidden(j))
		  {
		    QString cleaned(ui.table->item(i, j)->text());

		    cleaned.replace("\n", " ");
		    cleaned.replace("\r\n", " ");
		    cleaned = cleaned.trimmed();

		    if(cleaned.contains(","))
		      str += QString("\"%1\",").arg(cleaned);
		    else
		      str += QString("%1,").arg(cleaned);
		  }

	      if(str.endsWith(","))
		str = str.mid(0, str.length() - 1);

	      if(!str.isEmpty())
		stream << str << endl;
	    }

	  file.close();
	}

      qapp->restoreOverrideCursor();
    }
}

/*
** -- slotSectionResized() --
*/

void qtbook::slotSectionResized(int logicalIndex, int oldSize,
				int newSize)
{
  Q_UNUSED(logicalIndex);
  Q_UNUSED(oldSize);
  Q_UNUSED(newSize);
}

/*
** -- slotDuplicate() --
*/

void qtbook::slotDuplicate(void)
{
  if(!db.isOpen())
    return;

  int i = 0;
  bool error = false;
  QString oid = "";
  QString type = "";
  qtbook_cd *cd = 0;
  qtbook_dvd *dvd = 0;
  QModelIndex index;
  main_table *table = ui.table;
  qtbook_book *book = 0;
  qtbook_journal *journal = 0;
  qtbook_magazine *magazine = 0;
  qtbook_videogame *video_game = 0;
  qtbook_photographcollection *photograph = 0;
  QModelIndexList list = table->selectionModel()->selectedRows();

  if(list.isEmpty())
    {
      QMessageBox::critical(this, tr("BiblioteQ: User Error"),
			    tr("Please select at least one item to "
			       "duplicate."));
      return;
    }
  else if(list.size() >= 5)
    if(QMessageBox::question(this, tr("BiblioteQ: Question"),
			     tr("Are you sure that you wish to duplicate "
				"the ") +
			     QString::number(list.size()) +
			     tr(" selected items?"),
			     QMessageBox::Yes | QMessageBox::No,
			     QMessageBox::No) == QMessageBox::No)
      {
	list.clear();
	return;
      }

  QString id("");

  qStableSort(list.begin(), list.end());

  foreach(index, list)
    {
      i = index.row();
      oid = misc_functions::getColumnString
	(table, i, table->columnNumber("MYOID"));
      type = misc_functions::getColumnString
	(table, i, table->columnNumber("Type"));
      m_idCt += 1;
      id = QString("duplicate_%1").arg(m_idCt);

      if(type.toLower() == "cd")
	{
	  cd = new(std::nothrow) qtbook_cd(this, oid, i);

	  if(cd)
	    cd->duplicate(id, EDITABLE);
	}
      else if(type.toLower() == "dvd")
	{
	  dvd = new(std::nothrow) qtbook_dvd(this, oid, i);

	  if(dvd)
	    dvd->duplicate(id, EDITABLE);
	}
      else if(type.toLower() == "book")
	{
	  book = new(std::nothrow) qtbook_book(this, oid, i);

	  if(book)
	    book->duplicate(id, EDITABLE);
	}
      else if(type.toLower() == "journal")
	{
	  journal = new(std::nothrow) qtbook_journal(this, oid, i);

	  if(journal)
	    journal->duplicate(id, EDITABLE);
	}
      else if(type.toLower() == "magazine")
	{
	  magazine = new(std::nothrow) qtbook_magazine(this,
						       oid, i,
						       "magazine");

	  if(magazine)
	    magazine->duplicate(id, EDITABLE);
	}
      else if(type.toLower() == "photograph collection")
	{
	  photograph = new(std::nothrow) qtbook_photographcollection
	    (this, oid, i);

	  if(photograph)
	    photograph->duplicate(id, EDITABLE);
	}
      else if(type.toLower() == "video game")
	{
	  video_game = new(std::nothrow) qtbook_videogame(this,
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

  if(error)
    QMessageBox::critical(this, tr("BiblioteQ: Error"),
			  tr("Unable to determine the selected item's "
			     "type."));
}

/*
** -- updateSceneItem() --
*/

void qtbook::updateSceneItem(const QString &oid, const QImage &image)
{
  QList<QGraphicsItem *> items(ui.graphicsView->scene()->selectedItems());

  if(!items.isEmpty())
    {
      QStringList oids;
      QGraphicsPixmapItem *item = 0;

      while(!items.isEmpty())
	if((item = qgraphicsitem_cast<QGraphicsPixmapItem *> (items.
							      takeFirst())))
	  if(oid == item->data(0).toString())
	    {
	      QImage l_image(image);

	      if(!l_image.isNull())
		l_image = l_image.scaled
		  (126, 187, Qt::IgnoreAspectRatio,
		   Qt::SmoothTransformation);

	      QPixmap pixmap(QPixmap::fromImage(l_image));

	      if(!pixmap.isNull())
		item->setPixmap(pixmap);
	      else
		{
		  QImage l_image("icons.d/no_image.png");

		  if(!l_image.isNull())
		    l_image = l_image.scaled
		      (126, 187, Qt::IgnoreAspectRatio,
		       Qt::SmoothTransformation);

		  item->setPixmap(QPixmap::fromImage(l_image));
		}

	      break;
	    }

      items.clear();
    }
}

/*
** -- slotLanguageChanged() --
*/

void qtbook::slotLanguageChanged(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  if(action && action->isChecked())
    {
      s_locale = action->data().toString();
      qapp->removeTranslator(s_qtTranslator);
      qapp->removeTranslator(s_appTranslator);
      s_qtTranslator->load("qt_" + s_locale, "translations.d");
      qapp->installTranslator(s_qtTranslator);
      s_appTranslator->load("biblioteq_" + s_locale, "translations.d");
      qapp->installTranslator(s_appTranslator);
    }
}

/*
** -- changeEvent() --
*/

void qtbook::changeEvent(QEvent *event)
{
  if(event)
    switch(event->type())
      {
      case QEvent::LanguageChange:
	{
	  ab.retranslateUi(admin_diag);
	  al.retranslateUi(all_diag);
	  bb.retranslateUi(members_diag);
	  br.retranslateUi(branch_diag);
	  cq.retranslateUi(customquery_diag);
	  history.retranslateUi(history_diag);
	  er.retranslateUi(error_diag);
	  pass.retranslateUi(pass_diag);
	  ui.retranslateUi(this);
	  ui.table->resetTable
	    (db.userName(),
	     ui.typefilter->itemData(ui.typefilter->currentIndex()).toString(),
	     roles);
	  prepareFilter();
	  QMessageBox::information
	    (this,
	     tr("BiblioteQ: Information"),
	     tr("You have selected a new language. Please restart "
		"BiblioteQ."));
	  break;
	}
      default:
	break;
      }

  QMainWindow::changeEvent(event);
}

/*
** -- getSRUMaps() --
*/

QMap<QString, QHash<QString, QString> > qtbook::getSRUMaps(void) const
{
  return sruMaps;
}
