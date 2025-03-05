#include "..\Public\HierarchyNode.h"


CHierarchyNode::CHierarchyNode()
{
}

HRESULT CHierarchyNode::NativeConstruct(const char * pBoneName, _float4x4 TransformMatrix, CHierarchyNode * pParnetNode, _uint iDepth)
{
	strcpy_s(m_szBoneName, pBoneName);

	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());

	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixTranspose(XMLoadFloat4x4(&TransformMatrix)));

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	m_pParent = pParnetNode;

	Safe_AddRef(m_pParent);

	m_iDepth = iDepth;

	return S_OK;
}

const char * CHierarchyNode::Get_ParentOfDepthFive()
{
	if (5 > m_iDepth)
	{
		return nullptr;
	}

	else if (5 == m_iDepth)
	{
		return Get_Name();
	}

	else
	{
		_uint iParentDepth = m_pParent->Get_Depth();

		if (5 == iParentDepth)
		{
			return m_pParent->Get_Name();
		}

		else
		{
			return m_pParent->Get_ParentOfDepthFive();
		}
	}
}

void CHierarchyNode::Set_OffsetMatrix(const _float4x4* OffSetMarrix)
{
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(OffSetMarrix)));
}

void CHierarchyNode::Set_TransformationMatrix(_fmatrix TrasnformationMatrix)
{
	XMStoreFloat4x4(&m_TransformMatrix, TrasnformationMatrix);
}

void CHierarchyNode::Update_CombinedTransformationMatrix()
{
	_matrix ParentCombinedMatrix = XMMatrixIdentity();

	if (nullptr != m_pParent)
	{
		ParentCombinedMatrix = m_pParent->Get_CombinedTransformationMatrix();
	}

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformMatrix) * ParentCombinedMatrix);
}

CHierarchyNode * CHierarchyNode::Create(const char * pBoneName, _float4x4 TransformMatrix, CHierarchyNode * pParnetNode, _uint iDepth)
{
	CHierarchyNode* pInstance = new CHierarchyNode();
	if (FAILED(pInstance->NativeConstruct(pBoneName, TransformMatrix, pParnetNode, iDepth)))
	{
		MSGBOX(TEXT("Error : Failed Created HierarchyNode"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHierarchyNode::Free()
{
	Safe_Release(m_pParent);
}


