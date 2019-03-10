#include "Player.h"

void D3DGame::Player::init(ID3D11Device * d3dDevice, ID3D11DeviceContext * d3dImmediateContext)
{
	m_Device = d3dDevice;
	m_DeviceContext = d3dImmediateContext;
#pragma region     主角模型载入
	m_Player.initSprite(m_Device, m_DeviceContext);
	m_Player.CreateModel("player/dpe.obj");
	m_Player.setImageSRV(L"player/dp12.jpg");
	m_Player.setModelscale(0.08f, 0.08f, 0.08f);
	m_Player.setModelTranslation(0.0f, -3.0f, 0.0f);
#pragma endregion

#pragma region 初始化变量
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
	//玩家模型不被迷雾影响
	m_Player.draw(Effects::BasicFX->Light1TexAlphaClipTech);

}

std::wstring D3DGame::Player::weatherEffect(Weather weather)
{
	std::wstring Msg{};
	m_Weather = weather;
	switch (weather)
	{
	case SUNNY :										//晴
		m_FogRangeEffect += 5.0f;
		Msg = L"新的一天开始了!今天阳光明媚，视野+5!";
		break;
	case		HOT :											//暑天
		m_FogRangeEffect += 7.0f;
		m_SpeedEffect -= 0.1f;
		m_HP -= 1;
		Msg = L"新的一天开始了!今天烈日炎炎，视野+7,移速-10%,HP-1!";
		break;
	case		CLOUDY :									//阴天
		m_FogRangeEffect -= 5.0f;
		Msg = L"新的一天开始了!今天乌云密布，视野-5!";
		break;
	case		WINDY :										//狂风
		m_SpeedEffect -= 0.3f;
		Msg = L"新的一天开始了!今天狂风大作，移速-30%!";
		break;
	case		SPRINKLE :									//小雨
		m_SpeedEffect -= 0.1f;
		m_FogRangeEffect -= 3.0f;
		Msg = L"新的一天开始了!今天微微小雨，移速-10%,视野-3!";
		break;
	case		RAINSTORM :							//暴雨
		m_FogRangeEffect -= 7.0f;
		m_SpeedEffect -= 0.2f;
		m_HP -= 1;
		Msg = L"新的一天开始了!今天滂沱大雨，移速-20%!,视野-7,HP-1!";
		break;
	case		DAY :												//白天
		Msg = L"新的一天开始了!";
		break;
	case		NIGHT :										//黑夜
		m_FogRangeEffect -= 5.0f;
		Msg = L"入夜了，视野-5!";
		break;
	}
	return Msg;
}

std::wstring D3DGame::Player::pickGoods(std::pair<D3DGame::TYPE, std::wstring> goods)
{
	std::wstring PickMsg{};
	switch (goods.first)
	{
	case SEED:														//幼苗放入背包
		m_Bag.push_back(goods);
		PickMsg = L"你捡到了一个幼苗期的" + goods.second + L"，并放入了背包。";
		break;
	case GROW:													//生长期也放入背包
		m_Bag.push_back(goods);				
		PickMsg = L"你捡到了一个生长期的" + goods.second + L"，并放入了背包。";
		break;
	case RIPE:															//成熟期加血
		m_HP += 1;
		PickMsg = L"你吃了一个美味的" + goods.second + L"，HP+1!";
		break;
	case ROT:															//腐烂期掉血
		m_HP -= 1;
		PickMsg = L"你吃了一个腐烂的" + goods.second + L"，HP-1!";
		break;
	}
	m_PickGood = true;										//今天捡到物品
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
			PickMsg = L"你受到攻击，HP-1!";
		}
		//夜晚扣两血
		else
		{
			m_HP -= 2;
			PickMsg = L"你受到攻击，HP-2!";
		}
		break;
	case ROB:
	{	
		if (!m_Bag.size())
		{
			m_HP -= 1;
			PickMsg = L"因为没有东西可抢，敌人攻击了你，HP-1!";
			return PickMsg;
		}
		int i = rand() % m_Bag.size();
		PickMsg = L"你的"+(m_Bag[i].second) +L"被抢了!";
		m_Bag.erase(m_Bag.begin() + i);
		//夜晚抢劫两个物品
		if (!m_InDay)
		{
			int i = rand() % m_Bag.size();
			if (!m_Bag.size())
			{
				m_HP -= 1;
				PickMsg += L"因为没有东西可抢，敌人攻击了你，HP-1!";
			}
			else
			{
				m_Bag.erase(m_Bag.begin() + i);
				PickMsg += L"你的" + (m_Bag[i].second) + L"被抢了!";
			}
		}
	}
	break;
	case CHANGE:	
	{
		if (!m_Bag.size())
		{
			PickMsg = L"你的背包空空如也，交易失败";
			return PickMsg;
		}
		int i = rand() % m_Bag.size();
		PickMsg = L"对方对你的" + m_Bag[i].second + L"很感兴趣，交易成功!";
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
				PickMsg += L"加速10%!";
			}
			else
			{
				m_Speed += 0.15;
				PickMsg += L"加速15%!";
			}
		}
		else if (i == 2)
		{
			if (m_InDay)
			{
				m_FogStart += 5.0f;
				PickMsg += L"视野+5!";
			}
			else
			{
				m_FogStart += 7.0f;
				PickMsg += L"视野+7";
			}
		}
	}
	break;
	default :
		PickMsg = L"对方不想和你说话!";
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
		Msg = L"一天转瞬即逝。HP-1!";
	}
	else
	{
		m_HP -= 2;
		Msg = L"今天颗粒无收。HP-2";
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
