/// Code from ReEngine library
/// all rights belongs to Risist (Maciej Dominiak) (risistt@gmail.com)

#pragma once
// module include file


#include "ResEnvironment.h"

// Data scripts are used to save & load constant data to files
// No logic or any if statements should be included
#include "ResDataScriptLoader.h"
//#include <Re\Common\Scripts\ResDataScriptSaver.h>


// ISerialisable interface
#include "ResISerialisable.h"





/*support for multi line statements like:
<initial >
	< sub line 1 >
	< sub line 2 >
	// ect
<\>
*/
#define DATA_SCRIPT_MULTILINE(file, loader) \
	while( loader.nextLine(file), loader.getLine() != "\\" )


/* DATA_SCRIPT_MULTILINE save version
*/

#define DATA_SCRIPT_MULTILINE_SAVE(file, saver, b)	\
	while(saver.nextLine(file) , b);					\
	saver.setEndLine();									\
/*
small example of how to use this
brackets looks strange, but probably there is no other way

void load( ostream& file, DataScriptSaver& saver)
{
	list<int> v;
	for(auto it : v){
		saver.save("vi", it);
	DATA_SCRIPT_MULTILINE_SAVE(file, saver)
}
*/



/*
to allow serialisation of composite types you need to declare serialisation function

usage:

in base class declaration use macro:
	MULTI_SERIALISATION_INTERFACE(BaseClassName) data needed for serialisation

in all inheriting classes from base put
	SERIALISATION_NAME(ClassName)

in cpp of base class
MULTI_SERIALISATION_INTERFACE_IMPL(Base)
{
	MULTI_SERIALISATION_INTERFACE_CHECK(ClassA);
	else MULTI_SERIALISATION_INTERFACE_CHECK(ClassB);
	else MULTI_SERIALISATION_INTERFACE_CHECK(ClassC);
	else MULTI_SERIALISATION_INTERFACE_CHECK(ClassD);
	///...
	else return nullptr;
}

where ClassA, ClassB ect. are classes which inherits from base
*/

#define SERIALISATION_NAME(ClassName) \
	public:	\
		virtual const char* getSerialisationName() const {return #ClassName; }

#define MULTI_SERIALISATION_INTERFACE(BaseClassName) \
	static std::function<BaseClassName*(const std::string& )> creationFunction; \
	static BaseClassName* creationFunction_default(const std::string& name);	\
	SERIALISATION_NAME(BaseClassName)




#define MULTI_SERIALISATION_INTERFACE_CHECK(ClassToCheck) \
if ( name == #ClassToCheck) return new ClassToCheck

#define MULTI_SERIALISATION_INTERFACE_IMPL(BaseClassName) \
	std::function<BaseClassName*(const std::string& name)> BaseClassName :: creationFunction = BaseClassName :: creationFunction_default; \
	BaseClassName* BaseClassName :: creationFunction_default(const std::string& name )
