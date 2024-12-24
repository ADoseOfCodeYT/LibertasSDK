#pragma once
#include "CommonInclude.h"
#include "wiGraphicsDevice.h"
#include "wiScene_Decl.h"

namespace lb
{
	struct GPUBVH
	{
		// Scene BVH intersection resources:
		lb::graphics::GPUBuffer bvhNodeBuffer;
		lb::graphics::GPUBuffer bvhParentBuffer;
		lb::graphics::GPUBuffer bvhFlagBuffer;
		lb::graphics::GPUBuffer primitiveCounterBuffer;
		lb::graphics::GPUBuffer primitiveIDBuffer;
		lb::graphics::GPUBuffer primitiveBuffer;
		lb::graphics::GPUBuffer primitiveMortonBuffer;
		uint32_t primitiveCapacity = 0;
		bool IsValid() const { return primitiveCounterBuffer.IsValid(); }

		void Update(const lb::scene::Scene& scene);
		void Build(const lb::scene::Scene& scene, lb::graphics::CommandList cmd) const;

		void Clear();

		static void Initialize();
	};
}
