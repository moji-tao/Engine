#include <cassert>
#include "EngineRuntime/include/Function/Render/DirectX/D3D12InputLayout.h"

namespace Engine
{
	void InputLayoutManager::AddInputLayout(const std::string& name, const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayout)
	{
		assert(mInputLayoutMap.find(name) == mInputLayoutMap.end());
		
		mInputLayoutMap.emplace(name, inputLayout);
	}

	void InputLayoutManager::GetInputLayout(const std::string& name, std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayout) const
	{
		auto it = mInputLayoutMap.find(name);
		if(it == mInputLayoutMap.end())
		{
			assert(0);
		}
		else
		{
			inputLayout = it->second;
		}
	}
}
