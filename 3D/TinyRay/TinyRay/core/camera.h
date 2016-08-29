/********************************************************************
	created:	2016/07/17
	created:	17:7:2016   22:04
	filename: 	D:\Code\TinyRay\TinyRay\core\camera.h
	file path:	D:\Code\TinyRay\TinyRay\core
	file base:	camera
	file ext:	h
	copyright:
		The source come from book "Realistic Ray Tracing".
		
	modify:	

	purpose:	Camera

    V            up    
    |             |\
    |             | \
    |            l|  \t
    o---------------d \---------W
     \            |    \
      \      corner\    |
       \            \   |
        U            \  |
                    b \ |r
                       \|
                   across
*********************************************************************/
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "math/vector3.h"
#include "math/onb.h"
#include "ray.h"

namespace tinyray
{
	class Camera
	{
	public:
		// camera coordinate
		Vector3f origin_;
		ONB uvw_;

		// view rect
		Vector3f corner_;
		Vector3f across_;
		Vector3f up_;
		float l_;
		float r_;
		float b_;
		float t_;
		
		float lensRadius_;
		float distance_;

		Camera() {}
		Camera(const Vector3f& origin, const Vector3f& gaze, const Vector3f& vup, float aperture,
			float left, float right, float bottom, float top, float distance)
			: origin_(origin)
			, l_(left)
			, r_(right)
			, b_(bottom)
			, t_(top)
			, distance_(distance)
		{
			uvw_.InitFromWV(gaze, vup);
			corner_ = origin_ + l_ * uvw_.u_ + b_ * uvw_.v_ + distance_ * uvw_.w_;
			across_ = (r_ - l_) * uvw_.u_;
			up_ = (t_ - b_) * uvw_.v_;
			lensRadius_ = aperture / 2.0f;
		}

		Ray GetRay(float a, float b, float x, float y)
		{
			// x, y: sample point in lens, and x : [0,1.0], y : [0,1.0]
			// assume using a rect simulate disk for lens, then width and height of rect is 2 * lens radius
			// |-------|
			// |---o---|
			// |-------|
			// UV plane, o for origin_, we need map x,y from[0,1.0] to [-radius, radius] to compute origin of ray

			Vector3f org = origin_ + 2.0f * (x - 0.5f) * lensRadius_ * uvw_.u_
				+ 2.0f * (y - 0.5f) * lensRadius_ * uvw_.v_;
			Vector3f target = corner_ + across_ * a + up_ * b;
			Vector3f dir = target - org;
			dir.Normalize();
			return Ray(org, dir);
		}
	};
}

#endif