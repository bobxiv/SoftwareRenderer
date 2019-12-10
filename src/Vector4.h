#ifndef Vector4f_H
#define Vector4f_H

#ifdef SIMD_EXTENSION
	#include <xmmintrin.h>

	class Vector4f
	{
	public:

		__declspec(align(16)) union
		{
			__m128 m_xyzw;
			struct
			{
				float m_x, m_y, m_z, m_w;
			};
		};

		Vector4f(const __m128 &value): m_xyzw(value)
		{}

	public:

		Vector4f()
		{}

		Vector4f(const Vector4f &B)
		{
			m_xyzw = B.m_xyzw;
		}

		/**
		 * Creates a vector with the components passed as parameters.
		 *
		 * \param x First component.
		 * \param y Second component.
		 * \param z Third component.
		 * \param w Fourth component.
		 */
		Vector4f(const float x, const float y, const float z, const float w)
		{
			m_xyzw = _mm_set_ps(w,z,y,x);//set in the reverse order so m_x, m_y, m_z and m_w will be right
		}

		inline Vector4f& operator=(const Vector4f &B)
		{
			m_xyzw = B.m_xyzw;
			return *this;
		}

		inline bool operator==(const Vector4f &B)
		{
			//REALLY INEFICIENT!!
			//_mm_cmpeq_ps(m_xyzw, B.m_xyzw)
			return ((m_x == B.m_x) && (m_y == B.m_y) && (m_z == B.m_z) && (m_w == B.m_w));
		}

		//Vector addition
		inline Vector4f operator+(const Vector4f &B) const
		{
			return Vector4f( _mm_add_ps(m_xyzw, B.m_xyzw) );
		}
	
		//Vector subtraction
		inline Vector4f operator-(const Vector4f &B) const
		{
			return Vector4f( _mm_sub_ps(m_xyzw, B.m_xyzw) );
		}

		//Multiplication component per component
		inline Vector4f operator*(const Vector4f &B) const
		{
			return Vector4f( _mm_mul_ps(m_xyzw, B.m_xyzw) );
		}

		//Division component per component
		inline Vector4f operator/(const Vector4f &B) const
		{
			return Vector4f( _mm_div_ps(m_xyzw, B.m_xyzw) );
		}

		//Dot product
		inline float Dot(const Vector4f &B) const
		{
			Vector4f aux = this->operator*(B);
			return (aux.m_x+aux.m_y+aux.m_z+aux.m_w);//surely not eficient!
		}

		//Scalar product
		inline Vector4f operator*(const float B) const
		{
			__m128 scalar = _mm_set1_ps(B);
			return Vector4f( _mm_mul_ps(m_xyzw, scalar) );
		}

		inline Vector4f SquareLength() const
		{
			return Vector4f( _mm_sqrt_ps(m_xyzw) );
		}

	};

#else

	#include <math.h>

	class Vector4f
	{
	public:

		float m_x, m_y, m_z, m_w;

	public:

		Vector4f()
		{}

		Vector4f(const Vector4f &B): m_x(B.m_x), m_y(B.m_y), m_z(B.m_z), m_w(B.m_w)
		{}

		inline explicit Vector4f(const float x, const float y, const float z, const float w): m_x(x), m_y(y), m_z(z), m_w(w)
		{}

		inline Vector4f& operator=(const Vector4f &B)
		{
			m_x = B.m_x;
			m_y = B.m_y;
			m_z = B.m_z;
			m_w = B.m_w;
			return *this;
		}

		inline bool operator==(const Vector4f &B)
		{
			
			return ((m_x == B.m_x) && (m_y == B.m_y) && (m_z == B.m_z) && (m_w == B.m_w));
		}

		//Vector addition
		inline Vector4f operator+(const Vector4f &B) const
		{
			return Vector4f( m_x+B.m_x, m_y+B.m_y, m_z+B.m_z, m_w+B.m_w );
		}
	
		//Vector subtraction
		inline Vector4f operator-(const Vector4f &B) const
		{
			return Vector4f( m_x-B.m_x, m_y-B.m_y, m_z-B.m_z, m_w-B.m_w );
		}

		//Multiplication component per component
		inline Vector4f operator*(const Vector4f &B) const
		{
			return Vector4f( m_x*B.m_x, m_y*B.m_y, m_z*B.m_z, m_w*B.m_w );
		}

		//Division component per component
		inline Vector4f operator/(const Vector4f &B) const
		{
			return Vector4f( m_x/B.m_x, m_y/B.m_y, m_z/B.m_z, m_w/B.m_w );
		}

		//Dot product
		static inline float Dot(const Vector4f& va, const Vector4f& vb)
		{
			return (va.m_x*vb.m_x+va.m_y*vb.m_y+va.m_z*vb.m_z+va.m_w*vb.m_w);
		}

		//Scalar product
		inline Vector4f operator*(const float B) const
		{
			return Vector4f(m_x*B, m_y*B, m_z*B, m_w*B);
		}

		inline Vector4f SquareLength() const
		{
			return Vector4f( sqrtf(m_x), sqrtf(m_y), sqrtf(m_z), sqrtf(m_w) );
		}

		static inline Vector4f Cross(const Vector4f& va, const Vector4f& vb)
		{
			return Vector4f(va.m_y*vb.m_z-va.m_z*vb.m_y, va.m_z*vb.m_x-va.m_x*vb.m_z, va.m_x*vb.m_y-va.m_y*vb.m_x, 0.0f);
		}

		inline void SetX(float x)
		{
			m_x = x;
		}

		inline void SetY(float y)
		{
			m_y = y;
		}

		inline void SetZ(float z)
		{
			m_z = z;
		}

		inline void SetW(float w)
		{
			m_w = w;
		}

	};

#endif

#endif