#include "ResRuntime.h"
float Res::RuntimeExpression::evaluateConst(const Record::iterator & it)
{
	return it->_constant;
}
float Res::RuntimeExpression::evaluateVarriable( const Record::iterator & it)
{
	return *it->_varriable;
}

float Res::RuntimeExpression::evaluateFunction(const Record::iterator & it)
{
	return it->_function(it);
}