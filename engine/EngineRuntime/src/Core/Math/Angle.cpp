#include "EngineRuntime/include/Core/Math/Angle.h"
#include "EngineRuntime/include/Core/Math/Math.h"

namespace Engine
{
	Radian::Radian(float r) : m_Value(r) { }

	Radian::Radian(const Degree& d) : m_Value(d.ValueRadians()) { }

	float Radian::ValueRadians() const
	{
		return m_Value;
	}

	float Radian::ValueDegrees() const
	{
		return Math::RadiansToDegrees(m_Value);
	}

	float Radian::ValueAngleUnits() const
	{
		return Math::RadiansToAngleUnits(m_Value);
	}

	void Radian::SetValue(float f)
	{
		m_Value = f;
	}

	Radian& Radian::operator=(float f)
	{
		m_Value = f;
		return *this;
	}

	Radian& Radian::operator=(const Degree& d)
	{
		m_Value = d.ValueRadians();
		return *this;
	}

	const Radian& Radian::operator+() const
	{
		return *this;
	}

	Radian Radian::operator+(const Radian& r) const
	{
		return Radian(m_Value + r.m_Value);
	}

	Radian Radian::operator+(const Degree& d) const
	{
		return Radian(m_Value + d.ValueRadians());
	}

	Radian& Radian::operator+=(const Radian& r)
	{
		m_Value += r.m_Value;
		return *this;
	}

	Radian& Radian::operator+=(const Degree& d)
	{
		m_Value += d.ValueRadians();
		return *this;
	}

	Radian Radian::operator-() const
	{
		return Radian(-m_Value);
	}

	Radian Radian::operator-(const Radian& r) const
	{
		return Radian(m_Value - r.m_Value);
	}

	Radian Radian::operator-(const Degree& d) const
	{
		return Radian(m_Value - d.ValueRadians());
	}

	Radian& Radian::operator-=(const Radian& r)
	{
		m_Value -= r.m_Value;
		return *this;
	}

	Radian& Radian::operator-=(const Degree& d)
	{
		m_Value -= d.ValueRadians();
		return *this;
	}

	Radian Radian::operator*(float f) const
	{
		return Radian(m_Value * f);
	}

	Radian Radian::operator*(const Radian& f) const
	{
		return Radian(m_Value * f.m_Value);
	}

	Radian& Radian::operator*=(float f)
	{
		m_Value *= f;
		return *this;
	}

	Radian Radian::operator/(float f) const
	{
		return Radian(m_Value / f);
	}

	Radian& Radian::operator/=(float f)
	{
		m_Value /= f;
		return *this;
	}

	bool Radian::operator<(const Radian& r) const
	{
		return m_Value < r.m_Value;
	}

	bool Radian::operator<=(const Radian& r) const
	{
		return m_Value <= r.m_Value;
	}

	bool Radian::operator==(const Radian& r) const
	{
		return m_Value == r.m_Value;
	}

	bool Radian::operator!=(const Radian& r) const
	{
		return m_Value != r.m_Value;
	}

	bool Radian::operator>=(const Radian& r) const
	{
		return m_Value >= r.m_Value;
	}

	bool Radian::operator>(const Radian& r) const
	{
		return m_Value > r.m_Value;
	}

	Radian operator*(float a, const Radian& b)
	{
		return Radian(a * b.ValueRadians());
	}

	Radian operator/(float a, const Radian& b)
	{
		return Radian(a / b.ValueRadians());
	}

	Degree operator*(float a, const Degree& b)
	{
		return Degree(a * b.ValueDegrees());
	}

	Degree operator/(float a, const Degree& b)
	{
		return Degree(a / b.ValueDegrees());
	}

	Degree::Degree(float d) : m_Value(d) { }

	Degree::Degree(const Radian& r) : m_Value(r.ValueDegrees()) { }


	float Degree::ValueDegrees() const
	{
		return m_Value;
	}

	float Degree::ValueRadians() const
	{
		return Math::DegreesToRadians(m_Value);
	}

	float Degree::ValueAngleUnits() const
	{
		return Math::DegreesToAngleUnits(m_Value);
	}

	Degree& Degree::operator=(float f)
	{
		m_Value = f;
		return *this;
	}

	Degree& Degree::operator=(const Radian& r)
	{
		m_Value = r.ValueDegrees();
		return *this;
	}

	const Degree& Degree::operator+() const
	{
		return *this;
	}

	Degree Degree::operator+(const Degree& d) const
	{
		return Degree(m_Value + d.m_Value);
	}

	Degree Degree::operator+(const Radian& r) const
	{
		return Degree(m_Value + r.ValueDegrees());
	}

	Degree& Degree::operator+=(const Degree& d)
	{
		m_Value += d.m_Value;
		return *this;
	}

	Degree& Degree::operator+=(const Radian& r)
	{
		m_Value += r.ValueDegrees();
		return *this;
	}

	Degree Degree::operator-() const
	{
		return Degree(-m_Value);
	}

	Degree Degree::operator-(const Degree& d) const
	{
		return Degree(m_Value - d.m_Value);
	}

	Degree Degree::operator-(const Radian& r) const
	{
		return Degree(m_Value - r.ValueDegrees());
	}

	Degree& Degree::operator-=(const Degree& d)
	{
		m_Value -= d.m_Value;
		return *this;
	}

	Degree& Degree::operator-=(const Radian& r)
	{
		m_Value -= r.ValueDegrees();
		return *this;
	}

	Degree Degree::operator*(float f) const
	{
		return Degree(m_Value * f);
	}

	Degree Degree::operator*(const Degree& f) const
	{
		return Degree(m_Value * f.m_Value);
	}

	Degree& Degree::operator*=(float f)
	{
		m_Value *= f;
		return *this;
	}

	Degree Degree::operator/(float f) const
	{
		return Degree(m_Value / f);
	}

	Degree& Degree::operator/=(float f)
	{
		m_Value /= f;
		return *this;
	}

	bool Degree::operator<(const Degree& d) const
	{
		return m_Value < d.m_Value;
	}

	bool Degree::operator<=(const Degree& d) const
	{
		return m_Value <= d.m_Value;
	}

	bool Degree::operator==(const Degree& d) const
	{
		return m_Value == d.m_Value;
	}

	bool Degree::operator!=(const Degree& d) const
	{
		return m_Value != d.m_Value;
	}

	bool Degree::operator>=(const Degree& d) const
	{
		return m_Value >= d.m_Value;
	}

	bool Degree::operator>(const Degree& d) const
	{
		return m_Value > d.m_Value;
	}

	Angle::Angle(float angle) : m_Value(angle) { }

	Angle::operator Radian() const
	{
		return Radian(Math::AngleUnitsToRadians(m_Value));
	}

	Angle::operator Degree() const
	{
		return Degree(Math::AngleUnitsToDegrees(m_Value));
	}

}
