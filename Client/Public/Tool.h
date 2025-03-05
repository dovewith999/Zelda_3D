#pragma once
#include "Client_Defines.h"
#include "Base.h"


BEGIN(Client)
class CTerrain;
class CTool final : public CBase
{
	DECLARE_SINGLETON(CTool);

public:
	CTool();
	virtual ~CTool() override = default;
public:
	void SetUp_IMGUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	HRESULT Tool_Navigation();
	HRESULT Save_Navigation_Dungeon();
	HRESULT Save_Navigation_DungeonFinal();
	HRESULT Save_Object();
	HRESULT Save_Object_Dungeon_Final();

public:
	_float Get_Height() const
	{
		return m_fHeight;
	}

	_uint Get_CellOption() const
	{
		return m_iCellOption;
	}

	_float3 Get_FinalPos() const 
	{
		return m_Points.back();
	}

	_bool Get_NavigationMode() const
	{
		return m_bNavigationMode;
	}

	_bool Get_ObjectMode() const
	{
		return m_bObjectMode;
	}

	const int& Get_ObjectID() const
	{
		return m_iSelectObject;
	}

	float Get_Rotation(int iNum)
	{
		return m_fRotation[iNum];
	}

	_vector Get_Scale()
	{
		return XMVectorSet(m_fScale[0], m_fScale[1], m_fScale[2], 0.f);
	}

	float Get_Position(int iNum)
	{
		return m_fPosition[iNum];
	}


public:
	void Push_Back(_float3 vPoint)
	{
		m_Points.push_back(vPoint);
	}

	void Push_Back_Option(_uint iOption)
	{
		m_Options.push_back(iOption);
	}

	void Push_Back_ObjectID(_uint iObjID)
	{
		m_ObjectID.push_back(iObjID);
	}

	void Push_Back_WorldMatrix(_float4x4 WorldMatrix)
	{
		m_WorldMatrix.push_back(WorldMatrix);
	}

	void Pop_Back_And_Push_Back_WorldMatrix(_float4x4 WorldMatrix)
	{
		m_WorldMatrix.pop_back();
		m_WorldMatrix.push_back(WorldMatrix);
	}

	_float3 Compare_Length(_float3 vPos);

private:
	_bool m_bNavigationMode = true;
	_bool m_bObjectMode = false;

private: // 네비메쉬 찍는 툴에 필요한 변수들
	vector<_float3> m_Points;
	vector<_uint> m_Options;
	_float m_fHeight = 0.f;
	_uint m_iPointCount = 0;
	_uint m_iCellOption = 0;

private: // 오브젝트 찍는 툴에 필요한 변수들
	int m_iSelectObject = 0;
	int m_iSelectRoom = 0;
	float m_fRotation[3] = { 0.f, 0.f, 0.f };
	float m_fScale[3] = { 1.f, 1.f, 1.f };
	float m_fPosition[3]{ 0.f, 0.f, 0.f };
	vector<_uint> m_ObjectID;
	vector<_float4x4> m_WorldMatrix;

	const float RADIUS_SNAP_POINT = 0.3f;

private:
	CTerrain* m_pTerrain = nullptr;

public:
	virtual void Free() override;
};
END


