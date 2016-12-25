/*
** -- Qt Includes --
*/

#include <QFileDialog>
#include <QKeyEvent>
#include <QScrollBar>

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
#else
  m_ui.action_Save_As->setEnabled(false);
  m_ui.label->setText(tr("BiblioteQ was assembled without Poppler support."));
  m_ui.page->setEnabled(false);
#endif
#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  setAttribute(Qt::WA_MacMetalStyle, BIBLIOTEQ_WA_MACMETALSTYLE);
#endif
#endif
  connect(m_ui.action_Close,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotClose(void)));
  connect(m_ui.action_Save_As,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotSaveAs(void)));
  connect(m_ui.page,
	  SIGNAL(valueChanged(int)),
	  this,
	  SLOT(slotShowPage(int)));
  connect(m_ui.scrollArea->verticalScrollBar(),
	  SIGNAL(valueChanged(int)),
	  this,
	  SLOT(slotSliderValueChanged(int)));
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

void biblioteq_pdfreader::load(const QByteArray &data, const QString &fileName)
{
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
  delete m_document;
  m_document = Poppler::Document::loadFromData(data);

  if(!m_document)
    {
      m_ui.action_Save_As->setEnabled(false);
      m_ui.label->setText(tr("The PDF data could not be processed."));
      m_ui.page->setMaximum(1);
      return;
    }

  m_document->setRenderHint(Poppler::Document::Antialiasing, true);
  m_document->setRenderHint(Poppler::Document::TextAntialiasing, true);
  m_fileName = fileName.trimmed();
  m_ui.page->setMaximum(m_document->numPages());
  m_ui.page->setToolTip(tr("Page 1 of %1.").arg(m_ui.page->maximum()));

  if(fileName.trimmed().isEmpty())
    setWindowTitle(tr("BiblioteQ: PDF Reader"));
  else
    setWindowTitle(tr("BiblioteQ: PDF Reader (%1)").arg(fileName.trimmed()));

  slotShowPage(1);
#else
  Q_UNUSED(data);
  Q_UNUSED(fileName);
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
** -- slotSaveAs() --
*/

void biblioteq_pdfreader::slotSaveAs(void)
{
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
  if(!m_document)
    return;

  QFileDialog dialog(this);

#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  dialog.setAttribute(Qt::WA_MacMetalStyle, BIBLIOTEQ_WA_MACMETALSTYLE);
#endif
#endif
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setDirectory(QDir::homePath());
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setWindowTitle(tr("BiblioteQ: Save PDF As"));
  dialog.selectFile(m_fileName);

  if(dialog.exec() == QDialog::Accepted)
    {
      dialog.close();
#ifndef Q_OS_MAC
      repaint();
      QApplication::processEvents();
#endif
      QApplication::setOverrideCursor(Qt::WaitCursor);

      Poppler::PDFConverter *converter = m_document->pdfConverter();

      if(!converter)
	{
	  QApplication::restoreOverrideCursor();
	  return;
	}

      converter->setOutputFileName(dialog.selectedFiles().value(0));
      converter->convert();
      delete converter;
      QApplication::restoreOverrideCursor();
    }
#endif
}

/*
** -- slotShowPage() --
*/

void biblioteq_pdfreader::slotShowPage(int value)
{
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
  if(!m_document)
    return;
  else if(value <= 0 || value > m_document->numPages())
    return;

  QApplication::setOverrideCursor(Qt::WaitCursor);

  Poppler::Page *page = m_document->page(value - 1);

  if(!page)
    {
      QApplication::restoreOverrideCursor();
      return;
    }

  m_ui.page->setToolTip(tr("Page %1 of %2.").
			arg(value).
			arg(m_ui.page->maximum()));

  QImage image = page->renderToImage(100, 100);

  m_ui.label->setPixmap(QPixmap::fromImage(image));
  m_ui.label->setFocus();
  delete page;
  QApplication::restoreOverrideCursor();
#else
  Q_UNUSED(value);
#endif
}

/*
** -- slotSliderValueChanged() --
*/

void biblioteq_pdfreader::slotSliderValueChanged(int value)
{
  if(value == m_ui.scrollArea->verticalScrollBar()->minimum())
    {
      m_ui.page->setValue(m_ui.page->value() - 1);
      m_ui.scrollArea->verticalScrollBar()->setValue(value + 1);
    }
  else if(value == m_ui.scrollArea->verticalScrollBar()->maximum())
    {
      m_ui.page->setValue(m_ui.page->value() + 1);
      m_ui.scrollArea->verticalScrollBar()->setValue(value - 1);
    }
}
