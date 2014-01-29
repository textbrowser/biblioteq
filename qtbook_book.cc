/*
** -- Qt Includes --
*/

#include <QSqlField>
#include <QSqlRecord>
#include <QNetworkProxy>
#include <QAuthenticator>
#include <QXmlStreamReader>

/*
** Includes book-specific methods.
*/

/*
** -- Local Includes --
*/

#include "qtbook.h"
#include "qtbook_book.h"
#include "borrowers_editor.h"

extern qtbook *qmain;
extern QApplication *qapp;

/*
** -- qtbook_book() --
*/

qtbook_book::qtbook_book(QMainWindow *parentArg,
			 const QString &oidArg,
			 const int rowArg):
  QMainWindow()
{
  QMenu *menu = 0;
  QGraphicsScene *scene1 = 0;
  QGraphicsScene *scene2 = 0;

  if((menu = new(std::nothrow) QMenu(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene1 = new(std::nothrow) QGraphicsScene(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene2 = new(std::nothrow) QGraphicsScene(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((httpProgress = new(std::nothrow) qtbook_item_working_dialog(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((m_proxyDialog = new(std::nothrow) QDialog(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  ui_p.setupUi(m_proxyDialog);
#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  m_proxyDialog->setAttribute(Qt::WA_MacMetalStyle, true);
#endif
#endif
  parentWid = parentArg;
  oid = oidArg;
  row = rowArg;
  oldq = misc_functions::getColumnString
    (qmain->getUI().table, row,
     qmain->getUI().table->columnNumber("Quantity")).toInt();
  id.setupUi(this);
#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  setAttribute(Qt::WA_MacMetalStyle, true);
#endif
#endif
  updateFont(qapp->font(), static_cast<QWidget *> (this));
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
  connect(id.isbnAvailableCheckBox,
	  SIGNAL(toggled(bool)),
	  id.id,
	  SLOT(setEnabled(bool)));
  connect(id.isbnAvailableCheckBox,
	  SIGNAL(toggled(bool)),
	  id.isbn13,
	  SLOT(setEnabled(bool)));
  connect(id.isbnAvailableCheckBox,
	  SIGNAL(toggled(bool)),
	  id.isbn10to13,
	  SLOT(setEnabled(bool)));
  connect(id.isbnAvailableCheckBox,
	  SIGNAL(toggled(bool)),
	  id.isbn13to10,
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
  connect(httpProgress, SIGNAL(canceled(void)), this,
	  SLOT(slotCancelImageDownload(void)));
  connect(httpProgress, SIGNAL(rejected(void)), this,
	  SLOT(slotCancelImageDownload(void)));
  id.resetButton->setMenu(menu);
  connect(id.isbnAvailableCheckBox,
	  SIGNAL(toggled(bool)),
	  menu->actions()[2],
	  SLOT(setEnabled(bool)));
  connect(id.isbnAvailableCheckBox,
	  SIGNAL(toggled(bool)),
	  menu->actions()[3],
	  SLOT(setEnabled(bool)));

  QString errorstr("");

  qapp->setOverrideCursor(Qt::WaitCursor);
  id.binding->addItems
    (misc_functions::getBookBindingTypes(qmain->getDB(),
					 errorstr));
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the book binding types.")),
       errorstr, __FILE__, __LINE__);

  qapp->setOverrideCursor(Qt::WaitCursor);
  id.language->addItems
    (misc_functions::getLanguages(qmain->getDB(),
				  errorstr));
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the languages.")),
       errorstr, __FILE__, __LINE__);

  qapp->setOverrideCursor(Qt::WaitCursor);
  id.monetary_units->addItems
    (misc_functions::getMonetaryUnits(qmain->getDB(),
				      errorstr));
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the monetary units.")),
       errorstr, __FILE__, __LINE__);

  qapp->setOverrideCursor(Qt::WaitCursor);
  id.location->addItems
    (misc_functions::getLocations(qmain->getDB(),
				  "Book",
				  errorstr));
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the book locations.")),
       errorstr, __FILE__, __LINE__);

  id.front_image->setScene(scene1);
  id.back_image->setScene(scene2);
  httpProgress->setModal(true);

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
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((actionGroup2 = new(std::nothrow) QActionGroup(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  bool found = false;
  QMap<QString, QHash<QString, QString> > hashes(qmain->getSRUMaps());

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

  cb_orig_ss = id.edition->styleSheet();
  dt_orig_ss = id.publication_date->styleSheet();
  white_pal = te_orig_pal = id.author->viewport()->palette();

  /*
  ** Prepare the form.
  */

  resize(0.95 * parentWid->size().width(),
	 0.95 * parentWid->size().height());
  id.splitter->setStretchFactor(0, 0);
  id.splitter->setStretchFactor(1, 1);
  id.splitter->setStretchFactor(2, 0);
  misc_functions::center(this, parentWid);
  misc_functions::hideAdminFields(this, qmain->getRoles());
}

/*
** -- ~qtbook_book() --
*/

qtbook_book::~qtbook_book()
{
}

/*
** -- slotGo() --
*/

void qtbook_book::slotGo(void)
{
  int i = 0;
  int newq = 0;
  int maxcopynumber = 0;
  QString str = "";
  QString errorstr = "";
  QString searchstr = "";
  QSqlQuery query(qmain->getDB());

  if(engWindowTitle.contains("Create") ||
     engWindowTitle.contains("Modify"))
    {
      if(engWindowTitle.contains("Modify") && row > -1)
	{
	  newq = id.quantity->value();
	  qapp->setOverrideCursor(Qt::WaitCursor);
	  maxcopynumber = misc_functions::getMaxCopyNumber
	    (qmain->getDB(), oid, "Book", errorstr);

	  if(maxcopynumber < 0)
	    {
	      qapp->restoreOverrideCursor();
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

	  qapp->restoreOverrideCursor();

	  if(newq < maxcopynumber)
	    {
	      QMessageBox::critical
		(this, tr("BiblioteQ: User Error"),
		 tr("It appears that you are attempting to "
		    "decrease the "
		    "number of copies while there are "
		    "copies "
		    "that have been reserved."));
	      id.quantity->setValue(oldq);
	      return;
	    }
	  else if(newq > oldq)
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
	if(id.id->text().length() == 10)
	  slotConvertISBN10to13();

      if(id.isbnAvailableCheckBox->isChecked())
	if(id.isbn13->text().length() == 13)
	  slotConvertISBN13to10();

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

      qapp->setOverrideCursor(Qt::WaitCursor);

      if(!qmain->getDB().transaction())
	{
	  qapp->restoreOverrideCursor();
	  qmain->addError
	    (QString(tr("Database Error")),
	     QString(tr("Unable to create a database transaction.")),
	     qmain->getDB().lastError().text(), __FILE__, __LINE__);
	  QMessageBox::critical
	    (this, tr("BiblioteQ: Database Error"),
	     tr("Unable to create a database transaction."));
	  return;
	}

      qapp->restoreOverrideCursor();
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

      if(engWindowTitle.contains("Modify"))
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

      if(!id.front_image->image.isNull())
	{
	  QByteArray bytes;
	  QBuffer buffer(&bytes);

	  buffer.open(QIODevice::WriteOnly);
	  id.front_image->image.save
	    (&buffer, id.front_image->imageFormat.toLatin1(), 100);
	  query.bindValue(18, bytes.toBase64());
	}
      else
	{
	  id.front_image->imageFormat = "";
	  query.bindValue(18, QVariant());
	}

      if(!id.back_image->image.isNull())
	{
	  QByteArray bytes;
	  QBuffer buffer(&bytes);

	  buffer.open(QIODevice::WriteOnly);
	  id.back_image->image.save
	    (&buffer, id.back_image->imageFormat.toLatin1(), 100);
	  query.bindValue(19, bytes.toBase64());
	}
      else
	{
	  id.back_image->imageFormat = "";
	  query.bindValue(19, QVariant());
	}

      query.bindValue(20, id.place->toPlainText().trimmed());
      query.bindValue(21, id.marc_tags->toPlainText().trimmed());
      query.bindValue(22, id.keyword->toPlainText().trimmed());
      query.bindValue(23, id.originality->currentText().trimmed());
      query.bindValue(24, id.condition->currentText().trimmed());

      if(engWindowTitle.contains("Modify"))
	query.bindValue(25, oid);
      else if(qmain->getDB().driverName() == "QSQLITE")
	{
	  qint64 value = misc_functions::getSqliteUniqueId(qmain->getDB(),
							   errorstr);

	  if(errorstr.isEmpty())
	    {
	      query.bindValue(25, value);
	      oid = QString::number(value);
	    }
	  else
	    qmain->addError(QString(tr("Database Error")),
			    QString(tr("Unable to generate a unique "
				       "integer.")),
			    errorstr);
	}

      qapp->setOverrideCursor(Qt::WaitCursor);

      if(!query.exec())
	{
	  qapp->restoreOverrideCursor();
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

	  if(engWindowTitle.contains("Modify"))
	    {
	      query.prepare(QString("DELETE FROM book_copy_info WHERE "
				    "copy_number > ? AND "
				    "item_oid = "
				    "?"));
	      query.bindValue(0, id.quantity->text());
	      query.bindValue(1, oid);

	      if(!query.exec())
		{
		  qapp->restoreOverrideCursor();
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
		  qapp->restoreOverrideCursor();
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
		  oid = query.next();
		  oid = query.value(0).toString();
		}

	      if(id.id->text().isEmpty())
		misc_functions::createInitialCopies(oid,
						    id.quantity->value(),
						    qmain->getDB(),
						    "Book", errorstr);
	      else
		misc_functions::createInitialCopies(id.id->text(),
						    id.quantity->value(),
						    qmain->getDB(),
						    "Book", errorstr);

	      if(!errorstr.isEmpty())
		{
		  qapp->restoreOverrideCursor();
		  qmain->addError
		    (QString(tr("Database Error")),
		     QString(tr("Unable to create initial copies.")),
		     errorstr, __FILE__, __LINE__);
		  goto db_rollback;
		}

	      if(!qmain->getDB().commit())
		{
		  qapp->restoreOverrideCursor();
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

	  id.id->setPalette(te_orig_pal);
	  id.isbn13->setPalette(te_orig_pal);
	  id.edition->setStyleSheet(cb_orig_ss);
	  id.binding->setStyleSheet(cb_orig_ss);
	  id.category->viewport()->setPalette(te_orig_pal);
	  id.publication_date->setStyleSheet(dt_orig_ss);
	  id.author->viewport()->setPalette(te_orig_pal);
	  id.title->setPalette(te_orig_pal);
	  id.description->viewport()->setPalette(te_orig_pal);
	  id.marc_tags->viewport()->setPalette(white_pal);
	  id.keyword->viewport()->setPalette(white_pal);
	  id.publisher->viewport()->setPalette(te_orig_pal);
	  id.place->viewport()->setPalette(te_orig_pal);
	  id.lcnum->setPalette(white_pal);
	  id.callnum->setPalette(white_pal);
	  id.deweynum->setPalette(white_pal);
	  oldq = id.quantity->value();

	  if(id.front_image->image.isNull())
	    id.front_image->imageFormat = "";

	  if(id.back_image->image.isNull())
	    id.back_image->imageFormat = "";

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
	  qapp->restoreOverrideCursor();

	  if(engWindowTitle.contains("Modify"))
	    {
	      if(!id.id->text().isEmpty())
		str = QString(tr("BiblioteQ: Modify Book Entry (")) +
		  id.id->text() + tr(")");
	      else
		str = tr("BiblioteQ: Modify Book Entry");

	      engWindowTitle = "Modify";
	      setWindowTitle(str);

	      if((qmain->getTypeFilterString() == "All" ||
		  qmain->getTypeFilterString() == "All Available" ||
		  qmain->getTypeFilterString() == "All Overdue" ||
		  qmain->getTypeFilterString() == "All Requested" ||
		  qmain->getTypeFilterString() == "All Reserved" ||
		  qmain->getTypeFilterString() == "Books") &&
		 oid == misc_functions::getColumnString
		 (qmain->getUI().table,
		  row, qmain->getUI().table->columnNumber("MYOID")) &&
		 misc_functions::getColumnString
		 (qmain->getUI().table,
		  row, qmain->getUI().table->columnNumber("Type")) == "Book")
		{
		  qmain->getUI().table->setSortingEnabled(false);

		  QStringList names(qmain->getUI().table->columnNames());

		  for(i = 0; i < names.size(); i++)
		    {
		      if(names.at(i) == "ISBN-10" ||
			 names.at(i) == "ID Number")
			qmain->getUI().table->item(row, i)->setText
			  (id.id->text());
		      else if(names.at(i) == "Title")
			qmain->getUI().table->item(row, i)->setText
			  (id.title->text());
		      else if(names.at(i) == "Edition")
			qmain->getUI().table->item(row, i)->setText
			  (id.edition->currentText().trimmed());
		      else if(names.at(i) == "Author(s)")
			qmain->getUI().table->item(row, i)->setText
			  (id.author->toPlainText());
		      else if(names.at(i) == "Publication Date")
			qmain->getUI().table->item(row, i)->setText
			  (id.publication_date->date().toString
			   (Qt::ISODate));
		      else if(names.at(i) == "Publisher")
			qmain->getUI().table->item(row, i)->setText
			  (id.publisher->toPlainText());
		      else if(names.at(i) == "Place of Publication")
			qmain->getUI().table->item(row, i)->setText
			  (id.place->toPlainText());
		      else if(names.at(i) == "Categories")
			qmain->getUI().table->item(row, i)->setText
			  (id.category->toPlainText());
		      else if(names.at(i) == "Price")
			qmain->getUI().table->item(row, i)->setText
			  (id.price->cleanText());
		      else if(names.at(i) == "Language")
			qmain->getUI().table->item(row, i)->setText
			  (id.language->currentText().trimmed());
		      else if(names.at(i) == "Monetary Units")
			qmain->getUI().table->item(row, i)->setText
			  (id.monetary_units->currentText().trimmed());
		      else if(names.at(i) == "Quantity")
			qmain->getUI().table->item(row, i)->setText
			  (id.quantity->text());
		      else if(names.at(i) == "Book Binding Type")
			qmain->getUI().table->item(row, i)->setText
			  (id.binding->currentText().trimmed());
		      else if(names.at(i) == "Location")
			qmain->getUI().table->item(row, i)->setText
			  (id.location->currentText().trimmed());
		      else if(names.at(i) == "ISBN-13")
			qmain->getUI().table->item(row, i)->setText
			  (id.isbn13->text());
		      else if(names.at(i) == "LC Control Number")
			qmain->getUI().table->item(row, i)->setText
			  (id.lcnum->text());
		      else if(names.at(i) == "Call Number")
			qmain->getUI().table->item(row, i)->setText
			  (id.callnum->text());
		      else if(names.at(i) == "Dewey Class Number")
			qmain->getUI().table->item(row, i)->setText
			  (id.deweynum->text());
		      else if(names.at(i) == "Originality")
			qmain->getUI().table->item(row, i)->setText
			  (id.originality->currentText().trimmed());
		      else if(names.at(i) == "Condition")
			qmain->getUI().table->item(row, i)->setText
			  (id.condition->currentText().trimmed());
		      else if(names.at(i) == "Availability")
			{
			  qmain->getUI().table->item(row, i)->setText
			    (misc_functions::getAvailability
			     (oid, qmain->getDB(), "Book",
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
	      qmain->updateSceneItem(oid, "Book", id.front_image->image);
	    }
	  else
	    {
	      qmain->replaceBook(oid, this);
	      updateWindow(qtbook::EDITABLE);

	      if(qmain->getUI().actionAutoPopulateOnCreation->isChecked())
		(void) qmain->populateTable
		  (qtbook::POPULATE_ALL, "Books", QString(""));

	      raise();
	    }

	  storeData(this);
	}

      return;

    db_rollback:

      qapp->setOverrideCursor(Qt::WaitCursor);

      if(!qmain->getDB().rollback())
	qmain->addError
	  (QString(tr("Database Error")), QString(tr("Rollback failure.")),
	   qmain->getDB().lastError().text(), __FILE__, __LINE__);

      qapp->restoreOverrideCursor();
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

      if(!engWindowTitle.isEmpty())
	if(!id.id->text().trimmed().isEmpty())
	  searchstr.append("id LIKE '%" + id.id->text().trimmed() +
			   "%' AND ");

      QString E("");

      if(qmain->getDB().driverName() != "QSQLITE")
	E = "E";

      searchstr.append("title LIKE " + E + "'%" +
		       myqstring::escape(id.title->text().trimmed()) +
		       "%' AND ");

      if(!engWindowTitle.isEmpty())
	if(!id.isbn13->text().trimmed().isEmpty())
	  searchstr.append("isbn13 LIKE '%" +
			   id.isbn13->text().trimmed() + "%' AND ");

      searchstr.append("COALESCE(lccontrolnumber, '') LIKE " + E + "'%" +
		       myqstring::escape(id.lcnum->text().trimmed()) +
		       "%' AND ");
      searchstr.append("COALESCE(callnumber, '') LIKE " + E + "'%" +
		       myqstring::escape(id.callnum->text().trimmed()) +
		       "%' AND ");
      searchstr.append("COALESCE(deweynumber, '') LIKE " + E + "'%" +
		       myqstring::escape(id.deweynum->text().trimmed()) +
		       "%' AND ");

      if(id.edition->currentIndex() != 0)
	searchstr.append("edition = '" +
			 id.edition->currentText().trimmed() +
			 "' AND ");

      searchstr.append("author LIKE " + E + "'%" +
		       myqstring::escape(id.author->toPlainText().
					 trimmed()) + "%' AND ");

      if(id.publication_date->date().toString("MM/yyyy") != "01/7999")
	searchstr.append("SUBSTR(pdate, 1, 3) || SUBSTR(pdate, 7) = '" +
			 id.publication_date->date().toString
			 ("MM/yyyy") +
			 "' AND ");

      searchstr.append("publisher LIKE " + E + "'%" +
		       myqstring::escape
		       (id.publisher->toPlainText().trimmed()) +
		       "%' AND ");
      searchstr.append("place LIKE " + E + "'%" +
		       myqstring::escape
		       (id.place->toPlainText().trimmed()) +
		       "%' AND ");
      searchstr.append("category LIKE " + E + "'%" +
		       myqstring::escape
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
			 myqstring::escape(id.language->currentText().
					   trimmed()) +
			 "' AND ");

      if(id.monetary_units->currentIndex() != 0)
	searchstr.append("monetary_units = " + E + "'" +
			 myqstring::escape
			 (id.monetary_units->currentText().trimmed()) +
			 "' AND ");

      if(id.binding->currentIndex() != 0)
	searchstr.append("binding_type = " + E + "'" +
			 myqstring::escape(id.binding->currentText().
					   trimmed()) +
			 "' AND ");

      searchstr.append("description LIKE " + E + "'%" +
		       myqstring::escape
		       (id.description->toPlainText().trimmed()) +
		       "%' ");

      if(id.quantity->value() != 0)
	searchstr.append("AND quantity = " + id.quantity->text() + " ");

      if(id.location->currentIndex() != 0)
	searchstr.append("AND location = " + E + "'" +
			 myqstring::escape
			 (id.location->currentText().trimmed()) + "' ");

      searchstr.append("AND COALESCE(marc_tags, '') LIKE " + E + "'%" +
		       myqstring::escape
		       (id.marc_tags->toPlainText().trimmed()) +
		       "%' ");
      searchstr.append("AND COALESCE(keyword, '') LIKE " + E + "'%" +
		       myqstring::escape
		       (id.keyword->toPlainText().trimmed()) +
		       "%' ");

      if(id.originality->currentIndex() != 0)
	searchstr.append("AND originality = " + E + "'" +
			 myqstring::escape
			 (id.originality->currentText().trimmed()) + "' ");

      if(id.condition->currentIndex() != 0)
	searchstr.append("AND condition = " + E + "'" +
			 myqstring::escape
			 (id.condition->currentText().trimmed()) + "' ");

      /*
      ** Search the database.
      */

      (void) qmain->populateTable
	(qtbook::POPULATE_SEARCH, "Books", searchstr);
    }
}

/*
** -- search() --
*/

void qtbook_book::search(const QString &field, const QString &value)
{
  id.coverImages->setVisible(false);
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

      actions[0]->setVisible(false);
      actions[1]->setVisible(false);
      actions.clear();
      setWindowTitle(tr("BiblioteQ: Database Book Search"));
      engWindowTitle = "Search";
      id.id->setFocus();
      misc_functions::center(this, parentWid);
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

void qtbook_book::updateWindow(const int state)
{
  QString str = "";

  if(state == qtbook::EDITABLE)
    {
      id.copiesButton->setEnabled(true);
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

      engWindowTitle = "Modify";
    }
  else
    {
      id.isbnAvailableCheckBox->setCheckable(false);
      id.copiesButton->setVisible(false);
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

      engWindowTitle = "View";
    }

  id.coverImages->setVisible(true);
  setWindowTitle(str);
}

/*
** -- modify() --
*/

void qtbook_book::modify(const int state)
{
  int i = 0;
  QString str = "";
  QString fieldname = "";
  QString searchstr = "";
  QVariant var;
  QSqlQuery query(qmain->getDB());

  if(state == qtbook::EDITABLE)
    {
      setWindowTitle(tr("BiblioteQ: Modify Book Entry"));
      engWindowTitle = "Modify";
      id.copiesButton->setEnabled(true);
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
      misc_functions::highlightWidget
	(id.id, QColor(255, 248, 220));
      misc_functions::highlightWidget
	(id.isbn13, QColor(255, 248, 220));
      misc_functions::highlightWidget
	(id.title, QColor(255, 248, 220));
      misc_functions::highlightWidget
	(id.publisher->viewport(), QColor(255, 248, 220));
      misc_functions::highlightWidget
	(id.place->viewport(), QColor(255, 248, 220));
      misc_functions::highlightWidget
	(id.author->viewport(), QColor(255, 248, 220));
      misc_functions::highlightWidget
	(id.description->viewport(), QColor(255, 248, 220));
      misc_functions::highlightWidget
	(id.category->viewport(), QColor(255, 248, 220));
      te_orig_pal = id.id->palette();
    }
  else
    {
      id.isbnAvailableCheckBox->setCheckable(false);
      setWindowTitle(tr("BiblioteQ: View Book Details"));
      engWindowTitle = "View";
      id.copiesButton->setVisible(false);
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

      actions[0]->setVisible(false);
      actions[1]->setVisible(false);
      actions.clear();
    }

  id.quantity->setMinimum(1);
  id.price->setMinimum(0.00);
  id.okButton->setText(tr("&Save"));
  str = oid;
  searchstr = "SELECT title, "
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
    "FROM book WHERE myoid = ";
  searchstr.append(str);
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec(searchstr) || !query.next())
    {
      qapp->restoreOverrideCursor();
      qmain->addError
	(QString(tr("Database Error")),
	 QString(tr("Unable to retrieve the selected book's data.")),
	 query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical
	(this, tr("BiblioteQ: Database Error"),
	 tr("Unable to retrieve the selected book's data."));
      return;
    }
  else
    {
      qapp->restoreOverrideCursor();
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
	      if(state == qtbook::EDITABLE)
		{
		  if(!var.toString().trimmed().isEmpty())
		    str = QString(tr("BiblioteQ: Modify Book Entry (")) +
		      var.toString() + tr(")");
		  else
		    str = tr("BiblioteQ: Modify Book Entry");

		  engWindowTitle = "Modify";
		}
	      else
		{
		  if(!var.toString().trimmed().isEmpty())
		    str = QString(tr("BiblioteQ: View Book Details (")) +
		      var.toString() + tr(")");
		  else
		    str = tr("BiblioteQ: View Book Details");

		  engWindowTitle = "View";
		}

	      id.id->setText(var.toString());
	      setWindowTitle(str);

	      if(query.record().isNull(i))
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

		  if(id.front_image->image.isNull())
		    id.front_image->loadFromData(var.toByteArray());
		}
	    }
	  else if(fieldname == "back_cover")
	    {
	      if(!query.record().field(i).isNull())
		{
		  id.back_image->loadFromData
		    (QByteArray::fromBase64(var.toByteArray()));

		  if(id.back_image->image.isNull())
		    id.back_image->loadFromData(var.toByteArray());
		}
	    }
	}

      foreach(QLineEdit *textfield, findChildren<QLineEdit *>())
	textfield->setCursorPosition(0);

      storeData(this);
    }

  id.id->setFocus();
  raise();
}

/*
** -- insert() --
*/

void qtbook_book::insert(void)
{
  slotReset();
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
  misc_functions::highlightWidget
    (id.id, QColor(255, 248, 220));
  misc_functions::highlightWidget
    (id.isbn13, QColor(255, 248, 220));
  misc_functions::highlightWidget
    (id.title, QColor(255, 248, 220));
  misc_functions::highlightWidget
    (id.publisher->viewport(), QColor(255, 248, 220));
  misc_functions::highlightWidget
    (id.place->viewport(), QColor(255, 248, 220));
  misc_functions::highlightWidget
    (id.author->viewport(), QColor(255, 248, 220));
  misc_functions::highlightWidget
    (id.description->viewport(), QColor(255, 248, 220));
  misc_functions::highlightWidget
    (id.category->viewport(), QColor(255, 248, 220));
  te_orig_pal = id.id->palette();
  setWindowTitle(tr("BiblioteQ: Create Book Entry"));
  engWindowTitle = "Create";
  id.id->setFocus();
  storeData(this);
  show();
}

/*
** -- slotReset() --
*/

void qtbook_book::slotReset(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  if(action != 0)
    {
      QList<QAction *> actions = id.resetButton->menu()->actions();

      if(action == actions[0])
	id.front_image->clear();
      else if(action == actions[1])
	id.back_image->clear();
      else if(action == actions[2])
	{
	  id.id->clear();
	  id.id->setPalette(te_orig_pal);
	  id.id->setFocus();
	}
      else if(action == actions[3])
	{
	  id.isbn13->clear();
	  id.isbn13->setPalette(te_orig_pal);
	  id.isbn13->setFocus();
	}
      else if(action == actions[4])
	{
	  id.edition->setCurrentIndex(0);
	  id.edition->setStyleSheet(cb_orig_ss);
	  id.edition->setFocus();
	}
      else if(action == actions[5])
	{
	  if(!engWindowTitle.contains("Search"))
	    id.author->setPlainText("N/A");
	  else
	    id.author->clear();

	  id.author->viewport()->setPalette(te_orig_pal);
	  id.author->setFocus();
	}
      else if(action == actions[6])
	{
	  id.binding->setCurrentIndex(0);
	  id.binding->setStyleSheet(cb_orig_ss);
	  id.binding->setFocus();
	}
      else if(action == actions[7])
	{
	  id.lcnum->clear();
	  id.lcnum->setPalette(white_pal);
	  id.lcnum->setFocus();
	}
      else if(action == actions[8])
	{
	  id.callnum->clear();
	  id.callnum->setPalette(white_pal);
	  id.callnum->setFocus();
	}
      else if(action == actions[9])
	{
	  id.deweynum->clear();
	  id.deweynum->setPalette(white_pal);
	  id.deweynum->setFocus();
	}
      else if(action == actions[10])
	{
	  id.title->clear();
	  id.title->setPalette(te_orig_pal);
	  id.title->setFocus();
	}
      else if(action == actions[11])
	{
	  if(engWindowTitle.contains("Search"))
	    id.publication_date->setDate
	      (QDate::fromString("01/7999", "MM/yyyy"));
	  else
	    id.publication_date->setDate
	      (QDate::fromString("01/01/2000", "MM/dd/yyyy"));

	  id.publication_date->setStyleSheet(dt_orig_ss);
	  id.publication_date->setFocus();
	}
      else if(action == actions[12])
	{
	  if(!engWindowTitle.contains("Search"))
	    id.publisher->setPlainText("N/A");
	  else
	    id.publisher->clear();

	  id.publisher->viewport()->setPalette(te_orig_pal);
	  id.publisher->setFocus();
	}
      else if(action == actions[13])
	{
	  if(!engWindowTitle.contains("Search"))
	    id.place->setPlainText("N/A");
	  else
	    id.place->clear();

	  id.place->viewport()->setPalette(te_orig_pal);
	  id.place->setFocus();
	}
      else if(action == actions[14])
	{
	  if(!engWindowTitle.contains("Search"))
	    id.category->setPlainText("N/A");
	  else
	    id.category->clear();

	  id.category->viewport()->setPalette(te_orig_pal);
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
	  if(engWindowTitle.contains("Search"))
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
	  if(!engWindowTitle.contains("Search"))
	    id.description->setPlainText("N/A");
	  else
	    id.description->clear();

	  id.description->viewport()->setPalette(te_orig_pal);
	  id.description->setFocus();
	}
      else if(action == actions[23])
	{
	  id.marc_tags->clear();
	  id.marc_tags->viewport()->setPalette(white_pal);
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

      if(!engWindowTitle.contains("Search"))
	id.author->setPlainText("N/A");
      else
	id.author->clear();

      if(!engWindowTitle.contains("Search"))
	id.publisher->setPlainText("N/A");
      else
	id.publisher->clear();

      if(!engWindowTitle.contains("Search"))
	id.place->setPlainText("N/A");
      else
	id.place->clear();

      if(!engWindowTitle.contains("Search"))
	id.category->setPlainText("N/A");
      else
	id.category->clear();

      if(engWindowTitle.contains("Search"))
	id.publication_date->setDate(QDate::fromString("01/7999",
						       "MM/yyyy"));
      else
	id.publication_date->setDate(QDate::fromString("01/01/2000",
						       "MM/dd/yyyy"));

      id.quantity->setValue(id.quantity->minimum());

      if(!engWindowTitle.contains("Search"))
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

      if(engWindowTitle.contains("Search"))
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
      id.title->setPalette(te_orig_pal);
      id.lcnum->setPalette(white_pal);
      id.deweynum->setPalette(white_pal);
      id.callnum->setPalette(white_pal);
      id.id->setPalette(te_orig_pal);
      id.isbn13->setPalette(te_orig_pal);
      id.edition->setStyleSheet(cb_orig_ss);
      id.binding->setStyleSheet(cb_orig_ss);
      id.category->viewport()->setPalette(te_orig_pal);
      id.publication_date->setStyleSheet(dt_orig_ss);
      id.author->viewport()->setPalette(te_orig_pal);
      id.description->viewport()->setPalette(te_orig_pal);
      id.marc_tags->viewport()->setPalette(white_pal);
      id.keyword->viewport()->setPalette(white_pal);
      id.publisher->viewport()->setPalette(te_orig_pal);
      id.place->viewport()->setPalette(te_orig_pal);
      id.id->setFocus();
    }
}

/*
** -- slotConvertISBN10to13() --
*/

void qtbook_book::slotConvertISBN10to13(void)
{
  int i = 0;
  int arr[] = {1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3};
  int check = 0;
  int total = 0;
  QString str = "";
  QString numberstr = "";

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

void qtbook_book::slotConvertISBN13to10(void)
{
  int total = 0;
  QString z("");
  QString isbnnum(id.isbn13->text().trimmed().mid(3, 9));

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

void qtbook_book::closeEvent(QCloseEvent *e)
{
  if(engWindowTitle.contains("Create") ||
     engWindowTitle.contains("Modify"))
    if(hasDataChanged(this))
      if(QMessageBox::question(this, tr("BiblioteQ: Question"),
			       tr("You have unsaved data. Continue closing?"),
			       QMessageBox::Yes | QMessageBox::No,
			       QMessageBox::No) == QMessageBox::No)
	{
	  if(e)
	    e->ignore();

	  return;
	}

  qmain->removeBook(this);
  QMainWindow::closeEvent(e);
}

/*
** -- slotCancel() --
*/

void qtbook_book::slotCancel(void)
{
  close();
}

/*
** -- slotPopulateCopiesEditor() --
*/

void qtbook_book::slotPopulateCopiesEditor(void)
{
  copy_editor_book *copyeditor = 0;

  if((copyeditor = new(std::nothrow) copy_editor_book
      (static_cast<QWidget *> (this),
       static_cast<qtbook_item *> (this),
       false,
       id.quantity->value(), oid,
       id.quantity, font(),
       id.id->text().trimmed())) != 0)
    copyeditor->populateCopiesEditor();
}

/*
** -- slotShowUsers() --
*/

void qtbook_book::slotShowUsers(void)
{
  int state = 0;
  borrowers_editor *borrowerseditor = 0;

  if(!id.okButton->isHidden())
    state = qtbook::EDITABLE;
  else
    state = qtbook::VIEW_ONLY;

  if((borrowerseditor = new(std::nothrow) borrowers_editor
      (static_cast<QWidget *> (this), static_cast<qtbook_item *> (this),
       id.quantity->value(), oid, id.id->text().trimmed(), font(), "Book",
       state)) != 0)
    borrowerseditor->showUsers();
}

/*
** -- slotSRUQuery() --
*/

void qtbook_book::slotSRUQuery(void)
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

  QNetworkAccessManager *manager = findChild<QNetworkAccessManager *> ();

  if(manager)
    return;

  if(manager->findChild<QNetworkReply *> ())
    return;

  if((manager = new(std::nothrow) QNetworkAccessManager(this)) == 0)
    return;

  qtbook_item_working_dialog *working = 0;

  if((working = new(std::nothrow)
      qtbook_item_working_dialog(static_cast<QMainWindow *> (this))) == 0)
    {
      manager->deleteLater();
      return;
    }

  working->setObjectName("sru_dialog");
  working->setModal(true);
  working->setWindowTitle(tr("BiblioteQ: SRU Data Retrieval"));
  working->setLabelText(tr("Downloading information from the SRU "
			   "site. Please be patient..."));
  working->setMaximum(0);
  working->setMinimum(0);
  working->setCancelButton(0);
  working->show();
  working->update();

  bool found = false;
  QString name("");

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
  QHash<QString, QString> hash(qmain->getSRUMaps()[name]);

  searchstr = hash["url_isbn"];

  if(!id.id->text().trimmed().isEmpty())
    searchstr.replace("%1", id.id->text().trimmed());
  else
    searchstr.replace("%1", id.isbn13->text().trimmed());

  if(!id.isbn13->text().trimmed().isEmpty())
    searchstr.replace("%2", id.isbn13->text().trimmed());
  else
    searchstr.replace("%2", id.id->text().trimmed());

  QUrl url(QUrl::fromUserInput(searchstr));
  QString type("");
  QNetworkProxy proxy;

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
	  /*
	  ** This is required to resolve an odd error.
	  */

	  QNetworkReply *reply = manager->get
	    (QNetworkRequest(QUrl::fromUserInput("http://0.0.0.0")));

	  if(reply)
	    reply->deleteLater();
	}

      if(type == "http" || type == "socks5")
	{
	  if(type == "http")
	    proxy.setType(QNetworkProxy::HttpProxy);
	  else
	    proxy.setType(QNetworkProxy::Socks5Proxy);

	  quint16 port = 0;
	  QString host("");
	  QString user("");
	  QString password("");

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

	  manager->setProxy(proxy);
	}
      else if(type == "system")
	{
	  QNetworkProxyQuery query(url);
	  QList<QNetworkProxy> list
	    (QNetworkProxyFactory::systemProxyForQuery(query));

	  if(!list.isEmpty())
	    proxy = list.at(0);

	  manager->setProxy(proxy);
	}
    }

  QNetworkReply *reply = manager->get(QNetworkRequest(url));

  if(!reply)
    {
      manager->deleteLater();
      working->deleteLater();
    }
  else
    {
      m_sruResults.clear();
      connect(reply, SIGNAL(readyRead(void)),
	      this, SLOT(slotSRUReadyRead(void)));
      connect(reply, SIGNAL(finished(void)),
	      this, SLOT(slotSRUDownloadFinished(void)));
    }
}

/*
** -- slotZ3950Query() --
*/

void qtbook_book::slotZ3950Query(void)
{
  if(findChild<generic_thread *> ())
    return;

  int i = 0;
  int j = 0;
  QString str = "";
  QString etype = "";
  QString errorstr = "";
  QString searchstr = "";
  QStringList list;
  QStringList tmplist;
  qtbook_item_working_dialog working(static_cast<QMainWindow *> (this));

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

  if((thread = new(std::nothrow) generic_thread(this)) != 0)
    {
      working.setModal(true);
      working.setWindowTitle(tr("BiblioteQ: Z39.50 Data Retrieval"));
      working.setLabelText(tr("Downloading information from the Z39.50 "
			      "site. Please be patient..."));
      working.setMaximum(0);
      working.setMinimum(0);
      working.setCancelButton(0);
      working.show();
      working.update();

      QStringList isbns;

      isbns << id.id->text().trimmed()
	    << id.isbn13->text().trimmed();

      if(isbns.at(0).isEmpty())
	isbns.replace(0, isbns.at(1));
      else if(isbns.at(1).isEmpty())
	isbns.replace(1, isbns.at(0));

      searchstr = QString("@attr 1=7 @or %1 %2").arg(isbns.at(0)).
	arg(isbns.at(1));

      bool found = false;

      for(i = 0; i < id.z3950QueryButton->actions().size(); i++)
	if(id.z3950QueryButton->actions().at(i)->isChecked())
	  {
	    found = true;
	    thread->setZ3950Name
	      (id.z3950QueryButton->actions().at(i)->text());
	    break;
	  }

      if(!found)
	thread->setZ3950Name(qmain->getPreferredZ3950Site());

      thread->setType(generic_thread::Z3950_QUERY);
      thread->setZ3950SearchString(searchstr);
      thread->start();

      while(thread->isRunning())
	{
#ifndef Q_OS_MAC
	  qapp->processEvents();
#endif
	  thread->msleep(100);
	}

      working.hide();

      if((errorstr = thread->getErrorStr()).isEmpty() &&
	 !thread->getZ3950Results().isEmpty())
	{
	  if(QMessageBox::question
	     (this, tr("BiblioteQ: Question"),
	      tr("Replace existing values with those retrieved "
		 "from the Z39.50 site?"),
	      QMessageBox::Yes | QMessageBox::No,
	      QMessageBox::No) == QMessageBox::Yes)
	    {
	      list = QString(thread->getZ3950Results()[0]).split("\n");
	      id.edition->setCurrentIndex(0);
	      id.edition->setStyleSheet
		("background-color: rgb(162, 205, 90)");
	      id.marc_tags->setPlainText
		(thread->getZ3950Results()[0].trimmed());
	      misc_functions::highlightWidget
		(id.marc_tags->viewport(), QColor(162, 205, 90));

	      for(i = 0; i < list.size(); i++)
		if(list[i].startsWith("100 ") ||
		   list[i].startsWith("700 "))
		  id.author->clear();
		else if(list[i].startsWith("260 "))
		  id.place->clear();
		else if(list[i].startsWith("650 "))
		  id.category->clear();

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

		      str = str.mid(str.indexOf("$a") + 2).trimmed();

		      QStringList subfields;

		      subfields << "$b"
				<< "$z"
				<< "$8";

		      while(!subfields.isEmpty())
			if(str.contains(subfields.first()))
			  str = str.mid
			    (0, str.indexOf(subfields.takeFirst())).trimmed();
			else
			  subfields.takeFirst();

		      id.lcnum->setText(str);
		      misc_functions::highlightWidget
			(id.lcnum, QColor(162, 205, 90));
		    }
		  else if(str.startsWith("020 "))
		    {
		      /*
		      ** $a - International Standard Book Number (NR)
		      ** $c - Terms of availability (NR)
		      ** $z - Canceled/invalid ISBN (R)
		      ** $6 - Linkage (NR)
		      ** $8 - Field link and sequence number (R)
		      */

		      str = str.mid(str.indexOf("$a") + 2).trimmed();

		      if(str.toLower().contains("hardcover"))
			{
			  id.binding->setCurrentIndex(0);
			  id.binding->setStyleSheet
			    ("background-color: rgb(162, 205, 90)");
			}
		      else if(str.toLower().contains("pbk."))
			{
			  id.binding->setCurrentIndex(1);
			  id.binding->setStyleSheet
			    ("background-color: rgb(162, 205, 90)");
			}

		      if(str.contains(" ") && str.indexOf(" ") == 10)
			{
			  str = str.mid(0, 10).trimmed();
			  id.id->setText(str);
			  misc_functions::highlightWidget
			    (id.id, QColor(162, 205, 90));
			}
		      else if(str.contains(" ") && str.indexOf(" ") == 13)
			{
			  str = str.mid(0, 13).trimmed();
			  id.isbn13->setText(str);
			  misc_functions::highlightWidget
			    (id.isbn13, QColor(162, 205, 90));
			}
		      else if(str.length() == 10)
			{
			  id.id->setText(str);
			  misc_functions::highlightWidget
			    (id.id, QColor(162, 205, 90));
			}
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

		      str = str.mid(str.indexOf("$a") + 2).trimmed();
		      str = str.remove(" $b").trimmed();

		      QStringList subfields;

		      subfields << "$3"
				<< "$6"
				<< "$8";

		      while(!subfields.isEmpty())
			if(str.contains(subfields.first()))
			  str = str.mid
			    (0, str.indexOf(subfields.takeFirst())).trimmed();
			else
			  subfields.takeFirst();

		      id.callnum->setText(str);
		      misc_functions::highlightWidget
			(id.callnum, QColor(162, 205, 90));
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

		      str = str.mid(str.indexOf("$a") + 2).trimmed();
		      str = str.remove(" $b").remove
			(" $m").remove(" $q").trimmed();

		      QStringList subfields;

		      subfields << "$2"
				<< "$6"
				<< "$8";

		      while(!subfields.isEmpty())
			if(str.contains(subfields.first()))
			  str = str.mid
			    (0, str.indexOf(subfields.takeFirst())).trimmed();
			else
			  subfields.takeFirst();

		      id.deweynum->setText(str);
		      misc_functions::highlightWidget
			(id.deweynum, QColor(162, 205, 90));
		    }
		  else if(str.startsWith("100 ") ||
			  str.startsWith("700 "))
		    {
		      /*
		      ** $a - Personal name (NR)
		      ** $b - Numeration (NR)
		      ** $c - Titles and words associated with a name (R)
		      ** $d - Dates associated with a name (NR)
		      ** $e - Relator term (R)
		      ** $f - Date of a work (NR)
		      ** $g - Miscellaneous information (NR)
		      ** $j - Attribution qualifier (R)
		      ** $k - Form subheading (R)
		      ** $l - Language of a work (NR)
		      ** $n - Number of part/section of a work (R)
		      ** $p - Name of part/section of a work (R)
		      ** $q - Fuller form of name (NR)
		      ** $t - Title of a work (NR)
		      ** $u - Affiliation (NR)
		      ** $0 - Authority record control number (R)
		      ** $4 - Relator code (R)
		      ** $6 - Linkage (NR)
		      ** $8 - Field link and sequence number (R)
		      */

		      str = str.mid(str.indexOf("$a") + 2).trimmed();

		      QStringList subfields;

		      subfields << "$b"
				<< "$c"
				<< "$d"
				<< "$e"
				<< "$f"
				<< "$g"
				<< "$j"
				<< "$k"
				<< "$l"
				<< "$n"
				<< "$p"
				<< "$q"
				<< "$t"
				<< "$u"
				<< "$0"
				<< "$4"
				<< "$6"
				<< "$8";

		      while(!subfields.isEmpty())
			if(str.contains(subfields.first()))
			  str = str.mid
			    (0, str.indexOf(subfields.takeFirst())).trimmed();
			else
			  subfields.takeFirst();

		      if(str.endsWith(","))
			str = str.mid(0, str.length() - 1).trimmed();

		      if(!id.author->toPlainText().contains(str))
			{
			  if(list[i].startsWith("100 "))
			    id.author->setPlainText(str);
			  else if(!id.author->toPlainText().isEmpty())
			    id.author->setPlainText
			      (id.author->toPlainText() + "\n" +
			       str);
			  else
			    id.author->setPlainText(str);

			  misc_functions::highlightWidget
			    (id.author->viewport(), QColor(162, 205, 90));
			}
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

		      while(!subfields.isEmpty())
			if(str.contains(subfields.first()))
			  str = str.mid
			    (0, str.indexOf(subfields.takeFirst())).trimmed();
			else
			  subfields.takeFirst();

		      str = str.mid(0, str.lastIndexOf('/')).trimmed();
		      id.title->setText(str);
		      misc_functions::highlightWidget
			(id.title, QColor(162, 205, 90));
		    }
		  else if(str.startsWith("250 "))
		    {
		      /*
		      ** $a - Edition statement (NR)
		      ** $b - Remainder of edition statement (NR)
		      ** $6 - Linkage (NR)
		      ** $8 - Field link and sequence number (R)
		      */

		      str = str.mid(str.indexOf("$a") + 2).trimmed();

		      QStringList subfields;

		      subfields << "$b"
				<< "$6"
				<< "$8";

		      while(!subfields.isEmpty())
			if(str.contains(subfields.first()))
			  str = str.mid
			    (0, str.indexOf(subfields.takeFirst())).trimmed();
			else
			  subfields.takeFirst();

		      str = str.mid(0, str.indexOf(" ")).trimmed();

		      if(id.edition->findText(str) > -1)
			id.edition->setCurrentIndex
			  (id.edition->findText(str));
		      else
			{
			  str = str.remove("d").remove("h").remove("n").
			    remove("r").remove("s").remove("t").trimmed();

			  if(id.edition->findText(str) > -1)
			    id.edition->setCurrentIndex
			      (id.edition->findText(str));
			  else
			    id.edition->setCurrentIndex(0);
			}
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

		      QString tmpstr = str.mid(str.indexOf("$a") + 2).
			trimmed();

		      QStringList subfields;

		      subfields << "$b"
				<< "$c"
				<< "$e"
				<< "$f"
				<< "$g"
				<< "$3"
				<< "$6"
				<< "$8";

		      while(!subfields.isEmpty())
			if(tmpstr.contains(subfields.first()))
			  tmpstr = tmpstr.mid
			    (0, tmpstr.indexOf(subfields.takeFirst())).
			    trimmed();
			else
			  subfields.takeFirst();

		      tmplist = tmpstr.split("$a");

		      for(j = 0; j < tmplist.size(); j++)
			{
			  tmpstr = tmplist.at(j).trimmed();
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

			  if(id.place->toPlainText().isEmpty())
			    id.place->setPlainText(tmpstr);
			  else
			    id.place->setPlainText(id.place->toPlainText() +
						   "\n" + tmpstr);
			}

		      misc_functions::highlightWidget
			(id.place->viewport(), QColor(162, 205, 90));

		      if(str.mid(str.indexOf("$c") + 2, 4).toLower().
			 contains("c"))
			id.publication_date->setDate
			  (QDate::fromString
			   ("01/01/" +
			    str.mid(str.indexOf("$c") + 4, 4),
			    "MM/dd/yyyy"));
		      else
			id.publication_date->setDate
			  (QDate::fromString
			   ("01/01/" +
			    str.mid(str.indexOf("$c") + 3, 4),
			    "MM/dd/yyyy"));

		      id.publication_date->setStyleSheet
			("background-color: rgb(162, 205, 90)");

		      if(str.contains("$b"))
			str = str.mid(str.indexOf("$b") + 2).trimmed();

		      if(str.contains("$a"))
			{
			  str = str.mid(0, str.indexOf("$a")).trimmed();
			  str = str.mid(0, str.lastIndexOf(" ")).trimmed();
			}
		      else
			str = str.mid(0, str.indexOf("$c")).trimmed();

		      if(str.contains("$b "))
			str.remove("$b ");
		      else if(str.contains("$b"))
			str.remove("$b");

		      if(str.endsWith(","))
			str = str.mid(0, str.length() - 1).trimmed();

		      id.publisher->setPlainText(str);
		      misc_functions::highlightWidget
			(id.publisher->viewport(), QColor(162, 205, 90));
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

		      str = str.mid(str.indexOf("$a") + 2).trimmed();
		      str = str.remove(" $b").trimmed();
		      str = str.remove(" $c").trimmed();
		      str = str.remove(" $e").trimmed();
		      str = str.remove(" $f").trimmed();
		      str = str.remove(" $g").trimmed();
		      str = str.remove(" $3").trimmed();
		      str = str.remove(" $6").trimmed();
		      str = str.remove(" $8").trimmed();
		      id.description->setPlainText(str);
		      misc_functions::highlightWidget
			(id.description->viewport(), QColor(162, 205, 90));
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

		      while(!subfields.isEmpty())
			if(str.contains(subfields.first()))
			  str = str.mid
			    (0, str.indexOf(subfields.takeFirst())).trimmed();
			else
			  subfields.takeFirst();

		      if(!str.isEmpty())
			{
			  if(!str[str.length() - 1].isPunct())
			    str += ".";

			  if(!id.category->toPlainText().contains(str))
			    {
			      if(!id.category->toPlainText().isEmpty())
				id.category->setPlainText
				  (id.category->toPlainText() + "\n" +
				   str);
			      else
				id.category->setPlainText(str);

			      misc_functions::highlightWidget
				(id.category->viewport(),
				 QColor(162, 205, 90));
			    }
			}
		    }
		}

	      foreach(QLineEdit *textfield, findChildren<QLineEdit *>())
		textfield->setCursorPosition(0);
	    }
	}
      else if(errorstr.isEmpty() && thread->getZ3950Results().isEmpty())
	QMessageBox::critical
	  (this, tr("BiblioteQ: Z39.50 Query Error"),
	   tr("A Z39.50 entry may not yet exist for the provided ISBN(s)."));
      else
	etype = thread->getEType();

      thread->deleteLater();
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
	 tr("The Z39.50 entry could not be retrieved."));
    }
}

/*
** -- slotPrint() --
*/

void qtbook_book::slotPrint(void)
{
  html = "";
  html += "<b>" + tr("ISBN-10:") + "</b> " +
    id.id->text().trimmed() + "<br>";
  html += "<b>" + tr("ISBN-13:") + "</b> " +
    id.isbn13->text().trimmed() + "<br>";
  html += "<b>" + tr("Edition:") + "</b> " +
    id.edition->currentText() + "<br>";
  html += "<b>" + tr("Author(s):") + "</b> " +
    id.author->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Book Binding Type:") + "</b> " +
    id.binding->currentText() + "<br>";
  html += "<b>" + tr("LC Control Number:") + "</b> " +
    id.lcnum->text().trimmed() + "<br>";
  html += "<b>" + tr("Call Number:") + "</b> " +
    id.callnum->text().trimmed() + "<br>";
  html += "<b>" + tr("Dewey Class Number:") + "</b> " +
    id.deweynum->text().trimmed() + "<br>";

  /*
  ** General information.
  */

  html += "<b>" + tr("Title:") + "</b> " +
    id.title->text().trimmed() + "<br>";
  html += "<b>" + tr("Publication Date:") + "</b> " +
    id.publication_date->date().toString(Qt::ISODate) + "<br>";
  html += "<b>" + tr("Publisher:") + "</b> " +
    id.publisher->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Place of Publication:") + "</b> " +
    id.place->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Categories:") + "</b> " +
    id.category->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Price:") + "</b> " + id.price->cleanText() + "<br>";
  html += "<b>" + tr("Language:") + "</b> " +
    id.language->currentText() + "<br>";
  html += "<b>" + tr("Monetary Units:") + "</b> " +
    id.monetary_units->currentText() + "<br>";
  html += "<b>" + tr("Copies:") + "</b> " + id.quantity->text() + "<br>";
  html += "<b>" + tr("Location:") + "</b> " +
    id.location->currentText() + "<br>";
  html += "<b>" + tr("Originality:") + "</b> " +
    id.originality->currentText() + "<br>";
  html += "<b>" + tr("Condition:") + "</b> " +
    id.condition->currentText() + "<br>";
  html += "<b>" + tr("Abstract:") + "</b> " +
    id.description->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("MARC Tags:") + "</b> " +
    id.marc_tags->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Keywords:") + "</b> " +
    id.keyword->toPlainText().trimmed() + "<br>";
  print(this);
}

/*
** -- slotSelectImage() --
*/

void qtbook_book::slotSelectImage(void)
{
  QFileDialog dialog(this);
  QPushButton *button = qobject_cast<QPushButton *> (sender());

#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  dialog.setAttribute(Qt::WA_MacMetalStyle, true);
#endif
#endif
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setDirectory(QDir::homePath());
  dialog.setNameFilter("Image Files (*.bmp *.jpg *.jpeg *.png)");

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
	  id.front_image->image = QImage(dialog.selectedFiles().at(0));
	  id.front_image->imageFormat = dialog.selectedFiles().at(0).mid
	    (dialog.selectedFiles().at(0).lastIndexOf(".") + 1).toUpper();
	  id.front_image->scene()->addPixmap
	    (QPixmap().fromImage(id.front_image->image));
	  id.front_image->scene()->items().at(0)->setFlags
	    (QGraphicsItem::ItemIsSelectable);
	}
      else
	{
	  id.back_image->clear();
	  id.back_image->image = QImage(dialog.selectedFiles().at(0));
	  id.back_image->imageFormat = dialog.selectedFiles().at(0).mid
	    (dialog.selectedFiles().at(0).lastIndexOf(".") + 1).toUpper();
	  id.back_image->scene()->addPixmap
	    (QPixmap().fromImage(id.back_image->image));
	  id.back_image->scene()->items().at(0)->setFlags
	    (QGraphicsItem::ItemIsSelectable);
	}
    }
}

/*
** -- slotDownloadImage() --
*/

void qtbook_book::slotDownloadImage(void)
{
  if(httpProgress->isVisible())
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

  QBuffer *imgbuffer = findChild<QBuffer *> ();

  if(imgbuffer)
    return;

  QNetworkAccessManager *manager = findChild<QNetworkAccessManager *> ();

  if(manager)
    return;

  if(manager->findChild<QNetworkReply *> ())
    return;

  if((manager = new(std::nothrow) QNetworkAccessManager(this)) == 0)
    return;

  if((imgbuffer = new(std::nothrow) QBuffer(this)) == 0)
    {
      manager->deleteLater();
      return;
    }

  QPushButton *pb = qobject_cast<QPushButton *> (sender());

  if(pb == id.dwnldFront)
    imgbuffer->setProperty("which", "front");
  else
    imgbuffer->setProperty("which", "back");

  imgbuffer->open(QIODevice::WriteOnly);

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

  QString type;
  QNetworkProxy proxy;
  QHash<QString, QString> hash(qmain->getAmazonHash());

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
	  /*
	  ** This is required to resolve an odd error.
	  */

	  QNetworkReply *reply = manager->get
	    (QNetworkRequest(QUrl::fromUserInput("http://0.0.0.0")));

	  if(reply)
	    reply->deleteLater();

	  connect(manager,
		  SIGNAL(proxyAuthenticationRequired(const QNetworkProxy &,
						     QAuthenticator *)),
		  this,
		  SLOT(slotProxyAuthenticationRequired(const QNetworkProxy &,
						       QAuthenticator *)));
	}

      if(type == "http" || type == "socks5")
	{
	  if(type == "http")
	    proxy.setType(QNetworkProxy::HttpProxy);
	  else
	    proxy.setType(QNetworkProxy::Socks5Proxy);

	  quint16 port = 0;
	  QString host("");
	  QString user("");
	  QString password("");

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

	  manager->setProxy(proxy);
	}
      else if(type == "system")
	{
	  QNetworkProxyQuery query(url);
	  QList<QNetworkProxy> list
	    (QNetworkProxyFactory::systemProxyForQuery(query));

	  if(!list.isEmpty())
	    proxy = list.at(0);

	  manager->setProxy(proxy);
	}
    }

  QNetworkReply *reply = manager->get(QNetworkRequest(url));

  if(!reply)
    {
      manager->deleteLater();
      imgbuffer->deleteLater();
      return;
    }

  connect(reply, SIGNAL(readyRead(void)),
	  this, SLOT(slotReadyRead(void)));
  connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
	  this, SLOT(slotDataTransferProgress(qint64, qint64)));
  connect(reply, SIGNAL(finished(void)),
	  this, SLOT(slotDownloadFinished(void)));
  httpProgress->setMaximum(0);
  httpProgress->setMinimum(0);
  httpProgress->show();

  if(pb == id.dwnldFront)
    {
      httpProgress->setWindowTitle
	(tr("BiblioteQ: Front Cover Image Download"));
      httpProgress->setLabelText(tr("Downloading the front cover image. "
				    "Please be patient..."));
    }
  else
    {
      httpProgress->setWindowTitle
	(tr("BiblioteQ: Back Cover Image Download"));
      httpProgress->setLabelText(tr("Downloading the back cover image. "
				    "Please be patient..."));
    }
}

/*
** -- slotDownloadFinished() --
*/

void qtbook_book::slotDownloadFinished(void)
{
  if(httpProgress->isVisible())
    httpProgress->hide();

  QNetworkAccessManager *manager = findChild<QNetworkAccessManager *> ();

  if(manager)
    manager->deleteLater();

  QBuffer *imgbuffer = findChild<QBuffer *> ();

  if(imgbuffer)
    {
      if(imgbuffer->property("which") == "front")
	{
	  if(imgbuffer->data().size() > 1000)
	    {
	      id.front_image->clear();
	      id.front_image->loadFromData(imgbuffer->data());
	    }

	  if(imgbuffer->data().size() < 1000)
	    {
	      imgbuffer->deleteLater();
	      QMessageBox::warning
		(this, tr("BiblioteQ: HTTP Warning"),
		 tr("The front cover image for the specified "
		    "ISBN may not exist."));
	    }
	  else
	    imgbuffer->deleteLater();
	}
      else
	{
	  if(imgbuffer->data().size() > 1000)
	    {
	      id.back_image->clear();
	      id.back_image->loadFromData(imgbuffer->data());
	    }

	  if(imgbuffer->data().size() < 1000)
	    {
	      imgbuffer->deleteLater();
	      QMessageBox::warning
		(this, tr("BiblioteQ: HTTP Warning"),
		 tr("The back cover image for the specified ISBN "
		    "may not exist."));
	    }
	  else
	    imgbuffer->deleteLater();
	}
    }
}

/*
** -- slotDataTransferProgress() --
*/

void qtbook_book::slotDataTransferProgress(qint64 bytesread,
					   qint64 totalbytes)
{
  if(httpProgress->isVisible())
    {
      httpProgress->setMaximum(totalbytes);
      httpProgress->setValue(bytesread);
    }
}

/*
** -- slotCancelImageDownload() --
*/

void qtbook_book::slotCancelImageDownload(void)
{
  QBuffer *imgbuffer = findChild<QBuffer *> ();

  if(imgbuffer)
    imgbuffer->deleteLater();

  QNetworkAccessManager *manager = findChild<QNetworkAccessManager *> ();

  if(manager)
    manager->deleteLater();
}

/*
** -- slotReadyRead() --
*/

void qtbook_book::slotReadyRead(void)
{
  QBuffer *imgbuffer = findChild<QBuffer *> ();
  QNetworkAccessManager *manager = findChild<QNetworkAccessManager *> ();

  if(manager)
    {
      QNetworkReply *reply = manager->findChild<QNetworkReply *> ();

      if(reply && imgbuffer)
	imgbuffer->write(reply->readAll());
    }
}

/*
** -- duplicate() --
*/

void qtbook_book::duplicate(const QString &p_oid, const int state)
{
  modify(state); // Initial population.
  id.copiesButton->setEnabled(false);
  id.showUserButton->setEnabled(false);
  oid = p_oid;
  setWindowTitle(tr("BiblioteQ: Duplicate Book Entry"));
  engWindowTitle = "Create";
}

/*
** -- slotProxyAuthenticationRequired() --
*/

void qtbook_book::slotProxyAuthenticationRequired
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

void qtbook_book::changeEvent(QEvent *event)
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

void qtbook_book::slotSRUDownloadFinished(void)
{
  QNetworkAccessManager *manager = findChild<QNetworkAccessManager *> ();

  if(manager)
    manager->deleteLater();

  qtbook_item_working_dialog *dialog =
    findChild<qtbook_item_working_dialog *> ("sru_dialog");

  if(dialog)
    {
      dialog->hide();
      dialog->deleteLater();
      update();
    }

  /*
  ** Verify that the SRU data contains at least one record.
  */

  QXmlStreamReader reader(m_sruResults);

  while(!reader.atEnd())
    if(reader.readNextStartElement())
      if(reader.name().toString().trimmed().toLower() == "numberofrecords")
	if(reader.readElementText().trimmed().toInt() <= 0)
	  {
	    m_sruResults.clear();
	    break;
	  }

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
	  misc_functions::highlightWidget
	    (id.marc_tags->viewport(), QColor(162, 205, 90));

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

	  reader.clear();
	  reader.addData(m_sruResults);

	  while(!reader.atEnd())
	    if(reader.readNextStartElement())
	      {
		if(reader.name().toString().toLower().
		   trimmed() == "datafield")
		  {
		    QString tag(reader.attributes().value("tag").
				toString().toLower().trimmed());

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
			  if(reader.name().toString().toLower().
			     trimmed() == "subfield")
			    {
			      if(reader.attributes().value("code").
				 toString().toLower().trimmed() == "a")
				{
				  str.append(reader.readElementText());
				  break;
				}
			    }
			  else
			    break;

			str = str.trimmed();
			id.lcnum->setText(str);
			misc_functions::highlightWidget
			  (id.lcnum, QColor(162, 205, 90));
		      }
		    else if(tag == "020")
		      {
			/*
			** $a - International Standard Book Number (NR)
			** $c - Terms of availability (NR)
			** $z - Canceled/invalid ISBN (R)
			** $6 - Linkage (NR)
			** $8 - Field link and sequence number (R)
			*/

			QString str("");

			while(reader.readNextStartElement())
			  if(reader.name().toString().toLower().
			     trimmed() == "subfield")
			    str.append(reader.readElementText());
			  else
			    break;

			if(str.toLower().contains("hardcover"))
			  {
			    id.binding->setCurrentIndex(0);
			    id.binding->setStyleSheet
			      ("background-color: rgb(162, 205, 90)");
			  }
			else if(str.toLower().contains("pbk."))
			  {
			    id.binding->setCurrentIndex(1);
			    id.binding->setStyleSheet
			      ("background-color: rgb(162, 205, 90)");
			  }

			if(str.contains(" ") && str.indexOf(" ") == 10)
			  {
			    str = str.mid(0, 10).trimmed();
			    id.id->setText(str);
			    misc_functions::highlightWidget
			      (id.id, QColor(162, 205, 90));
			  }
			else if(str.contains(" ") &&
				str.indexOf(" ") == 13)
			  {
			    str = str.mid(0, 13).trimmed();
			    id.isbn13->setText(str);
			    misc_functions::highlightWidget
			      (id.isbn13, QColor(162, 205, 90));
			  }
			else if(str.length() == 10)
			  {
			    id.id->setText(str);
			    misc_functions::highlightWidget
			      (id.id, QColor(162, 205, 90));
			  }
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
			  if(reader.name().toString().toLower().
			     trimmed() == "subfield")
			    {
			      if(reader.attributes().value("code").
				 toString().toLower().trimmed() == "a" ||
				 reader.attributes().value("code").
				 toString().toLower().trimmed() == "b")
				str.append(reader.readElementText());
			    }
			  else
			    break;

			id.callnum->setText(str);
			misc_functions::highlightWidget
			  (id.callnum, QColor(162, 205, 90));
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
			  if(reader.name().toString().toLower().
			     trimmed() == "subfield")
			    {
			      if(reader.attributes().value("code").
				 toString().toLower().trimmed() == "a" ||
				 reader.attributes().value("code").
				 toString().toLower().trimmed() == "b" ||
				 reader.attributes().value("code").
				 toString().toLower().trimmed() == "m" ||
				 reader.attributes().value("code").
				 toString().toLower().trimmed() == "q")
				str.append(reader.readElementText());
			    }
			  else
			    break;

			id.deweynum->setText(str);
			misc_functions::highlightWidget
			  (id.deweynum, QColor(162, 205, 90));
		      }
		    else if(tag == "100" || tag == "700")
		      {
			/*
			** $a - Personal name (NR)
			** $b - Numeration (NR)
			** $c - Titles and words associated with a name (R)
			** $d - Dates associated with a name (NR)
			** $e - Relator term (R)
			** $f - Date of a work (NR)
			** $g - Miscellaneous information (NR)
			** $j - Attribution qualifier (R)
			** $k - Form subheading (R)
			** $l - Language of a work (NR)
			** $n - Number of part/section of a work (R)
			** $p - Name of part/section of a work (R)
			** $q - Fuller form of name (NR)
			** $t - Title of a work (NR)
			** $u - Affiliation (NR)
			** $0 - Authority record control number (R)
			** $4 - Relator code (R)
			** $6 - Linkage (NR)
			** $8 - Field link and sequence number (R)
			*/

			QString str("");

			while(reader.readNextStartElement())
			  if(reader.name().toString().toLower().
			     trimmed() == "subfield")
			    {
			      if(reader.attributes().value("code").
				 toString().toLower().trimmed() == "a")
				{
				  str.append(reader.readElementText());
				  break;
				}
			    }
			  else
			    break;

			if(str.endsWith(","))
			  str = str.mid(0, str.length() - 1).trimmed();

			if(!id.author->toPlainText().contains(str))
			  {
			    if(tag == "100")
			      id.author->setPlainText(str);
			    else if(!id.author->toPlainText().isEmpty())
			      id.author->setPlainText
				(id.author->toPlainText() + "\n" +
				 str);
			    else
			      id.author->setPlainText(str);

			    misc_functions::highlightWidget
			      (id.author->viewport(),
			       QColor(162, 205, 90));
			  }
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

			QString str;

			while(reader.readNextStartElement())
			  if(reader.name().toString().toLower().
			     trimmed() == "subfield")
			    {
			      if(reader.attributes().value("code").
				 toString().toLower().trimmed() == "a" ||
				 reader.attributes().value("code").
				 toString().toLower().trimmed() == "b")
				str.append(reader.readElementText());
			    }
			  else
			    break;

			str = str.mid
			  (0, str.lastIndexOf('/')).trimmed();
			id.title->setText(str);
			misc_functions::highlightWidget
			  (id.title, QColor(162, 205, 90));
		      }
		    else if(tag == "250")
		      {
			/*
			** $a - Edition statement (NR)
			** $b - Remainder of edition statement (NR)
			** $6 - Linkage (NR)
			** $8 - Field link and sequence number (R)
			*/

			QString str("");

			while(reader.readNextStartElement())
			  if(reader.name().toString().toLower().
			     trimmed() == "subfield")
			    {
			      if(reader.attributes().value("code").
				 toString().toLower().trimmed() == "a")
				{
				  str.append(reader.readElementText());
				  break;
				}
			    }
			  else
			    break;

			str = str.mid(0, str.indexOf(" ")).trimmed();

			int i = 0;

			for(i = 0; i < str.size(); i++)
			  if(!str.at(i).isDigit())
			    break;

			str = str.mid(0, i);

			if(id.edition->findText(str) > -1)
			  id.edition->setCurrentIndex
			    (id.edition->findText(str));
			else
			  id.edition->setCurrentIndex(0);
		      }
		    else if(tag == "260")
		      {
			/*
			** $a - Place of publication, distribution,
			**      etc. (R)
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
			  if(reader.name().toString().toLower().
			     trimmed() == "subfield")
			    {
			      if(reader.attributes().value("code").
				 toString().toLower().trimmed() == "a")
				place = reader.readElementText();
			      else if(reader.attributes().value("code").
				      toString().toLower().trimmed() == "b")
				publisher = reader.readElementText();
			      else if(reader.attributes().value("code").
				      toString().toLower().trimmed() == "c")
				{
				  date = reader.readElementText().toLower();

				  for(int i = date.length() - 1; i >= 0; i--)
				    if(!date.at(i).isNumber())
				      date.remove(i, 1);
				}
			    }
			  else
			    break;

			id.publication_date->setDate
			  (QDate::fromString("01/01/" + date,
					     "MM/dd/yyyy"));
			id.publication_date->setStyleSheet
			  ("background-color: rgb(162, 205, 90)");
			place = place.mid(0, place.lastIndexOf(" ")).
			  trimmed();

			if(!place.isEmpty())
			  if(!place[place.length() - 1].isLetter())
			    place = place.remove(place.length() - 1, 1).
			      trimmed();

			id.place->setPlainText(place);
			misc_functions::highlightWidget
			  (id.place->viewport(), QColor(162, 205, 90));

			if(publisher.endsWith(","))
			  publisher = publisher.mid
			    (0, publisher.length() - 1).trimmed();

			id.publisher->setPlainText(publisher);
			misc_functions::highlightWidget
			  (id.publisher->viewport(),
			   QColor(162, 205, 90));
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
			  if(reader.name().toString().toLower().
			     trimmed() == "subfield")
			    str.append(reader.readElementText());
			  else
			    break;

			id.description->setPlainText(str);
			misc_functions::highlightWidget
			  (id.description->viewport(),
			   QColor(162, 205, 90));
		      }
		    else if(tag == "650")
		      {
			/*
			** $a - Topical term or geographic name entry
			**      element (NR)
			** $b - Topical term following geographic name
			**      entry element (NR)
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
			  if(reader.name().toString().toLower().
			     trimmed() == "subfield")
			    {
			      if(reader.attributes().value("code").
				 toString().toLower().trimmed() == "a")
				{
				  str.append(reader.readElementText());
				  break;
				}
			    }
			  else
			    break;

			if(!str.isEmpty())
			  {
			    if(!str[str.length() - 1].isPunct())
			      str += ".";

			    if(!id.category->toPlainText().contains(str))
			      {
				if(!id.category->toPlainText().isEmpty())
				  id.category->setPlainText
				    (id.category->toPlainText() + "\n" +
				     str);
				else
				  id.category->setPlainText(str);

				misc_functions::highlightWidget
				  (id.category->viewport(),
				   QColor(162, 205, 90));
			      }
			  }
		      }
		  }
	      }

	  foreach(QLineEdit *textfield, findChildren<QLineEdit *>())
	    textfield->setCursorPosition(0);
	}
    }
  else
    QMessageBox::critical
      (this, tr("BiblioteQ: SRU Query Error"),
       tr("An SRU entry may not yet exist for the provided ISBN(s)."));
}

/*
** -- slotSRUReadyRead() --
*/

void qtbook_book::slotSRUReadyRead(void)
{
  QNetworkReply *reply = qobject_cast<QNetworkReply *> (sender());

  if(reply)
    m_sruResults.append(reply->readAll());
}
