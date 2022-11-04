#pragma once
namespace Engine
{
	class Radian;
	class Degree;
	class Angle;

	class Radian
	{
	public:
		explicit Radian(float r = 0);

		explicit Radian(const Degree& d);

	public:
		float ValueRadians() const;

		float ValueDegrees() const;

		float ValueAngleUnits() const;

		void SetValue(float f);

	public:
		Radian& operator=(float f);

		Radian& operator=(const Degree& d);

		const Radian& operator+() const;

		Radian operator+(const Radian& r) const;

		Radian operator+(const Degree& d) const;

		Radian& operator+=(const Radian& r);

		Radian& operator+=(const Degree& d);

		Radian operator-() const;

		Radian operator-(const Radian& r) const;

		Radian operator-(const Degree& d) const;

		Radian& operator-=(const Radian& f);

		Radian& operator-=(const Degree& d);

		Radian operator*(float f) const;

		Radian operator*(const Radian& r) const;

		friend Radian operator*(float a, const Radian& b);

		Radian& operator*=(float f);

		Radian operator/(float f) const;

		friend Radian operator/(float a, const Radian& b);

		Radian& operator/=(float f);

		bool operator<(const Radian& r) const;

		bool operator<=(const Radian& r) const;

		bool operator==(const Radian& r) const;

		bool operator!=(const Radian& r) const;

		bool operator>=(const Radian& r) const;

		bool operator>(const Radian& r) const;

	private:
		float m_Value;
	};

	class Degree
	{
	public:
		explicit Degree(float d = 0);

		explicit Degree(const Radian& r);

	public:
		float ValueDegrees() const;

		float ValueRadians() const;

		float ValueAngleUnits() const;

	public:
		Degree& operator=(float f);

		Degree& operator=(const Degree& d) = default;

		Degree& operator=(const Radian& r);

		const Degree& operator+() const;

		Degree operator+(const Degree& d) const;

		Degree operator+(const Radian& r) const;

		Degree& operator+=(const Degree& d);

		Degree& operator+=(const Radian& r);

		Degree operator-() const;

		Degree operator-(const Degree& d) const;

		Degree operator-(const Radian& r) const;

		Degree& operator-=(const Degree& d);

		Degree& operator-=(const Radian& r);

		Degree operator*(float f) const;

		Degree operator*(const Degree& f) const;

		friend Degree operator*(float a, const Degree& b);

		Degree& operator*=(float f);

		Degree operator/(float f) const;

		friend Degree operator/(float a, const Degree& b);

		Degree& operator/=(float f);

		bool operator<(const Degree& d) const;

		bool operator<=(const Degree& d) const;

		bool operator==(const Degree& d) const;

		bool operator!=(const Degree& d) const;

		bool operator>=(const Degree& d) const;

		bool operator>(const Degree& d) const;

	private:
		float m_Value;
	};

	class Angle
	{
	public:
		explicit Angle(float angle = 0);

		explicit operator Radian() const;

		explicit operator Degree() const;

	private:
		float m_Value;
	};
}
