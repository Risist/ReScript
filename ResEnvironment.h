#pragma once
#include <map>
#include <vector>
#include <string>
//#include <map>

#include "ResToken.h"


namespace Res
{

	class Environment
	{
		typedef int Priority_t;

		typedef float*									Varriable;
		typedef float									Constant;
		typedef std::function<float()>					VoidFunction;
		typedef std::function<float(float)>				UnaryFunction;
		typedef std::function<float(float,float)>		BinaryFunction;
		typedef std::function<float(float,float,float)>	TrenaryFunction;

		typedef std::map<Token_t, Varriable>			VarriableData;
		typedef std::map<Token_t, Constant>				ConstantData;
		typedef std::map<Token_t, VoidFunction>			VoidFunctionData;
		typedef std::map<Token_t, UnaryFunction>		UnaryFunctionData;
		typedef std::map<Token_t, TrenaryFunction>		TrenaryFunctionData;

		/// operator to place in middle of its arguments
		struct BinaryOperator
		{
			BinaryFunction function;
			Priority_t priority;
		};
		typedef std::map<Token_t, BinaryOperator>		BinaryOperatorData;
		

	public:
		


		void runIntrerpreter(bool *end);
		void runTokenTest(bool *end);
		void runOnpTest(bool *end);
		void runLoaderInterpreter(bool *end);

		Expression * buildExpressionTree(const char* text);

		inline void addVarriable(const Token_t& name, Varriable value)
		{
			floatVarriables[name] = value;
		}
		inline void addConstant(const Token_t& name, float value)
		{
			floatConstants[name] = value;
		}


		inline void addFunction	(const Token_t& name, const VoidFunction& f)
		{
			voidFunctions[name] = f;
		}
		inline void addFunction	(const Token_t& name, const UnaryFunction& f)
		{
			unaryFunctions[name] = f;
		}
		inline void addFunction	(const Token_t& name, const BinaryFunction& f, Priority_t priority = 1000)
		{
			binaryOperators[name] = { f, priority };
		}
		inline void addFunction(const Token_t& name, const TrenaryFunction& f)
		{
			trenaryFunctions[name] = f;
		}

		inline float getValueConstant(const Token_t& name) { return floatConstants[name]; }


		void loadMathLibrary();
		void loadBoolLibrary();
		void loadBasicOperators();

	private:
		void convertToOnp(std::vector<Token_t>& tokens_out, const char* text) const;
		void reduceTokensMinus(std::vector<Token_t>& tokens) const;
		void reduceTokensEqual(std::vector<Token_t>& tokens) const;
		bool isBinaryOperator(const Token_t& token) const;

	private:
		VarriableData		floatVarriables;
		ConstantData		floatConstants;
		VoidFunctionData	voidFunctions;
		UnaryFunctionData	unaryFunctions;
		TrenaryFunctionData	trenaryFunctions;
		BinaryOperatorData	binaryOperators;
	};


}