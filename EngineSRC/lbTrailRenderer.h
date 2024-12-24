#pragma once
#include "CommonInclude.h"
#include "lbVector.h"
#include "lbGraphicsDevice.h"
#include "lbEnums.h"
#include "lbScene_Decl.h"

namespace lb
{
	struct TrailRenderer
	{
		struct TrailPoint
		{
			XMFLOAT3 position = XMFLOAT3(0, 0, 0);
			float width = 1;
			XMFLOAT4 color = XMFLOAT4(1, 1, 1, 1);
		};
		lb::vector<TrailPoint> points;
		lb::vector<uint32_t> cuts;
		XMFLOAT4 color = XMFLOAT4(1, 1, 1, 1);
		lb::enums::BLENDMODE blendMode = lb::enums::BLENDMODE_ALPHA;
		uint32_t subdivision = 100;
		float width = 1;
		lb::graphics::Texture texture;
		lb::graphics::Texture texture2;
		XMFLOAT4 texMulAdd = XMFLOAT4(1, 1, 0, 0);
		XMFLOAT4 texMulAdd2 = XMFLOAT4(1, 1, 0, 0);

		void Cut();

		void Draw(const lb::scene::CameraComponent& camera, lb::graphics::CommandList cmd) const;

		static void Initialize();
	};
}
