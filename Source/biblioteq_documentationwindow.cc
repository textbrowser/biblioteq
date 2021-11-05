/*
** Alexis Megas.
*/

#include <QDesktopServices>
#include <QPrintDialog>
#include <QPrinter>

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
  connectSignals();
}

biblioteq_documentationwindow::
~biblioteq_documentationwindow()
{
}

void biblioteq_documentationwindow::connectSignals(void)
{
  connect(m_ui.action_Close,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(close(void)),
          Qt::UniqueConnection);
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

void biblioteq_documentationwindow::
setAllowOpeningOfExternalLinks(const bool state)
{
  m_openExternalLinks = state;
}

void biblioteq_documentationwindow::setHtml(const QString &html)
{
  m_ui.text->setHtml(html);
}

void biblioteq_documentationwindow::show(void)
{
  biblioteq_misc_functions::center
    (this, static_cast<QMainWindow *> (parentWidget()));
  QMainWindow::showNormal();
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
  m_ui.find->setFocus();
  m_ui.find->selectAll();
}

void biblioteq_documentationwindow::slotFindText(void)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
  QTextDocument::FindFlags options = QTextDocument::FindFlags();
#else
  QTextDocument::FindFlags options = 0;
#endif

  if(qobject_cast<QToolButton *> (sender()) == m_ui.previous)
    options = QTextDocument::FindBackward;

  if(m_ui.find->text().isEmpty())
    {
      m_ui.find->setPalette(m_originalFindPalette);
      m_ui.text->moveCursor(QTextCursor::Left);
    }
  else if(!m_ui.text->find(m_ui.find->text(), options))
    {
      QColor color(240, 128, 128); // Light Coral
      QPalette palette(m_ui.find->palette());

      palette.setColor(m_ui.find->backgroundRole(), color);
      m_ui.find->setPalette(palette);

      if(!options)
	m_ui.text->moveCursor(QTextCursor::Start);
      else
	m_ui.text->moveCursor(QTextCursor::End);
    }
  else
    m_ui.find->setPalette(m_originalFindPalette);
}

void biblioteq_documentationwindow::slotPrint(void)
{
  QPrinter printer(QPrinter::HighResolution);
  QPrintDialog printDialog(&printer, this);

  printDialog.setWindowIcon(windowIcon());

  if(printDialog.exec() == QDialog::Accepted)
    m_ui.text->print(&printer);
}
