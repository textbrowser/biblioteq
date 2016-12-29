/*
** -- Qt Includes --
*/

#include <QSettings>

/*
** -- Local Includes --
*/

#include "biblioteq.h"
#include "biblioteq_book.h"
#include "biblioteq_otheroptions.h"
#include "biblioteq_pdfreader.h"

/*
** -- slotShowOtherOptions() --
*/

void biblioteq::slotShowOtherOptions(void)
{
  biblioteq_misc_functions::center(m_otheroptions, this);
  m_otheroptions->showNormal();
  m_otheroptions->activateWindow();
  m_otheroptions->raise();
}

/*
** -- slotPreviewCanvasBackgroundColor() --
*/

void biblioteq::slotPreviewCanvasBackgroundColor(const QColor &color)
{
  ui.graphicsView->scene()->setBackgroundBrush(color);
}

/*
** -- slotMainWindowCanvasBackgroundColorChanged() --
*/

void biblioteq::slotMainWindowCanvasBackgroundColorChanged(const QColor &color)
{
  QSettings settings;

  if(color.isValid())
    {
      settings.setValue("mainwindow_canvas_backgound_color", color.name());
      ui.graphicsView->scene()->setBackgroundBrush(color);
    }
  else
    {
      QColor color(settings.value("mainwindow_canvas_backgound_color").
		   toString().trimmed());

      if(!color.isValid())
	color = Qt::white;

      ui.graphicsView->scene()->setBackgroundBrush(color);
    }
}

/*
** -- publicationDateFormat() --
*/

QString biblioteq::publicationDateFormat(const QString &itemType) const
{
  return m_otheroptions->publicationDateFormat(itemType);
}

/*
** -- slotOtherOptionsSaved() --
*/

void biblioteq::slotOtherOptionsSaved(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  foreach(QWidget *widget, QApplication::topLevelWidgets())
    if(qobject_cast<biblioteq_book *> (widget))
      qobject_cast<biblioteq_book *> (widget)->setPublicationDateFormat
	(m_otheroptions->publicationDateFormat("books"));
    else if(qobject_cast<biblioteq_cd *> (widget))
      qobject_cast<biblioteq_cd *> (widget)->setPublicationDateFormat
	(m_otheroptions->publicationDateFormat("musiccds"));
    else if(qobject_cast<biblioteq_dvd *> (widget))
      qobject_cast<biblioteq_dvd *> (widget)->setPublicationDateFormat
	(m_otheroptions->publicationDateFormat("dvds"));
    else if(qobject_cast<biblioteq_journal *> (widget))
      qobject_cast<biblioteq_journal *> (widget)->setPublicationDateFormat
	(m_otheroptions->publicationDateFormat("journals"));
    else if(qobject_cast<biblioteq_magazine *> (widget))
      qobject_cast<biblioteq_magazine *> (widget)->setPublicationDateFormat
	(m_otheroptions->publicationDateFormat("magazines"));
    else if(qobject_cast<biblioteq_photographcollection *> (widget))
      qobject_cast<biblioteq_photographcollection *> (widget)->
	setPublicationDateFormat
	(m_otheroptions->publicationDateFormat("photographcollections"));
    else if(qobject_cast<biblioteq_videogame *> (widget))
      qobject_cast<biblioteq_videogame *> (widget)->setPublicationDateFormat
	(m_otheroptions->publicationDateFormat("videogames"));

  QApplication::restoreOverrideCursor();
}

/*
** -- slotOpenPDFFile() --
*/

void biblioteq::slotOpenPDFFile(void)
{
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
  QFileDialog dialog(this);

#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  dialog.setAttribute(Qt::WA_MacMetalStyle, BIBLIOTEQ_WA_MACMETALSTYLE);
#endif
#endif
  dialog.setDirectory(QDir::homePath());
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setNameFilter("PDF File (*.pdf)");
  dialog.setWindowTitle(tr("BiblioteQ: Open PDF File"));

  if(dialog.exec() == QDialog::Accepted)
    {
      biblioteq_pdfreader *reader = new (std::nothrow) biblioteq_pdfreader(0);

      if(reader)
	{
	  QApplication::setOverrideCursor(Qt::WaitCursor);
	  reader->load(dialog.selectedFiles().value(0));
	  biblioteq_misc_functions::center(reader, this);
	  reader->show();
	  QApplication::restoreOverrideCursor();
	}
    }
#endif
}
