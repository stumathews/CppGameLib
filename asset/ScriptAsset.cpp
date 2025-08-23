#include "ScriptAsset.h"

#include <fstream>


gamelib::ScriptAsset::ScriptAsset(int inUid, const std::string& inName, const std::string& inPath,
                                  const std::string& inType, int inScene): Asset(inUid, inName, inPath, inType, inScene)
{
	AssetType = Asset::AssetType::Script;
}

void gamelib::ScriptAsset::Load()
{
	// Load the script from file
	std::ifstream script(this->FilePath);

	// Create a string on the heap to hold the script content
	this->scriptContent = new std::string();

	// Allocate all memory up front required to store the script into the string.
	// This prevents multiple memory allocations as the string grows
	script.seekg(0, std::ios::end);
	scriptContent->reserve(script.tellg()); // reserve as much as needed: input position indicator of the current associated streambuf object
	script.seekg(0, std::ios::beg);

	// Copy the contents of the file into the string
	scriptContent->assign((std::istreambuf_iterator<char>(script)),
		std::istreambuf_iterator<char>());

	IsLoadedInMemory = true; // Set to true when loaded
}

bool gamelib::ScriptAsset::Unload()
{
	// When the last shared_ptr to the script content is released, the memory will be freed
	delete scriptContent;

	scriptContent = nullptr;
	IsLoadedInMemory = false;

	return true; // Return true if successfully unloaded
}

std::string* gamelib::ScriptAsset::GetScriptContent() const
{
	return scriptContent;
}
