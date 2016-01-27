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


// Evaluates an integer arithmetic expression
// Precondition: infix is an infix integer arithmetic
//   expression consisting of single lower case letter operands,
//   parentheses, and the operators +, -, *, /, with embedded blanks
//   allowed for readability.
// Postcondition: If infix is a syntactically valid infix integer
//   expression whose only operands are single lower case letters
//   (whether or not they appear in the values map), then postfix is
//   set to the postfix form of the expression; otherwise postfix may
//   or may not be changed, result is unchanged, and the function
//   returns 1.  If infix is syntactically valid but contains at
//   least one lower case letter operand that does not appear in the
//   values map, then result is unchanged and the function returns 2.
//   If infix is syntactically valid and all its lower case operand
//   letters appear in the values map, then if evaluating the
//   expression (using for each letter in the expression the value in
//   the map that corresponds to it) attempts to divide by zero, then
//   result is unchanged and the function returns 3; otherwise,
//   result is set to the value of the expression and the function
//   returns 0.
int evaluate(string infix, const Map& values, string& postfix, int& result)
{
  if(!isValid(infix))
    return 1;
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
	  while(!s.empty() && s.top() != '(' && precedent(infix[i],s.top()))
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
  cout << postfix << endl;
  stack<int> op;
  for(int i = 0; i < postfix.size(); i++)
    {
      if(isOperator(postfix[i]))
	{
	  int v2 = op.top();
	  op.pop();
	  int v1 = op.top();
	  op.pop();
	  cout << v1 << " " << postfix[i] << " " << v2 << endl;
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
	  if(!values.get(postfix[i],v)) return 1;
	  op.push(v);
	}
    }
  result = op.top();
  return 0;
}


bool isOperator(char c)
{
  return c == '*' || c == '/' || c == '+' || c == '-';
}

bool isValid(string infix)
{
  int operatorCount = 0;
  int operandCount = 0;
  for(int i = 0; i < infix.size(); i++)
    {
      if(tolower(infix[i]) != infix[i]) return false;
      if(isOperator(infix[i])) operatorCount++;
      else if(isalpha(infix[i])) operandCount++;
      else if(infix[i] != '(' && infix[i] != ')' && infix[i] != ' ')
	return false;
    }
  if(operandCount <= operatorCount || operatorCount == 0)
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

int main()
{
  char vars[] = { 'a', 'e', 'i', 'o', 'u', 'y', '#' };
  int  vals[] = {  3,  -9,   6,   2,   4,   1  };
  Map m;
  for (int k = 0; vars[k] != '#'; k++)
    m.insert(vars[k], vals[k]);
  string pf;
  int answer;
  assert(evaluate("a+ e", m, pf, answer) == 0  &&
	 pf == "ae+"  &&  answer == -6);
  answer = 999;
  assert(evaluate("", m, pf, answer) == 1  &&  answer == 999);
  cout << "bye" << endl;
  assert(evaluate("a+", m, pf, answer) == 1  &&  answer == 999);
  assert(evaluate("a i", m, pf, answer) == 1  &&  answer == 999);
  assert(evaluate("ai", m, pf, answer) == 1  &&  answer == 999);
  assert(evaluate("()", m, pf, answer) == 1  &&  answer == 999);
  assert(evaluate("y(o+u)", m, pf, answer) == 1  &&  answer == 999);
  assert(evaluate("a+E", m, pf, answer) == 1  &&  answer == 999);
  assert(evaluate("(a+(i-o)", m, pf, answer) == 1  &&  answer == 999);
  cout << "hi" << endl;
  // unary operators not allowed:
  assert(evaluate("-a", m, pf, answer) == 1  &&  answer == 999);
  assert(evaluate("a*b", m, pf, answer) == 2  &&
	 pf == "ab*"  &&  answer == 999);
  assert(evaluate("y +o *(   a-u)  ", m, pf, answer) == 0  &&
	 pf == "yoau-*+"  &&  answer == -1);
  answer = 999;
  assert(evaluate("o/(y-y)", m, pf, answer) == 3  &&
	 pf == "oyy-/"  &&  answer == 999);
  assert(evaluate(" a  ", m, pf, answer) == 0  &&
	 pf == "a"  &&  answer == 3);
  assert(evaluate("((a))", m, pf, answer) == 0  &&
	 pf == "a"  &&  answer == 3);
  cout << "Passed all tests" << endl;
}
