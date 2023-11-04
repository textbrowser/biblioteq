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

#include "biblioteq.h"
#include "biblioteq_magic.h"

// Incomplete!

biblioteq_magic::biblioteq_magic(biblioteq *parent):QMainWindow(parent)
{
  m_qmain = parent;
  m_ui.setupUi(this);
  m_ui.cancel_download_images->setEnabled(false);
  connect(m_qmain,
	  SIGNAL(fontChanged(const QFont &)),
	  this,
	  SLOT(slotSetGlobalFonts(const QFont &)));
  connect(m_ui.close,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotClose(void)));
  connect(m_ui.download_images,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotDownloadImages(void)));
  setWindowModality(Qt::ApplicationModal); // Focus on your work.
}

void biblioteq_magic::changeEvent(QEvent *event)
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

void biblioteq_magic::reset(void)
{
  slotReset();
}

void biblioteq_magic::show(QMainWindow *parent, const bool center)
{
  static auto resized = false;

  if(!resized && parent)
    resize(qRound(0.70 * parent->size().width()),
	   qRound(0.80 * parent->size().height()));

  resized = true;

  if(center)
    biblioteq_misc_functions::center(this, parent);

  showNormal();
  activateWindow();
  raise();
}

void biblioteq_magic::slotClose(void)
{
#ifdef Q_OS_ANDROID
  hide();
#else
  close();
#endif
}

void biblioteq_magic::slotDownloadImageFinished(void)
{
  m_ui.cancel_download_images->setEnabled(false);
  m_ui.download_images->setEnabled(true);
}

void biblioteq_magic::slotDownloadImages(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);

  auto list(m_qmain->selectedISBN10s());

  if(list.isEmpty())
    {
      QApplication::restoreOverrideCursor();
      return;
    }

  m_ui.cancel_download_images->setEnabled(true);
  m_ui.download_images->setEnabled(false);
  m_ui.download_images_progress->setValue(0);
  QApplication::restoreOverrideCursor();
}

void biblioteq_magic::slotReset(void)
{
  if(m_ui.download_images_progress->value() != 0)
    return;

  m_ui.image_site->setCurrentIndex(0);
}

void biblioteq_magic::slotSetGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(auto widget, findChildren<QWidget *> ())
    {
      widget->setFont(font);
      widget->update();
    }

  update();
}
