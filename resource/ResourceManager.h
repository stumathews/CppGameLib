#pragma once
#include <string>
#include <map>
#include <memory>
#include "asset/asset.h"
#include "audio/AudioManager.h"
#include "events/EventManager.h"
#include "events/EventSubscriber.h"
#include "font/FontManager.h"
#include "graphic/SDLGraphicsManager.h"
#include <objects/MultipleInheritableEnableSharedFromThis.h>

namespace gamelib
{
	/***
	 * co-ordinates the resources in the game - such as holding definitions of all the resources/assets in the game
	 */
	class ResourceManager : public EventSubscriber
	{		
	    public:		
		static ResourceManager* Get();
		~ResourceManager();
		std::shared_ptr<Asset> GetAssetInfo(const std::string& name);
		std::shared_ptr<Asset> GetAssetInfo(int uuid);
		int GetCountResources() const { return countResources; }
		std::vector<std::shared_ptr<Event>> HandleEvent(std::shared_ptr<Event> the_event, unsigned long deltaMs) override;
		void Unload();
		
		bool Initialize(std::string filePath);
	    std::string GetSubscriberName() override;

		// index the resoures file
		void IndexResourceFile(std::string resources_file_path = "game/resources.xml");
		std::shared_ptr<gamelib::Asset>& CreateAssetFromElement(const char* type, std::shared_ptr<gamelib::Asset>& the_asset, tinyxml2::XMLElement* const& element);
		int GetCountUnloadedResources() const { return countUnloadedResources; }
	    int GetCountLoadedResources() const { return countLoadedResources; }

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

