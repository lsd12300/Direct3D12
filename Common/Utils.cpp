#include "Utils.h"
#include "stdafx.h"


CD3DX12_RESOURCE_DESC Utils::Buffer(UINT64 width, D3D12_RESOURCE_FLAGS flags, UINT64 alignment)
{
    return CD3DX12_RESOURCE_DESC(D3D12_RESOURCE_DIMENSION_BUFFER,
        alignment, width, 1, 1, 1,
        DXGI_FORMAT_UNKNOWN, 1, 0,
        D3D12_TEXTURE_LAYOUT_ROW_MAJOR, flags);
}
