#pragma once
#include "Base.h"

BEGIN(Engine)
class CHierarchyNode final : public CBase
{
private:
	CHierarchyNode();
	virtual ~CHierarchyNode() override = default;

public:
	HRESULT NativeConstruct(const char* pBoneName, _float4x4 TransformMatrix, CHierarchyNode* pParnetNode, _uint iDepth);

public:
	_uint Get_Depth() const {
		return m_iDepth;
	}

	const char* Get_Name() const {
		return m_szBoneName;
	}

	_matrix Get_OffSetMatrix() const
	{
		return XMLoadFloat4x4(&m_OffsetMatrix);
	}

	_matrix Get_CombinedTransformationMatrix() const
	{
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}

	CHierarchyNode* Get_ParentNode() 
	{
		return m_pParent;
	}

	_float4x4 Get_TransformMatrix()
	{
		return m_TransformMatrix;
	}

	const char* Get_ParentOfDepthFive();

	_float4x4 Get_PreTransformMatrix()
	{
		return m_PreTransformMatrix;
	}

public:
	void Set_OffsetMatrix(const _float4x4* OffSetMarrix);
	void Save_TransformationMatrix()
	{
		m_PreTransformMatrix = m_TransformMatrix;
	}

	void Set_TransformationMatrix(_fmatrix TrasnformationMatrix);
	void Update_CombinedTransformationMatrix();

private:
	char m_szBoneName[MAX_PATH] = "";
	_float4x4 m_OffsetMatrix;
	_float4x4 m_TransformMatrix;
	_float4x4 m_PreTransformMatrix;
	_float4x4 m_CombinedTransformationMatrix;
	CHierarchyNode* m_pParent = nullptr;
	_uint m_iDepth = 0;

public:
	static CHierarchyNode* Create(const char* pBoneName, _float4x4 TransformMatrix, CHierarchyNode* pParnetNode, _uint iDepth);
	virtual void Free() override;
};
END

