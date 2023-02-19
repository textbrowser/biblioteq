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
#include "biblioteq_borrowers_editor.h"
#include "biblioteq_copy_editor.h"
#include "biblioteq_filesize_table_item.h"
#include "biblioteq_magazine.h"
#include "biblioteq_marc.h"
#include "biblioteq_pdfreader.h"
#include "biblioteq_sruResults.h"
#include "biblioteq_z3950results.h"

#include <QActionGroup>
#include <QAuthenticator>
#include <QCryptographicHash>
#include <QFileDialog>
#include <QInputDialog>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QShortcut>
#include <QSqlField>
#include <QSqlRecord>
#include <QTimer>
#include <QXmlStreamReader>

biblioteq_magazine::biblioteq_magazine(biblioteq *parentArg,
				       const QString &oidArg,
				       const QModelIndex &index,
				       const QString &subTypeArg):
  QMainWindow(), biblioteq_item(index)
{
  m_duplicate = false;
  m_sruWorking = nullptr;
  qmain = parentArg;

  QGraphicsScene *scene1 = nullptr;
  QGraphicsScene *scene2 = nullptr;
  QMenu *menu = nullptr;

  menu = new QMenu(this);
  scene1 = new QGraphicsScene(this);
  scene2 = new QGraphicsScene(this);
  m_proxyDialog = new QDialog(this);
  m_sruManager = new QNetworkAccessManager(this);
  m_oid = oidArg;
  m_subType = "Magazine";
  m_parentWid = parentArg;
  m_oldq = biblioteq_misc_functions::getColumnString
    (qmain->getUI().table,
     m_index->row(),
     qmain->getUI().table->columnNumber("Quantity")).toInt();
  ma.setupUi(this);
  setQMain(this);
  biblioteq_misc_functions::sortCombinationBox(ma.marc_tags_format);
  ma.files->setColumnHidden(MYOID, true);
  ma.files->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  ma.marc_tags_format->setVisible(false);
  ma.parse_marc_tags->setVisible(false);
  ma.publication_date->setDisplayFormat
    (qmain->publicationDateFormat("magazines"));
  ma.publication_date_enabled->setVisible(false);
  ma.quantity->setMaximum(static_cast<int> (biblioteq::Limits::QUANTITY));
#ifndef BIBLIOTEQ_LINKED_WITH_POPPLER
  ma.view_pdf->setEnabled(false);
  ma.view_pdf->setToolTip
    (tr("BiblioteQ was not assembled with Poppler support."));
#else
  connect(ma.view_pdf,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotShowPDF(void)));
#endif
  connect(ma.attach_files,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAttachFiles(void)));
  connect(ma.delete_files,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotDeleteFiles(void)));
  connect(ma.export_files,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotExportFiles(void)));
  connect(ma.parse_marc_tags,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotParseMarcTags(void)));
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S),
		this,
		SLOT(slotGo(void)));
#else
  new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_S),
		this,
		SLOT(slotGo(void)));
#endif
  updateFont(QApplication::font(), qobject_cast<QWidget *> (this));
  connect(ma.files, SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
	  this, SLOT(slotFilesDoubleClicked(QTableWidgetItem *)));
  connect(ma.okButton, SIGNAL(clicked(void)), this, SLOT(slotGo(void)));
  connect(ma.showUserButton, SIGNAL(clicked(void)), this,
	  SLOT(slotShowUsers(void)));
  connect(ma.sruQueryButton, SIGNAL(clicked(void)), this,
	  SLOT(slotSRUQuery(void)));
  connect(ma.z3950QueryButton, SIGNAL(clicked(void)), this,
	  SLOT(slotZ3950Query(void)));
  connect(ma.cancelButton, SIGNAL(clicked(void)), this,
	  SLOT(slotCancel(void)));
  connect(ma.copiesButton, SIGNAL(clicked(void)), this,
	  SLOT(slotPopulateCopiesEditor(void)));
  connect(ma.resetButton, SIGNAL(clicked(void)), this,
	  SLOT(slotReset(void)));
  connect(ma.printButton, SIGNAL(clicked(void)), this, SLOT(slotPrint(void)));
  connect(menu->addAction(tr("Reset Front Cover Image")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Back Cover Image")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset ISSN")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Volume")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Issue")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset LC Control Number")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Call Number")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Dewey Number")),
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
  connect(menu->addAction(tr("Reset Abstract")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset MARC Tags")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Keywords")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset Accession Number")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(ma.frontButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  connect(ma.backButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  connect(ma.issnAvailableCheckBox,
	  SIGNAL(toggled(bool)),
	  ma.sruQueryButton,
	  SLOT(setEnabled(bool)));
#ifndef BIBLIOTEQ_LINKED_WITH_YAZ
  connect(ma.issnAvailableCheckBox,
	  SIGNAL(toggled(bool)),
	  ma.z3950QueryButton,
	  SLOT(setEnabled(bool)));
#endif
  connect(qmain,
	  SIGNAL(fontChanged(const QFont &)),
	  this,
	  SLOT(setGlobalFonts(const QFont &)));
  connect(this,
	  SIGNAL(sruQueryError(const QString &)),
	  this,
	  SLOT(slotSRUQueryError(const QString &)),
	  Qt::QueuedConnection);
  ma.id->setCursorPosition(0);
  ma.resetButton->setMenu(menu);

  if(menu->actions().size() >= 3)
    connect(ma.issnAvailableCheckBox,
	    SIGNAL(toggled(bool)),
	    menu->actions().at(2),
	    SLOT(setEnabled(bool)));

  QString errorstr("");

  QApplication::setOverrideCursor(Qt::WaitCursor);
  ma.language->addItems
    (biblioteq_misc_functions::getLanguages(qmain->getDB(),
					    errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the languages.")),
       errorstr, __FILE__, __LINE__);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  ma.monetary_units->addItems
    (biblioteq_misc_functions::getMonetaryUnits(qmain->getDB(),
						errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the monetary units.")),
       errorstr, __FILE__, __LINE__);

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(subTypeArg.toLower() == "journal")
    ma.location->addItems
      (biblioteq_misc_functions::getLocations(qmain->getDB(),
					      "Journal",
					      errorstr));
  else
    ma.location->addItems
      (biblioteq_misc_functions::getLocations(qmain->getDB(),
					      "Magazine",
					      errorstr));

  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    {
      if(subTypeArg.toLower() == "journal")
	qmain->addError
	  (QString(tr("Database Error")),
	   QString(tr("Unable to retrieve the journal locations.")),
	   errorstr, __FILE__, __LINE__);
      else
	qmain->addError
	  (QString(tr("Database Error")),
	   QString(tr("Unable to retrieve the magazine locations.")),
	   errorstr, __FILE__, __LINE__);
    }

  ma.front_image->setScene(scene1);
  ma.back_image->setScene(scene2);

  if(ma.language->findText(biblioteq::s_unknown) == -1)
    ma.language->addItem(biblioteq::s_unknown);

  if(ma.monetary_units->findText(biblioteq::s_unknown) == -1)
    ma.monetary_units->addItem(biblioteq::s_unknown);

  if(ma.location->findText(biblioteq::s_unknown) == -1)
    ma.location->addItem(biblioteq::s_unknown);

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
      ma.sruQueryButton->addAction(action);

      if(qmain->getPreferredSRUSite() == action->text())
	{
	  found = true;
	  action->setChecked(true);
	}
    }

  if(ma.sruQueryButton->actions().isEmpty())
    {
      actionGroup1->deleteLater();
      ma.sruQueryButton->setPopupMode(QToolButton::DelayedPopup);
    }
  else if(!found)
    ma.sruQueryButton->actions().at(0)->setChecked(true);

  found = false;
  list = qmain->getZ3950Names();
#ifndef BIBLIOTEQ_LINKED_WITH_YAZ
  ma.z3950QueryButton->setEnabled(false);
  ma.z3950QueryButton->setToolTip(tr("The YAZ library is not available."));
#endif

  for(int i = 0; i < list.size(); i++)
    {
      auto action = actionGroup2->addAction(list.at(i));

      if(!action)
	continue;

      action->setCheckable(true);
      ma.z3950QueryButton->addAction(action);

      if(qmain->getPreferredZ3950Site() == action->text())
	{
	  found = true;
	  action->setChecked(true);
	}
    }

  if(ma.z3950QueryButton->actions().isEmpty())
    {
      actionGroup2->deleteLater();
      ma.z3950QueryButton->setPopupMode(QToolButton::DelayedPopup);
    }
  else if(!found)
    ma.z3950QueryButton->actions().at(0)->setChecked(true);

  /*
  ** Save some palettes and style sheets.
  */

  m_dt_orig_ss = ma.publication_date->styleSheet();
  m_cb_orig_pal = ma.language->palette();
  m_white_pal = m_te_orig_pal = ma.description->viewport()->palette();

  /*
  ** Prepare the form.
  */

  if(m_parentWid)
    resize(qRound(0.95 * m_parentWid->size().width()),
	   qRound(0.95 * m_parentWid->size().height()));

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

  ma.splitter->setStretchFactor(0, 0);
  ma.splitter->setStretchFactor(1, 1);
  ma.splitter->setStretchFactor(2, 0);
  biblioteq_misc_functions::center(this, m_parentWid);
  biblioteq_misc_functions::hideAdminFields(this, qmain->getRoles());
}

biblioteq_magazine::~biblioteq_magazine()
{
}

Ui_magDialog biblioteq_magazine::dialog(void) const
{
  return ma;
}

void biblioteq_magazine::changeEvent(QEvent *event)
{
  if(event)
    switch(event->type())
      {
      case QEvent::LanguageChange:
	{
	  ma.retranslateUi(this);
	  break;
	}
      default:
	break;
      }

  QMainWindow::changeEvent(event);
}

void biblioteq_magazine::closeEvent(QCloseEvent *e)
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

  qmain->removeMagazine(this);
}

void biblioteq_magazine::createFile(const QByteArray &digest,
				    const QByteArray &bytes,
				    const QString &fileName) const
{
  QSqlQuery query(qmain->getDB());

  if(qmain->getDB().driverName() != "QSQLITE")
    query.prepare(QString("INSERT INTO %1_files "
			  "(file, file_digest, file_name, item_oid) "
			  "VALUES (?, ?, ?, ?)").arg(m_subType));
  else
    query.prepare(QString("INSERT INTO %1_files "
			  "(file, file_digest, file_name, item_oid, myoid) "
			  "VALUES (?, ?, ?, ?, ?)").arg(m_subType));

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

void biblioteq_magazine::createSRUDialog(void)
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

void biblioteq_magazine::duplicate(const QString &p_oid, const int state)
{
  m_duplicate = true;
  modify(state); // Initial population.
  ma.attach_files->setEnabled(false);
  ma.view_pdf->setEnabled(false);
  ma.delete_files->setEnabled(false);
  ma.export_files->setEnabled(false);
  ma.copiesButton->setEnabled(false);
  ma.showUserButton->setEnabled(false);
  m_oid = p_oid;

  if(m_subType.toLower() == "journal")
    setWindowTitle(tr("BiblioteQ: Duplicate Journal Entry"));
  else
    setWindowTitle(tr("BiblioteQ: Duplicate Magazine Entry"));

  m_duplicate = false;
  m_engWindowTitle = "Create";
}

void biblioteq_magazine::insert(void)
{
  slotReset();
  ma.attach_files->setEnabled(false);
  ma.delete_files->setEnabled(false);
  ma.export_files->setEnabled(false);
  ma.marc_tags_format->setVisible(true);
  ma.parse_marc_tags->setVisible(true);
  ma.view_pdf->setEnabled(false);
  ma.id->clear();
  ma.lcnum->clear();
  ma.callnum->clear();
  ma.deweynum->clear();
  ma.title->clear();
  ma.publisher->setPlainText("N/A");
  ma.description->setPlainText("N/A");
  ma.marc_tags->clear();
  ma.keyword->clear();
  ma.category->setPlainText("N/A");
  ma.place->setPlainText("N/A");
  ma.copiesButton->setEnabled(false);
  ma.sruQueryButton->setVisible(true);
  ma.z3950QueryButton->setVisible(true);
  ma.okButton->setText(tr("&Save"));
  ma.publication_date->setDate
    (QDate::fromString("01/01/2000", biblioteq::s_databaseDateFormat));
  ma.id->setCursorPosition(0);
  ma.price->setMinimum(0.00);
  ma.price->setValue(0.00);
  ma.quantity->setMinimum(1);
  ma.quantity->setValue(1);
  ma.volume->setMinimum(0);
  ma.volume->setValue(0);
  ma.issue->setMinimum(0);
  ma.issue->setValue(0);
  ma.showUserButton->setEnabled(false);
  ma.location->setCurrentIndex(0);
  ma.language->setCurrentIndex(0);
  ma.monetary_units->setCurrentIndex(0);
  ma.accession_number->clear();
  biblioteq_misc_functions::highlightWidget
    (ma.id, m_requiredHighlightColor);
  biblioteq_misc_functions::highlightWidget
    (ma.title, m_requiredHighlightColor);
  biblioteq_misc_functions::highlightWidget
    (ma.publisher->viewport(), m_requiredHighlightColor);
  biblioteq_misc_functions::highlightWidget
    (ma.description->viewport(), m_requiredHighlightColor);
  biblioteq_misc_functions::highlightWidget
    (ma.category->viewport(), m_requiredHighlightColor);
  biblioteq_misc_functions::highlightWidget
    (ma.place->viewport(), m_requiredHighlightColor);
  m_te_orig_pal = ma.description->viewport()->palette();

  if(m_subType == "Journal")
    setWindowTitle(QString(tr("BiblioteQ: Create Journal Entry")));
  else
    setWindowTitle(QString(tr("BiblioteQ: Create Magazine Entry")));

  m_engWindowTitle = "Create";
  ma.id->setFocus();
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

void biblioteq_magazine::modify(const int state)
{
  QSqlQuery query(qmain->getDB());
  QString fieldname = "";
  QString str = "";
  QVariant var;
  int i = 0;

  if(state == biblioteq::EDITABLE)
    {
      if(m_subType == "Journal")
	setWindowTitle(QString(tr("BiblioteQ: Modify Journal Entry")));
      else
	setWindowTitle(QString(tr("BiblioteQ: Modify Magazine Entry")));

      m_engWindowTitle = "Modify";
      ma.attach_files->setEnabled(true);
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
      ma.view_pdf->setEnabled(true);
#endif
      ma.delete_files->setEnabled(true);
      ma.export_files->setEnabled(true);
      ma.marc_tags_format->setVisible(true);
      ma.parse_marc_tags->setVisible(true);
      ma.showUserButton->setEnabled(true);
      ma.copiesButton->setEnabled(true);
      ma.sruQueryButton->setVisible(true);
      ma.z3950QueryButton->setVisible(true);
      ma.okButton->setVisible(true);
      ma.resetButton->setVisible(true);
      ma.frontButton->setVisible(true);
      ma.backButton->setVisible(true);
      biblioteq_misc_functions::highlightWidget
	(ma.id, m_requiredHighlightColor);
      biblioteq_misc_functions::highlightWidget
	(ma.title, m_requiredHighlightColor);
      biblioteq_misc_functions::highlightWidget
	(ma.publisher->viewport(), m_requiredHighlightColor);
      biblioteq_misc_functions::highlightWidget
	(ma.description->viewport(), m_requiredHighlightColor);
      biblioteq_misc_functions::highlightWidget
	(ma.category->viewport(), m_requiredHighlightColor);
      biblioteq_misc_functions::highlightWidget
	(ma.place->viewport(), m_requiredHighlightColor);
      m_te_orig_pal = ma.description->viewport()->palette();
      setReadOnlyFields(this, false);
    }
  else
    {
      if(m_subType == "Journal")
	setWindowTitle(QString(tr("BiblioteQ: View Journal Details")));
      else
	setWindowTitle(QString(tr("BiblioteQ: View Magazine Details")));

      m_engWindowTitle = "Modify";
      ma.attach_files->setVisible(false);
      ma.view_pdf->setVisible(true);
      ma.delete_files->setVisible(false);
      ma.export_files->setVisible(true);
      ma.issnAvailableCheckBox->setCheckable(false);

      if(qmain->isGuest())
	ma.showUserButton->setVisible(false);
      else
	ma.showUserButton->setEnabled(true);

      ma.copiesButton->setVisible(false);
      ma.sruQueryButton->setVisible(false);
      ma.z3950QueryButton->setVisible(false);
      ma.okButton->setVisible(false);
      ma.resetButton->setVisible(false);
      ma.frontButton->setVisible(false);
      ma.backButton->setVisible(false);

      QList<QAction *> actions = ma.resetButton->menu()->actions();

      if(actions.size() >= 2)
	{
	  actions[0]->setVisible(false);
	  actions[1]->setVisible(false);
	}

      actions.clear();
      setReadOnlyFields(this, true);
    }

  ma.quantity->setMinimum(1);
  ma.price->setMinimum(0.00);
  ma.okButton->setText(tr("&Save"));
  ma.volume->setMinimum(0);
  ma.issue->setMinimum(0);
  str = m_oid;
  query.prepare(QString("SELECT title, "
			"publisher, pdate, place, issuevolume, "
			"category, language, id, "
			"price, monetary_units, quantity, "
			"issueno, "
			"location, lccontrolnumber, callnumber, "
			"deweynumber, description, "
			"front_cover, "
			"back_cover, "
			"marc_tags, "
			"keyword, "
			"accession_number "
			"FROM "
			"%1 "
			"WHERE myoid = ?").arg(m_subType));
  query.bindValue(0, str);
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!query.exec() || !query.next())
    {
      QApplication::restoreOverrideCursor();

      if(m_subType == "Journal")
	{
	  qmain->addError
	    (QString(tr("Database Error")),
	     QString(tr("Unable to retrieve the selected journal's data.")),
	     query.lastError().text(), __FILE__, __LINE__);
	  QMessageBox::critical
	    (this, tr("BiblioteQ: Database Error"),
	     QString(tr("Unable to retrieve the selected journal's "
			"data.")));
	  QApplication::processEvents();
	}
      else
	{
	  qmain->addError
	    (QString(tr("Database Error")),
	     QString(tr("Unable to retrieve the selected magazine's data.")),
	     query.lastError().text(), __FILE__, __LINE__);
	  QMessageBox::critical
	    (this, tr("BiblioteQ: Database Error"),
	     QString(tr("Unable to retrieve the selected magazine's "
			"data.")));
	  QApplication::processEvents();
	}

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
	    ma.title->setText(var.toString().trimmed());
	  else if(fieldname == "publisher")
	    {
	      if(m_subType == "Journal")
		ma.publisher->setMultipleLinks
		  ("journal_search", "publisher",
		   var.toString().trimmed());
	      else
		ma.publisher->setMultipleLinks
		  ("magazine_search", "publisher",
		   var.toString().trimmed());
	    }
	  else if(fieldname == "pdate")
	    ma.publication_date->setDate
	      (QDate::fromString(var.toString().trimmed(),
				 biblioteq::s_databaseDateFormat));
	  else if(fieldname == "price")
	    ma.price->setValue(var.toDouble());
	  else if(fieldname == "place")
	    {
	      if(m_subType == "Journal")
		ma.place->setMultipleLinks("journal_search", "place",
					   var.toString().trimmed());
	      else
		ma.place->setMultipleLinks("magazine_search", "place",
					   var.toString().trimmed());
	    }
	  else if(fieldname == "category")
	    {
	      if(m_subType == "Journal")
		ma.category->setMultipleLinks("journal_search", "category",
					      var.toString().trimmed());
	      else
		ma.category->setMultipleLinks("magazine_search", "category",
					      var.toString().trimmed());
	    }
	  else if(fieldname == "language")
	    {
	      if(ma.language->findText(var.toString().trimmed()) > -1)
		ma.language->setCurrentIndex
		  (ma.language->findText(var.toString().trimmed()));
	      else
		ma.language->setCurrentIndex
		  (ma.language->findText(biblioteq::s_unknown));
	    }
	  else if(fieldname == "quantity")
	    {
	      ma.quantity->setValue(var.toInt());
	      m_oldq = ma.quantity->value();
	    }
	  else if(fieldname == "monetary_units")
	    {
	      if(ma.monetary_units->findText(var.toString().trimmed()) > -1)
		ma.monetary_units->setCurrentIndex
		  (ma.monetary_units->findText(var.toString().trimmed()));
	      else
		ma.monetary_units->setCurrentIndex
		  (ma.monetary_units->findText(biblioteq::s_unknown));
	    }
	  else if(fieldname == "issuevolume")
	    ma.volume->setValue(var.toInt());
	  else if(fieldname == "issueno")
	    ma.issue->setValue(var.toInt());
	  else if(fieldname == "location")
	    {
	      if(ma.location->findText(var.toString().trimmed()) > -1)
		ma.location->setCurrentIndex
		  (ma.location->findText(var.toString().trimmed()));
	      else
		ma.location->setCurrentIndex
		  (ma.location->findText(biblioteq::s_unknown));
	    }
	  else if(fieldname == "id")
	    {
	      if(state == biblioteq::EDITABLE)
		{
		  if(!var.toString().trimmed().trimmed().isEmpty())
		    {
		      if(m_subType == "Journal")
			str =
			  QString(tr("BiblioteQ: Modify Journal Entry (")) +
			  var.toString().trimmed() + tr(")");
		      else
			str =
			  QString(tr("BiblioteQ: Modify Magazine Entry (")) +
			  var.toString().trimmed() + tr(")");
		    }
		  else
		    {
		      if(m_subType == "Journal")
			str = tr("BiblioteQ: Modify Journal Entry");
		      else
			str = tr("BiblioteQ: Modify Magazine Entry");
		    }

		  m_engWindowTitle = "Modify";
		}
	      else
		{
		  if(!var.toString().trimmed().trimmed().isEmpty())
		    {
		      if(m_subType == "Journal")
			str =
			  QString(tr("BiblioteQ: View Journal Details (")) +
			  var.toString().trimmed() + tr(")");
		      else
			str =
			  QString(tr("BiblioteQ: View Magazine Details (")) +
			  var.toString().trimmed() + tr(")");
		    }
		  else
		    {
		      if(m_subType == "Journal")
			str = tr("BiblioteQ: View Journal Details");
		      else
			str = tr("BiblioteQ: View Magazine Details");
		    }

		  m_engWindowTitle = "View";
		}

	      setWindowTitle(str);
	      ma.id->setText(var.toString().trimmed());

	      if(query.isNull(i))
		ma.issnAvailableCheckBox->setChecked(false);
	      else
		ma.issnAvailableCheckBox->setChecked(true);
	    }
	  else if(fieldname == "description")
	    ma.description->setPlainText(var.toString().trimmed());
	  else if(fieldname == "marc_tags")
	    ma.marc_tags->setPlainText(var.toString().trimmed());
	  else if(fieldname == "keyword")
	    {
	      if(m_subType == "Journal")
		ma.keyword->setMultipleLinks("journal_search", "keyword",
					     var.toString().trimmed());
	      else
		ma.keyword->setMultipleLinks("magazine_search", "keyword",
					     var.toString().trimmed());
	    }
	  else if(fieldname == "lccontrolnumber")
	    ma.lcnum->setText(var.toString().trimmed());
	  else if(fieldname == "callnumber")
	    ma.callnum->setText(var.toString().trimmed());
	  else if(fieldname == "deweynumber")
	    ma.deweynum->setText(var.toString().trimmed());
	  else if(fieldname == "front_cover")
	    {
	      if(!record.field(i).isNull())
		{
		  ma.front_image->loadFromData
		    (QByteArray::fromBase64(var.toByteArray()));

		  if(ma.front_image->m_image.isNull())
		    ma.front_image->loadFromData(var.toByteArray());
		}
	    }
	  else if(fieldname == "back_cover")
	    {
	      if(!record.field(i).isNull())
		{
		  ma.back_image->loadFromData
		    (QByteArray::fromBase64(var.toByteArray()));

		  if(ma.back_image->m_image.isNull())
		    ma.back_image->loadFromData(var.toByteArray());
		}
	    }
	  else if(fieldname == "accession_number")
	    ma.accession_number->setText(var.toString().trimmed());
	}

      foreach(auto textfield, findChildren<QLineEdit *> ())
	textfield->setCursorPosition(0);

      storeData(this);

      if(!m_duplicate)
	populateFiles();
    }

  ma.id->setFocus();
  raise();
}

void biblioteq_magazine::populateDisplayAfterSRU(const QByteArray &data)
{
  if(!data.isEmpty())
    {
      ma.marc_tags->setText(data);
      biblioteq_misc_functions::highlightWidget
	(ma.marc_tags->viewport(), m_queryHighlightColor);
    }

  QXmlStreamReader reader(data);

  while(!reader.atEnd())
    if(reader.readNextStartElement())
      if(reader.name().toString().toLower().trimmed() == "datafield")
	{
	  auto tag(reader.attributes().value("tag").toString().trimmed());

	  if(tag == "260")
	    ma.place->clear();
	  else if(tag == "650")
	    ma.category->clear();
	}

  reader.clear();
  reader.addData(data);

  while(!reader.atEnd())
    if(reader.readNextStartElement())
      {
	if(reader.name().toString().toLower().trimmed() == "datafield")
	  {
	    auto tag(reader.attributes().value("tag").toString().trimmed());

	    if(tag == "010")
	      {
		/*
		** $a - LC control number (NR)
		** $b - NUCMC control number (R)
		** $z - Canceled/invalid LC control number (R)
		** $8 - Field link and sequence number (R)
		*/

		QString str("");

		while(reader.readNextStartElement())
		  if(reader.name().toString().toLower().trimmed() == "subfield")
		    {
		      if(reader.attributes().value("code").
			 toString().trimmed() == "a")
			{
			  str.append(reader.readElementText());
			  break;
			}
		      else
			reader.skipCurrentElement();
		    }
		  else
		    break;

		str = str.trimmed();
		ma.lcnum->setText(str);
		biblioteq_misc_functions::highlightWidget
		  (ma.lcnum, m_queryHighlightColor);
	      }
	    else if(tag == "050")
	      {
		/*
		** $a - Classification number (R)
		** $b - Item number (NR)
		** $3 - Materials specified (NR)
		** $6 - Linkage (NR)
		** $8 - Field link and sequence number (R)
		*/

		QString str("");

		while(reader.readNextStartElement())
		  if(reader.name().toString().toLower().trimmed() == "subfield")
		    {
		      if(reader.attributes().value("code").
			 toString().trimmed() == "a" ||
			 reader.attributes().value("code").
			 toString().trimmed() == "b")
			str.append(reader.readElementText());
		      else
			reader.skipCurrentElement();
		    }
		  else
		    break;

		ma.callnum->setText(str);
		biblioteq_misc_functions::highlightWidget
		  (ma.callnum, m_queryHighlightColor);
	      }
	    else if(tag == "082")
	      {
		/*
		** $a - Classification number (R)
		** $b - Item number (NR)
		** $m - Standard or optional designation (NR)
		** $q - Assigning agency (NR)
		** $2 - Edition number (NR)
		** $6 - Linkage (NR)
		** $8 - Field link and sequence number (R)
		*/

		QString str("");

		while(reader.readNextStartElement())
		  if(reader.name().toString().toLower().trimmed() == "subfield")
		    {
		      if(reader.attributes().value("code").
			 toString().trimmed() == "a" ||
			 reader.attributes().value("code").
			 toString().trimmed() == "b" ||
			 reader.attributes().value("code").
			 toString().trimmed() == "m" ||
			 reader.attributes().value("code").
			 toString().trimmed() == "q")
			str.append(reader.readElementText());
		      else
			reader.skipCurrentElement();
		    }
		  else
		    break;

		ma.deweynum->setText(str);
		biblioteq_misc_functions::highlightWidget
		  (ma.deweynum, m_queryHighlightColor);
	      }
	    else if(tag == "245")
	      {
		/*
		** $a - Title (NR)
		** $b - Remainder of title (NR)
		** $c - Statement of responsibility, etc. (NR)
		** $f - Inclusive dates (NR)
		** $g - Bulk dates (NR)
		** $h - Medium (NR)
		** $k - Form (R)
		** $n - Number of part/section of a work (R)
		** $p - Name of part/section of a work (R)
		** $s - Version (NR)
		** $6 - Linkage (NR)
		** $8 - Field link and sequence number (R)
		*/

		QString str("");

		while(reader.readNextStartElement())
		  if(reader.name().toString().toLower().trimmed() == "subfield")
		    {
		      if(reader.attributes().value("code").
			 toString().trimmed() == "a" ||
			 reader.attributes().value("code").
			 toString().trimmed() == "b")
			str.append(reader.readElementText());
		      else
			reader.skipCurrentElement();
		    }
		  else
		    break;

		if(str.lastIndexOf('/') > -1)
		  str = str.mid(0, str.lastIndexOf('/')).trimmed();

		ma.title->setText(str.trimmed());
		biblioteq_misc_functions::highlightWidget
		  (ma.title, m_queryHighlightColor);
	      }
	    else if(tag == "260")
	      {
		/*
		** $a - Place of publication, distribution, etc. (R)
		** $b - Name of publisher, distributor, etc. (R)
		** $c - Date of publication, distribution, etc. (R)
		** $e - Place of manufacture (R)
		** $f - Manufacturer (R)
		** $g - Date of manufacture (R)
		** $3 - Materials specified (NR)
		** $6 - Linkage (NR)
		** $8 - Field link and sequence number (R)
		*/

		QString date("");
		QString place("");
		QString publisher("");

		while(reader.readNextStartElement())
		  if(reader.name().toString().toLower().trimmed() == "subfield")
		    {
		      if(reader.attributes().value("code").
			 toString().trimmed() == "a")
			place = reader.readElementText();
		      else if(reader.attributes().value("code").
			      toString().trimmed() == "b")
			publisher = reader.readElementText();
		      else if(reader.attributes().value("code").
			      toString().trimmed() == "c")
			date = reader.readElementText();
		      else
			reader.skipCurrentElement();
		    }
		  else
		    break;

		ma.publication_date->setDate
		  (QDate::fromString("01/01/" + date,
				     biblioteq::s_databaseDateFormat));
		ma.publication_date->setStyleSheet
		  ("background-color: rgb(162, 205, 90)");

		if(place.lastIndexOf(" ") > -1)
		  place = place.mid(0, place.lastIndexOf(" ")).
		    trimmed();

		if(!place.isEmpty())
		  if(!place[place.length() - 1].isLetter())
		    place = place.remove(place.length() - 1, 1).
		      trimmed();

		ma.place->setPlainText(place);
		biblioteq_misc_functions::highlightWidget
		  (ma.place->viewport(), m_queryHighlightColor);

		if(publisher.endsWith(","))
		  publisher = publisher.mid
		    (0, publisher.length() - 1).trimmed();

		ma.publisher->setPlainText(publisher);
		biblioteq_misc_functions::highlightWidget
		  (ma.publisher->viewport(), m_queryHighlightColor);
	      }
	    else if(tag == "300")
	      {
		/*
		** $a - Extent (R)
		** $b - Other physical details (NR)
		** $c - Dimensions (R)
		** $e - Accompanying material (NR)
		** $f - Type of unit (R)
		** $g - Size of unit (R)
		** $3 - Materials specified (NR)
		** $6 - Linkage (NR)
		** $8 - Field link and sequence number (R)
		*/

		QString str("");

		while(reader.readNextStartElement())
		  if(reader.name().toString().toLower().trimmed() == "subfield")
		    str.append(reader.readElementText());
		  else
		    break;

		ma.description->setPlainText(str);
		biblioteq_misc_functions::highlightWidget
		  (ma.description->viewport(), m_queryHighlightColor);
	      }
	    else if(tag == "650")
	      {
		/*
		** $a - Topical term or geographic name entry
		**      element (NR)
		** $b - Topical term following geographic name entry
		**      element (NR)
		** $c - Location of event (NR)
		** $d - Active dates (NR)
		** $e - Relator term (R)
		** $4 - Relator code (R)
		** $v - Form subdivision (R)
		** $x - General subdivision (R)
		** $y - Chronological subdivision (R)
		** $z - Geographic subdivision (R)
		** $0 - Authority record control number (R)
		** $2 - Source of heading or term (NR)
		** $3 - Materials specified (NR)
		** $6 - Linkage (NR)
		** $8 - Field link and sequence number (R)
		*/

		QString str("");

		while(reader.readNextStartElement())
		  if(reader.name().toString().toLower().trimmed() == "subfield")
		    {
		      if(reader.attributes().value("code").
			 toString().trimmed() == "a")
			{
			  str.append(reader.readElementText());
			  break;
			}
		      else
			reader.skipCurrentElement();
		    }
		  else
		    break;

		if(!str.isEmpty())
		  {
		    if(!str[str.length() - 1].isPunct())
		      str += ".";

		    if(!ma.category->toPlainText().contains(str))
		      {
			if(!ma.category->toPlainText().isEmpty())
			  ma.category->setPlainText
			    (ma.category->toPlainText() + "\n" +
			     str);
			else
			  ma.category->setPlainText(str);

			biblioteq_misc_functions::highlightWidget
			  (ma.category->viewport(),
			   m_queryHighlightColor);
		      }
		  }
	      }
	  }
      }

  foreach(auto textfield, findChildren<QLineEdit *> ())
    textfield->setCursorPosition(0);
}

void biblioteq_magazine::populateDisplayAfterZ3950(const QStringList &list,
						   const QString &recordSyntax)
{
  QString str = "";
  QStringList tmplist;
  int i = 0;
  int j = 0;

  if(!list.isEmpty())
    ma.marc_tags->clear();

  if(recordSyntax == "MARC21")
    {
      for(i = 0; i < list.size(); i++)
	{
	  if(list[i].startsWith("260 "))
	    ma.place->clear();
	  else if(list[i].startsWith("650 "))
	    ma.category->clear();

	  str += list[i] + "\n";
	}
    }
  else
    {
      for(i = 0; i < list.size(); i++)
	{
	  if(list[i].startsWith("210 "))
	    ma.place->clear();
	  else if(list[i].startsWith("606 "))
	    ma.category->clear();

	  str += list[i] + "\n";
	}
    }

  if(!str.isEmpty())
    ma.marc_tags->setPlainText(str.trimmed());

  if(!list.isEmpty())
    biblioteq_misc_functions::highlightWidget
      (ma.marc_tags->viewport(), m_queryHighlightColor);

  if(recordSyntax == "UNIMARC")
    {
      biblioteq_marc m;

      m.initialize(biblioteq_marc::MAGAZINE, biblioteq_marc::Z3950,
		   biblioteq_marc::UNIMARC);
      m.parse(str);
      str = m.category();

      if(!str.isEmpty())
	{
	  ma.category->setPlainText(str);
	  biblioteq_misc_functions::highlightWidget
	    (ma.category->viewport(),
	     m_queryHighlightColor);
	}

      str = m.description();

      if(!str.isEmpty())
	{
	  ma.description->setPlainText(str);
	  biblioteq_misc_functions::highlightWidget
	    (ma.description->viewport(), m_queryHighlightColor);
	}

      str = m.place();

      if(!str.isEmpty())
	{
	  ma.place->setPlainText(str);
	  biblioteq_misc_functions::highlightWidget
	    (ma.place->viewport(), m_queryHighlightColor);
	}

      if(!m.publicationDate().isNull())
	{
	  ma.publication_date->setDate(m.publicationDate());
	  ma.publication_date->setStyleSheet
	    ("background-color: rgb(162, 205, 90)");
	}

      str = m.publisher();

      if(!str.isEmpty())
	{
	  ma.publisher->setPlainText(str);
	  biblioteq_misc_functions::highlightWidget
	    (ma.publisher->viewport(), m_queryHighlightColor);
	}

      str = m.title();

      if(!str.isEmpty())
	{
	  ma.title->setText(str.trimmed());
	  biblioteq_misc_functions::highlightWidget
	    (ma.title, m_queryHighlightColor);
	}

      foreach(auto textfield, findChildren<QLineEdit *> ())
	textfield->setCursorPosition(0);

      return;
    }

  for(i = 0; i < list.size(); i++)
    {
      str = list[i];

      if(str.startsWith("010 "))
	{
	  /*
	  ** $a - LC control number (NR)
	  ** $b - NUCMC control number (R)
	  ** $z - Canceled/invalid LC control number (R)
	  ** $8 - Field link and sequence number (R)
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  QStringList subfields;

	  subfields << "$b"
		    << "$z"
		    << "$8";

	  for(int i = 0; i < subfields.size(); i++)
	    if(str.contains(subfields.at(i)))
	      str = str.mid(0, str.indexOf(subfields.at(i))).trimmed();

	  ma.lcnum->setText(str);
	  biblioteq_misc_functions::highlightWidget
	    (ma.lcnum, m_queryHighlightColor);
	}
      else if(str.startsWith("050 "))
	{
	  /*
	  ** $a - Classification number (R)
	  ** $b - Item number (NR)
	  ** $3 - Materials specified (NR)
	  ** $6 - Linkage (NR)
	  ** $8 - Field link and sequence number (R)
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  str = str.remove(" $b").trimmed();

	  QStringList subfields;

	  subfields << "$3"
		    << "$6"
		    << "$8";

	  for(int i = 0; i < subfields.size(); i++)
	    if(str.contains(subfields.at(i)))
	      str = str.mid(0, str.indexOf(subfields.at(i))).trimmed();

	  ma.callnum->setText(str);
	  biblioteq_misc_functions::highlightWidget
	    (ma.callnum, m_queryHighlightColor);
	}
      else if(str.startsWith("082 "))
	{
	  /*
	  ** $a - Classification number (R)
	  ** $b - Item number (NR)
	  ** $m - Standard or optional designation (NR)
	  ** $q - Assigning agency (NR)
	  ** $2 - Edition number (NR)
	  ** $6 - Linkage (NR)
	  ** $8 - Field link and sequence number (R)
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  str = str.remove(" $b").remove
	    (" $m").remove(" $q").trimmed();

	  QStringList subfields;

	  subfields << "$2"
		    << "$6"
		    << "$8";

	  for(int i = 0; i < subfields.size(); i++)
	    if(str.contains(subfields.at(i)))
	      str = str.mid(0, str.indexOf(subfields.at(i))).trimmed();

	  ma.deweynum->setText(str);
	  biblioteq_misc_functions::highlightWidget
	    (ma.deweynum, m_queryHighlightColor);
	}
      else if(str.startsWith("245 "))
	{
	  /*
	  ** $a - Title (NR)
	  ** $b - Remainder of title (NR)
	  ** $c - Statement of responsibility, etc. (NR)
	  ** $f - Inclusive dates (NR)
	  ** $g - Bulk dates (NR)
	  ** $h - Medium (NR)
	  ** $k - Form (R)
	  ** $n - Number of part/section of a work (R)
	  ** $p - Name of part/section of a work (R)
	  ** $s - Version (NR)
	  ** $6 - Linkage (NR)
	  ** $8 - Field link and sequence number (R)
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  str = str.remove(" $b").trimmed();

	  QStringList subfields;

	  subfields << "$c"
		    << "$f"
		    << "$g"
		    << "$h"
		    << "$k"
		    << "$n"
		    << "$p"
		    << "$s"
		    << "$6"
		    << "$8";

	  for(int i = 0; i < subfields.size(); i++)
	    if(str.contains(subfields.at(i)))
	      str = str.mid(0, str.indexOf(subfields.at(i))).trimmed();

	  if(str.lastIndexOf('/') > -1)
	    str = str.mid(0, str.lastIndexOf('/')).trimmed();

	  ma.title->setText(str.trimmed());
	  biblioteq_misc_functions::highlightWidget
	    (ma.title, m_queryHighlightColor);
	}
      else if(str.startsWith("260 "))
	{
	  /*
	  ** $a - Place of publication, distribution, etc. (R)
	  ** $b - Name of publisher, distributor, etc. (R)
	  ** $c - Date of publication, distribution, etc. (R)
	  ** $e - Place of manufacture (R)
	  ** $f - Manufacturer (R)
	  ** $g - Date of manufacture (R)
	  ** $3 - Materials specified (NR)
	  ** $6 - Linkage (NR)
	  ** $8 - Field link and sequence number (R)
	  */

	  QString tmpstr = "";

	  if(str.indexOf("$a") > -1)
	    tmpstr = str.mid(str.indexOf("$a") + 2).trimmed();
	  else
	    tmpstr = str;

	  QStringList subfields;

	  subfields << "$b"
		    << "$c"
		    << "$e"
		    << "$f"
		    << "$g"
		    << "$3"
		    << "$6"
		    << "$8";

	  for(int i = 0; i < subfields.size(); i++)
	    if(tmpstr.contains(subfields.at(i)))
	      tmpstr = tmpstr.mid(0, tmpstr.indexOf(subfields.at(i))).trimmed();

	  tmplist = tmpstr.split("$a");

	  for(j = 0; j < tmplist.size(); j++)
	    {
	      tmpstr = tmplist.at(j).trimmed();

	      if(tmpstr.lastIndexOf(" ") > -1)
		tmpstr = tmpstr.mid(0, tmpstr.lastIndexOf(" ")).
		  trimmed();

	      if(tmpstr.isEmpty())
		continue;

	      if(!tmpstr[0].isLetterOrNumber())
		tmpstr = tmpstr.mid(1).trimmed();

	      if(tmpstr.isEmpty())
		continue;

	      if(!tmpstr[tmpstr.length() - 1].isLetter())
		tmpstr = tmpstr.remove(tmpstr.length() - 1, 1).
		  trimmed();

	      if(ma.place->toPlainText().isEmpty())
		ma.place->setPlainText(tmpstr);
	      else
		ma.place->setPlainText(ma.place->toPlainText() +
				       "\n" + tmpstr);
	    }

	  biblioteq_misc_functions::highlightWidget
	    (ma.place->viewport(), m_queryHighlightColor);

	  if(str.indexOf("$c") > -1 &&
	     str.mid(str.indexOf("$c") + 2, 4).contains("c"))
	    ma.publication_date->setDate
	      (QDate::fromString
	       ("01/01/" +
		str.mid(str.indexOf("$c") + 4, 4),
		biblioteq::s_databaseDateFormat));
	  else if(str.indexOf("$c") > -1)
	    ma.publication_date->setDate
	      (QDate::fromString
	       ("01/01/" +
		str.mid(str.indexOf("$c") + 3, 4),
		biblioteq::s_databaseDateFormat));

	  ma.publication_date->setStyleSheet
	    ("background-color: rgb(162, 205, 90)");

	  if(str.contains("$b"))
	    str = str.mid(str.indexOf("$b") + 2).trimmed();

	  if(str.contains("$a"))
	    {
	      str = str.mid(0, str.indexOf("$a")).trimmed();

	      if(str.lastIndexOf(" ") > -1)
		str = str.mid(0, str.lastIndexOf(" ")).trimmed();
	    }
	  else if(str.indexOf("$c") > -1)
	    str = str.mid(0, str.indexOf("$c")).trimmed();

	  if(str.endsWith(","))
	    str = str.mid(0, str.length() - 1).trimmed();

	  ma.publisher->setPlainText(str);
	  biblioteq_misc_functions::highlightWidget
	    (ma.publisher->viewport(), m_queryHighlightColor);
	}
      else if(str.startsWith("300 "))
	{
	  /*
	  ** $a - Extent (R)
	  ** $b - Other physical details (NR)
	  ** $c - Dimensions (R)
	  ** $e - Accompanying material (NR)
	  ** $f - Type of unit (R)
	  ** $g - Size of unit (R)
	  ** $3 - Materials specified (NR)
	  ** $6 - Linkage (NR)
	  ** $8 - Field link and sequence number (R)
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  str = str.remove(" $b").trimmed();
	  str = str.remove(" $c").trimmed();
	  str = str.remove(" $e").trimmed();
	  str = str.remove(" $f").trimmed();
	  str = str.remove(" $g").trimmed();
	  str = str.remove(" $3").trimmed();
	  str = str.remove(" $6").trimmed();
	  str = str.remove(" $8").trimmed();
	  ma.description->setPlainText(str);
	  biblioteq_misc_functions::highlightWidget
	    (ma.description->viewport(), m_queryHighlightColor);
	}
      else if(str.startsWith("650 "))
	{
	  /*
	  ** $a - Topical term or geographic name entry
	  **      element (NR)
	  ** $b - Topical term following geographic name entry
	  **      element (NR)
	  ** $c - Location of event (NR)
	  ** $d - Active dates (NR)
	  ** $e - Relator term (R)
	  ** $4 - Relator code (R)
	  ** $v - Form subdivision (R)
	  ** $x - General subdivision (R)
	  ** $y - Chronological subdivision (R)
	  ** $z - Geographic subdivision (R)
	  ** $0 - Authority record control number (R)
	  ** $2 - Source of heading or term (NR)
	  ** $3 - Materials specified (NR)
	  ** $6 - Linkage (NR)
	  ** $8 - Field link and sequence number (R)
	  */

	  if(str.indexOf("$a") > -1)
	    str = str.mid(str.indexOf("$a") + 2).trimmed();

	  QStringList subfields;

	  subfields << "$b"
		    << "$c"
		    << "$d"
		    << "$e"
		    << "$4"
		    << "$v"
		    << "$x"
		    << "$y"
		    << "$z"
		    << "$0"
		    << "$2"
		    << "$3"
		    << "$6"
		    << "$8";

	  for(int i = 0; i < subfields.size(); i++)
	    if(str.contains(subfields.at(i)))
	      str = str.mid(0, str.indexOf(subfields.at(i))).trimmed();

	  if(!str.isEmpty())
	    {
	      if(!str[str.length() - 1].isPunct())
		str += ".";

	      if(!ma.category->toPlainText().contains(str))
		{
		  if(!ma.category->toPlainText().isEmpty())
		    ma.category->setPlainText
		      (ma.category->toPlainText() + "\n" +
		       str);
		  else
		    ma.category->setPlainText(str);

		  biblioteq_misc_functions::highlightWidget
		    (ma.category->viewport(),
		     m_queryHighlightColor);
		}
	    }
	}
    }

  foreach(auto textfield, findChildren<QLineEdit *> ())
    textfield->setCursorPosition(0);
}

void biblioteq_magazine::populateFiles(void)
{
  ma.files->setRowCount(0);
  ma.files->setSortingEnabled(false);

  QSqlQuery query(qmain->getDB());

  if(m_subType == "Journal")
    query.prepare("SELECT COUNT(*) FROM journal_files WHERE item_oid = ?");
  else
    query.prepare("SELECT COUNT(*) FROM magazine_files WHERE item_oid = ?");

  query.bindValue(0, m_oid);

  if(query.exec())
    if(query.next())
      ma.files->setRowCount(query.value(0).toInt());

  query.prepare(QString("SELECT file_name, "
			"file_digest, "
			"LENGTH(file) AS f_s, "
			"description, "
			"myoid FROM %1_files "
			"WHERE item_oid = ? ORDER BY file_name").
		arg(m_subType));
  query.bindValue(0, m_oid);
  QApplication::setOverrideCursor(Qt::WaitCursor);

  QLocale locale;
  int row = 0;
  int totalRows = 0;

  if(query.exec())
    while(query.next() && totalRows < ma.files->rowCount())
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

	    ma.files->setItem(row, i, item);
	  }

	row += 1;
      }

  ma.files->horizontalHeader()->setSortIndicator(FILE, Qt::AscendingOrder);
  ma.files->setRowCount(totalRows);
  ma.files->setSortingEnabled(true);
  QApplication::restoreOverrideCursor();
}

void biblioteq_magazine::search(const QString &field, const QString &value)
{
  m_engWindowTitle = "Search";
  ma.attach_files->setVisible(false);
  ma.view_pdf->setVisible(false);
  ma.coverImages->setVisible(false);
  ma.delete_files->setVisible(false);
  ma.export_files->setVisible(false);
  ma.files->setVisible(false);
  ma.files_label->setVisible(false);
  ma.id->clear();
  ma.lcnum->clear();
  ma.callnum->clear();
  ma.deweynum->clear();
  ma.title->clear();
  ma.publisher->clear();
  ma.place->clear();
  ma.description->clear();
  ma.marc_tags->clear();
  ma.keyword->clear();
  ma.category->clear();
  ma.copiesButton->setVisible(false);
  ma.showUserButton->setVisible(false);
  ma.sruQueryButton->setVisible(false);
  ma.z3950QueryButton->setVisible(false);
  ma.okButton->setText(tr("&Search"));
  ma.publication_date->setDate(QDate::fromString("2001", "yyyy"));
  ma.publication_date->setDisplayFormat("yyyy");
  ma.publication_date_enabled->setVisible(true);
  ma.id->setCursorPosition(0);
  ma.price->setMinimum(-0.01);
  ma.price->setValue(-0.01);
  ma.quantity->setMinimum(0);
  ma.quantity->setValue(0);
  ma.volume->setMinimum(-1);
  ma.volume->setValue(-1);
  ma.issue->setMinimum(-1);
  ma.issue->setValue(-1);
  ma.language->insertItem(0, tr("Any"));
  ma.monetary_units->insertItem(0, tr("Any"));
  ma.location->insertItem(0, tr("Any"));
  ma.location->setCurrentIndex(0);
  ma.language->setCurrentIndex(0);
  ma.monetary_units->setCurrentIndex(0);
  ma.accession_number->clear();
  ma.issnAvailableCheckBox->setCheckable(false);

  if(field.isEmpty() && value.isEmpty())
    {
      auto actions = ma.resetButton->menu()->actions();

      if(actions.size() >= 2)
	{
	  actions[0]->setVisible(false);
	  actions[1]->setVisible(false);
	}

      actions.clear();

      if(m_subType == "Journal")
	setWindowTitle(QString(tr("BiblioteQ: Database Journal Search")));
      else
	setWindowTitle(QString(tr("BiblioteQ: Database Magazine Search")));

      ma.id->setFocus();
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
	ma.publisher->setPlainText(value);
      else if(field == "category")
	ma.category->setPlainText(value);
      else if(field == "place")
	ma.place->setPlainText(value);
      else if(field == "keyword")
	ma.keyword->setPlainText(value);

      slotGo();
    }
}

void biblioteq_magazine::setGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(auto widget, findChildren<QWidget *> ())
    {
      widget->setFont(font);
      widget->update();
    }

  ma.files->resizeRowsToContents();
  update();
}

void biblioteq_magazine::slotAttachFiles(void)
{
  QFileDialog fileDialog
    (this, tr("BiblioteQ: %1 Attachment(s)").arg(m_subType));

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

void biblioteq_magazine::slotCancel(void)
{
  close();
}

void biblioteq_magazine::slotDeleteFiles(void)
{
  auto list(ma.files->selectionModel()->selectedRows(MYOID));

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

      if(m_subType == "Journal")
	query.prepare("DELETE FROM journal_files WHERE "
		      "item_oid = ? AND myoid = ?");
      else
	query.prepare("DELETE FROM magazine_files WHERE "
		      "item_oid = ? AND myoid = ?");

      query.bindValue(0, m_oid);
      query.bindValue(1, list.at(i).data());
      query.exec();
    }

  QApplication::restoreOverrideCursor();
  populateFiles();
}

void biblioteq_magazine::slotExportFiles(void)
{
  QModelIndexList list(ma.files->selectionModel()->selectedRows(MYOID));

  if(list.isEmpty())
    return;

  QFileDialog dialog(this);

  dialog.setDirectory(QDir::homePath());
  dialog.setFileMode(QFileDialog::Directory);
  dialog.setOption(QFileDialog::DontUseNativeDialog);

  if(m_subType.toLower() == "journal")
    dialog.setWindowTitle(tr("BiblioteQ: Journal File Export"));
  else
    dialog.setWindowTitle(tr("BiblioteQ: Magazine File Export"));

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

	  if(m_subType == "Journal")
	    query.prepare("SELECT file, file_name FROM journal_files "
			  "WHERE item_oid = ? AND myoid = ?");
	  else
	    query.prepare("SELECT file, file_name FROM magazine_files "
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

void biblioteq_magazine::slotFilesDoubleClicked(QTableWidgetItem *item)
{
  if(!item)
    return;

  if(item->column() != DESCRIPTION || m_engWindowTitle != "Modify")
    {
      auto item1 = ma.files->item(item->row(), FILE);

      if(!item1)
	return;

#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
      if(item1->text().toLower().trimmed().endsWith(".pdf"))
	{
	  QApplication::setOverrideCursor(Qt::WaitCursor);

	  QByteArray data;
	  QSqlQuery query(qmain->getDB());

	  query.setForwardOnly(true);

	  if(m_subType == "Journal")
	    query.prepare("SELECT file, file_name FROM journal_files "
			  "WHERE item_oid = ? AND myoid = ?");
	  else
	    query.prepare("SELECT file, file_name FROM magazine_files "
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

  auto item1 = ma.files->item(item->row(), DESCRIPTION);

  if(!item1)
    return;

  auto description(item1->text());
  auto item2 = ma.files->item(item->row(), MYOID);

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

  if(m_subType == "Journal")
    query.prepare("UPDATE journal_files SET description = ? "
		  "WHERE item_oid = ? AND myoid = ?");
  else
    query.prepare("UPDATE magazine_files SET description = ? "
		  "WHERE item_oid = ? AND myoid = ?");

  query.bindValue(0, text);
  query.bindValue(1, m_oid);
  query.bindValue(2, myoid);

  if(query.exec())
    item1->setText(text);
}

void biblioteq_magazine::slotGo(void)
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
	  newq = ma.quantity->value();
	  QApplication::setOverrideCursor(Qt::WaitCursor);
	  maxcopynumber = biblioteq_misc_functions::getMaxCopyNumber
	    (qmain->getDB(), m_oid, m_subType, errorstr);

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
		 tr("It appears that you are attempting to decrease the "
		    "number of copies while there are copies "
		    "that have been reserved."));
	      QApplication::processEvents();
	      ma.quantity->setValue(m_oldq);
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

      str = ma.id->text().trimmed();
      ma.id->setText(str);

      if(ma.issnAvailableCheckBox->isChecked())
	{
	  if(ma.id->text().isEmpty())
	    {
	      QMessageBox::critical(this,
				    tr("BiblioteQ: User Error"),
				    tr("Please complete the ISSN field."));
	      QApplication::processEvents();
	      ma.id->setFocus();
	      return;
	    }
	}
      else
	ma.id->clear();

      str = ma.title->text().trimmed();
      ma.title->setText(str);

      if(ma.title->text().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Title field."));
	  QApplication::processEvents();
	  ma.title->setFocus();
	  return;
	}

      str = ma.publisher->toPlainText().trimmed();
      ma.publisher->setPlainText(str);

      if(ma.publisher->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Publisher field."));
	  QApplication::processEvents();
	  ma.publisher->setFocus();
	  return;
	}

      str = ma.place->toPlainText().trimmed();
      ma.place->setPlainText(str);

      if(ma.place->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Place of Publication "
				   "field."));
	  QApplication::processEvents();
	  ma.place->setFocus();
	  return;
	}

      str = ma.category->toPlainText().trimmed();
      ma.category->setPlainText(str);

      if(ma.category->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Categories field."));
	  QApplication::processEvents();
	  ma.category->setFocus();
	  return;
	}

      str = ma.description->toPlainText().trimmed();
      ma.description->setPlainText(str);

      if(ma.description->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Abstract field."));
	  QApplication::processEvents();
	  ma.description->setFocus();
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
      str = ma.lcnum->text().trimmed();
      ma.lcnum->setText(str);
      str = ma.callnum->text().trimmed();
      ma.callnum->setText(str);
      str = ma.deweynum->text().trimmed();
      ma.deweynum->setText(str);
      str = ma.marc_tags->toPlainText().trimmed();
      ma.marc_tags->setPlainText(str);
      str = ma.keyword->toPlainText().trimmed();
      ma.keyword->setPlainText(str);
      str = ma.accession_number->text().trimmed();
      ma.accession_number->setText(str);

      QSqlQuery query(qmain->getDB());

      if(m_engWindowTitle.contains("Modify"))
	query.prepare(QString("UPDATE %1 SET "
			      "id = ?, "
			      "title = ?, "
			      "pdate = ?, "
			      "publisher = ?, "
			      "category = ?, price = ?, "
			      "description = ?, "
			      "language = ?, "
			      "monetary_units = ?, "
			      "quantity = ?, "
			      "location = ?, "
			      "issuevolume = ?, "
			      "issueno = ?, "
			      "lccontrolnumber = ?, "
			      "callnumber = ?, "
			      "deweynumber = ?, "
			      "front_cover = ?, "
			      "back_cover = ?, "
			      "place = ?, "
			      "marc_tags = ?, "
			      "keyword = ?, "
			      "accession_number = ? "
			      "WHERE "
			      "myoid = ?").arg(m_subType));
      else if(qmain->getDB().driverName() != "QSQLITE")
	query.prepare(QString("INSERT INTO %1 "
			      "(id, "
			      "title, "
			      "pdate, publisher, "
			      "category, price, description, language, "
			      "monetary_units, quantity, "
			      "location, issuevolume, issueno, "
			      "lccontrolnumber, callnumber, deweynumber, "
			      "front_cover, back_cover, "
			      "place, marc_tags, keyword, accession_number, "
			      "type) "
			      "VALUES (?, ?, ?, ?, "
			      "?, ?, "
			      "?, ?, ?, ?, "
			      "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
			      "RETURNING myoid").arg
		      (m_subType));
      else
	query.prepare(QString("INSERT INTO %1 "
			      "(id, "
			      "title, "
			      "pdate, publisher, "
			      "category, price, description, language, "
			      "monetary_units, quantity, "
			      "location, issuevolume, issueno, "
			      "lccontrolnumber, callnumber, deweynumber, "
			      "front_cover, back_cover, "
			      "place, marc_tags, keyword, accession_number, "
			      "type, myoid) "
			      "VALUES (?, ?, ?, ?, ?, "
			      "?, ?, ?, "
			      "?, ?, "
			      "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)").arg
		      (m_subType));

      if(ma.issnAvailableCheckBox->isChecked() && !ma.id->text().isEmpty())
	query.bindValue(0, ma.id->text());
      else
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	query.bindValue(0, QVariant(QMetaType(QMetaType::QString)));
#else
	query.bindValue(0, QVariant::String);
#endif

      query.bindValue(1, ma.title->text());
      query.bindValue
	(2, ma.publication_date->date().toString(biblioteq::
						 s_databaseDateFormat));
      query.bindValue(3, ma.publisher->toPlainText());
      query.bindValue(4, ma.category->toPlainText().trimmed());
      query.bindValue(5, ma.price->value());
      query.bindValue(6, ma.description->toPlainText());
      query.bindValue(7, ma.language->currentText().trimmed());
      query.bindValue(8, ma.monetary_units->currentText().trimmed());
      query.bindValue(9, ma.quantity->text());
      query.bindValue(10, ma.location->currentText().trimmed());
      query.bindValue(11, ma.volume->text());
      query.bindValue(12, ma.issue->text());

      if(!ma.lcnum->text().isEmpty())
	query.bindValue(13, ma.lcnum->text());
      else
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	query.bindValue(13, QVariant(QMetaType(QMetaType::QString)));
#else
	query.bindValue(13, QVariant(QVariant::String));
#endif

      if(!ma.callnum->text().isEmpty())
	query.bindValue(14, ma.callnum->text());
      else
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	query.bindValue(14, QVariant(QMetaType(QMetaType::QString)));
#else
	query.bindValue(14, QVariant(QVariant::String));
#endif

      if(!ma.deweynum->text().isEmpty())
	query.bindValue(15, ma.deweynum->text());
      else
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	query.bindValue(15, QVariant(QMetaType(QMetaType::QString)));
#else
	query.bindValue(15, QVariant(QVariant::String));
#endif

      if(!ma.front_image->m_image.isNull())
	{
	  QByteArray bytes;
	  QBuffer buffer(&bytes);

	  if(buffer.open(QIODevice::WriteOnly))
	    {
	      ma.front_image->m_image.save
		(&buffer, ma.front_image->m_imageFormat.toLatin1(), 100);
	      query.bindValue(16, bytes.toBase64());
	    }
	  else
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	    query.bindValue(16, QVariant(QMetaType(QMetaType::QByteArray)));
#else
	    query.bindValue(16, QVariant(QVariant::ByteArray));
#endif
	}
      else
	{
	  ma.front_image->m_imageFormat = "";
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	  query.bindValue(16, QVariant(QMetaType(QMetaType::QByteArray)));
#else
	  query.bindValue(16, QVariant(QVariant::ByteArray));
#endif
	}

      if(!ma.back_image->m_image.isNull())
	{
	  QByteArray bytes;
	  QBuffer buffer(&bytes);

	  if(buffer.open(QIODevice::WriteOnly))
	    {
	      ma.back_image->m_image.save
		(&buffer, ma.back_image->m_imageFormat.toLatin1(), 100);
	      query.bindValue(17, bytes.toBase64());
	    }
	  else
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	    query.bindValue(17, QVariant(QMetaType(QMetaType::QByteArray)));
#else
	    query.bindValue(17, QVariant(QVariant::ByteArray));
#endif
	}
      else
	{
	  ma.back_image->m_imageFormat = "";
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	  query.bindValue(17, QVariant(QMetaType(QMetaType::QByteArray)));
#else
	  query.bindValue(17, QVariant(QVariant::ByteArray));
#endif
	}

      query.bindValue(18, ma.place->toPlainText().trimmed());
      query.bindValue(19, ma.marc_tags->toPlainText().trimmed());
      query.bindValue(20, ma.keyword->toPlainText().trimmed());
      query.bindValue(21, ma.accession_number->text().trimmed());

      if(m_engWindowTitle.contains("Modify"))
	query.bindValue(22, m_oid);
      else
	query.bindValue(22, m_subType);

      if(m_engWindowTitle.contains("Create"))
	if(qmain->getDB().driverName() == "QSQLITE")
	  {
	    auto value = biblioteq_misc_functions::getSqliteUniqueId
	      (qmain->getDB(), errorstr);

	    if(errorstr.isEmpty())
	      {
		query.bindValue(23, value);
		m_oid = QString::number(value);
	      }
	    else
	      qmain->addError(QString(tr("Database Error")),
			      QString(tr("Unable to generate a unique "
					 "integer.")),
			      errorstr);
	  }

      QApplication::setOverrideCursor(Qt::WaitCursor);

      if(!query.exec())
	{
	  QApplication::restoreOverrideCursor();
	  qmain->addError
	    (QString(tr("Database Error")),
	     QString(tr("Unable to create or update the entry.")),
	     query.lastError().text(), __FILE__, __LINE__);
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

	      query.prepare
		(QString("DELETE FROM %1_copy_info WHERE "
			 "myoid NOT IN "
			 "(SELECT myoid FROM %1_copy_info "
			 "WHERE item_oid = ? ORDER BY copy_number "
			 "LIMIT ?) AND item_oid = ?").arg(m_subType));
	      query.addBindValue(m_oid);
	      query.addBindValue(ma.quantity->text());
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
		     qmain->getDB().lastError().text(), __FILE__, __LINE__);
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

	      if(ma.id->text().trimmed().isEmpty())
		biblioteq_misc_functions::createInitialCopies
		  (m_oid + "," + ma.volume->text() + "," + ma.issue->text(),
		   ma.quantity->value(), qmain->getDB(), m_subType, errorstr);
	      else
		biblioteq_misc_functions::createInitialCopies
		  (ma.id->text().trimmed() + "," + ma.volume->text() + "," +
		   ma.issue->text(), ma.quantity->value(),
		   qmain->getDB(), m_subType, errorstr);

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

	  if(!ma.issnAvailableCheckBox->isChecked())
	    ma.id->clear();

	  ma.id->setPalette(m_te_orig_pal);
	  ma.category->viewport()->setPalette(m_te_orig_pal);
	  ma.place->viewport()->setPalette(m_te_orig_pal);
	  ma.lcnum->setPalette(m_white_pal);
	  ma.callnum->setPalette(m_white_pal);
	  ma.deweynum->setPalette(m_white_pal);
	  ma.title->setPalette(m_te_orig_pal);
	  ma.publication_date->setStyleSheet(m_dt_orig_ss);
	  ma.description->viewport()->setPalette(m_te_orig_pal);
	  ma.marc_tags->viewport()->setPalette(m_white_pal);
	  ma.publisher->viewport()->setPalette(m_te_orig_pal);
	  m_oldq = ma.quantity->value();

	  if(ma.front_image->m_image.isNull())
	    ma.front_image->m_imageFormat = "";

	  if(ma.back_image->m_image.isNull())
	    ma.back_image->m_imageFormat = "";

	  if(m_subType == "Journal")
	    ma.publisher->setMultipleLinks
	      ("journal_search", "publisher",
	       ma.publisher->toPlainText());
	  else
	    ma.publisher->setMultipleLinks
	      ("magazine_search", "publisher",
	       ma.publisher->toPlainText());

	  if(m_subType == "Journal")
	    ma.place->setMultipleLinks("journal_search", "place",
				       ma.place->toPlainText());
	  else
	    ma.place->setMultipleLinks("magazine_search", "place",
				       ma.place->toPlainText());

	  if(m_subType == "Journal")
	    ma.category->setMultipleLinks("journal_search", "category",
					  ma.category->toPlainText());
	  else
	    ma.category->setMultipleLinks("magazine_search", "category",
					  ma.category->toPlainText());

	  if(m_subType == "Journal")
	    ma.keyword->setMultipleLinks("journal_search", "keyword",
					 ma.keyword->toPlainText());
	  else
	    ma.keyword->setMultipleLinks("magazine_search", "keyword",
					 ma.keyword->toPlainText());

	  QApplication::restoreOverrideCursor();

	  if(m_engWindowTitle.contains("Modify"))
	    {
	      if(!ma.id->text().trimmed().isEmpty())
		{
		  if(m_subType == "Journal")
		    str = QString(tr("BiblioteQ: Modify Journal Entry ("));
		  else
		    str = QString(tr("BiblioteQ: Modify Magazine Entry ("));

		  str += ma.id->text().trimmed() + tr(")");
		}
	      else
		{
		  if(m_subType == "Journal")
		    str = tr("BiblioteQ: Modify Journal Entry");
		  else
		    str = tr("BiblioteQ: Modify Magazine Entry");
		}

	      setWindowTitle(str);
	      m_engWindowTitle = "Modify";

	      if(m_index->isValid() &&
		 (qmain->getTypeFilterString() == "All" ||
		  qmain->getTypeFilterString() == "All Available" ||
		  qmain->getTypeFilterString() == "All Overdue" ||
		  qmain->getTypeFilterString() == "All Requested" ||
		  qmain->getTypeFilterString() == "All Reserved" ||
		  qmain->getTypeFilterString() == "Journals" ||
		  qmain->getTypeFilterString() == "Magazines"))
		{
		  qmain->getUI().table->setSortingEnabled(false);

		  auto names(qmain->getUI().table->columnNames());

		  for(i = 0; i < names.size(); i++)
		    {
		      if(i == 0 && qmain->showMainTableImages())
			{
			  auto pixmap
			    (QPixmap::fromImage(ma.front_image->m_image));

			  if(!pixmap.isNull())
			    qmain->getUI().table->item(m_index->row(), i)->
			      setIcon(pixmap);
			  else
			    qmain->getUI().table->item(m_index->row(), i)->
			      setIcon(QIcon(":/no_image.png"));
			}

		      if(names.at(i) == "Call Number")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (ma.callnum->text());
		      else if(names.at(i) == "ISSN" ||
			      names.at(i) == "ID Number")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (ma.id->text().trimmed());
		      else if(names.at(i) == "Title")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (ma.title->text());
		      else if(names.at(i) == "Publication Date")
			{
			  if(qmain->getTypeFilterString() == "Journals")
			    qmain->getUI().table->item(m_index->row(), i)->
			      setText
			      (ma.publication_date->date().
			       toString(qmain->
					publicationDateFormat("journals")));
			  else if(qmain->getTypeFilterString() == "Magazines")
			    qmain->getUI().table->item(m_index->row(), i)->
			      setText
			      (ma.publication_date->date().
			       toString(qmain->
					publicationDateFormat("magazines")));
			    else
			      qmain->getUI().table->item(m_index->row(), i)->
				setText(ma.publication_date->date().
					toString(Qt::ISODate));
			}
		      else if(names.at(i) == "Publisher")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (ma.publisher->toPlainText());
		      else if(names.at(i) == "Place of Publication")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (ma.place->toPlainText());
		      else if(names.at(i) == "Categories")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (ma.category->toPlainText().trimmed());
		      else if(names.at(i) == "Price")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (QLocale().toString(ma.price->value()));
		      else if(names.at(i) == "Language")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (ma.language->currentText().trimmed());
		      else if(names.at(i) == "Monetary Units")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (ma.monetary_units->currentText().trimmed());
		      else if(names.at(i) == "Quantity")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (ma.quantity->text());
		      else if(names.at(i) == "Location")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (ma.location->currentText().trimmed());
		      else if(names.at(i) == "Volume")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (ma.volume->text());
		      else if(names.at(i) == "Issue")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (ma.issue->text());
		      else if(names.at(i) == "LC Control Number")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (ma.lcnum->text());
		      else if(names.at(i) == "Call Number")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (ma.callnum->text());
		      else if(names.at(i) == "Dewey Number")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (ma.deweynum->text());
		      else if(names.at(i) == "Availability")
			{
			  qmain->getUI().table->item(m_index->row(), i)->
			    setText
			    (biblioteq_misc_functions::getAvailability
			     (m_oid, qmain->getDB(), m_subType,
			      errorstr));

			  if(!errorstr.isEmpty())
			    qmain->addError
			      (QString(tr("Database Error")),
			       QString(tr("Retrieving availability.")),
			       errorstr, __FILE__, __LINE__);
			}
		      else if(names.at(i) == "Accession Number")
			qmain->getUI().table->item(m_index->row(), i)->setText
			  (ma.accession_number->text());
		    }

		  qmain->getUI().table->setSortingEnabled(true);
		  qmain->getUI().table->updateToolTips(m_index->row());

		  foreach(auto textfield, findChildren<QLineEdit *> ())
		    textfield->setCursorPosition(0);

		  qmain->slotResizeColumns();
		}

	      qmain->slotDisplaySummary();
	      qmain->updateSceneItem(m_oid, m_subType, ma.front_image->m_image);
	    }
	  else
	    {
	      if(m_subType == "Journal")
		qmain->replaceJournal
		  (m_oid, dynamic_cast<biblioteq_journal *> (this));
	      else if(m_subType == "Magazine")
		qmain->replaceMagazine(m_oid, this);

	      updateWindow(biblioteq::EDITABLE);

	      if(qmain->getUI().actionAutoPopulateOnCreation->isChecked())
		{
		  if(m_subType == "Journal")
		    (void) qmain->populateTable
		      (biblioteq::POPULATE_ALL, "Journals", "");
		  else
		    (void) qmain->populateTable
		      (biblioteq::POPULATE_ALL, "Magazines", "");
		}

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
	qmain->addError(QString(tr("Database Error")),
			QString(tr("Rollback failure.")),
			qmain->getDB().lastError().text(), __FILE__,
			__LINE__);

      QApplication::restoreOverrideCursor();
      QMessageBox::critical
	(this, tr("BiblioteQ: Database Error"),
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
	{
	  if(m_subType == "Journal")
	    frontCover = "journal.front_cover ";
	  else
	    frontCover = "magazine.front_cover ";
	}

      searchstr = QString("SELECT DISTINCT %1.title, "
			  "%1.publisher, "
			  "%1.pdate, "
			  "%1.place, "
			  "%1.issuevolume, "
			  "%1.issueno, "
			  "%1.category, "
			  "%1.language, "
			  "%1.id, "
			  "%1.price, "
			  "%1.monetary_units, "
			  "%1.quantity, "
			  "%1.location, "
			  "%1.lccontrolnumber, "
			  "%1.callnumber, "
			  "%1.deweynumber, "
			  "%1.quantity - COUNT(item_borrower.item_oid) "
			  "AS availability, "
			  "COUNT(item_borrower.item_oid) AS "
			  "total_reserved, "
			  "%1.accession_number, "
			  "%1.type, "
			  "%1.myoid, " +
			  frontCover +
			  "FROM "
			  "%1 LEFT JOIN item_borrower ON "
			  "%1.myoid = item_borrower.item_oid "
			  "AND item_borrower.type = '%1' "
			  "WHERE %1.type = '%1' AND ").arg(m_subType);

      if(!ma.id->text().trimmed().isEmpty())
	{
	  searchstr.append("LOWER(id) LIKE LOWER('%' || ? || '%') AND ");
	  values.append(ma.id->text().trimmed());
	}

      QString ESCAPE("");
      auto UNACCENT(qmain->unaccent());

      if(qmain->getDB().driverName() != "QSQLITE")
	ESCAPE = "E";

      searchstr.append
	(UNACCENT + "(LOWER(COALESCE(lccontrolnumber, ''))) LIKE " +
	 UNACCENT + "(LOWER(" + ESCAPE + "'%' || ? || '%')) AND ");
      values.append(biblioteq_myqstring::escape(ma.lcnum->text().trimmed()));
      searchstr.append
	(UNACCENT + "(LOWER(COALESCE(callnumber, ''))) LIKE " + UNACCENT +
	 "(LOWER(" + ESCAPE + "'%' || ? || '%')) AND ");
      values.append(biblioteq_myqstring::escape(ma.callnum->text().trimmed()));
      searchstr.append
	(UNACCENT + "(LOWER(COALESCE(deweynumber, ''))) LIKE " + UNACCENT +
	 "(LOWER(" + ESCAPE + "'%' || ? || '%')) AND ");
      values.append(biblioteq_myqstring::escape(ma.deweynum->text().trimmed()));

      if(ma.volume->value() != -1)
	searchstr.append("issuevolume = " + ma.volume->text() +
			 " AND ");

      if(ma.issue->value() != -1)
	searchstr.append("issueno = " + ma.issue->text() +
			 " AND ");

      searchstr.append
	(UNACCENT + "(LOWER(title)) LIKE " + UNACCENT +
	 "(LOWER(" + ESCAPE + "'%' || ? || '%')) AND ");
      values.append(biblioteq_myqstring::escape(ma.title->text().trimmed()));

      if(ma.publication_date_enabled->isChecked())
	searchstr.append("SUBSTR(pdate, 7) = '" +
			 ma.publication_date->date().toString("yyyy") +
			 "' AND ");

      searchstr.append
	(UNACCENT + "(LOWER(publisher)) LIKE " + UNACCENT +
	 "(LOWER(" + ESCAPE+ "'%' || ? || '%')) AND ");
      values.append
	(biblioteq_myqstring::escape(ma.publisher->toPlainText().trimmed()));
      searchstr.append
	(UNACCENT + "(LOWER(place)) LIKE " + UNACCENT +
	 "(LOWER(" + ESCAPE + "'%' || ? || '%')) AND ");
      values.append
	(biblioteq_myqstring::escape(ma.place->toPlainText().trimmed()));
      searchstr.append
	(UNACCENT + "(LOWER(category)) LIKE " + UNACCENT +
	 "(LOWER(" + ESCAPE + "'%' || ? || '%')) AND ");
      values.append
	(biblioteq_myqstring::escape(ma.category->toPlainText().trimmed()));

      if(ma.price->value() > -0.01)
	{
	  searchstr.append("price = ");
	  searchstr.append(QString::number(ma.price->value()));
	  searchstr.append(" AND ");
	}

      if(ma.language->currentIndex() != 0)
	searchstr.append
	  (UNACCENT + "(language) = " + UNACCENT + "(" + ESCAPE + "'" +
	   biblioteq_myqstring::escape(ma.language->currentText().
	    trimmed()) +
	   "') AND ");

      if(ma.monetary_units->currentIndex() != 0)
	searchstr.append
	  (UNACCENT + "(monetary_units) = " + UNACCENT + "(" + ESCAPE + "'" +
	   biblioteq_myqstring::escape(ma.monetary_units->currentText().
				       trimmed()) +
	   "') AND ");

      searchstr.append
	(UNACCENT + "(LOWER(description)) LIKE " + UNACCENT +
	 "(LOWER(" + ESCAPE + "'%' || ? || '%')) ");
      values.append
	(biblioteq_myqstring::escape(ma.description->toPlainText().trimmed()));

      if(ma.quantity->value() != 0)
	searchstr.append("AND quantity = " + ma.quantity->text() + " ");

      if(ma.location->currentIndex() != 0)
	searchstr.append
	  ("AND " + UNACCENT + "(location) = " +
	   UNACCENT + "(" + ESCAPE + "'" +
	   biblioteq_myqstring::escape(ma.location->currentText().
				       trimmed()) + "') ");

      searchstr.append
	("AND " + UNACCENT + "(LOWER(COALESCE(marc_tags, ''))) LIKE " +
	 UNACCENT + "(LOWER(" + ESCAPE + "'%' || ? || '%')) ");
      values.append
	(biblioteq_myqstring::escape(ma.marc_tags->toPlainText().trimmed()));
      searchstr.append
	("AND " + UNACCENT + "(LOWER(COALESCE(keyword, ''))) LIKE " +
	 UNACCENT + "(LOWER(" + ESCAPE + "'%' || ? || '%')) ");
      values.append
	(biblioteq_myqstring::escape(ma.keyword->toPlainText().trimmed()));
      searchstr.append
	("AND " + UNACCENT + "(LOWER(COALESCE(accession_number, ''))) LIKE " +
	 UNACCENT + "(LOWER(" +
	 ESCAPE + "'%' || ? || '%')) ");
      values.append
	(biblioteq_myqstring::escape(ma.accession_number->text().trimmed()));

      if(m_subType == "Journal")
	searchstr.append("GROUP BY journal.title, "
			 "journal.publisher, "
			 "journal.pdate, "
			 "journal.place, "
			 "journal.issuevolume, "
			 "journal.issueno, "
			 "journal.category, "
			 "journal.language, "
			 "journal.id, "
			 "journal.price, "
			 "journal.monetary_units, "
			 "journal.quantity, "
			 "journal.location, "
			 "journal.lccontrolnumber, "
			 "journal.callnumber, "
			 "journal.deweynumber, "
			 "journal.accession_number, "
			 "journal.type, "
			 "journal.myoid, "
			 "journal.front_cover");
      else
	searchstr.append("GROUP BY magazine.title, "
			 "magazine.publisher, "
			 "magazine.pdate, "
			 "magazine.place, "
			 "magazine.issuevolume, "
			 "magazine.issueno, "
			 "magazine.category, "
			 "magazine.language, "
			 "magazine.id, "
			 "magazine.price, "
			 "magazine.monetary_units, "
			 "magazine.quantity, "
			 "magazine.location, "
			 "magazine.lccontrolnumber, "
			 "magazine.callnumber, "
			 "magazine.deweynumber, "
			 "magazine.accession_number, "
			 "magazine.type, "
			 "magazine.myoid, "
			 "magazine.front_cover");

      auto query = new QSqlQuery(qmain->getDB());

      query->prepare(searchstr);

      for(int i = 0; i < values.size(); i++)
	query->addBindValue(values.at(i));

      if(m_subType == "Journal")
	(void) qmain->populateTable
	  (query, "Journals", biblioteq::NEW_PAGE, biblioteq::POPULATE_SEARCH);
      else
	(void) qmain->populateTable
	  (query, "Magazines", biblioteq::NEW_PAGE, biblioteq::POPULATE_SEARCH);
    }
}

void biblioteq_magazine::slotParseMarcTags(void)
{
  auto text(ma.marc_tags->toPlainText().trimmed());

  if(text.endsWith("</record>") && text.startsWith("<record"))
    populateDisplayAfterSRU(text.toUtf8());
  else
    switch(ma.marc_tags_format->currentIndex())
      {
      case 0:
	{
	  biblioteq_marc m;

	  m.initialize(biblioteq_marc::MAGAZINE,
		       biblioteq_marc::Z3950,
		       biblioteq_marc::UNIMARC);
	  m.parse(text);

	  if(!m.publisher().isEmpty())
	    populateDisplayAfterZ3950(QStringList() << text, "UNIMARC");
	  else
	    populateDisplayAfterZ3950(QStringList() << text, "MARC21");

	  break;
	}
      case 1:
	{
	  populateDisplayAfterZ3950(QStringList() << text, "MARC21");
	  break;
	}
      default:
	{
	  populateDisplayAfterZ3950(QStringList() << text, "UNIMARC");
	  break;
	}
      }
}

void biblioteq_magazine::slotPopulateCopiesEditor(void)
{
  biblioteq_copy_editor *copyeditor = nullptr;

  copyeditor = new biblioteq_copy_editor
    (qobject_cast<QWidget *> (this),
     qmain,
     static_cast<biblioteq_item *> (this),
     false,
     ma.quantity->value(),
     m_oid,
     ma.quantity,
     font(),
     m_subType,
     ma.id->text().trimmed(),
     false);
  copyeditor->populateCopiesEditor();
}

void biblioteq_magazine::slotPrint(void)
{
  m_html = "<html>";
  m_html += "<b>" + tr("ISSN:") + "</b> " + ma.id->text().trimmed() + "<br>";
  m_html += "<b>" + tr("Volume:") + "</b> " + ma.volume->text() + "<br>";
  m_html += "<b>" + tr("Issue (Number):") + "</b> " + ma.issue->text() +
    "<br>";
  m_html += "<b>" + tr("LC Control Number:") + "</b> " +
    ma.lcnum->text().trimmed() + "<br>";
  m_html += "<b>" + tr("Call Number:") + "</b> " +
    ma.callnum->text().trimmed() + "<br>";
  m_html += "<b>" + tr("Dewey Class Number:") + "</b> " +
    ma.deweynum->text().trimmed() + "<br>";

  /*
  ** General information.
  */

  m_html += "<b>" + tr("Title:") + "</b> " + ma.title->text().trimmed() +
    "<br>";
  m_html += "<b>" + tr("Publication Date:") + "</b> " +
    ma.publication_date->date().toString(Qt::ISODate) + "<br>";
  m_html += "<b>" + tr("Publisher:") + "</b> " +
    ma.publisher->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Place of Publication:") + "</b> " +
    ma.place->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Category:") + "</b> " +
    ma.category->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Price:") + "</b> " + ma.price->cleanText() + "<br>";
  m_html += "<b>" + tr("Language:") + "</b> " +
    ma.language->currentText() + "<br>";
  m_html += "<b>" + tr("Monetary Units:") + "</b> " +
    ma.monetary_units->currentText() + "<br>";
  m_html += "<b>" + tr("Copies:") + "</b> " + ma.quantity->text() + "<br>";
  m_html += "<b>" + tr("Location:") + "</b> " +
    ma.location->currentText() + "<br>";
  m_html += "<b>" + tr("Abstract:") + "</b> " +
    ma.description->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("MARC Tags:") + "</b> " +
    ma.marc_tags->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Keywords:") + "</b> " +
    ma.keyword->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Accession Number:") + "</b> " +
    ma.accession_number->text().trimmed();
  m_html += "</html>";
  print(this);
}

void biblioteq_magazine::slotProxyAuthenticationRequired
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

void biblioteq_magazine::slotPublicationDateEnabled(bool state)
{
  ma.publication_date->setEnabled(state);

  if(!state)
    ma.publication_date->setDate(QDate::fromString("2001", "yyyy"));
}

void biblioteq_magazine::slotReset(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(action != nullptr)
    {
      auto actions = ma.resetButton->menu()->actions();

      if(actions.size() < 22)
	{
	  // Error.
	}
      else if(action == actions[0])
	ma.front_image->clear();
      else if(action == actions[1])
	ma.back_image->clear();
      else if(action == actions[2])
	{
	  ma.id->clear();
	  ma.id->setCursorPosition(0);
	  ma.id->setPalette(m_te_orig_pal);
	  ma.id->setFocus();
	}
      else if(action == actions[3])
	{
	  ma.volume->setValue(ma.volume->minimum());
	  ma.volume->setFocus();
	}
      else if(action == actions[4])
	{
	  ma.issue->setValue(ma.issue->minimum());
	  ma.issue->setFocus();
	}
      else if(action == actions[5])
	{
	  ma.lcnum->clear();
	  ma.lcnum->setPalette(m_white_pal);
	  ma.lcnum->setFocus();
	}
      else if(action == actions[6])
	{
	  ma.callnum->clear();
	  ma.callnum->setPalette(m_white_pal);
	  ma.callnum->setFocus();
	}
      else if(action == actions[7])
	{
	  ma.deweynum->clear();
	  ma.deweynum->setPalette(m_white_pal);
	  ma.deweynum->setFocus();
	}
      else if(action == actions[8])
	{
	  ma.title->clear();
	  ma.title->setPalette(m_te_orig_pal);
	  ma.title->setFocus();
	}
      else if(action == actions[9])
	{
	  if(m_engWindowTitle.contains("Search"))
	    {
	      ma.publication_date->setDate(QDate::fromString("2001", "yyyy"));
	      ma.publication_date_enabled->setChecked(false);
	    }
	  else
	    ma.publication_date->setDate
	      (QDate::fromString("01/01/2000",
				 biblioteq::s_databaseDateFormat));

	  ma.publication_date->setFocus();
	  ma.publication_date->setStyleSheet(m_dt_orig_ss);
	}
      else if(action == actions[10])
	{
	  if(!m_engWindowTitle.contains("Search"))
	    ma.publisher->setPlainText("N/A");
	  else
	    ma.publisher->clear();

	  ma.publisher->viewport()->setPalette(m_te_orig_pal);
	  ma.publisher->setFocus();
	}
      else if(action == actions[11])
	{
	  if(!m_engWindowTitle.contains("Search"))
	    ma.place->setPlainText("N/A");
	  else
	    ma.place->clear();

	  ma.place->viewport()->setPalette(m_te_orig_pal);
	  ma.place->setFocus();
	}
      else if(action == actions[12])
	{
	  if(!m_engWindowTitle.contains("Search"))
	    ma.category->setPlainText("N/A");
	  else
	    ma.category->clear();

	  ma.category->viewport()->setPalette(m_te_orig_pal);
	  ma.category->setFocus();
	}
      else if(action == actions[13])
	{
	  ma.price->setValue(ma.price->minimum());
	  ma.price->setFocus();
	}
      else if(action == actions[14])
	{
	  ma.language->setCurrentIndex(0);
	  ma.language->setFocus();
	}
      else if(action == actions[15])
	{
	  ma.monetary_units->setCurrentIndex(0);
	  ma.monetary_units->setFocus();
	}
      else if(action == actions[16])
	{
	  ma.quantity->setValue(ma.quantity->minimum());
	  ma.quantity->setFocus();
	}
      else if(action == actions[17])
	{
	  ma.location->setCurrentIndex(0);
	  ma.location->setFocus();
	}
      else if(action == actions[18])
	{
	  if(!m_engWindowTitle.contains("Search"))
	    ma.description->setPlainText("N/A");
	  else
	    ma.description->clear();

	  ma.description->viewport()->setPalette(m_te_orig_pal);
	  ma.description->setFocus();
	}
      else if(action == actions[19])
	{
	  ma.marc_tags->clear();
	  ma.marc_tags->viewport()->setPalette(m_white_pal);
	  ma.marc_tags->setFocus();
	}
      else if(action == actions[20])
	{
	  ma.keyword->clear();
	  ma.keyword->setFocus();
	}
      else if(action == actions[21])
	{
	  ma.accession_number->clear();
	  ma.accession_number->setFocus();
	}

      actions.clear();
    }
  else
    {
      /*
      ** Reset all.
      */

      ma.id->clear();
      ma.id->setCursorPosition(0);
      ma.title->clear();

      if(!m_engWindowTitle.contains("Search"))
	ma.category->setPlainText("N/A");
      else
	ma.category->clear();

      if(!m_engWindowTitle.contains("Search"))
	ma.place->setPlainText("N/A");
      else
	ma.place->clear();

      if(!m_engWindowTitle.contains("Search"))
	ma.publisher->setPlainText("N/A");
      else
	ma.publisher->clear();

      if(!m_engWindowTitle.contains("Search"))
	ma.description->setPlainText("N/A");
      else
	ma.description->clear();

      ma.marc_tags->clear();
      ma.keyword->clear();
      ma.volume->setValue(ma.volume->minimum());
      ma.issue->setValue(ma.issue->minimum());
      ma.price->setValue(ma.price->minimum());

      if(m_engWindowTitle.contains("Search"))
	{
	  ma.publication_date->setDate(QDate::fromString("2001", "yyyy"));
	  ma.publication_date_enabled->setChecked(false);
	}
      else
	ma.publication_date->setDate
	  (QDate::fromString("01/01/2000", biblioteq::s_databaseDateFormat));

      ma.language->setCurrentIndex(0);
      ma.monetary_units->setCurrentIndex(0);
      ma.quantity->setValue(ma.quantity->minimum());
      ma.location->setCurrentIndex(0);
      ma.lcnum->clear();
      ma.callnum->clear();
      ma.deweynum->clear();
      ma.front_image->clear();
      ma.back_image->clear();
      ma.id->setPalette(m_te_orig_pal);
      ma.category->viewport()->setPalette(m_te_orig_pal);
      ma.place->viewport()->setPalette(m_te_orig_pal);
      ma.lcnum->setPalette(m_white_pal);
      ma.callnum->setPalette(m_white_pal);
      ma.deweynum->setPalette(m_white_pal);
      ma.title->setPalette(m_te_orig_pal);
      ma.publication_date->setStyleSheet(m_dt_orig_ss);
      ma.description->viewport()->setPalette(m_te_orig_pal);
      ma.marc_tags->viewport()->setPalette(m_white_pal);
      ma.publisher->viewport()->setPalette(m_te_orig_pal);
      ma.accession_number->clear();
      ma.id->setFocus();
    }
}

void biblioteq_magazine::slotSRUCanceled(void)
{
  auto reply = m_sruManager->findChild<QNetworkReply *> ();

  if(reply)
    reply->deleteLater();

  m_sruResults.clear();
}

void biblioteq_magazine::slotSRUDownloadFinished(bool error)
{
  auto canceled = true;

  if(m_sruWorking)
    {
      canceled = m_sruWorking->wasCanceled();
      m_sruWorking->deleteLater();
    }

  if(!canceled && !error)
    QTimer::singleShot(250, this, SLOT(sruDownloadFinished(void)));
}

void biblioteq_magazine::slotSRUDownloadFinished(void)
{
  auto error = false;
  auto reply = qobject_cast<QNetworkReply *> (sender());

  if(reply)
    {
      error = reply->error() != QNetworkReply::NoError;
      reply->deleteLater();
    }

  auto canceled = true;

  if(m_sruWorking)
    {
      canceled = m_sruWorking->wasCanceled();
      m_sruWorking->deleteLater();
    }

  if(!canceled && !error)
    QTimer::singleShot(250, this, SLOT(sruDownloadFinished(void)));
}

void biblioteq_magazine::slotSRUError(QNetworkReply::NetworkError error)
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

void biblioteq_magazine::slotSRUQuery(void)
{
  if(m_sruManager->findChild<QNetworkReply *> ())
    return;

  if(ma.id->text().trimmed().isEmpty())
    {
      QMessageBox::critical
	(this,
	 tr("BiblioteQ: User Error"),
	 tr("In order to query an SRU site, the ISSN must be provided."));
      QApplication::processEvents();
      ma.id->setFocus();
      return;
    }

  createSRUDialog();

  if(m_sruWorking)
    {
      m_sruWorking->show();
      m_sruWorking->update();
      m_sruWorking->repaint();
      QApplication::processEvents();
    }

  QString name("");
  auto found = false;

  for(int i = 0; i < ma.sruQueryButton->actions().size(); i++)
    if(ma.sruQueryButton->actions().at(i)->isChecked())
      {
	found = true;
	name = ma.sruQueryButton->actions().at(i)->text();
	break;
      }

  if(!found)
    name = qmain->getPreferredSRUSite();

  QString searchstr("");
  auto hash(qmain->getSRUHash(name));
  auto issn(ma.id->text().remove('-').trimmed());

  issn = issn.mid(0, 4) + "-" + issn.mid(4);
  searchstr = hash.value("url_issn");
  searchstr.replace("%1", issn);

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
	  QNetworkProxyQuery query(url);
	  auto list(QNetworkProxyFactory::systemProxyForQuery(query));

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

void biblioteq_magazine::slotSRUQueryError(const QString &text)
{
  if(text.trimmed().isEmpty())
    return;

  QMessageBox::critical
    (this, tr("BiblioteQ: SRU Query Error"),
     tr("A network error (%1) occurred.").arg(text.trimmed()));
  QApplication::processEvents();
}

void biblioteq_magazine::slotSRUReadyRead(void)
{
  auto reply = qobject_cast<QNetworkReply *> (sender());

  if(reply)
    m_sruResults.append(reply->readAll());
}

void biblioteq_magazine::slotSRUSslErrors(const QList<QSslError> &list)
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

void biblioteq_magazine::slotSelectImage(void)
{
  QFileDialog dialog(this);
  auto button = qobject_cast<QPushButton *> (sender());

  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setDirectory(QDir::homePath());
  dialog.setOption(QFileDialog::DontUseNativeDialog);

  if(button == ma.frontButton)
    dialog.setWindowTitle(tr("BiblioteQ: Front Cover Image Selection"));
  else
    dialog.setWindowTitle(tr("BiblioteQ: Back Cover Image Selection"));

  dialog.exec();
  QApplication::processEvents();

  if(dialog.result() == QDialog::Accepted)
    {
      if(button == ma.frontButton)
	{
	  ma.front_image->clear();
	  ma.front_image->m_image = QImage(dialog.selectedFiles().value(0));

	  if(dialog.selectedFiles().value(0).lastIndexOf(".") > -1)
	    ma.front_image->m_imageFormat = dialog.selectedFiles().value(0).mid
	      (dialog.selectedFiles().value(0).lastIndexOf(".") + 1).
	      toUpper();

	  ma.front_image->scene()->addPixmap
	    (QPixmap::fromImage(ma.front_image->m_image));

	  if(!ma.front_image->scene()->items().isEmpty())
	    ma.front_image->scene()->items().at(0)->setFlags
	      (QGraphicsItem::ItemIsSelectable);

	  ma.front_image->scene()->setSceneRect
	    (ma.front_image->scene()->itemsBoundingRect());
	}
      else
	{
	  ma.back_image->clear();
	  ma.back_image->m_image = QImage(dialog.selectedFiles().value(0));

	  if(dialog.selectedFiles().value(0).lastIndexOf(".") > -1)
	    ma.back_image->m_imageFormat = dialog.selectedFiles().value(0).mid
	      (dialog.selectedFiles().value(0).lastIndexOf(".") + 1).
	      toUpper();

	  ma.back_image->scene()->addPixmap
	    (QPixmap::fromImage(ma.back_image->m_image));

	  if(!ma.back_image->scene()->items().isEmpty())
	    ma.back_image->scene()->items().at(0)->setFlags
	      (QGraphicsItem::ItemIsSelectable);

	  ma.back_image->scene()->setSceneRect
	    (ma.back_image->scene()->itemsBoundingRect());
	}
    }
}

void biblioteq_magazine::slotShowPDF(void)
{
  auto list(ma.files->selectionModel()->selectedRows(MYOID));

  if(list.isEmpty())
    return;

  auto reader = new biblioteq_pdfreader(qmain);

  QApplication::setOverrideCursor(Qt::WaitCursor);

  QByteArray data;
  QSqlQuery query(qmain->getDB());

  if(m_subType == "Journal")
    query.prepare
      ("SELECT file, file_name FROM journal_files "
       "WHERE item_oid = ? AND myoid = ?");
  else
    query.prepare
      ("SELECT file, file_name FROM magazine_files "
       "WHERE item_oid = ? AND myoid = ?");

  query.bindValue(0, m_oid);
  query.bindValue(1, list.at(0).data());

  if(query.exec() && query.next())
    data = qUncompress(query.value(0).toByteArray());

  reader->load(data, query.value(1).toString().trimmed());
  reader->show();
  QApplication::restoreOverrideCursor();
}

void biblioteq_magazine::slotShowUsers(void)
{
  biblioteq_borrowers_editor *borrowerseditor = nullptr;
  int state = 0;

  if(!ma.okButton->isHidden())
    state = biblioteq::EDITABLE;
  else
    state = biblioteq::VIEW_ONLY;

  borrowerseditor = new biblioteq_borrowers_editor
    (qobject_cast<QWidget *> (this),
     qmain,
     static_cast<biblioteq_item *> (this),
     ma.quantity->value(),
     m_oid,
     ma.id->text().trimmed(),
     font(),
     m_subType,
     state);
  borrowerseditor->showUsers();
}

void biblioteq_magazine::slotZ3950Query(void)
{
  if(findChild<biblioteq_generic_thread *> ())
    return;

  QString errorstr = "";
  QString etype = "";
  QString searchstr = "";
  QStringList list;
  int i = 0;

  if(ma.id->text().trimmed().isEmpty())
    {
      QMessageBox::critical
	(this,
	 tr("BiblioteQ: User Error"),
	 tr("In order to query a Z39.50 site, the ISSN must be provided."));
      QApplication::processEvents();
      ma.id->setFocus();
      return;
    }

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
  auto found = false;

  for(i = 0; i < ma.z3950QueryButton->actions().size(); i++)
    if(ma.z3950QueryButton->actions().at(i)->isChecked())
      {
	found = true;
	recordSyntax = qmain->getZ3950Hash
	  (ma.z3950QueryButton->actions().at(i)->text()).
	  value("RecordSyntax");
	m_thread->setZ3950Name
	  (ma.z3950QueryButton->actions().at(i)->text());
	break;
      }

  if(!found)
    {
      recordSyntax = qmain->getZ3950Hash
	(qmain->getPreferredZ3950Site()).value("RecordSyntax");
      m_thread->setZ3950Name(qmain->getPreferredZ3950Site());
    }

  auto issn(ma.id->text().remove('-').trimmed());

  issn = issn.mid(0, 4) + "-" + issn.mid(4);
  searchstr = QString("@attr 1=8 %1").arg(issn);
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

  if((errorstr = m_thread->getErrorStr()).isEmpty())
    {
      if(m_thread->getZ3950Results().size() == 1)
	{
	  if(QMessageBox::question
	     (this, tr("BiblioteQ: Question"),
	      tr("Replace existing values with those retrieved "
		 "from the Z39.50 site?"),
	      QMessageBox::Yes | QMessageBox::No,
	      QMessageBox::No) == QMessageBox::Yes)
	    {
	      QApplication::processEvents();
	      list = QString(m_thread->getZ3950Results()[0]).split("\n");
	      populateDisplayAfterZ3950(list, recordSyntax);
	      list.clear();
	    }

	  QApplication::processEvents();
	}
      else if(m_thread->getZ3950Results().size() > 1)
	{
	  for(i = 0; i < m_thread->getZ3950Results().size(); i++)
	    list.append(m_thread->getZ3950Results()[i]);

	  /*
	  ** Display a selection dialog.
	  */

	  new biblioteq_z3950results
	    (qobject_cast<QWidget *> (this), list, this, font(), recordSyntax);
	}
      else
	{
	  QMessageBox::critical
	    (this,
	     tr("BiblioteQ: Z39.50 Query Error"),
	     tr("A Z39.50 entry may not yet exist for ") + issn + tr("."));
	  QApplication::processEvents();
	}
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
	 tr("The Z39.50 entry could not be retrieved."));
      QApplication::processEvents();
    }
}

void biblioteq_magazine::sruDownloadFinished(void)
{
  QList<QByteArray> list;

  while(m_sruResults.indexOf("<record") >= 0 &&
	m_sruResults.indexOf("</record>") >= 0)
    {
      list.append(m_sruResults.mid(m_sruResults.indexOf("<record"),
				   m_sruResults.indexOf("</record>") -
				   m_sruResults.indexOf("<record") + 9));
      m_sruResults.remove(m_sruResults.indexOf("<record"),
			  m_sruResults.indexOf("</record>") -
			  m_sruResults.indexOf("<record") + 9);
    }

  if(list.size() == 1)
    {
      if(QMessageBox::question(this, tr("BiblioteQ: Question"),
			       tr("Replace existing values with "
				  "those retrieved "
				  "from the SRU site?"),
			       QMessageBox::Yes | QMessageBox::No,
			       QMessageBox::No) == QMessageBox::Yes)
	{
	  QApplication::processEvents();
	  populateDisplayAfterSRU(list[0]);
	}

      QApplication::processEvents();
    }
  else if(list.size() > 1)
    {
      /*
      ** Display a selection dialog. Destroyed on close.
      */

      new biblioteq_sruresults
	(qobject_cast<QWidget *> (this), list, this, font());
    }
  else
    {
      QMessageBox::critical
	(this,
	 tr("BiblioteQ: SRU Query Error"),
	 tr("An SRU entry may not yet exist for ") +
	 ma.id->text().trimmed() +
	 tr(" or a network error occurred."));
      QApplication::processEvents();
    }
}

void biblioteq_magazine::updateWindow(const int state)
{
  QString str = "";

  if(state == biblioteq::EDITABLE)
    {
      ma.attach_files->setEnabled(true);
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
      ma.view_pdf->setEnabled(true);
#endif
      ma.copiesButton->setEnabled(true);
      ma.delete_files->setEnabled(true);
      ma.export_files->setEnabled(true);
      ma.marc_tags_format->setVisible(true);
      ma.parse_marc_tags->setVisible(true);
      ma.showUserButton->setEnabled(true);
      ma.copiesButton->setEnabled(true);
      ma.sruQueryButton->setVisible(true);
      ma.z3950QueryButton->setVisible(true);
      ma.okButton->setVisible(true);
      ma.resetButton->setVisible(true);
      ma.frontButton->setVisible(true);
      ma.backButton->setVisible(true);

      if(!ma.id->text().trimmed().isEmpty())
	{
	  if(m_subType == "Journal")
	    str = QString(tr("BiblioteQ: Modify Journal Entry ("));
	  else
	    str = QString(tr("BiblioteQ: Modify Magazine Entry ("));

	  str += ma.id->text().trimmed() + tr(")");
	}
      else
	{
	  if(m_subType == "Journal")
	    str = tr("BiblioteQ: Modify Journal Entry");
	  else
	    str = tr("BiblioteQ: Modify Magazine Entry");
	}

      m_engWindowTitle = "Modify";
    }
  else
    {
      ma.attach_files->setVisible(false);
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
      ma.view_pdf->setEnabled(true);
#endif
      ma.delete_files->setVisible(false);
      ma.export_files->setEnabled(true);
      ma.issnAvailableCheckBox->setCheckable(false);

      if(qmain->isGuest())
	ma.showUserButton->setVisible(false);
      else
	ma.showUserButton->setEnabled(true);

      ma.copiesButton->setVisible(false);
      ma.sruQueryButton->setVisible(false);
      ma.z3950QueryButton->setVisible(false);
      ma.okButton->setVisible(false);
      ma.resetButton->setVisible(false);
      ma.frontButton->setVisible(false);
      ma.backButton->setVisible(false);

      if(!ma.id->text().trimmed().isEmpty())
	{
	  if(m_subType == "Journal")
	    str = QString(tr("BiblioteQ: View Journal Details ("));
	  else
	    str = QString(tr("BiblioteQ: View Magazine Details ("));

	  str += ma.id->text().trimmed() + tr(")");
	}
      else
	{
	  if(m_subType == "Journal")
	    str = tr("BiblioteQ: View Journal Entry");
	  else
	    str = tr("BiblioteQ: View Magazine Entry");
	}

      m_engWindowTitle = "View";
    }

  ma.coverImages->setVisible(true);
  setReadOnlyFields(this, state != biblioteq::EDITABLE);
  setWindowTitle(str);
}
