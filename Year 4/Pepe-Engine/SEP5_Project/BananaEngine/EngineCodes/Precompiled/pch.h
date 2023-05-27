#pragma once
#ifndef PCH_H
#define PCH_H

//External Libraries
#if defined( _WIN32 )
#define VK_USE_PLATFORM_WIN32_KHR 1
#include <Windows.h>
#endif

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//Disable nameless struct warning
#pragma warning(disable : 4201)
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#pragma warning(default : 4201)

//SOIL
#include <SOIL2/SOIL2.h>		//this requires opengl32.lib
#include <SOIL2/image_DXT.h>

//Assimp
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/mesh.h"
#include "assimp/material.h"
#include "assimp/cimport.h" 

//Fmod
#include <fmod.hpp>

//Imgui
#include "../EditorSystem/ImguiStuff/imgui-master/imgui.h"
#include "../EditorSystem/ImguiStuff/imgui-master/imgui.h"
#include "../EditorSystem/ImguiStuff/imgui-master/imgui_impl_glfw.h"
#include "../EditorSystem/ImguiStuff/imgui-master/imgui_impl_vulkan.h"
#include "../EditorSystem/ImguiStuff/IconFont/IconsFontAwesome5.h"
#include "../EditorSystem/ImguiStuff/ImGuizmo-master/ImGuizmo.h"

//Input output
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdarg>
#include <fstream>
#include <filesystem>
#include <sstream>

//STL stuff
#include <random>
#include <utility>
#include <cmath>
#include <algorithm>
#include <memory>
#include <type_traits>
#include <exception>
#include <optional>
#include <functional>
#include <chrono>
#include <ratio>
#include <ranges>

//STL Data structure
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <bitset>
#include <array>
#include <deque>
#include <stack>
#include <queue>
	
//RTTR
#include <rttr/type>
#include <rttr/registration>
#include <rttr/registration_friend>

//JSON stuff
#include <document.h>
#include <writer.h>
#include <prettywriter.h>
#include <stringbuffer.h>
#include <ostreamwrapper.h>

//Jolt stuff
//
#pragma warning( push )
#pragma warning ( disable : 6385 )
#pragma warning ( disable : 6386 )
#pragma warning ( disable : 26495 )
#pragma warning ( disable : 6011 )
#pragma warning ( disable : 6255 )

#include <Jolt/Jolt.h>

#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/Core.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseQuery.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollidePointResult.h>

#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Core/Reference.h>
#include <Jolt/Physics/Character/CharacterBase.h>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
//Shapes
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>
#include <Jolt/Physics/Collision/Shape/OffsetCenterOfMassShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>

#pragma warning( pop )

// Disable common warnings triggered by Jolt
JPH_SUPPRESS_WARNINGS
//Class forward declaration
namespace BE
{
	//Maths
	template <typename T> class Vec3;
	template <typename T> class Mat3;
	template <typename T> class Quaternion;

	//Misc
	class BEID;

	//Engine systems
	class GameEngine;
	class ISystem;

	//Engine Events
	class IEvent;
	class EventDispatcher;
	class QuitEvent;
	class RestartEvent;
	class UnloadSceneEvent;
	class SceneLoadedEvent;
	class SceneRestartingEvent;
	class LoadAssetEvent;
	class KeyButtonEvent;
	class MouseButtonEvent;
	class MouseMoveEvent;
	class MouseScrollEvent;
	class WindowInterruptEvent;
	class WindowToggleFullScreenEvent;
	class WindowResizeEvent;

	//ECS
	class ECSEntityMgr;
	class ICompArray;
	template <typename ComponentType> class ECSCompArray;
	class ECSComponentMgr;
	class IECSTracker;
	class ECSTrackerMgr;
	class ECSCoordinator;

	//ECS Components
	class IECSComponent;
	class Transform;
	class EntityInfo;
	class AudioSource;
	class AudioListener;
	class AnimationComponent;
	class Renderer;
	class lightComponent;
	class cameraComponent;

	//Asset type and Asset Manager
	class AssetManager;
	class AudioClip;

	//Scene Manager
	class Scene;
	class SceneManager;

	//AudioProperties manager
	struct AudioEvent;
	class AudioManager;
	template <typename T>
	std::string RegistrationNameCreator()
	{
		std::string temp = typeid(T).name();
		return temp.substr(temp.find_last_of(':') + 1);
	}

	//User Interface
	class UIElement;
	class UIInteractable;
	class UICanvas;
	class UIManager;
	class UIButton;
	class UIFontText;
	class UIImage;
	class UIProgressBar;
	class UISlider;
	class UIToggle;
	
	//Editor
	class Assets_Window;
	class Console_Window;
	class EditorSystem;
	class ICommand;
	class CreateEntityCommand;
	class CloneEntityCommand;
	class DestroyEntityCommand;
	class AttachEntityCommand;
	class DetachEntityCommand;
	class EntityTag_Window;
	class FPS_Window;
	class Graphics_Window;
	class Hierarchy_Window;
	class Inspector_Window;
	class Menu_Window;
	class Resources_Window;
	class Tool_Bar;
	class UndoSystem;
	class CommandManager;

	// AI
	struct ListNode;

	//Graphics related
	class BeBuffer;
	class BeDevice;
	class BeBone;
	class BeModel;
	class BeDescriptorSetLayout;
	class BeDescriptorPool;
	class BeDescriptorWriter;
	class BePipeline;
	class BeRenderer;
	class BeSwapChain;

	struct GlobalUbo;
}

#endif
