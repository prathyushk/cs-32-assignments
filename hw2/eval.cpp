#include "Map.h"
#include <iostream>
#include <string>
#include <stack>
#include <cctype>
#include <cassert>
using namespace std;

bool precedent(char c1, char c2);
bool isValid(string infix);
bool isOperator(char c);
void toPostfix(string infix, string& postfix);

int evaluate(string infix, const Map& values, string& postfix, int& result)
{
  if(!isValid(infix))
    return 1;
  toPostfix(infix,postfix);
  stack<int> op;
  for(int i = 0; i < postfix.size(); i++)
    {
      if(isOperator(postfix[i]))
	{
	  int v2 = op.top();
	  op.pop();
	  int v1 = op.top();
	  op.pop();
	  int val = 0;
	  switch(postfix[i])
	    {
	    case '*':
	      val = v1 * v2;
	      break;
	    case '/':
	      if(v2 == 0) return 3;
	      val = v1 / v2;
	      break;
	    case '+':
	      val = v1 + v2;
	      break;
	    case '-':
	      val = v1 - v2;
	      break;
	    }
	  op.push(val);
	}
      else
	{
	  int v;
	  if(!values.get(postfix[i],v)) return 2;
	  op.push(v);
	}
    }
  result = op.top();
  return 0;
}

void toPostfix(string infix, string& postfix)
{
  postfix = "";
  stack<char> s;
  for(int i = 0; i < infix.size(); i++)
    {
      switch(infix[i])
	{
	case '(':
	  s.push(infix[i]);
	  break;
	case ')':
	  while(s.top() != '(')
	    {
	      postfix += s.top();
	      s.pop();
	    }
	  s.pop();
	  break;
	case '*':
	case '/':
	case '+':
	case '-':
	  while(!s.empty() && s.top() != '(' && !precedent(infix[i],s.top()))
	    {
	      postfix += s.top();
	      s.pop();
	    }
	  s.push(infix[i]);
	  break;
	case ' ':
	  break;
	default:
	  postfix += infix[i];
	  break;
	}
    }
  while(!s.empty())
    {
      postfix += s.top();
      s.pop();
    }
}

bool isOperator(char c)
{
  return c == '*' || c == '/' || c == '+' || c == '-';
}

bool isValid(string infix)
{
  int operatorCount = 0;
  int operandCount = 0, openParen = 0, closeParen = 0;
  for(int i = 0; i < infix.size(); i++)
    {
      if(tolower(infix[i]) != infix[i]) return false;
      if(isOperator(infix[i])) operatorCount++;
      else if(isalpha(infix[i])) operandCount++;
      else if(infix[i] == '(') openParen++;
      else if(infix[i] == ')') closeParen++;
      else if(infix[i] != ' ')
	return false;
    }
  if(operatorCount != operandCount - 1 || openParen != closeParen)
    return false;
  return true;
}

bool precedent(char c1, char c2)
{
  if(!isOperator(c2))
    return true;
  if(c1 == '*' || c1 == '/')
    return true;
  if((c1 == '+' || c1 == '-') && (c2 == '*' || c2 == '/'))
    return false;
  else
    return true;
}
