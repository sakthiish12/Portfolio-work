#include "BabyPointerUI.h"

namespace TH
{
	void BabyPointerUI::Awake() 
	{
		cameraObj = BE::ECS->FindEntitiesByName("CameraRig", 1).front();
		babyObj = BE::ECS->FindEntitiesByName("Baby Monkey", 1).front();
	}

	void BabyPointerUI::Update(double dt)
	{
		UNUSED(dt);
#if 1
		//Get the position of the camera obj and baby 
		const BE::Transform& camTf  = BE::ECS->GetComponent<BE::Transform>(cameraObj);
		const BE::Transform& babyTf = BE::ECS->GetComponent<BE::Transform>(babyObj);

		//Find the forward facing vector of the camera and project it onto XZ plane
		const BE::Quatf& camRot = camTf.GetRotation();
		BE::Vec3f camFacing{ 2 * (camRot.GetI() * camRot.GetK() + camRot.GetJ() * camRot.GetR()),				//[0][2]
							0.0f, 
							1 - (2 * camRot.GetI() * camRot.GetI()) - (2 * camRot.GetJ() * camRot.GetJ()) };	//[2][2]

		//Find the vector from the camera to the baby and project it onto XZ plane
		BE::Vec3f camToBaby = camTf.GetPosition().VecTowards(babyTf.GetPosition());
		camToBaby.y = 0.0f;

		//If camToBaby is 0 vector we just end this function
		if (BEMath::IsEqual<float>(camToBaby.Mag(), 0.0f))
			return;

		//Find the signed angle between the camFacing and camToBaby.
		//If camFacing is right of camToBaby, then angle is CCW (positive), else CW (negative)
		//Need to add 45 degree to the angle because the arrow texture is facing top right at 0 degree
		float angle = camFacing.SignedAngleBetween(camToBaby, BE::Vec3f(0, 1, 0), false) - (float)BEMath::PI_OVER_4;

		//Now set the texture rotation
		BE::UI->GetCanvas(uiID.first)->GetUI<BE::UIImage>(uiID.second)->SetUIRotRad(angle);
#endif
	}
}