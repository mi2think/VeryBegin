#include "camera.h"
#include "math/matrix33.h"
#include "math/matrix44.h"
#include "core/input_state.h"
#include "graphics_api/sims_sdk_dx9.h"

Camera::Camera(CameraType cameraType)
	: cameraType_(cameraType)
{
	eye_ = Vector3f(0.0f, 0.0f, 0.0f);
	look_ = Vector3f(0.0f, 0.0f, 1.0f);
	up_ = Vector3f(0.0f, 1.0f, 0.0f);
	right_ = Vector3f(1.0f, 0.0f, 0.0f);
}

// rotate on right vector
void Camera::Pitch(float radian)
{
	Matrix33f r;
	right_.Normalize();
	MatrixRotationAxis(r, right_, radian);

	up_ *= r;
	look_ *= r;
}

// rotate on up vector
void Camera::Yaw(float radian)
{
	Matrix33f r;
	up_.Normalize();

	if (cameraType_ == LandObject) // rotate on y-aixs (0,1,0) for land object
		MatrixRotationY(r, radian);
	else
		MatrixRotationAxis(r, up_, radian);

	right_ *= r;
	look_ *= r;
}

// rotate on look vector
void Camera::Roll(float radian)
{
	if (cameraType_ == Aircraft)
	{
		Matrix33f r;
		look_.Normalize();
		MatrixRotationAxis(r, look_, radian);

		up_ *= r;
		right_ *= r;
	}
}

// move left/right
void Camera::Strafe(float units)
{
	// move only on x-z plane
	if (cameraType_ == LandObject)
		eye_ += Vector3f(right_.x, 0.0f, right_.z) * units;
	else
		eye_ += right_ * units;
}

// move up/down
void Camera::Fly(float units)
{
	if (cameraType_ == LandObject)
		eye_.y += units;
	else
		eye_ += up_ * units;
}

// move forward/backward
void Camera::Walk(float units)
{
	// move only on x-z plane
	if (cameraType_ == LandObject)
		eye_ += Vector3f(look_.x, 0.0f, look_.z) * units;
	else
		eye_ += look_ * units;
}

void Camera::GetViewMatrix(D3DXMATRIX* view)
{
	Matrix44f translationTrans;
	MaxtrixTranslation(translationTrans, -eye_);

	look_.Normalize();
	up_ = CrossProduct(look_, right_);
	up_.Normalize();

	right_ = CrossProduct(up_, look_);
	right_.Normalize();

	Matrix44f rotateTrans;
	auto& m = rotateTrans.m;
	m[0][0] = right_.x;   m[0][1] = up_.x;   m[0][2] = look_.x;   m[0][3] = 0.0f;
	m[1][0] = right_.y;   m[1][1] = up_.y;   m[1][2] = look_.y;   m[1][3] = 0.0f;
	m[2][0] = right_.z;   m[2][1] = up_.z;   m[2][2] = look_.z;   m[2][3] = 0.0f;
	m[3][0] = 0.0f;       m[3][1] = 0.0f;    m[3][2] = 0.0f;      m[3][3] = 1.0f;

	Matrix44f v;
	MatrixMultiply(v, translationTrans, rotateTrans);
	memcpy(view, &v, sizeof(v));
}

void Camera::OnUpdate(const Timestep& timestep)
{
	float seconds = timestep.GetSeconds();

	if (InputState::GetKeyState('W'))
		Walk(100.0f * seconds);
	else if (InputState::GetKeyState('S'))
		Walk(-100.0f * seconds);
	else if (InputState::GetKeyState('A'))
		Strafe(-100.0f * seconds);
	else if (InputState::GetKeyState('D'))
		Strafe(100.0f * seconds);
	else if (InputState::GetKeyState('R'))
		Fly(20.0f * seconds);
	else if (InputState::GetKeyState('F'))
		Fly(-20.0f * seconds);
	else if (InputState::GetKeyState(KEY_UP))
		Pitch(1.0f * seconds);
	else if (InputState::GetKeyState(KEY_DOWN))
		Pitch(-1.0f * seconds);
	else if (InputState::GetKeyState(KEY_LEFT))
		Yaw(-1.0f * seconds);
	else if (InputState::GetKeyState(KEY_RIGHT))
		Yaw(1.0f * seconds);
	else if (InputState::GetKeyState('N'))
		Roll(4.0f * seconds);
	else if (InputState::GetKeyState('M'))
		Roll(-4.0f * seconds);
}