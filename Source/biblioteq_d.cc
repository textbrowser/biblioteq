#include "biblioteq.h"
#include "biblioteq_documentationwindow.h"
#include "biblioteq_files.h"

void biblioteq::slotShowDocumentation(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  QUrl url;

  if(action == ui.action_English)
    url = QUrl("qrc:/BiblioteQ.html");

  if(!m_documentation.value(action))
    m_documentation[action] = new biblioteq_documentationwindow(url, this);

  m_documentation.value(action)->show();
  QApplication::restoreOverrideCursor();
}

void biblioteq::slotShowFiles(void)
{
  if(!m_files)
    m_files = new biblioteq_files(this);

#ifdef Q_OS_ANDROID
  m_files->showMaximized();
#else
  biblioteq_misc_functions::center(m_files, this);
  m_files->showNormal();
#endif
  m_files->activateWindow();
  m_files->raise();
}
