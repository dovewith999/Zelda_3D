#include "stdafx.h"
#include "..\Public\Terrain.h"
#include "GameInstance.h"
#include "Cell.h"
#include "Tool.h"
#include "Point_Sphere.h"

#define FIRE_BRAZIER	 0
#define DOOR			 1
#define BOMB			 2
#define BOX				 3
#define CUBE			 4 
#define ROCK			 5
#define MAGMA			 6
#define WATERPOT		 7
#define WOODPLANKS	     8
#define BRIDGE			 9
#define LOCKDOOR		10
#define BOKOBLIN		11
#define TRIGGER_LADDER	12
#define TRIGGER_WALL	13
#define TRIGGER_HANG	14
#define	WOODDOOR		15
#define STONEBRIDGE		16
#define TRESUREBOXSHORT 17
#define	MagmaZet		18
#define TRIGGERBOSS		19

CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs)
{

}

HRESULT CTerrain::NativeConstruct_Prototype(const CTransform::TRANSFORMDESC& TransformDesc)
{
	if (FAILED(__super::NativeConstruct_Prototype(TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (FAILED(Create_FilterTexture()))
		return E_FAIL;
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-50.f, 0.f, -20.f, 1.f));

	for (auto& pCell : m_pNavigationCom->Get_Cells())
	{
		m_Cells.push_back(pCell);
		Safe_AddRef(pCell);
	}

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pGameInstance);

	CTool* pTool = CTool::Get_Instance();
	Safe_AddRef(pTool);

	_float4 vPos;
	// 기존에 저장했던 셀을 저장 리스트에 넣어서 다시 저장될 수 있도록 해주는 코드

	for (auto& pCell : m_Cells)
	{
		if (nullptr != pCell)
		{
			vPos = { pCell->Get_Point(CCell::POINT_A)->x, pCell->Get_Point(CCell::POINT_A)->y,pCell->Get_Point(CCell::POINT_A)->z, 1.f };

			vPos = { pCell->Get_Point(CCell::POINT_B)->x, pCell->Get_Point(CCell::POINT_B)->y,pCell->Get_Point(CCell::POINT_B)->z, 1.f };

			vPos = { pCell->Get_Point(CCell::POINT_C)->x, pCell->Get_Point(CCell::POINT_C)->y,pCell->Get_Point(CCell::POINT_C)->z, 1.f };

			pTool->Push_Back(*(pCell->Get_Point(CCell::POINT_A)));
			pTool->Push_Back(*(pCell->Get_Point(CCell::POINT_B)));
			pTool->Push_Back(*(pCell->Get_Point(CCell::POINT_C)));
			pTool->Push_Back_Option(pCell->Get_Option());
		}
	}

	Safe_Release(pTool);
	Safe_Release(pGameInstance);

	return S_OK;
}

void CTerrain::Tick(_float fTimeDelta)
{
}

void CTerrain::Late_Tick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderList(CRenderer::RENDER_NONALPHABLEND, this);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pGameInstance);

	CTool* pTool = CTool::Get_Instance();
	Safe_AddRef(pTool);
	
	_float4 vPos;
	_float3 vPointPos;
	m_fTimeAcc += fTimeDelta;

	_float4x4 WorldMatrix;
	ZeroMemory(&WorldMatrix, sizeof(_float4x4));
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	if (0.3f < m_fTimeAcc && GetKeyState(VK_CONTROL) & 0x8000 && GetKeyState(VK_LBUTTON) & 0x8000)
	{

		if (pGameInstance->Picking(m_pVIBufferCom, m_pTransformCom, &vPos))
		{
			if (pTool->Get_NavigationMode())
			{
				// 피킹 보정 코드
				vPos.x = pTool->Compare_Length(_float3(vPos.x, vPos.y, vPos.z)).x;
				vPos.y = pTool->Compare_Length(_float3(vPos.x, vPos.y, vPos.z)).y;
				vPos.z = pTool->Compare_Length(_float3(vPos.x, vPos.y, vPos.z)).z;

				#pragma region ==중략==
				m_vBrushPos = vPos;


				CPoint_Sphere*  pSphere = dynamic_cast<CPoint_Sphere*>(pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Point_Sphere")));
				if (nullptr == pSphere)
				{
					if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Point_Sphere"), TEXT("Prototype_GameObject_Point_Sphere"), &vPos)))
					{
						return;
					}

					pSphere = dynamic_cast<CPoint_Sphere*>(pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Point_Sphere")));
				}
#pragma endregion

				m_vPoints[m_iNumPoint] = _float3(vPos.x, vPos.y, vPos.z);

				#pragma region ==중략==
				pTool->Push_Back(m_vPoints[m_iNumPoint]);
				_float3 vLastPos = pTool->Get_FinalPos();
				_float4 vLastPos4 = { vLastPos.x, vLastPos.y, vLastPos.z, 1.f };
				pSphere->Set_Pos(vLastPos4);
#pragma endregion

				if (2 == m_iNumPoint) // 피킹한 점이 3개가 되면 그 3개의 포인트르 기준으로 셀을 만듦
				{
					CCell* pCell = CCell::Create(m_pDevice, m_pDeviceContext, m_vPoints, m_Cells.size(), static_cast<CCell::OPTION>(pTool->Get_CellOption()));
					m_Cells.push_back(pCell);

					m_iNumPoint = 0;

					pTool->Push_Back_Option(pTool->Get_CellOption());
				}

				else // 피킹한 점이 3개가 안되었으면 3개가 될 때까지 배열에 저장
				{
					++m_iNumPoint;
				}

				m_fTimeAcc = 0.f;
			}

			else if (pTool->Get_ObjectMode())
			{
				m_iObjectID = pTool->Get_ObjectID();

				WorldMatrix._41 = vPos.x;
				WorldMatrix._42 = vPos.y;
				WorldMatrix._43 = vPos.z;

				switch (m_iObjectID)
				{
				case FIRE_BRAZIER :
					if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_FireBrazier"), &WorldMatrix)))
					{
						return;
					}
					pTool->Push_Back_ObjectID(FIRE_BRAZIER);
					break;

				case DOOR:
					if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_Door"), &WorldMatrix)))
					{
						return;
					}
					pTool->Push_Back_ObjectID(DOOR);
					break;

				case BOMB:
					if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_Bomb"), &WorldMatrix)))
					{
						return;
					}
					pTool->Push_Back_ObjectID(BOMB);
					break;

					break;

				case BOX:
					if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_TresureBox"), &WorldMatrix)))
					{
						return;
					}
					pTool->Push_Back_ObjectID(BOX);
					break;

				case CUBE:
					if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_Cube"), &WorldMatrix)))
					{
						return;
					}
					pTool->Push_Back_ObjectID(CUBE);
					break;

				case ROCK:
					if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_Rock"), &WorldMatrix)))
					{
						return;
					}
					pTool->Push_Back_ObjectID(ROCK);
					break;
					
				case MAGMA:
					if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Magma"), &WorldMatrix)))
					{
						return;
					}
					pTool->Push_Back_ObjectID(MAGMA);
					break;

				case WATERPOT:
					if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_WaterPot"), &WorldMatrix)))
					{
						return;
					}
					pTool->Push_Back_ObjectID(WATERPOT);
					break;

				case WOODPLANKS:
					if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_WoodPlanks"), &WorldMatrix)))
					{
						return;
					}
					pTool->Push_Back_ObjectID(WOODPLANKS);
					break;

				case BRIDGE:
					if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_Bridge"), &WorldMatrix)))
					{
						return;
					}
					pTool->Push_Back_ObjectID(BRIDGE);
					break;

				case LOCKDOOR:
					if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_Lock"), &WorldMatrix)))
					{
						return;
					}
					pTool->Push_Back_ObjectID(LOCKDOOR);
					break;

				case BOKOBLIN:
					if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Monster"), &WorldMatrix)))
					{
						return;
					}
					pTool->Push_Back_ObjectID(BOKOBLIN);
					break;

				case TRIGGER_LADDER:
					if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Trigger_Ladder"), &WorldMatrix)))
					{
						return;
					}
					pTool->Push_Back_ObjectID(TRIGGER_LADDER);
					break;
					
				case TRIGGER_WALL:

					break;

				case TRIGGER_HANG:

					break;

				case WOODDOOR:
					if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_WoodDoor"), &WorldMatrix)))
					{
						return;
					}
					pTool->Push_Back_ObjectID(WOODDOOR);
					break;

				case STONEBRIDGE:
					if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_StoneBridge"), &WorldMatrix)))
					{
						return;
					}
					pTool->Push_Back_ObjectID(STONEBRIDGE);
					break;
				case TRESUREBOXSHORT:
					if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Object_TresureBoxShort"), &WorldMatrix)))
					{
						return;
					}
					 pTool->Push_Back_ObjectID(TRESUREBOXSHORT);
					break;

				case MagmaZet :
					if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_MagmaZet"), &WorldMatrix)))
					{
						return;
					}
					pTool->Push_Back_ObjectID(MagmaZet);
					break;

				case TRIGGERBOSS:
					if (FAILED(pGameInstance->Add_Layer(LEVEL_TOOL, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Trigger_Boss"), &WorldMatrix)))
					{
						return;
					}
					pTool->Push_Back_ObjectID(TRIGGERBOSS);
					break;

				}

				pTool->Push_Back_WorldMatrix(WorldMatrix);
			
				m_pTarget = pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_Object"));

				m_fTimeAcc = 0.f;
				m_fRotationX = 0.f;
				m_fRotationY = 0.f;
				m_fRotationZ = 0.f;

				m_fPositionX_Plus = 0.f;
				m_fPositionY_Plus = 0.f;
				m_fPositionZ_Plus = 0.f;
			}
		}

	}

	//높이를 다르게 하여 피킹을 하기 위한 코드
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), pTool->Get_Height()));
	
	// 오브젝트 회전 수치를 툴과 연동하기 위한 코드

	if (m_fRotationX != pTool->Get_Rotation(0))
	{
		if (!m_bRotation)
		{
			m_bRotation = true;
		}

		if (m_fRotationX > pTool->Get_Rotation(0))
		{

			m_pTarget->Set_Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), -1.f);
			--m_fRotationX;
		}

		else if (m_fRotationX < pTool->Get_Rotation(0))
		{

			m_pTarget->Set_Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), 1.f);
			++m_fRotationX;
		}
	}

	else if (m_bRotation && m_fRotationX == pTool->Get_Rotation(0))
	{
		XMStoreFloat4x4(&WorldMatrix, m_pTarget->Get_WorldMatrix());
		pTool->Pop_Back_And_Push_Back_WorldMatrix(WorldMatrix);
		m_bRotation = false;
	}

	if (m_fRotationY != pTool->Get_Rotation(1))
	{

		if (!m_bRotation)
		{
			m_bRotation = true;
		}

		if (m_fRotationY > pTool->Get_Rotation(1))
		{
			m_pTarget->Set_Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), -1.f);
			--m_fRotationY;
		}
		else if (m_fRotationY < pTool->Get_Rotation(1))
		{
			m_pTarget->Set_Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 1.f);
			++m_fRotationY;
		}

	}

	else if (m_bRotation && m_fRotationY == pTool->Get_Rotation(1))
	{

		XMStoreFloat4x4(&WorldMatrix, m_pTarget->Get_WorldMatrix());
		pTool->Pop_Back_And_Push_Back_WorldMatrix(WorldMatrix);
		m_bRotation = false;
	}

	if (m_fRotationZ != pTool->Get_Rotation(2))
	{
	
		if (!m_bRotation)
		{
			m_bRotation = true;
		}

		if (m_fRotationZ > pTool->Get_Rotation(2))
		{
			m_pTarget->Set_Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), -1.f);
			--m_fRotationZ;
		}

		else if (m_fRotationZ < pTool->Get_Rotation(2))
		{
			m_pTarget->Set_Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), 1.f);
			++m_fRotationZ;
		}
	}

	else if (m_bRotation && m_fRotationZ == pTool->Get_Rotation(2))
	{

		XMStoreFloat4x4(&WorldMatrix, m_pTarget->Get_WorldMatrix());
		pTool->Pop_Back_And_Push_Back_WorldMatrix(WorldMatrix);
		m_bRotation = false;
	}

	//오브젝트 스케일을 툴과 연동하기 위한 함수

	if (nullptr != m_pTarget)
	{
		Set_Scale(m_pTarget);
	}

	// 생성된 오브젝트의 포지션에 변화를 주기위한 함수
	if (nullptr != m_pTarget)
	{
		Set_Position(m_pTarget);
	}

	Safe_Release(pTool);
	Safe_Release(pGameInstance);
}

HRESULT CTerrain::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_iPassIndex)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;	

	//_float4x4 WorldMatrix;
	//XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	//m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	//m_pShader->Set_RawValue("g_vColor", &_float4(0.f, 1.f, 0.f, 1.f), sizeof(_float4));

	for (auto& pCell : m_Cells)
	{
		if (nullptr != pCell)
		{
			pCell->Render(m_pShader);
		}
	}

//#ifdef _DEBUG
//	m_pNavigationCom->Render();
//#endif // _DEBUG

	return S_OK;
}

void CTerrain::Set_Scale(CGameObject * pTarget)
{
	_float4x4 WorldMatrix;

	CTool* pTool = CTool::Get_Instance();
	Safe_AddRef(pTool);
	
	pTarget->Set_Scale(pTool->Get_Scale());

	XMStoreFloat4x4(&WorldMatrix, pTarget->Get_WorldMatrix());
	pTool->Pop_Back_And_Push_Back_WorldMatrix(WorldMatrix);

	Safe_Release(pTool);
}

void CTerrain::Set_Position(CGameObject * pTarget)
{

	_float4x4 WorldMatrix;

	CTool* pTool = CTool::Get_Instance();
	Safe_AddRef(pTool);

	if (m_fPositionX_Plus != pTool->Get_Position(0))
	{
		if (m_fPositionX_Plus < pTool->Get_Position(0))
		{
			pTarget->Set_Position(pTarget->Get_Position() + XMVectorSet(0.1f, 0.f, 0.f, 0.f));
			m_fPositionX_Plus += 0.1f;
		}

		else if (m_fPositionX_Plus > pTool->Get_Position(0))
		{
			pTarget->Set_Position(pTarget->Get_Position() + XMVectorSet(-0.1f, 0.f, 0.f, 0.f));
			m_fPositionX_Plus -= 0.1f;
		}

	}

	else
	{

		XMStoreFloat4x4(&WorldMatrix, pTarget->Get_WorldMatrix());
		pTool->Pop_Back_And_Push_Back_WorldMatrix(WorldMatrix);
	}

	if (m_fPositionY_Plus != pTool->Get_Position(1))
	{
		if (m_fPositionY_Plus < pTool->Get_Position(1))
		{
			pTarget->Set_Position(pTarget->Get_Position() + XMVectorSet( 0.f, 0.1f, 0.f, 0.f));
			m_fPositionY_Plus += 0.1f;
		}

		else if (m_fPositionY_Plus > pTool->Get_Position(1))
		{
			pTarget->Set_Position(pTarget->Get_Position() + XMVectorSet( 0.f, -0.1f, 0.f, 0.f));
			m_fPositionY_Plus -= 0.1f;
		}

	}

	else
	{

		XMStoreFloat4x4(&WorldMatrix, pTarget->Get_WorldMatrix());
		pTool->Pop_Back_And_Push_Back_WorldMatrix(WorldMatrix);

	}

	if (m_fPositionZ_Plus != pTool->Get_Position(2))
	{
		if (m_fPositionZ_Plus < pTool->Get_Position(2))
		{
			pTarget->Set_Position(pTarget->Get_Position() + XMVectorSet( 0.f, 0.f, 0.1f, 0.f));
			m_fPositionZ_Plus += 0.1f;
		}

		else if (m_fPositionZ_Plus > pTool->Get_Position(2))
		{
			pTarget->Set_Position(pTarget->Get_Position() + XMVectorSet( 0.f, 0.f,-0.1f, 0.f));
			m_fPositionZ_Plus -= 0.1f;
		}

	}

	else
	{

		XMStoreFloat4x4(&WorldMatrix, pTarget->Get_WorldMatrix());
		pTool->Pop_Back_And_Push_Back_WorldMatrix(WorldMatrix);

	}
	
	
	Safe_Release(pTool);
}

void CTerrain::Pop_Back_Object()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Release_GameObject(LEVEL_TOOL, TEXT("Layer_Object"));
	Safe_Release(pGameInstance);
}

HRESULT CTerrain::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;	

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom[TYPE_DIFFUSE])))
		return E_FAIL;

	/* For.Com_Filter */
	if (FAILED(__super::Add_Components(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Filter"), TEXT("Com_Filter"), (CComponent**)&m_pTextureCom[TYPE_FILTER])))
		return E_FAIL;

	/* For.Com_Brush */
	if (FAILED(__super::Add_Components(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Brush"), TEXT("Com_Brush"), (CComponent**)&m_pTextureCom[TYPE_BRUSH])))
		return E_FAIL;

	/* For.Com_Navigation*/
	if (FAILED(__super::Add_Components(LEVEL_TOOL, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom)))
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Navigation.hlsl"), VTXCOL_DECLARATION::Elements, VTXCOL_DECLARATION::iNumElement);

	return S_OK;
}

HRESULT CTerrain::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPositionFloat4(), sizeof(_float4));
	m_pShaderCom->Set_RawValue("g_vBrushPos", &m_vBrushPos, sizeof(_float4));

	const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (pLightDesc->eType == tagLightDesc::TYPE_POINT)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_vLightPos", &pLightDesc->vPosition, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRange", &pLightDesc->fRange, sizeof(_float))))
			return E_FAIL;

		m_iPassIndex = 1;
	}

	else
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
			return E_FAIL;

		m_iPassIndex = 0;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->Bind_OnShader(m_pShaderCom, "g_SourDiffTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->Bind_OnShader(m_pShaderCom, "g_DestDiffTexture", 1)))
		return E_FAIL;	
	if (FAILED(m_pTextureCom[TYPE_FILTER]->Bind_OnShader(m_pShaderCom, "g_FilterTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom[TYPE_BRUSH]->Bind_OnShader(m_pShaderCom, "g_BrushTexture", 0)))
		return E_FAIL;

	// m_pShaderCom->Set_SRV("g_FilterTexture", m_pFilterSRV);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CTerrain::Create_FilterTexture()
{
	ID3D11Texture2D*			pTexture = nullptr;

	D3D11_TEXTURE2D_DESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	/*	
	UINT CPUAccessFlags;
	UINT MiscFlags;
	*/

	TextureDesc.Width = 1025;
	TextureDesc.Height = 1025;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA		SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_SUBRESOURCE_DATA));

	_uint*			pPixel = new _uint[TextureDesc.Width * TextureDesc.Height];

	for (_uint i = 0; i < TextureDesc.Height; ++i)
	{
		for (_uint j = 0; j < TextureDesc.Width; ++j)
		{
			_uint		iIndex = i * TextureDesc.Width + j;

			if(j < 512)
				pPixel[iIndex] = D3DCOLOR_ABGR(255, 255, 255, 255);
			else
				pPixel[iIndex] = D3DCOLOR_ABGR(255, 0, 0, 0);

		}
	}

	SubResource.pSysMem = pPixel;
	SubResource.SysMemPitch = TextureDesc.Width * 4;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &SubResource, &pTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(pTexture, nullptr, &m_pFilterSRV)))
		return E_FAIL;

	if (FAILED(DirectX::SaveDDSTextureToFile(m_pDeviceContext, pTexture, TEXT("../Bin/Test.dds"))))
		return E_FAIL;

	Safe_Delete_Array(pPixel);
	Safe_Release(pTexture);

	return S_OK;
}

CTerrain * CTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const CTransform::TRANSFORMDESC& TransformDesc)
{
	CTerrain*		pInstance = new CTerrain(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(TransformDesc)))
	{
		MSGBOX(TEXT("Failed to Created : CTerrain"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTerrain::Clone(void * pArg)
{
	CTerrain*		pInstance = new CTerrain(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CTerrain"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
	{
		if (nullptr != pCell)
		{
			Safe_Release(pCell);
		}
	}

	m_Cells.clear();

	for (auto& pTexture : m_pTextureCom)		
		Safe_Release(pTexture);


	Safe_Release(m_pTarget);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pShader);
	Safe_Release(m_pFilterSRV);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
