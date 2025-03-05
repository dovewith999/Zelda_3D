#pragma once

#include "Base.h"

BEGIN(Engine)

class CPipeLine final : public CBase
{
	DECLARE_SINGLETON(CPipeLine);

public:
	enum TRANSFORMSTATETYPE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };

public:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	_float4 Get_CamPositionFloat4() const {
		return m_vCamPosition;
	}

	_vector Get_CamPositionVector() const {
		return XMLoadFloat4(&m_vCamPosition);
	}

	_matrix Get_TransformMatrix(TRANSFORMSTATETYPE eStateType) {
		return XMLoadFloat4x4(&m_TransformMatrix[eStateType]);
	}

	_float4x4 Get_TransformFloat4x4(TRANSFORMSTATETYPE eStateType) {
		return m_TransformMatrix[eStateType];
	}

	_float4x4 Get_TransformFloat4x4_TP(TRANSFORMSTATETYPE eStateType) {
		return m_TransposeMatrix[eStateType];
	}

	_float Get_Far()
	{
		return m_fCamFar;
	}
public:
	void Set_Transform(TRANSFORMSTATETYPE eStateType, _fmatrix TransformMatrix) {
		XMStoreFloat4x4(&m_TransformMatrix[eStateType], TransformMatrix);
	}

	void Set_Far(_float fFar)
	{
		m_fCamFar = fFar;
	}

public:
	void Tick();

private:
	_float4x4			m_TransformMatrix[D3DTS_END];
	_float4x4			m_TransposeMatrix[D3DTS_END];
	_float4				m_vCamPosition;
	_float				m_fCamFar = 0.f;

public:
	virtual void Free() override;
};

END