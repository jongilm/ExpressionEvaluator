// variable.h : 
// Interface/Include file for variable.cpp
// Jonathan Gilmore, 28/02/2009

////////////////////////////////////////////////////////////////////////////////////////
// CVariable Class
// This class defines a variable object. 
// Variables names are limited to single case-sensitive alphabetic characters.
// The variable can take on only real numeric floating point values (double).
////////////////////////////////////////////////////////////////////////////////////////
 

#if !defined(VARIABLE_H_INCLUDED_)
#define VARIABLE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>

class CVariable
{
  public:
    CVariable();                          // Constructor
    CVariable(char argName);              // Constructor where the name is also set. 
    ~CVariable();                         // Destructor

    void SetName(char argName);           // Set variable name
    const char GetName(void);             // Get variable name

    void SetValue(double &argValue);      // Set Value of numeric variable.
    void SetValue(std::string &argValue); // Set Value of numeric variable. Arg is a numeric string eg "5" or "3.14159"
    double GetValue(void);                // Get value of numeric variable

    void ClearValue(void);                // "Unset" variable. Subsequent call to IsValueSet() would return false

    bool IsValueSet(void);                // Has value been set

  private:
    char cName;                           // Variable name - limited to single case-sensitive alphabetic characters.
    double lfValue;                       // Variable value - real numeric only.
    bool bIsSet;                          // Flag to keep track of weather or not the value of has been set.
};

#endif // !defined(VARIABLE_H_INCLUDED_)
