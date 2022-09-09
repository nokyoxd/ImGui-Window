#ifndef __UI_H__
#define __UI_H__

#include "../include.hpp"

namespace UI
{
	inline bool bRun = true;

	/* Window size */
	constexpr int iWidth{ 400 };
	constexpr int iHeight{ 400 };

	/*  */
	inline POINTS Pos = { };

	/* */
	inline LPDIRECT3D9 g_pD3D{ nullptr };
	inline LPDIRECT3DDEVICE9 g_pd3dDevice{ nullptr };
	inline D3DPRESENT_PARAMETERS g_d3dpp{ };

	/* WINAPi window vars */
	inline HWND HWnd{ nullptr };
	inline WNDCLASSEX WndClass{ };

	void Setup();

	void CreateHWindow();
	void DestroyHWindow();

	bool CreateDevice();
	void ResetDevice();
	void DestroyDevice();

	void CreateImGui();
	void DestroyImGui();

	void CreateRender();
	void Render();
	void DestroyRender();
}

#endif // #ifndef __UI_H__
