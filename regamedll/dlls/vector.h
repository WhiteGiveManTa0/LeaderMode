/*
*
*   This program is free software; you can redistribute it and/or modify it
*   under the terms of the GNU General Public License as published by the
*   Free Software Foundation; either version 2 of the License, or (at
*   your option) any later version.
*
*   This program is distributed in the hope that it will be useful, but
*   WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software Foundation,
*   Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*   In addition, as a special exception, the author gives permission to
*   link the code of this program with the Half-Life Game Engine ("HL
*   Engine") and Modified Game Libraries ("MODs") developed by Valve,
*   L.L.C ("Valve").  You must obey the GNU General Public License in all
*   respects for all of the code used other than the HL Engine and MODs
*   from Valve.  If you modify this file, you may extend this exception
*   to your version of the file, but you are not obligated to do so.  If
*   you do not wish to do so, delete this exception statement from your
*   version.
*
*/

#pragma once

// Used for many pathfinding and many other operations that are treated as planar rather than 3D.
class Vector2D
{
public:
	// Construction/destruction
	constexpr Vector2D() : x(0), y(0) {}
	constexpr Vector2D(float X, float Y) : x(X), y(Y) {}
	Vector2D(const Vector2D &v) { *(int *)&x = *(int *)&v.x; *(int *)&y = *(int *)&v.y; }
	explicit Vector2D(const float rgfl[2]) { *(int*)&x = *(int*)&rgfl[0]; *(int*)&y = *(int*)&rgfl[1]; }

	// Operators
	decltype(auto) operator-()         const { return Vector2D(-x, -y); }
	bool operator==(const Vector2D &v) const { return x == v.x && y == v.y; }
	bool operator!=(const Vector2D &v) const { return !(*this == v); }

	decltype(auto) operator+(const Vector2D &v)  const { return Vector2D(x + v.x, y + v.y); }
	decltype(auto) operator-(const Vector2D &v)  const { return Vector2D(x - v.x, y - v.y); }
	decltype(auto) operator*(const Vector2D &v)  const { return Vector2D(x * v.x, y * v.y); }
	decltype(auto) operator/(const Vector2D &v)  const { return Vector2D(x / v.x, y / v.y); }

	decltype(auto) operator+=(const Vector2D &v) { return (*this = *this + v); }
	decltype(auto) operator-=(const Vector2D &v) { return (*this = *this - v); }
	decltype(auto) operator*=(const Vector2D &v) { return (*this = *this * v); }
	decltype(auto) operator/=(const Vector2D &v) { return (*this = *this / v); }

	decltype(auto) operator+(float fl) const { return Vector2D(x + fl, y + fl); }
	decltype(auto) operator-(float fl) const { return Vector2D(x - fl, y - fl); }

	// TODO: FIX ME!!
#ifdef PLAY_GAMEDLL
	decltype(auto) operator*(float fl) const { return Vector2D(vec_t(x * fl), vec_t(y * fl)); }
	decltype(auto) operator/(float fl) const { return Vector2D(vec_t(x / fl), vec_t(y / fl)); }
#else
	decltype(auto) operator*(float fl) const { return Vector2D(x * fl, y * fl); }
	decltype(auto) operator/(float fl) const { return Vector2D(x / fl, y / fl); }
#endif

	decltype(auto) operator=(std::nullptr_t) { return Vector2D(0, 0); }
	decltype(auto) operator+=(float fl) { return (*this = *this + fl); }
	decltype(auto) operator-=(float fl) { return (*this = *this - fl); }
	decltype(auto) operator*=(float fl) { return (*this = *this * fl); }
	decltype(auto) operator/=(float fl) { return (*this = *this / fl); }

	// Methods
	inline void Clear() { x = 0; y = 0; }
	inline void CopyToArray(float *rgfl) const { *(int *)&rgfl[0] = *(int *)&x; *(int *)&rgfl[1] = *(int *)&y; }
	inline real_t Length() const { return Q_sqrt(real_t(x * x + y * y)); }		// Get the vector's magnitude
	inline float LengthSquared() const { return (x * x + y * y); }				// Get the vector's magnitude squared

	operator float*()             { return &x; } // Vectors will now automatically convert to float * when needed
	operator const float*() const { return &x; } // Vectors will now automatically convert to float * when needed

	Vector2D Normalize() const
	{
		real_t flLen = Length();
		if (!flLen)
			return Vector2D(0, 0);

		flLen = 1 / flLen;

#ifdef PLAY_GAMEDLL
		return Vector2D(vec_t(x * flLen), vec_t(y * flLen));
#else
		return Vector2D(x * flLen, y * flLen);
#endif // PLAY_GAMEDLL
	}
	inline bool IsLengthLessThan   (float length) const { return (LengthSquared() < length * length); }
	inline bool IsLengthGreaterThan(float length) const { return (LengthSquared() > length * length); }
	real_t NormalizeInPlace()
	{
		real_t flLen = Length();
		if (flLen > 0.0)
		{
			x = vec_t(1 / flLen * x);
			y = vec_t(1 / flLen * y);
		}
		else
		{
			x = 1.0;
			y = 0.0;
		}
		return flLen;
	}
	bool IsZero(float tolerance = 0.01f) const
	{
		return (x > -tolerance && x < tolerance &&
			y > -tolerance && y < tolerance);
	}

	// anti-clockwise.
	Vector2D Rotate(float angle) const
	{
		float a, c, s;

		a = (angle * M_PI / 180.0);
		c = Q_cos(a);
		s = Q_sin(a);

		return Vector2D(c * x - s * y,
						s * x + c * y
		);
	}

	Vector2D Transform(const vec_t** matrix) const
	{
		/*
		| a  b | | x |   | ax + by |
		|      | |   | = |         |
		| c  d | | y |   | cx + dy |
		*/

		return Vector2D(
			matrix[0][0] * x + matrix[0][1] * y,
			matrix[1][0] * x + matrix[1][1] * y
		);
	}

	// Members
	vec_t x, y;
};

inline real_t DotProduct(const Vector2D &a, const Vector2D &b)
{
	return (a.x * b.x + a.y * b.y);
}

inline Vector2D operator*(float fl, const Vector2D &v)
{
	return v * fl;
}

inline float operator^(const Vector2D& a, const Vector2D& b)
{
	float length_ab = a.Length() * b.Length();

	if (length_ab == 0.0)
		return 0.0;

	return (double)(Q_acos(DotProduct(a, b) / length_ab) * (180.0 / M_PI));
}

// 2x2 Matrix
class Matrix2x2
{
public:
	// Construction/destruction
	constexpr Matrix2x2() : a(1), b(0), c(0), d(1) {}
	constexpr Matrix2x2(float A, float B, float C, float D) : a(A), b(B), c(C), d(D) {}
	constexpr Matrix2x2(const Vector2D& i_hat, const Vector2D& j_hat) : a(i_hat.x), b(j_hat.x), c(i_hat.y), d(j_hat.y) {}
	Matrix2x2(const Matrix2x2& m) { *(int*)&a = *(int*)&m.a; *(int*)&b = *(int*)&m.b; *(int*)&c = *(int*)&m.c; *(int*)&d = *(int*)&m.d;}
	Matrix2x2(const float rgfl[4]) { *(int*)&a = *(int*)&rgfl[0]; *(int*)&b = *(int*)&rgfl[1]; *(int*)&c = *(int*)&rgfl[2]; *(int*)&d = *(int*)&rgfl[3];}
	Matrix2x2(const float rgfl[2][2]) { *(int*)&a = *(int*)&rgfl[0][0]; *(int*)&b = *(int*)&rgfl[0][1]; *(int*)&c = *(int*)&rgfl[1][0]; *(int*)&d = *(int*)&rgfl[1][1]; }
	Matrix2x2(float flAngle)	// by a anti-clockwise rotation.
	{
		auto rad = (flAngle * M_PI / 180.0);
		auto sine = Q_sin(rad);
		auto cosine = Q_cos(rad);

		a = cosine;	b = -sine;
		c = sine;	d = cosine;
	}

	// Operators
	decltype(auto) operator~()			const { return Matrix2x2(d, -b, -c, a); }	// Inverse matrix
	bool operator==(const Matrix2x2& m) const { return a == m.a && b == m.b && c == m.c && d == m.d; }
	bool operator!=(const Matrix2x2& m) const { return !(*this == m); }

	decltype(auto) operator+(const Matrix2x2& m) const { return Matrix2x2(a + m.a, b + m.b, c + m.c, d + m.d); }
	decltype(auto) operator-(const Matrix2x2& m) const { return Matrix2x2(a - m.a, b - m.b, c - m.c, d - m.d); }
	decltype(auto) operator*(const Matrix2x2& m) const { return Matrix2x2(a * m.a + b * m.c, a * m.b + b * m.d, c * m.a + d * m.c, c * m.b + d * m.d); }

	decltype(auto) operator+=(const Matrix2x2& m) { return (*this = *this + m); }
	decltype(auto) operator-=(const Matrix2x2& m) { return (*this = *this - m); }
	decltype(auto) operator*=(const Matrix2x2& m) { return (*this = *this * m); }

	decltype(auto) operator=(std::nullptr_t) { return Matrix2x2(0, 0, 0, 0); }
	decltype(auto) operator*(float fl) const { return Matrix2x2(a * fl, b * fl, c * fl, d * fl); }
	decltype(auto) operator/(float fl) const { return Matrix2x2(a / fl, b / fl, c / fl, d / fl); }

	decltype(auto) operator*=(float fl) { return (*this = *this * fl); }
	decltype(auto) operator/=(float fl) { return (*this = *this / fl); }

	operator float* () { return &a; }
	operator const float* () const { return &a; }

	// Methods
	void Clear() { a = 0; b = 0; c = 0; d = 0; }
	static decltype(auto) Identity()	{ return Matrix2x2(1, 0, 0, 1); }
	static decltype(auto) Zero()		{ return Matrix2x2(0, 0, 0, 0); }

	// Members
	vec_t a, b, c, d;
};

inline Matrix2x2 operator*(float fl, const Matrix2x2& m)
{
	return m * fl;
}

inline Vector2D operator*(const Matrix2x2& m, const Vector2D& v)
{
	/*
	| a  b | | x |   | ax + by |
	|      | |   | = |         |
	| c  d | | y |   | cx + dy |
	*/

	return Vector2D(
		m.a * v.x + m.b * v.y,
		m.c * v.x + m.d * v.y
	);
}

// 3x3 Matrix
class Matrix3x3
{
public:
	// Construction/destruction
	constexpr Matrix3x3() noexcept {}
	constexpr Matrix3x3(const Matrix3x3& s) = default;
	constexpr Matrix3x3(Matrix3x3&& s) = default;
	Matrix3x3& operator=(const Matrix3x3& s) = default;
	Matrix3x3& operator=(Matrix3x3&& s) = default;
	~Matrix3x3() {}

	Matrix3x3(const float rgfl[3][3]) { memcpy(&dat, &rgfl, sizeof(dat)); }
	Matrix3x3(float a, float b, float c, float d, float e, float f, float g, float h, float i)
	{
		dat[0][0] = a; dat[0][1] = b; dat[0][2] = c;
		dat[1][0] = d; dat[1][1] = e; dat[1][2] = f;
		dat[2][0] = g; dat[2][1] = h; dat[2][2] = i;
	}

	// Static Methods
	static decltype(auto) Identity()
	{
		return Matrix3x3(
			1, 0, 0,
			0, 1, 0,
			0, 0, 1
		);
	}
	static decltype(auto) Zero()
	{
		return Matrix3x3(
			0, 0, 0,
			0, 0, 0,
			0, 0, 0
		);
	}
	static decltype(auto) Rotation2D(float flAngle)
	{
		auto rad = (flAngle * M_PI / 180.0);
		auto sine = Q_sin(rad);
		auto cosine = Q_cos(rad);

		return Matrix3x3(
			cosine,	-sine,	0,
			sine,	cosine,	0,
			0,		0,		1
		);
	}
	static decltype(auto) Translation2D(const Vector2D& v)
	{
		return Matrix3x3(
			1, 0, v.x,
			0, 1, v.y,
			0, 0, 1
		);
	}
	static decltype(auto) Stretch2D(float x, float y)
	{
		return Matrix3x3(
			x, 0, 0,
			0, y, 0,
			0, 0, 1
		);
	}
	static decltype(auto) Stretch2D(float k)
	{
		return Matrix3x3(
			k, 0, 0,
			0, k, 0,
			0, 0, 1
		);
	}
	static decltype(auto) Squeeze2D(float x, float y)
	{
		return Matrix3x3(
			1.0f / x,	0,			0,
			0,			1.0f / y,	0,
			0,			0,			1
		);
	}
	static decltype(auto) Squeeze2D(float k)
	{
		return Matrix3x3(
			1.0f / k,	0,			0,
			0,			1.0f / k,	0,
			0,			0,			1
		);
	}

	// Operators
	decltype(auto) operator~()			const	// Inverse matrix
	{
		vec_t invdet = 1.0f / Det();

		Matrix3x3 mxInv;
		mxInv.dat[0][0] = (dat[1][1] * dat[2][2] - dat[2][1] * dat[1][2]) * invdet;
		mxInv.dat[0][1] = (dat[0][2] * dat[2][1] - dat[0][1] * dat[2][2]) * invdet;
		mxInv.dat[0][2] = (dat[0][1] * dat[1][2] - dat[0][2] * dat[1][1]) * invdet;
		mxInv.dat[1][0] = (dat[1][2] * dat[2][0] - dat[1][0] * dat[2][2]) * invdet;
		mxInv.dat[1][1] = (dat[0][0] * dat[2][2] - dat[0][2] * dat[2][0]) * invdet;
		mxInv.dat[1][2] = (dat[1][0] * dat[0][2] - dat[0][0] * dat[1][2]) * invdet;
		mxInv.dat[2][0] = (dat[1][0] * dat[2][1] - dat[2][0] * dat[1][1]) * invdet;
		mxInv.dat[2][1] = (dat[2][0] * dat[0][1] - dat[0][0] * dat[2][1]) * invdet;
		mxInv.dat[2][2] = (dat[0][0] * dat[1][1] - dat[1][0] * dat[0][1]) * invdet;

		return mxInv;
	}
	bool operator==(const Matrix3x3& m) const { return !memcmp(&dat, &m.dat, sizeof(dat)); }
	bool operator!=(const Matrix3x3& m) const { return !(*this == m); }

	decltype(auto) operator+(const Matrix3x3& m) const
	{
		Matrix3x3 mx;

		for (size_t i = 0; i < 3; i++)
		{
			mx.dat[i][0] = dat[i][0] + m.dat[i][0];
			mx.dat[i][1] = dat[i][1] + m.dat[i][1];
			mx.dat[i][2] = dat[i][2] + m.dat[i][2];
		}

		return mx;
	}
	decltype(auto) operator-(const Matrix3x3& m) const
	{
		Matrix3x3 mx;

		for (size_t i = 0; i < 3; i++)
		{
			mx.dat[i][0] = dat[i][0] - m.dat[i][0];
			mx.dat[i][1] = dat[i][1] - m.dat[i][1];
			mx.dat[i][2] = dat[i][2] - m.dat[i][2];
		}

		return mx;
	}
	decltype(auto) operator*(const Matrix3x3& m) const
	{
		Matrix3x3 mx;

		mx.dat[0][0] = dat[0][0] * m.dat[0][0] + dat[0][1] * m.dat[1][0] + dat[0][2] * m.dat[2][0];
		mx.dat[0][1] = dat[0][0] * m.dat[0][1] + dat[0][1] * m.dat[1][1] + dat[0][2] * m.dat[2][1];
		mx.dat[0][2] = dat[0][0] * m.dat[0][2] + dat[0][1] * m.dat[1][2] + dat[0][2] * m.dat[2][2];
		mx.dat[1][0] = dat[1][0] * m.dat[0][0] + dat[1][1] * m.dat[1][0] + dat[1][2] * m.dat[2][0];
		mx.dat[1][1] = dat[1][0] * m.dat[0][1] + dat[1][1] * m.dat[1][1] + dat[1][2] * m.dat[2][1];
		mx.dat[1][2] = dat[1][0] * m.dat[0][2] + dat[1][1] * m.dat[1][2] + dat[1][2] * m.dat[2][2];
		mx.dat[2][0] = dat[2][0] * m.dat[0][0] + dat[2][1] * m.dat[1][0] + dat[2][2] * m.dat[2][0];
		mx.dat[2][1] = dat[2][0] * m.dat[0][1] + dat[2][1] * m.dat[1][1] + dat[2][2] * m.dat[2][1];
		mx.dat[2][2] = dat[2][0] * m.dat[0][2] + dat[2][1] * m.dat[1][2] + dat[2][2] * m.dat[2][2];

		return mx;
	}

	decltype(auto) operator+=(const Matrix3x3& m) { return (*this = *this + m); }
	decltype(auto) operator-=(const Matrix3x3& m) { return (*this = *this - m); }
	decltype(auto) operator*=(const Matrix3x3& m) { return (*this = *this * m); }

	decltype(auto) operator=(std::nullptr_t) { return Zero(); }
	decltype(auto) operator*(float fl) const
	{
		Matrix3x3 mx;

		for (size_t i = 0; i < 3; i++)
		{
			mx.dat[i][0] = dat[i][0] * fl;
			mx.dat[i][1] = dat[i][1] * fl;
			mx.dat[i][2] = dat[i][2] * fl;
		}

		return mx;
	}
	decltype(auto) operator/(float fl) const
	{
		Matrix3x3 mx;

		for (size_t i = 0; i < 3; i++)
		{
			mx.dat[i][0] = dat[i][0] / fl;
			mx.dat[i][1] = dat[i][1] / fl;
			mx.dat[i][2] = dat[i][2] / fl;
		}

		return mx;
	}

	decltype(auto) operator*=(float fl) { return (*this = *this * fl); }
	decltype(auto) operator/=(float fl) { return (*this = *this / fl); }

	operator float* () { return &dat[0][0]; }
	operator const float* () const { return &dat[0][0]; }

	// Methods
	vec_t Det(void) const
	{
		return	dat[0][0] * (dat[1][1] * dat[2][2] - dat[2][1] * dat[1][2]) -
				dat[0][1] * (dat[1][0] * dat[2][2] - dat[1][2] * dat[2][0]) +
				dat[0][2] * (dat[1][0] * dat[2][1] - dat[1][1] * dat[2][0]);
	}

	// Members
	vec_t dat[3][3]
	{
		{1, 0, 0},
		{0, 1, 0},
		{0, 0, 1}
	};
};

inline Matrix3x3 operator*(float fl, const Matrix3x3& m)
{
	return m * fl;
}

inline Vector2D operator*(const Matrix3x3& m, const Vector2D& v)
{
	/*
	| a  b  u | | x |   | ax + by + u |
	| c  d  v | | y | = | cx + dy + v |
	| 0  0  1 | | 1 |   |      1      |

	For the practice sake, we just ignore the additional [1] from the last row.
	*/

	return Vector2D(
		m.dat[0][0] * v.x + m.dat[0][1] * v.y + m.dat[0][2],
		m.dat[1][0] * v.x + m.dat[1][1] * v.y + m.dat[1][2]
	);
}

// 3D Vector
// Same data-layout as engine's vec3_t, which is a vec_t[3]
class Vector
{
public:
	// Construction/destruction
	constexpr Vector() : x(0), y(0), z(0) {}
	constexpr Vector(vec_t X, vec_t Y, vec_t Z) : x(X), y(Y), z(Z) {}
	constexpr Vector(const Vector2D& v2d, vec_t Z) : x(v2d.x), y(v2d.y), z(Z) {}
	Vector(const Vector &v) { *(int *)&x = *(int *)&v.x; *(int *)&y = *(int *)&v.y; *(int *)&z = *(int *)&v.z; }
	Vector(const vec_t rgfl[3]) { *(int *)&x = *(int *)&rgfl[0]; *(int *)&y = *(int *)&rgfl[1]; *(int *)&z = *(int *)&rgfl[2]; }

	// Operators
	decltype(auto) operator-()       const { return Vector(-x, -y, -z); }
	bool operator==(const Vector &v) const { return x == v.x && y == v.y && z == v.z; }
	bool operator!=(const Vector &v) const { return !(*this == v); }

	decltype(auto) operator+(const Vector &v) const { return Vector(x + v.x, y + v.y, z + v.z); }
	decltype(auto) operator-(const Vector &v) const { return Vector(x - v.x, y - v.y, z - v.z); }
	decltype(auto) operator*(const Vector &v) const { return Vector(x * v.x, y * v.y, z * v.z); }
	decltype(auto) operator/(const Vector &v) const { return Vector(x / v.x, y / v.y, z / v.z); }

	decltype(auto) operator+=(const Vector &v) { return (*this = *this + v); }
	decltype(auto) operator-=(const Vector &v) { return (*this = *this - v); }
	decltype(auto) operator*=(const Vector &v) { return (*this = *this * v); }
	decltype(auto) operator/=(const Vector &v) { return (*this = *this / v); }

	decltype(auto) operator+(float fl) const { return Vector(x + fl, y + fl, z + fl); }
	decltype(auto) operator-(float fl) const { return Vector(x - fl, y - fl, z - fl); }

	// TODO: FIX ME!!
#ifdef PLAY_GAMEDLL
	decltype(auto) operator*(float fl) const { return Vector(vec_t(x * fl), vec_t(y * fl), vec_t(z * fl)); }
	decltype(auto) operator/(float fl) const { return Vector(vec_t(x / fl), vec_t(y / fl), vec_t(z / fl)); }
#else
	decltype(auto) operator*(float fl) const { return Vector(x * fl, y * fl, z * fl); }
	decltype(auto) operator/(float fl) const { return Vector(x / fl, y / fl, z / fl); }
#endif

	decltype(auto) operator=(std::nullptr_t) { return Vector(0, 0, 0); }
	decltype(auto) operator+=(float fl) { return (*this = *this + fl); }
	decltype(auto) operator-=(float fl) { return (*this = *this - fl); }
	decltype(auto) operator*=(float fl) { return (*this = *this * fl); }
	decltype(auto) operator/=(float fl) { return (*this = *this / fl); }

	// Methods
	void Clear()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	void CopyToArray(float *rgfl) const
	{
		*(int *)&rgfl[0] = *(int *)&x;
		*(int *)&rgfl[1] = *(int *)&y;
		*(int *)&rgfl[2] = *(int *)&z;
	}

	// Get the vector's magnitude
	real_t Length() const
	{
		real_t x1 = real_t(x);
		real_t y1 = real_t(y);
		real_t z1 = real_t(z);

		return Q_sqrt(x1 * x1 + y1 * y1 + z1 * z1);
	}

	// Get the vector's magnitude squared
	real_t LengthSquared() const { return (x * x + y * y + z * z); }

	operator float*()             { return &x; } // Vectors will now automatically convert to float * when needed
	operator const float*() const { return &x; } // Vectors will now automatically convert to float * when needed

#ifndef PLAY_GAMEDLL
	Vector Normalize() const
	{
		float flLen = Length();
		if (flLen == 0)
			return Vector(0, 0, 1);

		flLen = 1 / flLen;
		return Vector(x * flLen, y * flLen, z * flLen);
	}
#else
	Vector Normalize()
	{
		real_t flLen = Length();
		if (flLen == 0)
			return Vector(0, 0, 1);

		vec_t fTemp = vec_t(1 / flLen);
		return Vector(x * fTemp, y * fTemp, z * fTemp);
	}
#endif // PLAY_GAMEDLL
	// for out precision normalize
	Vector NormalizePrecision() const
	{
#ifndef PLAY_GAMEDLL
		return Normalize();
#else
		real_t flLen = Length();
		if (flLen == 0)
			return Vector(0, 0, 1);

		flLen = 1 / flLen;
		return Vector(vec_t(x * flLen), vec_t(y * flLen), vec_t(z * flLen));
#endif // PLAY_GAMEDLL
	}
	Vector2D Make2D() const
	{
		Vector2D Vec2;
		*(int *)&Vec2.x = *(int *)&x;
		*(int *)&Vec2.y = *(int *)&y;
		return Vec2;
	}

	real_t Length2D() const { return Q_sqrt(real_t(x * x + y * y)); }

	inline bool IsLengthLessThan   (float length) const { return (LengthSquared() < length * length); }
	inline bool IsLengthGreaterThan(float length) const { return (LengthSquared() > length * length); }

#ifdef PLAY_GAMEDLL
	template<typename T = real_t>
	real_t NormalizeInPlace()
	{
		T flLen = Length();

		if (flLen > 0)
		{
			x = vec_t(1 / flLen * x);
			y = vec_t(1 / flLen * y);
			z = vec_t(1 / flLen * z);
		}
		else
		{
			x = 0;
			y = 0;
			z = 1;
		}

		return flLen;
	}
#else // PLAY_GAMEDLL
	float NormalizeInPlace()
	{
		float flLen = Length();
		if (flLen > 0)
		{
			x /= flLen;
			y /= flLen;
			z /= flLen;
		}
		else
		{
			x = 0;
			y = 0;
			z = 1;
		}
		return flLen;
	}
#endif // PLAY_GAMEDLL
	bool IsZero(float tolerance = 0.01f) const
	{
		return (x > -tolerance && x < tolerance &&
			y > -tolerance && y < tolerance &&
			z > -tolerance && z < tolerance);
	}

	Vector MakeVector() const
	{
		vec_t rad_pitch = (pitch * M_PI / 180.0f);
		vec_t rad_yaw = (yaw * M_PI / 180.0f);
		vec_t tmp = Q_cos(rad_pitch);

		return Vector(	(-tmp * -Q_cos(rad_yaw)),	// x
						(Q_sin(rad_yaw) * tmp),		// y
						-Q_sin(rad_pitch)			// z
		);
	}

	Vector VectorAngles(void) const
	{
		Vector a;
		a.pitch = 0;
		a.yaw = 0;
		a.roll = 0;

		if (y == 0 && x == 0)
		{
			a.yaw = 0;
			if (z > 0)
				a.pitch = 90;
			else
				a.pitch = 270;
		}
		else
		{
			a.yaw = (Q_atan2(-y, x) * 180.0f / M_PI);
			if (a.yaw < 0)
				a.yaw += 360;

			a.yaw = 360.0f - a.yaw;	// LUNA: why???

			auto tmp = Q_sqrt(x * x + y * y);
			a.pitch = (Q_atan2(z, tmp) * 180.0f / M_PI);
			if (a.pitch < 0)
				a.pitch += 360;
		}

		return a;
	}

	Vector RotateX(float angle) const
	{
		float a, c, s;

		a = (angle * M_PI / 180.0);
		c = Q_cos(a);
		s = Q_sin(a);

		return Vector(	x,
						c * y - s * z,
						s * y + c * z
		);
	}

	Vector RotateY(float angle) const
	{
		float a, c, s;

		a = (angle * M_PI / 180.0);
		c = Q_cos(a);
		s = Q_sin(a);

		return Vector(	c * x + s * z,
						y,
						-s * x + c * z
		);
	}

	Vector RotateZ(float angle) const
	{
		float a, c, s;

		a = (angle * M_PI / 180.0);
		c = Q_cos(a);
		s = Q_sin(a);

		return Vector(	c * x - s * y,
						s * x + c * y,
						z
		);
	}

	// Members
	union { vec_t x; vec_t pitch;	vec_t r; };
	union { vec_t y; vec_t yaw;		vec_t g; };
	union { vec_t z; vec_t roll;	vec_t b; };
};

inline Vector operator*(float fl, const Vector &v)
{
	return v * fl;
}

inline real_t DotProduct(const Vector &a, const Vector &b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

inline real_t DotProduct2D(const Vector& a, const Vector& b)
{
	return (a.x * b.x + a.y * b.y);
}

inline real_t DotProduct2D(const Vector2D &a, const Vector2D &b)
{
	return (a.x * b.x + a.y * b.y);
}

inline Vector CrossProduct(const Vector &a, const Vector &b)
{
	return Vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

inline float operator^(const Vector& a, const Vector& b)
{
	float length_ab = a.Length() * b.Length();

	if (length_ab == 0.0)
		return 0.0;

	return (double)(Q_acos(DotProduct(a, b) / length_ab) * (180.0 / M_PI));
}

template<
	typename X,
	typename Y,
	typename Z,
	typename LenType
>
inline LenType LengthSubtract(Vector vecStart, Vector vecDest)
{
	X floatX = (vecDest.x - vecStart.x);
	Y floatY = (vecDest.y - vecStart.y);
	Z floatZ = (vecDest.z - vecStart.z);

	return Q_sqrt(real_t(floatX * floatX + floatY * floatY + floatZ * floatZ));
}

template<
	typename X,
	typename Y,
	typename Z,
	typename LenType
>
inline Vector NormalizeSubtract(Vector vecStart, Vector vecDest)
{
	Vector dir;

#ifdef PLAY_GAMEDLL

	X floatX = (vecDest.x - vecStart.x);
	Y floatY = (vecDest.y - vecStart.y);
	Z floatZ = (vecDest.z - vecStart.z);

	LenType flLen = Q_sqrt(real_t(floatX * floatX + floatY * floatY + floatZ * floatZ));

	if (flLen == 0.0)
	{
		dir = Vector(0, 0, 1);
	}
	else
	{
		flLen = 1.0 / flLen;

		dir.x = vec_t(floatX * flLen);
		dir.y = vec_t(floatY * flLen);
		dir.z = vec_t(floatZ * flLen);
	}
#else
	dir = (vecDest - vecStart).Normalize();
#endif // PLAY_GAMEDLL

	return dir;
}

#ifdef PLAY_GAMEDLL
template<typename X, typename Y, typename LenType>
inline Vector NormalizeMulScalar(Vector2D vec, float scalar)
{
	LenType flLen;
	X floatX;
	Y floatY;

	flLen = (LenType)vec.Length();

	if (flLen <= 0.0)
	{
		floatX = 1;
		floatY = 0;
	}
	else
	{
		flLen = 1 / flLen;

		floatX = vec.x * flLen;
		floatY = vec.y * flLen;
	}

	return Vector(vec_t(floatX * scalar), vec_t(floatY * scalar), 0);
}
template<typename X, typename Y, typename LenType, typename LenCast>
inline Vector NormalizeMulScalar(Vector vec, float scalar)
{
	LenType flLen;
	X floatX = vec.x;
	Y floatY = vec.y;

	flLen = (LenType)vec.Length();

	if (flLen <= 0.0)
	{
		floatX = 1;
		floatY = 0;
	}
	else
	{
		floatX = floatX * LenCast(1 / flLen);
		floatY = floatY * LenCast(1 / flLen);
	}

	return Vector(vec_t(floatX * scalar), vec_t(floatY * scalar), 0);
}
#endif // PLAY_GAMEDLL
