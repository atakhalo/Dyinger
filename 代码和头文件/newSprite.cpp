#include "newSprite.h"

#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }

Sprite::Sprite() : m_ImageSRV(0)
{
	XMMATRIX I = XMMatrixIdentity();
	m_ModelWorld = I;
	m_World = I;
	m_Scale = I;
	m_Rotation = I;
	m_Trans = I;
	m_stride = sizeof(Vertex::Basic32);
	m_offset = 0;
	m_ModelVertexOffset = 0;
	m_ModelIndexOffset = 0;
}

Sprite::~Sprite()
{
	ReleaseCOM(m_VB);
	ReleaseCOM(m_IB);
	ReleaseCOM(m_ImageSRV);
}

void Sprite::initSprite(ID3D11Device* d3dDevice,ID3D11DeviceContext* d3dImmediateContext)
{
	m_d3dDevice = d3dDevice;
	m_d3dImmediateContext = d3dImmediateContext;
}

void Sprite::setImageSRV(std::wstring imageName)
{
	HR(D3DX11CreateShaderResourceViewFromFile(m_d3dDevice,
		imageName.c_str(), 0, 0, &m_ImageSRV, 0));
}

void Sprite::setModelscale(float scaleX, float scaleY, float scaleZ)
{
	m_ModelWorld *= XMMatrixScaling(scaleX, scaleY, scaleZ);
}

void Sprite::setModelRotation(float rotationX, float rotationY, float rotationZ)
{
	m_ModelWorld *= XMMatrixRotationX(rotationX);
	m_ModelWorld *= XMMatrixRotationY(rotationY);
	m_ModelWorld *= XMMatrixRotationZ(rotationZ);
}

void Sprite::setModelTranslation(float transX, float transY, float transZ)
{
	m_ModelWorld *= XMMatrixTranslation(transX, transY, transZ);
}

void Sprite::setTotalScale(float scaleX, float scaleY, float scaleZ)
{
	m_Scale = XMMatrixScaling(scaleX, scaleY, scaleZ);
}

void Sprite::setTotalRotation(float rotationX, float rotationY, float rotationZ)
{
	m_Rotation = XMMatrixRotationX(rotationX) *XMMatrixRotationY(rotationY) * XMMatrixRotationZ(rotationZ);
}

void Sprite::setTotalTranslation(float transX, float transY, float transZ)
{
	m_Trans = XMMatrixTranslation(transX, transY, transZ);
}

XMMATRIX Sprite::getModelWorld()
{
	return m_ModelWorld * m_Scale * m_Rotation * m_Trans;
}

ID3D11ShaderResourceView* Sprite::getImageSRV()
{
	return m_ImageSRV;
}

UINT Sprite::pickModel( XMFLOAT4X4 view,float vx,float vy)
{

	// Ray definition in view space.
	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	XMMATRIX V = XMLoadFloat4x4(&view);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(V), V);

	XMMATRIX W = getModelWorld();
	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

	XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

	rayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
	rayDir = XMVector3TransformNormal(rayDir, toLocal);

	// Make the ray direction unit length for the intersection tests.
	rayDir = XMVector3Normalize(rayDir);

	m_PickedTriangle = -1;
	float tmin = 0.0f;
	if (XNA::IntersectRayAxisAlignedBox(rayOrigin, rayDir, &m_MeshModel, &tmin))
	{
		// Find the nearest ray/triangle intersection.
		tmin = MathHelper::Infinity;
		for (UINT i = 0; i < m_ModelIndexCount / 3; ++i)
		{
			// Indices for this triangle.
			UINT i0 = m_Indices[i * 3 + 0];
			UINT i1 = m_Indices[i * 3 + 1];
			UINT i2 = m_Indices[i * 3 + 2];

			// Vertices for this triangle.
			XMVECTOR v0 = XMLoadFloat3(&m_Vertices[i0].Pos);
			XMVECTOR v1 = XMLoadFloat3(&m_Vertices[i1].Pos);
			XMVECTOR v2 = XMLoadFloat3(&m_Vertices[i2].Pos);

			// We have to iterate over all the triangles in order to find the nearest intersection.
			float t = 0.0f;
			if (XNA::IntersectRayTriangle(rayOrigin, rayDir, v0, v1, v2, &t))
			{
				if (t < tmin)
				{
					// This is the new nearest picked triangle.
					tmin = t;
					m_PickedTriangle = i;
				}
			}
		}
	}
	return m_PickedTriangle;
}

void Sprite::CreateModel(std::string Filename)
{
	std::vector<XMFLOAT3> postion;
	std::vector<XMFLOAT2>	texcrood;
	std::vector<XMFLOAT3> normal;
	
	m_Vertices.clear();
	m_Indices.clear();

	char type[2048];
	FILE * obj;
	fopen_s(&obj, Filename.c_str(), "r");
	XMFLOAT3 f3;
	XMFLOAT2 f2;
	int v, vn, vt, num = 0;

	while (!feof(obj))				//遇到文件尾
	{
		fscanf_s(obj, "%s", type, 2048);	//过滤多余字符
		if (strcmp(type, "v") == 0)
		{
			fscanf_s(obj, "%f%f%f", &f3.x, &f3.y, &f3.z);
			postion.push_back(f3);
		}
		else if (strcmp(type, "vt") == 0)
		{
			fscanf_s(obj, "%f%f", &f2.x, &f2.y);
			f2.y = 1 - f2.y;
			texcrood.push_back(f2);
		}
		else if (strcmp(type, "vn") == 0)
		{
			fscanf_s(obj, "%f%f%f", &f3.x, &f3.y, &f3.z);
			normal.push_back(f3);
		}
		else if (strcmp(type, "f") == 0)
		{
			fscanf_s(obj, "%d/%d/%d", &v, &vt, &vn);
			m_Vertices.emplace_back(Vertex::Basic32{ postion[v - 1], normal[vn - 1], texcrood[vt - 1] });

			fscanf_s(obj, "%d/%d/%d", &v, &vt, &vn);
			m_Vertices.emplace_back(Vertex::Basic32{ postion[v - 1], normal[vn - 1], texcrood[vt - 1] });

			fscanf_s(obj, "%d/%d/%d", &v, &vt, &vn);
			m_Vertices.emplace_back(Vertex::Basic32{ postion[v - 1], normal[vn - 1], texcrood[vt - 1] });

			num += 3;
		}
	}
	for (int i = 0; i < num; i++)
	{
		m_Indices.push_back(i);
	}

	//用来判断拾取
	XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);

	XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

	//得到顶点的最大和最小位置
	for (size_t i = 0; i < m_Vertices.size(); ++i)
	{
		XMVECTOR P = XMLoadFloat3(&m_Vertices[i].Pos);
		vMin = XMVectorMin(vMin, P);
		vMax = XMVectorMax(vMax, P);
	}
	//得到模型的半径和中心
	XMStoreFloat3(&m_MeshModel.Center, 0.5f*(vMin + vMax));
	XMStoreFloat3(&m_MeshModel.Extents, 0.5f*(vMax - vMin));

	D3D11_BUFFER_DESC vertexBuffDesc;
	vertexBuffDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBuffDesc.ByteWidth = sizeof(Vertex::Basic32) * m_Vertices.size();
	vertexBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBuffDesc.CPUAccessFlags = 0;
	vertexBuffDesc.MiscFlags = 0;
	vertexBuffDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vertexInitData;
	vertexInitData.pSysMem = &m_Vertices[0];
	HR(m_d3dDevice->CreateBuffer(&vertexBuffDesc, &vertexInitData, &m_VB));

	m_ModelIndexCount = num;

	D3D11_BUFFER_DESC indexBuffDesc;
	indexBuffDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBuffDesc.ByteWidth = sizeof(UINT) * num;
	indexBuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBuffDesc.CPUAccessFlags = 0;
	indexBuffDesc.MiscFlags = 0;
	indexBuffDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA indexInitData;
	indexInitData.pSysMem = &m_Indices[0];
	HR(m_d3dDevice->CreateBuffer(&indexBuffDesc, &indexInitData, &m_IB));
}

void Sprite::CreateModel(ModelType modelType)
{
	GeometryGenerator::MeshData model;
	GeometryGenerator geoGen;

	switch (modelType)
	{
		case BOX:
			geoGen.CreateBox(1.0f, 1.0f, 1.0f, model);
			break;

		case SPHERE:
			geoGen.CreateSphere(1.0f, 5, 10, model);
			break;

		case GEOSPHERE:
			geoGen.CreateGeosphere(1.0f, 10, model);
			break;

		case CYLINDER:
			geoGen.CreateCylinder(1.0f, 0.5f, 1.5f, 10, 10, model);
			break;

		case GRID:
			geoGen.CreateGrid(5.0f, 5.0f, 10, 10, model);
			break;
		default:break;
	}

	m_ModelIndexCount = model.Indices.size();

	m_ModelVertexOffset = 0;
	m_ModelIndexOffset = 0;

	UINT totalVertexCount = model.Vertices.size();

	UINT totalIndexCount = m_ModelIndexCount;

	//用来判断拾取
	XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);

	XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	XMVECTOR vMax = XMLoadFloat3(&vMaxf3);
	UINT k = 0;

	m_Vertices.resize(totalVertexCount);
	for (size_t i = 0; i < totalVertexCount; ++i, ++k)
	{
		m_Vertices[k].Pos = model.Vertices[i].Position;
		m_Vertices[k].Normal = model.Vertices[i].Normal;
		m_Vertices[k].Tex = model.Vertices[i].TexC;

		//得到顶点的最大和最小位置
		XMVECTOR P = XMLoadFloat3(&m_Vertices[i].Pos);
		vMin = XMVectorMin(vMin, P);
		vMax = XMVectorMax(vMax, P);
	}
	
	m_Indices.resize(totalIndexCount);
	for (UINT j = 0; j < totalIndexCount; ++j)
	{
		m_Indices[j] = model.Indices[j];
	}
	//得到模型的半径和中心
	XMStoreFloat3(&m_MeshModel.Center, 0.5f*(vMin + vMax));
	XMStoreFloat3(&m_MeshModel.Extents, 0.5f*(vMax - vMin));

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * totalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &m_Vertices[0];
	HR(m_d3dDevice->CreateBuffer(&vbd, &vinitData, &m_VB));




	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &m_Indices[0];
	HR(m_d3dDevice->CreateBuffer(&ibd, &iinitData, &m_IB));
}

void Sprite::CreateBox()
{
	CreateModel(BOX);
}

void Sprite::CreateSphere()
{
	CreateModel(SPHERE);
}

void Sprite::CreateGeosphere()
{
	CreateModel(GEOSPHERE);
}

void Sprite::CreateCylinder()
{
	CreateModel(CYLINDER);
}

void Sprite::CreateGrid()
{
	CreateModel(GRID);
}

void Sprite::drawModel(ID3DX11EffectTechnique* activeTech, UINT p)
{

	
	
	m_d3dImmediateContext->IASetVertexBuffers(0, 1, &m_VB, &m_stride, &m_offset);
	m_d3dImmediateContext->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, 0);
	activeTech->GetPassByIndex(p)->Apply(0, m_d3dImmediateContext);
	m_d3dImmediateContext->DrawIndexed(m_ModelIndexCount, m_ModelIndexOffset, m_ModelVertexOffset);
}

void Sprite::draw(ID3DX11EffectTechnique* activeTech)
{
	XMFLOAT4X4 FloatI;
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&FloatI, I);

	Material Mat;
	Mat.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Mat.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 64.0f);
	Mat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	Effects::BasicFX->SetTexTransform(I);
	Effects::BasicFX->SetMaterial(Mat);
	Effects::BasicFX->SetWorld(getModelWorld());
	XMMATRIX ModelworldInvTranspose = MathHelper::InverseTranspose(getModelWorld());
	Effects::BasicFX->SetWorldInvTranspose(ModelworldInvTranspose);
	Effects::BasicFX->SetDiffuseMap(m_ImageSRV);

	m_d3dImmediateContext->IASetVertexBuffers(0, 1, &m_VB, &m_stride, &m_offset);
	m_d3dImmediateContext->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, 0);
	activeTech->GetPassByIndex(0)->Apply(0, m_d3dImmediateContext);
	m_d3dImmediateContext->DrawIndexed(m_ModelIndexCount, m_ModelIndexOffset, m_ModelVertexOffset);
}