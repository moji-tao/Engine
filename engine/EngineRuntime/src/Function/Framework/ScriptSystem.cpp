#include "EngineRuntime/include/Function/Framework/ScriptSystem.h"
#include "EngineRuntime/include/Platform/FileSystem/FileSystem.h"
#include "EngineRuntime/include/Core/Math/Vector2.h"
#include "EngineRuntime/include/Core/Math/Vector3.h"
#include "EngineRuntime/include/Core/Math/Vector4.h"
#include "EngineRuntime/include/Core/Math/Matrix3x3.h"
#include "EngineRuntime/include/Core/Math/Matrix4x4.h"
#include "EngineRuntime/include/Core/Math/Quaternion.h"
#include "EngineRuntime/include/Core/Math/Transform.h"
#include "EngineRuntime/include/Function/Framework/Object/Actor.h"
#include "EngineRuntime/include/Function/Framework/Object/GameObject.h"

namespace Engine
{
	ScriptSystem::ScriptSystem()
	{
		
	}

	ScriptSystem::~ScriptSystem()
	{
		
	}

	bool ScriptSystem::Initialize()
	{
		mLuaState.open_libraries(sol::lib::base,
			sol::lib::package,
			sol::lib::coroutine,
			sol::lib::string,
			sol::lib::os,
			sol::lib::math,
			sol::lib::table,
			sol::lib::debug,
			sol::lib::bit32,
			sol::lib::io,
			sol::lib::utf8);

		InitLuaGlobal();

		InitLuaPath();

		return true;
	}

	bool ScriptSystem::Tick(float deltaTile, bool isEditorMode)
	{
		return true;
	}

	void ScriptSystem::Finalize()
	{

	}

	void ScriptSystem::InitLuaGlobal()
	{
		{
			auto tb = mLuaState["Vector2"].get_or_create<sol::table>();
			auto usertype = tb.new_usertype<Vector2>("Vector2", sol::call_constructor, sol::constructors<Vector2()>());
			usertype.set("x", &Vector2::x);
			usertype.set("y", &Vector2::y);
		}

		{
			auto tb = mLuaState["Vector3"].get_or_create<sol::table>();
			auto usertype = tb.new_usertype<Vector3>("Vector3", sol::call_constructor, sol::constructors<Vector3()>());
			usertype.set("x", &Vector3::x);
			usertype.set("y", &Vector3::y);
			usertype.set("z", &Vector3::z);
		}

		{
			auto tb = mLuaState["Vector4"].get_or_create<sol::table>();
			auto usertype = tb.new_usertype<Vector4>("Vector4", sol::call_constructor, sol::constructors<Vector4()>());
			usertype.set("x", &Vector4::x);
			usertype.set("y", &Vector4::y);
			usertype.set("z", &Vector4::z);
			usertype.set("w", &Vector4::w);
		}

		{
			auto tb = mLuaState["Matrix3x3"].get_or_create<sol::table>();
			auto usertype = tb.new_usertype<Matrix3x3>("Matrix3x3", sol::call_constructor, sol::constructors<Matrix3x3()>());
			usertype.set("m00", &Matrix3x3::m00);
			usertype.set("m01", &Matrix3x3::m01);
			usertype.set("m02", &Matrix3x3::m02);
			usertype.set("m10", &Matrix3x3::m10);
			usertype.set("m11", &Matrix3x3::m11);
			usertype.set("m12", &Matrix3x3::m12);
			usertype.set("m20", &Matrix3x3::m20);
			usertype.set("m21", &Matrix3x3::m21);
			usertype.set("m22", &Matrix3x3::m22);
		}
		
		{
			auto tb = mLuaState["Matrix4x4"].get_or_create<sol::table>();
			auto usertype = tb.new_usertype<Matrix4x4>("Matrix4x4", sol::call_constructor, sol::constructors<Matrix4x4()>());
			usertype.set("m00", &Matrix4x4::m00);
			usertype.set("m01", &Matrix4x4::m01);
			usertype.set("m02", &Matrix4x4::m02);
			usertype.set("m03", &Matrix4x4::m03);
			usertype.set("m10", &Matrix4x4::m10);
			usertype.set("m11", &Matrix4x4::m11);
			usertype.set("m12", &Matrix4x4::m12);
			usertype.set("m13", &Matrix4x4::m13);
			usertype.set("m20", &Matrix4x4::m20);
			usertype.set("m21", &Matrix4x4::m21);
			usertype.set("m22", &Matrix4x4::m22);
			usertype.set("m23", &Matrix4x4::m23);
			usertype.set("m30", &Matrix4x4::m30);
			usertype.set("m31", &Matrix4x4::m31);
			usertype.set("m32", &Matrix4x4::m32);
			usertype.set("m33", &Matrix4x4::m33);
		}
		
		{
			auto tb = mLuaState["Quaternion"].get_or_create<sol::table>();
			auto usertype = tb.new_usertype<Quaternion>("Quaternion", sol::call_constructor, sol::constructors<Quaternion()>());
			usertype.set("x", &Quaternion::x);
			usertype.set("y", &Quaternion::y);
			usertype.set("z", &Quaternion::z);
			usertype.set("w", &Quaternion::w);
		}
		
		{
			auto tb = mLuaState["Transform"].get_or_create<sol::table>();
			auto usertype = tb.new_usertype<Transform>("Transform", sol::call_constructor, sol::constructors<Transform()>());
			usertype.set("position", &Transform::position);
			usertype.set("rotation", &Transform::rotation);
			usertype.set("scale", &Transform::scale);
		}
		

		auto cppGlobalTable = mLuaState["CPP"].get_or_create<sol::table>();

		{
			auto usertype = cppGlobalTable.new_usertype<GameObject>("Object");
			usertype.set_function("SetLuaTableInstance", &GameObject::SetLuaTableInstance);
			usertype.set_function("Awake", &GameObject::Awake);
			usertype.set_function("Tick", &GameObject::Tick);
			usertype.set_function("OnDestroy", &GameObject::OnDestroy);
			usertype.set_function("OnEnable", &GameObject::OnEnable);
		}

		{
			auto usertype = cppGlobalTable.new_usertype<Component>("Component", sol::base_classes, sol::bases<GameObject>());
			usertype.set_function("LuaAttachParent", &Component::LuaAttachParent);
		}
		
		{
			auto usertype = cppGlobalTable.new_usertype<Actor>("GameObject", sol::call_constructor, sol::constructors<Actor()>(),
				sol::base_classes, sol::bases<GameObject>());
		}
	}

	void ScriptSystem::InitLuaPath()
	{
		sol::table packageTable = mLuaState["package"];
		std::string ph = packageTable["path"];
		std::string projScript = EngineFileSystem::GetInstance()->GetActualPath("Assets/Scripts/?.lua").generic_string();
		std::string engScript = EngineFileSystem::GetInstance()->GetActualPath("Scripts/?.lua").generic_string();

		ph.append(";" + projScript);
		ph.append(";" + engScript);
		packageTable["path"] = ph;
	}
}
