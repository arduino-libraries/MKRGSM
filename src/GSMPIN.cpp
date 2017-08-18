#include "GSMPIN.h"

GSMPIN::GSMPIN()
{
}

void GSMPIN::begin()
{
}

int GSMPIN::isPIN()
{
  return -2;
}

int GSMPIN::checkPIN(String pin)
{
  return -1;
}


int GSMPIN::checkPUK(String puk, String pin)
{
  return -1;
}

void GSMPIN::changePIN(String old, String pin)
{
}

void GSMPIN::switchPIN(String pin)
{
}

int GSMPIN::checkReg()
{
  return -1;
}

bool GSMPIN::getPINUsed()
{
  return false;
}

void GSMPIN::setPINUsed(bool used)
{
}
