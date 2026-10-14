#pragma once
#include "Common.h"
#include "engine/UniformBuffer.h"

enum TransformType
{
	TSR
};

struct Transform
{
	float tx, ty, tz;
	float sx, sy, sz;
	float rx, ry, rz;


	vmath::mat4 transformationMatrix;
	TransformType type;
	Transform()
	{
		tx = 0.0f; ty = 0.0f; tz = 0.0f;
		sx = 1.0f; sy = 1.0f; sz = 1.0f;
		rx = 0.0f; ry = 0.0f; rz = 0.0f;
		
	}

	Transform(float _tx, float _ty, float _tz, float _sx, float _sy, float _sz, float _rx, float _ry, float _rz, TransformType _type = TransformType::TSR): tx(_tx), ty(_ty), tz(_tz), sx(_sx), sy(_sy), sz(_sz), rx(_rx), ry(_ry), rz(_rz)
	{

		ErrorLog(NULL, "scale : [%f, %f, %f]\n", sx, sy, sz);

		mat4 translationMatrix = vmath::translate(_tx, _ty, _tz);
		mat4 scaleMatrix = vmath::scale(_sx, _sy, _sz);
		mat4 rotationMatrixX = vmath::rotate(_rx, 1.0f, 0.0f, 0.0f);
		mat4 rotationMatrixY = vmath::rotate(_ry, 0.0f, 1.0f, 0.0f);
		mat4 rotationMatrixZ = vmath::rotate(_rz, 0.0f, 0.0f, 1.0f);

		mat4 rotationMatrix = rotationMatrixX * rotationMatrixY * rotationMatrixZ;
		type = _type;
		
		if (type == TSR)
		{
			transformationMatrix = translationMatrix * scaleMatrix * rotationMatrix;
		}
	}

	void UpdateTransform()
	{
		mat4 translationMatrix = vmath::translate(tx, ty, tz);

		mat4 scaleMatrix = vmath::scale(sx, sy, sz);
		mat4 rotationMatrixX = vmath::rotate(rx, 1.0f, 0.0f, 0.0f);
		mat4 rotationMatrixY = vmath::rotate(ry, 0.0f, 1.0f, 0.0f);
		mat4 rotationMatrixZ = vmath::rotate(rz, 0.0f, 0.0f, 1.0f);
		mat4 rotationMatrix = rotationMatrixX * rotationMatrixY * rotationMatrixZ;

		if (type == TransformType::TSR)
		{
			transformationMatrix = translationMatrix * scaleMatrix * rotationMatrix;
		}
	}

	vmath::mat4 GetTransformationMatrix()
	{
		return transformationMatrix;
	}
};

static void ModelTransform(std::vector<struct Transform>& _transformModel, int _index, int _transform, float _step, float x, float y, float z)
{
	if (_index == -1)
		return;

	if (_transform == 0)
	{
		_transformModel[_index].tx += x * _step;
		_transformModel[_index].ty += y * _step;
		_transformModel[_index].tz += z * _step;
	}
	else if (_transform == 1)
	{
		_transformModel[_index].sx += x * _step;
		_transformModel[_index].sy += y * _step;
		_transformModel[_index].sz += z * _step;
	}
	else if (_transform == 2)
	{
		_transformModel[_index].rx += x * _step;
		_transformModel[_index].ry += y * _step;
		_transformModel[_index].rz += z * _step;
	}
	_transformModel[_index].UpdateTransform();
}

static void UpdateLightParameter(std::vector<POINTLIGHTMIRROR>& lights, int index, int member, float step, float x, float y, float z, float range)
{
	if (lights.size() == 0)
		return;

	switch (member)
	{
	case 0:
		lights[index].ambient[0] += step * x;
		lights[index].ambient[1] += step * y;
		lights[index].ambient[2] += step * z;
		break;

	case 1:
		lights[index].diffuse[0] += step * x;
		lights[index].diffuse[1] += step * y;
		lights[index].diffuse[2] += step * z;
		break;

	case 2:
		lights[index].specular[0] += step * x;
		lights[index].specular[1] += step * y;
		lights[index].specular[2] += step * z;
		break;

	case 3:
		lights[index].position[0] += step * x;
		lights[index].position[1] += step * y;
		lights[index].position[2] += step * z;
		break;

	case 4:
		float linear;
		float quadratic;
		if (range >= 0.5f)
		{
			linear = 4.5f / range;
			quadratic = 7.5f / (range * range);
		}
		else
		{
			linear = 0.7f;   //if range = 7. default
			quadratic = 1.8f;
		}
		lights[index].linear = linear;
		lights[index].quadratic = quadratic;
		break;

	case 5:
		lights[index].constant += step * x;
		break;
	}
}

static void AddPointLight(std::vector<POINTLIGHTMIRROR>& lights, vec4 position)
{
	POINTLIGHTMIRROR light = {};

	light.ambient = vec4(0.01f, 0.01f, 0.01f, 1.0f);
	light.diffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	light.specular = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light.position = position;
	light.constant = 1.0f;
	light.quadratic = 0.032f;
	light.linear = 0.09f;

	lights.push_back(light);
}