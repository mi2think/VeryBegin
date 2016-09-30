#pragma once

#include "sims.h"
#include "math/vector3.h"
using namespace sims;

struct D3DXMATRIX;

class Camera
{
public:
	enum CameraType { LandObject, Aircraft };

	Camera(CameraType cameraType = Aircraft);

	void SetCameraType(CameraType type) { cameraType_ = type; }

	void SetEye(const Vector3f& eye) { eye_ = eye; }

	const Vector3f& GetEye() const { return eye_; }
	const Vector3f& GetRight() const { return right_; }
	const Vector3f& GetUp() const { return up_; }
	const Vector3f& GetLook() const { return look_; }

	// rotate on right vector
	void Pitch(float radian);
	// rotate on up vector
	void Yaw(float radian);
	// rotate on look vector
	void Roll(float radian);

	// move left/right
	void Strafe(float units);
	// move up/down
	void Fly(float units);
	// move forward/backward
	void Walk(float units);

	void GetViewMatrix(D3DXMATRIX* view);

	void OnUpdate(const Timestep& timestep);
private:
	CameraType cameraType_;
	Vector3f eye_;

	Vector3f look_;
	Vector3f up_;
	Vector3f right_;
};

