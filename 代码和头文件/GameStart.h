//************************************************************************************
//用于播放过场动画
//使用实例:
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
		
		//初始化和载入图片
		void init(ID3D11Device* device, ID3D11DeviceContext* dc );
		//创建缓存
		void create();
		//绘制
		void draw(IDXGISwapChain* SwapChain,	ID3D11RenderTargetView* RenderTargetView, ID3D11DepthStencilView* DepthStencilView);
	private:
		//获取文件夹内文件名
		void getFiles(std::string path, std::vector<std::string>& files);
		//string转wstring
		std::wstring c2w(LPCSTR pszSrc, int nLen);
		//图片缓存
		std::vector<ID3D11ShaderResourceView*> m_imageSRV;
		//图片名
		std::vector<std::string> m_ImageName;
	};
}