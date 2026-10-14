#pragma once

extern "C" unsigned int getCylinderVerticesCount(
	unsigned int slices,
	unsigned int stacks
);

extern "C" unsigned int getCylinderElementsCount(
	unsigned int slices,
	unsigned int stacks
);

extern "C" void getCylinderVerticesData(
	float* cylinderPositionCoords,
	float* cylinderNormalCoords,
	float* cylinderTexCoords,
	unsigned short* cylinderElements,
	const float baseRadius,
	const float topRadius,
	const unsigned int slices,
	const unsigned int stacks
);
