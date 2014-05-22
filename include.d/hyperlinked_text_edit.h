#ifndef _HYPERLINKED_TEXT_EDIT_H_
#define _HYPERLINKED_TEXT_EDIT_H_

#include <iostream>

#include <QTextBrowser>
#include <QUrl>

using namespace std;

class hyperlinked_text_edit: public QTextBrowser
{
  Q_OBJECT

 public:
  hyperlinked_text_edit(QWidget *parent);
  void setMultipleLinks(const QString &searchType,
			const QString &searchField,
			const QString &str);

 private slots:
  void slotAnchorClicked(const QUrl &url);
};

#endif
