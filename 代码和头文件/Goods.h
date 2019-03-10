//************************************************************************
//周欣晖
//物品类
//
//************************************************************************
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
	
	class Goods  : public Object
	{
	public:

		struct Model
		{
			TYPE type;
			std::wstring name;
			Sprite  sprite;
		};
		Goods() ;
		~Goods() ;

		//将物品数据全部加载
		void init(ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dImmediateContext);
		//生成随机阶段的物品
		void create(std::vector<XMFLOAT3> modelpositon);
		//绘制当前表中物品
		void draw(ID3DX11EffectTechnique* tech, XMFLOAT4X4 ViewProj);
		//判断是否选中
		std::pair<TYPE, std::wstring> pickmodel(XMFLOAT4X4 proj, XMFLOAT4X4 view, int x, int y, int ClientWidth, int ClientHeight);

		std::vector<Model*> m_Models;
		Model m_Model[10];
		Model *m_pModel;
	};
}