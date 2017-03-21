// simpleeditor.h :
// Interface/Include file for simpleeditor.cpp - A Simple Editor class.
// Jonathan Gilmore, 28/02/2009

////////////////////////////////////////////////////////////////////////////////////////
// CSimpleEditor Class
// A simple console editor to get input from the user.
// Cursor navigation, Insertion & Deletion are not supported at this time.
// CR (Enter), LF or Tab are used to accept input.
// The user can press Escape at any time to cancel imput.
// By default, all alpha characters, numerics, space and dot are allowed.
// The supported characters can be extended (replaced) by calling 
// the AllowOtherCharacters() method.
// The Edit() method passes control to the user for input.
// If data capture is cancelled (ESC), this Edit() method returns false, otherwise true.
// The resultant string is retrieved via the GetResult() method.
////////////////////////////////////////////////////////////////////////////////////////
 

#if !defined(SIMPLEEDITOR_H_INCLUDED_)
#define SIMPLEEDITOR_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

class CSimpleEditor 
{
  public:
    CSimpleEditor(void);
    CSimpleEditor(bool argAllowNumerics, bool argAllowAlpha, char *szOtherCharacters);
    ~CSimpleEditor(); 

    void AllowNumerics(bool argAllowNumerics = true);
    void AllowAlpha(bool argAllowAlpha = true);
    void AllowOtherCharacters(char *szOtherCharacters);

    bool Edit(void);

    void GetResult(std::string &argResult);

  private:
    bool bAllowNumerics;
    bool bAllowAlpha;
    std::string sAllowOtherCharacters;
    std::string sResult;
};

#endif // !defined(SIMPLEEDITOR_H_INCLUDED_)
