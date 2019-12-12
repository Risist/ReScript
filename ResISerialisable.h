/// Code from ReEngine library
/// all rights belongs to Risist (Maciej Dominiak) (risistt@gmail.com)

#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "ResDataScriptLoader.h"
//#include <Re\Common\Scripts\ResDataScriptSaver.h>


/*
interface that unify all objects that need to be saved/ loaded
*/
namespace Res
{
	class ISerialisable
	{
	public:

		// opens a file from path. returns true at succes
		/*bool serialise(const std::string& path) const
		{
			std::ofstream file;
			file.open(path);

			if (file)
			{
				serialise(file);

				file.close();
				return true;
			}

			// log error
			std::cout << "Falied to open the file: \"" << path << "\" to save\n";
			std::cout << "badbit " << file.badbit << ", failbit" << file.failbit << ", eof" << file.eofbit;

			return false;
		}
		/*inline void serialise(std::ostream& file) const
		{
			DataScriptSaver saver;
			serialiseF(file, saver);
			saver.nextLine(file);
		}
		inline void serialise(std::ostream& file, DataScriptSaver& saver) const { serialiseF(file, saver); }*/

		////////////////////////////////////


		// opens a file from path. returns true at succes
		bool deserialise(const std::string& path)
		{
			std::ifstream file;
			file.open(path);

			if (file)
			{
				deserialise(file);

				file.close();
				return true;
			}

			// log error
			std::cout << "Falied to open the file: \"" << path << "\" to load\n";
			std::cout << "badbit " << file.badbit << ", failbit" << file.failbit << ", eof" << file.eofbit;

			return false;
		}
		inline void deserialise(std::istream& file)
		{
			DataScriptLoader loader;
			loader.nextLine(file);
			deserialiseF(file, loader);
		}
		inline void deserialise(std::istream& file, DataScriptLoader& loader) { deserialiseF(file, loader); }
		inline void deserialiseFromString(const std::string& definition)
		{
			istringstream stream(definition);
			DataScriptLoader loader;
			loader.nextLine(stream);
			deserialiseF(stream, loader);
		}
		inline void deserialiseFromString(const std::string& definition, DataScriptLoader& loader)
		{
			istringstream stream(definition);
			loader.nextLine(stream);
			deserialiseF(stream, loader);
		}


		// opens a file from path. returns true at succes
		bool deserialise(const std::string& path, Environment& environment)
		{
			std::ifstream file;
			file.open(path);

			if (file)
			{
				deserialise(file, environment);

				file.close();
				return true;
			}

			// log error
			std::cout << "Falied to open the file: \"" << path << "\" to load\n";
			std::cout << "badbit " << file.badbit << ", failbit" << file.failbit << ", eof" << file.eofbit;

			return false;
		}
		inline void deserialise(std::istream& file, Environment& environment)
		{
			DataScriptLoader loader;
			loader.nextLine(file);
			deserialiseF(file, loader, environment);
		}
		inline void deserialise(std::istream& file, DataScriptLoader& loader, Environment& environment) { deserialiseF(file, loader, environment); }
		inline void deserialiseFromString(const std::string& definition, Environment& environment)
		{
			istringstream stream(definition);
			DataScriptLoader loader;
			loader.nextLine(stream);
			deserialiseF(stream, loader, environment);
		}
		inline void deserialiseFromString(const std::string& definition, DataScriptLoader& loader, Environment& environment)
		{
			istringstream stream(definition);
			loader.nextLine(stream);
			deserialiseF(stream, loader, environment);
		}

	protected:
		// functions to overload
		// made with different name to avoid overloading other functions

		//virtual void serialiseF(std::ostream& file, DataScriptSaver& saver) const = 0;
		virtual void deserialiseF(std::istream& file, DataScriptLoader& loader) = 0;
		virtual void deserialiseF(std::istream& file, DataScriptLoader& loader, Environment& environment) { deserialiseF(file, loader); };
	};

}