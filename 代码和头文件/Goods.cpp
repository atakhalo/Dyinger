#include "Goods.h"

D3DGame::Goods::Goods()
{
	
}

D3DGame::Goods::~Goods()
{
}

void D3DGame::Goods::init(ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dImmediateContext)
{
	m_Device = d3dDevice;
	m_DeviceContext = d3dImmediateContext;
#pragma region Éú³ÉÎïÆ·×Ü±í
	m_Model[0].sprite.initSprite(m_Device, m_DeviceContext);
	m_Model[0].sprite.CreateModel("Model/·¬ÇÑ/model.obj");
	m_Model[0].sprite.setImageSRV(L"Model/·¬ÇÑ/model.jpg");
	m_Model[0].name = L"·¬ÇÑ";
	m_Model[0].sprite.setModelscale(0.05f, 0.05f, 0.05f);
	m_Model[1].sprite.initSprite(m_Device, m_DeviceContext);
	m_Model[1].sprite.CreateModel("Model/ºúÂÜ²·/model.obj");
	m_Model[1].sprite.setImageSRV(L"Model/ºúÂÜ²·/model.jpg");
	m_Model[1].name = L"ºúÂÜ²·";
	m_Model[1].sprite.setModelscale(0.04f,0.04f,0.04f);
	m_Model[2].sprite.initSprite(m_Device, m_DeviceContext);
	m_Model[2].sprite.CreateModel("Model/»Æ¹Ï/model.obj");
	m_Model[2].sprite.setImageSRV(L"Model/»Æ¹Ï/model.jpg");
	m_Model[2].name = L"»Æ¹Ï";
	m_Model[2].sprite.setModelscale(0.035f, 0.035f, 0.035f);
	m_Model[3].sprite.initSprite(m_Device, m_DeviceContext);
	m_Model[3].sprite.CreateModel("Model/»ÆÌÒ/model.obj");
	m_Model[3].sprite.setImageSRV(L"Model/»ÆÌÒ/model.jpg");
	m_Model[3].name = L"»ÆÌÒ";
	m_Model[3].sprite.setModelscale(0.3f, 0.3f, 0.3f);
	m_Model[4].sprite.initSprite(m_Device, m_DeviceContext);
	m_Model[4].sprite.CreateModel("Model/»ðÁú¹û/model.obj");
	m_Model[4].sprite.setImageSRV(L"Model/»ðÁú¹û/model.jpg");
	m_Model[4].name = L"»ðÁú¹û";
	m_Model[4].sprite.setModelscale(0.3f, 0.3f, 0.3f);
	m_Model[5].sprite.initSprite(m_Device, m_DeviceContext);
	m_Model[5].sprite.CreateModel("Model/ÂíÁåÊí/model.obj");
	m_Model[5].sprite.setImageSRV(L"Model/ÂíÁåÊí/model.jpg");
	m_Model[5].name = L"ÂíÁåÊí";
	m_Model[5].sprite.setModelscale(0.045f, 0.045f, 0.045f);
	m_Model[6].sprite.initSprite(m_Device, m_DeviceContext);
	m_Model[6].sprite.CreateModel("Model/â¨ºïÌÒ/model.obj");
	m_Model[6].sprite.setImageSRV(L"Model/â¨ºïÌÒ/model.jpg");
	m_Model[6].name = L"â¨ºïÌÒ";
	m_Model[6].sprite.setModelscale(0.05f, 0.05f, 0.05f);
	m_Model[7].sprite.initSprite(m_Device, m_DeviceContext);
	m_Model[7].sprite.CreateModel("Model/Ä¾¹Ï/model.obj");
	m_Model[7].sprite.setImageSRV(L"Model/Ä¾¹Ï/model.jpg");
	m_Model[7].name = L"Ä¾¹Ï";
	m_Model[7].sprite.setModelscale(0.07f, 0.07f, 0.07f);
	m_Model[8].sprite.initSprite(m_Device, m_DeviceContext);
	m_Model[8].sprite.CreateModel("Model/Æ»¹û/model.obj");
	m_Model[8].sprite.setImageSRV(L"Model/Æ»¹û/model.jpg");
	m_Model[8].name = L"Æ»¹û";
	m_Model[8].sprite.setModelscale(0.2f, 0.2f, 0.2f);
	m_Model[9].sprite.initSprite(m_Device, m_DeviceContext);
	m_Model[9].sprite.CreateModel("Model/Ïã¹½/model.obj");
	m_Model[9].sprite.setImageSRV(L"Model/Ïã¹½/model.jpg");
	m_Model[9].name = L"Ïã¹½";
	m_Model[9].sprite.setModelscale(0.5f, 0.5f, 0.5f);
#pragma endregion
}

void D3DGame::Goods::create(std::vector<XMFLOAT3> modelpositon)
{
	m_Models.resize(10);
	for (int i = 0; i != 10; ++i)
	{
		m_pModel = &m_Model[i];
		//¾ö¶¨½×¶Î
		switch (rand() % 4)
		{
		case 0:
			m_pModel->type = SEED;
			break;
		case 1:
			m_pModel->type = GROW;
			break;
		case 2:
			m_pModel->type = RIPE;
			break;
		case 3:
			m_pModel->type = ROT;
			break;
		}
		//ÉèÖÃÎ»ÖÃ
		m_pModel->sprite.setTotalTranslation(modelpositon[i].x, modelpositon[i].y, modelpositon[i].z);
		//´æ½øµ±Ç°±í
		m_Models[i] = m_pModel;
	}
}

void D3DGame::Goods::draw(ID3DX11EffectTechnique* tech, XMFLOAT4X4 ViewProj)
{
	XMMATRIX viewProj = XMLoadFloat4x4(&ViewProj);
	
	for (int i = 0; i != m_Models.size(); ++i)
	{
		XMMATRIX ModelworldViewProj = m_Models[i]->sprite.getModelWorld()*viewProj;
		Effects::BasicFX->SetWorldViewProj(ModelworldViewProj);
		//if(i != 9)
		m_Models[i]->sprite.draw(tech);
	}
}

std::pair<D3DGame::TYPE, std::wstring> D3DGame::Goods::pickmodel(XMFLOAT4X4 proj, XMFLOAT4X4 view, int x, int y, int ClientWidth, int ClientHeight)
{
	XMMATRIX P = XMLoadFloat4x4(&proj);
	float vx = (+2.0f*x / ClientWidth - 1.0f) / P(0, 0);
	float vy = (-2.0f*y / ClientHeight + 1.0f) / P(1, 1);
	for (auto it = m_Models.begin(); it != m_Models.end(); ++it)
	{
		if((*it)->sprite.pickModel(view, vx, vy) != -1)
		{
			std::pair<TYPE, std::wstring> pickMsg((*it)->type, (*it)->name);
			m_Models.erase(it);
			return pickMsg;
		}
	}
	return std::pair<TYPE, std::wstring>(NOGOOD,std::wstring());
	
}
