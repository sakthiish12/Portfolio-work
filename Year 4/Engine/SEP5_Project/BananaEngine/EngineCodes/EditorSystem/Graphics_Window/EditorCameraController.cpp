#include "pch.h"
#include <Utilities/Utility.h>

#ifdef USE_EDITOR

#include "EditorCameraController.hpp"
#include <EditorSystem/EditorSystem/EditorSystem.h>
#include <Systems/InputController/InputController.h>

namespace BE
{
	float EditorCameraController::moveSpeed{ 1.0f };
	float EditorCameraController::scrollSpeed{ 20.0f };
	float EditorCameraController::rotationSpeed{ 60.0f };
	float EditorCameraController::rotateDistance{ 10.0f };

	void EditorCameraController::UpdateEditorCamera(float dt, Transform& tf)
	{
		//If either windows are focused
		if (!Editorsystem->getGraphicsWindow()->IsGraphicsWindowFocused() && !Editorsystem->getHierarchyWindow()->isHierarchyFocused())
			return;

		//If pressed F, then teleport camera to that entity
		if (Hierarchy_Window::Get()->isusing && Input->IsKeyTriggered(Key::_F))
		{
			//Teleport the camera near the selected entity
			EntityID e = Hierarchy_Window::Get()->id;
			Vec3f dir = ECS->GetComponent<Transform>(e).GetPosition().DirTowards(tf.GetPosition());
			tf.SetPosition(ECS->GetComponent<Transform>(e).GetPosition() + dir);
		}

		//if graphics window is not focused - it must be focused for the rest of the features.
		if (!Editorsystem->getGraphicsWindow()->IsGraphicsWindowFocused())
			return;

		//If in play mode then dont do anything
		if (Tool_Bar::Get()->GetSceneState() != SceneState::Edit)
			return;

		Mat3f matrix{};
		matrix.SetRotation(tf.GetRotation());

		Vec3f right =	Vec3f{ matrix[0][0], matrix[1][0], matrix[2][0] };
		Vec3f up =		Vec3f{ matrix[0][1], matrix[1][1], matrix[2][1] };
		Vec3f forward = Vec3f{ matrix[0][2], matrix[1][2], matrix[2][2] };

		//middle mouse / mouse 4  -> Translate camera
		if (Input->IsMousePressed(Mouse::MOUSE_MIDDLE) || Input->IsMousePressed(Mouse::MOUSE_4))
		{
			Vec3f newPos = tf.GetPosition();
			
			//Horizontal delta
			newPos += -Input->GetMouseDeltaX() * dt * right * moveSpeed;

			//Vertical delta
			newPos += Input->GetMouseDeltaY() * dt * up * moveSpeed;
			tf.SetPosition(newPos);
		}

		//RMB -> Rotate around a point 5m in front
		if (Input->IsMousePressed(Mouse::MOUSE_RIGHT) && !Input->IsCtrlPressed())
		{
			//Camera will rotate about this point
			Vec3f anchorPoint = tf.GetPosition() + rotateDistance * forward;
			Vec3f newAngles = tf.GetEulerRotation();

			//Rotate about Y axis
			newAngles.y += Input->GetMouseDeltaX() * rotationSpeed * dt;

			//Rotate about X axis
			newAngles.x += Input->GetMouseDeltaY() * rotationSpeed * dt;

			newAngles.y = BEMath::Wrap(-180.0f, 180.0f, newAngles.y);
			newAngles.x = BEMath::Clamp(-89.0f, 89.0f, newAngles.x);
			tf.SetEulerRotation(newAngles);

			//Compute the new Forward
			Mat3f newMatrix{};
			newMatrix.SetRotation(tf.GetRotation());
			Vec3f newForward = Vec3f{ newMatrix[0][2], newMatrix[1][2], newMatrix[2][2] };

			tf.SetPosition(anchorPoint + rotateDistance * -newForward);
		}

		//Ctrl + RMB -> Rotate camera
		if (Input->IsMousePressed(Mouse::MOUSE_RIGHT) && Input->IsCtrlPressed())
		{
			Vec3f newAngles = tf.GetEulerRotation();

			//Rotate about Y axis
			newAngles.y += Input->GetMouseDeltaX() * rotationSpeed * dt;

			//Rotate about X axis
			newAngles.x += Input->GetMouseDeltaY() * rotationSpeed * dt;

			newAngles.y = BEMath::Wrap(-180.0f, 180.0f, newAngles.y);
			newAngles.x = BEMath::Clamp(-89.0f, 89.0f, newAngles.x);
			tf.SetEulerRotation(newAngles);
		}

		//Scroll wheel -> Move forward/backwards
		if (Input->GetVerticalScroll() > BEMath::EPSILON || Input->GetVerticalScroll() < -BEMath::EPSILON)
		{
			Vec3f newPos = tf.GetPosition();

			//Forward/backwards movement
			newPos += Input->GetVerticalScroll() * scrollSpeed * dt * forward;

			tf.SetPosition(newPos);
		}
	}
}

#endif

//// std
//#include <limits>
//
//namespace BE {
//
//void KeyboardMovementController::MoveInPlaneXZ(GLFWwindow* window, float dt, Transform& tf) {
//  glm::vec3 rotate{0};
//
//  if (BE::Editorsystem->getGraphicsWindow()->IsGraphicsWindowFocused())
//  {
//      if (InputController::GetInstance()->IsKeyPressed(Key::RIGHT)) rotate.y += 1.f;
//      if (InputController::GetInstance()->IsKeyPressed(Key::LEFT)) rotate.y -= 1.f;
//      if (InputController::GetInstance()->IsKeyPressed(Key::DOWN)) rotate.x += 1.f;
//      if (InputController::GetInstance()->IsKeyPressed(Key::UP)) rotate.x -= 1.f;
//  }
//
//  if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
//
//      glm::vec3 glmResult = lookSpeed * dt * glm::normalize(rotate);
//      Vec3f result{ glmResult.x,glmResult.y,glmResult.z };
//      tf.SetEulerRotation(tf.GetEulerRotation() + result);
//  }
//
//  // limit pitch values between about +/- 85ish degrees
//  tf.SetEulerRotation(
//      Vec3f{ glm::clamp(tf.GetEulerRotation().x,-85.0f,85.0f), tf.GetEulerRotation().y,tf.GetEulerRotation().z });
//
//  float yaw = glm::radians(tf.GetEulerRotation().y);
//  const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
//  const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
//  const glm::vec3 upDir{0.f, -1.f, 0.f};
//
//  glm::vec3 moveDir{0.f};
//
//  if (BE::Editorsystem->getGraphicsWindow()->IsGraphicsWindowFocused())
//  {
//      if (InputController::GetInstance()->IsKeyPressed(Key::_W)) moveDir += forwardDir;
//      if (InputController::GetInstance()->IsKeyPressed(Key::_S)) moveDir -= forwardDir;
//      if (InputController::GetInstance()->IsKeyPressed(Key::_D)) moveDir += rightDir;
//      if (InputController::GetInstance()->IsKeyPressed(Key::_A)) moveDir -= rightDir;
//      if (InputController::GetInstance()->IsKeyPressed(Key::_E)) moveDir += upDir;
//      if (InputController::GetInstance()->IsKeyPressed(Key::_Q)) moveDir -= upDir;
//  }
//
//  if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
//      glm::vec3 glmResult = moveSpeed * dt * glm::normalize(moveDir);
//      Vec3f result{ glmResult.x,glmResult.y,glmResult.z };
//
//      tf.SetPosition(tf.GetPositon()+result);
//  }
//}
//}  // namespace BE