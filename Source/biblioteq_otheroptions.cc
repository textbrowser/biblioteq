/*
** -- Qt Includes --
*/

/*
** -- Local Includes --
*/

#include "biblioteq_magazine.h"
#include "biblioteq_otheroptions.h"

/*
** -- biblioteq_otheroptions() --
*/

biblioteq_otheroptions::biblioteq_otheroptions(void):QMainWindow()
{
  m_ui.setupUi(this);
#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  setAttribute(Qt::WA_MacMetalStyle, BIBLIOTEQ_WA_MACMETALSTYLE);
#endif
#endif
  connect(m_ui.close,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotClose(void)));
}

/*
** -- ~biblioteq_otheroptions() --
*/

biblioteq_otheroptions::~biblioteq_otheroptions()
{
}

/*
** -- changeEvent() --
*/

void biblioteq_otheroptions::changeEvent(QEvent *event)
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

void biblioteq_otheroptions::closeEvent(QCloseEvent *event)
{
  QMainWindow::closeEvent(event);
}

/*
** -- keyPressEvent() --
*/

void biblioteq_otheroptions::keyPressEvent(QKeyEvent *event)
{
  if(event && event->key() == Qt::Key_Escape)
    close();

  QMainWindow::keyPressEvent(event);
}

/*
** -- setGlobalFonts() --
*/

void biblioteq_otheroptions::setGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(QWidget *widget, findChildren<QWidget *> ())
    widget->setFont(font);
}

/*
** -- slotClose() --
*/

void biblioteq_otheroptions::slotClose(void)
{
  close();
}
