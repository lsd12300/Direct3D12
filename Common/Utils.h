#pragma once

#include "stdafx.h"


class Utils
{
public:
	// GPU缓冲区 的描述符
	static CD3DX12_RESOURCE_DESC Buffer(UINT64 width, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE, UINT64 alignment = 0);


	

private:

};