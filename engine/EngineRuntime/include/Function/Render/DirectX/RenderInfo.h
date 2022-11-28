#pragma once
#ifdef _WIN64
#include <d3d12.h>

namespace Engine
{

	class RenderGemoetry
	{
	public:
		D3D12_VERTEX_BUFFER_VIEW GetVectexBufferView() const;

		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

	private:

	};

	struct RenderItem
	{
		
	};
}

#endif
