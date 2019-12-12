#pragma once
#include <iostream>
#include <cinttypes>
#include <string>
#include <sstream>
#include <map>
#include <assert.h>
#include <memory>
#include <functional>
#define _USE_MATH_DEFINES
#include <math.h>

#define ExpressionAssert( a ) assert(a) 


namespace Res
{
	class Expression
	{
	public:
		virtual float			evaluate()	= 0;
	};

	class ExpressionConstant : public Expression
	{
	public:
		ExpressionConstant(float value) : _value(value) {}
		float			evaluate()	override { return _value; }

	private:
		float _value;
	};

	class ExpressionVarriable : public Expression
	{
	public:
		ExpressionVarriable(float * recordValue) : _recordValue(recordValue) {}
		float			evaluate()	override 
		{ 
			ExpressionAssert(_recordValue);
			return *_recordValue;
		}

	private:
		float * _recordValue;
	};

	




	//////////////
	class ExpressionVoidLambda : public Expression
	{
	public:
		ExpressionVoidLambda(std::function<float()> operation)
			: _operation(operation) {}

		float			evaluate()	override { return _operation(); }

	private:
		std::function<float()> _operation;
	};
	class ExpressionUnaryLambda : public Expression
	{
	public:
		ExpressionUnaryLambda(std::function<float(float)> operation, Expression* arg)
			: _operation(operation), _arg(arg) {}

		float			evaluate()	override 
		{
			ExpressionAssert(_arg);
			return _operation(_arg->evaluate() ); 
		}

	private:
		std::function<float(float)> _operation;
		std::unique_ptr<Expression> _arg;
	};

	class ExpressionBinaryLambda : public Expression
	{
	public:
		ExpressionBinaryLambda(std::function<float(float, float)> operation, Expression *arg1, Expression *arg2)
			: _arg1(arg1), _arg2(arg2), _operation(operation) {}

		float			evaluate()	override
		{
			ExpressionAssert(_arg1 && _arg2);
			return _operation(_arg1->evaluate(), _arg2->evaluate());
		}

	private:
		std::function<float(float, float)> _operation;

		std::unique_ptr<Expression> _arg1;
		std::unique_ptr<Expression> _arg2;
	};

	class ExpressionTrenaryLambda : public Expression
	{
	public:
		ExpressionTrenaryLambda(std::function<float(float, float,float)> operation, Expression *arg1, Expression *arg2, Expression *arg3)
			: _arg1(arg1), _arg2(arg2), _arg3(arg3), _operation(operation){}

		float			evaluate()	override
		{
			ExpressionAssert(_arg1 && _arg2 && _arg3);
			return _operation(_arg1->evaluate(), _arg2->evaluate(), _arg3->evaluate());
		}

	private:
		std::function<float(float, float, float)> _operation;

		std::unique_ptr<Expression> _arg1;
		std::unique_ptr<Expression> _arg2;
		std::unique_ptr<Expression> _arg3;
	};	
}