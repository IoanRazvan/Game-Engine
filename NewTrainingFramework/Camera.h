#pragma once
#include "../Utilities/utilities.h"
#include "Globals.h"
#include <unordered_map>

class Camera
{
public:
	enum class Axis { X, Y, Z };
	enum class Direction {POSITIVE = 1, NEGATIVE = -1};
private:
	Vector3 position_m;
	Vector3 target_m;
	Vector3 up_m;
	GLfloat moveSpeed_m;
	GLfloat rotateSpeed_m;
	GLfloat nearPlane_m;
	GLfloat farPlane_m;
	GLfloat fov_m;
	GLfloat deltaTime_m;
	Matrix viewMatrix_m, worldMatrix_m, projection_m;
	std::unordered_map<Axis, Vector3> versor_m;
	std::unordered_map<Axis, Matrix & (Matrix::*) (GLfloat)> rotationFunction_m;
	void updateWorldView();
	void updateAxis();
	void setRotationFunctions();
	void initCamera();
	void initAxis();
public:
	//ctors
	Camera(Vector3 position, Vector3 target, GLfloat moveSpeed,
		GLfloat rotateSpeed, GLfloat nearPlane, GLfloat farPlane, GLfloat fov,
		GLfloat deltaTime);
	Camera();
	
	//movements
	void translate(Axis axis, Direction direction);
	void rotate(Axis axis, Direction direction);
	
	//getters
	GLfloat getMoveSpeed() const { return moveSpeed_m; }
	GLfloat getRotateSpeed() const { return rotateSpeed_m; }
	GLfloat getFarPlane() const { return farPlane_m; }
	GLfloat getNearPlane() const { return nearPlane_m; }
	GLfloat getDeltaTime() const { return deltaTime_m; }
	GLfloat getFieldOfView() const { return fov_m; }
	Matrix& getViewMatrix() { return viewMatrix_m; }
	Matrix& getWorldMatrix() { return worldMatrix_m; }
	Vector3& getPosition() { return position_m; }
	Matrix getMVP() { return viewMatrix_m * projection_m; }

	//setters
	void setMoveSpeed(GLfloat moveSpeed);
	void setRotateSpeed(GLfloat rotateSpeed);
	void setFarPlane(GLfloat farPlane);
	void setNearPlane(GLfloat nearPlane);
	void setDeltaTime(GLfloat deltaTime);
};

