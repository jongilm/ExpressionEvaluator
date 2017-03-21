// MyExpressionEvaluator.cpp : Defines the entry point for the console application.
// Jonathan Gilmore, 28/02/2009
//

//////////////////////////////////////////////////////////////////////////////
// Mandate:
//
// Please could you also write the following piece of code (ideally in C++).
// 
// Write a simple mathematical expression evaluator.
// 
// Expressions may contain any combination of numbers, simple operators (plus, 
// minus, divide and multiply), single letter variables and brackets, e.g.
// 
// 	(a + 10) * 50 / ((b - 6) * 9)
// 
// The user first enters an expression. The user is then prompted to enter 
// values for the variables and the evaluator calculates the result. The 
// evaluator should allow the user to enter different values for the variables 
// any number of times, calculating a new result each time.
// 
// The program will be judged on:
//   Efficacy
//   Design Elegance
//   Code Clarity
//   Robustness
//   Error Handling
//   Efficiency
//////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include <math.h>
#include <conio.h>
#include <iostream>

#include "simpleeditor.h"
#include "evaluator.h"
#include "variable.h"
#include "MyExpressionEvaluator.h"

using namespace std;

// Define TESTMODE to run a handfull of hand-picked expressions 
// (without variables) through the evaluator in order to test 
// for correct results.
//#define TESTMODE 


#ifdef TESTMODE
#include "testdata.h"
#endif // TESTMODE

class CConsoleEvaluator : public CEvaluator
{
  public:
    bool InitialiseVariable(char VariableName, double DefaultValue, double &ValueRet);
};

bool CConsoleEvaluator::InitialiseVariable(char VariableName, double DefaultValue, double &ValueRet)
{
  CSimpleEditor *pEd = new CSimpleEditor(true,false,".-"); // Allow Numerics, Decimal point and unary minus only
  string tempstring;
  bool rc;

  if (pEd == NULL)
  {
    ErrNo = ERR_NO_MEMORY;
    return false;
  }

  cout << "Value for variable " << VariableName << " [" << DefaultValue << "]" << " : ";

  rc = pEd->Edit();
  cout << endl;

  if (!rc)
  { 
    cout << "Cancelled." << endl;
    delete pEd;
    return false;
  }

  pEd->GetResult(tempstring);

  if (tempstring.length()>0) // If the string is empty, then we keep the default (previous) value
    ValueRet = atof(tempstring.c_str());
  else
    ValueRet = DefaultValue;

  delete pEd;
  return true;
}


static void ShowBanner(void)
{
	cout << "A Simple Expression Evaluator" << endl;
	cout << "J.Gilmore, 2009" << endl << endl;
	cout << "(Press <ESC> to quit. Default values are shown in [])" << endl << endl;
}

static bool RequestExpression(string &sExpression)
{
  bool rc;
  CSimpleEditor *pEd = new CSimpleEditor(true, true, "+-*/ .()"); // bAllowNumerics, bAllowAlpha, sAllowOtherCharacters

  if (pEd == NULL)
  {
    cout << "FATAL ERROR: Out of memory" << endl;
    return false;
  }

  cout << "Please enter an expression : ";
  rc = pEd->Edit();
  cout << endl;

  pEd->GetResult(sExpression);

  delete pEd;
  return rc;
}

static void ShowExpression(string &sExpression)
{
  cout << "------------------------------------------------" << endl;
  cout << "Expression: \"" << sExpression << "\"" << endl;
}

static void ShowResult(double &lfResult)
{
  cout << "Result = " << lfResult << endl << endl;
}

static void PressAnyKeyToContinue(void)
{
  cout << "Press any key to continue";
  getch();
  cout << endl;
}

static void ShowErrorDescription(CEvaluator *p)
{
  cout << endl << p->GetErrorDescription(p->GetErrorNumber()) << endl;
}

int main(int argc, char* argv[])
{
  CConsoleEvaluator *pEvaluator = new CConsoleEvaluator();
  string *pExpression = new string();
  double lfResult;

  if (pEvaluator == NULL || pExpression == NULL)
  {
    cout << endl << "FATAL ERROR: Out of memory" << endl;
    return 1;
  }

  ShowBanner();

#ifdef TESTMODE
  TestEvaluator(pEvaluator);
#else // TESTMODE
  if (RequestExpression(*pExpression))
  {
    if (pEvaluator->SetExpression(pExpression->c_str()))
    {
      for(;;)
      {
        ShowExpression(*pExpression);
        if (!pEvaluator->InitialiseVariables())
          break;

        lfResult = pEvaluator->EvaluateExpression();
        ShowResult(lfResult);

        if (pEvaluator->GetNumberOfVariables() == 0)
          break; // If the expression has no variables, then there is no point in doing this loop more than once.
      }
    }
  }
#endif // TESTMODE

  ShowErrorDescription(pEvaluator);
  PressAnyKeyToContinue();

  delete pExpression;
  delete pEvaluator;
  return 0;
}


