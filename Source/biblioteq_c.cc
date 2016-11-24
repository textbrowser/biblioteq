/*
** -- Qt Includes --
*/

#include <QSettings>

/*
** -- Local Includes --
*/

#include "biblioteq.h"
#include "biblioteq_otheroptions.h"

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
  return m_otheroptions->dateFormat(itemType);
}

/*
** -- slotOtherOptionsSaved() --
*/

void biblioteq::slotOtherOptionsSaved(void)
{
}
