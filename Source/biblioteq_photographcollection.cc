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

#include "biblioteq.h"
#include "biblioteq_bgraphicsscene.h"
#include "biblioteq_graphicsitempixmap.h"
#include "biblioteq_photographcollection.h"
#include "ui_biblioteq_photographview.h"

extern biblioteq *qmain;

/*
** -- biblioteq_photographcollection() --
*/

biblioteq_photographcollection::biblioteq_photographcollection
(QMainWindow *parentArg,
 const QString &oidArg,
 const int rowArg):QMainWindow(), biblioteq_item(rowArg)
{
  QGraphicsScene *scene1 = 0;
  QGraphicsScene *scene2 = 0;
  QGraphicsScene *scene3 = 0;
  QMenu *menu1 = 0;
  QMenu *menu2 = 0;

  if((menu1 = new(std::nothrow) QMenu(this)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  if((menu2 = new(std::nothrow) QMenu(this)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene1 = new(std::nothrow) QGraphicsScene(this)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene2 = new(std::nothrow) QGraphicsScene(this)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene3 = new(std::nothrow) QGraphicsScene(this)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  if((m_photo_diag = new(std::nothrow) QDialog(this)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  pc.setupUi(this);
  pc.thumbnail_item->enableDoubleClickResize(false);

  if((m_scene = new(std::nothrow) biblioteq_bgraphicsscene(pc.
							   graphicsView)) == 0)
    biblioteq::quit("Memory allocation failure", __FILE__, __LINE__);

  connect(m_scene,
	  SIGNAL(selectionChanged(void)),
	  this,
	  SLOT(slotSceneSelectionChanged(void)));
  m_oid = oidArg;
  m_row = rowArg;
  m_isQueryEnabled = false;
  m_parentWid = parentArg;
  photo.setupUi(m_photo_diag);
  photo.thumbnail_item->enableDoubleClickResize(false);
  pc.graphicsView->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(pc.graphicsView,
	  SIGNAL(customContextMenuRequested(const QPoint &)),
	  this,
	  SLOT(slotViewContextMenu(const QPoint &)));
  pc.graphicsView->setScene(m_scene);
  pc.graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
  pc.graphicsView->setRubberBandSelectionMode(Qt::IntersectsItemShape);
  pc.graphicsView->setSceneRect(0, 0,
				5 * 150,
				PHOTOGRAPHS_PER_PAGE / 5 * 200 + 15);
  pc.thumbnail_item->setReadOnly(true);
#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  setAttribute(Qt::WA_MacMetalStyle, true);
  m_photo_diag->setAttribute(Qt::WA_MacMetalStyle, true);
#endif
#endif
  updateFont(QApplication::font(), qobject_cast<QWidget *> (this));
  m_photo_diag->setWindowModality(Qt::WindowModal);
  updateFont(QApplication::font(), qobject_cast<QWidget *> (m_photo_diag));
  connect(pc.select_image_collection, SIGNAL(clicked(void)),
	  this, SLOT(slotSelectImage(void)));
  connect(photo.select_image_item, SIGNAL(clicked(void)),
	  this, SLOT(slotSelectImage(void)));
  connect(pc.okButton, SIGNAL(clicked(void)), this, SLOT(slotGo(void)));
  connect(pc.cancelButton, SIGNAL(clicked(void)), this,
	  SLOT(slotCancel(void)));
  connect(pc.importItems, SIGNAL(clicked(void)), this,
	  SLOT(slotImportItems(void)));
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
  connect(pc.graphicsView->scene(), SIGNAL(itemDoubleClicked(void)),
	  this, SLOT(slotViewPhotograph(void)));
  pc.resetButton->setMenu(menu1);
  pc.exportPhotographsToolButton->setMenu(menu2);

  QString errorstr("");

  QApplication::setOverrideCursor(Qt::WaitCursor);
  pc.location->addItems
    (biblioteq_misc_functions::getLocations(qmain->getDB(),
					    "Photograph Collection",
					    errorstr));
  QApplication::restoreOverrideCursor();

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

  if(m_parentWid)
    resize(qRound(0.95 * m_parentWid->size().width()),
	   qRound(0.95 * m_parentWid->size().height()));

  pc.splitter->setStretchFactor(0, 0);
  pc.splitter->setStretchFactor(1, 1);
  pc.splitter->setStretchFactor(2, 0);
  biblioteq_misc_functions::center(this, m_parentWid);
  biblioteq_misc_functions::hideAdminFields(this, qmain->getRoles());
  biblioteq_misc_functions::highlightWidget
    (photo.id_item, QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (photo.title_item, QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (photo.creators_item->viewport(), QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (photo.medium_item, QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (photo.reproduction_number_item->viewport(), QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (photo.copyright_item->viewport(), QColor(255, 248, 220));
}

/*
** -- ~biblioteq_photographcollection() --
*/

biblioteq_photographcollection::~biblioteq_photographcollection()
{
}

/*
** -- slotGo() --
*/

void biblioteq_photographcollection::slotGo(void)
{
  QString errorstr("");
  QString str("");

  if(m_engWindowTitle.contains("Create") ||
     m_engWindowTitle.contains("Modify"))
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
      else
	QApplication::restoreOverrideCursor();

      QSqlQuery query(qmain->getDB());

      if(m_engWindowTitle.contains("Modify"))
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

      if(!pc.thumbnail_collection->m_image.isNull())
	{
	  QByteArray bytes;
	  QBuffer buffer(&bytes);
	  QImage image;

	  if(buffer.open(QIODevice::WriteOnly))
	    {
	      pc.thumbnail_collection->m_image.save
		(&buffer, pc.thumbnail_collection->m_imageFormat.toLatin1(),
		 100);
	      query.bindValue(5, bytes.toBase64());
	    }
	  else
	    query.bindValue(5, QVariant(QVariant::ByteArray));

	  buffer.close();
	  bytes.clear();
	  image = pc.thumbnail_collection->m_image;

	  if(!image.isNull())
	    image = image.scaled
	      (126, 187, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	  if(buffer.open(QIODevice::WriteOnly))
	    {
	      image.save
		(&buffer, pc.thumbnail_collection->m_imageFormat.toLatin1(),
		 100);
	      query.bindValue(6, bytes.toBase64());
	    }
	  else
	    query.bindValue(6, QVariant(QVariant::ByteArray));
	}
      else
	{
	  pc.thumbnail_collection->m_imageFormat = "";
	  query.bindValue(5, QVariant(QVariant::ByteArray));
	  query.bindValue(6, QVariant(QVariant::ByteArray));
	}

      if(m_engWindowTitle.contains("Modify"))
	query.bindValue(7, m_oid);
      else if(qmain->getDB().driverName() == "QSQLITE")
	{
	  qint64 value = biblioteq_misc_functions::getSqliteUniqueId
	    (qmain->getDB(),
	     errorstr);

	  if(errorstr.isEmpty())
	    query.bindValue(7, value);
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
	  qmain->addError(QString(tr("Database Error")),
			  QString(tr("Unable to create or update the entry.")),
			  query.lastError().text(), __FILE__, __LINE__);
	  goto db_rollback;
	}
      else
	{
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

	  QApplication::restoreOverrideCursor();

	  if(m_engWindowTitle.contains("Modify"))
	    {
	      str = QString(tr("BiblioteQ: Modify Photograph Collection "
			       "Entry (")) +
		pc.id_collection->text() + tr(")");
	      setWindowTitle(str);
	      m_engWindowTitle = "Modify";

	      if((qmain->getTypeFilterString() == "All" ||
		  qmain->getTypeFilterString() == "All Available" ||
		  qmain->getTypeFilterString() == "All Overdue" ||
		  qmain->getTypeFilterString() == "All Requested" ||
		  qmain->getTypeFilterString() == "All Reserved" ||
		  qmain->getTypeFilterString() == "Photograph Collections") &&
		 m_oid == biblioteq_misc_functions::getColumnString
		 (qmain->getUI().table,
		  m_row, qmain->getUI().table->columnNumber("MYOID")) &&
		 biblioteq_misc_functions::getColumnString
		 (qmain->getUI().table,
		  m_row, qmain->getUI().table->columnNumber("Type")) ==
		 "Photograph Collection")
		{
		  qmain->getUI().table->setSortingEnabled(false);

		  QStringList names(qmain->getUI().table->columnNames());

		  for(int i = 0; i < names.size(); i++)
		    {
		      if(names.at(i) == "ID" ||
			 names.at(i) == "ID Number")
			qmain->getUI().table->item(m_row, i)->setText
			  (pc.id_collection->text());
		      else if(names.at(i) == "Title")
			qmain->getUI().table->item(m_row, i)->setText
			  (pc.title_collection->text());
		      else if(names.at(i) == "Location")
			qmain->getUI().table->item(m_row, i)->setText
			  (pc.location->currentText());
		      else if(names.at(i) == "About")
			qmain->getUI().table->item(m_row, i)->setText
			  (pc.about_collection->toPlainText().trimmed());
		    }

		  qmain->getUI().table->setSortingEnabled(true);

		  foreach(QLineEdit *textfield, findChildren<QLineEdit *>())
		    textfield->setCursorPosition(0);

		  qmain->slotResizeColumns();
		}

	      qmain->slotDisplaySummary();
	      qmain->updateSceneItem(m_oid, "Photograph Collection",
				     pc.thumbnail_collection->m_image);
	    }
	  else
	    {
	      QApplication::setOverrideCursor(Qt::WaitCursor);
	      m_oid = biblioteq_misc_functions::getOID
		(pc.id_collection->text(),
		 "Photograph Collection",
		 qmain->getDB(),
		 errorstr);
	      QApplication::restoreOverrideCursor();

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
		qmain->replacePhotographCollection(m_oid, this);

	      updateWindow(biblioteq::EDITABLE);

	      if(qmain->getUI().actionAutoPopulateOnCreation->isChecked())
		(void) qmain->populateTable
		  (biblioteq::POPULATE_ALL, "Photograph Collections",
		   QString(""));

	      raise();
	    }

	  storeData();
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
		       biblioteq_myqstring::escape(pc.title_collection->
						   text().trimmed()) +
		       "%' AND ");

      if(pc.location->currentIndex() != 0)
	searchstr.append("photograph_collection.location = " + E + "'" +
			 biblioteq_myqstring::escape
			 (pc.location->currentText().trimmed()) + "' AND ");

      searchstr.append("COALESCE(photograph_collection.about, '') LIKE " +
		       E + "'%" +
		       biblioteq_myqstring::escape
		       (pc.about_collection->toPlainText().trimmed()) +
		       "%' AND ");
      searchstr.append("COALESCE(photograph_collection.notes, '') LIKE " +
		       E + "'%" +
		       biblioteq_myqstring::escape
		       (pc.notes_collection->toPlainText().trimmed()) +
		       "%'");

      /*
      ** Search the database.
      */

      (void) qmain->populateTable
	(biblioteq::POPULATE_SEARCH, "Photograph Collections", searchstr);
    }
}

/*
** -- search() --
*/

void biblioteq_photographcollection::search
(const QString &field, const QString &value)
{
  Q_UNUSED(field);
  Q_UNUSED(value);
  pc.addItemButton->setVisible(false);
  pc.importItems->setVisible(false);
  pc.thumbnail_collection->setVisible(false);
  pc.select_image_collection->setVisible(false);
  pc.collectionGroup->setVisible(false);
  pc.itemGroup->setVisible(false);
  pc.exportPhotographsToolButton->setVisible(false);
  pc.location->insertItem(0, tr("Any"));
  pc.location->setCurrentIndex(0);

  QList<QAction *> actions = pc.resetButton->menu()->actions();

  if(!actions.isEmpty())
    actions[0]->setVisible(false);

  for(int i = 6; i < actions.size(); i++)
    actions.at(i)->setVisible(false);

  actions.clear();
  setWindowTitle(tr("BiblioteQ: Database Photograph Collection Search"));
  m_engWindowTitle = "Search";
  pc.okButton->setText(tr("&Search"));
  pc.id_collection->setFocus();
  biblioteq_misc_functions::center(this, m_parentWid);
  show();
}

/*
** -- updateWindow() --
*/

void biblioteq_photographcollection::updateWindow(const int state)
{
  QString str = "";

  if(state == biblioteq::EDITABLE)
    {
      pc.okButton->setVisible(true);
      pc.addItemButton->setEnabled(true);
      pc.importItems->setEnabled(true);
      pc.resetButton->setVisible(true);
      str = QString(tr("BiblioteQ: Modify Photograph Collection Entry (")) +
	pc.id_collection->text() + tr(")");
      m_engWindowTitle = "Modify";
      disconnect(m_scene,
		 SIGNAL(deleteKeyPressed(void)),
		 this,
		 SLOT(slotDeleteItem(void)));
      connect(m_scene,
	      SIGNAL(deleteKeyPressed(void)),
	      this,
	      SLOT(slotDeleteItem(void)));
    }
  else
    {
      pc.okButton->setVisible(false);
      pc.addItemButton->setEnabled(false);
      pc.importItems->setEnabled(false);
      pc.resetButton->setVisible(false);
      str = QString(tr("BiblioteQ: View Photograph Collection Details (")) +
	pc.id_collection->text() + tr(")");
      m_engWindowTitle = "View";
    }

  setWindowTitle(str);
}

/*
** -- modify() --
*/

void biblioteq_photographcollection::modify(const int state,
					    const QString &behavior)
{
  QSqlQuery query(qmain->getDB());
  QString fieldname("");
  QString str("");
  QVariant var;

  if(state == biblioteq::EDITABLE)
    {
      disconnect(m_scene,
		 SIGNAL(deleteKeyPressed(void)),
		 this,
		 SLOT(slotDeleteItem(void)));
      connect(m_scene,
	      SIGNAL(deleteKeyPressed(void)),
	      this,
	      SLOT(slotDeleteItem(void)));

      if(behavior.isEmpty())
	{
	  setWindowTitle(tr("BiblioteQ: Modify Photograph Collection Entry"));
	  m_engWindowTitle = "Modify";
	}
      else
	m_engWindowTitle = behavior;

      pc.okButton->setVisible(true);
      pc.addItemButton->setEnabled(true);
      pc.importItems->setEnabled(true);
      pc.resetButton->setVisible(true);
      pc.select_image_collection->setVisible(true);
      biblioteq_misc_functions::highlightWidget
	(pc.id_collection, QColor(255, 248, 220));
      biblioteq_misc_functions::highlightWidget
	(pc.title_collection, QColor(255, 248, 220));
    }
  else
    {
      if(behavior.isEmpty())
	{
	  setWindowTitle(tr("BiblioteQ: View Photograph Collection Details"));
	  m_engWindowTitle = "View";
	}
      else
	m_engWindowTitle = behavior;

      pc.okButton->setVisible(false);
      pc.addItemButton->setVisible(false);
      pc.importItems->setVisible(false);
      pc.resetButton->setVisible(false);
      pc.select_image_collection->setVisible(false);

      QList<QAction *> actions = pc.resetButton->menu()->actions();

      if(!actions.isEmpty())
	actions[0]->setVisible(false);

      actions.clear();
    }

  query.setForwardOnly(true);
  query.prepare("SELECT id, "
		"title, "
		"location, "
		"about, "
		"notes, "
		"image "
		"FROM "
		"photograph_collection "
		"WHERE myoid = ?");
  query.bindValue(0, m_oid);
  pc.okButton->setText(tr("&Save"));
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!query.exec() || !query.next())
    {
      QApplication::restoreOverrideCursor();
      qmain->addError(QString(tr("Database Error")),
		      QString(tr("Unable to retrieve the selected photograph "
				 "collection's data.")),
		      query.lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
			    tr("Unable to retrieve the selected photograph "
			       "collection's data."));
      pc.id_collection->setFocus();
      return;
    }
  else
    {
      QApplication::restoreOverrideCursor();

      for(int i = 0; i < query.record().count(); i++)
	{
	  var = query.record().field(i).value();
	  fieldname = query.record().fieldName(i);

	  if(fieldname == "id")
	    {
	      pc.id_collection->setText(var.toString());

	      if(behavior.isEmpty())
		{
		  if(state == biblioteq::EDITABLE)
		    {
		      str = QString
			(tr("BiblioteQ: Modify Photograph Collection "
			    "Entry (")) +
			var.toString() + tr(")");
		      m_engWindowTitle = "Modify";
		    }
		  else
		    {
		      str = QString(tr("BiblioteQ: View Photograph "
				       "Collection Details (")) +
			var.toString() + tr(")");
		      m_engWindowTitle = "View";
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

		  if(pc.thumbnail_collection->m_image.isNull())
		    pc.thumbnail_collection->loadFromData(var.toByteArray());
		}
	    }
	}

      int pages = 1;

      if(!m_engWindowTitle.contains("Create"))
	{
	  QApplication::setOverrideCursor(Qt::WaitCursor);
	  query.prepare("SELECT COUNT(*) "
			"FROM photograph "
			"WHERE collection_oid = ?");
	  query.bindValue(0, m_oid);

	  if(query.exec())
	    if(query.next())
	      {
		int i = PHOTOGRAPHS_PER_PAGE;

		pages = qCeil(query.value(0).toDouble() / qMax(1, i));
	      }

	  QApplication::restoreOverrideCursor();
	  pages = qMax(1, pages);
	}

      pc.page->blockSignals(true);
      pc.page->clear();

      for(int i = 1; i <= pages; i++)
	pc.page->addItem(QString::number(i));

      pc.page->blockSignals(false);

      if(!m_engWindowTitle.contains("Create"))
	{
	  QApplication::setOverrideCursor(Qt::WaitCursor);
	  showPhotographs(pc.page->currentText().toInt());
	  QApplication::restoreOverrideCursor();
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

void biblioteq_photographcollection::insert(void)
{
  pc.okButton->setText(tr("&Save"));
  pc.addItemButton->setEnabled(false);
  pc.importItems->setEnabled(false);
  pc.publication_date->setDate(QDate::fromString("01/01/2000",
						 "MM/dd/yyyy"));
  biblioteq_misc_functions::highlightWidget
    (pc.id_collection, QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (pc.title_collection, QColor(255, 248, 220));
  setWindowTitle(tr("BiblioteQ: Create Photograph Collection Entry"));
  m_engWindowTitle = "Create";
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

void biblioteq_photographcollection::slotReset(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  if(action != 0)
    {
      QList<QAction *> actions = pc.resetButton->menu()->actions();

      if(actions.size() < 6)
	{
	  // Error.
	}
      else if(action == actions[0])
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

void biblioteq_photographcollection::closeEvent(QCloseEvent *e)
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

  qmain->removePhotographCollection(this);
}

/*
** -- slotCancel() --
*/

void biblioteq_photographcollection::slotCancel(void)
{
  close();
}

/*
** -- slotQuery() --
*/

void biblioteq_photographcollection::slotQuery(void)
{
}

/*
** -- slotPrint() --
*/

void biblioteq_photographcollection::slotPrint(void)
{
  m_html = "";
  m_html += "<b>" + tr("Collection ID:") + "</b> " +
    pc.id_collection->text().trimmed() + "<br>";
  m_html += "<b>" + tr("Collection Title:") + "</b> " +
    pc.title_collection->text().trimmed() + "<br>";
  m_html += "<b>" + tr("Collection Location:") + "</b> " +
    pc.location->currentText().trimmed() + "<br>";
  m_html += "<b>" + tr("Collection About:") + "</b> " +
    pc.about_collection->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Collection Notes:") + "</b> " +
    pc.notes_collection->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Item ID:") + "</b> " +
    pc.id_item->text().trimmed() + "<br>";
  m_html += "<b>" + tr("Item Title:") + "</b> " +
    pc.title_item->text().trimmed() + "<br>";
  m_html += "<b>" + tr("Item Creator(s):") + "</b> " +
    pc.creators_item->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Item Publication Date:") + "</b> " +
    pc.publication_date->date().toString(Qt::ISODate) + "<br>";
  m_html += "<b>" + tr("Item Copies:") + "</b> " +
    pc.quantity->text() + "<br>";
  m_html += "<b>" + tr("Item Medium:") + "</b> " +
    pc.medium_item->text().trimmed() + "<br>";
  m_html += "<b>" + tr("Item Reproduction Number:") + "</b> " +
    pc.reproduction_number_item->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Item Copyright:") + "</b> " +
    pc.copyright_item->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Item Call Number:") + "</b> " +
    pc.call_number_item->text().trimmed() + "<br>";
  m_html += "<b>" + tr("Item Other Number:") + "</b> " +
    pc.other_number_item->text().trimmed() + "<br>";
  m_html += "<b>" + tr("Item Notes:") + "</b> " +
    pc.notes_item->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Item Subjects:") + "</b> " +
    pc.subjects_item->toPlainText().trimmed() + "<br>";
  m_html += "<b>" + tr("Item Format:") + "</b> " +
    pc.format_item->toPlainText().trimmed() + "<br>";
  print(this);
}

/*
** -- slotSelectImage() --
*/

void biblioteq_photographcollection::slotSelectImage(void)
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
	  pc.thumbnail_collection->m_image = QImage(dialog.selectedFiles().
						    value(0));

	  if(dialog.selectedFiles().value(0).lastIndexOf(".") > -1)
	    pc.thumbnail_collection->m_imageFormat =
	      dialog.selectedFiles().value(0).mid
	      (dialog.selectedFiles().value(0).lastIndexOf(".") + 1).
	      toUpper();

	  pc.thumbnail_collection->scene()->addPixmap
	    (QPixmap().fromImage(pc.thumbnail_collection->m_image));

	  if(pc.thumbnail_collection->scene()->items().size() > 0)
	    pc.thumbnail_collection->scene()->items().at(0)->setFlags
	      (QGraphicsItem::ItemIsSelectable);
	}
      else
	{
	  photo.thumbnail_item->clear();
	  photo.thumbnail_item->m_image = QImage(dialog.selectedFiles().
						 value(0));

	  if(dialog.selectedFiles().value(0).lastIndexOf(".") > -1)
	    photo.thumbnail_item->m_imageFormat = dialog.selectedFiles().
	      value(0).mid
	      (dialog.selectedFiles().value(0).lastIndexOf(".") + 1).
	      toUpper();

	  photo.thumbnail_item->scene()->addPixmap
	    (QPixmap().fromImage(photo.thumbnail_item->m_image));

	  if(photo.thumbnail_item->scene()->items().size() > 0)
	    photo.thumbnail_item->scene()->items().at(0)->setFlags
	      (QGraphicsItem::ItemIsSelectable);
	}
    }
}

/*
** -- duplicate() --
*/

void biblioteq_photographcollection::duplicate
(const QString &p_oid, const int state)
{
  modify(state, "Create"); // Initial population.
  pc.addItemButton->setEnabled(false);
  pc.importItems->setEnabled(false);
  m_oid = p_oid;
  setWindowTitle(tr("BiblioteQ: Duplicate Photograph Collection Entry"));
}

/*
** -- changeEvent() --
*/

void biblioteq_photographcollection::changeEvent(QEvent *event)
{
  if(event)
    switch(event->type())
      {
      case QEvent::LanguageChange:
	{
	  pc.retranslateUi(this);
	  photo.retranslateUi(m_photo_diag);
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

void biblioteq_photographcollection::showPhotographs(const int page)
{
  QSqlQuery query(qmain->getDB());

  query.setForwardOnly(true);
  query.prepare("SELECT image_scaled, myoid FROM "
		"photograph WHERE "
		"collection_oid = ? "
		"ORDER BY id "
		"LIMIT ? "
		"OFFSET ?");
  query.bindValue(0, m_oid);
  query.bindValue(1, PHOTOGRAPHS_PER_PAGE);
  query.bindValue(2, PHOTOGRAPHS_PER_PAGE * (page - 1));

  if(query.exec())
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
	  biblioteq_graphicsitempixmap *pixmapItem = 0;

	  image.loadFromData
	    (QByteArray::fromBase64(query.value(0).
				    toByteArray()));

	  if(image.isNull())
	    image.loadFromData(query.value(0).toByteArray());

	  if(image.isNull())
	    image = QImage(":/no_image.png");

	  /*
	  ** The size of no_image.png is 126x187.
	  */

	  if(!image.isNull())
	    image = image.scaled
	      (126, 187, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	  pixmapItem = new(std::nothrow) biblioteq_graphicsitempixmap
	    (QPixmap().fromImage(image), 0);

	  if(pixmapItem)
	    {
	      if(rowIdx == 0)
		pixmapItem->setPos(140 * columnIdx + 15, 15);
	      else
		pixmapItem->setPos(140 * columnIdx + 15, 200 * rowIdx);

	      pixmapItem->setData(0, query.value(1));
	      pixmapItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
	      pc.graphicsView->scene()->addItem(pixmapItem);
	    }

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

void biblioteq_photographcollection::slotAddItem(void)
{
  photo.saveButton->disconnect(SIGNAL(clicked(void)));
  connect(photo.saveButton, SIGNAL(clicked(void)), this,
	  SLOT(slotInsertItem(void)));
  m_photo_diag->resize(m_photo_diag->width(),
		       qRound(0.95 * size().height()));
  biblioteq_misc_functions::center(m_photo_diag, this);
  photo.thumbnail_item->clear();
#if QT_VERSION >= 0x040700
  photo.id_item->setText(QString::number(QDateTime::currentMSecsSinceEpoch()));
#else
  QDateTime dateTime(QDateTime::currentDateTime());

  photo.id_item->setText(QString::
                         number(static_cast<qint64> (dateTime.toTime_t())));
#endif
  photo.title_item->setText("N/A");
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
  m_photo_diag->show();
}

/*
** -- slotClosePhoto() --
*/

void biblioteq_photographcollection::slotClosePhoto(void)
{
  m_photo_diag->close();
}

/*
** -- verifyItemFields()
*/

bool biblioteq_photographcollection::verifyItemFields(void)
{
  QString str("");

  str = photo.id_item->text().trimmed();
  photo.id_item->setText(str);

  if(photo.id_item->text().isEmpty())
    {
      QMessageBox::critical(m_photo_diag, tr("BiblioteQ: User Error"),
			    tr("Please complete the item's "
			       "ID field."));
      photo.id_item->setFocus();
      return false;
    }

  str = photo.title_item->text().trimmed();
  photo.title_item->setText(str);

  if(photo.title_item->text().isEmpty())
    {
      QMessageBox::critical(m_photo_diag, tr("BiblioteQ: User Error"),
			    tr("Please complete the item's "
			       "Title field."));
      photo.title_item->setFocus();
      return false;
    }

  str = photo.creators_item->toPlainText().trimmed();
  photo.creators_item->setPlainText(str);

  if(photo.creators_item->toPlainText().isEmpty())
    {
      QMessageBox::critical(m_photo_diag, tr("BiblioteQ: User Error"),
			    tr("Please complete the item's "
			       "Creator(s) field."));
      photo.creators_item->setFocus();
      return false;
    }

  str = photo.medium_item->text().trimmed();
  photo.medium_item->setText(str);

  if(photo.medium_item->text().isEmpty())
    {
      QMessageBox::critical(m_photo_diag, tr("BiblioteQ: User Error"),
			    tr("Please complete the item's "
			       "Medium field."));
      photo.medium_item->setFocus();
      return false;
    }

  str = photo.reproduction_number_item->toPlainText().trimmed();
  photo.reproduction_number_item->setPlainText(str);

  if(photo.reproduction_number_item->toPlainText().isEmpty())
    {
      QMessageBox::critical(m_photo_diag, tr("BiblioteQ: User Error"),
			    tr("Please complete the item's "
			       "Reproduction Number field."));
      photo.reproduction_number_item->setFocus();
      return false;
    }

  str = photo.copyright_item->toPlainText().trimmed();
  photo.copyright_item->setPlainText(str);

  if(photo.copyright_item->toPlainText().isEmpty())
    {
      QMessageBox::critical(m_photo_diag, tr("BiblioteQ: User Error"),
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

void biblioteq_photographcollection::slotInsertItem(void)
{
  if(!verifyItemFields())
    return;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!qmain->getDB().transaction())
    {
      QApplication::restoreOverrideCursor();
      qmain->addError(QString(tr("Database Error")),
		      QString(tr("Unable to create a database transaction.")),
		      qmain->getDB().lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(m_photo_diag, tr("BiblioteQ: Database Error"),
			    tr("Unable to create a database transaction."));
      return;
    }
  else
    QApplication::restoreOverrideCursor();

  QSqlQuery query(qmain->getDB());
  QString errorstr("");
  int pages = 1;

  query.setForwardOnly(true);

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
  query.bindValue(1, m_oid);
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

  if(!photo.thumbnail_item->m_image.isNull())
    {
      QByteArray bytes;
      QBuffer buffer(&bytes);
      QImage image;

      if(buffer.open(QIODevice::WriteOnly))
	{
	  photo.thumbnail_item->m_image.save
	    (&buffer, photo.thumbnail_item->m_imageFormat.toLatin1(), 100);
	  query.bindValue(14, bytes.toBase64());
	}
      else
	query.bindValue(14, QVariant(QVariant::ByteArray));

      buffer.close();
      bytes.clear();
      image = photo.thumbnail_item->m_image;

      if(!image.isNull())
	image = image.scaled
	  (126, 187, Qt::KeepAspectRatio, Qt::SmoothTransformation);

      if(buffer.open(QIODevice::WriteOnly))
	{
	  image.save
	    (&buffer, photo.thumbnail_item->m_imageFormat.toLatin1(), 100);
	  query.bindValue(15, bytes.toBase64());
	}
      else
	query.bindValue(15, QVariant(QVariant::ByteArray));
    }
  else
    {
      photo.thumbnail_item->m_imageFormat = "";
      query.bindValue(14, QVariant(QVariant::ByteArray));
      query.bindValue(15, QVariant(QVariant::ByteArray));
    }

  if(qmain->getDB().driverName() == "QSQLITE")
    {
      qint64 value = biblioteq_misc_functions::getSqliteUniqueId
	(qmain->getDB(), errorstr);

      if(errorstr.isEmpty())
	query.bindValue(16, value);
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
      qmain->addError(QString(tr("Database Error")),
		      QString(tr("Unable to create or update the entry.")),
		      query.lastError().text(), __FILE__, __LINE__);
      goto db_rollback;
    }
  else
    {
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

      QApplication::restoreOverrideCursor();
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);
  query.prepare("SELECT COUNT(*) "
		"FROM photograph "
		"WHERE collection_oid = ?");
  query.bindValue(0, m_oid);

  if(query.exec())
    if(query.next())
      {
	int i = PHOTOGRAPHS_PER_PAGE;

	pages = qCeil(query.value(0).toDouble() / qMax(1, i));
      }

  QApplication::restoreOverrideCursor();
  pages = qMax(1, pages);
  pc.page->blockSignals(true);
  pc.page->clear();

  for(int i = 1; i <= pages; i++)
    pc.page->addItem(QString::number(i));

  pc.page->blockSignals(false);
  QApplication::setOverrideCursor(Qt::WaitCursor);
  showPhotographs(pc.page->currentText().toInt());
  QApplication::restoreOverrideCursor();
  photo.saveButton->disconnect(SIGNAL(clicked(void)));
  connect(photo.saveButton, SIGNAL(clicked(void)), this,
	  SLOT(slotModifyItem(void)));
  return;

 db_rollback:
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!qmain->getDB().rollback())
    qmain->addError
      (QString(tr("Database Error")), QString(tr("Rollback failure.")),
       qmain->getDB().lastError().text(), __FILE__, __LINE__);

  QApplication::restoreOverrideCursor();
  QMessageBox::critical(m_photo_diag, tr("BiblioteQ: Database Error"),
			tr("Unable to create the item. "
			   "Please verify that "
			   "the item does not already exist."));
}

/*
** -- slotSceneSelectionChanged() --
*/

void biblioteq_photographcollection::slotSceneSelectionChanged(void)
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

      query.setForwardOnly(true);
      query.prepare("SELECT id, "
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
		    "WHERE collection_oid = ? AND "
		    "myoid = ?");
      query.bindValue(0, m_oid);
      query.bindValue(1, item->data(0).toString());

      if(query.exec())
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

		      if(pc.thumbnail_item->m_image.isNull())
			pc.thumbnail_item->loadFromData
			  (var.toByteArray());

		      photo.thumbnail_item->loadFromData
			(QByteArray::fromBase64(var.toByteArray()));

		      if(photo.thumbnail_item->m_image.isNull())
			photo.thumbnail_item->loadFromData
			  (var.toByteArray());
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

void biblioteq_photographcollection::slotModifyItem(void)
{
  photo.saveButton->disconnect(SIGNAL(clicked(void)));
  connect(photo.saveButton, SIGNAL(clicked(void)), this,
	  SLOT(slotUpdateItem(void)));
  m_photo_diag->resize(m_photo_diag->width(),
		       qRound(0.95 * size().height()));
  biblioteq_misc_functions::center(m_photo_diag, this);
  photo.id_item->setFocus();
  photo.scrollArea->ensureVisible(0, 0);
  m_photo_diag->show();
}

/*
** -- storeData() --
*/

void biblioteq_photographcollection::storeData(void)
{
  QString classname = "";
  QString objectname = "";
  QList<QWidget *> list;

  m_widgetValues.clear();
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
	m_widgetValues[objectname] =
	  (qobject_cast<QLineEdit *> (widget))->text().trimmed();
      else if(classname == "QComboBox")
	m_widgetValues[objectname] =
	  (qobject_cast<QComboBox *> (widget))->currentText().trimmed();
      else if(classname == "QTextEdit")
	m_widgetValues[objectname] =
	  (qobject_cast<QTextEdit *> (widget))->toPlainText().trimmed();
      else if(classname == "biblioteq_image_drop_site")
	m_imageValues[objectname] =
	  (qobject_cast<biblioteq_image_drop_site *> (widget))->m_image;
    }
}

/*
** -- slotUpdateItem() --
*/

void biblioteq_photographcollection::slotUpdateItem(void)
{
  if(!verifyItemFields())
    return;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!qmain->getDB().transaction())
    {
      QApplication::restoreOverrideCursor();
      qmain->addError(QString(tr("Database Error")),
		      QString(tr("Unable to create a database transaction.")),
		      qmain->getDB().lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(m_photo_diag, tr("BiblioteQ: Database Error"),
			    tr("Unable to create a database transaction."));
      return;
    }
  else
    QApplication::restoreOverrideCursor();

  QSqlQuery query(qmain->getDB());
  QString errorstr("");

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

  if(!photo.thumbnail_item->m_image.isNull())
    {
      QBuffer buffer;
      QByteArray bytes;
      QImage image;

      buffer.setBuffer(&bytes);

      if(buffer.open(QIODevice::WriteOnly))
	{
	  photo.thumbnail_item->m_image.save
	    (&buffer, photo.thumbnail_item->m_imageFormat.toLatin1(), 100);
	  query.bindValue(13, bytes.toBase64());
	}
      else
	query.bindValue(13, QVariant(QVariant::ByteArray));

      buffer.close();
      bytes.clear();
      image = photo.thumbnail_item->m_image;

      if(!image.isNull())
	image = image.scaled
	  (126, 187, Qt::KeepAspectRatio, Qt::SmoothTransformation);

      if(buffer.open(QIODevice::WriteOnly))
	{
	  image.save
	    (&buffer, photo.thumbnail_item->m_imageFormat.toLatin1(), 100);
	  query.bindValue(14, bytes.toBase64());
	}
      else
	query.bindValue(14, QVariant(QVariant::ByteArray));
    }
  else
    {
      photo.thumbnail_item->m_imageFormat = "";
      query.bindValue(13, QVariant(QVariant::ByteArray));
      query.bindValue(14, QVariant(QVariant::ByteArray));
    }

  query.bindValue(15, m_oid);
  query.bindValue(16, m_itemOid);
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!query.exec())
    {
      QApplication::restoreOverrideCursor();
      qmain->addError(QString(tr("Database Error")),
		      QString(tr("Unable to create or update the entry.")),
		      query.lastError().text(), __FILE__, __LINE__);
      goto db_rollback;
    }
  else
    {
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

      QApplication::restoreOverrideCursor();
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
      pc.thumbnail_item->setImage(photo.thumbnail_item->m_image);
    }

  return;

 db_rollback:
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!qmain->getDB().rollback())
    qmain->addError
      (QString(tr("Database Error")), QString(tr("Rollback failure.")),
       qmain->getDB().lastError().text(), __FILE__, __LINE__);

  QApplication::restoreOverrideCursor();
  QMessageBox::critical(m_photo_diag, tr("BiblioteQ: Database Error"),
			tr("Unable to update the item. "
			   "Please verify that "
			   "the item does not already exist."));
}

/*
** -- slotDeleteItem() --
*/

void biblioteq_photographcollection::slotDeleteItem(void)
{
  QList<QGraphicsItem *> items(pc.graphicsView->scene()->selectedItems());

  if(items.isEmpty())
    return;
  else
    {
      if(QMessageBox::question(this, tr("BiblioteQ: Question"),
			       tr("Are you sure that you wish to permanently "
				  "delete the selected %1 item(s)?").
			       arg(items.size()),
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
  progress.setCancelButton(0);
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
	  query.bindValue(0, m_oid);
	  query.bindValue(1, itemOid);
	  query.exec();
	}

      progress.update();
#ifndef Q_OS_MAC
      QApplication::processEvents();
#endif
    }

  QSqlQuery query(qmain->getDB());
  int pages = 1;

  query.setForwardOnly(true);
  query.prepare("SELECT COUNT(*) "
		"FROM photograph "
		"WHERE collection_oid = ?");
  query.bindValue(0, m_oid);

  if(query.exec())
    if(query.next())
      {
	int i = PHOTOGRAPHS_PER_PAGE;

	pages = qCeil(query.value(0).toDouble() / qMax(1, i));
      }

  pages = qMax(1, pages);
  pc.page->blockSignals(true);
  pc.page->clear();

  for(int i = 1; i <= pages; i++)
    pc.page->addItem(QString::number(i));

  pc.page->blockSignals(false);
  progress.hide();
  QApplication::setOverrideCursor(Qt::WaitCursor);
  showPhotographs(pc.page->currentText().toInt());
  QApplication::restoreOverrideCursor();
}

/*
** -- slotPageChanged() --
*/

void biblioteq_photographcollection::slotPageChanged(const QString &text)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  pc.page->repaint();
  showPhotographs(text.toInt());
  QApplication::restoreOverrideCursor();
}

/*
** -- slotExportPhotographs() --
*/

void biblioteq_photographcollection::slotExportPhotographs(void)
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
      QApplication::setOverrideCursor(Qt::WaitCursor);

      QAction *action = qobject_cast<QAction *> (sender());

      if(!action ||
	 action == pc.exportPhotographsToolButton->menu()->actions().value(0))
	biblioteq_misc_functions::exportPhotographs
	  (qmain->getDB(), m_oid, -1,
	   dialog.selectedFiles().value(0));
      else
	biblioteq_misc_functions::exportPhotographs
	  (qmain->getDB(), m_oid, pc.page->currentText().toInt(),
	   dialog.selectedFiles().value(0));

      QApplication::restoreOverrideCursor();
    }
}

/*
** -- slotViewContextMenu() --
*/

void biblioteq_photographcollection::slotViewContextMenu(const QPoint &pos)
{
  QGraphicsPixmapItem *item = qgraphicsitem_cast<QGraphicsPixmapItem *>
    (pc.graphicsView->itemAt(pos));

  if(item)
    {
      QAction *action = 0;
      QMenu menu(this);

      action = menu.addAction(tr("&Modify Photograph"),
			      this,
			      SLOT(slotModifyItem(void)));

      if(m_engWindowTitle != "Modify")
	action->setEnabled(false);

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

void biblioteq_photographcollection::slotViewPhotograph(void)
{
  QAction *action = qobject_cast<QAction *> (sender());
  QPoint pos;

  if(action)
    pos = action->data().toPoint();

  if(pos.isNull())
    pos = pc.graphicsView->mapFromGlobal(QCursor::pos());

  loadPhotographFromItemInNewWindow
    (qgraphicsitem_cast<biblioteq_graphicsitempixmap *> (pc.graphicsView->
							 itemAt(pos)));
}

/*
** -- loadPhotographFromItem() --
*/

void biblioteq_photographcollection::loadPhotographFromItem
(QGraphicsScene *scene, QGraphicsPixmapItem *item, const int percent)
{
  if(!item || !scene)
    return;

  QSqlQuery query(qmain->getDB());

  QApplication::setOverrideCursor(Qt::WaitCursor);
  query.setForwardOnly(true);
  query.prepare("SELECT image FROM "
		"photograph WHERE "
		"collection_oid = ? AND "
		"myoid = ?");
  query.bindValue(0, m_oid);
  query.bindValue(1, item->data(0).toLongLong());

  if(query.exec())
    if(query.next())
      {
	QByteArray bytes(QByteArray::fromBase64(query.value(0).toByteArray()));
	QImage image;

	image.loadFromData(bytes);

	if(image.isNull())
	  {
	    bytes = query.value(0).toByteArray();
	    image.loadFromData(bytes);
	  }

	if(image.isNull())
	  image = QImage(":/no_image.png");

	QImage originalImage(image);
	QSize size(image.size());

	size.setHeight((percent * size.height()) / 100);
	size.setWidth((percent * size.width()) / 100);

	if(!image.isNull())
	  image = image.scaled
	    (size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	pc.graphicsView->scene()->clearSelection();
	scene->clear();

	QGraphicsPixmapItem *pixmapItem = scene->addPixmap
	  (QPixmap().fromImage(image));

	if(pixmapItem)
	  pixmapItem->setData(1, bytes);

	item->setSelected(true);

	if(!scene->items().isEmpty())
	  scene->items().at(0)->setData(0, item->data(0));

	QGraphicsView *view = scene->views().value(0);

	if(view)
	  {
	    view->horizontalScrollBar()->setValue(0);
	    view->verticalScrollBar()->setValue(0);
	  }
      }

  QApplication::restoreOverrideCursor();
}

/*
** -- loadPhotographFromItemInNewWindow() --
*/

void biblioteq_photographcollection::loadPhotographFromItemInNewWindow
(QGraphicsPixmapItem *item)
{
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
	  connect(ui.next,
		  SIGNAL(clicked(void)),
		  this,
		  SLOT(slotViewNextPhotograph(void)));
	  connect(ui.previous,
		  SIGNAL(clicked(void)),
		  this,
		  SLOT(slotViewPreviousPhotograph(void)));
	  connect(ui.view_size,
		  SIGNAL(currentIndexChanged(const QString &)),
		  this,
		  SLOT(slotImageViewSizeChanged(const QString &)));

	  QGraphicsScene *scene = 0;

	  if((scene = new(std::nothrow) QGraphicsScene(mainWindow)) != 0)
	    {
	      ui.view->setScene(scene);
	      loadPhotographFromItem
		(scene, item,
		 ui.view_size->currentText().remove("%").toInt());
	      biblioteq_misc_functions::center(mainWindow, this);
	      mainWindow->show();
	    }
	  else
	    mainWindow->show();
	}
    }
}

/*
** -- slotViewNextPhotograph()
*/

void biblioteq_photographcollection::slotViewNextPhotograph(void)
{
  QToolButton *toolButton = qobject_cast<QToolButton *> (sender());

  if(!toolButton)
    return;

  QWidget *parent = toolButton->parentWidget();

  do
    {
      if(!parent)
	break;

      if(qobject_cast<QMainWindow *> (parent))
	break;

      parent = parent->parentWidget();
    }
  while(true);

  if(!parent)
    return;

  QComboBox *comboBox = parent->findChild<QComboBox *> ();
  QGraphicsScene *scene = parent->findChild<QGraphicsScene *> ();
  int percent = comboBox ? comboBox->currentText().remove("%").toInt() : 100;

  if(scene)
    {
      QGraphicsPixmapItem *item = qgraphicsitem_cast
	<QGraphicsPixmapItem *> (scene->items().value(0));

      if(item)
	{
	  QList<QGraphicsItem *> list
	    (pc.graphicsView->scene()->items(Qt::AscendingOrder));
	  int idx = -1;

	  for(int i = 0; i < list.size(); i++)
	    if(item->data(0) == list.at(i)->data(0))
	      {
		idx = i;
		break;
	      }

	  idx += 1;

	  if(idx >= list.size())
	    idx = 0;

	  loadPhotographFromItem
	    (scene,
	     qgraphicsitem_cast<QGraphicsPixmapItem *> (list.value(idx)),
	     percent);
	}
    }
}

/*
** -- slotViewPreviousPhotograph()
*/

void biblioteq_photographcollection::slotViewPreviousPhotograph(void)
{
  QToolButton *toolButton = qobject_cast<QToolButton *> (sender());

  if(!toolButton)
    return;

  QWidget *parent = toolButton->parentWidget();

  do
    {
      if(!parent)
	break;

      if(qobject_cast<QMainWindow *> (parent))
	break;

      parent = parent->parentWidget();
    }
  while(true);

  if(!parent)
    return;

  QComboBox *comboBox = parent->findChild<QComboBox *> ();
  QGraphicsScene *scene = parent->findChild<QGraphicsScene *> ();
  int percent = comboBox ? comboBox->currentText().remove("%").toInt() : 100;

  if(scene)
    {
      QGraphicsPixmapItem *item = qgraphicsitem_cast
	<QGraphicsPixmapItem *> (scene->items().value(0));

      if(item)
	{
	  QList<QGraphicsItem *> list
	    (pc.graphicsView->scene()->items(Qt::AscendingOrder));
	  int idx = -1;

	  for(int i = 0; i < list.size(); i++)
	    if(item->data(0) == list.at(i)->data(0))
	      {
		idx = i;
		break;
	      }

	  idx -= 1;

	  if(idx < 0)
	    idx = list.size() - 1;

	  loadPhotographFromItem
	    (scene,
	     qgraphicsitem_cast<QGraphicsPixmapItem *> (list.value(idx)),
	     percent);
	}
    }
}

/*
** -- slotImportItems()
*/

void biblioteq_photographcollection::slotImportItems(void)
{
  QFileDialog dialog(this);

#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  dialog.setAttribute(Qt::WA_MacMetalStyle, true);
#endif
#endif
  dialog.setDirectory(QDir::homePath());
  dialog.setFileMode(QFileDialog::Directory);
  dialog.setWindowTitle(tr("BiblioteQ: Photograph Collection Import"));
  dialog.exec();

  if(dialog.result() != QDialog::Accepted)
    return;

  dialog.close();
  repaint();
#ifndef Q_OS_MAC
  QApplication::processEvents();
#endif

  QStringList list;

  list << "*.bmp" << "*.jpg" << "*.jpeg" << "*.png";

  QFileInfoList files(dialog.directory().entryInfoList(list));

  if(files.isEmpty())
    return;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!qmain->getDB().transaction())
    {
      QApplication::restoreOverrideCursor();
      qmain->addError(QString(tr("Database Error")),
		      QString(tr("Unable to create a database transaction.")),
		      qmain->getDB().lastError().text(), __FILE__, __LINE__);
      QMessageBox::critical(m_photo_diag, tr("BiblioteQ: Database Error"),
			    tr("Unable to create a database transaction."));
      return;
    }
  else
    QApplication::restoreOverrideCursor();

  QProgressDialog progress(this);

#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  progress.setAttribute(Qt::WA_MacMetalStyle, true);
#endif
#endif
  progress.setLabelText(tr("Importing image(s)..."));
  progress.setMaximum(files.size());
  progress.setMinimum(0);
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.show();
  progress.update();

  QSqlQuery query(qmain->getDB());
  QString errorstr("");
  int imported = 0;
  int pages = 0;

  query.setForwardOnly(true);

  for(int i = 0; i < files.size(); i++)
    {
      if(i + 1 <= progress.maximum())
	progress.setValue(i + 1);

      progress.update();
#ifndef Q_OS_MAC
      QApplication::processEvents();
#endif

      QByteArray bytes1;
      QFile file(files.at(i).absoluteFilePath());

      if(!file.open(QIODevice::ReadOnly))
	continue;
      else
	bytes1 = file.readAll();

      if(static_cast<int> (bytes1.length()) != file.size())
	continue;

      QImage image;

      if(!image.loadFromData(bytes1))
	continue;

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

      QString id("");

#if QT_VERSION >= 0x040700
      id = QString::number(QDateTime::currentMSecsSinceEpoch());
#else
      QDateTime dateTime(QDateTime::currentDateTime());

      id = QString::number(static_cast<qint64> (dateTime.toTime_t()));
#endif

      query.bindValue(0, id);
      query.bindValue(1, m_oid);
      query.bindValue(2, "N/A");
      query.bindValue(3, "N/A");
      query.bindValue(4, "01/01/2001");
      query.bindValue(5, 1);
      query.bindValue(6, "N/A");
      query.bindValue(7, "N/A");
      query.bindValue(8, "N/A");
      query.bindValue(9, "N/A");
      query.bindValue(10, "N/A");
      query.bindValue(11, "N/A");
      query.bindValue(12, "N/A");
      query.bindValue(13, "N/A");
      query.bindValue(14, bytes1.toBase64());

      QBuffer buffer;
      QByteArray bytes2;
      QString format(biblioteq_misc_functions::imageFormatGuess(bytes1));

      buffer.setBuffer(&bytes2);
      buffer.open(QIODevice::WriteOnly);

      if(!image.isNull())
	image = image.scaled
	  (126, 187, Qt::KeepAspectRatio, Qt::SmoothTransformation);

      if(image.isNull() || !image.save(&buffer, format.toLatin1().constData(),
				       100))
	bytes2 = bytes1;

      query.bindValue(15, bytes2.toBase64());

      if(qmain->getDB().driverName() == "QSQLITE")
	{
	  qint64 value = biblioteq_misc_functions::getSqliteUniqueId
	    (qmain->getDB(),
	     errorstr);

	  if(errorstr.isEmpty())
	    query.bindValue(16, value);
	  else
	    qmain->addError(QString(tr("Database Error")),
			    QString(tr("Unable to generate a unique "
				       "integer.")),
			    errorstr);
	}

      if(!query.exec())
	{
	  qmain->addError(QString(tr("Database Error")),
			  QString(tr("Unable to import photograph.")),
			  query.lastError().text(), __FILE__, __LINE__);
	  goto db_rollback;
	}
      else
	imported += 1;
    }

  progress.close();
  QApplication::setOverrideCursor(Qt::WaitCursor);

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
  else
    QApplication::restoreOverrideCursor();

  QApplication::setOverrideCursor(Qt::WaitCursor);
  query.prepare("SELECT COUNT(*) "
		"FROM photograph "
		"WHERE collection_oid = ?");
  query.bindValue(0, m_oid);

  if(query.exec())
    if(query.next())
      {
	int i = PHOTOGRAPHS_PER_PAGE;

	pages = qCeil(query.value(0).toDouble() / qMax(1, i));
      }

  QApplication::restoreOverrideCursor();
  pages = qMax(1, pages);
  pc.page->blockSignals(true);
  pc.page->clear();

  for(int i = 1; i <= pages; i++)
    pc.page->addItem(QString::number(i));

  pc.page->blockSignals(false);
  QApplication::setOverrideCursor(Qt::WaitCursor);
  showPhotographs(1);
  QApplication::restoreOverrideCursor();
  QMessageBox::information(this,
			   tr("BiblioteQ: Information"),
			   tr("A total of %1 image(s) where imported. "
			      "The directory %2 contains %3 image(s).").
			   arg(imported).
			   arg(dialog.directory().absolutePath()).
			   arg(files.size()));
  return;

 db_rollback:
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!qmain->getDB().rollback())
    qmain->addError
      (QString(tr("Database Error")), QString(tr("Rollback failure.")),
       qmain->getDB().lastError().text(), __FILE__, __LINE__);

  QApplication::restoreOverrideCursor();
  QMessageBox::critical(m_photo_diag, tr("BiblioteQ: Database Error"),
			tr("Unable to import all of the images."));

}

/*
** -- slotImageViewSizeChanged() --
*/

void biblioteq_photographcollection::slotImageViewSizeChanged
(const QString &text)
{
  QComboBox *comboBox = qobject_cast<QComboBox *> (sender());

  if(!comboBox)
    return;

  QWidget *parent = comboBox->parentWidget();

  do
    {
      if(!parent)
	break;

      if(qobject_cast<QMainWindow *> (parent))
	break;

      parent = parent->parentWidget();
    }
  while(true);

  if(!parent)
    return;

  QGraphicsScene *scene = parent->findChild<QGraphicsScene *> ();

  if(scene)
    {
      QGraphicsPixmapItem *item = qgraphicsitem_cast
	<QGraphicsPixmapItem *> (scene->items().value(0));

      if(item)
	{
	  QImage image;

	  if(image.loadFromData(item->data(1).toByteArray()))
	    {
	      QSize size(image.size());
	      int percent = QString(text).remove("%").toInt();

	      size.setHeight((percent * size.height()) / 100);
	      size.setWidth((percent * size.width()) / 100);

	      if(!image.isNull())
		image = image.scaled
		  (size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	      item->setPixmap(QPixmap().fromImage(image));
	    }
	}
    }
}
