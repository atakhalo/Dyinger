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
		NORMAL,									//����
		PICKGOOD,							//ʰȡ����Ʒ
		PICKNPC,									//ʰȡ��NPC
		DAYBIN,									//�µ�һ��
		DAYEND,									//����һ��
		TURNTODAY,						//�������
		TURNTONIGHT,					//����ҹ��
		DIED,											//����
		EXIT,												//�˳�
	};

	class Player : public Object
	{
	public:
		void init(ID3D11Device * d3dDevice, ID3D11DeviceContext * d3dImmediateContext);
		//void create();
		void draw(ID3DX11EffectTechnique * tech, XMFLOAT4X4 ViewProj);
		
		//����Ӱ��
		std::wstring weatherEffect(Weather weather);
		//��ƷӰ��
		std::wstring pickGoods(std::pair<D3DGame::TYPE, std::wstring>   m_goods);
		//NPCӰ��
		std::wstring pickNPC(NPCAction  npcAction);
		//һ�������
		std::wstring AfterDay();
		//���컹�Ǻ�ҹ
		void inDay(bool day);
		
		int getHP();
		float getSpeed();
		float getFogStart();

		void setPlayerPos(float x, float y, float z);
		void setPlayerRotation(float x, float y, float z);
		void setPlayerPos(XMFLOAT3 pos);
		void setPlayerRotation(float rotation);

	private :
		Sprite  m_Player;																										//����
		
		float m_FogRangeEffect;																								//��Ұ �仯
		float m_SpeedEffect;																										//���ٱ仯

		float m_FogStart;																								//��Ұ
		float m_Speed;																										//����
		int m_HP;																											//Ѫ��
		
		std::vector<std::pair<D3DGame::TYPE, std::wstring>>  m_Bag;			//����
		
		//XMFLOAT3 m_Postion;																						//λ��
		
		bool m_PickGood;																									//һ���Ƿ��м���Ʒ
		bool m_InDay;																											//����
		Weather m_Weather;																								//����	
	};
}