// ��� ���̴� ���ϵ��� ���� hpp ���� 
// cpp�� h�� ����� �����̴�. cpp = hlsl, h = hpp
// ���� ���������� sampler, RenderState�� ��Ÿ���� �Լ����� �ۼ��Ѵ�.

sampler		DefaultSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

sampler		PointSampler = sampler_state
{
	filter = min_mag_mip_point;
	AddressU = wrap;
	AddressV = wrap;
};

RasterizerState RS_Default
{
	FillMode = solid;
	CullMode = back;
	FrontCounterClockwise = false;
	// �������� ä����(solid) �׷���, �ĸ� �ø� ó��(back), �ݽð� �������� �׸���.
};

RasterizerState RS_Wireframe
{
	FillMode = wireframe;
	CullMode = back;
	FrontCounterClockwise = false;
	// �����Ӹ� �׸��� �� ���̴� ����� (wireframe) �׷���
};

RasterizerState RS_Sky
{
	FillMode = solid;
	CullMode = front;
	FrontCounterClockwise = false;

	// Skybox �뵵�� �Լ��̴�.
	// ��ī�̹ڽ��� ī�޶� ��ī�� �ڽ� �ȿ��� ��ī�� �ڽ��� ���� ������ ���߰� �ִ� �����̱� ������ ������ �ø��� �ݴ�� ������ ������ �޸��̶�� �ν��ؼ� �׸��� �ʴ´�.
};

RasterizerState RS_NonCulling
{
	FillMode = solid;
	CullMode = none;
	FrontCounterClockwise = false;
};

DepthStencilState	DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
	// Depth���ۿ� ���ٽǹ��� �̿� �����̴�. ���ٽ� ���۴� �ɼǵ��� Defalut�� ��� false�̱� ������ ���� �����������.
	// �����׽�Ʈ�� ����ϰڴ�(true), ����Ÿ���� ��� ������ ���̰��� �����(all), ���� ���� ���ų� ���� ��(less_equal)��.
};

DepthStencilState	DSS_None_ZTest_And_Write
{
	DepthEnable = false;
	DepthWriteMask = zero;
	// ���� �׽�Ʈ�� ����. �ƹ� �������� ���̰��� �����Ű�� �ʰڴ�.
};

BlendState BS_None
{
	BlendEnable[0] = false;
	// ���ĺ��� �ɼ��� ����.
};

BlendState BS_AlphaBlend
{
	BlendEnable[0] = true;
	SrcBlend[0] = SRC_ALPHA;
	DestBlend[0] = INV_SRC_ALPHA;
	BlendOp[0] = Add;
	// ���� �׸� ��(SrcBlend)�� ���� Ÿ�ٿ� �̹� �׷����ִ� ��(DestBlend)�� ���´�(Add)
	// [0]�� �ɼ��� �����ų ���� Ÿ���� ����Ų��. 0��° ���� Ÿ�ٿ� �����Ű�ڴٴ� ��
	//[]���� ��� ������ �׷��� ������ ��� ���� Ÿ�ٿ� �ɼ��� �����Ų�ٴ� ���̴�.
};

BlendState BS_One
{
	BlendEnable[0] = true;
	SrcBlend[0] = ONE;
	DestBlend[0] = ONE;
	BlendOp[0] = Add;
	// ���ĸ� �Ⱦ��� ���� �׸����� ���� �̹� �׷����ִ� ���� �״�� ���ϰڴ�.
};
