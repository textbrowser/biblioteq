/*
** -- Qt Includes --
*/

#include <QSqlField>
#include <QSqlRecord>

/*
** Includes grey-literature-specific methods.
*/

/*
** -- Local Includes --
*/

#include "biblioteq.h"
#include "biblioteq_grey_literature.h"

extern biblioteq *qmain;

biblioteq_grey_literature::biblioteq_grey_literature(QMainWindow *parentArg,
						     const QString &oidArg,
						     const int rowArg):
  QMainWindow(), biblioteq_item(rowArg)
{
  QMenu *menu = 0;
  QString errorstr("");

  if((menu = new(std::nothrow) QMenu(this)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  m_isQueryEnabled = false;
  m_oid = oidArg;
  m_parentWid = parentArg;
  m_row = rowArg;
  m_ui.setupUi(this);
  m_ui.resetButton->setMenu(menu);
  connect(m_ui.cancelButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotCancel(void)));
  connect(m_ui.okButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotGo(void)));
  connect(m_ui.printButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotPrint(void)));
  connect(m_ui.resetButton,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Title")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &ID")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Date")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Author(s)")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Client(s)")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Code-A")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Code-B")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Job Number")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Notes")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Location")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Status")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Type")),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotReset(void)));
#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  setAttribute(Qt::WA_MacMetalStyle, BIBLIOTEQ_WA_MACMETALSTYLE);
#endif
#endif
  QApplication::setOverrideCursor(Qt::WaitCursor);
  m_ui.date->setDisplayFormat(qmain->publicationDateFormat("greyliterature"));
  m_ui.location->addItems
    (biblioteq_misc_functions::getLocations(qmain->getDB(),
					    "Grey Literature",
					    errorstr));
  qmain->addError
    (QString(tr("Database Error")),
     QString(tr("Unable to retrieve the grey literature locations.")),
     errorstr, __FILE__, __LINE__);
  m_ui.type->addItems
    (biblioteq_misc_functions::getGreyLiteratureTypes(qmain->getDB(),
						      errorstr));
  qmain->addError
    (QString(tr("Database Error")),
     QString(tr("Unable to retrieve the grey literature document types.")),
     errorstr, __FILE__, __LINE__);

  if(m_ui.location->findText(tr("UNKNOWN")) == -1)
    m_ui.location->addItem(tr("UNKNOWN"));

  if(m_ui.type->findText(tr("UNKNOWN")) == -1)
    m_ui.type->addItem(tr("UNKNOWN"));

  QApplication::restoreOverrideCursor();
  m_dt_orig_ss = m_ui.date->styleSheet();
  m_te_orig_pal = m_ui.author->viewport()->palette();
  updateFont(QApplication::font(), qobject_cast<QWidget *> (this));
  biblioteq_misc_functions::center(this, m_parentWid);
  biblioteq_misc_functions::hideAdminFields(this, qmain->getRoles());
}

biblioteq_grey_literature::~biblioteq_grey_literature()
{
}

bool biblioteq_grey_literature::validateWidgets(void)
{
  QString error("");

  m_ui.title->setText(m_ui.title->text().trimmed());

  if(m_ui.title->text().isEmpty())
    {
      error = tr("Please complete the Title field.");
      m_ui.title->setFocus();
      goto done_label;
    }

  m_ui.id->setText(m_ui.id->text().trimmed());

  if(m_ui.id->text().isEmpty())
    {
      error = tr("Please complete the ID field.");
      m_ui.id->setFocus();
      goto done_label;
    }

  m_ui.author->setPlainText(m_ui.author->toPlainText().trimmed());

  if(m_ui.author->toPlainText().isEmpty())
    {
      error = tr("Please complete the Author(s) field.");
      m_ui.author->setFocus();
      goto done_label;
    }

  m_ui.code_a->setText(m_ui.code_a->text().trimmed());

  if(m_ui.code_a->text().isEmpty())
    {
      error = tr("Please complete the Code-A field.");
      m_ui.code_a->setFocus();
      goto done_label;
    }

  m_ui.code_b->setText(m_ui.code_b->text().trimmed());

  if(m_ui.code_b->text().isEmpty())
    {
      error = tr("Please complete the Code-B field.");
      m_ui.code_b->setFocus();
      goto done_label;
    }

  m_ui.job_number->setText(m_ui.job_number->text().trimmed());

  if(m_ui.job_number->text().isEmpty())
    {
      error = tr("Please complete the Job Number field.");
      m_ui.job_number->setFocus();
      goto done_label;
    }

  m_ui.client->setPlainText(m_ui.client->toPlainText().trimmed());
  m_ui.notes->setPlainText(m_ui.notes->toPlainText().trimmed());
  m_ui.status->setText(m_ui.status->text().trimmed());
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!qmain->getDB().transaction())
    {
      QApplication::restoreOverrideCursor();
      qmain->addError
	(QString(tr("Database Error")),
	 QString(tr("Unable to create a database transaction.")),
	 qmain->getDB().lastError().text(),
	 __FILE__,
	 __LINE__);
      QMessageBox::critical
	(this,
	 tr("BiblioteQ: Database Error"),
	 tr("Unable to create a database transaction."));
      return false;
    }

  QApplication::restoreOverrideCursor();

 done_label:

  if(!error.isEmpty())
    {
      QMessageBox::critical(this, tr("BiblioteQ: User Error"), error);
      return false;
    }

  return true;
}

void biblioteq_grey_literature::changeEvent(QEvent *event)
{
  if(event)
    switch(event->type())
      {
      case QEvent::LanguageChange:
	{
	  m_ui.retranslateUi(this);
	  break;
	}
      default:
	break;
      }

  QMainWindow::changeEvent(event);
}

void biblioteq_grey_literature::closeEvent(QCloseEvent *e)
{
  if(m_engWindowTitle.contains("Create") ||
     m_engWindowTitle.contains("Modify"))
    if(hasDataChanged(this))
      if(QMessageBox::
	 question(this,
		  tr("BiblioteQ: Question"),
		  tr("Your changes have not been saved. Continue closing?"),
		  QMessageBox::Yes | QMessageBox::No,
		  QMessageBox::No) == QMessageBox::No)
	{
	  if(e)
	    e->ignore();

	  return;
	}

  qmain->removeGreyLiterature(this);
}

void biblioteq_grey_literature::duplicate(const QString &p_oid, const int state)
{
  modify(state); // Initial population.
  m_engWindowTitle = "Create";
  m_oid = p_oid;
  setWindowTitle(tr("BiblioteQ: Duplicate Grey Literature Entry"));
}

void biblioteq_grey_literature::highlightRequiredWidgets(void)
{
  biblioteq_misc_functions::highlightWidget
    (m_ui.author->viewport(), QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget(m_ui.code_a, QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget(m_ui.code_b, QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget(m_ui.id, QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (m_ui.job_number, QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget(m_ui.title, QColor(255, 248, 220));
}

void biblioteq_grey_literature::insert(void)
{
  m_engWindowTitle = "Create";
  m_te_orig_pal = m_ui.id->palette();
  m_ui.attach_files->setEnabled(false);
  m_ui.attach_images->setEnabled(false);
  m_ui.author->setPlainText("N/A");
  m_ui.client->setPlainText("N/A");
  m_ui.code_a->clear();
  m_ui.code_b->clear();
  m_ui.date->setDate(QDate::fromString("01/01/2000", "MM/dd/yyyy"));
  m_ui.delete_files->setEnabled(false);
  m_ui.delete_images->setEnabled(false);
  m_ui.export_files->setEnabled(false);
  m_ui.export_images->setEnabled(false);
  m_ui.id->clear();
  m_ui.job_number->clear();
  m_ui.location->setCurrentIndex(0);
  m_ui.notes->clear();
  m_ui.okButton->setText(tr("&Save"));
  m_ui.status->clear();
  m_ui.title->clear();
  m_ui.type->setCurrentIndex(0);
  slotReset();
  highlightRequiredWidgets();
  setWindowTitle(tr("BiblioteQ: Create Grey Literature Entry"));
  storeData(this);
  showNormal();
  activateWindow();
  raise();
}

void biblioteq_grey_literature::insertDatabase(void)
{
  QSqlQuery query(qmain->getDB());

  if(qmain->getDB().driverName() != "QSQLITE")
    query.prepare
      ("INSERT INTO grey_literature "
       "(author, client, document_code_a, document_code_b, "
       "document_date, document_id, document_status, document_title, "
       "document_type, job_number, location, notes) "
       "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
       "RETURNING myoid");
  else
    query.prepare
      ("INSERT INTO grey_literature "
       "(author, client, document_code_a, document_code_b, "
       "document_date, document_id, document_status, document_title, "
       "document_type, job_number, location, notes, "
       "myoid) "
       "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

  query.addBindValue(m_ui.author->toPlainText());

  if(m_ui.client->toPlainText().isEmpty())
    query.addBindValue(QVariant::String);
  else
    query.addBindValue(m_ui.client->toPlainText());

  query.addBindValue(m_ui.code_a->text());
  query.addBindValue(m_ui.code_b->text());
  query.addBindValue(m_ui.date->date().toString("MM/dd/yyyy"));
  query.addBindValue(m_ui.id->text());
  query.addBindValue(m_ui.status->text());
  query.addBindValue(m_ui.title->text());
  query.addBindValue(m_ui.type->currentText());
  query.addBindValue(m_ui.job_number->text());
  query.addBindValue(m_ui.location->currentText());

  if(m_ui.notes->toPlainText().isEmpty())
    query.addBindValue(QVariant::String);
  else
    query.addBindValue(m_ui.notes->toPlainText());

  if(qmain->getDB().driverName() == "QSQLITE")
    {
      QString errorstr("");
      qint64 value = biblioteq_misc_functions::getSqliteUniqueId
	(qmain->getDB(), errorstr);

      if(errorstr.isEmpty())
	{
	  m_oid = QString::number(value);
	  query.addBindValue(value);
	}
      else
	qmain->addError(QString(tr("Database Error")),
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

  if(qmain->getDB().driverName() != "QSQLITE")
    {
      query.next();
      m_oid = query.value(0).toString();
    }

  if(!qmain->getDB().commit())
    {
      QApplication::restoreOverrideCursor();
      qmain->addError
	(QString(tr("Database Error")),
	 QString(tr("Unable to commit the current database transaction.")),
	 qmain->getDB().lastError().text(),
	 __FILE__,
	 __LINE__);
      goto db_rollback;
    }

  m_ui.author->setMultipleLinks
    ("greyliterature_search", "author", m_ui.author->toPlainText());
  m_ui.client->setMultipleLinks
    ("greyliterature_search", "client", m_ui.client->toPlainText());
  m_ui.notes->setMultipleLinks
    ("greyliterature_search", "notes", m_ui.notes->toPlainText());
  QApplication::restoreOverrideCursor();
  qmain->replaceGreyLiterature(m_oid, this);
  updateWindow(biblioteq::EDITABLE);

  if(qmain->getUI().actionAutoPopulateOnCreation->isChecked())
    qmain->populateTable
      (biblioteq::POPULATE_ALL, "Grey Literature", QString(""));

  raise();
  storeData(this);
  return;

 db_rollback:

  QApplication::setOverrideCursor(Qt::WaitCursor);
  m_oid.clear();

  if(!qmain->getDB().rollback())
    qmain->addError(QString(tr("Database Error")),
		    QString(tr("Rollback failure.")),
		    qmain->getDB().lastError().text(),
		    __FILE__,
		    __LINE__);

  QApplication::restoreOverrideCursor();
  QMessageBox::critical(this,
			tr("BiblioteQ: Database Error"),
			tr("Unable to create the entry. Please verify that "
			   "the entry does not already exist."));
}

void biblioteq_grey_literature::modify(const int state)
{
  if(state == biblioteq::EDITABLE)
    {
      setWindowTitle(tr("BiblioteQ: Modify Grey Literature Entry"));
      m_engWindowTitle = "Modify";
      m_te_orig_pal = m_ui.id->palette();
      m_ui.okButton->setVisible(true);
    }
  else
    {
      setWindowTitle(tr("BiblioteQ: View Grey Literature Details"));
      m_engWindowTitle = "View";
      m_ui.okButton->setVisible(false);
    }

  m_ui.okButton->setText("&Save");
  raise();
}

void biblioteq_grey_literature::search(const QString &field,
				       const QString &value)
{
  m_ui.attach_files->setVisible(false);
  m_ui.attach_images->setVisible(false);
  m_ui.date->setDate(QDate::fromString("2001", "yyyy"));
  m_ui.delete_files->setVisible(false);
  m_ui.delete_images->setVisible(false);
  m_ui.export_files->setVisible(false);
  m_ui.export_images->setVisible(false);
  m_ui.files->setVisible(false);
  m_ui.files_label->setVisible(false);
  m_ui.images->setVisible(false);
  m_ui.images_label->setVisible(false);
  m_ui.okButton->setText(tr("&Search"));

  if(field.isEmpty() && value.isEmpty())
    {
      m_engWindowTitle = "Search";
      m_ui.title->setFocus();
      setWindowTitle(tr("BiblioteQ: Database Grey Literature Search"));
      biblioteq_misc_functions::center(this, m_parentWid);
      showNormal();
      activateWindow();
      raise();
    }
  else
    {
      if(field == "author")
	m_ui.author->setPlainText(value);
      else if(field == "client")
	m_ui.client->setPlainText(value);
      else if(field == "notes")
	m_ui.notes->setPlainText(value);

      slotGo();
    }
}

void biblioteq_grey_literature::slotCancel(void)
{
  close();
}

void biblioteq_grey_literature::slotGo(void)
{
  if(m_engWindowTitle.contains("Create"))
    {
      if(validateWidgets())
	insertDatabase();
    }
  else if(m_engWindowTitle.contains("Modify"))
    {
      if(validateWidgets())
	{
	  if(qmain->getDB().driverName() != "QSQLITE")
	    updatePostgresql();
	  else
	    updateSqlite();
	}
    }
  else if(m_engWindowTitle.contains("Search"))
    {
    }
}

void biblioteq_grey_literature::slotPrint(void)
{
  m_html = "<html>";

  QStringList titles;
  QStringList values;

  titles << tr("Title:")
	 << tr("ID:")
	 << tr("Date:")
	 << tr("Author(s):")
	 << tr("Client(s):")
	 << tr("Code-A:")
	 << tr("Code-B:")
	 << tr("Job Number:")
	 << tr("Notes:")
	 << tr("Location:")
	 << tr("Status:")
	 << tr("Type:");
  values << m_ui.title->text().trimmed()
	 << m_ui.id->text().trimmed()
	 << m_ui.date->date().toString(Qt::ISODate)
	 << m_ui.author->toPlainText().trimmed()
	 << m_ui.client->toPlainText().trimmed()
	 << m_ui.code_a->text().trimmed()
	 << m_ui.code_b->text().trimmed()
	 << m_ui.job_number->text().trimmed()
	 << m_ui.notes->toPlainText().trimmed()
	 << m_ui.location->currentText().trimmed()
	 << m_ui.status->text().trimmed()
	 << m_ui.type->currentText().trimmed();

  for(int i = 0; i < titles.size(); i++)
    {
      m_html += "<b>" + titles.at(i) + "</b>" + values.at(i);

      if(i != titles.size() - 1)
	m_html += "<br>";
    }

  m_html += "</html>";
  print(this);
}

void biblioteq_grey_literature::slotPublicationDateEnabled(bool state)
{
  Q_UNUSED(state);
}

void biblioteq_grey_literature::slotQuery(void)
{
}

void biblioteq_grey_literature::slotReset(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  if(action != 0)
    {
      QList<QAction *> actions = m_ui.resetButton->menu()->actions();

      if(actions.size() < 12)
	{
	  // Error.
	}
      else if(action == actions[0])
	{
	  m_ui.title->clear();
	  m_ui.title->setPalette(m_te_orig_pal);
	  m_ui.title->setFocus();
	}
      else if(action == actions[1])
	{
	  m_ui.id->clear();
	  m_ui.id->setPalette(m_te_orig_pal);
	  m_ui.id->setFocus();
	}
      else if(action == actions[2])
	{
	  if(m_engWindowTitle.contains("Search"))
	    m_ui.date->setDate(QDate::fromString("2001", "yyyy"));
	  else
	    m_ui.date->setDate(QDate::fromString("01/01/2000", "MM/dd/yyyy"));

	  m_ui.date->setFocus();
	  m_ui.date->setStyleSheet(m_dt_orig_ss);
	}
      else if(action == actions[3])
	{
	  if(!m_engWindowTitle.contains("Search"))
	    m_ui.author->setPlainText("N/A");
	  else
	    m_ui.author->clear();

	  m_ui.author->setFocus();
	  m_ui.author->viewport()->setPalette(m_te_orig_pal);
	}
      else if(action == actions[4])
	{
	  m_ui.client->clear();
	  m_ui.client->setFocus();
	}
      else if(action == actions[5])
	{
	  if(!m_engWindowTitle.contains("Search"))
	    m_ui.code_a->setText("N/A");
	  else
	    m_ui.code_a->clear();

	  m_ui.code_a->setFocus();
	  m_ui.code_a->setPalette(m_te_orig_pal);
	}
      else if(action == actions[6])
	{
	  if(!m_engWindowTitle.contains("Search"))
	    m_ui.code_b->setText("N/A");
	  else
	    m_ui.code_b->clear();

	  m_ui.code_b->setFocus();
	  m_ui.code_b->setPalette(m_te_orig_pal);
	}
      else if(action == actions[7])
	{
	  if(!m_engWindowTitle.contains("Search"))
	    m_ui.job_number->setText("N/A");
	  else
	    m_ui.job_number->clear();

	  m_ui.job_number->setFocus();
	  m_ui.job_number->setPalette(m_te_orig_pal);
	}
      else if(action == actions[8])
	{
	  m_ui.notes->clear();
	  m_ui.notes->setFocus();
	}
      else if(action == actions[9])
	{
	  m_ui.location->setCurrentIndex(0);
	  m_ui.location->setFocus();
	}
      else if(action == actions[10])
	{
	  m_ui.status->clear();
	  m_ui.status->setFocus();
	}
      else if(action == actions[11])
	{
	  m_ui.type->setCurrentIndex(0);
	  m_ui.type->setFocus();
	}
    }
  else
    {
      /*
      ** Reset all.
      */

      if(!m_engWindowTitle.contains("Search"))
	m_ui.author->setPlainText("N/A");
      else
	m_ui.author->clear();

      m_ui.client->clear();

      if(!m_engWindowTitle.contains("Search"))
	m_ui.code_a->setText("N/A");
      else
	m_ui.code_a->clear();

      if(!m_engWindowTitle.contains("Search"))
	m_ui.code_b->setText("N/A");
      else
	m_ui.code_b->clear();

      if(m_engWindowTitle.contains("Search"))
	m_ui.date->setDate(QDate::fromString("2001", "yyyy"));
      else
	m_ui.date->setDate(QDate::fromString("01/01/2000", "MM/dd/yyyy"));

      m_ui.id->clear();

      if(!m_engWindowTitle.contains("Search"))
	m_ui.job_number->setText("N/A");
      else
	m_ui.job_number->clear();

      m_ui.location->setCurrentIndex(0);
      m_ui.notes->clear();
      m_ui.status->clear();
      m_ui.title->clear();
      m_ui.title->setFocus();
      m_ui.type->setCurrentIndex(0);
    }
}

void biblioteq_grey_literature::slotSelectImage(void)
{
}

void biblioteq_grey_literature::updatePostgresql(void)
{
}

void biblioteq_grey_literature::updateSqlite(void)
{
}

void biblioteq_grey_literature::updateWindow(const int state)
{
  QString str("");

  if(state == biblioteq::EDITABLE)
    {
      str = QString(tr("BiblioteQ: Modify Grey Literature Entry (")) +
	m_ui.id->text() + tr(")");
      m_engWindowTitle = "Modify";
      m_ui.attach_files->setEnabled(true);
      m_ui.attach_images->setEnabled(true);
      m_ui.delete_files->setEnabled(true);
      m_ui.delete_images->setEnabled(true);
      m_ui.export_files->setEnabled(true);
      m_ui.export_images->setEnabled(true);
      m_ui.okButton->setVisible(true);
    }
  else
    {
      str = QString(tr("BiblioteQ: View Grey Literature Details (")) +
	m_ui.id->text() + tr(")");
      m_engWindowTitle = "View";
      m_ui.attach_files->setEnabled(false);
      m_ui.attach_images->setEnabled(false);
      m_ui.delete_files->setEnabled(false);
      m_ui.delete_images->setEnabled(false);
      m_ui.export_files->setEnabled(false);
      m_ui.export_images->setEnabled(false);
      m_ui.okButton->setVisible(false);
    }

  setWindowTitle(str);
}
