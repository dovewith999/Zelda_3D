#pragma once
#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
	enum OPTION {OPTION_NORMAL, OPTION_NON, OPTION_JUMP, OPTION_CLIMB, OPTION_CLIMB_HIGH, OPTION_OBJECT, OPTION_END};

private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CCell() override = default;

public:
	const _float3* Get_Point(POINT ePoint)
	{
		return &m_vPoint[ePoint];
	}

	const _uint Get_Index() const
	{
		return m_iIndex;
	}

	const OPTION Get_Option() const
	{
		return m_eCellOption;
	}

	const int& Get_NeighborIndex(_uint iNum) const
	{
		return m_iNeigborIndex[iNum];
	}

public:
	void SetUp_Neighbor(LINE eLine, _uint iIndex)
	{
		m_iNeigborIndex[eLine] = iIndex;
	}

public:
	void Set_Option(OPTION eOption)
	{
		m_eCellOption = eOption;
	}

public:
	HRESULT NativeConstruct(const _float3* pPoint, _uint iIndex, OPTION eCellOption);
	_bool Compare_Points(const _float3* pSourPoint, const _float3* pDestPoint);
	_bool isIn(const _float3* pPosition, _int* pNeighborIndex);

#ifdef _DEBUG
public:
	HRESULT Render(class CShader* pShader);
#endif // _DEBUG

public: /*Get_Function */
	class CVIBuffer_Triangle* Get_ViBuffer() const
	{
		return m_pVIBuffer;
	}

	_float4x4 Get_WorldMatrix()
	{
		return m_WorldMatrix;
	}


private:
	ID3D11Device*	m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;

private:
	_float3 m_vPoint[POINT_END];
	_float3 m_vLine[LINE_END];
	_int m_iNeigborIndex[LINE_END] = {-1, -1, -1};
	_uint m_iIndex = 0;
	_float4x4 m_WorldMatrix;
	OPTION m_eCellOption = OPTION_END;

#ifdef _DEBUG
private:
	class CVIBuffer_Triangle* m_pVIBuffer = nullptr;
#endif // _DEBUG

#ifdef _DEBUG
private:
	HRESULT Ready_Debug_Buffer();
#endif // _DEBUG

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _float3* pPoints, _uint iIndex, OPTION eCellOption = OPTION_NORMAL);
	virtual void Free() override;
};
END

