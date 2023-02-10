#pragma once
#include <string>
#include "EngineRuntime/include/Core/Random/GUID.h"

namespace Engine
{
	class Rtti
	{
	public:
		Rtti(const char* rttiName, const Rtti* baseRtti);

		~Rtti();

		const std::string& GetName() const;

		bool IsSameType(const Rtti& type) const;

		bool CanCastTo(const Rtti& type) const;

		const Rtti* GetBase() const;

	public:
		std::string mName;
		const Rtti* mBase;
	};

#define DECLARE_RTTI														\
	public:																	\
		virtual const Engine::Rtti& GetType() const;						\
		static const Engine::Rtti mType;				

#define IMPLEMENT_RTTI(classname, baseclassname)							\
	const Engine::Rtti& classname::GetType() const							\
	{																		\
		return mType;														\
	}																		\
	const Engine::Rtti classname::mType(#classname, &baseclassname::mType);


	class Object
	{
		DECLARE_RTTI
	public:
		Object() = default;

		virtual ~Object() = default;

	public:
		void SetGuid(const GUID& guid);

		GUID GetGuid() const;

	protected:
		GUID mGuid;
	};

	template <typename T, typename F>
	T* StaticCast(F* obj)
	{
		return (T*)obj;
	}

	template <typename T, typename F>
	const T* StaticCast(const F* obj)
	{
		return (const T*)obj;
	}

	template <typename T, typename F>
	T* DynamicCast(F* obj)
	{
		return obj && obj->GetType().CanCastTo(T::mType) ? (T*)obj : nullptr;
	}

	template <typename T, typename F>
	const T* DynamicCast(const F* obj)
	{
		return obj && obj->GetType().CanCastTo(T::mType) ? (T*)obj : nullptr;
	}

}
