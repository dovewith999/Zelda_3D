#include "stdafx.h"

#include "../IMGUI/imgui.h" 
#include "../IMGUI/imgui_impl_dx11.h"
#include "../IMGUI/imgui_impl_win32.h"

#include "..\Public\Tool.h"
#include "GameInstance.h"
#include "Terrain.h"

IMPLEMENT_SINGLETON(CTool);

CTool::CTool()
{
}

void CTool::SetUp_IMGUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	//Setup IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(pDevice, pDeviceContext);
	ImGui::StyleColorsDark();
}

HRESULT CTool::Tool_Navigation()
{

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	ImGui::Begin("Tool");
	ImGui::Text("Current Level average %.3f ms/frame (%.1f FPS)", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	if(ImGui::Button("NavigationMode"))
	{
		m_bNavigationMode = true;
		m_bObjectMode = false;
	}

	ImGui::SameLine();

	if (ImGui::Button("ObjectMode"))
	{
		m_bNavigationMode = false;
		m_bObjectMode = true;
	}

	if (m_bNavigationMode)
	{
		ImGui::Text("Current Mode is NavigationMode");
	}

	else if(m_bObjectMode)
	{
		ImGui::Text("Current Mode is ObjectMode");
	}

	if (ImGui::Button("Height+0.01"))
	{
		m_fHeight += 0.01f;
	}

	ImGui::SameLine();

	if (ImGui::Button("Height-0.01"))
	{
		m_fHeight -= 0.01f;
	}

	if (ImGui::Button("Height+0.1"))
	{
		m_fHeight += 0.1f;
	}

	ImGui::SameLine();

	if (ImGui::Button("Height-0.1"))
	{
		m_fHeight -= 0.1f;
	}

	if (ImGui::Button("Height+0.5"))
	{
		m_fHeight += 0.5f;
	}

	ImGui::SameLine();

	if (ImGui::Button("Height-0.5"))
	{
		m_fHeight -= 0.5f;
	}

	if (ImGui::Button("Height+30.0"))
	{
		m_fHeight += 30.f;
	}

	ImGui::SameLine();

	if (ImGui::Button("Height-30.0"))
	{
		m_fHeight -= 30.f;
	}



	ImGui::Text("Height is %.2f", m_fHeight);

	if (m_bNavigationMode)
	{

		if (ImGui::Button("Delete_OneDot"))
		{
			if (!m_Points.empty())
			{
				m_Points.pop_back();
			}

			if (!m_Options.empty())
			{
				m_Options.pop_back();
			}

			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);

			if (nullptr == m_pTerrain)
			{
				m_pTerrain = dynamic_cast<CTerrain*>(pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_BackGround")));
			}

			Safe_Release(pGameInstance);
			m_pTerrain->Set_NumPoint();
		}

		//일반적인 Cell
		if (ImGui::Button("Cell OPTION_NORMAL"))
		{
			m_iCellOption = 0;
		}
		ImGui::SameLine();

		//가지 못하는 곳(끼임 방지)
		if (ImGui::Button("Cell OPTION_NON"))
		{
			m_iCellOption = 1;
		}
		ImGui::SameLine();
		
		//점프가 되는 Cell
		if (ImGui::Button("Cell OPTION_JUMP"))
		{
			m_iCellOption = 2;
		}

		//벽을 잡고 올라가는 Cell
		if (ImGui::Button("Cell OPTION_CLIMB"))
		{
			m_iCellOption = 3;
		}
		ImGui::SameLine();

		//OPTION_CLIMB와 애니메이션 구분 용 Cell
		if (ImGui::Button("Cell OPTION_CLIMB_HIGH"))
		{
			m_iCellOption = 4;
		}
		ImGui::SameLine();

		//오브젝트가 이 Cell에 가면 상호작용이 됨
		if (ImGui::Button("Cell OPTION_OBJECT"))
		{
			m_iCellOption = 5;
		}

		ImGui::Text("Option of Next Cell is %d", m_iCellOption);


		if (ImGui::Button("Save_Dungeon"))
		{
			Save_Navigation_Dungeon();
		}

		if (ImGui::Button("Save_Dungeon_Final"))
		{
			Save_Navigation_DungeonFinal();
		}

	}

	static const char* ObjectLists[]{ "FireBrazier", "Door", "Bomb", "Box", "Cube", "Rock", "Magma", "WaterPot", "WoodPlanks", "Bridge", "Lock", "Bokoblin", "Trigger_Ladder", "Trigger_Wall", "Trigger_Hang", "WoodDoor", "StoneBridge", "TresureBoxShort", "MagmaZet", "Trigger_Boss" };
	static const char* RoomLists[]{ "Room_0", "Room_1", "Room_2", "Room_3", "Room_4", "Room_5", "Room_6", "Room_7", "Room_8", "Room_9" };

	if (m_bObjectMode)
	{
		ImGui::Combo("Object", &m_iSelectObject, ObjectLists, IM_ARRAYSIZE(ObjectLists));
		ImGui::Combo("Room", &m_iSelectRoom, RoomLists, IM_ARRAYSIZE(RoomLists));

		ImGui::DragFloat3("Position", m_fPosition, 0.1f, -10.f, 10.f);
		ImGui::DragFloat3("Rotation", m_fRotation, 1.f, 0.f, 360.f);
		ImGui::DragFloat3("Scale", m_fScale, 0.1f, 0.1f, 100.f);
		if (ImGui::Button("Save_Dungeon"))
		{
			Save_Object();
		}

		if (ImGui::Button("Save_Dungeon_Final"))
		{
			Save_Object_Dungeon_Final();
		}


		if (ImGui::Button("Delete"))
		{
			if (!m_ObjectID.empty())
			{
				m_ObjectID.pop_back();
			}

			if (!m_WorldMatrix.empty())
			{
				m_WorldMatrix.pop_back();
			}

			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);
			m_pTerrain = dynamic_cast<CTerrain*>(pGameInstance->Find_GameObject(LEVEL_TOOL, TEXT("Layer_BackGround")));
			Safe_Release(pGameInstance);

			m_pTerrain->Pop_Back_Object();
		}
	}

	ImGui::End();
	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}

HRESULT CTool::Save_Navigation_Dungeon()
{
	_ulong			dwByte = 0;

	_uint iNumOption = 0;
	HANDLE			hFile = CreateFile(TEXT("../Bin/Data/Navigation.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoints[3];
	ZeroMemory(vPoints, sizeof(_float3) * 3);

	_uint i = m_Points.size();
	_uint j = m_Options.size();

	for (_uint i = 0; i < m_Points.size(); i += 3)
	{
		for (_uint j = 0; j < 3; ++j)
		{
			vPoints[j] = m_Points[i + j];

			if (2 == j)
			{
				WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr); // 점이 3개씩 모였을 경우에만 저장
				WriteFile(hFile, &m_Options[iNumOption], sizeof(_uint), &dwByte, nullptr); // 점 3개의 위치와 옵션 저장
				++iNumOption;
			}
		}
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CTool::Save_Navigation_DungeonFinal()
{
	_ulong			dwByte = 0;

	_uint iNumOption = 0;
	HANDLE			hFile = CreateFile(TEXT("../Bin/Data/Navigation_Dungeon_Final.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoints[3];
	ZeroMemory(vPoints, sizeof(_float3) * 3);

	_uint i = m_Points.size();
	_uint j = m_Options.size();

	for (_uint i = 0; i < m_Points.size(); i += 3)
	{
		for (_uint j = 0; j < 3; ++j)
		{
			vPoints[j] = m_Points[i + j];

			if (2 == j)
			{
				WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr); // 점이 3개씩 모였을 경우에만 저장
				WriteFile(hFile, &m_Options[iNumOption], sizeof(_uint), &dwByte, nullptr); // 점 3개의 위치와 옵션 저장
				++iNumOption;
			}
		}
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CTool::Save_Object()
{
	_ulong			dwByte = 0;

	HANDLE			hFile = CreateFile(TEXT("../Bin/Data/Object/ObjectMatrix_Dungeon.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float4x4 WorldMatrix;
	_uint iObjectID;

	_uint iVectorSize = m_WorldMatrix.size();

	for (_uint i = 0; i < iVectorSize; ++i)
	{

		WorldMatrix = m_WorldMatrix[i];
		iObjectID = m_ObjectID[i];

		WriteFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr); // 오브젝트의 월드 매트릭스 저장
		WriteFile(hFile, &iObjectID, sizeof(_uint), &dwByte, nullptr); // 오브젝트의 옵션 저장
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CTool::Save_Object_Dungeon_Final()
{

	_ulong			dwByte = 0;

	HANDLE			hFile = CreateFile(TEXT("../Bin/Data/Object/ObjectMatrix_Dungeon_Final.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float4x4 WorldMatrix;
	_uint iObjectID;

	_uint iVectorSize = m_WorldMatrix.size();

	for (_uint i = 0; i < iVectorSize; ++i)
	{

		WorldMatrix = m_WorldMatrix[i];
		iObjectID = m_ObjectID[i];

		WriteFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr); // 오브젝트의 월드 매트릭스 저장
		WriteFile(hFile, &iObjectID, sizeof(_uint), &dwByte, nullptr); // 오브젝트의 옵션 저장
	}

	CloseHandle(hFile);

	return S_OK;
}



_float3 CTool::Compare_Length(_float3 vPos)
{
	for (auto& Point : m_Points)
	{
		if (fabs(Point.x - vPos.x) < RADIUS_SNAP_POINT &&
			fabs(Point.z - vPos.z) < RADIUS_SNAP_POINT &&
			fabs(Point.y - vPos.y) < RADIUS_SNAP_POINT)
		{

			return Point;
		}
	}

	return vPos;
}

void CTool::Free()
{
	::ImGui_ImplDX11_Shutdown();
	::ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();


	m_ObjectID.clear();
	m_WorldMatrix.clear();
	m_Points.clear();
	m_Options.clear();

	Safe_Release(m_pTerrain);
}
