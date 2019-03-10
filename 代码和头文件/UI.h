#pragma once

#include "d3dUtil.h"
#include "D2Dtext.h"
#include "Object.h"
#include "GeometryGenerator.h"
#include "Effects.h"
#include "RenderStates.h"

namespace D3DGame
{
	class UI : public D3DGame::Object
	{
	public :
		UI();
		~UI();

		void init(ID3D11Device* device, ID3D11DeviceContext* dc);
		void create();
		void drawNPC(Weather weather);
		void drawPlayer(Weather weather);
		void drawMsg(Weather weather);

	private:
		void draw();

		ID3D11ShaderResourceView* m_NPCSRV;
		ID3D11ShaderResourceView* m_PlayerSRV;

		ID3D11ShaderResourceView* m_HotSRV;
		ID3D11ShaderResourceView* m_CloudySRV;
		ID3D11ShaderResourceView* m_SprinkleSRV;
		ID3D11ShaderResourceView* m_RainStormSRV;
		
		ID3D11ShaderResourceView* m_SunnyMsgSRV;
		ID3D11ShaderResourceView* m_HotMsgSRV;
		ID3D11ShaderResourceView* m_CloudyMsgSRV;
		ID3D11ShaderResourceView* m_SprinkleMsgSRV;
		ID3D11ShaderResourceView* m_RainStormMsgSRV;

		XMMATRIX I;

	Material m_Mat;
	DirectionalLight m_DirLight[3];
	};
}