#pragma once
#include <dinput.h>
#include <xinput.h>
#include <d3d9.h>
#include <d3dx11.h>
#include <D3D11.h>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"xinput.lib")

class DXInput
{
public :
	DXInput();
	~DXInput();

	bool DirectInput_Init(HWND);
	void DirectInput_Update();
	void DirectInput_Shutdown();
	bool Key_Down(int);
	int Mouse_Button(int);
	int Mouse_X();
	int Mouse_Y();
	
	LPDIRECTINPUT8 dinput;
	LPDIRECTINPUTDEVICE8 dimouse;
	LPDIRECTINPUTDEVICE8 dikeyboard;
	DIMOUSESTATE mouse_state;
	XINPUT_GAMEPAD controllers[4];
	char keys[256];
	//LPDIRECT3DDEVICE9 d3ddev;
	ID3D11Device*   md3dDevice;
};




