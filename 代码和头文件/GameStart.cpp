#include "GameStart.h"

D3DGame::GameStart::GameStart() 
{

}
D3DGame::GameStart::~GameStart()
{
	for (int i = 0; i != m_imageSRV.size(); ++i)
	{
		ReleaseCOM(m_imageSRV[i]);
	}

}
//载入图片
void D3DGame::GameStart::init(ID3D11Device* device,ID3D11DeviceContext* dc )
{
	m_Device = device;
	m_DeviceContext = dc;
	getFiles("开场动画", m_ImageName);
	
	for (int i = 0; i != m_ImageName.size();++i)
	{
		ID3D11ShaderResourceView* imageSRV{};
		HR(D3DX11CreateShaderResourceViewFromFile(m_Device,
			c2w(m_ImageName[i].c_str(),m_ImageName[i].size()).c_str(), 0, 0, &imageSRV, 0));
		m_imageSRV.push_back(imageSRV);
	}
}
//创建顶点和索引缓存
void D3DGame::GameStart::create()
{
	GeometryGenerator::MeshData grid;
	GeometryGenerator geoGen;
	geoGen.CreateBox(2.0f, 2.0f, 2.0f, grid);
	std::vector<Vertex::Basic32> vertices(4);
	for (size_t i = 0; i != 4; ++i)
	{
		vertices[i].Pos			=	 grid.Vertices[i].Position;
		vertices[i].Pos.z    += 1.0f;
		vertices[i].Normal	=	 grid.Vertices[i].Normal;
		vertices[i].Tex			=	 grid.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * 4;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(m_Device->CreateBuffer(&vbd, &vinitData, &m_VB));

	m_Indices.insert(m_Indices.end(), grid.Indices.begin(), grid.Indices.end());
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 6;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &m_Indices[0];
	HR(m_Device->CreateBuffer(&ibd, &iinitData, &m_IB));
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_DeviceContext->IASetInputLayout(InputLayouts::Basic32);
}


void D3DGame::GameStart::draw(IDXGISwapChain* SwapChain,	ID3D11RenderTargetView* RenderTargetView,	ID3D11DepthStencilView* DepthStencilView)
{
	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;	
	m_DeviceContext->IASetVertexBuffers(0, 1, &m_VB, &stride, &offset);
	m_DeviceContext->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, 0);

	XMFLOAT4X4 FloatI;
	XMMATRIX I =  XMMatrixIdentity();
	XMStoreFloat4x4(&FloatI, I);

	Material Mat;
	Mat.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Mat.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 64.0f);
	Mat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	DirectionalLight DirLight[3];
	DirLight[0].Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	DirLight[0].Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	DirLight[0].Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
	DirLight[0].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

	//设置着色器
	Effects::BasicFX->SetTexTransform(I);
	Effects::BasicFX->SetMaterial(Mat);
	Effects::BasicFX->SetWorld(I);
	Effects::BasicFX->SetWorldInvTranspose(I);
	Effects::BasicFX->SetWorldViewProj(I);
	Effects::BasicFX->SetDirLights(DirLight);

	ID3DX11EffectTechnique* tech = Effects::BasicFX->Light1TexTech;
	D3DX11_TECHNIQUE_DESC techDesc;	
	tech->GetDesc(&techDesc);
	ID3DX11EffectPass* pass = tech->GetPassByIndex(0);

	for (int i = 0; i != m_ImageName.size(); ++i)
	{

		m_DeviceContext->ClearRenderTargetView(RenderTargetView, reinterpret_cast<const float*>(&Colors::Blue));
		m_DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		Effects::BasicFX->SetDiffuseMap(m_imageSRV[i]);
			pass->Apply(0, m_DeviceContext);
			m_DeviceContext->DrawIndexed(6, 0, 0);
		HR(SwapChain->Present(0, 0));
		Sleep(3000);
	}
}

void D3DGame::GameStart::getFiles(std::string path, std::vector<std::string>& files)
{
	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			//如果不是,加入列表  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

std::wstring D3DGame::GameStart::c2w(LPCSTR pszSrc, int nLen)
{
	int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);
	if (nSize <= 0) return NULL;

	WCHAR *pwszDst = new WCHAR[nSize + 1];
	if (NULL == pwszDst) return NULL;

	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, pwszDst, nSize);
	pwszDst[nSize] = 0;

	if (pwszDst[0] == 0xFEFF) // skip Oxfeff  
		for (int i = 0; i < nSize; i++)
			pwszDst[i] = pwszDst[i + 1];

	std::wstring wcharString(pwszDst);
	delete pwszDst;

	return wcharString;
}