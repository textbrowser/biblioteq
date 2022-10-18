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

#include "biblioteq_bgraphicsscene.h"

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

biblioteq_bgraphicsscene::biblioteq_bgraphicsscene(QObject *parent):
  QGraphicsScene(parent)
{
}

biblioteq_bgraphicsscene::~biblioteq_bgraphicsscene()
{
}

void biblioteq_bgraphicsscene::keyPressEvent(QKeyEvent *event)
{
  QGraphicsScene::keyPressEvent(event);

  if(event)
    switch(event->key())
      {
      case Qt::Key_Backspace:
      case Qt::Key_Delete:
	{
	  emit deleteKeyPressed();
	  break;
	}
      case Qt::Key_Enter:
      case Qt::Key_Return:
	{
	  emit enterKeyPressed();
	  break;
	}
      default:
	{
	  break;
	}
      }
}

void biblioteq_bgraphicsscene::mouseDoubleClickEvent
(QGraphicsSceneMouseEvent *event)
{
  /*
  ** Do not issue QGraphicsScene::mouseDoubleClickEvent() as the
  ** scene may temporarily lose a future click.
  */

  if(event && itemAt(event->scenePos(), QTransform()))
    emit itemDoubleClicked();
}
