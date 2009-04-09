#ifndef _HYPERLINKED_TEXT_EDIT_H_
#define _HYPERLINKED_TEXT_EDIT_H_

/*
** -- C++ Includes --
*/

#include <iostream>
using namespace std;

/*
** -- Qt Includes --
*/

#include <QUrl>
#include <QTextBrowser>

class hyperlinked_text_edit: public QTextBrowser
{
  Q_OBJECT

 public:
  hyperlinked_text_edit(QWidget *);
  void setMultipleLinks(const QString &, const QString &, const QString &);

 private slots:
  void slotAnchorClicked(const QUrl &);
};

#endif
