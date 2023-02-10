#pragma once
#include <string>

namespace Engine
{
	class GUID
	{
	public:
		GUID();

		GUID(const std::string& str);

		GUID(const GUID& g);

		const char* Data() const;

		bool IsVaild() const;

		void SetNoVaild();

		static GUID Get();

		static uint16_t Size();

		bool operator==(const GUID& otr) const;

	private:
		//char mData[33] = { 0 };
		std::string mD;

		friend class SerializerDataFrame;
	};
}

namespace std
{
	template<>
	struct std::hash<Engine::GUID>
	{
	public:
		size_t operator()(const Engine::GUID& p) const
		{
			hash<string> hs;
			return hs(p.Data());
		}
	};
}
