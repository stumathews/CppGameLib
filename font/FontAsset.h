#pragma once
#include "SDL_ttf.h"
#include "asset/asset.h"

namespace gamelib
{
	/// <summary>
	/// Font asset details
	/// </summary>
	class FontAsset final : public Asset
	{
	public:

		/// <summary>
		/// Create Font details
		/// </summary>
		FontAsset(int uid, std::string name, std::string path, std::string type, int scene);

		/// <summary>
		/// Get font data
		/// </summary>
		/// <returns></returns>
		TTF_Font* GetFont() const;

		/// <summary>
		/// Load font into memory
		/// </summary>
		void Load() override;

		/// <summary>
		/// Free font from memory
		/// </summary>
		/// <returns></returns>
		bool Unload() override;

	private:

		/// <summary>
		/// Font data
		/// </summary>
		TTF_Font* font = nullptr;
	};
}
