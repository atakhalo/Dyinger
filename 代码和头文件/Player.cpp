#include "Player.h"

void D3DGame::Player::init(ID3D11Device * d3dDevice, ID3D11DeviceContext * d3dImmediateContext)
{
	m_Device = d3dDevice;
	m_DeviceContext = d3dImmediateContext;
#pragma region     ����ģ������
	m_Player.initSprite(m_Device, m_DeviceContext);
	m_Player.CreateModel("player/dpe.obj");
	m_Player.setImageSRV(L"player/dp12.jpg");
	m_Player.setModelscale(0.08f, 0.08f, 0.08f);
	m_Player.setModelTranslation(0.0f, -3.0f, 0.0f);
#pragma endregion

#pragma region ��ʼ������
	m_SpeedEffect = 0.0f;
	m_FogRangeEffect = 0.0f;

	m_Speed = 1.0f;			
	m_FogStart = 20.0f;
	m_HP = 5.0f;
	m_PickGood = false;
	m_InDay = true;

#pragma endregion
}

void D3DGame::Player::draw(ID3DX11EffectTechnique * tech, XMFLOAT4X4 ViewProj)
{
	XMMATRIX viewProj = XMLoadFloat4x4(&ViewProj);
	XMMATRIX ModelworldViewProj = m_Player.getModelWorld() * viewProj;
	Effects::BasicFX->SetWorldViewProj(ModelworldViewProj);
	if (m_InDay)
	{
		Effects::TerrainFX->SetFogColor(Colors::Silver);
		Effects::BasicFX->SetFogColor(Colors::Silver);
	}
	else
	{
		Effects::TerrainFX->SetFogColor(Colors::Black);
		Effects::BasicFX->SetFogColor(Colors::Black);
	}
	Effects::TerrainFX->SetFogStart(m_FogStart + m_FogRangeEffect);
	Effects::TerrainFX->SetFogRange(75.0f);
	Effects::BasicFX->SetFogStart(m_FogStart  +  m_FogRangeEffect);
	Effects::BasicFX->SetFogRange(75.0f);
	//���ģ�Ͳ�������Ӱ��
	m_Player.draw(Effects::BasicFX->Light1TexAlphaClipTech);

}

std::wstring D3DGame::Player::weatherEffect(Weather weather)
{
	std::wstring Msg{};
	m_Weather = weather;
	switch (weather)
	{
	case SUNNY :										//��
		m_FogRangeEffect += 5.0f;
		Msg = L"�µ�һ�쿪ʼ��!�����������ģ���Ұ+5!";
		break;
	case		HOT :											//����
		m_FogRangeEffect += 7.0f;
		m_SpeedEffect -= 0.1f;
		m_HP -= 1;
		Msg = L"�µ�һ�쿪ʼ��!�����������ף���Ұ+7,����-10%,HP-1!";
		break;
	case		CLOUDY :									//����
		m_FogRangeEffect -= 5.0f;
		Msg = L"�µ�һ�쿪ʼ��!���������ܲ�����Ұ-5!";
		break;
	case		WINDY :										//���
		m_SpeedEffect -= 0.3f;
		Msg = L"�µ�һ�쿪ʼ��!���������������-30%!";
		break;
	case		SPRINKLE :									//С��
		m_SpeedEffect -= 0.1f;
		m_FogRangeEffect -= 3.0f;
		Msg = L"�µ�һ�쿪ʼ��!����΢΢С�꣬����-10%,��Ұ-3!";
		break;
	case		RAINSTORM :							//����
		m_FogRangeEffect -= 7.0f;
		m_SpeedEffect -= 0.2f;
		m_HP -= 1;
		Msg = L"�µ�һ�쿪ʼ��!�����������꣬����-20%!,��Ұ-7,HP-1!";
		break;
	case		DAY :												//����
		Msg = L"�µ�һ�쿪ʼ��!";
		break;
	case		NIGHT :										//��ҹ
		m_FogRangeEffect -= 5.0f;
		Msg = L"��ҹ�ˣ���Ұ-5!";
		break;
	}
	return Msg;
}

std::wstring D3DGame::Player::pickGoods(std::pair<D3DGame::TYPE, std::wstring> goods)
{
	std::wstring PickMsg{};
	switch (goods.first)
	{
	case SEED:														//������뱳��
		m_Bag.push_back(goods);
		PickMsg = L"�����һ�������ڵ�" + goods.second + L"���������˱�����";
		break;
	case GROW:													//������Ҳ���뱳��
		m_Bag.push_back(goods);				
		PickMsg = L"�����һ�������ڵ�" + goods.second + L"���������˱�����";
		break;
	case RIPE:															//�����ڼ�Ѫ
		m_HP += 1;
		PickMsg = L"�����һ����ζ��" + goods.second + L"��HP+1!";
		break;
	case ROT:															//�����ڵ�Ѫ
		m_HP -= 1;
		PickMsg = L"�����һ�����õ�" + goods.second + L"��HP-1!";
		break;
	}
	m_PickGood = true;										//�������Ʒ
	return PickMsg;
}

std::wstring D3DGame::Player::pickNPC(NPCAction npcAction)
{
	std::wstring PickMsg{};
	switch (npcAction)
	{
	case ATTACK:
		if (m_InDay)
		{
			m_HP -= 1;
			PickMsg = L"���ܵ�������HP-1!";
		}
		//ҹ�����Ѫ
		else
		{
			m_HP -= 2;
			PickMsg = L"���ܵ�������HP-2!";
		}
		break;
	case ROB:
	{	
		if (!m_Bag.size())
		{
			m_HP -= 1;
			PickMsg = L"��Ϊû�ж������������˹������㣬HP-1!";
			return PickMsg;
		}
		int i = rand() % m_Bag.size();
		PickMsg = L"���"+(m_Bag[i].second) +L"������!";
		m_Bag.erase(m_Bag.begin() + i);
		//ҹ������������Ʒ
		if (!m_InDay)
		{
			int i = rand() % m_Bag.size();
			if (!m_Bag.size())
			{
				m_HP -= 1;
				PickMsg += L"��Ϊû�ж������������˹������㣬HP-1!";
			}
			else
			{
				m_Bag.erase(m_Bag.begin() + i);
				PickMsg += L"���" + (m_Bag[i].second) + L"������!";
			}
		}
	}
	break;
	case CHANGE:	
	{
		if (!m_Bag.size())
		{
			PickMsg = L"��ı����տ���Ҳ������ʧ��";
			return PickMsg;
		}
		int i = rand() % m_Bag.size();
		PickMsg = L"�Է������" + m_Bag[i].second + L"�ܸ���Ȥ�����׳ɹ�!";
		m_Bag.erase(m_Bag.begin() + i);
		i = rand() % 3;
		if (i == 0)
		{
			if (m_InDay)
			{
				m_HP += 1;
				PickMsg += L"HP+1!";
			}
			else
			{
				m_HP += 2;
				PickMsg += L"HP+2!";
			}
		}
		else if (i == 1)
		{
			if (m_InDay)
			{
				m_Speed += 0.1f;
				PickMsg += L"����10%!";
			}
			else
			{
				m_Speed += 0.15;
				PickMsg += L"����15%!";
			}
		}
		else if (i == 2)
		{
			if (m_InDay)
			{
				m_FogStart += 5.0f;
				PickMsg += L"��Ұ+5!";
			}
			else
			{
				m_FogStart += 7.0f;
				PickMsg += L"��Ұ+7";
			}
		}
	}
	break;
	default :
		PickMsg = L"�Է��������˵��!";
		break;
	}
	return PickMsg;
}

std::wstring D3DGame::Player::AfterDay()
{
	std::wstring Msg{};
	if (m_PickGood)
	{
		m_HP -= 1;
		Msg = L"һ��ת˲���š�HP-1!";
	}
	else
	{
		m_HP -= 2;
		Msg = L"����������ա�HP-2";
	}
	m_PickGood = false;
	m_FogRangeEffect = 0.0f;
	m_SpeedEffect = 0.0f;
	return Msg;
}

void D3DGame::Player::inDay(bool day)
{
	m_InDay = day;
}

int D3DGame::Player::getHP()
{
	return m_HP;
}

float D3DGame::Player::getSpeed()
{
	return (m_Speed + m_SpeedEffect > 0.0f) ? m_Speed + m_SpeedEffect : 0.0f;
}

float D3DGame::Player::getFogStart()
{
	return m_FogStart + m_FogRangeEffect;
}

void D3DGame::Player::setPlayerPos(float x, float y, float z)
{
	m_Player.setTotalTranslation(x,y,z);
}

void D3DGame::Player::setPlayerRotation(float x, float y, float z)
{
	m_Player.setModelRotation(x,y,z);
}

void D3DGame::Player::setPlayerPos(XMFLOAT3 pos)
{
	m_Player.setTotalTranslation(pos.x, pos.y, pos.z);
}

void D3DGame::Player::setPlayerRotation(float rotation)
{
	m_Player.setModelRotation(0.0f, rotation,0.0f);
}
