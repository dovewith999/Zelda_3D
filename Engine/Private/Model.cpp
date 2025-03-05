#include "..\Public\Model.h"
#include "HierarchyNode.h"
#include "MeshContainer.h"
#include "Shader.h"
#include "Texture.h"
#include "Animation.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{

}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_pScene(rhs.m_pScene)
	, m_eType(rhs.m_eType)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_Materials(rhs.m_Materials)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_Animations(rhs.m_Animations)
	, m_PivotMatrix(rhs.m_PivotMatrix)
{
	for (auto& pAnimation : m_Animations)
	{
		Safe_AddRef(pAnimation);
	}

	for (auto& pMeshContainer : m_Meshes)
	{
		Safe_AddRef(pMeshContainer);
	}

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pTexture)
		{
			Safe_AddRef(pTexture);
		}
	}

}

HRESULT CModel::SetUp_Material_OnShader(CShader * pShader, const char * pConstantName, _uint iMeshIndex, aiTextureType eTextureType)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	return m_Materials[iMaterialIndex].pTexture[eTextureType]->Bind_OnShader(pShader, pConstantName, 0);
}

HRESULT CModel::SetUp_Animation(_uint iAnimationIndex)
{
	if (iAnimationIndex > m_iNumAnimations)
	{
		return E_FAIL;
	}

	m_iCurrentAnimIndex = iAnimationIndex;

	//if (iAnimationIndex != m_iCurrentAnimIndex)
	//{
	//	//Change_Anim();
	//	//for (auto& pHierarchyNodes : m_HierarchyNodes)
	//	//{
	//	//	pHierarchyNodes->Save_TransformationMatrix();
	//	//}

	//	for (auto& pHierarchyNode : m_HierarchyNodes)
	//	{
	//		pHierarchyNode->Save_TransformationMatrix();
	//	}

	//	m_bChangeAnim = true;
	//}

	//else
	//{
	//	
	//}

	return S_OK;
}

HRESULT CModel::SetUp_BodyAnimation(_uint iAnimationIndex)
{
	if (iAnimationIndex > m_iNumAnimations)
	{
		return E_FAIL;
	}
	
	m_iNextAnimIndex = iAnimationIndex;

	if (iAnimationIndex != m_iCurrentBodyAnimIndex && (29 == m_iCurrentBodyAnimIndex))
	{
		////Change_Anim();
		//m_bChangeAnim = true;
		////m_Animations[m_iCurrentAnimIndex]->Init_KeyFrame();
		//for (auto& pHierarchyNode : m_BodyHierarchyNodes)
		//{
		//	pHierarchyNode->Save_TransformationMatrix();
		//}

		//m_iCurrentAnimIndex = iAnimationIndex;

		m_Animations[m_iCurrentBodyAnimIndex]->Init_KeyFrame();
	}
	
	m_iCurrentBodyAnimIndex = iAnimationIndex;


	return S_OK;
}

HRESULT CModel::SetUp_WaistAnimation(_uint iAnimationIndex)
{
	if (iAnimationIndex > m_iNumAnimations)
	{
		return E_FAIL;
	}

	m_iNextAnimIndex = iAnimationIndex;

	if (iAnimationIndex != m_iCurrentWaistAnimIndex && (29 == m_iCurrentWaistAnimIndex))
	{
		////Change_Anim();
		//m_bChangeAnim = true;
		////m_Animations[m_iCurrentAnimIndex]->Init_KeyFrame();
		//for (auto& pHierarchyNode : m_WaistHierarchyNodes)
		//{
		//	pHierarchyNode->Save_TransformationMatrix();
		//}

		//m_iCurrentAnimIndex = iAnimationIndex;

		m_Animations[m_iCurrentWaistAnimIndex]->Init_KeyFrame();

	}

	
	m_iCurrentWaistAnimIndex = iAnimationIndex;

	return S_OK;
}

HRESULT CModel::SetUp_BoneMatrices_OnShader(CShader * pShader, const char * pConstantName, _uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
	{
		return E_FAIL;
	}

	_float4x4 BoneMatrices[128];

	ZeroMemory(BoneMatrices, sizeof(_float4x4) * 128);

	m_Meshes[iMeshIndex]->Get_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

	if (FAILED(pShader->Set_RawValue(pConstantName, BoneMatrices, sizeof(_float4x4) * 128)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CModel::Play_Animation(_double TimeDelta)
{
	//if (m_bLerpIsFinished)
	//{
	//	m_iCurrentAnimIndex = m_iNextAnimIndex;
	//	m_Animations[m_iCurrentAnimIndex]->Animation_Finished();
	//}
	//m_bIsFinished = m_Animations[m_iCurrentAnimIndex]->Get_AnimationState();

	//if (!m_bIsFinished)
	//{
	//	m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(TimeDelta);
	//}

	//if (m_bIsFinished)
	//{
	//	m_bLerpIsFinished = false;

	//	if (FAILED(m_Animations[m_iCurrentAnimIndex]->Linear_Interpolation_Next_Anim(TimeDelta, m_Animations[m_iNextAnimIndex])))
	//	{
	//		m_bLerpIsFinished = true;
	//	}

	//}

	if (!m_bChangeAnim)
	{
		//m_iCurrentAnimIndex = m_iNextAnimIndex;

		m_Animations[m_iCurrentAnimIndex]->Animation_Finished();
		m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(TimeDelta);
		m_bIsFinished = m_Animations[m_iCurrentAnimIndex]->Get_AnimationState();

		//for (auto& pHierarchyNode : m_HierarchyNodes)
		//{
		//	pHierarchyNode->Update_CombinedTransformationMatrix();
		//}
	}

	else
	{
		//Change_Animation(TimeDelta);
		m_Animations[m_iCurrentAnimIndex]->Linear_Interpolation_Next_Anim(TimeDelta, m_Animations[m_iNextAnimIndex], &m_bChangeAnim);
		if (!m_bChangeAnim)
		{
			m_Animations[m_iCurrentAnimIndex]->Init_KeyFrame();
			m_iCurrentAnimIndex = m_iNextAnimIndex;
		}
	}
	if (!m_bIsFinished)
	{
		for (auto& pHierarchyNode : m_HierarchyNodes)
		{
			pHierarchyNode->Update_CombinedTransformationMatrix();
		}
	}
	
}

void CModel::Play_Animation_Loop(_double TimeDelta)
{
	/*if (!m_bChangeAnim)
	{*/
		//m_iCurrentAnimIndex = m_iNextAnimIndex;

		m_Animations[m_iCurrentAnimIndex]->Animation_Finished();
		m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(TimeDelta, false);
		m_bIsFinished = m_Animations[m_iCurrentAnimIndex]->Get_AnimationState();

		//for (auto& pHierarchyNode : m_HierarchyNodes)
		//{
		//	pHierarchyNode->Update_CombinedTransformationMatrix();
		//}
	//}

	//else
	//{
	//	//Change_Animation(TimeDelta);
	//	m_Animations[m_iCurrentAnimIndex]->Linear_Interpolation_Next_Anim(TimeDelta, m_Animations[m_iNextAnimIndex], &m_bChangeAnim);
	//	if (!m_bChangeAnim)
	//	{
			//m_Animations[m_iCurrentAnimIndex]->Init_KeyFrame();
	/*		m_iCurrentAnimIndex = m_iNextAnimIndex;
		}
	}*/

	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		pHierarchyNode->Update_CombinedTransformationMatrix();
	}
}

void CModel::Play_Animation_Body(_double TimeDelta)
{
	//if (!m_bChangeAnim)
	//{
	//	//m_iCurrentAnimIndex = m_iNextAnimIndex;

	//	m_Animations[m_iCurrentBodyAnimIndex]->Animation_Finished();
	//	m_Animations[m_iCurrentBodyAnimIndex]->Update_TransformationMatrix(TimeDelta);
	//	m_bIsFinished_Body = m_Animations[m_iCurrentAnimIndex]->Get_AnimationState();

	//	//for (auto& pHierarchyNode : m_HierarchyNodes)
	//	//{
	//	//	pHierarchyNode->Update_CombinedTransformationMatrix();
	//	//}
	//}

	//else
	//{
	//	//Change_Animation(TimeDelta);
	//	m_Animations[m_iCurrentBodyAnimIndex]->Linear_Interpolation_Next_Anim(TimeDelta, m_Animations[m_iNextAnimIndex], &m_bChangeAnim);
	//	if (!m_bChangeAnim)
	//	{
	//		//m_Animations[m_iCurrentBodyAnimIndex]->Init_KeyFrame();
	//		m_iCurrentBodyAnimIndex = m_iNextAnimIndex;
	//	}
	//}
	m_Animations[m_iCurrentBodyAnimIndex]->Animation_Finished();
	m_Animations[m_iCurrentBodyAnimIndex]->Update_TransformationMatrix(TimeDelta);
	m_bIsFinished_Body = m_Animations[m_iCurrentBodyAnimIndex]->Get_AnimationState();

	for (auto& pHierarchyNode : m_BodyHierarchyNodes)
	{
		pHierarchyNode->Update_CombinedTransformationMatrix();
	}

}

void CModel::Play_Animation_Waist(_double TimeDelta)
{
	//if (!m_bChangeAnim)
	//{
	//	//m_iCurrentAnimIndex = m_iNextAnimIndex;

	//	m_Animations[m_iCurrentWaistAnimIndex]->Animation_Finished();
	//	m_Animations[m_iCurrentWaistAnimIndex]->Update_TransformationMatrix(TimeDelta);
	//	m_bIsFinished_Waist = m_Animations[m_iCurrentWaistAnimIndex]->Get_AnimationState();


	//	//for (auto& pHierarchyNode : m_HierarchyNodes)
	//	//{
	//	//	pHierarchyNode->Update_CombinedTransformationMatrix();
	//	//}
	//}

	//else
	//{
	//	//Change_Animation(TimeDelta);
	//	m_Animations[m_iCurrentWaistAnimIndex]->Linear_Interpolation_Next_Anim(TimeDelta, m_Animations[m_iNextAnimIndex], &m_bChangeAnim);
	//	if (!m_bChangeAnim)
	//	{
	//		//m_Animations[m_iCurrentWaistAnimIndex]->Init_KeyFrame();
	//		m_iCurrentWaistAnimIndex = m_iNextAnimIndex;
	//	}
	//}
	m_Animations[m_iCurrentWaistAnimIndex]->Animation_Finished();
	m_Animations[m_iCurrentWaistAnimIndex]->Update_TransformationMatrix(TimeDelta);
	m_bIsFinished_Waist = m_Animations[m_iCurrentWaistAnimIndex]->Get_AnimationState();

	for (auto& pHierarchyNode : m_WaistHierarchyNodes)
	{
		pHierarchyNode->Update_CombinedTransformationMatrix();
	}
}

void CModel::Play_Animation_Body_Loop(_double TimeDelta)
{
	//m_Animations[m_iCurrentBodyAnimIndex]->Animation_Finished();
	m_Animations[m_iCurrentBodyAnimIndex]->Update_TransformationMatrix(TimeDelta, false);
	m_bIsFinished_Body = m_Animations[m_iCurrentBodyAnimIndex]->Get_AnimationState();

	//if (!m_bChangeAnim)
	//{
	//	//m_iCurrentAnimIndex = m_iNextAnimIndex;

	//	m_Animations[m_iCurrentBodyAnimIndex]->Update_TransformationMatrix(TimeDelta, false);
	//	m_bIsFinished_Body = m_Animations[m_iCurrentAnimIndex]->Get_AnimationState();

	//	//for (auto& pHierarchyNode : m_HierarchyNodes)
	//	//{
	//	//	pHierarchyNode->Update_CombinedTransformationMatrix();
	//	//}
	//}

	//else
	//{
	//	//Change_Animation(TimeDelta);
	//	m_Animations[m_iCurrentBodyAnimIndex]->Linear_Interpolation_Next_Anim(TimeDelta, m_Animations[m_iNextAnimIndex], &m_bChangeAnim);
	//	if (!m_bChangeAnim)
	//	{
	//		//m_Animations[m_iCurrentBodyAnimIndex]->Init_KeyFrame();
	//		m_iCurrentBodyAnimIndex = m_iNextAnimIndex;
	//	}
	//}

	for (auto& pHierarchyNode : m_BodyHierarchyNodes)
	{
		pHierarchyNode->Update_CombinedTransformationMatrix();
	}
}

void CModel::Play_Animation_Waist_Loop(_double TimeDelta)
{
//	m_Animations[m_iCurrentWaistAnimIndex]->Animation_Finished();
	m_Animations[m_iCurrentWaistAnimIndex]->Update_TransformationMatrix(TimeDelta, false);
	m_bIsFinished_Waist = m_Animations[m_iCurrentWaistAnimIndex]->Get_AnimationState();

	//if (!m_bChangeAnim)
	//{
	//	//m_iCurrentAnimIndex = m_iNextAnimIndex;

	//	m_Animations[m_iCurrentWaistAnimIndex]->Update_TransformationMatrix(TimeDelta, false);
	//	m_bIsFinished_Waist = m_Animations[m_iCurrentWaistAnimIndex]->Get_AnimationState();


	//	//for (auto& pHierarchyNode : m_HierarchyNodes)
	//	//{
	//	//	pHierarchyNode->Update_CombinedTransformationMatrix();
	//	//}
	//}

	//else
	//{
	//	//Change_Animation(TimeDelta);
	//	m_Animations[m_iCurrentWaistAnimIndex]->Linear_Interpolation_Next_Anim(TimeDelta, m_Animations[m_iNextAnimIndex], &m_bChangeAnim);
	//	if (!m_bChangeAnim)
	//	{
	//		//m_Animations[m_iCurrentWaistAnimIndex]->Init_KeyFrame();
	//		m_iCurrentWaistAnimIndex = m_iNextAnimIndex;
	//	}
	//}

	for (auto& pHierarchyNode : m_WaistHierarchyNodes)
	{
		pHierarchyNode->Update_CombinedTransformationMatrix();
	}
}

void CModel::Init_Animation()
{
	m_Animations[m_iCurrentWaistAnimIndex]->Init_KeyFrame();
}

void CModel::Change_Animation(_double TimeDelta)
{
	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		pHierarchyNode->Save_TransformationMatrix();
	}


}

HRESULT CModel::NativeConstruct_Prototype(const char * pModelFilePath, const char * pModelFileName, TYPE eType, _fmatrix PivotMatrix)
{
	strcpy_s(m_szBodyBoneName, "cl_body_chn                    ");
	strcpy_s(m_szWaistBoneName, "cl_waist_chn                   ");

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	char	szFullPath[MAX_PATH] = "";

	strcpy_s(szFullPath, pModelFilePath);
	strcat_s(szFullPath, pModelFileName);

	m_eType = eType;

	_uint		iFlag = 0;

	if (TYPE_NONANIM == m_eType)
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace;

	m_pScene = m_Importer.ReadFile(szFullPath, iFlag);
	if (nullptr == m_pScene)
	{
		return E_FAIL;
	}

	if (FAILED(Ready_HierarchyNode(m_pScene->mRootNode, nullptr, 0)))
	{
		return E_FAIL;
	}

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* Dest)
	{
		return pSour->Get_Depth() < Dest->Get_Depth();
	});

	if (FAILED(Ready_MeshContainers()))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Materials(pModelFilePath)))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Animation()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::NativeConstruct(void * pArg)
{
	strcpy_s(m_szBodyBoneName, "cl_body_chn                    ");
	strcpy_s(m_szWaistBoneName, "cl_waist_chn                   ");

	if (FAILED(Ready_HierarchyNode(m_pScene->mRootNode, nullptr, 0)))
	{
		return E_FAIL;
	}

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* Dest)
	{
		return pSour->Get_Depth() < Dest->Get_Depth();
	});

	if (FAILED(Clone_MeshContainer()))
	{
		return E_FAIL;
	}

	if (FAILED(Clone_Animation()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	if (nullptr != m_Meshes[iMeshIndex])
	{
		m_Meshes[iMeshIndex]->Render();
	}	

	return S_OK;
}
CHierarchyNode * CModel::Find_HierarchyNode(const char * pNodeName)
{
	auto iter = find_if(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&](CHierarchyNode* pNode)
	{
		return !strcmp(pNode->Get_Name(), pNodeName);
	});

	if (m_HierarchyNodes.end() == iter)
	{
		return nullptr;
	}

	return *iter;
}

HRESULT CModel::Ready_MeshContainers()
{
	if (nullptr == m_pScene)
		return E_FAIL;

	m_iNumMeshes = m_pScene->mNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		aiMesh*		pAIMesh = m_pScene->mMeshes[i];

		CMeshContainer*		pMeshContainer = CMeshContainer::Create(m_pDevice, m_pDeviceContext, m_eType, pAIMesh, XMLoadFloat4x4(&m_PivotMatrix), this);
		if (nullptr == pMeshContainer)
			return FALSE;

		
		m_Meshes.push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const char* pModelFilePath)
{
	if (nullptr == m_pScene)
		return E_FAIL;

	m_iNumMaterials = m_pScene->mNumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		aiMaterial*		pAIMaterial = m_pScene->mMaterials[i];

		MODELMATERIAL		ModelMaterial;
		ZeroMemory(&ModelMaterial, sizeof(MODELMATERIAL));

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{			
			aiString		StrPath;

			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &StrPath)))
				continue;

			char			szFileName[MAX_PATH] = "";
			char			szExt[MAX_PATH] = "";

			_splitpath_s(StrPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			char			szFullPath[MAX_PATH] = "";

			strcpy_s(szFullPath, pModelFilePath);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar			szTextureFullPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szTextureFullPath, MAX_PATH);

			ModelMaterial.pTexture[j] = CTexture::Create(m_pDevice, m_pDeviceContext, szTextureFullPath);
			if (nullptr == ModelMaterial.pTexture[j])
				return E_FAIL;
		}

		m_Materials.push_back(ModelMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_HierarchyNode(aiNode * pNode, CHierarchyNode * pParent, _uint iDepth)
{

#pragma region ==중략==
	_float4x4 TransformMatrix;
	memcpy(&TransformMatrix, &pNode->mTransformation, sizeof(_float4x4));

	CHierarchyNode* pHierarchyNode = CHierarchyNode::Create(pNode->mName.data, TransformMatrix, pParent, iDepth);

	if (nullptr == pHierarchyNode)
	{
		return E_FAIL;
	}
#pragma endregion

	if (SEPARATE_BONE_DEPTH > pHierarchyNode->Get_Depth())
	{
		m_WaistHierarchyNodes.push_back(pHierarchyNode);
	}
	
	else if (SEPARATE_BONE_DEPTH <= pHierarchyNode->Get_Depth())
	{
		if (!strcmp(m_szBodyBoneName, pHierarchyNode->Get_ParentOfDepthFive()))
		{
			m_BodyHierarchyNodes.push_back(pHierarchyNode);
		}

		else if (!strcmp(m_szWaistBoneName, pHierarchyNode->Get_ParentOfDepthFive()))
		{
			m_WaistHierarchyNodes.push_back(pHierarchyNode);
		}
	}

#pragma region ==중략==
	m_HierarchyNodes.push_back(pHierarchyNode);
#pragma endregion

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
	{
		if (FAILED(Ready_HierarchyNode(pNode->mChildren[i], pHierarchyNode, iDepth + 1)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::Ready_Animation()
{
	m_iNumAnimations = m_pScene->mNumAnimations;

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation* pAnimation = CAnimation::Create(m_pScene->mAnimations[i], this);
		if (nullptr == pAnimation)
		{
			return E_FAIL;
		}

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

HRESULT CModel::Clone_MeshContainer()
{
	vector<CMeshContainer*>			Meshes;

	for (auto& pPrototype : m_Meshes)
	{
		CMeshContainer*		pMeshContainer = (CMeshContainer*)pPrototype->Clone(this);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		Meshes.push_back(pMeshContainer);

		Safe_Release(pPrototype);
	}

	m_Meshes.clear();
	m_Meshes = Meshes;

	return S_OK;
}

HRESULT CModel::Clone_Animation()
{
	ANIMATIONS		Anims;

	for (auto& pAnimation : m_Animations)
	{
		if (nullptr != pAnimation)
		{
			CAnimation*		pAnim = pAnimation->Clone_Animation(this);
			if (nullptr != pAnim)
				Anims.push_back(pAnim);

			Safe_Release(pAnimation);
		}
	}
	m_Animations.clear();
	m_Animations = Anims;

	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const char * pModelFilePath, const char * pModelFileName, TYPE eType, _fmatrix PivotMatrix)
{
	CModel*		pInstance = new CModel(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pModelFilePath, pModelFileName, eType, PivotMatrix)))
	{
		MSGBOX(TEXT("Failed to Created : CModel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*		pInstance = new CModel(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(TEXT("Failed to Cloned : CModel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pHierarchyNodes : m_HierarchyNodes)
		Safe_Release(pHierarchyNodes);
	m_HierarchyNodes.clear();

	for (auto& pHierarchyNodes : m_BodyHierarchyNodes)
		Safe_Release(pHierarchyNodes);
	m_BodyHierarchyNodes.clear();

	for (auto& pHierarchyNodes : m_WaistHierarchyNodes)
		Safe_Release(pHierarchyNodes);
	m_WaistHierarchyNodes.clear();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

	for (auto& pMeshContainer : m_Meshes)
		Safe_Release(pMeshContainer);
	m_Meshes.clear();

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pTexture)
			Safe_Release(pTexture);
	}
	m_Materials.clear();


	m_Importer.FreeScene();
}
