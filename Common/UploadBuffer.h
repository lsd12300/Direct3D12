#pragma once
#include "stdafx.h"
#include "Utils.h"


// �ϴ�������ģ����.
//    �ϴ����ڻ�����: CPU �� GPU ���ܷ���, �ٶȱ�Ĭ�϶���
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
			0, // ����Դ����,  ָ����ӳ�������Դ.
			nullptr, // D3D12_RANGE ָ��, �������ڴ��ӳ�䷶Χ.  nullptr--��ʾ������Դ
			reinterpret_cast<void**>(&mMappedData))); // ��˫��ָ�뷵�� ��ӳ����Դ���ݵ�Ŀ���ڴ��.  ͨ��ʹ�� memcpy ��ϵͳ�ڴ渴�Ƶ�����������
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

	// �Ƿ��� ����������
	bool mIsConstantBuffer = false;

	// �������ֽڴ�С
	UINT mElementByteSize = 0;
};
