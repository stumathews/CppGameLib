#pragma once
#include <memory>

#include "asset.h"

namespace gamelib
{
    class ScriptAsset final : public Asset
    {
        public:
			ScriptAsset(int inUid, const std::string& inName, const std::string& inPath, const std::string& inType, int inScene);

		    // Load the script into memory
            void Load() override;

			// Unload the script from memory
			bool Unload() override;

			// Get text content of the script
			[[nodiscard]]
			std::string* GetScriptContent() const;

		private:
		std::string* scriptContent;
    };
}

