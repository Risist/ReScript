#include "ResScripts.h"

int main()
{
	Res::Environment env;

	env.loadMathLibrary();
	env.loadBasicOperators();
	env.loadBoolLibrary();

	bool exit = true;

	env.runIntrerpreter(&exit);
	//env.runTokenTest(&exit);
	//env.runOnpTest(&exit);
	//env.runLoaderInterpreter(&exit);
	system("pause");
}