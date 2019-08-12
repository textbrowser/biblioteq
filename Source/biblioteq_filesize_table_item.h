#ifndef _BIBLIOTEQ_FILESIZE_TABLE_ITEM_H_
#define _BIBLIOTEQ_FILESIZE_TABLE_ITEM_H_

#include <QTableWidgetItem>

class biblioteq_filesize_table_item: public QTableWidgetItem
{
public:
  biblioteq_filesize_table_item(const QString &text);
  bool operator <(const QTableWidgetItem &other) const;
};

#endif
