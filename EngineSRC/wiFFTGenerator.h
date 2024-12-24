#pragma once
#include "CommonInclude.h"
#include "wiGraphicsDevice.h"

namespace lb::fftgenerator
{
	void fft_512x512_c2c(
		const lb::graphics::GPUResource& pUAV_Dst,
		const lb::graphics::GPUResource& pSRV_Dst,
		const lb::graphics::GPUResource& pSRV_Src,
		lb::graphics::CommandList cmd);

	void LoadShaders();
}
