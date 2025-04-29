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

#include <QSettings>

#include "biblioteq.h"
#include "biblioteq_statistics.h"

biblioteq_statistics::biblioteq_statistics(biblioteq *parent):
  QMainWindow(parent)
{
  m_qmain = parent;
  m_ui.setupUi(this);
  prepareIcons();
}

biblioteq_statistics::~biblioteq_statistics()
{
#ifndef Q_OS_ANDROID
  isVisible() ?
    QSettings().setValue("statistics_geometry", saveGeometry()) : (void) 0;
#endif
}

void biblioteq_statistics::changeEvent(QEvent *event)
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

void biblioteq_statistics::closeEvent(QCloseEvent *event)
{
#ifndef Q_OS_ANDROID
  isVisible() ?
    QSettings().setValue("statistics_geometry", saveGeometry()) : (void) 0;
#endif
  QMainWindow::closeEvent(event);
}

void biblioteq_statistics::prepareIcons(void)
{
  QSettings setting;
  auto const index = setting.value
    ("otheroptions/display_icon_set_index", 0).toInt();

  if(index == 1)
    {
      // System.

      m_ui.close->setIcon
	(QIcon::fromTheme("window-close", QIcon(":/32x32/cancel.png")));
      m_ui.delete_query->setIcon
	(QIcon::fromTheme("edit-delete", QIcon(":/16x16/eraser.png")));
      m_ui.export_results->setIcon
	(QIcon::fromTheme("document-save-as", QIcon(":/32x32/save.png")));
      m_ui.go->setIcon(QIcon::fromTheme("dialog-ok", QIcon(":/32x32/ok.png")));
      m_ui.reset->setIcon
	(QIcon::fromTheme("view-refresh", QIcon(":/32x32/reload.png")));
      m_ui.save_query->setIcon
	(QIcon::fromTheme("dialog-ok", QIcon(":/16x16/eraser.png")));
    }
  else
    {
      // Faenza.

      m_ui.close->setIcon(QIcon(":/32x32/cancel.png"));
      m_ui.delete_query->setIcon(QIcon(":/16x16/eraser.png"));
      m_ui.export_results->setIcon(QIcon(":/32x32/save.png"));
      m_ui.go->setIcon(QIcon(":/32x32/ok.png"));
      m_ui.reset->setIcon(QIcon(":/32x32/reload.png"));
      m_ui.save_query->setIcon(QIcon(":/16x16/ok.png"));
    }
}

void biblioteq_statistics::reset(void)
{
  slotReset();
}

void biblioteq_statistics::show(QMainWindow *parent, const bool center)
{
  Q_UNUSED(center);
#ifdef Q_OS_ANDROID
  showMaximized();
#else
  auto static resized = false;

  if(!resized && parent)
    resize(qRound(0.95 * parent->size().width()),
	   qRound(0.95 * parent->size().height()));

  resized = true;
  biblioteq_misc_functions::center(this, parent);
  showNormal();
#endif
  activateWindow();
  raise();
}

void biblioteq_statistics::slotClose(void)
{
#ifdef Q_OS_ANDROID
  hide();
#else
  close();
#endif
}

void biblioteq_statistics::slotReset(void)
{
}

void biblioteq_statistics::slotSetGlobalFonts(const QFont &font)
{
  setFont(font);

  foreach(auto widget, findChildren<QWidget *> ())
    {
      widget->setFont(font);
      widget->update();
    }

  update();
}
