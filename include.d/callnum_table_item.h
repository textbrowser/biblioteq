/*
** Jason Perry.
*/

#ifndef _CALLNUM_TABLE_ITEM_H_
#define _CALLNUM_TABLE_ITEM_H_

/*
** -- Qt Includes --
*/

#include <QTableWidgetItem>

class callnum_table_item: public QTableWidgetItem
{
 public:
  callnum_table_item(const QString &);
  bool operator <(const QTableWidgetItem &) const;
};

#endif
