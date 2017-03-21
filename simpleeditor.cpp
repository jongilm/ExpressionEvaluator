// simpleeditor.cpp : Implementation of Simple Editor class.
// Jonathan Gilmore, 28/02/2009
//

////////////////////////////////////////////////////////////////////////////////////////
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

#include "stdafx.h"
#include <conio.h>

#include "simpleeditor.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////
// CSimpleEditor implementation
////////////////////////////////////////////////////////////////////////////
CSimpleEditor::CSimpleEditor(void)
// Default Constructor
{
  // By default, all alpha characters, numerics, space and dot are allowed.
  bAllowNumerics = true;
  bAllowAlpha = true;
  sAllowOtherCharacters = ". ";
  sResult.resize(0);
}

CSimpleEditor::CSimpleEditor(bool argAllowNumerics, bool argAllowAlpha, char *szOtherCharacters)
// Constructor with additional args allowing for customisation
{
  bAllowNumerics = argAllowNumerics;
  bAllowAlpha = argAllowAlpha;
  sAllowOtherCharacters = szOtherCharacters;
  sResult.resize(0);
}

CSimpleEditor::~CSimpleEditor(void) 
// Destructor
{
}

void CSimpleEditor::AllowNumerics(bool argAllowNumerics) // default value = true
// By default, numerics are allowed.
// This method allows entry of numerics [0-9] to be inhibited.
{
  bAllowNumerics = argAllowNumerics;
}

void CSimpleEditor::AllowAlpha(bool argAllowAlpha) // default value = true
// By default, alpha characters are allowed.
// This method allows entry of alpha characters [a-z,A-Z] to be inhibited.
{
  bAllowAlpha = argAllowAlpha;
}

void CSimpleEditor::AllowOtherCharacters(char *szOtherCharacters)
// By default, all alpha characters, numerics, space and dot are allowed.
// The supported characters can be extended (replaced) by calling 
// the AllowOtherCharacters() method.
{
  sAllowOtherCharacters = szOtherCharacters;
}

bool CSimpleEditor::Edit(void)
// The Edit() method passes control to the user for input.
// If data capture is cancelled (ESC), this Edit() method returns false, otherwise true.
{
  char ch = 0;

  // "cin >> argResult;" or "cin.get()" don't quite do what is needed
  // Limitations and TODO: Navigation, Insertion & Deletion are not yet supported

  sResult.resize(0);
  for(;;)
  {
    ch = getch(); 
    if (ch==0x1B || ch==EOF) // Escape or end of stream etc
    {
      return false;
    }
    else if (ch==0x0D || ch==0x0A || ch==0x09) // CR or LF or TAB
    {
      break;
    }
    else if (ch==0x08) // Backspace
    {
      if (sResult.size()>0)
      {
        putch(ch);
        putch(' ');
        putch(ch);
        sResult.resize(sResult.size()-1);
      }
    }
    else if ((bAllowAlpha    && isalpha(ch)) || 
             (bAllowNumerics && isdigit(ch)) ||
              sAllowOtherCharacters.find_first_of(ch)!=std::string::npos)
    {
      putch(ch);
      sResult += ch;
    }
  }
  return true;
}

void CSimpleEditor::GetResult(std::string &argResult)
{
  argResult = sResult;
}

