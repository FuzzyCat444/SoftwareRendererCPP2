#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Math.hpp"

class Camera
{
public:
	Camera(bool orthographic = false, double fov = 0.0, double aspect = 0.0, double nearClip = 0.0, Vector3 position = Vector3{ 0.0, 0.0, 0.0 }, double yaw = 0.0, double pitch = 0.0, double roll = 0.0);

    void setOrthographic(bool orthographic);
	void setFov(double fov);
	void setAspect(double aspect);
	void setNearClip(double nearClip);
	void setPosition(Vector3 position);
	void setYaw(double yaw);
	void setPitch(double pitch);
	void setRoll(double roll);

	void translate(Vector3 translation);
	void rotateYaw(double yaw);
	void rotatePitch(double pitch);
	void rotateRoll(double roll);

    bool getOrthographic() const;
	double getFov() const;
	double getPerspective() const;
	double getAspect() const;
	double getNearClip() const;
	Vector3 getPosition() const;
	double getYaw() const;
	double getPitch() const;
	double getRoll() const;

	const Transform& getTransform() const;
	Vector3 getForwardVec() const;
	Vector3 getRightVec() const;
	Vector3 getUpVec() const;

	Vector3 getFrontVec() const;
private:
    void limitPitch();

    bool orthographic;
	double fov;
	double perspective;
	double aspect;
	double nearClip;
	Vector3 position;
	double yaw;
	double pitch;
	double roll;

	Translate positionTransform;
	Rotate yawTransform;
	Rotate pitchTransform;
	Rotate rollTransform;
	Combined combinedTransform;
};

#endif
