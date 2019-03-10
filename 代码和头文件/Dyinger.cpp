//****************************************************************
//周欣晖  荒野收菜求生游戏主框架
//
//****************************************************************
#include "d3dApp.h"
#include "Terrain.h"
#include "RenderStates.h"
#include "Effects.h"
#include "Vertex.h"
#include "FTCamera.h"
#include "Terrain.h"
#include "GameStart.h"
#include "DXinput.h"
#include "Goods.h"
#include "NPC.h"
#include "Player.h"
#include "UI.h"
#include <future>
#include <ctime>
#include "D2Dtext.h"
#include "DirectSound.h"

class Dyinger : public D3DApp
{
public:
	Dyinger(HINSTANCE hInstance);
	~Dyinger();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

private:
	Terrain m_Terrain;								//地形	
	DirectionalLight m_DirLights[3];	//灯光
	Camera* m_Camera;							//主摄影机
	Camera m_FCamera;							//第一人称
	Camera m_TCamera;						//第三人称
	DXInput m_Dxinput;						//输入类
	D3DGame::Goods m_Goods;			//物品类
	D3DGame::NPC  m_NPC;			//NPC类
	D3DGame::Player m_Player;			//玩家类

	std::vector<XMFLOAT3> m_GoodsPositon;	//物品位置
	
	XMFLOAT3  m_FCameraPos;								//摄影机位置，玩家位置
	XMFLOAT3  m_LastPos;										//记录上次的位置
	float m_Speed;																//移动速度倍数
	float m_BasicV;															//基础移动速度
	float m_BasicWLR;														//基础转身速度
	float m_BasicWUD;														//基础转身速度

	float m_Time;																//一天时间
	bool m_DayBin;															//一天开始
	bool m_DayEnd;															//一天结束
	bool m_InDay;																//是否白天
	D3DGame::Weather m_Weather;							//天气
	int m_DayNum;															//第几天

	D3DGame::NPCAction m_NpcAction;												//NPC响应
	std::pair<D3DGame::TYPE, std::wstring> m_GoodsType;			//物品响应

	bool m_Hit;																			//单击
	bool m_LongHit;																	//长按

	float m_TerrainX;																//地图正边长
	float m_TerrainZ;																//地图正边长
	float m_TerrainX_3;															//三等分
	float m_TerrainZ_3;															//三等分

	D3DGame::UI m_UI;														//界面

	D3DGame::PlayerState m_PlayerState;						//主角状态
	std::wstring m_Msg;															//信息

	D2DText m_MsgText;																//人物信息对象
	D2DText m_NPCText;															//交互信息对象
	
	D2D1_RECT_F m_TextRect;																	//文字信息区域
	D2D1_RECT_F m_HPRect;																	//血量信息区域
	D2D1_RECT_F m_SpeedRect;																//移速信息区域
	D2D1_RECT_F m_SceneRect;																//视野信息区域
	D2D1_RECT_F m_DayRect;																	//天数信息区域
	D2D1_RECT_F m_PlayerRect;																//主角在小地图的位置

	CSoundManager m_SoundManager;														//音频管理
	CSound *m_Sound;																					//音频
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Dyinger theApp(hInstance);

	//设置随机种子
	srand((unsigned)time(0));
	
	if (!theApp.Init())
		return 0;

	return theApp.Run();
}

Dyinger::Dyinger(HINSTANCE hInstance)
	: D3DApp(hInstance)
{
	mMainWndCaption = L"Dyinger";



#pragma region 灯光信息
	m_DirLights[0].Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_DirLights[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_DirLights[0].Specular = XMFLOAT4(0.8f, 0.8f, 0.7f, 1.0f);
	m_DirLights[0].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

	m_DirLights[1].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_DirLights[1].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLights[1].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLights[1].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	m_DirLights[2].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_DirLights[2].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLights[2].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLights[2].Direction = XMFLOAT3(-0.57735f, -0.57735f, -0.57735f);
#pragma endregion
}

Dyinger::~Dyinger()
{
	m_d3dImmediateContext->ClearState();

	D2DText::Release();
	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();

	SafeDelete(m_Sound);
}

bool Dyinger::Init()
{
	if (!D3DApp::Init())
		return false;

#pragma region 设置全屏
	mSwapChain->SetFullscreenState(true, NULL);
#pragma endregion

#pragma region FX,InputLayout,RS
	Effects::InitAll(m_d3dDevice);
	InputLayouts::InitAll(m_d3dDevice);
	RenderStates::InitAll(m_d3dDevice);
#pragma endregion

#pragma region 音频
	m_SoundManager.Initialize(mhMainWnd, DSSCL_PRIORITY);
	m_SoundManager.SetPrimaryBufferFormat(2, 22050, 16);
	m_SoundManager.Create(&m_Sound, L"music.wav");
	m_Sound->Play(0, DSBPLAY_LOOPING);
	//m_Sound->Stop();													//用于停止循环音乐
#pragma endregion

#pragma region 时间
	m_Time = mTimer.DeltaTime();
	m_DayBin = true;
	m_DayEnd = false;
	m_InDay = true;
	m_DayNum = 0;
#pragma endregion

#pragma region 开场动画
	ID3D11Device* d3dDevice(m_d3dDevice);
	ID3D11DeviceContext* d3dImmediateContext(m_d3dImmediateContext);
	IDXGISwapChain* SwapChain(mSwapChain);
	ID3D11RenderTargetView* RenderTargetView(mRenderTargetView);
	ID3D11DepthStencilView* DepthStencilView(mDepthStencilView);
	auto f1 = std::async(
		[d3dDevice,d3dImmediateContext, SwapChain, RenderTargetView, DepthStencilView]
	{
	D3DGame::GameStart m_gameStart;	
	m_gameStart.init(d3dDevice, d3dImmediateContext);
	m_gameStart.create();
	m_gameStart.draw(SwapChain,RenderTargetView,DepthStencilView);
	});
#pragma endregion

#pragma region 地形
	Terrain::InitInfo tii;
	tii.HeightMapFilename = L"Textures/terrain.raw";
	tii.LayerMapFilename0 = L"Textures/grass.dds";
	tii.LayerMapFilename1 = L"Textures/darkdirt.dds";
	tii.LayerMapFilename2 = L"Textures/stone.dds";
	tii.LayerMapFilename3 = L"Textures/lightdirt.dds";
	tii.LayerMapFilename4 = L"Textures/snow.dds";
	tii.BlendMapFilename = L"Textures/blend.dds";
	tii.HeightScale = 50.0f;
	tii.HeightmapWidth = 2049;
	tii.HeightmapHeight = 2049;
	tii.CellSpacing = 0.5f;
	tii.texCount = 5;
	m_Terrain.Init(m_d3dDevice, m_d3dImmediateContext, tii);
	m_TerrainX = m_Terrain.GetDepth() / 2 ;
	m_TerrainZ = m_Terrain.GetWidth() / 2 ;
	m_TerrainX_3 = m_TerrainX / 3;
	m_TerrainZ_3 = m_TerrainZ / 3;
#pragma endregion

#pragma region 物品	
	XMFLOAT3   modelPositon;
	//modelPositon = XMFLOAT3(-2 * m_TerrainX_3, m_Terrain.GetHeight(-2 * m_TerrainX_3, m_TerrainZ_3) + 2.0f, m_TerrainZ_3);                    //番茄
	modelPositon = XMFLOAT3(0.0f, m_Terrain.GetHeight(0.0f, 0.0f) + 2.0f, 0.0f);	//为方便演示放在原点
	m_GoodsPositon.push_back(modelPositon);
	//modelPositon = XMFLOAT3(2 * m_TerrainX_3, m_Terrain.GetHeight(2 * m_TerrainX_3, m_TerrainZ_3) + 2.0f, m_TerrainZ_3);                    //胡萝卜
	modelPositon = XMFLOAT3(10.0f, m_Terrain.GetHeight(10.0f, 0.0f), 0.0f);	//为方便演示放在原点附近
	m_GoodsPositon.push_back(modelPositon);
	modelPositon = XMFLOAT3(-m_TerrainX_3, m_Terrain.GetHeight(-m_TerrainX_3, 2 * m_TerrainZ_3) + 2.0f, 2 * m_TerrainZ_3);                    //黄瓜
	m_GoodsPositon.push_back(modelPositon);
	modelPositon = XMFLOAT3(m_TerrainX_3, m_Terrain.GetHeight(m_TerrainX_3, 2 * m_TerrainZ_3) + 2.0f,2 * m_TerrainZ_3);                    //黄桃
	m_GoodsPositon.push_back(modelPositon);
	modelPositon = XMFLOAT3(2 * m_TerrainX_3, m_Terrain.GetHeight(2 * m_TerrainX_3, 2 * m_TerrainZ_3) + 2.0f, 2 * m_TerrainZ_3);                    //火龙果
	m_GoodsPositon.push_back(modelPositon);
	modelPositon = XMFLOAT3(2 * m_TerrainX_3, m_Terrain.GetHeight(2 * m_TerrainX_3, -m_TerrainZ_3) + 2.0f, -m_TerrainZ_3);                    //马铃薯
	m_GoodsPositon.push_back(modelPositon);
	modelPositon = XMFLOAT3(-2 * m_TerrainX_3, m_Terrain.GetHeight(-2 * m_TerrainX_3, -m_TerrainZ_3) + 2.0f, -m_TerrainZ_3);                    //猕猴桃
	m_GoodsPositon.push_back(modelPositon);
	modelPositon = XMFLOAT3(-2 * m_TerrainX_3, m_Terrain.GetHeight(-2 * m_TerrainX_3, -2 * m_TerrainZ_3) + 2.0f, -2 * m_TerrainZ_3);                    //木瓜
	m_GoodsPositon.push_back(modelPositon);
	modelPositon = XMFLOAT3(-m_TerrainX_3, m_Terrain.GetHeight(-m_TerrainX_3, -2 * m_TerrainZ_3) + 2.0f, -2 * m_TerrainZ_3);                    //苹果
	m_GoodsPositon.push_back(modelPositon);
	modelPositon = XMFLOAT3(m_TerrainX_3, m_Terrain.GetHeight(m_TerrainX_3, -2 * m_TerrainZ_3) + 2.0f, -2 * m_TerrainZ_3);                    //香菇
	m_GoodsPositon.push_back(modelPositon);

	m_Goods.init(m_d3dDevice, m_d3dImmediateContext);
#pragma endregion

#pragma region NPC
	std::vector<XMFLOAT3> NPCPositon;
	XMFLOAT3   npcPositon;
	//npcPositon = XMFLOAT3(m_TerrainX_3, m_Terrain.GetHeight(m_TerrainX_3, m_TerrainZ_3) + 2.0f, m_TerrainZ_3);                    //原
	npcPositon = XMFLOAT3(10.0f, m_Terrain.GetHeight(10.0f, 10.0f), 10.0f);//为方便演示放在原点附近
	NPCPositon.push_back(npcPositon);
	npcPositon = XMFLOAT3(-2 * m_TerrainX_3, m_Terrain.GetHeight(-2 * m_TerrainX_3, 2 * m_TerrainZ_3) + 2.0f, 2 * m_TerrainZ_3);                    //水
	NPCPositon.push_back(npcPositon);
	npcPositon = XMFLOAT3(-m_TerrainX_3, m_Terrain.GetHeight(-m_TerrainX_3, -m_TerrainZ_3) + 2.0f, -m_TerrainZ_3);                    //火龙果
	NPCPositon.push_back(npcPositon);
	npcPositon = XMFLOAT3(2 * m_TerrainX_3, m_Terrain.GetHeight(2 * m_TerrainX_3, -2 * m_TerrainZ_3) + 2.0f, -2 * m_TerrainZ_3);                    //马铃薯
	NPCPositon.push_back(npcPositon);
	m_NPC.init(m_d3dDevice, m_d3dImmediateContext);
	m_NPC.create(NPCPositon);
#pragma endregion 

#pragma region 摄影机
	m_Camera = &m_FCamera;
	m_BasicV = 20.0f;
	m_BasicWLR = 3.0f;
	m_BasicWUD = 2.0f;
	m_FCamera.SetPosition(0.0f, 5.0f, 0.0f);
	m_TCamera.SetPosition(0.0f, 5.0f, 0.0f);
#pragma endregion

#pragma region 玩家和玩家状态
	m_Player.init(m_d3dDevice, m_d3dImmediateContext);
	m_PlayerState = D3DGame::NORMAL;
#pragma endregion

#pragma region 隐藏鼠标
	ShowCursor(false);
#pragma endregion	

#pragma region 输入
	m_Dxinput.DirectInput_Init(mhMainWnd);
#pragma endregion

#pragma region 单击判断
	m_Hit = false;
	m_LongHit = false;
#pragma endregion
	
#pragma region 界面
	m_UI.init(m_d3dDevice, m_d3dImmediateContext);
	m_UI.create();
#pragma endregion
#pragma region 文字
	D2DText::Initialize(mhMainWnd, mSwapChain);
	m_NPCText.Reset(L"微软雅黑", 30.0f, D2D1::ColorF::Black,
		DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	m_MsgText.Reset(L"微软雅黑", 25.0f, D2D1::ColorF::Yellow,
		DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
#pragma endregion

	return true;
}

void Dyinger::OnResize()
{
#pragma region 更新DWrite缓存，窗口大小
	D2DText::Discard();
	D3DApp::OnResize();
	D2DText::OnReset();
#pragma endregion

#pragma region 投影矩阵
	m_FCamera.setProj(AspectRatio(), 1.0f, 3000.0f);
	m_TCamera.setProj(AspectRatio(), 1.0f, 3000.0f);
#pragma endregion
	 
#pragma region 更新文字区域
	m_TextRect		= {	313.0f	/ 1280.0f * mClientWidth, 580.0f / 720.0f * mClientHeight ,	1023.0f / 1280.0f * mClientWidth   ,mClientHeight * 700.0f/ 720.0f };
	m_HPRect		= { 1147.0f / 1280.0f *mClientWidth,	519.0f / 720.0f * mClientHeight ,  1.0f * mClientWidth,						  mClientHeight * 586.0f / 720.0f };
	m_SpeedRect	= { 1147.0f / 1280.0f *mClientWidth,  586.0f / 720.0f *mClientHeight ,	1.0f *  mClientWidth,						  mClientHeight * 653.0f/ 720.0f  };
	m_SceneRect	= { 1147.0f / 1280.0f* mClientWidth,  653.0f / 720.0f * mClientHeight ,	1.0f * mClientWidth,						  mClientHeight	* 1.0f					};
	m_DayRect		= { 1100.0f / 1280.0f* mClientWidth,  0.0f	   / 720.0f * mClientHeight ,	1.0f * mClientWidth,						  mClientHeight	* 128.0f/ 720.0f  };
#pragma endregion 

}

void Dyinger::UpdateScene(float dt)
{

#pragma region 一天处理	
#pragma region 一天开始时的处理    随机天气
	if (m_DayBin)
	{
		++m_DayNum;
		m_Goods.create(m_GoodsPositon);
		switch (rand() % 10)
		{
		case 1:case 2:case 3:
			m_Msg = m_Player.weatherEffect(D3DGame::Weather::SUNNY);
			m_Weather = D3DGame::Weather::SUNNY;
			break;
		case 4: case 5:
			m_Msg = m_Player.weatherEffect(D3DGame::Weather::CLOUDY);
			m_Weather = D3DGame::Weather::CLOUDY;
			break;
		case 6: case 7:
			m_Msg = m_Player.weatherEffect(D3DGame::Weather::SPRINKLE);
			m_Weather = D3DGame::Weather::SPRINKLE;
			break;
		case 8:
			m_Msg = m_Player.weatherEffect(D3DGame::Weather::WINDY);
			m_Weather = D3DGame::Weather::WINDY;
			break;
		case 9:
			m_Msg = m_Player.weatherEffect(D3DGame::Weather::HOT);
			m_Weather = D3DGame::Weather::HOT;
			break;
		case 0:
			m_Msg = m_Player.weatherEffect(D3DGame::Weather::RAINSTORM);
			m_Weather = D3DGame::Weather::RAINSTORM;
			break;
		}
		m_PlayerState = D3DGame::DAYBIN;
		m_Player.inDay(true);
		m_DayBin = false;
		m_InDay = true;
	}
#pragma endregion

#pragma region 进入夜晚
	if (mTimer.TotalTime() - m_Time > 60.0f  && m_InDay)
	{
		m_Player.inDay(false);
		m_Msg = m_Player.weatherEffect(D3DGame::Weather::NIGHT);
		m_InDay = false;
		m_PlayerState = D3DGame::TURNTONIGHT;
	}
#pragma endregion

#pragma region 结束一天
	if (mTimer.TotalTime() - m_Time > 120.0f)
	{
		m_Msg = m_Player.AfterDay();
		m_Time = mTimer.TotalTime();
		m_DayBin = true;
		m_PlayerState = D3DGame::DAYEND;
	}
	if (m_Player.getHP() <= 0)
	{
		m_PlayerState = D3DGame::DIED;
	}
#pragma endregion
#pragma endregion

#pragma region 输入响应
#pragma region 摄影机变化 
	//更新输入消息
	m_Dxinput.DirectInput_Update();
	m_Speed = m_Player.getSpeed();
	//镜头平移
	if (m_Dxinput.Key_Down(DIK_W))
	{
		m_FCamera.moveFB(m_BasicV * dt * m_Speed);
	}
	if (m_Dxinput.Key_Down(DIK_D))
	{
		m_FCamera.moveLR(m_BasicV * dt* m_Speed);
	}
	if (m_Dxinput.Key_Down(DIK_S))
	{
		m_FCamera.moveFB(-m_BasicV * dt* m_Speed);
	}
	if (m_Dxinput.Key_Down(DIK_A))
	{
		m_FCamera.moveLR(-m_BasicV * dt* m_Speed);
	}
	//第一人称旋转
	if (m_Dxinput.Mouse_X() < 0)
	{
		m_FCamera.rotateLR(-m_BasicWLR * dt);
		m_Player.setPlayerRotation(-m_BasicWLR * dt);
	}
	if (m_Dxinput.Mouse_X() > 0)
	{
		m_FCamera.rotateLR(m_BasicWLR * dt);
		m_Player.setPlayerRotation(m_BasicWLR * dt);
	}
	if (m_Dxinput.Mouse_Y() < 0)
	{
		m_FCamera.rotateUB(-m_BasicWUD * dt);
	}
	if (m_Dxinput.Mouse_Y() > 0)
	{
		m_FCamera.rotateUB(m_BasicWUD * dt);
	}
#pragma endregion

#pragma region 摄影机和人物位置
#pragma region 第一人称位置
	m_FCameraPos = m_FCamera.GetPosition();
#pragma region 过界处理
	if (m_FCameraPos.x > m_TerrainX - 5.0f)
		m_FCameraPos.x = m_TerrainX - 5.0f;
	if (m_FCameraPos.x < -m_TerrainX + 5.0f)
		m_FCameraPos.x = -m_TerrainX + 5.0f;
	if (m_FCameraPos.z > m_TerrainZ - 5.0f)
		m_FCameraPos.z = m_TerrainZ - 5.0f;
	if (m_FCameraPos.z < -m_TerrainZ + 5.0f)
		m_FCameraPos.z = -m_TerrainZ + 5.0f;
#pragma endregion 
	m_FCameraPos.y = m_Terrain.GetHeight(m_FCameraPos.x, m_FCameraPos.z) + 15.0f;
	m_FCamera.SetPosition(m_FCameraPos);
#pragma endregion 
#pragma region 第三人称位置
	m_FCamera.moveFB(-20.0f);
	m_TCamera = m_FCamera;
	m_FCamera.moveFB(20.0f);
	m_TCamera.SetTarget(m_FCamera.GetPosition());
#pragma endregion 
#pragma region  玩家位置
	m_FCameraPos = m_FCamera.GetPosition();
	m_FCameraPos.y = m_Terrain.GetHeight(m_FCameraPos.x, m_FCameraPos.z) + 2.0f;
	m_Player.setPlayerPos(m_FCameraPos);
	m_PlayerRect = {
									100.0f / 1280.0f * mClientWidth + m_FCameraPos.z * (100.0f / 1280.0f * mClientWidth) / m_TerrainZ - 5,
									620.0f / 720.0f  * mClientHeight + m_FCameraPos.x * (100.0f/ 720.0f  * mClientHeight)/ m_TerrainX - 5,
									100.0f / 1280.0f * mClientWidth + m_FCameraPos.z * (100.0f / 1280.0f * mClientWidth) / m_TerrainZ + 5,
									620.0f / 720.0f  * mClientHeight +m_FCameraPos.x *(100.0f / 720.0f  * mClientHeight) / m_TerrainX + 5};
#pragma endregion 
#pragma region  切换视角
	if (m_Dxinput.Key_Down(DIK_G))
	{
		m_Camera = &m_FCamera;
	}
	if (m_Dxinput.Key_Down(DIK_H))
	{
		m_Camera = &m_TCamera;
	}
#pragma endregion 
#pragma endregion 
	
#pragma region 退出游戏
	if (m_Dxinput.Key_Down(DIK_M))
		m_PlayerState = D3DGame::EXIT;
#pragma endregion 

#pragma region 拾取NPC或物品
	if (m_Dxinput.Key_Down(DIK_E))
	{
		if (m_LongHit == true)
		{
			m_Hit = false;
		}
		else
		{
			m_Hit = true;
			m_LongHit = true;
		}
		if (m_Hit)
		{
			m_NpcAction = m_NPC.pickNPC(m_Camera->getProjF4(), m_Camera->getViewF4(),mClientWidth / 2,mClientHeight / 2,mClientWidth,mClientHeight);
			if (m_NpcAction != D3DGame::NOPICK)
			{
				m_Msg = m_Player.pickNPC(m_NpcAction);
				m_PlayerState = D3DGame::PICKNPC;
			}
			else
			{
				m_GoodsType = m_Goods.pickmodel(m_Camera->getProjF4(), m_Camera->getViewF4(), mClientWidth / 2, mClientHeight / 2, mClientWidth, mClientHeight);
				if (m_GoodsType.first != D3DGame::NOGOOD)
				{
					m_Msg = m_Player.pickGoods(m_GoodsType);
					m_PlayerState = D3DGame::PICKGOOD;
				}
			}
		}
	}
	else
	{
		m_LongHit = false;
		m_Hit = false;
	}
#pragma endregion

#pragma region 全屏或窗口模式
	//全屏
	if (m_Dxinput.Key_Down(DIK_O))
	{
		mSwapChain->SetFullscreenState(true, NULL);
	}
	//窗口
	else if (m_Dxinput.Key_Down(DIK_P))
	{
		mSwapChain->SetFullscreenState(false, NULL);
	}
#pragma endregion

}

void Dyinger::DrawScene()
{
#pragma region 清屏
	assert(m_d3dImmediateContext);
	assert(mSwapChain);

	if (m_InDay)
	{
		m_d3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::White));
	}
	else
	{
		m_d3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Black));
	}
	m_d3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
#pragma endregion

#pragma region 观察投影矩阵 
	XMFLOAT4X4 ViewProj;
	XMStoreFloat4x4(&ViewProj, m_Camera->getViewProj());
#pragma endregion

#pragma region 地形
	m_Terrain.Draw(m_d3dImmediateContext, ViewProj, m_Camera->GetPosition(), m_DirLights);
	m_d3dImmediateContext->OMSetDepthStencilState(0, 0);
	m_d3dImmediateContext->RSSetState(0);
#pragma endregion

#pragma region  物品，NPC，主角
	m_d3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
	m_d3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Effects::BasicFX->SetDirLights(m_DirLights);
	Effects::BasicFX->SetEyePosW(m_Camera->GetPosition());

	ID3DX11EffectTechnique* activeTech = Effects::BasicFX->Light1TexAlphaClipFogTech;

	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);
	
	m_Goods.draw(activeTech, ViewProj);
	m_NPC.draw(activeTech, ViewProj);
	m_Player.draw(activeTech, ViewProj);

#pragma endregion

#pragma region 界面
	switch(m_PlayerState)
	{
	case D3DGame::NORMAL:
		m_UI.drawMsg(m_Weather);
		m_MsgText.DrawString(std::to_wstring(m_Player.getHP()).c_str() , m_HPRect);
		m_MsgText.DrawString(std::to_wstring(static_cast<int>(m_Player.getSpeed() * m_BasicV)).c_str(), m_SpeedRect);
		m_MsgText.DrawString(std::to_wstring(static_cast<int>(m_Player.getFogStart())).c_str(), m_SceneRect);
		m_MsgText.DrawString(std::wstring(L"第" + std::to_wstring(m_DayNum) + L"天").c_str(), m_DayRect);
		m_MsgText.DrawString(L".",m_PlayerRect);
		break;
	case D3DGame::PICKGOOD:
	case D3DGame::DAYBIN:
	case D3DGame::TURNTONIGHT:
	case D3DGame::DAYEND:
		m_UI.drawPlayer(m_Weather);
		m_NPCText.DrawString(m_Msg.c_str(), m_TextRect);
		break;
	case D3DGame::PICKNPC:
		m_UI.drawNPC(m_Weather);
		m_NPCText.DrawString(m_Msg.c_str(), m_TextRect);
		break;
	case D3DGame::DIED:
		m_UI.drawPlayer(m_Weather);
		m_NPCText.DrawString(L"饥饿与痛苦折磨着你，你曝尸荒野!", m_TextRect);
		break;
	case D3DGame::EXIT:
		m_UI.drawPlayer(m_Weather);
		m_NPCText.DrawString(L"生活就像海洋，只有意志坚强的人才能走到彼岸!", m_TextRect);
		break;
	}
#pragma endregion

	HR(mSwapChain->Present(0, 0));

	if (m_PlayerState != D3DGame::NORMAL)
	{
		Sleep(1000);
		if (m_PlayerState == D3DGame::EXIT || m_PlayerState == D3DGame::DIED)
		{
			mSwapChain->SetFullscreenState(false, NULL);
			PostQuitMessage(0);
		}
		m_PlayerState = D3DGame::NORMAL;
		m_Dxinput.DirectInput_Update();
	}
}
