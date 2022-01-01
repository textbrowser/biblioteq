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

  m_ui.publication_date->verticalHeader()->setSectionResizeMode
    (QHeaderView::Fixed);
  prepareSettings();
}

biblioteq_otheroptions::~biblioteq_otheroptions()
{
}

QColor biblioteq_otheroptions::availabilityColor(const QString &it) const
{
  QSettings settings;
  QString value("white");
  auto itemType(QString(it).remove(" ").toLower().trimmed());

  if(itemType == "book" || itemType == "books")
    value = settings.value
      ("otheroptions/book_availability_color").toString();
  else if(itemType == "cd" || itemType == "musiccds")
    value = settings.value
      ("otheroptions/cd_availability_color").toString();
  else if(itemType == "dvd" || itemType == "dvds")
    value = settings.value
      ("otheroptions/dvd_availability_color").toString();
  else if(itemType == "greyliterature")
    value = settings.value
      ("otheroptions/grey_literature_availability_color").toString();
  else if(itemType == "journal" || itemType == "journals")
    value = settings.value
      ("otheroptions/journal_availability_color").toString();
  else if(itemType == "magazine" || itemType == "magazines")
    value = settings.value
      ("otheroptions/magazine_availability_color").toString();
  else if(itemType == "videogame" || itemType == "videogames")
    value = settings.value
      ("otheroptions/videogame_availability_color").toString();

  return QColor(value);
}

QString biblioteq_otheroptions::isbn10DisplayFormat(void) const
{
  QSettings settings;
  auto index = qBound
    (0,
     settings.value("otheroptions/isbn10_display_format_index").toInt(),
     m_ui.isbn10_display_format->count() - 1);

  return m_ui.isbn10_display_format->itemText(index);
}

QString biblioteq_otheroptions::isbn13DisplayFormat(void) const
{
  QSettings settings;
  auto index = qBound
    (0,
     settings.value("otheroptions/isbn13_display_format_index").toInt(),
     m_ui.isbn13_display_format->count() - 1);

  return m_ui.isbn13_display_format->itemText(index);
}

QString biblioteq_otheroptions::publicationDateFormat
(const QString &it) const
{
  QSettings settings;
  auto itemType(it.toLower().trimmed());

  if(itemType == "books")
    return settings.value
      ("otheroptions/book_publication_date_format").toString();
  else if(itemType == "dvds")
    return settings.value
      ("otheroptions/dvd_publication_date_format").toString();
  else if(itemType == "greyliterature")
    return settings.value
      ("otheroptions/grey_literature_date_format").toString();
  else if(itemType == "journals")
    return settings.value
      ("otheroptions/journal_publication_date_format").toString();
  else if(itemType == "magazines")
    return settings.value
      ("otheroptions/magazine_publication_date_format").toString();
  else if(itemType == "musiccds")
    return settings.value("otheroptions/cd_publication_date_format").toString();
  else if(itemType == "photographcollections")
    return settings.value
      ("otheroptions/photograph_publication_date_format").toString();
  else if(itemType == "videogames")
    return settings.value
      ("otheroptions/videogame_publication_date_format").toString();
  else
    return "MM/dd/yyyy";
}

bool biblioteq_otheroptions::showMainTableImages(void) const
{
  QSettings settings;

  return settings.value("show_maintable_images", true).toBool();
}

bool biblioteq_otheroptions::showMainTableProgressDialogs(void) const
{
  QSettings settings;

  return settings.value("show_maintable_progress_dialogs", true).toBool();
}

int biblioteq_otheroptions::booksAccessionNumberIndex(void) const
{
  QSettings settings;

  return
    qBound(0,
	   settings.value("otheroptions/books_accession_number_index").toInt(),
	   m_ui.books_accession_number->count() - 1);
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
#ifdef Q_OS_ANDROID
    hide();
#else
    close();
#endif

  QMainWindow::keyPressEvent(event);
}

void biblioteq_otheroptions::prepareAvailability(void)
{
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
	<< tr("Video Games");
  list2 << settings.value("otheroptions/book_availability_color").toString()
	<< settings.value("otheroptions/dvd_availability_color").toString()
	<< settings.value("otheroptions/grey_literature_availability_color").
           toString()
	<< settings.value("otheroptions/journal_availability_color").toString()
	<< settings.value("otheroptions/magazine_availability_color").toString()
	<< settings.value("otheroptions/cd_availability_color").toString()
	<< settings.value("otheroptions/videogame_availability_color").
           toString();
  list3 << "books"
	<< "dvds"
	<< "greyliterature"
	<< "journals"
	<< "magazines"
	<< "musiccds"
	<< "videogames";
  m_ui.availability_color->setRowCount(list1.size());
  m_ui.availability_colors->setChecked
    (settings.value("otheroptions/availability_colors", false).toBool());

  for(int i = 0; i < list1.size(); i++)
    {
      auto item = new QTableWidgetItem(list1.at(i));
      auto layout = new QHBoxLayout();
      auto pushButton = new QPushButton();
      auto spacer1 = new QSpacerItem
	(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
      auto spacer2 = new QSpacerItem
	(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
      auto widget = new QWidget();

      connect(pushButton,
	      SIGNAL(clicked(void)),
	      this,
	      SLOT(slotSelectAvailabilityColor(void)));
      widget->setLayout(layout);
      layout->addSpacerItem(spacer1);
      layout->addWidget(pushButton);
      layout->addSpacerItem(spacer2);
      layout->setContentsMargins(0, 0, 0, 0);
      item->setData(Qt::UserRole, list3.at(i));
      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.availability_color->setCellWidget(i, AVAILABILITY_COLOR, widget);
      m_ui.availability_color->setItem(i, ITEM_TYPE, item);
      pushButton->setText(list2.at(i));
      pushButton->setStyleSheet
	(QString("background-color: %1").arg(list2.at(i)));
    }

  m_ui.availability_color->resizeColumnToContents(ITEM_TYPE);
  m_ui.availability_color->resizeRowsToContents();

}

void biblioteq_otheroptions::prepareSettings(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  prepareAvailability();

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
  m_ui.books_accession_number->setCurrentIndex
    (qBound(0,
	    settings.value("otheroptions/books_accession_number_index").toInt(),
	    m_ui.books_accession_number->count() - 1));
  m_ui.isbn10_display_format->setCurrentIndex
    (qBound(0,
	    settings.value("otheroptions/isbn10_display_format_index").toInt(),
	    m_ui.isbn10_display_format->count() - 1));
  m_ui.isbn13_display_format->setCurrentIndex
    (qBound(0,
	    settings.value("otheroptions/isbn13_display_format_index").toInt(),
	    m_ui.isbn13_display_format->count() - 1));
  m_ui.publication_date->setRowCount(list1.size());

  for(int i = 0; i < list1.size(); i++)
    {
      auto str(list2.at(i).trimmed());

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
      auto spacer1 = new QSpacerItem
	(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
      auto spacer2 = new QSpacerItem
	(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
      auto widget = new QWidget();

      widget->setLayout(layout);
      layout->addSpacerItem(spacer1);
      layout->addWidget(comboBox);
      layout->addSpacerItem(spacer2);
      layout->setContentsMargins(0, 0, 0, 0);
      item->setData(Qt::UserRole, list3.at(i));
      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.publication_date->setCellWidget(i, PUBLICATION_DATE_FORMAT, widget);
      m_ui.publication_date->setItem(i, ITEM_TYPE, item);
    }

  m_ui.publication_date->resizeColumnToContents(ITEM_TYPE);
  m_ui.publication_date->resizeRowsToContents();

  QColor color
    (settings.value("mainwindow_canvas_background_color").toString().trimmed());

  if(!color.isValid())
    color = Qt::white;

  m_ui.main_window_canvas_background_color->setStyleSheet
    (QString("background-color: %1").arg(color.name()));
  m_ui.main_window_canvas_background_color->setText(color.name());
  m_ui.show_maintable_images->setChecked
    (settings.value("show_maintable_images", true).toBool());
  m_ui.show_maintable_progress_dialogs->setChecked
    (settings.value("show_maintable_progress_dialogs", true).toBool());
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
#ifdef Q_OS_ANDROID
  hide();
#else
  close();
#endif
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

  settings.setValue
    ("otheroptions/availability_colors", m_ui.availability_colors->isChecked());

  QStringList list;

  list << "otheroptions/book_availability_color"
       << "otheroptions/dvd_availability_color"
       << "otheroptions/grey_literature_availability_color"
       << "otheroptions/journal_availability_color"
       << "otheroptions/magazine_availability_color"
       << "otheroptions/cd_availability_color"
       << "otheroptions/videogame_availability_color";

  for(int i = 0; i < list.size(); i++)
    {
      QString value("");
      auto widget = m_ui.availability_color->cellWidget(i, AVAILABILITY_COLOR);
      const auto &key(list.at(i));

      if(widget)
	{
	  auto pushButton = widget->findChild<QPushButton *> ();

	  if(pushButton)
	    value = pushButton->text();
	}

      settings.setValue(key, value);
    }

  list.clear();
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
      const auto &key(list.at(i));

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
    ("otheroptions/books_accession_number_index",
     m_ui.books_accession_number->currentIndex());
  settings.setValue
    ("otheroptions/isbn10_display_format_index",
     m_ui.isbn10_display_format->currentIndex());
  settings.setValue
    ("otheroptions/isbn13_display_format_index",
     m_ui.isbn13_display_format->currentIndex());
  settings.setValue
    ("show_maintable_images", m_ui.show_maintable_images->isChecked());
  settings.setValue
    ("show_maintable_progress_dialogs",
     m_ui.show_maintable_progress_dialogs->isChecked());
  settings.setValue
    ("show_maintable_tooltips", m_ui.show_maintable_tooltips->isChecked());
  settings.sync();
  emit mainWindowCanvasBackgroundColorChanged
    (QColor(m_ui.main_window_canvas_background_color->text()));
  emit saved();
  QApplication::restoreOverrideCursor();
}

void biblioteq_otheroptions::slotSelectAvailabilityColor(void)
{
  auto pushButton = qobject_cast<QPushButton *> (sender());

  if(!pushButton)
    return;

  QColorDialog dialog(this);

  dialog.setCurrentColor(QColor(pushButton->text()));
  dialog.setOption(QColorDialog::DontUseNativeDialog);

  if(dialog.exec() == QDialog::Accepted)
    {
      QApplication::processEvents();
      pushButton->setStyleSheet
	(QString("background-color: %1").arg(dialog.selectedColor().name()));
      pushButton->setText(dialog.selectedColor().name());
    }
  else
    QApplication::processEvents();
}

void biblioteq_otheroptions::slotSelectMainwindowCanvasBackgroundColor(void)
{
  QColor color(m_ui.main_window_canvas_background_color->text());
  QColorDialog dialog(this);

  dialog.setCurrentColor(color);
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
