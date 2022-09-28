#pragma once
#include "stdafx.h"
#include "Utils.h"


// 上传缓冲区模板类.
//    上传堆内缓冲区: CPU 和 GPU 都能访问, 速度比默认堆慢
template<typename T>
class UploadBuffer
{
public:
	UploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer) :
		mIsConstantBuffer(isConstantBuffer)
	{
		mElementByteSize = sizeof(T);

		if (mIsConstantBuffer)
			mElementByteSize = Utils::CalcConstantBufferByteSize(sizeof(T));

		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(mElementByteSize * elementCount);
		ThrowIfFailed(device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&mUploadBuffer)));

		ThrowIfFailed(mUploadBuffer->Map(
			0, // 子资源索引,  指向欲映射的子资源.
			nullptr, // D3D12_RANGE 指针, 描述了内存的映射范围.  nullptr--表示整个资源
			reinterpret_cast<void**>(&mMappedData))); // 用双重指针返回 待映射资源数据的目标内存块.  通常使用 memcpy 从系统内存复制到常量缓冲区
	}


	UploadBuffer(const UploadBuffer& rhs) = delete;
	UploadBuffer& operator=(const UploadBuffer& rhs) = delete;

	~UploadBuffer() {
		if (mUploadBuffer != nullptr)
			mUploadBuffer->Unmap(0, nullptr);

		mMappedData = nullptr;
	}

	ID3D12Resource* Resource() const {
		return mUploadBuffer.Get();
	}

	void CopyData(int elementIndex, const T& data) {
		memcpy(&mMappedData[elementIndex * mElementByteSize], &data, sizeof(T));
	}

private:
	ComPtr<ID3D12Resource> mUploadBuffer;
	BYTE* mMappedData = nullptr;

	// 是否是 常量缓冲区
	bool mIsConstantBuffer = false;

	// 缓冲区字节大小
	UINT mElementByteSize = 0;
};
