//Game state used to test the ECS architecture. Presently unused
#if 0
#include "../../pch.h"
#include <iostream>
#include <random>
#include <chrono>
#include "gameState.h"
#include "../gameStateManager.h"
#include "../../ECS/ECS.h"

testStateECS g_testStateECS;

//WHAT SHOULD BE MADE TO TEST THE ECS

//SOME COMPONENTS
//Transform: Position (X/Y), Rotation (Single Axis) and Scale (X/Y) values. (All objects must have a transform)
//https://docs.unity3d.com/Manual/class-Transform.html
//Physics: Velocity, Acceleration, Mass, Drag, RotDrag, FreezeX/Y, FreezeRot
//ColliderAABB: CentreOffset (0,0 represents centre), WidthScale (Default 1.0, based off X scale of transform), HeightScale (Default 1.0)
//Image: Imagefile

//WHAT TO DO TO TEST THE ECS?
//FIRST TIMING
//First, create 2 000 000 entities with TRANSFORM components
//SECOND TIMING
//Of these 2 000 000 entities,
//randomly assign 1 000 000 to have PHYSICS components,
//randomly assign 2 000 to have a RENDER component (rendering by printing to std::cout now)
//THIRD TIMING
//COMPLETELY delete 1 000 000 entities at random with their components.

//Number of reruns of this state to test state restart.
static int restarts = 1;

static ECS ecs;

//Test timings
static std::chrono::duration<float> testDuration;
static std::chrono::steady_clock::time_point testStartTime;
static std::chrono::steady_clock::time_point testEndTime;

//Test parameters
static constexpr size_t entitiesToCreate = 10;
static constexpr size_t initialSimulationLoops = 10;
static size_t remainingSimulationLoops;

//TODO move this to its own file
class physicsSystemTest : public systemBase
{
public:
	physicsSystemTest(bool requireComponentMatch = false) :
		systemBase(requireComponentMatch)
	{

	}

	void update(float dt)
	{
		for (auto const& e : getEntitiesRegistered())
		{
			//std::cout << "Getting transform of #";
			auto& transform = ecs.getComponent<transform2D>(e);
			//std::cout << "Getting physics of #";
			auto& physics = ecs.getComponent<physics2D>(e);

			//Velocity change by acceleration
			physics.velocityX += physics.accelerationX * dt;
			physics.velocityY += physics.accelerationY * dt;
			physics.velocityR += physics.accelerationR * dt;

			//Velocity change by drag (provided the object has mass)
			float velocityReductionX = 0.0f;
			float velocityReductionY = 0.0f;
			float velocityReductionR = 0.0f;
			if (physics.mass != 0.0f)
			{
				velocityReductionX =
					0.5f * physics.fluidDensity *
					physics.velocityX * physics.velocityX *
					physics.linearDragCoefficient *
					transform.scaleX * physics.scaleZ /
					physics.mass * dt;
				velocityReductionY =
					0.5f * physics.fluidDensity *
					physics.velocityY * physics.velocityY *
					physics.linearDragCoefficient *
					transform.scaleY * physics.scaleZ /
					physics.mass * dt;
				//Angular drag formula and derivation:
				//https://physics.stackexchange.com/questions/304742/angular-drag-on-body
				float averageRadius = (transform.scaleX + transform.scaleY) / 2.0f; //Estimated radius of object by average of length and height
				velocityReductionR =
					0.5f * physics.fluidDensity *
					averageRadius * averageRadius *
					physics.velocityR * physics.velocityR / 57.296f / 57.296f * //57.296 being number of degrees in a radian because angular velocity is in radians per second
					2.0f * 3.141592654f * averageRadius *
					physics.scaleZ * physics.angularDragCoefficient /
					physics.mass * dt;
				if (velocityReductionX > fabsf(physics.velocityX))
					physics.velocityX = 0.0f;
				else
				{
					if (physics.velocityX > 0.0f)
						physics.velocityX -= velocityReductionX;
					else physics.velocityX += velocityReductionX;
				}
				if (velocityReductionY > fabsf(physics.velocityY))
					physics.velocityY = 0.0f;
				else
				{
					if (physics.velocityY > 0.0f)
						physics.velocityY -= velocityReductionY;
					else physics.velocityY += velocityReductionY;
				}
				if (velocityReductionR > fabsf(physics.velocityR))
					physics.velocityR = 0.0f;
				else
				{
					if (physics.velocityR > 0.0f)
						physics.velocityR -= velocityReductionR;
					else physics.velocityR += velocityReductionR;
				}
			}

			//Position change by velocity
			transform.positionX += physics.velocityX * dt;
			transform.positionY += physics.velocityY * dt;
			transform.rotation += physics.velocityR * dt;
			if (transform.rotation > 360.0f) transform.rotation -= 360.0f;
			if (transform.rotation < 0.0f) transform.rotation += 360.0f;
		}
	}
};
std::shared_ptr<physicsSystemTest> physicsSysTest;

//TODO move this to its own file
class renderSystemTest : public systemBase
{
public:
	renderSystemTest(bool requireComponentMatch = false) :
		systemBase(requireComponentMatch)
	{

	}

	void update(float dt)
	{
		for (auto const& e : getEntitiesRegistered())
		{
			auto& transform = ecs.getComponent<transform2D>(e);
			auto& render = ecs.getComponent<imageNumberTest>(e);

			//Manipulate the render image number directly
			render.imageNumber += transform.positionX;
			render.imageNumber -= transform.positionY;
			render.imageNumber *= transform.scaleX;
			render.imageNumber /= transform.scaleY;
		}
	}
};
std::shared_ptr<renderSystemTest> renderSysTest;


void testStateECS::load()
{

}

void testStateECS::init()
{
	remainingSimulationLoops = initialSimulationLoops;

	//TEST 0: Initialise ECS
	testStartTime = std::chrono::high_resolution_clock::now();

	//Register systems
	physicsSysTest = ecs.registerSystem<physicsSystemTest>();
	ecs.setSystemComponentRequirement<physicsSystemTest, transform2D>();
	ecs.setSystemComponentRequirement<physicsSystemTest, physics2D>();

	//registering render system 
	renderSysTest = ecs.registerSystem<renderSystemTest>();
	ecs.setSystemComponentRequirement<renderSystemTest, transform2D>();//setting up transform component 
	ecs.setSystemComponentRequirement<renderSystemTest, imageNumberTest>();

	//Then reserve vectors. Do this only after registering systems
	ecs.reserveAll(entitiesToCreate);

	testEndTime = std::chrono::high_resolution_clock::now();
	testDuration = testEndTime - testStartTime;
	std::cout << "TEST 0 - INIT ECS: " << testDuration.count() <<
		" SECONDS\n\n";

	//Initialise randoms
	std::random_device r;
	std::default_random_engine gen(r());

	std::uniform_real_distribution<float> rand(0.0f, 1.0f);

	std::uniform_real_distribution<float> randPosition(-100.0f, 100.0f);
	std::uniform_real_distribution<float> randRotation(0.0f, 360.0f);
	std::uniform_real_distribution<float> randScale(1.0f, 5.0f);


	for (entityID i = 0; i < entitiesToCreate; ++i)
	{
		transform2D randomTransform;
		randomTransform.positionX = randPosition(gen);
		randomTransform.positionY = randPosition(gen);
		randomTransform.rotation = randRotation(gen);
		randomTransform.scaleX = randScale(gen);
		randomTransform.scaleY = randScale(gen);

		ecs.addComponent<transform2D>(randomTransform);
	}

	testEndTime = std::chrono::high_resolution_clock::now();
	testDuration = testEndTime - testStartTime;
	std::cout << "TEST 1 - CREATE NEW ENTITIES WITH TRANSFORM: " << 
		testDuration.count() << " SECONDS\n";
	std::cout << "RATE: " << testDuration.count() / entitiesToCreate <<
		"s/creation" << std::endl << std::endl;

	//TEST 2: Give 50% of entities a physics component
	testStartTime = std::chrono::high_resolution_clock::now();

	std::uniform_real_distribution<float> randMass(0.1f, 10.0f);
	std::uniform_real_distribution<float> randVel(-10.0f, 10.0f);
	std::uniform_real_distribution<float> randAcl(-10.0f, 10.0f);
	std::uniform_real_distribution<float> randDrag(0.01f, 2.3f);

	size_t numberWithPhysics = 0;

	for (entityID i = 0; i < entitiesToCreate; ++i)
	{
		if (rand(gen) < 0.5f)
		{
			physics2D randomPhysics;
			randomPhysics.mass = randMass(gen);
			randomPhysics.linearDragCoefficient = randDrag(gen);
			randomPhysics.angularDragCoefficient = randDrag(gen);
			randomPhysics.scaleZ = randScale(gen);
			randomPhysics.fluidDensity = 1.25f;
			randomPhysics.velocityX = randVel(gen);
			randomPhysics.velocityY = randVel(gen);
			randomPhysics.velocityR = randVel(gen);
			randomPhysics.accelerationX = randAcl(gen);
			randomPhysics.accelerationY = randAcl(gen);
			randomPhysics.accelerationR = randAcl(gen);

			ecs.addComponent(i, randomPhysics);
			++numberWithPhysics;
		}
	}
	testEndTime = std::chrono::high_resolution_clock::now();
	testDuration = testEndTime - testStartTime;
	std::cout << "TEST 2 - ASSIGN 50% WITH PHYSICS: " << 
		testDuration.count() << " SECONDS\n";
	std::cout << "WITH PHYSICS: " << numberWithPhysics << std::endl;
	std::cout << "RATE: " << testDuration.count() / numberWithPhysics <<
		"s/assign" << std::endl << std::endl;

	//TEST 3: Give 10% of entities a render component
	testStartTime = std::chrono::high_resolution_clock::now();

	std::uniform_int_distribution<int> randImage(0, 10000);

	size_t numberWithRender = 0;

	for (entityID i = 0; i < entitiesToCreate; ++i)
	{
		if (rand(gen) < 0.1f)
		{
			imageNumberTest image = imageNumberTest{ randImage(gen) };
			ecs.addComponent(i, image);
			++numberWithRender;
		}
	}
	testEndTime = std::chrono::high_resolution_clock::now();
	testDuration = testEndTime - testStartTime;
	std::cout << "TEST 3 - ASSIGN 10% WITH RENDER: " << testDuration.count() <<
		" SECONDS\n";
	std::cout << "WITH RENDER: " << numberWithRender << std::endl;
	std::cout << "RATE: " << testDuration.count() / numberWithRender <<
		"s/assign" << std::endl << std::endl;

	//TEST 4A: Delete 50% of the entities at random
	testStartTime = std::chrono::high_resolution_clock::now();

	size_t numberDeleted = 0;

	//Reverse for loop because it is faster to access vectors from the back
	//Conditional is still i < entitiesToCreate because i is unsigned
	for (entityID i = entitiesToCreate - 1; i < entitiesToCreate; --i)
	{
		if (rand(gen) < 0.5f)
		{
			ecs.deleteEntity(i);
			++numberDeleted;
		}
	}
	testEndTime = std::chrono::high_resolution_clock::now();
	testDuration = testEndTime - testStartTime;
	std::cout << "TEST 4A - RANDOM DELETE 50%: " << testDuration.count() <<
		" SECONDS\n";
	std::cout << "DELETED: " << numberDeleted << std::endl;
	std::cout << "RATE: " << testDuration.count() / numberDeleted <<
		"s/delete" << std::endl << std::endl;

	//TEST 4B: Duplicate entities
	testStartTime = std::chrono::high_resolution_clock::now();

	ecs.duplicateEntity(0, entitiesToCreate / 2);
	ecs.getSignatureOf(0);

	std::cout << "TEST 4B - DUPLICATE ENTITY 0: " << testDuration.count() <<
		" SECONDS\n";
	std::cout << "DUPLCIATE: " << entitiesToCreate / 2 << std::endl;
	std::cout << "RATE: " << 2 * testDuration.count() / entitiesToCreate <<
		"s/delete" << std::endl << std::endl;
}

void testStateECS::input(float dt)
{
	static_cast<void>(dt);//unreferenced variable 
	//std::cout << "testStateECS::input(" << dt << ")" << std::endl;
}

void testStateECS::update(float dt)
{
	std::cout << "testStateECS::update(" << dt << ")" << std::endl;

	//TEST 5: 1000 simulation loops
	if (--remainingSimulationLoops != 0)
	{
		physicsSysTest->update(dt);
	}
	else
	{
		gameStateManager::quitGame();
	}
}

void testStateECS::render(float dt)
{
	renderSysTest->update(dt);
}

void testStateECS::free()
{
	ecs.resetECS(); //resetting ecs systems 
}

void testStateECS::unload()
{
	testEndTime = std::chrono::high_resolution_clock::now();
	testDuration = testEndTime - testStartTime;
	std::cout << "TEST 5 - " << initialSimulationLoops <<
		" SIMULATION LOOPS: " << testDuration.count() <<
		" SECONDS\n";
	std::cout << "RATE: " << testDuration.count() / initialSimulationLoops <<
		"s/loop" << std::endl << std::endl;
}
#endif