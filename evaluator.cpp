// evaluator.cpp : 
// Implementation of evaluator class.
// Jonathan Gilmore, 28/02/2009
//

////////////////////////////////////////////////////////////////////////////////////////
// CEvaluator Abstract Class
// This class defines a simple Expression Evaluator object.
// Expressions are limited to the 4 basic operators, braces, 
// numeric constants and numeric variables with single letter names.
// Operator precedence is respected. Embedded spaces are ignored.
// There is no (practical) limit to the length of the expression. 
// The maximum number of variables is 52 [a-z,A-Z].
// 
// This class is implemented as an abstract class so as to keep all 
// platform specific UI separate from the functionality of the 
// Evaluator itself.
//
// The approach taken can be summarised as follows:
// 1. Parse the expression to determine what variables are used (if any)
// 2. Call the pure virtual function to get values for each distinct variable.
// 3. Parse the expression again, essentially converting the expression into RPN
//    (Reverse Polish Notation) format. This is done by building up two stacks
//    One for operands (vOperand) and one for operators (vOperator).
//    At any time that low precedence operators are encountered (+/-), then
//    the stacks are evaluated down to a single operand before this operator is 
//    put on the stack for future processing.
//    If an open-brace is encountered, then the evaluator is called recursively
//    and retuns when the associated close-brace is found - again resulting in a 
//    single operand on the stack of the calling context.
//    When the end of the expression is reached, there will be one operand remaining.
//    This is the result.
////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//#define SHOW_DEBUGGING

#include <math.h>
#ifdef SHOW_DEBUGGING
#include <iostream>
#endif

#include "evaluator.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////
// CEvaluator implementation
////////////////////////////////////////////////////////////////////////////
CEvaluator::CEvaluator(void)
{
  ErrNo = ERR_OK;
  sExpression.resize(0);
  vVariable.clear();
}

CEvaluator::~CEvaluator(void)
{
  sExpression.resize(0);
  vVariable.clear();
}

tERRNO CEvaluator::GetErrorNumber(void)
{
  return ErrNo;
}

const char const *CEvaluator::GetErrorDescription(tERRNO ErrNo)
{
  char *pErrDesc;
  switch(ErrNo)
  {
    case ERR_OK                : pErrDesc = "No Errors"; break;
    case ERR_EMPTY_EXPRESSION  : pErrDesc = "WARNING: Empty Expression"; break;
    case ERR_INVALID_VARNAME   : pErrDesc = "SYNTAX ERROR: Invalid variable name. Valid variable names are a-z & A-Z"; break;
    case ERR_VARNAME_TOO_LONG  : pErrDesc = "SYNTAX ERROR: Invalid variable name. Only single character variable names are allowed"; break;
    case ERR_UMATCHED_BRACES   : pErrDesc = "SYNTAX ERROR: Unmatched braces"; break;
    case ERR_DIVIDE_BY_ZERO    : pErrDesc = "SYNTAX ERROR: Divide by 0"; break;
    case ERR_UNKNOWN_OPERATOR  : pErrDesc = "SYNTAX ERROR: Unknown operator"; break;
    case ERR_OPERAND_EXPECTED  : pErrDesc = "SYNTAX ERROR: Operand expected"; break;
    case ERR_EVALUATION_FAILED : pErrDesc = "SYNTAX ERROR: Intermediate Evaluation failed"; break;
    case ERR_OPERATOR_EXPECTED : pErrDesc = "SYNTAX ERROR: Operator expected"; break;
    case ERR_TOO_MANY_OPERATORS: pErrDesc = "SYNTAX ERROR: Too many operators"; break;
    case ERR_TOO_MANY_OPERANDS : pErrDesc = "SYNTAX ERROR: Too many operands"; break;
    case ERR_NO_MEMORY         : pErrDesc = "FATAL ERROR: Out of memory"; break;
    default                    : pErrDesc = "UNKNOWN ERROR"; break;
  }
  return pErrDesc;
}


bool CEvaluator::SetExpression(const char *szExpression)
{
  sExpression = szExpression;
  return ParseExpressionForVariableNames();
}

bool CEvaluator::InitialiseVariables(void)
{
  vector<CVariable>::iterator cii;
  double lfValue = 0.0;
  bool rc;

  for(cii=vVariable.begin(); cii!=vVariable.end(); cii++)
  {
    // The following method is a pure virtual function in CEvaluator.
    // It is done this way so as to keep user interaction implementation specific.
    // i.e. CEvaluator makes no assumptions regarding environment of UI/MMI.
    rc = InitialiseVariable(cii->GetName(), cii->GetValue(), lfValue); 
    if (!rc) // Escape was pressed (or error)
    { 
      return false; // Cancelling
    }
    cii->SetValue(lfValue);
  }
  return true;
}

bool CEvaluator::VariableExists(char ch)
{
  vector<CVariable>::iterator cii;

  for(cii=vVariable.begin(); cii!=vVariable.end(); cii++)
  {
    if (cii->GetName() == ch)
      return true;
  }
  return false;
}

bool CEvaluator::AddVariable(char ch)
{
  if (!isalpha(ch))
  {
    ErrNo = ERR_INVALID_VARNAME;
    return false;
  }

  if (!VariableExists(ch))
  {
    vVariable.push_back(ch);
  }
  return true;
}

bool CEvaluator::ParseExpressionForVariableNames(void)
{
  int PosOfLastVariable = -2;
  int depth = 0;

  if (sExpression.length()==0)
  {
    ErrNo = ERR_EMPTY_EXPRESSION;
    return false;
  }  

  for (int i=0;i<sExpression.length(); i++)
  {
    char ch = sExpression[i];

    if (isalpha(ch))
    {
      // if the character emmediately prior to this one was a variable
      // i.e alpha, then we have found two consecutive alpha chars
      // which is not allowed.
      if (i==PosOfLastVariable+1) 
      {
        ErrNo = ERR_VARNAME_TOO_LONG;
        return false;
      }
      AddVariable(ch);
      PosOfLastVariable = i;
    }
    else if (ch=='(')
      depth++;
    else if (ch==')')
      depth--;
  }

  // Make use of this initial parse to also check for unmatched braces
  // (Note that this does not guarantee perfect syntax).
  if (depth!=0) 
  {
    ErrNo = ERR_UMATCHED_BRACES;
    return false;
  }
  return true;
}

int CEvaluator::GetNumberOfVariables(void)
{
  return vVariable.size();
}

double CEvaluator::GetVariableValue(char ch)
{
  vector<CVariable>::iterator cii;

  for(cii=vVariable.begin(); cii!=vVariable.end(); cii++)
  {
    if (cii->GetName() == ch)
      return cii->GetValue();
  }
  return 0.0;
}

bool CEvaluator::IsOperator(char ch) // static
{
  return (ch=='*' || ch=='/' || ch=='+' || ch=='-' );
}

bool CEvaluator::ProcessOperators(vector<double> &vOperand, vector<char> &vOperator)
{
  // Evaluate Expression
  while (vOperator.size() > 0)
  {
    double Operand1;
    double Operand2;
    char Operator;
    double lfTempResult;

    // Get last operator, and remove from stack
    Operator = vOperator.back();
    vOperator.pop_back();

    // Get last 2 operands, and remove from stack
    Operand1 = vOperand.back();
    vOperand.pop_back();
    Operand2 = vOperand.back();
    vOperand.pop_back();

    // Do calculation
    switch (Operator)
    {
      case '+':
        lfTempResult = Operand2 + Operand1;
        break;

      case '-':
        lfTempResult = Operand2 - Operand1;
        break;

      case '*':
        lfTempResult = Operand2 * Operand1;
        break;

      case '/':
        if (Operand1 == 0.0)
        {
          ErrNo = ERR_DIVIDE_BY_ZERO;
          return false;
        }
        lfTempResult = Operand2 / Operand1;
        break;

      default:
        ErrNo = ERR_UNKNOWN_OPERATOR;
        return false;
    }

#ifdef SHOW_DEBUGGING
    cout << "CALC(" << Operand2 << Operator << Operand1 << "=" << lfTempResult << ")" << endl;
#endif
    // Place result back into Operand stack.
    // This is either an intermediate result, to be used
    // in a future calculation, or could be our final result.
    vOperand.push_back(lfTempResult);
  }
  return true;
}

double CEvaluator::EvaluateExpression(string *pExpression,int *pNumberOfCharactersProcessed)
{
  double lfResult = 0.0;
  int i;
  double value1 = 0.0;
  tSTATE state = STATE_EXPECT_OPERAND;
  bool NegateNextOperand = false;
  string *pExpr;
  
  vector<double> vOperand;
  vector<char> vOperator;

  vOperand.clear();
  vOperator.clear();

  if (pExpression==NULL)
    pExpr = &sExpression; // member var as initialised with SetExpression(). (typically on first 'high level' call)
  else
    pExpr = pExpression; // sub-Expression (typically during recursion)

  for(i=0;i<pExpr->length();)
  {
    if (isspace((*pExpr)[i])) // Ignore all spaces
    {
      i++;
    }
    else //Non-space char
    {
      switch (state)
      {
        case STATE_EXPECT_OPERAND:
          // We expect each operator (+,-,*,/) to ultimately have an operator on each side.
          if ((*pExpr)[i] == '(')
          {
            int iNumberOfCharactersProcessed = 0;
            i++; // Increment our ptr to the character following the open brace
#ifdef SHOW_DEBUGGING
            cout << "OPENBRACE(" << endl;
#endif
            // Recursively evaluate the sub-expression found inside the open-brace.
            // If the sub-expression contains a subsequent open brace, the same will happen again.
            // Each recursed call will return when a close brace is encountered.
            value1 = EvaluateExpression(&pExpr->substr(i),&iNumberOfCharactersProcessed);
#ifdef SHOW_DEBUGGING
            cout << "OPERAND(" << value1 << ")" << endl;
#endif
            // The EvaluateExpression function returns the resultant operand.
            // We push that onto the stack.
            // In effect, the contents of the braces are replaced by a single operand.
            vOperand.push_back(value1);
            // Advance to the character immediately following the close brace.
            i += iNumberOfCharactersProcessed;
            // Now that we have a single operand (in place of the expression in braces),
            // we continue parsing the expression, expecting to find an operator (or end of expression).
            state = STATE_EXPECT_OPERATOR;
          }
          else if ((*pExpr)[i]=='-') // Unary Minus
          {
            i++;
            NegateNextOperand = true; // Remember this for later i.e. once we have the next operand.
          }
          else if (isalpha((*pExpr)[i])) // Alpha - must be a Variable 
          {
            value1 = GetVariableValue((*pExpr)[i]);
            i++;
            if (NegateNextOperand)
            {
              value1 = -value1;
              NegateNextOperand = false;
            }
#ifdef SHOW_DEBUGGING
            cout << "OPERAND(" << value1 << ")" << endl;
#endif
            // Push the variable value onto the stack as an operand
            vOperand.push_back(value1);
            state = STATE_EXPECT_OPERATOR;
          }
          else if (isdigit((*pExpr)[i])) // Numeric constant
          {
            std::string tempstring;

            // Build up a substring of the numeric constant
            tempstring.resize(0);
            while (isdigit((*pExpr)[i]) || (*pExpr)[i]=='.')
            {
              tempstring += (*pExpr)[i];
              i++;
            }
            // And convert to a double
            value1 = atof(tempstring.c_str());
            // Negate if required
            if (NegateNextOperand)
            {
              value1 = -value1;
              NegateNextOperand = false;
            }
#ifdef SHOW_DEBUGGING
            cout << "OPERAND(" << value1 << ")" << endl;
#endif
            // and push onto the stack as an operand
            vOperand.push_back(value1);
            state = STATE_EXPECT_OPERATOR;
          }
          else
          {
            ErrNo = ERR_OPERAND_EXPECTED;
            return lfResult;
          }
          break;
        case STATE_EXPECT_OPERATOR:
          if ((*pExpr)[i] == ')')
          {
#ifdef SHOW_DEBUGGING
            cout << "CLOSEBRACE)" << endl;
#endif
            i++;
            break; // return from recursive call (this break will break from the switch. See "break from loop" below)
          }
          else if (IsOperator((*pExpr)[i])) // Recognsed Operator found
          {
            char op = (*pExpr)[i];

            if (vOperator.size()>0 && (op=='+' || op=='-'))
            {
              // If this operator is + or -, (i.e. lowest precedence)
              // then evaluate the sub expressions we have parsed so far.
              // These may, or may not, include higher precedence operators.
              // If not, then this step makes no difference to the final result.
              // If yes, then this step is vital in order to evaluate * & /
              // before processing the + or -.
              if (!ProcessOperators(vOperand,vOperator))
              {
                ErrNo = ERR_EVALUATION_FAILED; // Intermediate Evaluation failed
                return lfResult;
              }
            }
#ifdef SHOW_DEBUGGING
            cout << "OPERATOR(" << op << ")" << endl;
#endif
            // Now we can push the operator (+ or -) onto the stack.
            vOperator.push_back(op);
            i++;
            state = STATE_EXPECT_OPERAND;
          }
          else
          {
            ErrNo = ERR_OPERATOR_EXPECTED;
            return lfResult;
          }
          break;
      } // end of switch

      if ((*pExpr)[i] == ')')
      {
        break; // break from loop
      }
    }
  }

  // We have either come to a clode brace, or the end of the expression.
  // Either way - now is the time to evaluate the operators and operands
  // we hold on our local (possibly recursive) operator/operand stacks.
  if (!ProcessOperators(vOperand,vOperator))
  {
    ErrNo = ERR_EVALUATION_FAILED; // Intermediate Evaluation failed
    return lfResult;
  }

  // Our local  (possibly recursed) Operator stack should now be empty.
  if (vOperator.size() > 0)
  {
    ErrNo = ERR_TOO_MANY_OPERATORS;
    return 0.0;
  }
  // And we should have one operand remaining - the result of the expression (or sub-expression).
  if (vOperand.size() != 1)
  {
    ErrNo = ERR_TOO_MANY_OPERANDS;
    return 0.0;
  }

  // Extract our last remaining operand (the result), leaving
  // both our local operand and local operator stacks empty before returning.
  lfResult = vOperand.back();
  vOperand.pop_back();

  if (pNumberOfCharactersProcessed)
  {
    *pNumberOfCharactersProcessed = i+1;
#ifdef SHOW_DEBUGGING
    cout << "CHARSPROCESSED(" << *pNumberOfCharactersProcessed << ")" << endl;
#endif
  }

#ifdef SHOW_DEBUGGING
  cout << "RETURN(" << lfResult << ")" << endl;
#endif
  return lfResult; // return value of expression (or subexpression if recursing)
}

