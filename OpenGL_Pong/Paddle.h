#pragma once

#include "Ball.h"
#include "glm/glm.hpp"

using namespace glm;

struct Paddle
{
	vec2 m_position;
	float m_height;

	void draw();
	bool intersectBall(Ball &const _ball);		//	ƒ{[ƒ‹‚Æ‚Ì“–‚½‚è”»’è‚ğ‚Æ‚é
};