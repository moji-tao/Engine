#include "EngineRuntime/include/Core/Meta/Serializer.h"
#include "EngineRuntime/include/Core/Math/Vector2.h"
#include "EngineRuntime/include/Core/Math/Vector3.h"
#include "EngineRuntime/include/Core/Math/Vector4.h"
#include "EngineRuntime/include/Core/Math/Quaternion.h"
#include "EngineRuntime/include/Core/Math/Transform.h"
#include "EngineRuntime/include/Core/Math/Matrix3x3.h"
#include "EngineRuntime/include/Core/Math/Matrix4x4.h"
#include "EngineRuntime/include/Framework/Interface/ISerializable.h"
#include "EngineRuntime/include/Core/Base/macro.h"
#include "EngineRuntime/include/Resource/ResourceType/Data/MeshData.h"

namespace Engine
{
    SerializerDataFrame::SerializerDataFrame()
	    :mOffset(0)
    { }

    SerializerDataFrame::~SerializerDataFrame()
    { }

    void SerializerDataFrame::Save(const std::filesystem::path& savePath)
    {
        if (!ProjectFileSystem::GetInstance()->WriteFile(savePath, mBuffer.data(), mBuffer.size()))
        {
            ASSERT(0);
        }

        mOffset = 0;
    }

    void SerializerDataFrame::Load(const std::filesystem::path& loadPath)
    {
        std::shared_ptr<Blob> blob = ProjectFileSystem::GetInstance()->ReadFile(loadPath);

        if (blob != nullptr)
        {
            mBuffer.resize(blob->GetSize());
            memcpy(mBuffer.data(), blob->GetData(), blob->GetSize());
            mOffset = 0;
        }
        else
        {
            ASSERT(0);
        }
    }

    void SerializerDataFrame::Load(const void* data, uint32_t length)
    {
        mBuffer.resize(length);
        memcpy(mBuffer.data(), data, length);
        mOffset = 0;
    }

    void SerializerDataFrame::Resize()
    {
        mOffset = 0;
    }

    void SerializerDataFrame::Write(const char* data, uint64_t len)
    {
        Reserve(len);
        uint64_t size = (uint64_t)mBuffer.size();
        mBuffer.resize(size + len);
        memcpy(&mBuffer[size], data, len);
    }

    void SerializerDataFrame::Write(bool value)
    {
        DataType type = DataType::BOOL;
        Write((char*)&type, sizeof(type));
        Write((char*)&value, sizeof(value));
    }

    void SerializerDataFrame::Write(char value)
    {
        DataType type = DataType::CHAR;
        Write((char*)&type, sizeof(type));
        Write((char*)&value, sizeof(value));
    }

    void SerializerDataFrame::Write(int32_t value)
    {
        DataType type = DataType::INT32;
        Write((char*)&type, sizeof(type));
        Write((char*)&value, sizeof(value));
    }

    void SerializerDataFrame::Write(uint32_t value)
    {
        DataType type = DataType::UINT32;
        Write((char*)&type, sizeof(type));
        Write((char*)&value, sizeof(value));
    }

    void SerializerDataFrame::Write(int64_t value)
    {
        DataType type = DataType::INT64;
        Write((char*)&type, sizeof(type));
        Write((char*)&value, sizeof(value));
    }

    void SerializerDataFrame::Write(uint64_t value)
    {
        DataType type = DataType::UINT64;
        Write((char*)&type, sizeof(type));
        Write((char*)&value, sizeof(value));
    }

    void SerializerDataFrame::Write(float value)
    {
        DataType type = DataType::FLOAT;
        Write((char*)&type, sizeof(type));
        Write((char*)&value, sizeof(value));
    }

    void SerializerDataFrame::Write(double value)
    {
        DataType type = DataType::DOUBLE;
        Write((char*)&type, sizeof(type));
        Write((char*)&value, sizeof(value));
    }

    void SerializerDataFrame::Write(const std::string& value)
    {
        DataType type = DataType::STRING;
        Write((char*)&type, sizeof(type));
        Write((uint32_t)value.size());
        if (value.size() > 0)
        {
            Write((char*)value.data(), (uint64_t)value.size());
        }
    }

    void SerializerDataFrame::Write(const Vector2& value)
    {
        DataType type = DataType::VECTOR2;
        Write((char*)&type, sizeof(type));
        Write((char*)&value, sizeof(value));
    }

    void SerializerDataFrame::Write(const Vector3& value)
    {
        DataType type = DataType::VECTOR3;
        Write((char*)&type, sizeof(type));
        Write((char*)&value, sizeof(value));
    }

    void SerializerDataFrame::Write(const Vector4& value)
    {
        DataType type = DataType::VECTOR4;
        Write((char*)&type, sizeof(type));
        Write((char*)&value, sizeof(value));
    }

    void SerializerDataFrame::Write(const Quaternion& value)
    {
        DataType type = DataType::QUATERNION;
        Write((char*)&type, sizeof(type));
        Write((char*)&value, sizeof(value));
    }

    void SerializerDataFrame::Write(const Transform& value)
    {
        DataType type = DataType::TRANSFORM;
        Write((char*)&type, sizeof(type));
        Write((char*)&value, sizeof(value));
    }

    void SerializerDataFrame::Write(const Matrix3x3& value)
    {
        DataType type = DataType::MATRIX3;
        Write((char*)&type, sizeof(type));
        Write((char*)&value, sizeof(value));
    }

    void SerializerDataFrame::Write(const Matrix4x4& value)
    {
        DataType type = DataType::MATRIX4;
        Write((char*)&type, sizeof(type));
        Write((char*)&value, sizeof(value));
    }

    void SerializerDataFrame::Write(const Vertex& value)
    {
        DataType type = DataType::VERTEX;
        Write((char*)&type, sizeof(type));
        Write((char*)&value, sizeof(value));
    }

    void SerializerDataFrame::Write(const ISerializable& value)
    {
        DataType type = DataType::CUSTOM;
        Write((char*)&type, sizeof(type));

        value.Serialize(*this);
    }

    bool SerializerDataFrame::Read(char* data, uint64_t len)
    {
        ASSERT(mOffset + len <= mBuffer.size());
        std::memcpy(data, &mBuffer[mOffset], len);
        mOffset += len;
        return true;
    }

    bool SerializerDataFrame::Read(bool& value)
    {
        ASSERT((DataType)mBuffer[mOffset] == DataType::BOOL);
        ++mOffset;

        return Read((char*)&value, sizeof(value));
    }

    bool SerializerDataFrame::Read(char& value)
    {
        ASSERT((DataType)mBuffer[mOffset] == DataType::CHAR);
        ++mOffset;

        return Read((char*)&value, sizeof(value));
    }

    bool SerializerDataFrame::Read(int32_t& value)
    {
        ASSERT((DataType)mBuffer[mOffset] == DataType::INT32);
        ++mOffset;

        return Read((char*)&value, sizeof(value));
    }

    bool SerializerDataFrame::Read(uint32_t& value)
    {
        ASSERT((DataType)mBuffer[mOffset] == DataType::UINT32);
        ++mOffset;

        return Read((char*)&value, sizeof(value));
    }

    bool SerializerDataFrame::Read(int64_t& value)
    {
        ASSERT((DataType)mBuffer[mOffset] == DataType::INT64);
        ++mOffset;

        return Read((char*)&value, sizeof(value));
    }

    bool SerializerDataFrame::Read(uint64_t& value)
    {
        ASSERT((DataType)mBuffer[mOffset] == DataType::UINT64);
        ++mOffset;

        return Read((char*)&value, sizeof(value));
    }

    bool SerializerDataFrame::Read(float& value)
    {
        ASSERT((DataType)mBuffer[mOffset] == DataType::FLOAT);
        ++mOffset;

        return Read((char*)&value, sizeof(value));
    }

    bool SerializerDataFrame::Read(double& value)
    {
        ASSERT((DataType)mBuffer[mOffset] == DataType::DOUBLE);
        ++mOffset;

        return Read((char*)&value, sizeof(value));
    }

    bool SerializerDataFrame::Read(std::string& value)
    {
        ASSERT((DataType)mBuffer[mOffset] == DataType::STRING);
        ++mOffset;

        uint32_t size;
        Read(size);
        if(size > 0)
        {
            value.resize(size);
            return Read((char*)value.data(), (uint64_t)value.size());
        }
        value = "";
        return true;
    }

    bool SerializerDataFrame::Read(Vector2& value)
    {
        ASSERT((DataType)mBuffer[mOffset] == DataType::VECTOR2);
        ++mOffset;

        return Read((char*)&value, sizeof(value));
    }

    bool SerializerDataFrame::Read(Vector3& value)
    {
        ASSERT((DataType)mBuffer[mOffset] == DataType::VECTOR3);
        ++mOffset;

        return Read((char*)&value, sizeof(value));
    }

    bool SerializerDataFrame::Read(Vector4& value)
    {
        ASSERT((DataType)mBuffer[mOffset] == DataType::VECTOR4);
        ++mOffset;

        return Read((char*)&value, sizeof(value));
    }

    bool SerializerDataFrame::Read(Quaternion& value)
    {
        ASSERT((DataType)mBuffer[mOffset] == DataType::QUATERNION);
        ++mOffset;

        return Read((char*)&value, sizeof(value));
    }

    bool SerializerDataFrame::Read(Transform& value)
    {
        ASSERT((DataType)mBuffer[mOffset] == DataType::TRANSFORM);
        ++mOffset;

        return Read((char*)&value, sizeof(value));
    }

    bool SerializerDataFrame::Read(Matrix3x3& value)
    {
        ASSERT((DataType)mBuffer[mOffset] == DataType::MATRIX3);
        ++mOffset;

        return Read((char*)&value, sizeof(value));
    }

    bool SerializerDataFrame::Read(Matrix4x4& value)
    {
        ASSERT((DataType)mBuffer[mOffset] == DataType::MATRIX4);
        ++mOffset;

        return Read((char*)&value, sizeof(value));
    }

    bool SerializerDataFrame::Read(Vertex& value)
    {
        ASSERT((DataType)mBuffer[mOffset] == DataType::VERTEX);
        ++mOffset;

        return Read((char*)&value, sizeof(value));
    }

    bool SerializerDataFrame::Read(ISerializable& value)
    {
        ASSERT((DataType)mBuffer[mOffset] == DataType::CUSTOM);
        ++mOffset;

        return value.Deserialize(*this);
    }

    void SerializerDataFrame::Reserve(uint64_t length)
    {
        uint64_t size = mBuffer.size();
        uint64_t cap = mBuffer.capacity();
        if (size + length > cap)
        {
            while (size + length > cap)
            {
                if (cap == 0)
                {
                    cap = 1;
                }
                else
                {
                    cap *= 2;
                }
            }
            mBuffer.reserve(cap);
        }
    }

    SerializerDataFrame& SerializerDataFrame::operator<<(bool value)
    {
        Write(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator<<(char value)
    {
        Write(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator<<(int32_t value)
    {
        Write(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator<<(uint32_t value)
    {
        Write(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator<<(int64_t value)
    {
        Write(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator<<(uint64_t value)
    {
        Write(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator<<(float value)
    {
        Write(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator<<(double value)
    {
        Write(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator<<(const std::string& value)
    {
        Write(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator<<(const Vector2& value)
    {
        Write(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator<<(const Vector3& value)
    {
        Write(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator<<(const Vector4& value)
    {
        Write(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator<<(const Quaternion& value)
    {
        Write(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator<<(const Transform& value)
    {
        Write(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator<<(const Matrix3x3& value)
    {
        Write(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator<<(const Matrix4x4& value)
    {
        Write(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator<<(const Vertex& value)
    {
        Write(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator<<(const char* value)
    {
        Write(std::string(value));
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator<<(const ISerializable& value)
    {
        Write(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator>>(bool& value)
    {
        Read(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator>>(char& value)
    {
        Read(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator>>(int32_t& value)
    {
        Read(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator>>(uint32_t& value)
    {
        Read(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator>>(int64_t& value)
    {
        Read(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator>>(uint64_t& value)
    {
        Read(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator>>(float& value)
    {
        Read(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator>>(double& value)
    {
        Read(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator>>(std::string& value)
    {
        Read(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator>>(Vector2& value)
    {
        Read(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator>>(Vector3& value)
    {
        Read(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator>>(Vector4& value)
    {
        Read(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator>>(Quaternion& value)
    {
        Read(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator>>(Transform& value)
    {
        Read(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator>>(Matrix3x3& value)
    {
        Read(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator>>(Matrix4x4& value)
    {
        Read(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator>>(Vertex& value)
    {
        Read(value);
        return *this;
    }

    SerializerDataFrame& SerializerDataFrame::operator>>(ISerializable& value)
    {
        Read(value);
        return *this;
    }
}
