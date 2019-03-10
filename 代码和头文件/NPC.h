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

	class NPC : public  Object
	{
	public:
		NPC() = default;
		~NPC() = default;

		void init(ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dImmediateContext);
		void create(std::vector<XMFLOAT3> NPCpositon);
		void draw(ID3DX11EffectTechnique* tech, XMFLOAT4X4 ViewProj);

		NPCAction  pickNPC(XMFLOAT4X4 proj, XMFLOAT4X4 view, int x, int y, int ClientWidth, int ClientHeight);

		 Sprite m_NPC[4];
	};
}