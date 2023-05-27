#include <iostream>
#include "Particle.h"
#include "../../../Graphics/Source/BatchRenderer2D.h"
#include "../../../Dependencies/Vendor/glm/glm.hpp"
#include <glm/ext/matrix_transform.hpp>


void Particle::Init(Mathf::vec3 startPos, float Speed, float Newlifespan, Mathf::vec2 scale)
{
	static_cast<void>(Speed);//unref
	lifespan = Newlifespan;
	_state.aliveData.pos = startPos;
	_state.aliveData.scale = scale;
	_isAlive = true;
}

void Particle::Update(float dt)
{
	//Update pos
	//std::cout << "Particle is alive at " << _state.aliveData.pos.x << " , " << _state.aliveData.pos.y << std::endl;
		
	//Update vel
	_state.aliveData.pos.x += _state.aliveData.vel.x * dt;
	_state.aliveData.pos.y += _state.aliveData.vel.y * dt;
}

void applyTransform(Particle particle, glm::mat4& appliedOn)
{
	if (particle.IsAlive())
	{
		auto& transform = particle._state.aliveData;

		glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(transform.pos.x, transform.pos.y, 0.0f)); //Z 'positioning' only determines layering, so is set at 0.0
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(transform.scale.x, transform.scale.y, 1.0f)); //There is no Z scaling, so it's set as 1.0
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(transform.rotation), glm::vec3(0.0f, 0.0f, 1.0f)); //Rotation about Z axis, TURN INTO RADIANS

		appliedOn = translate * rotation * scale * glm::mat4(1.0f);
	}
}

void Particle::Draw()
{
	// Add into batch rendering, if using texture
	glm::mat4 worldMatrix{};
	
	applyTransform(*this, worldMatrix);
	if (use_texture)
	{
		//BatchRenderer2D::GetInstance().AddRotatedQuad(worldMatrix, _state.aliveData.pos.z, textureId, _state.aliveData.color);
		BatchRenderer2D::GetInstance().AddRotatedQuad(worldMatrix, _state.aliveData.pos.z, textureId, { _state.aliveData.color.x, _state.aliveData.color.y, _state.aliveData.color.z, _state.aliveData.color.w });
	}
	else
	{
		//TODO: draw a geometric shape instead
		//BatchRenderer2D::GetInstance().AddRotateTriangle();
		
		//triangle with middle top, bottom left, bottom right for now
		BatchRenderer2D::GetInstance().AddRotateTriangle(
			_state.aliveData.pos.x,
			_state.aliveData.pos.y + _state.aliveData.scale.y,
			_state.aliveData.pos.x - _state.aliveData.scale.x,
			_state.aliveData.pos.y - _state.aliveData.scale.y,
			_state.aliveData.pos.x + _state.aliveData.scale.x,
			_state.aliveData.pos.y - _state.aliveData.scale.y,

			_state.aliveData.color.x,
			_state.aliveData.color.y,
			_state.aliveData.color.z
			//_state.aliveData.color.w
			);
	}
}



//GETTER / SETTER
void Particle::SetNext(Particle * nextPtr)
{
	_state.next = nextPtr;
}

Particle* Particle::GetNext()
{
	return _state.next;
}

bool Particle::IsAlive()
{
	if (lifespan > 0)
		return true;
	else
		return false;
}

bool Particle::IsDead()
{
	if (lifespan <= 0)
		return true;
	else
		return false;
}