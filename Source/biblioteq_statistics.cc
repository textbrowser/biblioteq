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

#include <QInputDialog>
#include <QSettings>

#include "biblioteq.h"
#include "biblioteq_otheroptions.h"
#include "biblioteq_sql_syntax_highlighter.h"
#include "biblioteq_statistics.h"

biblioteq_statistics::biblioteq_statistics
(biblioteq *parent, biblioteq_otheroptions *other):
  QMainWindow(parent)
{
  m_ui.setupUi(this);
  connect(m_ui.close,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotClose(void)));
  connect(m_ui.delete_query,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotDelete(void)));
  connect(m_ui.save_query,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSave(void)));
  connect(other,
	  SIGNAL(saved(void)),
	  this,
	  SLOT(slotOtherOptionsChanged(void)));
  m_otheroptions = other;
  m_qmain = parent;
  m_sqlSyntaxHighlighter = new biblioteq_sql_syntax_highlighter
    (m_ui.query->document());
  m_sqlSyntaxHighlighter->setKeywordsColors
    (m_otheroptions->customQueryColors());
  m_ui.splitter->setStretchFactor(0, 0);
  m_ui.splitter->setStretchFactor(1, 1);
  populateStatistics();
  prepareIcons();
  setAttribute(Qt::WA_DeleteOnClose, true);
}

biblioteq_statistics::~biblioteq_statistics()
{
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

void biblioteq_statistics::populateStatistics(void)
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  m_ui.queries->clear();

  QSettings settings;

  settings.beginGroup("statistics");

  foreach(auto const &str, settings.childKeys())
    {
      auto const bytes(settings.value(str).toString().trimmed());

      if(bytes.isEmpty())
	settings.remove(str);
      else if(m_ui.queries->findText(bytes) == -1)
	m_ui.queries->addItem(str, bytes);
      else
	settings.remove(str);
    }

  if(m_ui.queries->count() == 0)
    m_ui.queries->addItem(tr("(Empty)"));

  QApplication::restoreOverrideCursor();
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
	(QIcon::fromTheme("dialog-ok", QIcon(":/16x16/filesave.png")));
    }
  else
    {
      // Faenza.

      m_ui.close->setIcon(QIcon(":/32x32/cancel.png"));
      m_ui.delete_query->setIcon(QIcon(":/16x16/eraser.png"));
      m_ui.export_results->setIcon(QIcon(":/32x32/save.png"));
      m_ui.go->setIcon(QIcon(":/32x32/ok.png"));
      m_ui.reset->setIcon(QIcon(":/32x32/reload.png"));
      m_ui.save_query->setIcon(QIcon(":/16x16/filesave.png"));
    }
}

void biblioteq_statistics::show(QMainWindow *parent, const bool center)
{
  Q_UNUSED(center);
#ifdef Q_OS_ANDROID
  showMaximized();
#else
  if(parent)
    resize(qRound(0.80 * parent->size().width()),
	   qRound(0.80 * parent->size().height()));

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

void biblioteq_statistics::slotDelete(void)
{
  if(m_ui.queries->currentText() == tr("(Empty)"))
    {
      m_ui.delete_query->animateNegatively(2500);
      return;
    }

  if(QMessageBox::
     question(this,
	      tr("BiblioteQ: Question"),
	      tr("Are you sure that you wish to delete the query %1?").
	      arg(m_ui.queries->currentText()),
	      QMessageBox::No | QMessageBox::Yes,
	      QMessageBox::No) == QMessageBox::No)
    {
      QApplication::processEvents();
      return;
    }
  else
    QApplication::processEvents();

  QSettings settings;

  settings.remove(QString("statistics/%1").arg(m_ui.queries->currentText()));

  if(settings.status() == QSettings::NoError)
    m_ui.delete_query->animate(2500);
  else
    m_ui.delete_query->animateNegatively(2500);

  populateStatistics();
}

void biblioteq_statistics::slotOtherOptionsChanged(void)
{
  m_sqlSyntaxHighlighter->setKeywordsColors
    (m_otheroptions->customQueryColors());
  m_ui.query->setPlainText(m_ui.query->toPlainText());
  prepareIcons();
}

void biblioteq_statistics::slotReset(void)
{
  m_ui.queries->setCurrentIndex(0);
  m_ui.query->clear();
  m_ui.results_table->setRowCount(0);
}

void biblioteq_statistics::slotSave(void)
{
  auto const str(m_ui.query->toPlainText().trimmed());

  if(str.isEmpty())
    {
      m_ui.save_query->animateNegatively(2500);
      return;
    }

  QString name("");
  auto ok = true;

  name = QInputDialog::getText
    (this,
     tr("BiblioteQ: Save Query Name"),
     tr("Query Name"),
     QLineEdit::Normal,
     "",
     &ok).remove('\n').remove('\r').trimmed();

  if(!ok || name.isEmpty())
    return;

  QSettings().setValue(QString("statistics/%1").arg(name), str);
  populateStatistics();
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
