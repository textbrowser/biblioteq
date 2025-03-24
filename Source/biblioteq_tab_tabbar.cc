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

#include <QStyle>
#include <QToolButton>
#include <QtMath>

#include "biblioteq_tab.h"
#include "biblioteq_tab_tabbar.h"

biblioteq_tab_tabbar::biblioteq_tab_tabbar(QWidget *parent):QTabBar(parent)
{
  setContextMenuPolicy(Qt::NoContextMenu);
  setDocumentMode(true);
  setElideMode(Qt::ElideRight);
  setExpanding(true);
  setStyleSheet("QTabBar::tear {border: none; image: none; width: 0px;}");
  setUsesScrollButtons(true);

  foreach(auto toolButton, findChildren <QToolButton *> ())
    if(toolButton)
      toolButton->setStyleSheet
	(QString("QToolButton {background-color: %1;"
		 "border: none;"
		 "margin-bottom: 0px;"
		 "margin-top: 0px;"
		 "}"
		 "QToolButton::menu-button {border: none;}").
	 arg(QWidget::palette().color(QWidget::backgroundRole()).name()));
}

biblioteq_tab_tabbar::~biblioteq_tab_tabbar()
{
}

QSize biblioteq_tab_tabbar::tabSizeHint(int index) const
{
  auto const tabPosition = qobject_cast<biblioteq_tab *> (parentWidget()) ?
    qobject_cast<biblioteq_tab *> (parentWidget())->tabPosition() :
    QTabWidget::North;
  auto size(QTabBar::tabSizeHint(index));

  if(tabPosition == QTabWidget::East || tabPosition == QTabWidget::West)
    {
      auto preferredTabHeight = 0;

      preferredTabHeight = qBound
	(125,
	 qMax(rect().height() / qMax(1, count()), size.height()),
	 175);
      preferredTabHeight = 5 * qCeil(preferredTabHeight / 5.0);
      size.setHeight(preferredTabHeight);
    }
  else
    {
      auto preferredTabWidth = 0;

      preferredTabWidth = qBound
	(200,
	 qMax(rect().width() / qMax(1, count()), size.width()),
	 250);
      preferredTabWidth = 5 * qCeil(preferredTabWidth / 5.0);
      size.setWidth(preferredTabWidth);
    }

  return size;
}

QTabBar::ButtonPosition biblioteq_tab_tabbar::
preferredCloseButtonPositionOpposite(void) const
{
#ifdef Q_OS_MACOS
  if(!style())
    return QTabBar::RightSide;
#else
  if(!style())
    return QTabBar::RightSide;
#endif

  auto const buttonPosition = static_cast<QTabBar::ButtonPosition>
    (style()->styleHint(QStyle::SH_TabBar_CloseButtonPosition, 0, this));

#ifdef Q_OS_MACOS
  return buttonPosition == QTabBar::LeftSide ?
    QTabBar::RightSide : QTabBar::LeftSide;
#else
  return buttonPosition == QTabBar::LeftSide ?
    QTabBar::RightSide : QTabBar::LeftSide;
#endif
}
