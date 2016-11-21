/*
** -- Qt Includes --
*/

/*
** -- Local Includes --
*/

#include "biblioteq_magazine.h"
#include "biblioteq_options.h"

/*
** -- biblioteq_options() --
*/

biblioteq_options::biblioteq_options(void):QMainWindow()
{
  m_ui.setupUi(this);
#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  setAttribute(Qt::WA_MacMetalStyle, BIBLIOTEQ_WA_MACMETALSTYLE);
#endif
#endif
}

/*
** -- ~biblioteq_options() --
*/

biblioteq_options::~biblioteq_options()
{
}

/*
** -- changeEvent() --
*/

void biblioteq_options::changeEvent(QEvent *event)
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

/*
** -- closeEvent() --
*/

void biblioteq_options::closeEvent(QCloseEvent *event)
{
  QMainWindow::closeEvent(event);
}

/*
** -- keyPressEvent() --
*/

void biblioteq_options::keyPressEvent(QKeyEvent *event)
{
  if(event && event->key() == Qt::Key_Escape)
    close();

  QMainWindow::keyPressEvent(event);
}

/*
** -- setGlobalFonts() --
*/

void biblioteq_options::setGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(QWidget *widget, findChildren<QWidget *> ())
    widget->setFont(font);
}

/*
** -- slotClose() --
*/

void biblioteq_options::slotClose(void)
{
  close();
}
