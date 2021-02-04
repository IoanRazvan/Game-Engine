#include "stdafx.h"
#include "Camera.h"
#include <corecrt_math_defines.h>
#include <exception>

#define RADIANS(x) (((x)/180) * M_PI) 

void Camera::setRotationFunctions()
{
	rotationFunction_m[Axis::X] = &Matrix::SetRotationX;
	rotationFunction_m[Axis::Y] = &Matrix::SetRotationY;
	rotationFunction_m[Axis::Z] = &Matrix::SetRotationZ;
}

bool equalVecs(Vector3 v1, Vector3 v2) {
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

void Camera::initAxis() {
	versor_m[Axis::Z] = -(target_m - position_m).Normalize();
	Vector3 temp;
	if (versor_m[Axis::Z].x != 0)
		temp = Vector3(0.0f, 1.0f, 0.0f);
	else if (versor_m[Axis::Z].y >= 0.0f && versor_m[Axis::Z].z < 0.0f)
		temp = Vector3(0.0f, 1.0f, 0.0f);
	else if (versor_m[Axis::Z].y > 0.0f && versor_m[Axis::Z].z >= 0.0f)
		temp = Vector3(0.0f, 0.0f, 1.0f);
	else if (versor_m[Axis::Z].y <= 0.0f && versor_m[Axis::Y].z > 0.0f)
		temp = Vector3(0.0f, -1.0f, 0.0f);
	else
		temp = Vector3(0.0f, 0.0f, -1.0f);
	versor_m[Axis::X] = temp.Cross(versor_m[Axis::Z]).Normalize();
	up_m = versor_m[Axis::Y] = versor_m[Axis::Z].Cross(versor_m[Axis::X]).Normalize();

	printf("X: %f %f %f\n", versor_m[Axis::X].x, versor_m[Axis::X].y, versor_m[Axis::X].z);
	printf("Y: %f %f %f\n", versor_m[Axis::Y].x, versor_m[Axis::Y].y, versor_m[Axis::Y].z);
	printf("Z: %f %f %f\n\n", versor_m[Axis::Z].x, versor_m[Axis::Z].y, versor_m[Axis::Z].z);
}

void Camera::updateAxis()
{
	versor_m[Axis::Z] = -(target_m - position_m).Normalize();
	versor_m[Axis::Y] = up_m.Normalize();
	versor_m[Axis::X] = versor_m[Axis::Y].Cross(versor_m[Axis::Z]);
	
	printf("X: %f %f %f\n", versor_m[Axis::X].x, versor_m[Axis::X].y, versor_m[Axis::X].z);
	printf("Y: %f %f %f\n", versor_m[Axis::Y].x, versor_m[Axis::Y].y, versor_m[Axis::Y].z);
	printf("Z: %f %f %f\n\n", versor_m[Axis::Z].x, versor_m[Axis::Z].y, versor_m[Axis::Z].z);
}

void Camera::updateWorldView()
{
	Matrix R, T;

	R.m[0][0] = versor_m[Axis::X].x; R.m[0][1] = versor_m[Axis::X].y; R.m[0][2] = versor_m[Axis::X].z; R.m[0][3] = 0.0f;
	R.m[1][0] = versor_m[Axis::Y].x; R.m[1][1] = versor_m[Axis::Y].y; R.m[1][2] = versor_m[Axis::Y].z; R.m[1][3] = 0.0f;
	R.m[2][0] = versor_m[Axis::Z].x; R.m[2][1] = versor_m[Axis::Z].y; R.m[2][2] = versor_m[Axis::Z].z; R.m[2][3] = 0.0f;
	R.m[3][0] = 0.0f;				 R.m[3][1] = 0.0f;                R.m[3][2] = 0.0f;	               R.m[3][3] = 1.0f;

	worldMatrix_m = R * T.SetTranslation(position_m);

	viewMatrix_m = T.SetTranslation(-position_m) * R.Transpose();
}

void Camera::initCamera()
{
	initAxis();
	updateWorldView();
	setRotationFunctions();

	float aspectRatio = static_cast<float>(Globals::screenWidth) / Globals::screenHeight;
	projection_m.SetPerspective(fov_m, aspectRatio, nearPlane_m, farPlane_m);
}

Camera::Camera() : position_m(0.0f, 0.0f, -300.0f), target_m(0.0f, 0.0f, 0.0f), up_m(0.0f, 1.0f, 0.0f), moveSpeed_m(10.0f), rotateSpeed_m(60.0f), 
				   nearPlane_m(0.2f), farPlane_m(1000.0f), fov_m(45.0f), deltaTime_m(0.0f)
{
	initCamera();
}

Camera::Camera(Vector3 position, Vector3 target, GLfloat moveSpeed,
	GLfloat rotateSpeed, GLfloat nearPlane, GLfloat farPlane, GLfloat fov,
	GLfloat deltaTime)
	: position_m(position), target_m(target), moveSpeed_m(moveSpeed),
	rotateSpeed_m(rotateSpeed), nearPlane_m(nearPlane), farPlane_m(farPlane), fov_m(fov),
	deltaTime_m(deltaTime)
{
	initCamera();
}

void Camera::translate(Axis axis, Direction direction) 
{
	Vector3 movement = versor_m[axis] * static_cast<int>(direction);
	Vector3 displacementVector = movement * moveSpeed_m * deltaTime_m;
	
	position_m += displacementVector;
	target_m += displacementVector;
	
	updateWorldView();
}

void Camera::rotate(Axis axis, Direction direction)
{
	Matrix rotationMatrix;
	float angle = RADIANS(static_cast<int>(direction) * deltaTime_m * rotateSpeed_m);
	(rotationMatrix.*rotationFunction_m[axis])(direction == Direction::NEGATIVE ? -0.17453292519 : 0.17453292519);
	
	if (axis == Axis::X || axis == Axis::Z) {
		Vector4 rotatedLocalUp = Vector4(0.0f, 1.0f, 0.0f, 0.0f) * rotationMatrix;
		Vector4 up_vec4 = rotatedLocalUp * worldMatrix_m;
		up_m = Vector3(up_vec4.x, up_vec4.y, up_vec4.z);
	}

	if (axis == Axis::X || axis == Axis::Y) {
		Vector4 localTarget = Vector4(0.0f, 0.0f, -(target_m - position_m).Length(), 1.0f);
		Vector4 rotatedTarget = localTarget * rotationMatrix;
		Vector4 target_vec4 = rotatedTarget * worldMatrix_m;
		target_m = Vector3(target_vec4.x, target_vec4.y, target_vec4.z);
	}

	updateAxis();
	updateWorldView();
}

void Camera::setMoveSpeed(GLfloat moveSpeed)
{
	if (moveSpeed < 0.0f)
		throw std::exception("Negative translation speed not alowed!");
	moveSpeed_m = moveSpeed;
}

void Camera::setRotateSpeed(GLfloat rotateSpeed)
{
	if (rotateSpeed < 0.0f)
		throw std::exception("Negative rotation speed not alowed!");
	rotateSpeed_m = rotateSpeed;
}

void Camera::setFarPlane(GLfloat farPlane)
{
	if (farPlane < 0.0f)
		throw std::exception("The far plane can't be negative!");
	else if (farPlane < nearPlane_m)
		throw std::exception("The far plane can't be less than the near plane!");
	farPlane_m = farPlane;
}

void Camera::setNearPlane(GLfloat nearPlane)
{
	if (nearPlane < 0.0f)
		throw std::exception("The near plane can't be negative!");
	else if (nearPlane > farPlane_m)
		throw std::exception("The near plane can't greater than the near plane!");
	nearPlane_m = nearPlane;
}

void Camera::setDeltaTime(GLfloat deltaTime)
{
	if (deltaTime < 0.0f)
		throw std::exception("Delta time has to be positive!");
	deltaTime_m = deltaTime;
}