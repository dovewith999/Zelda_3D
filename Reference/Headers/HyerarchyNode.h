#pragma once
#include "Base.h"

BEGIN(Engine)
class CHyerarchyNode final : public CBase
{
private:
	explicit CHyerarchyNode();
	virtual ~CHyerarchyNode() override = default;

public:
	HRESULT NativeConstruct(const char* pBoneName, _float4x4 TransformMatrix, CHyerarchyNode* pParnetNode, _uint iDepth);

private:
	const char m_szBoneName[MAX_PATH] = "";
	_float4x4 m_TransformMatrix;
	CHyerarchyNode* m_pParent = nullptr;
	_uint m_iDepth = 0;

public:
	static CHyerarchyNode* Create(const char* pBoneName, _float4x4 TransformMatrix, CHyerarchyNode* pParnetNode, _uint iDepth);
	virtual void Free() override;
};
END

