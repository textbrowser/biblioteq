#include "biblioteq_files.h"

biblioteq_files::biblioteq_files(void):QMainWindow()
{
  m_ui.setupUi(this);
}

biblioteq_files::~biblioteq_files()
{
}

void biblioteq_files::changeEvent(QEvent *event)
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

void biblioteq_files::slotClose(void)
{
  close();
}
