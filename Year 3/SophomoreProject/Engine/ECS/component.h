// /******************************************************************************/
// /*!
// \file         Component.h
// \project name Engine
// \author(s)    Ryan Wang
// \date         8th Dec, 2021
// \brief        Declarations of ECS component types
// \lines        418 Lines
// 
// All content :copyright: 2021 DigiPen (SINGAPORE) Corporation, all rights
// reserved.
// Copyright (C) 2021 DigiPen Institute of Technology.
// Reproduction or disclosure of this file or its contents without the
// prior written consent of DigiPen Institute of Technology is prohibited.
//  */
//  /******************************************************************************/
//Main Author: Ryan Wang
//(However, other programmers are free to add more components into this file)

//This header file serves to contain the definitions of components
//to be used in the engine.

//POINTERS ON COMPONENT DESIGN
//1) A component should just be a struct of data (preferably primitive).
//   The component struct should not contain functions (beyond serialisation)
//   Functions should be handled by Systems instead.
//   It is also not advisable for the struct to contain OTHER structs.
//   (Note 7 Oct 21) Component structs need serialisation functions
//                   because the structs inherit from componentSerializable
//2) Among the most fundamental of components is the Transform.
//   By right, every entity should have a Transform that details its:
//   Positioning, Scaling and Rotation
//3) Try to keep components as small as possible.
//   This makes them easy to maintain and allows faster access within vectors
//   (Vector, which this ECS bases itself on, operations are quicker
//   the smaller in size its elements are)
//   (https://baptiste-wicht.com/posts/2017/05/cpp-containers-benchmark-vector-list-deque-plf-colony.html)
//4) Components should be general. You should forsee many entities containing
//   the component you are making. Much justification is needed for a component
//   that only serves for one or a small handful of entities.
//   (For example, a Player Component type within a Player Entity is justifed)
//5) As to how to think up of a component type, look at objects around where
//   you presently are and what sort of similarities and differences they have
//   from one another. That should serve as a good starting point for
//   brainstorming for component type ideas.

//Header for components
//Try to keep components as small as possible
//So that they are easy to maintain AND
//are well optimised when accessed within component vectors
//(vector operations are quicker the smaller in size its elements are)
//(https://baptiste-wicht.com/posts/2017/05/cpp-containers-benchmark-vector-list-deque-plf-colony.html)
//Because of the aforementioned statement,
//all real numbers within the component structs are represented by
//4 byte floats instead of 8 byte doubles.
//4 byte floats usually have a small error
//in the realm of 1E-8 to 1E-9 of the true value. They also cover a range wide 
//enough for most purposes (Magnitude from ~3.4E-38 to ~3.4E38)
//TODO Documentation

#ifndef COMPONENT_H
#define COMPONENT_H

#include <map>
#include <iostream>
#include <vector> //vector 
#include <cstdint> //Fixed width integers
#include <string> //std::string
#include <memory>
#include <iomanip>
#include <utility>
#include "../Math/Vector/Vector2.h"
using namespace Mathf;

#include "../Graphics/Source/GraphicsApplication.h"

#include "../Math/Vector/Vector2.h"//for vector
#include "../Math/Matrix/Matrix3.h"//mat 3 
#include "../Serializer/SerializeManager.h"
#include "../Serializer/ISerializable.h"
#include "Systems/Physics/ForceManager.h"
#include "Sprite.h"
#include "Systems/MonoHelpStruct.h"
#include "Systems/ParticleSystem/Emitter.h"

//TODO hide components' serialization and ImGui functions
//     by right, they should not be accessible from everywhere

//Virtual class depicting component requirement to render data in ImGui
class componentImGui
{
public:
	virtual ~componentImGui() = default;

	//Render data of component in inspector
	virtual void inspectorView() = 0;

	//List component to add to entity within 'Add Component' menu
	virtual void listView() = 0;
};

//Transform detailing the position, rotation and scaling of the entity
//All entities should have a transform.
//Most, if not all, systems rely on transform.
//https://docs.unity3d.com/Manual/class-Transform.html
//(Ryan)
struct transform2D : public componentSerializable, public componentImGui
{
	//Local coordinates
	//Directly editable via editor
	vec3 pos;
	float rotation; //Present rotation in degrees. Increasing CCW, with 0 degrees representing pointing to the right.
	vec2 scale;

	//World coordinates based on its ancestors (if any)
	//Not directly editable via editor
	//A system will decide what the value of this
	glm::mat4 worldMatrix;

	//Matrix3 transformMatrix{};
 
	//Serialize data to JSON file
	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;

	//Deserialize from JSON object into data
	//Data read is the entire JSON file for now
	bool OnDeserialize(const rapidjson::Value& data) override;

	//Write data into ECS
	//TODO change uint32_t into alias entityID if possible
	void addComponent(uint32_t toEntity) override;

	//Render data into ImGui inspector
	void inspectorView() override;

	//List component to add to entity within 'Add Component' menu 
	void listView() override;
};

//Handles entity's physics and forces (Chun Ren and Zi Xing)
struct rigidBody2D : public componentSerializable, public componentImGui
{
	enum class Types
	{
		Dynamics = 0,//Movable
		Static, //Non-movable
		Kinametics
	};
	bool freezeX = false; //Freeze horizontal movement
	bool freezeY = false; //Freeze vertical movement
	bool freezeR = false; //Freeze rotational movement
	bool isGravityOn{ false };
	bool isRotatable{ false };
	Types Type = Types::Dynamics;

	float rot_vel = 0.f;
	float orientation = 0.f; //rotation
	float Mass = 1.f;
	float InvMass = 1.f; //If static, put as 0, this variable affect velocity and collision response
	float InertiaMass = 1.f;//for rotation, treated same as mass for now
	float InvInertiaMass = 1.f;
	float restitution = 0.0f; //between 0 to 1.f, deter

	vec2 vel = vec2{ 0.f, 0.f };
	vec2 acceleration = vec2{ 0.f, 0.f };
	vec2 prev_pos = vec2{ 0.f, 0.f };
	
	ForceManager forceManager;
	
	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;

	//Deserialize from JSON object into data (reading in)
	//Data read is the entire JSON file for now
	bool OnDeserialize(const rapidjson::Value& data) override;
	void addComponent(uint32_t toEntity) override;
	//Render data into ImGui inspector
	void inspectorView() override;
	//List component to add to entity within 'Add Component' menu 
	void listView() override;
};

enum class CollisionFlag
{
	COLLIDED = 0,
	TOP,
	RIGHT,
	BOTTOM,
	LEFT, 
	NONE
};
//Base collider, to be added for all collidable
//struct Collider : public componentSerializable, public componentImGui
//{
//	enum class CollisionType
//	{
//		Box = 0,
//		Circle,
//		PolygonConvex,
//		Line,
//		Point,
//		NumberOfCollisionType
//	} type;
//
//	bool isOverlap = false;			//if true, it has collided in current frame
//	bool isActive = true;			//if true, will test for collision
//
//	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;
//	bool OnDeserialize(const rapidjson::Value& data) override;
//	void addComponent(uint32_t toEntity) override;
//	void inspectorView() override;
//	void listView() override;
//};

//AABB Collider (Chun Ren and Zi Xing)
struct AABB : public componentSerializable, public componentImGui
{
	vec2 min;
	vec2 max;

	vec2 offset_pos = {0.f, 0.f};// offset from position
	vec2 scale = { 1.f, 1.f }; //scale to be in ratio against transform scale, this NEEDS to be updated when its scaled up
	unsigned int isOverlap = 0;		//if 0, it is not colliding, number of overlapping obj
	bool isActive = true;			//NON FUNCTIONAL if true, will test for collision
	int collisionFlag = static_cast<int>(CollisionFlag::NONE);
	
	//Used when collided
	bool resolvedInX = false; //true once it collided with a static object, or to another object with true in this variable
	bool resolvedInY = false; //true once it collided with a static object, or to another object with true in this variable

	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;

	//Deserialize from JSON object into data
	//Data read is the entire JSON file for now
	bool OnDeserialize(const rapidjson::Value& data) override;
	void addComponent(uint32_t toEntity) override;
	//Render data into ImGui inspector
	void inspectorView() override;
	//List component to add to entity within 'Add Component' menu 
	void listView() override;
};

//For Box2DCollider
namespace {
	const std::vector<vec2> models_box2dCollider = { {-0.5f, 0.5f}, {0.5f, 0.5f}, {0.5f, -0.5f}, {-0.5f, -0.5f} };	// Points in local space determining the base shape
}

struct Box2DCollider : public componentSerializable, public componentImGui
{
	//Provide in clockwise for now
	std::vector<vec2> points = models_box2dCollider; // Collider points in world space as a result of transform, computed per physic update

	vec2 offset_pos = { 0.f, 0.f };	// position of shape
	vec2 scale_ratio = { 1.f, 1.f };//scale ratio against transform
	vec2 xHalfVector;
	vec2 yHalfVector;
	float angle_offset = 0.f;		//direction of the collider against transform, in radian
	bool isAABB = false;			//if true, doesn't rotate
	
	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;
	bool OnDeserialize(const rapidjson::Value& data) override;
	void addComponent(uint32_t toEntity) override;
	void inspectorView() override;
	void listView() override;
};

//Polygon collider (Chun Ren and Zi Xing)
struct Poly : public componentSerializable, public componentImGui
{
	//Provide in clockwise for now // defaulted to a square at the start
	std::vector<vec2> models {
		{ 0.5f, 0.5f }, 
		{ 0.5f, -0.5f }, 
		{ -0.5f, -0.5f }, 
		{ -0.5f, 0.5f }
	};	// Points in local space determining the base shape 
	std::vector<vec2> points{
		{ 0.5f, 0.5f },
		{ 0.5f, -0.5f },
		{ -0.5f, -0.5f },
		{ -0.5f, 0.5f }
	};	// Collider points in world space as a result of transform
	
	vec2 offset_pos = { 0.f, 0.f };					// position of shape
	vec2 scale_ratio = { 1.f, 1.f }; //scale ratio against transform2d in model
	float angle_offset = 0.f;				// direction of the shape
	bool overlap = false;

	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;

	//Deserialize from JSON object into data
	//Data read is the entire JSON file for now
	//TODO DESERIALISATION NON-TRIVIAL. NO IMPLEMENTATION DO NOT CALL
	bool OnDeserialize(const rapidjson::Value& data) override;
	
	void addComponent(uint32_t toEntity) override;

	//Render data into ImGui inspector
	void inspectorView() override;

	//List component to add to entity within 'Add Component' menu 
	void listView() override;
};

//Circle collider (Chun Ren and Zi Xing)
struct Circle : public componentSerializable, public componentImGui
{
	vec2 m_center;
	float m_radius{ 0 };
	float m_mass{ 1.f }; //Ryan: Why is there a mass for the collider? Shouldn't that be settled by rigidBody?

	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;

	//Deserialize from JSON object into data
	//Data read is the entire JSON file for now
	bool OnDeserialize(const rapidjson::Value& data) override;

	void addComponent(uint32_t toEntity) override;

	//Render data into ImGui inspector
	void inspectorView() override;

	//List component to add to entity within 'Add Component' menu 
	void listView() override;
};

//Ray collider (Chun Ren and Zi Xing)
struct Ray : public componentSerializable, public componentImGui
{
	Vec2	m_pt0;
	Vec2	m_dir;

	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;

	//Deserialize from JSON object into data
	//Data read is the entire JSON file for now
	bool OnDeserialize(const rapidjson::Value& data) override;

	void addComponent(uint32_t toEntity) override;

	//Render data into ImGui inspector
	void inspectorView() override;

	//List component to add to entity within 'Add Component' menu 
	void listView() override;
};

/******************************************************
* @ Brief : this is for the graphics components
*******************************************************/

/**********************************
* @Brief - Material Component (Wilfred)
**********************************/
struct material : public componentSerializable, public componentImGui {
	std::string shader;//file location of shader
	std::string mesh;//indicates the type of mesh 
	Mathf::Vector4 color; //stores the color of the object 

	//for the renderer //smart pointers
	std::shared_ptr<VertexArray> vertexArrayId; 
	std::shared_ptr<VertexBuffer> vertexBufferId;
	std::shared_ptr<IndexBuffer> indexBufferId;
	std::shared_ptr<Shader> shaderId;
	std::shared_ptr<Texture> textureId;

	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;

	//Deserialize from JSON object into data
	//Data read is the entire JSON file for now
	//TODO eventually there should be a string detailing the mesh
	bool OnDeserialize(const rapidjson::Value& data) override;

	void addComponent(uint32_t toEntity) override;

	//Render data into ImGui inspector
	void inspectorView() override;

	//List component to add to entity within 'Add Component' menu 
	void listView() override;
};


/**********************************
* @Brief - Sprite Components (Wilfred)
**********************************/
struct spriteCom : public componentSerializable, public componentImGui {
	//Mathf::Vector2 frameDimensions;//frame width and height
	//Mathf::Vector2 spriteSheetDimensions;//sprite sheet width and height 

	std::vector<size_t> currentAnimation; //Tracks current frames of animation

	size_t startFrame;//tracks the starting frame (DEPRECATE)
	size_t currentFrame; //tracks the current frame INDEX of the sprite 
	size_t endFrame;//tracks the end frame number (DEPRECATE)

	float offSetStartingPos; //offset of the starting position of the frame 
	bool loop;//loop sprite sheet 
	bool isVisible = true;// true if visible 
	bool toLoaded{ false };

	int columns{ 4 }; //column for sprite sheet
	int rows{ 4 };//rows for sprite sheet 
	float speed; //speed of animation, the framerate of the animation in FPS
	bool spriteIdleStart = false;//indicates if the idle start frame is loaded

	//Between 0 and 1. For proceeding with the animation
	//Is decreased in animation until hitting 0, which should signal that
	//the next frame is to be shown
	float frameLifetimeRemaining{ 0.0f };

	std::array<Mathf::Vector2,4> currentTexCoords;//current texture coords

	//animation data 
	std::unordered_map<std::string, std::vector<size_t>> animationData;//stores the animation name & animation frame index if the to loop is true TODO: multithreading try 
	std::vector<std::array<Mathf::Vector2, 4>> frameData;//stores the four corners for all frames in the sprite sheet
	std::string animationFilePath;//file path for the animation 
	int totalNumOfFrames;//tracks the total number of frames

	std::string textureId;//texture path 
	float textureSlotsId;//texture slot unit 
	//std::shared_ptr<Sprite> spriteId;//shared ptr to the sprite id 

	//(12 Apr)
	//This is a final attempt to fix animations not being loaded in
	bool animationLoaded{ false };
	std::string animationToLoad;

	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;

	//Deserialize from JSON object into data
	//Data read is the entire JSON file for now
	bool OnDeserialize(const rapidjson::Value& data) override;

	void addComponent(uint32_t toEntity) override;

	//Render data into ImGui inspector
	void inspectorView() override;

	//List component to add to entity within 'Add Component' menu 
	void listView() override;

	//(1 Mar)
	//Sets current animation of the sprite if found. If not, does nothing
	void setAnimation(std::string const& newAnimation);
};

/**********************************
* @Brief - Camera Components  (Wilfred)
**********************************/
struct camera2DCom : public componentSerializable, public componentImGui {
	//Mathf::Vector2 right, up; //right and up vector 
	//glm::mat3 view_xform, camwin_to_ndc_xform, world_to_ndc_xform;//camera matrix 

	//window parameters
	float height{ 200.f };
	float aspectRatio;
	//window parameters that changes 
	float min_height{ 50 }, max_height{ 2000 };
	//increments by which window height is changed per Z key press
	int height_change_val{ 5 };
	//camera's speed when button U is pressed 
	float linear_speed{ 2.f };

	float translation_speed{ 20.f };//translation speed 

	//TODO DEPRECATED, DELETE
	Mathf::vec3 cam_position{ 100.f,100.f,1.f };//starting position of camera 

	bool isFixedToPlayer{ true };//set if it's fixed to the player 

	bool isPrimary{ true };//is the camera the primary camera?

	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;

	//Deserialize from JSON object into data
	//Data read is the entire JSON file for now
	bool OnDeserialize(const rapidjson::Value& data) override;

	void addComponent(uint32_t toEntity) override;

	//Render data into ImGui inspector
	void inspectorView() override;

	//List component to add to entity within 'Add Component' menu 
	void listView() override;
};
extern transform2D* activeCameraTransform;
extern camera2DCom* activeCameraData;
//extern transform2D* activeCameraTrans;
//extern unsigned int activeCameraID;


/******************************************************
* end of graphics component
*******************************************************/


//Player Controller (Chun Ren)
struct controller : public componentSerializable, public componentImGui
{
	//Maybe directions or key press?
	//Or maybe scripting??
	enum class Direction
	{
		UP,
		RIGHT,
		DOWN,
		LEFT
	};
	Direction currDir = Direction::UP;
	//Direction lastDir = Direction::UP; // do we need?
	bool up = false;
	bool right = false;
	bool down = false;
	bool left = false;
	char lives_remaining = 1;
	int HP = 10;

	int numOfTomatoesCaptured = 0;//tracks the number of tomatoes captured
	int numOfMushroomsCaptured = 0;//tracks the number of mushrooms captured

	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;

	bool OnDeserialize(const rapidjson::Value& data) override;

	void addComponent(uint32_t toEntity) override;

	//Render data into ImGui inspector
	void inspectorView() override;

	//List component to add to entity within 'Add Component' menu 
	void listView() override;
};



/******************************************************
* @ Brief : START of AStar PathFinding Component
*******************************************************/
//Component for Path Finding 
struct Com_FindPath : public componentSerializable, public componentImGui{

	bool	_find{ false }; //if it's finding path 
	bool	_found{ false }; //if path is found 
	bool	_reached{ false }; //if path is reached next to it
	static inline bool targetPosInitialized{ false };//indicates if the player's position has been intitialized 
	static inline Mathf::Vec2 targetPos; //the target's position 

	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;

	bool OnDeserialize(const rapidjson::Value& data) override;

	void addComponent(uint32_t toEntity) override;

	//Render data into ImGui inspector
	void inspectorView() override;

	//List component to add to entity within 'Add Component' menu 
	void listView() override;

};

/******************************************************
* @ Brief : END of AStar PathFinding 
*******************************************************/


/******************************************************
* @ Brief : START of Enemy States 
*******************************************************/

//Components for the type of enemys
struct Com_type : public componentSerializable , public componentImGui {
	int	type{ 0 };//current type of the entity 
	enum type {
		player,
		enemy,
		enemyrange,
		camera,
		gui,
		obstacles,
		bullet,
		spawner,
		enemyBullet,
		bush,
		blockage,
		boulder
	};

	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;

	bool OnDeserialize(const rapidjson::Value& data) override;

	void addComponent(uint32_t toEntity) override;

	//Render data into ImGui inspector
	void inspectorView() override;

	//List component to add to entity within 'Add Component' menu 
	void listView() override;
};

struct Com_Health : public componentSerializable, public componentImGui {
	int health{ 3 };
	bool isActive = true;

	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;

	bool OnDeserialize(const rapidjson::Value& data) override;

	void addComponent(uint32_t toEntity) override;

	//Render data into ImGui inspector
	void inspectorView() override;

	//List component to add to entity within 'Add Component' menu 
	void listView() override;
};

struct Com_EnemyStates : public componentSerializable , public componentImGui {

	enum class states {
		IDLE,
		MOVE,
		ATTACK,
	}_current_state{ states::IDLE };

	//TODO float
	int		_speed{ 2 };//the speed of the enemy
	int		_counter{ _speed };//counter 
	bool  reached{ false };//check if the enemy has reached the player's location 

	//Com_TilePosition* _player;//set the player's tile position 

	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;

	bool OnDeserialize(const rapidjson::Value& data) override;

	void addComponent(uint32_t toEntity) override;

	//Render data into ImGui inspector
	void inspectorView() override;

	//List component to add to entity within 'Add Component' menu 
	void listView() override;
};

/******************************************************
* @ Brief : END of Enemy States 
*******************************************************/

/******************************************************
* @ Brief : Font Rendering Component
*******************************************************/

struct Com_Font : public componentSerializable, public componentImGui {

	Mathf::vec3 color{ 1.f,1.f,1.f };//stores the color of the font 
	std::string text;//the text to render out 

	bool isVisible;//indicates if the font is visible 

	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;

	bool OnDeserialize(const rapidjson::Value& data) override;

	void addComponent(uint32_t toEntity) override;

	//Render data into ImGui inspector
	void inspectorView() override;

	//List component to add to entity within 'Add Component' menu 
	void listView() override;
};

/******************************************************
* @ Brief : GUI Component
*******************************************************/

struct Com_GUI : public componentSerializable, public componentImGui {
	
	bool isVisible{ true };//indicates if the GUI is visible 
	static inline Mathf::vec3 CameraPos;//stores the current camera position 
	static inline Mathf::vec3 prevCameraPos;//stores the previous camera position 
	static inline bool preCamPosSet = false;//indicates if the previous camera position has been set 

	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;

	bool OnDeserialize(const rapidjson::Value& data) override;

	void addComponent(uint32_t toEntity) override;

	//Render data into ImGui inspector
	void inspectorView() override;

	//List component to add to entity within 'Add Component' menu 
	void listView() override;
};

/******************************************************
* @ Brief : Level Loader Button
*******************************************************/

struct levelLoaderButton : public componentSerializable, public componentImGui
{
	std::string targetLevelFile;

	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;

	bool OnDeserialize(const rapidjson::Value& data) override;

	void addComponent(uint32_t toEntity) override;

	//Render data into ImGui inspector
	void inspectorView() override;

	//List component to add to entity within 'Add Component' menu 
	void listView() override;
};

struct scriptBase
{
	//CPP Scripting
	//base data
	std::string name; //name identifier
	enum class ScriptState {
		START,
		UPDATE,
		END
	} state = ScriptState::START;
	unsigned int id;

	//functions
	scriptBase(std::string scriptName) : name{ scriptName } {};
	virtual void init(float dt, unsigned int idToBeSet) = 0; //start
	virtual void update(float dt) = 0; //every update frame
	virtual void end(float dt) = 0; //end
	std::string getScriptName() { return name; }
private:
	friend class scriptManager;
	virtual std::shared_ptr<scriptBase> createCopy() = 0;
};
struct scriptComponent : public componentSerializable, public componentImGui
{
	std::map<std::string, std::shared_ptr<scriptBase> > scriptContainer{};
	unsigned int belongToEntityId;

	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;
	bool OnDeserialize(const rapidjson::Value& data) override;
	void addComponent(uint32_t toEntity) override;
	void inspectorView() override;
	void listView() override;
};

struct CSharpScriptComponent : public componentSerializable, public componentImGui 
{
	std::map <std::string, MonoClassObject> _scriptMap; //all the script for an entity
	unsigned int belongToEntityId;

	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;
	bool OnDeserialize(const rapidjson::Value& data) override;
	void addComponent(uint32_t toEntity) override;
	//Render data into ImGui inspector
	void inspectorView() override;
	//List component to add to entity within 'Add Component' menu 
	void listView() override;
};

//#define getMonoClassPtr _scriptMap[s.first].second

/******************************************************
* @ Brief : Abilties Component
*******************************************************/

struct Com_Abilities : public componentSerializable, public componentImGui {

	int	typeAbilities{ 0 };//current type of the entity 
	enum abilities {
		waterCannon,
		superSpeed,
		shield,
		fireball
	};

	bool isShieldLoaded = false; //bool to indicate if the shield is loaded 

	//TODO NO IMPLEMENTATION. DO NOT CALL
	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;

	//TODO NO IMPLEMENTATION. DO NOT CALL
	bool OnDeserialize(const rapidjson::Value& data) override;

	void addComponent(uint32_t toEntity) override;

	//Render data into ImGui inspector
	void inspectorView() override;

	//List component to add to entity within 'Add Component' menu 
	void listView() override;
};

/******************************************************
* @ Brief : Enemy Spawner Component
*******************************************************/
struct Com_EnemySpawner : public componentSerializable, public componentImGui {

	int numOfSpawns;//the number of spawns for the enemy 
	float timeDelay;//the time delay is seconds before every spawn 
	float currentTimer = 0.f;//tracks the time in seconds
	int typeOfEnemy;//the type of enemy to spawn 

	//the type of enemies 
	enum enemyType {
		tomato,
		onion,
		pumpkin,
		chilli
	};

	//TODO NO IMPLEMENTATION. DO NOT CALL
	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;

	//TODO NO IMPLEMENTATION. DO NOT CALL
	bool OnDeserialize(const rapidjson::Value& data) override;

	void addComponent(uint32_t toEntity) override;

	//Render data into ImGui inspector
	void inspectorView() override;

	//List component to add to entity within 'Add Component' menu 
	void listView() override;
};

/******************************************************
* @ Brief : Time Destructor Component 
*******************************************************/
struct Com_TimeDestructor : public componentSerializable, public componentImGui {

	float timeDelayDestructor;//the time delay till it kills itself 

	//TODO NO IMPLEMENTATION. DO NOT CALL
	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;

	//TODO NO IMPLEMENTATION. DO NOT CALL
	bool OnDeserialize(const rapidjson::Value& data) override;

	void addComponent(uint32_t toEntity) override;

	//Render data into ImGui inspector
	void inspectorView() override;

	//List component to add to entity within 'Add Component' menu 
	void listView() override;
};


/******************************************************
* @ Brief : Dialogue Component
*******************************************************/
struct Com_Dialogue : public componentSerializable, public componentImGui {
	
	bool timeTrigger;//indicates if it's triggered by time 
	std::string dialogueTexture;//stores the dialogue texture 
	std::string dialogueText;//stores the dialogue text 
	bool isDialogueLoaded = false; //bool to indicate if the dialogue is loaded 
	bool isToShow = true;//indicates if it should show the dialogue again if it enters the proximity 
	bool isNewlyLoaded = false;//bool to indicate if the dialogue is newly loaded 


	bool isClicked = false;//tracks if the dialogue is clicked 

	float offSetYPos;//set the offset of the y 
	float offSetXPos;//set the offset of the x

	//TODO NO IMPLEMENTATION. DO NOT CALL
	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;

	//TODO NO IMPLEMENTATION. DO NOT CALL
	bool OnDeserialize(const rapidjson::Value& data) override;

	void addComponent(uint32_t toEntity) override;

	//Render data into ImGui inspector
	void inspectorView() override;

	//List component to add to entity within 'Add Component' menu 
	void listView() override;
};

struct EmitterComponent : public componentSerializable, public componentImGui
{
	//std::shared_ptr<Emitter> emitter = std::make_shared<Emitter>();
	//std::vector< std::shared_ptr<Emitter> > EmitterContainer{ std::make_shared<Emitter>() };
	//std::vector< std::shared_ptr<Emitter> > emitter{ std::make_shared<Emitter>() };
	
	//using map instead of vector because of the need to serialise and deserialise.
	//a way to identify the file that is being used
	std::map<std::string, std::shared_ptr<Emitter> > emitterContainer{};

	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;
	bool OnDeserialize(const rapidjson::Value& data) override;
	void addComponent(uint32_t toEntity) override;
	void inspectorView() override;
	void listView() override;
};


/******************************************************
* @ Brief : Dialogue Manager Component
*******************************************************/
struct Com_DialogueManager : public componentSerializable, public componentImGui {

	std::string dialogueStorage;//stores the dialogue 
	float textSpeed; //the speed of iterating through each character 

	bool isSerialized = false;//tracks if the dialogue is serialized 
	bool isActive = false;//tracks if the dialogue manager is active 
	bool isCompleted = false;//tracks if the individual dialogue is completed 
	bool toLoadTextFully = false;//tracks if you want to load the text fully 

	bool allFullyLoaded = false;//tracks if all the dialogue is fully loaded 

	bool isPrevLoaded = false;//tracks if there is a previous message before this 
	float currentTimer = 0;//timer 
	int textIndex = 0;//tracks the index of the text

	int secondEntityID = -1;//tracks the second entityid 

	//std::vector<entityID> extendedDialogue; //stores all the extended dialogue that is out of the scope 

	//TODO NO IMPLEMENTATION. DO NOT CALL
	bool OnSerialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const override;

	//TODO NO IMPLEMENTATION. DO NOT CALL
	bool OnDeserialize(const rapidjson::Value& data) override;

	void addComponent(uint32_t toEntity) override;

	//Render data into ImGui inspector
	void inspectorView() override;

	//List component to add to entity within 'Add Component' menu 
	void listView() override;
};

#endif //COMPONENT_H