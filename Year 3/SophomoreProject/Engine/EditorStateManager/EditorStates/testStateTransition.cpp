//TODO Documentation
//Sample game state to showcase state change

#include <iostream>
#include "gameState.h"
#include "../gameStateManager.h"

testStateTransition g_testStateTransition;

void testStateTransition::load()
{
	std::cout << "testStateTransition::load()" << std::endl;
}

void testStateTransition::init()
{
	std::cout << "testStateTransition::init()" << std::endl;
}

void testStateTransition::input(float dt)
{
	std::cout << "testStateTransition::input(" << dt << ")" << std::endl;
}

void testStateTransition::update(float dt)
{
	std::cout << "testStateTransition::update(" << dt << ")" << std::endl;
	gameStateManager::quitGame();
}

void testStateTransition::render(float dt)
{
	std::cout << "testStateTransition::render(" << dt << ")" << std::endl;
}

void testStateTransition::free()
{
	std::cout << "testStateTransition::free()" << std::endl;
}

void testStateTransition::unload()
{
	std::cout << "testStateTransition::unload()" << std::endl;
}