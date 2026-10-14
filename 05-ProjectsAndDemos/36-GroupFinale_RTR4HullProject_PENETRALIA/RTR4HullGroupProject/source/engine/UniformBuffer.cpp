#include "engine/UniformBuffer.h"

#pragma region UniformBufferLayoutDefinitions

UniformBlockLayout ublLights = {
	"Lights"	// UniformBlockLayout.name
	, 644		// UniformBlockLayout.size
	, 2			// UniformBlockLayout.memberCount
	, {
		{
			"directionalLights"			// UniformBlockLayout.members[0].name
			, 16						// UniformBlockLayout.members[0].baseAlignment
			, 0							// UniformBlockLayout.members[0].alignedOffset
			, 640						// UniformBlockLayout.members[0].memberSize
		}
		, {
			"directionalLightCount"		// UniformBlockLayout.members[1].name
			, 4							// UniformBlockLayout.members[1].baseAlignment
			, 640						// UniformBlockLayout.members[1].alignedOffset
			, 4							// UniformBlockLayout.members[1].memberSize
		}
	}			// UniformBlockLayout.members
};

UniformBlockLayout ublPointLights = {
	"PointLights"	// UniformBlockLayout.name
	, 804			// UniformBlockLayout.size
	, 2				// UniformBlockLayout.memberCount
	, {
		{
			"pointLights"				// UniformBlockLayout.members[0].name
			, 16						// UniformBlockLayout.members[0].baseAlignment
			, 0							// UniformBlockLayout.members[0].alignedOffset
			, 800						// UniformBlockLayout.members[0].memberSize
		}
		, {
			"pointLightCount"			// UniformBlockLayout.members[1].name
			, 4							// UniformBlockLayout.members[1].baseAlignment
			, 800						// UniformBlockLayout.members[1].alignedOffset
			, 4							// UniformBlockLayout.members[1].memberSize
		}
	}				// UniformBlockLayout.members
};

#pragma endregion UniformBufferLayoutDefinitions

#pragma region UniformBufferObjectDefinitions

LIGHTSUBO gLightsUBO = {
	{
		0,				// LIGHTSUBO.ubo.uniform
		ublLights		// LIGHTSUBO.ubo.layout
	},		// LIGHTSUBO.ubo
	{
		{
			{
				vmath::vec4(150.0, 150.0, 150.0, 1.0),		// LIGHTSUBO.data.directionalLights[0].direction
				vmath::vec4(0.4, 0.4, 0.4, 0.0),		// LIGHTSUBO.data.directionalLights[0].ambient
				vmath::vec4(0.0, 0.0, 0.0, 0.0),		// LIGHTSUBO.data.directionalLights[0].diffuse
				vmath::vec4(0.0, 0.0, 0.0, 0.0),		// LIGHTSUBO.data.directionalLights[0].specular
			},	// LIGHTSUBO.data.directionalLights[0]
			{
				vmath::vec4(-50.0, 50.0, 50.0, 1.0),	// LIGHTSUBO.data.directionalLights[1].direction
				vmath::vec4(0.0, 0.0, 1.0, 0.0),		// LIGHTSUBO.data.directionalLights[1].ambient
				vmath::vec4(0.0, 0.0, 1.0, 0.0),		// LIGHTSUBO.data.directionalLights[1].diffuse
				vmath::vec4(1.0, 1.0, 1.0, 0.0),		// LIGHTSUBO.data.directionalLights[1].specular
			},	// LIGHTSUBO.data.directionalLights[1]
		},	// LIGHTSUBO.data.directionalLights
		1,	// LIGHTSUBO.data.directionalLightCount
	},		// LIGHTSUBO.data
	true	// LIGHTSUBO.isUpdated
};

POINTLIGHTSUBO gPointLightsUBO = {
	{
		0,				// POINTLIGHTSUBO.ubo.uniform
		ublPointLights	// POINTLIGHTSUBO.ubo.layout
	},		// POINTLIGHTSUBO.ubo
	{
		{
			{
				vmath::vec4(0.0, 48.0f, 850.0f, 1.0),		// POINTLIGHTSUBO.data.pointLights[0].position
				vmath::vec4(1.0, 1.0, 1.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[0].ambient
				vmath::vec4(10.0, 0.0, 0.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[0].diffuse
				vmath::vec4(10.0, 0.0, 0.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[0].specular
				1.0f,									// POINTLIGHTSUBO.data.pointLights[0].constant
				0.0045f,									// POINTLIGHTSUBO.data.pointLights[0].linear
				0.0000075f,									// POINTLIGHTSUBO.data.pointLights[0].quadratic
				0.0f,									// POINTLIGHTSUBO.data.pointLights[0].padding_77_to_80
			},	// POINTLIGHTSUBO.data.pointLights[0]
			{
				vmath::vec4(-240.0, 42.0f, 770.0f, 1.0),		// POINTLIGHTSUBO.data.pointLights[0].position
				vmath::vec4(1.0, 1.0, 1.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[0].ambient
				vmath::vec4(10.0, 0.0, 0.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[0].diffuse
				vmath::vec4(10.0, 0.0, 0.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[0].specular
				1.0f,									// POINTLIGHTSUBO.data.pointLights[0].constant
				0.0045f,									// POINTLIGHTSUBO.data.pointLights[0].linear
				0.0000075f,									// POINTLIGHTSUBO.data.pointLights[0].quadratic
				0.0f,									// POINTLIGHTSUBO.data.pointLights[0].padding_77_to_80
			},	// POINTLIGHTSUBO.data.pointLights[1]
			{
				vmath::vec4(170.0, 44.0f, 900.0f, 1.0),		// POINTLIGHTSUBO.data.pointLights[0].position
				vmath::vec4(1.0, 1.0, 1.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[0].ambient
				vmath::vec4(1.0, 0.0, 0.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[0].diffuse
				vmath::vec4(1.0, 0.0, 0.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[0].specular
				1.0f,									// POINTLIGHTSUBO.data.pointLights[0].constant
				0.0045f,									// POINTLIGHTSUBO.data.pointLights[0].linear
				0.0000075f,									// POINTLIGHTSUBO.data.pointLights[0].quadratic
				0.0f,									// POINTLIGHTSUBO.data.pointLights[0].padding_77_to_80
			},	// POINTLIGHTSUBO.data.pointLights[2]
			{
				vmath::vec4(240.0, 40.0f, 840.0f, 1.0),		// POINTLIGHTSUBO.data.pointLights[0].position
				vmath::vec4(1.0, 1.0, 1.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[0].ambient
				vmath::vec4(1.0, 0.0, 0.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[0].diffuse
				vmath::vec4(1.0, 0.0, 0.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[0].specular
				1.0f,									// POINTLIGHTSUBO.data.pointLights[0].constant
				0.0045f,									// POINTLIGHTSUBO.data.pointLights[0].linear
				0.0000075f,									// POINTLIGHTSUBO.data.pointLights[0].quadratic
				0.0f,									// POINTLIGHTSUBO.data.pointLights[0].padding_77_to_80
			},	// POINTLIGHTSUBO.data.pointLights[3]
			{
				vmath::vec4(190.0, 41.0f, 750.0f, 1.0),		// POINTLIGHTSUBO.data.pointLights[0].position
				vmath::vec4(1.0, 1.0, 1.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[0].ambient
				vmath::vec4(1.0, 0.0, 0.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[0].diffuse
				vmath::vec4(1.0, 0.0, 0.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[0].specular
				1.0f,									// POINTLIGHTSUBO.data.pointLights[0].constant
				0.0045f,									// POINTLIGHTSUBO.data.pointLights[0].linear
				0.0000075f,									// POINTLIGHTSUBO.data.pointLights[0].quadratic
				0.0f,									// POINTLIGHTSUBO.data.pointLights[0].padding_77_to_80
			},	// POINTLIGHTSUBO.data.pointLights[4]
			{
				vmath::vec4(-120.0, 42.0f, 920.0f, 1.0),		// POINTLIGHTSUBO.data.pointLights[0].position
				vmath::vec4(1.0, 1.0, 1.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[0].ambient
				vmath::vec4(1.0, 0.0, 0.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[0].diffuse
				vmath::vec4(1.0, 0.0, 0.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[0].specular
				1.0f,									// POINTLIGHTSUBO.data.pointLights[0].constant
				0.0045f,									// POINTLIGHTSUBO.data.pointLights[0].linear
				0.0000075f,									// POINTLIGHTSUBO.data.pointLights[0].quadratic
				0.0f,									// POINTLIGHTSUBO.data.pointLights[0].padding_77_to_80
			},	// POINTLIGHTSUBO.data.pointLights[5]
			{
				vmath::vec4(-84.0, 44.0f, 900.0f, 1.0),	// POINTLIGHTSUBO.data.pointLights[1].position
				vmath::vec4(1.0, 1.0, 1.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[1].ambient
				vmath::vec4(0.0, 1.0, 0.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[1].diffuse
				vmath::vec4(0.0, 1.0, 0.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[1].specular
				1.0f,									// POINTLIGHTSUBO.data.pointLights[1].constant
				0.09f,									// POINTLIGHTSUBO.data.pointLights[1].linear
				0.032f,									// POINTLIGHTSUBO.data.pointLights[1].quadratic
				0.0f,									// POINTLIGHTSUBO.data.pointLights[1].padding_77_to_80
			},	// POINTLIGHTSUBO.data.pointLights[6]
			{
				vmath::vec4(85.0, 41.0f, 900.0f, 1.0),		// POINTLIGHTSUBO.data.pointLights[2].position
				vmath::vec4(1.0, 1.0, 1.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[2].ambient
				vmath::vec4(0.0, 0.0, 1.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[2].diffuse
				vmath::vec4(0.0, 0.0, 1.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[2].specular
				1.0f,									// POINTLIGHTSUBO.data.pointLights[2].constant
				0.09f,									// POINTLIGHTSUBO.data.pointLights[2].linear
				0.032f,									// POINTLIGHTSUBO.data.pointLights[2].quadratic
				0.0f,									// POINTLIGHTSUBO.data.pointLights[2].padding_77_to_80
			},	// POINTLIGHTSUBO.data.pointLights[7]
			{
				vmath::vec4(120.0, 46.0f, 790.0f, 1.0),		// POINTLIGHTSUBO.data.pointLights[2].position
				vmath::vec4(1.0, 1.0, 1.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[2].ambient
				vmath::vec4(1.0, 1.0, 0.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[2].diffuse
				vmath::vec4(1.0, 1.0, 0.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[2].specular
				1.0f,									// POINTLIGHTSUBO.data.pointLights[2].constant
				0.09f,									// POINTLIGHTSUBO.data.pointLights[2].linear
				0.032f,									// POINTLIGHTSUBO.data.pointLights[2].quadratic
				0.0f,									// POINTLIGHTSUBO.data.pointLights[2].padding_77_to_80
			},	// POINTLIGHTSUBO.data.pointLights[8]

			{
				vmath::vec4(-40.0, 40.0f, 710.0f, 1.0),		// POINTLIGHTSUBO.data.pointLights[2].position
				vmath::vec4(1.0, 1.0, 1.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[2].ambient
				vmath::vec4(1.0, 0.0, 1.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[2].diffuse
				vmath::vec4(1.0, 0.0, 1.0, 0.0),		// POINTLIGHTSUBO.data.pointLights[2].specular
				1.0f,									// POINTLIGHTSUBO.data.pointLights[2].constant
				0.09f,									// POINTLIGHTSUBO.data.pointLights[2].linear
				0.032f,									// POINTLIGHTSUBO.data.pointLights[2].quadratic
				0.0f,									// POINTLIGHTSUBO.data.pointLights[2].padding_77_to_80
			},	// POINTLIGHTSUBO.data.pointLights[9]

		},	// POINTLIGHTSUBO.data.pointLights
		10,	// POINTLIGHTSUBO.data.pointLightCount
	},		// POINTLIGHTSUBO.data
	true	// POINTLIGHTSUBO.isUpdated
};

#pragma endregion UniformBufferObjectDefinitions

bool InitializeUBOs(void)
{
	// Code

	// Directional light uniform buffer object - start
	/*glGenBuffers(1, &(uboDirectionalLight.uniform));
	glBindBuffer(GL_UNIFORM_BUFFER, uboDirectionalLight.uniform);
	glBufferData(GL_UNIFORM_BUFFER, uboDirectionalLight.layout.size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);*/
	// Directional light uniform buffer object - end

	// Lights uniform buffer object - start
	glGenBuffers(1, &(gLightsUBO.ubo.uniform));
	glBindBuffer(GL_UNIFORM_BUFFER, gLightsUBO.ubo.uniform);
	glBufferData(GL_UNIFORM_BUFFER, gLightsUBO.ubo.layout.size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// Lights uniform buffer object - end

	// PointLights uniform buffer object - start
	glGenBuffers(1, &(gPointLightsUBO.ubo.uniform));
	glBindBuffer(GL_UNIFORM_BUFFER, gPointLightsUBO.ubo.uniform);
	glBufferData(GL_UNIFORM_BUFFER, gPointLightsUBO.ubo.layout.size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// PointLights uniform buffer object - end

	return(true);
}

bool BindUBOs(void)
{
	// Bind directional light UBO - start
	//glBindBufferBase(GL_UNIFORM_BUFFER, UBO_DIRECTIONA_LIGHT, uboDirectionalLight.uniform);
	// Bind directional light UBO - end

	// Bind Lights UBO - start
	glBindBufferBase(GL_UNIFORM_BUFFER, UBO_LIGHTS, gLightsUBO.ubo.uniform);
	// Bind Lights UBO - end

	// Bind PointLights UBO - start
	glBindBufferBase(GL_UNIFORM_BUFFER, UBO_POINT_LIGHTS, gPointLightsUBO.ubo.uniform);
	// Bind PointLights UBO - end

	return(true);
}

void SetUBOs(void)
{
	// Code

	// Set directional light UBO - start
	/*glBindBuffer(GL_UNIFORM_BUFFER, uboDirectionalLight.uniform);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, uboDirectionalLight.layout.size, &gDirectionalLight);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);*/
	// Set directional light UBO - end

	// Set Lights UBO - start
	if (gLightsUBO.isUpdated)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, gLightsUBO.ubo.uniform);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, gLightsUBO.ubo.layout.size, &gLightsUBO.data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		gLightsUBO.isUpdated = false;
	}
	// Set Lights UBO - end

	// Set PointLights UBO - start
	if (gPointLightsUBO.isUpdated)
	{
		/*gPointLightsUBO.data.pointLights[1].position[0] = lightPosition_X;
		gPointLightsUBO.data.pointLights[1].position[1] = lightPosition_Y;
		gPointLightsUBO.data.pointLights[1].position[2] = lightPosition_Z;
		
		gPointLightsUBO.data.pointLights[1].linear = linear;
		gPointLightsUBO.data.pointLights[1].quadratic = quadratic;*/

		glBindBuffer(GL_UNIFORM_BUFFER, gPointLightsUBO.ubo.uniform);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, gPointLightsUBO.ubo.layout.size, &gPointLightsUBO.data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		gPointLightsUBO.isUpdated = false;
	}
	// Set PointLights UBO - end

	
}

void UninitializeUBOs(void)
{
	// Delete directional light UBO - start
	/*if (uboDirectionalLight.uniform)
	{
		glDeleteBuffers(1, &uboDirectionalLight.uniform);
		uboDirectionalLight.uniform = 0;
	}*/
	// Delete directional light UBO - end

	// Delete Lights UBO - start
	if (gLightsUBO.ubo.uniform)
	{
		glDeleteBuffers(1, &gLightsUBO.ubo.uniform);
		gLightsUBO.ubo.uniform = 0;
	}
	// Delete Lights UBO - end

	// Delete PointLights UBO - start
	if (gPointLightsUBO.ubo.uniform)
	{
		glDeleteBuffers(1, &gPointLightsUBO.ubo.uniform);
		gPointLightsUBO.ubo.uniform = 0;
	}
	// Delete PointLights UBO - end
}
