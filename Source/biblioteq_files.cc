#include "biblioteq_files.h"

biblioteq_files::biblioteq_files(QWidget *parent):QMainWindow(parent)
{
  m_ui.setupUi(this);
  connect(m_ui.close,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotClose(void)));
  connect(m_ui.refresh,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRefresh(void)));
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

void biblioteq_files::slotRefresh(void)
{
}
