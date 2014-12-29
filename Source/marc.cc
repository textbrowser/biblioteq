#include "marc.h"

marc::marc(const QString &protocol, const QString &recordSyntax)
{
  m_protocol = protocol;
  m_recordSyntax = recordSyntax;
}

marc::~marc()
{
}
