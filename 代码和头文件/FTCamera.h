#pragma once

#include "d3dUtil.h"

class Camera
{
public:
	Camera();
	~Camera();

	XMVECTOR GetPositionXM()const;
	XMFLOAT3 GetPosition()const;
	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3& v);

	XMFLOAT3 GetTarget()const;
	void SetTarget(const XMFLOAT3& v);
	void SetTarget(float x, float y, float z);

	void moveLR(float distance);
	void moveFB(float distance);
	void moveUB(float distance);
	void rotateLR(float angle);
	void rotateUB(float angle);

	void rotateTPLR(float angle);
	void rotateTPUB(float angle);
	void rotateTP(float Phi, float Theta);

	void setProj(float scale,float nearZ,float farZ);

	void setView(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp);
	void setView(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up);

	XMMATRIX getView();
	XMMATRIX getProj();
	XMMATRIX getViewProj();

	XMFLOAT4X4	 getViewF4();
	XMFLOAT4X4 getProjF4();

private:
	XMFLOAT3 m_Position;
	XMFLOAT3 m_Right;
	XMFLOAT3 m_Up;
	XMFLOAT3 m_Look;
	XMFLOAT3 m_Target;

	float m_FPLimitAngle;
	float m_Phi;
	float m_Theta;

	XMFLOAT4X4 m_View;
	XMFLOAT4X4 m_Proj;
};