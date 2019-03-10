#pragma once
#include "d3dUtil.h"
#include "xnacollision.h"
#include "Object.h"
#include "Effects.h"
#include "Vertex.h"
#include "newSprite.h"
#include <ctime>

namespace D3DGame
{
	enum PlayerState
	{
		NORMAL,									//正常
		PICKGOOD,							//拾取到物品
		PICKNPC,									//拾取到NPC
		DAYBIN,									//新的一天
		DAYEND,									//结束一天
		TURNTODAY,						//进入白天
		TURNTONIGHT,					//进入夜晚
		DIED,											//死亡
		EXIT,												//退出
	};

	class Player : public Object
	{
	public:
		void init(ID3D11Device * d3dDevice, ID3D11DeviceContext * d3dImmediateContext);
		//void create();
		void draw(ID3DX11EffectTechnique * tech, XMFLOAT4X4 ViewProj);
		
		//天气影响
		std::wstring weatherEffect(Weather weather);
		//物品影响
		std::wstring pickGoods(std::pair<D3DGame::TYPE, std::wstring>   m_goods);
		//NPC影响
		std::wstring pickNPC(NPCAction  npcAction);
		//一天结束后
		std::wstring AfterDay();
		//白天还是黑夜
		void inDay(bool day);
		
		int getHP();
		float getSpeed();
		float getFogStart();

		void setPlayerPos(float x, float y, float z);
		void setPlayerRotation(float x, float y, float z);
		void setPlayerPos(XMFLOAT3 pos);
		void setPlayerRotation(float rotation);

	private :
		Sprite  m_Player;																										//主角
		
		float m_FogRangeEffect;																								//视野 变化
		float m_SpeedEffect;																										//移速变化

		float m_FogStart;																								//视野
		float m_Speed;																										//移速
		int m_HP;																											//血量
		
		std::vector<std::pair<D3DGame::TYPE, std::wstring>>  m_Bag;			//背包
		
		//XMFLOAT3 m_Postion;																						//位置
		
		bool m_PickGood;																									//一天是否有捡到物品
		bool m_InDay;																											//白天
		Weather m_Weather;																								//天气	
	};
}