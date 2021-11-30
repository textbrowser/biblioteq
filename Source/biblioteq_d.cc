#include <QDir>

#include "biblioteq.h"
#include "biblioteq_documentationwindow.h"
#include "biblioteq_files.h"
#include "biblioteq_otheroptions.h"
#include "biblioteq_sqlite_merge_databases.h"

bool biblioteq::showMainTableImages(void) const
{
  return m_otheroptions->showMainTableImages();
}

void biblioteq::slotMergeSQLiteDatabases(void)
{
  if(!m_sqliteMergeDatabases)
    m_sqliteMergeDatabases = new biblioteq_sqlite_merge_databases(this);

  biblioteq_misc_functions::center(m_sqliteMergeDatabases, this);
  m_sqliteMergeDatabases->showNormal();
  m_sqliteMergeDatabases->activateWindow();
  m_sqliteMergeDatabases->raise();
}

void biblioteq::slotShowDocumentation(void)
{
  auto action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  repaint();
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!m_documentation.value(action))
    {
      m_documentation[action] = new biblioteq_documentationwindow(this);

      QFile file;

      if(action == ui.action_English)
	{
#ifdef Q_OS_ANDROID
	  file.setFileName("assets:/BiblioteQ.html");
#else
	  file.setFileName
	    (QString("Documentation%1BiblioteQ.html").arg(QDir::separator()));
#endif
	}
      else
	{
#ifdef Q_OS_ANDROID
	  file.setFileName("assets:/BiblioteQ-Fr.html");
#else
	  file.setFileName
	    (QString("Documentation%1Contributed%1BiblioteQ-Fr.html").
	     arg(QDir::separator()));
#endif
	}

      if(file.open(QIODevice::ReadOnly))
	m_documentation.value(action)->setHtml(file.readAll());
    }

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

void biblioteq::slotShowReleaseNotes(void)
{
  repaint();
  QApplication::setOverrideCursor(Qt::WaitCursor);

  if(!m_releaseNotes)
    {
      m_releaseNotes = new biblioteq_documentationwindow
	(QUrl("qrc:/Release-Notes.html"), this);
      m_releaseNotes->setWindowTitle(tr("BiblioteQ: Release Notes"));
    }

  m_releaseNotes->show();
  QApplication::restoreOverrideCursor();
}
