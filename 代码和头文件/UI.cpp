#include "UI.h"

D3DGame::UI::UI()
{
	I = XMMatrixIdentity();
}

D3DGame::UI::~UI()
{
	ReleaseCOM(m_NPCSRV);
	ReleaseCOM(m_PlayerSRV);
	ReleaseCOM(m_HotSRV);
	ReleaseCOM(m_CloudySRV);
	ReleaseCOM(m_SprinkleSRV);
	ReleaseCOM(m_RainStormSRV);
	ReleaseCOM(m_SunnyMsgSRV);
	ReleaseCOM(m_HotMsgSRV);
	ReleaseCOM(m_CloudyMsgSRV);
	ReleaseCOM(m_SprinkleMsgSRV);
	ReleaseCOM(m_RainStormMsgSRV);
}

void D3DGame::UI::init(ID3D11Device* device, ID3D11DeviceContext* dc)
{
	m_Device = device;
	m_DeviceContext = dc;
	
	m_Mat.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Mat.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 64.0f);
	m_Mat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	
	m_DirLight[0].Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_DirLight[0].Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_DirLight[0].Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
	m_DirLight[0].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);
	
	HR(D3DX11CreateShaderResourceViewFromFile(m_Device,
		L"UI/Player.png", 0, 0, &m_PlayerSRV, 0));
	HR(D3DX11CreateShaderResourceViewFromFile(m_Device,
		L"UI/NPC.png", 0, 0, &m_NPCSRV, 0));
	HR(D3DX11CreateShaderResourceViewFromFile(m_Device,
		L"UI/—◊»»±≥æ∞.png", 0, 0, &m_HotSRV, 0));
	HR(D3DX11CreateShaderResourceViewFromFile(m_Device,
		L"UI/“ıÃÏ±≥æ∞.png", 0, 0, &m_CloudySRV, 0));
		HR(D3DX11CreateShaderResourceViewFromFile(m_Device,
			L"UI/–°”Í±≥æ∞.png", 0, 0, &m_SprinkleSRV, 0));
	HR(D3DX11CreateShaderResourceViewFromFile(m_Device,
		L"UI/±©”Í±≥æ∞.png", 0, 0, &m_RainStormSRV, 0));
		HR(D3DX11CreateShaderResourceViewFromFile(m_Device,
			L"UI/«ÁÃÏ.png", 0, 0, &m_SunnyMsgSRV, 0));
	HR(D3DX11CreateShaderResourceViewFromFile(m_Device,
		L"UI/—◊»».png", 0, 0, &m_HotMsgSRV, 0));
	HR(D3DX11CreateShaderResourceViewFromFile(m_Device,
		L"UI/“ıÃÏ.png", 0, 0, &m_CloudyMsgSRV, 0));
	HR(D3DX11CreateShaderResourceViewFromFile(m_Device,
		L"UI/–°”Í.png", 0, 0, &m_SprinkleMsgSRV, 0));
	HR(D3DX11CreateShaderResourceViewFromFile(m_Device,
		L"UI/±©”Í.png", 0, 0, &m_RainStormMsgSRV, 0));
}

void D3DGame::UI::create()
{
	GeometryGenerator::MeshData grid;
	GeometryGenerator geoGen;
	geoGen.CreateBox(2.0f, 2.0f, 2.0f, grid);
	std::vector<Vertex::Basic32> vertices(4);
	for (size_t i = 0; i != 4; ++i)
	{
		vertices[i].Pos = grid.Vertices[i].Position;
		vertices[i].Pos.z += 1.0f;
		vertices[i].Normal = grid.Vertices[i].Normal;
		vertices[i].Tex = grid.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * 4;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(m_Device->CreateBuffer(&vbd, &vinitData, &m_VB));

	m_Indices.insert(m_Indices.end(), grid.Indices.begin(), grid.Indices.end());
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 6;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &m_Indices[0];
	HR(m_Device->CreateBuffer(&ibd, &iinitData, &m_IB));
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_DeviceContext->IASetInputLayout(InputLayouts::Basic32);
}

void D3DGame::UI::draw()
{
	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;
	m_DeviceContext->IASetVertexBuffers(0, 1, &m_VB, &stride, &offset);
	m_DeviceContext->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, 0);

	Effects::BasicFX->SetTexTransform(I);
	Effects::BasicFX->SetMaterial(m_Mat);
	Effects::BasicFX->SetWorld(I);
	Effects::BasicFX->SetWorldInvTranspose(I);
	Effects::BasicFX->SetWorldViewProj(I);
	Effects::BasicFX->SetDirLights(m_DirLight);
}

void D3DGame::UI::drawNPC(Weather weather)
{
	draw();

	ID3DX11EffectTechnique* tech = Effects::BasicFX->Light1TexAlphaClipTech;
	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);
	ID3DX11EffectPass* pass = tech->GetPassByIndex(0);

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_DeviceContext->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);
	Effects::BasicFX->SetDiffuseMap(m_NPCSRV);
	pass->Apply(0, m_DeviceContext);
	m_DeviceContext->DrawIndexed(6, 0, 0);
	switch (weather)
	{
	case  SUNNY:
		return;
	case		HOT:
		Effects::BasicFX->SetDiffuseMap(m_HotSRV);
		break;
	case		CLOUDY:
		Effects::BasicFX->SetDiffuseMap(m_CloudySRV);
		break;
	case		WINDY:
		return;
	case		SPRINKLE:
		Effects::BasicFX->SetDiffuseMap(m_SprinkleSRV);
		break;
	case		RAINSTORM:
		Effects::BasicFX->SetDiffuseMap(m_RainStormSRV);
		break;
	}
	pass->Apply(0, m_DeviceContext);
	m_DeviceContext->DrawIndexed(6, 0, 0);



	m_DeviceContext->OMSetBlendState(0, blendFactor, 0xffffffff);
}

void D3DGame::UI::drawPlayer(Weather weather)
{
	draw();

	ID3DX11EffectTechnique* tech = Effects::BasicFX->Light1TexAlphaClipTech;
	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);
	ID3DX11EffectPass* pass = tech->GetPassByIndex(0);

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_DeviceContext->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);
	
	Effects::BasicFX->SetDiffuseMap(m_PlayerSRV);
	pass->Apply(0, m_DeviceContext);
	m_DeviceContext->DrawIndexed(6, 0, 0);

	switch (weather)
	{
	case  SUNNY:
		return;
	case		HOT:
		Effects::BasicFX->SetDiffuseMap(m_HotSRV);
		break;
	case		CLOUDY:
		Effects::BasicFX->SetDiffuseMap(m_CloudySRV);
		break;
	case		WINDY:
		return;
	case		SPRINKLE:
		Effects::BasicFX->SetDiffuseMap(m_SprinkleSRV);
		break;
	case		RAINSTORM:
		Effects::BasicFX->SetDiffuseMap(m_RainStormSRV);
		break;
	}
	pass->Apply(0, m_DeviceContext);
	m_DeviceContext->DrawIndexed(6, 0, 0);


	
	m_DeviceContext->OMSetBlendState(0, blendFactor, 0xffffffff);
}

void D3DGame::UI::drawMsg(Weather weather)
{
	draw();

	ID3DX11EffectTechnique* tech = Effects::BasicFX->Light1TexTech;
	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);
	ID3DX11EffectPass* pass = tech->GetPassByIndex(0);

	switch (weather)
	{
	case  SUNNY:
		Effects::BasicFX->SetDiffuseMap(m_SunnyMsgSRV);
		break;
	case		HOT:
		Effects::BasicFX->SetDiffuseMap(m_HotMsgSRV);
		break;
	case		CLOUDY:
		Effects::BasicFX->SetDiffuseMap(m_CloudyMsgSRV);
		break;
	case		WINDY:
		Effects::BasicFX->SetDiffuseMap(m_SunnyMsgSRV);
		break;
	case		SPRINKLE:
		Effects::BasicFX->SetDiffuseMap(m_SprinkleMsgSRV);
		break;
	case		RAINSTORM:
		Effects::BasicFX->SetDiffuseMap(m_RainStormMsgSRV);
		break;
	}

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_DeviceContext->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);
	pass->Apply(0, m_DeviceContext);
	m_DeviceContext->DrawIndexed(6, 0, 0);
	m_DeviceContext->OMSetBlendState(0, blendFactor, 0xffffffff);
}

