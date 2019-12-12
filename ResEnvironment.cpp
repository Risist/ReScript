#include "ResEnvironment.h"
#include "../Math/Math.h"
#include "ResDataScriptLoader.h"

namespace Res
{
	void Environment::runIntrerpreter(bool * end)
	{
		while (end)
		{
			std::cout << "input>>\t";
			std::string s;
			std::getline(std::cin, s, '\n');
			auto expr = buildExpressionTree(s.c_str());
			if (expr)
			{
				std::cout << "value>>\t" << expr->evaluate() << "\n\n";
				delete expr;
			}
		}
	}

	void Environment::runTokenTest(bool * end)
	{
		while (end)
		{
			std::cout << "input>>\t";
			std::string s;
			std::getline(std::cin, s, '\n');
			std::vector<Token_t> tokens;
			tokenize(tokens, s.c_str());
			for (auto it : tokens)
				std::cout << "\ttoken>> \"" << it << "\"\n";
		}
	}

	void Environment::runOnpTest(bool * end)
	{
		while (end)
		{
			std::cout << "input>>\t";
			std::string s;
			std::getline(std::cin, s, '\n');
			std::vector<Token_t> tokens;
			convertToOnp(tokens, s.c_str());
			for (auto it : tokens)
				std::cout << "\ttoken>> \"" << it << "\"\n";
		}
	}

	void Environment::runLoaderInterpreter(bool * end)
	{
		Res::DataScriptLoader loader;
		while (*end)
		{
			loader.nextLine(cin);

			for (auto it : loader.getParams())
			{
				auto expr = buildExpressionTree(loader.loadRaw(it.first).c_str());
				if (expr)
					cout << ">>" << it.first << " : " << expr->evaluate() << endl;
			}
		}
	}


	static Expression* argumentError(std::stack<Expression*>& expressions, const Token_t& token, int n, int nRequired)
	{
		std::cerr << ">>Error: not enough arguments for token: \"" << token << "\"\n";
		std::cerr << "\t>> given: " << n << " arguments but required: " << nRequired << "\n";
		while (!expressions.empty())
		{
			delete expressions.top();
			expressions.pop();
		}
		return nullptr;
	}
	Expression * Environment::buildExpressionTree(const char * text)
	{
		std::vector<Token_t> tokens;
		std::stack<Expression*> expressions;

		convertToOnp(tokens, text);

		auto itEnd = tokens.end();
		for (auto itToken = tokens.begin(); itToken != itEnd; ++itToken)
		{

			auto itTokenString = *itToken;

			VoidFunctionData::const_iterator	fItVoid = voidFunctions.find(itTokenString);
			if (fItVoid != voidFunctions.end())
			{
				expressions.push(new ExpressionVoidLambda(fItVoid->second));
				continue;
			}

			UnaryFunctionData::const_iterator	fItUnary = unaryFunctions.find(itTokenString);
			if (fItUnary != unaryFunctions.end())
			{
				if (expressions.empty())
					return argumentError(expressions, itTokenString, 0, 1);
				auto arg = expressions.top();
				expressions.pop();
				expressions.push(new ExpressionUnaryLambda(fItUnary->second, arg));
				continue;
			}

			BinaryOperatorData::const_iterator fitBinaryOp = binaryOperators.find(itTokenString);
			if (fitBinaryOp != binaryOperators.end())
			{
				if (expressions.empty())
					return argumentError(expressions, itTokenString, 0, 2);
				auto arg = expressions.top();
				expressions.pop();
				if (expressions.empty())
					return argumentError(expressions, itTokenString, 1, 2);
				auto arg2 = expressions.top();
				expressions.pop();

				expressions.push(new ExpressionBinaryLambda(fitBinaryOp->second.function, arg2, arg));
				continue;
			}

			TrenaryFunctionData::const_iterator fitTrenaryOp = trenaryFunctions.find(itTokenString);
			if (fitTrenaryOp != trenaryFunctions.end())
			{
				if (expressions.empty())
					return argumentError(expressions, itTokenString, 0, 3);
				auto arg = expressions.top();
				expressions.pop();
				if (expressions.empty())
					return argumentError(expressions, itTokenString, 1, 3);
				auto arg2 = expressions.top();
				expressions.pop();
				if (expressions.empty())
					return argumentError(expressions, itTokenString, 2, 3);
				auto arg3 = expressions.top();
				expressions.pop();

				expressions.push(new ExpressionTrenaryLambda(fitTrenaryOp->second, arg3, arg2, arg));
				continue;
			}

			if ((*itToken)[0] == '=')
			{
				if (expressions.empty())
					return argumentError(expressions, itTokenString, 0, 1);
				auto arg = expressions.top();
				expressions.pop();
				addConstant(std::string(itToken->begin() + 1, itToken->end()), arg->evaluate());
				delete arg;
				continue;
			}
			
			auto it = floatVarriables.find(itTokenString);
			if ( it != floatVarriables.end())
			{
				expressions.push(new ExpressionVarriable(it->second));
				continue;
			}

			auto itConstant = floatConstants.find(itTokenString);
			if (itConstant != floatConstants.end())
			{
				expressions.push(new ExpressionConstant(itConstant->second));
				continue;
			}
			
			if(isConstant(itTokenString.c_str()))
			{
				std::istringstream stream(itTokenString);
				float f; stream >> f;
				expressions.push(new ExpressionConstant(f));
				continue;
			}

			std::cerr << ">>Error: unknown token: \"" << itTokenString << "\"\n";
			
			while (!expressions.empty())
			{
				delete expressions.top();
				expressions.pop();
			}
			return nullptr;
		}

		if (expressions.empty())
			return nullptr;

		auto expr = expressions.top();
		expressions.pop();
		if (!expressions.empty())
		{
			std::cerr << ">>Error: unconsumed expressions on stack \n\t>> at execution of >> Environment::buildExpressionTree\n";
			do
			{
				delete expressions.top();
				expressions.pop();
			} while (!expressions.empty());
		}
		return expr;
	}
	
	void Environment::convertToOnp(std::vector<Token_t>& tokens_out, const char * text) const
	{
		std::vector<Token_t> tokens;
		tokenize(tokens, text);
		reduceTokensEqual(tokens);
		reduceTokensMinus(tokens);

		struct OperatorInfo
		{
			Token_t token;
			Priority_t priority;
		};
		std::stack<OperatorInfo> operators;

		auto itEnd = tokens.end();
		for (auto itToken = tokens.begin(); itToken != itEnd; ++itToken)
		{
			auto itTokenString = *itToken;
			char c = (itTokenString)[0];
			if (itTokenString == "(")
			{
				operators.push({ "(", 0});
				continue;
			}
			if (c == ')' || isComma(c))
			{
				while (operators.top().token != "(")
				{
					tokens_out.push_back(operators.top().token);
					operators.pop();
				}
				if (c == ')')
					operators.pop();
				continue;
			}
			
			Priority_t p = 1000;
			BinaryOperatorData::const_iterator	fitBinaryOp = binaryOperators.find(itTokenString);
			if (fitBinaryOp != binaryOperators.end())
				p = fitBinaryOp->second.priority;
			
			if(
				fitBinaryOp != binaryOperators.end()							||
				unaryFunctions.find(itTokenString) != unaryFunctions.end()		||
				voidFunctions.find(itTokenString) != voidFunctions.end()		||
				trenaryFunctions.find(itTokenString) != trenaryFunctions.end()  
				)
			{
				while (!operators.empty() && operators.top().token != "(" && operators.top().priority >= p)
				{
					tokens_out.push_back(operators.top().token);
					operators.pop();
				}

				operators.push({ itTokenString, p});
				continue;
			}
			
			if ((*itToken)[0] == '=')
			{
				operators.push({ itTokenString, -1 });
				continue;
			}

			{
				tokens_out.push_back(itTokenString);
			}

		}

		while (!operators.empty())
		{
			tokens_out.push_back(operators.top().token);
			operators.pop();
		}
	}

	void Environment::reduceTokensMinus(std::vector<Token_t>& tokens) const
	{
		if (tokens.front() == "-" && isConstant(tokens[1].c_str()) && tokens[1][0] != '-')
		{
			tokens.erase(tokens.begin());
			tokens.front() = "-" + tokens.front();
		}
		for (size_t i = 1; i + 1 < tokens.size(); ++i)
		{
			auto it = tokens[i];
			if (it == "-" && isConstant(tokens[i + 1].c_str()) && tokens[i + 1][0] != '-')
			{
				if (isBinaryOperator(tokens[i - 1].c_str()) || tokens[i - 1] == "(" || tokens[i - 1] == ",")
				{
					tokens[i + 1] = "-" + tokens[i + 1];
					tokens.erase(tokens.begin() + i);
					--i;
				}
			}

		}
	}

	void Environment::reduceTokensEqual(std::vector<Token_t>& tokens) const
	{
		for (size_t i = 0; i + 1 < tokens.size(); ++i)
		{
			if (tokens[i] == "=" && i + 1 < tokens.size() && tokens[i + 1] == "=" )
			{
				tokens.erase(tokens.begin() + i);
				tokens[i] = "==";
			}
		}

		if (tokens.size() < 2)
			return;


		if (tokens[1] == "=")
		{
			tokens[0] = "=" + tokens[0];
			tokens.erase(tokens.begin() + 1);
		}
		else if (tokens[2] == "=")
		{
			tokens.erase(tokens.begin() + 2);
			tokens.insert(tokens.begin() + 1, tokens[0]);
			tokens[0] = "=" + tokens[0];
		}
	}

	bool Environment::isBinaryOperator(const Token_t & token) const
	{
		return binaryOperators.find(token) != binaryOperators.end();
	}

	void Environment::loadMathLibrary()
	{
		addFunction("sin", [](float x) {return sin(x*0.0055555555555555556f*M_PI); });
		addFunction("cos", [](float x) {return cos(x*0.0055555555555555556f*M_PI); });
		addFunction("tan", [](float x) {return tan(x*0.0055555555555555556f*M_PI); });
		
		addFunction("asin", [](float x) {return asin(x)*0.0055555555555555556f*M_PI; });
		addFunction("acos", [](float x) {return acos(x)*0.0055555555555555556f*M_PI; });
		addFunction("atan", [](float x) {return atan(x)*0.0055555555555555556f*M_PI; });


		addFunction("normaliseAngle", [](float x) {
			if (x > 180.f)
				return x - 360.f;
			else if (x < 180.f)
				return x + 360.f;
			return x;
		});
		addFunction("isNormalisedAngle", [](float x) {
			return x <= 180.f && x >= -180.f;
		});


		addFunction("average", [](float x, float y) {return (x + y) *0.5f; });
		addFunction("sqrt", (float(*)(float))sqrt );
		addFunction("randRange", [](float min, float max) { return Math::randRange(min, max); });
		addFunction("randValue", []() {return randRange(0.0f, 1.0f); });
		addFunction("min", [](float x, float y) {return Math::min(x, y);});
		addFunction("max", [](float x, float y) {return Math::max(x,y); });
		addFunction("clamp", [](float v, float x, float y) {return Math::clamp(v, x, y); });
		addFunction("lerp", [](float v, float x, float y) {return Math::lerp(v, x, y); });
		addFunction("exp", (float(*)(float))exp );
		addFunction("vLength", [](float x, float y, float z) { return sqrt(x*x + y * y + z * z); });
		addFunction("vLengthSq", [](float x, float y, float z) { return x*x + y * y + z * z; });

		addConstant("e", (float)exp(1.0f));
		addConstant("pi", (float)M_PI);
	}
	void Environment::loadBoolLibrary()
	{
		// todo change semantic from fervent to	lazy one
		addFunction("if", [](float b, float t, float f) {return (int)b ? t : f; });

		addFunction("and", [](float lhs, float rhs) {return (int)lhs && (int)rhs; }, 1);
		addFunction("or", [](float lhs, float rhs) {return (int)lhs || (int)rhs; }, 1);
		addFunction("!", [](float lhs) {return !(int)lhs;	});
		addFunction("==", [](float lhs, float rhs) {return abs(lhs- rhs)*abs(lhs - rhs) < std::numeric_limits<float>::min(); }, 10);
		addFunction(">", [](float lhs, float rhs) {return lhs >  rhs; }, 10);
		addFunction(">=", [](float lhs, float rhs) {return lhs >= rhs; }, 10);
		addFunction("<", [](float lhs, float rhs) {return lhs <  rhs; }, 10);
		addFunction("<=", [](float lhs, float rhs) {return lhs <= rhs; }, 10);

		addConstant("true", 1.0f);
		addConstant("false", 0.0f);
		addFunction("toBool", [](float lhs) {return (bool)lhs;	});
	}
	void Environment::loadBasicOperators()
	{
		addFunction("+", [](float lhs, float rhs) {return lhs + rhs; }, 1);
		addFunction("-", [](float lhs, float rhs) {return lhs - rhs; }, 1);
		addFunction("/", [](float lhs, float rhs) {return lhs / rhs; }, 2);
		addFunction("*", [](float lhs, float rhs) {return lhs * rhs; }, 2);
		addFunction("^", [](float lhs, float rhs) {return pow(lhs,rhs); }, 3);
	}
}
