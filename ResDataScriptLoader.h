/// Code from ReEngine library
/// all rights belongs to Risist (Maciej Dominiak) (risistt@gmail.com)

#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include "ResEnvironment.h"

using namespace std;

/* simple class for mantaining loading data from .res files
Data scripts are used to save & load constant data to files
No logic or any if statements should be included
workflow should look like that:
//open file
ifstream file(path);
ScriptLoader loader;
loader.nextLine(file);
int value1 = loader.load("trait1", 1);
float value2 = loader.load("trait2", 2.f);
string value3 = loader.load("trait3", "def_str");
MyClass value3 = loader.load("trait3", myClassObject); // assume there is defined << operator for MyClass
file.close();
*/

namespace Res
{
	class DataScriptLoader
	{
	public:

		void nextLine(std::istream& stream)
		{
			_traits.clear();
			/// load data to _line
			while (stream.get() != '<');
			getline(stream, _line, '>');

			/// fill _traits vector
			auto itBegin = _line.begin();
			auto itEqual = _line.begin();
			while (true)
			{
				itEqual = itBegin;
				/// skip all signs to find '='
				while (true)
				{
					if (itEqual == _line.end())
						return;
					if (*itEqual == '=')
						break;
					++itEqual;
				}
				string trait, value;
				{/// find trait name
					stringstream _stream(string(itBegin, itEqual));
					_stream >> trait;
				}
				{
					itBegin = itEqual;
					// find end of the value (as well as the new beggining)
					while (true)
					{
						if (itBegin == _line.end() || *itBegin == ';')
							break;
						++itBegin;
					}
					_traits[trait] = string(itEqual + 1, itBegin);
					if (itBegin == _line.end())
						return;
					++itBegin;
				}

			}
		}

		// loads whole statement equal to traits
		// in .res file it could be written as
		// <trait1 = value1, trait2 = value2 dfsfsafs, trait3 = value3, trait4 = value4>
		// the result for call loadRaw("trait2") will be
		// " value2 dfsfsafs"
		// for loadRaw("traitUnexisting")
		// "_undefined"
		// and so on
		std::string loadRaw(const std::string& trait, const std::string& defaultValue = "_undefined") const
		{
			auto it = _traits.find(trait);
			if (it != _traits.end())
			{
				return it->second;
			}
			else
				return defaultValue;
		}
		// loads statement equal to traits
		// in a way similar to standard stream clases
		// the loaded type must have overloaded operator <<
		// in .res file it could be written as
		// <trait1 = value1, trait2 = value2 dfsfsafs, trait3 = value3, trait4 = value4>
		// the result for call load("trait2", someDefaultValue) will be
		// "value2" not " value2 dfsfsafs"like in raw wersion
		// for load("traitUnexisting", someDefaultValue)
		// someDefaultValue
		// and so on
		template<typename Ty>
		Ty load(const std::string& trait, const Ty& defaultValue) const
		{
			std::string s = loadRaw(trait, "_undefined");

			Ty r;
			if (s != "_undefined")
			{
				std::stringstream stream(s);
				stream >> r;
			}
			else
				r = defaultValue;
			return r;
		}
		float load(const std::string& trait, float defaultValue, Environment& environment)
		{
			std::string s = loadRaw(trait, "_undefined");

			
			if (s != "_undefined")
			{
				auto expr = environment.buildExpressionTree(s.c_str());
				if (expr)
					return expr->evaluate();
				else
				{
					std::cerr << ">>Error : failed to create expression from string \"" << s << "\"\n";
					std::cerr << "\t >>trait = \"" << trait << "\"\n";
					return 0;
				}
			}
			else
				return defaultValue;
		}
		Expression* load(const std::string& trait, Environment& environment)
		{
			std::string s = loadRaw(trait, "_undefined");

			if (s != "_undefined")
				return  environment.buildExpressionTree(s.c_str());
			
			return nullptr;
		}

		const std::string& getLine() const
		{
			return _line;
		}

		std::map<std::string, std::string> getParams() { return _traits; }

	protected:
		/// map which contains key and a string describing the trait
		std::map<std::string, std::string> _traits;
		std::string _line;
	};
}