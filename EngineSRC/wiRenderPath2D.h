#pragma once
#include "wiRenderPath.h"
#include "wiGUI.h"
#include "wiVector.h"

#include <string>

namespace lb
{
	class Sprite;
	class SpriteFont;

	class RenderPath2D :
		public RenderPath
	{
	protected:
		lb::graphics::Texture rtStenciled;
		lb::graphics::Texture rtStenciled_resolved;
		lb::graphics::Texture rtFinal;
		lb::graphics::Texture rtFinal_MSAA;

		lb::gui::GUI GUI;

		XMUINT2 current_buffersize{};
		float current_layoutscale{};

		float hdr_scaling = 9.0f;

		uint32_t msaaSampleCount = 1;

	public:
		// Delete GPU resources and initialize them to default
		virtual void DeleteGPUResources();
		// create resolution dependent resources, such as render targets
		virtual void ResizeBuffers();
		// update DPI dependent elements, such as GUI elements, sprites
		virtual void ResizeLayout();

		void Update(float dt) override;
		void FixedUpdate() override;
		void Render() const override;
		void Compose(lb::graphics::CommandList cmd) const override;

		virtual void setMSAASampleCount(uint32_t value) { msaaSampleCount = value; }
		constexpr uint32_t getMSAASampleCount() const { return msaaSampleCount; }

		const lb::graphics::Texture& GetRenderResult() const { return rtFinal; }
		virtual const lb::graphics::Texture* GetDepthStencil() const { return nullptr; }
		virtual const lb::graphics::Texture* GetGUIBlurredBackground() const { return nullptr; }

		void AddSprite(lb::Sprite* sprite, const std::string& layer = "");
		void RemoveSprite(lb::Sprite* sprite);
		void ClearSprites();
		int GetSpriteOrder(lb::Sprite* sprite);

		void AddFont(lb::SpriteFont* font, const std::string& layer = "");
		void RemoveFont(lb::SpriteFont* font);
		void ClearFonts();
		int GetFontOrder(lb::SpriteFont* font);

		struct RenderItem2D
		{
			enum class TYPE
			{
				SPRITE,
				FONT,
			} type = TYPE::SPRITE;
			union
			{
				lb::Sprite* sprite = nullptr;
				lb::SpriteFont* font;
			};
			int order = 0;
		};
		struct RenderLayer2D
		{
			lb::vector<RenderItem2D> items;
			std::string name;
			int order = 0;
		};
		lb::vector<RenderLayer2D> layers{ 1 };
		void AddLayer(const std::string& name);
		void SetLayerOrder(const std::string& name, int order);
		void SetSpriteOrder(lb::Sprite* sprite, int order);
		void SetFontOrder(lb::SpriteFont* font, int order);
		void SortLayers();
		void CleanLayers();

		const lb::gui::GUI& GetGUI() const { return GUI; }
		lb::gui::GUI& GetGUI() { return GUI; }

		float resolutionScale = 1.0f;
		XMUINT2 GetInternalResolution() const
		{
			return XMUINT2(
				uint32_t((float)GetPhysicalWidth() * resolutionScale),
				uint32_t((float)GetPhysicalHeight() * resolutionScale)
			);
		}

		float GetHDRScaling() const { return hdr_scaling; }
		void SetHDRScaling(float value) { hdr_scaling = value; }
	};

}
