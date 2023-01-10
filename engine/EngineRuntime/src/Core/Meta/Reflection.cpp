#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Core/Meta/Reflection.h"

namespace Engine
{
	ClassFactory* ClassFactory::GetInstance()
	{
		static ClassFactory v;
		return &v;
	}

	Class* ClassFactory::GetClassForName(const std::string& className)
	{
		auto it = mClasses.find(className);
		if (it == mClasses.end())
		{
			return nullptr;
		}
		return it->second;
	}

	void ClassFactory::RegisterClass(const std::string& className, create_object method)
	{
		auto it = mClasses.find(className);
		if (it != mClasses.end())
		{
			ASSERT(0);
		}

		Class* cs = new Class(className, method);
		mClasses.emplace(className, cs);
	}


	void ClassFactory::RegisterClassField(const std::string& className, const std::string& fieldName, const std::string& fieldType, size_t offset)
	{
		auto it1 = mClasses.find(className);

		if (it1 == mClasses.end())
		{
			ASSERT(0);
		}

		Class* cs = it1->second;
		auto it2 = cs->mFieldsIndex.find(fieldName);
		if (it2 != cs->mFieldsIndex.end())
		{
			ASSERT(0);
		}

		cs->mFieldsIndex.emplace(fieldName, cs->mFields.size());
		cs->mFields.emplace_back(fieldName, fieldType, offset);
	}

	void ClassFactory::RegisterClassMethod(const std::string& className, const std::string& methodName, uintptr_t method)
	{
		auto it1 = mClasses.find(className);

		if (it1 == mClasses.end())
		{
			ASSERT(0);
		}

		Class* cs = it1->second;
		auto it2 = cs->mMethodsIndex.find(methodName);
		if (it2 != cs->mMethodsIndex.end())
		{
			ASSERT(0);
		}

		cs->mMethodsIndex.emplace(methodName, cs->mMethods.size());
		cs->mMethods.emplace_back(methodName, method);
	}

	Class::Class(const std::string& className, create_object method)
		:mClassName(className), mCreateMethod(method)
	{ }

	std::string Class::GetClassName()
	{
		return mClassName;
	}

	Field Class::GetField(const std::string& fieldName)
	{
		auto it = mFieldsIndex.find(fieldName);

		if (it == mFieldsIndex.end() || it->second > mFields.size())
		{
			ASSERT(0);
		}

		return mFields[it->second];
	}

	const std::vector<Field>& Class::GetFields()
	{
		return mFields;
	}

	Method Class::GetMethod(const std::string& methodName)
	{
		auto it = mMethodsIndex.find(methodName);

		if (it == mMethodsIndex.end() || it->second > mMethods.size())
		{
			ASSERT(0);
		}

		return mMethods[it->second];
	}

	const std::vector<Method>& Class::GetMethods()
	{
		return mMethods;
	}

	ReflectObjectPtr Class::SpanClass()
	{
		return mCreateMethod();
	}

	Class* Class::ForName(const std::string className)
	{
		return ClassFactory::GetInstance()->GetClassForName(className);
	}

	Field::Field(const std::string& fieldName, const std::string& fieldType, size_t fieldOffset)
		:FieldName(fieldName), FieldType(fieldType), mFieldOffset(fieldOffset)
	{ }

	Method::Method(const std::string& methodName, uintptr_t methodPtr)
		:MethodName(methodName), mMethodPtr(methodPtr)
	{ }

	ClassRegister::ClassRegister(const std::string& className, create_object method)
	{
		ClassFactory::GetInstance()->RegisterClass(className, method);
	}

	ClassRegister::ClassRegister(const std::string& className, const std::string& fieldName, const std::string& fieldType, size_t offset)
	{
		ClassFactory::GetInstance()->RegisterClassField(className, fieldName, fieldType, offset);
	}

	ClassRegister::ClassRegister(const std::string& className, const std::string& methodName, uintptr_t method)
	{
		ClassFactory::GetInstance()->RegisterClassMethod(className, methodName, method);
	}

}
