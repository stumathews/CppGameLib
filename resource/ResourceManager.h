#pragma once
#include <string>
#include <map>
#include <memory>
#include "asset/asset.h"
#include "events/EventManager.h"
#include "events/EventSubscriber.h"
#include "graphic/SDLGraphicsManager.h"

namespace gamelib
{
	/***
	 * co-ordinates the resources in the game - such as holding definitions of all the resources/assets in the game
	 */
	class ResourceManager final : public EventSubscriber
	{		
	    public:		
		static ResourceManager* Get();
		~ResourceManager() override;
		std::shared_ptr<Asset> GetAssetInfo(const std::string& name);
		std::shared_ptr<Asset> GetAssetInfo(int uuid);
		[[nodiscard]] int GetCountResources() const { return countResources; }
		std::vector<std::shared_ptr<Event>> HandleEvent(std::shared_ptr<Event> theEvent, unsigned long deltaMs) override;
		void Unload() const;
		
		bool Initialize(const std::string& filePath);
	    std::string GetSubscriberName() override;

		// index the resources file
		void IndexResourceFile(const std::string& resourcesFilePath = "game/resources.xml");
		std::shared_ptr<Asset>& CreateAssetFromElement(const char* type, std::shared_ptr<Asset>& theAsset, tinyxml2::XMLElement* const& element);
		[[nodiscard]] int GetCountUnloadedResources() const { return countUnloadedResources; }
		[[nodiscard]] int GetCountLoadedResources() const { return countLoadedResources; }

		enum class ErrorNumbers
		{
			NoAssetManagerForType,
			FailedToLoadResourceFile,
			UnknownResourceType
		};
	protected:
		static ResourceManager* Instance;
				
	private:
		ResourceManager();		
		void LoadSceneAssets(int level);
	    void StoreAsset(const std::shared_ptr<Asset>& asset);
		std::map<int, std::vector<std::shared_ptr<Asset>>> resourcesByScene;   
		std::map<std::string, std::shared_ptr<Asset>> resourcesByName;   
		std::map<int, std::shared_ptr<Asset>> resourcesById;

		int countResources = 0;
		int countLoadedResources = 0;
		int countUnloadedResources = 0;
		bool debug;
		
	};
}

