#include "FTCamera.h"

Camera::Camera()
	: m_Position(0.0f, 0.0f, 0.0f),
	m_Right(1.0f, 0.0f, 0.0f),
	m_Up(0.0f, 1.0f, 0.0f),
	m_Look(0.0f, 0.0f, 1.0f),
	m_FPLimitAngle(0),m_Target(0.0f,0.0f,0.0f),m_Phi(0.5 * MathHelper::Pi),m_Theta(0)
{
	setProj(1.0f, 1.0f, 1000.0f);
}
Camera::~Camera()
{

}

XMVECTOR Camera::GetPositionXM()const
{
	return XMLoadFloat3(&m_Position);
}

XMFLOAT3 Camera::GetPosition()const
{
	return m_Position;
}

void Camera::SetPosition(float x, float y, float z)
{
	m_Position = XMFLOAT3(x, y, z);
}

void Camera::SetPosition(const XMFLOAT3& v)
{
	m_Position = v;
}

void Camera::SetTarget(float x, float y, float z)
{
	m_Target = XMFLOAT3(x, y, z);
	setView(m_Position, m_Target, m_Up);
}

void Camera::SetTarget(const XMFLOAT3& v)
{
	m_Target = v;
}

XMFLOAT3 Camera::GetTarget()const
{
	return m_Target;
}

void Camera::moveLR(float distance)
{
	XMVECTOR s = XMVectorReplicate(distance);
	XMVECTOR r = XMLoadFloat3(&m_Right);
	XMVECTOR p = XMLoadFloat3(&m_Position);
	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(s, r, p));
}

void Camera::moveFB(float distance)
{
	XMVECTOR s = XMVectorReplicate(distance);
	XMVECTOR l = XMLoadFloat3(&m_Look);
	XMVECTOR p = XMLoadFloat3(&m_Position);
	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(s, l, p));
}

void Camera::moveUB(float distance)
{
	XMVECTOR s = XMVectorReplicate(distance);
	XMVECTOR u = XMLoadFloat3(&m_Up);
	XMVECTOR p = XMLoadFloat3(&m_Position);
	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(s, u, p));
}

void Camera::rotateLR(float angle)
{
	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&m_Right, XMVector3TransformNormal(XMLoadFloat3(&m_Right), R));
	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
	XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));
}

void Camera::rotateUB(float angle)
{
	
	//if ((m_FPLimitAngle > XMConvertToRadians(80.0f) && angle > 0) || (m_FPLimitAngle < XMConvertToRadians(-80.0f) && angle < 0))
		//return;
	//m_FPLimitAngle += angle;
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_Right), angle);
	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
	XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));
}

void Camera::rotateTP(float Phi,float Theta)
{
	XMVECTOR P = XMLoadFloat3(&m_Position);
	XMVECTOR T = XMLoadFloat3(&m_Target);
	float distance = XMVectorGetX( XMVector3Length(XMVectorSubtract(P, T)));
	m_Phi = acosf((m_Position.y - m_Target.y) / distance);
	m_Theta = atanf((m_Position.z - m_Target.z) / (m_Position.x - m_Target.x));
	if ((m_Theta > XMConvertToRadians(87.0f) && Theta > 0) || (m_Theta < XMConvertToRadians(-87.0f) && Theta < 0))
	{
		return;
	}m_Position.y = distance * cosf(m_Phi + Phi);
	m_Position.x = distance * sinf(m_Phi + Phi) * cosf(m_Theta + Theta);
	m_Position.z = distance * sinf(m_Phi + Phi) * sinf(m_Theta + Theta);
	m_Position.y += m_Target.y;
	m_Position.x += m_Target.x;
	m_Position.z += m_Target.z;
	setView(m_Position, m_Target, m_Up);
}

void Camera::rotateTPUB(float angle)
{
	rotateTP(angle, 0.0f);
}

void Camera::rotateTPLR(float angle)
{
	rotateTP(0.0f, angle);
}

void Camera::setProj(float scale, float nearZ, float farZ)
{
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, scale, nearZ, farZ);
	XMStoreFloat4x4(&m_Proj, P);
}

void Camera::setView(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&m_Position, pos);
	XMStoreFloat3(&m_Look, L);
	XMStoreFloat3(&m_Right, R);
	XMStoreFloat3(&m_Up, U);
}	

void Camera::setView(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	setView(P, T, U);
}

XMMATRIX Camera::getView()
{
	XMVECTOR R = XMLoadFloat3(&m_Right);
	XMVECTOR U = XMLoadFloat3(&m_Up);
	XMVECTOR L = XMLoadFloat3(&m_Look);
	XMVECTOR P = XMLoadFloat3(&m_Position);

	// Keep camera's axes orthogonal to each other and of unit length.
	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3Cross(L, R));

	// U, L already ortho-normal, so no need to normalize cross product.
	R = XMVector3Cross(U, L);

	// Fill in the view matrix entries.
	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	XMStoreFloat3(&m_Right, R);
	XMStoreFloat3(&m_Up, U);
	XMStoreFloat3(&m_Look, L);

	m_View(0, 0) = m_Right.x;
	m_View(1, 0) = m_Right.y;
	m_View(2, 0) = m_Right.z;
	m_View(3, 0) = x;

	m_View(0, 1) = m_Up.x;
	m_View(1, 1) = m_Up.y;
	m_View(2, 1) = m_Up.z;
	m_View(3, 1) = y;

	m_View(0, 2) = m_Look.x;
	m_View(1, 2) = m_Look.y;
	m_View(2, 2) = m_Look.z;
	m_View(3, 2) = z;

	m_View(0, 3) = 0.0f;
	m_View(1, 3) = 0.0f;
	m_View(2, 3) = 0.0f;
	m_View(3, 3) = 1.0f;

	return XMLoadFloat4x4(&m_View);
}

XMMATRIX Camera::getProj()
{
	return XMLoadFloat4x4(&m_Proj);
}

XMMATRIX Camera::getViewProj()
{
	return XMMatrixMultiply(getView(), getProj());
}

XMFLOAT4X4 Camera::getViewF4()
{
	return m_View;
}

XMFLOAT4X4 Camera::getProjF4()
{
	return m_Proj;
}

