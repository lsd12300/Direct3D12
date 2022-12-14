#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "stdafx.h"
#include "GameTimer.h"


#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")


class D3DApp
{
protected:
    D3DApp(HINSTANCE hInstance);
    D3DApp(const D3DApp& rhs) = delete; // " = delete"  表示禁用函数.  此处为禁止拷贝构造函数
    D3DApp& operator=(const D3DApp& rhs) = delete;
    virtual ~D3DApp();


public:
    static D3DApp* GetApp();

    HINSTANCE AppInst()const;
    HWND      MainWnd()const;
    float     AspectRatio()const;

    bool Get4xMsaaState()const;
    void Set4xMsaaState(bool value);

    int Run();

    virtual bool Initialize();
    virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
    virtual void CreateRtvAndDsvDescriptorHeaps();
    virtual void OnResize();
    virtual void Update(const GameTimer& gt) = 0; // "= 0"  全虚函数.  子类必须实现
    virtual void Draw(const GameTimer& gt) = 0;

    // 处理鼠标事件
    virtual void OnMouseDown(WPARAM btnState, int x, int y) { }
    virtual void OnMouseUp(WPARAM btnState, int x, int y) { }
    virtual void OnMouseMove(WPARAM btnState, int x, int y) { }


protected:

    bool InitMainWindow();
    bool InitDirect3D();
    void CreateCommandObjects();
    void CreateSwapChain();

    void FlushCommandQueue();

    ID3D12Resource* CurrentBackBuffer()const;
    D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
    D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;

    void CalculateFrameStats();

    void LogAdapters();
    void LogAdapterOutputs(IDXGIAdapter* adapter);
    void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);


protected:

    static D3DApp* mApp;

    HINSTANCE mhAppInst = nullptr; // 应用实例句柄
    HWND      mhMainWnd = nullptr; // 主窗口句柄
    bool      mAppPaused = false;
    bool      mMinimized = false;
    bool      mMaximized = false;
    bool      mResizing = false;   // 窗口尺寸变化
    bool      mFullscreenState = false;// 全屏

    bool      m4xMsaaState = false;    // 4X MSAA 是否开启
    UINT      m4xMsaaQuality = 0;      // 4X MSAA 采样等级

    GameTimer mTimer;

    ComPtr<IDXGIFactory4> mdxgiFactory;
    ComPtr<IDXGISwapChain> mSwapChain;
    ComPtr<ID3D12Device> md3dDevice;

    ComPtr<ID3D12Fence> mFence;
    UINT64 mCurrentFence = 0;

    ComPtr<ID3D12CommandQueue> mCommandQueue;
    ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
    ComPtr<ID3D12GraphicsCommandList> mCommandList;

    static const int SwapChainBufferCount = 2;
    int mCurrBackBuffer = 0;
    ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
    ComPtr<ID3D12Resource> mDepthStencilBuffer;

    ComPtr<ID3D12DescriptorHeap> mRtvHeap;
    ComPtr<ID3D12DescriptorHeap> mDsvHeap;

    D3D12_VIEWPORT mScreenViewport;
    D3D12_RECT mScissorRect;

    UINT mRtvDescriptorSize = 0;
    UINT mDsvDescriptorSize = 0;
    UINT mCbvSrvUavDescriptorSize = 0;

    // 子类需要在构造函数里 自定义的参数
    std::wstring mMainWndCaption = L"d3d App";
    D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
    DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    int mClientWidth = 800;
    int mClientHeight = 600;
};
