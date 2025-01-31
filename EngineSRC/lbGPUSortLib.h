#pragma once
#include "CommonInclude.h"
#include "lbGraphicsDevice.h"

namespace lb::gpusortlib
{
	// Perform bitonic sort on a GPU dataset
	//	maxCount				-	Maximum size of the dataset. GPU count can be smaller (see: counterBuffer_read param)
	//	comparisonBuffer_read	-	Buffer containing values to compare by (Read Only)
	//	counterBuffer_read		-	Buffer containing count of values to sort (Read Only)
	//	counterReadOffset		-	Byte offset into the counter buffer to read the count value (Read Only)
	//	indexBuffer_write		-	The index list which to sort. Contains index values which can index the sortBase_read buffer. This will be modified (Read + Write)
	void Sort(
		uint32_t maxCount, 
		const lb::graphics::GPUBuffer& comparisonBuffer_read, 
		const lb::graphics::GPUBuffer& counterBuffer_read, 
		uint32_t counterReadOffset, 
		const lb::graphics::GPUBuffer& indexBuffer_write,
		lb::graphics::CommandList cmd
	);

	void Initialize();
};
