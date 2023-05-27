#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include <memory>
#include "../../../Math/Vector/Vector2.h"
#include "../../../Math/Vector/Vector3.h"
#include "../../../Math/Vector/Vector4.h"
#include "../../../Graphics/Source/texture.h"

struct Particle
{
	//lifespan, to be the amount of seconds left
	float lifespan = 5.f;
	bool _isAlive = false;
	// IF HAVE time, maybe can try have enum outside to pick the type, so a specific type can have a set of data, that is represented in union.
	 
	//union, data if alive, pointer if dead
	union ParticleDataOrPointer
	{
		//Normal Particle Data
		struct {
			Mathf::vec3 pos;
			Mathf::vec2 vel;
			//Mathf::vec2 acc;
			Mathf::vec2 scale;
			float rotation;
			Mathf::Vector4 color;
			/*
			If a union contains a non-static data member with a non-trivial 
			special member function (copy/move constructor, copy/move assignment, 
			or destructor), that function is deleted by default in the union 
			and needs to be defined explicitly by the programmer.

			If a union contains a non-static data member with a non-trivial default 
			constructor, the default constructor of the union is deleted by default 
			unless a variant member of the union has a default member initializer .

			At most one variant member can have a default member initializer.
			*/
			/* If ever need to add data with non trival c/d-tor, it will be deleted 
			and we need to manully write it here.*/
		} aliveData;
		
		// State when it's available
		Particle* next = nullptr;
	} _state{};

	bool use_texture = true;
	std::string textureId = "Tree3.png"; //putting outside of union because has non trival c-tor, d-tor, wont save any more space either way
	std::vector<Mathf::vec3> VertexArray{}; //Use geometric shape if not using textureID, aka use_texture == false

	Particle() = default;
	~Particle() = default;
	void Init(Mathf::vec3 startPos, float Speed, float lifespan = 10.f, Mathf::vec2 scale = {100.f, 100.f});
	void Update(float dt);
	void Draw();



	//GETTER / SETTER====================================================
	void SetNext(Particle* nextPtr);
	Particle* GetNext();
	bool IsAlive();
	bool IsDead();
	//GETTER / SETTER====================================================
};

#endif
