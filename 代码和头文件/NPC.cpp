#include "NPC.h"

void D3DGame::NPC::init(ID3D11Device * d3dDevice, ID3D11DeviceContext * d3dImmediateContext)
{
	m_Device = d3dDevice;
	m_DeviceContext = d3dImmediateContext;
#pragma region Éú³ÉNPC
	m_NPC[0].initSprite(m_Device, m_DeviceContext);
	m_NPC[0].CreateModel("Model/NPC/ta.obj");
	m_NPC[0].setImageSRV(L"Model/NPC/ta.jpg");
	m_NPC[0].setModelscale(0.007f, 0.007f, 0.007f);
	m_NPC[1].initSprite(m_Device, m_DeviceContext);
	m_NPC[1].CreateModel("Model/NPC/ta.obj");
	m_NPC[1].setImageSRV(L"Model/NPC/water.dds");
	m_NPC[1].setModelscale(0.007f, 0.007f, 0.007f);
	m_NPC[2].initSprite(m_Device, m_DeviceContext);
	m_NPC[2].CreateModel("Model/NPC/ta.obj");
	m_NPC[2].setImageSRV(L"Model/NPC/green.jpg");
	m_NPC[2].setModelscale(0.007f, 0.007f, 0.007f);
	m_NPC[3].initSprite(m_Device, m_DeviceContext);
	m_NPC[3].CreateModel("Model/NPC/ta.obj");
	m_NPC[3].setImageSRV(L"Model/NPC/yellow.jpg");
	m_NPC[3].setModelscale(0.007f, 0.007f, 0.007f);
#pragma endregion
}

void D3DGame::NPC::create(std::vector<XMFLOAT3> NPCpositon)
{
	for (int i = 0; i != 4; ++i)
	{
		m_NPC[i].setModelTranslation(NPCpositon[i].x, NPCpositon[i].y, NPCpositon[i].z);
	}
}

void D3DGame::NPC::draw(ID3DX11EffectTechnique * tech, XMFLOAT4X4 ViewProj)
{
	XMMATRIX viewProj = XMLoadFloat4x4(&ViewProj);
	for (int i = 0; i != 4; ++i)
	{
		XMMATRIX ModelworldViewProj = m_NPC[i].getModelWorld()*viewProj;
		Effects::BasicFX->SetWorldViewProj(ModelworldViewProj);
		m_NPC[i].draw(tech);
	}
}

D3DGame::NPCAction D3DGame::NPC::pickNPC(XMFLOAT4X4 proj, XMFLOAT4X4 view, int x, int y, int ClientWidth, int ClientHeight)
{
	XMMATRIX P = XMLoadFloat4x4(&proj);
	float vx = (+2.0f*x / ClientWidth - 1.0f) / P(0, 0);
	float vy = (-2.0f*y / ClientHeight + 1.0f) / P(1, 1);
	for (int i = 0; i != 4; ++i)
	{
		if (m_NPC[i].pickModel(view, vx, vy) != -1)
		{
			switch (rand() % 4)
			{
			case 0:
				return ATTACK;
			case 1:
				return ROB;
			case 2:
			case 3:
				return CHANGE;
			}
		}
	}
	return NOPICK;
}

