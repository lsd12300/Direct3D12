#pragma once

#include "stdafx.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")


class Utils
{
public:
	// GPU缓冲区 的描述符
	static CD3DX12_RESOURCE_DESC Buffer(UINT64 width, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE, UINT64 alignment = 0);


	/// <summary>
	///  创建默认堆缓冲区(仅 GPU 能访问, 效率高)
	///		三类缓冲区:
	///			1. 系统内存缓冲区.   仅 CPU访问, 速度快
	///			2. 上传缓冲区.  CPU和GPU都能访问, 速度较慢
	///			3. 默认堆缓冲区.  仅 GPU访问,  速度快
	/// 
	///		创建默认堆缓冲区步骤:
	///			1. 创建系统内存缓冲区
	///			2. 提交到 上传缓冲区
	///			3. 复制到 默认堆缓冲区
	/// </summary>
	/// <param name="device"></param>
	/// <param name="cmdList"></param>
	/// <param name="initData"></param>
	/// <param name="byteSize"></param>
	/// <param name="uploadBuffer"></param>
	/// <returns></returns>
	ComPtr<ID3D12Resource> static CreateDefaultBuffer(
		ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
		const void* initData, UINT64 byteSize, ComPtr<ID3D12Resource>& uploadBuffer);

	// 计算常量缓冲区大小(必须是 256B的整数倍)
	static UINT CalcConstantBufferByteSize(UINT byteSize);

private:

};