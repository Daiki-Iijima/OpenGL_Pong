#pragma once

#include "Ball.h"
#include "glm/glm.hpp"

using namespace glm;

struct Paddle
{
	float m_height;
	vec2 m_position;

	void draw();
	bool intersectBall(Ball &const _ball);		//	�{�[���Ƃ̓����蔻����Ƃ�
};