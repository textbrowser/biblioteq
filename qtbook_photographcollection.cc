/*
** -- Qt Includes --
*/

#include <QSqlField>
#include <QSqlRecord>
#include <QtCore/qmath.h>

/*
** Includes photograph collection-specific methods.
*/

/*
** -- Local Includes --
*/

#include "qtbook.h"
#include "bgraphicsscene.h"
#include "ui_photographview.h"
#include "qtbook_photographcollection.h"

extern qtbook *qmain;
extern QApplication *qapp;

/*
** -- qtbook_photographcollection() --
*/

qtbook_photographcollection::qtbook_photographcollection
(QMainWindow *parentArg,
 const QString &oidArg,
 const int rowArg):QMainWindow()
{
  QMenu *menu1 = 0;
  QMenu *menu2 = 0;
  QGraphicsScene *scene1 = 0;
  QGraphicsScene *scene2 = 0;
  QGraphicsScene *scene3 = 0;

  if((menu1 = new(std::nothrow) QMenu(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((menu2 = new(std::nothrow) QMenu(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene1 = new(std::nothrow) QGraphicsScene(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene2 = new(std::nothrow) QGraphicsScene(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene3 = new(std::nothrow) QGraphicsScene(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((photo_diag = new(std::nothrow) QDialog(this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  pc.setupUi(this);

  if((scene = new(std::nothrow) bgraphicsscene(pc.graphicsView)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  connect(scene,
	  SIGNAL(selectionChanged(void)),
	  this,
	  SLOT(slotSceneSelectionChanged(void)));
  oid = oidArg;
  row = rowArg;
  isQueryEnabled = false;
  parentWid = parentArg;
  photo.setupUi(photo_diag);
  pc.graphicsView->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(pc.graphicsView,
	  SIGNAL(customContextMenuRequested(const QPoint &)),
	  this,
	  SLOT(slotViewContextMenu(const QPoint &)));
  pc.graphicsView->setScene(scene);
  pc.graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
  pc.graphicsView->setRubberBandSelectionMode(Qt::IntersectsItemShape);
  pc.graphicsView->setSceneRect(0, 0,
				5 * 150,
				PHOTOGRAPHS_PER_PAGE / 5 * 200);
  pc.thumbnail_item->setReadOnly(true);
#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  setAttribute(Qt::WA_MacMetalStyle, true);
  photo_diag->setAttribute(Qt::WA_MacMetalStyle, true);
#endif
#endif
  updateFont(qapp->font(), static_cast<QWidget *> (this));
  photo_diag->setWindowModality(Qt::WindowModal);
  updateFont(qapp->font(), static_cast<QWidget *> (photo_diag));
  connect(pc.select_image_collection, SIGNAL(clicked(void)),
	  this, SLOT(slotSelectImage(void)));
  connect(photo.select_image_item, SIGNAL(clicked(void)),
	  this, SLOT(slotSelectImage(void)));
  connect(pc.okButton, SIGNAL(clicked(void)), this, SLOT(slotGo(void)));
  connect(pc.cancelButton, SIGNAL(clicked(void)), this,
	  SLOT(slotCancel(void)));
  connect(pc.resetButton, SIGNAL(clicked(void)), this,
	  SLOT(slotReset(void)));
  connect(pc.printButton, SIGNAL(clicked(void)), this, SLOT(slotPrint(void)));
  connect(pc.addItemButton, SIGNAL(clicked(void)), this,
	  SLOT(slotAddItem(void)));
  connect(photo.cancelButton, SIGNAL(clicked(void)), this,
	  SLOT(slotClosePhoto(void)));
  connect(menu1->addAction(tr("Reset Collection &Image")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Collection &ID")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Collection &Title")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Collection &Location")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Collection &About")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Collection &Notes")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu2->addAction(tr("&All")),
	  SIGNAL(triggered(void)), this, SLOT(slotExportPhotographs(void)));
  connect(menu2->addAction(tr("&Current Page")),
	  SIGNAL(triggered(void)), this, SLOT(slotExportPhotographs(void)));
  connect(pc.page, SIGNAL(currentIndexChanged(const QString &)),
	  this, SLOT(slotPageChanged(const QString &)));
  connect(pc.exportPhotographsToolButton,
	  SIGNAL(clicked(void)),
	  this, SLOT(slotExportPhotographs(void)));
  pc.resetButton->setMenu(menu1);
  pc.exportPhotographsToolButton->setMenu(menu2);

  QString errorstr("");

  qapp->setOverrideCursor(Qt::WaitCursor);
  pc.location->addItems
    (misc_functions::getLocations(qmain->getDB(),
				  "Photograph Collection",
				  errorstr));
  qapp->restoreOverrideCursor();

  if(!errorstr.isEmpty())
    qmain->addError
      (QString(tr("Database Error")),
       QString(tr("Unable to retrieve the photograph collection locations.")),
       errorstr, __FILE__, __LINE__);

  pc.thumbnail_collection->setScene(scene1);
  pc.thumbnail_item->setScene(scene2);
  photo.thumbnail_item->setScene(scene3);

  if(pc.location->findText(tr("UNKNOWN")) == -1)
    pc.location->addItem(tr("UNKNOWN"));

  resize(qRound(0.95 * parentWid->size().width()),
	 qRound(0.95 * parentWid->size().height()));
  pc.splitter->setStretchFactor(0, 0);
  pc.splitter->setStretchFactor(1, 1);
  pc.splitter->setStretchFactor(2, 0);
  misc_functions::center(this, parentWid);
  misc_functions::hideAdminFields(this, qmain->getRoles());
  misc_functions::highlightWidget
    (photo.id_item, QColor(255, 248, 220));
  misc_functions::highlightWidget
    (photo.title_item, QColor(255, 248, 220));
  misc_functions::highlightWidget
    (photo.creators_item->viewport(), QColor(255, 248, 220));
  misc_functions::highlightWidget
    (photo.medium_item, QColor(255, 248, 220));
  misc_functions::highlightWidget
    (photo.reproduction_number_item->viewport(), QColor(255, 248, 220));
  misc_functions::highlightWidget
    (photo.copyright_item->viewport(), QColor(255, 248, 220));
}

/*
** -- ~qtbook_photographcollection() --
*/

qtbook_photographcollection::~qtbook_photographcollection()
{
}

/*
** -- slotGo() --
*/

void qtbook_photographcollection::slotGo(void)
{
  QString str("");
  QString errorstr("");

  if(engWindowTitle.contains("Create") ||
     engWindowTitle.contains("Modify"))
    {
      str = pc.id_collection->text().trimmed();
      pc.id_collection->setText(str);

      if(pc.id_collection->text().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the collection's "
				   "ID field."));
	  pc.id_collection->setFocus();
	  return;
	}

      str = pc.title_collection->text().trimmed();
      pc.title_collection->setText(str);

      if(pc.title_collection->text().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the collection's "
				   "Title field."));
	  pc.title_collection->setFocus();
	  return;
	}

      pc.about_collection->setPlainText
	(pc.about_collection->toPlainText().trimmed());
      pc.notes_collection->setPlainText
	(pc.notes_collection->toPlainText().trimmed());
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
      else
	qapp->restoreOverrideCursor();

      QSqlQuery query(qmain->getDB());

      if(engWindowTitle.contains("Modify"))
	query.prepare("UPDATE photograph_collection SET "
		      "id = ?, "
		      "title = ?, "
		      "location = ?, "
		      "about = ?, "
		      "notes = ?, "
		      "image = ?, "
		      "image_scaled = ? "
		      "WHERE "
		      "myoid = ?");
      else if(qmain->getDB().driverName() != "QSQLITE")
	query.prepare("INSERT INTO photograph_collection "
		      "(id, title, location, about, notes, image, "
		      "image_scaled) VALUES (?, "
		      "?, ?, ?, ?, ?, ?)");
      else
	query.prepare("INSERT INTO photograph_collection "
		      "(id, title, location, about, notes, image, "
		      "image_scaled, myoid) "
		      "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");

      query.bindValue(0, pc.id_collection->text());
      query.bindValue(1, pc.title_collection->text());
      query.bindValue(2, pc.location->currentText());
      query.bindValue(3, pc.about_collection->toPlainText().trimmed());
      query.bindValue(4, pc.notes_collection->toPlainText().trimmed());

      if(!pc.thumbnail_collection->image.isNull())
	{
	  QImage image;
	  QByteArray bytes;
	  QBuffer buffer(&bytes);

	  if(buffer.open(QIODevice::WriteOnly))
	    {
	      pc.thumbnail_collection->image.save
		(&buffer, pc.thumbnail_collection->imageFormat.toLatin1(),
		 100);
	      query.bindValue(5, bytes.toBase64());
	    }
	  else
	    query.bindValue(5, QVariant(QVariant::ByteArray));

	  buffer.close();
	  bytes.clear();
	  image = pc.thumbnail_collection->image;
	  image = image.scaled
	    (126, 187, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	  if(buffer.open(QIODevice::WriteOnly))
	    {
	      image.save
		(&buffer, pc.thumbnail_collection->imageFormat.toLatin1(),
		 100);
	      query.bindValue(6, bytes.toBase64());
	    }
	  else
	    query.bindValue(6, QVariant(QVariant::ByteArray));
	}
      else
	{
	  pc.thumbnail_collection->imageFormat = "";
	  query.bindValue(5, QVariant(QVariant::ByteArray));
	  query.bindValue(6, QVariant(QVariant::ByteArray));
	}

      if(engWindowTitle.contains("Modify"))
	query.bindValue(7, oid);
      else if(qmain->getDB().driverName() == "QSQLITE")
	{
	  qint64 value = misc_functions::getSqliteUniqueId(qmain->getDB(),
							   errorstr);

	  if(errorstr.isEmpty())
	    query.bindValue(7, value);
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
	  qmain->addError(QString(tr("Database Error")),
			  QString(tr("Unable to create or update the entry.")),
			  query.lastError().text(), __FILE__, __LINE__);
	  goto db_rollback;
	}
      else
	{
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

	  qapp->restoreOverrideCursor();

	  if(engWindowTitle.contains("Modify"))
	    {
	      str = QString(tr("BiblioteQ: Modify Photograph Collection "
			       "Entry (")) +
		pc.id_collection->text() + tr(")");
	      setWindowTitle(str);
	      engWindowTitle = "Modify";

	      if((qmain->getTypeFilterString() == "All" ||
		  qmain->getTypeFilterString() == "All Available" ||
		  qmain->getTypeFilterString() == "All Overdue" ||
		  qmain->getTypeFilterString() == "All Requested" ||
		  qmain->getTypeFilterString() == "All Reserved" ||
		  qmain->getTypeFilterString() == "Photograph Collections") &&
		 oid == misc_functions::getColumnString
		 (qmain->getUI().table,
		  row, qmain->getUI().table->columnNumber("MYOID")) &&
		 misc_functions::getColumnString
		 (qmain->getUI().table,
		  row, qmain->getUI().table->columnNumber("Type")) ==
		 "Photograph Collection")
		{
		  qmain->getUI().table->setSortingEnabled(false);

		  QStringList names(qmain->getUI().table->columnNames());

		  for(int i = 0; i < names.size(); i++)
		    {
		      if(names.at(i) == "ID" ||
			 names.at(i) == "ID Number")
			qmain->getUI().table->item(row, i)->setText
			  (pc.id_collection->text());
		      else if(names.at(i) == "Title")
			qmain->getUI().table->item(row, i)->setText
			  (pc.title_collection->text());
		      else if(names.at(i) == "Location")
			qmain->getUI().table->item(row, i)->setText
			  (pc.location->currentText());
		      else if(names.at(i) == "About")
			qmain->getUI().table->item(row, i)->setText
			  (pc.about_collection->toPlainText().trimmed());
		    }

		  qmain->getUI().table->setSortingEnabled(true);

		  foreach(QLineEdit *textfield, findChildren<QLineEdit *>())
		    textfield->setCursorPosition(0);

		  qmain->slotResizeColumns();
		}

	      qmain->slotDisplaySummary();
	      qmain->updateSceneItem(oid, "Photograph Collection",
				     pc.thumbnail_collection->image);
	    }
	  else
	    {
	      qapp->setOverrideCursor(Qt::WaitCursor);
	      oid = misc_functions::getOID(pc.id_collection->text(),
					   "Photograph Collection",
					   qmain->getDB(),
					   errorstr);
	      qapp->restoreOverrideCursor();

	      if(!errorstr.isEmpty())
		{
		  qmain->addError(QString(tr("Database Error")),
				  QString(tr("Unable to retrieve the "
					     "photograph collection's OID.")),
				  errorstr, __FILE__, __LINE__);
		  QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
					tr("Unable to retrieve the "
					   "photograph collection's OID."));
		}
	      else
		qmain->replacePhotographCollection(oid, this);

	      updateWindow(qtbook::EDITABLE);

	      if(qmain->getUI().actionAutoPopulateOnCreation->isChecked())
		(void) qmain->populateTable
		  (qtbook::POPULATE_ALL, "Photograph Collections",
		   QString(""));

	      raise();
	    }

	  storeData();
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
      QString searchstr("");

      searchstr = "SELECT DISTINCT photograph_collection.title, "
	"photograph_collection.id, "
	"photograph_collection.location, "
	"COUNT(photograph.myoid) AS photograph_count, "
	"photograph_collection.about, "
	"photograph_collection.type, "
	"photograph_collection.myoid, "
	"photograph_collection.image_scaled "
	"FROM photograph_collection LEFT JOIN "
	"photograph "
	"ON photograph_collection.myoid = photograph.collection_oid "
	"WHERE ";
      searchstr.append("photograph_collection.id LIKE '%" +
		       pc.id_collection->text().trimmed() +
		       "%' AND ");

      QString E("");

      if(qmain->getDB().driverName() != "QSQLITE")
	E = "E";

      searchstr.append("photograph_collection.title LIKE " + E + "'%" +
		       myqstring::escape(pc.title_collection->
					 text().trimmed()) +
		       "%' AND ");

      if(pc.location->currentIndex() != 0)
	searchstr.append("photograph_collection.location = " + E + "'" +
			 myqstring::escape
			 (pc.location->currentText().trimmed()) + "' AND ");

      searchstr.append("COALESCE(photograph_collection.about, '') LIKE " +
		       E + "'%" +
		       myqstring::escape
		       (pc.about_collection->toPlainText().trimmed()) +
		       "%' AND ");
      searchstr.append("COALESCE(photograph_collection.notes, '') LIKE " +
		       E + "'%" +
		       myqstring::escape
		       (pc.notes_collection->toPlainText().trimmed()) +
		       "%'");

      /*
      ** Search the database.
      */

      (void) qmain->populateTable
	(qtbook::POPULATE_SEARCH, "Photograph Collections", searchstr);
    }
}

/*
** -- search() --
*/

void qtbook_photographcollection::search
(const QString &field, const QString &value)
{
  Q_UNUSED(field);
  Q_UNUSED(value);
  pc.addItemButton->setVisible(false);
  pc.thumbnail_collection->setVisible(false);
  pc.select_image_collection->setVisible(false);
  pc.collectionGroup->setVisible(false);
  pc.itemGroup->setVisible(false);
  pc.exportPhotographsToolButton->setVisible(false);
  pc.location->insertItem(0, tr("Any"));
  pc.location->setCurrentIndex(0);

  QList<QAction *> actions = pc.resetButton->menu()->actions();

  actions[0]->setVisible(false);

  for(int i = 6; i < actions.size(); i++)
    actions.at(i)->setVisible(false);

  actions.clear();
  setWindowTitle(tr("BiblioteQ: Database Photograph Collection Search"));
  engWindowTitle = "Search";
  pc.okButton->setText(tr("&Search"));
  pc.id_collection->setFocus();
  misc_functions::center(this, parentWid);
  show();
}

/*
** -- updateWindow() --
*/

void qtbook_photographcollection::updateWindow(const int state)
{
  QString str = "";

  if(state == qtbook::EDITABLE)
    {
      pc.okButton->setVisible(true);
      pc.addItemButton->setEnabled(true);
      pc.resetButton->setVisible(true);
      str = QString(tr("BiblioteQ: Modify Photograph Collection Entry (")) +
	pc.id_collection->text() + tr(")");
      engWindowTitle = "Modify";
    }
  else
    {
      pc.okButton->setVisible(false);
      pc.addItemButton->setEnabled(false);
      pc.resetButton->setVisible(false);
      str = QString(tr("BiblioteQ: View Photograph Collection Details (")) +
	pc.id_collection->text() + tr(")");
      engWindowTitle = "View";
    }

  setWindowTitle(str);
}

/*
** -- modify() --
*/

void qtbook_photographcollection::modify(const int state,
					 const QString &behavior)
{
  QString str("");
  QString fieldname("");
  QString searchstr("");
  QVariant var;
  QSqlQuery query(qmain->getDB());

  if(state == qtbook::EDITABLE)
    {
      disconnect(scene,
		 SIGNAL(deleteKeyPressed(void)),
		 this,
		 SLOT(slotDeleteItem(void)));
      connect(scene,
	      SIGNAL(deleteKeyPressed(void)),
	      this,
	      SLOT(slotDeleteItem(void)));

      if(behavior.isEmpty())
	{
	  setWindowTitle(tr("BiblioteQ: Modify Photograph Collection Entry"));
	  engWindowTitle = "Modify";
	}
      else
	engWindowTitle = behavior;

      pc.okButton->setVisible(true);
      pc.addItemButton->setEnabled(true);
      pc.resetButton->setVisible(true);
      pc.select_image_collection->setVisible(true);
      misc_functions::highlightWidget
	(pc.id_collection, QColor(255, 248, 220));
      misc_functions::highlightWidget
	(pc.title_collection, QColor(255, 248, 220));
      pc.graphicsView->scene()->disconnect(SIGNAL(itemDoubleClicked(void)));
      connect(pc.graphicsView->scene(), SIGNAL(itemDoubleClicked(void)),
	      this, SLOT(slotModifyItem(void)));
    }
  else
    {
      if(behavior.isEmpty())
	{
	  setWindowTitle(tr("BiblioteQ: View Photograph Collection Details"));
	  engWindowTitle = "View";
	}
      else
	engWindowTitle = behavior;

      pc.okButton->setVisible(false);
      pc.addItemButton->setVisible(false);
      pc.resetButton->setVisible(false);
      pc.select_image_collection->setVisible(false);

      QList<QAction *> actions = pc.resetButton->menu()->actions();

      actions[0]->setVisible(false);
      actions.clear();
    }

  searchstr = "SELECT id, "
    "title, "
    "location, "
    "about, "
    "notes, "
    "image "
    "FROM "
    "photograph_collection "
    "WHERE myoid = ";
  searchstr.append(oid);
  pc.okButton->setText(tr("&Save"));
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec(searchstr) || !query.next())
    {
      qapp->restoreOverrideCursor();
      qmain->addError(QString(tr("Database Error")),
		      QString(tr("Unable to retrieve the selected photograph "
				 "collection's data.")),
		      query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			    tr("Unable to retrieve the selected photograph "
			       "collection's data."));
      return;
    }
  else
    {
      qapp->restoreOverrideCursor();

      for(int i = 0; i < query.record().count(); i++)
	{
	  var = query.record().field(i).value();
	  fieldname = query.record().fieldName(i);

	  if(fieldname == "id")
	    {
	      pc.id_collection->setText(var.toString());

	      if(behavior.isEmpty())
		{
		  if(state == qtbook::EDITABLE)
		    {
		      str = QString
			(tr("BiblioteQ: Modify Photograph Collection "
			    "Entry (")) +
			var.toString() + tr(")");
		      engWindowTitle = "Modify";
		    }
		  else
		    {
		      str = QString(tr("BiblioteQ: View Photograph "
				       "Collection Details (")) +
			var.toString() + tr(")");
		      engWindowTitle = "View";
		    }

		  setWindowTitle(str);
		}
	    }
	  else if(fieldname == "title")
	    pc.title_collection->setText(var.toString());
	  else if(fieldname == "location")
	    {
	      if(pc.location->findText(var.toString()) > -1)
		pc.location->setCurrentIndex
		  (pc.location->findText(var.toString()));
	      else
		pc.location->setCurrentIndex
		  (pc.location->findText(tr("UNKNOWN")));
	    }
	  else if(fieldname == "about")
	    pc.about_collection->setPlainText(var.toString());
	  else if(fieldname == "notes")
	    pc.notes_collection->setPlainText(var.toString());
	  else if(fieldname == "image")
	    {
	      if(!query.record().field(i).isNull())
		{
		  pc.thumbnail_collection->loadFromData
		    (QByteArray::fromBase64(var.toByteArray()));

		  if(pc.thumbnail_collection->image.isNull())
		    pc.thumbnail_collection->loadFromData(var.toByteArray());
		}
	    }
	}

      int pages = 1;

      if(!engWindowTitle.contains("Create"))
	{
	  qapp->setOverrideCursor(Qt::WaitCursor);

	  if(query.exec(QString("SELECT COUNT(*) "
				"FROM photograph "
				"WHERE collection_oid = %1").
			arg(oid)))
	    if(query.next())
	      pages = qCeil(query.value(0).toDouble() /
			    PHOTOGRAPHS_PER_PAGE);

	  qapp->restoreOverrideCursor();
	  pages = qMax(1, pages);
	}

      pc.page->blockSignals(true);
      pc.page->clear();

      for(int i = 1; i <= pages; i++)
	pc.page->addItem(QString::number(i));

      pc.page->blockSignals(false);

      if(!engWindowTitle.contains("Create"))
	{
	  qapp->setOverrideCursor(Qt::WaitCursor);
	  showPhotographs(pc.page->currentText().toInt());
	  qapp->restoreOverrideCursor();
	}

      foreach(QLineEdit *textfield, findChildren<QLineEdit *>())
	textfield->setCursorPosition(0);

      storeData();
      showNormal();
    }

  pc.id_collection->setFocus();
  raise();
}

/*
** -- insert() --
*/

void qtbook_photographcollection::insert(void)
{
  pc.okButton->setText(tr("&Save"));
  pc.addItemButton->setEnabled(false);
  pc.publication_date->setDate(QDate::fromString("01/01/2000",
						 "MM/dd/yyyy"));
  misc_functions::highlightWidget
    (pc.id_collection, QColor(255, 248, 220));
  misc_functions::highlightWidget
    (pc.title_collection, QColor(255, 248, 220));
  setWindowTitle(tr("BiblioteQ: Create Photograph Collection Entry"));
  engWindowTitle = "Create";
  pc.id_collection->setFocus();
  pc.page->blockSignals(true);
  pc.page->clear();
  pc.page->addItem("1");
  pc.page->blockSignals(false);
  storeData();
  show();
}

/*
** -- slotReset() --
*/

void qtbook_photographcollection::slotReset(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  if(action != 0)
    {
      QList<QAction *> actions = pc.resetButton->menu()->actions();

      if(action == actions[0])
	pc.thumbnail_collection->clear();
      else if(action == actions[1])
	{
	  pc.id_collection->clear();
	  pc.id_collection->setFocus();
	}
      else if(action == actions[2])
	{
	  pc.title_collection->clear();
	  pc.title_collection->setFocus();
	}
      else if(action == actions[3])
	{
	  pc.location->setCurrentIndex(0);
	  pc.location->setFocus();
	}
      else if(action == actions[4])
	{
	  pc.about_collection->clear();
	  pc.about_collection->setFocus();
	}
      else if(action == actions[5])
	{
	  pc.notes_collection->clear();
	  pc.notes_collection->setFocus();
	}

      actions.clear();
    }
  else
    {
      /*
      ** Reset all.
      */

      pc.thumbnail_collection->clear();
      pc.id_collection->clear();
      pc.title_collection->clear();
      pc.about_collection->clear();
      pc.notes_collection->clear();
      pc.location->setCurrentIndex(0);
      pc.id_collection->setFocus();
    }
}

/*
** -- closeEvent() --
*/

void qtbook_photographcollection::closeEvent(QCloseEvent *e)
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

  qmain->removePhotographCollection(this);
  QMainWindow::closeEvent(e);
}

/*
** -- slotCancel() --
*/

void qtbook_photographcollection::slotCancel(void)
{
  close();
}

/*
** -- slotQuery() --
*/

void qtbook_photographcollection::slotQuery(void)
{
}

/*
** -- slotPrint() --
*/

void qtbook_photographcollection::slotPrint(void)
{
  html = "";
  html += "<b>" + tr("Collection ID:") + "</b> " +
    pc.id_collection->text().trimmed() + "<br>";
  html += "<b>" + tr("Collection Title:") + "</b> " +
    pc.title_collection->text().trimmed() + "<br>";
  html += "<b>" + tr("Collection Location:") + "</b> " +
    pc.location->currentText().trimmed() + "<br>";
  html += "<b>" + tr("Collection About:") + "</b> " +
    pc.about_collection->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Collection Notes:") + "</b> " +
    pc.notes_collection->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Item ID:") + "</b> " +
    pc.id_item->text().trimmed() + "<br>";
  html += "<b>" + tr("Item Title:") + "</b> " +
    pc.title_item->text().trimmed() + "<br>";
  html += "<b>" + tr("Item Creator(s):") + "</b> " +
    pc.creators_item->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Item Publication Date:") + "</b> " +
    pc.publication_date->date().toString(Qt::ISODate) + "<br>";
  html += "<b>" + tr("Item Copies:") + "</b> " +
    pc.quantity->text() + "<br>";
  html += "<b>" + tr("Item Medium:") + "</b> " +
    pc.medium_item->text().trimmed() + "<br>";
  html += "<b>" + tr("Item Reproduction Number:") + "</b> " +
    pc.reproduction_number_item->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Item Copyright:") + "</b> " +
    pc.copyright_item->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Item Call Number:") + "</b> " +
    pc.call_number_item->text().trimmed() + "<br>";
  html += "<b>" + tr("Item Other Number:") + "</b> " +
    pc.other_number_item->text().trimmed() + "<br>";
  html += "<b>" + tr("Item Notes:") + "</b> " +
    pc.notes_item->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Item Subjects:") + "</b> " +
    pc.subjects_item->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Item Format:") + "</b> " +
    pc.format_item->toPlainText().trimmed() + "<br>";
  print(this);
}

/*
** -- slotSelectImage() --
*/

void qtbook_photographcollection::slotSelectImage(void)
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

  if(button == pc.select_image_collection)
    dialog.setWindowTitle(tr("BiblioteQ: Photograph Collection "
			     "Image Selection"));
  else
    dialog.setWindowTitle(tr("BiblioteQ: Photograph Collection Item "
			     "Image Selection"));

  dialog.exec();

  if(dialog.result() == QDialog::Accepted)
    {
      if(button == pc.select_image_collection)
	{
	  pc.thumbnail_collection->clear();
	  pc.thumbnail_collection->image =
	    QImage(dialog.selectedFiles().at(0));
	  pc.thumbnail_collection->imageFormat =
	    dialog.selectedFiles().at(0).mid
	    (dialog.selectedFiles().at(0).lastIndexOf(".") + 1).toUpper();
	  pc.thumbnail_collection->scene()->addPixmap
	    (QPixmap().fromImage(pc.thumbnail_collection->image));
	  pc.thumbnail_collection->scene()->items().at(0)->setFlags
	    (QGraphicsItem::ItemIsSelectable);
	}
      else
	{
	  photo.thumbnail_item->clear();
	  photo.thumbnail_item->image = QImage(dialog.selectedFiles().at(0));
	  photo.thumbnail_item->imageFormat = dialog.selectedFiles().at(0).mid
	    (dialog.selectedFiles().at(0).lastIndexOf(".") + 1).toUpper();
	  photo.thumbnail_item->scene()->addPixmap
	    (QPixmap().fromImage(photo.thumbnail_item->image));
	  photo.thumbnail_item->scene()->items().at(0)->setFlags
	    (QGraphicsItem::ItemIsSelectable);
	}
    }
}

/*
** -- duplicate() --
*/

void qtbook_photographcollection::duplicate
(const QString &p_oid, const int state)
{
  modify(state, "Create"); // Initial population.
  pc.addItemButton->setEnabled(false);
  oid = p_oid;
  setWindowTitle(tr("BiblioteQ: Duplicate Photograph Collection Entry"));
}

/*
** -- changeEvent() --
*/

void qtbook_photographcollection::changeEvent(QEvent *event)
{
  if(event)
    switch(event->type())
      {
      case QEvent::LanguageChange:
	{
	  pc.retranslateUi(this);
	  photo.retranslateUi(photo_diag);
	  break;
	}
      default:
	break;
      }

  QMainWindow::changeEvent(event);
}

/*
** -- showPhotographs() --
*/

void qtbook_photographcollection::showPhotographs(const int page)
{
  QSqlQuery query(qmain->getDB());

  if(query.exec(QString("SELECT image_scaled, myoid FROM "
			"photograph WHERE "
			"collection_oid = %1 "
			"ORDER BY id "
			"LIMIT %2 "
			"OFFSET %3").
		arg(oid).
		arg(PHOTOGRAPHS_PER_PAGE).
		arg(PHOTOGRAPHS_PER_PAGE * (page - 1))))
    {
      pc.graphicsView->scene()->clear();
      pc.graphicsView->resetTransform();
      pc.graphicsView->verticalScrollBar()->setValue(0);
      pc.graphicsView->horizontalScrollBar()->setValue(0);

      int rowIdx = 0;
      int columnIdx = 0;

      while(query.next())
	{
	  QImage image;
	  QGraphicsPixmapItem *pixmapItem = 0;

	  image.loadFromData
	    (QByteArray::fromBase64(query.value(0).
				    toByteArray()));

	  if(image.isNull())
	    image.loadFromData(query.value(0).toByteArray());

	  if(image.isNull())
	    image = QImage("icons.d/no_image.png");

	  /*
	  ** The size of no_image.png is 126x187.
	  */

	  if(!image.isNull())
	    image = image.scaled
	      (126, 187, Qt::KeepAspectRatio,
	       Qt::SmoothTransformation);

	  pixmapItem = pc.graphicsView->scene()->addPixmap
	    (QPixmap().fromImage(image));
	  pixmapItem->setPos(150 * columnIdx, 200 * rowIdx);
	  pixmapItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
	  pixmapItem->setData(0, query.value(1));
	  columnIdx += 1;

	  if(columnIdx >= 5)
	    {
	      rowIdx += 1;
	      columnIdx = 0;
	    }
	}
    }
}

/*
** -- slotAddItem() --
*/

void qtbook_photographcollection::slotAddItem(void)
{
  photo.saveButton->disconnect(SIGNAL(clicked(void)));
  connect(photo.saveButton, SIGNAL(clicked(void)), this,
	  SLOT(slotInsertItem(void)));
  photo_diag->resize(photo_diag->width(),
		     qRound(0.95 * size().height()));
  misc_functions::center(photo_diag, this);
  photo.thumbnail_item->clear();
#if QT_VERSION >= 0x040700
  photo.id_item->setText(QString::number(QDateTime::currentMSecsSinceEpoch()));
#else
  QDateTime dateTime(QDateTime::currentDateTime());

  photo.id_item->setText(QString::
                         number(static_cast<qint64> (dateTime.toTime_t())));
#endif
  photo.title_item->clear();
  photo.creators_item->setPlainText("N/A");
  photo.publication_date->setDate(QDate::fromString("01/01/2000",
						    "MM/dd/yyyy"));
  photo.quantity->setValue(1);
  photo.medium_item->setText("N/A");
  photo.reproduction_number_item->setPlainText("N/A");
  photo.copyright_item->setPlainText("N/A");
  photo.call_number_item->clear();
  photo.other_number_item->clear();
  photo.notes_item->clear();
  photo.subjects_item->clear();
  photo.format_item->clear();
  photo.scrollArea->ensureVisible(0, 0);
  photo.id_item->setFocus();
  photo_diag->show();
}

/*
** -- slotClosePhoto() --
*/

void qtbook_photographcollection::slotClosePhoto(void)
{
  photo_diag->close();
}

/*
** -- verifyItemFields()
*/

bool qtbook_photographcollection::verifyItemFields(void)
{
  QString str("");

  str = photo.id_item->text().trimmed();
  photo.id_item->setText(str);

  if(photo.id_item->text().isEmpty())
    {
      QMessageBox::critical(photo_diag, tr("BiblioteQ: User Error"),
			    tr("Please complete the item's "
			       "ID field."));
      photo.id_item->setFocus();
      return false;
    }

  str = photo.title_item->text().trimmed();
  photo.title_item->setText(str);

  if(photo.title_item->text().isEmpty())
    {
      QMessageBox::critical(photo_diag, tr("BiblioteQ: User Error"),
			    tr("Please complete the item's "
			       "Title field."));
      photo.title_item->setFocus();
      return false;
    }

  str = photo.creators_item->toPlainText().trimmed();
  photo.creators_item->setPlainText(str);

  if(photo.title_item->text().isEmpty())
    {
      QMessageBox::critical(photo_diag, tr("BiblioteQ: User Error"),
			    tr("Please complete the item's "
			       "Creator(s) field."));
      photo.creators_item->setFocus();
      return false;
    }

  str = photo.medium_item->text().trimmed();
  photo.medium_item->setText(str);

  if(photo.medium_item->text().isEmpty())
    {
      QMessageBox::critical(photo_diag, tr("BiblioteQ: User Error"),
			    tr("Please complete the item's "
			       "Medium field."));
      photo.medium_item->setFocus();
      return false;
    }

  str = photo.reproduction_number_item->toPlainText().trimmed();
  photo.reproduction_number_item->setPlainText(str);

  if(photo.reproduction_number_item->toPlainText().isEmpty())
    {
      QMessageBox::critical(photo_diag, tr("BiblioteQ: User Error"),
			    tr("Please complete the item's "
			       "Reproduction Number field."));
      photo.reproduction_number_item->setFocus();
      return false;
    }

  str = photo.copyright_item->toPlainText().trimmed();
  photo.copyright_item->setPlainText(str);

  if(photo.copyright_item->toPlainText().isEmpty())
    {
      QMessageBox::critical(photo_diag, tr("BiblioteQ: User Error"),
			    tr("Please complete the item's "
			       "Copyright field."));
      photo.copyright_item->setFocus();
      return false;
    }

  return true;
}

/*
** -- slotInsertItem() --
*/

void qtbook_photographcollection::slotInsertItem(void)
{
  if(!verifyItemFields())
    return;

  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!qmain->getDB().transaction())
    {
      qapp->restoreOverrideCursor();
      qmain->addError(QString(tr("Database Error")),
		      QString(tr("Unable to create a database transaction.")),
		      qmain->getDB().lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(photo_diag, tr("BiblioteQ: Database Error"),
			    tr("Unable to create a database transaction."));
      return;
    }
  else
    qapp->restoreOverrideCursor();

  int pages = 1;
  QString errorstr("");
  QSqlQuery query(qmain->getDB());

  if(qmain->getDB().driverName() != "QSQLITE")
    query.prepare("INSERT INTO photograph "
		  "(id, collection_oid, title, creators, pdate, "
		  "quantity, medium, reproduction_number, "
		  "copyright, callnumber, other_number, notes, subjects, "
		  "format, image, image_scaled) "
		  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, "
		  "?, ?, ?, ?, ?, ?, ?, ?)");
  else
    query.prepare("INSERT INTO photograph "
		  "(id, collection_oid, title, creators, pdate, "
		  "quantity, medium, reproduction_number, "
		  "copyright, callnumber, other_number, notes, subjects, "
		  "format, image, image_scaled, myoid) "
		  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, "
		  "?, ?, ?, ?, ?, ?, ?, ?, ?)");

  query.bindValue(0, photo.id_item->text());
  query.bindValue(1, oid);
  query.bindValue(2, photo.title_item->text());
  query.bindValue(3, photo.creators_item->toPlainText());
  query.bindValue(4, photo.publication_date->date().toString("MM/dd/yyyy"));
  query.bindValue(5, photo.quantity->value());
  query.bindValue(6, photo.medium_item->text());
  query.bindValue(7, photo.reproduction_number_item->toPlainText());
  query.bindValue(8, photo.copyright_item->toPlainText());
  query.bindValue(9, photo.call_number_item->text().trimmed());
  query.bindValue(10, photo.other_number_item->text().trimmed());
  query.bindValue(11, photo.notes_item->toPlainText().trimmed());
  query.bindValue(12, photo.subjects_item->toPlainText().trimmed());
  query.bindValue(13, photo.format_item->toPlainText().trimmed());

  if(!photo.thumbnail_item->image.isNull())
    {
      QImage image;
      QByteArray bytes;
      QBuffer buffer(&bytes);

      if(buffer.open(QIODevice::WriteOnly))
	{
	  photo.thumbnail_item->image.save
	    (&buffer, photo.thumbnail_item->imageFormat.toLatin1(), 100);
	  query.bindValue(14, bytes.toBase64());
	}
      else
	query.bindValue(14, QVariant(QVariant::ByteArray));

      buffer.close();
      bytes.clear();
      image = photo.thumbnail_item->image;
      image = image.scaled
	(126, 187, Qt::KeepAspectRatio, Qt::SmoothTransformation);

      if(buffer.open(QIODevice::WriteOnly))
	{
	  image.save
	    (&buffer, photo.thumbnail_item->imageFormat.toLatin1(), 100);
	  query.bindValue(15, bytes.toBase64());
	}
      else
	query.bindValue(15, QVariant(QVariant::ByteArray));
    }
  else
    {
      photo.thumbnail_item->imageFormat = "";
      query.bindValue(14, QVariant(QVariant::ByteArray));
      query.bindValue(15, QVariant(QVariant::ByteArray));
    }

  if(qmain->getDB().driverName() == "QSQLITE")
    {
      qint64 value = misc_functions::getSqliteUniqueId(qmain->getDB(),
						       errorstr);

      if(errorstr.isEmpty())
	query.bindValue(16, value);
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
      qmain->addError(QString(tr("Database Error")),
		      QString(tr("Unable to create or update the entry.")),
		      query.lastError().text(), __FILE__, __LINE__);
      goto db_rollback;
    }
  else
    {
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

      qapp->restoreOverrideCursor();
    }

  qapp->setOverrideCursor(Qt::WaitCursor);

  if(query.exec(QString("SELECT COUNT(*) "
			"FROM photograph "
			"WHERE collection_oid = %1").
		arg(oid)))
    if(query.next())
      pages = qCeil(query.value(0).toDouble() / PHOTOGRAPHS_PER_PAGE);

  qapp->restoreOverrideCursor();
  pages = qMax(1, pages);
  pc.page->blockSignals(true);
  pc.page->clear();

  for(int i = 1; i <= pages; i++)
    pc.page->addItem(QString::number(i));

  pc.page->blockSignals(false);
  qapp->setOverrideCursor(Qt::WaitCursor);
  showPhotographs(pc.page->currentText().toInt());
  qapp->restoreOverrideCursor();
  photo.saveButton->disconnect(SIGNAL(clicked(void)));
  connect(photo.saveButton, SIGNAL(clicked(void)), this,
	  SLOT(slotModifyItem(void)));
  return;

 db_rollback:
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!qmain->getDB().rollback())
    qmain->addError
      (QString(tr("Database Error")), QString(tr("Rollback failure.")),
       qmain->getDB().lastError().text(), __FILE__, __LINE__);

  qapp->restoreOverrideCursor();
  QMessageBox::critical(photo_diag, tr("BiblioteQ: Database Error"),
			tr("Unable to create the item. "
			   "Please verify that "
			   "the item does not already exist."));
}

/*
** -- slotSceneSelectionChanged() --
*/

void qtbook_photographcollection::slotSceneSelectionChanged(void)
{
  QList<QGraphicsItem *> items(pc.graphicsView->scene()->selectedItems());

  if(items.isEmpty())
    {
      m_itemOid.clear();
      pc.thumbnail_item->clear();
      pc.id_item->clear();
      pc.title_item->clear();
      pc.creators_item->clear();
      pc.publication_date->setDate
	(QDate::fromString("01/01/2000", "MM/dd/yyyy"));
      pc.quantity->setValue(1);
      pc.medium_item->clear();
      pc.reproduction_number_item->clear();
      pc.copyright_item->clear();
      pc.call_number_item->clear();
      pc.other_number_item->clear();
      pc.notes_item->clear();
      pc.subjects_item->clear();
      pc.format_item->clear();
      return;
    }

  QGraphicsPixmapItem *item = 0;

  if((item = qgraphicsitem_cast<QGraphicsPixmapItem *> (items.
							takeFirst())))
    {
      m_itemOid = item->data(0).toString();

      QSqlQuery query(qmain->getDB());

      if(query.exec(QString("SELECT id, "
			    "title, "
			    "creators, "
			    "pdate, "
			    "quantity, "
			    "medium, "
			    "reproduction_number, "
			    "copyright, "
			    "callnumber, "
			    "other_number, "
			    "notes, "
			    "subjects, "
			    "format, "
			    "image "
			    "FROM photograph "
			    "WHERE collection_oid = %1 AND "
			    "myoid = %2").
		    arg(oid).
		    arg(item->data(0).toString())))
	if(query.next())
	  for(int i = 0; i < query.record().count(); i++)
	    {
	      QString fieldname(query.record().fieldName(i));
	      QVariant var(query.record().field(i).value());

	      if(fieldname == "id")
		{
		  pc.id_item->setText(var.toString());
		  photo.id_item->setText(var.toString());
		}
	      else if(fieldname == "title")
		{
		  pc.title_item->setText(var.toString());
		  photo.title_item->setText(var.toString());
		}
	      else if(fieldname == "creators")
		{
		  pc.creators_item->setPlainText(var.toString());
		  photo.creators_item->setPlainText(var.toString());
		}
	      else if(fieldname == "pdate")
		{
		  pc.publication_date->setDate
		    (QDate::fromString(var.toString(), "MM/dd/yyyy"));
		  photo.publication_date->setDate
		    (QDate::fromString(var.toString(), "MM/dd/yyyy"));
		}
	      else if(fieldname == "quantity")
		{
		  pc.quantity->setValue(var.toInt());
		  photo.quantity->setValue(var.toInt());
		}
	      else if(fieldname == "medium")
		{
		  pc.medium_item->setText(var.toString());
		  photo.medium_item->setText(var.toString());
		}
	      else if(fieldname == "reproduction_number")
		{
		  pc.reproduction_number_item->setPlainText(var.toString());
		  photo.reproduction_number_item->setPlainText(var.toString());
		}
	      else if(fieldname == "copyright")
		{
		  pc.copyright_item->setPlainText(var.toString());
		  photo.copyright_item->setPlainText(var.toString());
		}
	      else if(fieldname == "callnumber")
		{
		  pc.call_number_item->setText(var.toString());
		  photo.call_number_item->setText(var.toString());
		}
	      else if(fieldname == "other_number")
		{
		  pc.other_number_item->setText(var.toString());
		  photo.other_number_item->setText(var.toString());
		}
	      else if(fieldname == "notes")
		{
		  pc.notes_item->setPlainText(var.toString());
		  photo.notes_item->setPlainText(var.toString());
		}
	      else if(fieldname == "subjects")
		{
		  pc.subjects_item->setPlainText(var.toString());
		  photo.subjects_item->setPlainText(var.toString());
		}
	      else if(fieldname == "format")
		{
		  pc.format_item->setPlainText(var.toString());
		  photo.format_item->setPlainText(var.toString());
		}
	      else if(fieldname == "image")
		{
		  if(!query.record().field(i).isNull())
		    {
		      pc.thumbnail_item->loadFromData
			(QByteArray::fromBase64(var.toByteArray()));
		      photo.thumbnail_item->loadFromData
			(QByteArray::fromBase64(var.toByteArray()));

		      if(pc.thumbnail_collection->image.isNull())
			{
			  pc.thumbnail_item->loadFromData
			    (var.toByteArray());
			  photo.thumbnail_item->loadFromData
			    (var.toByteArray());
			}
		    }
		  else
		    {
		      pc.thumbnail_item->clear();
		      photo.thumbnail_item->clear();
		    }
		}
	    }
    }
}

/*
** -- slotModifyItem() --
*/

void qtbook_photographcollection::slotModifyItem(void)
{
  photo.saveButton->disconnect(SIGNAL(clicked(void)));
  connect(photo.saveButton, SIGNAL(clicked(void)), this,
	  SLOT(slotUpdateItem(void)));
  photo_diag->resize(photo_diag->width(),
		     qRound(0.95 * size().height()));
  misc_functions::center(photo_diag, this);
  photo.id_item->setFocus();
  photo.scrollArea->ensureVisible(0, 0);
  photo_diag->show();
}

/*
** -- storeData() --
*/

void qtbook_photographcollection::storeData(void)
{
  QString classname = "";
  QString objectname = "";
  QList<QWidget *> list;

  widgetValues.clear();
  list << pc.thumbnail_collection
       << pc.id_collection
       << pc.title_collection
       << pc.location
       << pc.about_collection
       << pc.notes_collection;

  foreach(QWidget *widget, list)
    {
      classname = widget->metaObject()->className();
      objectname = widget->objectName();

      if(classname == "QLineEdit")
	widgetValues[objectname] =
	  (static_cast<QLineEdit *> (widget))->text().trimmed();
      else if(classname == "QComboBox")
	widgetValues[objectname] =
	  (static_cast<QComboBox *> (widget))->currentText().trimmed();
      else if(classname == "QTextEdit")
	widgetValues[objectname] =
	  (static_cast<QTextEdit *> (widget))->toPlainText().trimmed();
      else if(classname == "image_drop_site")
	imageValues[objectname] =
	  (static_cast<image_drop_site *> (widget))->image;
    }
}

/*
** -- slotUpdateItem() --
*/

void qtbook_photographcollection::slotUpdateItem(void)
{
    if(!verifyItemFields())
    return;

  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!qmain->getDB().transaction())
    {
      qapp->restoreOverrideCursor();
      qmain->addError(QString(tr("Database Error")),
		      QString(tr("Unable to create a database transaction.")),
		      qmain->getDB().lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(photo_diag, tr("BiblioteQ: Database Error"),
			    tr("Unable to create a database transaction."));
      return;
    }
  else
    qapp->restoreOverrideCursor();

  QString errorstr("");
  QSqlQuery query(qmain->getDB());

  query.prepare("UPDATE photograph SET "
		"id = ?, title = ?, "
		"creators = ?, pdate = ?, "
		"quantity = ?, medium = ?, reproduction_number = ?, "
		"copyright = ?, callnumber = ?, other_number = ?, "
		"notes = ?, subjects = ?, "
		"format = ?, image = ?, image_scaled = ? "
		"WHERE collection_oid = ? AND myoid = ?");
  query.bindValue(0, photo.id_item->text());
  query.bindValue(1, photo.title_item->text());
  query.bindValue(2, photo.creators_item->toPlainText());
  query.bindValue(3, photo.publication_date->date().toString("MM/dd/yyyy"));
  query.bindValue(4, photo.quantity->value());
  query.bindValue(5, photo.medium_item->text());
  query.bindValue(6, photo.reproduction_number_item->toPlainText());
  query.bindValue(7, photo.copyright_item->toPlainText());
  query.bindValue(8, photo.call_number_item->text().trimmed());
  query.bindValue(9, photo.other_number_item->text().trimmed());
  query.bindValue(10, photo.notes_item->toPlainText().trimmed());
  query.bindValue(11, photo.subjects_item->toPlainText().trimmed());
  query.bindValue(12, photo.format_item->toPlainText().trimmed());

  if(!photo.thumbnail_item->image.isNull())
    {
      QImage image;
      QBuffer buffer;
      QByteArray bytes;

      buffer.setBuffer(&bytes);

      if(buffer.open(QIODevice::WriteOnly))
	{
	  photo.thumbnail_item->image.save
	    (&buffer, photo.thumbnail_item->imageFormat.toLatin1(), 100);
	  query.bindValue(13, bytes.toBase64());
	}
      else
	query.bindValue(13, QVariant(QVariant::ByteArray));

      buffer.close();
      bytes.clear();
      image = photo.thumbnail_item->image;
      image = image.scaled
	(126, 187, Qt::KeepAspectRatio, Qt::SmoothTransformation);

      if(buffer.open(QIODevice::WriteOnly))
	{
	  image.save
	    (&buffer, photo.thumbnail_item->imageFormat.toLatin1(), 100);
	  query.bindValue(14, bytes.toBase64());
	}
      else
	query.bindValue(14, QVariant(QVariant::ByteArray));
    }
  else
    {
      photo.thumbnail_item->imageFormat = "";
      query.bindValue(13, QVariant(QVariant::ByteArray));
      query.bindValue(14, QVariant(QVariant::ByteArray));
    }

  query.bindValue(15, oid);
  query.bindValue(16, m_itemOid);
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec())
    {
      qapp->restoreOverrideCursor();
      qmain->addError(QString(tr("Database Error")),
		      QString(tr("Unable to create or update the entry.")),
		      query.lastError().text(), __FILE__, __LINE__);
      goto db_rollback;
    }
  else
    {
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

      qapp->restoreOverrideCursor();
      pc.id_item->setText(photo.id_item->text());
      pc.title_item->setText(photo.title_item->text());
      pc.creators_item->setPlainText(photo.creators_item->toPlainText());
      pc.publication_date->setDate
	(photo.publication_date->date());
      pc.quantity->setValue(photo.quantity->value());
      pc.medium_item->setText(photo.medium_item->text());
      pc.reproduction_number_item->setPlainText
	(photo.reproduction_number_item->toPlainText());
      pc.copyright_item->setPlainText(photo.copyright_item->toPlainText());
      pc.call_number_item->setText(photo.call_number_item->text());
      pc.other_number_item->setText(photo.other_number_item->text());
      pc.notes_item->setPlainText(photo.notes_item->toPlainText());
      pc.subjects_item->setPlainText(photo.subjects_item->toPlainText());
      pc.format_item->setPlainText(photo.format_item->toPlainText());
      pc.thumbnail_item->setImage(photo.thumbnail_item->image);
    }

  return;

 db_rollback:
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!qmain->getDB().rollback())
    qmain->addError
      (QString(tr("Database Error")), QString(tr("Rollback failure.")),
       qmain->getDB().lastError().text(), __FILE__, __LINE__);

  qapp->restoreOverrideCursor();
  QMessageBox::critical(photo_diag, tr("BiblioteQ: Database Error"),
			tr("Unable to update the item. "
			   "Please verify that "
			   "the item does not already exist."));
}

/*
** -- slotDeleteItem() --
*/

void qtbook_photographcollection::slotDeleteItem(void)
{
  QList<QGraphicsItem *> items(pc.graphicsView->scene()->selectedItems());

  if(items.isEmpty())
    return;
  else
    {
      if(QMessageBox::question(this, tr("BiblioteQ: Question"),
			       tr("Are you sure that you wish to permanently "
				  "delete the selected item(s)?"),
			       QMessageBox::Yes | QMessageBox::No,
			       QMessageBox::No) == QMessageBox::No)
	return;
    }

  QProgressDialog progress(this);

#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  progress.setAttribute(Qt::WA_MacMetalStyle, true);
#endif
#endif
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.setLabelText(tr("Deleting the selected item(s)..."));
  progress.setMaximum(items.size());
  progress.setMinimum(0);
  progress.show();
  progress.update();

  int i = 0;

  while(!items.isEmpty())
    {
      if(i + 1 <= progress.maximum())
	progress.setValue(i + 1);

      QGraphicsPixmapItem *item = 0;

      if((item = qgraphicsitem_cast<QGraphicsPixmapItem *> (items.
							    takeFirst())))
	{
	  QString itemOid(item->data(0).toString());
	  QSqlQuery query(qmain->getDB());

	  query.prepare("DELETE FROM photograph WHERE "
			"collection_oid = ? AND myoid = ?");
	  query.bindValue(0, oid);
	  query.bindValue(1, itemOid);
	  query.exec();
	}

      progress.update();
#ifndef Q_OS_MAC
      qapp->processEvents();
#endif
    }

  int pages = 1;
  QSqlQuery query(qmain->getDB());

  if(query.exec(QString("SELECT COUNT(*) "
			"FROM photograph "
			"WHERE collection_oid = %1").
		arg(oid)))
    if(query.next())
      pages = qCeil(query.value(0).toDouble() / PHOTOGRAPHS_PER_PAGE);

  pages = qMax(1, pages);
  pc.page->blockSignals(true);
  pc.page->clear();

  for(int i = 1; i <= pages; i++)
    pc.page->addItem(QString::number(i));

  pc.page->blockSignals(false);
  showPhotographs(pc.page->currentText().toInt());
  progress.hide();
}

/*
** -- slotPageChanged() --
*/

void qtbook_photographcollection::slotPageChanged(const QString &text)
{
  showPhotographs(text.toInt());
}

/*
** -- slotExportPhotographs() --
*/

void qtbook_photographcollection::slotExportPhotographs(void)
{
  QFileDialog dialog(this);

#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  dialog.setAttribute(Qt::WA_MacMetalStyle, true);
#endif
#endif
  dialog.setFileMode(QFileDialog::Directory);
  dialog.setDirectory(QDir::homePath());
  dialog.setWindowTitle(tr("BiblioteQ: Photograph Collection Photographs "
			   "Export"));
  dialog.exec();

  if(dialog.result() == QDialog::Accepted &&
     dialog.selectedFiles().size() > 0)
    {
      qapp->setOverrideCursor(Qt::WaitCursor);

      QAction *action = qobject_cast<QAction *> (sender());

      if(!action ||
	 action == pc.exportPhotographsToolButton->menu()->actions().value(0))
	misc_functions::exportPhotographs(qmain->getDB(), oid, -1,
					  dialog.selectedFiles().value(0));
      else
	misc_functions::exportPhotographs
	  (qmain->getDB(), oid, pc.page->currentText().toInt(),
	   dialog.selectedFiles().value(0));

      qapp->restoreOverrideCursor();
    }
}

/*
** -- slotViewContextMenu() --
*/

void qtbook_photographcollection::slotViewContextMenu(const QPoint &pos)
{
  QGraphicsPixmapItem *item = qgraphicsitem_cast<QGraphicsPixmapItem *>
    (pc.graphicsView->itemAt(pos));

  if(item)
    {
      QMenu menu(this);
      QAction *action = 0;

      action = menu.addAction(tr("&View Photograph"),
			      this,
			      SLOT(slotViewPhotograph(void)));

      if(action)
	{
	  action->setData(pos);
	  menu.exec(QCursor::pos());
	}
    }
}

/*
** -- slotViewPhotograph() --
*/

void qtbook_photographcollection::slotViewPhotograph(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  QPoint pos(action->data().toPoint());
  QGraphicsPixmapItem *item = qgraphicsitem_cast<QGraphicsPixmapItem *>
    (pc.graphicsView->itemAt(pos));

  if(item)
    {
      QMainWindow *mainWindow = 0;
      Ui_photographView ui;

      if((mainWindow = new(std::nothrow) QMainWindow(this)) != 0)
	{
	  mainWindow->setAttribute(Qt::WA_DeleteOnClose, true);
	  ui.setupUi(mainWindow);
	  connect(ui.closeButton,
		  SIGNAL(clicked(void)),
		  mainWindow,
		  SLOT(close(void)));

	  QGraphicsScene *scene = 0;

	  if((scene = new(std::nothrow) QGraphicsScene(mainWindow)) != 0)
	    {
	      ui.view->setScene(scene);

	      QSqlQuery query(qmain->getDB());

	      qapp->setOverrideCursor(Qt::WaitCursor);

	      if(query.exec(QString("SELECT image FROM "
				    "photograph WHERE "
				    "collection_oid = %1 AND "
				    "myoid = %2").
			    arg(oid).
			    arg(item->data(0).toLongLong())))
		if(query.next())
		  {
		    QImage image;

		    image.loadFromData
		      (QByteArray::fromBase64(query.value(0).
					      toByteArray()));

		    if(image.isNull())
		      image.loadFromData(query.value(0).toByteArray());

		    if(image.isNull())
		      image = QImage("icons.d/no_image.png");

		    scene->addPixmap(QPixmap().fromImage(image));
		    misc_functions::center(mainWindow, this);
		    mainWindow->show();
		  }

	      qapp->restoreOverrideCursor();
	    }
	}
    }
}
