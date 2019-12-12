#pragma once
#include <functional>
#include <vector>

namespace Res
{

	// sizeof: 8 + 40 bytes
	class RuntimeExpression
	{
	public:
		typedef std::vector<RuntimeExpression> Record;

		RuntimeExpression() {}
		~RuntimeExpression() {}
		float(*evaluate)( const Record::iterator& it);

		// sizeof : 4/8 bytes
		union
		{
			float										_constant;
			float *										_varriable;
			float(*_function)(const Record::iterator & it);
		};

		static float evaluateConst(		const Record::iterator& it);
		static float evaluateVarriable( const Record::iterator& it);
		static float evaluateFunction(		const Record::iterator& it);
	};
}/**/