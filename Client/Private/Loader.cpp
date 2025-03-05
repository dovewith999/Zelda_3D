#include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"

#pragma region GameObject
#include "BackGround.h"
#include "Sky.h"
#include "Camera_Player.h"
#include "Camera_Tool.h"
#include "Terrain.h"
#include "Player.h"
#include "Monster.h"
#include "BossMonster.h"
#include "Zelda.h"
#include "Point_Sphere.h"


/*Level_Dungeon*/
#include "Object_StatueCube.h"
#include "Object_Door.h"
#include "Object_Lock.h"
#include "Magma.h"
#include "MagmaZet.h"
#include "Object_FireBrazier.h"
#include "Object_TresureBox.h"
#include "Object_TresureBoxShort.h"
#include "Object_Cube.h"
#include "Object_Bomb.h"
#include "Object_BFlower.h"
#include "Object_Bridge.h"
#include "Object_MagRock.h"
#include "Object_Rock.h"
#include "Object_WaterPot.h"
#include "Object_WoodPlanks.h"
#include "Object_WoodPlanks_Broken.h"
#include "Object_Fire.h"
#include "Trigger_Ladder.h"
#include "Trigger_Boss.h"

/*Level_Dungeon_Final*/
#include "Object_WoodDoor.h"
#include "Object_StoneBridge.h"
#pragma endregion

#pragma region Item
#include "Item_Sheath.h"
#include "Item_SwordHandle.h"
#include "Item_Blade.h"
#include "Item_Shield.h"
#include "Collider_GndSword.h"
#include "Item_Boko.h"
#include "Item_Bow.h"
#pragma endregion 

#pragma region Stage
#include "Stage_OutSet.h"
#include "Stage_GTower.h"
#include "Stage_Dragon.h"
#include "Dungeon_Room0.h"
#include "Dungeon_Room1.h"
#include "Dungeon_Room2.h"
#include "Dungeon_Room3.h"
#include "Dungeon_Room4.h"
#include "Dungeon_Room5_OutSide.h"
#include "Dungeon_Room6.h"
#include "Dungeon_Room7.h"
#include "Dungeon_Room8.h"
#include "Dungeon_Room9.h"
#include "Dungeon_Room10.h"
#include "Dungeon_RoomFinal.h"
#pragma endregion

#pragma region UI
/* Level_Logo */
#include "UI_MainLogo.h"
#include "UI_TitleLogo.h"
#include "UI_SubLogo.h"
#include "UI_PressStart.h"
#include "UI_LogoBackGround.h"

/* Level_GamePlay */
#include "UI_PlayerHp.h"
#include "UI_AttentionBar.h"
#include "UI_BlackOut.h"

#include "UI_End.h"
#pragma endregion

#pragma region PARTICLE
#include "Particle_Bomb.h"
#pragma endregion


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

_uint APIENTRY EntryFunction(void* pArg)
{
	CLoader*			pLoader = (CLoader*)pArg;
	
	EnterCriticalSection(&pLoader->Get_CriticalSection());

	_uint		iProgress = 0;

	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_LOGO:
		iProgress = pLoader->Loading_ForLogo();
		//iProgress = pLoader->Loading_ForPlayStatic();
		break;
	case LEVEL_GAMEPLAY:
		iProgress = pLoader->Loading_ForGamePlay();
		break;
	case LEVEL_TOOL:
		iProgress = pLoader->Loading_ForPlayStatic();
		iProgress = pLoader->Loading_ForTool();
		break;

	case LEVEL_DUNGEON:
		iProgress = pLoader->Loading_ForDungeon();
		iProgress = pLoader->Loading_ForPlayStatic();
		break;

	case LEVEL_DUNGEON_FINAL:
		iProgress = pLoader->Loading_ForPlayStatic();
		iProgress = pLoader->Loading_ForDungeonFinal();
		break;
	}

	LeaveCriticalSection(&pLoader->Get_CriticalSection());

	return iProgress;
}

HRESULT CLoader::NativeConstruct(LEVEL eNextLevelID)
{
	CoInitializeEx(nullptr, 0);

	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, EntryFunction, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

_uint CLoader::Loading_ForLogo()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

#pragma region GAMEOBJECT
	lstrcpy(m_szLoadingText, TEXT("객체 원형을 생성중입니다."));	

	/* For.Prototype_GameObject_UI_MainLogo */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_MainLogo"),
		CUI_MainLogo::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(90.0f))))))
		return -1;

	/* For.Prototype_GameObject_UI_TitleLogo */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_TitleLogo"),
		CUI_TitleLogo::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(90.0f))))))
		return -1;

	/* For.Prototype_GameObject_UI_SubLogo */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_SubLogo"),
		CUI_SubLogo::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(90.0f))))))
		return -1;

	/* For.Prototype_GameObject_UI_PressStart */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_PressStart"),
		CUI_PressStart::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(90.0f))))))
		return -1;


	/* For.Prototype_GameObject_UI_PressStart */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_LogoBackGround"),
		CUI_LogoBackGround::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(90.0f))))))
		return -1;


#pragma endregion


#pragma region COMPONENT
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_UI_LogoBackGround"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Logo/UI_LogoBackGround.dds")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_UI_TitleLogo"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Logo/UI_TitleLogo.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_UI_MainLogo"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Logo/UI_MainLogo.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_UI_SubLogo"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Logo/UI_SubLogo.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_UI_PressStart"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Logo/UI_PressStart.png")))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("모델을 생성중입니다."));

#pragma endregion


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return _uint();
}

_uint CLoader::Loading_ForGamePlay()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

#pragma region GAMEOBJECT
	lstrcpy(m_szLoadingText, TEXT("객체 원형을 생성중입니다."));

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

	/*For.Prototype_GameObject_Player*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(2.f, XMConvertToRadians(80.f))))))
		return -1;

	/*For.Prototype_GameObject_Zelda*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Zelda"),
		CZelda::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
		return -1;

	/*For.Prototype_GameObject_Monster*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
		CMonster::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
		return -1;

	/*For.Prototype_GameObject_BossMonster*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossMonster"),
		CBossMonster::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(2.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Stage_OutSet_Island */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stage_OutSet_Island"),
		CStage_OutSet::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

	/* For.Prototype_GameObject_Stage_Ganon's_Tower */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stage_GTower"),
		CStage_GTower::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

	/* For.Prototype_GameObject_Stage_Dragon_Island */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stage_Dragon_Island"),
		CStage_Dragon::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

	/*For.Prototype_GameObject_Dungeon_Room0*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room0"),
		CDungeon_Room0::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
		return -1;


	/* For.Prototype_GameObject_Item_Sheath */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Sheath"),
		CItem_Sheath::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

	/* For.Prototype_GameObject_Item_SwordHandle */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_SwordHandle"),
		CItem_SwordHandle ::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

	/* For.Prototype_GameObject_Item_Blade */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Blade"),
		CItem_Blade::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

	/* For.Prototype_GameObject_Item_Shield */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Shield"),
		CItem_Shield::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

	/* For.Prototype_GameObject_Collider_GndSword */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Collider_GndSword"),
		CCollider_GndSword::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

	/* For.Prototype_GameObject_MagmaZet */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MagmaZet"),
		CMagmaZet::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;


	/* For.Prototype_GameObject_Camera_Player */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Player"),
		CCamera_Player::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;


	//**********************************Ui_Object*****************************************

	/* For.Prototype_GameObject_UI_PlayerHp */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_PlayerHp"),
		CUI_PlayerHp::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_UI_AttentionBar*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_AttentionBar"),
		CUI_AttentionBar::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

#pragma endregion

#pragma region COMPONENT
	lstrcpy(m_szLoadingText, TEXT("지형을 생성 중입니다. "));

	///* For.Prototype_Component_VIBuffer_Terrain. */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"), 
	//	CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
	//	return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 생성 중입니다. "));

	///* For.Prototype_Component_Texture_Default. */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Default"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
	//	return E_FAIL;


	///* For.Prototype_Component_Texture_Terrain . */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"), 
	//	CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2))))
	//	return E_FAIL;

	/* For.Prototype_Component_Texture_Filter. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Filter"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/Filter.bmp"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Brush. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_SkyBox. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SkyBox"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;

	//**********************************Ui_Object*****************************************

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_PlayerHp"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Player/UI_PlayerHp_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_AttentionBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Player/UI_Attention_Bar.png")))))
		return E_FAIL;
	
	lstrcpy(m_szLoadingText, TEXT("모델을 생성 중입니다. "));

	_matrix		PivotMatrix;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Link. */

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Link"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Link/", "Link.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Zelda. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Zelda"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Zelda/", "Zelda.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Boko. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Boko"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Boko/", "Boko.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Gnd. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Gnd"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Gnd/", "Gnd_1.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Stage_OutSet_Island. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Stage_OutSet_Island"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/OutSet_Island/", "OutSetIsland.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Stage_GTower. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Stage_GTower"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Ganon's_Tower/", "TowerSmall.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f)/** XMMatrixRotationY(XMConvertToRadians(180.0f))*/;

	/* For.Prototype_Component_Model_Stage_Dragon_Island. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Stage_Dragon_Island"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dragon_Island/", "Dragon.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.0f));

	/* For.Prototype_Component_Model_Dungeon_Room0. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Dungeon_Room0"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_0/", "Room_0.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	/* For.Prototype_Component_Model_Item_Sheath. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Item_Sheath"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Item/Sword/Sheath/", "Sheath.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Item_SwordHandle. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Item_SwordHandle"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Item/Sword/Handle/", "SwordHandle.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Item_Blade. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Item_Blade"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Item/Sword/Blade/", "Blade.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Item_Shield. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Item_Shield"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Item/Shield/", "Shield.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("콜라이더를 생성 중입니다. "));

	/* For.Prototype_Component_Collier_AABB. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collier_OBB. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collier_SPHERE. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("네비게이션을 생성 중입니다."));
	/*if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Data/Navigation.dat")))))
		return E_FAIL;*/
	
	lstrcpy(m_szLoadingText, TEXT("셰이더를 생성 중입니다. "));

	/* For.Prototype_Component_Shader_VtxNonAnim */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNonAnim"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxNonAnim.hlsl"), VTXNONANIM_DECLARATION::Elements, VTXNONANIM_DECLARATION::iNumElement))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnim */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnim"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnim.hlsl"), VTXANIM_DECLARATION::Elements, VTXANIM_DECLARATION::iNumElement))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXTEXCUBE_DECLARATION::Elements, VTXTEXCUBE_DECLARATION::iNumElement))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

#pragma endregion

	Safe_Release(pGameInstance);

	return _uint();
}

_uint CLoader::Loading_ForTool()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

#pragma region GAMEOBJECT
	lstrcpy(m_szLoadingText, TEXT("객체 원형을 생성중입니다."));

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

	/*For.Prototype_GameObject_Zelda*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Zelda"),
		CZelda::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Item_Blade */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Blade"),
		CItem_Blade::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

	/* For.Prototype_GameObject_Stage_OutSet_Island */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stage_OutSet_Island"),
		CStage_OutSet::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

	/* For.Prototype_GameObject_Stage_Ganon's_Tower */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stage_GTower"),
		CStage_GTower::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

	/* For.Prototype_GameObject_Stage_Dragon_Island */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stage_Dragon_Island"),
		CStage_Dragon::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

	/*For.Prototype_GameObject_Dungeon_Room0*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room0"),
		CDungeon_Room0::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
		return -1;

	/*For.Prototype_GameObject_Dungeon_Room1*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room1"),
		CDungeon_Room1::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
		return -1;

	/*For.Prototype_GameObject_Dungeon_Room2*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room2"),
		CDungeon_Room2::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
		return -1;

	/*For.Prototype_GameObject_Dungeon_Room3*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room3"),
		CDungeon_Room3::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
		return -1;

	/*For.Prototype_GameObject_Dungeon_Room4*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room4"),
		CDungeon_Room4::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
		return -1;

	/*For.Prototype_GameObject_Dungeon_Room5_OutSide*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room5_OutSide"),
		CDungeon_Room5_OutSide::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
		return -1;

	/*For.Prototype_GameObject_Dungeon_Room6*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room6"),
		CDungeon_Room6::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
		return -1;

	/*For.Prototype_GameObject_Dungeon_Room7*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room7"),
		CDungeon_Room7::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
		return -1;

	/*For.Prototype_GameObject_Dungeon_Room8*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room8"),
		CDungeon_Room8::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
		return -1;

	/*For.Prototype_GameObject_Dungeon_Room9*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room9"),
		CDungeon_Room9::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
		return -1;


	/*For.Prototype_GameObject_Dungeon_Room10*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room10"),
		CDungeon_Room10::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
		return -1;

	/*For.Prototype_GameObject_Dungeon_RoomFinal*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_RoomFinal"),
		CDungeon_RoomFinal::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Camera_Tool */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Tool"),
		CCamera_Tool::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(20.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Point_Sphere */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Point_Sphere"),
		CPoint_Sphere::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(30.f, XMConvertToRadians(60.f))))))
		return -1;



	/*For.Prototype_GameObject_Monster*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
		CMonster::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Magma */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Magma"),
		CMagma::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_FireBrazier */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_FireBrazier"),
		CObject_FireBrazier::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_Door */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_Door"),
		CObject_Door::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_Lock */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_Lock"),
		CObject_Lock::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_TresureBox*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_TresureBox"),
		CObject_TresureBox::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_TresureBoxShort*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_TresureBoxShort"),
		CObject_TresureBoxShort::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_Cube */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_Cube"),
		CObject_Cube::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(2.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_Bomb */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_Bomb"),
		CObject_Bomb::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_BFlower */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_BFlower"),
		CObject_BFlower::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_Bridge */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_Bridge"),
		CObject_Bridge::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_MagRock */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_MagRock"),
		CObject_MagRock::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_Rock */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_Rock"),
		CObject_Rock::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_WaterPot */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_WaterPot"),
		CObject_WaterPot::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_WoodPlanks */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_WoodPlanks"),
		CObject_WoodPlanks::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;


	/* For.Prototype_GameObject_Trigger_Ladder */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Trigger_Ladder"),
		CTrigger_Ladder ::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Trigger_Boss */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Trigger_Boss"),
		CTrigger_Boss::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_WoodDoor */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_WoodDoor"),
		CObject_WoodDoor::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_StoneBridge */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_StoneBridge"),
		CObject_StoneBridge::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_MagmaZet */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MagmaZet"),
		CMagmaZet::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

#pragma endregion


#pragma region COMPONENT
	lstrcpy(m_szLoadingText, TEXT("지형을 생성 중입니다. "));

	/* For.Prototype_Component_VIBuffer_Terrain. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	/* For.Prototype_Component_Texture_Terrain . */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Filter. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Filter"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/Filter.bmp"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Brush. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;


	/* For.Prototype_Component_Texture_Magma. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Magma"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Object/Magma.dds")))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("모델을 생성중입니다."));

	_matrix		PivotMatrix;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Stage_OutSet_Island. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Stage_OutSet_Island"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/OutSet_Island/", "OutSetIsland.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Stage_GTower. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Stage_GTower"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Ganon's_Tower/", "TowerSmall.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f)/** XMMatrixRotationY(XMConvertToRadians(180.0f))*/;

	/* For.Prototype_Component_Model_Stage_GTower. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Stage_Dragon_Island"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dragon_Island/", "Dragon.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.0f));

	/* For.Prototype_Component_Model_Dungeon_Room0. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Dungeon_Room0"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_0/", "Room_0.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(90.0f));

	/* For.Prototype_Component_Model_Dungeon_Room1. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Dungeon_Room1"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_1/", "Room_1.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	/* For.Prototype_Component_Model_Dungeon_Room2. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Dungeon_Room2"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_2/", "Room_2.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;


	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	/* For.Prototype_Component_Model_Dungeon_Room3. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Dungeon_Room3"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_3/", "Room_3.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;


	PivotMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	/* For.Prototype_Component_Model_Dungeon_Room4. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Dungeon_Room4"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_4/", "Room_4.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;


	PivotMatrix = /*XMMatrixScaling(0.1f, 0.1f, 0.1f) * */ XMMatrixRotationY(XMConvertToRadians(90.0f));
	/* For.Prototype_Component_Model_Dungeon_Room5_OutSide. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Dungeon_Room5_OutSide"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_5_OutSide/", "Room_5_OutSide.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;


	PivotMatrix = /*XMMatrixScaling(0.1f, 0.1f, 0.1f)* */  XMMatrixRotationY(XMConvertToRadians(90.0f));
	/* For.Prototype_Component_Model_Dungeon_Room6. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Dungeon_Room6"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_6/", "Room_6.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.0105f, 0.0105f, 0.0105f)*   XMMatrixRotationY(XMConvertToRadians(90.0f));
	/* For.Prototype_Component_Model_Dungeon_Room7. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Dungeon_Room7"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_7/", "Room_7.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;


	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	/* For.Prototype_Component_Model_Dungeon_Room8. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Dungeon_Room8"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_8/", "Room_8.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;


	PivotMatrix = /*XMMatrixScaling(0.01f, 0.01f, 0.01f) **/ XMMatrixRotationY(XMConvertToRadians(90.0f));
	/* For.Prototype_Component_Model_Dungeon_Room9. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Dungeon_Room9"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_9/", "Room_9.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = /*XMMatrixScaling(0.01f, 0.01f, 0.01f) **/ XMMatrixRotationY(XMConvertToRadians(90.0f));
	/* For.Prototype_Component_Model_Dungeon_Room10. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Dungeon_Room10"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_10/", "Room_10.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	/* For.Prototype_Component_Model_Dungeon_RoomFinal. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Dungeon_RoomFinal"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_Final/", "Room_Final.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;



	/* For.Prototype_Component_Model_Item_Blade. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Item_Blade"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Item/Sword/Blade/", "Blade.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_FireBrazier */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Object_FireBrazier"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/Fire_Brazier/", "Fire_Brazier.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.003f, 0.003f, 0.003f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_Door */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Object_Door"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/Door/", "Door.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) /** XMMatrixRotationY(XMConvertToRadians(180.0f))*/;

	/* For.Prototype_Component_Model_Object_Lock */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Object_Lock"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/Lock/", "Lock.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;


	PivotMatrix = XMMatrixScaling(0.013f, 0.013f, 0.013f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	/* For.Prototype_Component_Model_Object_TresureBox */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Object_TresureBox"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/Box/Box_A/", "Box_A.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_Cube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Object_Cube"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/Cube/", "Cube.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_Bomb */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Object_Bomb"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/Bomb/", "Bomb.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_BFlower */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Object_BFlower"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/Bomb/Flower/", "BFlower.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_Bridge */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Object_Bridge"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/Bridge/", "Bridge.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_MagRock */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Object_MagRock"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/MagRock/", "MagRock.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_Rock */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Object_Rock"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/Rock/", "Rock.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_WaterPot */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Object_WaterPot"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/WaterPot/", "WaterPot.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_WoodPlanks */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Object_WoodPlanks"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/WoodPlanks/", "WoodPlanks.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_WoodPlanks_Break */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Object_WoodPlanks_Break"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/WoodPlanks/Break/", "WoodPlanksBreak.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_WoodDoor */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Object_WoodDoor"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Dungeon_final/WoodDoor/", "WoodDoor.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_StoneBridge */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Object_StoneBridge"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Dungeon_final/StoneBridge/", "StoneBridge.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;



	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_MagmaZet */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_MagmaZet"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Etc/MagmaZet/", "MagmaZet.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Boko. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Boko"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Boko/", "Boko.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Zelda. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_Zelda"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Zelda/", "Zelda.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("네비게이션을 생성 중입니다."));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Data/Navigation.dat")))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Navigation_Dungeon_Final"),
		CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Data/Navigation_Dungeon_Final.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("콜라이더를 생성 중입니다. "));

	/* For.Prototype_Component_Collier_AABB. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collier_OBB. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collier_SPHERE. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("셰이더를 생성 중입니다. "));

	/* For.Prototype_Component_Shader_VtxNonAnim */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNonAnim"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxNonAnim.hlsl"), VTXNONANIM_DECLARATION::Elements, VTXNONANIM_DECLARATION::iNumElement))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnim */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxAnim"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnim.hlsl"), VTXANIM_DECLARATION::Elements, VTXANIM_DECLARATION::iNumElement))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXTEXCUBE_DECLARATION::Elements, VTXTEXCUBE_DECLARATION::iNumElement))))
		return E_FAIL;

#pragma endregion


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return _uint();
}

_uint CLoader::Loading_ForPlayStatic()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);


	lstrcpy(m_szLoadingText, TEXT("Play_Static 개임 오브젝트 로당을 시작합니다."));
#pragma region GAMEOBJECT

	_matrix PivotMatrix;


	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Link. */

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Model_Link"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Link/", "Link.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	/* For.Prototype_Component_Model_Item_Sheath. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Model_Item_Sheath"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Item/Sword/Sheath/", "Sheath.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Item_SwordHandle. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Model_Item_SwordHandle"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Item/Sword/Handle/", "SwordHandle.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Item_Blade. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Model_Item_Blade"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Item/Sword/Blade/", "Blade.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Item_Shield. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Model_Item_Shield"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Item/Shield/", "Shield.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;


	/*----------------UI_OBJECT---------------*/

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerHp"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Player/UI_PlayerHp_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Texture_UI_AttentionBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Player/UI_Attention_Bar.png")))))
		return E_FAIL;

#pragma endregion


#pragma region COMPONENT

	lstrcpy(m_szLoadingText, TEXT("static 콜라이더를 생성 중입니다. "));

	/* For.Prototype_Component_Collier_AABB. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collier_OBB. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collier_SPHERE. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;
	lstrcpy(m_szLoadingText, TEXT("static 버퍼를 생성 중입니다. "));

	/* For.Prototype_Component_VIBuffer_Cube. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("static 셰이더를 생성 중입니다. "));

	/* For.Prototype_Component_Shader_VtxNonAnim */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Shader_VtxNonAnim"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxNonAnim.hlsl"), VTXNONANIM_DECLARATION::Elements, VTXNONANIM_DECLARATION::iNumElement))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnim */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnim.hlsl"), VTXANIM_DECLARATION::Elements, VTXANIM_DECLARATION::iNumElement))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXTEXCUBE_DECLARATION::Elements, VTXTEXCUBE_DECLARATION::iNumElement))))
		return E_FAIL;
#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	Safe_Release(pGameInstance);
	return _uint();
}

_uint CLoader::Loading_ForDungeon()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

#pragma region GAMEOBJECT
	lstrcpy(m_szLoadingText, TEXT("객체 원형을 생성중입니다."));

	/*For.Prototype_GameObject_Player*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(2.f, XMConvertToRadians(80.f))))))
		return -1;

	/*For.Prototype_GameObject_Monster*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
		CMonster::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(4.f, XMConvertToRadians(60.f))))))
		return -1;


	/*For.Prototype_GameObject_Dungeon_Room0*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room0"),
		CDungeon_Room0::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
		return -1;


	/*For.Prototype_GameObject_Dungeon_Room1*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_Room1"),
		CDungeon_Room1::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
		return -1;


	/* For.Prototype_GameObject_Object_FireBrazier */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_FireBrazier"),
		CObject_FireBrazier::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_Door */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_Door"),
		CObject_Door::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_Lock */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_Lock"),
		CObject_Lock::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_TresureBox*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_TresureBox"),
		CObject_TresureBox::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_TresureBoxShort*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_TresureBoxShort"),
		CObject_TresureBoxShort::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_Cube */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_Cube"),
		CObject_Cube::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(4.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_Bomb */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_Bomb"),
		CObject_Bomb::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_BFlower */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_BFlower"),
		CObject_BFlower::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_Bridge */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_Bridge"),
		CObject_Bridge::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_MagRock */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_MagRock"),
		CObject_MagRock::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_Rock */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_Rock"),
		CObject_Rock::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_WaterPot */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_WaterPot"),
		CObject_WaterPot::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_WoodPlanks */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_WoodPlanks"),
		CObject_WoodPlanks::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;


	/* For.Prototype_GameObject_Object_WoodPlanks_Broken */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_WoodPlanks_Broken"),
		CObject_WoodPlanks_Broken::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Item_Sheath */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Sheath"),
		CItem_Sheath::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

	/* For.Prototype_GameObject_Item_SwordHandle */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_SwordHandle"),
		CItem_SwordHandle::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

	/* For.Prototype_GameObject_Item_Blade */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Blade"),
		CItem_Blade::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

	/* For.Prototype_GameObject_Item_Shield */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Shield"),
		CItem_Shield::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;


	/* For.Prototype_GameObject_Item_Boko */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Boko"),
		CItem_Boko::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

	/* For.Prototype_GameObject_Object_StatueCube */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_StatueCube"),
		CObject_StatueCube::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(2.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Magma */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Magma"),
		CMagma::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Magma */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_Fire"),
		CObject_Fire::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_MagmaZet */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MagmaZet"),
		CMagmaZet::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Trigger_Ladder */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Trigger_Ladder"),
		CTrigger_Ladder::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Camera_Player */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Player"),
		CCamera_Player::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;



	//**********************************Ui_Object*****************************************

	/* For.Prototype_GameObject_UI_PlayerHp */ 
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_PlayerHp"),
		CUI_PlayerHp::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_UI_AttentionBar*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_AttentionBar"),
		CUI_AttentionBar::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_UI_AttentionBar*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_BlackOut"),
		CUI_BlackOut::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Particle_Bomb */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Bomb"),
		CParticle_Bomb::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

#pragma endregion

#pragma region COMPONENT
	lstrcpy(m_szLoadingText, TEXT("지형을 생성 중입니다. "));


	/* For.Prototype_Component_VIBuffer_Cube. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_PointInstance. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_VIBuffer_PointInstance"),
		CVIBuffer_PointInstance::Create(m_pDevice, m_pDeviceContext, 8))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 생성 중입니다. "));

	/* For.Prototype_Component_Texture_SkyBox. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Texture_SkyBox"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_Zelda.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Magma. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Texture_Magma"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Object/Magma.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Object_Fire. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Texture_Object_Fire"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Object/Fire/Fire_%d.dds"), 6))))
		return E_FAIL;

	//**********************************Ui_Object*****************************************

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Texture_UI_PlayerHp"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Player/UI_PlayerHp_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Texture_UI_AttentionBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Player/UI_Attention_Bar.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Texture_Particle_Bomb"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Particle/Particle_Base_%d.png"), 3))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을 생성 중입니다. "));

	_matrix		PivotMatrix;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Link. */

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Link"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Link/", "Link.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;


	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Boko. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Boko"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Boko/", "Boko.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.0f));

	/* For.Prototype_Component_Model_Dungeon_Room0. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Dungeon_Room0"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_0/", "Room_0.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(90.0f));

	/* For.Prototype_Component_Model_Stage_GTower. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Dungeon_Room1"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_1/", "Room_1.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f)/* * XMMatrixRotationY(XMConvertToRadians(270.0f))*/;

	/* For.Prototype_Component_Model_Object_StatueCube. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Object_StatueCube"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Room0/", "Cube.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;



	PivotMatrix = XMMatrixScaling(0.006f, 0.006f, 0.006f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_FireBrazier */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Object_FireBrazier"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/Fire_Brazier/", "Fire_Brazier.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.003f, 0.003f, 0.003f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_Door */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Object_Door"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/Door/", "Door.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	/* For.Prototype_Component_Model_Object_Lock */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Object_Lock"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/Lock/", "Lock.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_Cube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Object_Cube"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/Cube/", "Cube.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_Bomb */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Object_Bomb"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/Bomb/", "Bomb.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_BFlower */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Object_BFlower"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/Bomb/Flower/", "BFlower.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_Bridge */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Object_Bridge"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/Bridge/", "Bridge.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_MagRock */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Object_MagRock"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/MagRock/", "MagRock.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_Rock */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Object_Rock"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/Rock/", "Rock.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_WaterPot */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Object_WaterPot"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/WaterPot/", "WaterPot.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_WoodPlanks */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Object_WoodPlanks"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/WoodPlanks/", "WoodPlanks.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_WoodPlanks_Break */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Object_WoodPlanks_Break"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/WoodPlanks/Break/", "WoodPlanksBreak.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;


	PivotMatrix = XMMatrixScaling(0.013f, 0.013f, 0.013f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	/* For.Prototype_Component_Model_Object_TresureBox */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Object_TresureBox"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/Box/Box_A/", "Box_A.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;


	PivotMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	/* For.Prototype_Component_Model_Item_Sheath. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Item_Sheath"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Item/Sword/Sheath/", "Sheath.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Item_SwordHandle. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Item_SwordHandle"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Item/Sword/Handle/", "SwordHandle.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Item_Blade. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Item_Blade"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Item/Sword/Blade/", "Blade.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Item_Shield. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Item_Shield"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Item/Shield/", "Shield.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	//PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Item_Boko. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_Item_Boko"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Item/Boko/", "Boko_Weapon.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_MagmaZet */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Model_MagmaZet"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Etc/MagmaZet/", "MagmaZet.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("콜라이더를 생성 중입니다. "));

	/* For.Prototype_Component_Collier_AABB. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collier_OBB. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collier_SPHERE. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("네비게이션을 생성 중입니다."));


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Data/Navigation.dat")))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("셰이더를 생성 중입니다. "));

	/* For.Prototype_Component_Shader_VtxNonAnim */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Shader_VtxNonAnim"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxNonAnim.hlsl"), VTXNONANIM_DECLARATION::Elements, VTXNONANIM_DECLARATION::iNumElement))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnim */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Shader_VtxAnim"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnim.hlsl"), VTXANIM_DECLARATION::Elements, VTXANIM_DECLARATION::iNumElement))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXTEXCUBE_DECLARATION::Elements, VTXTEXCUBE_DECLARATION::iNumElement))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointInstance*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTINSTANCE_DECLARATION::Elements, VTXPOINTINSTANCE_DECLARATION::iNumElement))))
		return E_FAIL;

#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("Dungeon 로딩이 끝났습니다."));
	Safe_Release(pGameInstance);

	return _uint();
}

_uint CLoader::Loading_ForDungeonFinal()
{ 
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("객체원형을 생성 중입니다."));

	///*For.Prototype_GameObject_Player*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
	//	CPlayer::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(2.f, XMConvertToRadians(80.f))))))
	//	return -1;

	/*For.Prototype_GameObject_BossMonster*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossMonster"),
		CBossMonster::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(2.f, XMConvertToRadians(60.f))))))
		return -1;

	/*For.Prototype_GameObject_Zelda*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Zelda"),
		CZelda::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
		return -1;


	/* For.Prototype_GameObject_Item_Bow */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Bow"),
		CItem_Bow::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
		return -1;

	/* For.Prototype_GameObject_Trigger_Boss */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Trigger_Boss"),
		CTrigger_Boss::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	///* For.Prototype_GameObject_Camera_Player */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Player"),
	//	CCamera_Player::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
	//	return -1;

	///* For.Prototype_GameObject_Item_Sheath */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Sheath"),
	//	CItem_Sheath::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
	//	return -1;

	///* For.Prototype_GameObject_Item_SwordHandle */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_SwordHandle"),
	//	CItem_SwordHandle::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
	//	return -1;

	///* For.Prototype_GameObject_Item_Blade */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Blade"),
	//	CItem_Blade::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
	//	return -1;

	///* For.Prototype_GameObject_Item_Shield */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Shield"),
	//	CItem_Shield::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC()))))
	//	return -1;

	/*For.Prototype_GameObject_Dungeon_RoomFinal*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dungeon_RoomFinal"),
		CDungeon_RoomFinal::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
		return -1;

	///*For.Prototype_GameObject_Monster*/
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
	//	CMonster::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f))))))
	//	return -1;

	/* For.Prototype_GameObject_Object_WoodDoor */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_WoodDoor"),
		CObject_WoodDoor::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_StoneBridge */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Object_StoneBridge"),
		CObject_StoneBridge::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	/* For.Prototype_GameObject_Object_StoneBridge */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_End"),
		CUI_End::Create(m_pDevice, m_pDeviceContext, CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f))))))
		return -1;

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 생성 중입니다."));

	/* For.Prototype_Component_Texture_SkyBox. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON_FINAL, TEXT("Prototype_Component_Texture_SkyBox"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_Zelda.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_End. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON_FINAL, TEXT("Prototype_Component_Texture_UI_End"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/End/UI_End.dds")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을 생성 중입니다."));

	_matrix PivotMatrix;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));

	/* For.Prototype_Component_Model_Zelda. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON_FINAL, TEXT("Prototype_Component_Model_Zelda"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Zelda/", "Zelda.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) *  XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Gnd. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON_FINAL, TEXT("Prototype_Component_Model_Gnd"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Gnd/", "Gnd_1.fbx", CModel::TYPE_ANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	/* For.Prototype_Component_Model_Dungeon_RoomFinal. */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON_FINAL, TEXT("Prototype_Component_Model_Dungeon_RoomFinal"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Stage/Dungeon/Room_Final/", "Room_Final.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	/* For.Prototype_Component_Model_Object_WoodDoor */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON_FINAL, TEXT("Prototype_Component_Model_Object_WoodDoor"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Dungeon_final/WoodDoor/", "WoodDoor.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	/* For.Prototype_Component_Model_Object_StoneBridge */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON_FINAL, TEXT("Prototype_Component_Model_Object_StoneBridge"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Dungeon_final/StoneBridge/", "StoneBridge.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.003f, 0.003f, 0.003f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	/* For.Prototype_Component_Model_Object_Door */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON_FINAL, TEXT("Prototype_Component_Model_Object_Door"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Object/Dungeon/Static/Door/", "Door.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Item_Bow */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON_FINAL, TEXT("Prototype_Component_Model_Item_Bow"),
		CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Item/Bow/", "Bow.fbx", CModel::TYPE_NONANIM, PivotMatrix))))
		return E_FAIL;

	
	lstrcpy(m_szLoadingText, TEXT("네비게이션을 생성 중입니다."));

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_DUNGEON_FINAL, TEXT("Prototype_Component_Navigation_Dungeon_Final"),
		CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Data/Navigation_Dungeon_Final.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("LEVEL_DUNGEON_FINAL 로딩이 완료되었습니다."));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return _uint();
}


CLoader * CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eNextLevelID)
{
	CLoader*	pInstance = new CLoader(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(eNextLevelID)))
	{
		MSGBOX(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	DeleteCriticalSection(&m_CriticalSection);

	DeleteObject(m_hThread);
	
	CloseHandle(m_hThread);
}
