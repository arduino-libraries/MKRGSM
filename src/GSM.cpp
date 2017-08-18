#include "GSM.h"

GSM::GSM(bool debug)
{

}

GSM3_NetworkStatus_t GSM::begin(const char* pin, bool restart, bool synchronous)
{
  return ERROR;
}

int GSM::isAccessAlive()
{
  return 0;
}

bool GSM::shutdown()
{
  return false;
}

bool GSM::secureShutdown()
{
  return true;
}


int GSM::ready()
{
  return 0;
}
