#include "biblioteq.h"
#include "biblioteq_sqlite_merge_databases.h"

biblioteq_sqlite_merge_databases::biblioteq_sqlite_merge_databases
(biblioteq *parent):QMainWindow(parent)
{
  m_qmain = parent;
  m_ui.setupUi(this);
  connect(m_qmain,
	  SIGNAL(fontChanged(const QFont &)),
	  this,
	  SLOT(setGlobalFonts(const QFont &)));
}

void biblioteq_sqlite_merge_databases::changeEvent(QEvent *event)
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
	{
	  break;
	}
      }

  QMainWindow::changeEvent(event);
}

void biblioteq_sqlite_merge_databases::setGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(auto widget, findChildren<QWidget *> ())
    {
      widget->setFont(font);
      widget->update();
    }

  update();
}
