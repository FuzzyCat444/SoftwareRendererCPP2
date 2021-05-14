#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Math.hpp"

class Camera
{
public:
	Camera();
	Camera(double fov, double aspect, double nearClip);
	Camera(double fov, double aspect, double nearClip, Vector3 position, double yaw, double pitch);

	void setFov(double fov);
	void setAspect(double aspect);
	void setNearClip(double nearClip);
	void setPosition(Vector3 position);
	void setYaw(double yaw);
	void setPitch(double pitch);

	void translate(Vector3 translation);
	void rotateYaw(double yaw);
	void rotatePitch(double pitch);

	double getFov() const;
	double getPerspective() const;
	double getAspect() const;
	double getNearClip() const;
	Vector3 getPosition() const;
	double getYaw() const;
	double getPitch() const;

	const Transform& getTransform() const;
	Vector3 getForwardVec() const;
	Vector3 getRightVec() const;
	Vector3 getUpVec() const;

	Vector3 getFrontVec() const;
private:
    void limitPitch();

	double fov;
	double perspective;
	double aspect;
	double nearClip;
	Vector3 position;
	double yaw;
	double pitch;

	Rotate pitchTransform;
	Translate positionTransform;
	Rotate yawTransform;
	Combined combinedTransform;
};

#endif
