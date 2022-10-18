/*
** Copyright (c) 2006 - present, Alexis Megas, Jason Perry.
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

#include "biblioteq_callnum_table_item.h"

#include <QDebug>
#include <QTableWidgetItem>

#undef BIBLIOTEQ_CALLNUM_DEBUG

static int callnum_lt(const QString &m, const QString &n)
{
  // In all of its glory.

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  QString lc_regexp
    ("([A-Za-z]+) *([0-9]+)(?: *\\.([0-9]+))?"
     "(?: *\\.([A-Za-z][0-9]+))?(?: *([A-Za-z][0-9]+))?(?: +([0-9]{4}))? *");

  // CAPTURE FIELDS - only the first two are required.
  //   1. Call letters.
  //   2. Call number.
  //   3. Call number decimal places.
  //   4. Cutter number 1  (these are sorted as decimals, so no need to
  //      split the characters off - lexicographic works).
  //   5. Cutter number 2.
  //   6. Year (space is required before it).
  // (http://geography.about.com/library/congress/blhowto.htm)

  QRegExp match1(lc_regexp);
  QRegExp match2(match1); // Copy constructor should be faster.
  auto res1 = match1.exactMatch(m);
  auto res2 = match2.exactMatch(n);

#ifdef BIBLIOTEQ_CALLNUM_DEBUG
  qDebug() << "result: " << res1 << " " << res2 << endl;
  qDebug() << "matches: " << endl;

  for(int i = 1; i <= 6; i++)
    qDebug() << match1.cap(i) << " " << match2.cap(i) << endl;
#endif

  // First, make sure the regex matched.

  if(res1 && res2)
    {
      if(match1.cap(1) == match2.cap(1))
	{
	  if(match1.cap(2) == match2.cap(2))
	    {
	      // From here on out we can lexicographically sort.

	      for(int i = 3; i <= 5; i++)
		{
		  if(match1.cap(i) != match2.cap(i))
		    // Empty string is less than anything.

		    return match1.cap(i) < match2.cap(i);
		}

	      return match1.cap(6) < match2.cap(6);
	    }
	  else // Different call numbers.
	    return match1.cap(2).toInt() < match2.cap(2).toInt();
	}
      else // Different call letters.
	return match1.cap(1).toUpper() < match2.cap(1).toUpper();
    }

#ifdef BIBLIOTEQ_CALLNUM_DEBUG
  qDebug() << "Call number regex match failed." << endl;
#endif
#endif
  return m < n;
}

biblioteq_callnum_table_item::biblioteq_callnum_table_item
(const QString &str):QTableWidgetItem(str)
{
}

bool biblioteq_callnum_table_item::
operator <(const QTableWidgetItem &other) const
{
  return callnum_lt(text(), other.text());
}
