/*
** -- Qt Includes --
*/

#include <QKeyEvent>

/*
** -- Local Includes --
*/

#include "biblioteq_pdfreader.h"

/*
** -- biblioteq_pdfreader() --
*/

biblioteq_pdfreader::biblioteq_pdfreader(void):QMainWindow()
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
** -- ~biblioteq_pdfreader() --
*/

biblioteq_pdfreader::~biblioteq_pdfreader()
{
}

/*
** -- changeEvent() --
*/

void biblioteq_pdfreader::changeEvent(QEvent *event)
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

void biblioteq_pdfreader::closeEvent(QCloseEvent *event)
{
  QMainWindow::closeEvent(event);
  deleteLater();
}

/*
** -- keyPressEvent() --
*/

void biblioteq_pdfreader::keyPressEvent(QKeyEvent *event)
{
  if(event && event->key() == Qt::Key_Escape)
    close();

  QMainWindow::keyPressEvent(event);
}

/*
** -- setGlobalFonts() --
*/

void biblioteq_pdfreader::setGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(QWidget *widget, findChildren<QWidget *> ())
    widget->setFont(font);
}

/*
** -- showNormal() --
*/

void biblioteq_pdfreader::showNormal(void)
{
  QMainWindow::showNormal();
}

/*
** -- slotClose() --
*/

void biblioteq_pdfreader::slotClose(void)
{
  close();
}
