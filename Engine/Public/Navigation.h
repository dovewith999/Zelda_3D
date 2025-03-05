#pragma once
#include "Component.h"

BEGIN(Engine)
class CCell;
class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		_uint iCurrentIndex = 0;

    }NAVIGATIONDESC;

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() override = default;

public:
	HRESULT NativeConstruct_Prototype(const _tchar* pNavigationDataFilePath);
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	_bool Move_OnNavigation(const _float3* pPosition);
	_vector Compute_Height(_fvector vWorldPos);

public:
	vector<CCell*> Get_Cells() const
	{
		return m_Cells;
	}
	
	_uint Get_CellOption() const;

	_uint Get_PreCellOption() const;

	_uint Get_NumCurrentCell() const
	{
		return m_iCurrentCellIndex;
	}

public:
	void Find_RealCell(const _float3 * pPosition);
	void Find_CellOption(_uint m_iOptionID);

public:
	HRESULT SetUp_Neighbor();

#ifdef _DEBUG
public:
	HRESULT Ready_Debug();
	HRESULT Render();
#endif // _DEBUG


private:
	_uint m_iNumCell = 0;
	_int m_iCurrentCellIndex = -1;
	_int m_iPreCellIndex = -1;
	vector<CCell*> m_Cells;
	typedef vector<CCell*> CELLS;

#ifdef _DEBUG
private:
	class CShader* m_pShader = nullptr;
#endif // _DEBUG

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pNavigationDataFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
END

