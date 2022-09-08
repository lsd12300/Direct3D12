
#include "../Common/D3DApp.h"
#include <DirectXColors.h>
#include <array>
#include "../Common/Utils.h"
#include "../Common/MathHelper.h"
#include "../Common/UploadBuffer.h"

using namespace DirectX;


struct Vertex1
{
    XMFLOAT3 Pos; // XMFloat 占4字节.
    XMFLOAT3 Color;
};

struct Vertex2
{
    XMFLOAT3 Pos;
    XMFLOAT3 Normal;
    XMFLOAT2 Tex0;
    XMFLOAT2 Tex1;
};

struct ObjectConstants
{
    XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
};


class InitDirect3DApp : public D3DApp
{

public:
	InitDirect3DApp(HINSTANCE hInstance);
	~InitDirect3DApp();

	virtual bool Initialize() override;

private:
    XMFLOAT2 mLastMousePos; // 上帧鼠标位置
    float mTheta; // Box旋转
    float mPhi;
    float mRadius; // 相机可视范围半径

    XMFLOAT4X4 mWorld; // 模型空间到世界空间的 变换矩阵
    XMFLOAT4X4 mView; // 世界空间到相机空间的 变换矩阵
    XMFLOAT4X4 mProj; // 相机空间到裁剪空间的 变换矩阵

    std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB; // 常量缓冲区,  存储n个物体的数据


	virtual void OnResize() override;
	virtual void Update(const GameTimer& gt) override;
	virtual void Draw(const GameTimer& gt) override;

    virtual void OnMouseMove(WPARAM btnState, int x, int y) override;


    // 绘制几何体
    void DrawGeometry();

    // 常量缓冲区
    void BuildConstantBuffers();
};


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE prevInstance, _In_ LPSTR cmdLine, _In_ int showCmd)
{
    // 开启  运行时内存检测
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    try
    {
        InitDirect3DApp theApp(hInstance);
        if (!theApp.Initialize())
            return 0;

        return theApp.Run();
    }
    catch (DxException& e)
    {
        MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
        return 0;
    }
}


InitDirect3DApp::InitDirect3DApp(HINSTANCE hInstance)
	: D3DApp(hInstance)
{
}

InitDirect3DApp::~InitDirect3DApp()
{
}

bool InitDirect3DApp::Initialize()
{
    if (!D3DApp::Initialize())
        return false;

    DrawGeometry();

    return true;
}

void InitDirect3DApp::OnResize()
{
    D3DApp::OnResize(); // 调用父类方法
}

void InitDirect3DApp::Update(const GameTimer& gt)
{
    // 旋转  球坐标 转换 笛卡尔坐标
    float x = mRadius * sinf(mPhi) * cosf(mTheta);
    float y = mRadius * sinf(mPhi) * sinf(mTheta);
    float z = mRadius * cosf(mPhi);

    // 观察矩阵
    XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
    XMStoreFloat4x4(&mView, view);

    XMMATRIX world = XMLoadFloat4x4(&mWorld);
    XMMATRIX proj = XMLoadFloat4x4(&mProj);
    XMMATRIX worldViewProj = world * view * proj;

    // 更新常量缓冲区
    ObjectConstants objConstants;
    XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
    mObjectCB->CopyData(0, objConstants);
}

// 每帧绘制
void InitDirect3DApp::Draw(const GameTimer& gt)
{
    // 重复使用 记录命令的相关内存
    //  只有在 与GPU相关联的命令列表执行完成时  才能重置
    ThrowIfFailed(mDirectCmdListAlloc->Reset());
    ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

    // 转换资源状态.  从 显示状态 转换为 渲染目标状态
    CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);
    mCommandList->ResourceBarrier(1, &resourceBarrier);

    // 设置视口和裁剪矩形
    mCommandList->RSSetViewports(1, &mScreenViewport);
    mCommandList->RSSetScissorRects(1, &mScissorRect);

    // 清除后台缓冲区 和 深度缓冲区
    mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
    mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    // 指定要渲染的缓冲区
    D3D12_CPU_DESCRIPTOR_HANDLE curBackBufferView = CurrentBackBufferView();
    D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = DepthStencilView();
    mCommandList->OMSetRenderTargets(1, &curBackBufferView, true, &depthStencilView);

    // 转换资源.  从 渲染目标状态 转换为 显示状态
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);
    mCommandList->ResourceBarrier(1, &barrier);

    // 停止记录命令
    ThrowIfFailed(mCommandList->Close());

    // 将 命令加入 命令队列
    ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    // 交换 后台缓冲区和前台缓冲区
    ThrowIfFailed(mSwapChain->Present(0, 0));
    mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

    // 等待命令执行完毕
    FlushCommandQueue();
}

void InitDirect3DApp::OnMouseMove(WPARAM btnState, int x, int y)
{
    if ((btnState & MK_LBUTTON) != 0) {
        // 根据鼠标左键移动距离 计算旋转角度.
        float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
        float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

        // 根据鼠标输入 更新相机绕立方体旋转的角度
        mTheta += dx;
        mPhi += dy;

        mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
    }
    else if ((btnState & MK_RBUTTON) != 0)
    {
        // 根据鼠标右键移动距离 计算缩放
        float dx = 0.005f * static_cast<float>(x - mLastMousePos.x);
        float dy = 0.005f * static_cast<float>(y - mLastMousePos.y);

        mRadius += dx - dy;
        mRadius = MathHelper::Clamp(mRadius, 3.0f, 15.0f);
    }

    mLastMousePos.x = x;
    mLastMousePos.y = y;
}

void InitDirect3DApp::DrawGeometry()
{
    // 输入数据 布局
    D3D12_INPUT_ELEMENT_DESC desc1[] = {
        //{语义, 语义后缀编号, 数据格式, 输入槽索引, 输入槽中首地址起始偏移, 实例化或顶点数数据, 实例化中数据步进值}
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };
    D3D12_INPUT_ELEMENT_DESC desc2[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };


    // 矩形顶点数据
    std::array<Vertex1, 8> vertices = {
        Vertex1({XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(Colors::White)}),
        Vertex1({XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT3(Colors::Black)}),
        Vertex1({XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT3(Colors::Red)}),
        Vertex1({XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT3(Colors::Green)}),
        Vertex1({XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT3(Colors::Blue)}),
        Vertex1({XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT3(Colors::Yellow)}),
        Vertex1({XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT3(Colors::Cyan)}),
        Vertex1({XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT3(Colors::Magenta)})
    };
    std::array<std::uint16_t, 36> indices = {
        // 前
        0,1,2,
        0,2,3,

        // 后
        4,6,5,
        4,7,6,

        // 左
        4,5,1,
        4,1,0,

        // 右
        3,2,6,
        3,6,7,

        // 上
        1,5,6,
        1,6,2,

        // 下
        4,0,3,
        4,3,7
    };

    const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex1);
    const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

    // 默认堆缓冲区
    ComPtr<ID3D12Resource> vertexBufferGPU = nullptr;
    ComPtr<ID3D12Resource> vertexBufferUploader = nullptr;
    vertexBufferGPU = Utils::CreateDefaultBuffer(md3dDevice.Get(), mCommandList.Get(), &vertices, vbByteSize, vertexBufferUploader);

    // 绑定到渲染流水线
    D3D12_VERTEX_BUFFER_VIEW vbv; // 顶点缓冲区视图
    vbv.BufferLocation = vertexBufferGPU->GetGPUVirtualAddress();
    vbv.StrideInBytes = sizeof(Vertex1); // 每个元素字节数
    vbv.SizeInBytes = 8 * sizeof(Vertex1); // 整个视图字节数
    
    // 绑定到  输入装配器阶段
    D3D12_VERTEX_BUFFER_VIEW vertexBuffers[1] = { vbv };
    mCommandList->IASetVertexBuffers(
        0, // 起始输入槽
        1, // 要绑定到输入槽的顶点缓冲区数量
        vertexBuffers); // 顶点缓冲区数组地址


    // 索引缓冲区
    ComPtr<ID3D12Resource> indexBufferGPU = nullptr;
    ComPtr<ID3D12Resource> indexBufferUploader = nullptr;
    indexBufferGPU = Utils::CreateDefaultBuffer(md3dDevice.Get(), mCommandList.Get(), &indices, ibByteSize, indexBufferUploader);

    D3D12_INDEX_BUFFER_VIEW ibv;
    ibv.BufferLocation = indexBufferGPU->GetGPUVirtualAddress();
    ibv.Format = DXGI_FORMAT_R16_UINT;
    ibv.SizeInBytes = ibByteSize;

    mCommandList->IASetIndexBuffer(&ibv);


    mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 绘制的 拓扑图形
    mCommandList->DrawIndexedInstanced(
        indices.size(), // 每个物体实例顶点索引数量
        1, // 要实例化的数量. GPU实例化参数
        0, // 指定顶点缓冲区内第一个要绘制的顶点索引
        0, // 本次绘制每个顶点索引偏移的值.  将多个顶点数组合并成一个后, 顶点索引不再是从0开始. 如: 两个三角形顶点合并成6元素顶点数组后, 第二个三角形的顶点索引从3开始
        0); // GPU实例化参数
}

void InitDirect3DApp::BuildConstantBuffers()
{
    mObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(md3dDevice.Get(), 1, true);
}