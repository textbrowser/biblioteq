/*
** Copyright (c) 2006 - present, Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from BiblioteQ without specific prior written permission.
**
** BIBLIOTEQ IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** BIBLIOTEQ, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef BIBLIOTEQ_QT_PDF_SUPPORTED
#include <QBuffer>
#endif
#include <QDesktopServices>
#ifdef BIBLIOTEQ_QT_PDF_SUPPORTED
#include <QPdfDocument>
#include <QPdfView>
#endif
#include <QPrintDialog>
#include <QPrinter>
#include <QSettings>

#include "biblioteq_documentationwindow.h"
#include "biblioteq_misc_functions.h"

biblioteq_documentationwindow::biblioteq_documentationwindow
(QWidget *parent):biblioteq_documentationwindow(QUrl(), parent)
{
}

biblioteq_documentationwindow::biblioteq_documentationwindow
(const QUrl &url, QWidget *parent):QMainWindow(parent)
{
  m_openExternalLinks = false;
  m_ui.setupUi(this);
  m_ui.find->setPlaceholderText("Find");
  m_ui.text->setSource(url);
  m_originalFindPalette = m_ui.find->palette();

  if(menuBar())
    menuBar()->setNativeMenuBar(true);

#ifdef BIBLIOTEQ_QT_PDF_SUPPORTED
  if(url.fileName().toLower().trimmed().endsWith(".pdf"))
    {
      m_pdfDocument = new QPdfDocument(this);
      m_pdfDocument->load(url.path());
      m_pdfView = new QPdfView(this);
      m_pdfView->setDocument(m_pdfDocument);
      m_pdfView->setPageMode(QPdfView::PageMode::MultiPage);
      m_ui.action_Find->setEnabled(false);
      m_ui.action_Print->setEnabled(false);
      m_ui.frame->setVisible(false);
      m_ui.stack->addWidget(m_pdfView);
      m_ui.stack->setCurrentIndex(1);
    }
  else
    {
      m_pdfDocument = nullptr;
      m_pdfView = nullptr;
    }
#endif

  connectSignals();
  prepareIcons();
}

biblioteq_documentationwindow::~biblioteq_documentationwindow()
{
}

void biblioteq_documentationwindow::connectSignals(void)
{
#ifdef Q_OS_ANDROID
  connect(m_ui.action_Close,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(hide(void)),
          Qt::UniqueConnection);
#else
  connect(m_ui.action_Close,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(close(void)),
          Qt::UniqueConnection);
#endif
  connect(m_ui.action_Find,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotFind(void)),
          Qt::UniqueConnection);
  connect(m_ui.action_Print,
          SIGNAL(triggered(void)),
          this,
          SLOT(slotPrint(void)),
          Qt::UniqueConnection);
  connect(m_ui.find,
	  SIGNAL(returnPressed(void)),
	  this,
	  SLOT(slotFindText(void)),
          Qt::UniqueConnection);
  connect(m_ui.find,
	  SIGNAL(textEdited(const QString &)),
	  this,
	  SLOT(slotFindText(void)),
          Qt::UniqueConnection);
  connect(m_ui.next,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotFindText(void)),
          Qt::UniqueConnection);
  connect(m_ui.previous,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotFindText(void)),
          Qt::UniqueConnection);
  connect(m_ui.text,
          SIGNAL(anchorClicked(const QUrl &)),
          this,
          SLOT(slotAnchorClicked(const QUrl &)),
          Qt::UniqueConnection);
}

void biblioteq_documentationwindow::load(const QByteArray &data)
{
#ifdef BIBLIOTEQ_QT_PDF_SUPPORTED
  if(!m_pdfDocument)
    {
      m_pdfDocument = new QPdfDocument(this);
      m_pdfView = new QPdfView(this);
      m_pdfView->setDocument(m_pdfDocument);
      m_pdfView->setPageMode(QPdfView::PageMode::MultiPage);
      m_ui.action_Find->setEnabled(false);
      m_ui.action_Print->setEnabled(false);
      m_ui.frame->setVisible(false);
      m_ui.stack->addWidget(m_pdfView);
      m_ui.stack->setCurrentIndex(1);
    }

  auto buffer = findChild<QBuffer *> ("pdf-buffer");

  if(!buffer)
    {
      buffer = new QBuffer(this);
      buffer->setObjectName("pdf-buffer");
    }
  else
    buffer->close();

  buffer->setData(data);

  if(buffer->open(QIODevice::ReadOnly))
    m_pdfDocument->load(buffer);
#endif
}

void biblioteq_documentationwindow::prepareIcons(void)
{
  QSettings setting;
  auto const index = setting.value
    ("otheroptions/display_icon_set_index", 0).toInt();

  if(index == 1)
    {
      // System.

      m_ui.action_Close->setIcon
	(QIcon::fromTheme("window-close", QIcon(":/16x16/cancel.png")));
      m_ui.action_Find->setIcon
	(QIcon::fromTheme("edit-find", QIcon(":/16x16/find.png")));
      m_ui.action_Print->setIcon
	(QIcon::fromTheme("document-print", QIcon(":/16x16/fileprint.png")));
      m_ui.next->setIcon
	(QIcon::fromTheme("go-next", QIcon(":/16x16/next.png")));
      m_ui.previous->setIcon
	(QIcon::fromTheme("go-previous", QIcon(":/16x16/previous.png")));
    }
  else
    {
      // Faenza.

      m_ui.action_Close->setIcon(QIcon(":/16x16/cancel.png"));
      m_ui.action_Find->setIcon(QIcon(":/16x16/find.png"));
      m_ui.action_Print->setIcon(QIcon(":/16x16/fileprint.png"));
      m_ui.next->setIcon(QIcon(":/16x16/next.png"));
      m_ui.previous->setIcon(QIcon(":/16x16/previous.png"));
    }
}

void biblioteq_documentationwindow::setAllowOpeningOfExternalLinks
(const bool state)
{
  m_openExternalLinks = state;
}

void biblioteq_documentationwindow::setHtml(const QString &html)
{
  m_ui.text->setHtml(html);
}

void biblioteq_documentationwindow::show(void)
{
#ifdef Q_OS_ANDROID
  QMainWindow::showMaximized();
#else
  biblioteq_misc_functions::center
    (this, static_cast<QMainWindow *> (parentWidget()));
  QMainWindow::showNormal();
#endif
  QMainWindow::activateWindow();
  QMainWindow::raise();
}

void biblioteq_documentationwindow::slotAnchorClicked(const QUrl &url)
{
  if(url.scheme().toLower().trimmed() == "qrc")
    m_ui.text->setSource(url);
  else if(m_openExternalLinks)
    QDesktopServices::openUrl(url);
}

void biblioteq_documentationwindow::slotFind(void)
{
  m_ui.find->selectAll();
  m_ui.find->setFocus();
}

void biblioteq_documentationwindow::slotFindText(void)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
  auto options = QTextDocument::FindFlags();
#else
  QTextDocument::FindFlags options = 0;
#endif

  if(m_ui.previous == qobject_cast<QPushButton *> (sender()))
    options = QTextDocument::FindBackward;

  if(m_ui.find->text().isEmpty())
    {
      m_ui.find->setPalette(m_originalFindPalette);
      m_ui.find->setProperty("found", true);
      m_ui.text->moveCursor(QTextCursor::Left);
    }
  else if(!m_ui.text->find(m_ui.find->text(), options))
    {
      auto const found = m_ui.find->property("found").toBool();

      if(found)
	m_ui.find->setProperty("found", false);
      else
	{
	  QColor color(240, 128, 128); // Light Coral
	  auto palette(m_ui.find->palette());

	  palette.setColor(m_ui.find->backgroundRole(), color);
	  m_ui.find->setPalette(palette);
	}

      if(options)
	m_ui.text->moveCursor(QTextCursor::End);
      else
	m_ui.text->moveCursor(QTextCursor::Start);

      if(found)
	slotFindText();
    }
  else
    {
      m_ui.find->setPalette(m_originalFindPalette);
      m_ui.find->setProperty("found", true);
    }
}

void biblioteq_documentationwindow::slotPrint(void)
{
  QPrinter printer;
  QPrintDialog printDialog(&printer, this);

  printDialog.setWindowIcon(windowIcon());

  if(printDialog.exec() == QDialog::Accepted)
    m_ui.text->print(&printer);
}
