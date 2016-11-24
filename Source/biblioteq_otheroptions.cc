/*
** -- Qt Includes --
*/

#include <QColorDialog>
#include <QSettings>

/*
** -- Local Includes --
*/

#include "biblioteq.h"
#include "biblioteq_otheroptions.h"

extern biblioteq *qmain;

/*
** -- biblioteq_otheroptions() --
*/

biblioteq_otheroptions::biblioteq_otheroptions(void):QMainWindow()
{
  m_ui.setupUi(this);
#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  setAttribute(Qt::WA_MacMetalStyle, BIBLIOTEQ_WA_MACMETALSTYLE);
#endif
#endif
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
  prepareSettings();
}

/*
** -- ~biblioteq_otheroptions() --
*/

biblioteq_otheroptions::~biblioteq_otheroptions()
{
}

/*
** -- dateFormat() --
*/

QString biblioteq_otheroptions::dateFormat(const QString &itemType) const
{
  for(int i = 0; i < m_ui.publication_date->rowCount(); i++)
    if(m_ui.publication_date->item(i, 0) &&
       itemType.toLower().trimmed() ==
       m_ui.publication_date->item(i, 0)->text().toLower())
      {
	if(m_ui.publication_date->item(i, 1))
	  return m_ui.publication_date->item(i, 1)->text();

	break;
      }

  return "MM/dd/yyyy";
}

/*
** -- changeEvent() --
*/

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

/*
** -- closeEvent() --
*/

void biblioteq_otheroptions::closeEvent(QCloseEvent *event)
{
  QMainWindow::closeEvent(event);
}

/*
** -- keyPressEvent() --
*/

void biblioteq_otheroptions::keyPressEvent(QKeyEvent *event)
{
  if(event && event->key() == Qt::Key_Escape)
    close();

  QMainWindow::keyPressEvent(event);
}

/*
** -- prepareSettings() --
*/

void biblioteq_otheroptions::prepareSettings(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  m_ui.publication_date->clearContents();

  QSettings settings;
  QStringList list1;
  QStringList list2;

  list1 << tr("Books")
	<< tr("DVDs")
	<< tr("Journals")
	<< tr("Magazines")
	<< tr("Music CDs")
	<< tr("Photographs")
	<< tr("Video Games");
  list2 << settings.value("otheroptions/book_publication_date_format").
           toString()
	<< settings.value("otheroptions/dvd_publication_date_format").toString()
	<< settings.value("otheroptions/journal_publication_date_format").
           toString()
	<< settings.value("otheroptions/magazine_publication_date_format").
           toString()
    	<< settings.value("otheroptions/cd_publication_date_format").toString()
	<< settings.value("otheroptions/photograph_publication_date_format").
           toString()
	<< settings.value("otheroptions/videogame_publication_date_format").
           toString();
  m_ui.publication_date->setRowCount(list1.size());

  for(int i = 0; i < list1.size(); i++)
    {
      QString str(list2.at(i).trimmed());

      if(!(str == "MM/dd/yyyy" ||
	   str == "MM/dd" ||
	   str == "MM"))
	str = "MM/dd/yyyy";

      QComboBox *comboBox = new(std::nothrow) QComboBox();
      QTableWidgetItem *item = new(std::nothrow) QTableWidgetItem
	(list1.at(i));

      if(!comboBox || !item)
	{
	  qmain->addError(QString(tr("Memory Error")),
			  QString(tr("Unable to allocate memory for "
				     "\"comboBox\" or \"item\". "
				     "This is a serious problem!")),
			  QString(""), __FILE__, __LINE__);
	  continue;
	}

      comboBox->addItems(QStringList() << "MM/dd/yyyy"
			               << "MM/dd"
			               << "MM");

      if(comboBox->findText(str) >= 0)
	comboBox->setCurrentIndex(comboBox->findText(str));
      else
	comboBox->setCurrentIndex(0);

      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_ui.publication_date->setItem(i, 0, item);
      m_ui.publication_date->setCellWidget(i, 1, comboBox);
    }

  m_ui.publication_date->resizeColumnToContents(0);

  QColor color(settings.value("mainwindow_canvas_background_color").
	       toString().trimmed());

  if(!color.isValid())
    color = Qt::white;

  m_ui.main_window_canvas_background_color->setStyleSheet
    (QString("background-color: %1").arg(color.name()));
  m_ui.main_window_canvas_background_color->setText(color.name());
  QApplication::restoreOverrideCursor();
}

/*
** -- setGlobalFonts() --
*/

void biblioteq_otheroptions::setGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(QWidget *widget, findChildren<QWidget *> ())
    widget->setFont(font);
}

/*
** -- showNormal() --
*/

void biblioteq_otheroptions::showNormal(void)
{
  prepareSettings();
  QMainWindow::showNormal();
}

/*
** -- slotClose() --
*/

void biblioteq_otheroptions::slotClose(void)
{
  close();
}

/*
** -- slotPreviewCanvasBackgroundColor() --
*/

void biblioteq_otheroptions::slotPreviewCanvasBackgroundColor
(const QColor &color)
{
  m_ui.main_window_canvas_background_color->setStyleSheet
    (QString("background-color: %1").arg(color.name()));
  m_ui.main_window_canvas_background_color->setText(color.name());
}

/*
** -- slotSave() --
*/

void biblioteq_otheroptions::slotSave(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  QSettings settings;
  QStringList list;

  list << "otheroptions/book_publication_date_format"
       << "otheroptions/dvd_publication_date_format"
       << "otheroptions/journal_publication_date_format"
       << "otheroptions/magazine_publication_date_format"
       << "otheroptions/cd_publication_date_format"
       << "otheroptions/photograph_publication_date_format"
       << "otheroptions/videogame_publication_date_format";

  for(int i = 0; i < list.size(); i++)
    {
      QComboBox *comboBox = qobject_cast<QComboBox *>
	(m_ui.publication_date->cellWidget(i, 1));
      QString key(list.at(i));
      QString value("");

      if(comboBox)
	value = comboBox->currentText();
      else
	value = "MM/dd/yyyy";

      settings.setValue(key, value);
    }

  settings.setValue
    ("mainwindow_canvas_background_color",
     m_ui.main_window_canvas_background_color->text());
  emit mainWindowCanvasBackgroundColorChanged
    (QColor(m_ui.main_window_canvas_background_color->text()));
  QApplication::restoreOverrideCursor();
}

/*
** -- slotSelectMainwindowCanvasBackgroundColor() --
*/

void biblioteq_otheroptions::slotSelectMainwindowCanvasBackgroundColor(void)
{
  QColor color(m_ui.main_window_canvas_background_color->text());
  QColorDialog dialog(this);

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
      m_ui.main_window_canvas_background_color->setStyleSheet
	(QString("background-color: %1").arg(dialog.selectedColor().name()));
      m_ui.main_window_canvas_background_color->setText
	(dialog.selectedColor().name());
    }
  else
    {
      emit mainWindowCanvasBackgroundColorChanged(QColor());
      m_ui.main_window_canvas_background_color->setStyleSheet
	(QString("background-color: %1").arg(color.name()));
      m_ui.main_window_canvas_background_color->setText(color.name());
    }
}
