#pragma once
#include "lbRenderPath3D.h"
#include "lbVector.h"

namespace lb
{

	class RenderPath3D_PathTracing :
		public RenderPath3D
	{
	protected:
		int sam = -1;
		int target = 1024;
		lb::graphics::Texture traceResult;
		lb::graphics::Texture traceDepth;
		lb::graphics::Texture traceStencil;

		lb::vector<uint8_t> texturedata_src;
		lb::vector<uint8_t> texturedata_dst;
		lb::vector<uint8_t> texturedata_albedo;
		lb::vector<uint8_t> texturedata_normal;
		lb::graphics::Texture denoiserAlbedo;
		lb::graphics::Texture denoiserNormal;
		lb::graphics::Texture denoiserResult;
		lb::jobsystem::context denoiserContext;

		void ResizeBuffers() override;

	public:

		void Update(float dt) override;
		void Render() const override;
		void Compose(lb::graphics::CommandList cmd) const override;

		int getCurrentSampleCount() const { return sam; }
		void setTargetSampleCount(int value) { target = value; }
		float getProgress() const { return (float)sam / (float)target; }

		float denoiserProgress = 0;
		float getDenoiserProgress() const { return denoiserProgress; }
		bool isDenoiserAvailable() const;

		void resetProgress() { sam = -1; denoiserProgress = 0; volumetriccloudResources.frame = 0; }

		uint8_t instanceInclusionMask_PathTrace = 0xFF;
	};

}
