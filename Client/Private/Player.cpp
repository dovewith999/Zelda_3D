#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"
#include "UI_PlayerHp.h"
#include "Camera_Player.h"
#include "HierarchyNode.h"
#include "Cell.h"
#include "SeamLessLoader.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
{

}

HRESULT CPlayer::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pModelCom->SetUp_WaistAnimation(51);
	m_pModelCom->SetUp_BodyAnimation(51);

	m_PlayerState = PLAYER_IDLE;
	m_eDir = DIR_FRONT;

	if (FAILED(SetUp_PlayerUI()))
	{
		MSGBOX(TEXT("Error : SetUp_PlayerUI"));
		return E_FAIL;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pGameInstance);

	if (static_cast<_uint>(LEVEL_DUNGEON) == pGameInstance->Get_NextLevelID())
	{
		m_pSeamLess_Loader = CSeamLess_Loader::Create(m_pDevice, m_pDeviceContext);
	}
	
	Safe_Release(pGameInstance);

	m_PivotMatrix = m_pModelCom->Get_PivotMatrix();

	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	if (0.f == m_fCooldown && nullptr != m_pSeamLess_Loader)
	{
		m_pSeamLess_Loader->Ready_NextRoom(m_iNextRoomID);
		++m_iNextRoomID;
		m_fCooldown += fTimeDelta;
	}

	m_bBodyAnimFinished = m_pModelCom->Get_BodyAnimationState();
	m_bWaistAnimFinished = m_pModelCom->Get_WaistAnimationState();

	//플레이어가 오브젝트를 집으면 이 위치로 오브젝트가 위치하게 됨
	XMStoreFloat4(&m_vGrapPoint, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 1.2f, 0.f, 0.f));

	//_float4x4 SocketMatrix; 
	//"cl_center                      "
	XMStoreFloat4x4(&m_SocketMatrix, /*XMMatrixTranspose(m_pModelCom->Find_HierarchyNode("cl_link_root                   ")->Get_OffSetMatrix()) **/ m_pModelCom->Find_HierarchyNode("cl_link_root                   ")->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_PivotMatrix));

	XMStoreFloat3((_float3*)&m_SocketMatrix.m[0], XMVector3Normalize(XMLoadFloat3((_float3*)&m_SocketMatrix.m[0])));
	XMStoreFloat3((_float3*)&m_SocketMatrix.m[1], XMVector3Normalize(XMLoadFloat3((_float3*)&m_SocketMatrix.m[1])));
	XMStoreFloat3((_float3*)&m_SocketMatrix.m[2], XMVector3Normalize(XMLoadFloat3((_float3*)&m_SocketMatrix.m[2])));

	//XMLoadFloat4x4(&SocketMatrix).r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4x4(&SocketMatrix).r[3]);
	//XMLoadFloat4x4(&SocketMatrix).r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	//if (PLAYER_ATTACK == m_PlayerState)
	//{
	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4x4(&m_SocketMatrix).r[3]);
	//}

	m_fTickAcc += fTimeDelta;
	m_fPatternAcc += fTimeDelta;

	if (GetKeyState('Q') & 0x8000)
	{
		if (0 < m_iPlayerHp)
		{
			--m_iPlayerHp;
			--m_iPlayerHpAcc;
			Set_HpUI();
		}
	}

	else if (GetKeyState('E') & 0x8000)
	{
		++m_iPlayerHp;
		++m_iPlayerHpAcc;
		Set_HpUI();
	}

	if (m_bDamaged)
	{

	}

	switch (m_PlayerState)
	{
	case PLAYER_JUMP:
		Pattern_Jump(fTimeDelta);
		break;
	case PLAYER_CLIMB:
		Pattern_climb(fTimeDelta);
		break;
	case PLAYER_PUSH_PULL:
		Pattern_PushOrPull(fTimeDelta);
		break;
	case PLAYER_CLIMB_CUBE:
		Pattern_Climb_Cube(fTimeDelta);
		break;
	case PLAYER_CLIMB_HiGH :
		Pattern_Climb_High(fTimeDelta);
		break;
	case PLAYER_CLIMB_HiGH_END:
		Pattern_Climb_High(fTimeDelta);
		break;
	case PLAYER_GRAB:
		Pattern_Grab_Throw(fTimeDelta);
		break;
	case PLAYER_LADDER:
		Pattern_Ladder(fTimeDelta);
		break;
	case PLAYER_DAMAGED_MAGMA:
		Pattern_Damaged_Magma(fTimeDelta);
		break;
	case PLAYER_GETITEM:
		Pattern_GetItem(fTimeDelta);
		break;
	case PLAYER_GETITEM_SHORT:
		Pattern_GetItem(fTimeDelta);
		break;
	case PLAYER_BOKO:
		Pattern_Boko(fTimeDelta);
		break;
	default:
		m_iPatternCount = 0;
		if (FAILED(SetUp_OnTerrain()))
		{
			return;
		}
		if (GetKeyState(VK_RBUTTON) & 0x8000)
		{
			if (!m_bAttention)
			{
				m_pTarget = m_pCollider_Manager->Get_Target();
				Safe_AddRef(m_pTarget);

				CGameInstance* pGameInstance = CGameInstance::Get_Instance();
				Safe_AddRef(pGameInstance);

				pGameInstance->PlaySoundW(L"Target_On.wav", SOUND_EFFECT, 0.3f);

				Safe_Release(pGameInstance);

			}

			m_bAttention = true;
		}

		else
		{
			if (m_bAttention)
			{
				CGameInstance* pGameInstance = CGameInstance::Get_Instance();
				Safe_AddRef(pGameInstance);

				pGameInstance->PlaySoundW(L"Target_Off.wav", SOUND_EFFECT, 0.3f);

				Safe_Release(pGameInstance);
			}
			m_bAttention = false;
			m_pTarget = nullptr;

		}

		if (m_bAttention)
		{
			if (nullptr != m_pTarget)
			{
				m_pTransformCom->LookAt(m_pTarget->Get_Position());
			}

			Input_Key_Attention(fTimeDelta);
		}

		else
		{
			m_pCollider_Manager->Init_CollisionList();
			Input_Key(fTimeDelta);
			m_pTarget = nullptr;
		}
		break;
	}


	if (0 == m_iPatternCount && !m_bJump && !m_bClimb && CCell::OPTION_NON == m_pNavigationCom->Get_CellOption())
	{
		++m_iPatternCount;

		if (CCell::OPTION_JUMP == m_pNavigationCom->Get_PreCellOption())
		{
				m_bJump = true;
				if (PLAYER_JUMP != m_PlayerState)
				{
					if (PLAYER_GRAB == m_PlayerState)
					{
						m_ePlayerPreState = PLAYER_GRAB;
		
					}	

					m_PlayerState = PLAYER_JUMP;
				}
				if (470 < m_pNavigationCom->Get_NumCurrentCell() && 480 > m_pNavigationCom->Get_NumCurrentCell())
				{
					m_fJumpPower = 5.f;
				}
				
				else
				{
					m_fJumpPower = 3.f;
				}

				m_fCurrentPosY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		}

		else if (CCell::OPTION_CLIMB == m_pNavigationCom->Get_PreCellOption())
		{
			m_bClimb = true;
			if (PLAYER_CLIMB != m_PlayerState)
			{
				m_PlayerState = PLAYER_CLIMB;
			}
		}

		else if (CCell::OPTION_CLIMB_HIGH == m_pNavigationCom->Get_PreCellOption())
		{
			m_bClimb = true;
			if (PLAYER_CLIMB != m_PlayerState)
			{
				m_PlayerState = PLAYER_CLIMB_HiGH;
			}
		}
	}

	if (!m_bJump && !m_bClimb && PLAYER_LADDER != m_PlayerState)
	{
		//m_iPatternCount = 0;
		if (FAILED(SetUp_OnTerrain()))
		{
			return; 
		}
	}

	if (m_bCollision)
	{
		if (GetKeyState('G') & 0x8000)
		{
			if (PLAYER_PUSH_PULL != m_PlayerState)
			{
				m_PlayerState = PLAYER_PUSH_PULL;
			}
		}

		else if (0.5f < m_fPatternAcc && 0.5f < m_fTickAcc && GetKeyState(VK_SPACE) & 0x8000)
		{
			if (m_bUnderCollision)
			{
				m_bUnderCollision = false;
			}
			m_PlayerState = PLAYER_CLIMB_CUBE;
			m_fTickAcc = 0.f;
		}
	}

	if (m_bUnderCollision && !m_bCollision && PLAYER_DASH == m_PlayerState)
	{
		m_fClimbTimeAcc += fTimeDelta;

		if (0.1f < m_fClimbTimeAcc || m_bUnderMagRock)
		{
			m_bJump = true;
			if (PLAYER_JUMP != m_PlayerState)
			{
				m_PlayerState = PLAYER_JUMP;
			}

			if (1 < m_iNumUnderObject)
			{
				m_fJumpPower = 2.f;
			}

			else if (500 < m_pNavigationCom->Get_NumCurrentCell())
			{
				m_fJumpPower = 4.f;
			}
			
			else
			{
				m_fJumpPower = 6.f;
			}

			m_fCurrentPosY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_bUnderCollision = false;
			m_bUnderMagRock = false;
			m_iNumUnderObject = 0;
			m_fSizeOfCubeY = 0.f;
			m_fClimbTimeAcc = 0.f;
		}
	}

	if (m_bCanOpenBox)
	{
		if (GetKeyState('G') & 0x8000)
		{
			m_PlayerState = PLAYER_GETITEM;
		}
	}

	if (m_bCanOpenBoxShort)
	{
		if (GetKeyState('G') & 0x8000)
		{
			m_PlayerState = PLAYER_GETITEM_SHORT;
		}
	}

	
	if (PLAYER_GETITEM != m_PlayerState && PLAYER_GETITEM_SHORT != m_PlayerState && /*PLAYER_GRAB != m_PlayerState &&*/ PLAYER_CLIMB != m_PlayerState && PLAYER_JUMP != m_PlayerState)
	{
		m_pModelCom->Play_Animation_Body(fTimeDelta * m_fBodyAnimSpeed);
		m_pModelCom->Play_Animation_Waist(fTimeDelta * m_fWaistAnimSpeed);
	}

	else
	{
		m_pModelCom->Play_Animation_Body_Loop(fTimeDelta * m_fBodyAnimSpeed);
		m_pModelCom->Play_Animation_Waist_Loop(fTimeDelta * m_fWaistAnimSpeed);
	}

	m_pTransformCom->Set_Scaled(XMVectorSet(1.f, 1.f, 1.f, 1.f));

	m_pOBB->Update(XMLoadFloat4x4(&m_SocketMatrix) * m_pTransformCom->Get_WorldMatrix());
	m_pSPHERE->Update(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHABLEND, this);

	m_bCollision = false;
	
	if (PLAYER_JUMP != m_PlayerState && PLAYER_CLIMB != m_PlayerState)
	{
		m_iPatternCount = 0;
	}

	//m_pRendererCom->Add_DebugList(m_pOBB);
	//m_pRendererCom->Add_DebugList(m_pSPHERE);
	//m_pRendererCom->Add_DebugList(m_pNavigationCom);

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_pOBB->Get_Pos().x, m_pOBB->Get_Pos().y, m_pOBB->Get_Pos().z, 1.f));
	
}

HRESULT CPlayer::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->SetUp_BoneMatrices_OnShader(m_pShaderCom, "g_Bones", i);

		m_pModelCom->SetUp_Material_OnShader(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(1);
		m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	//m_pAABB->Render();
	//m_pOBB->Render();
	//m_pSPHERE->Render();
	//m_pNavigationCom->Render();
#endif // _DEBUG

	return S_OK;
}

void CPlayer::Find_RealCell(_float3 vPos)
{
	m_pNavigationCom->Find_RealCell(&vPos);
}

void CPlayer::Check_Front(_double TimeDelta, _bool bResult, _float fFirstX, _float fFirstZ)
{

	if (bResult)
	{
		//_vector vPos;

		m_pTransformCom->Go_Axis(TimeDelta, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK) * -1.f));

	}
}

HRESULT CPlayer::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider_Manager */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Manager"), TEXT("Com_Collider_Manager"), (CComponent**)&m_pCollider_Manager)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Model_Link"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Collider_OBB*/
	CCollider::COLLIDER_DESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDER_DESC));
	ColliderDesc.vScale = _float3(0.7f, 1.f, 0.8f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngle = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"), (CComponent**)&m_pOBB, &ColliderDesc)))
		return E_FAIL;

	 
	/* For.Com_Collider_SPHERE*/
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDER_DESC));
	ColliderDesc.vScale = _float3(8.f, 8.f, 8.f);
	ColliderDesc.vPosition = _float3(0.f, 0.0f, 0.f);

	if (FAILED(__super::Add_Components(LEVEL_PLAY_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_SPHERE"), (CComponent**)&m_pSPHERE, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Navigation*/
	CNavigation::NAVIGATIONDESC		NaviDesc;
	NaviDesc.iCurrentIndex = 0;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (static_cast<_uint>(LEVEL_DUNGEON) == pGameInstance->Get_NextLevelID())
	{
		if (FAILED(__super::Add_Components(LEVEL_DUNGEON, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
	}

	else if (static_cast<_uint>(LEVEL_DUNGEON_FINAL) == pGameInstance->Get_NextLevelID())
	{
		if (FAILED(__super::Add_Components(LEVEL_DUNGEON_FINAL, TEXT("Prototype_Component_Navigation_Dungeon_Final"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CPlayer::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	//m_pTransformCom->Mul_WorldMatrix(SocketMatrix);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	/*_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(&SocketMatrix)));

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));*/
	

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPositionFloat4(), sizeof(_float4));

	//const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	//if (nullptr == pLightDesc)
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPlayer::SetUp_PlayerUI()
{
	_uint iNumPlayerHp = m_iPlayerHp / 4;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	for (_uint i = 0; i < iNumPlayerHp; ++i)
	{
		if (static_cast<_uint>(LEVEL_DUNGEON) == pGameInstance->Get_NextLevelID())
		{
			if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_UI_PlayerHp"), TEXT("Prototype_GameObject_UI_PlayerHp"), &_float2(i * 50.f + 25.f, 25.f))))
				return E_FAIL;

			m_PlayerHps.push_back(dynamic_cast<CUI_PlayerHp*>(pGameInstance->Find_GameObject(LEVEL_DUNGEON, TEXT("Layer_UI_PlayerHp"))));
			Safe_AddRef(m_PlayerHps.back());
		}

		else if (static_cast<_uint>(LEVEL_DUNGEON_FINAL) == pGameInstance->Get_NextLevelID())
		{
			if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON_FINAL, TEXT("Layer_UI_PlayerHp"), TEXT("Prototype_GameObject_UI_PlayerHp"), &_float2(i * 50.f + 25.f, 25.f))))
				return E_FAIL;

			m_PlayerHps.push_back(dynamic_cast<CUI_PlayerHp*>(pGameInstance->Find_GameObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_UI_PlayerHp"))));
			Safe_AddRef(m_PlayerHps.back());
		}
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CPlayer::SetUp_OnTerrain()
{
	_vector vPosition = XMVectorSetW(m_pNavigationCom->Compute_Height(m_pTransformCom->Get_State(CTransform::STATE_POSITION)), 1.f);

	_vector vPos = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), (XMVectorGetY(vPosition) + m_iNumUnderObject * m_fSizeOfCubeY));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	return S_OK;
}

void CPlayer::Set_HpUI()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_uint iNumPlayerHp = m_iPlayerHp % 4 - 1;

	m_PlayerHps.back()->Set_TextureNum(iNumPlayerHp);

	if (0 == m_iPlayerHp % 4)
	{
		if (4 == m_iPlayerHpAcc)
		{

			if (static_cast<_uint>(LEVEL_DUNGEON) == pGameInstance->Get_NextLevelID())
			{
				if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON, TEXT("Layer_UI_PlayerHp"), TEXT("Prototype_GameObject_UI_PlayerHp"), &_float2((m_PlayerHps.size() - 1) * 50.f + 25.f, 25.f))))
					return;

				m_PlayerHps.push_back(dynamic_cast<CUI_PlayerHp*>(pGameInstance->Find_GameObject(LEVEL_DUNGEON, TEXT("Layer_UI_PlayerHp"))));
				Safe_AddRef(m_PlayerHps.back());
			}

			else if (static_cast<_uint>(LEVEL_DUNGEON_FINAL) == pGameInstance->Get_NextLevelID())
			{
				if (FAILED(pGameInstance->Add_Layer(LEVEL_DUNGEON_FINAL, TEXT("Layer_UI_PlayerHp"), TEXT("Prototype_GameObject_UI_PlayerHp"), &_float2((m_PlayerHps.size() - 1) * 50.f + 25.f, 25.f))))
					return;

				m_PlayerHps.push_back(dynamic_cast<CUI_PlayerHp*>(pGameInstance->Find_GameObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_UI_PlayerHp"))));
				Safe_AddRef(m_PlayerHps.back());
			}


			m_iPlayerHpAcc = 0;
		}

		else if (-4 == m_iPlayerHpAcc)
		{
			if (static_cast<_uint>(LEVEL_DUNGEON) == pGameInstance->Get_NextLevelID())
			{
				if (FAILED(pGameInstance->Release_GameObject(LEVEL_DUNGEON, TEXT("Layer_UI_PlayerHp"))))
					return;
				m_PlayerHps.pop_back();
			}

			else if (static_cast<_uint>(LEVEL_DUNGEON_FINAL) == pGameInstance->Get_NextLevelID())
			{
				if (FAILED(pGameInstance->Release_GameObject(LEVEL_DUNGEON_FINAL, TEXT("Layer_UI_PlayerHp"))))
					return;
				m_PlayerHps.pop_back();
			}

			m_iPlayerHpAcc = 0;
		}
	}

	Safe_Release(pGameInstance);
}

void CPlayer::Input_Key(_float fTimeDelta)
{
	if ( PLAYER_IDLE == m_PlayerState 
		|| PLAYER_DASH == m_PlayerState 
		|| PLAYER_WALK == m_PlayerState 
		|| m_bWaistAnimFinished 
		|| m_bBodyAnimFinished
		|| (m_bBodyAnimFinished && PLAYER_WEAR == m_PlayerState))
	{

		m_fBodyAnimSpeed = 0.5f;
		m_fWaistAnimSpeed = 0.5f;
		
		m_pModelCom->SetUp_WaistAnimation(86);
		if(PLAYER_WEAR != m_PlayerState || (PLAYER_WEAR == m_PlayerState && m_bBodyAnimFinished))
		{
			m_PlayerState = PLAYER_IDLE;
			m_pModelCom->SetUp_BodyAnimation(86);
		}
		
	}

	if (GetKeyState('A') & 0x8000)
	{

		if (GetKeyState('W') & 0x8000)
		{
			if (DIR_7 != m_eDir && DIR_LEFT == m_eDir)
			{
				m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.0f));
				m_eDir = DIR_7;
			}
			else if (DIR_7 != m_eDir && DIR_FRONT == m_eDir)
			{
				m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), -XMConvertToRadians(45.0f));
				m_eDir = DIR_7;
			}
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
		}

		else if (GetKeyState('S') & 0x8000)
		{
			if (DIR_1 != m_eDir && DIR_LEFT == m_eDir)
			{
				m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), -XMConvertToRadians(45.0f));
				m_eDir = DIR_1;
			}

			else if (DIR_1 != m_eDir && DIR_BACK == m_eDir)
			{
				m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.0f));
				m_eDir = DIR_1;
			}
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
		}

		else
		{
			m_eDir = DIR_LEFT;
			m_pTransformCom->LookAtAxis(XMLoadFloat4(&m_Axis_Side) * -1.f);
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
		}


		if (PLAYER_ROLLING != m_PlayerState || (m_bBodyAnimFinished && m_bWaistAnimFinished))
		{
			if (PLAYER_WEAR != m_PlayerState)
			{
				if (m_bWearItem)
				{
					m_pModelCom->SetUp_BodyAnimation(44);
				}
				else
				{
					m_pModelCom->SetUp_BodyAnimation(43);
				}
				m_PlayerState = PLAYER_DASH;
			}

			if (m_bWearItem)
			{
				m_pModelCom->SetUp_WaistAnimation(44);
			}
			else
			{
				m_pModelCom->SetUp_WaistAnimation(43);
			}
		}

		m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f)));
		m_fBodyAnimSpeed = 1.f;
		m_fWaistAnimSpeed = 1.f;

		if (PLAYER_WEAR == m_PlayerState)
		{
			m_fWaistAnimSpeed = 2.f;
		}

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		pGameInstance->PlaySoundW(L"Walk.wav", SOUND_WALK, 0.1f);
		

		Safe_Release(pGameInstance);
	}

	else if (GetKeyState('D') & 0x8000)
	{

		if (GetKeyState('W') & 0x8000)
		{
			if (DIR_9 != m_eDir && DIR_RIGHT == m_eDir)
			{
				m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), -XMConvertToRadians(45.0f));
				m_eDir = DIR_9;
			}

			else if (DIR_9 != m_eDir && DIR_FRONT == m_eDir)
			{
				m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.0f));
				m_eDir = DIR_9;
			}
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
		}

		else if (GetKeyState('S') & 0x8000)
		{
			if (DIR_3 != m_eDir && DIR_RIGHT == m_eDir)
			{
				m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.0f));
				m_eDir = DIR_3;
			}

			if (DIR_3 != m_eDir && DIR_BACK == m_eDir)
			{
				m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), -XMConvertToRadians(45.0f));
				m_eDir = DIR_3;
			}
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
		}

		else
		{
			m_eDir = DIR_RIGHT;
			m_pTransformCom->LookAtAxis(XMLoadFloat4(&m_Axis_Side));
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
		}

	
		if (PLAYER_ROLLING != m_PlayerState || (m_bBodyAnimFinished && m_bWaistAnimFinished))
		{
			if (PLAYER_WEAR != m_PlayerState)
			{
				if (m_bWearItem)
				{
					m_pModelCom->SetUp_BodyAnimation(44);
				}
				else
				{
					m_pModelCom->SetUp_BodyAnimation(43);
				}
				m_PlayerState = PLAYER_DASH;
			}

			if (m_bWearItem)
			{
				m_pModelCom->SetUp_WaistAnimation(44);
			}
			else
			{
				m_pModelCom->SetUp_WaistAnimation(43);
			}
		}
		m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f)));
		m_fBodyAnimSpeed = 1.f;
		m_fWaistAnimSpeed = 1.f;

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		pGameInstance->PlaySoundW(L"Walk.wav", SOUND_WALK, 0.1f);
	

		Safe_Release(pGameInstance);

		if (PLAYER_WEAR == m_PlayerState)
		{
			m_fWaistAnimSpeed = 2.f;
		}

	}


	else if (GetKeyState('W') & 0x8000)
	{
		
		m_eDir = DIR_FRONT;
		m_pTransformCom->LookAtAxis(XMLoadFloat4(&m_Axis_Front));
	/*	if (!m_bCollision && !m_bUnderCollision)
		{*/
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
		//}
		
	
		if (PLAYER_ROLLING != m_PlayerState || (m_bBodyAnimFinished && m_bWaistAnimFinished))
		{
			if (PLAYER_WEAR != m_PlayerState)
			{
				if (m_bWearItem)
				{
					m_pModelCom->SetUp_BodyAnimation(44);
				}
				else
				{
					m_pModelCom->SetUp_BodyAnimation(43);
				}
				m_PlayerState = PLAYER_DASH;
			}

			if (m_bWearItem)
			{
				m_pModelCom->SetUp_WaistAnimation(44);
			}
			else
			{
				m_pModelCom->SetUp_WaistAnimation(43);
			}
			
		}

		m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f)));
		m_fBodyAnimSpeed = 1.f;
		m_fWaistAnimSpeed = 1.f;

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		pGameInstance->PlaySoundW(L"Walk.wav", SOUND_WALK, 0.1f);
	

		Safe_Release(pGameInstance);

		if (PLAYER_WEAR == m_PlayerState)
		{
			m_fWaistAnimSpeed = 2.f;
		}

	}

	else if (GetKeyState('S') & 0x8000)
	{
		m_eDir = DIR_BACK;
		m_pTransformCom->LookAtAxis(XMLoadFloat4(&m_Axis_Front) * -1.f);
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);

		if (PLAYER_ROLLING != m_PlayerState || (m_bBodyAnimFinished && m_bWaistAnimFinished))
		{
			if (PLAYER_WEAR != m_PlayerState)
			{
				if (m_bWearItem)
				{
					m_pModelCom->SetUp_BodyAnimation(44);
				}
				else
				{
					m_pModelCom->SetUp_BodyAnimation(43);
				}
				m_PlayerState = PLAYER_DASH;
			}

			if (m_bWearItem)
			{
				m_pModelCom->SetUp_WaistAnimation(44);
			}
			else
			{
				m_pModelCom->SetUp_WaistAnimation(43);
			}
		}
		m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f)));
		m_fBodyAnimSpeed = 1.f;
		m_fWaistAnimSpeed = 1.f;

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		pGameInstance->PlaySoundW(L"Walk.wav", SOUND_WALK, 0.1f);


		Safe_Release(pGameInstance);

		if (PLAYER_WEAR == m_PlayerState)
		{
			m_fWaistAnimSpeed = 2.f;
		}
	}

	if (0.1f <= m_fTickAcc && GetKeyState(VK_LBUTTON) & 0x8000 && PLAYER_ATTACK != m_PlayerState)
	{
		++m_iAttackAcc;

		if (3 > m_iAttackAcc)
		{
			m_pModelCom->SetUp_WaistAnimation(3);
			m_pModelCom->SetUp_BodyAnimation(3);
			m_PlayerState = PLAYER_ATTACK;
			m_fBodyAnimSpeed = 1.f;
			m_fWaistAnimSpeed = 1.f;


			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);
			pGameInstance->PlaySoundW(L"Attack_1.wav", SOUND_PLAYER, 0.2f);


			pGameInstance->PlaySoundW(L"Sword_Slash.wav", SOUND_ITEM, 0.2f);
			Safe_Release(pGameInstance);
		}

		else if (3 == m_iAttackAcc)
		{
			m_pModelCom->SetUp_WaistAnimation(6);
			m_pModelCom->SetUp_BodyAnimation(6);
			m_PlayerState = PLAYER_ATTACK;
			m_fBodyAnimSpeed = 1.f;
			m_fWaistAnimSpeed = 1.f;
			m_iAttackAcc = 0;

			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);
			pGameInstance->PlaySoundW(L"Attack1_Final.wav", SOUND_PLAYER, 0.2f);

			pGameInstance->PlaySoundW(L"Sword_Slash.wav", SOUND_ITEM, 0.2f);
			Safe_Release(pGameInstance);

		}

		m_fTickAcc = 0.f;
	}

	if (1.0f <= m_fTickAcc && (PLAYER_WEAR == m_PlayerState || (PLAYER_IDLE == m_PlayerState && GetKeyState(VK_CONTROL) & 0x8000) || (PLAYER_WALK == m_PlayerState && GetKeyState(VK_CONTROL) & 0x8000) || (PLAYER_DASH == m_PlayerState && GetKeyState(VK_CONTROL) & 0x8000)))
	{
		if (m_bWearItem)
		{
			m_bWearItem = false;
		}

		else
		{
			m_bWearItem = true;
		}

		if (PLAYER_WEAR != m_PlayerState)
		{
			m_pModelCom->SetUp_BodyAnimation(73);

			m_PlayerState = PLAYER_WEAR;
		}

		m_fBodyAnimSpeed = 1.f;
		m_fWaistAnimSpeed = 1.f;
		m_fTickAcc = 0.f;

	}

	if (PLAYER_DASH == m_PlayerState && GetKeyState(VK_SHIFT) & 0x8000)
	{
		if (PLAYER_ROLLING != m_PlayerState || (m_bBodyAnimFinished && m_bWaistAnimFinished))
		{
			m_pModelCom->SetUp_WaistAnimation(87);
			m_pModelCom->SetUp_BodyAnimation(87);
			m_PlayerState = PLAYER_WALK;
		}
		m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(2.f, XMConvertToRadians(60.f)));
		m_fBodyAnimSpeed = 1.f;
		m_fWaistAnimSpeed = 1.f;
	}

	if (PLAYER_ROLLING == m_PlayerState || (PLAYER_WALK == m_PlayerState && GetKeyState(VK_SPACE) & 0x8000) || (PLAYER_DASH == m_PlayerState && GetKeyState(VK_SPACE) & 0x8000))
	{
		if (PLAYER_ROLLING != m_PlayerState)
		{
			m_pModelCom->SetUp_WaistAnimation(76);
			m_pModelCom->SetUp_BodyAnimation(76);
			m_PlayerState = PLAYER_ROLLING;

			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);
			pGameInstance->PlaySoundW(L"Rolling.wav", SOUND_PLAYER, 0.2f);

			pGameInstance->PlaySoundW(L"Rolling_Effect.wav", SOUND_EFFECT, 0.2f);
			Safe_Release(pGameInstance);
		}
		m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f)));
		m_fBodyAnimSpeed = 1.f;
		m_fWaistAnimSpeed = 1.f;
	}

}

void CPlayer::Input_Key_Attention(_float fTimeDelta)
{

	if (PLAYER_IDLE == m_PlayerState || PLAYER_WALK == m_PlayerState || m_bWaistAnimFinished || m_bBodyAnimFinished)
	{
		m_fBodyAnimSpeed = 0.5;
		m_fWaistAnimSpeed = 0.5;
		m_PlayerState = PLAYER_IDLE;
		if (m_bWearItem)
		{
			m_pModelCom->SetUp_BodyAnimation(20);
			m_pModelCom->SetUp_WaistAnimation(82);

			m_fWaistAnimSpeed = 1.f;
		}

		else
		{

			if (DIR_RIGHT == m_eDir)
			{
				m_pModelCom->SetUp_BodyAnimation(25);
				m_pModelCom->SetUp_WaistAnimation(25);
			}
			else
			{

				m_pModelCom->SetUp_BodyAnimation(21);
				m_pModelCom->SetUp_WaistAnimation(21);
				
			}
		}

	}

	if (0.1f <= m_fTickAcc && GetKeyState(VK_LBUTTON) & 0x8000 && PLAYER_ATTACK != m_PlayerState)
	{
		++m_iAttackAcc;

		if (3 > m_iAttackAcc)
		{
			m_PlayerState = PLAYER_ATTACK;
			m_pModelCom->SetUp_WaistAnimation(0);
			m_pModelCom->SetUp_BodyAnimation(0);
			m_fBodyAnimSpeed = 1.f;
			m_fWaistAnimSpeed = 1.f;


			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);
			pGameInstance->PlaySoundW(L"Attack2.wav", SOUND_PLAYER, 0.2f);


			pGameInstance->PlaySoundW(L"Sword_Slash.wav", SOUND_ITEM, 0.2f);
			Safe_Release(pGameInstance);
		}

		else if (3 <= m_iAttackAcc)
		{
			m_pModelCom->SetUp_WaistAnimation(11);
			m_pModelCom->SetUp_BodyAnimation(11);
			m_PlayerState = PLAYER_ATTACK;
			m_fBodyAnimSpeed = 1.f;
			m_fWaistAnimSpeed = 1.f;
			m_iAttackAcc = 0;

			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);
			pGameInstance->PlaySoundW(L"Attack2_Final.wav", SOUND_PLAYER, 0.2f);


			pGameInstance->PlaySoundW(L"Sword_Slash.wav", SOUND_ITEM, 0.2f);
			Safe_Release(pGameInstance);
		}

		m_fTickAcc = 0.f;
	}

	if (GetKeyState('S') & 0x8000)
	{
		if (2 != m_iRollingAcc)
		{
			m_pTransformCom->Go_Axis(fTimeDelta, XMLoadFloat4(&m_Axis_Front) * -1.f);
		}

		if (PLAYER_ROLLING != m_PlayerState || (m_bWaistAnimFinished))
		{
			m_pModelCom->SetUp_WaistAnimation(19);
			if (!m_bWearItem)
			{
				m_pModelCom->SetUp_BodyAnimation(19);
			}
			m_PlayerState = PLAYER_WALK;
			m_iRollingAcc = 0;
		}
		m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(3.f, XMConvertToRadians(60.f)));

		if (m_bWearItem)
		{
			m_fBodyAnimSpeed = 1.f;
			m_fWaistAnimSpeed = 1.f;

		}
		else
		{
			m_fBodyAnimSpeed = 0.5f;
			m_fWaistAnimSpeed = 0.5f;

		}
	
		if (PLAYER_ROLLING == m_PlayerState ||  GetKeyState(VK_SPACE) & 0x8000)
		{

			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);
			
			if (PLAYER_ROLLING != m_PlayerState)
			{
				m_pModelCom->SetUp_WaistAnimation(74);
				m_pModelCom->SetUp_BodyAnimation(74);
				m_PlayerState = PLAYER_ROLLING;

				pGameInstance->PlaySoundW(L"Rolling.wav", SOUND_PLAYER, 0.2f);
				pGameInstance->PlaySoundW(L"Rolling_Effect.wav", SOUND_EFFECT, 0.2f);
			}
			m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f)));
			m_fBodyAnimSpeed = 1.f;
			m_fWaistAnimSpeed = 1.f;
			m_iRollingAcc = 1;

		

			Safe_Release(pGameInstance);
		}

		if (1 == m_iRollingAcc && (m_bBodyAnimFinished && m_bWaistAnimFinished))
		{
			m_PlayerState = PLAYER_ROLLING;
			m_pModelCom->SetUp_WaistAnimation(75);
			m_fBodyAnimSpeed = 1.f;
			m_fWaistAnimSpeed = 1.f;
			m_iRollingAcc = 2;
		}
	}

	else if ((PLAYER_ROLLING == m_PlayerState || PLAYER_IDLE == m_PlayerState || PLAYER_WALK == m_PlayerState || PLAYER_DASH == m_PlayerState) && GetKeyState('W') & 0x8000)
	{
		m_pTransformCom->Go_Axis(fTimeDelta, XMLoadFloat4(&m_Axis_Front));
		if (PLAYER_ROLLING != m_PlayerState || (m_bWaistAnimFinished))
		{
			m_pModelCom->SetUp_WaistAnimation(44);
			if (!m_bWearItem)
			{
				m_pModelCom->SetUp_BodyAnimation(44);
			}
			m_PlayerState = PLAYER_WALK;
		}
		m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(2.f, XMConvertToRadians(60.f)));
		m_fBodyAnimSpeed = 1.f;
		m_fWaistAnimSpeed = 1.f;

		if (PLAYER_ROLLING == m_PlayerState || GetKeyState(VK_SPACE) & 0x8000)
		{
			if (PLAYER_ROLLING != m_PlayerState)
			{
				m_pModelCom->SetUp_WaistAnimation(76);
				m_pModelCom->SetUp_BodyAnimation(76);
				m_PlayerState = PLAYER_ROLLING;

				CGameInstance* pGameInstance = CGameInstance::Get_Instance();
				Safe_AddRef(pGameInstance);
				pGameInstance->PlaySoundW(L"Rolling.wav", SOUND_PLAYER, 0.2f);
				pGameInstance->PlaySoundW(L"Rolling_Effect.wav", SOUND_EFFECT, 0.2f);

				Safe_Release(pGameInstance);
			}
			m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f)));
			m_fBodyAnimSpeed = 1.f;
			m_fWaistAnimSpeed = 1.f;


		}
	}

	if (PLAYER_ROLLING != m_PlayerState && GetKeyState('A') & 0x8000)
	{
		m_eDir = DIR_LEFT;
		if (2 != m_iRollingAcc)
		{
			m_pTransformCom->Go_Axis(fTimeDelta, XMLoadFloat4(&m_Axis_Side) * -1.f);
		}

		if (PLAYER_ROLLING != m_PlayerState)
		{
			m_pModelCom->SetUp_WaistAnimation(24);
			if (!m_bWearItem)
			{
				m_pModelCom->SetUp_BodyAnimation(24);
			}
			m_PlayerState = PLAYER_WALK;
			m_iRollingAcc = 0;

		}

		m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(2.f, XMConvertToRadians(60.f)));
		m_fBodyAnimSpeed = 1.f;
		m_fWaistAnimSpeed = 1.f;

		if (PLAYER_ROLLING == m_PlayerState || GetKeyState(VK_SPACE) & 0x8000)
		{
			if (PLAYER_ROLLING != m_PlayerState)
			{
				m_pModelCom->SetUp_WaistAnimation(22);
				if (!m_bWearItem)
				{
					m_pModelCom->SetUp_BodyAnimation(22);
				}
				m_PlayerState = PLAYER_ROLLING;

				CGameInstance* pGameInstance = CGameInstance::Get_Instance();
				Safe_AddRef(pGameInstance);
				pGameInstance->PlaySoundW(L"Rolling.wav", SOUND_PLAYER, 0.2f);
				pGameInstance->PlaySoundW(L"Rolling_Effect.wav", SOUND_EFFECT, 0.2f);

				Safe_Release(pGameInstance);
			}
			m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f)));
			m_fBodyAnimSpeed = 1.f;
			m_fWaistAnimSpeed = 1.f;
			m_iRollingAcc = 1;
		}

		if (1 == m_iRollingAcc && (m_bBodyAnimFinished && m_bWaistAnimFinished))
		{
			m_PlayerState = PLAYER_ROLLING;
			m_pModelCom->SetUp_WaistAnimation(23);
			m_fBodyAnimSpeed = 1.f;
			m_fWaistAnimSpeed = 1.f;
			m_iRollingAcc = 2;
		}
	}

	if (PLAYER_ROLLING != m_PlayerState && GetKeyState('D') & 0x8000)
	{
		m_eDir = DIR_RIGHT;

		if (2 != m_iRollingAcc)
		{
			m_pTransformCom->Go_Axis(fTimeDelta, XMLoadFloat4(&m_Axis_Side));
		}

		if (PLAYER_ROLLING != m_PlayerState)
		{
			m_pModelCom->SetUp_WaistAnimation(28);
			if (!m_bWearItem)
			{
				m_pModelCom->SetUp_BodyAnimation(28);
			}
			m_PlayerState = PLAYER_WALK;
			m_iRollingAcc = 0;
		}

		m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(2.f, XMConvertToRadians(60.f)));
		m_fBodyAnimSpeed = 1.f;
		m_fWaistAnimSpeed = 1.f;

		if (PLAYER_ROLLING == m_PlayerState || GetKeyState(VK_SPACE) & 0x8000)
		{
			if (PLAYER_ROLLING != m_PlayerState)
			{
				m_pModelCom->SetUp_WaistAnimation(26);


				if (!m_bWearItem)
				{
					m_pModelCom->SetUp_BodyAnimation(26);
				}
				m_PlayerState = PLAYER_ROLLING;

				CGameInstance* pGameInstance = CGameInstance::Get_Instance();
				Safe_AddRef(pGameInstance);
				pGameInstance->PlaySoundW(L"Rolling.wav", SOUND_PLAYER, 0.2f);
				pGameInstance->PlaySoundW(L"Rolling_Effect.wav", SOUND_EFFECT, 0.2f);
				Safe_Release(pGameInstance);
			}
			m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(7.f, XMConvertToRadians(60.f)));
			m_fBodyAnimSpeed = 1.f;
			m_fWaistAnimSpeed = 1.f;
			m_iRollingAcc = 1;
		}

		if (1 == m_iRollingAcc && (m_bBodyAnimFinished && m_bWaistAnimFinished))
		{
			m_PlayerState = PLAYER_ROLLING;
			m_pModelCom->SetUp_WaistAnimation(27);
			m_fBodyAnimSpeed = 1.f;
			m_fWaistAnimSpeed = 1.f;
			m_iRollingAcc = 2;
		}
	}

}

void CPlayer::Pattern_Jump(_float fTimeDelta)
{
	if (0.f == m_fJumpTimeAcc)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		pGameInstance->PlaySoundW(L"Jump.wav", SOUND_PLAYER, 0.2f);
		Safe_Release(pGameInstance);
	}
	m_fJumpTimeAcc += fTimeDelta;

	_float fCellPosY = XMVectorGetY(XMVectorSetW(m_pNavigationCom->Compute_Height(m_pTransformCom->Get_State(CTransform::STATE_POSITION)), 1.f));

	_float fY = m_fCurrentPosY + ((m_fJumpPower * m_fJumpTimeAcc) - ((9.8f * m_fJumpTimeAcc * m_fJumpTimeAcc) * 0.5f));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), fY));

	_float fMyPosY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (fMyPosY > fCellPosY)
	{

		m_pTransformCom->Go_Straight(static_cast<_double>(fTimeDelta), m_pNavigationCom);

		if (((m_fJumpPower * m_fJumpTimeAcc) > ((9.8f * m_fJumpTimeAcc * m_fJumpTimeAcc) * 0.5f)))
		{
			if (PLAYER_GRAB == m_ePlayerPreState)
			{
				m_pModelCom->SetUp_BodyAnimation(51);
			}
			else
			{
				m_pModelCom->SetUp_BodyAnimation(62); // Jump Start
			}
			m_pModelCom->SetUp_WaistAnimation(62);
		}

		else if ((m_fJumpPower * m_fJumpTimeAcc) <= ((9.8f * m_fJumpTimeAcc * m_fJumpTimeAcc) * 0.5f))
		{
			if (PLAYER_GRAB == m_ePlayerPreState)
			{
				m_pModelCom->SetUp_BodyAnimation(51);
			}
			else
			{
				m_pModelCom->SetUp_BodyAnimation(59);
			}
			m_pModelCom->SetUp_WaistAnimation(59);
		}
	}

	else if (fMyPosY <= fCellPosY)
	{

		if (FAILED(SetUp_OnTerrain()))
		{
			return;
		}

		if (m_bBodyAnimFinished || m_bWaistAnimFinished)
		{
			if (PLAYER_GRAB == m_ePlayerPreState)
			{
				m_pModelCom->SetUp_BodyAnimation(51);
			}
			else
			{
				m_pModelCom->SetUp_BodyAnimation(61); // Jump Land
			}

			m_pModelCom->SetUp_WaistAnimation(61);

			m_bJump = false;

			if (PLAYER_GRAB == m_ePlayerPreState)
			{
				m_PlayerState = PLAYER_GRAB;
				m_eSTATE_GRAB = GRAB_IDLE;
				m_ePlayerPreState = PLAYER_END;
			}
			else
			{
				m_PlayerState = PLAYER_IDLE;
			}
			m_fJumpTimeAcc = 0.f;
			m_fJumpPower = 0.f;

			m_fPatternAcc = 0.f;
		}

	}

}

void CPlayer::Pattern_Hang(_float fTimeDelta)
{
}

void CPlayer::Pattern_Swim(_float fTimeDelta)
{
}

void CPlayer::Pattern_PushOrPull(_float fTimeDelta)
{
	m_bCollision = true;

	m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(2.f, XMConvertToRadians(60.f)));

	m_pModelCom->SetUp_BodyAnimation(70);
	m_pModelCom->SetUp_WaistAnimation(70);

	if (GetKeyState('W') & 0x8000)
	{
		m_pModelCom->SetUp_BodyAnimation(72);
		m_pModelCom->SetUp_WaistAnimation(72);

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		pGameInstance->PlaySoundW(L"Link_Push.wav", SOUND_PLAYER, 0.2f);
		Safe_Release(pGameInstance);
	}

	else if (GetKeyState('S') & 0x8000)
	{
		m_pModelCom->SetUp_BodyAnimation(71);
		m_pModelCom->SetUp_WaistAnimation(71);

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		pGameInstance->PlaySoundW(L"Link_Push.wav", SOUND_PLAYER, 0.2f);
		Safe_Release(pGameInstance);
	}

	if (!m_bCollision || GetKeyState(VK_SPACE) & 0x8000 )
	{
		m_PlayerState = PLAYER_IDLE;
		m_fPatternAcc = 0.f;
	}
}

void CPlayer::Pattern_climb(_float fTimeDelta)
{
	//m_pModelCom->SetUp_BodyAnimation(57); // Hang_Ready_StandUp
	//m_pModelCom->SetUp_BodyAnimation(57); // Hang_Ready_StandUp
	//m_pTransformCom->Go_Straight(0.01f, m_pNavigationCom);
	
	//if (FAILED(SetUp_OnTerrain()))
	//{
	//	return;
	//}
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.8f, 0.f, 0.f));

	if (CLIMB_END == m_eState_Climb)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 1.f, 0.f, 0.f));

		if (0.2f >= m_fClimbTimeAcc)
		{
			m_fClimbTimeAcc += fTimeDelta;
			m_pTransformCom->Go_Straight(static_cast<_double>(fTimeDelta), m_pNavigationCom);
		}

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		pGameInstance->PlaySoundW(L"Climb.wav", SOUND_PLAYER, 0.2f);
		Safe_Release(pGameInstance);

		m_eState_Climb = CLIMB_READY;
	}

	if (CLIMB_READY == m_eState_Climb)
	{
		m_pModelCom->SetUp_BodyAnimation(57); // Hang_StandUp
		m_pModelCom->SetUp_WaistAnimation(57); // Hang_StandUp

	/*	m_fBodyAnimSpeed = 0.1f;
		m_fWaistAnimSpeed = 0.1f;*/
		if (/*m_bBodyAnimFinished ||*/ m_bWaistAnimFinished)
		{
			m_bClimb = false;
			m_PlayerState = PLAYER_IDLE;
			m_eState_Climb = CLIMB_END;

			m_pNavigationCom->Find_CellOption(2);
			m_fClimbTimeAcc = 0.f;
	
			/*_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);*/

			m_fPatternAcc = 0.f;
		}

	}


}

void CPlayer::Pattern_Climb_Cube(_float fTimeDelta)
{
	//if (0.2f >= m_fClimbTimeAcc)
	//{
	//	m_fClimbTimeAcc += fTimeDelta;
	//	m_pTransformCom->Go_Straight(static_cast<_double>(fTimeDelta), m_pNavigationCom);
	//}
	
	if (CLIMBCUBE_END == m_eSTATE_CUBE)
	{
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * fTimeDelta * 3.0f);

		m_eSTATE_CUBE = CLIMBCUBE_READY;
	}

	if (CLIMBCUBE_READY == m_eSTATE_CUBE)
	{
		m_pModelCom->SetUp_BodyAnimation(55); // Hang_Ready
		m_pModelCom->SetUp_WaistAnimation(55);

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		pGameInstance->PlaySoundW(L"Climb.wav", SOUND_PLAYER, 0.2f);
		Safe_Release(pGameInstance);

		if (m_bBodyAnimFinished || m_bWaistAnimFinished)
		{
			++m_iNumUnderObject;

			m_eSTATE_CUBE = CLIMBCUBE_STAND;
		}
	}

	else if (CLIMBCUBE_STAND == m_eSTATE_CUBE)
	{
		m_pModelCom->SetUp_BodyAnimation(57); // Climb 
		m_pModelCom->SetUp_WaistAnimation(57);

		if (m_bBodyAnimFinished || m_bWaistAnimFinished)
		{
			m_bUnderCollision = true;
			m_PlayerState = PLAYER_IDLE;
			m_eSTATE_CUBE = CLIMBCUBE_END;
			m_fClimbTimeAcc = 0.f;

			//m_fChangeAnimCount += fTimeDelta;
			//if (1.f < m_fChangeAnimCount)
			//{
			//	m_pModelCom->SetUp_BodyAnimation(57); // Climb 
			//	m_pModelCom->SetUp_WaistAnimation(57);
			//}
		}
	}

	//m_pModelCom->SetUp_BodyAnimation(52); // Hang_Idle
	//m_pModelCom->SetUp_WaistAnimation(52);

	//m_pModelCom->SetUp_BodyAnimation(57); // Climb 
	//m_pModelCom->SetUp_WaistAnimation(57);

}

void CPlayer::Pattern_Climb_High(_float fTimeDelta)
{
	if (PLAYER_CLIMB_HiGH == m_PlayerState)
	{
		m_pModelCom->SetUp_BodyAnimation(55); // Hang_Ready
		m_pModelCom->SetUp_WaistAnimation(55);


		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		pGameInstance->PlaySoundW(L"Climb.wav", SOUND_PLAYER, 0.2f);
		Safe_Release(pGameInstance);

		if (m_bBodyAnimFinished || m_bWaistAnimFinished)
		{
			m_PlayerState = PLAYER_CLIMB_HiGH_END;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 1.5f, 0.f, 0.f));

		}
	}

	else if (PLAYER_CLIMB_HiGH_END == m_PlayerState)
	{
		m_pModelCom->SetUp_BodyAnimation(57); // Climb 
		m_pModelCom->SetUp_WaistAnimation(57);

		if (m_bBodyAnimFinished || m_bWaistAnimFinished)
		{
			m_PlayerState = PLAYER_IDLE;
			m_bClimb = false;
			m_iNumUnderObject = 0;
			m_pNavigationCom->Find_CellOption(0);

			/*_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);*/

			m_fPatternAcc = 0.f;
		}
	}


	//m_pModelCom->SetUp_BodyAnimation(52); // Hang_Idle
	//m_pModelCom->SetUp_WaistAnimation(52);

	//m_pModelCom->SetUp_BodyAnimation(57); // Climb 
	//m_pModelCom->SetUp_WaistAnimation(57);
}

void CPlayer::Pattern_Grab_Throw(_float fTimeDelta)
{

	//m_pModelCom->SetUp_BodyAnimation(49); // Throw
	//m_pModelCom->SetUp_WaistAnimation(49);

	//m_pModelCom->SetUp_BodyAnimation(50); // Grab_Ready
	//m_pModelCom->SetUp_WaistAnimation(50);

	m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(3.f, XMConvertToRadians(60.f)));

	if (GRAB_END == m_eSTATE_GRAB)
	{
		m_eSTATE_GRAB = GRAB_READY;
	}

	if (GRAB_READY == m_eSTATE_GRAB)
	{
		m_pModelCom->SetUp_BodyAnimation(50); // Grab_Ready
		m_pModelCom->SetUp_WaistAnimation(50);

		if (m_bBodyAnimFinished || m_bWaistAnimFinished)
		{
			m_eSTATE_GRAB = GRAB_IDLE;
		}


		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		pGameInstance->PlaySoundW(L"Grab.wav", SOUND_PLAYER, 0.2f);
		Safe_Release(pGameInstance);

	}

	else if (GRAB_IDLE == m_eSTATE_GRAB)
	{

		m_pModelCom->SetUp_BodyAnimation(51); // Grab_Ing
		m_pModelCom->SetUp_WaistAnimation(51); // Ing는 이동도 해야되고 상하체 분리도 되야되서 InputKey로 가는것이 맞을 듯?

	

		if (GetKeyState('A') & 0x8000)
		{

			if (GetKeyState('W') & 0x8000)
			{
				if (DIR_7 != m_eDir && DIR_LEFT == m_eDir)
				{
					m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.0f));
					m_eDir = DIR_7;
				}
				else if (DIR_7 != m_eDir && DIR_FRONT == m_eDir)
				{
					m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), -XMConvertToRadians(45.0f));
					m_eDir = DIR_7;
				}
				m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			}

			else if (GetKeyState('S') & 0x8000)
			{
				if (DIR_1 != m_eDir && DIR_LEFT == m_eDir)
				{
					m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), -XMConvertToRadians(45.0f));
					m_eDir = DIR_1;
				}

				else if (DIR_1 != m_eDir && DIR_BACK == m_eDir)
				{
					m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.0f));
					m_eDir = DIR_1;
				}
				m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			}

			else
			{
				m_eDir = DIR_LEFT;
				m_pTransformCom->LookAtAxis(XMLoadFloat4(&m_Axis_Side) * -1.f);
				m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			}


			m_pModelCom->SetUp_BodyAnimation(51);
			m_pModelCom->SetUp_WaistAnimation(87);
			m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f)));
			m_fBodyAnimSpeed = 1.f;
			m_fWaistAnimSpeed = 1.f;

			if (PLAYER_WEAR == m_PlayerState)
			{
				m_fWaistAnimSpeed = 2.f;
			}
		}

		else if (GetKeyState('D') & 0x8000)
		{

			if (GetKeyState('W') & 0x8000)
			{
				if (DIR_9 != m_eDir && DIR_RIGHT == m_eDir)
				{
					m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), -XMConvertToRadians(45.0f));
					m_eDir = DIR_9;
				}

				else if (DIR_9 != m_eDir && DIR_FRONT == m_eDir)
				{
					m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.0f));
					m_eDir = DIR_9;
				}
				m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			}

			else if (GetKeyState('S') & 0x8000)
			{
				if (DIR_3 != m_eDir && DIR_RIGHT == m_eDir)
				{
					m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.0f));
					m_eDir = DIR_3;
				}

				if (DIR_3 != m_eDir && DIR_BACK == m_eDir)
				{
					m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), -XMConvertToRadians(45.0f));
					m_eDir = DIR_3;
				}
				m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			}

			else
			{
				m_eDir = DIR_RIGHT;
				m_pTransformCom->LookAtAxis(XMLoadFloat4(&m_Axis_Side));
				m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			}


			m_pModelCom->SetUp_WaistAnimation(87);
			m_pModelCom->SetUp_BodyAnimation(51);
			m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f)));
			m_fBodyAnimSpeed = 1.f;
			m_fWaistAnimSpeed = 2.f;


		}

		else if (GetKeyState('W') & 0x8000)
		{

			m_eDir = DIR_FRONT;
			m_pTransformCom->LookAtAxis(XMLoadFloat4(&m_Axis_Front));
			/*	if (!m_bCollision && !m_bUnderCollision)
			{*/
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			//}

			
			m_pModelCom->SetUp_WaistAnimation(87);
			m_pModelCom->SetUp_BodyAnimation(51);
			m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f)));
			m_fBodyAnimSpeed = 1.f;
			m_fWaistAnimSpeed = 2.f;


		}

		else if (GetKeyState('S') & 0x8000)
		{
			m_eDir = DIR_BACK;
			m_pTransformCom->LookAtAxis(XMLoadFloat4(&m_Axis_Front) * -1.f);
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);

			m_pModelCom->SetUp_WaistAnimation(87); //87
			m_pModelCom->SetUp_BodyAnimation(51);

			m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f)));
			m_fBodyAnimSpeed = 1.f;
			m_fWaistAnimSpeed = 2.f;


		}

		if (0.1f <= m_fTickAcc && GetKeyState(VK_LBUTTON) & 0x8000)
		{
			m_eSTATE_GRAB = GRAB_THROW;
		}

	}

	else if (GRAB_THROW == m_eSTATE_GRAB)
	{

		m_pModelCom->SetUp_BodyAnimation(49); // Throw
		m_pModelCom->SetUp_WaistAnimation(49);

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		pGameInstance->PlaySoundW(L"Throw.wav", SOUND_PLAYER, 0.2f);
		Safe_Release(pGameInstance);

		m_fBodyAnimSpeed = 1.f;
		m_fWaistAnimSpeed = 1.f;

		if (m_bBodyAnimFinished || m_bWaistAnimFinished)
		{
			m_PlayerState = PLAYER_IDLE;
			m_eSTATE_GRAB = GRAB_END;
		}
	}

}

void CPlayer::Pattern_Damaged_Magma(_float fTimeDelta)
{
	m_iPlayerHp = 8;
	//m_iPlayerHpAcc = -4;
	Set_HpUI();
}

void CPlayer::Pattern_Ladder(_float fTimeDelta)
{
	m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(2.f, XMConvertToRadians(60.f)));

	if (LADDER_END == m_eState_Ladder)
	{
		m_eState_Ladder = LADDER_READY;
	}

	if (LADDER_READY == m_eState_Ladder)
	{
		m_pModelCom->SetUp_BodyAnimation(69);
		m_pModelCom->SetUp_WaistAnimation(69);
		
		if (m_bBodyAnimFinished || m_bWaistAnimFinished)
		{
			m_eState_Ladder = LADDER_UP_LEFT;
		}
	}

	else if (LADDER_UP_LEFT == m_eState_Ladder)
	{
		m_pModelCom->SetUp_BodyAnimation(65);
		m_pModelCom->SetUp_WaistAnimation(65);

		if (GetKeyState('W') & 0x8000)
		{
			m_fBodyAnimSpeed = 0.5f;
			m_fWaistAnimSpeed = 0.5f;
			//m_pTransformCom->Go_Axis(static_cast<_double>(fTimeDelta), m_pTransformCom->Get_State(CTransform::STATE_UP));
		}

		else
		{
			m_fBodyAnimSpeed = 0.f;
			m_fWaistAnimSpeed = 0.f;
		}

		if (m_bBodyAnimFinished || m_bWaistAnimFinished)
		{
			m_eState_Ladder = LADDER_UP_RIGHT;
			//m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.5f, 0.f, 0.f));
		}

	}

	else if (LADDER_UP_RIGHT == m_eState_Ladder)
	{
		m_pModelCom->SetUp_BodyAnimation(66);
		m_pModelCom->SetUp_WaistAnimation(66);

		if (GetKeyState('W') & 0x8000)
		{
			m_fBodyAnimSpeed = 0.5f;
			m_fWaistAnimSpeed = 0.5f;
			
		}

		else
		{
			m_fBodyAnimSpeed = 0.f;
			m_fWaistAnimSpeed = 0.f;
		}

		if (m_bBodyAnimFinished || m_bWaistAnimFinished)
		{
			m_eState_Ladder = LADDER_UP_LEFT;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.7f, 0.f, 0.f));
		}

	}

	else if (LADDER_FINISH == m_eState_Ladder)
	{

	}
}

void CPlayer::Pattern_GetItem(_float fTimeDelta)
{
	m_fBodyAnimSpeed = 0.5f;
	m_fWaistAnimSpeed = 0.5f;

	if (FAILED(SetUp_OnTerrain()))
	{
		return;
	}

	if (ITEM_END == m_eState_GetItem)
	{
		
		switch (m_PlayerState)
		{
		case PLAYER_GETITEM:
			m_eState_GetItem = ITEM_OPEN;
			break;
		case PLAYER_GETITEM_SHORT:
			m_eState_GetItem = ITEM_OPEN_SHORT;
			break;
		default:
			break;
		}
	}

	else if (ITEM_OPEN == m_eState_GetItem || ITEM_OPEN_SHORT == m_eState_GetItem)
	{
		switch (m_PlayerState)
		{
		case PLAYER_GETITEM:
			m_pModelCom->SetUp_BodyAnimation(30);
			m_pModelCom->SetUp_WaistAnimation(30);

			if (m_bWaistAnimFinished || m_bBodyAnimFinished)
			{
				m_pModelCom->Init_Animation();
				m_eState_GetItem = ITEM_GET;
			}
			break;

		case PLAYER_GETITEM_SHORT:
			m_pModelCom->SetUp_BodyAnimation(31);
			m_pModelCom->SetUp_WaistAnimation(31);

			if (m_bWaistAnimFinished || m_bBodyAnimFinished)
			{
				m_pModelCom->Init_Animation();
				m_eState_GetItem = ITEM_GET;
			}
			break;
			
		default:
			break;
		}

		//if (m_bWaistAnimFinished || m_bBodyAnimFinished)
		//{
		//	m_pModelCom->Init_Animation();
		//	m_eState_GetItem = ITEM_GET;
		//}
	}

	else if (ITEM_GET == m_eState_GetItem)
	{
		m_pModelCom->SetUp_BodyAnimation(29);
		m_pModelCom->SetUp_WaistAnimation(29);
		if (0 == m_fGetItemAnimAcc)
		{
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);

			pGameInstance->PlaySoundW(L"Get_Item.wav", SOUND_PLAYER, 0.2f);
			m_fGetItemAnimAcc += fTimeDelta;

			Safe_Release(pGameInstance);
		}
		
		if (m_bWaistAnimFinished || m_bBodyAnimFinished)
		{
			/*m_fBodyAnimSpeed = 0.f;
			m_fWaistAnimSpeed = 0.f;*/

			m_fGetItemAnimAcc += fTimeDelta;

			if (3.f < m_fGetItemAnimAcc)
			{
				m_pModelCom->Init_Animation();
				m_eState_GetItem = ITEM_END;
				m_PlayerState = PLAYER_IDLE;
				m_bCanOpenBox = false;
				m_fGetItemAnimAcc = 0.f;

				++m_iNumKey;
				cout << "+key" << endl;
			}
		}
	}
}

void CPlayer::Pattern_Boko(_float fTimeDelta)
{

	//m_pModelCom->SetUp_BodyAnimation(49); // Throw
	//m_pModelCom->SetUp_WaistAnimation(49);

	//m_pModelCom->SetUp_BodyAnimation(50); // Grab_Ready
	//m_pModelCom->SetUp_WaistAnimation(50);

	if (BOKO_END == m_eState_Boko)
	{
		m_eState_Boko = BOKO_IDLE;
	}

	else if (BOKO_IDLE == m_eState_Boko)
	{

		m_pModelCom->SetUp_BodyAnimation(85); // Grab_Ing
		m_pModelCom->SetUp_WaistAnimation(85); // Ing는 이동도 해야되고 상하체 분리도 되야되서 InputKey로 가는것이 맞을 듯?



		if (GetKeyState('A') & 0x8000)
		{

			if (GetKeyState('W') & 0x8000)
			{
				if (DIR_7 != m_eDir && DIR_LEFT == m_eDir)
				{
					m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.0f));
					m_eDir = DIR_7;
				}
				else if (DIR_7 != m_eDir && DIR_FRONT == m_eDir)
				{
					m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), -XMConvertToRadians(45.0f));
					m_eDir = DIR_7;
				}
				m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			}

			else if (GetKeyState('S') & 0x8000)
			{
				if (DIR_1 != m_eDir && DIR_LEFT == m_eDir)
				{
					m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), -XMConvertToRadians(45.0f));
					m_eDir = DIR_1;
				}

				else if (DIR_1 != m_eDir && DIR_BACK == m_eDir)
				{
					m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.0f));
					m_eDir = DIR_1;
				}
				m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			}

			else
			{
				m_eDir = DIR_LEFT;
				m_pTransformCom->LookAtAxis(XMLoadFloat4(&m_Axis_Side) * -1.f);
				m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			}


			m_pModelCom->SetUp_BodyAnimation(42);
			m_pModelCom->SetUp_WaistAnimation(44);
			m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f)));
			m_fBodyAnimSpeed = 1.f;
			m_fWaistAnimSpeed = 1.f;

			if (PLAYER_WEAR == m_PlayerState)
			{
				m_fWaistAnimSpeed = 2.f;
			}
		}

		else if (GetKeyState('D') & 0x8000)
		{

			if (GetKeyState('W') & 0x8000)
			{
				if (DIR_9 != m_eDir && DIR_RIGHT == m_eDir)
				{
					m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), -XMConvertToRadians(45.0f));
					m_eDir = DIR_9;
				}

				else if (DIR_9 != m_eDir && DIR_FRONT == m_eDir)
				{
					m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.0f));
					m_eDir = DIR_9;
				}
				m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			}

			else if (GetKeyState('S') & 0x8000)
			{
				if (DIR_3 != m_eDir && DIR_RIGHT == m_eDir)
				{
					m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.0f));
					m_eDir = DIR_3;
				}

				if (DIR_3 != m_eDir && DIR_BACK == m_eDir)
				{
					m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), -XMConvertToRadians(45.0f));
					m_eDir = DIR_3;
				}
				m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			}

			else
			{
				m_eDir = DIR_RIGHT;
				m_pTransformCom->LookAtAxis(XMLoadFloat4(&m_Axis_Side));
				m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			}


			m_pModelCom->SetUp_BodyAnimation(42);
			m_pModelCom->SetUp_WaistAnimation(44);
			m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f)));
			m_fBodyAnimSpeed = 1.f;
			m_fWaistAnimSpeed = 2.f;


		}

		else if (GetKeyState('W') & 0x8000)
		{

			m_eDir = DIR_FRONT;
			m_pTransformCom->LookAtAxis(XMLoadFloat4(&m_Axis_Front));
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			


			m_pModelCom->SetUp_BodyAnimation(42);
			m_pModelCom->SetUp_WaistAnimation(44);
			m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f)));
			m_fBodyAnimSpeed = 1.f;
			m_fWaistAnimSpeed = 2.f;


		}

		else if (GetKeyState('S') & 0x8000)
		{
			m_eDir = DIR_BACK;
			m_pTransformCom->LookAtAxis(XMLoadFloat4(&m_Axis_Front) * -1.f);
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);


			m_pModelCom->SetUp_BodyAnimation(42);
			m_pModelCom->SetUp_WaistAnimation(44);

			m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(5.f, XMConvertToRadians(60.f)));
			m_fBodyAnimSpeed = 1.f;
			m_fWaistAnimSpeed = 2.f;


		}

		if (0.1f <= m_fTickAcc && GetKeyState(VK_LBUTTON) & 0x8000)
		{
			m_eState_Boko = BOKO_ATTACK;
		}

		if (GetKeyState(VK_SPACE) & 0x8000)
		{
			m_eState_Boko = BOKO_THROW;
		}


		//if (GetKeyState(VK_SHIFT) & 0x8000)
		//{
		//	m_pModelCom->SetUp_WaistAnimation(87);
		//	m_pModelCom->SetUp_BodyAnimation(51);

		//	m_pTransformCom->Set_TransformDesc(CTransform::TRANSFORMDESC(2.f, XMConvertToRadians(60.f)));
		//	m_fBodyAnimSpeed = 1.f;
		//	m_fWaistAnimSpeed = 2.f;
		//}
	}

	else if (BOKO_ATTACK == m_eState_Boko)
	{

		m_pModelCom->SetUp_BodyAnimation(2); // Throw
		m_pModelCom->SetUp_WaistAnimation(2);

		m_fBodyAnimSpeed = 1.f;
		m_fWaistAnimSpeed = 1.f;

		if (m_bBodyAnimFinished || m_bWaistAnimFinished)
		{
			m_eState_Boko = BOKO_IDLE;
		}
	}

	else if (BOKO_THROW == m_eState_Boko)
	{
		m_pModelCom->SetUp_BodyAnimation(17); // Throw
		m_pModelCom->SetUp_WaistAnimation(17);

		m_fBodyAnimSpeed = 1.f;
		m_fWaistAnimSpeed = 1.f;

		if (m_bBodyAnimFinished || m_bWaistAnimFinished)
		{
			m_PlayerState = PLAYER_IDLE;
			m_eState_Boko = BOKO_END;
		}
	}
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc)
{
	CPlayer*		pInstance = new CPlayer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CPlayer"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*		pInstance = new CPlayer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CPlayer"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (auto& pUI_PlayerHp : m_PlayerHps)
	{
		Safe_Release(pUI_PlayerHp);
	}
	m_PlayerHps.clear();

	if (nullptr != m_pSeamLess_Loader)
	{
		Safe_Release(m_pSeamLess_Loader);
	}

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pTarget);
	Safe_Release(m_pCollider_Manager);
	Safe_Release(m_pSPHERE);
	Safe_Release(m_pOBB);
	//Safe_Release(m_pAABB);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
