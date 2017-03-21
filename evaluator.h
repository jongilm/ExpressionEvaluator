// evaluator.h : 
// Interface/Include file for evaluator.cpp
// Jonathan Gilmore, 28/02/2009

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
// Please also see implementation notes in .cpp file
////////////////////////////////////////////////////////////////////////////////////////

#if !defined(EVALUATOR_H_INCLUDED_)
#define EVALUATOR_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
#include "variable.h"

typedef enum tagSTATE
{
  STATE_EXPECT_OPERAND = 1,
  STATE_EXPECT_OPERATOR = 2,
} tSTATE;

typedef enum tagERRNO
{
  ERR_OK = 0,
  ERR_EMPTY_EXPRESSION  ,
  ERR_INVALID_VARNAME   ,
  ERR_VARNAME_TOO_LONG  ,
  ERR_UMATCHED_BRACES   ,
  ERR_DIVIDE_BY_ZERO    ,
  ERR_UNKNOWN_OPERATOR  ,
  ERR_OPERAND_EXPECTED  ,
  ERR_EVALUATION_FAILED ,
  ERR_OPERATOR_EXPECTED ,
  ERR_TOO_MANY_OPERATORS,
  ERR_TOO_MANY_OPERANDS ,
  ERR_NO_MEMORY         ,
  ERR_UNKNOWN
} tERRNO;

class CEvaluator 
{
  public:
    CEvaluator();
    ~CEvaluator(); 

    bool SetExpression(const char *szExpression);
    bool InitialiseVariables(void);
    int GetNumberOfVariables(void);
    double EvaluateExpression(std::string *pExpression=NULL, int *pNumberOfCharactersProcessed=NULL);

    // The names of the variables for which values are required, are only known after 
    // the initial parsing of the expression.
    // The following method is a pure virtual function.
    // It is done this way so as to keep user interaction implementation specific.
    virtual bool InitialiseVariable(char VariableName, double DefaultValue, double &ValueRet) = 0;

    tERRNO GetErrorNumber(void);
    const char const *GetErrorDescription(tERRNO ErrNo);

  protected:
    tERRNO ErrNo;

  private:
    bool ParseExpressionForVariableNames(void);
    bool VariableExists(char ch);
    bool AddVariable(char ch);
    double GetVariableValue(char ch);
    bool ProcessOperators(std::vector<double> &vOperand, std::vector<char> &vOperator);

    static bool IsOperator(char cToken);

    std::string sExpression;
    std::vector<CVariable> vVariable;
};

#endif // !defined(EVALUATOR_H_INCLUDED_)
