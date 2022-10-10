#ifndef PWSHBUILD_GENERATOR_H
#define PWSHBUILD_GENERATOR_H
#include <string>
#include <filesystem>
#include <vector>

enum class GenObjectDescriptor
{
	FILE,
	PATH,
};

class GeneratorObject
{
	public:
		GeneratorObject() = default;

	protected:
		std::filesystem::path 	_location;
		GenObjectDescriptor 	_descriptor;
};

class GeneratorProject
{
	public:
		GeneratorProject(std::string);
		GeneratorProject(std::filesystem::path);

		void parse();

	protected:
		std::filesystem::path _generator_file;
		std::vector<GeneratorObject> _generator_objects;
};

#endif