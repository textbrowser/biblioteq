/*
** -- Qt Includes --
*/

#include <QFileDialog>
#include <QKeyEvent>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QProgressDialog>
#include <QResizeEvent>
#include <QScrollBar>

/*
** -- Local Includes --
*/

#include "biblioteq_misc_functions.h"
#include "biblioteq_pdfreader.h"

biblioteq_pdfreader::biblioteq_pdfreader(QWidget *parent):QMainWindow(parent)
{
  m_ui.setupUi(this);

  if(menuBar())
    menuBar()->setNativeMenuBar(true);

#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
  m_document = 0;
#else
  m_ui.action_Contents->setEnabled(false);
  m_ui.action_Print->setEnabled(false);
  m_ui.action_Save_As->setEnabled(false);
  m_ui.case_sensitive->setEnabled(false);
  m_ui.find->setEnabled(false);
  m_ui.find_next->setEnabled(false);
  m_ui.find_previous->setEnabled(false);
  m_ui.page->setEnabled(false);
  m_ui.page_1->setText(tr("BiblioteQ was assembled without Poppler support."));
  m_ui.page_2->setText(m_ui.page_1->text());
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
  connect(m_ui.action_Contents,
	  SIGNAL(triggered(bool)),
	  this,
	  SLOT(slotShowContents(bool)));
  connect(m_ui.action_Find,
	  SIGNAL(triggered(void)),
	  m_ui.find,
	  SLOT(selectAll(void)));
  connect(m_ui.action_Find,
	  SIGNAL(triggered(void)),
	  m_ui.find,
	  SLOT(setFocus(void)));
  connect(m_ui.action_Print,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotPrint(void)));
  connect(m_ui.action_Save_As,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotSaveAs(void)));
  connect(m_ui.contents,
	  SIGNAL(itemDoubleClicked(QListWidgetItem *)),
	  this,
	  SLOT(slotContentsDoubleClicked(QListWidgetItem *)));
  connect(m_ui.find,
	  SIGNAL(returnPressed(void)),
	  this,
	  SLOT(slotSearchNext(void)));
  connect(m_ui.find_next,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSearchNext(void)));
  connect(m_ui.find_previous,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSearchPrevious(void)));
  connect(m_ui.page,
	  SIGNAL(valueChanged(int)),
	  this,
	  SLOT(slotShowPage(int)));
  connect(m_ui.scrollArea->verticalScrollBar(),
	  SIGNAL(actionTriggered(int)),
	  this,
	  SLOT(slotSliderTriggerAction(int)));
  connect(m_ui.view_size,
	  SIGNAL(currentIndexChanged(int)),
	  this,
	  SLOT(slotChangePageViewSize(int)));
  biblioteq_misc_functions::center(this, qobject_cast<QMainWindow *> (parent));
  m_ui.contents->setVisible(false);
}

biblioteq_pdfreader::~biblioteq_pdfreader()
{
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
  delete m_document;
#endif
}

bool biblioteq_pdfreader::event(QEvent *event)
{
  if(event && event->type() == QEvent::KeyRelease)
    {
      QKeyEvent *keyEvent = static_cast<QKeyEvent *> (event);

      if(keyEvent)
	switch(keyEvent->key())
	  {
	  case Qt::Key_Down:
	  case Qt::Key_PageDown:
	    {
	      if(!m_ui.scrollArea->verticalScrollBar()->isVisible() ||
		 (m_ui.scrollArea->verticalScrollBar()->isVisible() &&
		  m_ui.scrollArea->verticalScrollBar()->maximum() ==
		  m_ui.scrollArea->verticalScrollBar()->value()))
		m_ui.page->setValue(m_ui.page->value() + 2);

	      break;
	    }
	  case Qt::Key_PageUp:
	  case Qt::Key_Up:
	    {
	      if(!m_ui.scrollArea->verticalScrollBar()->isVisible() ||
		 (m_ui.scrollArea->verticalScrollBar()->isVisible() &&
		  m_ui.scrollArea->verticalScrollBar()->minimum() ==
		  m_ui.scrollArea->verticalScrollBar()->value()))
		m_ui.page->setValue(m_ui.page->value() - 2);

	      break;
	    }
	  }
    }

  return QMainWindow::event(event);
}

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

void biblioteq_pdfreader::closeEvent(QCloseEvent *event)
{
  QMainWindow::closeEvent(event);
  deleteLater();
}

void biblioteq_pdfreader::keyPressEvent(QKeyEvent *event)
{
  if(event)
    {
      if(event->key() == Qt::Key_End)
	m_ui.page->setValue(m_ui.page->maximum());
      else if(event->key() == Qt::Key_Home)
	m_ui.page->setValue(m_ui.page->minimum());
    }

  QMainWindow::keyPressEvent(event);
}

void biblioteq_pdfreader::load(const QByteArray &data, const QString &fileName)
{
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
  delete m_document;
  m_document = Poppler::Document::loadFromData(data);

  if(!m_document)
    {
      m_ui.action_Print->setEnabled(false);
      m_ui.action_Save_As->setEnabled(false);
      m_ui.page->setMaximum(1);
      m_ui.page_1->setText(tr("The PDF data could not be processed."));
      m_ui.page_2->setText(m_ui.page_1->text());
      return;
    }

  prepareContents();
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

void biblioteq_pdfreader::load(const QString &fileName)
{
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
  delete m_document;
  m_document = Poppler::Document::load(fileName);

  if(!m_document)
    {
      m_ui.action_Print->setEnabled(false);
      m_ui.action_Save_As->setEnabled(false);
      m_ui.page->setMaximum(1);
      m_ui.page_1->setText(tr("The PDF data could not be processed."));
      m_ui.page_2->setText(m_ui.page_1->text());
      return;
    }

  prepareContents();
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
  Q_UNUSED(fileName);
#endif
}

void biblioteq_pdfreader::prepareContents(void)
{
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
  if(!m_document)
    return;

  m_ui.contents->clear();

  for(int i = 1; i <= m_document->numPages(); i++)
    {
      QListWidgetItem *item = new QListWidgetItem(tr("Page %1").arg(i));

      item->setData(Qt::UserRole, i);
      m_ui.contents->addItem(item);
    }
#endif
}

void biblioteq_pdfreader::resizeEvent(QResizeEvent *event)
{
  if(m_ui.view_size->currentIndex() != 4)
    slotShowPage(m_ui.page->value());

  QMainWindow::resizeEvent(event);
}

void biblioteq_pdfreader::setGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(QWidget *widget, findChildren<QWidget *> ())
    {
      widget->setFont(font);
      widget->update();
    }

  update();
}

void biblioteq_pdfreader::showNormal(void)
{
  QMainWindow::showNormal();
}

void biblioteq_pdfreader::slotChangePageViewSize(int value)
{
  Q_UNUSED(value);
  slotShowPage(m_ui.page->value());
}

void biblioteq_pdfreader::slotClose(void)
{
  close();
}

void biblioteq_pdfreader::slotContentsDoubleClicked(QListWidgetItem *item)
{
  if(item)
    m_ui.page->setValue(item->data(Qt::UserRole).toInt());
}

void biblioteq_pdfreader::slotPrint(void)
{
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
  if(!m_document)
    return;

  QPrinter printer;
  QPrintDialog dialog(&printer, this);

#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
  dialog.setAttribute(Qt::WA_MacMetalStyle, BIBLIOTEQ_WA_MACMETALSTYLE);
#endif
#endif
  dialog.setMinMax(1, m_document->numPages());
  printer.setColorMode(QPrinter::Color);
  printer.setDuplex(QPrinter::DuplexAuto);
  printer.setFromTo(1, m_document->numPages());
  printer.setPageSize(QPrinter::Letter);

  if(dialog.exec() == QDialog::Accepted)
    {
      QProgressDialog progress(this);

#ifdef Q_OS_MAC
#if QT_VERSION < 0x050000
      progress.setAttribute(Qt::WA_MacMetalStyle, BIBLIOTEQ_WA_MACMETALSTYLE);
#endif
#endif
      progress.setLabelText(tr("Printing PDF..."));
      progress.setMaximum(0);
      progress.setMinimum(0);
      progress.setModal(true);
      progress.setWindowTitle(QObject::tr("BiblioteQ: Progress Dialog"));
      progress.show();
#ifndef Q_OS_MAC
      progress.repaint();
      QApplication::processEvents();
#endif

      QPainter painter(&printer);
      int end = printer.toPage();
      int start = printer.fromPage();

      if(end == 0 && start == 0)
	{
	  end = m_document->numPages();
	  start = 1;
	}

      for(int i = start; i <= end; i++)
	{
#ifndef Q_OS_MAC
	  progress.setLabelText(tr("Printing PDF... Page %1...").arg(i));
	  progress.repaint();
	  QApplication::processEvents();
#endif

	  Poppler::Page *page = m_document->page(i - 1);

	  if(!page)
	    break;

	  QImage image
	    (page->renderToImage(printer.resolution(), printer.resolution()));
	  QRect rect(painter.viewport());
	  QSize size(image.size());

	  size.scale(rect.size(), Qt::KeepAspectRatio);
	  painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
	  painter.setWindow(image.rect());
	  painter.drawImage(QPoint(0, 0), image);
	  delete page;

	  if(i == end)
	    break;

	  if(progress.wasCanceled())
	    {
	      printer.abort();
	      break;
	    }

	  printer.newPage();
	}

      painter.end();
    }
#endif
}

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

void biblioteq_pdfreader::slotSearchNext(void)
{
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
  if(!m_document || m_ui.find->text().isEmpty())
    {
      m_searchLocation = QRectF();
      slotShowPage(m_ui.page->value(), m_searchLocation);
      return;
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  int page = m_ui.page->value() - 1;

  while(page < m_document->numPages())
    {
#if QT_VERSION >= 0x050000
      double bottom = m_searchLocation.bottom();
      double left = m_searchLocation.left();
      double right = m_searchLocation.right();
      double top = m_searchLocation.top();

      if(m_document->page(page)->
	 search(m_ui.find->text(),
		left,
		top,
		right,
		bottom,
		Poppler::Page::NextResult,
		m_ui.case_sensitive->isChecked() ?
		Poppler::Page::SearchFlag(0) : Poppler::Page::IgnoreCase))
#else
      if(m_document->page(page)->
	 search(m_ui.find->text(),
		m_searchLocation,
		Poppler::Page::NextResult,
		m_ui.case_sensitive->isChecked() ?
		Poppler::Page::CaseSensitive : Poppler::Page::CaseInsensitive))
#endif
	{
#if QT_VERSION >= 0x050000
	  m_searchLocation = QRectF(left, top, right - left, bottom - top);
#endif

	  if(!m_searchLocation.isNull())
	    {
	      QApplication::restoreOverrideCursor();
	      slotShowPage(page + 1, m_searchLocation);
#if QT_VERSION >= 0x050000
	      m_searchLocation.setX(right);
#else
	      m_searchLocation.setX(m_searchLocation.right());
#endif
	      m_ui.find->setFocus();
	      m_ui.page->blockSignals(true);
	      m_ui.page->setValue(page + 1);
	      m_ui.page->blockSignals(false);
	      return;
	    }
	}

      m_searchLocation = QRectF();
      page += 1;
    }

  m_ui.page->setValue(1);
  m_ui.find->setFocus();
  QApplication::restoreOverrideCursor();
#endif
}

void biblioteq_pdfreader::slotSearchPrevious(void)
{
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
  if(!m_document || m_ui.find->text().isEmpty())
    {
      m_searchLocation = QRectF();
      slotShowPage(m_ui.page->value(), m_searchLocation);
      return;
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  int page = m_ui.page->value() - 1;

  while(page >= 0)
    {
#if QT_VERSION >= 0x050000
      double bottom = m_searchLocation.bottom();
      double left = m_searchLocation.left();
      double right = m_searchLocation.right();
      double top = m_searchLocation.top();

      if(m_document->page(page)->
	 search(m_ui.find->text(),
		left,
		top,
		right,
		bottom,
		Poppler::Page::PreviousResult,
		m_ui.case_sensitive->isChecked() ?
		Poppler::Page::SearchFlag(0) : Poppler::Page::IgnoreCase))
#else
      if(m_document->page(page)->
	 search(m_ui.find->text(),
		m_searchLocation,
		Poppler::Page::PreviousResult,
		m_ui.case_sensitive->isChecked() ?
		Poppler::Page::CaseSensitive : Poppler::Page::CaseInsensitive))
#endif
	{
#if QT_VERSION >= 0x050000
	  m_searchLocation = QRectF(left, top, right - left, bottom - top);
#endif
	  if(!m_searchLocation.isNull())
	    {
	      QApplication::restoreOverrideCursor();
	      slotShowPage(page + 1, m_searchLocation);
#if QT_VERSION >= 0x050000
	      m_searchLocation.setX(m_searchLocation.left() - 1);
#else
	      m_searchLocation.setX(m_searchLocation.right() -
				    m_searchLocation.left());
#endif
	      m_ui.find->setFocus();
	      m_ui.page->blockSignals(true);
	      m_ui.page->setValue(page + 1);
	      m_ui.page->blockSignals(false);
	      return;
	    }
	}

      m_searchLocation = QRectF
	(m_document->page(page)->pageSizeF().width(),
	 m_document->page(page)->pageSizeF().height(),
	 1,
	 1);
      page -= 1;
    }

  m_ui.page->setValue(1);
  m_ui.find->setFocus();
  QApplication::restoreOverrideCursor();
#endif
}

void biblioteq_pdfreader::slotShowContents(bool state)
{
  m_ui.contents->setVisible(state);
}

void biblioteq_pdfreader::slotShowPage(int value, const QRectF &location)
{
#ifdef BIBLIOTEQ_LINKED_WITH_POPPLER
  if(!m_document)
    {
      m_searchLocation = QRectF();
      return;
    }
  else if(value <= 0 || value > m_document->numPages())
    {
      m_searchLocation = QRectF();
      return;
    }

  QApplication::setOverrideCursor(Qt::WaitCursor);

  Poppler::Page *page = m_document->page(value - 1);

  if(!page)
    {
      m_searchLocation = QRectF();
      m_ui.page_1->setText(tr("The PDF data could not be processed."));
      m_ui.page_2->setText(m_ui.page_1->text());
      QApplication::restoreOverrideCursor();
      return;
    }

  m_ui.page->setToolTip(tr("Page %1 of %2.").
			arg(value).
			arg(m_ui.page->maximum()));

  QImage image;
  double scaleFactor =
    m_ui.view_size->currentText().remove("%").toInt() / 100.0;
  int pX = qMax(72, m_ui.page_1->physicalDpiX());
  int pY = qMax(72, m_ui.page_1->physicalDpiY());

  if(m_ui.view_size->currentIndex() == 4)
    image = page->renderToImage(pX, pY);
  else
    image = page->renderToImage
      (scaleFactor * physicalDpiX(), scaleFactor * physicalDpiY());

  if(!location.isNull())
    {
      /*
      ** Highlight the discovered text.
      */

      QMatrix matrix(m_ui.view_size->currentIndex() != 4 ?
		     scaleFactor * physicalDpiX() / 72.0 :
		     physicalDpiX() / 72.0,
		     0,
		     0,
		     m_ui.view_size->currentIndex() != 4 ?
		     scaleFactor * physicalDpiY() / 72.0 :
		     physicalDpiY() / 72.0,
		     0,
		     0);
      QRect highlightRect(matrix.mapRect(location).toRect());

      highlightRect.adjust(-2, -2, 2, 2);

      QImage imageHighlight(image.copy(highlightRect));
      QPainter painter;

      painter.begin(&image);
      painter.fillRect(image.rect(), QColor(0, 0, 0, 32));
      painter.drawImage(highlightRect, imageHighlight);
      painter.end();
    }

  m_ui.page_1->setPixmap(QPixmap::fromImage(image));
  m_ui.page_1->setFocus();
  delete page;

  page = m_document->page(value);

  if(!page)
    m_ui.page_2->setText(tr("The PDF data could not be processed."));
  else
    {
      if(m_ui.view_size->currentIndex() == 4)
	image = page->renderToImage(pX, pY);
      else
	image = page->renderToImage
	  (scaleFactor * physicalDpiX(), scaleFactor * physicalDpiY());

      m_ui.page_2->setPixmap(QPixmap::fromImage(image));
    }

  delete page;
  QApplication::restoreOverrideCursor();
#else
  Q_UNUSED(location);
  Q_UNUSED(value);
#endif
}

void biblioteq_pdfreader::slotSliderTriggerAction(int action)
{
  if(action == QAbstractSlider::SliderSingleStepSub &&
     m_ui.scrollArea->verticalScrollBar()->minimum() ==
     m_ui.scrollArea->verticalScrollBar()->value())
    {
      m_ui.page->setValue(m_ui.page->value() - 2);
      m_ui.scrollArea->verticalScrollBar()->setValue
	(m_ui.scrollArea->verticalScrollBar()->minimum());
    }
  else if(action == QAbstractSlider::SliderSingleStepAdd &&
	  m_ui.scrollArea->verticalScrollBar()->maximum() ==
	  m_ui.scrollArea->verticalScrollBar()->value())
    {
      m_ui.page->setValue(m_ui.page->value() + 2);
      m_ui.scrollArea->verticalScrollBar()->setValue
	(m_ui.scrollArea->verticalScrollBar()->maximum());
    }
}
