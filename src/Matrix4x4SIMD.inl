#pragma once

#ifndef MATRIX4X4_SIMD_H
#define MATRIX4X4_SIMD_H

#include "Vector4SIMD.inl"

//This Vector class is created only if using SIMD operations because is implemented in the most efficient way(that i know of)
//for the SIMD operations, ie using overloaded operations of the class. In contrast if the SIMD operations are not implemented
//then the class that uses expression templates(that are best for fpu but not so good for SIMD) inside Vector4_ExpressionTemplates is used
#ifdef SIMD_EXTENSION

#include "sse_type.h"

namespace MathModule
{
	using namespace SIMD;

	///////////////////////////////////////////
	// Matrix4x4
	///////////////////////////////////////////
	template <typename Component, typename SIMDArray>
	class Matrix4x4
	{
	public:
		//------------------------------------------------------------
		// Constructors interface implementation
		//------------------------------------------------------------

		INLINE Matrix4x4() { }

		INLINE explicit Matrix4x4(Component *pVec)
			: _column1(simd_set(pVec)), _column2(simd_set(pVec+4)), _column3(simd_set(pVec+8)), _column4(simd_set(pVec+12))
		{ }

		INLINE explicit Matrix4x4(Component f)
			: _column1(simd_replicate(f)), _column2(simd_replicate(f)), _column3(simd_replicate(f)), _column4(simd_replicate(f))
		{ }

		INLINE explicit Matrix4x4(const Component c11, const Component c21, const Component c31, const Component c41,
								  const Component c12, const Component c22, const Component c32, const Component c42,
								  const Component c13, const Component c23, const Component c33, const Component c43,
								  const Component c14, const Component c24, const Component c34, const Component c44)
			: _column1(simd_set(c11, c21, c31, c41)), _column2(simd_set(c12, c22, c32, c42)),
			  _column3(simd_set(c13, c23, c33, c43)), _column4(simd_set(c14, c24, c34, c44))
		{ }

		INLINE explicit Matrix4x4(const SIMDArray& col1, const SIMDArray& col2, const SIMDArray& col3, const SIMDArray& col4)
			: _column1(col1), _column2(col2), _column3(col3), _column4(col4)
		{ }

		INLINE Matrix4x4(const Matrix4x4& copy)
			: _column1(copy._column1), _column2(copy._column2), _column3(copy._column3), _column4(copy._column4)
		{ }

		INLINE Matrix4x4& operator= (const Matrix4x4& copy)
		{
			_column1 = copy._column1;
			_column2 = copy._column2;
			_column3 = copy._column3;
			_column4 = copy._column4;
			return *this;
		}

		//------------------------------------------------------------
		// Comparison interface implementation
		//------------------------------------------------------------

		INLINE bool operator==(const Matrix4x4& b) const
		{
			return ( simd_cmpeq(_column1, b._column1) && simd_cmpeq(_column2, b._column2) &&
				     simd_cmpeq(_column3, b._column3) && simd_cmpeq(_column4, b._column4) );
		}

		INLINE bool operator!=(const Matrix4x4& b) const
		{
			return ( !simd_cmpeq(_column1, b._column1) && !simd_cmpeq(_column2, b._column2) &&
				     !simd_cmpeq(_column3, b._column3) && !simd_cmpeq(_column4, b._column4) );
		}

		//------------------------------------------------------------
		// Arithmetic interface implementation
		//------------------------------------------------------------

		INLINE Matrix4x4& operator+= (const Matrix4x4& rhs)
		{
			_column1 = simd_add(_column1, rhs._column1);
			_column2 = simd_add(_column2, rhs._column2);
			_column3 = simd_add(_column3, rhs._column3);
			_column4 = simd_add(_column4, rhs._column4);
			return *this;
		}

		INLINE Matrix4x4& operator-= (const Matrix4x4& rhs)
		{
			_column1 = simd_subtract(_column1, rhs._column1);
			_column2 = simd_subtract(_column2, rhs._column2);
			_column3 = simd_subtract(_column3, rhs._column3);
			_column4 = simd_subtract(_column4, rhs._column4);
			return *this;
		}
		
		INLINE Matrix4x4& operator*= (const Matrix4x4& rhs)
		{
			SIMDArray res[4];
			
			res[0] = simd_multiply(_column1, simd_replicate(rhs._column1.m128_f32[0]) );

			res[0] = simd_multiply_add(_column2, simd_replicate(rhs._column1.m128_f32[1]), res[0]);
			res[0] = simd_multiply_add(_column3, simd_replicate(rhs._column1.m128_f32[2]), res[0]);
			res[0] = simd_multiply_add(_column4, simd_replicate(rhs._column1.m128_f32[3]), res[0]);
				
			res[1] = simd_multiply(_column1, simd_replicate(rhs._column2.m128_f32[0]) );

			res[1] = simd_multiply_add(_column2, simd_replicate(rhs._column2.m128_f32[1]), res[1]);
			res[1] = simd_multiply_add(_column3, simd_replicate(rhs._column2.m128_f32[2]), res[1]);
			res[1] = simd_multiply_add(_column4, simd_replicate(rhs._column2.m128_f32[3]), res[1]);

			res[2] = simd_multiply(_column1, simd_replicate(rhs._column3.m128_f32[0]) );

			res[2] = simd_multiply_add(_column2, simd_replicate(rhs._column3.m128_f32[1]), res[2]);
			res[2] = simd_multiply_add(_column3, simd_replicate(rhs._column3.m128_f32[2]), res[2]);
			res[2] = simd_multiply_add(_column4, simd_replicate(rhs._column3.m128_f32[3]), res[2]);

			res[3] = simd_multiply(_column1, simd_replicate(rhs._column4.m128_f32[0]) );

			res[3] = simd_multiply_add(_column2, simd_replicate(rhs._column4.m128_f32[1]), res[3]);
			res[3] = simd_multiply_add(_column3, simd_replicate(rhs._column4.m128_f32[2]), res[3]);
			res[3] = simd_multiply_add(_column4, simd_replicate(rhs._column4.m128_f32[3]), res[3]);

			_column1 = res[0];
			_column2 = res[1];
			_column3 = res[2];
			_column4 = res[3];
			return *this;
		}

		INLINE Matrix4x4& operator*= (const Component rhs)
		{
			SIMDArray aux = simd_replicate(rhs);

			_column1 = simd_multiply(_column1, aux);
			_column2 = simd_multiply(_column2, aux);
			_column3 = simd_multiply(_column3, aux);
			_column4 = simd_multiply(_column4, aux);
			return *this;
		}

		INLINE Matrix4x4& operator/= (const Component rhs)
		{
			SIMDArray aux = simd_replicate(rhs);

			#ifdef ESTIMATE_DIVISION
				_column1 = simd_divide_est(_column1, aux);
				_column2 = simd_divide_est(_column2, aux);
				_column3 = simd_divide_est(_column3, aux);
				_column4 = simd_divide_est(_column4, aux);
			#else
				_column1 = simd_divide(_column1, aux);
				_column2 = simd_divide(_column2, aux);
				_column3 = simd_divide(_column3, aux);
				_column4 = simd_divide(_column4, aux);
			#endif
			return *this;
		}

		INLINE Matrix4x4 operator+ (const Matrix4x4& rhs) const
		{
			return Matrix4x4(simd_add(_column1, rhs._column1), simd_add(_column2, rhs._column2),
				             simd_add(_column3, rhs._column3), simd_add(_column4, rhs._column4));
		}

		INLINE Matrix4x4 operator- (const Matrix4x4& rhs) const
		{
			return Matrix4x4(simd_subtract(_column1, rhs._column1), simd_subtract(_column2, rhs._column2),
							 simd_subtract(_column3, rhs._column3), simd_subtract(_column4, rhs._column4));
		}

		INLINE Matrix4x4 operator* (const Matrix4x4& rhs) const
		{
			/*SIMDArray row1 = rhs._column1;
			SIMDArray row2 = rhs._column2;
			SIMDArray row3 = rhs._column3;
			SIMDArray row4 = rhs._column4;
			_MM_TRANSPOSE4_PS(row1, row2, row3, row4);//MS specific
			return Matrix4x4(simd_dot4(row1, rhs._column1).m128_f32[0], simd_dot4(row2, rhs._column1).m128_f32[0],
							 simd_dot4(row3, rhs._column1).m128_f32[0], simd_dot4(row4, rhs._column1).m128_f32[0],
							 simd_dot4(row1, rhs._column2).m128_f32[0], simd_dot4(row2, rhs._column2).m128_f32[0],
							 simd_dot4(row3, rhs._column2).m128_f32[0], simd_dot4(row4, rhs._column2).m128_f32[0],
							 simd_dot4(row1, rhs._column3).m128_f32[0], simd_dot4(row2, rhs._column3).m128_f32[0],
							 simd_dot4(row3, rhs._column3).m128_f32[0], simd_dot4(row4, rhs._column3).m128_f32[0],
							 simd_dot4(row1, rhs._column4).m128_f32[0], simd_dot4(row2, rhs._column4).m128_f32[0],
							 simd_dot4(row3, rhs._column4).m128_f32[0], simd_dot4(row4, rhs._column4).m128_f32[0] );*/

			// C=A*B
			//			For each column j of C we have:
			//			c_1j = a_11 * b_1j + a_12 * b_2j + a_13 * b_3j + a_14 * b_4j
			//			c_2j = a_21 * b_1j + a_22 * b_2j + a_23 * b_3j + a_24 * b_4j
			//			c_3j = a_31 * b_1j + a_32 * b_2j + a_33 * b_3j + a_34 * b_4j
			//			c_4j = a_41 * b_1j + a_42 * b_2j + a_43 * b_3j + a_44 * b_4j
			//						^			  ^				^			  ^
			//						|			  |				|			  |---> res[j] = simd_multiply_add(_column4, simd_replicate(rhs._columnj.m128_f32[3]), res[0]); Fourth line per column
			//						|			  |				|---> res[j] = simd_multiply_add(_column3, simd_replicate(rhs._columnj.m128_f32[2]), res[0]); Third line per column
			//						|			  |---> res[j] = simd_multiply_add(_column2, simd_replicate(rhs._columnj.m128_f32[1]), res[0]); Second line per column
			//						|---> res[j] = simd_multiply(_column1, simd_replicate(rhs._columnj.m128_f32[0]) );	First line per column

			SIMDArray res[4];
			//Column 1 of C
			res[0] = simd_multiply(_column1, simd_replicate(rhs._column1.m128_f32[0]) );
			res[0] = simd_multiply_add(_column2, simd_replicate(rhs._column1.m128_f32[1]), res[0]);
			res[0] = simd_multiply_add(_column3, simd_replicate(rhs._column1.m128_f32[2]), res[0]);
			res[0] = simd_multiply_add(_column4, simd_replicate(rhs._column1.m128_f32[3]), res[0]);
			//Column 2 of C
			res[1] = simd_multiply(_column1, simd_replicate(rhs._column2.m128_f32[0]) );
			res[1] = simd_multiply_add(_column2, simd_replicate(rhs._column2.m128_f32[1]), res[1]);
			res[1] = simd_multiply_add(_column3, simd_replicate(rhs._column2.m128_f32[2]), res[1]);
			res[1] = simd_multiply_add(_column4, simd_replicate(rhs._column2.m128_f32[3]), res[1]);
			//Column 3 of C
			res[2] = simd_multiply(_column1, simd_replicate(rhs._column3.m128_f32[0]) );
			res[2] = simd_multiply_add(_column2, simd_replicate(rhs._column3.m128_f32[1]), res[2]);
			res[2] = simd_multiply_add(_column3, simd_replicate(rhs._column3.m128_f32[2]), res[2]);
			res[2] = simd_multiply_add(_column4, simd_replicate(rhs._column3.m128_f32[3]), res[2]);
			//Column 4 of C
			res[3] = simd_multiply(_column1, simd_replicate(rhs._column4.m128_f32[0]) );
			res[3] = simd_multiply_add(_column2, simd_replicate(rhs._column4.m128_f32[1]), res[3]);
			res[3] = simd_multiply_add(_column3, simd_replicate(rhs._column4.m128_f32[2]), res[3]);
			res[3] = simd_multiply_add(_column4, simd_replicate(rhs._column4.m128_f32[3]), res[3]);
			return Matrix4x4(res[0], res[1], res[2], res[3]);
		}

		INLINE Matrix4x4 operator* (const Component& rhs) const
		{
			SIMDArray aux = simd_replicate(rhs);

			return Matrix4x4( simd_multiply(_column1, aux), simd_multiply(_column2, aux), 
							  simd_multiply(_column3, aux), simd_multiply(_column4, aux) );
		}

		INLINE Matrix4x4 operator/ (const Component& rhs) const
		{
			SIMDArray aux = simd_replicate(rhs);

			#ifdef ESTIMATE_DIVISION
				return Matrix4x4( simd_divide_est(_column1, aux), simd_divide_est(_column2, aux), 
								  simd_divide_est(_column3, aux), simd_divide_est(_column4, aux) );
			#else
				return Matrix4x4( simd_divide(_column1, aux), simd_divide(_column2, aux), 
								  simd_divide(_column3, aux), simd_divide(_column4, aux) );
			#endif
	
		}

		INLINE Matrix4x4 componentMultiplication(const Matrix4x4& rhs) const
		{
			return Matrix4x4(simd_multiply(_column1, rhs._column1), simd_multiply(_column2, rhs._column2),
							 simd_multiply(_column3, rhs._column3), simd_multiply(_column4, rhs._column4));
		}

		INLINE Matrix4x4 componentDivision(const Matrix4x4& rhs) const
		{
			#ifdef ESTIMATE_DIVISION
				return Matrix4x4(simd_divide_est(_column1, rhs._column1), simd_divide_est(_column2, rhs._column2),
								 simd_divide_est(_column3, rhs._column3), simd_divide_est(_column4, rhs._column4));
			#else
				return Matrix4x4(simd_divide(_column1, rhs._column1), simd_divide(_column2, rhs._column2),
								 simd_divide(_column3, rhs._column3), simd_divide(_column4, rhs._column4));
			#endif
		}

		INLINE Vector4<Component, SIMDArray> operator* (const Vector4<Component, SIMDArray>& rhs)
		{
			// c=A*b
			//			The components of vector c are:
			//			c_x = a_11 * b_x + a_12 * b_y + a_13 * b_z + a_14 * b_w
			//			c_y = a_21 * b_x + a_22 * b_y + a_23 * b_z + a_24 * b_w
			//			c_z = a_31 * b_x + a_32 * b_y + a_33 * b_z + a_34 * b_w
			//			c_w = a_41 * b_x + a_42 * b_y + a_43 * b_z + a_44 * b_w
			//						^			  ^				^			  ^
			//						|			  |				|			  |---> res = simd_multiply_add(lhs._column4, simd_replicate(rhs._xyzw.m128_f32[3]), res);
			//						|			  |				|---> res = simd_multiply_add(lhs._column3, simd_replicate(rhs._xyzw.m128_f32[2]), res);
			//						|			  |---> res = simd_multiply_add(lhs._column2, simd_replicate(rhs._xyzw.m128_f32[1]), res);
			//						|---> res = simd_multiply(lhs._column1, simd_replicate(rhs._xyzw.m128_f32[0]) );

			SIMDArray res;
			res = simd_multiply(_column1, simd_replicate(rhs._xyzw.m128_f32[0]) );
			res = simd_multiply_add(_column2, simd_replicate(rhs._xyzw.m128_f32[1]), res);
			res = simd_multiply_add(_column3, simd_replicate(rhs._xyzw.m128_f32[2]), res);
			res = simd_multiply_add(_column4, simd_replicate(rhs._xyzw.m128_f32[3]), res);

			return Vector4<Component, SIMDArray>(res);
		}

		//------------------------------------------------------------
		// Object matrix operations interface implementation
		//------------------------------------------------------------

		INLINE void Transpose()
		{
			simd_transpose(_column1, _column2, _column3, _column4);
		}

		//------------------------------------------------------------
		// Access interface implementation
		//------------------------------------------------------------

		INLINE void GetComponents(Component *pVec) const
		{
			simd_get_elements(pVec, _column1);
			simd_get_elements(pVec+4, _column2);
			simd_get_elements(pVec+8, _column3);
			simd_get_elements(pVec+12, _column4);
		}

		INLINE Component GetC11() const
		{
			return simd_get_element(_column1, 0);
		}

		INLINE Component GetC21() const
		{
			return simd_get_element(_column1, 1);
		}

		INLINE Component GetC31() const
		{
			return simd_get_element(_column1, 2);
		}

		INLINE Component GetC41() const
		{
			return simd_get_element(_column1, 3);
		}

		INLINE Component GetC12() const
		{
			return simd_get_element(_column2, 0);
		}

		INLINE Component GetC22() const
		{
			return simd_get_element(_column2, 1);
		}

		INLINE Component GetC32() const
		{
			return simd_get_element(_column2, 2);
		}

		INLINE Component GetC42() const
		{
			return simd_get_element(_column2, 3);
		}

		INLINE Component GetC13() const
		{
			return simd_get_element(_column3, 0);
		}

		INLINE Component GetC23() const
		{
			return simd_get_element(_column3, 1);
		}

		INLINE Component GetC33() const
		{
			return simd_get_element(_column3, 2);
		}

		INLINE Component GetC43() const
		{
			return simd_get_element(_column3, 3);
		}

		INLINE Component GetC14() const
		{
			return simd_get_element(_column4, 0);
		}

		INLINE Component GetC24() const
		{
			return simd_get_element(_column4, 1);
		}

		INLINE Component GetC34() const
		{
			return simd_get_element(_column4, 2);
		}

		INLINE Component GetC44() const
		{
			return simd_get_element(_column4, 3);
		}

		INLINE void SetC11(Component c11)
		{
			simd_set_element(_column1, c11, 0);
		}

		INLINE void SetC21(Component c21)
		{
			simd_set_element(_column1, c21, 1);
		}

		INLINE void SetC31(Component c31)
		{
			simd_set_element(_column1, c31, 2);
		}

		INLINE void SetC41(Component c41)
		{
			simd_set_element(_column1, c41, 3);
		}

		INLINE void SetC12(Component c12)
		{
			simd_set_element(_column2, c12, 0);
		}

		INLINE void SetC22(Component c22)
		{
			simd_set_element(_column2, c22, 1);
		}

		INLINE void SetC32(Component c32)
		{
			simd_set_element(_column2, c32, 2);
		}

		INLINE void SetC42(Component c42)
		{
			simd_set_element(_column2, c42, 3);
		}

		INLINE void SetC13(Component c13)
		{
			simd_set_element(_column3, c13, 0);
		}

		INLINE void SetC23(Component c23)
		{
			simd_set_element(_column3, c23, 1);
		}

		INLINE void SetC33(Component c33)
		{
			simd_set_element(_column3, c33, 2);
		}

		INLINE void SetC43(Component c43)
		{
			simd_set_element(_column3, c43, 3);
		}

		INLINE void SetC14(Component c14)
		{
			simd_set_element(_column4, c14, 0);
		}

		INLINE void SetC24(Component c24)
		{
			simd_set_element(_column4, c24, 1);
		}

		INLINE void SetC34(Component c34)
		{
			simd_set_element(_column4, c34, 2);
		}

		INLINE void SetC44(Component c44)
		{
			simd_set_element(_column4, c44, 3);
		}

		//------------------------------------------------------------
		// Static operantions interface implementation
		//------------------------------------------------------------

		static INLINE void CreateTranslation(Matrix4x4& outputMatrix, const Vector4<Component, SIMDArray>& translation)
		{
			outputMatrix._column1 = simd_set(static_cast<Component>(1), static_cast<Component>(0),
											 static_cast<Component>(0), static_cast<Component>(0) );
			outputMatrix._column2 = simd_set(static_cast<Component>(0), static_cast<Component>(1),
											 static_cast<Component>(0), static_cast<Component>(0) );
			outputMatrix._column3 = simd_set(static_cast<Component>(0), static_cast<Component>(0),
											 static_cast<Component>(1), static_cast<Component>(0) );

			outputMatrix._column4 = translation._xyzw;
			outputMatrix.SetC44(static_cast<Component>(1));//if translate is really a vector should have w=0
		}

		static INLINE void CreateScale(Matrix4x4& outputMatrix,
			const Component scaleXYZ)
		{
			outputMatrix._column1 = simd_set(scaleXYZ                 , static_cast<Component>(0),
											 static_cast<Component>(0), static_cast<Component>(0) );
			outputMatrix._column2 = simd_set(static_cast<Component>(0), scaleXYZ                 ,
											 static_cast<Component>(0), static_cast<Component>(0) );
			outputMatrix._column3 = simd_set(static_cast<Component>(0), static_cast<Component>(0),
											 scaleXYZ                 , static_cast<Component>(0) );
			outputMatrix._column4 = simd_set(static_cast<Component>(0), static_cast<Component>(0),
											 static_cast<Component>(0), static_cast<Component>(1) );
		}

		static INLINE void CreateScale(Matrix4x4& outputMatrix, 
			const Component scaleX, const Component scaleY, const Component scaleZ)
		{
			outputMatrix._column1 = simd_set(scaleX                   , static_cast<Component>(0),
											 static_cast<Component>(0), static_cast<Component>(0) );
			outputMatrix._column2 = simd_set(static_cast<Component>(0), scaleY                   ,
											 static_cast<Component>(0), static_cast<Component>(0) );
			outputMatrix._column3 = simd_set(static_cast<Component>(0), static_cast<Component>(0),
											 scaleZ                   , static_cast<Component>(0) );
			outputMatrix._column4 = simd_set(static_cast<Component>(0), static_cast<Component>(0),
											 static_cast<Component>(0), static_cast<Component>(1) );
		}

		static INLINE void CreateRotationAroundX(Matrix4x4& outputMatrix, const Component radians)
		{
			Component cosAux = cos(radians);
			Component sinAux = sin(radians);
			outputMatrix._column1 = simd_set(static_cast<Component>(1), static_cast<Component>(0), 
											 static_cast<Component>(0), static_cast<Component>(0) );
			outputMatrix._column2 = simd_set(static_cast<Component>(0), cosAux                   ,
											 sinAux					  , static_cast<Component>(0) );
			outputMatrix._column3 = simd_set(static_cast<Component>(0), -sinAux                  ,
											 cosAux                   , static_cast<Component>(0) );
			outputMatrix._column4 = simd_set(static_cast<Component>(0), static_cast<Component>(0),
											 static_cast<Component>(0), static_cast<Component>(1) );
		}

		static INLINE void CreateRotationAroundY(Matrix4x4& outputMatrix, const Component radians)
		{
			Component cosAux = cos(radians);
			Component sinAux = sin(radians);
			outputMatrix._column1 = simd_set(cosAux					  , static_cast<Component>(0), 
											 -sinAux				  , static_cast<Component>(0) );
			outputMatrix._column2 = simd_set(static_cast<Component>(0), 1		                 ,
											 static_cast<Component>(0), static_cast<Component>(0) );
			outputMatrix._column3 = simd_set(sinAux					  , static_cast<Component>(0),
											 cosAux                   , static_cast<Component>(0) );
			outputMatrix._column4 = simd_set(static_cast<Component>(0), static_cast<Component>(0),
											 static_cast<Component>(0), static_cast<Component>(1) );
		}

		static INLINE void CreateRotationAroundZ(Matrix4x4& outputMatrix, const Component radians)
		{
			Component cosAux = cos(radians);
			Component sinAux = sin(radians);
			outputMatrix._column1 = simd_set(cosAux					  , sinAux					 , 
											 static_cast<Component>(0), static_cast<Component>(0) );
			outputMatrix._column2 = simd_set(-sinAux				  , cosAux					 ,
											 static_cast<Component>(0), static_cast<Component>(0) );
			outputMatrix._column3 = simd_set(static_cast<Component>(0), static_cast<Component>(0),
											 static_cast<Component>(1), static_cast<Component>(0) );
			outputMatrix._column4 = simd_set(static_cast<Component>(0), static_cast<Component>(0),
											 static_cast<Component>(0), static_cast<Component>(1) );
		}

		static INLINE void CreateRotationAroundAxis(Matrix4x4& outputMatrix, const Vector4<Component, SIMDArray>& axis, const Component radians)
		{
            //Auxiliary values
            Component aux1 = (1 - (Component)cos(radians));//(1-cos(angle))
            Component aux2  = axis.GetX()*axis.GetZ();//x*z
            Component aux3  = axis.GetY()*axis.GetZ();//y*z
            Component aux4  = axis.GetX()*axis.GetY();//x*y
            Component aux5  = -(axis.GetZ()*axis.GetZ());//-(z*z)
            Component aux6  = -(axis.GetY()*axis.GetY());//-(y*y)
            Component aux7  = -(axis.GetX()*axis.GetX());//-(x*x)
            Component aux8  = axis.GetX()*(Component)sin(radians);//x*sin(angle)
            Component aux9  = axis.GetY()*(Component)sin(radians);//y*sin(angle)
            Component aux10 = axis.GetZ()*(Component)sin(radians);//z*sin(angle)

            //Rotation matrix calcs
            outputMatrix.SetC11( 1+aux1*aux5*aux6 );
            outputMatrix.SetC12( aux10+aux1*aux4 );
            outputMatrix.SetC13( aux9+aux1*aux2 );
            outputMatrix.SetC21( -aux10+aux1*aux4 );
            outputMatrix.SetC22( 1+aux1*aux5*aux7 );
            outputMatrix.SetC23( aux8+aux1*aux6*aux7 );
            outputMatrix.SetC31( aux9+aux1*aux2 );
            outputMatrix.SetC32( -aux8+aux1*aux3 );
            outputMatrix.SetC33( 1+aux1*aux6*aux7 );

			outputMatrix.SetC41(static_cast<Component>(0));
			outputMatrix.SetC42(static_cast<Component>(0));
			outputMatrix.SetC43(static_cast<Component>(0));

			outputMatrix._column4 = simd_set(static_cast<Component>(0), static_cast<Component>(0), 
											 static_cast<Component>(0), static_cast<Component>(1) );
		}


	//-------------------------------------------------------------------------------------------
	// CHECKED AND WORKING!
	// Creates a column major perspective transformation from a left handed camera space(4D)
	// to a left handed device or clip space(4D). The frustrum will have focal point at the origin,
	// the projection viewport is defined by the rectangle passed as parameter and the near and
	// far clipping planes. The frustrum looks(then projects) to the +z in the camera space, and
	// have +y pointing up(in both spaces) and +x pointing right(in both spaces).
	// The space inside the frustrum will be mapped for the clipping to the box defined by:
	//	x = [-1,1]
	//	y = [-1,1]
	//	z = [ 0,1]
	//	w = 1      ->   ie projective divide have already be done
	//-------------------------------------------------------------------------------------------
	// Parameters:
	// l				Left dimension of the frustrum viewport (l < r)
	// r				Right dimension of the frustrum viewport (r > l)
	// b				Bottom dimension of the frustrum viewport (b < t)
	// t				Top dimension of the frustrum viewport (t > b)
	// nearClipPlane	Near Clip plane distance (n > 0)
	// farClipPlane		Far  Clip plane distance (f > n)
	static INLINE Matrix4x4 CreatePerspectiveProjection(const Component l, const Component r, const Component b, const Component t,
		const Component nearClipPlane, const Component farClipPlane)
	{
		Component invWidth  = 1/static_cast<Component>(r-l);
		Component invHeight = 1/static_cast<Component>(t-b);
		Component invDepth  = 1/static_cast<Component>(farClipPlane-nearClipPlane);
		Component twoNear   = 2*nearClipPlane;

		/*
			Column major matrix:

			T =	| 2*n/(r-l)		0		-(r+l)/(r-l)		0			|
				| 0			 2*n/(t-b)	-(t+b)/(t-b)		0			|
				| 0				0		f/(f-n)				-f*n/(f-n)	|
				| 0				0		1					0			|
		*/
		SIMDArray column1 = simd_set(twoNear*invWidth		  , static_cast<Component>(0), 
									 static_cast<Component>(0), static_cast<Component>(0) );
		SIMDArray column2 = simd_set(static_cast<Component>(0), twoNear*invHeight		 ,
									 static_cast<Component>(0), static_cast<Component>(0) );
		SIMDArray column3 = simd_set(-(r+l)*invWidth		  , -(t+b)*invHeight		 ,
										 farClipPlane*invDepth, static_cast<Component>(1) );
		SIMDArray column4 = simd_set(static_cast<Component>(0), static_cast<Component>(0),
						-(farClipPlane*nearClipPlane)*invDepth, static_cast<Component>(0) );

		return Matrix4x4(column1, column2, column3, column4);
	}

	//Puede tener problemas
	// Creates a left handed perspective transformation. The frustrum will be defines by the
	// horizontal FOV, the aspect ratio of the desired projection plane and the near and far clipping planes.
	// The space inside the frustrum will be mapped for the clipping to the box defined by:
	//											x = [-1,1]
	//											y = [-1,1]
	//											z = [ 0,1]
	//											w = 1      ->   ie projective divide have already be done
	//
	// Parameters:
	// FOV				  Horizontal fild of view in radians
	// aspectRatio		  The ratio of the output as Xpixels/Ypixels
	// nearClipPlane Near Clip plane Distance
	// farClipPlane  Far  Clip plane Distance
	static INLINE Matrix4x4 CreatePerspectiveProjection(const Component FOV,const Component aspectRatio, 
		const Component nearClipPlane, const Component farClipPlane)
	{
		Component zoomX = 1/tan(FOV/static_cast<Component>(2));
		Component zoomY = zoomX/aspectRatio;
		Component invDepth  = 1/static_cast<Component>(farClipPlane-nearClipPlane);

		SIMDArray column1 = simd_set(zoomX						  , static_cast<Component>(0), 
										 static_cast<Component>(0), static_cast<Component>(0) );
		SIMDArray column2 = simd_set(static_cast<Component>(0)	  , zoomY					 ,
										 static_cast<Component>(0), static_cast<Component>(0) );
		SIMDArray column3 = simd_set(static_cast<Component>(0)	  , static_cast<Component>(0),
							(farClipPlane+nearClipPlane)*invDepth , static_cast<Component>(1) );
		SIMDArray column4 = simd_set(static_cast<Component>(0)	  , static_cast<Component>(0),
						-(2*farClipPlane*nearClipPlane)*invDepth  , static_cast<Component>(0) );

		return Matrix4x4(column1, column2, column3, column4);
	}

	//-------------------------------------------------------------------------------------------
	// Creates a column major viewport(we use sub-window as alias) transformation from a left 
	// handed device space(4D) to a screen or window space(4D). We call screen space the usual 
	// window space, with +y pointing to the bottom of the window and width and height as window 
	// resolution, plus the same z and w information from the device space(to be use for z buffer 
	// and pespective correction). The origin in the screen space is the top left corner.
	// The w component should be 1 in device space to transform.
	//-------------------------------------------------------------------------------------------
	// Parameters:
	// WinCenterX		The x component of the center of the sub-window in global window space
	// WinCenterY		The y component of the center of the sub-window in global window space
	// WinWidth			The width of the sub-winsow
	// WinHeight		The height of the sub-winsow
	static INLINE Matrix4x4 CreateViewport(const Component WinCenterX, const Component WinCenterY,
		const Component WinWidth, const Component WinHeight)
	{
		/*
			Column major matrix:

			T =	| WinWidth/2	0			0		WinCenterX	|
				| 0			 -WinHeight/2	0		WinCenterY	|
				| 0				0			1			0		|
				| 0				0			0			1		|
		*/
		SIMDArray column1 = simd_set(WinWidth/static_cast<Component>(2), static_cast<Component>(0), 
											  static_cast<Component>(0), static_cast<Component>(0) );
		SIMDArray column2 = simd_set(		  static_cast<Component>(0), -WinHeight/static_cast<Component>(2),
											  static_cast<Component>(0), static_cast<Component>(0) );
		SIMDArray column3 = simd_set(		  static_cast<Component>(0), static_cast<Component>(0),
											  static_cast<Component>(1), static_cast<Component>(0) );
		SIMDArray column4 = simd_set(						 WinCenterX, WinCenterY				  ,
											  static_cast<Component>(0), static_cast<Component>(1) );

		return Matrix4x4(column1, column2, column3, column4);
	}

	static Matrix4x4 Identity;

	private:
		// Matrix [col1,col2,col3,col4] order by columns, which are represented by a SIMD values
		SIMDArray _column1;
		SIMDArray _column2;
		SIMDArray _column3;
		SIMDArray _column4;
	} ;	

	//Instanciate the template to a concrete given SIMD instruction set type
	#if SSE_LEVEL > 0
		typedef Matrix4x4<float, sse_type> Matrix4x4f;
		Matrix4x4f Matrix4x4f::Identity = Matrix4x4f(1.0f, 0.0f, 0.0f, 0.0f,
													 0.0f, 1.0f, 0.0f, 0.0f,
													 0.0f, 0.0f, 1.0f, 0.0f,
													 0.0f, 0.0f, 0.0f, 1.0f);
	#endif
	//When more simd instructions sets are implemented here we should add the else if 
	//so Matrix4x4 uses that type
	//#else if 

}

#endif

#endif // end MATRIX4X4_SIMD_H