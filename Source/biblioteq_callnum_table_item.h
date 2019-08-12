/*
** Jason Perry.
*/

#ifndef _BIBLIOTEQ_CALLNUM_TABLE_ITEM_H_
#define _BIBLIOTEQ_CALLNUM_TABLE_ITEM_H_

#include <QTableWidgetItem>

class biblioteq_callnum_table_item: public QTableWidgetItem
{
 public:
  biblioteq_callnum_table_item(const QString &str);
  bool operator <(const QTableWidgetItem &other) const;
};

#endif
