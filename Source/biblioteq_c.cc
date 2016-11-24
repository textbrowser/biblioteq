/*
** -- Local Includes --
*/

#include "biblioteq.h"

/*
** -- slotShowOtherOptions() --
*/

void biblioteq::slotShowOtherOptions(void)
{
  biblioteq_misc_functions::center(m_otheroptions_diag, this);
  m_otheroptions_diag->showNormal();
  m_otheroptions_diag->activateWindow();
  m_otheroptions_diag->raise();
}
