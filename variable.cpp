// variable.cpp : 
// Implementation of variable class.
// Jonathan Gilmore, 28/02/2009
//

#include "stdafx.h"
#include "variable.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////
// CVariable implementation
////////////////////////////////////////////////////////////////////////////
CVariable::CVariable(void)
{
  cName = 0;
  lfValue = 0.0;
  bIsSet = false;
}

CVariable::CVariable(char argName)
{
  cName = argName;
  lfValue = 0.0;
  bIsSet = false;
}

CVariable::~CVariable(void)
{
}

void CVariable::SetName(char argName)
{
  cName = argName;
}

const char CVariable::GetName(void)
{
  return cName;
}

void CVariable::SetValue(double &argValue)
{
  lfValue = argValue;
  bIsSet = true;
}

void CVariable::SetValue(string &argValue)
{
  lfValue = atof(argValue.c_str());
  bIsSet = true;
}

double CVariable::GetValue(void)
{
  return lfValue;
}

bool CVariable::IsValueSet(void)
{
  return bIsSet;
}

void CVariable::ClearValue(void)
{
  lfValue = 0.0;
  bIsSet = false;
}
