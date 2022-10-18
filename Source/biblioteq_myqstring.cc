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

#include "biblioteq_myqstring.h"

biblioteq_myqstring::biblioteq_myqstring(const QString &str):QString(str)
{
}

biblioteq_myqstring::biblioteq_myqstring(const char *str):QString(str)
{
}

QString biblioteq_myqstring::escape(const QString &str,
				    const bool caseinsensitive)
{
  auto mystr(str);

  mystr = mystr.replace("'", "''");

  if(caseinsensitive)
    mystr = mystr.toLower();

  return mystr.trimmed();
}

QString biblioteq_myqstring::prepConfigString
(const QString &str, const bool ignore_embedded_comments)
{
  auto mystr = str;

  if(mystr.trimmed().startsWith("#"))
    return mystr.trimmed();
  else if(mystr.contains("#") && !ignore_embedded_comments)
    mystr.resize(mystr.indexOf('#'));

  return mystr.trimmed();
}
