//************************************************************************************
//���ڲ��Ź�������
//ʹ��ʵ��:
//
//************************************************************************************
#pragma once
#include "d3dUtil.h"
#include "Object.h"
#include "GeometryGenerator.h"
#include "Effects.h"
#include <io.h>
namespace D3DGame
{
	class GameStart : public D3DGame::Object
	{
	public :
		GameStart();
		~GameStart();
		
		//��ʼ��������ͼƬ
		void init(ID3D11Device* device, ID3D11DeviceContext* dc );
		//��������
		void create();
		//����
		void draw(IDXGISwapChain* SwapChain,	ID3D11RenderTargetView* RenderTargetView, ID3D11DepthStencilView* DepthStencilView);
	private:
		//��ȡ�ļ������ļ���
		void getFiles(std::string path, std::vector<std::string>& files);
		//stringתwstring
		std::wstring c2w(LPCSTR pszSrc, int nLen);
		//ͼƬ����
		std::vector<ID3D11ShaderResourceView*> m_imageSRV;
		//ͼƬ��
		std::vector<std::string> m_ImageName;
	};
}