#include "GSMBand.h"

GSMBand::GSMBand(bool trace)
{
}

GSM3_NetworkStatus_t GSMBand::begin()
{
  return IDLE;
}

String GSMBand::getBand()
{
  return "";
}
    
bool GSMBand::setBand(String band)
{
  return false;
}
