/*
** -- Local Includes --
*/

#include "biblioteq_filesize_table_item.h"

biblioteq_filesize_table_item::biblioteq_filesize_table_item
(const QString &text):QTableWidgetItem(text)
{
  m_text = text;
}

bool biblioteq_filesize_table_item::operator <
(const QTableWidgetItem &other) const
{
  return QString(m_text).remove(',').toLongLong() <
    QString(other.text()).remove(',').toLongLong();
}
