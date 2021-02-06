#include "biblioteq.h"
#include "biblioteq_bgraphicsscene.h"
#include "biblioteq_graphicsitempixmap.h"
#include "biblioteq_photographcollection.h"
#include "ui_biblioteq_photographview.h"

#include <QFileDialog>
#include <QScrollBar>
#include <QShortcut>
#include <QSqlField>
#include <QSqlRecord>
#include <QUuid>
#include <QtCore/qmath.h>

#include <limits>

biblioteq_photographcollection::biblioteq_photographcollection
(biblioteq *parentArg,
 const QString &oidArg,
 const int rowArg):QMainWindow(), biblioteq_item(rowArg)
{
  qmain = parentArg;

  QGraphicsScene *scene1 = nullptr;
  QGraphicsScene *scene2 = nullptr;
  QGraphicsScene *scene3 = nullptr;
  QMenu *menu1 = nullptr;
  QMenu *menu2 = nullptr;

  m_photo_diag = new QDialog(this);
  menu1 = new QMenu(this);
  menu2 = new QMenu(this);
  scene1 = new QGraphicsScene(this);
  scene2 = new QGraphicsScene(this);
  scene3 = new QGraphicsScene(this);
  pc.setupUi(this);
  setQMain(this);
  pc.publication_date->setDisplayFormat
    (qmain->publicationDateFormat("photographcollections"));
  pc.thumbnail_item->enableDoubleClickResize(false);
  m_scene = new biblioteq_bgraphicsscene(pc.graphicsView);
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

  if(photographsPerPage() != -1) // Unlimited.
    pc.graphicsView->setSceneRect(0, 0,
				  5 * 150,
				  photographsPerPage() / 5 * 200 + 15);

  pc.thumbnail_item->setReadOnly(true);
  new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_A),
		this,
		SLOT(slotSelectAll(void)));
  new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S),
		this,
		SLOT(slotGo(void)));
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
  connect(menu1->addAction(tr("Reset Collection Image")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Collection ID")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Collection Title")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Collection Location")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Collection About")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Collection Notes")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu1->addAction(tr("Reset Accession Number")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu2->addAction(tr("&All...")),
	  SIGNAL(triggered(void)), this, SLOT(slotExportPhotographs(void)));
  connect(menu2->addAction(tr("&Current Page...")),
	  SIGNAL(triggered(void)), this, SLOT(slotExportPhotographs(void)));
  connect(menu2->addAction(tr("&Selected...")),
	  SIGNAL(triggered(void)), this, SLOT(slotExportPhotographs(void)));
  connect(pc.page, SIGNAL(currentIndexChanged(const QString &)),
	  this, SLOT(slotPageChanged(const QString &)));
  connect(pc.graphicsView->scene(), SIGNAL(itemDoubleClicked(void)),
	  this, SLOT(slotViewPhotograph(void)));
  pc.resetButton->setMenu(menu1);
  pc.exportPhotographsToolButton->setMenu(menu2);
  connect(pc.exportPhotographsToolButton,
	  SIGNAL(clicked(void)),
	  pc.exportPhotographsToolButton,
	  SLOT(showMenu(void)));

  if(menu2->actions().size() >= 3)
    menu2->actions().at(2)->setEnabled(false);

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

#ifdef Q_OS_MAC
  foreach(QToolButton *tool_button, findChildren<QToolButton *> ())
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

biblioteq_photographcollection::~biblioteq_photographcollection()
{
}

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
      QApplication::processEvents();
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
      QApplication::processEvents();
      photo.title_item->setFocus();
      return false;
    }

  str = photo.creators_item->toPlainText().trimmed();
  photo.creators_item->setPlainText(str);

  if(photo.creators_item->toPlainText().isEmpty())
    {
      QMessageBox::critical(m_photo_diag, tr("BiblioteQ: User Error"),
			    tr("Please complete the item's "
			       "Creators field."));
      QApplication::processEvents();
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
      QApplication::processEvents();
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
      QApplication::processEvents();
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
      QApplication::processEvents();
      photo.copyright_item->setFocus();
      return false;
    }

  str = photo.accession_number_item->text().trimmed();
  photo.accession_number_item->setText(str);
  return true;
}

int biblioteq_photographcollection::photographsPerPage(void)
{
  int integer = qmain->setting("photographs_per_page").toInt();

  if(!(integer == -1 || (integer >= 25 && integer <= 100)))
    integer = 25;

  return integer;
}

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

void biblioteq_photographcollection::closeEvent(QCloseEvent *event)
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

	    if(event)
	      event->ignore();

	    return;
	  }

	QApplication::processEvents();
      }

  qmain->removePhotographCollection(this);
}

void biblioteq_photographcollection::duplicate(const QString &p_oid,
					       const int state)
{
  modify(state, "Create"); // Initial population.
  pc.addItemButton->setEnabled(false);
  pc.importItems->setEnabled(false);
  m_oid = p_oid;
  setWindowTitle(tr("BiblioteQ: Duplicate Photograph Collection Entry"));
}

void biblioteq_photographcollection::insert(void)
{
  pc.okButton->setText(tr("&Save"));
  pc.addItemButton->setEnabled(false);
  pc.importItems->setEnabled(false);
  pc.publication_date->setDate(QDate::fromString("01/01/2000",
						 "MM/dd/yyyy"));
  pc.accession_number->clear();
  biblioteq_misc_functions::highlightWidget
    (pc.id_collection, QColor(255, 248, 220));
  biblioteq_misc_functions::highlightWidget
    (pc.title_collection, QColor(255, 248, 220));
  setWindowTitle(tr("BiblioteQ: Create Photograph Collection Entry"));
  m_engWindowTitle = "Create";
  pc.id_collection->setFocus();
  pc.id_collection->setText
    (QUuid::createUuid().toString().remove("{").remove("}"));
  pc.page->blockSignals(true);
  pc.page->clear();
  pc.page->addItem("1");
  pc.page->blockSignals(false);
  storeData();
  showNormal();
  activateWindow();
  raise();
}

void biblioteq_photographcollection::loadPhotographFromItem
(QGraphicsScene *scene, QGraphicsPixmapItem *item, const int percent)
{
  if(!item || !scene || !scene->views().value(0))
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
	QString format(biblioteq_misc_functions::imageFormatGuess(bytes));

	image.loadFromData(bytes);

	if(image.isNull())
	  {
	    bytes = query.value(0).toByteArray();
	    image.loadFromData(bytes);
	  }

	if(image.isNull())
	  image = QImage(":/no_image.png");

	QSize size;

	if(percent == 0)
	  size = scene->views().value(0)->size();
	else
	  {
	    size = image.size();
	    size.setHeight((percent * size.height()) / 100);
	    size.setWidth((percent * size.width()) / 100);
	  }

	if(!image.isNull())
	  image = image.scaled
	    (size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	pc.graphicsView->scene()->clearSelection();

	QGraphicsPixmapItem *pixmapItem = nullptr;

	if(!scene->items().isEmpty())
	  {
	    pixmapItem = qgraphicsitem_cast<QGraphicsPixmapItem *>
	      (scene->items().at(0));

	    if(pixmapItem)
	      pixmapItem->setPixmap(QPixmap::fromImage(image));
	  }
	else
	  pixmapItem = scene->addPixmap(QPixmap::fromImage(image));

	if(pixmapItem)
	  pixmapItem->setData(1, bytes);

	item->setSelected(true);

	if(!scene->items().isEmpty())
	  {
	    scene->items().at(0)->setData(0, item->data(0)); // myoid
	    scene->items().at(0)->setData(2, item->data(2)); // Navigation.
	  }

	scene->setSceneRect(scene->itemsBoundingRect());

	auto view = qobject_cast<biblioteq_photograph_view *>
	  (scene->views().value(0));

	if(view)
	  {
	    connect(view,
		    SIGNAL(save(const QImage &,
				const QString &,
				const qint64)),
		    this,
		    SLOT(slotSaveRotatedImage(const QImage &,
					      const QString &,
					      const qint64)),
		    Qt::UniqueConnection);
	    view->horizontalScrollBar()->setValue(0);
	    view->setBestFit(percent == 0);
	    view->setImage(image, format, item->data(0).toLongLong());
	    view->verticalScrollBar()->setValue(0);
	  }
      }

  QApplication::restoreOverrideCursor();
}

void biblioteq_photographcollection::loadPhotographFromItemInNewWindow
(QGraphicsPixmapItem *item)
{
  if(item)
    {
      QMainWindow *mainWindow = nullptr;
      Ui_photographView ui;

      mainWindow = new QMainWindow(this);
      mainWindow->setAttribute(Qt::WA_DeleteOnClose, true);
      ui.setupUi(mainWindow);
      connect(ui.closeButton,
	      SIGNAL(clicked(void)),
	      mainWindow,
	      SLOT(close(void)));
      connect(ui.exportItem,
	      SIGNAL(clicked(void)),
	      this,
	      SLOT(slotExportItem(void)));
      connect(ui.next,
	      SIGNAL(clicked(void)),
	      this,
	      SLOT(slotViewNextPhotograph(void)));
      connect(ui.previous,
	      SIGNAL(clicked(void)),
	      this,
	      SLOT(slotViewPreviousPhotograph(void)));
      connect(ui.rotate_left,
	      SIGNAL(clicked(void)),
	      ui.view,
	      SLOT(slotRotateLeft(void)));
      connect(ui.rotate_right,
	      SIGNAL(clicked(void)),
	      ui.view,
	      SLOT(slotRotateRight(void)));
      connect(ui.save,
	      SIGNAL(clicked(void)),
	      ui.view,
	      SLOT(slotSave(void)));
      connect(ui.view_size,
	      SIGNAL(currentIndexChanged(const QString &)),
	      this,
	      SLOT(slotImageViewSizeChanged(const QString &)));
      ui.save->setVisible(m_engWindowTitle.contains("Modify"));

      auto scene = new QGraphicsScene(mainWindow);

      mainWindow->show();
      biblioteq_misc_functions::center(mainWindow, this);
      mainWindow->hide();
      scene->setProperty("view_size", ui.view->viewport()->size());
      ui.view->setScene(scene);
      loadPhotographFromItem
	(scene, item, ui.view_size->currentText().remove("%").toInt());
      mainWindow->show();
    }
}

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

      setReadOnlyFields(this, false);
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

      setReadOnlyFields(this, true);
      pc.okButton->setVisible(false);
      pc.page->setEnabled(true);
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
		"image, "
		"accession_number "
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
      QApplication::processEvents();
      close();
      return;
    }
  else
    {
      QApplication::restoreOverrideCursor();

      QSqlRecord record(query.record());

      for(int i = 0; i < record.count(); i++)
	{
	  var = record.field(i).value();
	  fieldname = record.fieldName(i);

	  if(fieldname == "id")
	    {
	      pc.id_collection->setText(var.toString().trimmed());

	      if(behavior.isEmpty())
		{
		  if(state == biblioteq::EDITABLE)
		    {
		      str = QString
			(tr("BiblioteQ: Modify Photograph Collection "
			    "Entry (")) +
			var.toString().trimmed() + tr(")");
		      m_engWindowTitle = "Modify";
		    }
		  else
		    {
		      str = QString(tr("BiblioteQ: View Photograph "
				       "Collection Details (")) +
			var.toString().trimmed() + tr(")");
		      m_engWindowTitle = "View";
		    }

		  setWindowTitle(str);
		}
	    }
	  else if(fieldname == "title")
	    pc.title_collection->setText(var.toString().trimmed());
	  else if(fieldname == "location")
	    {
	      if(pc.location->findText(var.toString().trimmed()) > -1)
		pc.location->setCurrentIndex
		  (pc.location->findText(var.toString().trimmed()));
	      else
		pc.location->setCurrentIndex
		  (pc.location->findText(tr("UNKNOWN")));
	    }
	  else if(fieldname == "about")
	    pc.about_collection->setPlainText(var.toString().trimmed());
	  else if(fieldname == "notes")
	    pc.notes_collection->setPlainText(var.toString().trimmed());
	  else if(fieldname == "image")
	    {
	      if(!record.field(i).isNull())
		{
		  pc.thumbnail_collection->loadFromData
		    (QByteArray::fromBase64(var.toByteArray()));

		  if(pc.thumbnail_collection->m_image.isNull())
		    pc.thumbnail_collection->loadFromData(var.toByteArray());
		}
	    }
	  else if(fieldname == "accession_number")
	    pc.accession_number->setText(var.toString().trimmed());
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
		int i = photographsPerPage();

		if(i == -1) // Unlimited.
		  {
		    pages = 1;
		    setSceneRect(query.value(0).toInt());
		  }
		else
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

      foreach(QLineEdit *textfield, findChildren<QLineEdit *> ())
	textfield->setCursorPosition(0);

      storeData();
      showNormal();
      activateWindow();
      raise();
      repaint();
      QApplication::processEvents();

      if(!m_engWindowTitle.contains("Create"))
	showPhotographs(pc.page->currentText().toInt());
    }

  pc.id_collection->setFocus();
}

void biblioteq_photographcollection::search(const QString &field,
					    const QString &value)
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
  pc.accession_number->clear();

  QList<QAction *> actions = pc.resetButton->menu()->actions();

  if(!actions.isEmpty())
    actions[0]->setVisible(false);

  for(int i = 7; i < actions.size(); i++)
    actions.at(i)->setVisible(false);

  actions.clear();
  setWindowTitle(tr("BiblioteQ: Database Photograph Collection Search"));
  m_engWindowTitle = "Search";
  pc.okButton->setText(tr("&Search"));
  pc.id_collection->setFocus();
  biblioteq_misc_functions::center(this, m_parentWid);
  showNormal();
  activateWindow();
  raise();
}

void biblioteq_photographcollection::setSceneRect(const int size)
{
  if(size > 0 && (size / 250 <= std::numeric_limits<int>::max()))
    pc.graphicsView->setSceneRect(0, 0, 5 * 150, size * 250 + 15);
  else
    pc.graphicsView->setSceneRect
      (0, 0, 5 * 150, std::numeric_limits<int>::max());
}

void biblioteq_photographcollection::showPhotographs(const int page)
{
  QProgressDialog progress(this);

  progress.setLabelText(tr("Loading image(s)..."));
  progress.setMaximum(0);
  progress.setMinimum(0);
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.show();
  progress.repaint();
  QApplication::processEvents();

  QSqlQuery query(qmain->getDB());

  query.setForwardOnly(true);

  if(photographsPerPage() == -1) // Unlimited.
    {
      query.prepare("SELECT image_scaled, myoid FROM "
		    "photograph WHERE "
		    "collection_oid = ? "
		    "ORDER BY id");
      query.bindValue(0, m_oid);
    }
  else
    {
      query.prepare("SELECT image_scaled, myoid FROM "
		    "photograph WHERE "
		    "collection_oid = ? "
		    "ORDER BY id "
		    "LIMIT ? "
		    "OFFSET ?");
      query.bindValue(0, m_oid);
      query.bindValue(1, photographsPerPage());
      query.bindValue(2, photographsPerPage() * (page - 1));
    }

  if(query.exec())
    {
      pc.graphicsView->scene()->clear();
      pc.graphicsView->resetTransform();
      pc.graphicsView->verticalScrollBar()->setValue(0);
      pc.graphicsView->horizontalScrollBar()->setValue(0);

      int columnIdx = 0;
      int i = -1;
      int rowIdx = 0;

      while(query.next())
	{
	  i += 1;
	  progress.repaint();
	  QApplication::processEvents();

	  if(progress.wasCanceled())
	    break;

	  QImage image;
	  biblioteq_graphicsitempixmap *pixmapItem = nullptr;

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

	  pixmapItem = new biblioteq_graphicsitempixmap
	    (QPixmap::fromImage(image), nullptr);

	  if(rowIdx == 0)
	    pixmapItem->setPos(140 * columnIdx + 15, 15);
	  else
	    pixmapItem->setPos(140 * columnIdx + 15, 200 * rowIdx);

	  pixmapItem->setData(0, query.value(1)); // myoid
	  pixmapItem->setData(2, i); // Next / previous navigation.
	  pixmapItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
	  pc.graphicsView->scene()->addItem(pixmapItem);
	  columnIdx += 1;

	  if(columnIdx >= 5)
	    {
	      rowIdx += 1;
	      columnIdx = 0;
	    }
	}
    }

  progress.close();
}

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

  photo.id_item->setText
    (QString::number(static_cast<qint64> (dateTime.toTime_t())));
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
  photo.accession_number_item->clear();
  photo.id_item->setFocus();
  m_photo_diag->show();
}

void biblioteq_photographcollection::slotCancel(void)
{
  close();
}

void biblioteq_photographcollection::slotClosePhoto(void)
{
  m_photo_diag->close();
}

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
	{
	  QApplication::processEvents();
	  return;
	}

      QApplication::processEvents();
    }

  QProgressDialog progress(this);

  progress.setCancelButton(nullptr);
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.setLabelText(tr("Deleting the selected item(s)..."));
  progress.setMaximum(items.size());
  progress.setMinimum(0);
  progress.show();
  progress.repaint();
  QApplication::processEvents();

  for(int i = 0; i < items.size(); i++)
    {
      if(i + 1 <= progress.maximum())
	progress.setValue(i + 1);

      progress.repaint();
      QApplication::processEvents();

      QGraphicsPixmapItem *item = nullptr;

      if((item = qgraphicsitem_cast<QGraphicsPixmapItem *> (items.at(i))))
	{
	  QSqlQuery query(qmain->getDB());
	  QString itemOid(item->data(0).toString());

	  query.prepare("DELETE FROM photograph WHERE "
			"collection_oid = ? AND myoid = ?");
	  query.bindValue(0, m_oid);
	  query.bindValue(1, itemOid);

	  if(query.exec())
	    {
	      pc.graphicsView->scene()->removeItem(item);
	      delete item;
	    }
	}
    }

  QSqlQuery query(qmain->getDB());
  int pages = 1;

  query.prepare("SELECT COUNT(*) "
		"FROM photograph "
		"WHERE collection_oid = ?");
  query.bindValue(0, m_oid);

  if(query.exec())
    if(query.next())
      {
	updateTablePhotographCount(query.value(0).toInt());

	int i = photographsPerPage();

	if(i == -1) // Unlimited.
	  {
	    pages = 1;
	    setSceneRect(query.value(0).toInt());
	  }
	else
	  pages = qCeil(query.value(0).toDouble() / qMax(1, i));
      }

  pages = qMax(1, pages);
  pc.page->blockSignals(true);
  pc.page->clear();

  for(int i = 1; i <= pages; i++)
    pc.page->addItem(QString::number(i));

  pc.page->blockSignals(false);
  progress.close();
  repaint();
  QApplication::processEvents();
  showPhotographs(pc.page->currentText().toInt());
}

void biblioteq_photographcollection::slotExportItem(void)
{
  auto pushButton = qobject_cast<QPushButton *> (sender());

  if(!pushButton)
    return;

  QWidget *parent = pushButton->parentWidget();

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

  QByteArray bytes;
  auto scene = parent->findChild<QGraphicsScene *> ();

  if(scene)
    {
      auto item = qgraphicsitem_cast<QGraphicsPixmapItem *>
	(scene->items().value(0));

      if(item)
	bytes = item->data(1).toByteArray();
    }

  if(bytes.isEmpty())
    return;

  QFileDialog dialog(this);

  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setDirectory(QDir::homePath());
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowTitle(tr("BiblioteQ: Photograph Collection Photograph "
			   "Export"));
  dialog.selectFile(QString("biblioteq-image-export.%1").
		    arg(biblioteq_misc_functions::imageFormatGuess(bytes)));

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();

      QFile file(dialog.selectedFiles().value(0));

      if(file.open(QIODevice::WriteOnly))
	{
	  file.write(bytes);
	  file.flush();
	  file.close();
	}
    }

  QApplication::processEvents();
}

void biblioteq_photographcollection::slotExportPhotographs(void)
{
  if(pc.graphicsView->scene()->items().isEmpty())
    return;
  else
    {
      if(!qobject_cast<QAction *> (sender()))
	if(pc.graphicsView->scene()->selectedItems().isEmpty())
	  return;
    }

  QFileDialog dialog(this);

  dialog.setFileMode(QFileDialog::Directory);
  dialog.setDirectory(QDir::homePath());
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowTitle(tr("BiblioteQ: Photograph Collection Photographs "
			   "Export"));
  dialog.exec();
  QApplication::processEvents();

  if(dialog.result() == QDialog::Accepted &&
     dialog.selectedFiles().size() > 0)
    {
      repaint();
      QApplication::processEvents();

      auto action = qobject_cast<QAction *> (sender());

      if(!action ||
	 action == pc.exportPhotographsToolButton->menu()->actions().value(0))
	/*
	** Export all photographs.
	*/

	biblioteq_misc_functions::exportPhotographs
	  (qmain->getDB(),
	   m_oid,
	   -1,
	   photographsPerPage(),
	   dialog.selectedFiles().value(0),
	   this);
      else if(action ==
	      pc.exportPhotographsToolButton->menu()->actions().value(1))
	/*
	** Export the current page.
	*/

	biblioteq_misc_functions::exportPhotographs
	  (qmain->getDB(),
	   m_oid,
	   pc.page->currentText().toInt(),
	   photographsPerPage(),
	   dialog.selectedFiles().value(0),
	   this);
      else
	/*
	** Export the selected photograp(s).
	*/

	biblioteq_misc_functions::exportPhotographs
	  (qmain->getDB(),
	   m_oid,
	   dialog.selectedFiles().value(0),
	   pc.graphicsView->scene()->selectedItems(),
	   this);
    }
}

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
	  QApplication::processEvents();
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
	  QApplication::processEvents();
	  pc.title_collection->setFocus();
	  return;
	}

      pc.about_collection->setPlainText
	(pc.about_collection->toPlainText().trimmed());
      pc.notes_collection->setPlainText
	(pc.notes_collection->toPlainText().trimmed());
      pc.accession_number->setText
	(pc.accession_number->text().trimmed());
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
		      "image_scaled = ?, "
		      "accession_number = ? "
		      "WHERE "
		      "myoid = ?");
      else if(qmain->getDB().driverName() != "QSQLITE")
	query.prepare("INSERT INTO photograph_collection "
		      "(id, title, location, about, notes, image, "
		      "image_scaled, accession_number) VALUES (?, "
		      "?, ?, ?, ?, ?, ?, ?)");
      else
	query.prepare("INSERT INTO photograph_collection "
		      "(id, title, location, about, notes, image, "
		      "image_scaled, accession_number, myoid) "
		      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");

      query.bindValue(0, pc.id_collection->text().trimmed());
      query.bindValue(1, pc.title_collection->text().trimmed());
      query.bindValue(2, pc.location->currentText().trimmed());
      query.bindValue(3, pc.about_collection->toPlainText().trimmed());
      query.bindValue(4, pc.notes_collection->toPlainText().trimmed());

      if(!pc.thumbnail_collection->m_image.isNull())
	{
	  QBuffer buffer;
	  QByteArray bytes;
	  QImage image;

	  buffer.setBuffer(&bytes);

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

      query.bindValue(7, pc.accession_number->text().trimmed());

      if(m_engWindowTitle.contains("Modify"))
	query.bindValue(8, m_oid);
      else if(qmain->getDB().driverName() == "QSQLITE")
	{
	  qint64 value = biblioteq_misc_functions::getSqliteUniqueId
	    (qmain->getDB(), errorstr);

	  if(errorstr.isEmpty())
	    query.bindValue(8, value);
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
		      if(i == 0)
			{
			  QPixmap pixmap
			    (QPixmap::
			     fromImage(pc.thumbnail_collection->m_image));

			  if(!pixmap.isNull())
			    qmain->getUI().table->item(m_row, i)->setIcon
			      (pixmap);
			  else
			    qmain->getUI().table->item(m_row, i)->setIcon
			      (QIcon(":/no_image.png"));
			}

		      if(names.at(i) == "Call Number")
			qmain->getUI().table->item(m_row, i)->setText
			  (pc.call_number_item->text());
		      else if(names.at(i) == "ID" ||
			      names.at(i) == "ID Number")
			qmain->getUI().table->item(m_row, i)->setText
			  (pc.id_collection->text());
		      else if(names.at(i) == "Title")
			qmain->getUI().table->item(m_row, i)->setText
			  (pc.title_collection->text());
		      else if(names.at(i) == "Location")
			qmain->getUI().table->item(m_row, i)->setText
			  (pc.location->currentText().trimmed());
		      else if(names.at(i) == "About")
			qmain->getUI().table->item(m_row, i)->setText
			  (pc.about_collection->toPlainText().trimmed());
		      else if(names.at(i) == "Accession Number")
			qmain->getUI().table->item(m_row, i)->setText
			  (pc.accession_number->text());
		    }

		  qmain->getUI().table->setSortingEnabled(true);

		  foreach(QLineEdit *textfield, findChildren<QLineEdit *> ())
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
		  QApplication::processEvents();
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
      QSqlQuery query(qmain->getDB());
      QString searchstr("");

      searchstr = "SELECT DISTINCT photograph_collection.title, "
	"photograph_collection.id, "
	"photograph_collection.location, "
	"COUNT(photograph.myoid) AS photograph_count, "
	"photograph_collection.about, "
	"photograph_collection.accession_number, "
	"photograph_collection.type, "
	"photograph_collection.myoid, "
	"photograph_collection.image_scaled "
	"FROM photograph_collection LEFT JOIN "
	"photograph "
	"ON photograph_collection.myoid = photograph.collection_oid "
	"WHERE ";

      QString ESCAPE("");
      QString UNACCENT(qmain->unaccent());

      if(qmain->getDB().driverName() != "QSQLITE")
	ESCAPE = "E";

      searchstr.append
	(UNACCENT + "(LOWER(photograph_collection.id)) LIKE " + UNACCENT +
	 "(LOWER(" + ESCAPE + "'%' || ? || '%')) AND ");

      searchstr.append
	(UNACCENT + "(LOWER(photograph_collection.title)) LIKE " +
	 UNACCENT + "(LOWER(" + ESCAPE + "'%' || ? || '%')) AND ");

      if(pc.location->currentIndex() != 0)
	searchstr.append
	  (UNACCENT + "(photograph_collection.location) = " + UNACCENT +
	   "(" + ESCAPE + "'" +
	   biblioteq_myqstring::escape(pc.location->currentText().
				       trimmed()) + "') AND ");

      searchstr.append
	(UNACCENT + "(LOWER(COALESCE(photograph_collection.about, ''))) " +
	 "LIKE " + UNACCENT + "(LOWER(" +
	 ESCAPE + "'%' || ? || '%')) AND ");
      searchstr.append
	(UNACCENT + "(LOWER(COALESCE(photograph_collection.notes, ''))) " +
	 "LIKE " + UNACCENT + "(LOWER(" +
	 ESCAPE + "'%' || ? || '%')) AND ");
      searchstr.append
	(UNACCENT +
	 "(LOWER(COALESCE(photograph_collection.accession_number, ''))) "
	 "LIKE " + UNACCENT + "(LOWER(" + ESCAPE + "'%' || ? || '%'))");
      searchstr.append("GROUP BY photograph_collection.title, "
		       "photograph_collection.id, "
		       "photograph_collection.location, "
		       "photograph_collection.about, "
		       "photograph_collection.accession_number, "
		       "photograph_collection.type, "
		       "photograph_collection.myoid, "
		       "photograph_collection.image_scaled");
      query.prepare(searchstr);
      query.addBindValue(pc.id_collection->text().trimmed());
      query.addBindValue
	(biblioteq_myqstring::escape(pc.title_collection->text().trimmed()));
      query.addBindValue
	(biblioteq_myqstring::escape(pc.about_collection->toPlainText().
				     trimmed()));
      query.addBindValue
	(biblioteq_myqstring::escape(pc.notes_collection->toPlainText().
				     trimmed()));
      query.addBindValue
	(biblioteq_myqstring::escape(pc.accession_number->text().trimmed()));
      (void) qmain->populateTable
	(query,
	 "Photograph Collections",
	 biblioteq::NEW_PAGE,
	 biblioteq::POPULATE_SEARCH);
    }
}

void biblioteq_photographcollection::slotImageViewSizeChanged
(const QString &text)
{
  auto comboBox = qobject_cast<QComboBox *> (sender());

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

  auto scene = parent->findChild<QGraphicsScene *> ();

  if(scene)
    {
      auto item = qgraphicsitem_cast<QGraphicsPixmapItem *>
	(scene->items().value(0));

      if(item)
	{
	  QImage image;

	  if(image.loadFromData(item->data(1).toByteArray()))
	    {
	      QSize size;
	      int percent = QString(text).remove("%").toInt();

	      if(percent == 0)
		{
		  if(scene->views().value(0))
		    {
		      scene->setProperty
			("view_size", scene->views().value(0)->size());
		      size = scene->views().value(0)->size();
		    }
		  else
		    size = scene->property("view_size").toSize();
		}
	      else
		{
		  size = image.size();
		  size.setHeight((percent * size.height()) / 100);
		  size.setWidth((percent * size.width()) / 100);
		}

	      if(!image.isNull())
		image = image.scaled
		  (size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	      item->setPixmap(QPixmap::fromImage(image));
	      scene->setSceneRect(scene->itemsBoundingRect());

	      auto view = qobject_cast<biblioteq_photograph_view *>
		(scene->views().value(0));

	      if(view)
		view->setBestFit(percent == 0);
	    }
	}
    }
}

void biblioteq_photographcollection::slotImportItems(void)
{
  QFileDialog dialog(this);
  QStringList list;

  list << "*" << "*.bmp" << "*.jpg" << "*.jpeg" << "*.png";

  dialog.setDirectory(QDir::homePath());
  dialog.setFileMode(QFileDialog::ExistingFiles);
  dialog.setNameFilters(list);
  dialog.setOption(QFileDialog::DontUseNativeDialog);
  dialog.setWindowTitle(tr("BiblioteQ: Photograph Collection Import"));
  dialog.exec();
  QApplication::processEvents();

  if(dialog.result() != QDialog::Accepted)
    return;

  repaint();
  QApplication::processEvents();

  QStringList files(dialog.selectedFiles());

  if(files.isEmpty())
    return;

  QProgressDialog progress(this);

  progress.setLabelText(tr("Importing image(s)..."));
  progress.setMaximum(files.size());
  progress.setMinimum(0);
  progress.setModal(true);
  progress.setWindowTitle(tr("BiblioteQ: Progress Dialog"));
  progress.show();
  progress.repaint();
  QApplication::processEvents();

  int imported = 0;
  int pages = 0;

  for(int i = 0; i < files.size(); i++)
    {
      if(i + 1 <= progress.maximum())
	progress.setValue(i + 1);

      progress.repaint();
      QApplication::processEvents();

      if(progress.wasCanceled())
	break;

      QByteArray bytes1;
      QFile file(files.at(i));

      if(!file.open(QIODevice::ReadOnly))
	continue;
      else
	bytes1 = file.readAll();

      if(static_cast<qint64> (bytes1.length()) != file.size())
	continue;

      QImage image;

      if(!image.loadFromData(bytes1))
	continue;

      QSqlQuery query(qmain->getDB());

      if(qmain->getDB().driverName() != "QSQLITE")
	query.prepare("INSERT INTO photograph "
		      "(id, collection_oid, title, creators, pdate, "
		      "quantity, medium, reproduction_number, "
		      "copyright, callnumber, other_number, notes, subjects, "
		      "format, image, image_scaled, accession_number) "
		      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, "
		      "?, ?, ?, ?, ?, ?, ?, ?)");
      else
	query.prepare("INSERT INTO photograph "
		      "(id, collection_oid, title, creators, pdate, "
		      "quantity, medium, reproduction_number, "
		      "copyright, callnumber, other_number, notes, subjects, "
		      "format, image, image_scaled, accession_number, myoid) "
		      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, "
		      "?, ?, ?, ?, ?, ?, ?, ?, ?)");

      QString id("");

#if QT_VERSION >= 0x040700
      id = QString::number(QDateTime::currentMSecsSinceEpoch() +
			   static_cast<qint64> (imported));
#else
      QDateTime dateTime(QDateTime::currentDateTime());

      id = QString::number(static_cast<qint64> (dateTime.toTime_t()) +
			   static_cast<qint64> (imported));
#endif

      query.bindValue(0, id);
      query.bindValue(1, m_oid);
      query.bindValue(2, "N/A");
      query.bindValue(3, "N/A");
      query.bindValue(4, "01/01/2000");
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

      if(image.isNull() || !image.save(&buffer,
				       format.toLatin1().constData(),
				       100))
	bytes2 = bytes1;

      query.bindValue(15, bytes2.toBase64());
      query.bindValue(16, "N/A");

      if(qmain->getDB().driverName() == "QSQLITE")
	{
	  QString errorstr("");
	  qint64 value = biblioteq_misc_functions::getSqliteUniqueId
	    (qmain->getDB(),
	     errorstr);

	  if(errorstr.isEmpty())
	    query.bindValue(17, value);
	  else
	    qmain->addError(QString(tr("Database Error")),
			    QString(tr("Unable to generate a unique "
				       "integer.")),
			    errorstr);
	}

      if(!query.exec())
	qmain->addError(QString(tr("Database Error")),
			QString(tr("Unable to import photograph.")),
			query.lastError().text(), __FILE__, __LINE__);
      else
	imported += 1;
    }

  progress.close();
  repaint();
  QApplication::processEvents();
  QApplication::setOverrideCursor(Qt::WaitCursor);

  QSqlQuery query(qmain->getDB());

  query.prepare("SELECT COUNT(*) "
		"FROM photograph "
		"WHERE collection_oid = ?");
  query.bindValue(0, m_oid);

  if(query.exec())
    if(query.next())
      {
	updateTablePhotographCount(query.value(0).toInt());

	int i = photographsPerPage();

	if(i == -1) // Unlimited.
	  {
	    pages = 1;
	    setSceneRect(query.value(0).toInt());
	  }
	else
	  pages = qCeil(query.value(0).toDouble() / qMax(1, i));
      }

  QApplication::restoreOverrideCursor();
  pages = qMax(1, pages);
  pc.page->blockSignals(true);
  pc.page->clear();

  for(int i = 1; i <= pages; i++)
    pc.page->addItem(QString::number(i));

  pc.page->blockSignals(false);
  showPhotographs(1);
  QMessageBox::information(this,
			   tr("BiblioteQ: Information"),
			   tr("Imported a total of %1 image(s) from the "
			      "directory %2.").
			   arg(imported).
			   arg(dialog.directory().absolutePath()));
  QApplication::processEvents();
}

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
      QApplication::processEvents();
      return;
    }
  else
    QApplication::restoreOverrideCursor();

  QSqlQuery query(qmain->getDB());
  QString errorstr("");
  int pages = 1;

  if(qmain->getDB().driverName() != "QSQLITE")
    query.prepare("INSERT INTO photograph "
		  "(id, collection_oid, title, creators, pdate, "
		  "quantity, medium, reproduction_number, "
		  "copyright, callnumber, other_number, notes, subjects, "
		  "format, image, image_scaled, accession_number) "
		  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, "
		  "?, ?, ?, ?, ?, ?, ?, ?)");
  else
    query.prepare("INSERT INTO photograph "
		  "(id, collection_oid, title, creators, pdate, "
		  "quantity, medium, reproduction_number, "
		  "copyright, callnumber, other_number, notes, subjects, "
		  "format, image, image_scaled, accession_number, myoid) "
		  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, "
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
      QBuffer buffer;
      QByteArray bytes;
      QImage image;

      buffer.setBuffer(&bytes);

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

  query.bindValue(16, photo.accession_number_item->text().trimmed());

  if(qmain->getDB().driverName() == "QSQLITE")
    {
      qint64 value = biblioteq_misc_functions::getSqliteUniqueId
	(qmain->getDB(), errorstr);

      if(errorstr.isEmpty())
	query.bindValue(17, value);
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
	updateTablePhotographCount(query.value(0).toInt());

	int i = photographsPerPage();

	if(i == -1) // Unlimited.
	  {
	    pages = 1;
	    setSceneRect(query.value(0).toInt());
	  }
	else
	  pages = qCeil(query.value(0).toDouble() / qMax(1, i));
      }

  QApplication::restoreOverrideCursor();
  pages = qMax(1, pages);
  pc.page->blockSignals(true);
  pc.page->clear();

  for(int i = 1; i <= pages; i++)
    pc.page->addItem(QString::number(i));

  pc.page->blockSignals(false);
  showPhotographs(pc.page->currentText().toInt());
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
  QApplication::processEvents();
}

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

void biblioteq_photographcollection::slotPageChanged(const QString &text)
{
  pc.page->repaint();
  QApplication::processEvents();
  showPhotographs(text.toInt());
}

void biblioteq_photographcollection::slotPrint(void)
{
  m_html = "<html>";
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
  m_html += "<b>" + tr("Item Creators:") + "</b> " +
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
  m_html += "<b>" + tr("Accession Number:") + "</b> " +
    pc.accession_number_item->text().trimmed();
  m_html += "</html>";
  print(this);
}

void biblioteq_photographcollection::slotQuery(void)
{
}

void biblioteq_photographcollection::slotReset(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(action != nullptr)
    {
      QList<QAction *> actions = pc.resetButton->menu()->actions();

      if(actions.size() < 7)
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
      else if(action == actions[6])
	{
	  pc.accession_number->clear();
	  pc.accession_number->setFocus();
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
      pc.accession_number->clear();
      pc.id_collection->setFocus();
    }
}

void biblioteq_photographcollection::slotSaveRotatedImage
(const QImage &image, const QString &format, const qint64 oid)
{
  if(image.isNull() || oid < 0)
    return;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  QBuffer buffer;
  QByteArray bytes1;

  buffer.setBuffer(&bytes1);

  if(buffer.open(QIODevice::WriteOnly) &&
     image.save(&buffer, format.toUpper().toLatin1().constData(), 100))
    {
      QSqlQuery query(qmain->getDB());

      query.prepare("UPDATE photograph SET "
		    "image = ?, "
		    "image_scaled = ? "
		    "WHERE myoid = ?");
      query.addBindValue(bytes1);

      QBuffer buffer;
      QByteArray bytes2;
      QImage i(image);

      buffer.setBuffer(&bytes2);
      buffer.open(QIODevice::WriteOnly);
      i = i.scaled(126, 187, Qt::KeepAspectRatio, Qt::SmoothTransformation);

      if(i.isNull() || !i.save(&buffer,
			       format.toUpper().toLatin1().constData(),
			       100))
	bytes2 = bytes1;

      query.addBindValue(bytes2);
      query.addBindValue(oid);

      if(!query.exec())
	qmain->addError(QString(tr("Database Error")),
			QString(tr("Unable to update photograph.")),
			query.lastError().text(), __FILE__, __LINE__);
      else
	{
	  QList<QGraphicsItem *> list
	    (pc.graphicsView->scene()->items(Qt::AscendingOrder));

	  for(int i = 0; i < list.size(); i++)
	    if(list.at(i)->data(0).toLongLong() == oid)
	      {
		auto item = qgraphicsitem_cast<QGraphicsPixmapItem *>
		  (list.at(i));

		if(item)
		  item->setPixmap
		    (QPixmap::
		     fromImage(image.scaled(126,
					    187,
					    Qt::KeepAspectRatio,
					    Qt::SmoothTransformation)));

		break;
	      }
	}
    }

  QApplication::restoreOverrideCursor();
}

void biblioteq_photographcollection::slotSceneSelectionChanged(void)
{
  QList<QGraphicsItem *> items(pc.graphicsView->scene()->selectedItems());

  if(items.isEmpty())
    {
      m_itemOid.clear();

      if(pc.exportPhotographsToolButton->menu() &&
	 pc.exportPhotographsToolButton->menu()->actions().size() >= 3)
	pc.exportPhotographsToolButton->menu()->actions()[2]->
	  setEnabled(false);

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
      pc.accession_number_item->clear();
      return;
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(pc.exportPhotographsToolButton->menu() &&
     pc.exportPhotographsToolButton->menu()->actions().size() >= 3)
    pc.exportPhotographsToolButton->menu()->actions()[2]->
      setEnabled(true);

  QGraphicsPixmapItem *item = nullptr;

  if((item = qgraphicsitem_cast<QGraphicsPixmapItem *> (items.at(0))))
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
		    "image, "
		    "accession_number "
		    "FROM photograph "
		    "WHERE collection_oid = ? AND "
		    "myoid = ?");
      query.bindValue(0, m_oid);
      query.bindValue(1, item->data(0).toString());

      if(query.exec())
	if(query.next())
	  {
	    QSqlRecord record(query.record());

	    for(int i = 0; i < record.count(); i++)
	      {
		QString fieldname(record.fieldName(i));
		QVariant var(record.field(i).value());

		if(fieldname == "id")
		  {
		    pc.id_item->setText(var.toString().trimmed());
		    photo.id_item->setText(var.toString().trimmed());
		  }
		else if(fieldname == "title")
		  {
		    pc.title_item->setText(var.toString().trimmed());
		    photo.title_item->setText(var.toString().trimmed());
		  }
		else if(fieldname == "creators")
		  {
		    pc.creators_item->setPlainText(var.toString().trimmed());
		    photo.creators_item->setPlainText(var.toString().trimmed());
		  }
		else if(fieldname == "pdate")
		  {
		    pc.publication_date->setDate
		      (QDate::fromString(var.toString().trimmed(),
					 "MM/dd/yyyy"));
		    photo.publication_date->setDate
		      (QDate::fromString(var.toString().trimmed(),
					 "MM/dd/yyyy"));
		  }
		else if(fieldname == "quantity")
		  {
		    pc.quantity->setValue(var.toInt());
		    photo.quantity->setValue(var.toInt());
		  }
		else if(fieldname == "medium")
		  {
		    pc.medium_item->setText(var.toString().trimmed());
		    photo.medium_item->setText(var.toString().trimmed());
		  }
		else if(fieldname == "reproduction_number")
		  {
		    pc.reproduction_number_item->setPlainText
		      (var.toString().trimmed());
		    photo.reproduction_number_item->setPlainText
		      (var.toString().trimmed());
		  }
		else if(fieldname == "copyright")
		  {
		    pc.copyright_item->setPlainText(var.toString().trimmed());
		    photo.copyright_item->setPlainText
		      (var.toString().trimmed());
		  }
		else if(fieldname == "callnumber")
		  {
		    pc.call_number_item->setText(var.toString().trimmed());
		    photo.call_number_item->setText(var.toString().trimmed());
		  }
		else if(fieldname == "other_number")
		  {
		    pc.other_number_item->setText(var.toString().trimmed());
		    photo.other_number_item->setText(var.toString().trimmed());
		  }
		else if(fieldname == "notes")
		  {
		    pc.notes_item->setPlainText(var.toString().trimmed());
		    photo.notes_item->setPlainText(var.toString().trimmed());
		  }
		else if(fieldname == "subjects")
		  {
		    pc.subjects_item->setPlainText(var.toString().trimmed());
		    photo.subjects_item->setPlainText
		      (var.toString().trimmed());
		  }
		else if(fieldname == "format")
		  {
		    pc.format_item->setPlainText(var.toString().trimmed());
		    photo.format_item->setPlainText(var.toString().trimmed());
		  }
		else if(fieldname == "image")
		  {
		    if(!record.field(i).isNull())
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
		else if(fieldname == "accession_number")
		  {
		    pc.accession_number_item->setText(var.toString().trimmed());
		    photo.accession_number_item->setText
		      (var.toString().trimmed());
		  }
	      }
	  }
    }

  QApplication::restoreOverrideCursor();
}

void biblioteq_photographcollection::slotSelectAll(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  QPainterPath painterPath;

  painterPath.addRect(pc.graphicsView->sceneRect());
  pc.graphicsView->scene()->setSelectionArea(painterPath, QTransform());
  QApplication::restoreOverrideCursor();
}

void biblioteq_photographcollection::slotSelectImage(void)
{
  QFileDialog dialog(this);
  auto button = qobject_cast<QPushButton *> (sender());

  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setDirectory(QDir::homePath());
  dialog.setOption(QFileDialog::DontUseNativeDialog);

  if(button == pc.select_image_collection)
    dialog.setWindowTitle(tr("BiblioteQ: Photograph Collection "
			     "Image Selection"));
  else
    dialog.setWindowTitle(tr("BiblioteQ: Photograph Collection Item "
			     "Image Selection"));

  dialog.exec();
  QApplication::processEvents();

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
	    (QPixmap::fromImage(pc.thumbnail_collection->m_image));

	  if(!pc.thumbnail_collection->scene()->items().isEmpty())
	    pc.thumbnail_collection->scene()->items().at(0)->setFlags
	      (QGraphicsItem::ItemIsSelectable);

	  pc.thumbnail_collection->scene()->setSceneRect
	    (pc.thumbnail_collection->scene()->itemsBoundingRect());
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
	    (QPixmap::fromImage(photo.thumbnail_item->m_image));

	  if(!photo.thumbnail_item->scene()->items().isEmpty())
	    photo.thumbnail_item->scene()->items().at(0)->setFlags
	      (QGraphicsItem::ItemIsSelectable);

	  photo.thumbnail_item->scene()->setSceneRect
	    (photo.thumbnail_item->scene()->itemsBoundingRect());
	}
    }
}

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
      QApplication::processEvents();
      return;
    }
  else
    QApplication::restoreOverrideCursor();

  QSqlQuery query(qmain->getDB());

  query.prepare("UPDATE photograph SET "
		"id = ?, title = ?, "
		"creators = ?, pdate = ?, "
		"quantity = ?, medium = ?, reproduction_number = ?, "
		"copyright = ?, callnumber = ?, other_number = ?, "
		"notes = ?, subjects = ?, "
		"format = ?, image = ?, image_scaled = ?, "
		"accession_number = ? "
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

  query.bindValue(15, photo.accession_number_item->text().trimmed());
  query.bindValue(16, m_oid);
  query.bindValue(17, m_itemOid);
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
      pc.accession_number_item->setText(photo.accession_number_item->text());
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
  QApplication::processEvents();
}

void biblioteq_photographcollection::slotViewContextMenu(const QPoint &pos)
{
  auto item = qgraphicsitem_cast<QGraphicsPixmapItem *>
    (pc.graphicsView->itemAt(pos));

  if(item)
    {
      item->setSelected(true);

      QAction *action = nullptr;
      QMenu menu(this);

      action = menu.addAction(tr("&Delete Photograph"),
			      this,
			      SLOT(slotDeleteItem(void)));
      action->setData(pos);

      if(m_engWindowTitle != "Modify")
	action->setEnabled(false);

      action = menu.addAction(tr("&Modify Photograph..."),
			      this,
			      SLOT(slotModifyItem(void)));
      action->setData(pos);

      if(m_engWindowTitle != "Modify")
	action->setEnabled(false);

      action = menu.addAction(tr("&View Photograph..."),
			      this,
			      SLOT(slotViewPhotograph(void)));
      action->setData(pos);
      menu.exec(QCursor::pos());
    }
}

void biblioteq_photographcollection::slotViewNextPhotograph(void)
{
  auto toolButton = qobject_cast<QToolButton *> (sender());

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

  auto comboBox = parent->findChild<QComboBox *> ();
  auto scene = parent->findChild<QGraphicsScene *> ();
  int percent = comboBox ? comboBox->currentText().remove("%").toInt() : 100;

  if(scene)
    {
      auto item = qgraphicsitem_cast<QGraphicsPixmapItem *>
	(scene->items().value(0));

      if(item)
	{
	  QApplication::setOverrideCursor(Qt::WaitCursor);

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

	  QApplication::restoreOverrideCursor();
	  loadPhotographFromItem
	    (scene,
	     qgraphicsitem_cast<QGraphicsPixmapItem *> (list.value(idx)),
	     percent);
	}
    }
}

void biblioteq_photographcollection::slotViewPhotograph(void)
{
  auto action = qobject_cast<QAction *> (sender());
  QPoint pos;

  if(action)
    pos = action->data().toPoint();

  if(pos.isNull())
    pos = pc.graphicsView->mapFromGlobal(QCursor::pos());

  loadPhotographFromItemInNewWindow
    (qgraphicsitem_cast<biblioteq_graphicsitempixmap *> (pc.graphicsView->
							 itemAt(pos)));
}

void biblioteq_photographcollection::slotViewPreviousPhotograph(void)
{
  auto toolButton = qobject_cast<QToolButton *> (sender());

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

  auto comboBox = parent->findChild<QComboBox *> ();
  auto scene = parent->findChild<QGraphicsScene *> ();
  int percent = comboBox ? comboBox->currentText().remove("%").toInt() : 100;

  if(scene)
    {
      auto item = qgraphicsitem_cast<QGraphicsPixmapItem *>
	(scene->items().value(0));

      if(item)
	{
	  QApplication::setOverrideCursor(Qt::WaitCursor);

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

	  QApplication::restoreOverrideCursor();
	  loadPhotographFromItem
	    (scene,
	     qgraphicsitem_cast<QGraphicsPixmapItem *> (list.value(idx)),
	     percent);
	}
    }
}

void biblioteq_photographcollection::storeData(void)
{
  QList<QWidget *> list;
  QString classname("");
  QString objectname("");

  m_widgetValues.clear();
  list << pc.thumbnail_collection
       << pc.id_collection
       << pc.title_collection
       << pc.location
       << pc.about_collection
       << pc.notes_collection
       << pc.accession_number;

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

void biblioteq_photographcollection::updateTablePhotographCount
(const int count)
{
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
	if(names.at(i) == "Photograph Count")
	  {
	    qmain->getUI().table->item(m_row, i)->
	      setText(QString::number(count));
	    qmain->slotDisplaySummary();
	    break;
	  }

      qmain->getUI().table->setSortingEnabled(true);
    }
}

void biblioteq_photographcollection::updateWindow(const int state)
{
  QString str("");

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

  setReadOnlyFields(this, state != biblioteq::EDITABLE);
  setWindowTitle(str);
  pc.page->setEnabled(true);
}
