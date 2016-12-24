/*
** -- Qt Includes --
*/

#include <QKeyEvent>

/*
** -- Local Includes --
*/

#include "biblioteq_misc_functions.h"
#include "biblioteq_pdfreader.h"

/*
** -- biblioteq_pdfreader() --
*/

biblioteq_pdfreader::biblioteq_pdfreader(QWidget *parent):QMainWindow(parent)
{
  m_ui.setupUi(this);
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
  m_document = 0;
#endif
#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  setAttribute(Qt::WA_MacMetalStyle, BIBLIOTEQ_WA_MACMETALSTYLE);
#endif
#endif
  connect(m_ui.close,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotClose(void)));
  connect(m_ui.page,
	  SIGNAL(valueChanged(int)),
	  this,
	  SLOT(slotShowPage(int)));
  biblioteq_misc_functions::center(this, qobject_cast<QMainWindow *> (parent));
}

/*
** -- ~biblioteq_pdfreader() --
*/

biblioteq_pdfreader::~biblioteq_pdfreader()
{
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
  delete m_document;
#endif
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
** -- load() --
*/

void biblioteq_pdfreader::load(const QByteArray &data)
{
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
  delete m_document;
  m_document = Poppler::Document::loadFromData(data);

  if(!m_document)
    return;

  m_document->setRenderHint(Poppler::Document::Antialiasing, true);
  m_document->setRenderHint(Poppler::Document::TextAntialiasing, true);
  m_ui.page->setMaximum(m_document->numPages());
  m_ui.page->setToolTip(tr("Page 1 of %1.").arg(m_ui.page->maximum()));
  slotShowPage(0);
#else
  Q_UNUSED(data);
#endif
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

/*
** -- slotShowPage() --
*/

void biblioteq_pdfreader::slotShowPage(int index)
{
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
  if(!m_document)
    return;

  Poppler::Page *page = m_document->page(index);

  if(!page)
    return;

  m_ui.page->setToolTip(tr("Page %1 of %2.").
			arg(index + 1).
			arg(m_ui.page->maximum()));

  QImage image = page->renderToImage(100, 100);

  m_ui.label->setPixmap(QPixmap::fromImage(image));
  delete page;
#else
  Q_UNUSED(index);
#endif
}
