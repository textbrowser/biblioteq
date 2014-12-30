#include "marc.h"

marc::marc(const PROTOCOL protocol, const RECORD_SYNTAX recordSyntax)
{
  m_protocol = protocol;
  m_recordSyntax = recordSyntax;
}

marc::~marc()
{
}

void marc::parseSRU(void)
{
}

void marc::parseZ3950(void)
{
}

void marc::setData(const QString &data)
{
  m_data = data;

  if(m_protocol == SRU)
    parseSRU();
  else
    parseZ3950();
}
