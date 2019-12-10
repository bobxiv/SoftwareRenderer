#pragma once

#ifndef VECTOR4_SIMD_H
#define VECTOR4_SIMD_H

//This Vector class is created only if using SIMD operations because is implemented in the most efficient way(that i know of)
//for the SIMD operations, ie using overloaded operations of the class. In contrast if the SIMD operations are not implemented
//then the class that uses expression templates(that are best for fpu but not so good for SIMD) inside Vector4_ExpressionTemplates is used
#ifdef SIMD_EXTENSION

#include "sse_type.h"

namespace MathModule
{
	using namespace SIMD;

	template <typename Component, typename SIMDArray>
	class Matrix4x4;

	///////////////////////////////////////////
	// Vector4
	///////////////////////////////////////////
	template <typename Component, typename SIMDArray>
	class Vector4
	{
	public:

		//This is because so the Matrix4x4::operator* can access the data of Vector4
		friend Matrix4x4<Component,SIMDArray>;

		//------------------------------------------------------------
		// Constructors interface implementation
		//------------------------------------------------------------

		INLINE Vector4() { }

		INLINE explicit Vector4(Component *pVec)
			: _xyzw(simd_set(pVec))
		{ }

		INLINE explicit Vector4(Component f)
			: _xyzw(simd_replicate(f))
		{ }

		INLINE explicit Vector4(const Component x, const Component y, const Component z, const Component w)
			: _xyzw(simd_set(x, y, z, w))
		{ }

		INLINE explicit Vector4(const SIMDArray& xyzw)
			: _xyzw(xyzw)
		{ }

		INLINE Vector4(const Vector4& copy)
			: _xyzw(copy._xyzw)
		{ }

		INLINE Vector4& operator= (const Vector4& copy)
		{
			_xyzw = copy._xyzw;
			return *this;
		}

		//------------------------------------------------------------
		// Comparison interface implementation
		//------------------------------------------------------------

		INLINE bool operator==(const Vector4& b) const
		{
			return simd_cmpeq(_xyzw, b._xyzw);
		}

		INLINE bool operator!=(const Vector4& b) const
		{
			return !simd_cmpeq(_xyzw, b._xyzw);
		}

		//------------------------------------------------------------
		// Arithmetic interface implementation
		//------------------------------------------------------------

		INLINE Vector4& operator+= (const Vector4& rhs)
		{
			_xyzw = simd_add(_xyzw, rhs._xyzw);
			return *this;
		}

		INLINE Vector4& operator-= (const Vector4& rhs)
		{
			_xyzw = simd_subtract(_xyzw, rhs._xyzw);
			return *this;
		}

		INLINE Vector4& operator*= (const Vector4& rhs)
		{
			_xyzw = simd_multiply(_xyzw, rhs._xyzw);
			return *this;
		}

		INLINE Vector4& operator*= (const Component rhs)
		{
			_xyzw = simd_multiply(_xyzw, simd_replicate(rhs));
			return *this;
		}

		INLINE Vector4& operator/= (const Vector4& rhs)
		{
			#ifdef ESTIMATE_DIVISION
				_xyzw = simd_divide_est(_xyzw, rhs._xyzw);
			#else
				_xyzw = simd_divide(_xyzw, rhs._xyzw);
			#endif
			return *this;
		}

		INLINE Vector4& operator/= (const Component rhs)
		{
			#ifdef ESTIMATE_DIVISION
				_xyzw = simd_divide_est(_xyzw, simd_replicate(rhs));
			#else
				_xyzw = simd_divide(_xyzw, simd_replicate(rhs));
			#endif
			return *this;
		}

		INLINE Vector4 operator+ (const Vector4& rhs) const
		{
			return Vector4(simd_add(_xyzw, rhs._xyzw));
		}

		INLINE Vector4 operator- (const Vector4& rhs) const
		{
			return Vector4(simd_subtract(_xyzw, rhs._xyzw));
		}

		INLINE Vector4 operator- () const
		{
			return Vector4(simd_negate(_xyzw));
		}

		INLINE Vector4 operator* (const Vector4& rhs) const
		{
			return Vector4(simd_multiply(_xyzw, rhs._xyzw));
		}

		INLINE Vector4 operator* (const Component rhs) const
		{
			return Vector4(simd_multiply(_xyzw, simd_replicate(rhs)));
		}

		INLINE Vector4 operator/ (const Vector4& rhs) const
		{
			#ifdef ESTIMATE_DIVISION
				return Vector4(simd_divide_est(_xyzw, rhs._xyzw));
			#else
				return Vector4(simd_divide(_xyzw, rhs._xyzw));
			#endif
		}

		INLINE Vector4 operator/ (const Component rhs) const
		{
			#ifdef ESTIMATE_DIVISION
				return Vector4(simd_divide_est(_xyzw, simd_replicate(rhs)));
			#else
				return Vector4(simd_divide(_xyzw, simd_replicate(rhs)));
			#endif
		}

		//------------------------------------------------------------
		// Object vector operations interface implementation
		//------------------------------------------------------------

		INLINE void Normalize()
		{
			//we dont want to normalize w component(it just tells if it is a point or a vector),
			//so we put it to zero temporary and reset it after the normalization
			Component auxW = simd_get_element(_xyzw, 3);
			simd_set_element(_xyzw, static_cast<Component>(0), 3);
			#ifdef ESTIMATE_SQRT
				#ifdef ESTIMATE_DIVISION
					_xyzw = simd_divide_est(_xyzw, simd_sqrt_est(simd_dot4(_xyzw, _xyzw)));
				#else
					_xyzw = simd_divide(_xyzw, simd_sqrt_est(simd_dot4(_xyzw, _xyzw)));
				#endif
			#else
				#ifdef ESTIMATE_DIVISION
					_xyzw = simd_divide_est(_xyzw, simd_sqrt(simd_dot4(_xyzw, _xyzw)));
				#else
					_xyzw = simd_divide(_xyzw, simd_sqrt(simd_dot4(_xyzw, _xyzw)));
				#endif
			#endif
			simd_set_element(_xyzw, auxW, 3);
		}

		INLINE void PerspectiveDivision()
		{
			#ifdef ESTIMATE_DIVISION
				_xyzw = simd_divide_est(_xyzw, simd_replicate( simd_get_element(_xyzw, 3) ));
			#else
				_xyzw = simd_divide(_xyzw, simd_replicate( simd_get_element(_xyzw, 3) ));
			#endif
		}

		INLINE Component GetLength() const
		{
			#ifdef ESTIMATE_SQRT
				return simd_get_element(simd_sqrt_est(simd_dot4(_xyzw, _xyzw)), 0);
			#else
				return simd_get_element(simd_sqrt(simd_dot4(_xyzw, _xyzw)), 0);
			#endif
		}

		INLINE Component GetSquareLength() const
		{
			return simd_get_element(simd_dot4(_xyzw, _xyzw), 0);
		}

		//------------------------------------------------------------
		// Access interface implementation
		//------------------------------------------------------------

		INLINE void GetXYZW(Component *pVec) const
		{
			simd_get_elements(pVec, _xyzw);
		}

		INLINE Component GetX() const
		{
			return simd_get_element(_xyzw, 0);
		}

		INLINE Component GetY() const
		{
			return simd_get_element(_xyzw, 1);
		}

		INLINE Component GetZ() const
		{
			return simd_get_element(_xyzw, 2);
		}

		INLINE Component GetW() const
		{
			return simd_get_element(_xyzw, 3);
		}

		INLINE void SetX(Component x)
		{
			simd_set_element(_xyzw, x, 0);
		}

		INLINE void SetY(Component y)
		{
			simd_set_element(_xyzw, y, 1);
		}

		INLINE void SetZ(Component z)
		{
			simd_set_element(_xyzw, z, 2);
		}

		INLINE void SetW(Component w)
		{
			simd_set_element(_xyzw, w, 3);
		}

		//------------------------------------------------------------
		// Static operantions interface implementation
		//------------------------------------------------------------

		static INLINE Component Dot(const Vector4& va, const Vector4& vb)
		{
			return simd_get_element(simd_dot4(va._xyzw, vb._xyzw), 0);
		}

		static INLINE Vector4 Cross(const Vector4& va, const Vector4& vb)
		{
			return Vector4(simd_cross(va._xyzw, vb._xyzw));
		}

		static INLINE Vector4 Sqrt(const Vector4& va)
		{
			#ifdef ESTIMATE_SQRT
				return Vector4(simd_sqrt_est(va._xyzw));
			#else
				return Vector4(simd_sqrt(va._xyzw));
			#endif
		}

		static INLINE Vector4 Clamp(const Vector4& v, const Vector4& vmin, const Vector4& vmax)
		{
			return Vector4(simd_clamp(v._xyzw, vmin._xyzw, vmax._xyzw));
		}

		static INLINE Vector4 Saturate(const Vector4& v)
		{
			return Vector4(simd_saturate(v._xyzw));
		}

		// Linear interpolation between 2 vectors.
		// A scale of 0 gives va, a scale of 1 gives vb.
		// Parameters:
		// va	First vector of the operation
		// vb	Second vector of the operation
		// s	Scale of the interpolation
		//
		// Return: the interpolated vector
		static INLINE Vector4 Lerp(const Vector4& va, const Vector4& vb, const float s)
		{
			return Vector4( simd_add( va._xyzw, simd_multiply( simd_subtract(vb._xyzw, va._xyzw), simd_replicate(s) ) ) );
		}

		// Linear interpolation between 2 vectors positions pa and pb with values
		// va and vb respectiblely. The function calculates the value at vector position
		// pc(WICH MUST BE BETWEEN PA AND PB), ie the value of vc using a linear interpolation
		// between va and vb.
		// Parameters:
		// pa	First position vector
		// va	The value at position pa
		// pb	Second position vector
		// vb	The value at position pb
		// pc	The position vector between pa and pb where to calc the value
		//
		// Return: the interpolated value at position pc
		static INLINE Vector4 Lerp(const Vector4& pa, const Vector4& va, const Vector4& pb, const Vector4& vb, const Vector4& pc)
		{
			/*
				If A and B are 2 position vectors, C is a position vector between A and B. And f(X) is a function
				of a vector position X, given that f(A) and f(B) are known the value at vector position C => f(C) is:

							  C
						A -------------
									   ---------- B

						f(C) = f(A)*|B-C|/|B-A| + f(B)*|C-A|/|B-A|
			*/
			SIMDArray BA_length = simd_subtract(pb._xyzw, pa._xyzw);
			BA_length = simd_sqrt(simd_dot4(BA_length, BA_length));

			SIMDArray BC_length = simd_subtract(pb._xyzw, pc._xyzw);
			length = simd_sqrt(simd_dot4(BC_length, BC_length));

			SIMDArray CA_length = simd_subtract(pc._xyzw, pa._xyzw);
			length = simd_sqrt(simd_dot4(CA_length, CA_length));

			return Vector4( simd_add( simd_multiply(va, simd_divide(BC_length, BA_length)), simd_multiply(vb, simd_divide(CA_length, BA_length)) ) );
		}
		
		static INLINE void CalcBaricentricCoords3D(const Vector4& v1, const Vector4& v2, const Vector4& v3,
			const Vector4& p, Component &b1, Component &b2, Component &b3)
		{
			SIMDArray e1 = simd_subtract(v2._xyzw, v1._xyzw);
			SIMDArray e2 = simd_subtract(v3._xyzw, v2._xyzw);
			SIMDArray e3 = simd_subtract(v1._xyzw, v3._xyzw);
			SIMDArray d1 = simd_subtract(p._xyzw, v1._xyzw);
			SIMDArray d2 = simd_subtract(p._xyzw, v2._xyzw);
			SIMDArray d3 = simd_subtract(p._xyzw, v3._xyzw);

			SIMDArray n = simd_cross(e1, e2);

			SIMDArray denom = simd_dot4(simd_cross(e1, e2), n);

			#ifdef ESTIMATE_DIVISION
				b1 = simd_get_element( simd_divide_est(simd_dot4(simd_cross(e2, d2), n),  denom) , 0);
				b2 = simd_get_element( simd_divide_est(simd_dot4(simd_cross(e3, d3), n),  denom) , 0);
				b3 = simd_get_element( simd_divide_est(simd_dot4(simd_cross(e1, d1), n),  denom) , 0);
			#else
				b1 = simd_get_element( simd_divide(simd_dot4(simd_cross(e2, d2), n),  denom) , 0);
				b2 = simd_get_element( simd_divide(simd_dot4(simd_cross(e3, d3), n),  denom) , 0);
				b3 = simd_get_element( simd_divide(simd_dot4(simd_cross(e1, d1), n),  denom) , 0);
			#endif
		}

		static INLINE void CalcBaricentricCoords2D(const Vector4& v1, const Vector4& v2, const Vector4& v3,
			const Vector4& p, Component &b1, Component &b2, Component &b3)
		{
			float y1 = simd_get_element(v1._xyzw,1);
			float y2 = simd_get_element(v2._xyzw,1);
			float y3 = simd_get_element(v3._xyzw,1);
			float x1 = simd_get_element(v1._xyzw,0);
			float x2 = simd_get_element(v2._xyzw,0);
			float x3 = simd_get_element(v3._xyzw,0);
			SIMDArray py = simd_replicate(simd_get_element(p._xyzw,1));
			SIMDArray px = simd_replicate(simd_get_element(p._xyzw,0));
			SIMDArray auxX1 = simd_set(x2, x3, x1, 0);
			SIMDArray auxX2 = simd_swizzle<1,2,0,3>(auxX1);

			SIMDArray auxY1 = simd_set(y2, y3, y1, 0);
			SIMDArray auxY2 = simd_swizzle<1,2,0,3>(auxY1);

			SIMDArray denom = simd_replicate( (y1-y3)*(x2-x3)+(y2-y3)*(x3-x1) );

			//SIMDArray b = ( (py-auxY2)*(auxX1-auxX2)+(auxY1-auxY2)*(auxX2-px) ) / denom;
			#ifdef ESTIMATE_DIVISION
				SIMDArray b = simd_divide_est(simd_add(simd_multiply(simd_subtract(py,auxY2),simd_subtract(auxX1,auxX2)),simd_multiply(simd_subtract(auxY1,auxY2),simd_subtract(auxX2,px))),denom);
			#else
				SIMDArray b = simd_divide(simd_add(simd_multiply(simd_subtract(py,auxY2),simd_subtract(auxX1,auxX2)),simd_multiply(simd_subtract(auxY1,auxY2),simd_subtract(auxX2,px))),denom);
			#endif
			b1 = simd_get_element(b,0);
			b2 = simd_get_element(b,1);
			b3 = simd_get_element(b,2);
		}

		static INLINE Vector4 EvalBaricentricCoords(const Vector4& v1, const Vector4& v2, const Vector4& v3, 
			const Component b1, const Component b2, const Component b3)
		{
			SIMDArray res = simd_multiply( simd_replicate(b1), v1._xyzw);
			res = simd_multiply_add( simd_replicate(b2), v2._xyzw, res);
			res = simd_multiply_add( simd_replicate(b3), v3._xyzw, res);

			return Vector4(res);
		}

	private:
		// Array [X,Y,Z,W] represented by a SIMD values
		SIMDArray _xyzw;
	} ;	

	//Instanciate the template to a concrete given SIMD instruction set type
	#if SSE_LEVEL > 0
		typedef Vector4<float, sse_type> Vector4f;
	#endif
	//When more simd instructions sets are implemented here we should add the else if 
	//so Vector4 uses that type
	//#else if 

}

#endif

#endif // end VECTOR4_SIMD_H