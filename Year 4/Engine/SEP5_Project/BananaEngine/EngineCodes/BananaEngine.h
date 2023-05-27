#pragma once
#ifndef BANANA_ENGINE
#define BANANA_ENGINE

#include "Precompiled/pch.h"

#include "Maths/MathLib.h"
#include "Maths/Vector3D.h"
#include "Maths/Matrix3D.h"
#include "Maths/Quaternion.h"
#include "Utilities/Random/Random.h"

#include "Debugger/Debugger.h"
#include <Debugger/StopWatch.h>

#include "Systems/ECS/ECS.h"

#include "Engine/GameEngine.h"
#include "Serialisation/BananaSerialisation.h"

#include "Systems/Graphics/Be_descriptors.hpp"
#include "Systems/Graphics/Be_device.hpp"
#include "Systems/Graphics/Be_renderer.hpp"
#include "Systems/Window/Window.h"
#include "Systems/Graphics/Be_buffer.hpp"
#include "Systems/Graphics/Systems/point_light_system.hpp"
#include "Systems/Graphics/Systems/simple_render_system.hpp"
#include "Systems/Graphics/Systems/particle_render_system.hpp"
#include "Systems/Graphics/Offscreen.h"
#include "Systems/Graphics/Camera/Be_camera.hpp"
#include "Systems/Graphics/GridSystem/Grid_System.hpp"
//
#include "AI/FSM/FSM.h"
#include "AI/Astar/AStarPather.h"

#include "Engine/PauseControl.h"



#endif