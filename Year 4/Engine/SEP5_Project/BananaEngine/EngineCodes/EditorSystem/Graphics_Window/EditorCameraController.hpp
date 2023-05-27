#pragma once
#ifndef	EDITOR_CAMERA_CONTROLLER_H
#define EDITOR_CAMERA_CONTROLLER_H

#ifdef USE_EDITOR
#include <Systems/Window/Window.h>

namespace BE
{
	class EditorCameraController
	{
	public:
		void UpdateEditorCamera(float dt, Transform& tf);
		static float scrollSpeed;
		static float moveSpeed;
		static float rotationSpeed;
		static float rotateDistance;

		
	};
}
#endif

#endif




//#pragma once
//
//#include "../Window/Window.h"
//
//namespace BE {
//class KeyboardMovementController {
// public:
//  void MoveInPlaneXZ(GLFWwindow* window, float dt, Transform& tf);
//
//  float moveSpeed{3.f};
//  float lookSpeed{100.0f};
//};
//}  // namespace BE