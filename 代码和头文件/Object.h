#pragma once
#include "Vertex.h"
#include <vector>
namespace D3DGame
{
	//��Ʒ�׶�
	enum TYPE
	{
		SEED,				//����
		GROW,			//������
		RIPE,					//������
		ROT,					//������
		NOGOOD,	//ûʰȡ��
	};
	//NPC��Ϊ
	enum NPCAction
	{
		ATTACK,		//����
		ROB,					//����
		CHANGE,		//����
		NOPICK,		//ûʰȡ��
		};
	//����
	enum Weather
	{
		SUNNY,										//��
		HOT,											//����
		CLOUDY,									//����
		WINDY,										//���
		SPRINKLE,									//С��
		RAINSTORM,							//����
		DAY,												//����
		NIGHT,										//��ҹ
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