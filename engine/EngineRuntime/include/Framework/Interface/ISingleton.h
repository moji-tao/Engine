#pragma once

namespace Engine
{
	template <class T>
	class ISingleton {
	public:
		static T* GetInstance()
		{
			static T v;
			return &v;
		}
	};
}
