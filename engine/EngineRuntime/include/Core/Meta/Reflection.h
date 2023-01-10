#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace Engine
{
	class Class;
	class Method;
	class Field;
	class ClassFactory;

	typedef void* ReflectObjectPtr;

	typedef ReflectObjectPtr(*create_object)();

	class ClassRegister
	{
	public:
		ClassRegister(const std::string& className, create_object method);

		ClassRegister(const std::string& className, const std::string& fieldName, const std::string& fieldType, size_t offset);

		ClassRegister(const std::string& className, const std::string& methodName, uintptr_t method);
	};

#define REGISTER_CLASS(className)																	\
	Engine::ReflectObjectPtr createObject##className()												\
	{																								\
		Engine::ReflectObjectPtr obj = new className();												\
		return obj;																					\
	}																								\
	Engine::ClassRegister class_Register__##className(#className, createObject##className);	

#define REGISTER_CLASS_FIELD(className, fieldName, fieldType)																						\
	Engine::ClassRegister class_Register__##className##fieldName##fieldType(#className, #fieldName, #fieldType, offsetof(className, fieldName));

#define REGISTER_CLASS_METHOD(className, methodName, returnType, ...)																				\
	std::function<returnType(className*, ##__VA_ARGS__)> className##methodName##method##REGISIER_CLASS_METHOD = &className::methodName;				\
	Engine::ClassRegister class_Register__##className##methodName##returnType(#className, #methodName, (uintptr_t)&(className##methodName##method##REGISIER_CLASS_METHOD ));	

	class ClassFactory
	{
	public:
		static ClassFactory* GetInstance();

	public:
		Class* GetClassForName(const std::string& className);

	private:
		void RegisterClass(const std::string& className, create_object method);

		void RegisterClassField(const std::string& className, const std::string& fieldName, const std::string& fieldType, size_t offset);

		void RegisterClassMethod(const std::string& className, const std::string& methodName, uintptr_t method);

	private:
		std::unordered_map<std::string, Class*> mClasses;

		friend ClassRegister;
	};

	class Class
	{
	public:
		Class(const std::string& className, create_object method);

		std::string GetClassName();

		Field GetField(const std::string& fieldName);

		const std::vector<Field>& GetFields();

		Method GetMethod(const std::string& methodName);

		const std::vector<Method>& GetMethods();

		ReflectObjectPtr SpanClass();

	public:
		static Class* ForName(const std::string className);

	private:
		create_object mCreateMethod;

		std::string mClassName;

		std::unordered_map<std::string, size_t> mFieldsIndex;

		std::vector<Field> mFields;

		std::unordered_map<std::string, size_t> mMethodsIndex;

		std::vector<Method> mMethods;

		friend class ClassFactory;
	};

	class Method
	{
	public:
		Method(const std::string& methodName, uintptr_t methodPtr);

		template <typename R = void, typename ...Args>
		R Call(ReflectObjectPtr object, Args... args)
		{
			typedef std::function<R(decltype(object), Args...)> func;
			return (*((func*)mMethodPtr))(object, args...);
		}

		std::string MethodName;

	private:
		uintptr_t mMethodPtr;
	};

	class Field
	{
	public:
		Field(const std::string& fieldName, const std::string& fieldType, size_t fieldOffset);
		const std::string FieldName;
		const std::string FieldType;

		template <typename T>
		void Get(const ReflectObjectPtr object, T& value) const
		{
			value = *(T*)(((unsigned char*)object) + mFieldOffset);
		}

		template <typename T>
		T& Get(const ReflectObjectPtr object) const
		{
			return *(T*)(((unsigned char*)object) + mFieldOffset);
		}

		template <typename T>
		void Set(ReflectObjectPtr object, const T& value) const
		{
			*(T*)((unsigned char*)object + mFieldOffset) = value;
		}

	private:
		const size_t mFieldOffset;
	};
}
