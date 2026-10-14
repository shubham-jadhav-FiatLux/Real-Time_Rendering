#pragma once

#include "Common.h"

#define MAX_NAME_LENGTH 512
#define MAX_UNIFORM_BLOCK_LAYOUT_MEMBER_COUNT 10

#pragma region UniformBufferLayoutDataTypes

struct UniformBlockLayoutMember
{
	char name[MAX_NAME_LENGTH];
	unsigned int baseAlignment;
	unsigned int alignedOffset;
	unsigned int memberSize;
};

struct UniformBlockLayout
{
	char name[MAX_NAME_LENGTH];
	unsigned int size;
	unsigned int memberCount;
	UniformBlockLayoutMember members[MAX_UNIFORM_BLOCK_LAYOUT_MEMBER_COUNT];
};

#pragma endregion UniformBufferLayoutDataTypes

#pragma region UniformBufferLayouts

extern UniformBlockLayout ublLights;

#pragma endregion UniformBufferLayouts
