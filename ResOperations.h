#pragma once
#include "ResToken.h"

namespace Res
{
	/*////// Functions //////////////////////////

	
	typedef std::map<Token_t, std::function<float()>>				VoidFunctionData;
	typedef std::map<Token_t, std::function<float(float)>>			UnaryFunctionData;
	typedef std::map<Token_t, std::function<float(float,float)>>	BinaryFunctionData;

	////// Operators ////////////////////////////////////////

	typedef int Priority_t;
	Priority_t getPriority(const std::string& c)
	{
		if (c == "+" || c == "-")
			return 1;
		else if (c == "*" || c == "/" || c == "%")
			return 2;
		else if (c == "^")
			return 3;

		return 1000;
	}
	Expression* createOperatorExpressionBinary(const std::string& op, Expression* arg1, Expression* arg2)
	{
		if (op == "+")
			return new ExpressionBinaryPlus(arg1, arg2);
		if (op == "-")
			return new ExpressionBinaryMinus(arg1, arg2);
		if (op == "*")
			return new ExpressionBinaryMltp(arg1, arg2);
		if (op == "/")
			return new ExpressionBinaryDiv(arg1, arg2);
		//if (op == "%")
		//	return new ExpressionBinaryMod(arg1, arg2);
		if (op == "^")
			return new ExpressionBinaryPow(arg1, arg2);
	}
	Expression* createOperatorExpressionUnary(const char* op, Expression* arg)
	{
		if (op == "-")
			return new ExpressionUnaryMinus(arg);
	}



	/////////////////////////////////////////////////////////

	void convertToOnp(std::vector<Token_t>& tokens_out, const char* text,
		const VoidFunctionData& voidFunctions,
		const UnaryFunctionData& unaryFunctions,
		const BinaryFunctionData& binaryFunctions)
	{
		std::vector<Token_t> tokens;
		tokenize(tokens, text);

		std::stack<Token_t> operators;
		std::stack<Token_t> unaryOperators;

		auto itEnd = tokens.end();
		for (auto itToken = tokens.begin(); itToken != itEnd; ++itToken)
		{

			char c = (*itToken)[0];
			if (*itToken == "(")
			{
				operators.push("(");
			}
			else if (c == ')' || isComma(c) )
			{
				while (operators.top() != "(")
				{
					tokens_out.push_back(operators.top());
					operators.pop();
				}
				if(c == ')')
					operators.pop();
			}
			else if (isOperator(*itToken) ||
				voidFunctions.find(*itToken) != voidFunctions.end() ||
				unaryFunctions.find(*itToken) != unaryFunctions.end() ||
				binaryFunctions.find(*itToken) != binaryFunctions.end()
				)
			{
				Priority_t p = getPriority(*itToken);
				while (!operators.empty() && operators.top() != "(" && getPriority(operators.top()) >= p)
				{
					tokens_out.push_back(operators.top());
					operators.pop();
				}

				operators.push(*itToken);
			}
			else
			{
				tokens_out.push_back(*itToken);
			}

		}

		while (!operators.empty())
		{
			tokens_out.push_back(operators.top());
			operators.pop();
		}
	}

	Expression* buildExpressionTree(const char* text, 
		const VoidFunctionData& voidFunctions,
		const UnaryFunctionData& unaryFunctions,
		const BinaryFunctionData& binaryFunctions)
	{
		std::vector<Token_t> tokens;
		std::stack<Expression*> expressions;

		convertToOnp(tokens, text, voidFunctions, unaryFunctions, binaryFunctions);

		auto itEnd = tokens.end();
		for (auto itToken = tokens.begin(); itToken != itEnd; ++itToken)
		{

			VoidFunctionData::const_iterator	fItVoid = voidFunctions.find(*itToken);
			UnaryFunctionData::const_iterator	fItUnary = unaryFunctions.find(*itToken);
			BinaryFunctionData::const_iterator	fItBinary = binaryFunctions.find(*itToken);
			if ( fItVoid != voidFunctions.end() )
			{
				expressions.push(new ExpressionVoidLambda( fItVoid->second, *itToken));
			}else if (fItUnary != unaryFunctions.end())
			{
				auto arg = expressions.top();
				expressions.pop();
				expressions.push(new ExpressionUnaryLambda(fItUnary->second, *itToken, arg));
			}
			else if (fItBinary != binaryFunctions.end())
			{
				auto arg = expressions.top();
				expressions.pop();
				auto arg2 = expressions.top();
				expressions.pop();

				expressions.push(new ExpressionBinaryLambda(fItBinary->second, *itToken, arg2, arg));
			}
			else if (isOperator(*itToken))
			{
				auto a1 = expressions.top();
				expressions.pop();
				auto a2 = expressions.top();
				expressions.pop();

				expressions.push(createOperatorExpressionBinary(*itToken, a2, a1));
			}
			else //if (isConstant(*itToken))
			{
				std::istringstream stream(*itToken);
				float f; stream >> f;
				expressions.push(new ExpressionConstant(f));
			}
		}

		return expressions.top();
	}*/
}