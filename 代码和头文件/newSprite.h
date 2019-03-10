#pragma once

#include "d3dUtil.h"
#include <string>
#include "GeometryGenerator.h"
#include "xnacollision.h"
#include "Vertex.h"
#include "Effects.h"

#ifndef HR
#define HR(x)                                              \
	{                                                          \
		HRESULT hr = (x);                                      \
		if(FAILED(hr))                                         \
		{                                                      \
			DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true); \
		}                                                      \
	}
#endif

enum ModelType
{
		BOX,
		SPHERE,		//Çò
		GEOSPHERE,	//Çò
		CYLINDER,	//Ô²Ì¨
		GRID,		//Æ½Ãæ
};

class Sprite
{
public:
	Sprite();
	~Sprite();

	void initSprite(ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dImmediateContext);
	void setImageSRV(std::wstring imageName);
	void setModelscale(float scaleX, float scaleY, float scaleZ);
	void setModelRotation(float rotationX, float rotationY, float rotationZ);
	void setModelTranslation(float transX, float transY, float transZ);
	void setTotalScale(float scaleX, float scaleY, float scaleZ);
	void setTotalRotation(float rotationX, float rotationY, float rotationZ);
	void setTotalTranslation(float transX, float transY, float transZ);


	XMMATRIX getModelWorld();
	ID3D11ShaderResourceView* getImageSRV();
	UINT pickModel(XMFLOAT4X4 view, float vx, float vy);

	void CreateModel(ModelType modelType);
	void CreateModel(std::string Filename);
	void CreateBox();
	void CreateSphere();
	void CreateGeosphere();
	void CreateCylinder();
	void CreateGrid();

	void draw(ID3DX11EffectTechnique* activeTech);
	void drawModel(ID3DX11EffectTechnique* activeTech, UINT p);
private:
	ID3D11Device* m_d3dDevice;
	ID3D11DeviceContext* m_d3dImmediateContext;

	ID3DX11Effect* m_FX;
	
	ID3D11ShaderResourceView* m_ImageSRV;
	
	XMMATRIX m_ModelWorld;
	XMMATRIX m_World;
	XMMATRIX m_Scale;
	XMMATRIX m_Trans;
	XMMATRIX m_Rotation;

	ID3D11Buffer* m_VB;
	ID3D11Buffer* m_IB;

	std::vector<Vertex::Basic32> m_Vertices;
	std::vector<UINT> m_Indices;
	UINT m_ModelIndexCount;

	UINT m_stride ;
	UINT m_offset ;
	UINT m_ModelVertexOffset = 0;
	UINT m_ModelIndexOffset = 0;
	
	UINT m_PickedTriangle;
	
	XNA::AxisAlignedBox m_MeshModel;
};
