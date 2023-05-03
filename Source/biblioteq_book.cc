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

#include "biblioteq.h"
#include "biblioteq_book.h"
#include "biblioteq_borrowers_editor.h"
#include "biblioteq_copy_editor_book.h"
#include "biblioteq_filesize_table_item.h"
#include "biblioteq_pdfreader.h"

#include <QActionGroup>
#include <QAuthenticator>
#include <QCryptographicHash>
#include <QFileDialog>
#include <QInputDialog>
#include <QNetworkProxy>
#include <QShortcut>
#include <QSqlField>
#include <QSqlRecord>
#include <QTimer>
#include <QXmlStreamReader>

biblioteq_book::biblioteq_book(biblioteq *parentArg,
			       const QString &oidArg,
			       const QModelIndex &index):
  QMainWindow(), biblioteq_item(index)
{
  m_duplicate = false;
  m_openLibraryWorking = nullptr;
  m_sruWorking = nullptr;
  qmain = parentArg;

  QGraphicsScene *scene1 = nullptr;
  QGraphicsScene *scene2 = nullptr;
  QMenu *menu = nullptr;

  menu = new QMenu(this);
  scene1 = new QGraphicsScene(this);
  scene2 = new QGraphicsScene(this);
  m_imageManager = new QNetworkAccessManager(this);
  m_openLibraryManager = new QNetworkAccessManager(this);
  m_proxyDialog = new QDialog(this);
  m_sruManager = new QNetworkAccessManager(this);
  ui_p.setupUi(m_proxyDialog);
  m_parentWid = parentArg;
  m_oid = oidArg;
  m_oldq = biblioteq_misc_functions::getColumnString
    (qmain->getUI().table,
     m_index->row(),
     qmain->getUI().table->columnNumber("Quantity")).toInt();
  id.setupUi(this);
  setQMain(this);
  id.files->setColumnHidden(Columns::MYOID, true);
  id.files->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  id.marc_tags_format->setVisible(false);
  id.parse_marc_tags->setVisible(false);
  id.publication_date->setDisplayFormat(qmain->publicationDateFormat("books"));
  id.publication_date_enabled->setVisible(false);
  id.quantity->setMaximum(static_cast<int> (biblioteq::Limits::QUANTITY));
#ifndef BIBLIOTEQ_LINKED_WITH_POPPLER
  id.view_pdf->setEnabled(false);
  id.view_pdf->setToolTip
    (tr("BiblioteQ was not assembled with Poppler support."));
#else
  connect(id.view_pdf,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotShowPDF(void)));
#endif
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S),
		this,
		SLOT(slotGo(void)));
#else
  new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_S),
		this,
		SLOT(slotGo(void)));
#endif
  biblioteq_misc_functions::sortCombinationBox(id.condition);
  biblioteq_misc_functions::sortCombinationBox(id.marc_tags_format);
  biblioteq_misc_functions::sortCombinationBox(id.originality);
  updateFont(QApplication::font(), qobject_cast<QWidget *> (this));
  connect(id.attach_files,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAttachFiles(void)));
  connect(id.delete_files,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotDeleteFiles(void)));
  connect(id.export_files,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotExportFiles(void)));
  connect(id.files, SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
	  this, SLOT(slotFilesDoubleClicked(QTableWidgetItem *)));
  connect(id.okButton, SIGNAL(clicked(void)), this, SLOT(slotGo(void)));
  connect(id.openLibraryQuery, SIGNAL(clicked(void)), this,
	  SLOT(slotOpenLibraryQuery(void)));
  connect(id.parse_marc_tags, SIGNAL(clicked(void)), this,
	  SLOT(slotParseMarcTags(void)));
  connect(id.showUserButton, SIGNAL(clicked(void)), this,
	  SLOT(slotShowUsers(void)));
  connect(id.sruQueryButton, SIGNAL(clicked(void)), this,
	  SLOT(slotSRUQuery(void)));
  connect(id.z3950QueryButton, SIGNAL(clicked(void)), this,
	  SLOT(slotZ3950Query(void)));
  connect(id.cancelButton, SIGNAL(clicked(void)), this,
	  SLOT(slotCancel(void)));
  connect(id.copiesButton, SIGNAL(clicked()), this,
	  SLOT(slotPopulateCopiesEditor(void)));
  connect(id.resetButton, SIGNAL(clicked(void)), this,
	  SLOT(slotReset(void)));
  connect(id.isbn10to13, SIGNAL(clicked(void)), this,
	  SLOT(slotConvertISBN10to13(void)));
  connect(id.isbn13to10, SIGNAL(clicked(void)), this,
	  SLOT(slotConvertISBN13to10(void)));
  connect(id.isbnAvailableCheckBox,
	  SIGNAL(toggled(bool)),
	  id.sruQueryButton,
	  SLOT(setEnabled(bool)));
#ifdef BIBLIOTEQ_LINKED_WITH_YAZ
  connect(id.isbnAvailableCheckBox,
	  SIGNAL(toggled(bool)),
	  id.z3950QueryButton,
	  SLOT(setEnabled(bool)));
#endif
  connect(menu->addAction(tr("Reset Front Cover Image")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Back Cover Image")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset ISBN-10")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset ISBN-13")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Edition")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Authors")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Book Binding Type")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset LC Control Number")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Call Number")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Dewey Class Number")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Title")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Publication Date")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Publisher")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Place of Publication")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Categories")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Price")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Language")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Monetary Units")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Copies")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Location")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Originality")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Condition")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Abstract")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset MARC Tags")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Keywords")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Accession Number")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset URL")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Multi-Volume Set ISBN")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Target Audience")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Volume Number")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(id.frontButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  connect(id.backButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  connect(id.dwnldFront, SIGNAL(clicked(void)), id.dwnldFront,
	  SLOT(showMenu(void)));
  connect(id.dwnldBack, SIGNAL(clicked(void)), id.dwnldBack,
	  SLOT(showMenu(void)));
  connect(qmain,
	  SIGNAL(fontChanged(const QFont &)),
	  this,
	  SLOT(setGlobalFonts(const QFont &)));
  connect(this,
	  SIGNAL(openLibraryQueryError(const QString &)),
	  this,
	  SLOT(slotOpenLibraryQueryError(const QString &)),
	  Qt::QueuedConnection);
  connect(this,
	  SIGNAL(sruQueryError(const QString &)),
	  this,
	  SLOT(slotSRUQueryError(const QString &)),
	  Qt::QueuedConnection);
  id.resetButton->setMenu(menu);

  if(menu->actions().size() >= 4)
    {
      connect(id.isbnAvailableCheckBox,
	      SIGNAL(toggled(bool)),
	      menu->actions().at(2),
	      SLOT(setEnabled(bool)));
      connect(id.isbnAvailableCheckBox,
	      SIGNAL(toggled(bool)),
	      menu->actions().at(3),
	      SLOT(setEnabled(bool)));
      connect(id.isbnAvailableCheckBox,
	      SIGNAL(toggled(bool)),
	      menu->actions().at(27),
	      SLOT(setEnabled(bool)));
    }

  QString errorstr("");

  QApplication::setOverrideCursor(Qt::WaitCursor);
  id.condition->addItems
    (biblioteq_misc_functions::getBookConditions(qmain->getDB(), errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the conditions.")),
       errorstr, __FILE__, __LINE__);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  id.originality->addItems
    (biblioteq_misc_functions::getBookOriginality(qmain->getDB(), errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the originality list.")),
       errorstr, __FILE__, __LINE__);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  id.binding->addItems
    (biblioteq_misc_functions::getBookBindingTypes(qmain->getDB(), errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the binding types.")),
       errorstr, __FILE__, __LINE__);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  id.language->addItems
    (biblioteq_misc_functions::getLanguages(qmain->getDB(), errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the languages.")),
       errorstr, __FILE__, __LINE__);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  id.monetary_units->addItems
    (biblioteq_misc_functions::getMonetaryUnits(qmain->getDB(), errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the monetary units.")),
       errorstr, __FILE__, __LINE__);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  id.location->addItems
    (biblioteq_misc_functions::getLocations(qmain->getDB(), "Book", errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the locations.")),
       errorstr, __FILE__, __LINE__);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  id.target_audience->addItems
    (biblioteq_misc_functions::
     getBookTargetAudiences(qmain->getDB(), errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the target audiences.")),
       errorstr, __FILE__, __LINE__);

  id.front_image->setScene(scene1);
  id.back_image->setScene(scene2);

  if(id.condition->findText(biblioteq::s_unknown) == -1)
    id.condition->addItem(biblioteq::s_unknown);

  if(id.originality->findText(biblioteq::s_unknown) == -1)
    id.originality->addItem(biblioteq::s_unknown);

  if(id.binding->findText(biblioteq::s_unknown) == -1)
    id.binding->addItem(biblioteq::s_unknown);

  if(id.language->findText(biblioteq::s_unknown) == -1)
    id.language->addItem(biblioteq::s_unknown);

  if(id.location->findText(biblioteq::s_unknown) == -1)
    id.location->addItem(biblioteq::s_unknown);

  if(id.monetary_units->findText(biblioteq::s_unknown) == -1)
    id.monetary_units->addItem(biblioteq::s_unknown);

  if(id.target_audience->findText(biblioteq::s_unknown) == -1)
    id.target_audience->addItem(biblioteq::s_unknown);

  QActionGroup *actionGroup1 = nullptr;
  QActionGroup *actionGroup2 = nullptr;

  actionGroup1 = new QActionGroup(this);
  actionGroup2 = new QActionGroup(this);

  auto found = false;
  auto list(qmain->getSRUNames());

  for(int i = 0; i < list.size(); i++)
    {
      auto action = actionGroup1->addAction(list.at(i));

      if(!action)
	continue;

      action->setCheckable(true);
      id.sruQueryButton->addAction(action);

      if(qmain->getPreferredSRUSite() == action->text())
	{
	  found = true;
	  action->setChecked(true);
	}
    }

  if(id.sruQueryButton->actions().isEmpty())
    {
      actionGroup1->deleteLater();
      id.sruQueryButton->setPopupMode(QToolButton::DelayedPopup);
    }
  else if(!found)
    id.sruQueryButton->actions().at(0)->setChecked(true);

  for(int i = 1; i <= 2; i++)
    {
      auto action = new QAction(tr("&Amazon"), this);

      if(i == 1)
	{
	  action->setProperty("download_type", "amazon_front");
	  id.dwnldFront->addAction(action);
	}
      else
	{
	  action->setProperty("download_type", "amazon_back");
	  id.dwnldBack->addAction(action);
	}

      connect(action,
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotDownloadImage(void)));
    }

  for(int i = 1; i <= 2; i++)
    {
      auto action = new QAction(tr("&Open Library"), this);

      if(i == 1)
	{
	  action->setProperty("download_type", "open_library_front");
	  id.dwnldFront->addAction(action);
	}
      else
	{
	  action->setProperty("download_type", "open_library_back");
	  id.dwnldBack->addAction(action);
	}

      connect(action,
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotDownloadImage(void)));
    }

  for(int i = 1; i <= 2; i++)
    {
      auto action = new QAction(tr("&Other"), this);

      if(i == 1)
	{
	  action->setProperty("download_type", "other_front");
	  id.dwnldFront->addAction(action);
	}
      else
	{
	  action->setProperty("download_type", "other_back");
	  id.dwnldBack->addAction(action);
	}

      connect(action,
	      SIGNAL(triggered(void)),
	      this,
	      SLOT(slotDownloadImage(void)));
    }

  auto action = new QAction(tr("All..."), this);

  connect(action,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotPrint(void)));
  id.printButton->addAction(action);
  action = new QAction(tr("Author, Title, Dewey Class Number..."), this);
  connect(action,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotPrintAuthorTitleDewey(void)));
  id.printButton->addAction(action);
  action = new QAction(tr("Call Number, Dewey Class Number..."), this);
  connect(action,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotPrintCallDewey(void)));
  id.printButton->addAction(action);

  if(id.printButton->actions().isEmpty())
    id.printButton->setPopupMode(QToolButton::MenuButtonPopup);

  connect(id.printButton,
	  SIGNAL(clicked(void)),
	  id.printButton,
	  SLOT(showMenu(void)));
  found = false;
  list = qmain->getZ3950Names();
#ifndef BIBLIOTEQ_LINKED_WITH_YAZ
  id.z3950QueryButton->setEnabled(false);
  id.z3950QueryButton->setToolTip(tr("The YAZ library is not available."));
#endif

  for(int i = 0; i < list.size(); i++)
    {
      auto action = actionGroup2->addAction(list.at(i));

      if(!action)
	continue;

      action->setCheckable(true);
      id.z3950QueryButton->addAction(action);

      if(qmain->getPreferredZ3950Site() == action->text())
	{
	  found = true;
	  action->setChecked(true);
	}
    }

  if(id.z3950QueryButton->actions().isEmpty())
    {
      actionGroup2->deleteLater();
      id.z3950QueryButton->setPopupMode(QToolButton::DelayedPopup);
    }
  else if(!found)
    id.z3950QueryButton->actions().at(0)->setChecked(true);

  for(int i = 1; i <= 1000; i++)
    id.edition->addItem(QString::number(i));

  /*
  ** Save some palettes and style sheets.
  */

  m_cb_orig_ss = id.edition->styleSheet();
  m_dt_orig_ss = id.publication_date->styleSheet();
  m_white_pal = m_te_orig_pal = id.author->viewport()->palette();

  /*
  ** Prepare the form.
  */

#ifndef Q_OS_ANDROID
  if(m_parentWid)
    resize(qRound(0.95 * m_parentWid->size().width()),
	   qRound(0.95 * m_parentWid->size().height()));
#endif

#ifdef Q_OS_MACOS
  foreach(auto tool_button, findChildren<QToolButton *> ())
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

  id.splitter->setStretchFactor(0, 0);
  id.splitter->setStretchFactor(1, 1);
  id.splitter->setStretchFactor(2, 0);
  biblioteq_misc_functions::center(this, m_parentWid);
  biblioteq_misc_functions::hideAdminFields(this, qmain->getRoles());
}

biblioteq_book::~biblioteq_book()
{
}

biblioteq_item_working_dialog *biblioteq_book::createImageDownloadDialog
(const QString &downloadType)
{
  biblioteq_item_working_dialog *dialog = nullptr;

  dialog = new
    biblioteq_item_working_dialog(qobject_cast<QMainWindow *> (this));
  dialog->resize(dialog->sizeHint());
  dialog->setLabelText(tr("Downloading..."));
  dialog->setMaximum(0);
  dialog->setMinimum(0);
  dialog->setModal(true);

  if(downloadType.contains("back"))
    dialog->setWindowTitle(tr("BiblioteQ: Back Cover Image Download"));
  else
    dialog->setWindowTitle(tr("BiblioteQ: Front Cover Image Download"));

  dialog->show();
  dialog->update();
  dialog->repaint();
  connect(dialog,
	  SIGNAL(canceled(void)),
	  dialog,
	  SLOT(deleteLater(void)));
  connect(dialog,
	  SIGNAL(canceled(void)),
	  this,
	  SLOT(slotCancelImageDownload(void)));
  connect(dialog,
	  SIGNAL(rejected(void)),
	  dialog,
	  SLOT(deleteLater(void)));
  connect(dialog,
	  SIGNAL(rejected(void)),
	  this,
	  SLOT(slotCancelImageDownload(void)));
  return dialog;
}

void biblioteq_book::changeEvent(QEvent *event)
{
  if(event)
    switch(event->type())
      {
      case QEvent::LanguageChange:
	{
	  id.retranslateUi(this);
	  ui_p.retranslateUi(m_proxyDialog);
	  break;
	}
      default:
	break;
      }

  QMainWindow::changeEvent(event);
}

void biblioteq_book::closeEvent(QCloseEvent *e)
{
  if(m_engWindowTitle.contains("Create") ||
     m_engWindowTitle.contains("Modify"))
    if(hasDataChanged(this))
      {
	if(QMessageBox::
	   question(this, tr("BiblioteQ: Question"),
		    tr("Your changes have not been saved. Continue closing?"),
		    QMessageBox::Yes | QMessageBox::No,
		    QMessageBox::No) == QMessageBox::No)
	  {
	    QApplication::processEvents();

	    if(e)
	      e->ignore();

	    return;
	  }

	QApplication::processEvents();
      }

  qmain->removeBook(this);
}

void biblioteq_book::createFile(const QByteArray &digest,
				const QByteArray &bytes,
				const QString &fileName) const
{
  QSqlQuery query(qmain->getDB());

  if(qmain->getDB().driverName() != "QSQLITE")
    query.prepare("INSERT INTO book_files "
		  "(file, file_digest, file_name, item_oid) "
		  "VALUES (?, ?, ?, ?)");
  else
    query.prepare("INSERT INTO book_files "
		  "(file, file_digest, file_name, item_oid, myoid) "
		  "VALUES (?, ?, ?, ?, ?)");

  query.bindValue(0, bytes);
  query.bindValue(1, digest.toHex().constData());
  query.bindValue(2, fileName);
  query.bindValue(3, m_oid);

  if(qmain->getDB().driverName() == "QSQLITE")
    {
      QString errorstr("");
      auto value = biblioteq_misc_functions::getSqliteUniqueId
	(qmain->getDB(), errorstr);

      if(errorstr.isEmpty())
	query.bindValue(4, value);
      else
	qmain->addError(QString(tr("Database Error")),
			QString(tr("Unable to generate a unique "
				   "integer.")),
			errorstr);
    }

  if(!query.exec())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to create a database transaction.")),
       query.lastError().text(), __FILE__, __LINE__);
}

void biblioteq_book::createOpenLibraryDialog(void)
{
  if(m_openLibraryWorking)
    m_openLibraryWorking->deleteLater();

  m_openLibraryWorking = new
    biblioteq_item_working_dialog(qobject_cast<QMainWindow *> (this));
  m_openLibraryWorking->resize(m_openLibraryWorking->sizeHint());
  m_openLibraryWorking->setLabelText(tr("Downloading..."));
  m_openLibraryWorking->setMaximum(0);
  m_openLibraryWorking->setMinimum(0);
  m_openLibraryWorking->setModal(true);
  m_openLibraryWorking->setWindowTitle
    (tr("BiblioteQ: Open Library Data Retrieval"));
  connect(m_openLibraryWorking,
	  SIGNAL(canceled(void)),
	  this,
	  SLOT(slotOpenLibraryCanceled(void)));
  connect(m_openLibraryWorking,
	  SIGNAL(rejected(void)),
	  this,
	  SLOT(slotOpenLibraryCanceled(void)));
}

void biblioteq_book::createSRUDialog(void)
{
  if(m_sruWorking)
    m_sruWorking->deleteLater();

  m_sruWorking = new
    biblioteq_item_working_dialog(qobject_cast<QMainWindow *> (this));
  m_sruWorking->resize(m_sruWorking->sizeHint());
  m_sruWorking->setLabelText(tr("Downloading..."));
  m_sruWorking->setMaximum(0);
  m_sruWorking->setMinimum(0);
  m_sruWorking->setModal(true);
  m_sruWorking->setWindowTitle(tr("BiblioteQ: SRU Data Retrieval"));
  connect(m_sruWorking,
	  SIGNAL(canceled(void)),
	  this,
	  SLOT(slotSRUCanceled(void)));
  connect(m_sruWorking,
	  SIGNAL(rejected(void)),
	  this,
	  SLOT(slotSRUCanceled(void)));
}

void biblioteq_book::downloadFinished(void)
{
  if(m_imageBuffer.property("which") == "front")
    {
      if(m_imageBuffer.data().size() > 1000)
	{
	  id.front_image->clear();
	  id.front_image->loadFromData(m_imageBuffer.data());
	}

      if(m_imageBuffer.data().size() < 1000)
	{
	  m_imageBuffer.close();
	  QMessageBox::warning
	    (this, tr("BiblioteQ: HTTP Warning"),
	     tr("The front cover image for the specified "
		"ISBN may not exist."));
	  QApplication::processEvents();
	}
    }
  else
    {
      if(m_imageBuffer.data().size() > 1000)
	{
	  id.back_image->clear();
	  id.back_image->loadFromData(m_imageBuffer.data());
	}

      if(m_imageBuffer.data().size() < 1000)
	{
	  m_imageBuffer.close();
	  QMessageBox::warning
	    (this, tr("BiblioteQ: HTTP Warning"),
	     tr("The back cover image for the specified ISBN "
		"may not exist."));
	  QApplication::processEvents();
	}
    }

  m_imageBuffer.close();
}

void biblioteq_book::duplicate(const QString &p_oid, const int state)
{
  m_duplicate = true;
  modify(state); // Initial population.
  id.accession_number->setText
    (biblioteq_misc_functions::
     accessionNumberAsSpecialText(biblioteq_misc_functions::
				  bookAccessionNumber(qmain->getDB())));
  id.attach_files->setEnabled(false);
  id.view_pdf->setEnabled(false);
  id.copiesButton->setEnabled(false);
  id.delete_files->setEnabled(false);
  id.export_files->setEnabled(false);
  id.showUserButton->setEnabled(false);
  m_oid = p_oid;
  setWindowTitle(tr("BiblioteQ: Duplicate Book Entry"));
  m_duplicate = false;
  m_engWindowTitle = "Create";
}

void biblioteq_book::insert(void)
{
  slotReset();
  id.accession_number->setText
    (biblioteq_misc_functions::
     accessionNumberAsSpecialText(biblioteq_misc_functions::
				  bookAccessionNumber(qmain->getDB())));
  id.attach_files->setEnabled(false);
  id.marc_tags_format->setVisible(true);
  id.parse_marc_tags->setVisible(true);
  id.view_pdf->setEnabled(false);
  id.id->clear();
  id.isbn13->clear();
  id.category->setPlainText("N/A");
  id.author->setPlainText("N/A");
  id.lcnum->clear();
  id.callnum->clear();
  id.deweynum->clear();
  id.title->clear();
  id.publisher->setPlainText("N/A");
  id.place->setPlainText("N/A");
  id.description->setPlainText("N/A");
  id.marc_tags->clear();
  id.keyword->clear();
  id.copiesButton->setEnabled(false);
  id.delete_files->setEnabled(false);
  id.export_files->setEnabled(false);
  id.openLibraryQuery->setVisible(true);
  id.sruQueryButton->setVisible(true);
  id.z3950QueryButton->setVisible(true);
  id.okButton->setText(tr("&Save"));
  id.publication_date->setDate
    (QDate::fromString("01/01/2000", biblioteq::s_databaseDateFormat));
  id.price->setMinimum(0.00);
  id.price->setValue(0.00);
  id.quantity->setMinimum(1);
  id.quantity->setValue(1);
  id.showUserButton->setEnabled(false);
  id.location->setCurrentIndex(0);
  id.edition->setCurrentIndex(0);
  id.language->setCurrentIndex(0);
  id.monetary_units->setCurrentIndex(0);
  id.binding->setCurrentIndex(0);
  id.target_audience->setEditable(true);
  id.url->clear();
  biblioteq_misc_functions::highlightWidget
    (id.id, m_requiredHighlightColor);
  biblioteq_misc_functions::highlightWidget
    (id.isbn13, m_requiredHighlightColor);
  biblioteq_misc_functions::highlightWidget
    (id.title, m_requiredHighlightColor);
  biblioteq_misc_functions::highlightWidget
    (id.publisher->viewport(), m_requiredHighlightColor);
  biblioteq_misc_functions::highlightWidget
    (id.place->viewport(), m_requiredHighlightColor);
  biblioteq_misc_functions::highlightWidget
    (id.author->viewport(), m_requiredHighlightColor);
  biblioteq_misc_functions::highlightWidget
    (id.description->viewport(), m_requiredHighlightColor);
  biblioteq_misc_functions::highlightWidget
    (id.category->viewport(), m_requiredHighlightColor);
  m_te_orig_pal = id.id->palette();
  setWindowTitle(tr("BiblioteQ: Create Book Entry"));
  m_engWindowTitle = "Create";
  id.id->setFocus();
  storeData(this);
#ifdef Q_OS_ANDROID
  showMaximized();
#else
  biblioteq_misc_functions::center(this, m_parentWid);
  showNormal();
#endif
  activateWindow();
  raise();
}

void biblioteq_book::modify(const int state)
{
  QSqlQuery query(qmain->getDB());
  QString fieldname = "";
  QString str = "";
  QVariant var;
  int i = 0;

  if(state == biblioteq::EDITABLE)
    {
      setReadOnlyFields(this, false);
      setWindowTitle(tr("BiblioteQ: Modify Book Entry"));
      m_engWindowTitle = "Modify";
      id.attach_files->setEnabled(true);
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
      id.view_pdf->setEnabled(true);
#endif
      id.copiesButton->setEnabled(true);
      id.delete_files->setEnabled(true);
      id.export_files->setEnabled(true);
      id.isbnAvailableCheckBox->setChecked(false);
      id.marc_tags_format->setVisible(true);
      id.parse_marc_tags->setVisible(true);
      id.showUserButton->setEnabled(true);
      id.okButton->setVisible(true);
      id.openLibraryQuery->setVisible(true);
      id.sruQueryButton->setVisible(true);
      id.z3950QueryButton->setVisible(true);
      id.resetButton->setVisible(true);
      id.frontButton->setVisible(true);
      id.backButton->setVisible(true);
      id.dwnldFront->setVisible(true);
      id.dwnldBack->setVisible(true);
      id.isbn10to13->setVisible(true);
      id.isbn13to10->setVisible(true);
      id.target_audience->setEditable(true);
      biblioteq_misc_functions::highlightWidget
	(id.id, m_requiredHighlightColor);
      biblioteq_misc_functions::highlightWidget
	(id.isbn13, m_requiredHighlightColor);
      biblioteq_misc_functions::highlightWidget
	(id.title, m_requiredHighlightColor);
      biblioteq_misc_functions::highlightWidget
	(id.publisher->viewport(), m_requiredHighlightColor);
      biblioteq_misc_functions::highlightWidget
	(id.place->viewport(), m_requiredHighlightColor);
      biblioteq_misc_functions::highlightWidget
	(id.author->viewport(), m_requiredHighlightColor);
      biblioteq_misc_functions::highlightWidget
	(id.description->viewport(), m_requiredHighlightColor);
      biblioteq_misc_functions::highlightWidget
	(id.category->viewport(), m_requiredHighlightColor);
      m_te_orig_pal = id.id->palette();
    }
  else
    {
      setReadOnlyFields(this, true);
      id.isbnAvailableCheckBox->setCheckable(false);
      setWindowTitle(tr("BiblioteQ: View Book Details"));
      m_engWindowTitle = "View";
      id.attach_files->setVisible(false);
      id.view_pdf->setVisible(true);
      id.copiesButton->setVisible(false);
      id.delete_files->setVisible(false);
      id.export_files->setVisible(true);

      if(qmain->isGuest())
	id.showUserButton->setVisible(false);
      else
	id.showUserButton->setEnabled(true);

      id.okButton->setVisible(false);
      id.openLibraryQuery->setVisible(false);
      id.sruQueryButton->setVisible(false);
      id.z3950QueryButton->setVisible(false);
      id.resetButton->setVisible(false);
      id.frontButton->setVisible(false);
      id.backButton->setVisible(false);
      id.dwnldFront->setVisible(false);
      id.dwnldBack->setVisible(false);
      id.isbn10to13->setVisible(false);
      id.isbn13to10->setVisible(false);

      auto actions = id.resetButton->menu()->actions();

      if(actions.size() >= 2)
	{
	  actions[0]->setVisible(false);
	  actions[1]->setVisible(false);
	}

      actions.clear();
    }

  id.quantity->setMinimum(1);
  id.price->setMinimum(0.00);
  id.okButton->setText(tr("&Save"));
  str = m_oid;
  query.prepare("SELECT title, "
		"author, "
		"publisher, "
		"pdate, "
		"place, "
		"edition, "
		"category, "
		"language, "
		"id, "
		"price, "
		"monetary_units, "
		"quantity, "
		"binding_type, "
		"location, "
		"isbn13, "
		"lccontrolnumber, "
		"callnumber, "
		"deweynumber, "
		"description, "
		"front_cover, "
		"back_cover, "
		"marc_tags, "
		"keyword, "
		"originality, "
		"condition, "
		"accession_number, "
		"url, "
		"alternate_id_1, "
		"multivolume_set_isbn, "
		"target_audience, "
		"volume_number "
		"FROM book WHERE myoid = ?");
  query.bindValue(0, str);
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!query.exec() || !query.next())
    {
      QApplication::restoreOverrideCursor();
      qmain->addError
	(QString(tr("Database Error")),
	 QString(tr("Unable to retrieve the selected book's data.")),
	 query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical
	(this, tr("BiblioteQ: Database Error"),
	 tr("Unable to retrieve the selected book's data."));
      QApplication::processEvents();
      close();
      return;
    }
  else
    {
      QApplication::restoreOverrideCursor();
#ifdef Q_OS_ANDROID
      showMaximized();
#else
      biblioteq_misc_functions::center(this, m_parentWid);
      showNormal();
#endif
      activateWindow();
      raise();

      auto record(query.record());

      for(i = 0; i < record.count(); i++)
	{
	  var = record.field(i).value();
	  fieldname = record.fieldName(i);

	  if(fieldname == "title")
	    id.title->setText(var.toString().trimmed());
	  else if(fieldname == "author")
	    id.author->setMultipleLinks("book_search", "author",
					var.toString().trimmed());
	  else if(fieldname == "publisher")
	    id.publisher->setMultipleLinks("book_search", "publisher",
					   var.toString().trimmed());
	  else if(fieldname == "place")
	    id.place->setMultipleLinks("book_search", "place",
				       var.toString().trimmed());
	  else if(fieldname == "pdate")
	    id.publication_date->setDate
	      (QDate::fromString(var.toString().trimmed(),
				 biblioteq::s_databaseDateFormat));
	  else if(fieldname == "edition")
	    {
	      if(id.edition->findText(var.toString().trimmed()) > -1)
		id.edition->setCurrentIndex
		  (id.edition->findText(var.toString().trimmed()));
	      else
		id.edition->setCurrentIndex(0);
	    }
	  else if(fieldname == "price")
	    id.price->setValue(var.toDouble());
	  else if(fieldname == "category")
	    id.category->setMultipleLinks("book_search", "category",
					  var.toString().trimmed());
	  else if(fieldname == "language")
	    {
	      if(id.language->findText(var.toString().trimmed()) > -1)
		id.language->setCurrentIndex
		  (id.language->findText(var.toString().trimmed()));
	      else
		id.language->setCurrentIndex
		  (id.language->findText(biblioteq::s_unknown));
	    }
	  else if(fieldname == "quantity")
	    {
	      id.quantity->setValue(var.toInt());
	      m_oldq = id.quantity->value();
	    }
	  else if(fieldname == "monetary_units")
	    {
	      if(id.monetary_units->findText(var.toString().trimmed()) > -1)
		id.monetary_units->setCurrentIndex
		  (id.monetary_units->findText(var.toString().trimmed()));
	      else
		id.monetary_units->setCurrentIndex
		  (id.monetary_units->findText(biblioteq::s_unknown));
	    }
	  else if(fieldname == "binding_type")
	    {
	      if(id.binding->findText(var.toString().trimmed()) > -1)
		id.binding->setCurrentIndex
		  (id.binding->findText(var.toString().trimmed()));
	      else
		id.binding->setCurrentIndex(0);
	    }
	  else if(fieldname == "location")
	    {
	      if(id.location->findText(var.toString().trimmed()) > -1)
		id.location->setCurrentIndex
		  (id.location->findText(var.toString().trimmed()));
	      else
		id.location->setCurrentIndex
		  (id.location->findText(biblioteq::s_unknown));
	    }
	  else if(fieldname == "id")
	    {
	      if(state == biblioteq::EDITABLE)
		{
		  if(!var.toString().trimmed().trimmed().isEmpty())
		    str = QString(tr("BiblioteQ: Modify Book Entry (")) +
		      var.toString().trimmed() + tr(")");
		  else
		    str = tr("BiblioteQ: Modify Book Entry");

		  m_engWindowTitle = "Modify";
		}
	      else
		{
		  if(!var.toString().trimmed().trimmed().isEmpty())
		    str = QString(tr("BiblioteQ: View Book Details (")) +
		      var.toString().trimmed() + tr(")");
		  else
		    str = tr("BiblioteQ: View Book Details");

		  m_engWindowTitle = "View";
		}

	      id.id->setText(qmain->formattedISBN10(var.toString().trimmed()));
	      setWindowTitle(str);

	      if(!query.isNull(i))
		id.isbnAvailableCheckBox->setChecked(true);
	    }
	  else if(fieldname == "description")
	    id.description->setPlainText(var.toString().trimmed());
	  else if(fieldname == "marc_tags")
	    id.marc_tags->setPlainText(var.toString().trimmed());
	  else if(fieldname == "keyword")
	    id.keyword->setMultipleLinks("book_search", "keyword",
					 var.toString().trimmed());
	  else if(fieldname == "isbn13")
	    {
	      id.isbn13->setText
		(qmain->formattedISBN13(var.toString().trimmed()));

	      if(!query.isNull(i))
		id.isbnAvailableCheckBox->setChecked(true);
	    }
	  else if(fieldname == "lccontrolnumber")
	    id.lcnum->setText(var.toString().trimmed());
	  else if(fieldname == "callnumber")
	    id.callnum->setText(var.toString().trimmed());
	  else if(fieldname == "deweynumber")
	    id.deweynum->setText(var.toString().trimmed());
	  else if(fieldname == "originality")
	    {
	      if(id.originality->findText(var.toString().trimmed()) > -1)
		id.originality->setCurrentIndex
		  (id.originality->findText(var.toString().trimmed()));
	      else
		id.originality->setCurrentIndex(2);
	    }
	  else if(fieldname == "condition")
	    {
	      if(id.condition->findText(var.toString().trimmed()) > -1)
		id.condition->setCurrentIndex
		  (id.condition->findText(var.toString().trimmed()));
	      else
		id.condition->setCurrentIndex(0);
	    }
	  else if(fieldname == "front_cover")
	    {
	      if(!record.field(i).isNull())
		{
		  id.front_image->loadFromData
		    (QByteArray::fromBase64(var.toByteArray()));

		  if(id.front_image->m_image.isNull())
		    id.front_image->loadFromData(var.toByteArray());
		}
	    }
	  else if(fieldname == "back_cover")
	    {
	      if(!record.field(i).isNull())
		{
		  id.back_image->loadFromData
		    (QByteArray::fromBase64(var.toByteArray()));

		  if(id.back_image->m_image.isNull())
		    id.back_image->loadFromData(var.toByteArray());
		}
	    }
	  else if(fieldname == "accession_number")
	    id.accession_number->setText(var.toString().trimmed());
	  else if(fieldname == "url")
	    id.url->setPlainText(var.toString().trimmed());
	  else if(fieldname == "alternate_id_1")
	    id.alternate_id_1->setText(var.toString().trimmed());
	  else if(fieldname == "multivolume_set_isbn")
	    {
	      auto str(var.toString().remove('-').trimmed());

	      if(str.length() <= 10)
		id.multivolume_set_isbn->setText(qmain->formattedISBN10(str));
	      else
		id.multivolume_set_isbn->setText(qmain->formattedISBN13(str));
	    }
	  else if(fieldname == "target_audience")
	    {
	      if(id.target_audience->findText(var.toString().trimmed()) > -1)
		id.target_audience->setCurrentIndex
		  (id.target_audience->findText(var.toString().trimmed()));
	      else if(var.toString().trimmed().isEmpty())
		id.target_audience->setCurrentIndex
		  (id.target_audience->findText(biblioteq::s_unknown));
	      else
		{
		  id.target_audience->insertItem(0, var.toString().trimmed());
		  id.target_audience->setCurrentIndex(0);
		}
	    }
	  else if(fieldname == "volume_number")
	    id.volume_number->setText(var.toString().trimmed());
	}

      foreach(auto textfield, findChildren<QLineEdit *> ())
	textfield->setCursorPosition(0);

      storeData(this);

      if(!m_duplicate)
	populateFiles();
    }

  id.id->setFocus();
  raise();
}

void biblioteq_book::openLibraryDownloadFinished(void)
{
  m_openLibraryResults = m_openLibraryResults.trimmed();

  if(m_openLibraryResults.isEmpty() ||
     m_openLibraryResults.length() <= 10)
    {
      QMessageBox::critical
	(this,
	 tr("BiblioteQ: Open Library Query Error"),
	 tr("The Open Library query produced invalid results. "
	    "Perhaps the book is not defined."));
      QApplication::processEvents();
    }
  else
    {
      if(QMessageBox::question
	 (this,
	  tr("BiblioteQ: Question"),
	  tr("Replace existing values with those retrieved from Open Library?"),
	  QMessageBox::No | QMessageBox::Yes,
	  QMessageBox::No) == QMessageBox::No)
	{
	  QApplication::processEvents();
	  return;
	}

      populateAfterOpenLibrary();
    }
}

void biblioteq_book::populateAfterOpenLibrary(void)
{
  QString authors("");
  QString deweyDecimalClass("");
  QString lcClassifications("");
  QString lccn("");
  QString pagination("");
  QString place("");
  QString publicationDate("");
  QString publishers("");
  QString subjects("");
  QString subtitle("");
  QString title("");
  QStringList keys;

  keys << "\"authors\":"
       << "\"dewey_decimal_class\":"
       << "\"lc_classifications\":"
       << "\"lccn\":"
       << "\"pagination\":"
       << "\"publish_date\":"
       << "\"publish_places\":"
       << "\"publishers\":"
       << "\"subjects\":"
       << "\"subtitle\":"
       << "\"title\":";

  while(!keys.isEmpty())
    {
      auto const &key(keys.takeFirst());
      auto index = m_openLibraryResults.indexOf(key.toUtf8());

      if(index > -1)
	{
	  index += key.length();

	  if(key == "\"dewey_decimal_class\":" ||
	     key == "\"lc_classifications\":" ||
	     key == "\"lccn\":" ||
	     key == "\"pagination\":")
	    {
	      QByteArray bytes;

	      for(int i = m_openLibraryResults.indexOf('"', index) + 1;
		  i < m_openLibraryResults.length();
		  i++)
		if(m_openLibraryResults.at(i) == '"')
		  break;
		else
		  bytes.append(m_openLibraryResults.at(i));

	      auto value(QString::fromUtf8(bytes));

	      if(!value.isEmpty())
		{
		  if(key == "\"dewey_decimal_class\":")
		    deweyDecimalClass = value;
		  else if(key == "\"lc_classifications\":")
		    lcClassifications = value;
		  else if(key == "\"lccn\":")
		    lccn = value;
		  else if(key == "\"pagination\":")
		    pagination = value;
		}

	      continue;
	    }
	  else if(key == "\"publish_date\":" ||
		  key == "\"subtitle\":" ||
		  key == "\"title\":")
	    {
	      int o_index = -1; // Open bracket.

	      for(int i = index - key.length(); i >= 0; i--)
		if(m_openLibraryResults.at(i) == ']')
		  break;
		else if(m_openLibraryResults.at(i) == '[')
		  {
		    o_index = i;
		    break;
		  }

	      if(o_index != -1)
		{
		  keys << key;
		  m_openLibraryResults.remove
		    (index - key.length(), key.length());
		  continue;
		}

	      QByteArray bytes;

	      for(int i = m_openLibraryResults.indexOf('"', index) + 1;
		  i < m_openLibraryResults.length();
		  i++)
		if(m_openLibraryResults.at(i) == '"')
		  break;
		else
		  bytes.append(m_openLibraryResults.at(i));

	      auto value(QString::fromUtf8(bytes));

	      if(!value.isEmpty())
		{
		  if(key == "\"publish_date\":")
		    publicationDate = value;
		  else if(key == "\"subtitle\":")
		    subtitle = value;
		  else if(key == "\"title\":")
		    title = value;
		}

	      continue;
	    }

	  while(true)
	    {
	      index = m_openLibraryResults.indexOf("\"name\":", index);

	      if(index > 0)
		{
		  index += static_cast<int> (qstrlen("\"name\":"));

		  QByteArray bytes;

		  for(int i = m_openLibraryResults.indexOf('"', index) + 1;
		      i < m_openLibraryResults.length();
		      i++)
		    if(m_openLibraryResults.at(i) == '"')
		      break;
		    else
		      bytes.append(m_openLibraryResults.at(i));

		  auto name(QString::fromUtf8(bytes));

		  if(!name.isEmpty())
		    {
		      if(key == "\"authors\":")
			{
			  if(!authors.isEmpty())
			    authors.append("\n");

			  authors.append(name);
			}
		      else if(key == "\"publish_places\":")
			{
			  if(!place.isEmpty())
			    place.append("\n");

			  place.append(name);
			}
		      else if(key == "\"publishers\":")
			{
			  if(!publishers.isEmpty())
			    publishers.append("\n");

			  publishers.append(name);
			}
		      else if(key == "\"subjects\":")
			{
			  if(!subjects.isEmpty())
			    subjects.append("\n");

			  subjects.append(name);
			}
		    }

		  if(m_openLibraryResults.indexOf("\"name\":", index) >
		     m_openLibraryResults.indexOf(']', index))
		    break;
		}
	      else
		break;
	    }
	}
    }

  biblioteq_misc_functions::highlightWidget
    (id.marc_tags->viewport(), m_queryHighlightColor);
  id.marc_tags->setPlainText(m_openLibraryResults);

  if(!authors.isEmpty())
    {
      biblioteq_misc_functions::highlightWidget
	(id.author->viewport(), m_queryHighlightColor);
      id.author->setPlainText(authors);
    }

  if(!deweyDecimalClass.isEmpty())
    {
      biblioteq_misc_functions::highlightWidget
	(id.deweynum, m_queryHighlightColor);
      id.deweynum->setText(deweyDecimalClass);
    }

  if(!lcClassifications.isEmpty())
    {
      biblioteq_misc_functions::highlightWidget
	(id.callnum, m_queryHighlightColor);
      id.callnum->setText(lcClassifications);
    }

  if(!lccn.isEmpty())
    {
      biblioteq_misc_functions::highlightWidget
	(id.lcnum, m_queryHighlightColor);
      id.lcnum->setText(lccn);
    }

  if(!pagination.isEmpty())
    {
      biblioteq_misc_functions::highlightWidget
	(id.description->viewport(), m_queryHighlightColor);
      id.description->setPlainText(pagination);
    }

  if(!place.isEmpty())
    {
      biblioteq_misc_functions::highlightWidget
	(id.place->viewport(), m_queryHighlightColor);
      id.place->setPlainText(place);
    }

  if(!publicationDate.isEmpty())
    {
      id.publication_date->setDate
	(QDate::fromString("01/01/" + publicationDate,
			   biblioteq::s_databaseDateFormat));
      id.publication_date->setStyleSheet
	(QString("background-color: %1").arg(m_queryHighlightColor.name()));
    }

  if(!publishers.isEmpty())
    {
      biblioteq_misc_functions::highlightWidget
	(id.publisher->viewport(), m_queryHighlightColor);
      id.publisher->setPlainText(publishers);
    }

  if(!subjects.isEmpty())
    {
      biblioteq_misc_functions::highlightWidget
	(id.category->viewport(), m_queryHighlightColor);
      id.category->setPlainText(subjects);
    }

  if(!title.isEmpty())
    {
      biblioteq_misc_functions::highlightWidget
	(id.title, m_queryHighlightColor);

      if(!subtitle.isEmpty())
	title = QString("%1 (%2)").arg(title).arg(subtitle);

      id.title->setText(title);
    }
}

void biblioteq_book::populateAfterSRU
(const QString &recordSyntax, const QString &text)
{
  id.edition->setCurrentIndex(0);
  id.edition->setStyleSheet
    (QString("background-color: %1").arg(m_queryHighlightColor.name()));
  id.marc_tags->setText(text);
  biblioteq_misc_functions::highlightWidget
    (id.marc_tags->viewport(), m_queryHighlightColor);

  QString str("");
  auto isbn10User = false;
  auto isbn13User = false;

  if(id.id->text().remove('-').trimmed().length() == 10)
    isbn10User = true;

  if(id.isbn13->text().remove('-').trimmed().length() == 13)
    isbn13User = true;

  QXmlStreamReader reader(text);

  while(!reader.atEnd())
    if(reader.readNextStartElement())
      if(reader.name().toString().toLower().trimmed() == "datafield")
	{
	  auto tag
	    (reader.attributes().value("tag").toString().toLower().trimmed());

	  if(tag == "100" || tag == "700")
	    id.author->clear();
	  else if(tag == "260")
	    id.place->clear();
	  else if(tag == "650")
	    id.category->clear();
	}

  biblioteq_marc m;

  if(recordSyntax == "MARC21")
    m.initialize
      (biblioteq_marc::BOOK, biblioteq_marc::SRU, biblioteq_marc::MARC21);
  else if(recordSyntax == "UNIMARC")
    m.initialize
      (biblioteq_marc::BOOK, biblioteq_marc::SRU, biblioteq_marc::UNIMARC);
  else
    {
      /*
      ** Guess?
      */

      if(text.contains("marc21", Qt::CaseInsensitive))
	m.initialize
	  (biblioteq_marc::BOOK, biblioteq_marc::SRU, biblioteq_marc::MARC21);
      else
	m.initialize
	  (biblioteq_marc::BOOK, biblioteq_marc::SRU, biblioteq_marc::UNIMARC);
    }

  m.parse(text);
  str = m.author();

  if(!str.isEmpty())
    {
      id.author->setPlainText(str);
      biblioteq_misc_functions::highlightWidget
	(id.author->viewport(), m_queryHighlightColor);
    }

  str = m.binding();

  if(id.binding->findText(str, Qt::MatchFixedString) > -1)
    id.binding->setCurrentIndex
      (id.binding->findText(str, Qt::MatchFixedString));

  id.binding->setStyleSheet
    (QString("background-color: %1").arg(m_queryHighlightColor.name()));
  str = m.callnum();

  if(!str.isEmpty())
    {
      id.callnum->setText(str);
      biblioteq_misc_functions::highlightWidget
	(id.callnum, m_queryHighlightColor);
    }

  str = m.category();

  if(!str.isEmpty())
    {
      id.category->setPlainText(str);
      biblioteq_misc_functions::highlightWidget
	(id.category->viewport(), m_queryHighlightColor);
    }

  str = m.description();

  if(!str.isEmpty())
    {
      id.description->setPlainText(str);
      biblioteq_misc_functions::highlightWidget
	(id.description->viewport(), m_queryHighlightColor);
    }

  str = m.deweynum();

  if(!str.isEmpty())
    {
      id.deweynum->setText(str);
      biblioteq_misc_functions::highlightWidget
	(id.deweynum, m_queryHighlightColor);
    }

  str = m.edition();

  if(id.edition->findText(str) > -1)
    id.edition->setCurrentIndex(id.edition->findText(str));
  else
    id.edition->setCurrentIndex(0);

  str = m.isbn10();

  if(!isbn10User && !str.isEmpty())
    {
      id.id->setText(qmain->formattedISBN10(str));
      biblioteq_misc_functions::highlightWidget(id.id, m_queryHighlightColor);
    }

  str = m.isbn13();

  if(!isbn13User && !str.isEmpty())
    {
      id.isbn13->setText(qmain->formattedISBN13(str));
      biblioteq_misc_functions::highlightWidget
	(id.isbn13, m_queryHighlightColor);
    }

  str = m.lcnum();

  if(!str.isEmpty())
    {
      id.lcnum->setText(str);
      biblioteq_misc_functions::highlightWidget
	(id.lcnum, m_queryHighlightColor);
    }

  str = m.place();

  if(!str.isEmpty())
    {
      id.place->setPlainText(str);
      biblioteq_misc_functions::highlightWidget
	(id.place->viewport(), m_queryHighlightColor);
    }

  if(!m.publicationDate().isNull())
    {
      id.publication_date->setDate(m.publicationDate());
      id.publication_date->setStyleSheet
	(QString("background-color: %1").arg(m_queryHighlightColor.name()));
    }

  str = m.publisher();

  if(!str.isEmpty())
    {
      id.publisher->setPlainText(str);
      biblioteq_misc_functions::highlightWidget
	(id.publisher->viewport(), m_queryHighlightColor);
    }

  if(id.alternate_id_1->text().trimmed().isEmpty())
    {
      id.alternate_id_1->setText(m.sru003());
      biblioteq_misc_functions::highlightWidget
	(id.alternate_id_1, m_queryHighlightColor);
    }

  str = m.targetAudience();

  if(!str.isEmpty())
    {
      if(id.target_audience->findText(str) > -1)
	id.target_audience->setCurrentIndex(id.target_audience->findText(str));
      else
	{
	  id.target_audience->insertItem(0, str);
	  id.target_audience->setCurrentIndex(0);
	}

      id.target_audience->setStyleSheet
	(QString("background-color: %1").arg(m_queryHighlightColor.name()));
    }

  str = m.title();

  if(!str.isEmpty())
    {
      id.title->setText(str);
      biblioteq_misc_functions::highlightWidget
	(id.title, m_queryHighlightColor);
    }

  str = m.volumeNumber();

  if(!str.isEmpty())
    {
      id.volume_number->setText(str);
      biblioteq_misc_functions::highlightWidget
	(id.volume_number, m_queryHighlightColor);
    }

  foreach(auto textfield, findChildren<QLineEdit *> ())
    textfield->setCursorPosition(0);
}

void biblioteq_book::populateAfterZ3950
(const QString &text, const biblioteq_marc::RECORD_SYNTAX recordSyntax)
{
  QString str("");
  QStringList list;
  biblioteq_marc m;
  auto isbn10User = false;
  auto isbn13User = false;

  if(id.id->text().remove('-').trimmed().length() == 10)
    isbn10User = true;

  if(id.isbn13->text().remove('-').trimmed().length() == 13)
    isbn13User = true;

  m.initialize(biblioteq_marc::BOOK, biblioteq_marc::Z3950, recordSyntax);
  m.parse(text);
  list = text.split("\n");
  id.edition->setCurrentIndex(0);
  id.edition->setStyleSheet
    (QString("background-color: %1").arg(m_queryHighlightColor.name()));
  id.marc_tags->setPlainText(text.trimmed());
  biblioteq_misc_functions::highlightWidget
    (id.marc_tags->viewport(), m_queryHighlightColor);

  for(int i = 0; i < list.size(); i++)
    if(list[i].startsWith("100 ") ||
       list[i].startsWith("700 "))
      id.author->clear();
    else if(list[i].startsWith("260 ") ||
	    list[i].startsWith("264 "))
      id.place->clear();
    else if(list[i].startsWith("650 "))
      id.category->clear();

  str = m.author();

  if(!str.isEmpty())
    {
      id.author->setPlainText(str);
      biblioteq_misc_functions::highlightWidget
	(id.author->viewport(), m_queryHighlightColor);
    }

  str = m.binding();

  if(id.binding->findText(str, Qt::MatchFixedString) > -1)
    id.binding->setCurrentIndex
      (id.binding->findText(str, Qt::MatchFixedString));

  id.binding->setStyleSheet
    (QString("background-color: %1").arg(m_queryHighlightColor.name()));
  str = m.callnum();

  if(!str.isEmpty())
    {
      id.callnum->setText(str);
      biblioteq_misc_functions::highlightWidget
	(id.callnum, m_queryHighlightColor);
    }

  str = m.category();

  if(!str.isEmpty())
    {
      id.category->setPlainText(str);
      biblioteq_misc_functions::highlightWidget
	(id.category->viewport(), m_queryHighlightColor);
    }

  str = m.description();

  if(!str.isEmpty())
    {
      id.description->setPlainText(str);
      biblioteq_misc_functions::highlightWidget
	(id.description->viewport(), m_queryHighlightColor);
    }

  str = m.deweynum();

  if(!str.isEmpty())
    {
      id.deweynum->setText(str);
      biblioteq_misc_functions::highlightWidget
	(id.deweynum, m_queryHighlightColor);
    }

  str = m.edition();

  if(id.edition->findText(str) > -1)
    id.edition->setCurrentIndex(id.edition->findText(str));
  else
    id.edition->setCurrentIndex(0);

  str = m.isbn10();

  if(!isbn10User && !str.isEmpty())
    {
      id.id->setText(qmain->formattedISBN10(str));
      biblioteq_misc_functions::highlightWidget(id.id, m_queryHighlightColor);
    }

  str = m.isbn13();

  if(!isbn13User && !str.isEmpty())
    {
      id.isbn13->setText(qmain->formattedISBN13(str));
      biblioteq_misc_functions::highlightWidget
	(id.isbn13, m_queryHighlightColor);
    }

  str = m.lcnum();

  if(!str.isEmpty())
    {
      id.lcnum->setText(str);
      biblioteq_misc_functions::highlightWidget
	(id.lcnum, m_queryHighlightColor);
    }

  str = m.place();

  if(!str.isEmpty())
    {
      id.place->setPlainText(str);
      biblioteq_misc_functions::highlightWidget
	(id.place->viewport(), m_queryHighlightColor);
    }

  if(!m.publicationDate().isNull())
    {
      id.publication_date->setDate(m.publicationDate());
      id.publication_date->setStyleSheet
	(QString("background-color: %1").arg(m_queryHighlightColor.name()));
    }

  str = m.publisher();

  if(!str.isEmpty())
    {
      id.publisher->setPlainText(str);
      biblioteq_misc_functions::highlightWidget
	(id.publisher->viewport(), m_queryHighlightColor);
    }

  str = m.targetAudience();

  if(!str.isEmpty())
    {
      if(id.target_audience->findText(str) > -1)
	id.target_audience->setCurrentIndex(id.target_audience->findText(str));
      else
	{
	  id.target_audience->insertItem(0, str);
	  id.target_audience->setCurrentIndex(0);
	}

      id.target_audience->setStyleSheet
	(QString("background-color: %1").arg(m_queryHighlightColor.name()));
    }

  str = m.title();

  if(!str.isEmpty())
    {
      id.title->setText(str);
      biblioteq_misc_functions::highlightWidget
	(id.title, m_queryHighlightColor);
    }

  str = m.volumeNumber();

  if(!str.isEmpty())
    {
      id.volume_number->setText(str);
      biblioteq_misc_functions::highlightWidget
	(id.volume_number, m_queryHighlightColor);
    }

  foreach(auto textfield, findChildren<QLineEdit *> ())
    textfield->setCursorPosition(0);
}

void biblioteq_book::populateFiles(void)
{
  id.files->setRowCount(0);
  id.files->setSortingEnabled(false);

  QSqlQuery query(qmain->getDB());

  query.prepare("SELECT COUNT(*) FROM book_files WHERE item_oid = ?");
  query.bindValue(0, m_oid);

  if(query.exec())
    if(query.next())
      id.files->setRowCount(query.value(0).toInt());

  query.prepare("SELECT file_name, "
		"file_digest, "
		"LENGTH(file) AS f_s, "
		"description, "
		"myoid FROM book_files "
                "WHERE item_oid = ? ORDER BY file_name");
  query.bindValue(0, m_oid);
  QApplication::setOverrideCursor(Qt::WaitCursor);

  QLocale locale;
  int row = 0;
  int totalRows = 0;

  if(query.exec())
    while(query.next() && totalRows < id.files->rowCount())
      {
	totalRows += 1;

	auto record(query.record());

	for(int i = 0; i < record.count(); i++)
	  {
	    QTableWidgetItem *item = nullptr;

	    if(record.fieldName(i) == "f_s")
	      item = new biblioteq_filesize_table_item
		(locale.toString(query.value(i).toLongLong()));
	    else
	      item = new QTableWidgetItem(query.value(i).toString().trimmed());

	    item->setData
	      (Qt::UserRole, query.value(record.count() - 1).toLongLong());
	    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

	    if(m_engWindowTitle == "Modify")
	      if(record.fieldName(i) == "description")
		item->setToolTip(tr("Double-click to edit."));

	    id.files->setItem(row, i, item);
	  }

	row += 1;
      }

  id.files->horizontalHeader()->setSortIndicator
    (Columns::FILE, Qt::AscendingOrder);
  id.files->setRowCount(totalRows);
  id.files->setSortingEnabled(true);
  QApplication::restoreOverrideCursor();
}

void biblioteq_book::resetQueryHighlights(void)
{
  id.alternate_id_1->setPalette(m_te_orig_pal);
  id.author->viewport()->setPalette(m_te_orig_pal);
  id.binding->setStyleSheet(m_cb_orig_ss);
  id.callnum->setPalette(m_white_pal);
  id.category->viewport()->setPalette(m_te_orig_pal);
  id.description->viewport()->setPalette(m_te_orig_pal);
  id.deweynum->setPalette(m_white_pal);
  id.edition->setStyleSheet(m_cb_orig_ss);
  id.id->setPalette(m_te_orig_pal);
  id.isbn13->setPalette(m_te_orig_pal);
  id.keyword->viewport()->setPalette(m_white_pal);
  id.lcnum->setPalette(m_white_pal);
  id.marc_tags->viewport()->setPalette(m_white_pal);
  id.place->viewport()->setPalette(m_te_orig_pal);
  id.publication_date->setStyleSheet(m_dt_orig_ss);
  id.publisher->viewport()->setPalette(m_te_orig_pal);
  id.target_audience->setStyleSheet(m_cb_orig_ss);
  id.title->setPalette(m_te_orig_pal);
  id.volume_number->setPalette(m_te_orig_pal);
}

void biblioteq_book::search(const QString &field, const QString &value)
{
  id.attach_files->setVisible(false);
  id.coverImages->setVisible(false);
  id.delete_files->setVisible(false);
  id.export_files->setVisible(false);
  id.files->setVisible(false);
  id.files_label->setVisible(false);
  id.view_pdf->setVisible(false);
  id.id->setText("%");
  id.category->clear();
  id.isbn13->setText("%");
  id.author->clear();
  id.lcnum->clear();
  id.callnum->clear();
  id.deweynum->clear();
  id.title->clear();
  id.publisher->clear();
  id.place->clear();
  id.description->clear();
  id.marc_tags->clear();
  id.keyword->clear();
  id.url->clear();
  id.copiesButton->setVisible(false);
  id.showUserButton->setVisible(false);
  id.openLibraryQuery->setVisible(false);
  id.sruQueryButton->setVisible(false);
  id.z3950QueryButton->setVisible(false);
  id.okButton->setText(tr("&Search"));
  id.publication_date->setDate(QDate::fromString("2001", "yyyy"));
  id.publication_date->setDisplayFormat("yyyy");
  id.publication_date_enabled->setVisible(true);
  id.price->setMinimum(-0.01);
  id.price->setValue(-0.01);
  id.quantity->setMinimum(0);
  id.quantity->setValue(0);
  id.edition->insertItem(0, tr("Any"));
  id.language->insertItem(0, tr("Any"));
  id.monetary_units->insertItem(0, tr("Any"));
  id.binding->insertItem(0, tr("Any"));
  id.location->insertItem(0, tr("Any"));
  id.originality->insertItem(0, tr("Any"));
  id.condition->insertItem(0, tr("Any"));
  id.target_audience->insertItem(0, "%");
  id.location->setCurrentIndex(0);
  id.edition->setCurrentIndex(0);
  id.language->setCurrentIndex(0);
  id.monetary_units->setCurrentIndex(0);
  id.binding->setCurrentIndex(0);
  id.originality->setCurrentIndex(0);
  id.condition->setCurrentIndex(0);
  id.target_audience->setCurrentIndex(0);
  id.accession_number->clear();
  id.isbnAvailableCheckBox->setCheckable(false);
  id.target_audience->setCurrentIndex(0);
  id.target_audience->setEditable(true);
  id.target_audience->setToolTip
    (tr("<html>Values from individual books are not included. "
	"Please see the Database Enumerations Browser.</html>"));
  id.volume_number->clear();
  m_engWindowTitle = "Search";

  if(field.isEmpty() && value.isEmpty())
    {
      auto actions = id.resetButton->menu()->actions();

      if(actions.size() >= 2)
	{
	  actions[0]->setVisible(false);
	  actions[1]->setVisible(false);
	}

      actions.clear();
      setWindowTitle(tr("BiblioteQ: Database Book Search"));
      id.id->setFocus();
#ifdef Q_OS_ANDROID
      showMaximized();
#else
      biblioteq_misc_functions::center(this, m_parentWid);
      showNormal();
#endif
      activateWindow();
      raise();
    }
  else
    {
      if(field == "publisher")
	id.publisher->setPlainText(value);
      else if(field == "author")
	id.author->setPlainText(value);
      else if(field == "category")
	id.category->setPlainText(value);
      else if(field == "place")
	id.place->setPlainText(value);
      else if(field == "keyword")
	id.keyword->setPlainText(value);

      slotGo();
    }
}

void biblioteq_book::setGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(auto widget, findChildren<QWidget *> ())
    {
      widget->setFont(font);
      widget->update();
    }

  id.files->resizeRowsToContents();
  update();
}

void biblioteq_book::slotAttachFiles(void)
{
  QFileDialog fileDialog(this, tr("BiblioteQ: Book Attachment(s)"));

  fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
  fileDialog.setDirectory(QDir::homePath());
  fileDialog.setFileMode(QFileDialog::ExistingFiles);
  fileDialog.setOption(QFileDialog::DontUseNativeDialog);

  if(fileDialog.exec() == QDialog::Accepted)
    {
      repaint();
      QApplication::processEvents();

      QProgressDialog progress(this);
      auto files(fileDialog.selectedFiles());

      progress.setLabelText(tr("Uploading files..."));
      progress.setMaximum(files.size());
      progress.setMinimum(0);
      progress.setModal(true);
      progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
      progress.show();
      progress.repaint();
      QApplication::processEvents();

      for(int i = 0; i < files.size() && !progress.wasCanceled(); i++)
	{
	  QCryptographicHash digest(QCryptographicHash::Sha1);
	  QFile file;
	  const auto &fileName(files.at(i));

	  file.setFileName(fileName);

	  if(file.open(QIODevice::ReadOnly))
	    {
	      QByteArray bytes(4096, 0);
	      QByteArray total;
	      qint64 rc = 0;

	      while((rc = file.read(bytes.data(), bytes.size())) > 0)
		{
		  digest.addData(bytes.mid(0, static_cast<int> (rc)));
		  total.append(bytes.mid(0, static_cast<int> (rc)));
		}

	      if(!total.isEmpty())
		{
		  total = qCompress(total, 9);
		  createFile(digest.result(), total,
			     QFileInfo(fileName).fileName());
		}
	    }

	  file.close();

	  if(i + 1 <= progress.maximum())
	    progress.setValue(i + 1);

	  progress.repaint();
	  QApplication::processEvents();
	}

      QApplication::restoreOverrideCursor();
      populateFiles();
    }

  QApplication::processEvents();
}

void biblioteq_book::slotCancel(void)
{
  close();
}

void biblioteq_book::slotCancelImageDownload(void)
{
  auto reply = m_imageManager->findChild<QNetworkReply *> ();

  if(reply)
    reply->deleteLater();

  m_imageBuffer.close();
}

void biblioteq_book::slotConvertISBN10to13(void)
{
  id.isbn13->setText
    (qmain->
     formattedISBN13(biblioteq_misc_functions::isbn10to13(id.id->text())));
}

void biblioteq_book::slotConvertISBN13to10(void)
{
  if(!id.isbn13->text().remove('-').trimmed().startsWith("978"))
    {
      QMessageBox::critical
	(this,
	 tr("BiblioteQ: User Error"),
	 tr("The ISBN-13 must begin with 978."));
      QApplication::processEvents();
      return;
    }

  id.id->setText
    (qmain->
     formattedISBN10(biblioteq_misc_functions::isbn13to10(id.isbn13->text())));
}

void biblioteq_book::slotDataTransferProgress(qint64 bytesread,
					      qint64 totalbytes)
{
  Q_UNUSED(bytesread);
  Q_UNUSED(totalbytes);
}

void biblioteq_book::slotDeleteFiles(void)
{
  auto list(id.files->selectionModel()->selectedRows(Columns::MYOID));

  if(list.isEmpty())
    {
      QMessageBox::critical
	(this, tr("BiblioteQ: User Error"),
	 tr("Please select at least one file to delete."));
      QApplication::processEvents();
      return;
    }

  if(QMessageBox::question(this, tr("BiblioteQ: Question"),
			   tr("Are you sure that you wish to delete the "
			      "selected file(s)?"),
			   QMessageBox::Yes | QMessageBox::No,
			   QMessageBox::No) == QMessageBox::No)
    {
      QApplication::processEvents();
      list.clear();
      return;
    }

  QApplication::processEvents();
  QApplication::setOverrideCursor(Qt::WaitCursor);

  for(int i = 0; i < list.size(); i++)
    {
      QSqlQuery query(qmain->getDB());

      query.prepare("DELETE FROM book_files WHERE item_oid = ? AND myoid = ?");
      query.bindValue(0, m_oid);
      query.bindValue(1, list.at(i).data());
      query.exec();
    }

  QApplication::restoreOverrideCursor();
  populateFiles();
}

void biblioteq_book::slotDownloadFinished(void)
{
  auto reply = qobject_cast<QNetworkReply *> (sender());

  if(reply)
    reply->deleteLater();

  QTimer::singleShot(250, this, SLOT(downloadFinished(void)));
}

void biblioteq_book::slotDownloadImage(void)
{
  if(m_imageManager->findChild<QNetworkReply *> ())
    return;

  auto action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  auto downloadType(action->property("download_type").toString());

  if(downloadType.contains("amazon") || downloadType.contains("open"))
    {
      auto ok = false;

      if(!id.alternate_id_1->text().trimmed().isEmpty())
	ok = true;

      if(id.isbnAvailableCheckBox->isChecked() &&
	 id.id->text().remove('-').trimmed().length() == 10)
	ok = true;

      if(!ok)
	{
	  QMessageBox::critical
	    (this,
	     tr("BiblioteQ: User Error"),
	     tr("In order to download a cover image, "
		"Alternate Identifier or ISBN-10 must be provided."));
	  QApplication::processEvents();
	  id.id->setFocus();
	  return;
	}
    }
  else
    {
      auto ok = false;

      if(!id.alternate_id_1->text().trimmed().isEmpty())
	ok = true;

      if(!ok)
	{
	  QMessageBox::critical
	    (this,
	     tr("BiblioteQ: User Error"),
	     tr("In order to download a cover image, "
		"Alternate Identifier must be provided."));
	  QApplication::processEvents();
	  id.id->setFocus();
	  return;
	}
    }

  if(downloadType.contains("back"))
    m_imageBuffer.setProperty("which", "back");
  else
    m_imageBuffer.setProperty("which", "front");

  if(!m_imageBuffer.open(QIODevice::WriteOnly))
    return;

  QUrl url;

  if(downloadType.contains("amazon"))
    {
      if(downloadType.contains("back"))
	url = QUrl::fromUserInput
	  (qmain->getAmazonHash().value("back_cover_host") +
	   QString(qmain->getAmazonHash().value("back_cover_path")).replace
	   ("%", id.id->text().remove('-').trimmed()));
      else
	url = QUrl::fromUserInput
	  (qmain->getAmazonHash().value("front_cover_host") +
	   QString(qmain->getAmazonHash().value("front_cover_path")).replace
	   ("%", id.id->text().remove('-').trimmed()));

      url.setScheme("https");

      QNetworkProxy proxy;
      QString type("none");
      auto hash(qmain->getAmazonHash());

      if(downloadType.contains("back"))
	type = hash.value("back_proxy_type").toLower().trimmed();
      else
	type = hash.value("front_proxy_type").toLower().trimmed();

      if(type == "none")
	{
	  proxy.setType(QNetworkProxy::NoProxy);
	  m_imageManager->setProxy(proxy);
	}
      else
	{
	  if(type == "http" || type == "socks5" || type == "system")
	    {
	      /*
	      ** This is required to resolve an odd error.
	      */

	      auto reply = m_imageManager->get
		(QNetworkRequest(QUrl::fromUserInput("http://0.0.0.0")));

	      if(reply)
		reply->deleteLater();

	      connect
		(m_imageManager,
		 SIGNAL(proxyAuthenticationRequired(const QNetworkProxy &,
						    QAuthenticator *)),
		 this,
		 SLOT(slotProxyAuthenticationRequired(const QNetworkProxy &,
						      QAuthenticator *)),
		 Qt::UniqueConnection);
	    }

	  if(type == "http" || type == "socks5")
	    {
	      if(type == "http")
		proxy.setType(QNetworkProxy::HttpProxy);
	      else
		proxy.setType(QNetworkProxy::Socks5Proxy);

	      QString host("");
	      QString password("");
	      QString user("");
	      quint16 port = 0;

	      if(downloadType.contains("back"))
		{
		  host = hash.value("back_proxy_host");
		  port = hash.value("back_proxy_port").toUShort();
		  user = hash.value("back_proxy_username");
		  password = hash.value("back_proxy_password");
		}
	      else
		{
		  host = hash.value("front_proxy_host");
		  port = hash.value("front_proxy_port").toUShort();
		  user = hash.value("front_proxy_username");
		  password = hash.value("front_proxy_password");
		}

	      proxy.setHostName(host);
	      proxy.setPort(port);

	      if(!user.isEmpty())
		proxy.setUser(user);

	      if(!password.isEmpty())
		proxy.setPassword(password);

	      m_imageManager->setProxy(proxy);
	    }
	  else if(type == "system")
	    {
	      QList<QNetworkProxy> list;
	      QNetworkProxyQuery query(url);

	      list = QNetworkProxyFactory::systemProxyForQuery(query);

	      if(!list.isEmpty())
		proxy = list.at(0);

	      m_imageManager->setProxy(proxy);
	    }
	  else
	    {
	      proxy.setType(QNetworkProxy::NoProxy);
	      m_imageManager->setProxy(proxy);
	    }
	}
    }
  else if(downloadType.contains("open"))
    {
      QString string("");

      if(downloadType.contains("back"))
	string = qmain->getOpenLibraryImagesHash().value("back_url");
      else
	string = qmain->getOpenLibraryImagesHash().value("front_url");

      if(id.isbnAvailableCheckBox->isChecked() &&
	 id.id->text().remove('-').trimmed().length() == 10)
	{
	  string.replace("$key", "isbn");
	  string.replace
	    ("$value-$size", id.id->text().remove('-').trimmed() + "-L");
	}
      else
	{
	  string.replace("$key", "olid");
	  string.replace
	    ("$value-$size", id.alternate_id_1->text().trimmed() + "-L");
	}

      url = QUrl::fromUserInput(string);

      QNetworkProxy proxy;
      auto hash(qmain->getOpenLibraryImagesHash());
      auto type(hash.value("proxy_type").toLower().trimmed());

      if(type == "none")
	{
	  proxy.setType(QNetworkProxy::NoProxy);
	  m_imageManager->setProxy(proxy);
	}
      else
	{
	  if(type == "http" || type == "socks5" || type == "system")
	    {
	      /*
	      ** This is required to resolve an odd error.
	      */

	      auto reply = m_imageManager->get
		(QNetworkRequest(QUrl::fromUserInput("http://0.0.0.0")));

	      if(reply)
		reply->deleteLater();

	      connect
		(m_imageManager,
		 SIGNAL(proxyAuthenticationRequired(const QNetworkProxy &,
						    QAuthenticator *)),
		 this,
		 SLOT(slotProxyAuthenticationRequired(const QNetworkProxy &,
						      QAuthenticator *)),
		 Qt::UniqueConnection);
	    }

	  if(type == "http" || type == "socks5")
	    {
	      if(type == "http")
		proxy.setType(QNetworkProxy::HttpProxy);
	      else
		proxy.setType(QNetworkProxy::Socks5Proxy);

	      QString host("");
	      QString password("");
	      QString user("");
	      quint16 port = 0;

	      host = hash.value("proxy_host");
	      password = hash.value("proxy_password");
	      port = hash.value("proxy_port").toUShort();
	      user = hash.value("proxy_username");
	      proxy.setHostName(host);
	      proxy.setPort(port);

	      if(!user.isEmpty())
		proxy.setUser(user);

	      if(!password.isEmpty())
		proxy.setPassword(password);

	      m_imageManager->setProxy(proxy);
	    }
	  else if(type == "system")
	    {
	      QList<QNetworkProxy> list;
	      QNetworkProxyQuery query(url);

	      list = QNetworkProxyFactory::systemProxyForQuery(query);

	      if(!list.isEmpty())
		proxy = list.at(0);

	      m_imageManager->setProxy(proxy);
	    }
	  else
	    {
	      proxy.setType(QNetworkProxy::NoProxy);
	      m_imageManager->setProxy(proxy);
	    }
	}
    }
  else
    {
      QString string("");

      if(downloadType.contains("back"))
	string = qmain->otherImagesHash().value("back_url");
      else
	string = qmain->otherImagesHash().value("front_url");

      string.replace("%1", id.alternate_id_1->text());
      url = QUrl::fromUserInput(string);
    }

  auto dialog = createImageDownloadDialog(downloadType);

  if(!dialog)
    {
      m_imageBuffer.close();
      return;
    }

  auto reply = m_imageManager->get(QNetworkRequest(url));

  if(!reply)
    {
      dialog->deleteLater();
      m_imageBuffer.close();
      return;
    }

  connect(reply, SIGNAL(readyRead(void)),
	  this, SLOT(slotReadyRead(void)));
  connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
	  this, SLOT(slotDataTransferProgress(qint64, qint64)));
  connect(reply, SIGNAL(finished(void)),
	  dialog, SLOT(deleteLater(void)));
  connect(reply, SIGNAL(finished(void)),
	  this, SLOT(slotDownloadFinished(void)));
  reply->ignoreSslErrors();
  QApplication::processEvents();
}

void biblioteq_book::slotExportFiles(void)
{
  auto list(id.files->selectionModel()->selectedRows(Columns::MYOID));

  if(list.isEmpty())
    return;

  QFileDialog dialog(this);

  dialog.setDirectory(QDir::homePath());
  dialog.setFileMode(QFileDialog::Directory);
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowTitle(tr("BiblioteQ: Book File Export"));
  dialog.exec();
  QApplication::processEvents();

  if(dialog.result() == QDialog::Accepted)
    {
      repaint();
      QApplication::processEvents();

      QProgressDialog progress(this);

      progress.setLabelText(tr("Exporting file(s)..."));
      progress.setMaximum(list.size());
      progress.setMinimum(0);
      progress.setModal(true);
      progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
      progress.show();
      progress.repaint();
      QApplication::processEvents();

      for(int i = 0; i < list.size() && !progress.wasCanceled(); i++)
	{
	  QSqlQuery query(qmain->getDB());

	  query.setForwardOnly(true);
	  query.prepare("SELECT file, file_name FROM book_files "
			"WHERE item_oid = ? AND myoid = ?");
	  query.bindValue(0, m_oid);
	  query.bindValue(1, list.at(i).data());

	  if(query.exec() && query.next())
	    {
	      QFile file(dialog.selectedFiles().value(0) + QDir::separator() +
			 query.value(1).toString().trimmed());

	      if(file.open(QIODevice::WriteOnly))
		file.write(qUncompress(query.value(0).toByteArray()));

	      file.flush();
	      file.close();
	    }

	  if(i + 1 <= progress.maximum())
	    progress.setValue(i + 1);

	  progress.repaint();
	  QApplication::processEvents();
	}
    }
}

void biblioteq_book::slotFilesDoubleClicked(QTableWidgetItem *item)
{
  if(!item)
    return;

  if(item->column() != Columns::DESCRIPTION || m_engWindowTitle != "Modify")
    {
      auto item1 = id.files->item(item->row(), Columns::FILE);

      if(!item1)
	return;

#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
      if(item1->text().toLower().trimmed().endsWith(".pdf"))
	{
	  QApplication::setOverrideCursor(Qt::WaitCursor);

	  QByteArray data;
	  QSqlQuery query(qmain->getDB());

	  query.setForwardOnly(true);
	  query.prepare("SELECT file, file_name FROM book_files "
			"WHERE item_oid = ? AND myoid = ?");
	  query.addBindValue(m_oid);
	  query.addBindValue(item1->data(Qt::UserRole).toLongLong());

	  if(query.exec() && query.next())
	    data = qUncompress(query.value(0).toByteArray());

	  if(!data.isEmpty())
	    {
	      auto reader = new biblioteq_pdfreader(qmain);

	      reader->load(data, item1->text());
	      biblioteq_misc_functions::center(reader, this);
	      reader->show();
	    }

	  QApplication::restoreOverrideCursor();
	}
#endif

      return;
    }

  if(m_engWindowTitle != "Modify")
    return;

  auto item1 = id.files->item(item->row(), Columns::DESCRIPTION);

  if(!item1)
    return;

  auto description(item1->text());
  auto item2 = id.files->item(item->row(), Columns::MYOID);

  if(!item2)
    return;

  auto ok = true;
  auto text
    (QInputDialog::getText(this,
			   tr("BiblioteQ: File Description"),
			   tr("Description"), QLineEdit::Normal,
			   description, &ok).trimmed());

  if(!ok)
    return;

  QSqlQuery query(qmain->getDB());
  auto myoid(item2->text());

  query.prepare("UPDATE book_files SET description = ? "
		"WHERE item_oid = ? AND myoid = ?");
  query.bindValue(0, text);
  query.bindValue(1, m_oid);
  query.bindValue(2, myoid);

  if(query.exec())
    item1->setText(text);
}

void biblioteq_book::slotGo(void)
{
  QString errorstr = "";
  QString searchstr = "";
  QString str = "";
  int i = 0;
  int maxcopynumber = 0;
  int newq = 0;

  if(m_engWindowTitle.contains("Create") ||
     m_engWindowTitle.contains("Modify"))
    {
      if(m_engWindowTitle.contains("Modify") && m_index->isValid())
	{
	  newq = id.quantity->value();
	  QApplication::setOverrideCursor(Qt::WaitCursor);
	  maxcopynumber = biblioteq_misc_functions::getMaxCopyNumber
	    (qmain->getDB(), m_oid, "Book", errorstr);

	  if(maxcopynumber < 0)
	    {
	      QApplication::restoreOverrideCursor();
	      qmain->addError
		(QString(tr("Database Error")),
		 QString(tr("Unable to determine the maximum copy number of "
			    "the item.")),
		 errorstr, __FILE__, __LINE__);
	      QMessageBox::critical
		(this, tr("BiblioteQ: Database Error"),
		 tr("Unable to determine the maximum copy number of "
		    "the item."));
	      QApplication::processEvents();
	      return;
	    }

	  QApplication::restoreOverrideCursor();

	  if(newq < maxcopynumber)
	    {
	      QMessageBox::critical
		(this, tr("BiblioteQ: User Error"),
		 tr("It appears that you are attempting to "
		    "decrease the "
		    "number of copies while there are "
		    "copies "
		    "that have been reserved."));
	      QApplication::processEvents();
	      id.quantity->setValue(m_oldq);
	      return;
	    }
	  else if(newq > m_oldq)
	    {
	      if(QMessageBox::question
		 (this, tr("BiblioteQ: Question"),
		  tr("You have increased the number of copies. "
		     "Would you like to modify copy information?"),
		  QMessageBox::Yes | QMessageBox::No,
		  QMessageBox::No) == QMessageBox::Yes)
		{
		  QApplication::processEvents();
		  slotPopulateCopiesEditor();
		}

	      QApplication::processEvents();
	    }
	}

      str = id.id->text().trimmed();
      id.id->setText(qmain->formattedISBN10(str));
      str = id.isbn13->text().trimmed();
      id.isbn13->setText(qmain->formattedISBN13(str));

      if(id.isbnAvailableCheckBox->isChecked())
	{
	  if(id.id->text().remove('-').length() == 10)
	    if(!(id.isbn13->text().startsWith("978") ||
		 id.isbn13->text().startsWith("979")))
	      slotConvertISBN10to13();
	}
      else
	id.id->clear();

      if(id.isbnAvailableCheckBox->isChecked())
	{
	  if(id.isbn13->text().remove('-').length() == 13 &&
	     id.isbn13->text().startsWith("978"))
	    slotConvertISBN13to10();
	}
      else
	id.isbn13->clear();

      if(id.isbnAvailableCheckBox->isChecked())
	if(!(id.id->text().remove('-').length() == 10 ||
	     id.isbn13->text().remove('-').length() == 13))
	  {
	    QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				  tr("Please complete the ISBN-10 or "
				     "the ISBN-13 field."));
	    QApplication::processEvents();

	    if(id.id->text().remove('-').length() != 10)
	      id.id->setFocus();
	    else
	      id.isbn13->setFocus();

	    return;
	  }

      str = id.author->toPlainText().trimmed();
      id.author->setPlainText(str);

      if(id.author->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Authors field."));
	  QApplication::processEvents();
	  id.author->setFocus();
	  return;
	}

      str = id.title->text().trimmed();
      id.title->setText(str);

      if(id.title->text().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Title field."));
	  QApplication::processEvents();
	  id.title->setFocus();
	  return;
	}

      str = id.publisher->toPlainText().trimmed();
      id.publisher->setPlainText(str);

      if(id.publisher->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Publisher field."));
	  QApplication::processEvents();
	  id.publisher->setFocus();
	  return;
	}

      str = id.place->toPlainText().trimmed();
      id.place->setPlainText(str);

      if(id.place->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Place of Publication "
				   "field."));
	  QApplication::processEvents();
	  id.place->setFocus();
	  return;
	}

      str = id.category->toPlainText().trimmed();
      id.category->setPlainText(str);

      if(id.category->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Categories field."));
	  QApplication::processEvents();
	  id.category->setFocus();
	  return;
	}

      str = id.description->toPlainText().trimmed();
      id.description->setPlainText(str);

      if(id.description->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Abstract field."));
	  QApplication::processEvents();
	  id.description->setFocus();
	  return;
	}

      QApplication::setOverrideCursor(Qt::WaitCursor);

      if(!qmain->getDB().transaction())
	{
	  QApplication::restoreOverrideCursor();
	  qmain->addError
	    (QString(tr("Database Error")),
	     QString(tr("Unable to create a database transaction.")),
	     qmain->getDB().lastError().text(), __FILE__, __LINE__);
	  QMessageBox::critical
	    (this, tr("BiblioteQ: Database Error"),
	     tr("Unable to create a database transaction."));
	  QApplication::processEvents();
	  return;
	}

      QApplication::restoreOverrideCursor();
      str = id.lcnum->text().trimmed();
      id.lcnum->setText(str);
      str = id.callnum->text().trimmed();
      id.callnum->setText(str);
      str = id.deweynum->text().trimmed();
      id.deweynum->setText(str);
      str = id.marc_tags->toPlainText().trimmed();
      id.marc_tags->setPlainText(str);
      str = id.keyword->toPlainText().trimmed();
      id.keyword->setPlainText(str);
      str = id.accession_number->text().trimmed();
      id.accession_number->setText(str);
      id.url->setPlainText(id.url->toPlainText().trimmed());
      id.alternate_id_1->setText(id.alternate_id_1->text().trimmed());
      id.multivolume_set_isbn->setText
	(id.multivolume_set_isbn->text().remove('-').trimmed());
      id.volume_number->setText(id.volume_number->text().trimmed());

      if(id.multivolume_set_isbn->text().length() == 10)
	id.multivolume_set_isbn->setText
	  (qmain->formattedISBN10(id.multivolume_set_isbn->text()));
      else if(id.multivolume_set_isbn->text().length() == 13)
	id.multivolume_set_isbn->setText
	  (qmain->formattedISBN13(id.multivolume_set_isbn->text()));

      QSqlQuery query(qmain->getDB());

      if(m_engWindowTitle.contains("Modify"))
	query.prepare("UPDATE book SET id = ?, "
		      "title = ?, "
		      "edition = ?, author = ?, "
		      "pdate = ?, "
		      "publisher = ?, "
		      "category = ?, price = ?, "
		      "description = ?, "
		      "language = ?, "
		      "monetary_units = ?, "
		      "quantity = ?, "
		      "binding_type = ?, "
		      "location = ?, "
		      "isbn13 = ?, "
		      "lccontrolnumber = ?, "
		      "callnumber = ?, "
		      "deweynumber = ?, "
		      "front_cover = ?, "
		      "back_cover = ?, "
		      "place = ?, "
		      "marc_tags = ?, "
		      "keyword = ?, "
		      "originality = ?, "
		      "condition = ?, "
		      "accession_number = ?, "
		      "url = ?, "
		      "alternate_id_1 = ?, "
		      "multivolume_set_isbn = ?, "
		      "target_audience = ?, "
		      "volume_number = ? "
		      "WHERE "
		      "myoid = ?");
      else if(qmain->getDB().driverName() != "QSQLITE")
	query.prepare("INSERT INTO book (id, title, "
		      "edition, author, pdate, publisher, "
		      "category, price, description, language, "
		      "monetary_units, quantity, "
		      "binding_type, location, "
		      "isbn13, lccontrolnumber, callnumber, "
		      "deweynumber, front_cover, "
		      "back_cover, "
		      "place, marc_tags, keyword, originality, condition, "
		      "accession_number, url, alternate_id_1, "
		      "multivolume_set_isbn, target_audience, volume_number) "
		      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
		      "?, ?, ?, "
		      "?, ?, ?, "
		      "?, ?, ?, "
		      "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) RETURNING myoid");
      else
	query.prepare("INSERT INTO book (id, title, "
		      "edition, author, pdate, publisher, "
		      "category, price, description, language, "
		      "monetary_units, quantity, "
		      "binding_type, location, "
		      "isbn13, lccontrolnumber, callnumber, "
		      "deweynumber, front_cover, "
		      "back_cover, "
		      "place, marc_tags, keyword, originality, condition, "
		      "accession_number, url, alternate_id_1, "
		      "multivolume_set_isbn, target_audience, "
		      "volume_number, myoid) "
		      "VALUES (?, ?, ?, ?, ?, ?, "
		      "?, ?, ?, ?, ?, "
		      "?, ?, ?, "
		      "?, ?, ?, "
		      "?, ?, ?, ?, ?, ?, ?, ?, ?, "
		      "?, ?, ?, ?, ?, ?)");

      if(id.isbnAvailableCheckBox->isChecked() &&
	 !id.id->text().remove('-').isEmpty())
	query.bindValue(0, id.id->text().remove('-'));
      else
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	query.bindValue(0, QVariant(QMetaType(QMetaType::QString)));
#else
	query.bindValue(0, QVariant(QVariant::String));
#endif

      query.bindValue(1, id.title->text().trimmed());
      query.bindValue(2, id.edition->currentText().trimmed());
      query.bindValue(3, id.author->toPlainText().trimmed());
      query.bindValue
	(4,
	 id.publication_date->date().toString(biblioteq::s_databaseDateFormat));
      query.bindValue(5, id.publisher->toPlainText().trimmed());
      query.bindValue(6, id.category->toPlainText().trimmed());
      query.bindValue(7, id.price->value());
      query.bindValue(8, id.description->toPlainText());
      query.bindValue(9, id.language->currentText().trimmed());
      query.bindValue(10, id.monetary_units->currentText().trimmed());
      query.bindValue(11, id.quantity->text());
      query.bindValue(12, id.binding->currentText().trimmed());
      query.bindValue(13, id.location->currentText().trimmed());

      if(id.isbnAvailableCheckBox->isChecked() &&
	 !id.isbn13->text().remove('-').isEmpty())
	query.bindValue(14, id.isbn13->text().remove('-'));
      else
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	query.bindValue(14, QVariant(QMetaType(QMetaType::QString)));
#else
	query.bindValue(14, QVariant(QVariant::String));
#endif

      if(!id.lcnum->text().isEmpty())
	query.bindValue(15, id.lcnum->text());
      else
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	query.bindValue(15, QVariant(QMetaType(QMetaType::QString)));
#else
	query.bindValue(15, QVariant(QVariant::String));
#endif

      if(!id.callnum->text().isEmpty())
	query.bindValue(16, id.callnum->text());
      else
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	query.bindValue(16, QVariant(QMetaType(QMetaType::QString)));
#else
	query.bindValue(16, QVariant(QVariant::String));
#endif

      if(!id.deweynum->text().isEmpty())
	query.bindValue(17, id.deweynum->text());
      else
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	query.bindValue(17, QVariant(QMetaType(QMetaType::QString)));
#else
	query.bindValue(17, QVariant(QVariant::String));
#endif

      if(!id.front_image->m_image.isNull())
	{
	  QByteArray bytes;
	  QBuffer buffer(&bytes);

	  if(buffer.open(QIODevice::WriteOnly))
	    {
	      id.front_image->m_image.save
		(&buffer, id.front_image->m_imageFormat.toLatin1(), 100);
	      query.bindValue(18, bytes.toBase64());
	    }
	  else
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	    query.bindValue(18, QVariant(QMetaType(QMetaType::QByteArray)));
#else
	    query.bindValue(18, QVariant(QVariant::ByteArray));
#endif
	}
      else
	{
	  id.front_image->m_imageFormat = "";
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	  query.bindValue(18, QVariant(QMetaType(QMetaType::QByteArray)));
#else
	  query.bindValue(18, QVariant(QVariant::ByteArray));
#endif
	}

      if(!id.back_image->m_image.isNull())
	{
	  QBuffer buffer;
	  QByteArray bytes;

	  buffer.setBuffer(&bytes);

	  if(buffer.open(QIODevice::WriteOnly))
	    {
	      id.back_image->m_image.save
		(&buffer, id.back_image->m_imageFormat.toLatin1(), 100);
	      query.bindValue(19, bytes.toBase64());
	    }
	  else
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	    query.bindValue(19, QVariant(QMetaType(QMetaType::QByteArray)));
#else
	    query.bindValue(19, QVariant(QVariant::ByteArray));
#endif
	}
      else
	{
	  id.back_image->m_imageFormat = "";
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	  query.bindValue(19, QVariant(QMetaType(QMetaType::QByteArray)));
#else
	  query.bindValue(19, QVariant(QVariant::ByteArray));
#endif
	}

      query.bindValue(20, id.place->toPlainText().trimmed());
      query.bindValue(21, id.marc_tags->toPlainText().trimmed());
      query.bindValue(22, id.keyword->toPlainText().trimmed());
      query.bindValue(23, id.originality->currentText().trimmed());
      query.bindValue(24, id.condition->currentText().trimmed());
      query.bindValue(25, id.accession_number->text().trimmed());
      query.bindValue(26, id.url->toPlainText().trimmed());
      query.bindValue(27, id.alternate_id_1->text().trimmed());

      if(id.multivolume_set_isbn->text().isEmpty())
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	query.bindValue(28, QVariant(QMetaType(QMetaType::QString)));
#else
	query.bindValue(28, QVariant(QVariant::String));
#endif
      else
	query.bindValue(28, id.multivolume_set_isbn->text().remove('-'));

      query.bindValue(29, id.target_audience->currentText());
      query.bindValue(30, id.volume_number->text().trimmed());

      if(m_engWindowTitle.contains("Modify"))
	query.bindValue(31, m_oid);
      else if(qmain->getDB().driverName() == "QSQLITE")
	{
	  auto value = biblioteq_misc_functions::getSqliteUniqueId
	    (qmain->getDB(), errorstr);

	  if(errorstr.isEmpty())
	    {
	      m_oid = QString::number(value);
	      query.bindValue(31, value);
	    }
	  else
	    qmain->addError
	      (QString(tr("Database Error")),
	       QString(tr("Unable to generate a unique integer.")),
	       errorstr);
	}

      QApplication::setOverrideCursor(Qt::WaitCursor);

      if(!query.exec())
	{
	  QApplication::restoreOverrideCursor();
	  qmain->addError
	    (QString(tr("Database Error")),
	     QString(tr("Unable to create or update the entry.")),
	     query.lastError().text(),
	     __FILE__,
	     __LINE__);
	  goto db_rollback;
	}
      else
	{
	  /*
	  ** Remove copies if the quantity has been decreased.
	  */

	  if(m_engWindowTitle.contains("Modify"))
	    {
	      /*
	      ** Retain quantity copies.
	      */

	      query.prepare("DELETE FROM book_copy_info WHERE "
			    "myoid NOT IN "
			    "(SELECT myoid FROM book_copy_info "
			    "WHERE item_oid = ? ORDER BY copy_number "
			    "LIMIT ?) AND item_oid = ?");
	      query.addBindValue(m_oid);
	      query.addBindValue(id.quantity->text());
	      query.addBindValue(m_oid);

	      if(!query.exec())
		{
		  QApplication::restoreOverrideCursor();
		  qmain->addError
		    (QString(tr("Database Error")),
		     QString(tr("Unable to purge unnecessary copy "
				"data.")),
		     query.lastError().text(), __FILE__,
		     __LINE__);
		  goto db_rollback;
		}

	      if(!qmain->getDB().commit())
		{
		  QApplication::restoreOverrideCursor();
		  qmain->addError
		    (QString(tr("Database Error")),
		     QString(tr("Unable to commit the current database "
				"transaction.")),
		     qmain->getDB().lastError().text(), __FILE__,
		     __LINE__);
		  goto db_rollback;
		}
	    }
	  else
	    {
	      /*
	      ** Create initial copies.
	      */

	      if(qmain->getDB().driverName() != "QSQLITE")
		{
		  query.next();
		  m_oid = query.value(0).toString().trimmed();
		}

	      if(id.id->text().remove('-').isEmpty())
		biblioteq_misc_functions::
		  createInitialCopies(m_oid,
				      id.quantity->value(),
				      qmain->getDB(),
				      "Book", errorstr);
	      else
		biblioteq_misc_functions::
		  createInitialCopies(id.id->text().remove('-'),
				      id.quantity->value(),
				      qmain->getDB(),
				      "Book", errorstr);

	      if(!errorstr.isEmpty())
		{
		  QApplication::restoreOverrideCursor();
		  qmain->addError
		    (QString(tr("Database Error")),
		     QString(tr("Unable to create initial copies.")),
		     errorstr, __FILE__, __LINE__);
		  goto db_rollback;
		}

	      if(!qmain->getDB().commit())
		{
		  QApplication::restoreOverrideCursor();
		  qmain->addError
		    (QString(tr("Database Error")),
		     QString(tr("Unable to commit the current database "
				"transaction.")),
		     qmain->getDB().lastError().text(), __FILE__,
		     __LINE__);
		  goto db_rollback;
		}
	    }

	  if(!id.isbnAvailableCheckBox->isChecked())
	    {
	      id.id->clear();
	      id.isbn13->clear();
	    }

	  m_oldq = id.quantity->value();
	  resetQueryHighlights();

	  if(id.front_image->m_image.isNull())
	    id.front_image->m_imageFormat = "";

	  if(id.back_image->m_image.isNull())
	    id.back_image->m_imageFormat = "";

	  id.author->setMultipleLinks("book_search", "author",
				      id.author->toPlainText());
	  id.category->setMultipleLinks("book_search", "category",
					id.category->toPlainText());
	  id.publisher->setMultipleLinks("book_search", "publisher",
					 id.publisher->toPlainText());
	  id.place->setMultipleLinks("book_search", "place",
				     id.place->toPlainText());
	  id.keyword->setMultipleLinks("book_search", "keyword",
				       id.keyword->toPlainText());
	  QApplication::restoreOverrideCursor();

	  if(m_engWindowTitle.contains("Modify"))
	    {
	      if(!id.id->text().remove('-').isEmpty())
		str = QString(tr("BiblioteQ: Modify Book Entry (")) +
		  id.id->text() + tr(")");
	      else
		str = tr("BiblioteQ: Modify Book Entry");

	      m_engWindowTitle = "Modify";
	      setWindowTitle(str);

	      if(m_index->isValid() &&
		 (qmain->getTypeFilterString() == "All" ||
		  qmain->getTypeFilterString() == "All Available" ||
		  qmain->getTypeFilterString() == "All Overdue" ||
		  qmain->getTypeFilterString() == "All Requested" ||
		  qmain->getTypeFilterString() == "All Reserved" ||
		  qmain->getTypeFilterString() == "Books"))
		{
		  qmain->getUI().table->setSortingEnabled(false);

		  auto names(qmain->getUI().table->columnNames());
		  int imageColumn = -1;

		  for(i = 0; i < names.size(); i++)
		    {
		      if(names.at(i) == "Call Number")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (id.callnum->text());
		      else if(names.at(i) == "ISBN-10" ||
			      names.at(i) == "ID Number")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (id.id->text());
		      else if(names.at(i) == "Title")
			{
			  imageColumn = i;
			  qmain->getUI().table->item(m_index->row(), i)->setText
			    (id.title->text());
			}
		      else if(names.at(i) == "Edition")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (id.edition->currentText().trimmed());
		      else if(names.at(i) == "Authors")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (id.author->toPlainText());
		      else if(names.at(i) == "Publication Date")
			{
			  if(qmain->getTypeFilterString() == "Books")
			    qmain->getUI().table->item(m_index->row(), i)->
			      setText
			      (id.publication_date->date().
			       toString(qmain->publicationDateFormat("books")));
			  else
			    qmain->getUI().table->item(m_index->row(), i)->
			      setText(id.publication_date->date().
				      toString(Qt::ISODate));
			}
		      else if(names.at(i) == "Publisher")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (id.publisher->toPlainText());
		      else if(names.at(i) == "Place of Publication")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (id.place->toPlainText());
		      else if(names.at(i) == "Categories")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (id.category->toPlainText());
		      else if(names.at(i) == "Price")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (QLocale().toString(id.price->value()));
		      else if(names.at(i) == "Language")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (id.language->currentText().trimmed());
		      else if(names.at(i) == "Monetary Units")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (id.monetary_units->currentText().trimmed());
		      else if(names.at(i) == "Quantity")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (id.quantity->text());
		      else if(names.at(i) == "Book Binding Type")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (id.binding->currentText().trimmed());
		      else if(names.at(i) == "Location")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (id.location->currentText().trimmed());
		      else if(names.at(i) == "ISBN-13")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (id.isbn13->text());
		      else if(names.at(i) == "LC Control Number")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (id.lcnum->text());
		      else if(names.at(i) == "Call Number")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (id.callnum->text());
		      else if(names.at(i) == "Dewey Class Number")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (id.deweynum->text());
		      else if(names.at(i) == "Originality")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (id.originality->currentText().trimmed());
		      else if(names.at(i) == "Condition")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (id.condition->currentText().trimmed());
		      else if(names.at(i) == "Accession Number")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (id.accession_number->text());
		      else if(names.at(i) == "Alternate Identifier")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (id.alternate_id_1->text());
		      else if(names.at(i) == "Availability")
			{
			  qmain->getUI().table->item(m_index->row(), i)->setText
			    (biblioteq_misc_functions::getAvailability
			     (m_oid, qmain->getDB(), "Book", errorstr));

			  if(!errorstr.isEmpty())
			    qmain->addError
			      (QString(tr("Database Error")),
			       QString(tr("Retrieving availability.")),
			       errorstr, __FILE__, __LINE__);
			}
		      else if(names.at(i) == "Volume Number")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (id.volume_number->text().trimmed());
		    }

		  if(imageColumn == -1)
		    imageColumn = 0;

		  if(qmain->showMainTableImages())
		    {
		      auto pixmap(QPixmap::fromImage(id.front_image->m_image));

		      if(!pixmap.isNull())
			qmain->getUI().table->item
			  (m_index->row(), imageColumn)->setIcon(pixmap);
		      else
			qmain->getUI().table->item
			  (m_index->row(), imageColumn)->setIcon
			  (QIcon(":/no_image.png"));
		    }

		  qmain->getUI().table->setSortingEnabled(true);
		  qmain->getUI().table->updateToolTips(m_index->row());

		  foreach(auto textfield, findChildren<QLineEdit *> ())
		    textfield->setCursorPosition(0);

		  qmain->slotResizeColumns();
		}

	      qmain->slotDisplaySummary();
	      qmain->updateSceneItem(m_oid, "Book", id.front_image->m_image);
	    }
	  else
	    {
	      qmain->replaceBook(m_oid, this);
	      updateWindow(biblioteq::EDITABLE);

	      if(qmain->getUI().actionAutoPopulateOnCreation->isChecked())
		(void) qmain->populateTable
		  (biblioteq::POPULATE_ALL, "Books", "");

	      raise();
	    }

	  storeData(this);
	}

      return;

    db_rollback:

      QApplication::setOverrideCursor(Qt::WaitCursor);

      if(m_engWindowTitle.contains("Create"))
	m_oid.clear();

      if(!qmain->getDB().rollback())
	qmain->addError
	  (QString(tr("Database Error")), QString(tr("Rollback failure.")),
	   qmain->getDB().lastError().text(), __FILE__, __LINE__);

      QApplication::restoreOverrideCursor();
      QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			    tr("Unable to create or update the entry. "
			       "Please verify that "
			       "the entry does not already exist."));
      QApplication::processEvents();
    }
  else if(m_engWindowTitle.contains("Search"))
    {
      QList<QVariant> values;
      QString frontCover("'' AS front_cover ");

      if(qmain->showMainTableImages())
	frontCover = "book.front_cover ";

      searchstr = "SELECT DISTINCT book.title, "
	"book.author, "
	"book.publisher, "
	"book.pdate, "
	"book.place, "
	"book.edition, "
	"book.category, "
	"book.language, "
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
	"book.quantity - COUNT(item_borrower.item_oid) AS availability, "
	"COUNT(item_borrower.item_oid) AS total_reserved, "
	"book.originality, "
	"book.condition, "
	"book.accession_number, "
	"book.alternate_id_1, "
	"book.volume_number, "
	"book.type, "
	"book.myoid, " +
	frontCover +
	"FROM "
	"book LEFT JOIN item_borrower ON "
	"book.myoid = item_borrower.item_oid "
	"AND item_borrower.type = 'Book' "
	"WHERE ";

      if(!id.id->text().remove('-').trimmed().isEmpty())
	{
	  searchstr.append
	    ("LOWER(COALESCE(id, '')) LIKE LOWER('%' || ? || '%') AND ");
	  values.append(id.id->text().remove('-').trimmed());
	}
      else
	searchstr.append("id IS NULL AND ");

      QString ESCAPE("");
      auto UNACCENT(qmain->unaccent());

      if(qmain->getDB().driverName() != "QSQLITE")
	ESCAPE = "E";

      searchstr.append
	(UNACCENT + "(LOWER(title)) LIKE " + UNACCENT +
	 "(LOWER(" + ESCAPE + "'%' || ? || '%')) AND ");
      values.append(biblioteq_myqstring::escape(id.title->text().trimmed()));

      if(!id.isbn13->text().remove('-').trimmed().isEmpty())
	{
	  searchstr.append
	    ("LOWER(COALESCE(isbn13, '')) LIKE LOWER('%' || ? || '%') AND ");
	  values.append(id.isbn13->text().remove('-').trimmed());
	}
      else
	searchstr.append("isbn13 IS NULL AND ");

      searchstr.append
	(UNACCENT +
	 "(LOWER(COALESCE(lccontrolnumber, ''))) LIKE " + UNACCENT +
	 "(LOWER(" + ESCAPE + "'%' || ? || '%')) AND ");
      values.append(biblioteq_myqstring::escape(id.lcnum->text().trimmed()));
      searchstr.append
	(UNACCENT + "(LOWER(COALESCE(callnumber, ''))) LIKE " + UNACCENT +
	 "(LOWER(" + ESCAPE + "'%' || ? || '%')) AND ");
      values.append(biblioteq_myqstring::escape(id.callnum->text().trimmed()));
      searchstr.append
	(UNACCENT + "(LOWER(COALESCE(deweynumber, ''))) LIKE " + UNACCENT +
	 "(LOWER(" + ESCAPE + "'%' || ? || '%')) AND ");
      values.append(biblioteq_myqstring::escape(id.deweynum->text().trimmed()));

      if(id.edition->currentIndex() != 0)
	{
	  searchstr.append("edition = ? AND ");
	  values.append(id.edition->currentText().trimmed());
	}

      searchstr.append
	(UNACCENT + "(LOWER(author)) LIKE " + UNACCENT +
	 "(LOWER(" + ESCAPE + "'%' || ? || '%')) AND ");
      values.append
	(biblioteq_myqstring::escape(id.author->toPlainText().trimmed()));

      if(id.publication_date_enabled->isChecked())
	{
	  searchstr.append("SUBSTR(pdate, 7) = ? AND ");
	  values.append(id.publication_date->date().toString("yyyy"));
	}

      searchstr.append
	(UNACCENT + "(LOWER(publisher)) LIKE " + UNACCENT +
	 "(LOWER(" + ESCAPE + "'%' || ? || '%')) AND ");
      values.append
	(biblioteq_myqstring::escape(id.publisher->toPlainText().trimmed()));
      searchstr.append
	(UNACCENT + "(LOWER(place)) LIKE " + UNACCENT +
	 "(LOWER(" + ESCAPE + "'%' || ? || '%')) AND ");
      values.append
	(biblioteq_myqstring::escape(id.place->toPlainText().trimmed()));
      searchstr.append
	(UNACCENT + "(LOWER(category)) LIKE " + UNACCENT +
	 "(LOWER(" + ESCAPE + "'%' || ? || '%')) AND ");
      values.append
	(biblioteq_myqstring::escape(id.category->toPlainText().trimmed()));

      if(id.price->value() > -0.01)
	{
	  searchstr.append("price = ");
	  searchstr.append(QString::number(id.price->value()));
	  searchstr.append(" AND ");
	}

      if(id.language->currentIndex() != 0)
	{
	  searchstr.append(UNACCENT + "(language) = " + UNACCENT + "(?) AND ");
	  values.append
	    (biblioteq_myqstring::escape(id.language->currentText().trimmed()));
	}

      if(id.monetary_units->currentIndex() != 0)
	{
	  searchstr.append
	    (UNACCENT + "(monetary_units) = " + UNACCENT + "(?) AND ");
	  values.append
	    (biblioteq_myqstring::
	     escape(id.monetary_units->currentText().trimmed()));
	}

      if(id.binding->currentIndex() != 0)
	{
	  searchstr.append
	    (UNACCENT + "(binding_type) = " + UNACCENT + "(?) AND ");
	  values.append
	    (biblioteq_myqstring::escape(id.binding->currentText().trimmed()));
	}

      searchstr.append
	(UNACCENT + "(LOWER(description)) LIKE " + UNACCENT +
	 "(LOWER(" + ESCAPE + "'%' || ? || '%')) ");
      values.append
	(biblioteq_myqstring::escape(id.description->toPlainText().trimmed()));

      if(id.quantity->value() != 0)
	searchstr.append
	  ("AND quantity = " + QString::number(id.quantity->value()) + " ");

      if(id.location->currentIndex() != 0)
	{
	  searchstr.append
	    ("AND " + UNACCENT + "(location) = " + UNACCENT + "(?) ");
	  values.append
	    (biblioteq_myqstring::escape(id.location->currentText().trimmed()));
	}

      searchstr.append
	("AND " + UNACCENT + "(LOWER(COALESCE(marc_tags, ''))) LIKE " +
	 UNACCENT + "(LOWER(" + ESCAPE + "'%' || ? || '%')) ");
      values.append
	(biblioteq_myqstring::escape(id.marc_tags->toPlainText().trimmed()));
      searchstr.append
	("AND " + UNACCENT + "(LOWER(COALESCE(keyword, ''))) LIKE " +
	 UNACCENT + "(LOWER(" + ESCAPE + "'%' || ? || '%')) ");
      values.append
	(biblioteq_myqstring::escape(id.keyword->toPlainText().trimmed()));
      searchstr.append
	("AND " + UNACCENT + "(LOWER(COALESCE(url, ''))) LIKE " +
	 UNACCENT + "(LOWER(" + ESCAPE + "'%' || ? || '%')) ");
      values.append
	(biblioteq_myqstring::escape(id.url->toPlainText().trimmed()));

      if(id.originality->currentIndex() != 0)
	{
	  searchstr.append
	    ("AND " + UNACCENT + "(originality) = " + UNACCENT + "(?) ");
	  values.append
	    (biblioteq_myqstring::
	     escape(id.originality->currentText().trimmed()));
	}

      if(id.condition->currentIndex() != 0)
	{
	  searchstr.append
	    ("AND " + UNACCENT + "(condition) = " + UNACCENT + "(?) ");
	  values.append
	    (biblioteq_myqstring::
	     escape(id.condition->currentText().trimmed()));
	}

      searchstr.append
	("AND " + UNACCENT + "(LOWER(COALESCE(accession_number, ''))) LIKE " +
	 UNACCENT + "(LOWER(" + ESCAPE + "'%' || ? || '%')) ");
      values.append
	(biblioteq_myqstring::escape(id.accession_number->text().trimmed()));
      searchstr.append
	("AND " + UNACCENT + "(LOWER(COALESCE(alternate_id_1, ''))) LIKE " +
	 UNACCENT + "(LOWER(" + ESCAPE + "'%' || ? || '%')) AND ");
      values.append
	(biblioteq_myqstring::escape(id.alternate_id_1->text().trimmed()));
      searchstr.append
	("LOWER(COALESCE(REPLACE(multivolume_set_isbn, '-', ''), '')) "
	 "LIKE LOWER('%' || ? || '%') AND ");
      values.append(id.multivolume_set_isbn->text().remove('-').trimmed());
      searchstr.append
	("LOWER(COALESCE(target_audience, '')) LIKE " +
	 UNACCENT + "(LOWER('%' || ? || '%')) AND ");
      values.append
	(biblioteq_myqstring::
	 escape(id.target_audience->currentText().trimmed()));
      searchstr.append
	(UNACCENT + "(LOWER(COALESCE(volume_number, ''))) LIKE " +
	 UNACCENT + "(LOWER(" + ESCAPE + "'%' || ? || '%')) ");
      values.append
	(biblioteq_myqstring::escape(id.volume_number->text().trimmed()));
      searchstr.append
	("GROUP BY book.title, "
	 "book.author, "
	 "book.publisher, "
	 "book.pdate, "
	 "book.place, "
	 "book.edition, "
	 "book.category, "
	 "book.language, "
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
	 "book.originality, "
	 "book.condition, "
	 "book.accession_number, "
	 "book.alternate_id_1, "
	 "book.multivolume_set_isbn, "
	 "book.target_audience, "
	 "book.volume_number, "
	 "book.type, "
	 "book.myoid, "
	 "book.front_cover "
	 "ORDER BY book.title");

      auto query = new QSqlQuery(qmain->getDB());

      query->prepare(searchstr);

      for(int i = 0; i < values.size(); i++)
	query->addBindValue(values.at(i));

      (void) qmain->populateTable
	(query, "Books", biblioteq::NEW_PAGE, biblioteq::POPULATE_SEARCH);
    }
}

void biblioteq_book::slotOpenLibraryCanceled(void)
{
  auto reply = m_openLibraryManager->findChild<QNetworkReply *> ();

  if(reply)
    reply->deleteLater();

  m_openLibraryResults.clear();
}

void biblioteq_book::slotOpenLibraryDownloadFinished(bool error)
{
  auto canceled = false;

  if(m_openLibraryWorking)
    {
      canceled = m_openLibraryWorking->wasCanceled();
      m_openLibraryWorking->deleteLater();
    }

  if(!canceled && !error)
    QTimer::singleShot(250, this, SLOT(openLibraryDownloadFinished(void)));
}

void biblioteq_book::slotOpenLibraryDownloadFinished(void)
{
  auto error = false;
  auto reply = qobject_cast<QNetworkReply *> (sender());

  if(reply)
    {
      error = reply->error() != QNetworkReply::NoError;
      reply->deleteLater();
    }

  auto canceled = false;

  if(m_openLibraryWorking)
    {
      canceled = m_openLibraryWorking->wasCanceled();
      m_openLibraryWorking->deleteLater();
    }

  if(!canceled && !error)
    QTimer::singleShot(250, this, SLOT(openLibraryDownloadFinished(void)));
}

void biblioteq_book::slotOpenLibraryError(QNetworkReply::NetworkError error)
{
  if(m_openLibraryWorking)
    m_openLibraryWorking->deleteLater();

  auto reply = qobject_cast<QNetworkReply *> (sender());

  if(reply)
    {
      auto error(reply->errorString());

      reply->deleteLater();
      emit openLibraryQueryError(error);
    }
  else
    emit openLibraryQueryError(tr("A network error (%1) occurred.").arg(error));
}

void biblioteq_book::slotOpenLibraryQuery(void)
{
  if(m_openLibraryManager->findChild<QNetworkReply *> ())
    return;

  auto ok = false;

  if(!id.alternate_id_1->text().trimmed().isEmpty())
    ok = true;

  if(id.isbnAvailableCheckBox->isChecked())
    if(id.id->text().remove('-').trimmed().length() == 10 ||
       id.isbn13->text().remove('-').trimmed().length() == 13)
      ok = true;

  if(!ok)
    {
      QMessageBox::critical
	(this,
	 tr("BiblioteQ: User Error"),
	 tr("In order to query an Open Library site, "
	    "Alternate Identifier, ISBN-10, or ISBN-13 must be provided."));
      QApplication::processEvents();
      id.id->setFocus();
      return;
    }

  createOpenLibraryDialog();
  resetQueryHighlights();

  if(m_openLibraryWorking)
    {
      m_openLibraryWorking->show();
      m_openLibraryWorking->update();
      m_openLibraryWorking->repaint();
      QApplication::processEvents();
    }

  QString searchstr("");
  auto hash(qmain->getOpenLibraryItemsHash());

  searchstr = hash.value("url_isbn");

  if(!id.alternate_id_1->text().trimmed().isEmpty())
    searchstr.replace("%3", id.alternate_id_1->text().trimmed());
  else
    searchstr.remove(",OLID:%3");

  if(id.isbnAvailableCheckBox->isChecked())
    {
      if(!id.id->text().remove('-').trimmed().isEmpty())
	{
	  searchstr.replace("%1", id.id->text().remove('-').trimmed());

	  if(id.isbn13->text().remove('-').trimmed().isEmpty())
	    searchstr.replace
	      ("%2", biblioteq_misc_functions::isbn10to13(id.id->text()));
	}
      else
	searchstr.replace("%1", id.isbn13->text().remove('-').trimmed());

      if(!id.isbn13->text().remove('-').trimmed().isEmpty())
	searchstr.replace("%2", id.isbn13->text().remove('-').trimmed());
      else
	searchstr.replace("%2", id.id->text().remove('-').trimmed());
    }
  else
    searchstr.remove("ISBN:%1,ISBN:%2,");

  QNetworkProxy proxy;
  QString type("none");
  auto url(QUrl::fromUserInput(searchstr));

  if(hash.contains("proxy_type"))
    type = hash.value("proxy_type").toLower().trimmed();

  if(type == "none")
    {
      proxy.setType(QNetworkProxy::NoProxy);
      m_openLibraryManager->setProxy(proxy);
    }
  else
    {
      if(type == "http" || type == "socks5" || type == "system")
	{
	  /*
	  ** This is required to resolve an odd error.
	  */

	  auto reply = m_openLibraryManager->get
	    (QNetworkRequest(QUrl::fromUserInput("http://0.0.0.0")));

	  if(reply)
	    reply->deleteLater();

	  connect
	    (m_openLibraryManager,
	     SIGNAL(proxyAuthenticationRequired(const QNetworkProxy &,
						QAuthenticator *)),
	     this,
	     SLOT(slotProxyAuthenticationRequired(const QNetworkProxy &,
						  QAuthenticator *)),
	     Qt::UniqueConnection);
	}

      if(type == "http" || type == "socks5")
	{
	  if(type == "http")
	    proxy.setType(QNetworkProxy::HttpProxy);
	  else
	    proxy.setType(QNetworkProxy::Socks5Proxy);

	  QString host("");
	  QString password("");
	  QString user("");
	  quint16 port = 0;

	  host = hash.value("proxy_host");
	  port = hash.value("proxy_port").toUShort();
	  user = hash.value("proxy_username");
	  password = hash.value("proxy_password");
	  proxy.setHostName(host);
	  proxy.setPort(port);

	  if(!user.isEmpty())
	    proxy.setUser(user);

	  if(!password.isEmpty())
	    proxy.setPassword(password);

	  m_openLibraryManager->setProxy(proxy);
	}
      else if(type == "system")
	{
	  QList<QNetworkProxy> list;
	  QNetworkProxyQuery query(url);

	  list = QNetworkProxyFactory::systemProxyForQuery(query);

	  if(!list.isEmpty())
	    proxy = list.at(0);

	  m_openLibraryManager->setProxy(proxy);
	}
      else
	{
	  proxy.setType(QNetworkProxy::NoProxy);
	  m_openLibraryManager->setProxy(proxy);
	}
    }

  auto reply = m_openLibraryManager->get(QNetworkRequest(url));

  if(reply)
    {
      m_openLibraryResults.clear();
      connect(reply, SIGNAL(readyRead(void)),
	      this, SLOT(slotOpenLibraryReadyRead(void)));
      connect(reply, SIGNAL(finished(void)),
	      this, SLOT(slotOpenLibraryDownloadFinished(void)));
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
      connect(reply,
	      SIGNAL(error(QNetworkReply::NetworkError)),
	      this,
	      SLOT(slotOpenLibraryError(QNetworkReply::NetworkError)));
#else
      connect(reply,
	      SIGNAL(errorOccurred(QNetworkReply::NetworkError)),
	      this,
	      SLOT(slotOpenLibraryError(QNetworkReply::NetworkError)));
#endif
      connect(reply, SIGNAL(sslErrors(const QList<QSslError> &)),
	      this, SLOT(slotOpenLibrarySslErrors(const QList<QSslError> &)));
    }
  else
    {
      if(m_openLibraryWorking)
	m_openLibraryWorking->deleteLater();
    }
}

void biblioteq_book::slotOpenLibraryQueryError(const QString &text)
{
  if(text.trimmed().isEmpty())
    return;

  QMessageBox::critical
    (this, tr("BiblioteQ: Open Library Query Error"),
     tr("A network error (%1) occurred.").arg(text.trimmed()));
  QApplication::processEvents();
}

void biblioteq_book::slotOpenLibraryReadyRead(void)
{
  auto reply = qobject_cast<QNetworkReply *> (sender());

  if(reply)
    m_openLibraryResults.append(reply->readAll());
}

void biblioteq_book::slotOpenLibrarySslErrors(const QList<QSslError> &list)
{
  Q_UNUSED(list);

  if(m_openLibraryWorking)
    m_openLibraryWorking->deleteLater();

  auto reply = qobject_cast<QNetworkReply *> (sender());

  if(reply)
    reply->deleteLater();

  emit openLibraryQueryError
    (tr("One or more SSL errors occurred. Please verify your settings."));
}

void biblioteq_book::slotParseMarcTags(void)
{
  auto text(id.marc_tags->toPlainText().trimmed());

  if(text.startsWith("<?xml version=\"1.0\""))
    {
      if(id.marc_tags_format->currentIndex() == 0)
	populateAfterSRU("", text);
      else if(id.marc_tags_format->currentIndex() == 1)
	populateAfterSRU("MARC21", text);
      else
	populateAfterSRU("UNIMARC", text);
    }
  else if(text.startsWith("{") && text.endsWith("}"))
    {
      m_openLibraryResults = text.toUtf8();
      populateAfterOpenLibrary();
    }
  else
    switch(id.marc_tags_format->currentIndex())
      {
      case 0:
	{
	  biblioteq_marc m;

	  m.initialize(biblioteq_marc::BOOK,
		       biblioteq_marc::Z3950,
		       biblioteq_marc::MARC21);
	  m.parse(text);

	  if(!m.author().isEmpty())
	    populateAfterZ3950(text, biblioteq_marc::MARC21);
	  else
	    populateAfterZ3950(text, biblioteq_marc::UNIMARC);

	  break;
	}
      case 1:
	{
	  populateAfterZ3950(text, biblioteq_marc::MARC21);
	  break;
	}
      default:
	{
	  populateAfterZ3950(text, biblioteq_marc::UNIMARC);
	  break;
	}
      }
}

void biblioteq_book::slotPopulateCopiesEditor(void)
{
  biblioteq_copy_editor_book *copyeditor = nullptr;

  copyeditor = new biblioteq_copy_editor_book
    (qobject_cast<QWidget *> (this),
     qmain,
     static_cast<biblioteq_item *> (this),
     false,
     id.quantity->value(),
     m_oid,
     id.quantity,
     font(),
     id.id->text().remove('-').trimmed());
  copyeditor->populateCopiesEditor();
}

void biblioteq_book::slotPrint(void)
{
  m_html = "<html>";
  m_html += "<b>" + tr("ISBN-10:") + "</b> " +
    id.id->text().trimmed() + "<br>";
  m_html += "<b>" + tr("ISBN-13:") + "</b> " +
    id.isbn13->text().trimmed() + "<br>";
  m_html += "<b>" + tr("Edition:") + "</b> " +
    id.edition->currentText() + "<br>";
  m_html += "<b>" + tr("Authors:") + "</b> " +
    id.author->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Book Binding Type:") + "</b> " +
    id.binding->currentText() + "<br>";
  m_html += "<b>" + tr("LC Control Number:") + "</b> " +
    id.lcnum->text().trimmed() + "<br>";
  m_html += "<b>" + tr("Call Number:") + "</b> " +
    id.callnum->text().trimmed() + "<br>";
  m_html += "<b>" + tr("Dewey Class Number:") + "</b> " +
    id.deweynum->text().trimmed() + "<br>";

  /*
  ** General information.
  */

  m_html += "<b>" + tr("Title:") + "</b> " +
    id.title->text().trimmed() + "<br>";
  m_html += "<b>" + tr("Publication Date:") + "</b> " +
    id.publication_date->date().toString(Qt::ISODate) + "<br>";
  m_html += "<b>" + tr("Publisher:") + "</b> " +
    id.publisher->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Place of Publication:") + "</b> " +
    id.place->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Categories:") + "</b> " +
    id.category->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Price:") + "</b> " + id.price->cleanText() + "<br>";
  m_html += "<b>" + tr("Language:") + "</b> " +
    id.language->currentText() + "<br>";
  m_html += "<b>" + tr("Monetary Units:") + "</b> " +
    id.monetary_units->currentText() + "<br>";
  m_html += "<b>" + tr("Copies:") + "</b> " + id.quantity->text() + "<br>";
  m_html += "<b>" + tr("Location:") + "</b> " +
    id.location->currentText() + "<br>";
  m_html += "<b>" + tr("Originality:") + "</b> " +
    id.originality->currentText() + "<br>";
  m_html += "<b>" + tr("Condition:") + "</b> " +
    id.condition->currentText() + "<br>";
  m_html += "<b>" + tr("Abstract:") + "</b> " +
    id.description->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("MARC Tags:") + "</b> " +
    id.marc_tags->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Keywords:") + "</b> " +
    id.keyword->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Accession Number:") + "</b> " +
    id.accession_number->text().trimmed() + "<br>";
  m_html += "<b>" + tr("URL:") + "</b> " +
    id.url->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Multi-Volume ISBN:") + "</b>" +
    id.multivolume_set_isbn->text().trimmed() + "<br>";
  m_html += "<b>" + tr("Target Audience:") + "</b>" +
    id.target_audience->currentText().trimmed() + "<br>";
  m_html += "<b>" + tr("Volume Number:") + "</b>" +
    id.volume_number->text().trimmed();
  m_html += "</html>";
  print(this);
}

void biblioteq_book::slotPrintAuthorTitleDewey(void)
{
  QString html("");

  html += "<html>";
  html += id.author->toPlainText().trimmed() + "<br>";
  html += id.title->text().trimmed() + "<br>";
  html += id.deweynum->text().trimmed();
  html += "</html>";

  QPrinter printer;
  QScopedPointer<QPrintDialog> dialog(new QPrintDialog(&printer, this));
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
      document.setHtml(html);
      document.print(&printer);
    }

  QApplication::processEvents();
}

void biblioteq_book::slotPrintCallDewey(void)
{
  QString html("");
  auto list
    ((id.callnum->text().trimmed() + " " +
      id.deweynum->text().trimmed()).split(QRegularExpression("\\W+"),
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
					   Qt::SkipEmptyParts
#else
					   QString::SkipEmptyParts
#endif
					   ));

  html += "<html>";

  for(int i = 0; i < list.size(); i++)
    html += list.at(i) + "<br>";

  html += "</html>";

  QPrinter printer;
  QScopedPointer<QPrintDialog> dialog(new QPrintDialog(&printer, this));
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
      document.setHtml(html);
      document.print(&printer);
    }

  QApplication::processEvents();
}

void biblioteq_book::slotProxyAuthenticationRequired
(const QNetworkProxy &proxy, QAuthenticator *authenticator)
{
  if(authenticator)
    {
      ui_p.messageLabel->setText
	(QString(tr("The proxy %1:%2 is requesting "
		    "credentials.").
		 arg(proxy.hostName()).
		 arg(proxy.port())));
      m_proxyDialog->resize
	(QSize(m_proxyDialog->width(), m_proxyDialog->sizeHint().height()));

      if(m_proxyDialog->exec() == QDialog::Accepted)
	{
	  QApplication::processEvents();
	  authenticator->setUser(ui_p.usernameLineEdit->text());
	  authenticator->setPassword(ui_p.passwordLineEdit->text());
	}

      QApplication::processEvents();
    }
}

void biblioteq_book::slotPublicationDateEnabled(bool state)
{
  id.publication_date->setEnabled(state);

  if(!state)
    id.publication_date->setDate(QDate::fromString("2001", "yyyy"));
}

void biblioteq_book::slotReadyRead(void)
{
  if(!m_imageBuffer.isOpen())
    return;

  auto reply = qobject_cast<QNetworkReply *> (sender());

  if(reply)
    m_imageBuffer.write(reply->readAll());
}

void biblioteq_book::slotReset(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(action != nullptr)
    {
      auto actions = id.resetButton->menu()->actions();

      if(actions.size() < 30)
	{
	  // Error.
	}
      else if(action == actions[0])
	id.front_image->clear();
      else if(action == actions[1])
	id.back_image->clear();
      else if(action == actions[2])
	{
	  if(!m_engWindowTitle.contains("Search"))
	    id.id->clear();
	  else
	    id.id->setText("%");

	  id.id->setPalette(m_te_orig_pal);
	  id.id->setFocus();
	}
      else if(action == actions[3])
	{
	  if(!m_engWindowTitle.contains("Search"))
	    id.isbn13->clear();
	  else
	    id.isbn13->setText("%");

	  id.isbn13->setPalette(m_te_orig_pal);
	  id.isbn13->setFocus();
	}
      else if(action == actions[4])
	{
	  id.edition->setCurrentIndex(0);
	  id.edition->setStyleSheet(m_cb_orig_ss);
	  id.edition->setFocus();
	}
      else if(action == actions[5])
	{
	  if(!m_engWindowTitle.contains("Search"))
	    id.author->setPlainText("N/A");
	  else
	    id.author->clear();

	  id.author->viewport()->setPalette(m_te_orig_pal);
	  id.author->setFocus();
	}
      else if(action == actions[6])
	{
	  id.binding->setCurrentIndex(0);
	  id.binding->setStyleSheet(m_cb_orig_ss);
	  id.binding->setFocus();
	}
      else if(action == actions[7])
	{
	  id.lcnum->clear();
	  id.lcnum->setPalette(m_white_pal);
	  id.lcnum->setFocus();
	}
      else if(action == actions[8])
	{
	  id.callnum->clear();
	  id.callnum->setPalette(m_white_pal);
	  id.callnum->setFocus();
	}
      else if(action == actions[9])
	{
	  id.deweynum->clear();
	  id.deweynum->setPalette(m_white_pal);
	  id.deweynum->setFocus();
	}
      else if(action == actions[10])
	{
	  id.title->clear();
	  id.title->setPalette(m_te_orig_pal);
	  id.title->setFocus();
	}
      else if(action == actions[11])
	{
	  if(m_engWindowTitle.contains("Search"))
	    {
	      id.publication_date->setDate(QDate::fromString("2001", "yyyy"));
	      id.publication_date_enabled->setChecked(false);
	    }
	  else
	    id.publication_date->setDate
	      (QDate::fromString("01/01/2000",
				 biblioteq::s_databaseDateFormat));

	  id.publication_date->setFocus();
	  id.publication_date->setStyleSheet(m_dt_orig_ss);
	}
      else if(action == actions[12])
	{
	  if(!m_engWindowTitle.contains("Search"))
	    id.publisher->setPlainText("N/A");
	  else
	    id.publisher->clear();

	  id.publisher->viewport()->setPalette(m_te_orig_pal);
	  id.publisher->setFocus();
	}
      else if(action == actions[13])
	{
	  if(!m_engWindowTitle.contains("Search"))
	    id.place->setPlainText("N/A");
	  else
	    id.place->clear();

	  id.place->viewport()->setPalette(m_te_orig_pal);
	  id.place->setFocus();
	}
      else if(action == actions[14])
	{
	  if(!m_engWindowTitle.contains("Search"))
	    id.category->setPlainText("N/A");
	  else
	    id.category->clear();

	  id.category->viewport()->setPalette(m_te_orig_pal);
	  id.category->setFocus();
	}
      else if(action == actions[15])
	{
	  id.price->setValue(id.price->minimum());
	  id.price->setFocus();
	}
      else if(action == actions[16])
	{
	  id.language->setCurrentIndex(0);
	  id.language->setFocus();
	}
      else if(action == actions[17])
	{
	  id.monetary_units->setCurrentIndex(0);
	  id.monetary_units->setFocus();
	}
      else if(action == actions[18])
	{
	  id.quantity->setValue(id.quantity->minimum());
	  id.quantity->setFocus();
	}
      else if(action == actions[19])
	{
	  id.location->setCurrentIndex(0);
	  id.location->setFocus();
	}
      else if(action == actions[20])
	{
	  if(m_engWindowTitle.contains("Search"))
	    id.originality->setCurrentIndex(0);
	  else
	    id.originality->setCurrentIndex(2);

	  id.originality->setFocus();
	}
      else if(action == actions[21])
	{
	  id.condition->setCurrentIndex(0);
	  id.condition->setFocus();
	}
      else if(action == actions[22])
	{
	  if(!m_engWindowTitle.contains("Search"))
	    id.description->setPlainText("N/A");
	  else
	    id.description->clear();

	  id.description->viewport()->setPalette(m_te_orig_pal);
	  id.description->setFocus();
	}
      else if(action == actions[23])
	{
	  id.marc_tags->clear();
	  id.marc_tags->viewport()->setPalette(m_white_pal);
	  id.marc_tags->setFocus();
	}
      else if(action == actions[24])
	{
	  id.keyword->clear();
	  id.keyword->setFocus();
	}
      else if(action == actions[25])
	{
	  id.accession_number->clear();
	  id.accession_number->setFocus();
	}
      else if(action == actions[26])
	{
	  id.url->clear();
	  id.url->setFocus();
	}
      else if(action == actions[27])
	{
	  id.multivolume_set_isbn->clear();
	  id.multivolume_set_isbn->setFocus();
	}
      else if(action == actions[28])
	{
	  id.target_audience->setCurrentIndex(0);
	  id.target_audience->setStyleSheet(m_cb_orig_ss);
	}
      else if(action == actions[29])
	{
	  id.volume_number->clear();
	  id.volume_number->setFocus();
	}

      actions.clear();
    }
  else
    {
      /*
      ** Reset all.
      */

      if(!m_engWindowTitle.contains("Search"))
	id.id->clear();
      else
	id.id->setText("%");

      id.title->clear();

      if(!m_engWindowTitle.contains("Search"))
	id.author->setPlainText("N/A");
      else
	id.author->clear();

      if(!m_engWindowTitle.contains("Search"))
	id.publisher->setPlainText("N/A");
      else
	id.publisher->clear();

      if(!m_engWindowTitle.contains("Search"))
	id.place->setPlainText("N/A");
      else
	id.place->clear();

      if(!m_engWindowTitle.contains("Search"))
	id.category->setPlainText("N/A");
      else
	id.category->clear();

      if(m_engWindowTitle.contains("Search"))
	{
	  id.publication_date->setDate(QDate::fromString("2001", "yyyy"));
	  id.publication_date_enabled->setChecked(false);
	}
      else
	id.publication_date->setDate(QDate::fromString("01/01/2000",
						       biblioteq::
						       s_databaseDateFormat));

      id.quantity->setValue(id.quantity->minimum());

      if(!m_engWindowTitle.contains("Search"))
	id.description->setPlainText("N/A");
      else
	id.description->clear();

      id.marc_tags->clear();
      id.marc_tags_format->setCurrentIndex(0);
      id.keyword->clear();
      id.url->clear();

      if(!m_engWindowTitle.contains("Search"))
	id.isbn13->clear();
      else
	id.isbn13->setText("%");

      id.lcnum->clear();
      id.callnum->clear();
      id.deweynum->clear();
      id.location->setCurrentIndex(0);

      if(m_engWindowTitle.contains("Search"))
	id.originality->setCurrentIndex(0);
      else
	id.originality->setCurrentIndex(2);

      id.accession_number->clear();
      id.back_image->clear();
      id.binding->setCurrentIndex(0);
      id.condition->setCurrentIndex(0);
      id.edition->setCurrentIndex(0);
      id.front_image->clear();
      id.id->setFocus();
      id.language->setCurrentIndex(0);
      id.monetary_units->setCurrentIndex(0);
      id.multivolume_set_isbn->clear();
      id.price->setValue(id.price->minimum());
      id.target_audience->setCurrentIndex(0);
      id.volume_number->clear();
      resetQueryHighlights();
    }
}

void biblioteq_book::slotSRUCanceled(void)
{
  auto reply = m_sruManager->findChild<QNetworkReply *> ();

  if(reply)
    reply->deleteLater();

  m_sruResults.clear();
}

void biblioteq_book::slotSRUDownloadFinished(bool error)
{
  auto canceled = false;

  if(m_sruWorking)
    {
      canceled = m_sruWorking->wasCanceled();
      m_sruWorking->deleteLater();
    }

  if(!canceled && !error)
    QTimer::singleShot(250, this, SLOT(sruDownloadFinished(void)));
}

void biblioteq_book::slotSRUDownloadFinished(void)
{
  auto error = false;
  auto reply = qobject_cast<QNetworkReply *> (sender());

  if(reply)
    {
      error = reply->error() != QNetworkReply::NoError;
      reply->deleteLater();
    }

  auto canceled = false;

  if(m_sruWorking)
    {
      canceled = m_sruWorking->wasCanceled();
      m_sruWorking->deleteLater();
    }

  if(!canceled && !error)
    QTimer::singleShot(250, this, SLOT(sruDownloadFinished(void)));
}

void biblioteq_book::slotSRUError(QNetworkReply::NetworkError error)
{
  if(m_sruWorking)
    m_sruWorking->deleteLater();

  auto reply = qobject_cast<QNetworkReply *> (sender());

  if(reply)
    {
      auto error(reply->errorString());

      reply->deleteLater();
      emit sruQueryError(error);
    }
  else
    emit sruQueryError(tr("A network error (%1) occurred.").arg(error));
}

void biblioteq_book::slotSRUQuery(void)
{
  if(m_sruManager->findChild<QNetworkReply *> ())
    return;

  if(!(id.id->text().remove('-').trimmed().length() == 10 ||
       id.isbn13->text().remove('-').trimmed().length() == 13))
    {
      QMessageBox::critical
	(this,
	 tr("BiblioteQ: User Error"),
	 tr("In order to query an SRU site, "
	    "ISBN-10 or ISBN-13 must be provided."));
      QApplication::processEvents();
      id.id->setFocus();
      return;
    }

  createSRUDialog();
  resetQueryHighlights();

  if(m_sruWorking)
    {
      m_sruWorking->show();
      m_sruWorking->update();
      m_sruWorking->repaint();
      QApplication::processEvents();
    }

  QString name("");
  auto found = false;

  for(int i = 0; i < id.sruQueryButton->actions().size(); i++)
    if(id.sruQueryButton->actions().at(i)->isChecked())
      {
	found = true;
	name = id.sruQueryButton->actions().at(i)->text();
	break;
      }

  if(!found)
    name = qmain->getPreferredSRUSite();

  QString searchstr("");
  auto hash(qmain->getSRUHash(name));

  searchstr = hash.value("url_isbn");

  if(!id.id->text().remove('-').trimmed().isEmpty())
    {
      searchstr.replace("%1", id.id->text().remove('-').trimmed());

      if(id.isbn13->text().remove('-').trimmed().isEmpty())
	searchstr.replace
	  ("%2", biblioteq_misc_functions::isbn10to13(id.id->text()));
    }
  else
    searchstr.replace("%1", id.isbn13->text().remove('-').trimmed());

  if(!id.isbn13->text().remove('-').trimmed().isEmpty())
    searchstr.replace("%2", id.isbn13->text().remove('-').trimmed());
  else
    searchstr.replace("%2", id.id->text().remove('-').trimmed());

  QNetworkProxy proxy;
  QString type("none");
  auto url(QUrl::fromUserInput(searchstr));

  if(hash.contains("proxy_type"))
    type = hash.value("proxy_type").toLower().trimmed();

  if(type == "none")
    {
      proxy.setType(QNetworkProxy::NoProxy);
      m_sruManager->setProxy(proxy);
    }
  else
    {
      if(type == "http" || type == "socks5" || type == "system")
	{
	  /*
	  ** This is required to resolve an odd error.
	  */

	  auto reply = m_sruManager->get
	    (QNetworkRequest(QUrl::fromUserInput("http://0.0.0.0")));

	  if(reply)
	    reply->deleteLater();

	  connect
	    (m_sruManager,
	     SIGNAL(proxyAuthenticationRequired(const QNetworkProxy &,
						QAuthenticator *)),
	     this,
	     SLOT(slotProxyAuthenticationRequired(const QNetworkProxy &,
						  QAuthenticator *)),
	     Qt::UniqueConnection);
	}

      if(type == "http" || type == "socks5")
	{
	  if(type == "http")
	    proxy.setType(QNetworkProxy::HttpProxy);
	  else
	    proxy.setType(QNetworkProxy::Socks5Proxy);

	  QString host("");
	  QString password("");
	  QString user("");
	  quint16 port = 0;

	  host = hash.value("proxy_host");
	  port = hash.value("proxy_port").toUShort();
	  user = hash.value("proxy_username");
	  password = hash.value("proxy_password");
	  proxy.setHostName(host);
	  proxy.setPort(port);

	  if(!user.isEmpty())
	    proxy.setUser(user);

	  if(!password.isEmpty())
	    proxy.setPassword(password);

	  m_sruManager->setProxy(proxy);
	}
      else if(type == "system")
	{
	  QList<QNetworkProxy> list;
	  QNetworkProxyQuery query(url);

	  list = QNetworkProxyFactory::systemProxyForQuery(query);

	  if(!list.isEmpty())
	    proxy = list.at(0);

	  m_sruManager->setProxy(proxy);
	}
      else
	{
	  proxy.setType(QNetworkProxy::NoProxy);
	  m_sruManager->setProxy(proxy);
	}
    }

  auto reply = m_sruManager->get(QNetworkRequest(url));

  if(reply)
    {
      m_sruResults.clear();
      connect(reply, SIGNAL(readyRead(void)),
	      this, SLOT(slotSRUReadyRead(void)));
      connect(reply, SIGNAL(finished(void)),
	      this, SLOT(slotSRUDownloadFinished(void)));
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
      connect(reply,
	      SIGNAL(error(QNetworkReply::NetworkError)),
	      this,
	      SLOT(slotSRUError(QNetworkReply::NetworkError)));
#else
      connect(reply,
	      SIGNAL(errorOccurred(QNetworkReply::NetworkError)),
	      this,
	      SLOT(slotSRUError(QNetworkReply::NetworkError)));
#endif
      connect(reply, SIGNAL(sslErrors(const QList<QSslError> &)),
	      this, SLOT(slotSRUSslErrors(const QList<QSslError> &)));
    }
  else
    {
      if(m_sruWorking)
	m_sruWorking->deleteLater();
    }
}

void biblioteq_book::slotSRUQueryError(const QString &text)
{
  if(text.trimmed().isEmpty())
    return;

  QMessageBox::critical
    (this, tr("BiblioteQ: SRU Query Error"),
     tr("A network error (%1) occurred.").arg(text.trimmed()));
  QApplication::processEvents();
}

void biblioteq_book::slotSRUReadyRead(void)
{
  auto reply = qobject_cast<QNetworkReply *> (sender());

  if(reply)
    m_sruResults.append(reply->readAll());
}

void biblioteq_book::slotSRUSslErrors(const QList<QSslError> &list)
{
  Q_UNUSED(list);

  if(m_sruWorking)
    m_sruWorking->deleteLater();

  auto reply = qobject_cast<QNetworkReply *> (sender());

  if(reply)
    reply->deleteLater();

  emit sruQueryError
    (tr("One or more SSL errors occurred. Please verify your settings."));
}

void biblioteq_book::slotSelectImage(void)
{
  QFileDialog dialog(this);
  auto button = qobject_cast<QPushButton *> (sender());

  dialog.setDirectory(QDir::homePath());
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setOption(QFileDialog::DontUseNativeDialog);

  if(button == id.frontButton)
    dialog.setWindowTitle(tr("BiblioteQ: Front Cover Image Selection"));
  else
    dialog.setWindowTitle(tr("BiblioteQ: Back Cover Image Selection"));

  dialog.exec();
  QApplication::processEvents();

  if(dialog.result() == QDialog::Accepted)
    {
      if(button == id.frontButton)
	{
	  id.front_image->clear();
	  id.front_image->m_image = QImage(dialog.selectedFiles().value(0));

	  if(dialog.selectedFiles().value(0).lastIndexOf(".") > -1)
	    id.front_image->m_imageFormat = dialog.selectedFiles().value(0).mid
	      (dialog.selectedFiles().value(0).lastIndexOf(".") + 1).
	      toUpper();

	  id.front_image->scene()->addPixmap
	    (QPixmap::fromImage(id.front_image->m_image));

	  if(!id.front_image->scene()->items().isEmpty())
	    id.front_image->scene()->items().at(0)->setFlags
	      (QGraphicsItem::ItemIsSelectable);

	  id.front_image->scene()->setSceneRect
	    (id.front_image->scene()->itemsBoundingRect());
	}
      else
	{
	  id.back_image->clear();
	  id.back_image->m_image = QImage(dialog.selectedFiles().value(0));

	  if(dialog.selectedFiles().value(0).lastIndexOf(".") > -1)
	    id.back_image->m_imageFormat = dialog.selectedFiles().value(0).mid
	      (dialog.selectedFiles().value(0).lastIndexOf(".") + 1).
	      toUpper();

	  id.back_image->scene()->addPixmap
	    (QPixmap::fromImage(id.back_image->m_image));

	  if(!id.back_image->scene()->items().isEmpty())
	    id.back_image->scene()->items().at(0)->setFlags
	      (QGraphicsItem::ItemIsSelectable);

	  id.back_image->scene()->setSceneRect
	    (id.back_image->scene()->itemsBoundingRect());
	}
    }
}

void biblioteq_book::slotShowPDF(void)
{
  auto list(id.files->selectionModel()->selectedRows(Columns::MYOID));

  if(list.isEmpty())
    return;

  auto reader = new biblioteq_pdfreader(qmain);

  QApplication::setOverrideCursor(Qt::WaitCursor);

  QByteArray data;
  QSqlQuery query(qmain->getDB());

  query.prepare("SELECT file, file_name FROM book_files "
		"WHERE item_oid = ? AND myoid = ?");
  query.bindValue(0, m_oid);
  query.bindValue(1, list.at(0).data());

  if(query.exec() && query.next())
    data = qUncompress(query.value(0).toByteArray());

  reader->load(data, query.value(1).toString().trimmed());
  reader->show();
  QApplication::restoreOverrideCursor();
}

void biblioteq_book::slotShowUsers(void)
{
  biblioteq_borrowers_editor *borrowerseditor = nullptr;
  int state = 0;

  if(!id.okButton->isHidden())
    state = biblioteq::EDITABLE;
  else
    state = biblioteq::VIEW_ONLY;

  borrowerseditor = new biblioteq_borrowers_editor
    (qobject_cast<QWidget *> (this),
     qmain,
     static_cast<biblioteq_item *> (this),
     id.quantity->value(),
     m_oid,
     id.id->text().remove('-').trimmed(),
     font(),
     "Book",
     state);
  borrowerseditor->showUsers();
}

void biblioteq_book::slotZ3950Query(void)
{
  if(findChild<biblioteq_generic_thread *> ())
    return;

  QString errorstr = "";
  QString etype = "";
  QString searchstr = "";
  int i = 0;

  if(!(id.id->text().remove('-').trimmed().length() == 10 ||
       id.isbn13->text().remove('-').trimmed().length() == 13))
    {
      QMessageBox::critical
	(this,
	 tr("BiblioteQ: User Error"),
	 tr("In order to query a Z39.50 site, "
	    "ISBN-10 or ISBN-13 must be provided."));
      QApplication::processEvents();
      id.id->setFocus();
      return;
    }

  resetQueryHighlights();
  m_thread = new biblioteq_generic_thread(this, qmain);

  biblioteq_item_working_dialog working(qobject_cast<QMainWindow *> (this));

  working.setCancelButton(nullptr);
  working.setModal(true);
  working.setWindowTitle(tr("BiblioteQ: Z39.50 Data Retrieval"));
  working.setLabelText(tr("Downloading..."));
  working.setMaximum(0);
  working.setMinimum(0);
  working.resize(working.sizeHint());
  working.show();
  working.update();
  working.repaint();
  QApplication::processEvents();

  QString recordSyntax("MARC21");
  QStringList isbns;

  isbns << id.id->text().remove('-').trimmed();

  if(id.isbn13->text().remove('-').trimmed().isEmpty())
    isbns << biblioteq_misc_functions::isbn10to13(id.id->text());
  else
    isbns << id.isbn13->text().remove('-').trimmed();

  if(isbns.at(0).isEmpty())
    searchstr = QString("@attr 1=7 %1").arg(isbns.at(1));
  else if(isbns.at(1).isEmpty())
    searchstr = QString("@attr 1=7 %1").arg(isbns.at(0));
  else
    searchstr = QString("@attr 1=7 @or %1 %2").arg(isbns.at(0)).
      arg(isbns.at(1));

  auto found = false;

  for(i = 0; i < id.z3950QueryButton->actions().size(); i++)
    if(id.z3950QueryButton->actions().at(i)->isChecked())
      {
	found = true;
	recordSyntax = qmain->getZ3950Hash
	  (id.z3950QueryButton->actions().at(i)->text()).value("RecordSyntax");
	m_thread->setZ3950Name
	  (id.z3950QueryButton->actions().at(i)->text());
	break;
      }

  if(!found)
    {
      recordSyntax = qmain->getZ3950Hash
	(qmain->getPreferredZ3950Site()).value("RecordSyntax");
      m_thread->setZ3950Name(qmain->getPreferredZ3950Site());
    }

  m_thread->setType(biblioteq_generic_thread::Z3950_QUERY);
  m_thread->setZ3950SearchString(searchstr);
  m_thread->start();

  while(!m_thread->isFinished())
    {
      QApplication::processEvents();
      m_thread->msleep(100UL);
    }

  auto canceled = working.wasCanceled(); // QProgressDialog::close()!

  working.close();
  working.reset(); // Qt 5.5.x adjustment.

  if(canceled)
    {
      m_thread->deleteLater();
      return;
    }

  if((errorstr = m_thread->getErrorStr()).isEmpty() &&
     !m_thread->getZ3950Results().isEmpty())
    {
      if(QMessageBox::question
	 (this, tr("BiblioteQ: Question"),
	  tr("Replace existing values with those retrieved "
	     "from the Z39.50 site?"),
	  QMessageBox::Yes | QMessageBox::No,
	  QMessageBox::No) == QMessageBox::Yes)
	{
	  QApplication::processEvents();

	  if(recordSyntax == "MARC21")
	    populateAfterZ3950
	      (m_thread->getZ3950Results()[0], biblioteq_marc::MARC21);
	  else
	    populateAfterZ3950
	      (m_thread->getZ3950Results()[0], biblioteq_marc::UNIMARC);
	}

      QApplication::processEvents();
    }
  else if(errorstr.isEmpty() && m_thread->getZ3950Results().isEmpty())
    {
      QMessageBox::critical
	(this, tr("BiblioteQ: Z39.50 Query Error"),
	 tr("A Z39.50 entry may not yet exist for the provided ISBN(s)."));
      QApplication::processEvents();
    }
  else
    etype = m_thread->getEType();

  m_thread->deleteLater();

  if(!errorstr.isEmpty())
    {
      qmain->addError(QString(tr("Z39.50 Query Error")), etype, errorstr,
		      __FILE__, __LINE__);
      QMessageBox::critical
	(this, tr("BiblioteQ: Z39.50 Query Error"),
	 tr("The Z39.50 entry could not be retrieved. Please view the "
	    "error log."));
      QApplication::processEvents();
    }
}

void biblioteq_book::sruDownloadFinished(void)
{
  /*
  ** Verify that the SRU data contains at least one record.
  */

  QXmlStreamReader reader(m_sruResults);
  int records = -1;

  while(!reader.atEnd())
    if(reader.readNextStartElement())
      if(reader.name().toString().trimmed().toLower() == "numberofrecords")
	{
	  records = reader.readElementText().trimmed().toInt();
	  break;
	}

  if(records <= 0)
    m_sruResults.clear();

  if(!m_sruResults.isEmpty())
    {
      if(QMessageBox::question(this, tr("BiblioteQ: Question"),
			       tr("Replace existing values with "
				  "those retrieved "
				  "from the SRU site?"),
			       QMessageBox::Yes | QMessageBox::No,
			       QMessageBox::No) == QMessageBox::Yes)
	{
	  QApplication::processEvents();
	  populateAfterSRU("MARC21", m_sruResults);
	}

      QApplication::processEvents();
    }
  else if(records == 0)
    QMessageBox::critical
      (this, tr("BiblioteQ: SRU Query Error"),
       tr("An SRU entry may not yet exist for the provided ISBN(s)."));
  else
    QMessageBox::critical
      (this, tr("BiblioteQ: SRU Query Error"),
       tr("The SRU query produced invalid results."));

  QApplication::processEvents();
}

void biblioteq_book::updateWindow(const int state)
{
  QString str = "";

  if(state == biblioteq::EDITABLE)
    {
      id.attach_files->setEnabled(true);
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
      id.view_pdf->setEnabled(true);
#endif
      id.copiesButton->setEnabled(true);
      id.delete_files->setEnabled(true);
      id.export_files->setEnabled(true);
      id.marc_tags_format->setVisible(true);
      id.parse_marc_tags->setVisible(true);
      id.showUserButton->setEnabled(true);
      id.okButton->setVisible(true);
      id.openLibraryQuery->setVisible(true);
      id.sruQueryButton->setVisible(true);
      id.z3950QueryButton->setVisible(true);
      id.resetButton->setVisible(true);
      id.frontButton->setVisible(true);
      id.backButton->setVisible(true);
      id.dwnldFront->setVisible(true);
      id.dwnldBack->setVisible(true);
      id.isbn10to13->setVisible(true);
      id.isbn13to10->setVisible(true);
      id.target_audience->setEditable(true);

      if(!id.id->text().remove('-').trimmed().isEmpty())
	str = QString(tr("BiblioteQ: Modify Book Entry (")) +
	  id.id->text().trimmed() + tr(")");
      else
	str = tr("BiblioteQ: Modify Book Entry");

      m_engWindowTitle = "Modify";
    }
  else
    {
      id.attach_files->setVisible(false);
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
      id.view_pdf->setEnabled(true);
#endif
      id.isbnAvailableCheckBox->setCheckable(false);
      id.copiesButton->setVisible(false);
      id.delete_files->setVisible(false);
      id.export_files->setEnabled(true);

      if(qmain->isGuest())
	id.showUserButton->setVisible(false);
      else
	id.showUserButton->setEnabled(true);

      id.okButton->setVisible(false);
      id.openLibraryQuery->setVisible(false);
      id.sruQueryButton->setVisible(false);
      id.z3950QueryButton->setVisible(false);
      id.resetButton->setVisible(false);
      id.frontButton->setVisible(false);
      id.backButton->setVisible(false);
      id.dwnldFront->setVisible(false);
      id.dwnldBack->setVisible(false);
      id.isbn10to13->setVisible(false);
      id.isbn13to10->setVisible(false);

      if(!id.id->text().remove('-').trimmed().isEmpty())
	str = QString(tr("BiblioteQ: View Book Details (")) +
	  id.id->text().trimmed() + tr(")");
      else
	str = tr("BiblioteQ: View Book Details");

      m_engWindowTitle = "View";
    }

  id.coverImages->setVisible(true);
  setReadOnlyFields(this, state != biblioteq::EDITABLE);
  setWindowTitle(str);
}
