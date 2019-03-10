#include "DXinput.h"

#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }

DXInput::DXInput()
{

}

DXInput::~DXInput()
{
	DirectInput_Shutdown();
}

bool DXInput::DirectInput_Init(HWND hwnd)
{
	//initialize DirectInput object
	DirectInput8Create(
		GetModuleHandle(NULL),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&dinput,
		NULL);

	//initialize the keyboard
	dinput->CreateDevice(GUID_SysKeyboard, &dikeyboard, NULL);
	dikeyboard->SetDataFormat(&c_dfDIKeyboard);
	dikeyboard->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	dikeyboard->Acquire();

	//initialize the mouse
	dinput->CreateDevice(GUID_SysMouse, &dimouse, NULL);
	dimouse->SetDataFormat(&c_dfDIMouse);
	dimouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	dimouse->Acquire();
	//d3ddev->ShowCursor(false);//--

	return true;
}

void DXInput::DirectInput_Update()
{
	//update mouse
	dimouse->Poll();
	if (!SUCCEEDED(dimouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouse_state)))
	{
		//mouse device lose, try to re-acquire
		dimouse->Acquire();
	}

	//update keyboard
	dikeyboard->Poll();
	if (!SUCCEEDED(dikeyboard->GetDeviceState(256, (LPVOID)&keys)))
	{
		//keyboard device lost, try to re-acquire
		dikeyboard->Acquire();
	}

	//update controllers
	for (int i = 0; i< 4; i++)
	{
		ZeroMemory(&controllers[i], sizeof(XINPUT_STATE));

		//get the state of the controller
		XINPUT_STATE state;
		DWORD result = XInputGetState(i, &state);

		//store state in global controllers array
		if (result == 0) controllers[i] = state.Gamepad;
	}
}


int DXInput::Mouse_X()
{
	return mouse_state.lX;
}

int DXInput::Mouse_Y()
{
	return mouse_state.lY;
}

int DXInput::Mouse_Button(int button)
{
	return mouse_state.rgbButtons[button] & 0x80;
}

bool DXInput::Key_Down(int key)
{
	return (bool)(keys[key] & 0x80);
}

void DXInput::DirectInput_Shutdown()
{
	if (dikeyboard)
	{
		dikeyboard->Unacquire();
		dikeyboard->Release();
		dikeyboard = NULL;
	}
	if (dimouse)
	{
		dimouse->Unacquire();
		dimouse->Release();
		dimouse = NULL;
	}
}

