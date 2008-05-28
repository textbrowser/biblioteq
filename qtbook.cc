/*
** Copyright (c) 2006, 2007, 2008 Alexis Megas, Diana Megas
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
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
** -- Local Includes --
*/

#include "qtbook.h"

/*
** -- Local Variables --
*/

qtbook *qmain = NULL;

/*
** -- Global Variables --
*/

QApplication *qapp = NULL;

/*
** -- main() --
*/

int main(int argc, char *argv[])
{
  QApplication::setColorSpec(QApplication::NormalColor);

  /*
  ** Create the user interface.
  */

  if((qapp = new QApplication(argc, argv)) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  qapp->connect(qapp, SIGNAL(lastWindowClosed()), qapp, SLOT(quit(void)));

  if((qmain = new qtbook()) == NULL)
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
  if(qmain != NULL)
    {
      qmain->cleanup();

      if(qmain->ui.actionAutomaticallySaveSettingsOnExit->isChecked())
	qmain->slotSaveConfig();
    }

  if(qapp != NULL)
    qapp->quit();

  cout << "BiblioteQ has exited." << endl;
  exit(EXIT_SUCCESS);
}

/*
** -- cleanup () --
*/

void qtbook::cleanup(void)
{
  if(qapp != NULL && qmain != NULL && qmain->isVisible())
    qapp->setOverrideCursor(Qt::WaitCursor);

  if(db.isOpen())
    db.close();

  if(qapp != NULL && qmain != NULL && qmain->isVisible())
    qapp->restoreOverrideCursor();
}

/*
** -- quit() --
*/

void qtbook::quit(const char *msg, const char *file, const int line)
{
  if(msg != NULL && strlen(msg) > 0)
    cerr << msg << " in file " << file << ", line " << line << "." << endl;
  else
    cerr << "An unknown error occurred in file " << file << ", line "
	 << line << "." << endl;

  if(qmain != NULL)
    qmain->cleanup();

  exit(EXIT_SUCCESS);
}

/*
** -- qtbook() --
*/

qtbook::qtbook(void):QMainWindow()
{
  int h = 0;
  int w = 0;
  QMenu *menu1 = NULL;
  QMenu *menu2 = NULL;
  QMenu *menu3 = NULL;
  QMenu *menu4 = NULL;

  previousTypeFilter = "";

  if((branch_diag = new QDialog(this)) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((pass_diag = new QDialog(this)) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((all_diag = new QMainWindow(this)) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((admin_diag = new QMainWindow(this)) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((members_diag = new QMainWindow()) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((history_diag = new QMainWindow()) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((customquery_diag = new QMainWindow(this)) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((userinfo_diag = new QDialog(members_diag)) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((error_diag = new QMainWindow(this)) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((menu1 = new QMenu()) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((menu2 = new QMenu()) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((menu3 = new QMenu()) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((menu4 = new QMenu()) == NULL)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  connect(menu1->addAction("Insert &Book"),
	  SIGNAL(triggered(void)), this, SLOT(slotInsertBook(void)));
  // menu1->addAction("Insert &Cassette Tape");
  connect(menu1->addAction("Insert &CD"),
	  SIGNAL(triggered(void)), this, SLOT(slotInsertCD(void)));
  connect(menu1->addAction("Insert &DVD"),
	  SIGNAL(triggered(void)), this, SLOT(slotInsertDVD(void)));
  connect(menu1->addAction("Insert &Journal"),
	  SIGNAL(triggered(void)), this, SLOT(slotInsertJourn(void)));
  connect(menu1->addAction("Insert &Magazine"),
	  SIGNAL(triggered(void)), this, SLOT(slotInsertMag(void)));
  // menu1->addAction("Insert &Newspaper");
  connect(menu1->addAction("Insert &Video Game"),
	  SIGNAL(triggered(void)), this, SLOT(slotInsertVideoGame(void)));
  // menu1->addAction("Insert &VHS");
  // menu1->addAction("Insert &Vinyl Record");
  connect(menu2->addAction("&General Search"),
	  SIGNAL(triggered(void)), this, SLOT(slotSearch(void)));
  menu2->addSeparator();
  connect(menu2->addAction("&Book Search"),
	  SIGNAL(triggered(void)), this, SLOT(slotBookSearch(void)));
  connect(menu2->addAction("&CD Search"),
	  SIGNAL(triggered(void)), this, SLOT(slotCDSearch(void)));
  connect(menu2->addAction("&DVD Search"),
	  SIGNAL(triggered(void)), this, SLOT(slotDVDSearch(void)));
  connect(menu2->addAction("&Journal Search"),
	  SIGNAL(triggered(void)), this, SLOT(slotJournSearch(void)));
  connect(menu2->addAction("&Magazine Search"),
	  SIGNAL(triggered(void)), this, SLOT(slotMagSearch(void)));
  connect(menu2->addAction("&Video Game Search"),
	  SIGNAL(triggered(void)), this, SLOT(slotVideoGameSearch(void)));
  menu3->setTearOffEnabled(true);
  menu3->setWindowTitle("BiblioteQ: Columns Selection");
  connect(menu4->addAction("Reset &ID Number"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu4->addAction("Reset &Title"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu4->addAction("Reset &Release Date"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu4->addAction("Reset &Publisher"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu4->addAction("Reset &Category"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu4->addAction("Reset &Type"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu4->addAction("Reset &Price"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu4->addAction("Reset &Language"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu4->addAction("Reset &Monetary Units"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu4->addAction("Reset &Abstract"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu4->addAction("Reset &Copies"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu4->addAction("Reset &Location"),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  ui.setupUi(this);
  bb.setupUi(members_diag);
  userinfo.setupUi(userinfo_diag);
  history.setupUi(history_diag);
  br.setupUi(branch_diag);
  pass.setupUi(pass_diag);
  al.setupUi(all_diag);
  cq.setupUi(customquery_diag);
  er.setupUi(error_diag);
  ab.setupUi(admin_diag);
  pass_diag->setModal(true);
  userinfo_diag->setModal(true);
  history_diag->setWindowModality(Qt::WindowModal);
  branch_diag->setModal(true);
  connect(ui.table->horizontalHeader(), SIGNAL(sectionClicked(int)),
	  this, SLOT(slotResizeColumnsAfterSort(void)));
  connect(ui.table->horizontalHeader(), SIGNAL(sectionClicked(int)),
	  this, SLOT(slotUpdateIndicesAfterSort(int)));
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
  connect(ui.actionDeleteEntry, SIGNAL(triggered(void)), this,
	  SLOT(slotDelete(void)));
  connect(ui.refreshTool, SIGNAL(triggered(void)), this,
	  SLOT(slotRefresh(void)));
  connect(ui.actionRefreshTable, SIGNAL(triggered(void)), this,
	  SLOT(slotRefresh(void)));
  connect(ui.typefilter, SIGNAL(activated(int)), this,
	  SLOT(slotAutoPopOnFilter(void)));
  connect(ui.modifyTool, SIGNAL(triggered(void)), this,
	  SLOT(slotModify(void)));
  connect(ui.errorLogTool, SIGNAL(triggered(void)), this,
	  SLOT(slotShowErrorDialog(void)));
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
  connect(userinfo.okButton, SIGNAL(clicked(void)), this,
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
  connect(userinfo.nextTool, SIGNAL(clicked(void)), this,
	  SLOT(slotShowNext(void)));
  connect(userinfo.prevTool, SIGNAL(clicked(void)), this,
	  SLOT(slotShowPrev(void)));
  connect(history.nextTool, SIGNAL(clicked(void)), this,
	  SLOT(slotShowNext(void)));
  connect(history.prevTool, SIGNAL(clicked(void)), this,
	  SLOT(slotShowPrev(void)));
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
  connect(userinfo.cancelButton, SIGNAL(clicked(void)), this,
	  SLOT(slotCancelAddUser(void)));
  connect(userinfo_diag, SIGNAL(finished(int)), this,
	  SLOT(slotCancelAddUser(void)));
  connect(bb.checkoutButton, SIGNAL(clicked(void)), this,
	  SLOT(slotCheckout(void)));
  connect(bb.listButton, SIGNAL(clicked(void)), this,
	  SLOT(slotListReservedItems(void)));
  connect(bb.overdueButton, SIGNAL(clicked(void)), this,
	  SLOT(slotListOverdueItems(void)));
  connect(al.resetButton, SIGNAL(clicked(void)), this,
	  SLOT(slotSearch(void)));
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
  connect(bb.filter, SIGNAL(returnPressed(void)), this,
	  SLOT(slotPopulateMembersBrowser(void)));
  connect(ui.configTool, SIGNAL(triggered(void)), this,
	  SLOT(slotShowMenu(void)));
  connect(ui.printTool, SIGNAL(triggered(void)), this,
	  SLOT(slotPrintSelected(void)));
  connect(cq.close_pb, SIGNAL(clicked(void)), this,
	  SLOT(slotCloseCustomQueryDialog(void)));
  connect(cq.execute_pb, SIGNAL(clicked(void)), this,
	  SLOT(slotExecuteCustomQuery(void)));
  connect(pass.okButton, SIGNAL(clicked(void)), this,
	  SLOT(slotSavePassword(void)));
  connect(br.resetButton, SIGNAL(clicked(void)), this,
	  SLOT(slotResetLoginDialog(void)));
  connect(br.fileButton, SIGNAL(clicked(void)), this,
	  SLOT(slotSelectDatabaseFile(void)));
  connect(ui.actionConfigureAdministratorPrivileges,
	  SIGNAL(triggered(void)), this, SLOT(slotShowAdminDialog(void)));
  connect(ab.reloadButton, SIGNAL(clicked(void)), this,
	  SLOT(slotRefreshAdminList(void)));
  connect(ab.addButton, SIGNAL(clicked(void)), this, SLOT(slotAddAdmin(void)));
  connect(ab.deleteButton, SIGNAL(clicked(void)), this,
	  SLOT(slotDeleteAdmin(void)));
  connect(ab.saveButton, SIGNAL(clicked(void)), this,
	  SLOT(slotSaveAdministrators(void)));
  bb.table->verticalHeader()->setResizeMode(QHeaderView::Fixed);
  er.table->verticalHeader()->setResizeMode(QHeaderView::Fixed);
  history.table->verticalHeader()->setResizeMode(QHeaderView::Fixed);
  w = qapp->desktop()->width();
  h = qapp->desktop()->height();
  setGeometry(qapp->desktop()->x() + (w - minimumSize().width()) / 2,
	      qapp->desktop()->y() + (h - minimumSize().height()) / 2,
	      minimumSize().width(), minimumSize().height());
  ui.createTool->setMenu(menu1);
  ui.searchTool->setMenu(menu2);
  ui.configTool->setMenu(menu3);
  al.resetButton->setMenu(menu4);
  ui.actionRequests->setEnabled(false);
  ui.actionReservationHistory->setEnabled(false);
  ui.actionChangePassword->setEnabled(false);
  ui.printTool->setEnabled(false);
  ui.deleteTool->setEnabled(false);
  ui.actionDeleteEntry->setEnabled(false);
  ui.createTool->setEnabled(false);
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
  ui.table->resetTable("All", roles);
  ui.summary->setVisible(false);
  ui.actionRememberSQLiteFilename->setEnabled(false);
  ui.actionConfigureAdministratorPrivileges->setEnabled(false);
  previousTypeFilter = "All";
  prepareFilter();
  addConfigOptions(previousTypeFilter);
  setUpdatesEnabled(true);
  userinfo.telephoneNumber->setInputMask("999-999-9999");
  userinfo.zip->setInputMask("99999");
  ui.splitter->setStretchFactor(ui.splitter->indexOf(ui.itemSummary),  0);
  ui.splitter->setStretchFactor(ui.splitter->indexOf(ui.table),  1);

  /*
  ** Highlight userinfo widgets.
  */

  misc_functions::highlightWidget(userinfo.firstName, QColor(255, 248, 220));
  misc_functions::highlightWidget(userinfo.lastName, QColor(255, 248, 220));
  misc_functions::highlightWidget(userinfo.street, QColor(255, 248, 220));
  misc_functions::highlightWidget(userinfo.city, QColor(255, 248, 220));
  misc_functions::highlightWidget(userinfo.zip, QColor(255, 248, 220));
}

/*
** -- addConfigOptions() --
*/

void qtbook::addConfigOptions(const QString &typefilter)
{
  int i = 0;
  QAction *action = NULL;

  /*
  ** Delete existing actions, if any.
  */

  while(!ui.configTool->menu()->actions().isEmpty())
    delete ui.configTool->menu()->actions().takeFirst();

  for(i = 0; i < ui.table->columnCount(); i++)
    {
      if(typefilter != "All" && typefilter != "All Overdue" &&
	 typefilter != "All Requested" && typefilter != "All Reserved")
	{
	  if(ui.table->horizontalHeaderItem(i)->text() == "OID" ||
	     ui.table->horizontalHeaderItem(i)->text() == "Type")
	    continue;
	}
      else if(ui.table->horizontalHeaderItem(i)->text() == "OID" ||
	      ui.table->horizontalHeaderItem(i)->text() == "REQUESTOID")
	continue;

      if((action = new QAction(ui.table->horizontalHeaderItem(i)->text(),
			       ui.configTool)) == NULL)
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
  QString typefilter = ui.typefilter->currentText();

  for(i = 0; i < ui.configTool->menu()->actions().size(); i++)
    {
      ui.table->setColumnHidden
	(i, !ui.configTool->menu()->actions().at(i)->isChecked());
      ui.table->recordColumnHidden
	(typefilter, i, !ui.configTool->menu()->actions().at(i)->isChecked());
    }
}

/*
** -- getRoles() --
*/

QString qtbook::getRoles(void)
{
  return roles;
}

/*
** -- adminSetup() --
*/

void qtbook::adminSetup(void)
{
  bb.addButton->setEnabled(true);
  bb.deleteButton->setEnabled(true);
  bb.modifyButton->setEnabled(true);
  bb.historyButton->setEnabled(true);
  bb.listButton->setEnabled(true);
  bb.printButton->setEnabled(true);
  bb.checkoutButton->setEnabled(true);
  bb.overdueButton->setEnabled(true);
  ui.detailsTool->setEnabled(true);
  ui.actionViewDetails->setEnabled(true);

  if(status_bar_label != NULL)
    {
      status_bar_label->setPixmap(QPixmap("icons.d/16x16/unlock.png"));

      if(roles.contains("administrator"))
	status_bar_label->setToolTip("Administrator Mode");
      else if(roles == "circulation")
	status_bar_label->setToolTip("Circulation Mode");
      else if(roles == "librarian")
	status_bar_label->setToolTip("Librarian Mode");
      else if(roles == "membership")
	status_bar_label->setToolTip("Membership Mode");
      else
	status_bar_label->setToolTip("Privileged Mode");
    }

  if(roles.contains("administrator") || roles.contains("librarian"))
    {
      ui.table->disconnect(SIGNAL(itemDoubleClicked(QTableWidgetItem *)));
      connect(ui.table, SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
	      this, SLOT(slotModify(void)));
      updateItemWindows();
    }

  if(roles.contains("administrator") || roles.contains("librarian"))
    ui.deleteTool->setEnabled(true);

  if(roles.contains("administrator") || roles.contains("librarian"))
    {
      ui.actionDeleteEntry->setEnabled(true);
      ui.createTool->setEnabled(true);
      ui.modifyTool->setEnabled(true);
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

  if(selectedBranch["database_type"] != "sqlite")
    ui.actionConfigureAdministratorPrivileges->setEnabled
      (roles.contains("administrator"));

  if(selectedBranch["database_type"] != "sqlite")
    {
      if(!(roles.contains("administrator") || roles.contains("circulation")))
	ui.actionRequests->setToolTip("Item Requests");
      else
	ui.actionRequests->setToolTip("Cancel Selected Request(s)");
    }
  else
    ui.actionRequests->setToolTip("Item Requests");

  /*
  ** Hide certain fields in the Member's Browser.
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
      bb.deleteButton->setEnabled(false);
      bb.modifyButton->setEnabled(false);
    }
  else
    {
      connect(bb.table, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this,
	      SLOT(slotModifyBorrower(void)));
      bb.addButton->setEnabled(true);
      bb.deleteButton->setEnabled(true);
      bb.modifyButton->setEnabled(true);
    }
}

/*
** -- showMain() --
*/

void qtbook::showMain(void)
{
  if((connected_bar_label = new QLabel()) != NULL)
    {
      connected_bar_label->setPixmap
	(QPixmap("icons.d/16x16/disconnected.png"));
      connected_bar_label->setToolTip("Disconnected");
      qmain->statusBar()->addPermanentWidget(connected_bar_label);
    }

  if((status_bar_label = new QLabel()) != NULL)
    {
      status_bar_label->setPixmap(QPixmap("icons.d/16x16/lock.png"));
      status_bar_label->setToolTip("Standard User Mode");
      qmain->statusBar()->addPermanentWidget(status_bar_label);
    }

  if((error_bar_label = new QLabel()) != NULL)
    {
      error_bar_label->setPixmap(QPixmap("icons.d/16x16/ok.png"));
      error_bar_label->setToolTip("Empty Error Log");
      qmain->statusBar()->addPermanentWidget(error_bar_label);
    }

  show();

  /*
  ** Configure the global settings file.
  */

  readGlobalSetup();

  /*
  ** Initial update.
  */

  initialUpdate();
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
  qtbook::quit();
}

/*
** -- slotAbout() --
*/

void qtbook::slotAbout(void)
{
  QMessageBox mb(this);

  mb.setFont(qapp->font());
  mb.setWindowTitle("BiblioteQ: About");
  mb.setTextFormat(Qt::RichText);
  mb.setText("<html>BiblioteQ Version 6.03.<br>"
	     "Copyright (c) 2006, 2007, 2008 "
	     "Diana Megas.<br>"
	     "Icons copyright (c) Everaldo.<br><br>"
	     "Please visit <a href=\"http://biblioteq.sourceforge.net\">"
	     "http://biblioteq.sourceforge.net</a> for "
	     "additional information.<br><br>"
	     "The program is provided AS IS with NO WARRANTY OF ANY KIND, "
	     "INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND "
	     "FITNESS FOR A PARTICULAR PURPOSE.</html>");
  mb.setStandardButtons(QMessageBox::Ok);
  mb.setIconPixmap(QPixmap("./icons.d/book.gif"));
  mb.exec();
}

/*
** -- slotSearch() --
*/

void qtbook::slotSearch(void)
{
  /*
  ** Hide certain fields if we're a regular user.
  */

  misc_functions::hideAdminFields(all_diag, roles);
  al.idnumber->clear();
  al.title->clear();
  al.publisher->clear();
  al.publication_date->setDate(QDate::fromString("01/01/7999",
						 "MM/dd/yyyy"));
  al.price->setMinimum(0.00);
  al.price->setValue(0.00);
  al.quantity->setMinimum(0);
  al.quantity->setValue(0);
  al.description->clear();

  /*
  ** Add "any".
  */

  if(al.category->findText("Any") == -1)
    al.category->insertItem(0, "Any");

  if(al.language->findText("Any") == -1)
    al.language->insertItem(0, "Any");

  if(al.monetary_units->findText("Any") == -1)
    al.monetary_units->insertItem(0, "Any");

  if(al.location->findText("Any") == -1)
    al.location->insertItem(0, "Any");

  al.location->setCurrentIndex(0);
  al.language->setCurrentIndex(0);
  al.monetary_units->setCurrentIndex(0);
  al.category->setCurrentIndex(0);
  al.idnumber->setFocus();

  if(!all_diag->isVisible())
    {
      all_diag->updateGeometry();
      all_diag->resize(all_diag->sizeHint());
    }

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
  int i = 0;
  bool error = false;
  QString oid = "";
  QString type = "";
  qtbook_cd *cd = NULL;
  qtbook_dvd *dvd = NULL;
  QModelIndex index;
  qtbook_book *book = NULL;
  qtbook_journal *journal = NULL;
  qtbook_magazine *magazine = NULL;
  qtbook_videogame *video_game = NULL;
  QModelIndexList list = ui.table->selectionModel()->selectedRows();

  if(list.isEmpty())
    {
      QMessageBox::critical(this, "BiblioteQ: User Error",
			    "Please select at least one item to modify.");
      return;
    }
  else if(list.size() >= 5)
    if(QMessageBox::question(this, "BiblioteQ: Question",
			     "Are you sure that you wish to modify the " +
			     QString::number(list.size()) + " selected items?",
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
      oid = misc_functions::getColumnString(ui.table, i, "OID");
      type = misc_functions::getColumnString(ui.table, i, "Type");

      if(type.toLower() == "cd")
	{
	  if(cds.contains(oid))
	    cd = cds.value(oid);
	  else
	    {
	      if((cd = new qtbook_cd(this, cd_categories, languages,
				     monetary_units, cd_locations,
				     cd_formats, oid, i)) != NULL)
		cds.insert(oid, cd);
	    }

	  if(cd != NULL)
	    cd->modify(EDITABLE);
	}
      else if(type.toLower() == "dvd")
	{
	  if(dvds.contains(oid))
	    dvd = dvds.value(oid);
	  else
	    {
	      if((dvd = new qtbook_dvd(this, dvd_categories, languages,
					 monetary_units, dvd_locations,
				       dvd_ratings, dvd_aspectratios,
				       dvd_regions, oid, i)) != NULL)
		dvds.insert(oid, dvd);
	    }

	  if(dvd != NULL)
	    dvd->modify(EDITABLE);
	}
      else if(type.toLower() == "book")
	{
	  if(books.contains(oid))
	    book = books.value(oid);
	  else
	    {
	      if((book = new qtbook_book(this, book_categories, languages,
					 monetary_units, book_locations,
					 oid, i)) != NULL)
		books.insert(oid, book);
	    }

	  if(book != NULL)
	    book->modify(EDITABLE);
	}
      else if(type.toLower() == "journal")
	{
	  if(journals.contains(oid))
	    journal = journals.value(oid);
	  else
	    {
	      if((journal = new qtbook_journal(this, journal_categories,
					       languages, monetary_units,
					       journal_locations,
					       oid, i)) != NULL)
		journals.insert(oid, journal);
	    }

	  if(journal != NULL)
	    journal->modify(EDITABLE);
	}
      else if(type.toLower() == "magazine")
	{
	  if(magazines.contains(oid))
	    magazine = magazines.value(oid);
	  else
	    {
	      if((magazine = new qtbook_magazine(this, magazine_categories,
						 languages, monetary_units,
						 magazine_locations,
						 oid, i)) != NULL)
		magazines.insert(oid, magazine);
	      }

	  if(magazine != NULL)
	    magazine->modify(EDITABLE);
	}
      else if(type.toLower() == "video game")
	{
	  if(video_games.contains(oid))
	    video_game = video_games.value(oid);
	  else
	    {
	      if((video_game = new qtbook_videogame(this, vg_ratings,
						    vg_platforms,
						    vg_genres,
						    languages,
						    monetary_units,
						    vg_locations,
						    oid, i)) != NULL)
		video_games.insert(oid, video_game);
	    }

	  if(video_game != NULL)
	    video_game->modify(EDITABLE);
	}
      else
	{
	  error = true;
	  break;
	}
    }

  list.clear();

  if(error)
    QMessageBox::critical(this, "BiblioteQ: User Error",
			  "Unable to determine the selected item's "
			  "type.");
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
  qtbook_cd *cd = NULL;
  qtbook_dvd *dvd = NULL;
  QModelIndex index;
  qtbook_book *book = NULL;
  qtbook_journal *journal = NULL;
  qtbook_magazine *magazine = NULL;
  QModelIndexList list = ui.table->selectionModel()->selectedRows();
  qtbook_videogame *video_game = NULL;

  if(list.isEmpty())
    {
      QMessageBox::critical(this, "BiblioteQ: User Error",
			    "Please select at least one item to view.");
      return;
    }
  else if(list.size() >= 5)
    if(QMessageBox::question(this, "BiblioteQ: Question",
			     "Are you sure that you wish to view the " +
			     QString::number(list.size()) + " selected items?",
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
      oid = misc_functions::getColumnString(ui.table,i, "OID");
      type = misc_functions::getColumnString(ui.table, i, "Type");

      if(type.toLower() == "cd")
	{
	  if(cds.contains(oid))
	    cd = cds.value(oid);
	  else
	    {
	      if((cd = new qtbook_cd(this, cd_categories, languages,
				     monetary_units, cd_locations,
				     cd_formats, oid, i)) != NULL)
		cds.insert(oid, cd);
	    }

	  if(cd != NULL)
	    cd->modify(VIEW_ONLY);
	}
      else if(type.toLower() == "dvd")
	{
	  if(dvds.contains(oid))
	    dvd = dvds.value(oid);
	  else
	    {
	      if((dvd = new qtbook_dvd(this, dvd_categories, languages,
				       monetary_units, dvd_locations,
				       dvd_ratings, dvd_aspectratios,
				       dvd_regions, oid, i)) != NULL)
		dvds.insert(oid, dvd);
	    }

	  if(dvd != NULL)
	    dvd->modify(VIEW_ONLY);
	}
      else if(type.toLower() == "book")
	{
	  if(books.contains(oid))
	    book = books.value(oid);
	  else
	    {
	      if((book = new qtbook_book(this, book_categories, languages,
					 monetary_units, book_locations,
					 oid, i)) != NULL)
		books.insert(oid, book);
	    }

	  if(book != NULL)
	    book->modify(VIEW_ONLY);
	}
      else if(type.toLower() == "journal")
	{
	  if(journals.contains(oid))
	    journal = journals.value(oid);
	  else
	    {
	      if((journal = new qtbook_journal(this, journal_categories,
					       languages, monetary_units,
					       journal_locations,
					       oid, i)) != NULL)
		journals.insert(oid, journal);
	    }

	  if(journal != NULL)
	    journal->modify(VIEW_ONLY);
	}
      else if(type.toLower() == "magazine")
	{
	  if(magazines.contains(oid))
	    magazine = magazines.value(oid);
	  else
	    {
	      if((magazine = new qtbook_magazine(this, magazine_categories,
						 languages, monetary_units,
						 magazine_locations,
						 oid, i)) != NULL)
		magazines.insert(oid, magazine);
	    }

	  if(magazine != NULL)
	    magazine->modify(VIEW_ONLY);
	}
      else if(type.toLower() == "video game")
	{
	  if(video_games.contains(oid))
	    video_game = video_games.value(oid);
	  else
	    {
	      if((video_game = new qtbook_videogame(this, vg_ratings,
						    vg_platforms,
						    vg_genres,
						    languages,
						    monetary_units,
						    vg_locations,
						    oid, i)) != NULL)
		video_games.insert(oid, video_game);
	    }

	  if(video_game != NULL)
	    video_game->modify(VIEW_ONLY);
	}
      else
	{
	  error = true;
	  break;
	}
    }

  list.clear();

  if(error)
    QMessageBox::critical(this, "BiblioteQ: User Error",
			  "Unable to determine the selected item's "
			  "type.");
}

/*
** -- slotDelete() --
*/

void qtbook::slotDelete(void)
{
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

  if(list.isEmpty())
    {
      QMessageBox::critical(this, "BiblioteQ: User Error",
			    "Please select an item to delete.");
      return;
    }

  col = misc_functions::getColumnNumber(ui.table, "OID");

  foreach(index, list)
    {
      i = index.row();

      if(ui.table->item(i, col) == NULL)
	continue;

      oid = misc_functions::getColumnString(ui.table, i, "OID");
      itemType = misc_functions::getColumnString(ui.table, i, "Type");

      if(oid.isEmpty() || itemType.isEmpty())
	{
	  addError(QString("User Error"),
		   QString("The main table does not contain enough "
			   "information for item deletion."),
		   QString("The main table does not contain enough "
			   "information for item deletion."),
		   __FILE__, __LINE__);
	  QMessageBox::critical(this, "BiblioteQ: Error",
				"The main table does not contain enough "
				"information for item deletion.");
	  list.clear();
	  return;
	}

      qapp->setOverrideCursor(Qt::WaitCursor);
      isCheckedOut = misc_functions::isCheckedOut(db, oid, itemType, errorstr);
      qapp->restoreOverrideCursor();

      if(!errorstr.isEmpty())
	{
	  addError(QString("Database Error"),
		   QString("Unable to determine if the item has been "
			   "reserved."),
		   errorstr, __FILE__, __LINE__);
	  QMessageBox::critical(this, "BiblioteQ: Error",
				"Unable to determine if the item has "
				"been reserved.");
	  list.clear();
	  return;
	}

      if(isCheckedOut)
	{
	  QMessageBox::critical(this, "BiblioteQ: Error",
				"Reserved items may not be deleted.");
	  list.clear();
	  return;
	}

      qapp->setOverrideCursor(Qt::WaitCursor);
      isRequested = misc_functions::isRequested(db, oid, itemType, errorstr);
      qapp->restoreOverrideCursor();

      if(!errorstr.isEmpty())
	{
	  addError(QString("Database Error"),
		   QString("Unable to determine if the item has been "
			   "requested."),
		   errorstr, __FILE__, __LINE__);
	  QMessageBox::critical(this, "BiblioteQ: Error",
				"Unable to determine if the item has "
				"been requested.");
	  list.clear();
	  return;
	}

      if(isRequested)
	{
	  QMessageBox::critical(this, "BiblioteQ: Error",
				"Requested items may not be deleted.");
	  list.clear();
	  return;
	}
    }

  if(list.size() > 0)
    if(QMessageBox::question(this, "BiblioteQ: Question",
			     "Are you sure that you wish to permanently "
			     "delete the selected item(s)?",
			     QMessageBox::Yes | QMessageBox::No,
			     QMessageBox::No) == QMessageBox::No)
      {
	list.clear();
	return;
      }

  progress.setModal(true);
  progress.setWindowTitle("BiblioteQ: Progress Dialog");
  progress.setLabelText("Deleting the selected item(s)...");
  progress.setMaximum(list.size());
  progress.show();
  progress.update();

  foreach(index, list)
    {
      i = index.row();

      if(ui.table->item(i, col) == NULL)
	continue;

      str = ui.table->item(i, col)->text();
      itemType = misc_functions::getColumnString(ui.table, i, "Type");
      itemType = itemType.toLower().remove(" ");

      if(isItemBusy(str, itemType))
	continue;

      if(itemType == "journal")
	query.prepare("DELETE FROM magazine WHERE myoid = ?");
      else
	query.prepare(QString("DELETE FROM %1 WHERE myoid = ?").arg(itemType));

      query.bindValue(0, str);

      if(!query.exec())
	{
	  error = true;
	  addError(QString("Database Error"),
		   QString("Unable to delete the item."),
		   query.lastError().text(), __FILE__, __LINE__);
	}
      else
	{
	  deleteItem(str, itemType);
	  numdeleted += 1;
	}

      progress.setValue(list.size() + 1);
      progress.update();
      qapp->processEvents();
    }

  progress.hide();

  /*
  ** Provide some fancy messages.
  */

  if(error)
    QMessageBox::critical(this, "BiblioteQ: Database Error",
			  "Unable to delete all or some of the selected "
			  "items.");

  if(numdeleted > 0)
    slotRefresh();

  list.clear();
}

/*
** -- closeEvent() --
*/

void qtbook::closeEvent(QCloseEvent *e)
{
  (void) e;
  slotExit();
}

/*
** -- slotRefresh() --
*/

void qtbook::slotRefresh(void)
{
  QString str = "";

  if(ui.typefilter->currentText() == "All Overdue" && roles.isEmpty())
    str = br.userid->text();
  else if(ui.typefilter->currentText() == "All Requested" && roles.isEmpty())
    str = br.userid->text();
  else if(ui.typefilter->currentText() == "All Reserved" && roles.isEmpty())
    str = br.userid->text();
  else if(ui.typefilter->currentText() == "All Reserved")
    str = "%";

  (void) populateTable(POPULATE_ALL, ui.typefilter->currentText(), str);
}

/*
** -- populateTable() --
*/

int qtbook::populateTable(const int search_type, const QString &typefilter,
			  const QString &searchstrArg)
{
  int i = -1;
  int j = 0;
  QDate now = QDate::currentDate();
  QString str = "";
  QString type = "";
  QString itemType = "";
  QString searchstr = "";
  QSqlQuery query(db);
  QStringList types;
  QStringList tmplist;
  QProgressDialog progress(this);
  QTableWidgetItem *item = NULL;

  if(!roles.isEmpty() && typefilter == "All Requested")
    ui.actionRequests->setEnabled(true);
  else if(roles.isEmpty() && (typefilter == "All" ||
			      typefilter == "Books" ||
			      typefilter == "DVDs" ||
			      typefilter == "Journals" ||
			      typefilter == "Magazines" ||
			      typefilter == "Music CDs" ||
			      typefilter == "Video Games"))
    ui.actionRequests->setEnabled(true);
  else
    ui.actionRequests->setEnabled(false);

  /*
  ** The order of the fields in the select statements should match
  ** the original column order.
  */

  switch(search_type)
    {
    case CUSTOM_QUERY:
      {
	searchstr = searchstrArg;
	break;
      }
    case POPULATE_ALL:
      {
	if(typefilter == "All")
	  {
	    searchstr = "SELECT DISTINCT book.title, "
	      "book.id, "
	      "book.publisher, book.pdate, "
	      "book.category, "
	      "book.language, "
	      "book.price, book.monetary_units, "
	      "book.quantity, "
	      "book.location, "
	      "book.quantity - COUNT(item_borrower_vw.item_oid) "
	      "AS availability, "
	      "book.type, "
	      "book.myoid "
	      "FROM "
	      "book LEFT JOIN item_borrower_vw ON "
	      "book.myoid = item_borrower_vw.item_oid "
	      "AND item_borrower_vw.type = 'Book' "
	      "GROUP BY book.title, "
	      "book.id, "
	      "book.publisher, book.pdate, "
	      "book.category, "
	      "book.language, "
	      "book.price, book.monetary_units, "
	      "book.quantity, "
	      "book.location, "
	      "book.type, "
	      "book.myoid "
	      "UNION "
	      "SELECT DISTINCT cd.title, "
	      "cd.id, "
	      "cd.recording_label, cd.rdate, "
	      "cd.category, "
	      "cd.language, "
	      "cd.price, cd.monetary_units, "
	      "cd.quantity, "
	      "cd.location, "
	      "cd.quantity - COUNT(item_borrower_vw.item_oid) "
	      "AS availability, "
	      "cd.type, "
	      "cd.myoid "
	      "FROM "
	      "cd LEFT JOIN item_borrower_vw ON "
	      "cd.myoid = item_borrower_vw.item_oid "
	      "AND item_borrower_vw.type = 'CD' "
	      "GROUP BY cd.title, "
	      "cd.id, "
	      "cd.recording_label, cd.rdate, "
	      "cd.category, "
	      "cd.language, "
	      "cd.price, cd.monetary_units, "
	      "cd.quantity, "
	      "cd.location, "
	      "cd.type, "
	      "cd.myoid "
	      "UNION "
	      "SELECT DISTINCT dvd.title, "
	      "dvd.id, "
	      "dvd.studio, dvd.rdate, "
	      "dvd.category, "
	      "dvd.language, "
	      "dvd.price, dvd.monetary_units, "
	      "dvd.quantity, "
	      "dvd.location, "
	      "dvd.quantity - COUNT(item_borrower_vw.item_oid) AS "
	      "availability, "
	      "dvd.type, "
	      "dvd.myoid "
	      "FROM "
	      "dvd LEFT JOIN item_borrower_vw ON "
	      "dvd.myoid = item_borrower_vw.item_oid "
	      "AND item_borrower_vw.type = 'DVD' "
	      "GROUP BY dvd.title, "
	      "dvd.id, "
	      "dvd.studio, dvd.rdate, "
	      "dvd.category, "
	      "dvd.language, "
	      "dvd.price, dvd.monetary_units, "
	      "dvd.quantity, "
	      "dvd.location, "
	      "dvd.type, "
	      "dvd.myoid "
	      "UNION "
	      "SELECT DISTINCT magazine.title, "
	      "magazine.id, "
	      "magazine.publisher, magazine.pdate, "
	      "magazine.category, "
	      "magazine.language, "
	      "magazine.price, magazine.monetary_units, "
	      "magazine.quantity, "
	      "magazine.location, "
	      "magazine.quantity - COUNT(item_borrower_vw.item_oid) AS "
	      "availability, "
	      "magazine.type, "
	      "magazine.myoid "
	      "FROM "
	      "magazine LEFT JOIN item_borrower_vw ON "
	      "magazine.myoid = item_borrower_vw.item_oid "
	      "AND item_borrower_vw.type = magazine.type "
	      "GROUP BY magazine.title, "
	      "magazine.id, "
	      "magazine.publisher, magazine.pdate, "
	      "magazine.category, "
	      "magazine.language, "
	      "magazine.price, magazine.monetary_units, "
	      "magazine.quantity, "
	      "magazine.location, "
	      "magazine.type, "
	      "magazine.myoid "
	      "UNION "
	      "SELECT DISTINCT videogame.title, "
	      "videogame.id, "
	      "videogame.publisher, videogame.rdate, "
	      "videogame.genre, "
	      "videogame.language, "
	      "videogame.price, videogame.monetary_units, "
	      "videogame.quantity, "
	      "videogame.location, "
	      "videogame.quantity - COUNT(item_borrower_vw.item_oid) "
	      "AS "
	      "availability, "
	      "videogame.type, "
	      "videogame.myoid "
	      "FROM "
	      "videogame LEFT JOIN item_borrower_vw ON "
	      "videogame.myoid = item_borrower_vw.item_oid "
	      "AND item_borrower_vw.type = 'Video Game' "
	      "GROUP BY videogame.title, "
	      "videogame.id, "
	      "videogame.publisher, videogame.rdate, "
	      "videogame.genre, "
	      "videogame.language, "
	      "videogame.price, videogame.monetary_units, "
	      "videogame.quantity, "
	      "videogame.location, "
	      "videogame.type, "
	      "videogame.myoid "
	      "ORDER BY 1";
	  }
	else if(typefilter == "All Overdue")
	  {
	    searchstr = "";

	    if(roles.isEmpty())
	      {
		searchstr.append("SELECT DISTINCT "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "book.title, "
				 "book.id, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.quantity - "
				 "COUNT(item_borrower_vw.item_oid) "
				 "AS availability, "
				 "book.type, "
				 "book.myoid "
				 "FROM "
				 "book LEFT JOIN item_borrower_vw ON "
				 "book.myoid = item_borrower_vw.item_oid "
				 "AND item_borrower_vw.type = 'Book' "
				 "WHERE "
				 "item_borrower_vw.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append("item_borrower_vw.duedate < '");
		searchstr.append(now.toString("MM/dd/yyyy"));
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "book.title, "
				 "book.id, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.type, "
				 "book.myoid ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "cd.title, "
				 "cd.id, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.quantity - "
				 "COUNT(item_borrower_vw.item_oid) "
				 "AS availability, "
				 "cd.type, "
				 "cd.myoid "
				 "FROM "
				 "cd LEFT JOIN item_borrower_vw ON "
				 "cd.myoid = item_borrower_vw.item_oid "
				 "AND item_borrower_vw.type = 'CD' "
				 "WHERE "
				 "item_borrower_vw.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append("item_borrower_vw.duedate < '");
		searchstr.append(now.toString("MM/dd/yyyy"));
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "cd.title, "
				 "cd.id, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.type, "
				 "cd.myoid ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "dvd.title, "
				 "dvd.id, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.quantity - "
				 "COUNT(item_borrower_vw.item_oid) "
				 "AS availability, "
				 "dvd.type, "
				 "dvd.myoid "
				 "FROM "
				 "dvd LEFT JOIN item_borrower_vw ON "
				 "dvd.myoid = item_borrower_vw.item_oid "
				 "AND item_borrower_vw.type = 'DVD' "
				 "WHERE "
				 "item_borrower_vw.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append("item_borrower_vw.duedate < '");
		searchstr.append(now.toString("MM/dd/yyyy"));
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "dvd.title, "
				 "dvd.id, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.type, "
				 "dvd.myoid ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.quantity - "
				 "COUNT(item_borrower_vw.item_oid) "
				 "AS availability, "
				 "magazine.type, "
				 "magazine.myoid "
				 "FROM "
				 "magazine LEFT JOIN item_borrower_vw ON "
				 "magazine.myoid = item_borrower_vw.item_oid "
				 "AND item_borrower_vw.type = magazine.type "
				 "WHERE "
				 "item_borrower_vw.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append("item_borrower_vw.duedate < '");
		searchstr.append(now.toString("MM/dd/yyyy"));
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.type, "
				 "magazine.myoid ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "videogame.title, "
				 "videogame.id, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.quantity - "
				 "COUNT(item_borrower_vw.item_oid) "
				 "AS availability, "
				 "videogame.type, "
				 "videogame.myoid "
				 "FROM "
				 "videogame LEFT JOIN item_borrower_vw ON "
				 "videogame.myoid = "
				 "item_borrower_vw.item_oid "
				 "AND item_borrower_vw.type = 'Video Game' "
				 "WHERE "
				 "item_borrower_vw.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append("item_borrower_vw.duedate < '");
		searchstr.append(now.toString("MM/dd/yyyy"));
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "videogame.title, "
				 "videogame.id, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.type, "
				 "videogame.myoid ");
		searchstr.append("ORDER BY 1");
	      }
	    else
	      {
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "book.title, "
				 "book.id, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "book.type, "
				 "book.myoid "
				 "FROM "
				 "member, "
				 "book LEFT JOIN item_borrower ON "
				 "book.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = 'Book' "
				 "WHERE "
				 "member.memberid LIKE '%");
		searchstr.append(searchstrArg);
		searchstr.append("%' AND ");
		searchstr.append("item_borrower.duedate < '");
		searchstr.append(now.toString("MM/dd/yyyy"));
		searchstr.append("' AND ");
		searchstr.append("item_borrower.memberid = member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "book.title, "
				 "book.id, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.type, "
				 "book.myoid ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "cd.title, "
				 "cd.id, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "cd.type, "
				 "cd.myoid "
				 "FROM "
				 "member, "
				 "cd LEFT JOIN item_borrower ON "
				 "cd.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = 'CD' "
				 "WHERE "
				 "member.memberid LIKE '%");
		searchstr.append(searchstrArg);
		searchstr.append("%' AND ");
		searchstr.append("item_borrower.duedate < '");
		searchstr.append(now.toString("MM/dd/yyyy"));
		searchstr.append("' AND ");
		searchstr.append("item_borrower.memberid = member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "cd.title, "
				 "cd.id, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.type, "
				 "cd.myoid ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "dvd.title, "
				 "dvd.id, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "dvd.type, "
				 "dvd.myoid "
				 "FROM "
				 "member, "
				 "dvd LEFT JOIN item_borrower ON "
				 "dvd.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = 'DVD' "
				 "WHERE "
				 "member.memberid LIKE '%");
		searchstr.append(searchstrArg);
		searchstr.append("%' AND ");
		searchstr.append("item_borrower.duedate < '");
		searchstr.append(now.toString("MM/dd/yyyy"));
		searchstr.append("' AND ");
		searchstr.append("item_borrower.memberid = member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "dvd.title, "
				 "dvd.id, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.type, "
				 "dvd.myoid ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "magazine.type, "
				 "magazine.myoid "
				 "FROM "
				 "member, "
				 "magazine LEFT JOIN item_borrower ON "
				 "magazine.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = magazine.type "
				 "WHERE "
				 "member.memberid LIKE '%");
		searchstr.append(searchstrArg);
		searchstr.append("%' AND ");
		searchstr.append("item_borrower.duedate < '");
		searchstr.append(now.toString("MM/dd/yyyy"));
		searchstr.append("' AND ");
		searchstr.append("item_borrower.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.type, "
				 "magazine.myoid ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "videogame.title, "
				 "videogame.id, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "videogame.type, "
				 "videogame.myoid "
				 "FROM "
				 "member, "
				 "videogame LEFT JOIN item_borrower ON "
				 "videogame.myoid = "
				 "item_borrower.item_oid "
				 "AND item_borrower.type = 'Video Game' "
				 "WHERE "
				 "member.memberid LIKE '%");
		searchstr.append(searchstrArg);
		searchstr.append("%' AND ");
		searchstr.append("item_borrower.duedate < '");
		searchstr.append(now.toString("MM/dd/yyyy"));
		searchstr.append("' AND ");
		searchstr.append("item_borrower.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "videogame.title, "
				 "videogame.id, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.type, "
				 "videogame.myoid ");
		searchstr.append("ORDER BY 1");
	      }
	  }
	else if(typefilter == "All Requested")
	  {
	    searchstr = "";

	    if(roles.isEmpty())
	      {
		searchstr.append("SELECT DISTINCT "
				 "item_request.requestdate, "
				 "book.title, "
				 "book.id, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.type, "
				 "book.myoid, "
				 "item_request.myoid "
				 "FROM "
				 "book LEFT JOIN item_request ON "
				 "book.myoid = item_request.item_oid "
				 "AND item_request.type = 'Book' "
				 "WHERE "
				 "item_request.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_request.requestdate, "
				 "book.title, "
				 "book.id, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.type, "
				 "book.myoid, "
				 "item_request.myoid ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_request.requestdate, "
				 "cd.title, "
				 "cd.id, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.type, "
				 "cd.myoid, "
				 "item_request.myoid "
				 "FROM "
				 "cd LEFT JOIN item_request ON "
				 "cd.myoid = item_request.item_oid "
				 "AND item_request.type = 'CD' "
				 "WHERE "
				 "item_request.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_request.requestdate, "
				 "cd.title, "
				 "cd.id, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.type, "
				 "cd.myoid, "
				 "item_request.myoid ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_request.requestdate, "
				 "dvd.title, "
				 "dvd.id, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.type, "
				 "dvd.myoid, "
				 "item_request.myoid "
				 "FROM "
				 "dvd LEFT JOIN item_request ON "
				 "dvd.myoid = item_request.item_oid "
				 "AND item_request.type = 'DVD' "
				 "WHERE "
				 "item_request.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_request.requestdate, "
				 "dvd.title, "
				 "dvd.id, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.type, "
				 "dvd.myoid, "
				 "item_request.myoid ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_request.requestdate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.type, "
				 "magazine.myoid, "
				 "item_request.myoid "
				 "FROM "
				 "magazine LEFT JOIN item_request ON "
				 "magazine.myoid = "
				 "item_request.item_oid "
				 "AND item_request.type = magazine.type "
				 "WHERE "
				 "item_request.memberid LIKE '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_request.requestdate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.type, "
				 "magazine.myoid, "
				 "item_request.myoid ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_request.requestdate, "
				 "videogame.title, "
				 "videogame.id, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.type, "
				 "videogame.myoid, "
				 "item_request.myoid "
				 "FROM "
				 "videogame LEFT JOIN item_request ON "
				 "videogame.myoid = "
				 "item_request.item_oid "
				 "AND item_request.type = 'Video Game' "
				 "WHERE "
				 "item_request.memberid LIKE '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_request.requestdate, "
				 "videogame.title, "
				 "videogame.id, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.type, "
				 "videogame.myoid, "
				 "item_request.myoid ");
		searchstr.append("ORDER BY 1");
	      }
	    else
	      {
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_request.requestdate, "
				 "book.title, "
				 "book.id, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.type, "
				 "book.myoid, "
				 "item_request.myoid "
				 "FROM "
				 "member, "
				 "book LEFT JOIN item_request ON "
				 "book.myoid = item_request.item_oid "
				 "AND item_request.type = 'Book' "
				 "WHERE ");
		searchstr.append("item_request.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "item_request.requestdate, "
				 "book.title, "
				 "book.id, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.type, "
				 "book.myoid, "
				 "item_request.myoid ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_request.requestdate, "
				 "cd.title, "
				 "cd.id, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.type, "
				 "cd.myoid, "
				 "item_request.myoid "
				 "FROM "
				 "member, "
				 "cd LEFT JOIN item_request ON "
				 "cd.myoid = item_request.item_oid "
				 "AND item_request.type = 'CD' "
				 "WHERE ");
		searchstr.append("item_request.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "item_request.requestdate, "
				 "cd.title, "
				 "cd.id, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.type, "
				 "cd.myoid, "
				 "item_request.myoid ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_request.requestdate, "
				 "dvd.title, "
				 "dvd.id, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.type, "
				 "dvd.myoid, "
				 "item_request.myoid "
				 "FROM "
				 "member, "
				 "dvd LEFT JOIN item_request ON "
				 "dvd.myoid = item_request.item_oid "
				 "AND item_request.type = 'DVD' "
				 "WHERE ");
		searchstr.append("item_request.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "item_request.requestdate, "
				 "dvd.title, "
				 "dvd.id, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.type, "
				 "dvd.myoid, "
				 "item_request.myoid ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_request.requestdate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.type, "
				 "magazine.myoid, "
				 "item_request.myoid "
				 "FROM "
				 "member, "
				 "magazine LEFT JOIN item_request ON "
				 "magazine.myoid = "
				 "item_request.item_oid "
				 "AND item_request.type = magazine.type "
				 "WHERE ");
		searchstr.append("item_request.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "item_request.requestdate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.type, "
				 "magazine.myoid, "
				 "item_request.myoid ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_request.requestdate, "
				 "videogame.title, "
				 "videogame.id, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.type, "
				 "videogame.myoid, "
				 "item_request.myoid "
				 "FROM "
				 "member, "
				 "videogame LEFT JOIN item_request ON "
				 "videogame.myoid = "
				 "item_request.item_oid "
				 "AND item_request.type = 'Video Game' "
				 "WHERE ");
		searchstr.append("item_request.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "item_request.requestdate, "
				 "videogame.title, "
				 "videogame.id, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.type, "
				 "videogame.myoid, "
				 "item_request.myoid ");
		searchstr.append("ORDER BY 1");
	      }
	  }
	else if(typefilter == "All Reserved")
	  {
	    searchstr = "";

	    if(roles.isEmpty())
	      {
		searchstr.append("SELECT DISTINCT "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "book.title, "
				 "book.id, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.quantity - "
				 "COUNT(item_borrower_vw.item_oid) "
				 "AS availability, "
				 "book.type, "
				 "book.myoid "
				 "FROM "
				 "book LEFT JOIN item_borrower_vw ON "
				 "book.myoid = item_borrower_vw.item_oid "
				 "AND item_borrower_vw.type = 'Book' "
				 "WHERE "
				 "item_borrower_vw.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "book.title, "
				 "book.id, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.type, "
				 "book.myoid ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "cd.title, "
				 "cd.id, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.quantity - "
				 "COUNT(item_borrower_vw.item_oid) "
				 "AS availability, "
				 "cd.type, "
				 "cd.myoid "
				 "FROM "
				 "cd LEFT JOIN item_borrower_vw ON "
				 "cd.myoid = item_borrower_vw.item_oid "
				 "AND item_borrower_vw.type = 'CD' "
				 "WHERE "
				 "item_borrower_vw.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "cd.title, "
				 "cd.id, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.type, "
				 "cd.myoid ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "dvd.title, "
				 "dvd.id, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.quantity - "
				 "COUNT(item_borrower_vw.item_oid) "
				 "AS availability, "
				 "dvd.type, "
				 "dvd.myoid "
				 "FROM "
				 "dvd LEFT JOIN item_borrower_vw ON "
				 "dvd.myoid = item_borrower_vw.item_oid "
				 "AND item_borrower_vw.type = 'DVD' "
				 "WHERE "
				 "item_borrower_vw.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "dvd.title, "
				 "dvd.id, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.type, "
				 "dvd.myoid ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.quantity - "
				 "COUNT(item_borrower_vw.item_oid) "
				 "AS availability, "
				 "magazine.type, "
				 "magazine.myoid "
				 "FROM "
				 "magazine LEFT JOIN item_borrower_vw ON "
				 "magazine.myoid = "
				 "item_borrower_vw.item_oid "
				 "AND item_borrower_vw.type = magazine.type "
				 "WHERE "
				 "item_borrower_vw.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.type, "
				 "magazine.myoid ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "videogame.title, "
				 "videogame.id, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.quantity - "
				 "COUNT(item_borrower_vw.item_oid) "
				 "AS availability, "
				 "videogame.type, "
				 "videogame.myoid "
				 "FROM "
				 "videogame LEFT JOIN item_borrower_vw ON "
				 "videogame.myoid = "
				 "item_borrower_vw.item_oid "
				 "AND item_borrower_vw.type = 'Video Game' "
				 "WHERE "
				 "item_borrower_vw.memberid = '");
		searchstr.append(searchstrArg);
		searchstr.append("' ");
		searchstr.append("GROUP BY "
				 "item_borrower_vw.copyid, "
				 "item_borrower_vw.reserved_date, "
				 "item_borrower_vw.duedate, "
				 "videogame.title, "
				 "videogame.id, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.type, "
				 "videogame.myoid ");
		searchstr.append("ORDER BY 1");
	      }
	    else
	      {
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "book.title, "
				 "book.id, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "book.type, "
				 "book.myoid "
				 "FROM "
				 "member, "
				 "book LEFT JOIN item_borrower ON "
				 "book.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = 'Book' "
				 "WHERE "
				 "member.memberid LIKE '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append("item_borrower.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "book.title, "
				 "book.id, "
				 "book.publisher, book.pdate, "
				 "book.category, "
				 "book.language, "
				 "book.price, book.monetary_units, "
				 "book.quantity, "
				 "book.location, "
				 "book.type, "
				 "book.myoid ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "cd.title, "
				 "cd.id, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "cd.type, "
				 "cd.myoid "
				 "FROM "
				 "member, "
				 "cd LEFT JOIN item_borrower ON "
				 "cd.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = 'CD' "
				 "WHERE "
				 "member.memberid LIKE '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append("item_borrower.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "cd.title, "
				 "cd.id, "
				 "cd.recording_label, cd.rdate, "
				 "cd.category, "
				 "cd.language, "
				 "cd.price, cd.monetary_units, "
				 "cd.quantity, "
				 "cd.location, "
				 "cd.type, "
				 "cd.myoid ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "dvd.title, "
				 "dvd.id, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "dvd.type, "
				 "dvd.myoid "
				 "FROM "
				 "member, "
				 "dvd LEFT JOIN item_borrower ON "
				 "dvd.myoid = item_borrower.item_oid "
				 "AND item_borrower.type = 'DVD' "
				 "WHERE "
				 "member.memberid LIKE '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append("item_borrower.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "dvd.title, "
				 "dvd.id, "
				 "dvd.studio, dvd.rdate, "
				 "dvd.category, "
				 "dvd.language, "
				 "dvd.price, dvd.monetary_units, "
				 "dvd.quantity, "
				 "dvd.location, "
				 "dvd.type, "
				 "dvd.myoid ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "magazine.type, "
				 "magazine.myoid "
				 "FROM "
				 "member, "
				 "magazine LEFT JOIN item_borrower ON "
				 "magazine.myoid = "
				 "item_borrower.item_oid "
				 "AND item_borrower.type = magazine.type "
				 "WHERE "
				 "member.memberid LIKE '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append("item_borrower.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "magazine.title, "
				 "magazine.id, "
				 "magazine.publisher, magazine.pdate, "
				 "magazine.category, "
				 "magazine.language, "
				 "magazine.price, magazine.monetary_units, "
				 "magazine.quantity, "
				 "magazine.location, "
				 "magazine.type, "
				 "magazine.myoid ");
		searchstr.append("UNION ");
		searchstr.append("SELECT DISTINCT "
				 "member.last_name || ', ' || "
				 "member.first_name AS name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "videogame.title, "
				 "videogame.id, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.quantity - "
				 "COUNT(item_borrower.item_oid) "
				 "AS availability, "
				 "videogame.type, "
				 "videogame.myoid "
				 "FROM "
				 "member, "
				 "videogame LEFT JOIN item_borrower ON "
				 "videogame.myoid = "
				 "item_borrower.item_oid "
				 "AND item_borrower.type = 'Video Game' "
				 "WHERE "
				 "member.memberid LIKE '");
		searchstr.append(searchstrArg);
		searchstr.append("' AND ");
		searchstr.append("item_borrower.memberid = "
				 "member.memberid ");
		searchstr.append("GROUP BY "
				 "name, "
				 "member.memberid, "
				 "item_borrower.copyid, "
				 "item_borrower.reserved_date, "
				 "item_borrower.duedate, "
				 "videogame.title, "
				 "videogame.id, "
				 "videogame.publisher, videogame.rdate, "
				 "videogame.genre, "
				 "videogame.language, "
				 "videogame.price, videogame.monetary_units, "
				 "videogame.quantity, "
				 "videogame.location, "
				 "videogame.type, "
				 "videogame.myoid ");
		searchstr.append("ORDER BY 1");
	      }
	  }
	else if(typefilter == "Video Games")
	  {
	    searchstr = "SELECT videogame.title, "
	      "videogame.vgrating, "
	      "videogame.vgplatform, "
	      "videogame.vgmode, "
	      "videogame.publisher, "
	      "videogame.rdate, "
	      "videogame.genre, "
	      "videogame.language, "
	      "videogame.id, "
	      "videogame.price, "
	      "videogame.monetary_units, "
	      "videogame.quantity, "
	      "videogame.location, "
	      "videogame.quantity - "
	      "COUNT(item_borrower_vw.item_oid) "
	      "AS availability, "
	      "videogame.type, "
	      "videogame.myoid "
	      "FROM "
	      "videogame LEFT JOIN item_borrower_vw ON "
	      "videogame.myoid = item_borrower_vw.item_oid "
	      "AND item_borrower_vw.type = 'Video Game' "
	      "GROUP BY "
	      "videogame.title, "
	      "videogame.vgrating, "
	      "videogame.vgplatform, "
	      "videogame.vgmode, "
	      "videogame.publisher, "
	      "videogame.rdate, "
	      "videogame.genre, "
	      "videogame.language, "
	      "videogame.id, "
	      "videogame.price, "
	      "videogame.monetary_units, "
	      "videogame.quantity, "
	      "videogame.location, "
	      "videogame.type, "
	      "videogame.myoid ORDER BY "
	      "videogame.title";
	  }
	else if(typefilter == "Books")
	  {
	    searchstr = "SELECT book.title, "
	      "book.author, "
	      "book.publisher, book.pdate, "
	      "book.edition, "
	      "book.category, book.language, "
	      "book.id, "
	      "book.price, book.monetary_units, "
	      "book.quantity, "
	      "book.binding_type, "
	      "book.location, "
	      "book.isbn13, "
	      "book.lccontrolnumber, "
	      "book.callnumber, "
	      "book.deweynumber, "
	      "book.quantity - "
	      "COUNT(item_borrower_vw.item_oid) "
	      "AS availability, "
	      "book.type, "
	      "book.myoid "
	      "FROM "
	      "book LEFT JOIN item_borrower_vw ON "
	      "book.myoid = item_borrower_vw.item_oid "
	      "AND item_borrower_vw.type = 'Book' "
	      "GROUP BY "
	      "book.title, "
	      "book.author, "
	      "book.publisher, book.pdate, "
	      "book.edition, "
	      "book.category, book.language, "
	      "book.id, "
	      "book.price, book.monetary_units, "
	      "book.quantity, "
	      "book.binding_type, "
	      "book.location, "
	      "book.isbn13, "
	      "book.lccontrolnumber, "
	      "book.callnumber, "
	      "book.deweynumber, "
	      "book.type, "
	      "book.myoid ORDER BY "
	      "book.title";
	  }
	else if(typefilter == "DVDs")
	  {
	    searchstr = "SELECT dvd.title, "
	      "dvd.dvdformat, "
	      "dvd.studio, "
	      "dvd.rdate, "
	      "dvd.dvddiskcount, "
	      "dvd.dvdruntime, "
	      "dvd.category, "
	      "dvd.language, "
	      "dvd.id, "
	      "dvd.price, "
	      "dvd.monetary_units, "
	      "dvd.quantity, "
	      "dvd.location, "
	      "dvd.dvdrating, "
	      "dvd.dvdregion, "
	      "dvd.dvdaspectratio, "
	      "dvd.quantity - "
	      "COUNT(item_borrower_vw.item_oid) "
	      "AS availability, "
	      "dvd.type, "
	      "dvd.myoid "
	      "FROM "
	      "dvd LEFT JOIN item_borrower_vw ON "
	      "dvd.myoid = item_borrower_vw.item_oid "
	      "AND item_borrower_vw.type = 'DVD' "
	      "GROUP BY "
	      "dvd.title, "
	      "dvd.dvdformat, "
	      "dvd.studio, "
	      "dvd.rdate, "
	      "dvd.dvddiskcount, "
	      "dvd.dvdruntime, "
	      "dvd.category, "
	      "dvd.language, "
	      "dvd.id, "
	      "dvd.price, "
	      "dvd.monetary_units, "
	      "dvd.quantity, "
	      "dvd.location, "
	      "dvd.dvdrating, "
	      "dvd.dvdregion, "
	      "dvd.dvdaspectratio, "
	      "dvd.type, "
	      "dvd.myoid "
	      "ORDER BY "
	      "dvd.title";
	  }
	else if(typefilter == "Music CDs")
	  {
	    searchstr = "SELECT cd.title, "
	      "cd.artist, "
	      "cd.cdformat, "
	      "cd.recording_label, "
	      "cd.rdate, "
	      "cd.cddiskcount, "
	      "cd.cdruntime, "
	      "cd.category, "
	      "cd.language, "
	      "cd.id, "
	      "cd.price, "
	      "cd.monetary_units, "
	      "cd.quantity, "
	      "cd.location, "
	      "cd.cdaudio, "
	      "cd.cdrecording, "
	      "cd.quantity - COUNT(item_borrower_vw.item_oid) AS "
	      "availability, "
	      "cd.type, "
	      "cd.myoid "
	      "FROM "
	      "cd LEFT JOIN item_borrower_vw ON "
	      "cd.myoid = item_borrower_vw.item_oid "
	      "AND item_borrower_vw.type = 'CD' "
	      "GROUP BY "
	      "cd.title, "
	      "cd.artist, "
	      "cd.cdformat, "
	      "cd.recording_label, "
	      "cd.rdate, "
	      "cd.cddiskcount, "
	      "cd.cdruntime, "
	      "cd.category, "
	      "cd.language, "
	      "cd.id, "
	      "cd.price, "
	      "cd.monetary_units, "
	      "cd.quantity, "
	      "cd.location, "
	      "cd.cdaudio, "
	      "cd.cdrecording, "
	      "cd.type, "
	      "cd.myoid "
	      "ORDER BY "
	      "cd.title";
	  }
	else if(typefilter == "Journals" || typefilter == "Magazines")
	  {
	    if(typefilter == "Journals")
	      type = "Journal";
	    else
	      type = "Magazine";

	    searchstr = QString("SELECT magazine.title, "
				"magazine.publisher, magazine.pdate, "
				"magazine.mag_volume, magazine.mag_no, "
				"magazine.category, magazine.language, "
				"magazine.id, "
				"magazine.price, magazine.monetary_units, "
				"magazine.quantity, "
				"magazine.location, "
				"magazine.lccontrolnumber, "
				"magazine.callnumber, "
				"magazine.deweynumber, "
				"magazine.quantity - "
				"COUNT(item_borrower_vw.item_oid) AS "
				"availability, "
				"magazine.type, "
				"magazine.myoid "
				"FROM "
				"magazine LEFT JOIN item_borrower_vw ON "
				"magazine.myoid = "
				"item_borrower_vw.item_oid "
				"AND item_borrower_vw.type = magazine.type "
				"WHERE "
				"magazine.type = '%1' "
				"GROUP BY "
				"magazine.title, "
				"magazine.publisher, magazine.pdate, "
				"magazine.mag_volume, magazine.mag_no, "
				"magazine.category, magazine.language, "
				"magazine.id, "
				"magazine.price, magazine.monetary_units, "
				"magazine.quantity, "
				"magazine.location, "
				"magazine.lccontrolnumber, "
				"magazine.callnumber, "
				"magazine.deweynumber, "
				"magazine.type, "
				"magazine.myoid ORDER BY "
				"magazine.title").arg(type);
	  }

	break;
      }
    case POPULATE_SEARCH: default:
      {
	if(typefilter == "All")
	  {
	    types.append("Book");
	    types.append("CD");
	    types.append("DVD");
	    types.append("Journal");
	    types.append("Magazine");
	    types.append("Video Game");
	    searchstr = "";

	    while(!types.isEmpty())
	      {
		type = types.takeFirst();

		if(type == "Journal")
		  str = QString
		    ("SELECT DISTINCT %1.title, "
		     "%1.id, "
		     "%1.publisher, %1.pdate, "
		     "%1.category, "
		     "%1.language, "
		     "%1.price, %1.monetary_units, "
		     "%1.quantity, "
		     "%1.location, "
		     "%1.quantity - "
		     "COUNT(item_borrower_vw.item_oid) AS availability, "
		     "%1.type, ").arg("magazine");
		else
		  str = QString
		    ("SELECT DISTINCT %1.title, "
		     "%1.id, "
		     "%1.publisher, %1.pdate, "
		     "%1.category, "
		     "%1.language, "
		     "%1.price, %1.monetary_units, "
		     "%1.quantity, "
		     "%1.location, "
		     "%1.quantity - "
		     "COUNT(item_borrower_vw.item_oid) AS availability, "
		     "%1.type, ").
		    arg(type.toLower().remove(" "));

		if(type == "Journal")
		  str += QString("%1.myoid "
				 "FROM "
				 "%1 LEFT JOIN item_borrower_vw ON "
				 "%1.myoid = "
				 "item_borrower_vw.item_oid "
				 "AND item_borrower_vw.type = 'Journal' "
				 "WHERE ").arg("magazine");
		else
		  str += QString("%1.myoid "
				 "FROM "
				 "%1 LEFT JOIN item_borrower_vw ON "
				 "%1.myoid = "
				 "item_borrower_vw.item_oid "
				 "AND item_borrower_vw.type = '%2' "
				 "WHERE ").arg(type.toLower().remove(" ")).
		    arg(type);

		str.append("(LOWER(id) LIKE '%" +
			   myqstring::escape(al.idnumber->text().toLower()) +
			   "%' ");

		if(type == "Book")
		  str.append("OR LOWER(isbn13) LIKE '%" +
			     myqstring::escape(al.idnumber->text().
					       toLower()) + "%')");
		else
		  str.append(")");

		str.append(" AND ");
		str.append("LOWER(title) LIKE '%" +
			   myqstring::escape(al.title->text().toLower()) +
			   "%' AND ");

		if(al.publication_date->date().toString
		   ("MM/dd/yyyy") != "01/01/7999")
		  str.append("pdate = '" +
			     al.publication_date->date().toString
			     ("MM/dd/yyyy") + "' AND ");

		if(al.category->currentText() != "Any")
		  str.append("category = '" +
			     myqstring::escape
			     (al.category->currentText()) +
			     "' AND ");

		str.append("LOWER(publisher) LIKE '%" +
			   myqstring::escape(al.publisher->text().toLower()) +
			   "%' AND ");

		if(al.price->value() > 0)
		  {
		    str.append("price = ");
		    str.append(al.price->text());
		    str.append(" AND ");
		  }

		if(al.language->currentText() != "Any")
		  str.append("language = '" +
			     myqstring::escape
			     (al.language->currentText()) +
			     "' AND ");

		if(al.monetary_units->currentText() != "Any")
		  str.append("monetary_units = '" +
			     myqstring::escape
			     (al.monetary_units->currentText()) +
			     "' AND ");

		str.append("LOWER(description) LIKE '%" +
			   myqstring::escape
			   (al.description->toPlainText().toLower()) + "%' ");

		if(al.quantity->value() != 0)
		  str.append("AND quantity = " + al.quantity->text() + " ");

		if(al.location->currentText() != "Any")
		  str.append("AND location = '" +
			     myqstring::escape
			     (al.location->currentText()) + "' ");

		if(type == "Journal")
		  str.append(QString("AND %1.type = 'Journal' ").
			     arg("magazine"));
		else if(type == "Magazine")
		  str.append(QString("AND %1.type = 'Magazine' ").
			     arg(type.toLower().remove(" ")));

		if(type == "Journal")
		  str += QString("GROUP BY "
				 "%1.title, "
				 "%1.id, "
				 "%1.publisher, %1.pdate, "
				 "%1.category, "
				 "%1.language, "
				 "%1.price, "
				 "%1.monetary_units, "
				 "%1.quantity, "
				 "%1.location, "
				 "%1.type, "
				 "%1.myoid ").arg("magazine");
		else
		  str += QString("GROUP BY "
				 "%1.title, "
				 "%1.id, "
				 "%1.publisher, %1.pdate, "
				 "%1.category, "
				 "%1.language, "
				 "%1.price, "
				 "%1.monetary_units, "
				 "%1.quantity, "
				 "%1.location, "
				 "%1.type, "
				 "%1.myoid ").arg
		    (type.toLower().remove(" "));

		if(type == "CD")
		  {
		    str = str.replace("pdate", "rdate");
		    str = str.replace("publisher", "recording_label");
		  }
		else if(type == "DVD")
		  {
		    str = str.replace("pdate", "rdate");
		    str = str.replace("publisher", "studio");
		  }
		else if(type == "Video Game")
		  {
		    str = str.replace("pdate", "rdate");
		    str = str.replace("category", "genre");
		  }

		if(type != "Video Game")
		  str += "UNION ";
		else
		  str += " ";

		searchstr += str;
	      }

	    searchstr += "ORDER BY 1";
	  }
	else if(typefilter == "Books")
	  {
	    searchstr.append(searchstrArg);
	    searchstr.append("GROUP BY book.title, "
			     "book.author, "
			     "book.publisher, book.pdate, "
			     "book.edition, "
			     "book.category, book.language, "
			     "book.id, "
			     "book.price, "
			     "book.monetary_units, "
			     "book.quantity, "
			     "book.binding_type, "
			     "book.location, "
			     "book.isbn13, "
			     "book.lccontrolnumber, "
			     "book.callnumber, "
			     "book.deweynumber, "
			     "book.type, "
			     "book.myoid "
			     "ORDER BY book.title");
	  }
	else if(typefilter == "Video Games")
	  {
	    searchstr.append(searchstrArg);
	    searchstr.append("GROUP BY "
			     "videogame.title, "
			     "videogame.vgrating, "
			     "videogame.vgplatform, "
			     "videogame.vgmode, "
			     "videogame.publisher, "
			     "videogame.rdate, "
			     "videogame.genre, "
			     "videogame.language, "
			     "videogame.id, "
			     "videogame.price, "
			     "videogame.monetary_units, "
			     "videogame.quantity, "
			     "videogame.location, "
			     "videogame.type, "
			     "videogame.myoid ORDER BY "
			     "videogame.title");
	  }
	else if(typefilter == "Music CDs")
	  {
	    searchstr.append(searchstrArg);
	    searchstr.append("GROUP BY "
			     "cd.title, "
			     "cd.artist, "
			     "cd.cdformat, "
			     "cd.recording_label, "
			     "cd.rdate, "
			     "cd.cddiskcount, "
			     "cd.cdruntime, "
			     "cd.category, "
			     "cd.language, "
			     "cd.id, "
			     "cd.price, "
			     "cd.monetary_units, "
			     "cd.quantity, "
			     "cd.location, "
			     "cd.cdaudio, "
			     "cd.cdrecording, "
			     "cd.type, "
			     "cd.myoid "
			     "ORDER BY "
			     "cd.title");
	  }
	else if(typefilter == "DVDs")
	  {
	    searchstr.append(searchstrArg);
	    searchstr.append("GROUP BY "
			     "dvd.title, "
			     "dvd.dvdformat, "
			     "dvd.studio, "
			     "dvd.rdate, "
			     "dvd.dvddiskcount, "
			     "dvd.dvdruntime, "
			     "dvd.category, "
			     "dvd.language, "
			     "dvd.id, "
			     "dvd.price, "
			     "dvd.monetary_units, "
			     "dvd.quantity, "
			     "dvd.location, "
			     "dvd.dvdrating, "
			     "dvd.dvdregion, "
			     "dvd.dvdaspectratio, "
			     "dvd.type, "
			     "dvd.myoid "
			     "ORDER BY "
			     "dvd.title");
	  }
	else if(typefilter == "Journals" || typefilter == "Magazines")
	  {
	    searchstr.append(searchstrArg);
	    searchstr.append("GROUP BY magazine.title, "
			     "magazine.publisher, magazine.pdate, "
			     "magazine.mag_volume, "
			     "magazine.mag_no, "
			     "magazine.category, magazine.language, "
			     "magazine.id, "
			     "magazine.price, "
			     "magazine.monetary_units, "
			     "magazine.quantity, "
			     "magazine.location, "
			     "magazine.lccontrolnumber, "
			     "magazine.callnumber, "
			     "magazine.deweynumber, "
			     "magazine.type, "
			     "magazine.myoid "
			     "ORDER BY magazine.title, "
			     "magazine.mag_volume, magazine.mag_no");
	  }

	break;
      }
    }

  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec(searchstr))
    {
      qapp->restoreOverrideCursor();

      if(!previousTypeFilter.isEmpty())
	ui.typefilter->setCurrentIndex
	  (ui.typefilter->findText(previousTypeFilter));

      addError(QString("Database Error"),
	       QString("Unable to retrieve the data required for "
		       "populating the main table."),
	       query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(this, "BiblioteQ: Database Error",
			    "Unable to retrieve the data required for "
			    "populating the main table.");
      return 1;
    }
  else
    {
      qapp->restoreOverrideCursor();

      if(ui.typefilter->findText(typefilter) > -1)
	previousTypeFilter = typefilter;

      if(typefilter.isEmpty())
	ui.typefilter->setCurrentIndex(0);
      else if(ui.typefilter->findText(typefilter) > -1)
	ui.typefilter->setCurrentIndex(ui.typefilter->findText(typefilter));
      else
	ui.typefilter->setCurrentIndex(0);

      if(search_type == POPULATE_SEARCH && all_diag->isVisible())
	all_diag->close();

      if(search_type != CUSTOM_QUERY)
	{
	  ui.table->resetTable(typefilter, roles);
	  addConfigOptions(typefilter);
	}
      else
	ui.table->resetTable("", roles);

      if(selectedBranch["database_type"] != "sqlite")
	ui.table->setRowCount(query.size());

      progress.setModal(true);
      progress.setWindowTitle("BiblioteQ: Progress Dialog");
      progress.setLabelText("Populating the table...");

      if(selectedBranch["database_type"] == "sqlite")
	progress.setMaximum
	  (misc_functions::sqliteQuerySize(searchstr, getDB(),
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
		str = query.value(j).toString();

		if(search_type == CUSTOM_QUERY)
		  if(!tmplist.contains(query.record().fieldName(j)))
		    {
		      tmplist.append(query.record().fieldName(j));
		      ui.table->setColumnCount(tmplist.size());
		    }

		if(query.record().fieldName(j) == "issue" ||
		   query.record().fieldName(j) == "price" ||
		   query.record().fieldName(j) == "volume" ||
		   query.record().fieldName(j) == "quantity" ||
		   query.record().fieldName(j) == "mag_no" ||
		   query.record().fieldName(j) == "mag_volume" ||
		   query.record().fieldName(j) == "cddiskcount" ||
		   query.record().fieldName(j) == "dvddiskcount" ||
		   query.record().fieldName(j) == "availability")
		  item = new numeric_table_item(str.toDouble());
		else
		  item = new QTableWidgetItem();

		if(item != NULL)
		  {
		    item->setText(str);
		    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

		    if(selectedBranch["database_type"] == "sqlite")
		      ui.table->setRowCount(i + 1);

		    ui.table->setItem(i, j, item);

		    if(query.record().fieldName(j) == "type")
		      {
			itemType = str;
			itemType = itemType.toLower().remove(" ");
		      }

		    if(query.record().fieldName(j) == "myoid")
		      updateRows(str, i, itemType);
		  }
		else
		  addError(QString("Memory Error"),
			   QString("Unable to allocate "
				   "memory for the \"item\" "
				   "object. "
				   "This is a serious "
				   "problem!"), QString(""),
			   __FILE__, __LINE__);
	      }

	  progress.setValue(i + 1);
	  progress.update();
	  qapp->processEvents();
	}

      if(search_type == CUSTOM_QUERY)
	{
	  if(tmplist.isEmpty())
	    for(i = 0; i < query.record().count(); i++)
	      if(!tmplist.contains(query.record().fieldName(i)))
		{
		  tmplist.append(query.record().fieldName(i));
		  ui.table->setColumnCount(tmplist.size());
		}

	  ui.table->setColumnCount(tmplist.size());
	  ui.table->setHorizontalHeaderLabels(tmplist);
	  tmplist.clear();
	  addConfigOptions("Custom");
	}

      query.clear();
      ui.table->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
      ui.table->setRowCount(i);
      slotDisplaySummary();

      if(ui.actionAutoResizeColumns->isChecked())
	slotResizeColumns();

      return 0;
    }
}

/*
** -- slotResizeColumnsAfterSort() --
*/

void qtbook::slotResizeColumnsAfterSort(void)
{
  QObject *object = qobject_cast<QObject *>(sender());
  QObject *parent = NULL;

  if(ui.actionAutoResizeColumns->isChecked())
    if(object != NULL && object->parent() != NULL)
      {
	qapp->setOverrideCursor(Qt::WaitCursor);
	parent = object->parent();
	(qobject_cast<QTableWidget *>(parent))->resizeColumnsToContents();
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

  if(ui.table->horizontalHeader()->sortIndicatorOrder() != Qt::AscendingOrder)
    order = Qt::DescendingOrder;
  else
    order = Qt::AscendingOrder;

  ui.table->horizontalHeader()->setSortIndicator(column, order);
  ui.table->sortByColumn(column, order);

  for(i = 0; i < ui.table->rowCount(); i++)
    {
      oid = misc_functions::getColumnString(ui.table, i, "OID");
      itemType = misc_functions::getColumnString(ui.table, i, "Type");
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

  userinfo.memberid->setText("m" + nowTime.toString("yyyyMMddhhmmss"));
  userinfo.membersince->setFocus();
  userinfo.membersince->setDate(now);
  userinfo.membersince->setMaximumDate(now);
  userinfo.firstName->clear();
  userinfo.lastName->clear();
  userinfo.middle->clear();
  userinfo.dob->setDate(now.addYears(-25));
  userinfo.sex->setCurrentIndex(0);
  userinfo.street->clear();
  userinfo.city->clear();
  userinfo.state->setCurrentIndex(0);
  userinfo.zip->setText("00000");
  userinfo.zip->setCursorPosition(0);
  userinfo.telephoneNumber->clear();
  userinfo_diag->setWindowTitle("BiblioteQ: Create New Member");
  userinfo.prevTool->setVisible(false);
  userinfo.nextTool->setVisible(false);
  userinfo_diag->updateGeometry();
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
  QString str1 = "";
  QString str2 = "";
  QString str3 = "";
  QString str4 = "";
  QString str5 = "";
  QString str6 = "";
  QString str7 = "";
  QString checksum = "";
  QString errorstr = "";
  QSqlQuery query(db);
  QTableWidgetItem *column = NULL;

  str1 = userinfo.firstName->text().trimmed();
  userinfo.firstName->setText(str1);
  str2 = userinfo.lastName->text().trimmed();
  userinfo.lastName->setText(str2);
  str3 = userinfo.middle->text().trimmed();
  userinfo.middle->setText(str3);
  str4 = userinfo.street->text().trimmed();
  userinfo.street->setText(str4);
  str5 = userinfo.city->text().trimmed();
  userinfo.city->setText(str5);
  str6 = userinfo.zip->text().trimmed();
  userinfo.zip->setText(str6);
  str7 = userinfo.telephoneNumber->text().trimmed();
  userinfo.telephoneNumber->setText(str7);

  if(userinfo.firstName->text().isEmpty())
    {
      QMessageBox::critical(userinfo_diag, "BiblioteQ: User Error",
			    "Please provide a valid First Name.");
      userinfo.firstName->setFocus();
      return;
    }

  if(userinfo.lastName->text().isEmpty())
    {
      QMessageBox::critical(userinfo_diag, "BiblioteQ: User Error",
			    "Please provide a valid Last Name.");
      userinfo.lastName->setFocus();
      return;
    }

  if(userinfo.street->text().isEmpty())
    {
      QMessageBox::critical(userinfo_diag, "BiblioteQ: User Error",
			    "Please provide a valid Street.");
      userinfo.street->setFocus();
      return;
    }

  if(userinfo.city->text().isEmpty())
    {
      QMessageBox::critical(userinfo_diag, "BiblioteQ: User Error",
			    "Please provide a valid City.");
      userinfo.city->setFocus();
      return;
    }

  if(userinfo.zip->text().length() != 5)
    {
      QMessageBox::critical(userinfo_diag, "BiblioteQ: User Error",
			    "Please provide a valid ZIP Code.");
      userinfo.zip->setFocus();
      return;
    }

  checksum.append(userinfo.dob->text());
  checksum.append(userinfo.sex->currentText());
  checksum.append(userinfo.firstName->text());
  checksum.append(userinfo.middle->text());
  checksum.append(userinfo.lastName->text());
  checksum.append(userinfo.street->text());
  checksum.append(userinfo.city->text());
  checksum.append(userinfo.state->currentText());
  checksum.append(userinfo.zip->text());
  qapp->setOverrideCursor(Qt::WaitCursor);
  count = misc_functions::getMemberMatchCount(checksum.toLower(),
					      userinfo.memberid->text(),
					      db,
					      errorstr);
  qapp->restoreOverrideCursor();

  if(count < 0)
    {
      addError(QString("Database Error"),
	       QString("Unable to determine the uniqueness of the "
		       "proposed member."),
	       errorstr, __FILE__, __LINE__);
      QMessageBox::critical(userinfo_diag, "BiblioteQ: Database Error",
			    "Unable to determine the uniqueness of "
			    "the proposed member.");
      return;
    }

  if(count != 0)
    {
      QMessageBox::critical(userinfo_diag, "BiblioteQ: User Error",
			    "An identical member already exists.");
      return;
    }

  if(userinfo_diag->windowTitle().contains("New"))
    {
      qapp->setOverrideCursor(Qt::WaitCursor);

      if(!getDB().transaction())
	{
	  qapp->restoreOverrideCursor();
	  addError
	    (QString("Database Error"),
	     QString("Unable to create a database transaction."),
	     getDB().lastError().text(), __FILE__, __LINE__);
	  QMessageBox::critical
	    (userinfo_diag, "BiblioteQ: Database Error",
	     "Unable to create a database transaction.");
	  return;
	}

      qapp->restoreOverrideCursor();
      query.prepare(QString("INSERT INTO member "
			    "(memberid, membersince, dob, sex, "
			    "first_name, middle_init, last_name, "
			    "telephone_num, street, city, "
			    "state_abbr, zip) "
			    "VALUES "
			    "(?, ?, ?, ?, "
			    "?, ?, ?, "
			    "?, ?, ?, "
			    "?, ?)"));
      query.bindValue(0, userinfo.memberid->text());
      query.bindValue(1, userinfo.membersince->text());
      query.bindValue(2, userinfo.dob->text());
      query.bindValue(3, userinfo.sex->currentText());
      query.bindValue(4, userinfo.firstName->text());
      query.bindValue(5, userinfo.middle->text());
      query.bindValue(6, userinfo.lastName->text());
      query.bindValue(7, userinfo.telephoneNumber->text());
      query.bindValue(8, userinfo.street->text());
      query.bindValue(9, userinfo.city->text());
      query.bindValue(10, userinfo.state->currentText());
      query.bindValue(11, userinfo.zip->text());
    }
  else
    {
      query.prepare(QString("UPDATE member SET "
			    "membersince = ?, "
			    "dob = ?, sex = ?, "
			    "first_name = ?, "
			    "middle_init = ?, "
			    "last_name = ?, "
			    "telephone_num = ?, "
			    "street = ?, "
			    "city = ?, "
			    "state_abbr = ?, zip = ? "
			    "WHERE memberid = ?"));
      query.bindValue(0, userinfo.membersince->text());
      query.bindValue(1, userinfo.dob->text());
      query.bindValue(2, userinfo.sex->currentText());
      query.bindValue(3, userinfo.firstName->text());
      query.bindValue(4, userinfo.middle->text());
      query.bindValue(5, userinfo.lastName->text());
      query.bindValue(6, userinfo.telephoneNumber->text());
      query.bindValue(7, userinfo.street->text());
      query.bindValue(8, userinfo.city->text());
      query.bindValue(9, userinfo.state->currentText());
      query.bindValue(10, userinfo.zip->text());
      query.bindValue(11, userinfo.memberid->text());
    }

  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec())
    {
      if(userinfo_diag->windowTitle().contains("New"))
	if(!getDB().rollback())
	  addError
	    (QString("Database Error"), QString("Rollback failure."),
	     getDB().lastError().text(), __FILE__, __LINE__);

      qapp->restoreOverrideCursor();
      addError(QString("Database Error"),
	       QString("Unable to save the member's information."),
	       query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(userinfo_diag, "BiblioteQ: Database Error",
			    "Unable to save the member's information.");
    }
  else
    {
      if(userinfo_diag->windowTitle().contains("New"))
	{
	  /*
	  ** Create a database account for the new member.
	  */

	  misc_functions::DBAccount(userinfo.memberid->text(),
				    db, misc_functions::CREATE_USER,
				    errorstr);

	  if(!errorstr.isEmpty())
	    {
	      if(!getDB().rollback())
		addError
		  (QString("Database Error"), QString("Rollback failure."),
		   getDB().lastError().text(), __FILE__, __LINE__);

	      qapp->restoreOverrideCursor();
	      addError
		  (QString("Database Error"),
		   QString("An error occurred while attempting to "
			   "create a database account "
			   "for the new member."),
		   errorstr, __FILE__, __LINE__);
	      QMessageBox::critical(userinfo_diag,
				    "BiblioteQ: Database Error",
				    "An error occurred while attempting to "
				    "create a database account "
				    "for the new member.");
	      return;
	    }
	  else
	    if(!getDB().commit())
	      {
		qapp->restoreOverrideCursor();
		addError
		  (QString("Database Error"),
		   QString("Unable to commit the current database "
			   "transaction."),
		   getDB().lastError().text(), __FILE__,
		   __LINE__);
		QMessageBox::critical(userinfo_diag,
				      "BiblioteQ: Database Error",
				      "Unable to commit the current "
				      "database transaction.");
		return;
	      }
	}
      else
	{
	  /*
	  ** Update privileges.
	  */

	  misc_functions::DBAccount(userinfo.memberid->text(),
				    db, misc_functions::UPDATE_USER,
				    errorstr);

	  if(!errorstr.isEmpty())
	    addError(QString("Database Error"),
		     QString("An error occurred while attempting to "
			     "update the database account "
			     "for %1.").arg(userinfo.memberid->text()),
		     errorstr, __FILE__, __LINE__);
	}

      qapp->restoreOverrideCursor();

      if(userinfo_diag->windowTitle().contains("Modify"))
	{
	  bb.table->setSortingEnabled(false);

	  for(i = 0; i < bb.table->columnCount(); i++)
	    {
	      column = bb.table->horizontalHeaderItem(i);

	      if(column == NULL)
		continue;

	      if(column->text() == "First Name")
		bb.table->item(row, i)->setText(userinfo.firstName->text());
	      else if(column->text() == "Last Name")
		bb.table->item(row, i)->setText(userinfo.lastName->text());
	      else if(column->text() == "Member Since")
		bb.table->item(row, i)->setText(userinfo.membersince->text());
	    }

	  bb.table->setSortingEnabled(true);
	}
      else
	{
	  userinfo_diag->close();

	  if(selectedBranch["database_type"] != "sqlite")
	    QMessageBox::information(members_diag, "BiblioteQ: Information",
				     "Please notify the new member that their "
				     "default password has been set "
				     "to tempPass.");

	  slotPopulateMembersBrowser();
	}
    }
}

/*
** -- readGlobalSetup() --
*/

void qtbook::readGlobalSetup(void)
{
  int i = 0;
  int j = 0;
  enum enumtype {BOOK_CATEGORY,
		 CD_CATEGORY,
		 JOURNAL_CATEGORY,
		 MAGAZINE_CATEGORY,
		 LANGUAGE,
		 MONETARY_UNITS,
		 CD_LOCATION,
		 BOOK_LOCATION,
		 JOURNAL_LOCATION,
		 MAGAZINE_LOCATION,
		 CD_FORMAT,
		 VG_LOCATION,
		 VG_GENRE,
		 VG_RATING,
		 VG_PLATFORM,
		 LIBRARY_OF_CONGRESS_CONFIGURATION,
		 DVD_CATEGORY,
		 DVD_LOCATION,
		 DVD_RATING,
		 DVD_ASPECT_RATIO,
		 DVD_REGION,
		 BRANCHES,
		 UNKNOWN};
  QString str = "";
  QString filename = "";
  enumtype type = UNKNOWN;
  QStringList categories;
  QStringList locations;
  generic_thread *thread = NULL;
  QHash<QString, QString> tmphash;

#ifdef Q_OS_WIN
  filename = "biblioteq.conf";
#else
  filename.append(CONFIGFILE);
#endif
  al.language->clear();
  al.monetary_units->clear();
  al.location->clear();
  al.category->clear();

  if((thread = new generic_thread()) != NULL)
    {
      qapp->setOverrideCursor(Qt::WaitCursor);
      thread->setFilename(filename);
      lockApp(true);
      thread->setType(generic_thread::READ_GLOBAL_CONFIG_FILE);
      thread->start();

      while(thread->isRunning())
	{
	  if(statusBar() != NULL)
	    statusBar()->showMessage("Processing the global "
				     "configuration file.");

	  qapp->processEvents();
	  thread->wait(100);
	}

      if(!thread->getErrorStr().isEmpty())
	{
	  if(statusBar() != NULL)
	    statusBar()->clearMessage();

	  addError(QString("File Error"),
		   thread->getErrorStr(),
		   thread->getErrorStr(), __FILE__, __LINE__);
	}
      else
	for(i = 0; i < thread->getList().size(); i++)
	  {
	    str = thread->getList().at(i);

	    if(str == "[CD Category]")
	      type = CD_CATEGORY;
	    else if(str == "[CD Format]")
	      type = CD_FORMAT;
	    else if(str == "[Book Category]")
	      type = BOOK_CATEGORY;
	    else if(str == "[Magazine Category]")
	      type = MAGAZINE_CATEGORY;
	    else if(str == "[Journal Category]")
	      type = JOURNAL_CATEGORY;
	    else if(str == "[Language]")
	      type = LANGUAGE;
	    else if(str == "[Monetary Units]")
	      type = MONETARY_UNITS;
	    else if(str == "[CD Location]")
	      type = CD_LOCATION;
	    else if(str == "[Book Location]")
	      type = BOOK_LOCATION;
	    else if(str == "[Magazine Location]")
	      type = MAGAZINE_LOCATION;
	    else if(str == "[Journal Location]")
	      type = JOURNAL_LOCATION;
	    else if(str == "[Video Game Genre]")
	      type = VG_GENRE;
	    else if(str == "[Video Game Location]")
	      type = VG_LOCATION;
	    else if(str == "[Video Game Rating]")
	      type = VG_RATING;
	    else if(str == "[Video Game Platform]")
	      type = VG_PLATFORM;
	    else if(str == "[Library of Congress Configuration]")
	      type = LIBRARY_OF_CONGRESS_CONFIGURATION;
	    else if(str == "[DVD Category]")
	      type = DVD_CATEGORY;
	    else if(str == "[DVD Location]")
	      type = DVD_LOCATION;
	    else if(str == "[DVD Rating]")
	      type = DVD_RATING;
	    else if(str == "[DVD Aspect Ratio]")
	      type = DVD_ASPECT_RATIO;
	    else if(str == "[DVD Region]")
	      type = DVD_REGION;
	    else if(str == "[Branches]")
	      type = BRANCHES;
	    else
	      type = UNKNOWN;

	    if(type != UNKNOWN)
	      for(j = i + 1; j < thread->getList().size(); j++)
		{
		  str = thread->getList().at(j);

		  if(str.isEmpty())
		    break;

		  switch(type)
		    {
		    case CD_CATEGORY:
		      {
			if(!cd_categories.contains(str))
			  cd_categories.append(str);

			if(!categories.contains(str))
			  categories.append(str);

			break;
		      }
		    case CD_FORMAT:
		      {
			if(!cd_formats.contains(str))
			  cd_formats.append(str);

			break;
		      }
		    case BOOK_CATEGORY:
		      {
			if(!book_categories.contains(str))
			  book_categories.append(str);

			if(!categories.contains(str))
			  categories.append(str);

			break;
		      }
		    case JOURNAL_CATEGORY:
		      {
			if(!journal_categories.contains(str))
			  journal_categories.append(str);

			if(!categories.contains(str))
			  categories.append(str);

			break;
		      }
		    case MAGAZINE_CATEGORY:
		      {
			if(!magazine_categories.contains(str))
			  magazine_categories.append(str);

			if(!categories.contains(str))
			  categories.append(str);

			break;
		      }
		    case LANGUAGE:
		      {
			if(al.language->findText(str) == -1)
			  al.language->addItem(str);

			if(!languages.contains(str))
			  languages.append(str);

			break;
		      }
		    case MONETARY_UNITS:
		      {
			if(al.monetary_units->findText(str) == -1)
			  al.monetary_units->addItem(str);

			if(!monetary_units.contains(str))
			  monetary_units.append(str);

			break;
		      }
		    case CD_LOCATION:
		      {
			if(!locations.contains(str))
			  locations.append(str);

			if(!cd_locations.contains(str))
			  cd_locations.append(str);

			break;
		      }
		    case BOOK_LOCATION:
		      {
			if(!locations.contains(str))
			  locations.append(str);

			if(!book_locations.contains(str))
			  book_locations.append(str);

			break;
		      }
		    case JOURNAL_LOCATION:
		      {
			if(!locations.contains(str))
			  locations.append(str);

			if(!journal_locations.contains(str))
			  journal_locations.append(str);

			break;
		      }
		    case MAGAZINE_LOCATION:
		      {
			if(!locations.contains(str))
			  locations.append(str);

			if(!magazine_locations.contains(str))
			  magazine_locations.append(str);

			break;
		      }
		    case VG_LOCATION:
		      {
			if(!locations.contains(str))
			  locations.append(str);

			if(!vg_locations.contains(str))
			  vg_locations.append(str);

			break;
		      }
		    case VG_GENRE:
		      {
			if(!vg_genres.contains(str))
			  vg_genres.append(str);

			if(!categories.contains(str))
			  categories.append(str);

			break;
		      }
		    case VG_RATING:
		      {
			if(!vg_ratings.contains(str))
			  vg_ratings.append(str);

			break;
		      }
		    case VG_PLATFORM:
		      {
			if(!vg_platforms.contains(str))
			  vg_platforms.append(str);

			break;
		      }
		    case DVD_RATING:
		      {
			if(!dvd_ratings.contains(str))
			  dvd_ratings.append(str);

			break;
		      }
		    case DVD_CATEGORY:
		      {
			if(!dvd_categories.contains(str))
			  dvd_categories.append(str);

			if(!categories.contains(str))
			  categories.append(str);

			break;
		      }
		    case DVD_LOCATION:
		      {
			if(!locations.contains(str))
			  locations.append(str);

			if(!dvd_locations.contains(str))
			  dvd_locations.append(str);

			break;
		      }
		    case DVD_REGION:
		      {
			if(!dvd_regions.contains(str))
			  dvd_regions.append(str);

			break;
		      }
		    case DVD_ASPECT_RATIO:
		      {
			if(!dvd_aspectratios.contains(str))
			  dvd_aspectratios.append(str);

			break;
		      }
		    case LIBRARY_OF_CONGRESS_CONFIGURATION:
		      {
			if(!LOCHash.contains("Address"))
			  LOCHash["Address"] = str;
			else if(!LOCHash.contains("Port"))
			  LOCHash["Port"] = str;
			else
			  LOCHash["Database"] = str;

			break;
		      }
		    case BRANCHES:
		      {
			if(!tmphash.contains("branch_name"))
			  tmphash["branch_name"] = str;
			else if(!tmphash.contains("hostname"))
			  tmphash["hostname"] = str;
			else if(!tmphash.contains("database_type"))
			  tmphash["database_type"] = str;
			else if(!tmphash.contains("database_port"))
			  tmphash["database_port"] = str;
			else if(!tmphash.contains("ssl_enabled"))
			  {
			    tmphash["ssl_enabled"] = str;

			    if(!branches.contains(tmphash["branch_name"]))
			      branches[tmphash["branch_name"]] = tmphash;

			    if(br.branch_name->
			       findText(tmphash["branch_name"]) == -1)
			      br.branch_name->addItem(tmphash["branch_name"]);

			    tmphash.clear();
			  }

			break;
		      }
		    default:
		      break;
		    }
		}
	  }

      tmphash.clear();
      locations.sort();
      categories.sort();

      if(categories.contains("UNKNOWN"))
	categories.removeAll("UNKNOWN");

      categories.append("UNKNOWN");

      while(!locations.isEmpty())
	al.location->addItem(locations.takeFirst());

      while(!categories.isEmpty())
	al.category->addItem(categories.takeFirst());

      if(al.language->count() == 0)
	al.language->addItem("UNKNOWN");

      if(al.monetary_units->count() == 0)
	al.monetary_units->addItem("UNKNOWN");

      if(al.location->count() == 0)
	al.location->addItem("UNKNOWN");

      if(al.category->count() == 0)
	al.category->addItem("UNKNOWN");

      if(br.branch_name->count() == 0)
	br.branch_name->addItem("UNKNOWN");

      if(!LOCHash.contains("Address"))
	LOCHash["Address"] = "z3950.loc.gov";

      if(!LOCHash.contains("Port"))
	LOCHash["Port"] = "7090";

      if(!LOCHash.contains("Database"))
	LOCHash["Database"] = "Voyager";

      if(statusBar() != NULL)
	statusBar()->clearMessage();

      lockApp(false);
      qapp->restoreOverrideCursor();

      if(!thread->getErrorStr().isEmpty())
	QMessageBox::critical(this, "BiblioteQ: File Error",
			      thread->getErrorStr());

      delete thread;
    }
  else
    {
      addError(QString("File Error"),
	       QString("Unable to read ") + filename +
	       QString(" due to insufficient resources."),
	       QString(""),
	       __FILE__, __LINE__);
      QMessageBox::critical(this, "BiblioteQ: File Error",
			    "Unable to read " + filename +
			    " due to insufficient resources.");
    }
}

/*
** -- lockApp() --
*/

void qtbook::lockApp(const bool lock)
{
  ui.connectTool->setEnabled(!lock);
  ui.actionConnect->setEnabled(!lock);
  ui.actionSaveSettings->setEnabled(!lock);
}

/*
** -- readConfig() --
*/

void qtbook::readConfig(void)
{
  int i = 0;
  QFont font;
  QString str = "";
  QString filename = "";
  QStringList tmplist;
  generic_thread *thread = NULL;

  filename.append(QDir::homePath());
  filename.append("/.biblioteq.dat");

  if((thread = new generic_thread()) != NULL)
    {
      qapp->setOverrideCursor(Qt::WaitCursor);
      thread->setFilename(filename);
      lockApp(true);
      thread->setType(generic_thread::READ_USER_CONFIG_FILE);
      thread->start();

      while(thread->isRunning())
	{
	  if(statusBar() != NULL)
	    statusBar()->showMessage("Processing the user's "
				     "configuration file.");

	  qapp->processEvents();
	  thread->wait(100);
	}

      if(!thread->getErrorStr().isEmpty())
	{
	  if(statusBar() != NULL)
	    statusBar()->clearMessage();

	  lockApp(false);
	  addError(QString("File Error"),
		   thread->getErrorStr(),
		   thread->getErrorStr(), __FILE__, __LINE__);
	  qapp->restoreOverrideCursor();
	  QMessageBox::warning(this, "BiblioteQ: File Warning",
			       thread->getErrorStr());
	  delete thread;
	  return;
	}

      for(i = 0; i < thread->getList().size(); i++)
	{
	  str = thread->getList().at(i);

	  if(str.startsWith("show_table_grid"))
	    if(str.endsWith("1"))
	      ui.actionShowGrid->setChecked(true);
	    else
	      ui.actionShowGrid->setChecked(false);

	  if(str.startsWith("populate_table_on_connect"))
	    if(str.endsWith("1"))
	      ui.actionPopulateOnStart->setChecked(true);
	    else
	      ui.actionPopulateOnStart->setChecked(false);

	  if(str.startsWith("automatically_resize_columns"))
	    if(str.endsWith("1"))
	      ui.actionAutoResizeColumns->setChecked(true);
	    else
	      ui.actionAutoResizeColumns->setChecked(false);

	  if(str.startsWith("automatically_populate_on_filter"))
	    if(str.endsWith("1"))
	      ui.actionAutoPopulateOnFilter->setChecked(true);
	    else
	      ui.actionAutoPopulateOnFilter->setChecked(false);

	  if(str.startsWith("reset_error_log_on_disconnect"))
	    if(str.endsWith("1"))
	      ui.actionResetErrorLogOnDisconnect->setChecked(true);
	    else
	      ui.actionResetErrorLogOnDisconnect->setChecked(false);

	  if(str.startsWith("automatically_populate_on_create"))
	    if(str.endsWith("1"))
	      ui.actionAutoPopulateOnCreation->setChecked(true);
	    else
	      ui.actionAutoPopulateOnCreation->setChecked(false);

	  if(str.startsWith("main_window_geometry"))
	    {
	      if(str.contains("x"))
		tmplist = str.remove("main_window_geometry=").split("x");

	      if(!tmplist.isEmpty() && tmplist.size() == 4)
		{
		  ui.actionPreserveGeometry->setChecked(true);
		  setGeometry(tmplist[0].toInt(),
			      tmplist[1].toInt(),
			      tmplist[2].toInt(),
			      tmplist[3].toInt());
		}
	      else
		ui.actionPreserveGeometry->setChecked(false);

	      tmplist.clear();
	    }

	  if(str.startsWith("global_font"))
	    font.fromString(str.remove("global_font="));

	  if(str.startsWith("sqlite_db"))
	    {
	      br.filename->setText(str.remove("sqlite_db="));
	      br.filename->setCursorPosition(0);
	      ui.actionRememberSQLiteFilename->setChecked
		(!br.filename->text().isEmpty());
	    }

	  if(str.startsWith("save_settings_on_exit"))
	    if(str.endsWith("1"))
	      ui.actionAutomaticallySaveSettingsOnExit->setChecked(true);
	    else
	      ui.actionAutomaticallySaveSettingsOnExit->setChecked(false);
	}

      if(statusBar() != NULL)
	statusBar()->clearMessage();

      delete thread;
      setGlobalFonts(font);
      slotResizeColumns();
      lockApp(false);
      qapp->restoreOverrideCursor();
    }
  else
    {
      addError(QString("File Warning"),
	       QString("Unable to read ") + filename +
	       QString(" due to insufficient resources."),
	       QString(""),
	       __FILE__, __LINE__);
      QMessageBox::warning(this, "BiblioteQ: File Warning",
			   "Unable to read " + filename +
			   " due to insufficient resources.");
    }
}

/*
** -- getGeometryString() --
*/

QString qtbook::getGeometryString(void)
{
  QString str = "";

  if(ui.actionPreserveGeometry->isChecked())
    str = QString("%1x%2x%3x%4").arg(geometry().x()).
      arg(geometry().y()).
      arg(geometry().width()).
      arg(geometry().height());

  return str;
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
      QMessageBox::critical(members_diag, "BiblioteQ: User Error",
			    "Please select a member to delete.");
      return;
    }

  memberid = misc_functions::getColumnString(bb.table, row, "Member ID");
  qapp->setOverrideCursor(Qt::WaitCursor);
  counts = misc_functions::getItemsReservedCounts(db, memberid, errorstr);
  qapp->restoreOverrideCursor();
  
  if(!errorstr.isEmpty())
    {
      addError(QString("Database Error"),
	       QString("Unable to determine the number of items that "
		       "are reserved by the selected member."),
	       errorstr, __FILE__, __LINE__);
      QMessageBox::critical(members_diag, "BiblioteQ: Database Error",
			    "Unable to determine the number of items that "
			    "are reserved by the selected member.");
      return;
    }

  totalReserved = counts["numbooks"].toInt() + counts["numcds"].toInt() +
    counts["numdvds"].toInt() + counts["numjournals"].toInt() +
    counts["nummagazines"].toInt() + counts["numvideogames"].toInt();
  counts.clear();

  if(totalReserved != 0)
    {
      QMessageBox::critical(members_diag, "BiblioteQ: User Error",
			    "You may not remove a member that has reserved "
			    "items.");
      return;
    }

  if(QMessageBox::question(members_diag, "BiblioteQ: Question",
			   "Are you sure that you wish to delete the "
			   "selected member?",
			   QMessageBox::Yes | QMessageBox::No,
			   QMessageBox::No) == QMessageBox::No)
    return;

  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!getDB().transaction())
    {
      qapp->restoreOverrideCursor();
      addError
	(QString("Database Error"),
	 QString("Unable to create a database transaction."),
	 getDB().lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical
	(members_diag, "BiblioteQ: Database Error",
	 "Unable to create a database transaction.");
      return;
    }

  qapp->restoreOverrideCursor();
  query.prepare(QString("DELETE FROM member WHERE "
			"memberid = ?"));
  query.bindValue(0, memberid);
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec())
    {
      if(!getDB().rollback())
	addError
	  (QString("Database Error"), QString("Rollback failure."),
	   getDB().lastError().text(), __FILE__, __LINE__);

      qapp->restoreOverrideCursor();
      addError(QString("Database Error"),
	       QString("Unable to remove the selected member."),
	       query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(members_diag, "BiblioteQ: Database Error",
			    "Unable to remove the selected member.");
    }
  else
    {
      misc_functions::DBAccount(memberid, db, misc_functions::DELETE_USER,
				errorstr);

      if(!query.exec())
	{
	  if(!getDB().rollback())
	    addError
	      (QString("Database Error"), QString("Rollback failure."),
	       getDB().lastError().text(), __FILE__, __LINE__);
	}
      else
	if(!getDB().commit())
	  {
	    qapp->restoreOverrideCursor();
	    addError
	      (QString("Database Error"),
	       QString("Unable to commit the current database "
		       "transaction."),
	       getDB().lastError().text(), __FILE__,
	       __LINE__);
	    QMessageBox::critical(members_diag,
				  "BiblioteQ: Database Error",
				  "Unable to commit the current "
				  "database transaction.");
	    return;
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
  QString filename = "";
  QList<bool> list;
  generic_thread *thread = NULL;

  filename.append(QDir::homePath());
  filename.append("/.biblioteq.dat");

  if((thread = new generic_thread()) != NULL)
    {
      if(isVisible())
	qapp->setOverrideCursor(Qt::WaitCursor);

      thread->setFilename(filename);
      ui.actionSaveSettings->setEnabled(false);
      list.append(ui.actionShowGrid->isChecked());
      list.append(ui.actionPopulateOnStart->isChecked());
      list.append(ui.actionAutoResizeColumns->isChecked());
      list.append(ui.actionAutoPopulateOnFilter->isChecked());
      list.append(ui.actionResetErrorLogOnDisconnect->isChecked());
      list.append(ui.actionAutoPopulateOnCreation->isChecked());
      list.append(ui.actionAutomaticallySaveSettingsOnExit->isChecked());
      thread->setType(generic_thread::WRITE_USER_CONFIG_FILE);
      thread->setOutputList(list);
      list.clear();
      thread->start();

      while(thread->isRunning())
	{
	  if(statusBar() != NULL && isVisible())
	    statusBar()->showMessage("Saving the user's configuration file.");

	  qapp->processEvents();
	  thread->wait(100);
	}

      if(!thread->getErrorStr().isEmpty())
	{
	  if(statusBar() != NULL && isVisible())
	    statusBar()->clearMessage();

	  if(isVisible())
	    {
	      ui.actionSaveSettings->setEnabled(true);
	      addError(QString("File Error"),
		       thread->getErrorStr(),
		       thread->getErrorStr(), __FILE__, __LINE__);
	      qapp->restoreOverrideCursor();
	      QMessageBox::critical(this, "BiblioteQ: File Error",
				    thread->getErrorStr());
	    }

	  delete thread;
	  return;
	}

      if(statusBar() != NULL && isVisible())
	statusBar()->clearMessage();

      delete thread;

      if(isVisible())
	{
	  ui.actionSaveSettings->setEnabled(true);
	  qapp->restoreOverrideCursor();
	}
    }
  else if(isVisible())
    {
      addError(QString("File Error"),
	       QString("Unable to save ") + filename +
	       QString(" due to insufficient resources."),
	       QString(""),
	       __FILE__, __LINE__);
      QMessageBox::critical(this, "BiblioteQ: File Error",
			    "Unable to save " + filename +
			    " due to insufficient resources.");
    }
}

/*
** -- slotShowColumns() --
*/

void qtbook::slotShowColumns(void)
{
  int i = 0;
  QTableWidgetItem *column = NULL;

  qapp->setOverrideCursor(Qt::WaitCursor);

  for(i = 0; i < ui.table->columnCount(); i++)
    {
      column = ui.table->horizontalHeaderItem(i);

      if(column == NULL)
	continue;

      if(column->text() == "Publisher")
	ui.table->setColumnHidden(i, false);
    }

  qapp->restoreOverrideCursor();
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
      oid = misc_functions::getColumnString(ui.table, i, "OID");
      type = misc_functions::getColumnString(ui.table, i, "Type");
      summary = "<html>";

      if(type == "Book")
	{
	  summary += "<b>" +
	    misc_functions::getColumnString(ui.table, i, "Title") +
	    "</b>";
	  summary += "<br>";
	  tmpstr = misc_functions::getColumnString(ui.table, i, "ISBN-10");

	  if(tmpstr.isEmpty())
	    tmpstr = misc_functions::getColumnString(ui.table, i, "ID Number");

	  summary += tmpstr;
	  summary += "<br>";
	  summary += misc_functions::getColumnString(ui.table, i,
						     "Publication Date");
	  summary += "<br>";
	  summary += misc_functions::getColumnString(ui.table, i,
						     "Publisher");
	  summary += "<br>";
	}
      else if(type == "CD")
	{
	  summary += "<b>" +
	    misc_functions::getColumnString(ui.table, i, "Title") +
	    "</b>";
	  summary += "<br>";
	  tmpstr = misc_functions::getColumnString(ui.table, i,
						   "Catalog Number");

	  if(tmpstr.isEmpty())
	    tmpstr = misc_functions::getColumnString(ui.table, i, "ID Number");

	  summary += tmpstr;
	  summary += "<br>";
	  tmpstr = misc_functions::getColumnString(ui.table, i,
						   "Publication Date");

	  if(tmpstr.isEmpty())
	    tmpstr = misc_functions::getColumnString(ui.table, i,
						     "Release Date");

	  summary += tmpstr;
	  summary += "<br>";
	  tmpstr = misc_functions::getColumnString(ui.table, i,
						   "Publisher");

	  if(tmpstr.isEmpty())
	    tmpstr = misc_functions::getColumnString(ui.table, i,
						     "Recording Label");

	  summary += tmpstr;
	  summary += "<br>";
	}
      else if(type == "DVD")
	{
	  summary += "<b>" +
	    misc_functions::getColumnString(ui.table, i, "Title") +
	    "</b>";
	  summary += "<br>";
	  tmpstr = misc_functions::getColumnString(ui.table, i,
						   "UPC");

	  if(tmpstr.isEmpty())
	    tmpstr = misc_functions::getColumnString(ui.table, i, "ID Number");

	  summary += tmpstr;
	  summary += "<br>";
	  tmpstr = misc_functions::getColumnString(ui.table, i,
						   "Publication Date");

	  if(tmpstr.isEmpty())
	    tmpstr = misc_functions::getColumnString(ui.table, i,
						     "Release Date");

	  summary += tmpstr;
	  summary += "<br>";
	  tmpstr = misc_functions::getColumnString(ui.table, i,
						   "Publisher");

	  if(tmpstr.isEmpty())
	    tmpstr = misc_functions::getColumnString(ui.table, i,
						     "Studio");

	  summary += tmpstr;
	  summary += "<br>";
	}
      else if(type == "Journal" || type == "Magazine")
	{
	  summary += "<b>" +
	    misc_functions::getColumnString(ui.table, i, "Title") +
	    "</b>";
	  summary += "<br>";
	  tmpstr = misc_functions::getColumnString(ui.table, i, "ISSN");

	  if(tmpstr.isEmpty())
	    tmpstr = misc_functions::getColumnString(ui.table, i, "ID Number");

	  summary += tmpstr;
	  summary += "<br>";
	  summary += misc_functions::getColumnString(ui.table, i,
						     "Publication Date");
	  summary += "<br>";
	  summary += misc_functions::getColumnString(ui.table, i,
						     "Publisher");
	  summary += "<br>";
	}
      else if(type == "Video Game")
	{
	  summary += "<b>" +
	    misc_functions::getColumnString(ui.table, i, "Title") +
	    "</b>";
	  summary += "<br>";
	  tmpstr = misc_functions::getColumnString(ui.table, i, "UPC");

	  if(tmpstr.isEmpty())
	    tmpstr = misc_functions::getColumnString(ui.table, i, "ID Number");

	  summary += tmpstr;
	  summary += "<br>";
	  tmpstr = misc_functions::getColumnString(ui.table, i,
						   "Publication Date");

	  if(tmpstr.isEmpty())
	    tmpstr = misc_functions::getColumnString(ui.table, i,
						     "Release Date");

	  summary += tmpstr;
	  summary += "<br>";
	  summary += misc_functions::getColumnString(ui.table, i,
						     "Publisher");
	  summary += "<br>";
	}

      summary = summary.remove("<br><br>");
      summary += misc_functions::getColumnString(ui.table, i, "Location");
      summary += "</html>";
      ui.summary->setVisible(true);
      ui.summary->setText(summary);
      qapp->setOverrideCursor(Qt::WaitCursor);
      frontImage = misc_functions::getImage(oid, "front_cover", type,
					    getDB());
      backImage = misc_functions::getImage(oid, "back_cover", type,
					   getDB());
      qapp->restoreOverrideCursor();

      if(!frontImage.isNull())
	ui.frontImage->setPixmap(QPixmap().fromImage(frontImage));
      else
	ui.frontImage->clear();

      if(!backImage.isNull())
	ui.backImage->setPixmap(QPixmap().fromImage(backImage));
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
      ui.frontImage->clear();
      ui.backImage->clear();
    }
}

/*
** -- slotShowNext() --
*/

void qtbook::slotShowNext(void)
{
  int row = -1;
  QTableWidget *table = NULL;

  table = bb.table;
  row = table->currentRow();

  if(row == (table->rowCount() - 1))
    row = 0;
  else
    row += 1;

  table->clearSelection();
  table->selectRow(row);

  if(history_diag->isVisible())
    slotShowHistory();
  else
    slotModifyBorrower();
}

/*
** -- slotShowPrev() --
*/

void qtbook::slotShowPrev(void)
{
  int row = -1;
  QTableWidget *table = NULL;

  table = bb.table;
  row = table->currentRow();

  if(row == 0)
    row = table->rowCount() - 1;
  else
    row -= 1;

  table->clearSelection();
  table->selectRow(row);

  if(history_diag->isVisible())
    slotShowHistory();
  else
    slotModifyBorrower();
}

/*
** -- slotShowConnectionDB() --
*/

void qtbook::slotShowConnectionDB(void)
{
  slotBranchChanged();
  branch_diag->show();
}

/*
** -- slotConnectDB() --
*/

void qtbook::slotConnectDB(void)
{
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
      QMessageBox::critical(branch_diag, "BiblioteQ: Database Error",
			    "The selected branch's database type does not "
			    "have a driver associated with it.\n"
			    "The following drivers are available: " +
			    drivers + ".\n"
			    "In addition, Qt expects plugins to exist "
			    "in: " + plugins + ".\n"
			    "Please contact your administrator.");
      return;
    }

  db = QSqlDatabase::addDatabase(str, "Default");

  if(tmphash["database_type"] == "sqlite")
    db.setDatabaseName(br.filename->text());
  else
    {
      db.setHostName(tmphash["hostname"]);
      db.setDatabaseName(br.branch_name->currentText());
      db.setUserName(br.userid->text());
      db.setPassword(br.password->text());
      db.setPort(tmphash["database_port"].toInt());
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
      addError(QString("Database Error"),
	       QString("Unable to open a database connection "
		       "with the provided information."),
	       db.lastError().text(),
	       __FILE__, __LINE__);
      QMessageBox::critical(branch_diag, "BiblioteQ: Database Error",
			    "Unable to open a database "
			    "connection with the provided information.");
    }
  else
    {
      if(!db.driver()->hasFeature(QSqlDriver::Transactions))
	{
	  error = true;
	  addError(QString("Database Error"),
		   QString("The current database driver that you're using "
			   "does not support transactions. "
			   "Please upgrade your database and/or driver."),
		   db.lastError().text(),
		   __FILE__, __LINE__);
	  QMessageBox::critical
	    (branch_diag, "BiblioteQ: Database Error",
	     "The current database driver that you're using "
	     "does not support transactions. "
	     "Please upgrade your database and/or driver.");
	}
    }

  if(tmphash["database_type"] != "sqlite")
    {
      if(!error)
	{
	  qapp->setOverrideCursor(Qt::WaitCursor);
	  roles = misc_functions::getRoles
	    (db, br.userid->text(), errorstr).toLower();
	  qapp->restoreOverrideCursor();

	  if(br.adminCheck->isChecked() && roles.isEmpty())
	    {
	      error = true;
	      QMessageBox::critical
		(branch_diag, "BiblioteQ: User Error",
		 QString("It appears that the user %1 does not have "
			 "administrator privileges.").arg
		 (br.userid->text()));
	    }
	  else if(!br.adminCheck->isChecked() && !roles.isEmpty())
	    {
	      error = true;
	      QMessageBox::critical
		(branch_diag, "BiblioteQ: User Error",
		 "It appears that you are attempting to use an "
		 "administrator login in a non-administrator mode.");
	    }
	}
    }
  else if(!error)
    roles = "administrator";

  tmphash.clear();

  if(error)
    {
      db = QSqlDatabase();
      QSqlDatabase::removeDatabase("Default");
      return;
    }
  else
    branch_diag->close();

  selectedBranch = branches[br.branch_name->currentText()];

  if(connected_bar_label != NULL)
    {
      connected_bar_label->setPixmap(QPixmap("icons.d/16x16/connected.png"));
      connected_bar_label->setToolTip("Connected");
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

  if(selectedBranch["database_type"] == "sqlite")
    {
      ui.actionChangePassword->setEnabled(false);
      ui.actionRememberSQLiteFilename->setEnabled(true);
    }
  else
    {
      ui.actionChangePassword->setEnabled(true);
      ui.actionRememberSQLiteFilename->setEnabled(false);
      connect(ui.table, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this,
	      SLOT(slotViewDetails(void)));
    }

  prepareFilter();

  if(br.adminCheck->isChecked() || selectedBranch["database_type"] == "sqlite")
    adminSetup();
  else
    {
      ui.actionRequests->setToolTip("Request Selected Item(s)");
      ui.actionRequests->setEnabled(true);
      ui.actionReservationHistory->setEnabled(true);
    }

  if(ui.actionPopulateOnStart->isChecked())
    slotRefresh();
}

/*
** -- slotDisconnect() --
*/

void qtbook::slotDisconnect(void)
{
  roles = "";
  all_diag->close();
  members_diag->close();
  history_diag->close();
  customquery_diag->close();
  admin_diag->close();
  resetAdminBrowser();
  resetMembersBrowser();
  ui.actionReservationHistory->setEnabled(false);
  ui.printTool->setEnabled(false);
  ui.actionChangePassword->setEnabled(false);
  ui.deleteTool->setEnabled(false);
  ui.actionDeleteEntry->setEnabled(false);
  ui.createTool->setEnabled(false);
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
  ui.actionRememberSQLiteFilename->setEnabled(false);
  ui.actionConfigureAdministratorPrivileges->setEnabled(false);
  ui.actionRequests->setEnabled(false);
  ui.actionRequests->setToolTip("Item Requests");
  bb.table->disconnect(SIGNAL(itemDoubleClicked(QTableWidgetItem *)));
  ui.table->disconnect(SIGNAL(itemDoubleClicked(QTableWidgetItem *)));

  if(connected_bar_label != NULL)
    {
      connected_bar_label->setPixmap
	(QPixmap("icons.d/16x16/disconnected.png"));
      connected_bar_label->setToolTip("Disconnected");
    }

  if(status_bar_label != NULL)
    {
      status_bar_label->setPixmap(QPixmap("icons.d/16x16/lock.png"));
      status_bar_label->setToolTip("Standard User Mode");
    }

  if(ui.actionResetErrorLogOnDisconnect->isChecked())
    slotResetErrorLog();

  previousTypeFilter = "All";
  ui.table->resetTable("All", roles);
  ui.table->clearHiddenColumnsRecord();
  prepareFilter();
  addConfigOptions("All");
  ui.typefilter->setCurrentIndex(0);
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
  ab.table->setCurrentItem(NULL);
  ab.table->setColumnCount(0);
  ab.table->setRowCount(0);
  ab.table->scrollToTop();
  ab.table->horizontalScrollBar()->setValue(0);
  ab.table->setSortingEnabled(false);
  list.clear();
  list.append("ID");
  list.append("Administrator");
  list.append("Circulation");
  list.append("Librarian");
  list.append("Membership");
  ab.table->setColumnCount(list.size());
  ab.table->setHorizontalHeaderLabels(list);
  list.clear();
}

/*
** -- resetMembersBrowser() --
*/

void qtbook::resetMembersBrowser(void)
{
  QStringList list;

  bb.table->clear();
  bb.table->setCurrentItem(NULL);
  bb.table->setColumnCount(0);
  bb.table->setRowCount(0);
  bb.table->scrollToTop();
  bb.table->horizontalScrollBar()->setValue(0);
  list.append("Member ID");
  list.append("First Name");
  list.append("Last Name");
  list.append("Member Since");
  list.append("Books Reserved");
  list.append("CDs Reserved");
  list.append("DVDs Reserved");
  list.append("Journals Reserved");
  list.append("Magazines Reserved");
  list.append("Video Games Reserved");
  bb.table->setColumnCount(list.size());
  bb.table->setHorizontalHeaderLabels(list);
  list.clear();
  bb.table->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  bb.table->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}

/*
** -- slotShowMembersBrowser() --
*/

void qtbook::slotShowMembersBrowser(void)
{
  bb.filter->clear();
  bb.filterBox->setCheckState(Qt::Checked);
  bb.filtertype->setCurrentIndex(0);
  bb.filter->setFocus();
  bb.table->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);

  if(!members_diag->isVisible())
    {
      members_diag->updateGeometry();
      members_diag->resize(members_diag->sizeHint());
    }

  misc_functions::center(members_diag, this);
  members_diag->raise();
  members_diag->show();
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
  QTableWidgetItem *item = NULL;

  str = "SELECT member.memberid, "
    "member.first_name, "
    "member.last_name, "
    "member.membersince, "
    "COUNT(DISTINCT ib1.item_oid) AS numbooks, "
    "COUNT(DISTINCT ib2.item_oid) AS numcds, "
    "COUNT(DISTINCT ib3.item_oid) AS numdvds, "
    "COUNT(DISTINCT ib4.item_oid) AS numjournals, "
    "COUNT(DISTINCT ib5.item_oid) AS nummagazines, "
    "COUNT(DISTINCT ib6.item_oid) AS numvideogames "
    "FROM member "
    "LEFT JOIN item_borrower ib1 ON "
    "member.memberid = ib1.memberid AND ib1.type = 'Book' "
    "LEFT JOIN item_borrower ib2 ON "
    "member.memberid = ib2.memberid AND ib2.type = 'CD' "
    "LEFT JOIN item_borrower ib3 ON "
    "member.memberid = ib3.memberid AND ib3.type = 'DVD' "
    "LEFT JOIN item_borrower ib4 ON "
    "member.memberid = ib4.memberid AND ib4.type = 'Journal' "
    "LEFT JOIN item_borrower ib5 ON "
    "member.memberid = ib5.memberid AND ib5.type = 'Magazine' "
    "LEFT JOIN item_borrower ib6 ON "
    "member.memberid = ib6.memberid AND ib6.type = 'Video Game' ";

  if(bb.filterBox->isChecked())
    {
      str.append("WHERE ");

      if(bb.filtertype->currentText() == "Member ID")
	str.append("member.memberid LIKE '%");
      else
	str.append("LOWER(member.last_name) LIKE '%");

      str.append(myqstring::escape(bb.filter->text().trimmed()));
      str.append("%' ");
    }

  str.append("GROUP BY "
	     "member.memberid, "
	     "member.first_name, "
	     "member.last_name, "
	     "member.membersince ");
  str.append("ORDER BY member.memberid");
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec(str))
    {
      qapp->restoreOverrideCursor();
      addError(QString("Database Error"),
	       QString("Unable to retrieve member data for table "
		       "populating."),
	       query.lastError().text(),
	       __FILE__, __LINE__);
      QMessageBox::critical(members_diag, "BiblioteQ: Database Error",
			    "Unable to retrieve member data for "
			    "table populating.");
      return;
    }

  qapp->restoreOverrideCursor();
  resetMembersBrowser();
  bb.table->setSortingEnabled(false);

  if(selectedBranch["database_type"] != "sqlite")
    bb.table->setRowCount(query.size());

  progress.setModal(true);
  progress.setWindowTitle("BiblioteQ: Progress Dialog");
  progress.setLabelText("Populating the table...");

  if(selectedBranch["database_type"] == "sqlite")
    progress.setMaximum
      (misc_functions::sqliteQuerySize(str, getDB(), __FILE__, __LINE__));
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
	    str = query.value(j).toString();

	    if(str == "0")
	      str = "";

	    if((item = new QTableWidgetItem()) != NULL)
	      {
		item->setText(str);
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

		if(selectedBranch["database_type"] == "sqlite")
		  bb.table->setRowCount(i + 1);

		bb.table->setItem(i, j, item);
	      }
	    else
	      addError(QString("Memory Error"),
		       QString("Unable to allocate memory for the "
			       "\"item\" object. "
			       "This is a serious problem!"),
		       QString(""), __FILE__, __LINE__);
	  }

      progress.setValue(i + 1);
      progress.update();
      qapp->processEvents();
    }

  query.clear();
  bb.table->setSortingEnabled(true);
  bb.table->setRowCount(i);
  bb.table->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
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

  memberid = misc_functions::getColumnString(bb.table, row, "Member ID");
  qapp->setOverrideCursor(Qt::WaitCursor);
  counts = misc_functions::getItemsReservedCounts(db, memberid, errorstr);
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    addError(QString("Database Error"),
	     QString("Unable to determine the number of reserved items "
		     "for the selected member."),
	     errorstr, __FILE__, __LINE__);
  else
    {
      misc_functions::updateColumn(bb.table, row, "Books Reserved",
				   counts["numbooks"]);
      misc_functions::updateColumn(bb.table, row, "CDs Reserved",
				   counts["numcds"]);
      misc_functions::updateColumn(bb.table, row, "DVDs Reserved",
				   counts["numdvds"]);
      misc_functions::updateColumn(bb.table, row, "Journals Reserved",
				   counts["numjournals"]);
      misc_functions::updateColumn(bb.table, row, "Magazines Reserved",
				   counts["nummagazines"]);
      misc_functions::updateColumn(bb.table, row, "Video Games Reserved",
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

  qapp->setOverrideCursor(Qt::WaitCursor);
  counts = misc_functions::getItemsReservedCounts(db, memberid, errorstr);
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    addError(QString("Database Error"),
	     QString("Unable to retrieve the number of reserved items "
		     "by the selected member."),
	     errorstr, __FILE__, __LINE__);
  else
    {
      qapp->setOverrideCursor(Qt::WaitCursor);

      for(i = 0; i < bb.table->rowCount(); i++)
	{
	  str = misc_functions::getColumnString(bb.table, i, "Member ID");

	  if(str == memberid)
	    {
	      misc_functions::updateColumn(bb.table, i, "Books Reserved",
					   counts["numbooks"]);
	      misc_functions::updateColumn(bb.table, i, "CDs Reserved",
					   counts["numcds"]);
	      misc_functions::updateColumn(bb.table, i, "DVDs Reserved",
					   counts["numdvds"]);
	      misc_functions::updateColumn(bb.table, i, "Journals Reserved",
					   counts["numjournals"]);
	      misc_functions::updateColumn(bb.table, i, "Magazines Reserved",
					   counts["nummagazines"]);
	      misc_functions::updateColumn(bb.table, i,
					   "Video Games Reserved",
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
      QMessageBox::critical(members_diag, "BiblioteQ: User Error",
			    "Please select a member to modify.");
      return;
    }

  str = misc_functions::getColumnString(bb.table, row, "Member ID");
  searchstr = "SELECT * FROM member WHERE memberid = '";
  searchstr.append(str);
  searchstr.append("'");
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec(searchstr) || !query.next())
    {
      qapp->restoreOverrideCursor();
      addError(QString("Database Error"),
	       QString("Unable to retrieve the selected member's "
		       "information."),
	       query.lastError().text(),
	       __FILE__, __LINE__);

      if(userinfo_diag->isVisible())
	QMessageBox::critical(userinfo_diag, "BiblioteQ: Database Error",
			      "Unable to retrieve the selected member's "
			      "information.");
      else
	QMessageBox::critical(members_diag, "BiblioteQ: Database Error",
			      "Unable to retrieve the selected member's "
			      "information.");

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
	    userinfo.memberid->setText(var.toString());
	  else if(fieldname == "membersince")
	    userinfo.membersince->setDate
	      (QDate::fromString(var.toString(), "MM/dd/yyyy"));
	  else if(fieldname == "dob")
	    userinfo.dob->setDate
	      (QDate::fromString(var.toString(), "MM/dd/yyyy"));
	  else if(fieldname == "sex")
	    userinfo.sex->setCurrentIndex
	      (userinfo.sex->findText(var.toString()));
	  else if(fieldname == "first_name")
	    userinfo.firstName->setText(var.toString());
	  else if(fieldname == "middle_init")
	    userinfo.middle->setText(var.toString());
	  else if(fieldname == "last_name")
	    userinfo.lastName->setText(var.toString());
	  else if(fieldname == "telephone_num")
	    userinfo.telephoneNumber->setText(var.toString());
	  else if(fieldname == "street")
	    userinfo.street->setText(var.toString());
	  else if(fieldname == "city")
	    userinfo.city->setText(var.toString());
	  else if(fieldname == "state_abbr")
	    {
	      if(userinfo.state->findText(var.toString()) == -1)
		userinfo.state->setCurrentIndex(0);
	      else
		userinfo.state->setCurrentIndex
		  (userinfo.state->findText(var.toString()));
	    }
	  else if(fieldname == "zip")
	    userinfo.zip->setText(var.toString());
	}

      foreach(QLineEdit *textfield,
	      userinfo_diag->findChildren<QLineEdit *>())
	textfield->setCursorPosition(0);
    }

  userinfo.prevTool->setVisible(true);
  userinfo.nextTool->setVisible(true);
  userinfo_diag->setWindowTitle("BiblioteQ: Modify Member");
  userinfo.membersince->setMaximumDate(QDate::currentDate());
  userinfo.membersince->setFocus();
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
  QString itemTitle = "";
  QString realItemType = "";
  copy_editor *copyeditor = NULL;
  qtbook_item *item = NULL;

  if(row2 > -1)
    {
      /*
      ** Is the item available?
      */

      oid = misc_functions::getColumnString
	(ui.table, row2, "OID");
      type = misc_functions::getColumnString(ui.table, row2, "Type");
      qapp->setOverrideCursor(Qt::WaitCursor);
      availability = misc_functions::getAvailability
	(oid, db, type, errorstr).toInt();
      qapp->restoreOverrideCursor();

      if(!errorstr.isEmpty())
	{
	  addError(QString("Database Error"),
		   QString("Unable to determine the availability of "
			   "the selected item."),
		   errorstr, __FILE__, __LINE__);
	  return;
	}

      if(availability < 1)
	{
	  QMessageBox::critical(members_diag, "BiblioteQ: User Error",
				"It appears that the item you selected "
				"is no longer available for reservation.");
	  return;
	}
    }

  if(row1 < 0 || row2 < 0)
    {
      QMessageBox::critical(members_diag, "BiblioteQ: User Error",
			    "Please select a member and an item "
			    "to complete the reservation process.");
      return;
    }
  else
    {
      if((item = new qtbook_item(row2)) != NULL)
	{
	  realItemType = type;
	  quantity = misc_functions::getColumnString(ui.table, row2,
						     "Quantity").toInt();
	  itemTitle = misc_functions::getColumnString(ui.table, row2,
						      "Title");

	  if(type.toLower() == "book")
	    itemid = misc_functions::getColumnString(ui.table, row2,
						     "ISBN-10");
	  else if(type.toLower() == "dvd")
	    itemid = misc_functions::getColumnString(ui.table, row2, "UPC");
	  else if(type.toLower() == "journal" || type.toLower() == "magazine")
	    itemid = misc_functions::getColumnString(ui.table, row2, "ISSN");
	  else if(type.toLower() == "cd")
	    itemid = misc_functions::getColumnString(ui.table, row2,
						     "Catalog Number");
	  else if(type.toLower() == "video game")
	    itemid = misc_functions::getColumnString(ui.table, row2, "UPC");

	  if(itemid.isEmpty())
	    itemid = misc_functions::getColumnString(ui.table, row2,
						     "ID Number");

	  if(itemid.isEmpty())
	    itemid = misc_functions::getColumnString(ui.table, row2, "id");

	  if(type.toLower() == "journal")
	    type = "Magazine";

	  if(itemid.isEmpty())
	    QMessageBox::critical(members_diag, "BiblioteQ: User Error",
				  "Unable to determine the selected item's "
				  "type.");
	  else if((copyeditor = new copy_editor(members_diag, item, true,
						quantity, oid, itemid,
						(QSpinBox *) NULL,
						font(), type,
						itemTitle,
						realItemType)) != NULL)
	    copyeditor->populateCopiesEditor();
	}
    }
}

/*
** -- slotAutoPopOnFilter() --
*/

void qtbook::slotAutoPopOnFilter(void)
{
  /*
  ** Populate the main table only if we're connected to a database.
  */

  if(db.isOpen() && ui.actionAutoPopulateOnFilter->isChecked())
    slotRefresh();
}

/*
** -- slotReset() --
*/

void qtbook::slotReset(void)
{
  QAction *action = qobject_cast<QAction *>(sender());
  QString name = "";

  if(action != NULL)
    {
      name = action->text();

      if(all_diag->isVisible())
	{
	  if(name.contains("ID Number"))
	    {
	      al.idnumber->clear();
	      al.idnumber->setFocus();
	    }
	  else if(name.contains("Title"))
	    {
	      al.title->clear();
	      al.title->setFocus();
	    }
	  else if(name.contains("Release Date"))
	    {
	      al.publication_date->setDate
		(QDate::fromString("01/01/7999", "MM/dd/yyyy"));
	      al.publication_date->setFocus();
	    }
	  else if(name.contains("Publisher"))
	    {
	      al.publisher->clear();
	      al.publisher->setFocus();
	    }
	  else if(name.contains("Category"))
	    {
	      al.category->setCurrentIndex(0);
	      al.category->setFocus();
	    }
	  else if(name.contains("Price"))
	    {
	      al.price->setValue(0.00);
	      al.price->setFocus();
	    }
	  else if(name.contains("Language"))
	    {
	      al.language->setCurrentIndex(0);
	      al.language->setFocus();
	    }
	  else if(name.contains("Monetary Units"))
	    {
	      al.monetary_units->setCurrentIndex(0);
	      al.monetary_units->setFocus();
	    }
	  else if(name.contains("Abstract"))
	    {
	      al.description->clear();
	      al.description->setFocus();
	    }
	  else if(name.contains("Copies"))
	    {
	      al.quantity->setValue(0);
	      al.quantity->setFocus();
	    }
	  else if(name.contains("Location"))
	    {
	      al.location->setCurrentIndex(0);
	      al.location->setFocus();
	    }
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
  QTableWidgetItem *item = NULL;

  if(error_bar_label != NULL)
    {
      error_bar_label->setPixmap(QPixmap("icons.d/16x16/log.png"));
      error_bar_label->setToolTip("Error Log Active");
    }

  er.table->setSortingEnabled(false);
  er.table->setRowCount(er.table->rowCount() + 1);

  for(i = 0; i < 6; i++)
    if((item = new QTableWidgetItem()) != NULL)
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
      cerr << "Memory failure in addError()!" << endl;

  er.table->resizeColumnsToContents();
  er.table->setSortingEnabled(true);
}

/*
** -- resetErrorLog() --
*/

void qtbook::slotResetErrorLog(void)
{
  QStringList list;

  list.append("Event Time");
  list.append("Event Type");
  list.append("Summary");
  list.append("Full Description");
  list.append("File");
  list.append("Line Number");
  er.table->clear();
  er.table->setCurrentItem(NULL);
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

  if(error_bar_label != NULL)
    {
      error_bar_label->setPixmap(QPixmap("icons.d/16x16/ok.png"));
      error_bar_label->setToolTip("Empty Error Log");
    }
}

/*
** -- getBB() --
*/

Ui_membersBrowser qtbook::getBB(void)
{
  return bb;
}

/*
** -- getUI() --
*/

Ui_mainWindow qtbook::getUI(void)
{
  return ui;
}

/*
** -- getDB() --
*/

QSqlDatabase qtbook::getDB(void)
{
  return db;
}

/*
** -- removeCD() --
*/

void qtbook::removeCD(qtbook_cd *cd)
{
  if(cd != NULL)
    {
      if(cds.contains(cd->getID()))
	cds.remove(cd->getID());

      cd->deleteLater();
    }
}

/*
** -- replaceCD() --
*/

void qtbook::replaceCD(const QString &id, qtbook_cd *cd)
{
  if(cds.contains(id))
    cds.remove(id);

  if(cd != NULL)
    cds.insert(cd->getID(), cd);
}

/*
** -- removeDVD() --
*/

void qtbook::removeDVD(qtbook_dvd *dvd)
{
  if(dvd != NULL)
    {
      if(dvds.contains(dvd->getID()))
	dvds.remove(dvd->getID());

      dvd->deleteLater();
    }
}

/*
** -- replaceDVD() --
*/

void qtbook::replaceDVD(const QString &id, qtbook_dvd *dvd)
{
  if(dvds.contains(id))
    dvds.remove(id);

  if(dvd != NULL)
    dvds.insert(dvd->getID(), dvd);
}

/*
** -- removeBook() --
*/

void qtbook::removeBook(qtbook_book *book)
{
  if(book != NULL)
    {
      if(books.contains(book->getID()))
	books.remove(book->getID());

      book->deleteLater();
    }
}

/*
** -- replaceBook() --
*/

void qtbook::replaceBook(const QString &id, qtbook_book *book)
{
  if(books.contains(id))
    books.remove(id);

  if(book != NULL)
    books.insert(book->getID(), book);
}

/*
** -- removeJournal() --
*/

void qtbook::removeJournal(qtbook_journal *journal)
{
  if(journal != NULL)
    {
      if(journals.contains(journal->getID()))
	journals.remove(journal->getID());

      journal->deleteLater();
    }
}

/*
** -- removeMagazine() --
*/

void qtbook::removeMagazine(qtbook_magazine *magazine)
{
  if(magazine != NULL)
    {
      if(magazines.contains(magazine->getID()))
	magazines.remove(magazine->getID());

      magazine->deleteLater();
    }
}

/*
** -- replaceJournal() --
*/

void qtbook::replaceJournal(const QString &id, qtbook_journal *journal)
{
  if(journals.contains(id))
    journals.remove(id);

  if(journal != NULL)
    journals.insert(journal->getID(), journal);
}

/*
** -- replaceMagazine() --
*/

void qtbook::replaceMagazine(const QString &id, qtbook_magazine *magazine)
{
  if(magazines.contains(id))
    magazines.remove(id);

  if(magazine != NULL)
    magazines.insert(magazine->getID(), magazine);
}

/*
** -- removeVideoGame() --
*/

void qtbook::removeVideoGame(qtbook_videogame *video_game)
{
  if(video_game != NULL)
    {
      if(video_games.contains(video_game->getID()))
	video_games.remove(video_game->getID());

      video_game->deleteLater();
    }
}

/*
** -- replaceVideoGame() --
*/

void qtbook::replaceVideoGame(const QString &id,
			      qtbook_videogame *video_game)
{
  if(video_games.contains(id))
    video_games.remove(id);

  if(video_game != NULL)
    video_games.insert(video_game->getID(), video_game);
}

/*
** -- updateItemWindows() --
*/

void qtbook::updateItemWindows(void)
{
  /*
  ** This method is no longer needed.
  */

  QHash<QString, qtbook_cd *>::const_iterator cdit;
  QHash<QString, qtbook_dvd *>::const_iterator dvdit;
  QHash<QString, qtbook_book *>::const_iterator bookit;
  QHash<QString, qtbook_journal *>::const_iterator journalit;
  QHash<QString, qtbook_magazine *>::const_iterator magazineit;
  QHash<QString, qtbook_videogame *>::const_iterator videogameit;

  qapp->setOverrideCursor(Qt::WaitCursor);
  cdit = cds.constBegin();

  while(cdit != cds.constEnd())
    {
      if(cdit.value()->isVisible())
	cdit.value()->updateWindow(EDITABLE);

      ++cdit;
    }

  dvdit = dvds.constBegin();

  while(dvdit != dvds.constEnd())
    {
      if(dvdit.value()->isVisible())
	dvdit.value()->updateWindow(EDITABLE);

      ++dvdit;
    }

  bookit = books.constBegin();

  while(bookit != books.constEnd())
    {
      if(bookit.value()->isVisible())
	bookit.value()->updateWindow(EDITABLE);

      ++bookit;
    }

  journalit = journals.constBegin();

  while(journalit != journals.constEnd())
    {
      if(journalit.value()->isVisible())
	journalit.value()->updateWindow(EDITABLE);

      ++journalit;
    }

  magazineit = magazines.constBegin();

  while(magazineit != magazines.constEnd())
    {
      if(magazineit.value()->isVisible())
	magazineit.value()->updateWindow(EDITABLE);

      ++magazineit;
    }

  videogameit = video_games.constBegin();

  while(videogameit != video_games.constEnd())
    {
      if(videogameit.value()->isVisible())
	videogameit.value()->updateWindow(EDITABLE);

      ++videogameit;
    }

  qapp->restoreOverrideCursor();
}

/*
** -- emptyContainers() --
*/

void qtbook::emptyContainers(void)
{
  QHash<QString, qtbook_cd *>::const_iterator cdit;
  QHash<QString, qtbook_dvd *>::const_iterator dvdit;
  QHash<QString, qtbook_book *>::const_iterator bookit;
  QHash<QString, qtbook_journal *>::const_iterator journalit;
  QHash<QString, qtbook_magazine *>::const_iterator magazineit;
  QHash<QString, qtbook_videogame *>::const_iterator videogameit;

  qapp->setOverrideCursor(Qt::WaitCursor);
  cdit = cds.constBegin();

  while(cdit != cds.constEnd())
    {
      delete cdit.value();
      ++cdit;
    }

  cds.clear();
  dvdit = dvds.constBegin();

  while(dvdit != dvds.constEnd())
    {
      delete dvdit.value();
      ++dvdit;
    }

  dvds.clear();
  bookit = books.constBegin();

  while(bookit != books.constEnd())
    {
      delete bookit.value();
      ++bookit;
    }

  books.clear();
  journalit = journals.constBegin();

  while(journalit != journals.constEnd())
    {
      delete journalit.value();
      ++journalit;
    }

  journals.clear();
  magazineit = magazines.constBegin();

  while(magazineit != magazines.constEnd())
    {
      delete magazineit.value();
      ++magazineit;
    }

  magazines.clear();
  videogameit = video_games.constBegin();

  while(videogameit != video_games.constEnd())
    {
      delete videogameit.value();
      ++videogameit;
    }

  video_games.clear();
  qapp->restoreOverrideCursor();
}

/*
** -- getAdminID() --
*/

QString qtbook::getAdminID(void)
{
  if(selectedBranch["database_type"] != "sqlite")
    return br.userid->text();
  else
    return "N/A";
}

/*
** -- slotInsertCD() --
*/

void qtbook::slotInsertCD(void)
{
  qtbook_cd *cd = NULL;

  if((cd = cds.value("insert")) == NULL)
    {
      if((cd = new qtbook_cd(this, cd_categories, languages,
			     monetary_units, cd_locations,
			     cd_formats, "insert", -1)) != NULL)
	cds.insert("insert", cd);

      if(cd != NULL)
	cd->insert();
    }
  else
    {
      cd->insert();
      cd->raise();
    }
}

/*
** -- slotInsertDVD() --
*/

void qtbook::slotInsertDVD(void)
{
  qtbook_dvd *dvd = NULL;

  if((dvd = dvds.value("insert")) == NULL)
    {
      if((dvd = new qtbook_dvd(this, dvd_categories, languages,
			       monetary_units, dvd_locations,
			       dvd_ratings, dvd_aspectratios,
			       dvd_regions, "insert", -1)) != NULL)
	dvds.insert("insert", dvd);

      if(dvd != NULL)
	dvd->insert();
    }
  else
    {
      dvd->insert();
      dvd->raise();
    }
}

/*
** -- slotInsertBook() --
*/

void qtbook::slotInsertBook(void)
{
  qtbook_book *book = NULL;

  if((book = books.value("insert")) == NULL)
    {
      if((book = new qtbook_book(this, book_categories, languages,
				 monetary_units, book_locations,
				 "insert", -1)) != NULL)
	books.insert("insert", book);

      if(book != NULL)
	book->insert();
    }
  else
    {
      book->insert();
      book->raise();
    }
}

/*
** -- slotInsertJourn() --
*/

void qtbook::slotInsertJourn(void)
{
  qtbook_journal *journal = NULL;

  if((journal = journals.value("insert")) == NULL)
    {
      if((journal = new qtbook_journal(this, journal_categories,
				       languages,
				       monetary_units, journal_locations,
				       "insert", -1)) != NULL)
	journals.insert("insert", journal);

      if(journal != NULL)
	journal->insert();
    }
  else
    {
      journal->insert();
      journal->raise();
    }
}

/*
** -- slotInsertMag() --
*/

void qtbook::slotInsertMag(void)
{
  qtbook_magazine *magazine = NULL;

  if((magazine = magazines.value("insert")) == NULL)
    {
      if((magazine = new qtbook_magazine(this, magazine_categories,
					 languages,
					 monetary_units, magazine_locations,
					 "insert", -1)) != NULL)
	magazines.insert("insert", magazine);

      if(magazine != NULL)
	magazine->insert();
    }
  else
    {
      magazine->insert();
      magazine->raise();
    }
}

/*
** -- slotInsertVideoGame() --
*/

void qtbook::slotInsertVideoGame(void)
{
  qtbook_videogame *video_game = NULL;

  if((video_game = video_games.value("insert")) == NULL)
    {
      if((video_game = new qtbook_videogame(this, vg_ratings, vg_platforms,
					    vg_genres, languages,
					    monetary_units, vg_locations,
					    "insert", -1)) != NULL)
	video_games.insert("insert", video_game);

      if(video_game != NULL)
	video_game->insert();
    }
  else
    {
      video_game->insert();
      video_game->raise();
    }
}

/*
** -- deleteItem() --
*/

void qtbook::deleteItem(const QString &oid, const QString &itemType)
{
  QObject *item = NULL;

  if(itemType == "cd")
    {
      if((item = cds.value(oid)) != NULL)
	removeCD(qobject_cast<qtbook_cd *>(item));
    }
  else if(itemType == "dvd")
    {
      if((item = dvds.value(oid)) != NULL)
	removeDVD(qobject_cast<qtbook_dvd *>(item));
    }
  else if(itemType == "book")
    {
      if((item = books.value(oid)) != NULL)
	removeBook(qobject_cast<qtbook_book *>(item));
    }
  else if(itemType == "journal")
    {
      if((item = journals.value(oid)) != NULL)
	removeJournal(qobject_cast<qtbook_journal *>(item));
    }
  else if(itemType == "magazine")
    {
      if((item = magazines.value(oid)) != NULL)
	removeMagazine(qobject_cast<qtbook_magazine *>(item));
    }
  else if(itemType == "videogame")
    {
      if((item = video_games.value(oid)) != NULL)
	removeVideoGame(qobject_cast<qtbook_videogame *>(item));
    }
}

/*
** -- bookSearch() --
*/

void qtbook::bookSearch(const QString &field, const QString &value)
{
  qtbook_book *book = NULL;

  if((book = books.value("search")) == NULL)
    {
      if((book = new qtbook_book(this, book_categories, languages,
				 monetary_units, book_locations,
				 "search", -1)) != NULL)
	books.insert("search", book);

      if(book != NULL)
	book->search(field, value);
    }
  else
    book->search(field, value);
}

/*
** -- slotBookSearch() --
*/

void qtbook::slotBookSearch(void)
{
  qtbook_book *book = NULL;

  if((book = books.value("search")) == NULL)
    {
      if((book = new qtbook_book(this, book_categories, languages,
				 monetary_units, book_locations,
				 "search", -1)) != NULL)
	books.insert("search", book);

      if(book != NULL)
	book->search();
    }
  else
    {
      book->raise();
      book->search();
    }
}

/*
** -- cdSearch() --
*/

void qtbook::cdSearch(const QString &field, const QString &value)
{
  qtbook_cd *cd = NULL;

  if((cd = cds.value("search")) == NULL)
    {
      if((cd = new qtbook_cd(this, cd_categories, languages,
			     monetary_units, cd_locations, cd_formats,
			     "search", -1)) != NULL)
	cds.insert("search", cd);

      if(cd != NULL)
	cd->search(field, value);
    }
  else
    cd->search(field, value);
}

/*
** -- slotCDSearch() --
*/

void qtbook::slotCDSearch(void)
{
  qtbook_cd *cd = NULL;

  if((cd = cds.value("search")) == NULL)
    {
      if((cd = new qtbook_cd(this, cd_categories, languages,
			     monetary_units, cd_locations, cd_formats,
			     "search", -1)) != NULL)
	cds.insert("search", cd);

      if(cd != NULL)
	cd->search();
    }
  else
    {
      cd->raise();
      cd->search();
    }
}

/*
** -- dvdSearch() --
*/

void qtbook::dvdSearch(const QString &field, const QString &value)
{
  qtbook_dvd *dvd = NULL;

  if((dvd = dvds.value("search")) == NULL)
    {
      if((dvd = new qtbook_dvd(this, dvd_categories, languages,
			       monetary_units, dvd_locations, dvd_ratings,
			       dvd_aspectratios, dvd_regions,
			       "search", -1)) != NULL)
	dvds.insert("search", dvd);

      if(dvd != NULL)
	dvd->search(field, value);
    }
  else
    dvd->search(field, value);
}

/*
** -- slotDVDSearch() --
*/

void qtbook::slotDVDSearch(void)
{
  qtbook_dvd *dvd = NULL;

  if((dvd = dvds.value("search")) == NULL)
    {
      if((dvd = new qtbook_dvd(this, dvd_categories, languages,
			       monetary_units, dvd_locations, dvd_ratings,
			       dvd_aspectratios, dvd_regions,
			       "search", -1)) != NULL)
	dvds.insert("search", dvd);

      if(dvd != NULL)
	dvd->search();
    }
  else
    {
      dvd->raise();
      dvd->search();
    }
}

/*
** -- journSearch() --
*/

void qtbook::journSearch(const QString &field, const QString &value)
{
  qtbook_journal *journal = NULL;

  if((journal = journals.value("search")) == NULL)
    {
      if((journal = new qtbook_journal(this, journal_categories, languages,
				       monetary_units, journal_locations,
				       "search", -1)) != NULL)
	journals.insert("search", journal);

      if(journal != NULL)
	journal->search(field, value);
    }
  else
    journal->search(field, value);
}

/*
** -- slotJournSearch() --
*/

void qtbook::slotJournSearch(void)
{
  qtbook_journal *journal = NULL;

  if((journal = journals.value("search")) == NULL)
    {
      if((journal = new qtbook_journal(this, journal_categories, languages,
				       monetary_units, journal_locations,
				       "search", -1)) != NULL)
	journals.insert("search", journal);

      if(journal != NULL)
	journal->search();
    }
  else
    {
      journal->raise();
      journal->search();
    }
}

/*
** -- magSearch() --
*/

void qtbook::magSearch(const QString &field, const QString &value)
{
  qtbook_magazine *magazine = NULL;

  if((magazine = magazines.value("search")) == NULL)
    {
      if((magazine = new qtbook_magazine(this, magazine_categories, languages,
					 monetary_units, magazine_locations,
					 "search", -1)) != NULL)
	magazines.insert("search", magazine);

      if(magazine != NULL)
	magazine->search(field, value);
    }
  else
    magazine->search(field, value);
}

/*
** -- slotMagSearch() --
*/

void qtbook::slotMagSearch(void)
{
  qtbook_magazine *magazine = NULL;

  if((magazine = magazines.value("search")) == NULL)
    {
      if((magazine = new qtbook_magazine(this, magazine_categories, languages,
					 monetary_units, magazine_locations,
					 "search", -1)) != NULL)
	magazines.insert("search", magazine);

      if(magazine != NULL)
	magazine->search();
    }
  else
    {
      magazine->raise();
      magazine->search();
    }
}

/*
** -- vgSearch() --
*/

void qtbook::vgSearch(const QString &field, const QString &value)
{
  qtbook_videogame *video_game = NULL;

  if((video_game = video_games.value("search")) == NULL)
    {
      if((video_game = new qtbook_videogame(this, vg_ratings, vg_platforms,
					    vg_genres, languages,
					    monetary_units, vg_locations,
					    "search", -1)) != NULL)
	video_games.insert("search", video_game);

      if(video_game != NULL)
	video_game->search(field, value);
    }
  else
    video_game->search(field, value);
}

/*
** -- slotVideoGameSearch() --
*/

void qtbook::slotVideoGameSearch(void)
{
  qtbook_videogame *video_game = NULL;

  if((video_game = video_games.value("search")) == NULL)
    {
      if((video_game = new qtbook_videogame(this, vg_ratings, vg_platforms,
					    vg_genres, languages,
					    monetary_units, vg_locations,
					    "search", -1)) != NULL)
	video_games.insert("search", video_game);

      if(video_game != NULL)
	video_game->search();
    }
  else
    {
      video_game->raise();
      video_game->search();
    }
}

/*
** -- updateRows() --
*/

void qtbook::updateRows(const QString &oid, const int row,
			const QString &itemType)
{
  QObject *item = NULL;

  if(itemType == "cd")
    {
      if((item = cds.value(oid)) != NULL)
	(qobject_cast<qtbook_cd *>(item))->updateRow(row);
    }
  else if(itemType == "dvd")
    {
      if((item = dvds.value(oid)) != NULL)
	(qobject_cast<qtbook_dvd *>(item))->updateRow(row);
    }
  else if(itemType == "book")
    {
      if((item = books.value(oid)) != NULL)
	(qobject_cast<qtbook_book *>(item))->updateRow(row);
    }
  else if(itemType == "journal")
    {
      if((item = journals.value(oid)) != NULL)
	(qobject_cast<qtbook_journal *>(item))->updateRow(row);
    }
  else if(itemType == "magazine")
    {
      if((item = magazines.value(oid)) != NULL)
	(qobject_cast<qtbook_magazine *>(item))->updateRow(row);
    }
  else if(itemType == "videogame")
    {
      if((item = video_games.value(oid)) != NULL)
	(qobject_cast<qtbook_videogame *>(item))->updateRow(row);
    }
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
      QMessageBox::critical(members_diag, "BiblioteQ: User Error",
			    "In order to list a member's reserved items, "
			    "you must first select the member.");
      return;
    }

  members_diag->close();
  memberid = misc_functions::getColumnString(bb.table, row, "Member ID");
  (void) populateTable(POPULATE_ALL, "All Reserved", memberid);
}

/*
** -- slotListOverdueItems() --
*/

void qtbook::slotListOverdueItems(void)
{
  int row = bb.table->currentRow();
  QString memberid = "";

  if(members_diag->isVisible())
    {
      members_diag->close();
      memberid = misc_functions::getColumnString(bb.table, row, "Member ID");
    }
  else if(roles.isEmpty())
    memberid = br.userid->text();

  (void) populateTable(POPULATE_ALL, "All Overdue", memberid);
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
	QMessageBox::critical(members_diag, "BiblioteQ: User Error",
			      "In order to reserve an item, you must "
			      "first select it.");
      else
	QMessageBox::critical(this, "BiblioteQ: User Error",
			      "In order to reserve an item, you must "
			      "first select it.");

      return;
    }

  oid = misc_functions::getColumnString(ui.table, row, "OID");
  type = misc_functions::getColumnString(ui.table, row, "Type");
  qapp->setOverrideCursor(Qt::WaitCursor);
  availability = misc_functions::getAvailability
    (oid, db, type, errorstr).toInt();
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    {
      addError(QString("Database Error"),
	       QString("Unable to determine the availability of "
		       "the selected item."),
	       errorstr, __FILE__, __LINE__);
      QMessageBox::critical(this, "BiblioteQ: Database Error",
			    "Unable to determine the availability of "
			    "the selected item.");
      return;
    }

  if(availability < 1)
    {
      if(members_diag->isVisible())
	QMessageBox::critical(members_diag, "BiblioteQ: User Error",
			      "It appears that the item you selected "
			      "is no longer available for reservation.");
      else
	QMessageBox::critical(this, "BiblioteQ: User Error",
			      "It appears that the item you selected "
			      "is no longer available for reservation.");

      return;
    }

  slotShowMembersBrowser();

  if(QMessageBox::question(members_diag, "BiblioteQ: Question",
			   "Would you like to retrieve the list of "
			   "members?",
			   QMessageBox::Yes | QMessageBox::No,
			   QMessageBox::No) == QMessageBox::Yes)
    slotPopulateMembersBrowser();

  bb.table->selectRow(0);
}

/*
** -- getLOCHash() --
*/

QHash<QString, QString> qtbook::getLOCHash(void)
{
  return LOCHash;
}

/*
** -- isItemBusy() --
*/

bool qtbook::isItemBusy(const QString &oid, const QString &itemType)
{
  if(itemType == "book")
    {
      if(books.contains(oid))
	return books.value(oid)->isBusy();
    }
  else if(itemType == "journal")
    {
      if(journals.contains(oid))
	return journals.value(oid)->isBusy();
    }
  else if(itemType == "magazine")
    {
      if(magazines.contains(oid))
	return magazines.value(oid)->isBusy();
    }

  return false;
}

/*
** -- slotShowMenu() --
*/

void qtbook::slotShowMenu(void)
{
  QAction *action = qobject_cast<QAction *>(sender());

  action->menu()->exec(QCursor::pos());
}

/*
** -- slotSetFonts() --
*/

void qtbook::slotSetFonts(void)
{
  bool ok = false;
  QFont font = QFontDialog::getFont(&ok, this);

  if(ok)
    setGlobalFonts(font);
}

/*
** -- setGlobalFonts() --
*/

void qtbook::setGlobalFonts(const QFont &font)
{
  qapp->setOverrideCursor(Qt::WaitCursor);
  qapp->setFont(font);

  foreach(QWidget *widget, qapp->allWidgets())
    widget->setFont(font);

  qmain->menuBar()->setFont(font);

  foreach(QMenu *menu, qmain->menuBar()->findChildren<QMenu *>())
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
  QTreeWidgetItem *item1 = NULL;
  QTreeWidgetItem *item2 = NULL;

  if(cq.tables_t->columnCount() == 0)
    {
      qapp->setOverrideCursor(Qt::WaitCursor);
      list << "book"
	   << "book_copy_info"
	   << "cd"
	   << "cd_songs"
	   << "cd_copy_info"
	   << "dvd"
	   << "dvd_copy_info"
	   << "magazine"
	   << "magazine_copy_info"
	   << "videogame"
	   << "videogame_copy_info";
      list.sort();
      cq.tables_t->setColumnCount(3);
      cq.tables_t->setHeaderLabels(QStringList()
				   << "Table Name"
				   << "Column"
				   << "Column Type");


      for(i = 0; i < list.size(); i++)
	if((item1 = new QTreeWidgetItem(cq.tables_t)) != NULL)
	  {
	    item1->setText(0, list[i]);
	    rec = getDB().record(list[i]);

	    for(j = 0; j < rec.count(); j++)
	      {
		if((item2 = new QTreeWidgetItem(item1)) == NULL)
		  {
		    addError(QString("Memory Error"),
			     QString("Unable to allocate "
				     "memory for the \"item2\" "
				     "object. "
				     "This is a serious "
				     "problem!"), QString(""),
			     __FILE__, __LINE__);
		    continue;
		  }

		field = rec.field(rec.fieldName(j));
		item2->setText(1, rec.fieldName(j));
		item2->setText(2, QVariant::typeToName(field.type()));
	      }
	  }
	else
	  addError(QString("Memory Error"),
		   QString("Unable to allocate "
			   "memory for the \"item1\" "
			   "object. "
			   "This is a serious "
			   "problem!"), QString(""),
		   __FILE__, __LINE__);

      for(i = 0; i < cq.tables_t->columnCount(); i++)
	{
	  cq.tables_t->header()->setResizeMode
	    (i, QHeaderView::ResizeToContents);
	  cq.tables_t->resizeColumnToContents(i);
	}

      qapp->restoreOverrideCursor();
    }

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
      QMessageBox::critical(customquery_diag, "BiblioteQ: User Error",
			    "Please provide a valid SQL statement.");
      return;
    }

  if(querystr.toLower().contains("alter") ||
     querystr.toLower().contains("create") ||
     querystr.toLower().contains("delete") ||
     querystr.toLower().contains("drop") ||
     querystr.toLower().contains("insert") ||
     querystr.toLower().contains("update"))
    {
      QMessageBox::critical(customquery_diag, "BiblioteQ: User Error",
			    "Please provide a non-destructive SQL "
			    "statement.");
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
** -- slotPrintSelected() --
*/

void qtbook::slotPrintSelected(void)
{
  int i = 0;
  int j = 0;
  QString html = "<html>";
  QPrinter printer;
  QModelIndex index;
  QPrintDialog dialog(&printer, this);
  QTextDocument document;
  QModelIndexList list = ui.table->selectionModel()->selectedRows();

  if(list.isEmpty())
    {
      QMessageBox::critical(this, "BiblioteQ: User Error",
			    "Please select at least one item to print.");
      return;
    }

  qapp->setOverrideCursor(Qt::WaitCursor);

  foreach(index, list)
    {
      i = index.row();

      for(j = 0; j < ui.table->columnCount(); j++)
	if(!ui.table->isColumnHidden(j))
	  html += "<b>" + ui.table->horizontalHeaderItem(j)->text() +
	    ":</b> " + ui.table->item(i, j)->text() + "<br>";

      html = html.trimmed();
      html += "<br>";
    }

  html = html.trimmed();
  html += "</html>";
  list.clear();
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

  if(row < 0)
    {
      QMessageBox::critical(members_diag, "BiblioteQ: User Error",
			    "In order to print a member's reserved items, "
			    "you must first select the member.");
      return;
    }

  qapp->setOverrideCursor(Qt::WaitCursor);
  itemsReserved = misc_functions::getColumnString
    (bb.table, row, "Books Reserved").toInt() +
    misc_functions::getColumnString
    (bb.table, row, "CDs Reserved").toInt() +
    misc_functions::getColumnString
    (bb.table, row, "DVDs Reserved").toInt() +
    misc_functions::getColumnString
    (bb.table, row, "Journals Reserved").toInt() +
    misc_functions::getColumnString
    (bb.table, row, "Magazines Reserved").toInt() +
    misc_functions::getColumnString
    (bb.table, row, "Video Games Reserved").toInt();
  qapp->restoreOverrideCursor();

  if(itemsReserved < 1)
    {
      QMessageBox::critical(members_diag, "BiblioteQ: User Error",
			    "The member that you selected does not have "
			    "any reserved items.");
      return;
    }

  memberid = misc_functions::getColumnString(bb.table, row, "Member ID");
  memberinfo["firstname"] = misc_functions::getColumnString(bb.table, row,
							    "First Name");
  memberinfo["lastname"] = misc_functions::getColumnString(bb.table, row,
							   "Last Name");
  qapp->setOverrideCursor(Qt::WaitCursor);
  itemsList = misc_functions::getReservedItems(memberid, getDB(), errorstr);
  qapp->restoreOverrideCursor();

  if(errorstr.isEmpty())
    {
      str = "<html>\n";
      str += "Reserved Items for " + memberinfo["lastname"] +
	", " + memberinfo["firstname"] + "<br><br>";

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
      addError(QString("Database Error"),
	       QString("Unable to determine the reserved items for "
		       "the selected member."),
	       errorstr, __FILE__, __LINE__);
      QMessageBox::critical(members_diag, "BiblioteQ: Database Error",
			    "Unable to determine the reserved items for "
			    "the selected member.");
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
      QMessageBox::critical(error_diag, "BiblioteQ: User Error",
			    "To copy the contents of the Error Dialog into "
			    "the clipboard buffer, you must first "
			    "select at least one row.");
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
  QTableWidgetItem *item = NULL;

  if(members_diag->isVisible())
    if(row < 0)
      {
	QMessageBox::critical(members_diag, "BiblioteQ: User Error",
			      "In order to display a member's reservation "
			      "history, you must first select the member.");
	return;
      }

  list << "cd" << "dvd" << "book" << "magazine" << "videogame";

  if(members_diag->isVisible())
    for(i = 0; i < list.size(); i++)
      {
	querystr += QString("SELECT "
			    "history.memberid, "
			    "member.first_name, "
			    "member.last_name, "
			    "%1.title, "
			    "history.item_id, "
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
			    "%1.id = history.item_id AND "
			    "%1.myoid = history.item_oid AND "
			    "member.memberid = '%2' AND %1.type = "
			    "history.type ").arg(list[i]).arg
	  (misc_functions::getColumnString(bb.table, row, "Member ID"));

	if(i != list.size() - 1)
	  querystr += "UNION ";
      }
  else
    for(i = 0; i < list.size(); i++)
      {
	querystr += QString("SELECT "
			    "history.memberid, "
			    "%1.title, "
			    "history.item_id, "
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
			    "%1.id = history.item_id AND "
			    "%1.myoid = history.item_oid AND %1.type = "
			    "history.type ").arg(list[i]).arg
	  (br.userid->text());

	if(i != list.size() - 1)
	  querystr += "UNION ";
      }

  querystr.append("ORDER BY 1");
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec(querystr))
    {
      qapp->restoreOverrideCursor();
      addError(QString("Database Error"),
	       QString("Unable to retrieve reservation history data for table "
		       "populating."),
	       query.lastError().text(),
	       __FILE__, __LINE__);

      if(history_diag->isVisible())
	QMessageBox::critical(history_diag, "BiblioteQ: Database Error",
			      "Unable to retrieve reservation "
			      "history data for table populating.");
      else if(members_diag->isVisible())
	QMessageBox::critical(members_diag, "BiblioteQ: Database Error",
			      "Unable to retrieve reservation "
			      "history data for table populating.");
      else
	QMessageBox::critical(this, "BiblioteQ: Database Error",
			      "Unable to retrieve reservation "
			      "history data for table populating.");

      return;
    }

  qapp->restoreOverrideCursor();
  history.table->clear();
  history.table->setCurrentItem(NULL);
  history.table->setColumnCount(0);
  history.table->setRowCount(0);
  list.clear();
  list.append("Member ID");

  if(members_diag->isVisible())
    {
      list.append("First Name");
      list.append("Last Name");
    }

  list.append("Item Title");
  list.append("Item ID");
  list.append("Barcode");
  list.append("Item Type");
  list.append("Reservation Date");
  list.append("Original Due Date");
  list.append("Returned Date");
  list.append("Lender");
  list.append("OID");
  history.table->setColumnCount(list.size());
  history.table->setHorizontalHeaderLabels(list);
  history.table->setColumnHidden(history.table->columnCount() - 1, true);
  list.clear();
  history.table->setSortingEnabled(false);

  if(selectedBranch["database_type"] != "sqlite")
    history.table->setRowCount(query.size());

  history.table->scrollToTop();
  history.table->horizontalScrollBar()->setValue(0);
  progress.setModal(true);
  progress.setWindowTitle("BiblioteQ: Progress Dialog");
  progress.setLabelText("Populating the table...");

  if(selectedBranch["database_type"] == "sqlite")
    progress.setMaximum
      (misc_functions::sqliteQuerySize(querystr, getDB(),
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
	    str = query.value(j).toString();

	    if((item = new QTableWidgetItem()) != NULL)
	      {
		item->setText(str);

		if(selectedBranch["database_type"] == "sqlite")
		  history.table->setRowCount(i + 1);

		history.table->setItem(i, j, item);
	      }
	    else
	      addError(QString("Memory Error"),
		       QString("Unable to allocate memory for the "
			       "\"item\" object. "
			       "This is a serious problem!"),
		       QString(""), __FILE__, __LINE__);
	  }

      progress.setValue(i + 1);
      progress.update();
      qapp->processEvents();
    }

  query.clear();
  history.table->setRowCount(i);
  history.table->setSortingEnabled(true);
  history.table->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
  history.table->horizontalHeader()->resizeSections
    (QHeaderView::ResizeToContents);
  history.nextTool->setVisible(members_diag->isVisible());
  history.prevTool->setVisible(members_diag->isVisible());

  if(members_diag->isVisible())
    misc_functions::center(history_diag, members_diag);
  else
    misc_functions::center(history_diag, this);

  history_diag->raise();
  history_diag->show();
}

/*
** -- getMembersBrowser() --
*/

QMainWindow *qtbook::getMembersBrowser(void)
{
  return members_diag;
}

/*
** -- slotPrintReservationHistory() --
*/

void qtbook::slotPrintReservationHistory(void)
{
  int i = 0;
  int j = 0;
  QString html = "<html>";
  QPrinter printer;
  QPrintDialog dialog(&printer, members_diag);
  QTextDocument document;

  if(history.table->rowCount() == 0)
    {
      if(members_diag->isVisible())
	QMessageBox::critical(history_diag, "BiblioteQ: User Error",
			      "The selected member does not yet have a "
			      "reservation history to print.");
      else
	QMessageBox::critical(history_diag, "BiblioteQ: User Error",
			      "You do not yet have a reservation history "
			      "to print.");

      return;
    }

  qapp->setOverrideCursor(Qt::WaitCursor);
  html = "Reservation History<br><br>";

  for(i = 0; i < history.table->rowCount(); i++)
    {
      for(j = 0; j < history.table->columnCount(); j++)
	if(!history.table->isColumnHidden(j))
	  html += "<b>" + history.table->horizontalHeaderItem(j)->text() +
	    ":</b> " + history.table->item(i, j)->text() + "<br>";

      html = html.trimmed();
      html += "<br>";
    }

  html = html.trimmed();
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

  branch_diag->close();
  tmphash = branches[br.branch_name->currentText()];

  if(tmphash["database_type"] == "sqlite")
    {
      br.file_frame->setVisible(true);
      br.db_frame->setVisible(false);
      br.fileButton->setFocus();
    }
  else
    {
      br.file_frame->setVisible(false);
      br.db_frame->setVisible(true);
      br.userid->setFocus();
    }

  tmphash.clear();
  branch_diag->updateGeometry();
  branch_diag->resize(branch_diag->sizeHint());
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

  if(history_diag->isVisible())
    if(history.table->rowCount() > 0 &&
       misc_functions::getColumnString(history.table, 0, "Member ID") ==
       memberid)
      {
	qapp->setOverrideCursor(Qt::WaitCursor);

	for(i = 0; i < history.table->rowCount(); i++)
	  {
	    value1 = misc_functions::getColumnString
	      (history.table, i, "OID");
	    value2 = misc_functions::getColumnString
	      (history.table, i, "Barcode");
	    value3 = misc_functions::getColumnString
	      (history.table, i, "Item Type").toLower().remove(" ");

	    if(value1 == ioid && value2 == copyid && value3 == itemType)
	      {
		misc_functions::updateColumn(history.table, i,
					     "Returned Date", returnedDate);
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
  pass.userid->setText(br.userid->text());
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
      QMessageBox::critical(pass_diag, "BiblioteQ: User Error",
			    "The current password is incorrect.");
      pass.currentpassword->setFocus();
      return;
    }
  else if(pass.password->text().length() < 8)
    {
      QMessageBox::critical(pass_diag, "BiblioteQ: User Error",
			    "The password must be at least eight characters "
			    "long.");
      pass.password->setFocus();
      return;
    }
  else if(pass.password->text() != pass.passwordAgain->text())
    {
      QMessageBox::critical(pass_diag, "BiblioteQ: User Error",
			    "The passwords do not match. Please try again.");
      pass.password->setFocus();
      return;
    }

  qapp->setOverrideCursor(Qt::WaitCursor);
  misc_functions::savePassword(pass.userid->text(), getDB(),
			       pass.password->text(), errorstr);
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    {
      addError(QString("Database Error"),
	       QString("Unable to save the new password."),
	       errorstr, __FILE__, __LINE__);
      QMessageBox::critical(pass_diag, "BiblioteQ: Database Error",
			    "Unable to save the new password.");
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
  br.branch_name->setCurrentIndex(0);
  slotBranchChanged();
}

/*
** -- slotSelectDatabaseFile() --
*/

void qtbook::slotSelectDatabaseFile(void)
{
  QFileDialog dialog(branch_diag);

  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setFilter("SQLite Databases (*.db)");
  dialog.setWindowTitle("SQLite Database Selection");
  dialog.exec();

  if(dialog.result() == QDialog::Accepted)
    br.filename->setText(dialog.selectedFiles().at(0));
}

/*
** -- sqlitefile() --
*/

QString qtbook::sqlitefile(void)
{
  if(ui.actionRememberSQLiteFilename->isChecked())
    return br.filename->text();
  else
    return "";
}

/*
** -- slotShowAdminDialog() --
*/

void qtbook::slotShowAdminDialog(void)
{
  misc_functions::center(admin_diag, this);
  admin_diag->raise();
  admin_diag->show();
}

/*
** -- slotAddAdmin() --
*/

void qtbook::slotAddAdmin(void)
{
  int i = 0;
  QCheckBox *checkBox = NULL;
  QTableWidgetItem *item = NULL;

  ab.table->setRowCount(ab.table->rowCount() + 1);

  for(i = 0; i < ab.table->columnCount(); i++)
    if(i == 0)
      {
	if((item = new QTableWidgetItem()) != NULL)
	  {
	    item->setFlags(item->flags() | Qt::ItemIsEditable);
	    ab.table->setItem(ab.table->rowCount() - 1, 0, item);
	  }
	else
	  addError(QString("Memory Error"),
		   QString("Unable to allocate memory for the "
			   "\"item\" object. "
			   "This is a serious problem!"),
		   QString(""), __FILE__, __LINE__);
      }
    else
      {
	if((checkBox = new QCheckBox()) == NULL)
	  addError(QString("Memory Error"),
		   QString("Unable to allocate memory for the "
			   "\"checkBox\" object. "
			   "This is a serious problem!"),
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
      QMessageBox::critical(admin_diag, "BiblioteQ: User Error",
			    "Please select an administrator or empty entry "
			    "to delete.");
      return;
    }

  str = ab.table->item(row, 0)->text().trimmed();

  if((ab.table->item(row, 0)->flags() & Qt::ItemIsEditable) == 0 &&
     str == getAdminID())
    {
      QMessageBox::critical(admin_diag, "BiblioteQ: User Error",
			    "As an administrator, you may not remove "
			    "yourself.");
      return;
    }
  else
    {
      if(!str.isEmpty() && !deletedAdmins.contains(str))
	deletedAdmins.append(str);

      ab.table->removeRow(row);
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
    if(column == 1)
      {
	for(i = 2; i < ab.table->columnCount(); i++)
	  if(box->isChecked())
	    ((QCheckBox *) ab.table->cellWidget(row, i))->setChecked(false);
      }
    else
      if(box->isChecked())
	((QCheckBox *) ab.table->cellWidget(row, 1))->setChecked(false);
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
  QCheckBox *checkBox = NULL;
  QSqlQuery query(db);
  QStringList list;
  QProgressDialog progress(admin_diag);
  QTableWidgetItem *item = NULL;

  querystr = "SELECT username, roles FROM admin ORDER BY username";
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec(querystr))
    {
      qapp->restoreOverrideCursor();
      addError(QString("Database Error"),
	       QString("Unable to retrieve administrator data for table "
		       "populating."),
	       query.lastError().text(),
	       __FILE__, __LINE__);
      QMessageBox::critical(admin_diag, "BiblioteQ: Database Error",
			    "Unable to retrieve administrator "
			    "data for table populating.");

      return;
    }

  qapp->restoreOverrideCursor();
  resetAdminBrowser();

  if(selectedBranch["database_type"] != "sqlite")
    ab.table->setRowCount(query.size());

  progress.setModal(true);
  progress.setWindowTitle("BiblioteQ: Progress Dialog");
  progress.setLabelText("Populating the table...");

  if(selectedBranch["database_type"] == "sqlite")
    progress.setMaximum
      (misc_functions::sqliteQuerySize(querystr, getDB(),
				       __FILE__, __LINE__));
  else
    progress.setMaximum(query.size());

  progress.show();
  progress.update();
  i = -1;

  while(i++, !progress.wasCanceled() && query.next())
    {
      if(query.isValid())
	if((item = new QTableWidgetItem()) != NULL)
	  {
	    str = query.value(0).toString();
	    item->setText(str);
	    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	    str = query.value(1).toString();

	    if(selectedBranch["database_type"] == "sqlite")
	      ab.table->setRowCount(i + 1);

	    ab.table->setItem(i, 0, item);

	    for(j = 1; j < ab.table->columnCount(); j++)
	      if(query.value(0).toString() == getAdminID() && j > 1)
		{
		  if((item = new QTableWidgetItem()) != NULL)
		    {
		      item->setFlags(Qt::ItemIsEnabled |
				     Qt::ItemIsSelectable);
		      ab.table->setItem(i, j, item);
		    }
		  else
		    addError(QString("Memory Error"),
			     QString("Unable to allocate memory for the "
				     "\"item\" object. "
				     "This is a serious problem!"),
			     QString(""), __FILE__, __LINE__);
		}
	      else if((checkBox = new QCheckBox()) != NULL)
		{
		  columnname = ab.table->horizontalHeaderItem
		    (j)->text().toLower();

		  if(str.toLower().contains(columnname))
		    checkBox->setChecked(true);

		  if(query.value(0).toString() == getAdminID())
		    checkBox->setEnabled(false);
		  else
		    connect(checkBox, SIGNAL(stateChanged(int)), this,
			    SLOT(slotAdminCheckBoxClicked(int)));

		  ab.table->setCellWidget(i, j, checkBox);
		}
	      else
		addError(QString("Memory Error"),
			 QString("Unable to allocate memory for the "
				 "\"checkBox\" object. "
				 "This is a serious problem!"),
			 QString(""), __FILE__, __LINE__);
	  }
	else
	  addError(QString("Memory Error"),
		   QString("Unable to allocate memory for the "
			   "\"item\" object. "
			   "This is a serious problem!"),
		   QString(""), __FILE__, __LINE__);

      progress.setValue(i + 1);
      progress.update();
      qapp->processEvents();
    }

  query.clear();
  ab.table->setRowCount(i);
  ab.table->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
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
  QCheckBox *checkBox = NULL;
  QSqlQuery query(qmain->getDB());
  QStringList tmplist;
  QProgressDialog progress(admin_diag);

  /*
  ** 1. Prohibit duplicate administrator ids.
  ** 2. Create a database transaction.
  ** 3. Delete required entries from the admin table.
  ** 4. Remove all deleted database accounts.
  ** 5. Create new entries in the admin table.
  ** 6. Create new database accounts with correct privileges.
  ** 7. Commit or rollback the current database transaction.
  */

  /*
  ** Prepare a database transaction.
  */

  ab.saveButton->setFocus();

  for(i = 0; i < ab.table->rowCount(); i++)
    if(ab.table->item(i, 0)->text().trimmed().isEmpty())
      continue;
    else if(!tmplist.contains(ab.table->item(i, 0)->text().trimmed()))
      tmplist.append(ab.table->item(i, 0)->text().trimmed());
    else
      {
	tmplist.clear();
	ab.table->selectRow(i);
	ab.table->horizontalScrollBar()->setValue(i);
	QMessageBox::critical
	  (admin_diag, "BiblioteQ: User Error",
	   "Duplicate administrator ids are not allowed.");
	return;
      }

  tmplist.clear();
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!getDB().transaction())
    {
      qapp->restoreOverrideCursor();
      addError
	(QString("Database Error"),
	 QString("Unable to create a database transaction."),
	 getDB().lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical
	(admin_diag, "BiblioteQ: Database Error",
	 "Unable to create a database transaction.");
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
	  addError(QString("Database Error"),
		   QString("An error occurred while attempting to remove "
			   "%1.").arg(deletedAdmins[i]),
		   query.lastError().text(), __FILE__, __LINE__);
	  goto db_rollback;
	}

      misc_functions::DBAccount(deletedAdmins[i], getDB(),
				misc_functions::DELETE_USER, errorstr);

      if(!errorstr.isEmpty())
	{
	  qapp->restoreOverrideCursor();
	  addError
	    (QString("Database Error"),
	     QString("An error occurred while attempting to "
		     "remove the database account %1.").arg
	     (deletedAdmins[i]),
	     errorstr, __FILE__, __LINE__);
	  goto db_rollback;
	}
    }

  qapp->restoreOverrideCursor();
  progress.setModal(true);
  progress.setWindowTitle("BiblioteQ: Progress Dialog");
  progress.setLabelText("Saving administrator information...");
  progress.setMaximum(ab.table->rowCount());
  progress.show();
  progress.update();

  /*
  ** Add or modify administrators.
  */

  for(i = 0; i < ab.table->rowCount(); i++)
    {
      str = "";
      adminStr = ab.table->item(i, 0)->text().trimmed();

      if(adminStr.isEmpty())
	continue; // Ignore empty administrator ids.
      else if(adminStr == getAdminID())
	continue; // Ignore current administrator.

      if(((QCheckBox *) ab.table->cellWidget(i, 1))->isChecked())
	str = "administrator";
      else
	for(j = 2; j < ab.table->columnCount(); j++)
	  {
	    checkBox = (QCheckBox *) ab.table->cellWidget(i, j);

	    if(checkBox->isChecked())
	      str += ab.table->horizontalHeaderItem(j)->text().toLower() +
		" ";
	  }

      str = str.trimmed();

      if(str.isEmpty())
	str = "none";

      ucount = misc_functions::userCount(adminStr, getDB(), errorstr);

      if(ucount == 0)
	{
	  querystr = "INSERT INTO admin (username, roles) VALUES (?, ?)";
	  query.prepare(querystr);
	  query.bindValue(0, adminStr);
	  query.bindValue(1, str);
	}
      else
	{
	  querystr = "UPDATE admin SET roles = ? WHERE username = ?";
	  query.prepare(querystr);
	  query.bindValue(0, str);
	  query.bindValue(1, adminStr);
	}

      if(!query.exec())
	{
	  progress.hide();
	  addError
	    (QString("Database Error"),
	     QString("Unable to create or update the administrator entry "
		     "for %1.").arg(adminStr), query.lastError().text(),
	     __FILE__, __LINE__);
	  goto db_rollback;
	}

      if(ucount == 0)
	{
	  misc_functions::DBAccount(adminStr, db, misc_functions::CREATE_USER,
				    errorstr, str);

	  if(!errorstr.isEmpty())
	    {
	      progress.hide();
	      addError
		(QString("Database Error"),
		 QString("An error occurred while attempting to "
			 "create a database account for %1.").arg
		 (adminStr), errorstr, __FILE__, __LINE__);
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
		(QString("Database Error"),
		 QString("An error occurred while attempting to "
			 "revoke privileges from %1.").arg
		 (adminStr), errorstr, __FILE__, __LINE__);
	      goto db_rollback;
	    }

	  misc_functions::grantPrivs(adminStr, str, db, errorstr);

	  if(!errorstr.isEmpty())
	    {
	      progress.hide();
	      addError
		(QString("Database Error"),
		 QString("An error occurred while attempting to "
			 "grant privileges to %1.").arg
		 (adminStr), errorstr, __FILE__, __LINE__);
	      goto db_rollback;
	    }
	}

      progress.setValue(i + 1);
      progress.update();
      qapp->processEvents();
    }

  progress.hide();
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!getDB().commit())
    {
      qapp->restoreOverrideCursor();
      addError
	(QString("Database Error"),
	 QString("Unable to commit the current database "
		 "transaction."),
	 getDB().lastError().text(), __FILE__,
	 __LINE__);
      QMessageBox::critical(admin_diag,
			    "BiblioteQ: Database Error",
			    "Unable to commit the current "
			    "database transaction.");
      return;
    }

  qapp->restoreOverrideCursor();
  deletedAdmins.clear();

  if(adminCreated)
    QMessageBox::information(admin_diag, "BiblioteQ: Information",
			     "Please notify new administrators that their "
			     "default password has been set "
			     "to tempPass.");

  slotRefreshAdminList();
  return;

 db_rollback:

  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!getDB().rollback())
    addError(QString("Database Error"), QString("Rollback failure."),
	     getDB().lastError().text(), __FILE__, __LINE__);

  qapp->restoreOverrideCursor();
  QMessageBox::critical(admin_diag, "BiblioteQ: Database Error",
			"An error occurred while attempting to save "
			"the administrator information.");
}

/*
** -- slotRequest --
*/

void qtbook::slotRequest(void)
{
  int i = 0;
  int ct = 0;
  int numcompleted = 0;
  bool error = false;
  QDate now = QDate::currentDate();
  QString oid = "";
  QString itemType = "";
  QString querystr = "";
  QSqlQuery query(db);
  QModelIndex index;
  QProgressDialog progress(this);
  QModelIndexList list = ui.table->selectionModel()->selectedRows();

  if(roles.isEmpty())
    {
      if(list.isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please select at least one item to place "
				"on request.");
	  return;
	}

      if(list.size() > 0)
	if(QMessageBox::question(this, "BiblioteQ: Question",
				 "Are you sure that you wish to request "
				 "the selected item(s)? Once an item is "
				 "requested, only "
				 "an administrator may cancel it.",
				 QMessageBox::Yes | QMessageBox::No,
				 QMessageBox::No) == QMessageBox::No)
	  {
	    list.clear();
	    return;
	  }
    }
  else
    {
      if(list.isEmpty())
	{
	  QMessageBox::critical(this, "BiblioteQ: User Error",
				"Please select at least one request to "
				"cancel.");
	  return;
	}

      if(list.size() > 0)
	if(QMessageBox::question(this, "BiblioteQ: Question",
				 "Are you sure that you wish to "
				 "cancel the selected request(s)?",
				 QMessageBox::Yes | QMessageBox::No,
				 QMessageBox::No) == QMessageBox::No)
	  {
	    list.clear();
	    return;
	  }
    }

  progress.setModal(true);
  progress.setWindowTitle("BiblioteQ: Progress Dialog");
  progress.setLabelText("Requesting the selected item(s)...");
  progress.setMaximum(list.size());
  progress.show();
  progress.update();

  foreach(index, list)
    {
      i = index.row();
      ct += 1;

      if(roles.isEmpty())
	oid = misc_functions::getColumnString(ui.table, i, "OID");
      else
	oid = misc_functions::getColumnString(ui.table, i, "REQUESTOID");

      if(roles.isEmpty())
	{
	  itemType = misc_functions::getColumnString(ui.table, i, "Type");
	  querystr = "INSERT INTO item_request (item_oid, memberid, "
	    "requestdate, type) VALUES(?, ?, ?, ?)";
	  query.prepare(querystr);
	  query.bindValue(0, oid);
	  query.bindValue(1, br.userid->text());
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

	  if(roles.isEmpty())
	    addError(QString("Database Error"),
		     QString("Unable to request the item."),
		     query.lastError().text(), __FILE__, __LINE__);
	  else
	    addError(QString("Database Error"),
		     QString("Unable to cancel the request."),
		     query.lastError().text(), __FILE__, __LINE__);
	}
      else
	{
	  numcompleted += 1;

	  if(!roles.isEmpty())
	    deleteItem(oid, itemType);
	}

      progress.setValue(ct);
      progress.update();
      qapp->processEvents();
    }

  progress.hide();

  /*
  ** Provide some fancy messages.
  */

  if(error && roles.isEmpty())
    QMessageBox::critical(this, "BiblioteQ: Database Error",
			  "Unable to request all of the selected items.");
  else if(error)
    QMessageBox::critical(this, "BiblioteQ: Database Error",
			  "Unable to cancel some or all of the selected "
			  "requests.");

  if(!roles.isEmpty() && numcompleted > 0)
    slotRefresh();

  list.clear();
}

/*
** -- prepareFilter() --
*/

void qtbook::prepareFilter(void)
{
  QStringList tmplist;

  if(selectedBranch.contains("database_type") &&
     selectedBranch["database_type"] == "sqlite")
    tmplist << "All"
	    << "All Overdue"
	    << "All Reserved"
	    << "Books"
	    << "DVDs"
	    << "Journals"
	    << "Magazines"
	    << "Music CDs"
	    << "Video Games";
  else if(roles.isEmpty() || roles.contains("administrator") ||
	  roles.contains("circulation"))
    tmplist << "All"
	    << "All Overdue"
	    << "All Requested"
	    << "All Reserved"
	    << "Books"
	    << "DVDs"
	    << "Journals"
	    << "Magazines"
	    << "Music CDs"
	    << "Video Games";
  else
    tmplist << "All"
	    << "Books"
	    << "DVDs"
	    << "Journals"
	    << "Magazines"
	    << "Music CDs"
	    << "Video Games";

  while(ui.typefilter->count() > 0)
    ui.typefilter->removeItem(0);

  ui.typefilter->addItems(tmplist);
  tmplist.clear();
}
