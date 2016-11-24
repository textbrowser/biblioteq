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
