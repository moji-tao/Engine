#pragma once
#include <filesystem>
#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include "EngineRuntime/include/core/base/macro.h"
#include "EngineRuntime/include/Platform/FileSystem/FileSystem.h"

namespace Engine
{
	class ISerializable;
	class Vector2;
	class Vector3;
	class Vector4;
	class Quaternion;
	class Transform;
	class Matrix3x3;
	class Matrix4x4;
	struct Vertex;

	class SerializerDataFrame
	{
	public:
		SerializerDataFrame();

		~SerializerDataFrame();

	public:
		enum class DataType : uint8_t
		{
			BOOL = 0,
			CHAR,
			INT32,
			UINT32,
			INT64,
			UINT64,
			FLOAT,
			DOUBLE,
			VECTOR2,
			VECTOR3,
			VECTOR4,
			QUATERNION,
			TRANSFORM,
			MATRIX3,
			MATRIX4,
			VERTEX,
			STRING,
			ARRAY,
			LIST,
			MAP,
			PAIR,
			UNORDERED_MAP,
			SET,
			UNORDERED_SET,
			CUSTOM
		};

	public:
		void Save(const std::filesystem::path& savePath);

		void Load(const std::filesystem::path& loadPath);

		void Load(const void* data, uint32_t length);

		void Resize();

	public:
		SerializerDataFrame& operator<<(bool value);
		SerializerDataFrame& operator<<(char value);
		SerializerDataFrame& operator<<(int32_t value);
		SerializerDataFrame& operator<<(uint32_t value);
		SerializerDataFrame& operator<<(int64_t value);
		SerializerDataFrame& operator<<(uint64_t value);
		SerializerDataFrame& operator<<(float value);
		SerializerDataFrame& operator<<(double value);
		SerializerDataFrame& operator<<(const std::string& value);
		SerializerDataFrame& operator<<(const Vector2& value);
		SerializerDataFrame& operator<<(const Vector3& value);
		SerializerDataFrame& operator<<(const Vector4& value);
		SerializerDataFrame& operator<<(const Quaternion& value);
		SerializerDataFrame& operator<<(const Transform& value);
		SerializerDataFrame& operator<<(const Matrix3x3& value);
		SerializerDataFrame& operator<<(const Matrix4x4& value);
		SerializerDataFrame& operator<<(const Vertex& value);
		SerializerDataFrame& operator<<(const char* value);

		template<typename T>
		SerializerDataFrame& operator<<(const std::vector<T>& value);

		template<typename T>
		SerializerDataFrame& operator<<(const std::list<T>& value);

		template<typename Key, typename Value>
		SerializerDataFrame& operator<<(const std::map<Key, Value>& value);

		template<typename Key, typename Value>
		SerializerDataFrame& operator<<(const std::unordered_map<Key, Value>& value);

		template<typename T>
		SerializerDataFrame& operator<<(const std::set<T>& value);

		template<typename T>
		SerializerDataFrame& operator<<(const std::unordered_set<T>& value);

		template<typename T1, typename T2>
		SerializerDataFrame& operator<<(const std::pair<T1, T2>& value);

		SerializerDataFrame& operator<<(const ISerializable& value);

	public:
		SerializerDataFrame& operator>>(bool& value);
		SerializerDataFrame& operator>>(char& value);
		SerializerDataFrame& operator>>(int32_t& value);
		SerializerDataFrame& operator>>(uint32_t& value);
		SerializerDataFrame& operator>>(int64_t& value);
		SerializerDataFrame& operator>>(uint64_t& value);
		SerializerDataFrame& operator>>(float& value);
		SerializerDataFrame& operator>>(double& value);
		SerializerDataFrame& operator>>(std::string& value);
		SerializerDataFrame& operator>>(Vector2& value);
		SerializerDataFrame& operator>>(Vector3& value);
		SerializerDataFrame& operator>>(Vector4& value);
		SerializerDataFrame& operator>>(Quaternion& value);
		SerializerDataFrame& operator>>(Transform& value);
		SerializerDataFrame& operator>>(Matrix3x3& value);
		SerializerDataFrame& operator>>(Matrix4x4& value);
		SerializerDataFrame& operator>>(Vertex& value);

		template<typename T>
		SerializerDataFrame& operator>>(std::vector<T>& value);

		template<typename T>
		SerializerDataFrame& operator>>(std::list<T>& value);

		template<typename Key, typename Value>
		SerializerDataFrame& operator>>(std::map<Key, Value>& value);

		template<typename Key, typename Value>
		SerializerDataFrame& operator>>(std::unordered_map<Key, Value>& value);

		template<typename T>
		SerializerDataFrame& operator>>(std::set<T>& value);

		template<typename T>
		SerializerDataFrame& operator>>(std::unordered_set<T>& value);

		template<typename T1, typename T2>
		SerializerDataFrame& operator>>(std::pair<T1, T2>& value);

		SerializerDataFrame& operator>>(ISerializable& value);

	public:
		void Write(const char* data, uint64_t len);
		void Write(bool value);
		void Write(char value);
		void Write(int32_t value);
		void Write(uint32_t value);
		void Write(int64_t value);
		void Write(uint64_t value);
		void Write(float value);
		void Write(double value);
		void Write(const std::string& value);
		void Write(const Vector2& value);
		void Write(const Vector3& value);
		void Write(const Vector4& value);
		void Write(const Quaternion& value);
		void Write(const Transform& value);
		void Write(const Matrix3x3& value);
		void Write(const Matrix4x4& value);
		void Write(const Vertex& value);

		template<typename T>
		void Write(const std::vector<T>& value);

		template<typename T>
		void Write(const std::list<T>& value);

		template<typename Key, typename Value>
		void Write(const std::map<Key, Value>& value);

		template<typename Key, typename Value>
		void Write(const std::unordered_map<Key, Value>& value);

		template<typename T>
		void Write(const std::set<T>& value);

		template<typename T>
		void Write(const std::unordered_set<T>& value);

		template<typename T1, typename T2>
		void Write(const std::pair<T1, T2>& value);

		void Write(const ISerializable& value);

	public:
		bool Read(char* data, uint64_t len);
		bool Read(bool& value);
		bool Read(char& value);
		bool Read(int32_t& value);
		bool Read(uint32_t& value);
		bool Read(int64_t& value);
		bool Read(uint64_t& value);
		bool Read(float& value);
		bool Read(double& value);
		bool Read(std::string& value);
		bool Read(Vector2& value);
		bool Read(Vector3& value);
		bool Read(Vector4& value);
		bool Read(Quaternion& value);
		bool Read(Transform& value);
		bool Read(Matrix3x3& value);
		bool Read(Matrix4x4& value);
		bool Read(Vertex& value);

		template<typename T>
		bool Read(std::vector<T>& value);

		template<typename T>
		bool Read(std::list<T>& value);

		template<typename Key, typename Value>
		bool Read(std::map<Key, Value>& value);

		template<typename Key, typename Value>
		bool Read(std::unordered_map<Key, Value>& value);

		template<typename T>
		bool Read(std::set<T>& value);

		template<typename T>
		bool Read(std::unordered_set<T>& value);

		template<typename T1, typename T2>
		bool Read(std::pair<T1, T2>& value);

		bool Read(ISerializable& value);

	private:
		void Reserve(uint64_t length);

	private:
		std::vector<char> mBuffer;
		uint64_t mOffset;
	};

	class ResourceSerializer
	{
	public:
		template<typename T>
		static T* LoadResourceFromFile(const std::filesystem::path& resourcePath)
		{
			SerializerDataFrame frame;
			frame.Load(resourcePath);

			if (!ProjectFileSystem::GetInstance()->FileExists(resourcePath))
			{
				return nullptr;
			}

			std::shared_ptr<Blob> blob = ProjectFileSystem::GetInstance()->ReadFile(resourcePath);

			T* result = new T();
			frame >> *result;

			return result;
		}

		template<typename T>
		static T* LoadResourceFromMemory(const void* data, uint32_t length)
		{
			SerializerDataFrame frame;
			frame.Load(data, length);

			T* result = new T();
			frame >> *result;

			return result;
		}
	};

	template<typename T>
	inline void SerializerDataFrame::Write(const std::vector<T>& value)
	{
		DataType type = DataType::ARRAY;
		Write((char*)&type, sizeof(type));
		Write((uint32_t)value.size());
		for (auto it = value.begin(); it != value.end(); ++it)
		{
			Write(*it);
		}
	}

	template<typename T>
	inline void SerializerDataFrame::Write(const std::list<T>& value)
	{
		DataType type = DataType::LIST;
		Write((char*)&type, sizeof(type));
		Write((uint32_t)value.size());
		for (auto it = value.begin(); it != value.end(); ++it)
		{
			Write(*it);
		}
	}

	template<typename Key, typename Value>
	inline void SerializerDataFrame::Write(const std::map<Key, Value>& value)
	{
		DataType type = DataType::MAP;
		Write((char*)&type, sizeof(type));
		Write((uint32_t)value.size());
		for (auto it = value.begin(); it != value.end(); ++it)
		{
			Write(it->first);
			Write(it->second);
		}
	}

	template<typename Key, typename Value>
	inline void SerializerDataFrame::Write(const std::unordered_map<Key, Value>& value)
	{
		DataType type = DataType::UNORDERED_MAP;
		Write((char*)&type, sizeof(type));
		Write((uint32_t)value.size());
		for (auto it = value.begin(); it != value.end(); ++it)
		{
			Write(it->first);
			Write(it->second);
		}
	}

	template<typename T>
	inline void SerializerDataFrame::Write(const std::set<T>& value)
	{
		DataType type = DataType::SET;
		Write((char*)&type, sizeof(type));
		Write((uint32_t)value.size());
		for (auto it = value.begin(); it != value.end(); ++it)
		{
			Write(*it);
		}
	}

	template<typename T>
	inline void SerializerDataFrame::Write(const std::unordered_set<T>& value)
	{
		DataType type = DataType::UNORDERED_SET;
		Write((char*)&type, sizeof(type));
		Write((uint32_t)value.size());
		for (auto it = value.begin(); it != value.end(); ++it)
		{
			Write(*it);
		}
	}

	template<typename T1, typename T2>
	inline void SerializerDataFrame::Write(const std::pair<T1, T2>& value)
	{
		DataType type = DataType::PAIR;
		Write((char*)&type, sizeof(type));
		Write(value.first);
		Write(value.second);
	}

	template<typename T>
	inline bool SerializerDataFrame::Read(std::vector<T>& value)
	{
		ASSERT((DataType)mBuffer[mOffset] == DataType::ARRAY);
		++mOffset;
		uint32_t len;
		Read(len);
		value.clear();
		value.reserve(len);

		for (uint32_t i = 0; i < len; ++i)
		{
			T v;
			Read(v);
			value.push_back(v);
		}
		return true;
	}

	template<typename T>
	inline bool SerializerDataFrame::Read(std::list<T>& value)
	{
		ASSERT((DataType)mBuffer[mOffset] == DataType::LIST);
		++mOffset;
		uint32_t len;
		Read(len);
		value.clear();

		for(uint32_t i = 0; i < len; ++i)
		{
			T v;
			Read(v);
			value.push_back(v);
		}
		return true;
	}

	template<typename Key, typename Value>
	inline bool SerializerDataFrame::Read(std::map<Key, Value>& value)
	{
		ASSERT((DataType)mBuffer[mOffset] == DataType::MAP);
		++mOffset;
		uint32_t len;
		Read(len);
		value.clear();

		for (uint32_t i = 0; i < len; ++i)
		{
			Key k;
			Value v;
			Read(k);
			Read(v);

			value[k] = v;
		}
		return true;
	}

	template<typename Key, typename Value>
	inline bool SerializerDataFrame::Read(std::unordered_map<Key, Value>& value)
	{
		ASSERT((DataType)mBuffer[mOffset] == DataType::UNORDERED_MAP);
		++mOffset;
		uint32_t len;
		Read(len);
		value.clear();

		for (uint32_t i = 0; i < len; ++i)
		{
			Key k;
			Value v;
			Read(k);
			Read(v);

			value[k] = v;
		}
		return true;
	}

	template<typename T>
	inline bool SerializerDataFrame::Read(std::set<T>& value)
	{
		ASSERT((DataType)mBuffer[mOffset] == DataType::SET);
		++mOffset;
		uint32_t len;
		Read(len);
		value.clear();

		for (uint32_t i = 0; i < len; ++i)
		{
			T t;
			Read(t);

			value.insert(t);
		}
		return true;
	}

	template<typename T>
	inline bool SerializerDataFrame::Read(std::unordered_set<T>& value)
	{
		ASSERT((DataType)mBuffer[mOffset] == DataType::UNORDERED_SET);
		++mOffset;
		uint32_t len;
		Read(len);
		value.clear();

		for (uint32_t i = 0; i < len; ++i)
		{
			T t;
			Read(t);

			value.insert(t);
		}
		return true;
	}

	template<typename T1, typename T2>
	inline bool SerializerDataFrame::Read(std::pair<T1, T2>& value)
	{
		ASSERT((DataType)mBuffer[mOffset] == DataType::PAIR);
		++mOffset;

		Read(value.first);
		Read(value.second);

		return true;
	}

	template<typename T>
	inline SerializerDataFrame& SerializerDataFrame::operator<<(const std::vector<T>& value)
	{
		Write(value);
		return *this;
	}

	template<typename T>
	inline SerializerDataFrame& SerializerDataFrame::operator<<(const std::list<T>& value)
	{
		Write(value);
		return *this;
	}

	template<typename T1, typename T2>
	inline SerializerDataFrame& SerializerDataFrame::operator<<(const std::pair<T1, T2>& value)
	{
		Write(value);
		return *this;
	}

	template<typename Key, typename Value>
	inline SerializerDataFrame& SerializerDataFrame::operator<<(const std::map<Key, Value>& value)
	{
		Write(value);
		return *this;
	}

	template<typename Key, typename Value>
	inline SerializerDataFrame& SerializerDataFrame::operator<<(const std::unordered_map<Key, Value>& value)
	{
		Write(value);
		return *this;
	}

	template<typename T>
	inline SerializerDataFrame& SerializerDataFrame::operator<<(const std::set<T>& value)
	{
		Write(value);
		return *this;
	}

	template<typename T>
	inline SerializerDataFrame& SerializerDataFrame::operator<<(const std::unordered_set<T>& value)
	{
		Write(value);
		return *this;
	}

	template<typename T>
	inline SerializerDataFrame& SerializerDataFrame::operator>>(std::vector<T>& value)
	{
		Read(value);
		return *this;
	}

	template<typename T>
	inline SerializerDataFrame& SerializerDataFrame::operator>>(std::list<T>& value)
	{
		Read(value);
		return *this;
	}

	template<typename T1, typename T2>
	inline SerializerDataFrame& SerializerDataFrame::operator>>(std::pair<T1, T2>& value)
	{
		Read(value);
		return *this;
	}

	template<typename Key, typename Value>
	inline SerializerDataFrame& SerializerDataFrame::operator>>(std::map<Key, Value>& value)
	{
		Read(value);
		return *this;
	}

	template<typename Key, typename Value>
	inline SerializerDataFrame& SerializerDataFrame::operator>>(std::unordered_map<Key, Value>& value)
	{
		Read(value);
		return *this;
	}

	template<typename T>
	inline SerializerDataFrame& SerializerDataFrame::operator>>(std::set<T>& value)
	{
		Read(value);
		return *this;
	}

	template<typename T>
	inline SerializerDataFrame& SerializerDataFrame::operator>>(std::unordered_set<T>& value)
	{
		Read(value);
		return *this;
	}
}
