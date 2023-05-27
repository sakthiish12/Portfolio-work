#pragma once
#ifndef	EVENTS_H
#define EVENTS_H

#include <Precompiled/pch.h>
#include <Maths/Vector3D.h>
#include <Debugger/Debugger.h>
#include <Systems/SceneManager/Scene.h>
#include <Systems/ECS/ECSConfig.h>
#include <Systems/InputController/KeyCodes.h>

namespace BE
{
	//Event types that the engine will be processing throughout


	//The types of events that can be handled by the event manager
	enum class EventType
	{
		//Application
		quit = 0,				//For when the application needs to close
		restart,				//For when the application needs to restart

		//Windows
		windowInterrupt,		//For when window is interrupted/lost focus on/off
		toggleFullScreen,		//For when window is toggled fullscreen on/off
		windowResize,			//For when window is resized

		//Inputs - Please use input controller's GetKey/Mouse functions if you want to query inputs, these are for input controller only
		keyButton,				//For when a key button is pressed
		mouseButton,			//For when a mouse button is pressed
		mouseMove,				//For when a mouse button is moved
		mouseScroll,			//For when mouse scrollwheel is scrolled

		//Scenes
		unloadScene,			//For when the current scene is going to start the unloading process, like changing to a new scene
		sceneFinishedLoading,	//For when a new scene has just finished loading (asset, entities, UI)
		restartScene,			//For when the current scene going to restart
		assetsLoaded,			//For when all required assets have been loaded

		//Miscellaneous
		collision,				//For when collision happened
		entityDestroy,			//For when entity is destroyed
		pause,					//For when game is paused

		//...
		last
	};

	//Base class for all event type structs
	class IEvent
	{
	protected:
		bool m_handled = false;		//If true, disable the event from getting propagated further
		
		IEvent() = default;
		virtual ~IEvent() = 0 {}

	public:
		//Handling of event. If want to stop event from propagating further, call MarkAsHandled
		inline void MarkAsHandled()					{ m_handled = true; }
		inline bool IsHandled() const				{ return m_handled; }

		//Getters
		virtual inline EventType GetEventType() const = 0;

		//For debug
		virtual inline const char* GetEventName() const = 0;							//Get the name of this event
		virtual void LogEvent() const				{ LogCore("%s", GetEventName()); }	//Logs the event into the debugger/console

		friend class EventDispatcher;
	};

	//	--------------------------------V Define all events here V--------------------------------------------

	//Add this line to the event class that you are creating. type is an enum value in EventType.
	#define EVENT_TYPE(type)	inline static EventType GetStaticType()  			{ return EventType::##type; } \
								inline EventType GetEventType() const override		{ return GetStaticType(); } \
								inline const char* GetEventName() const override	{ return "Event: "#type; }

	//GetStaticType() will return the event type for that class.
	//GetEventType() will return the event type for the event object calling it.
	//GetEventName() prints the event type as a string, for debugging. 
	//GetEventInfo() prints more detailed info for the event, for debugging.

	//	------------------------------------------------------------------------------------------------------

	//For when the application is quitting
	class QuitEvent : public IEvent
	{
	public:
		QuitEvent() = default;
		EVENT_TYPE(quit)
	};

	//For when restarting the application
	class RestartEvent : public IEvent
	{
	public:
		RestartEvent() = default;
		EVENT_TYPE(restart)
	};



	//For when window lost/gain focus
	class WindowInterruptEvent : public IEvent
	{
		bool m_hasLostFocus;
	public:
		WindowInterruptEvent(bool _lostFocus) : m_hasLostFocus{ _lostFocus } {}
		inline bool HasLostFocus() { return m_hasLostFocus; }
		EVENT_TYPE(windowInterrupt)
	};

	//When toggled full screen
	class WindowToggleFullScreenEvent : public IEvent
	{
		bool m_isFullScreen;
	public:
		WindowToggleFullScreenEvent(bool _isFullScreen) : m_isFullScreen{ _isFullScreen } {}
		inline bool IsFullScreen() { return m_isFullScreen; }
		EVENT_TYPE(toggleFullScreen)
	};

	//When window resized
	class WindowResizeEvent : public IEvent
	{
		std::pair<int,int> m_newSize;
	public:
		WindowResizeEvent(int _w, int _h) : m_newSize{ _w, _h } {}
		inline const std::pair<int, int>& IsFullScreen() const { return m_newSize; }
		EVENT_TYPE(windowResize)
	};

	//For when a scene is about to unload and a new scene is going to load
	class UnloadSceneEvent : public IEvent
	{
		Scene newScene;

	public:
		UnloadSceneEvent(const Scene& _newScene) : newScene(_newScene) {}
		inline const Scene& GetNextScene() { return newScene; }

		EVENT_TYPE(unloadScene)
	};

	//For when a scene finished loading
	class SceneLoadedEvent : public IEvent
	{
	public:
		SceneLoadedEvent() = default;
		EVENT_TYPE(sceneFinishedLoading)
	};

	//For when a scene needs to restart
	class SceneRestartingEvent : public IEvent
	{
	public:
		SceneRestartingEvent() = default;
		EVENT_TYPE(restartScene)
	};

	//For when all the required assets for the new scene have been loaded
	class LoadAssetEvent : public IEvent
	{
		Scene loadedScene;
	public:
		LoadAssetEvent(Scene _scene) : loadedScene(_scene) {}
		inline const Scene& GetScene() { return loadedScene; }

		EVENT_TYPE(assetsLoaded)
	};




	//For when entity is destroyed
	class EntityDestroyedEvent : public IEvent
	{
		EntityID e;
	public:
		EntityDestroyedEvent(EntityID _e) : e(_e) {}
		inline const EntityID& GetDestroyedEntity() { return e; }

		EVENT_TYPE(entityDestroy)
	};




	class KeyButtonEvent : public IEvent
	{
		//GLFW values of key and action, will be converted to our version in input controller side
		int key;
		int action;
	
	public:
		KeyButtonEvent(int _key, int _action) : key(_key), action(_action) {}
		inline int GetKey() const { return key; }
		inline int GetAction() const { return action; }
		EVENT_TYPE(keyButton)
	};

	class MouseButtonEvent : public IEvent
	{
		//GLFW values of mouse and action, will be converted to our version in input controller side
		int mouse;
		int action;

	public:
		MouseButtonEvent(int _mouse, int _action) : mouse(_mouse), action(_action) {}
		inline int GetKey() const { return mouse; }
		inline int GetAction() const { return action; }
		EVENT_TYPE(mouseButton)
	};

	class MouseMoveEvent : public IEvent
	{
		Vec3f newPos;

	public:
		MouseMoveEvent(float xPos, float yPos) : newPos(xPos, yPos, 0) {}
		inline const Vec3f& GetNewPos() const { return newPos; }
		EVENT_TYPE(mouseMove)
	};

	class MouseScrollEvent : public IEvent
	{
		Vec3f newOffset;

	public:
		MouseScrollEvent(float xOffset, float yOffset) : newOffset(xOffset, yOffset, 0) {}
		inline const Vec3f& GetOffset() const { return newOffset; }
		EVENT_TYPE(mouseScroll)
	};


	class GamePauseEvent : public IEvent
	{
		bool isPause;

	public:
		GamePauseEvent(bool _isPause) : isPause(_isPause) {}
		inline bool GetIsPaused() const { return isPause; }
		EVENT_TYPE(pause)
	};
}

#endif