#pragma once

#pragma warning (disable : 4251)

#ifdef _DEBUG
#include <iostream>
#endif // DEBUG

#include <d3d11.h>

#include <d3dcompiler.h>
#include <Effects11/d3dx11effect.h>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/ScreenGrab.h>
#include <DirectXTK/Effects.h>
#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/VertexTypes.h>

#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>

#include <DirectXCollision.h>

#include <DirectXMath.h>
using namespace DirectX;

#include <Assimp/scene.h>
#include <Assimp/Importer.hpp>
#include <Assimp/postprocess.h>

#include <typeinfo>

#include <process.h>

#include "Engine_Typedef.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <algorithm>


#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG


#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_TypeDef.h"
#include "Engine_Struct.h"

using namespace Engine;
using namespace std;

enum CHANNELID { SOUND_BGM, SOUND_PLAYER, SOUND_WALK, SOUND_NPC, SOUND_BOSS, SOUND_MONSTER, SOUND_EFFECT, SOUND_ITEM, SOUND_UI, MAXCHANNEL }; 
