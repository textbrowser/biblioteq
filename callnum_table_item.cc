/*
** Jason Perry.
*/

/*
** -- Qt Includes --
*/
#include <QString>
#include <QRegExp>
#include <QDebug>
#include <QTableWidgetItem>

/*
** -- Local Includes --
*/
#include "callnum_table_item.h"

#undef CALLNUM_DEBUG

callnum_table_item::callnum_table_item(const QString& str) :
    QTableWidgetItem(str) {}

int callnum_lt (QString m, QString n) {
    
    // In all its glory...
    QString lc_regexp("([A-Za-z]+) *([0-9]+)(?: *\\.([0-9]+))?(?: *\\.([A-Za-z][0-9]+))?(?: *([A-Za-z][0-9]+))?(?: +([0-9]{4}))? *");
    // CAPTURE FIELDS - only the first two are required.
    //   1. Call letters
    //   2. Call number
    //   3. Call number decimal places
    //   4. Cutter number 1  (these are sorted as decimals, so no need to
    //      split the characters off - lexicographic works.
    //   5. Cutter number 2
    //   6. Year (space is required before it)
    // reference: http://geography.about.com/library/congress/blhowto.htm
    
    QRegExp match1(lc_regexp);
    QRegExp match2(match1); // copy ctor should be faster than re-parse.

    bool res1 = match1.exactMatch(m);
    bool res2 = match2.exactMatch(n);

#ifdef CALLNUM_DEBUG
    qDebug() << "result: " << res1 << " " << res2 << endl;
    qDebug() << "matches: " << endl;
    for (int i = 1; i <= 6; i++) 
        qDebug() << match1.cap(i) << " " << match2.cap(i) << endl;
#endif

    // first, make sure the regex matched.
    if (res1 && res2) {
        if (match1.cap(1) == match2.cap(1)) {
            if (match1.cap(2) == match2.cap(2)) {
                // from here on out we can lexicographically sort.
                for (int i=3; i <= 5; i++) {
                    if (match1.cap(i) != match2.cap(i))
                        // empty string is less than anything.
                        return match1.cap(i) < match2.cap(i);
                }
                return match1.cap(6) < match2.cap(6);
            }
            else // different call numbers
                return match1.cap(2).toInt() < match2.cap(2).toInt();
        }
        else // different call letters
            return match1.cap(1).toUpper() < match2.cap(1).toUpper();
    }
    else {
        qDebug() << "Call number regex match failed" << endl;
        return m < n;
    }

}

bool callnum_table_item::operator <(const QTableWidgetItem &other) const
{
    return callnum_lt(text(), other.text());
}

