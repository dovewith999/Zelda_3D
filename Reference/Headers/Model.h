#pragma once

#include "Component.h"

BEGIN(Engine)
class CHierarchyNode;
class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}	

	_float4x4 Get_PivotMatrix() const
	{
		return m_PivotMatrix;
	}

	_bool Get_AnimationState() const
	{
		return m_bIsFinished;
	}

	_bool Get_BodyAnimationState() const
	{
		return m_bIsFinished_Body;
	}


	_bool Get_WaistAnimationState() const
	{
		return m_bIsFinished_Waist;
	}

public:
	HRESULT SetUp_Material_OnShader(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType);
	HRESULT SetUp_Animation(_uint iAnimationIndex);
	HRESULT SetUp_BodyAnimation(_uint iAnimationIndex);
	HRESULT SetUp_WaistAnimation(_uint iAnimationIndex);
	HRESULT SetUp_BoneMatrices_OnShader(class CShader* pShader, const char* pConstantName, _uint iMeshIndex);

	void Play_Animation(_double TimeDelta);
	void Play_Animation_Loop(_double TimeDelta);
	void Play_Animation_Body(_double TimeDelta);
	void Play_Animation_Waist(_double TimeDelta);
	void Play_Animation_Body_Loop(_double TimeDelta);
	void Play_Animation_Waist_Loop(_double TimeDelta);
	void Init_Animation();

public:
	void Change_Animation(_double TimeDelta);

public:
	virtual HRESULT NativeConstruct_Prototype(const char* pModelFilePath, const char* pModelFileName, TYPE eType, _fmatrix PivotMatrix);
	virtual HRESULT NativeConstruct(void* pArg);

public:
	HRESULT Render(_uint iMeshIndex);
	CHierarchyNode* Find_HierarchyNode(const char* pNodeName);

private: // Assimp의 Scene객체
	const aiScene*			m_pScene = nullptr;
	Assimp::Importer		m_Importer;
	TYPE					m_eType = TYPE_END;

private:
	_bool	m_bIsFinished_Body = false; // 상체 애니메이션이 끝났는지 확인하기 위한 변수
	_bool   m_bIsFinished_Waist = false; // 하체 애니메이션이 끝났는지 확인하기 위한 변수
	_bool	m_bIsFinished = false; // 애니메이션이 끝났는지 확인하기 위한 변수
	_bool   m_bLerpIsFinished = true; // 애니메이션 교체시 선형보간이 끝났는지 확인하기 위한 변수

private:
	_uint									m_iNumMeshes = 0;
	vector<class CMeshContainer*>			m_Meshes;
	typedef vector<class CMeshContainer*>	MESHES;

private: // Animation을 저장하기 위한 Vector
	_uint m_iNumAnimations = 0;
	_uint m_iCurrentAnimIndex = 0;
	_uint m_iCurrentBodyAnimIndex = 0; // 현재 상체 애니메이션 인덱스
	_uint m_iCurrentWaistAnimIndex = 0; // 현재 하체 애니메이션 인덱스
	char m_szBodyBoneName[MAX_PATH] = "";
	char m_szWaistBoneName[MAX_PATH] = "";
	_uint m_iNextAnimIndex = 0; // 선형보간 하기위한 변수
	_bool m_bChangeAnim = false;
	vector<class CAnimation*>   m_Animations;
	typedef vector<class CAnimation*> ANIMATIONS;

private:
	_float m_fTimeAcc = 0.f;

private: // HierarchyNode를 저장하기 위한 벡터
	vector<CHierarchyNode*> m_BodyHierarchyNodes; // 상체뼈를 저장하기 위한 벡터
	vector<CHierarchyNode*> m_WaistHierarchyNodes; // 하체 뼈를 저장하기 위한 벡터

	vector<CHierarchyNode*>					m_HierarchyNodes;
	typedef vector<CHierarchyNode*>			HIERARCHYNODES;

private:
	_float4x4				m_PivotMatrix;

private: // Texture에 Material을 저장하기 위한 Vector
	_uint									m_iNumMaterials = 0;
	vector<MODELMATERIAL>					m_Materials;
	typedef vector<MODELMATERIAL>			MATERIALS;

private:
	HRESULT Ready_MeshContainers();
	HRESULT Ready_Materials(const char* pModelFilePath);
	HRESULT Ready_HierarchyNode(aiNode* pNode, CHierarchyNode* pParent, _uint iDepth);
	HRESULT Ready_Animation();

	HRESULT Clone_MeshContainer();
	HRESULT Clone_Animation();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* pModelFilePath, const char* pModelFileName, TYPE eType, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END