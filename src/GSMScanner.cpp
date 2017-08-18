#include "GSMScanner.h"

GSMScanner::GSMScanner(bool trace)
{
}

GSM3_NetworkStatus_t GSMScanner::begin()
{
  return IDLE;
}

String GSMScanner::getCurrentCarrier()
{
  return "";
}

String GSMScanner::getSignalStrength()
{
  return "";
}

String GSMScanner::readNetworks()
{
  return "";
}
