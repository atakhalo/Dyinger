#pragma once
#include "Vertex.h"
#include <vector>
namespace D3DGame
{
	//物品阶段
	enum TYPE
	{
		SEED,				//幼苗
		GROW,			//生长期
		RIPE,					//成熟期
		ROT,					//腐烂期
		NOGOOD,	//没拾取到
	};
	//NPC行为
	enum NPCAction
	{
		ATTACK,		//攻击
		ROB,					//抢劫
		CHANGE,		//交易
		NOPICK,		//没拾取到
		};
	//天气
	enum Weather
	{
		SUNNY,										//晴
		HOT,											//暑天
		CLOUDY,									//阴天
		WINDY,										//狂风
		SPRINKLE,									//小雨
		RAINSTORM,							//暴雨
		DAY,												//白天
		NIGHT,										//黑夜
	};

	class Object
	{
	public :
		Object() {}
		~Object() 
		{
			ReleaseCOM(m_VB);
			ReleaseCOM(m_IB);
		}
		virtual void init() {}
		virtual void create() {}
		virtual void draw() {}
		
		std::vector<Vertex::Basic32> m_Vertex;
		std::vector<UINT> m_Indices;

		ID3D11Buffer* m_VB = 0;
		ID3D11Buffer* m_IB = 0;

		ID3D11Device* m_Device; 
		ID3D11DeviceContext* m_DeviceContext;
	};
}