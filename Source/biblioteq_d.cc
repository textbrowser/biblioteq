#include <QDir>

#include "biblioteq.h"
#include "biblioteq_documentationwindow.h"
#include "biblioteq_files.h"

void biblioteq::slotShowDocumentation(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  repaint();
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!m_documentation.value(action))
    m_documentation[action] = new biblioteq_documentationwindow(this);

  QFile file;

  if(action == ui.action_English)
    file.setFileName
      (QString("Documentation%1BiblioteQ.html").arg(QDir::separator()));
  else
    file.setFileName
      (QString("Documentation%1Contributed%1BiblioteQ-Fr.html").
       arg(QDir::separator()));

  if(file.open(QIODevice::ReadOnly))
    m_documentation.value(action)->setHtml(file.readAll());

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
