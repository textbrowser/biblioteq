/*
** -- Qt Includes --
*/

#include <QAuthenticator>
#include <QCryptographicHash>
#include <QNetworkProxy>
#include <QSqlField>
#include <QSqlRecord>
#include <QXmlStreamReader>

/*
** Includes book-specific methods.
*/

/*
** -- Local Includes --
*/

#include "biblioteq.h"
#include "biblioteq_book.h"
#include "biblioteq_borrowers_editor.h"
#include "biblioteq_marc.h"

extern biblioteq *qmain;

/*
** -- biblioteq_book() --
*/

biblioteq_book::biblioteq_book(QMainWindow *parentArg,
			       const QString &oidArg,
			       const int rowArg):
  QMainWindow(), biblioteq_item(rowArg)
{
  QGraphicsScene *scene1 = 0;
  QGraphicsScene *scene2 = 0;
  QMenu *menu = 0;

  if((menu = new(std::nothrow) QMenu(this)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene1 = new(std::nothrow) QGraphicsScene(this)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene2 = new(std::nothrow) QGraphicsScene(this)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  if((m_httpProgress = new(std::nothrow)
      biblioteq_item_working_dialog(qobject_cast<QMainWindow *> (this))) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  m_httpProgress->reset(); // Qt 5.5.x adjustment.
  m_httpProgress->setWindowTitle(tr("BiblioteQ: Image Download"));

  if(useHttp())
    {
#if QT_VERSION < 0x050000
      if((m_imageHttp = new(std::nothrow) QHttp(this)) == 0)
	biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);
#endif
    }
  else
    {
      if((m_imageManager = new(std::nothrow) QNetworkAccessManager(this)) == 0)
	biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);
    }

  if((m_proxyDialog = new(std::nothrow) QDialog(this)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  if(useHttp())
    {
#if QT_VERSION < 0x050000
      if((m_sruHttp = new(std::nothrow) QHttp(this)) == 0)
	biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);
#endif
    }
  else
    {
      if((m_sruManager = new(std::nothrow)
	  QNetworkAccessManager(this)) == 0)
	biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);
    }

  if((m_sruWorking = new(std::nothrow)
      biblioteq_item_working_dialog(qobject_cast<QMainWindow *> (this))) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  m_sruWorking->reset(); // Qt 5.5.x adjustment.
  m_sruWorking->setModal(true);
  m_sruWorking->setWindowTitle(tr("BiblioteQ: SRU Data Retrieval"));
  m_sruWorking->setLabelText(tr("Downloading information from the SRU "
				"site. Please be patient..."));
  m_sruWorking->setMaximum(0);
  m_sruWorking->setMinimum(0);
  m_sruWorking->setCancelButton(0);
  ui_p.setupUi(m_proxyDialog);
#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  m_proxyDialog->setAttribute
    (Qt::WA_MacMetalStyle, BIBLIOTEQ_WA_MACMETALSTYLE);
#endif
#endif
  m_parentWid = parentArg;
  m_oid = oidArg;
  m_row = rowArg;
  m_oldq = biblioteq_misc_functions::getColumnString
    (qmain->getUI().table, m_row,
     qmain->getUI().table->columnNumber("Quantity")).toInt();
  id.setupUi(this);
  id.files->setColumnHidden(id.files->columnCount() - 1, true); // myoid
#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  setAttribute(Qt::WA_MacMetalStyle, BIBLIOTEQ_WA_MACMETALSTYLE);
#endif
#endif
  updateFont(QApplication::font(), qobject_cast<QWidget *> (this));
  connect(id.attach_files,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAttachFiles(void)));
  connect(id.okButton, SIGNAL(clicked(void)), this, SLOT(slotGo(void)));
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
  connect(id.printButton, SIGNAL(clicked(void)), this, SLOT(slotPrint(void)));
  connect(id.isbnAvailableCheckBox,
	  SIGNAL(toggled(bool)),
	  id.dwnldFront,
	  SLOT(setEnabled(bool)));
  connect(id.isbnAvailableCheckBox,
	  SIGNAL(toggled(bool)),
	  id.dwnldBack,
	  SLOT(setEnabled(bool)));
  connect(id.isbnAvailableCheckBox,
	  SIGNAL(toggled(bool)),
	  id.sruQueryButton,
	  SLOT(setEnabled(bool)));
  connect(id.isbnAvailableCheckBox,
	  SIGNAL(toggled(bool)),
	  id.z3950QueryButton,
	  SLOT(setEnabled(bool)));
  connect(menu->addAction(tr("Reset &Front Cover Image")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Back Cover Image")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &ISBN-10")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &ISBN-13")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Edition")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Author(s)")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Book Binding Type")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &LC Control Number")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Call Number")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Dewey Class Number")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Title")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Publication Date")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Publisher")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Place of Publication")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Categories")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Price")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Language")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Monetary Units")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Copies")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Location")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Originality")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Condition")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Abstract")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &MARC Tags")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Keywords")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(id.frontButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  connect(id.backButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  connect(id.dwnldFront, SIGNAL(clicked(void)), this,
	  SLOT(slotDownloadImage(void)));
  connect(id.dwnldBack, SIGNAL(clicked(void)), this,
	  SLOT(slotDownloadImage(void)));
  connect(m_httpProgress, SIGNAL(canceled(void)), this,
	  SLOT(slotCancelImageDownload(void)));
  connect(m_httpProgress, SIGNAL(rejected(void)), this,
	  SLOT(slotCancelImageDownload(void)));
  id.resetButton->setMenu(menu);

  if(menu->actions().size() >= 4)
    {
      connect(id.isbnAvailableCheckBox,
	      SIGNAL(toggled(bool)),
	      menu->actions()[2],
	      SLOT(setEnabled(bool)));
      connect(id.isbnAvailableCheckBox,
	      SIGNAL(toggled(bool)),
	      menu->actions()[3],
	      SLOT(setEnabled(bool)));
    }

  QString errorstr("");

  QApplication::setOverrideCursor(Qt::WaitCursor);
  id.binding->addItems
    (biblioteq_misc_functions::getBookBindingTypes(qmain->getDB(),
						   errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the book binding types.")),
       errorstr, __FILE__, __LINE__);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  id.language->addItems
    (biblioteq_misc_functions::getLanguages(qmain->getDB(),
					    errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the languages.")),
       errorstr, __FILE__, __LINE__);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  id.monetary_units->addItems
    (biblioteq_misc_functions::getMonetaryUnits(qmain->getDB(),
						errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the monetary units.")),
       errorstr, __FILE__, __LINE__);

  QApplication::setOverrideCursor(Qt::WaitCursor);
  id.location->addItems
    (biblioteq_misc_functions::getLocations(qmain->getDB(),
					    "Book",
					    errorstr));
  QApplication::restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the book locations.")),
       errorstr, __FILE__, __LINE__);

  id.front_image->setScene(scene1);
  id.back_image->setScene(scene2);
  m_httpProgress->setModal(true);

  if(id.binding->findText(tr("UNKNOWN")) == -1)
    id.binding->addItem(tr("UNKNOWN"));

  if(id.language->findText(tr("UNKNOWN")) == -1)
    id.language->addItem(tr("UNKNOWN"));

  if(id.monetary_units->findText(tr("UNKNOWN")) == -1)
    id.monetary_units->addItem(tr("UNKNOWN"));

  if(id.location->findText(tr("UNKNOWN")) == -1)
    id.location->addItem(tr("UNKNOWN"));

  QActionGroup *actionGroup1 = 0;
  QActionGroup *actionGroup2 = 0;

  if((actionGroup1 = new(std::nothrow) QActionGroup(this)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  if((actionGroup2 = new(std::nothrow) QActionGroup(this)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  QMap<QString, QHash<QString, QString> > hashes(qmain->getSRUMaps());
  bool found = false;

  for(int i = 0; i < hashes.size(); i++)
    {
      QAction *action = actionGroup1->addAction(hashes.keys().at(i));

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
    id.sruQueryButton->setPopupMode(QToolButton::DelayedPopup);
  else if(!found)
    id.sruQueryButton->actions()[0]->setChecked(true);

  hashes.clear();
  found = false;
  hashes = qmain->getZ3950Maps();

  for(int i = 0; i < hashes.size(); i++)
    {
      QAction *action = actionGroup2->addAction(hashes.keys().at(i));

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
    id.z3950QueryButton->setPopupMode(QToolButton::DelayedPopup);
  else if(!found)
    id.z3950QueryButton->actions()[0]->setChecked(true);

  hashes.clear();

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

  if(m_parentWid)
    resize(qRound(0.95 * m_parentWid->size().width()),
	   qRound(0.95 * m_parentWid->size().height()));

  id.splitter->setStretchFactor(0, 0);
  id.splitter->setStretchFactor(1, 1);
  id.splitter->setStretchFactor(2, 0);
  biblioteq_misc_functions::center(this, m_parentWid);
  biblioteq_misc_functions::hideAdminFields(this, qmain->getRoles());
}

/*
** -- ~biblioteq_book() --
*/

biblioteq_book::~biblioteq_book()
{
}

/*
** -- slotGo() --
*/

void biblioteq_book::slotGo(void)
{
  QSqlQuery query(qmain->getDB());
  QString errorstr = "";
  QString searchstr = "";
  QString str = "";
  int i = 0;
  int maxcopynumber = 0;
  int newq = 0;

  if(m_engWindowTitle.contains("Create") ||
     m_engWindowTitle.contains("Modify"))
    {
      if(m_engWindowTitle.contains("Modify") && m_row > -1)
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
	      id.quantity->setValue(m_oldq);
	      return;
	    }
	  else if(newq > m_oldq)
	    if(QMessageBox::question
	       (this, tr("BiblioteQ: Question"),
		tr("You have increased the number of copies. "
		   "Would you like to modify copy information?"),
		QMessageBox::Yes | QMessageBox::No,
		QMessageBox::No) == QMessageBox::Yes)
	      slotPopulateCopiesEditor();
	}

      str = id.id->text().trimmed();
      id.id->setText(str);
      str = id.isbn13->text().trimmed();
      id.isbn13->setText(str);

      if(id.isbnAvailableCheckBox->isChecked())
	{
	  if(id.id->text().length() == 10)
	    slotConvertISBN10to13();
	}
      else
	id.id->clear();

      if(id.isbnAvailableCheckBox->isChecked())
	{
	  if(id.isbn13->text().length() == 13)
	    slotConvertISBN13to10();
	}
      else
	id.isbn13->clear();

      if(id.isbnAvailableCheckBox->isChecked())
	if(id.id->text().length() != 10 ||
	   id.isbn13->text().length() != 13)
	  {
	    QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				  tr("Please complete both the "
				     "ISBN-10 and ISBN-13 fields."));

	    if(id.id->text().length() != 10)
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
				tr("Please complete the Author(s) field."));
	  id.author->setFocus();
	  return;
	}

      str = id.title->text().trimmed();
      id.title->setText(str);

      if(id.title->text().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Title field."));
	  id.title->setFocus();
	  return;
	}

      str = id.publisher->toPlainText().trimmed();
      id.publisher->setPlainText(str);

      if(id.publisher->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Publisher field."));
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
	  id.place->setFocus();
	  return;
	}

      str = id.category->toPlainText().trimmed();
      id.category->setPlainText(str);

      if(id.category->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Categories field."));
	  id.category->setFocus();
	  return;
	}

      str = id.description->toPlainText().trimmed();
      id.description->setPlainText(str);

      if(id.description->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Abstract field."));
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
		      "condition = ? "
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
		      "place, marc_tags, keyword, originality, condition) "
		      "VALUES (?, ?, ?, ?, ?, "
		      "?, ?, ?, "
		      "?, ?, ?, "
		      "?, ?, ?, "
		      "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) RETURNING myoid");
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
		      "myoid) "
		      "VALUES (?, ?, ?, ?, "
		      "?, ?, ?, ?, ?, "
		      "?, ?, "
		      "?, ?, ?, "
		      "?, ?, ?, ?, ?, ?, ?, ?, ?, "
		      "?, ?, ?)");

      if(id.isbnAvailableCheckBox->isChecked() &&
	 !id.id->text().isEmpty())
	query.bindValue(0, id.id->text());
      else
	query.bindValue(0, QVariant(QVariant::String));

      query.bindValue(1, id.title->text().trimmed());
      query.bindValue(2, id.edition->currentText().trimmed());
      query.bindValue(3, id.author->toPlainText().trimmed());
      query.bindValue(4, id.publication_date->date().toString("MM/dd/yyyy"));
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
	 !id.isbn13->text().isEmpty())
	query.bindValue(14, id.isbn13->text());
      else
	query.bindValue(14, QVariant(QVariant::String));

      if(!id.lcnum->text().isEmpty())
	query.bindValue(15, id.lcnum->text());
      else
	query.bindValue(15, QVariant(QVariant::String));

      if(!id.callnum->text().isEmpty())
	query.bindValue(16, id.callnum->text());
      else
	query.bindValue(16, QVariant(QVariant::String));

      if(!id.deweynum->text().isEmpty())
	query.bindValue(17, id.deweynum->text());
      else
	query.bindValue(17, QVariant(QVariant::String));

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
	    query.bindValue(18, QVariant(QVariant::ByteArray));
	}
      else
	{
	  id.front_image->m_imageFormat = "";
	  query.bindValue(18, QVariant(QVariant::ByteArray));
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
	    query.bindValue(19, QVariant(QVariant::ByteArray));
	}
      else
	{
	  id.back_image->m_imageFormat = "";
	  query.bindValue(19, QVariant(QVariant::ByteArray));
	}

      query.bindValue(20, id.place->toPlainText().trimmed());
      query.bindValue(21, id.marc_tags->toPlainText().trimmed());
      query.bindValue(22, id.keyword->toPlainText().trimmed());
      query.bindValue(23, id.originality->currentText().trimmed());
      query.bindValue(24, id.condition->currentText().trimmed());

      if(m_engWindowTitle.contains("Modify"))
	query.bindValue(25, m_oid);
      else if(qmain->getDB().driverName() == "QSQLITE")
	{
	  qint64 value = biblioteq_misc_functions::getSqliteUniqueId
	    (qmain->getDB(), errorstr);

	  if(errorstr.isEmpty())
	    {
	      query.bindValue(25, value);
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
	      query.prepare("DELETE FROM book_copy_info WHERE "
			    "copy_number > ? AND "
			    "item_oid = "
			    "?");
	      query.bindValue(0, id.quantity->text());
	      query.bindValue(1, m_oid);

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
		  m_oid = query.value(0).toString();
		}

	      if(id.id->text().isEmpty())
		biblioteq_misc_functions::
		  createInitialCopies(m_oid,
				      id.quantity->value(),
				      qmain->getDB(),
				      "Book", errorstr);
	      else
		biblioteq_misc_functions::
		  createInitialCopies(id.id->text(),
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

	  id.id->setPalette(m_te_orig_pal);
	  id.isbn13->setPalette(m_te_orig_pal);
	  id.edition->setStyleSheet(m_cb_orig_ss);
	  id.binding->setStyleSheet(m_cb_orig_ss);
	  id.category->viewport()->setPalette(m_te_orig_pal);
	  id.publication_date->setStyleSheet(m_dt_orig_ss);
	  id.author->viewport()->setPalette(m_te_orig_pal);
	  id.title->setPalette(m_te_orig_pal);
	  id.description->viewport()->setPalette(m_te_orig_pal);
	  id.marc_tags->viewport()->setPalette(m_white_pal);
	  id.keyword->viewport()->setPalette(m_white_pal);
	  id.publisher->viewport()->setPalette(m_te_orig_pal);
	  id.place->viewport()->setPalette(m_te_orig_pal);
	  id.lcnum->setPalette(m_white_pal);
	  id.callnum->setPalette(m_white_pal);
	  id.deweynum->setPalette(m_white_pal);
	  m_oldq = id.quantity->value();

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
	      if(!id.id->text().isEmpty())
		str = QString(tr("BiblioteQ: Modify Book Entry (")) +
		  id.id->text() + tr(")");
	      else
		str = tr("BiblioteQ: Modify Book Entry");

	      m_engWindowTitle = "Modify";
	      setWindowTitle(str);

	      if((qmain->getTypeFilterString() == "All" ||
		  qmain->getTypeFilterString() == "All Available" ||
		  qmain->getTypeFilterString() == "All Overdue" ||
		  qmain->getTypeFilterString() == "All Requested" ||
		  qmain->getTypeFilterString() == "All Reserved" ||
		  qmain->getTypeFilterString() == "Books") &&
		 m_oid == biblioteq_misc_functions::getColumnString
		 (qmain->getUI().table,
		  m_row, qmain->getUI().table->columnNumber("MYOID")) &&
		 biblioteq_misc_functions::getColumnString
		 (qmain->getUI().table,
		  m_row, qmain->getUI().table->columnNumber("Type")) == "Book")
		{
		  qmain->getUI().table->setSortingEnabled(false);

		  QStringList names(qmain->getUI().table->columnNames());

		  for(i = 0; i < names.size(); i++)
		    {
		      if(names.at(i) == "ISBN-10" ||
			 names.at(i) == "ID Number")
			qmain->getUI().table->item(m_row, i)->setText
			  (id.id->text());
		      else if(names.at(i) == "Title")
			qmain->getUI().table->item(m_row, i)->setText
			  (id.title->text());
		      else if(names.at(i) == "Edition")
			qmain->getUI().table->item(m_row, i)->setText
			  (id.edition->currentText().trimmed());
		      else if(names.at(i) == "Author(s)")
			qmain->getUI().table->item(m_row, i)->setText
			  (id.author->toPlainText());
		      else if(names.at(i) == "Publication Date")
			qmain->getUI().table->item(m_row, i)->setText
			  (id.publication_date->date().toString
			   (Qt::ISODate));
		      else if(names.at(i) == "Publisher")
			qmain->getUI().table->item(m_row, i)->setText
			  (id.publisher->toPlainText());
		      else if(names.at(i) == "Place of Publication")
			qmain->getUI().table->item(m_row, i)->setText
			  (id.place->toPlainText());
		      else if(names.at(i) == "Categories")
			qmain->getUI().table->item(m_row, i)->setText
			  (id.category->toPlainText());
		      else if(names.at(i) == "Price")
			qmain->getUI().table->item(m_row, i)->setText
			  (id.price->cleanText());
		      else if(names.at(i) == "Language")
			qmain->getUI().table->item(m_row, i)->setText
			  (id.language->currentText().trimmed());
		      else if(names.at(i) == "Monetary Units")
			qmain->getUI().table->item(m_row, i)->setText
			  (id.monetary_units->currentText().trimmed());
		      else if(names.at(i) == "Quantity")
			qmain->getUI().table->item(m_row, i)->setText
			  (id.quantity->text());
		      else if(names.at(i) == "Book Binding Type")
			qmain->getUI().table->item(m_row, i)->setText
			  (id.binding->currentText().trimmed());
		      else if(names.at(i) == "Location")
			qmain->getUI().table->item(m_row, i)->setText
			  (id.location->currentText().trimmed());
		      else if(names.at(i) == "ISBN-13")
			qmain->getUI().table->item(m_row, i)->setText
			  (id.isbn13->text());
		      else if(names.at(i) == "LC Control Number")
			qmain->getUI().table->item(m_row, i)->setText
			  (id.lcnum->text());
		      else if(names.at(i) == "Call Number")
			qmain->getUI().table->item(m_row, i)->setText
			  (id.callnum->text());
		      else if(names.at(i) == "Dewey Class Number")
			qmain->getUI().table->item(m_row, i)->setText
			  (id.deweynum->text());
		      else if(names.at(i) == "Originality")
			qmain->getUI().table->item(m_row, i)->setText
			  (id.originality->currentText().trimmed());
		      else if(names.at(i) == "Condition")
			qmain->getUI().table->item(m_row, i)->setText
			  (id.condition->currentText().trimmed());
		      else if(names.at(i) == "Availability")
			{
			  qmain->getUI().table->item(m_row, i)->setText
			    (biblioteq_misc_functions::getAvailability
			     (m_oid, qmain->getDB(), "Book",
			      errorstr));

			  if(!errorstr.isEmpty())
			    qmain->addError
			      (QString(tr("Database Error")),
			       QString(tr("Retrieving availability.")),
			       errorstr, __FILE__, __LINE__);
			}
		    }

		  qmain->getUI().table->setSortingEnabled(true);

		  foreach(QLineEdit *textfield, findChildren<QLineEdit *>())
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
		qmain->populateTable
		  (biblioteq::POPULATE_ALL, "Books", QString(""));

	      raise();
	    }

	  storeData(this);
	}

      return;

    db_rollback:

      QApplication::setOverrideCursor(Qt::WaitCursor);

      if(!qmain->getDB().rollback())
	qmain->addError
	  (QString(tr("Database Error")), QString(tr("Rollback failure.")),
	   qmain->getDB().lastError().text(), __FILE__, __LINE__);

      QApplication::restoreOverrideCursor();
      QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			    tr("Unable to create or update the entry. "
			       "Please verify that "
			       "the entry does not already exist."));
    }
  else
    {
      searchstr = "SELECT DISTINCT book.title, "
	"book.author, "
	"book.publisher, book.pdate, book.place, book.edition, "
	"book.category, book.language, book.id, "
	"book.price, book.monetary_units, "
	"book.quantity, "
	"book.binding_type, "
	"book.location, "
	"book.isbn13, "
	"book.lccontrolnumber, "
	"book.callnumber, "
	"book.deweynumber, "
	"book.quantity - COUNT(item_borrower_vw.item_oid) "
	"AS availability, "
	"COUNT(item_borrower_vw.item_oid) AS total_reserved, "
	"book.originality, "
	"book.condition, "
	"book.type, "
	"book.myoid, "
	"book.front_cover "
	"FROM "
	"book LEFT JOIN item_borrower_vw ON "
	"book.myoid = item_borrower_vw.item_oid "
	"AND item_borrower_vw.type = 'Book' "
	"WHERE ";

      if(!id.id->text().trimmed().isEmpty())
	searchstr.append("id LIKE '%" + id.id->text().trimmed() +
			 "%' AND ");

      QString E("");

      if(qmain->getDB().driverName() != "QSQLITE")
	E = "E";

      searchstr.append("title LIKE " + E + "'%" +
		       biblioteq_myqstring::
		       escape(id.title->text().trimmed()) +
		       "%' AND ");

      if(!m_engWindowTitle.isEmpty())
	if(!id.isbn13->text().trimmed().isEmpty())
	  searchstr.append("isbn13 LIKE '%" +
			   id.isbn13->text().trimmed() + "%' AND ");

      searchstr.append("COALESCE(lccontrolnumber, '') LIKE " + E + "'%" +
		       biblioteq_myqstring::
		       escape(id.lcnum->text().trimmed()) +
		       "%' AND ");
      searchstr.append("COALESCE(callnumber, '') LIKE " + E + "'%" +
		       biblioteq_myqstring::
		       escape(id.callnum->text().trimmed()) +
		       "%' AND ");
      searchstr.append("COALESCE(deweynumber, '') LIKE " + E + "'%" +
		       biblioteq_myqstring::
		       escape(id.deweynum->text().trimmed()) +
		       "%' AND ");

      if(id.edition->currentIndex() != 0)
	searchstr.append("edition = '" +
			 id.edition->currentText().trimmed() +
			 "' AND ");

      searchstr.append("author LIKE " + E + "'%" +
		       biblioteq_myqstring::
		       escape(id.author->toPlainText().
			      trimmed()) + "%' AND ");

      if(id.publication_date->date().toString("MM/yyyy") != "01/7999")
	searchstr.append("SUBSTR(pdate, 1, 3) || SUBSTR(pdate, 7) = '" +
			 id.publication_date->date().toString
			 ("MM/yyyy") +
			 "' AND ");

      searchstr.append("publisher LIKE " + E + "'%" +
		       biblioteq_myqstring::escape
		       (id.publisher->toPlainText().trimmed()) +
		       "%' AND ");
      searchstr.append("place LIKE " + E + "'%" +
		       biblioteq_myqstring::escape
		       (id.place->toPlainText().trimmed()) +
		       "%' AND ");
      searchstr.append("category LIKE " + E + "'%" +
		       biblioteq_myqstring::escape
		       (id.category->toPlainText().trimmed()) +
		       "%' AND ");

      if(id.price->value() > -0.01)
	{
	  searchstr.append("price = ");
	  searchstr.append(QString::number(id.price->value()));
	  searchstr.append(" AND ");
	}

      if(id.language->currentIndex() != 0)
	searchstr.append("language = " + E + "'" +
			 biblioteq_myqstring::
			 escape(id.language->currentText().
				trimmed()) +
			 "' AND ");

      if(id.monetary_units->currentIndex() != 0)
	searchstr.append("monetary_units = " + E + "'" +
			 biblioteq_myqstring::escape
			 (id.monetary_units->currentText().trimmed()) +
			 "' AND ");

      if(id.binding->currentIndex() != 0)
	searchstr.append("binding_type = " + E + "'" +
			 biblioteq_myqstring::
			 escape(id.binding->currentText().
				trimmed()) +
			 "' AND ");

      searchstr.append("description LIKE " + E + "'%" +
		       biblioteq_myqstring::escape
		       (id.description->toPlainText().trimmed()) +
		       "%' ");

      if(id.quantity->value() != 0)
	searchstr.append("AND quantity = " + id.quantity->text() + " ");

      if(id.location->currentIndex() != 0)
	searchstr.append("AND location = " + E + "'" +
			 biblioteq_myqstring::escape
			 (id.location->currentText().trimmed()) + "' ");

      searchstr.append("AND COALESCE(marc_tags, '') LIKE " + E + "'%" +
		       biblioteq_myqstring::escape
		       (id.marc_tags->toPlainText().trimmed()) +
		       "%' ");
      searchstr.append("AND COALESCE(keyword, '') LIKE " + E + "'%" +
		       biblioteq_myqstring::escape
		       (id.keyword->toPlainText().trimmed()) +
		       "%' ");

      if(id.originality->currentIndex() != 0)
	searchstr.append("AND originality = " + E + "'" +
			 biblioteq_myqstring::escape
			 (id.originality->currentText().trimmed()) + "' ");

      if(id.condition->currentIndex() != 0)
	searchstr.append("AND condition = " + E + "'" +
			 biblioteq_myqstring::escape
			 (id.condition->currentText().trimmed()) + "' ");

      /*
      ** Search the database.
      */

      (void) qmain->populateTable
	(biblioteq::POPULATE_SEARCH, "Books", searchstr);
    }
}

/*
** -- search() --
*/

void biblioteq_book::search(const QString &field, const QString &value)
{
  id.attach_files->setVisible(false);
  id.coverImages->setVisible(false);
  id.delete_files->setVisible(false);
  id.export_files->setVisible(false);
  id.id->clear();
  id.category->clear();
  id.isbn13->clear();
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
  id.copiesButton->setVisible(false);
  id.showUserButton->setVisible(false);
  id.sruQueryButton->setVisible(false);
  id.z3950QueryButton->setVisible(false);
  id.okButton->setText(tr("&Search"));
  id.publication_date->setDate(QDate::fromString("01/7999",
						 "MM/yyyy"));
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
  id.location->setCurrentIndex(0);
  id.edition->setCurrentIndex(0);
  id.language->setCurrentIndex(0);
  id.monetary_units->setCurrentIndex(0);
  id.binding->setCurrentIndex(0);
  id.originality->setCurrentIndex(0);
  id.condition->setCurrentIndex(0);
  id.isbnAvailableCheckBox->setCheckable(false);

  if(field.isEmpty() && value.isEmpty())
    {
      QList<QAction *> actions = id.resetButton->menu()->actions();

      if(actions.size() >= 2)
	{
	  actions[0]->setVisible(false);
	  actions[1]->setVisible(false);
	}

      actions.clear();
      setWindowTitle(tr("BiblioteQ: Database Book Search"));
      m_engWindowTitle = "Search";
      id.id->setFocus();
      biblioteq_misc_functions::center(this, m_parentWid);
      show();
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

/*
** -- updateWindow() --
*/

void biblioteq_book::updateWindow(const int state)
{
  QString str = "";

  if(state == biblioteq::EDITABLE)
    {
      id.attach_files->setEnabled(true);
      id.copiesButton->setEnabled(true);
      id.delete_files->setEnabled(true);
      id.export_files->setEnabled(true);
      id.showUserButton->setEnabled(true);
      id.okButton->setVisible(true);
      id.sruQueryButton->setVisible(true);
      id.z3950QueryButton->setVisible(true);
      id.resetButton->setVisible(true);
      id.frontButton->setVisible(true);
      id.backButton->setVisible(true);
      id.dwnldFront->setVisible(true);
      id.dwnldBack->setVisible(true);
      id.isbn10to13->setVisible(true);
      id.isbn13to10->setVisible(true);

      if(!id.id->text().trimmed().isEmpty())
	str = QString(tr("BiblioteQ: Modify Book Entry (")) +
	  id.id->text().trimmed() + tr(")");
      else
	str = tr("BiblioteQ: Modify Book Entry");

      m_engWindowTitle = "Modify";
    }
  else
    {
      id.attach_files->setVisible(false);
      id.isbnAvailableCheckBox->setCheckable(false);
      id.copiesButton->setVisible(false);
      id.delete_files->setVisible(false);
      id.export_files->setEnabled(true);
      id.showUserButton->setEnabled(true);
      id.okButton->setVisible(false);
      id.sruQueryButton->setVisible(false);
      id.z3950QueryButton->setVisible(false);
      id.resetButton->setVisible(false);
      id.frontButton->setVisible(false);
      id.backButton->setVisible(false);
      id.dwnldFront->setVisible(false);
      id.dwnldBack->setVisible(false);
      id.isbn10to13->setVisible(false);
      id.isbn13to10->setVisible(false);

      if(!id.id->text().trimmed().isEmpty())
	str = QString(tr("BiblioteQ: View Book Details (")) +
	  id.id->text().trimmed() + tr(")");
      else
	str = tr("BiblioteQ: View Book Details");

      m_engWindowTitle = "View";
    }

  id.coverImages->setVisible(true);
  setWindowTitle(str);
}

/*
** -- modify() --
*/

void biblioteq_book::modify(const int state)
{
  QSqlQuery query(qmain->getDB());
  QString fieldname = "";
  QString str = "";
  QVariant var;
  int i = 0;

  if(state == biblioteq::EDITABLE)
    {
      setWindowTitle(tr("BiblioteQ: Modify Book Entry"));
      m_engWindowTitle = "Modify";
      id.attach_files->setEnabled(true);
      id.copiesButton->setEnabled(true);
      id.delete_files->setEnabled(true);
      id.export_files->setEnabled(true);
      id.showUserButton->setEnabled(true);
      id.okButton->setVisible(true);
      id.sruQueryButton->setVisible(true);
      id.z3950QueryButton->setVisible(true);
      id.resetButton->setVisible(true);
      id.frontButton->setVisible(true);
      id.backButton->setVisible(true);
      id.dwnldFront->setVisible(true);
      id.dwnldBack->setVisible(true);
      id.isbn10to13->setVisible(true);
      id.isbn13to10->setVisible(true);
      biblioteq_misc_functions::highlightWidget
	(id.id, QColor(255, 248, 220));
      biblioteq_misc_functions::highlightWidget
	(id.isbn13, QColor(255, 248, 220));
      biblioteq_misc_functions::highlightWidget
	(id.title, QColor(255, 248, 220));
      biblioteq_misc_functions::highlightWidget
	(id.publisher->viewport(), QColor(255, 248, 220));
      biblioteq_misc_functions::highlightWidget
	(id.place->viewport(), QColor(255, 248, 220));
      biblioteq_misc_functions::highlightWidget
	(id.author->viewport(), QColor(255, 248, 220));
      biblioteq_misc_functions::highlightWidget
	(id.description->viewport(), QColor(255, 248, 220));
      biblioteq_misc_functions::highlightWidget
	(id.category->viewport(), QColor(255, 248, 220));
      m_te_orig_pal = id.id->palette();
    }
  else
    {
      id.isbnAvailableCheckBox->setCheckable(false);
      setWindowTitle(tr("BiblioteQ: View Book Details"));
      m_engWindowTitle = "View";
      id.attach_files->setVisible(false);
      id.copiesButton->setVisible(false);
      id.delete_files->setVisible(false);
      id.export_files->setVisible(true);
      id.showUserButton->setEnabled(true);
      id.okButton->setVisible(false);
      id.sruQueryButton->setVisible(false);
      id.z3950QueryButton->setVisible(false);
      id.resetButton->setVisible(false);
      id.frontButton->setVisible(false);
      id.backButton->setVisible(false);
      id.dwnldFront->setVisible(false);
      id.dwnldBack->setVisible(false);
      id.isbn10to13->setVisible(false);
      id.isbn13to10->setVisible(false);

      QList<QAction *> actions = id.resetButton->menu()->actions();

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
  query.setForwardOnly(true);
  query.prepare("SELECT title, "
		"author, "
		"publisher, pdate, place, edition, "
		"category, language, id, "
		"price, monetary_units, quantity, "
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
		"condition "
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
      id.id->setFocus();
      return;
    }
  else
    {
      QApplication::restoreOverrideCursor();
      showNormal();

      for(i = 0; i < query.record().count(); i++)
	{
	  var = query.record().field(i).value();
	  fieldname = query.record().fieldName(i);

	  if(fieldname == "title")
	    id.title->setText(var.toString());
	  else if(fieldname == "author")
	    id.author->setMultipleLinks("book_search", "author",
					var.toString());
	  else if(fieldname == "publisher")
	    id.publisher->setMultipleLinks("book_search", "publisher",
					   var.toString());
	  else if(fieldname == "place")
	    id.place->setMultipleLinks("book_search", "place",
				       var.toString());
	  else if(fieldname == "pdate")
	    id.publication_date->setDate
	      (QDate::fromString(var.toString(), "MM/dd/yyyy"));
	  else if(fieldname == "edition")
	    {
	      if(id.edition->findText(var.toString()) > -1)
		id.edition->setCurrentIndex
		  (id.edition->findText(var.toString()));
	      else
		id.edition->setCurrentIndex(0);
	    }
	  else if(fieldname == "price")
	    id.price->setValue(var.toDouble());
	  else if(fieldname == "category")
	    id.category->setMultipleLinks("book_search", "category",
					  var.toString());
	  else if(fieldname == "language")
	    {
	      if(id.language->findText(var.toString()) > -1)
		id.language->setCurrentIndex
		  (id.language->findText(var.toString()));
	      else
		id.language->setCurrentIndex
		  (id.language->findText(tr("UNKNOWN")));
	    }
	  else if(fieldname == "quantity")
	    id.quantity->setValue(var.toInt());
	  else if(fieldname == "monetary_units")
	    {
	      if(id.monetary_units->findText(var.toString()) > -1)
		id.monetary_units->setCurrentIndex
		  (id.monetary_units->findText(var.toString()));
	      else
		id.monetary_units->setCurrentIndex
		  (id.monetary_units->findText(tr("UNKNOWN")));
	    }
	  else if(fieldname == "binding_type")
	    {
	      if(id.binding->findText(var.toString()) > -1)
		id.binding->setCurrentIndex
		  (id.binding->findText(var.toString()));
	      else
		id.binding->setCurrentIndex(0);
	    }
	  else if(fieldname == "location")
	    {
	      if(id.location->findText(var.toString()) > -1)
		id.location->setCurrentIndex
		  (id.location->findText(var.toString()));
	      else
		id.location->setCurrentIndex
		  (id.location->findText(tr("UNKNOWN")));
	    }
	  else if(fieldname == "id")
	    {
	      if(state == biblioteq::EDITABLE)
		{
		  if(!var.toString().trimmed().isEmpty())
		    str = QString(tr("BiblioteQ: Modify Book Entry (")) +
		      var.toString() + tr(")");
		  else
		    str = tr("BiblioteQ: Modify Book Entry");

		  m_engWindowTitle = "Modify";
		}
	      else
		{
		  if(!var.toString().trimmed().isEmpty())
		    str = QString(tr("BiblioteQ: View Book Details (")) +
		      var.toString() + tr(")");
		  else
		    str = tr("BiblioteQ: View Book Details");

		  m_engWindowTitle = "View";
		}

	      id.id->setText(var.toString());
	      setWindowTitle(str);

	      if(query.isNull(i))
		id.isbnAvailableCheckBox->setChecked(false);
	      else
		id.isbnAvailableCheckBox->setChecked(true);
	    }
	  else if(fieldname == "description")
	    id.description->setPlainText(var.toString());
	  else if(fieldname == "marc_tags")
	    id.marc_tags->setPlainText(var.toString());
	  else if(fieldname == "keyword")
	    id.keyword->setMultipleLinks("book_search", "keyword",
					 var.toString());
	  else if(fieldname == "isbn13")
	    id.isbn13->setText(var.toString());
	  else if(fieldname == "lccontrolnumber")
	    id.lcnum->setText(var.toString());
	  else if(fieldname == "callnumber")
	    id.callnum->setText(var.toString());
	  else if(fieldname == "deweynumber")
	    id.deweynum->setText(var.toString());
	  else if(fieldname == "originality")
	    {
	      if(id.originality->findText(var.toString()) > -1)
		id.originality->setCurrentIndex
		  (id.originality->findText(var.toString()));
	      else
		id.originality->setCurrentIndex(2);
	    }
	  else if(fieldname == "condition")
	    {
	      if(id.condition->findText(var.toString()) > -1)
		id.condition->setCurrentIndex
		  (id.condition->findText(var.toString()));
	      else
		id.condition->setCurrentIndex(0);
	    }
	  else if(fieldname == "front_cover")
	    {
	      if(!query.record().field(i).isNull())
		{
		  id.front_image->loadFromData
		    (QByteArray::fromBase64(var.toByteArray()));

		  if(id.front_image->m_image.isNull())
		    id.front_image->loadFromData(var.toByteArray());
		}
	    }
	  else if(fieldname == "back_cover")
	    {
	      if(!query.record().field(i).isNull())
		{
		  id.back_image->loadFromData
		    (QByteArray::fromBase64(var.toByteArray()));

		  if(id.back_image->m_image.isNull())
		    id.back_image->loadFromData(var.toByteArray());
		}
	    }
	}

      foreach(QLineEdit *textfield, findChildren<QLineEdit *>())
	textfield->setCursorPosition(0);

      storeData(this);
      populateFiles();
    }

  id.id->setFocus();
  raise();
}

/*
** -- insert() --
*/

void biblioteq_book::insert(void)
{
  slotReset();
  id.attach_files->setEnabled(false);
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
  id.sruQueryButton->setVisible(true);
  id.z3950QueryButton->setVisible(true);
  id.okButton->setText(tr("&Save"));
  id.publication_date->setDate(QDate::fromString("01/01/2000",
						 "MM/dd/yyyy"));
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
  biblioteq_misc_functions::highlightWidget
    (id.id, QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (id.isbn13, QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (id.title, QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (id.publisher->viewport(), QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (id.place->viewport(), QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (id.author->viewport(), QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (id.description->viewport(), QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (id.category->viewport(), QColor(255, 248, 220));
  m_te_orig_pal = id.id->palette();
  setWindowTitle(tr("BiblioteQ: Create Book Entry"));
  m_engWindowTitle = "Create";
  id.id->setFocus();
  storeData(this);
  show();
}

/*
** -- slotReset() --
*/

void biblioteq_book::slotReset(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  if(action != 0)
    {
      QList<QAction *> actions = id.resetButton->menu()->actions();

      if(actions.size() < 25)
	{
	  // Error.
	}
      else if(action == actions[0])
	id.front_image->clear();
      else if(action == actions[1])
	id.back_image->clear();
      else if(action == actions[2])
	{
	  id.id->clear();
	  id.id->setPalette(m_te_orig_pal);
	  id.id->setFocus();
	}
      else if(action == actions[3])
	{
	  id.isbn13->clear();
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
	    id.publication_date->setDate
	      (QDate::fromString("01/7999", "MM/yyyy"));
	  else
	    id.publication_date->setDate
	      (QDate::fromString("01/01/2000", "MM/dd/yyyy"));

	  id.publication_date->setStyleSheet(m_dt_orig_ss);
	  id.publication_date->setFocus();
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

      actions.clear();
    }
  else
    {
      /*
      ** Reset all.
      */

      id.id->clear();
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
	id.publication_date->setDate(QDate::fromString("01/7999",
						       "MM/yyyy"));
      else
	id.publication_date->setDate(QDate::fromString("01/01/2000",
						       "MM/dd/yyyy"));

      id.quantity->setValue(id.quantity->minimum());

      if(!m_engWindowTitle.contains("Search"))
	id.description->setPlainText("N/A");
      else
	id.description->clear();

      id.marc_tags->clear();
      id.keyword->clear();
      id.isbn13->clear();
      id.lcnum->clear();
      id.callnum->clear();
      id.deweynum->clear();
      id.location->setCurrentIndex(0);

      if(m_engWindowTitle.contains("Search"))
	id.originality->setCurrentIndex(0);
      else
	id.originality->setCurrentIndex(2);

      id.condition->setCurrentIndex(0);
      id.edition->setCurrentIndex(0);
      id.price->setValue(id.price->minimum());
      id.language->setCurrentIndex(0);
      id.monetary_units->setCurrentIndex(0);
      id.binding->setCurrentIndex(0);
      id.front_image->clear();
      id.back_image->clear();
      id.title->setPalette(m_te_orig_pal);
      id.lcnum->setPalette(m_white_pal);
      id.deweynum->setPalette(m_white_pal);
      id.callnum->setPalette(m_white_pal);
      id.id->setPalette(m_te_orig_pal);
      id.isbn13->setPalette(m_te_orig_pal);
      id.edition->setStyleSheet(m_cb_orig_ss);
      id.binding->setStyleSheet(m_cb_orig_ss);
      id.category->viewport()->setPalette(m_te_orig_pal);
      id.publication_date->setStyleSheet(m_dt_orig_ss);
      id.author->viewport()->setPalette(m_te_orig_pal);
      id.description->viewport()->setPalette(m_te_orig_pal);
      id.marc_tags->viewport()->setPalette(m_white_pal);
      id.keyword->viewport()->setPalette(m_white_pal);
      id.publisher->viewport()->setPalette(m_te_orig_pal);
      id.place->viewport()->setPalette(m_te_orig_pal);
      id.id->setFocus();
    }
}

/*
** -- slotConvertISBN10to13() --
*/

void biblioteq_book::slotConvertISBN10to13(void)
{
  QString numberstr = "";
  QString str = "";
  int arr[] = {1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3};
  int check = 0;
  int i = 0;
  int total = 0;

  str = "978" + id.id->text().trimmed().left(9);

  for(i = 0; i < (int) (sizeof(arr) / sizeof(int)); i++)
    if(i < str.length())
      total += str[i].digitValue() * arr[i];
    else
      break;

  check = 10 - (total % 10);

  if(check == 10)
    check = 0;

  numberstr.setNum(check);
  id.isbn13->setText(str + numberstr);
}

/*
** -- slotConvertISBN13to10() --
*/

void biblioteq_book::slotConvertISBN13to10(void)
{
  QString isbnnum(id.isbn13->text().trimmed().mid(3, 9));
  QString z("");
  int total = 0;

  for(int i = 0; i < 9; i++)
    if(i < isbnnum.length())
      total += isbnnum[i].digitValue() * (10 - i);
    else
      break;

  z = QString::number((11 - (total % 11)) % 11);

  if(z == "10")
    z = "X";

  id.id->setText(isbnnum + z);
}

/*
** -- closeEvent() --
*/

void biblioteq_book::closeEvent(QCloseEvent *e)
{
  if(m_engWindowTitle.contains("Create") ||
     m_engWindowTitle.contains("Modify"))
    if(hasDataChanged(this))
      if(QMessageBox::
	 question(this, tr("BiblioteQ: Question"),
		  tr("Your changes have not been saved. Continue closing?"),
		  QMessageBox::Yes | QMessageBox::No,
		  QMessageBox::No) == QMessageBox::No)
	{
	  if(e)
	    e->ignore();

	  return;
	}

  qmain->removeBook(this);
}

/*
** -- slotCancel() --
*/

void biblioteq_book::slotCancel(void)
{
  close();
}

/*
** -- slotPopulateCopiesEditor() --
*/

void biblioteq_book::slotPopulateCopiesEditor(void)
{
  biblioteq_copy_editor_book *copyeditor = 0;

  if((copyeditor = new(std::nothrow) biblioteq_copy_editor_book
      (qobject_cast<QWidget *> (this),
       static_cast<biblioteq_item *> (this),
       false,
       id.quantity->value(), m_oid,
       id.quantity, font(),
       id.id->text().trimmed())) != 0)
    copyeditor->populateCopiesEditor();
}

/*
** -- slotShowUsers() --
*/

void biblioteq_book::slotShowUsers(void)
{
  biblioteq_borrowers_editor *borrowerseditor = 0;
  int state = 0;

  if(!id.okButton->isHidden())
    state = biblioteq::EDITABLE;
  else
    state = biblioteq::VIEW_ONLY;

  if((borrowerseditor = new(std::nothrow) biblioteq_borrowers_editor
      (qobject_cast<QWidget *> (this), static_cast<biblioteq_item *> (this),
       id.quantity->value(), m_oid, id.id->text().trimmed(), font(), "Book",
       state)) != 0)
    borrowerseditor->showUsers();
}

/*
** -- slotSRUQuery() --
*/

void biblioteq_book::slotSRUQuery(void)
{
  if(!(id.id->text().trimmed().length() == 10 ||
       id.isbn13->text().trimmed().length() == 13))
    {
      QMessageBox::critical
	(this, tr("BiblioteQ: User Error"),
	 tr("In order to query an SRU site, either the ISBN-10 "
	    "or ISBN-13 must be provided."));
      id.id->setFocus();
      return;
    }

  if(useHttp())
    {
#if QT_VERSION < 0x050000
      if(m_sruHttp->currentId() != 0)
	return;
#endif
    }
  else
    {
      if(m_sruManager->findChild<QNetworkReply *> ())
	return;
    }

  m_sruWorking->reset(); // Qt 5.5.x adjustment.
  m_sruWorking->setMaximum(0);
  m_sruWorking->setMinimum(0);
  m_sruWorking->show();
  m_sruWorking->update();

  QString name("");
  bool found = false;

  for(int i = 0; i < id.sruQueryButton->actions().size(); i++)
    if(id.sruQueryButton->actions().at(i)->isChecked())
      {
	found = true;
	name = id.sruQueryButton->actions().at(i)->text();
	break;
      }

  if(!found)
    name = qmain->getPreferredSRUSite();

  QHash<QString, QString> hash(qmain->getSRUMaps()[name]);
  QString searchstr("");

  searchstr = hash["url_isbn"];

  if(!id.id->text().trimmed().isEmpty())
    searchstr.replace("%1", id.id->text().trimmed());
  else
    searchstr.replace("%1", id.isbn13->text().trimmed());

  if(!id.isbn13->text().trimmed().isEmpty())
    searchstr.replace("%2", id.isbn13->text().trimmed());
  else
    searchstr.replace("%2", id.id->text().trimmed());

  QNetworkProxy proxy;
  QString type("");
  QUrl url(QUrl::fromUserInput(searchstr));

  if(hash.contains("proxy_type"))
    type = hash["proxy_type"].toLower().trimmed();
  else if(hash.contains("proxy_type"))
    type = hash["proxy_type"].toLower().trimmed();

  if(type == "none")
    proxy.setType(QNetworkProxy::NoProxy);
  else
    {
      if(type == "http" || type == "socks5" || type == "system")
	{
	  if(useHttp())
	    {
#if QT_VERSION < 0x050000
	      connect
		(m_sruHttp,
		 SIGNAL(proxyAuthenticationRequired(const QNetworkProxy &,
						    QAuthenticator *)),
		 this,
		 SLOT(slotProxyAuthenticationRequired(const QNetworkProxy &,
						      QAuthenticator *)),
		 Qt::UniqueConnection);
#endif
	    }
	  else
	    {
	      /*
	      ** This is required to resolve an odd error.
	      */

	      QNetworkReply *reply = m_sruManager->get
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

	  host = hash["proxy_host"];
	  port = hash["proxy_port"].toUShort();
	  user = hash["proxy_username"];
	  password = hash["proxy_password"];
	  proxy.setHostName(host);
	  proxy.setPort(port);

	  if(!user.isEmpty())
	    proxy.setUser(user);

	  if(!password.isEmpty())
	    proxy.setPassword(password);

	  if(useHttp())
	    {
#if QT_VERSION < 0x050000
	      m_sruHttp->setProxy(proxy);
#endif
	    }
	  else
	    m_sruManager->setProxy(proxy);
	}
      else if(type == "system")
	{
	  QList<QNetworkProxy> list;
	  QNetworkProxyQuery query(url);

	  list = QNetworkProxyFactory::systemProxyForQuery(query);

	  if(!list.isEmpty())
	    proxy = list.at(0);

	  if(useHttp())
	    {
#if QT_VERSION < 0x050000
	      m_sruHttp->setProxy(proxy);
#endif
	    }
	  else
	    m_sruManager->setProxy(proxy);
	}
    }

  if(useHttp())
    {
#if QT_VERSION < 0x050000
      if(url.port() == -1)
	url.setPort(80);

      m_sruHttp->abort();
      m_sruResults.clear();
      connect(m_sruHttp, SIGNAL(done(bool)),
	      this, SLOT(slotSRUDownloadFinished(bool)),
	      Qt::UniqueConnection);
      connect(m_sruHttp, SIGNAL(readyRead(const QHttpResponseHeader &)),
	      this, SLOT(slotSRUReadyRead(const QHttpResponseHeader &)),
	      Qt::UniqueConnection);
      connect(m_sruHttp, SIGNAL(sslErrors(const QList<QSslError> &)),
	      this, SLOT(slotSRUSslErrors(const QList<QSslError> &)),
	      Qt::UniqueConnection);
      m_sruHttp->setHost(url.host(), url.port());
      m_sruHttp->get(url.toEncoded());
#endif
    }
  else
    {
      QNetworkReply *reply = m_sruManager->get(QNetworkRequest(url));

      if(reply)
	{
	  m_sruResults.clear();
	  connect(reply, SIGNAL(readyRead(void)),
		  this, SLOT(slotSRUReadyRead(void)));
	  connect(reply, SIGNAL(finished(void)),
		  this, SLOT(slotSRUDownloadFinished(void)));
	  connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
		  this, SLOT(slotSRUError(QNetworkReply::NetworkError)));
	  connect(reply, SIGNAL(sslErrors(const QList<QSslError> &)),
		  this, SLOT(slotSRUSslErrors(const QList<QSslError> &)));
	}
      else
	{
	  m_sruWorking->reset(); // Qt 5.5.x adjustment.
	  m_sruWorking->close();
	}
    }
}

/*
** -- slotZ3950Query() --
*/

void biblioteq_book::slotZ3950Query(void)
{
  if(findChild<biblioteq_generic_thread *> ())
    return;

  QString errorstr = "";
  QString etype = "";
  QString searchstr = "";
  QString str = "";
  QStringList list;
  QStringList tmplist;
  int i = 0;

  if(!(id.id->text().trimmed().length() == 10 ||
       id.isbn13->text().trimmed().length() == 13))
    {
      QMessageBox::critical
	(this, tr("BiblioteQ: User Error"),
	 tr("In order to query a Z39.50 site, either the ISBN-10 "
	    "or ISBN-13 must be provided."));
      id.id->setFocus();
      return;
    }

  if((m_thread = new(std::nothrow) biblioteq_generic_thread(this)) != 0)
    {
      biblioteq_item_working_dialog working
	(qobject_cast<QMainWindow *> (this));

      working.setModal(true);
      working.setWindowTitle(tr("BiblioteQ: Z39.50 Data Retrieval"));
      working.setLabelText(tr("Downloading information from the Z39.50 "
			      "site. Please be patient..."));
      working.setMaximum(0);
      working.setMinimum(0);
      working.show();
      working.update();

      QString recordSyntax("MARC21");
      QStringList isbns;
      bool isbn10User = false;
      bool isbn13User = false;

      isbns << id.id->text().trimmed()
	    << id.isbn13->text().trimmed();

      if(id.id->text().trimmed().length() == 10)
	isbn10User = true;

      if(id.isbn13->text().trimmed().length() == 13)
	isbn13User = true;

      if(isbns.at(0).isEmpty())
	searchstr = QString("@attr 1=7 %1").arg(isbns.at(1));
      else if(isbns.at(1).isEmpty())
	searchstr = QString("@attr 1=7 %1").arg(isbns.at(0));
      else
	searchstr = QString("@attr 1=7 @or %1 %2").arg(isbns.at(0)).
	  arg(isbns.at(1));

      bool found = false;

      for(i = 0; i < id.z3950QueryButton->actions().size(); i++)
	if(id.z3950QueryButton->actions().at(i)->isChecked())
	  {
	    found = true;
	    recordSyntax = qmain->getZ3950Maps().value
	      (id.z3950QueryButton->actions().at(i)->text())["RecordSyntax"];
	    m_thread->setZ3950Name
	      (id.z3950QueryButton->actions().at(i)->text());
	    break;
	  }

      if(!found)
	{
	  recordSyntax = qmain->getZ3950Maps().value
	    (qmain->getPreferredZ3950Site())["RecordSyntax"];
	  m_thread->setZ3950Name(qmain->getPreferredZ3950Site());
	}

      m_thread->setType(biblioteq_generic_thread::Z3950_QUERY);
      m_thread->setZ3950SearchString(searchstr);
      m_thread->start();

      while(m_thread->isRunning())
	{
#ifndef Q_OS_MAC
	  QApplication::processEvents();
#endif
	  m_thread->msleep(100);
	}

      working.close();

      bool canceled = working.wasCanceled();

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
	      biblioteq_marc m;

	      if(recordSyntax == "MARC21")
		m.initialize
		  (biblioteq_marc::BOOK, biblioteq_marc::Z3950,
		   biblioteq_marc::MARC21);
	      else
		m.initialize(biblioteq_marc::BOOK, biblioteq_marc::Z3950,
			     biblioteq_marc::UNIMARC);

	      m.setData(m_thread->getZ3950Results()[0]);
	      list = QString(m_thread->getZ3950Results()[0]).split("\n");
	      id.edition->setCurrentIndex(0);
	      id.edition->setStyleSheet
		("background-color: rgb(162, 205, 90)");
	      id.marc_tags->setPlainText
		(m_thread->getZ3950Results()[0].trimmed());
	      biblioteq_misc_functions::highlightWidget
		(id.marc_tags->viewport(), QColor(162, 205, 90));

	      for(i = 0; i < list.size(); i++)
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
		    (id.author->viewport(), QColor(162, 205, 90));
		}

	      str = m.binding();

	      if(id.binding->findText(str, Qt::MatchFixedString) > -1)
		id.binding->setCurrentIndex
		  (id.binding->findText(str, Qt::MatchFixedString));

	      id.binding->setStyleSheet
		("background-color: rgb(162, 205, 90)");
	      str = m.callnum();

	      if(!str.isEmpty())
		{
		  id.callnum->setText(str);
		  biblioteq_misc_functions::highlightWidget
		    (id.callnum, QColor(162, 205, 90));
		}

	      str = m.category();

	      if(!str.isEmpty())
		{
		  id.category->setPlainText(str);
		  biblioteq_misc_functions::highlightWidget
		    (id.category->viewport(),
		     QColor(162, 205, 90));
		}

	      str = m.description();

	      if(!str.isEmpty())
		{
		  id.description->setPlainText(str);
		  biblioteq_misc_functions::highlightWidget
		    (id.description->viewport(), QColor(162, 205, 90));
		}

	      str = m.deweynum();

	      if(!str.isEmpty())
		{
		  id.deweynum->setText(str);
		  biblioteq_misc_functions::highlightWidget
		    (id.deweynum, QColor(162, 205, 90));
		}

	      str = m.edition();

	      if(id.edition->findText(str) > -1)
		id.edition->setCurrentIndex
		  (id.edition->findText(str));
	      else
		id.edition->setCurrentIndex(0);

	      str = m.isbn10();

	      if(!isbn10User && !str.isEmpty())
		{
		  id.id->setText(str);
		  biblioteq_misc_functions::highlightWidget
		    (id.id, QColor(162, 205, 90));
		}

	      str = m.isbn13();

	      if(!isbn13User && !str.isEmpty())
		{
		  id.isbn13->setText(str);
		  biblioteq_misc_functions::highlightWidget
		    (id.isbn13, QColor(162, 205, 90));
		}

	      str = m.lcnum();

	      if(!str.isEmpty())
		{
		  id.lcnum->setText(str);
		  biblioteq_misc_functions::highlightWidget
		    (id.lcnum, QColor(162, 205, 90));
		}

	      str = m.place();

	      if(!str.isEmpty())
		{
		  id.place->setPlainText(str);
		  biblioteq_misc_functions::highlightWidget
		    (id.place->viewport(), QColor(162, 205, 90));
		}

	      if(!m.publicationDate().isNull())
		{
		  id.publication_date->setDate
		    (m.publicationDate());
		  id.publication_date->setStyleSheet
		    ("background-color: rgb(162, 205, 90)");
		}

	      str = m.publisher();

	      if(!str.isEmpty())
		{
		  id.publisher->setPlainText(str);
		  biblioteq_misc_functions::highlightWidget
		    (id.publisher->viewport(), QColor(162, 205, 90));
		}

	      str = m.title();

	      if(!str.isEmpty())
		{
		  id.title->setText(str);
		  biblioteq_misc_functions::highlightWidget
		    (id.title, QColor(162, 205, 90));
		}

	      foreach(QLineEdit *textfield, findChildren<QLineEdit *>())
		textfield->setCursorPosition(0);
	    }
	}
      else if(errorstr.isEmpty() && m_thread->getZ3950Results().isEmpty())
	QMessageBox::critical
	  (this, tr("BiblioteQ: Z39.50 Query Error"),
	   tr("A Z39.50 entry may not yet exist for the provided ISBN(s)."));
      else
	etype = m_thread->getEType();

      m_thread->deleteLater();
    }
  else
    {
      etype = tr("Memory Error");
      errorstr = tr("Unable to create a thread due to "
		    "insufficient resources.");
    }

  if(!errorstr.isEmpty())
    {
      qmain->addError(QString(tr("Z39.50 Query Error")), etype, errorstr,
		      __FILE__, __LINE__);
      QMessageBox::critical
	(this, tr("BiblioteQ: Z39.50 Query Error"),
	 tr("The Z39.50 entry could not be retrieved. Please view the "
	    "error log."));
    }
}

/*
** -- slotPrint() --
*/

void biblioteq_book::slotPrint(void)
{
  m_html = "";
  m_html += "<b>" + tr("ISBN-10:") + "</b> " +
    id.id->text().trimmed() + "<br>";
  m_html += "<b>" + tr("ISBN-13:") + "</b> " +
    id.isbn13->text().trimmed() + "<br>";
  m_html += "<b>" + tr("Edition:") + "</b> " +
    id.edition->currentText() + "<br>";
  m_html += "<b>" + tr("Author(s):") + "</b> " +
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
  print(this);
}

/*
** -- slotSelectImage() --
*/

void biblioteq_book::slotSelectImage(void)
{
  QFileDialog dialog(this);
  QPushButton *button = qobject_cast<QPushButton *> (sender());

#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  dialog.setAttribute(Qt::WA_MacMetalStyle, BIBLIOTEQ_WA_MACMETALSTYLE);
#endif
#endif
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setDirectory(QDir::homePath());

  if(button == id.frontButton)
    dialog.setWindowTitle(tr("BiblioteQ: Front Cover Image Selection"));
  else
    dialog.setWindowTitle(tr("BiblioteQ: Back Cover Image Selection"));

  dialog.exec();

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
	    (QPixmap().fromImage(id.front_image->m_image));

	  if(id.front_image->scene()->items().size() > 0)
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
	    (QPixmap().fromImage(id.back_image->m_image));

	  if(id.back_image->scene()->items().size() > 0)
	    id.back_image->scene()->items().at(0)->setFlags
	      (QGraphicsItem::ItemIsSelectable);

	  id.back_image->scene()->setSceneRect
	    (id.back_image->scene()->itemsBoundingRect());
	}
    }
}

/*
** -- slotDownloadImage() --
*/

void biblioteq_book::slotDownloadImage(void)
{
  if(m_httpProgress->isVisible())
    return;

  if(id.id->text().trimmed().length() != 10)
    {
      QMessageBox::critical
	(this, tr("BiblioteQ: User Error"),
	 tr("In order to download a cover image from Amazon, the ISBN-10 "
	    "must be provided."));
      id.id->setFocus();
      return;
    }

  if(useHttp())
    {
#if QT_VERSION < 0x050000
      if(m_imageHttp->currentId() != 0)
	return;
#endif
    }
  else
    {
      if(m_imageManager->findChild<QNetworkReply *> ())
	return;
    }

  QPushButton *pb = qobject_cast<QPushButton *> (sender());

  if(pb == id.dwnldFront)
    m_imageBuffer.setProperty("which", "front");
  else
    m_imageBuffer.setProperty("which", "back");

  if(!m_imageBuffer.open(QIODevice::WriteOnly))
    return;

  QUrl url;

  if(pb == id.dwnldFront)
    url = QUrl::fromUserInput
      (qmain->getAmazonHash()["front_cover_host"] +
       qmain->getAmazonHash()["front_cover_path"].replace
       ("%", id.id->text().trimmed()));
  else
    url = QUrl::fromUserInput
      (qmain->getAmazonHash()["back_cover_host"] +
       qmain->getAmazonHash()["back_cover_path"].replace
       ("%", id.id->text().trimmed()));

  QHash<QString, QString> hash(qmain->getAmazonHash());
  QNetworkProxy proxy;
  QString type;

  if(hash.contains("front_proxy_type"))
    type = hash["front_proxy_type"].toLower().trimmed();
  else if(hash.contains("back_proxy_type"))
    type = hash["back_proxy_type"].toLower().trimmed();

  if(type == "none")
    proxy.setType(QNetworkProxy::NoProxy);
  else
    {
      if(type == "http" || type == "socks5" || type == "system")
	{
	  if(useHttp())
	    {
#if QT_VERSION < 0x050000
	      connect
		 (m_imageHttp,
		  SIGNAL(proxyAuthenticationRequired(const QNetworkProxy &,
						     QAuthenticator *)),
		  this,
		  SLOT(slotProxyAuthenticationRequired(const QNetworkProxy &,
						       QAuthenticator *)),
		  Qt::UniqueConnection);
#endif
	    }
	  else
	    {
	      /*
	      ** This is required to resolve an odd error.
	      */

	      QNetworkReply *reply = m_imageManager->get
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

	  if(pb == id.dwnldFront)
	    {
	      host = hash["front_proxy_host"];
	      port = hash["front_proxy_port"].toUShort();
	      user = hash["front_proxy_username"];
	      password = hash["front_proxy_password"];
	    }
	  else
	    {
	      host = hash["back_proxy_host"];
	      port = hash["back_proxy_port"].toUShort();
	      user = hash["back_proxy_username"];
	      password = hash["back_proxy_password"];
	    }

	  proxy.setHostName(host);
	  proxy.setPort(port);

	  if(!user.isEmpty())
	    proxy.setUser(user);

	  if(!password.isEmpty())
	    proxy.setPassword(password);

	  if(useHttp())
	    {
#if QT_VERSION < 0x050000
	      m_imageHttp->setProxy(proxy);
#endif
	    }
	  else
	    m_imageManager->setProxy(proxy);
	}
      else if(type == "system")
	{
	  QList<QNetworkProxy> list;
	  QNetworkProxyQuery query(url);

	  list = QNetworkProxyFactory::systemProxyForQuery(query);

	  if(!list.isEmpty())
	    proxy = list.at(0);

	  if(useHttp())
	    {
#if QT_VERSION < 0x050000
	      m_imageHttp->setProxy(proxy);
#endif
	    }
	  else
	    m_imageManager->setProxy(proxy);
	}
    }

  if(useHttp())
    {
#if QT_VERSION < 0x050000
      if(url.port() == -1)
	url.setPort(80);

      m_imageHttp->abort();
      connect(m_imageHttp, SIGNAL(done(bool)),
	      this, SLOT(slotDownloadFinished(bool)),
	      Qt::UniqueConnection);
      connect(m_imageHttp, SIGNAL(readyRead(const QHttpResponseHeader &)),
	      this, SLOT(slotReadyRead(const QHttpResponseHeader &)),
	      Qt::UniqueConnection);
      m_imageHttp->setHost(url.host(), url.port());
      m_imageHttp->get(url.toEncoded());
#endif
    }
  else
    {
      QNetworkReply *reply = m_imageManager->get(QNetworkRequest(url));

      if(!reply)
	{
	  m_imageBuffer.close();
	  return;
	}

      connect(reply, SIGNAL(readyRead(void)),
	      this, SLOT(slotReadyRead(void)));
      connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
	      this, SLOT(slotDataTransferProgress(qint64, qint64)));
      connect(reply, SIGNAL(finished(void)),
	      this, SLOT(slotDownloadFinished(void)));
    }

  m_httpProgress->reset(); // Qt 5.5.x adjustment.
  m_httpProgress->setMaximum(0);
  m_httpProgress->setMinimum(0);
  m_httpProgress->show();

  if(pb == id.dwnldFront)
    {
      m_httpProgress->setWindowTitle
	(tr("BiblioteQ: Front Cover Image Download"));
      m_httpProgress->setLabelText(tr("Downloading the front cover image. "
				      "Please be patient..."));
    }
  else
    {
      m_httpProgress->setWindowTitle
	(tr("BiblioteQ: Back Cover Image Download"));
      m_httpProgress->setLabelText(tr("Downloading the back cover image. "
				      "Please be patient..."));
    }
}

/*
** -- slotDownloadFinished() --
*/

void biblioteq_book::slotDownloadFinished(bool error)
{
  Q_UNUSED(error);
  downloadFinished();
}

/*
** -- slotDownloadFinished() --
*/

void biblioteq_book::slotDownloadFinished(void)
{
  QNetworkReply *reply = qobject_cast<QNetworkReply *> (sender());

  if(reply)
    reply->deleteLater();

  downloadFinished();
}

/*
** -- downloadFinished() --
*/

void biblioteq_book::downloadFinished(void)
{
  if(m_httpProgress->isVisible())
    {
      m_httpProgress->reset(); // Qt 5.5.x adjustment.
      m_httpProgress->close();
    }

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
	}
    }

  m_imageBuffer.close();
}

/*
** -- slotDataTransferProgress() --
*/

void biblioteq_book::slotDataTransferProgress(qint64 bytesread,
					      qint64 totalbytes)
{
  if(m_httpProgress->isVisible())
    {
      m_httpProgress->setMaximum(static_cast<int> (totalbytes));
      m_httpProgress->setValue(static_cast<int> (bytesread));
    }
}

/*
** -- slotCancelImageDownload() --
*/

void biblioteq_book::slotCancelImageDownload(void)
{
  if(useHttp())
    {
#if QT_VERSION < 0x050000
      m_imageHttp->abort();
#endif
    }
  else
    {
      QNetworkReply *reply = m_imageManager->findChild<QNetworkReply *> ();

      if(reply)
	reply->abort();
    }

  m_imageBuffer.close();
}

/*
** -- slotReadyRead() --
*/

#if QT_VERSION < 0x050000
void biblioteq_book::slotReadyRead(const QHttpResponseHeader &resp)
{
  Q_UNUSED(resp);
  m_imageBuffer.write(m_imageHttp->readAll());
}
#endif

/*
** -- slotReadyRead() --
*/

void biblioteq_book::slotReadyRead(void)
{
  QNetworkReply *reply = qobject_cast<QNetworkReply *> (sender());

  if(reply)
    m_imageBuffer.write(reply->readAll());
}

/*
** -- duplicate() --
*/

void biblioteq_book::duplicate(const QString &p_oid, const int state)
{
  modify(state); // Initial population.
  id.attach_files->setEnabled(false);
  id.copiesButton->setEnabled(false);
  id.delete_files->setEnabled(false);
  id.export_files->setEnabled(false);
  id.showUserButton->setEnabled(false);
  m_oid = p_oid;
  setWindowTitle(tr("BiblioteQ: Duplicate Book Entry"));
  m_engWindowTitle = "Create";
}

/*
** -- slotProxyAuthenticationRequired() --
*/

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

      if(m_proxyDialog->exec() == QDialog::Accepted)
	{
	  authenticator->setUser(ui_p.usernameLineEdit->text());
	  authenticator->setPassword(ui_p.passwordLineEdit->text());
	}
    }
}

/*
** -- changeEvent() --
*/

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

/*
** -- slotSRUDownloadFinished() --
*/

void biblioteq_book::slotSRUDownloadFinished(bool error)
{
  Q_UNUSED(error);
  sruDownloadFinished();
}

/*
** -- slotSRUDownloadFinished() --
*/

void biblioteq_book::slotSRUDownloadFinished(void)
{
  QNetworkReply *reply = qobject_cast<QNetworkReply *> (sender());

  if(reply)
    reply->deleteLater();

  sruDownloadFinished();
}

/*
** -- sruDownloadFinished() --
*/

void biblioteq_book::sruDownloadFinished(void)
{
  m_sruWorking->reset(); // Qt 5.5.x adjustment.
  m_sruWorking->close();
  update();

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
	  id.edition->setCurrentIndex(0);
	  id.edition->setStyleSheet
	    ("background-color: rgb(162, 205, 90)");
	  id.marc_tags->setText(m_sruResults);
	  biblioteq_misc_functions::highlightWidget
	    (id.marc_tags->viewport(), QColor(162, 205, 90));

	  QString str("");
	  bool isbn10User = false;
	  bool isbn13User = false;

	  if(id.id->text().trimmed().length() == 10)
	    isbn10User = true;

	  if(id.isbn13->text().trimmed().length() == 13)
	    isbn13User = true;

	  QXmlStreamReader reader(m_sruResults);

	  while(!reader.atEnd())
	    if(reader.readNextStartElement())
	      if(reader.name().toString().toLower().
		 trimmed() == "datafield")
		{
		  QString tag(reader.attributes().value("tag").
			      toString().toLower().trimmed());

		  if(tag == "100" || tag == "700")
		    id.author->clear();
		  else if(tag == "260")
		    id.place->clear();
		  else if(tag == "650")
		    id.category->clear();
		}

	  QString recordSyntax("MARC21");
	  biblioteq_marc m;

	  if(recordSyntax == "MARC21")
	    m.initialize(biblioteq_marc::BOOK, biblioteq_marc::SRU,
			 biblioteq_marc::MARC21);
	  else
	    m.initialize(biblioteq_marc::BOOK, biblioteq_marc::SRU,
			 biblioteq_marc::UNIMARC);

	  m.setData(m_sruResults);
	  str = m.author();

	  if(!str.isEmpty())
	    {
	      id.author->setPlainText(str);
	      biblioteq_misc_functions::highlightWidget
		(id.author->viewport(), QColor(162, 205, 90));
	    }

	  str = m.binding();

	  if(id.binding->findText(str, Qt::MatchFixedString) > -1)
	    id.binding->setCurrentIndex
	      (id.binding->findText(str, Qt::MatchFixedString));

	  id.binding->setStyleSheet
	    ("background-color: rgb(162, 205, 90)");
	  str = m.callnum();

	  if(!str.isEmpty())
	    {
	      id.callnum->setText(str);
	      biblioteq_misc_functions::highlightWidget
		(id.callnum, QColor(162, 205, 90));
	    }

	  str = m.category();

	  if(!str.isEmpty())
	    {
	      id.category->setPlainText(str);
	      biblioteq_misc_functions::highlightWidget
		(id.category->viewport(),
		 QColor(162, 205, 90));
	    }

	  str = m.description();

	  if(!str.isEmpty())
	    {
	      id.description->setPlainText(str);
	      biblioteq_misc_functions::highlightWidget
		(id.description->viewport(), QColor(162, 205, 90));
	    }

	  str = m.deweynum();

	  if(!str.isEmpty())
	    {
	      id.deweynum->setText(str);
	      biblioteq_misc_functions::highlightWidget
		(id.deweynum, QColor(162, 205, 90));
	    }

	  str = m.edition();

	  if(id.edition->findText(str) > -1)
	    id.edition->setCurrentIndex
	      (id.edition->findText(str));
	  else
	    id.edition->setCurrentIndex(0);

	  str = m.isbn10();

	  if(!isbn10User && !str.isEmpty())
	    {
	      id.id->setText(str);
	      biblioteq_misc_functions::highlightWidget
		(id.id, QColor(162, 205, 90));
	    }

	  str = m.isbn13();

	  if(!isbn13User && !str.isEmpty())
	    {
	      id.isbn13->setText(str);
	      biblioteq_misc_functions::highlightWidget
		(id.isbn13, QColor(162, 205, 90));
	    }

	  str = m.lcnum();

	  if(!str.isEmpty())
	    {
	      id.lcnum->setText(str);
	      biblioteq_misc_functions::highlightWidget
		(id.lcnum, QColor(162, 205, 90));
	    }

	  str = m.place();

	  if(!str.isEmpty())
	    {
	      id.place->setPlainText(str);
	      biblioteq_misc_functions::highlightWidget
		(id.place->viewport(), QColor(162, 205, 90));
	    }

	  if(!m.publicationDate().isNull())
	    {
	      id.publication_date->setDate
		(m.publicationDate());
	      id.publication_date->setStyleSheet
		("background-color: rgb(162, 205, 90)");
	    }

	  str = m.publisher();

	  if(!str.isEmpty())
	    {
	      id.publisher->setPlainText(str);
	      biblioteq_misc_functions::highlightWidget
		(id.publisher->viewport(), QColor(162, 205, 90));
	    }

	  str = m.title();

	  if(!str.isEmpty())
	    {
	      id.title->setText(str);
	      biblioteq_misc_functions::highlightWidget
		(id.title, QColor(162, 205, 90));
	    }

	  foreach(QLineEdit *textfield, findChildren<QLineEdit *>())
	    textfield->setCursorPosition(0);
	}
    }
  else if(records == 0)
    QMessageBox::critical
      (this, tr("BiblioteQ: SRU Query Error"),
       tr("An SRU entry may not yet exist for the provided ISBN(s)."));
}

/*
** -- slotSRUReadyRead() --
*/

#if QT_VERSION < 0x050000
void biblioteq_book::slotSRUReadyRead(const QHttpResponseHeader &resp)
{
  Q_UNUSED(resp);
  m_sruResults.append(m_sruHttp->readAll());
}
#endif

/*
** -- slotSRUReadyRead() --
*/

void biblioteq_book::slotSRUReadyRead(void)
{
  QNetworkReply *reply = qobject_cast<QNetworkReply *> (sender());

  if(reply)
    m_sruResults.append(reply->readAll());
}

/*
** -- slotSRUError() --
*/

void biblioteq_book::slotSRUError(QNetworkReply::NetworkError error)
{
  QNetworkReply *reply = qobject_cast<QNetworkReply *> (sender());

  if(reply)
    QMessageBox::critical
      (this, tr("BiblioteQ: SRU Query Error"),
       tr("A network error (%1) occurred.").arg(reply->errorString()));
  else
    QMessageBox::critical
      (this, tr("BiblioteQ: SRU Query Error"),
       tr("A network error (%1) occurred.").arg(error));
}

/*
** -- slotSRUSslErrors() --
*/

void biblioteq_book::slotSRUSslErrors(const QList<QSslError> &list)
{
  Q_UNUSED(list);
  QMessageBox::critical
    (this, tr("BiblioteQ: SRU Query Error"),
     tr("One or more SSL errors occurred. Please verify your settings."));
}

/*
** -- useHttp() --
*/

bool biblioteq_book::useHttp(void) const
{
#if QT_VERSION < 0x050000
#ifdef Q_OS_MAC
  if(QSysInfo::MacintoshVersion <= QSysInfo::MV_10_6)
    return true;
  else
    return false;
#else
  return false;
#endif
#else
  return false;
#endif
}

/*
** -- slotAttachFiles() --
*/

void biblioteq_book::slotAttachFiles(void)
{
  QFileDialog fileDialog(this, tr("BiblioteQ: Attach File(s)"));

  fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
  fileDialog.setDirectory(QDir::homePath());
  fileDialog.setFileMode(QFileDialog::ExistingFiles);

  if(fileDialog.exec() == QDialog::Accepted)
    {
      QApplication::setOverrideCursor(Qt::WaitCursor);

      QStringList files(fileDialog.selectedFiles());

      while(!files.isEmpty())
	{
	  QCryptographicHash digest(QCryptographicHash::Sha1);
	  QFile file;
	  QString fileName(files.takeFirst());

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

	      if(!bytes.isEmpty())
		{
		  total = qCompress(total, 9);
		  createFile(digest.result(), total,
			     QFileInfo(fileName).fileName());
		}
	    }

	  file.close();
	}

      populateFiles();
      QApplication::restoreOverrideCursor();
    }
}

/*
** -- createFile() --
*/

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
      qint64 value = biblioteq_misc_functions::getSqliteUniqueId
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

/*
** -- populateFiles() --
*/

void biblioteq_book::populateFiles(void)
{
  id.files->clearContents();
  id.files->setRowCount(0);

  QSqlQuery query(qmain->getDB());

  query.setForwardOnly(true);
  query.prepare("SELECT COUNT(*) FROM book_files WHERE item_oid = ?");
  query.bindValue(0, m_oid);

  if(query.exec())
    if(query.next())
      id.files->setRowCount(query.value(0).toInt());

  query.prepare("SELECT file_name, "
		"file_digest, "
		"LENGTH(file), "
		"description, "
		"myoid FROM book_files "
                "WHERE item_oid = ? ORDER BY file_name");
  query.bindValue(0, m_oid);
  QApplication::setOverrideCursor(Qt::WaitCursor);

  int row = 0;
  int totalRows = 0;

  if(query.exec())
    while(query.next() && totalRows < id.files->rowCount())
      {
	totalRows += 1;

	for(int i = 0; i < query.record().count() - 1; i++)
	  {
	    QTableWidgetItem *item = new(std::nothrow)
	      QTableWidgetItem(query.value(i).toString());

	    if(!item)
	      continue;

	    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	    id.files->setItem(row, i, item);
	  }

	row += 1;
      }

  id.files->setRowCount(totalRows);
  QApplication::restoreOverrideCursor();
}

/*
** -- slotDeleteFiles() --
*/

void biblioteq_book::slotDeleteFiles(void)
{
}
