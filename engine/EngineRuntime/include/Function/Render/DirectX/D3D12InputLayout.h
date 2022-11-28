#pragma once
#include <d3d12.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace Engine
{
	class InputLayoutManager
	{
	public:
		void AddInputLayout(const std::string& name, const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayout);

		void GetInputLayout(const std::string& name, std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayout) const;

	private:
		std::unordered_map<std::string, std::vector<D3D12_INPUT_ELEMENT_DESC>> mInputLayoutMap;
	};
}
