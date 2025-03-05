#pragma once

#include <process.h>
#include "Engine_Defines.h"

namespace Client 
{	

	const unsigned int g_iWinCX = 1280;
	const unsigned int g_iWinCY = 720;

	/* LEVEL_STATIC : �Ҵ�Ǳ����� ������ �ƴϴ�. ���� �ΰ��ӳ����� ���Ǵ� ������Ʈ���� �Ŵ����鿡�� ���. */ 
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_OUTSET, LEVEL_PLAY_STATIC, LEVEL_DRAGON, LEVEL_DUNGEON, LEVEL_DUNGEON_FINAL, LEVEL_BOSSROOM, LEVEL_TOOL, LEVEL_END };
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
using namespace Client;
