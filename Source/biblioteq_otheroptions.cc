#include "biblioteq.h"
#include "biblioteq_otheroptions.h"

#include <QColorDialog>
#include <QSettings>

biblioteq_otheroptions::biblioteq_otheroptions(biblioteq *parent):QMainWindow()
{
  m_ui.setupUi(this);
  qmain = parent;
  connect(m_ui.close,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotClose(void)));
  connect(m_ui.main_window_canvas_background_color,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSelectMainwindowCanvasBackgroundColor(void)));
  connect(m_ui.save,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSave(void)));

  if(qmain)
    connect(qmain,
	    SIGNAL(fontChanged(const QFont &)),
	    this,
	    SLOT(setGlobalFonts(const QFont &)));

#if QT_VERSION >= 0x050000
  m_ui.publication_date->verticalHeader()->setSectionResizeMode
    (QHeaderView::Fixed);
#else
  m_ui.publication_date->verticalHeader()->setResizeMode(QHeaderView::Fixed);
#endif
  prepareSettings();
}

biblioteq_otheroptions::~biblioteq_otheroptions()
{
}

QString biblioteq_otheroptions::publicationDateFormat
(const QString &itemType) const
{
  for(int i = 0; i < m_ui.publication_date->rowCount(); i++)
    if(m_ui.publication_date->item(i, ITEM_TYPE) &&
       itemType.toLower().trimmed() ==
       m_ui.publication_date->item(i, ITEM_TYPE)->data(Qt::UserRole).toString().
       toLower())
      {
	auto widget = m_ui.publication_date->cellWidget
	  (i, PUBLICATION_DATE_FORMAT);

	if(widget)
	  {
	    auto comboBox = widget->findChild<QComboBox *> ();

	    if(comboBox)
	      return comboBox->currentText();
	  }

	break;
      }

  return "MM/dd/yyyy";
}

bool biblioteq_otheroptions::showMainTableImages(void) const
{
  QSettings settings;

  return settings.value("show_maintable_images", true).toBool();
}

void biblioteq_otheroptions::changeEvent(QEvent *event)
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

void biblioteq_otheroptions::closeEvent(QCloseEvent *event)
{
  QMainWindow::closeEvent(event);
}

void biblioteq_otheroptions::keyPressEvent(QKeyEvent *event)
{
  if(event && event->key() == Qt::Key_Escape)
    close();

  QMainWindow::keyPressEvent(event);
}

void biblioteq_otheroptions::prepareSettings(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  m_ui.publication_date->setRowCount(0);

  QSettings settings;
  QStringList list1;
  QStringList list2;
  QStringList list3;

  list1 << tr("Books")
	<< tr("DVDs")
	<< tr("Grey Literature")
	<< tr("Journals")
	<< tr("Magazines")
	<< tr("Music CDs")
	<< tr("Photograph Collections")
	<< tr("Video Games");
  list2 << settings.value("otheroptions/book_publication_date_format").
           toString()
	<< settings.value("otheroptions/dvd_publication_date_format").toString()
	<< settings.value("otheroptions/grey_literature_date_format").toString()
	<< settings.value("otheroptions/journal_publication_date_format").
           toString()
	<< settings.value("otheroptions/magazine_publication_date_format").
           toString()
    	<< settings.value("otheroptions/cd_publication_date_format").toString()
	<< settings.value("otheroptions/photograph_publication_date_format").
           toString()
	<< settings.value("otheroptions/videogame_publication_date_format").
           toString();
  list3 << "books"
	<< "dvds"
	<< "greyliterature"
	<< "journals"
	<< "magazines"
	<< "musiccds"
	<< "photographcollections"
	<< "videogames";
  m_ui.publication_date->setRowCount(list1.size());

  for(int i = 0; i < list1.size(); i++)
    {
      QString str(list2.at(i).trimmed());

      if(!(str == "MM/dd/yyyy" ||
	   str == "MM/dd" ||
	   str == "MM/yyyy" ||
	   str == "MM" ||
	   str == "dd/MM/yyyy" ||
	   str == "yyyy" ||
	   str == "yyyy/MM/dd"))
	str = "MM/dd/yyyy";

      auto comboBox = new QComboBox();
      auto item = new QTableWidgetItem(list1.at(i));

      comboBox->addItems(QStringList() << "MM/dd/yyyy"
			               << "MM/dd"
			               << "MM/yyyy"
			               << "MM"
			               << "dd/MM/yyyy"
			               << "yyyy"
			               << "yyyy/MM/dd");

      if(comboBox->findText(str) >= 0)
	comboBox->setCurrentIndex(comboBox->findText(str));
      else
	comboBox->setCurrentIndex(0);

      comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
      comboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

      auto layout = new QHBoxLayout();
      auto spacer = new QSpacerItem
	(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
      auto widget = new QWidget();

      widget->setLayout(layout);
      layout->addWidget(comboBox);
      layout->addSpacerItem(spacer);
      layout->setContentsMargins(0, 0, 0, 0);
      item->setData(Qt::UserRole, list3.at(i));
      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.publication_date->setCellWidget(i, PUBLICATION_DATE_FORMAT, widget);
      m_ui.publication_date->setItem(i, ITEM_TYPE, item);
    }

  m_ui.publication_date->resizeColumnToContents(ITEM_TYPE);
  m_ui.publication_date->resizeRowsToContents();

  QColor color(settings.value("mainwindow_canvas_background_color").
	       toString().trimmed());

  if(!color.isValid())
    color = Qt::white;

  m_ui.main_window_canvas_background_color->setStyleSheet
    (QString("background-color: %1").arg(color.name()));
  m_ui.main_window_canvas_background_color->setText(color.name());
  m_ui.show_maintable_images->setChecked
    (settings.value("show_maintable_images", true).toBool());
  m_ui.show_maintable_tooltips->setChecked
    (settings.value("show_maintable_tooltips", false).toBool());
  QApplication::restoreOverrideCursor();
}

void biblioteq_otheroptions::setGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(auto widget, findChildren<QWidget *> ())
    {
      widget->setFont(font);
      widget->update();
    }

  m_ui.publication_date->resizeRowsToContents();
  update();
}

void biblioteq_otheroptions::showNormal(void)
{
  prepareSettings();
  QMainWindow::showNormal();
}

void biblioteq_otheroptions::slotClose(void)
{
  close();
}

void biblioteq_otheroptions::slotPreviewCanvasBackgroundColor
(const QColor &color)
{
  m_ui.main_window_canvas_background_color->setStyleSheet
    (QString("background-color: %1").arg(color.name()));
  m_ui.main_window_canvas_background_color->setText(color.name());
}

void biblioteq_otheroptions::slotSave(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  QSettings settings;
  QStringList list;

  list << "otheroptions/book_publication_date_format"
       << "otheroptions/dvd_publication_date_format"
       << "otheroptions/grey_literature_date_format"
       << "otheroptions/journal_publication_date_format"
       << "otheroptions/magazine_publication_date_format"
       << "otheroptions/cd_publication_date_format"
       << "otheroptions/photograph_publication_date_format"
       << "otheroptions/videogame_publication_date_format";

  for(int i = 0; i < list.size(); i++)
    {
      QString value("MM/dd/yyyy");
      auto widget = m_ui.publication_date->cellWidget
	(i, PUBLICATION_DATE_FORMAT);
      const QString &key(list.at(i));

      if(widget)
	{
	  auto comboBox = widget->findChild<QComboBox *> ();

	  if(comboBox)
	    value = comboBox->currentText();
	}

      settings.setValue(key, value);
    }

  settings.setValue
    ("mainwindow_canvas_background_color",
     m_ui.main_window_canvas_background_color->text().toLatin1());
  settings.setValue
    ("show_maintable_images", m_ui.show_maintable_images->isChecked());
  settings.setValue
    ("show_maintable_tooltips", m_ui.show_maintable_tooltips->isChecked());
  settings.sync();
  emit mainWindowCanvasBackgroundColorChanged
    (QColor(m_ui.main_window_canvas_background_color->text()));
  emit saved();
  QApplication::restoreOverrideCursor();
}

void biblioteq_otheroptions::slotSelectMainwindowCanvasBackgroundColor(void)
{
  QColor color(m_ui.main_window_canvas_background_color->text());
  QColorDialog dialog(this);

  dialog.setOption(QColorDialog::DontUseNativeDialog);

  connect(&dialog,
	  SIGNAL(currentColorChanged(const QColor &)),
	  this,
	  SIGNAL(mainWindowCanvasBackgroundColorPreview(const QColor &)));
  connect(&dialog,
	  SIGNAL(currentColorChanged(const QColor &)),
	  this,
	  SLOT(slotPreviewCanvasBackgroundColor(const QColor &)));

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      m_ui.main_window_canvas_background_color->setStyleSheet
	(QString("background-color: %1").arg(dialog.selectedColor().name()));
      m_ui.main_window_canvas_background_color->setText
	(dialog.selectedColor().name());
    }
  else
    {
      QApplication::processEvents();
      emit mainWindowCanvasBackgroundColorChanged(QColor());
      m_ui.main_window_canvas_background_color->setStyleSheet
	(QString("background-color: %1").arg(color.name()));
      m_ui.main_window_canvas_background_color->setText(color.name());
    }
}
