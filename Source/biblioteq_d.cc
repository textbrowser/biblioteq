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
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QProcess>
#include <QSettings>
#include <QShortcut>
#include <QSqlRecord>

#include "biblioteq.h"
#include "biblioteq_documentationwindow.h"
#include "biblioteq_files.h"
#include "biblioteq_otheroptions.h"
#include "biblioteq_query_history.h"
#include "biblioteq_sqlite_merge_databases.h"
#include "biblioteq_statistics.h"
#include "biblioteq_swifty.h"

QColor biblioteq::itemMandatoryFieldColor(void) const
{
  return m_otherOptions->itemMandatoryFieldColor();
}

QColor biblioteq::itemQueryResultColor(void) const
{
  return m_otherOptions->itemQueryResultColor();
}

QHash<QString, QString> biblioteq::otherImagesHash(void) const
{
  return m_otherImages;
}

QMap<QPair<QString, QString>, QColor> biblioteq::specialValueColors(void) const
{
  return m_otherOptions->specialValueColors();
}

QMap<QString, QKeySequence> biblioteq::shortcuts(void) const
{
  QMap<QString, QKeySequence> map;

  foreach(auto action, findChildren<QAction *> ())
    if(action && action->shortcut().isEmpty() == false)
      map[action->text()] = action->shortcut();

  foreach(auto shortcut, findChildren<QShortcut *> ())
    if(shortcut && shortcut->key().isEmpty() == false)
      map[shortcut->key().toString()] = shortcut->key();

  return map;
}

QString biblioteq::formattedISBN10(const QString &str) const
{
  return m_otherOptions->isbn10DisplayFormat(str);
}

QString biblioteq::formattedISBN13(const QString &str) const
{
  return m_otherOptions->isbn13DisplayFormat(str);
}

QStringList biblioteq::scripts(void) const
{
  QMap<QString, char> map;
  auto const list
    (QSettings().value("otheroptions/scripts", "").
     toString().trimmed().split('\n') +
     m_specialExecutables.keys());

  foreach(auto const &str, list)
    map[str] = 0;

  return map.keys();
}

QStringList biblioteq::selectedISBN10s(void) const
{
  QStringList list;

  foreach(auto const &index, ui.table->selectionModel()->selectedRows())
    {
      auto const string = biblioteq_misc_functions::getColumnString
	(ui.table, index.row(), ui.table->columnNumber("ISBN-10")).trimmed();

      if(!string.isEmpty())
	list << string;
    }

  return list;
}

biblioteq_item *biblioteq::findItemInTab(const QString &oid) const
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  for(int i = 1; i < ui.tab->count(); i++)
    {
      auto item = dynamic_cast<biblioteq_item *> (ui.tab->widget(i));

      if(item && item->getID() == oid)
	{
	  QApplication::restoreOverrideCursor();
	  return item;
	}
    }

  QApplication::restoreOverrideCursor();
  return nullptr;
}

bool biblioteq::canAccessDatabaseEnumerations(void) const
{
  if(!m_db.isOpen())
    return false;
  else if(m_db.driverName() == "QSQLITE")
    return true;
  else if(m_roles.contains("administrator") || m_roles.contains("librarian"))
    return true;
  else
    return false;
}

bool biblioteq::isCurrentItemAPhotograph(void) const
{
  return biblioteq_misc_functions::getColumnString
    (ui.table,
     ui.table->currentRow(),
     ui.table->columnNumber("Type")) == "Photograph Collection";
}

bool biblioteq::isLocked(void) const
{
  std::unique_lock lock{m_mutex, std::defer_lock};

  return !lock.try_lock();
}

bool biblioteq::showBookReadStatus(void) const
{
  return m_otherOptions->showBookReadStatus();
}

bool biblioteq::showMainTableImages(void) const
{
  return m_otherOptions->showMainTableImages();
}

void biblioteq::addItemWindowToTab(QMainWindow *window)
{
  if(!QSettings().value("tabbedItemWindows", true).toBool() || !window)
    {
      if(window)
	{
	  auto const index = ui.tab->indexOf(window);

	  if(index > 0)
	    {
	      m_attachedWindows << window;
	      ui.tab->removeTab(index);
	      prepareTabWidgetCloseButtons();
	      window->setParent(nullptr, window->windowFlags());
	      QTimer::singleShot(150, this, SLOT(slotDetachTabbed(void)));
	    }
	  else
	    {
#ifdef Q_OS_ANDROID
	      window->showMaximized();
#else
	      biblioteq_misc_functions::center(window, this);
	      window->showNormal();
#endif
	      window->activateWindow();
	      window->raise();
	    }
	}

      return;
    }

  auto const index = ui.tab->indexOf(window);

  if(index >= 0)
    {
      ui.tab->setCurrentIndex(index);
      return;
    }

  QString title("");
  auto item = dynamic_cast<biblioteq_item *> (window);

  if(item)
    title = item->fancyTitleForTab().trimmed();
  else
    title = window->windowTitle().trimmed();

  if(title.isEmpty())
    title = tr("Item");

  connect(window,
	  SIGNAL(destroyed(void)),
	  this,
	  SLOT(slotItemWindowClosed(void)),
	  Qt::UniqueConnection);
  connect(window,
	  SIGNAL(imageChanged(const QImage &)),
	  this,
	  SLOT(slotItemImageChanged(const QImage &)),
	  Qt::UniqueConnection);
  connect(window,
	  SIGNAL(windowTitleChanged(const QString &)),
	  this,
	  SLOT(slotItemTitleChanged(const QString &)),
	  Qt::ConnectionType(Qt::QueuedConnection | Qt::UniqueConnection));
  ui.tab->addTab(window, QIcon(":/missing_image.png"), title);
  ui.tab->setCurrentIndex(ui.tab->count() - 1);
  ui.tab->setTabToolTip(ui.tab->count() - 1, title);
  ui.tab->setTabsClosable(true);
  QApplication::processEvents();
  prepareTabWidgetCloseButtons();
}

void biblioteq::executeCustomQuery(QWidget *widget, const QString &text)
{
  auto querystr(text.trimmed());

  if(querystr.isEmpty())
    {
      QMessageBox::critical(widget == nullptr ? this : widget,
			    tr("BiblioteQ: User Error"),
			    tr("Please provide a valid SQL statement."));
      QApplication::processEvents();
      return;
    }

  auto const q(querystr.toLower());

  if(q.startsWith("alter ") ||
     q.startsWith("cluster ") ||
     q.startsWith("create " ) ||
     q.startsWith("drop ") ||
     q.startsWith("grant ") ||
     q.startsWith("insert ") ||
     q.startsWith("lock ") ||
     q.startsWith("revoke ") ||
     q.startsWith("truncate "))
    {
      QMessageBox::critical
	(widget == nullptr ? this : widget,
	 tr("BiblioteQ: User Error"),
	 tr("Please provide a non-destructive SQL statement."));
      QApplication::processEvents();
    }
  else if(q.startsWith("delete "))
    {
      if(QMessageBox::
	 question(widget == nullptr ? this : widget,
		  tr("BiblioteQ: Question"),
		  tr("Are you sure that you wish to execute the statement?"),
		  QMessageBox::No | QMessageBox::Yes,
		  QMessageBox::No) == QMessageBox::No)
	{
	  QApplication::processEvents();
	  return;
	}
      else
	QApplication::processEvents();

      QSqlQuery query(m_db);

      if(query.exec(querystr))
	slotRefresh();
    }
  else if(q.startsWith("update "))
    {
      if(QMessageBox::
	 question(widget == nullptr ? this : widget,
		  tr("BiblioteQ: Question"),
		  tr("Are you sure that you wish to execute the statement?"),
		  QMessageBox::No | QMessageBox::Yes,
		  QMessageBox::No) == QMessageBox::No)
	{
	  QApplication::processEvents();
	  return;
	}
      else
	QApplication::processEvents();

      QSqlQuery query(m_db);
      auto const list(ui.table->selectionModel()->selectedRows());

      if(list.isEmpty())
	{
	  if(query.exec(querystr))
	    slotRefresh();
	}
      else
	{
	  querystr.append(" WHERE myoid IN (");

	  auto const column = ui.table->columnNumber("MYOID");

	  foreach(auto const &index, list)
	    {
	      auto item = ui.table->item(index.row(), column);

	      if(item)
		{
		  querystr.append(item->text());
		  querystr.append(", ");
		}
	    }

	  if(querystr.endsWith(", "))
	    querystr = querystr.mid(0, querystr.length() - 2);

	  querystr.append(")");

	  if(query.exec(querystr))
	    slotRefresh();
	}
    }
  else
    (void) populateTable(CUSTOM_QUERY, "Custom", querystr);
}

void biblioteq::populateFavorites(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  ui.menu_Custom_Query->clear();

  QSettings settings;
  auto const favorite
    (settings.value("custom_query_favorite").toString().trimmed());
  auto const shortcut
    (settings.value("custom_query_favorite_shortcut").toString().trimmed());

  settings.beginGroup("customqueries");

  foreach(auto const &key, settings.childKeys())
    if(!key.trimmed().isEmpty() && key != s_empty)
      {
	QAction *action = nullptr;
	QString summary("");
	auto const k(key.mid(0, static_cast<int> (Limits::FAVORITES_LENGTH)).
		     remove('\n').remove('\r'));

	action = ui.menu_Custom_Query->addAction
	  (k, this, SLOT(slotCustomQuery(void)));
	action->setProperty("name", k);
	summary = QString::fromUtf8
	  (QByteArray::fromBase64(settings.value(k).toByteArray()));

	if(summary.length() > 32)
	  summary = summary.left(16) + "..." + summary.right(16);

	if(favorite == k)
	  action->setShortcut(QKeySequence(shortcut));
	else
	  action->setShortcut(QKeySequence());

	action->setText(QString("%1 (%2)").arg(k).arg(summary));
      }

  if(ui.menu_Custom_Query->actions().isEmpty())
    ui.menu_Custom_Query->addAction(s_empty);

  QApplication::restoreOverrideCursor();
}

void biblioteq::prepareCustomQueryFavoriteShortcut(void)
{
  QSettings settings;
  auto const favorite
    (settings.value("custom_query_favorite").toString().trimmed());
  auto const shortcut
    (settings.value("custom_query_favorite_shortcut").toString().trimmed());

  foreach(auto action, ui.menu_Custom_Query->actions())
    if(action && action->property("name").toString() == favorite)
      action->setShortcut(QKeySequence(shortcut));
    else if(action)
      action->setShortcut(QKeySequence());
}

void biblioteq::prepareExternalApplicationsMenu(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  ui.menuExternal_Applications->clear();

  auto const list(scripts());

  for(int i = 0; i < list.size(); i++)
    ui.menuExternal_Applications->addAction
      (QIcon(m_specialExecutablesIcons.value(list.at(i))),
       list.at(i),
       this,
       SLOT(slotSpecialApplication(void)));

  if(ui.menuExternal_Applications->isEmpty())
    ui.menuExternal_Applications->addAction(s_empty);

  QApplication::restoreOverrideCursor();
}

void biblioteq::prepareIcons(void)
{
  QSettings setting;
  auto const index = setting.value
    ("otheroptions/display_icon_set_index", 0).toInt();

  if(index == 1)
    {
      // System.

      ab.addButton->setIcon
	(QIcon::fromTheme("list-add", QIcon(":/32x32/add.png")));
      ab.cancelButton->setIcon
	(QIcon::fromTheme("window-close", QIcon(":/32x32/cancel.png")));
      ab.deleteButton->setIcon
	(QIcon::fromTheme("list-remove", QIcon(":/32x32/remove.png")));
      ab.reloadButton->setIcon
	(QIcon::fromTheme("view-refresh", QIcon(":/32x32/reload.png")));
      ab.saveButton->setIcon
	(QIcon::fromTheme("document-save", QIcon(":/32x32/save.png")));
      bb.action_Export->setIcon
	(QIcon::fromTheme("document-export", QIcon(":/16x16/fileexport.png")));
      bb.addButton->setIcon
	(QIcon::fromTheme("list-add", QIcon(":/32x32/add.png")));
      bb.cancelButton->setIcon
	(QIcon::fromTheme("window-close", QIcon(":/32x32/cancel.png")));
      bb.checkoutButton->setIcon
	(QIcon::fromTheme("go-home", QIcon(":/32x32/checkout.png")));
      bb.deleteButton->setIcon
	(QIcon::fromTheme("list-remove", QIcon(":/32x32/remove.png")));
      bb.grantButton->setIcon
	(QIcon::fromTheme("dialog-password", QIcon(":/32x32/permissions.png")));
      bb.historyButton->setIcon
	(QIcon::fromTheme("appointment-new", QIcon(":/32x32/history.png")));
      bb.listButton->setIcon(QIcon(":/32x32/stamp.png"));
      bb.modifyButton->setIcon
	(QIcon::fromTheme("document-edit", QIcon(":/32x32/edit.png")));
      bb.overdueButton->setIcon(QIcon(":/32x32/date.png"));
      bb.printButton->setIcon
	(QIcon::fromTheme("document-print", QIcon(":/32x32/print.png")));
      bb.reloadButton->setIcon
	(QIcon::fromTheme("view-refresh", QIcon(":/32x32/reload.png")));
      bb.speedy->setIcon
	(QIcon::fromTheme("dialog-ok", QIcon(":/32x32/ok.png")));
      br.cancelButton->setIcon
	(QIcon::fromTheme("window-close", QIcon(":/16x16/cancel.png")));
      br.fileButton->setIcon
	(QIcon::fromTheme("document-open", QIcon(":/16x16/fileopen.png")));
      br.okButton->setIcon
	(QIcon::fromTheme("dialog-ok", QIcon(":/16x16/ok.png")));
      br.resetButton->setIcon
	(QIcon::fromTheme("edit-reset", QIcon(":/16x16/reset.png")));
      er.cancelButton->setIcon
	(QIcon::fromTheme("window-close", QIcon(":/16x16/cancel.png")));
      er.copyButton->setIcon
	(QIcon::fromTheme("edit-copy", QIcon(":/16x16/editcopy.png")));
      er.resetButton->setIcon
	(QIcon::fromTheme("edit-reset", QIcon(":/16x16/reset.png")));

      foreach(auto label, ab.frame->findChildren<QLabel *> ())
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
	if(!label->pixmap(Qt::ReturnByValue).isNull())
#else
	if(label->pixmap() && label->pixmap()->isNull() == false)
#endif
	  {
	    QIcon icon(QIcon::fromTheme("dialog-ok", QIcon(":/16x16/ok.png")));

	    label->setPixmap(icon.pixmap(QSize(16, 16)));
	  }

      history.actionExport_As_CSV->setIcon
	(QIcon::fromTheme("document-export", QIcon(":/16x16/fileexport.png")));
      history.cancelButton->setIcon
	(QIcon::fromTheme("window-close", QIcon(":/16x16/cancel.png")));
      history.nextTool->setIcon
	(QIcon::fromTheme("go-next", QIcon(":/32x32/next.png")));
      history.prevTool->setIcon
	(QIcon::fromTheme("go-previous", QIcon(":/32x32/previous.png")));
      history.printButton->setIcon
	(QIcon::fromTheme("document-print", QIcon(":/16x16/fileprint.png")));
      history.reloadButton->setIcon
	(QIcon::fromTheme("view-refresh", QIcon(":/16x16/reload.png")));
      pass.cancelButton->setIcon
	(QIcon::fromTheme("dialog-cancel", QIcon(":/16x16/cancel.png")));
      pass.okButton->setIcon
	(QIcon::fromTheme("dialog-ok", QIcon(":/16x16/ok.png")));
      ui.actionChangePassword->setIcon
	(QIcon::fromTheme("dialog-password", QIcon(":/32x32/password.png")));
      ui.actionConfigureAdministratorPrivileges->setIcon
	(QIcon::fromTheme("preferences-desktop-personal",
			  QIcon(":/32x32/identity.png")));
      ui.actionConnect->setIcon
	(QIcon::fromTheme("network-idle", QIcon(":/16x16/connected.png")));
      ui.actionDatabaseSearch->setIcon
	(QIcon::fromTheme("edit-find", QIcon(":/16x16/find.png")));
      ui.actionDatabase_Enumerations->setIcon
	(QIcon::fromTheme("emblem-system", QIcon(":/32x32/settings.png")));
      ui.actionDeleteEntry->setIcon
	(QIcon::fromTheme("edit-delete", QIcon(":/16x16/eraser.png")));
      ui.actionDisconnect->setIcon
	(QIcon::fromTheme("network-offline",
			  QIcon(":/16x16/disconnected.png")));
      ui.actionDuplicateEntry->setIcon
	(QIcon::fromTheme("edit-copy", QIcon(":/16x16/editcopy.png")));
      ui.actionExit->setIcon
	(QIcon::fromTheme("application-exit", QIcon(":/16x16/exit.png")));
      ui.actionExport_Current_View->setIcon
	(QIcon::fromTheme("document-export", QIcon(":/16x16/fileexport.png")));
      ui.actionExport_View_as_PNG->setIcon
	(QIcon::fromTheme("document-export", QIcon(":/16x16/fileexport.png")));
      ui.actionImportCSV->setIcon
	(QIcon::fromTheme("document-import", QIcon(":/32x32/import.png")));
      ui.actionMembersBrowser->setIcon(QIcon(":/16x16/members.png"));
      ui.actionModifyEntry->setIcon
	(QIcon::fromTheme("document-edit", QIcon(":/16x16/edit.png")));
      ui.actionRefreshTable->setIcon
	(QIcon::fromTheme("view-refresh", QIcon(":/16x16/reload.png")));
      ui.actionRequests->setIcon(QIcon(":/32x32/request.png"));
      ui.actionReservationHistory->setIcon
	(QIcon::fromTheme("appointment-new", QIcon(":/32x32/history.png")));
      ui.actionResizeColumns->setIcon(QIcon(":/16x16/resizecol.png"));
      ui.actionSaveSettings->setIcon
	(QIcon::fromTheme("document-save", QIcon(":/16x16/filesave.png")));
      ui.actionSetGlobalFonts->setIcon
	(QIcon::fromTheme("preferences-desktop-font",
			  QIcon(":/16x16/fonts.png")));
      ui.actionShowErrorDialog->setIcon
	(QIcon::fromTheme("dialog-error", QIcon(":/16x16/log.png")));
      ui.actionViewDetails->setIcon
	(QIcon::fromTheme("dialog-information", QIcon(":/16x16/details.png")));
      ui.action_Database_Enumerations->setIcon
	(QIcon::fromTheme("system-emblem", QIcon(":/32x32/settings.png")));
      ui.action_New_SQLite_Database->setIcon
	(QIcon::fromTheme("document-new", QIcon(":/16x16/filenew.png")));
      ui.action_Print_Icons_View->setIcon
	(QIcon::fromTheme("document-print", QIcon(":/16x16/fileprint.png")));
      ui.configTool->setIcon(QIcon(":/32x32/configure.png"));
      ui.connectTool->setIcon
	(QIcon::fromTheme("network-idle",
			  QIcon(":/32x32/connect_established.png")));
      ui.createTool->setIcon
	(QIcon::fromTheme("list-add", QIcon(":/32x32/add.png")));
      ui.customQueryTool->setIcon
	(QIcon::fromTheme("emblem-favorite",
			  QIcon(":/32x32/customquery.png")));
      ui.deleteTool->setIcon
	(QIcon::fromTheme("list-remove", QIcon(":/32x32/remove.png")));
      ui.detailsTool->setIcon
	(QIcon::fromTheme("dialog-information", QIcon(":/32x32/details.png")));
      ui.disconnectTool->setIcon
	(QIcon::fromTheme("network-offline", QIcon(":/32x32/connect_no.png")));
      ui.duplicateTool->setIcon
	(QIcon::fromTheme("edit-copy", QIcon(":/32x32/duplicate.png")));
      ui.exitTool->setIcon
	(QIcon::fromTheme("application-exit", QIcon(":/32x32/exit.png")));
      ui.filesTool->setIcon
	(QIcon::fromTheme("document-open", QIcon(":/32x32/fileopen.png")));
      ui.menu_Add_Item->setIcon
	(QIcon::fromTheme("list-add", QIcon(":/16x16/add.png")));
      ui.modifyTool->setIcon
	(QIcon::fromTheme("document-edit", QIcon(":/32x32/edit.png")));
      ui.next->setIcon
	(QIcon::fromTheme("go-next", QIcon(":/16x16/next.png")));
      ui.nextPageButton->setIcon
	(QIcon::fromTheme("go-next", QIcon(":/16x16/next.png")));
      ui.previousPageButton->setIcon
	(QIcon::fromTheme("go-previous", QIcon(":/16x16/previous.png")));
      ui.printTool->setIcon
	(QIcon::fromTheme("document-print", QIcon(":/32x32/print.png")));
      ui.refreshTool->setIcon
	(QIcon::fromTheme("view-refresh", QIcon(":/32x32/reload.png")));
      ui.reserveTool->setIcon
	(QIcon::fromTheme("go-home", QIcon(":/32x32/checkout.png")));
      ui.resetAllSearch->setIcon
	(QIcon::fromTheme("edit-reset", QIcon(":/16x16/reset.png")));
      ui.searchTool->setIcon
	(QIcon::fromTheme("edit-find", QIcon(":/32x32/find.png")));
      ui.userTool->setIcon(QIcon(":/32x32/members.png"));
      userinfo_diag->m_userinfo.cancelButton->setIcon
	(QIcon::fromTheme("window-close", QIcon(":/16x16/cancel.png")));
      userinfo_diag->m_userinfo.menu->setIcon
	(QIcon::fromTheme("mail-message-new", QIcon(":/32x32/mail.png")));
      userinfo_diag->m_userinfo.nextTool->setIcon
	(QIcon::fromTheme("go-next", QIcon(":/32x32/next.png")));
      userinfo_diag->m_userinfo.okButton->setIcon
	(QIcon::fromTheme("dialog-ok", QIcon(":/16x16/ok.png")));
      userinfo_diag->m_userinfo.prevTool->setIcon
	(QIcon::fromTheme("go-previous", QIcon(":/32x32/previous.png")));
    }
  else
    {
      // Faenza.

      ab.addButton->setIcon(QIcon(":/32x32/add.png"));
      ab.cancelButton->setIcon(QIcon(":/32x32/cancel.png"));
      ab.deleteButton->setIcon(QIcon(":/32x32/remove.png"));
      ab.reloadButton->setIcon(QIcon(":/32x32/reload.png"));
      ab.saveButton->setIcon(QIcon(":/32x32/save.png"));
      bb.action_Export->setIcon(QIcon(":/16x16/fileexport.png"));
      bb.addButton->setIcon(QIcon(":/32x32/add.png"));
      bb.cancelButton->setIcon(QIcon(":/32x32/cancel.png"));
      bb.checkoutButton->setIcon(QIcon(":/32x32/checkout.png"));
      bb.deleteButton->setIcon(QIcon(":/32x32/remove.png"));
      bb.grantButton->setIcon(QIcon(":/32x32/permissions.png"));
      bb.historyButton->setIcon(QIcon(":/32x32/history.png"));
      bb.listButton->setIcon(QIcon(":/32x32/stamp.png"));
      bb.modifyButton->setIcon(QIcon(":/32x32/edit.png"));
      bb.overdueButton->setIcon(QIcon(":/32x32/date.png"));
      bb.printButton->setIcon(QIcon(":/32x32/print.png"));
      bb.reloadButton->setIcon(QIcon(":/32x32/reload.png"));
      bb.speedy->setIcon(QIcon(":/32x32/ok.png"));
      br.cancelButton->setIcon(QIcon(":/16x16/cancel.png"));
      br.fileButton->setIcon(QIcon(":/16x16/fileopen.png"));
      br.okButton->setIcon(QIcon(":/16x16/ok.png"));
      br.resetButton->setIcon(QIcon(":/16x16/reset.png"));
      er.cancelButton->setIcon(QIcon(":/16x16/cancel.png"));
      er.copyButton->setIcon(QIcon(":/16x16/editcopy.png"));
      er.resetButton->setIcon(QIcon(":/16x16/reset.png"));

      foreach(auto label, ab.frame->findChildren<QLabel *> ())
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
	if(!label->pixmap(Qt::ReturnByValue).isNull())
#else
	if(label->pixmap() && label->pixmap()->isNull() == false)
#endif
	  label->setPixmap(QPixmap(":/16x16/ok.png"));

      history.actionExport_As_CSV->setIcon(QIcon(":/16x16/fileexport.png"));
      history.cancelButton->setIcon(QIcon(":/16x16/cancel.png"));
      history.nextTool->setIcon(QIcon(":/32x32/next.png"));
      history.prevTool->setIcon(QIcon(":/32x32/previous.png"));
      history.printButton->setIcon(QIcon(":/16x16/fileprint.png"));
      history.reloadButton->setIcon(QIcon(":/16x16/reload.png"));
      pass.cancelButton->setIcon(QIcon(":/16x16/cancel.png"));
      pass.okButton->setIcon(QIcon(":/16x16/ok.png"));
      ui.actionChangePassword->setIcon(QIcon(":/32x32/password.png"));
      ui.actionConfigureAdministratorPrivileges->setIcon
	(QIcon(":/32x32/identity.png"));
      ui.actionConnect->setIcon(QIcon(":/16x16/connected.png"));
      ui.actionDatabaseSearch->setIcon(QIcon(":/16x16/find.png"));
      ui.actionDatabase_Enumerations->setIcon(QIcon(":/32x32/settings.png"));
      ui.actionDeleteEntry->setIcon(QIcon(":/16x16/eraser.png"));
      ui.actionDisconnect->setIcon(QIcon(":/16x16/disconnected.png"));
      ui.actionDuplicateEntry->setIcon(QIcon(":/16x16/editcopy.png"));
      ui.actionExit->setIcon(QIcon(":/16x16/exit.png"));
      ui.actionExport_Current_View->setIcon(QIcon(":/16x16/fileexport.png"));
      ui.actionExport_View_as_PNG->setIcon(QIcon(":/16x16/fileexport.png"));
      ui.actionImportCSV->setIcon(QIcon(":/32x32/import.png"));
      ui.actionMembersBrowser->setIcon(QIcon(":/16x16/members.png"));
      ui.actionModifyEntry->setIcon(QIcon(":/16x16/edit.png"));
      ui.actionRefreshTable->setIcon(QIcon(":/16x16/reload.png"));
      ui.actionRequests->setIcon(QIcon(":/32x32/request.png"));
      ui.actionReservationHistory->setIcon(QIcon(":/32x32/history.png"));
      ui.actionResizeColumns->setIcon(QIcon(":/16x16/resizecol.png"));
      ui.actionSaveSettings->setIcon(QIcon(":/16x16/filesave.png"));
      ui.actionSetGlobalFonts->setIcon(QIcon(":/16x16/fonts.png"));
      ui.actionShowErrorDialog->setIcon(QIcon(":/16x16/log.png"));
      ui.actionViewDetails->setIcon(QIcon(":/16x16/details.png"));
      ui.action_Database_Enumerations->setIcon(QIcon(":/32x32/settings.png"));
      ui.action_New_SQLite_Database->setIcon(QIcon(":/16x16/filenew.png"));
      ui.action_Print_Icons_View->setIcon(QIcon(":/16x16/fileprint.png"));
      ui.configTool->setIcon(QIcon(":/32x32/configure.png"));
      ui.connectTool->setIcon(QIcon(":/32x32/connect_established.png"));
      ui.createTool->setIcon(QIcon(":/32x32/add.png"));
      ui.customQueryTool->setIcon(QIcon(":/32x32/customquery.png"));
      ui.deleteTool->setIcon(QIcon(":/32x32/remove.png"));
      ui.detailsTool->setIcon(QIcon(":/32x32/details.png"));
      ui.disconnectTool->setIcon(QIcon(":/32x32/connect_no.png"));
      ui.duplicateTool->setIcon(QIcon(":/32x32/duplicate.png"));
      ui.exitTool->setIcon(QIcon(":/32x32/exit.png"));
      ui.filesTool->setIcon(QIcon(":/32x32/fileopen.png"));
      ui.menu_Add_Item->setIcon(QIcon(":/16x16/add.png"));
      ui.modifyTool->setIcon(QIcon(":/32x32/edit.png"));
      ui.next->setIcon(QIcon(":/16x16/next.png"));
      ui.nextPageButton->setIcon(QIcon(":/16x16/next.png"));
      ui.previousPageButton->setIcon(QIcon(":/16x16/previous.png"));
      ui.printTool->setIcon(QIcon(":/32x32/print.png"));
      ui.refreshTool->setIcon(QIcon(":/32x32/reload.png"));
      ui.reserveTool->setIcon(QIcon(":/32x32/checkout.png"));
      ui.resetAllSearch->setIcon(QIcon(":/16x16/reset.png"));
      ui.searchTool->setIcon(QIcon(":/32x32/find.png"));
      ui.userTool->setIcon(QIcon(":/32x32/members.png"));
      userinfo_diag->m_userinfo.cancelButton->setIcon
	(QIcon(":/16x16/cancel.png"));
      userinfo_diag->m_userinfo.menu->setIcon(QIcon(":/32x32/mail.png"));
      userinfo_diag->m_userinfo.nextTool->setIcon(QIcon(":/32x32/next.png"));
      userinfo_diag->m_userinfo.okButton->setIcon(QIcon(":/16x16/ok.png"));
      userinfo_diag->m_userinfo.prevTool->setIcon
	(QIcon(":/32x32/previous.png"));
    }
}

void biblioteq::prepareItemPagesMenu(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  ui.menuItem_Pages->clear();

  auto action = ui.menuItem_Pages->addAction(tr("&Close All"));

  action->setData(-1);
  action->setEnabled(ui.tab->count() > 1);
  connect(action,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotSelectItemTab(void)));
  ui.menuItem_Pages->addSeparator();

  auto group = ui.menuItem_Pages->findChild<QActionGroup *> ();

  if(!group)
    group = new QActionGroup(ui.menuItem_Pages);

  for(int i = 0; i < ui.tab->count(); i++)
    {
      auto action = new QAction(ui.tab->tabText(i), this);
      auto font(action->font());

      font.setBold(i == ui.tab->currentIndex());
      action->setCheckable(true);
      action->setChecked(i == ui.tab->currentIndex());
      action->setData(i);
      action->setFont(font);
      action->setIcon(ui.tab->tabIcon(i));
      connect(action,
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotSelectItemTab(void)));
      group->addAction(action);
      ui.menuItem_Pages->addAction(action);
    }

  if(group->actions().isEmpty())
    group->deleteLater();

  QApplication::restoreOverrideCursor();
}

void biblioteq::prepareStatusBarIcons(void)
{
  QSettings setting;
  auto const index = setting.value
    ("otheroptions/display_icon_set_index", 0).toInt();

  if(index == 1)
    {
      m_connected_bar_label ? m_connected_bar_label->setPixmap
	(m_db.isOpen() ?
	 QIcon::fromTheme("network-idle",
			  QPixmap(":/16x16/connected.png")).
	 pixmap(QSize(16, 16)) :
	 QIcon::fromTheme("network-offline",
			  QPixmap(":/16x16/disconnected.png")).
	 pixmap(QSize(16, 16))) : (void) 0;
      m_error_bar_label ? m_error_bar_label->setIcon
	(er.table->rowCount() > 0 ?
	 QIcon::fromTheme("dialog-error", QIcon(":/16x16/log.png")) :
	 QIcon::fromTheme("dialog-ok", QIcon(":/16x16/ok.png"))) : (void) 0;
      m_status_bar_label ? m_status_bar_label->setPixmap
	(m_roles.isEmpty() ?
	 QPixmap(":/16x16/lock.png") :
	 QPixmap(":/16x16/unlock.png")) : (void) 0;
    }
  else
    {
      // Faenza.

      m_connected_bar_label ? m_connected_bar_label->setPixmap
	(m_db.isOpen() ?
	 QPixmap(":/16x16/connected.png") :
	 QPixmap(":/16x16/disconnected.png")) : (void) 0;
      m_error_bar_label ? m_error_bar_label->setIcon
	(er.table->rowCount() > 0 ?
	 QIcon(":/16x16/log.png"):
	 QIcon(":/16x16/ok.png")) : (void) 0;
      m_status_bar_label ? m_status_bar_label->setPixmap
	(m_roles.isEmpty() ?
	 QPixmap(":/16x16/lock.png") :
	 QPixmap(":/16x16/unlock.png")) : (void) 0;
    }
}

void biblioteq::prepareTabWidgetCloseButtons(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  ui.tab->setTabsClosable(ui.tab->count() > 1);

  static QList<QTabBar::ButtonPosition> const list
    (QList<QTabBar::ButtonPosition> ()
     << QTabBar::LeftSide
     << QTabBar::RightSide);
  static const int index = 0;

  for(int i = 0; i < list.size(); i++)
    {
      ui.tab->tabBar()->tabButton(index, list.at(i)) ?
	ui.tab->tabBar()->tabButton(index, list.at(i))->deleteLater() :
	(void) 0;
      ui.tab->tabBar()->setTabButton(index, list.at(i), nullptr);
    }

  QApplication::processEvents();
  QApplication::restoreOverrideCursor();
}

void biblioteq::prepareTearOffMenus(void)
{
  createConfigToolMenu();
  m_configToolMenu->setTearOffEnabled(ui.actionTearOffMenus->isChecked());
  ui.menu_Edit->setTearOffEnabled(ui.actionTearOffMenus->isChecked());
  ui.menu_Options->setTearOffEnabled(ui.actionTearOffMenus->isChecked());
  ui.menu_Tools->setTearOffEnabled(ui.actionTearOffMenus->isChecked());
  ui.menu_View->setTearOffEnabled(ui.actionTearOffMenus->isChecked());
}

void biblioteq::prepareUpgradeNotification(void)
{
  if(!m_db.isOpen())
    return;

  /*
  ** Display an error if the current database's schema is not current.
  */

  auto const record(m_db.record("book_copy_info"));

  if(!(record.indexOf("notes") >= 0))
    {
      if(m_db.driverName() == "QPSQL")
	QMessageBox::critical
	  (this,
	   tr("BiblioteQ: Database Error"),
	   tr("The current PostgreSQL schema must be updated. "
	      "Please execute the statement(s) in %1 for version %2.").
	   arg("postgresql_update_schema.sql").
	   arg(BIBLIOTEQ_VERSION));
      else
	QMessageBox::critical
	  (this,
	   tr("BiblioteQ: Database Error"),
	   tr("The current SQLite schema must be updated. "
	      "Tools -> Upgrade SQLite Schema (Recent)."));

      QApplication::processEvents();
    }
}

void biblioteq::refresh(const QString &filter)
{
  foreach(auto action, ui.menu_Category->actions())
    if(action->text() == filter)
      {
	action->trigger();
	break;
      }
}

void biblioteq::showStatusBarMessage(const QString &text, const int duration)
{
  if(statusBar() == nullptr || text.trimmed().isEmpty())
    return;

  statusBar()->showMessage(text, qBound(1000, duration, 250000));
  statusBar()->repaint();
  statusBar()->update();
}

void biblioteq::slotAboutToShowExternalApplicationsMenu(void)
{
  prepareExternalApplicationsMenu();
}

void biblioteq::slotAboutToShowItemsPagesMenu(void)
{
  prepareItemPagesMenu();
}

void biblioteq::slotActionToggled(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  if(action == ui.actionCenter_Child_Windows)
    QSettings().setValue("center_child_windows", action->isChecked());
  else if(action == ui.actionDisplay_Tab_Bar)
    {
      QSettings().setValue("showTabBar", action->isChecked());
      ui.tab->tabBar()->setVisible(action->isChecked());
    }
  else if(action == ui.actionDownload_Version_Information)
    QSettings().setValue("download_version_information", action->isChecked());
}
void biblioteq::slotAnimateAbout(void)
{
  if(!m_about)
    return;

  if(m_aboutColors.first.lightness() >= 100)
    m_aboutTimer.stop();
  else
    {
      m_aboutColors.first = m_aboutColors.first.lighter(105);
      m_aboutColors.second = m_aboutColors.second.lighter(105);
    }

  m_about->setStyleSheet
    (QString("QDialog {background: qlineargradient(y1: 0, y2: 1, "
	     "stop: 0.45 %1, "
	     "stop: 0.46 %2);} "
	     "QWidget {color: white;}").
     arg(m_aboutColors.first.name()).arg(m_aboutColors.second.name()));
}

void biblioteq::slotContributors(void)
{
  if(!m_contributors)
    {
      m_contributors = new biblioteq_documentationwindow(this);
      m_contributors->setHtml
	(tr("<html>"
	    "<b>The following people have made BiblioteQ beautiful. "
	    "If your name is not mentioned, please submit a ticket at "
	    "https://github.com/textbrowser/biblioteq/issues. "
	    "Thank You!</b><br><br>"
	    "Ana Monteiro<br>"
	    "Arti<br>"
	    "Bill Burns<br>"
	    "Ceres<br>"
	    "Csanád Baksay<br>"
	    "Fares Othman<br>"
	    "Fátima Dias<br>"
	    "Frans<br>"
	    "J. Cornavin<br>"
	    "Jeepee<br>"
	    "Lazaros S.<br>"
	    "Leif-W<br>"
	    "Marty<br>"
	    "Nick<br>"
	    "Nicola<br>"
	    "ResetFlag<br>"
	    "Safranil<br>"
	    "SigmaX<br>"
	    "a12554<br>"
	    "caalma<br>"
	    "coldacid<br>"
	    "duhow<br>"
	    "fmra357<br>"
	    "jerzyPL<br>"
	    "jferby<br>"
	    "luismontilla<br>"
	    "meteos77<br>"
	    "numibesi<br>"
	    "ozgurcan<br>"
	    "robindegen<br>"
	    "sidheban<br>"
	    "sit42<br>"
	    "sunyuyangg555<br>"
	    "tamascz<br>"
	    "wohali<br>"
	    "yasbean"
	    "</html>"));
      m_contributors->setWindowTitle(tr("BiblioteQ: Contributors"));
    }

  m_contributors->show();
}

void biblioteq::slotCustomQuery(void)
{
  if(!m_db.isOpen())
    return;

  auto action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  QSettings settings;
  auto const name(action->property("name").toString());
  auto const string
    (QString::
     fromUtf8(QByteArray::
	      fromBase64(settings.
			 value(QString("customqueries/%1").arg(name)).
			 toByteArray()).constData()));

  QApplication::restoreOverrideCursor();
  executeCustomQuery(this, string);
}

void biblioteq::slotDelayedPreparation(void)
{
#ifdef Q_OS_MACOS
  QFileInfo const fileInfo(BIBLIOTEQ_MACOS_LIBPQ_PATH);

  if(!fileInfo.isReadable())
    {
      QMessageBox::critical
	(this,
	 tr("BiblioteQ: Configuration Error"),
	 tr("The PostgreSQL library %1 cannot be read. "
	    "PostgreSQL services will not be functional.").
	 arg(fileInfo.absoluteFilePath()));
      QApplication::processEvents();
    }
#endif
}

void biblioteq::slotDetachTabbed(void)
{
  for(int i = m_attachedWindows.size() - 1; i >= 0; i--)
    {
      auto window = m_attachedWindows.takeAt(i);

      if(window)
	{
#ifdef Q_OS_ANDROID
	  window->showMaximized();
#else
	  biblioteq_misc_functions::center(window, this);
	  window->showNormal();
#endif
	  window->activateWindow();
	  window->raise();
	}
    }
}

void biblioteq::slotExecuteQuery(const QString &text)
{
  if(text.trimmed().isEmpty())
    return;

  (void) populateTable(CUSTOM_QUERY, "Custom", text);
}

void biblioteq::slotExportAsPNG(void)
{
  QFileDialog dialog(this);

  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setDefaultSuffix("png");
  dialog.setDirectory(QDir::homePath());
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setNameFilter(tr("PNG (*.png)"));
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowTitle(tr("BiblioteQ: Export Icons View As PNG"));
  dialog.exec();
  QApplication::processEvents();

  if(dialog.result() == QDialog::Accepted)
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);

      QImage image
	(ui.graphicsView->scene()->itemsBoundingRect().size().toSize(),
	 QImage::Format_RGB32);
      QPainter painter;

      image.fill(Qt::white);
      painter.begin(&image);
      ui.graphicsView->scene()->render
	(&painter,
	 QRectF(),
	 ui.graphicsView->scene()->itemsBoundingRect());
      painter.end();
      image.save(dialog.selectedFiles().value(0), "PNG", 100);
      QApplication::restoreOverrideCursor();
    }
}

void biblioteq::slotExportMembersHistoryAsCSV(void)
{
  exportAsCSV
    (history.table, m_members_diag, tr("BiblioteQ: Member's History As CSV"));
}

void biblioteq::slotGenerateAndCopyMemberLetter(void)
{
  auto clipboard = QApplication::clipboard();

  if(!clipboard)
    return;

  QSettings settings;
  QString str
    (QString::
     fromUtf8(QByteArray::
	      fromBase64(settings.value("otheroptions/generated_letter").
			 toByteArray()).constData()));

  str.replace("%1", userinfo_diag->m_userinfo.lastName->text().trimmed());
  str.replace("%2", userinfo_diag->m_userinfo.firstName->text().trimmed());
  str.replace("%3", userinfo_diag->m_userinfo.membershipfees->text());
  clipboard->setText(str);
}

void biblioteq::slotItemImageChanged(const QImage &image)
{
  auto widget = qobject_cast<QWidget *> (sender());

  if(!widget)
    return;

  auto pixmap(QPixmap::fromImage(image));

  if(pixmap.isNull())
    pixmap = QPixmap(":/missing_image.png");

  ui.tab->setTabIcon(ui.tab->indexOf(widget), pixmap);
}

void biblioteq::slotItemTitleChanged(const QString &t)
{
  auto widget = qobject_cast<QWidget *> (sender());

  if(!widget)
    return;

  auto item = dynamic_cast<biblioteq_item *> (widget);
  auto title(t.trimmed());

  if(item)
    title = item->fancyTitleForTab().trimmed();

  if(title.isEmpty())
    title = tr("Item");

  ui.tab->setTabText(ui.tab->indexOf(widget), title);
  ui.tab->setTabToolTip(ui.tab->indexOf(widget), title);
}

void biblioteq::slotItemWindowClosed(void)
{
  auto widget = qobject_cast<QWidget *> (sender());

  if(widget)
    {
      auto const index = ui.tab->indexOf(widget);

      if(index > 0)
	{
	  ui.tab->removeTab(index);
	  prepareTabWidgetCloseButtons();
	}
    }
}

void biblioteq::slotLaunchEmailSoftware(void)
{
  QSettings settings;
  QString str
    (QString::
     fromUtf8(QByteArray::
	      fromBase64(settings.value("otheroptions/generated_letter").
			 toByteArray()).constData()));

  str.replace("%1", userinfo_diag->m_userinfo.lastName->text().trimmed());
  str.replace("%2", userinfo_diag->m_userinfo.firstName->text().trimmed());
  str.replace("%3", userinfo_diag->m_userinfo.membershipfees->text());
  QDesktopServices::openUrl
    (QUrl::fromUserInput(QString("mailto:%1?body=%2&subject=BiblioteQ").
			 arg(userinfo_diag->m_userinfo.email->text().trimmed()).
			 arg(str)));
}

void biblioteq::slotMembersPagesChanged(int value)
{
  QSettings().setValue("membersPerPage", value);
}

void biblioteq::slotMergeSQLiteDatabases(void)
{
  if(!m_sqliteMergeDatabases)
    m_sqliteMergeDatabases = new biblioteq_sqlite_merge_databases(this);

#ifndef Q_OS_ANDROID
  m_sqliteMergeDatabases->restoreGeometry
    (QSettings().value("sqlite_merge_databases_geometry").toByteArray());
#endif
#ifdef Q_OS_ANDROID
  m_sqliteMergeDatabases->showMaximized();
#else
  biblioteq_misc_functions::center(m_sqliteMergeDatabases, this, false);
  m_sqliteMergeDatabases->showNormal();
#endif
  m_sqliteMergeDatabases->activateWindow();
  m_sqliteMergeDatabases->raise();
}

void biblioteq::slotPrintIconsView(void)
{
  if(menuBar())
    menuBar()->repaint();

  QApplication::processEvents();

  QPrinter printer;

  printer.setColorMode(QPrinter::Color);
  printer.setOutputFormat(QPrinter::PdfFormat);

  QScopedPointer<QPrintDialog> dialog(new QPrintDialog(&printer, this));

  if(dialog->exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      QApplication::setOverrideCursor(Qt::WaitCursor);

      QPainter painter(&printer);

      painter.setRenderHint(QPainter::Antialiasing);
      ui.graphicsView->scene()->render(&painter);
      QApplication::restoreOverrideCursor();
    }

  QApplication::processEvents();
}

void biblioteq::slotSaveGeneralSearchCaseSensitivity(bool state)
{
  QSettings().setValue("generalSearchCaseSensitivity", state);
}

void biblioteq::slotSelectAll(void)
{
  QPainterPath path;

  path.addRect(ui.graphicsView->sceneRect());
  ui.graphicsView->scene()->blockSignals(true);
  ui.graphicsView->scene()->setSelectionArea(path);
  ui.graphicsView->scene()->blockSignals(false);
  ui.table->blockSignals(true);
  ui.table->selectAll();
  ui.table->blockSignals(false);
}

void biblioteq::slotSelectItemTab(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(!action)
    return;
  else if(action->data().toInt() < 0)
    {
      for(int i = ui.tab->count() - 1; i > 0; i--)
	slotTabClosed(i);

      prepareTabWidgetCloseButtons();
    }
  else
    ui.tab->setCurrentIndex
      (qBound(0, action->data().toInt(), ui.tab->count() - 1));
}

void biblioteq::slotSetMembershipFees(void)
{
  auto ok = true;
  double value = 0.0;

  value = QInputDialog::getDouble(m_members_diag,
				  tr("BiblioteQ: Set Membership Dues"),
				  tr("Membership Dues"),
				  0.0,
				  0.0,
				  std::numeric_limits<double>::max(),
				  2,
				  &ok);

  if(!ok)
    return;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  QSqlQuery query(m_db);
  auto const list(bb.table->selectionModel()->selectedRows(0));

  if(list.isEmpty())
    {
      query.prepare("UPDATE member SET membership_fees = ?");
      query.addBindValue(value);

      if(!query.exec())
	{
	  QApplication::restoreOverrideCursor();
	  addError
	    (tr("Database Error"),
	     tr("Unable to update the entries."),
	     query.lastError().text(),
	     __FILE__,
	     __LINE__);
	  QMessageBox::critical
	    (this,
	     tr("BiblioteQ: Database Error"),
	     tr("Unable to update the entries."));
	  QApplication::processEvents();
	}
      else
	QApplication::restoreOverrideCursor();
    }
  else
    {
      foreach(auto const &index, list)
	{
	  auto const str(index.data().toString());

	  query.prepare
	    ("UPDATE member SET membership_fees = ? WHERE memberid = ?");
	  query.addBindValue(value);
	  query.addBindValue(str);

	  if(!query.exec())
	    addError
	      (tr("Database Error"),
	       tr("Unable to update the entry %1.").arg(str),
	       query.lastError().text(),
	       __FILE__,
	       __LINE__);
	}

      QApplication::restoreOverrideCursor();
    }
}

void biblioteq::slotShowBatchActivitiesBrowser(void)
{
  m_batchActivities->show(this);
}

void biblioteq::slotShowDocumentation(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  repaint();
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!m_documentation.value(action))
    {
      m_documentation[action] = new biblioteq_documentationwindow(this);

      QFile file;

      if(action == ui.action_English)
	file.setFileName(":/BiblioteQ.html");
      else
	{
#ifdef Q_OS_ANDROID
	  file.setFileName("assets:/BiblioteQ-Fr.html");
#else
	  file.setFileName
	    (QString(":/Contributed%1French%1BiblioteQ-Fr.html").
	     arg(QDir::separator()));
#endif
	}

      if(file.open(QIODevice::ReadOnly))
	m_documentation.value(action)->setHtml(file.readAll());
    }

  m_documentation.value(action)->show();
  QApplication::restoreOverrideCursor();
}

void biblioteq::slotShowFiles(void)
{
  if(!m_files)
    m_files = new biblioteq_files(this);

#ifdef Q_OS_ANDROID
  m_files->showMaximized();
#else
  biblioteq_misc_functions::center(m_files, this);
  m_files->showNormal();
#endif
  m_files->activateWindow();
  m_files->raise();
}

void biblioteq::slotShowMagic(void)
{
  m_magic->show(this);
}

void biblioteq::slotShowQueryHistory(void)
{
#ifndef Q_OS_ANDROID
  m_queryHistory->restoreGeometry
    (QSettings().value("query_history_geometry").toByteArray());
#endif
#ifdef Q_OS_ANDROID
  m_queryHistory->showMaximized();
#else
  biblioteq_misc_functions::center(m_queryHistory, this, false);
  m_queryHistory->showNormal();
#endif
  m_queryHistory->activateWindow();
  m_queryHistory->raise();
}

void biblioteq::slotShowReleaseNotes(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  repaint();
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!m_releaseNotes.value(action))
    {
      if(action == ui.action_French_Release_Notes)
	m_releaseNotes[action] = new biblioteq_documentationwindow
	  (QUrl("qrc:/Contributed/French/Release-Notes-French.html"), this);
      else
	m_releaseNotes[action] = new biblioteq_documentationwindow
	  (QUrl("qrc:/Release-Notes.html"), this);

      m_releaseNotes.value(action)->setWindowTitle
	(tr("BiblioteQ: Release Notes"));
    }

  m_releaseNotes.value(action)->show();
  QApplication::restoreOverrideCursor();
}

void biblioteq::slotShowStatistics(void)
{
  auto window = new biblioteq_statistics(this, m_otherOptions);

#ifdef Q_OS_ANDROID
  window->showMaximized();
#else
  biblioteq_misc_functions::center(window, this);
  window->show(this);
#endif
  window->activateWindow();
  window->raise();
}

void biblioteq::slotSpecialApplication(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  auto text(action->text());

  text = m_db.driverName() == "QPSQL" ?
    text : text.replace("%1", m_db.databaseName());
  qputenv("BIBLIOTEQ_DATABASE_NAME", m_db.databaseName().toUtf8());
  QProcess::startDetached(text, QStringList());
  qunsetenv("BIBLIOTEQ_DATABASE_NAME");
}

void biblioteq::slotTabClosed(int index)
{
  if(index <= 0)
    return;

  std::unique_lock lock{m_mutex, std::defer_lock};

  if(!lock.try_lock())
    return;
  else
    lock.unlock();

  auto deleted = false;

  if(index > 0)
    {
      auto book = dynamic_cast<biblioteq_book *> (ui.tab->widget(index));

      if(book && book->close())
	{
	  deleted = true;
	  goto done_label;
	}

      auto cd = dynamic_cast<biblioteq_cd *> (ui.tab->widget(index));

      if(cd && cd->close())
	{
	  deleted = true;
	  goto done_label;
	}

      auto dvd = dynamic_cast<biblioteq_dvd *> (ui.tab->widget(index));

      if(dvd && dvd->close())
	{
	  deleted = true;
	  goto done_label;
	}

      auto gl = dynamic_cast<biblioteq_grey_literature *>
	(ui.tab->widget(index));

      if(gl && gl->close())
	{
	  deleted = true;
	  goto done_label;
	}

      auto journal = dynamic_cast<biblioteq_journal *> (ui.tab->widget(index));

      if(journal && journal->close())
	{
	  deleted = true;
	  goto done_label;
	}

      if(!dynamic_cast<biblioteq_journal *> (ui.tab->widget(index)))
	{
	  auto magazine = dynamic_cast<biblioteq_magazine *>
	    (ui.tab->widget(index));

	  if(magazine && magazine->close())
	    {
	      deleted = true;
	      goto done_label;
	    }
	}

      auto photograph = dynamic_cast<biblioteq_photographcollection *>
	(ui.tab->widget(index));

      if(photograph && photograph->close())
	{
	  deleted = true;
	  goto done_label;
	}

      auto videogame = dynamic_cast<biblioteq_videogame *>
	(ui.tab->widget(index));

      if(videogame && videogame->close())
	{
	  deleted = true;
	  goto done_label;
	}
    }

 done_label:

  if(deleted || ui.tab->widget(index) == nullptr)
    {
      ui.tab->removeTab(index);

      if(sender()) // Avoid iterations.
	prepareTabWidgetCloseButtons();
    }
}

void biblioteq::slotTabbedItemWindows(bool state)
{
  QSettings().setValue("tabbedItemWindows", state);
}

void biblioteq::slotTableFindNext(void)
{
  if(!m_db.isOpen() || ui.stackedWidget->currentIndex() != 1)
    return;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(m_findList.isEmpty())
    m_findList = ui.table->findItems
      (ui.find->text().trimmed(), Qt::MatchContains);

  if(!m_findList.isEmpty())
    {
      ui.table->scrollToItem(ui.table->item(m_findList.at(0)->row(), 0));
      ui.table->selectRow(m_findList.at(0)->row());
      m_findList.removeFirst();
    }

  QApplication::restoreOverrideCursor();
}

void biblioteq::slotTableFindTextCleared(const QString &text)
{
  Q_UNUSED(text);
  m_findList.clear();
}

void biblioteq::slotTearOffMenus(void)
{
  prepareTearOffMenus();
}

void biblioteq::slotSwifty(void)
{
  if(!statusBar())
    return;

  auto const version1(QString(BIBLIOTEQ_VERSION_LTS).remove('.').toLong());
  auto const version2
    (m_swifty->newest_version().remove('.').trimmed().toLong());

  if(version1 < version2)
    {
      QTimer::singleShot(10000, this, SLOT(slotSwifty(void)));
      statusBar()->showMessage
	(tr("A new official version %1 of BiblioteQ is available!").
	 arg(m_swifty->newest_version().trimmed()),
	 10000);
    }
}
