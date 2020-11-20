#include "biblioteq.h"
#include "biblioteq_files.h"

void biblioteq::slotShowFiles(void)
{
  if(!m_files)
    m_files = new biblioteq_files(this);

  biblioteq_misc_functions::center(m_files, this);
  m_files->showNormal();
  m_files->activateWindow();
  m_files->raise();
}
