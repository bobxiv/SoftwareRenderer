#pragma once

#ifndef MATRIX4X4_H
#define MATRIX4X4_H

#define DOT(AC1, AC2, AC3, AC4, BC1, BC2, BC3, BC4)         \
			AC1*BC1+AC2*BC2+AC3*BC3+AC4*BC4

namespace MathModule
{

	///////////////////////////////////////////
	// Matrix4x4
	///////////////////////////////////////////
	template <typename Component>
	class Matrix4x4
	{
	public:
		//------------------------------------------------------------
		// Constructors interface implementation
		//------------------------------------------------------------

		inline Matrix4x4() { }

		inline explicit Matrix4x4(Component *pVec)
		{
			memcpy(_components, pVec, sizeof(Component)*16);
		}

		inline explicit Matrix4x4(Component f)
		{ 
			_components[0] = f;
			_components[1] = f;
			_components[2] = f;
			_components[3] = f;
			_components[4] = f;
			_components[5] = f;
			_components[6] = f;
			_components[7] = f;
			_components[8] = f;
			_components[9] = f;
			_components[10] = f;
			_components[11] = f;
			_components[12] = f;
			_components[13] = f;
			_components[14] = f;
			_components[15] = f;
		}

		inline explicit Matrix4x4(const Component c11, const Component c21, const Component c31, const Component c41,
								  const Component c12, const Component c22, const Component c32, const Component c42,
								  const Component c13, const Component c23, const Component c33, const Component c43,
								  const Component c14, const Component c24, const Component c34, const Component c44)
		{
			_components[0] = c11;
			_components[1] = c21;
			_components[2] = c31;
			_components[3] = c41;
			_components[4] = c12;
			_components[5] = c22;
			_components[6] = c32;
			_components[7] = c42;
			_components[8] = c13;
			_components[9] = c23;
			_components[10] = c33;
			_components[11] = c43;
			_components[12] = c14;
			_components[13] = c24;
			_components[14] = c34;
			_components[15] = c44;
		}

		inline Matrix4x4(const Matrix4x4& copy)
		{ 
			memcpy(_components, copy._components, sizeof(Component)*16);
		}

		inline Matrix4x4& operator= (const Matrix4x4& copy)
		{
			memcpy(_components, copy._components, sizeof(Component)*16);
			return *this;
		}

		//------------------------------------------------------------
		// Comparison interface implementation
		//------------------------------------------------------------

		inline bool operator==(const Matrix4x4& b) const
		{
			return ( (_components[0]  == b._components[0]) && (_components[1]   == b._components[1]) &&
				     (_components[2]  == b._components[2]) && (_components[3]   == b._components[3]) &&
					 (_components[4]  == b._components[4]) && (_components[5]   == b._components[5]) &&
					 (_components[6]  == b._components[6]) && (_components[7]   == b._components[7]) &&
					 (_components[8]  == b._components[8]) && (_components[9]   == b._components[9]) &&
					 (_components[10] == b._components[10]) && (_components[11] == b._components[11]) &&
					 (_components[12] == b._components[12]) && (_components[13] == b._components[13]) &&
					 (_components[14] == b._components[14]) && (_components[15] == b._components[15]) );
		}

		inline bool operator!=(const Matrix4x4& b) const
		{
			return ( (_components[0]  != b._components[0]) && (_components[1]   != b._components[1]) &&
				     (_components[2]  != b._components[2]) && (_components[3]   != b._components[3]) &&
					 (_components[4]  != b._components[4]) && (_components[5]   != b._components[5]) &&
					 (_components[6]  != b._components[6]) && (_components[7]   != b._components[7]) &&
					 (_components[8]  != b._components[8]) && (_components[9]   != b._components[9]) &&
					 (_components[10] != b._components[10]) && (_components[11] != b._components[11]) &&
					 (_components[12] != b._components[12]) && (_components[13] != b._components[13]) &&
					 (_components[14] != b._components[14]) && (_components[15] != b._components[15]) );
		}

		//------------------------------------------------------------
		// Arithmetic interface implementation
		//------------------------------------------------------------

		inline Matrix4x4& operator+= (const Matrix4x4& rhs)
		{
			_components[0] = _components[0] + rhs._components[0];
			_components[1] = _components[1] + rhs._components[1];
			_components[2] = _components[2] + rhs._components[2];
			_components[3] = _components[3] + rhs._components[3];
			_components[4] = _components[4] + rhs._components[4];
			_components[5] = _components[5] + rhs._components[5];
			_components[6] = _components[6] + rhs._components[6];
			_components[7] = _components[7] + rhs._components[7];
			_components[8] = _components[8] + rhs._components[8];
			_components[9] = _components[9] + rhs._components[9];
			_components[10] = _components[10] + rhs._components[10];
			_components[11] = _components[11] + rhs._components[11];
			_components[12] = _components[12] + rhs._components[12];
			_components[13] = _components[13] + rhs._components[13];
			_components[14] = _components[14] + rhs._components[14];
			_components[15] = _components[15] + rhs._components[15];
			return *this;
		}

		inline Matrix4x4& operator-= (const Matrix4x4& rhs)
		{
			_components[0] = _components[0] - rhs._components[0];
			_components[1] = _components[1] - rhs._components[1];
			_components[2] = _components[2] - rhs._components[2];
			_components[3] = _components[3] - rhs._components[3];
			_components[4] = _components[4] - rhs._components[4];
			_components[5] = _components[5] - rhs._components[5];
			_components[6] = _components[6] - rhs._components[6];
			_components[7] = _components[7] - rhs._components[7];
			_components[8] = _components[8] - rhs._components[8];
			_components[9] = _components[9] - rhs._components[9];
			_components[10] = _components[10] - rhs._components[10];
			_components[11] = _components[11] - rhs._components[11];
			_components[12] = _components[12] - rhs._components[12];
			_components[13] = _components[13] - rhs._components[13];
			_components[14] = _components[14] - rhs._components[14];
			_components[15] = _components[15] - rhs._components[15];
			return *this;
		}
		/*
		inline Matrix4x4& operator*= (const Matrix4x4& rhs)
		{
			_column1 = simd_multiply(_column1, rhs._column1);
			_column2 = simd_multiply(_column2, rhs._column2);
			_column3 = simd_multiply(_column3, rhs._column3);
			_column4 = simd_multiply(_column4, rhs._column4);
			return *this;
		}

		inline Matrix4x4& operator/= (const Matrix4x4& rhs)
		{
			#ifdef ESTIMATE_DIVISION
				_column1 = simd_divide_est(_column1, rhs._column1);
				_column2 = simd_divide_est(_column2, rhs._column2);
				_column3 = simd_divide_est(_column3, rhs._column3);
				_column4 = simd_divide_est(_column4, rhs._column4);
			#else
				_column1 = simd_divide(_column1, rhs._column1);
				_column2 = simd_divide(_column2, rhs._column2);
				_column3 = simd_divide(_column3, rhs._column3);
				_column4 = simd_divide(_column4, rhs._column4);
			#endif
			return *this;
		}
		*/
		inline Matrix4x4 operator+ (const Matrix4x4& rhs) const
		{
			return Matrix4x4(_components[0]+rhs._components[0]  , _components[1]+rhs._components[1]  ,
							 _components[2]+rhs._components[2]  , _components[3]+rhs._components[3]  ,
							 _components[4]+rhs._components[4]  , _components[5]+rhs._components[5]  ,
							 _components[6]+rhs._components[6]  , _components[7]+rhs._components[7]  ,
							 _components[8]+rhs._components[8]  , _components[9]+rhs._components[9]  ,
							 _components[10]+rhs._components[10], _components[11]+rhs._components[11],
							 _components[12]+rhs._components[12], _components[13]+rhs._components[13],
							 _components[14]+rhs._components[14], _components[15]+rhs._components[15] );
		}

		inline Matrix4x4 operator- (const Matrix4x4& rhs) const
		{
			return Matrix4x4(_components[0]-rhs._components[0]  , _components[1]-rhs._components[1]  ,
							 _components[2]-rhs._components[2]  , _components[3]-rhs._components[3]  ,
							 _components[4]-rhs._components[4]  , _components[5]-rhs._components[5]  ,
							 _components[6]-rhs._components[6]  , _components[7]-rhs._components[7]  ,
							 _components[8]-rhs._components[8]  , _components[9]-rhs._components[9]  ,
							 _components[10]-rhs._components[10], _components[11]-rhs._components[11],
							 _components[12]-rhs._components[12], _components[13]-rhs._components[13],
							 _components[14]-rhs._components[14], _components[15]-rhs._components[15] );
		}

		inline Matrix4x4 operator* (const Matrix4x4& rhs) const
		{
			return Matrix4x4(DOT(_components[0],_components[4],_components[8],_components[12],rhs._components[0],rhs._components[1],rhs._components[2],rhs._components[3]),
							 DOT(_components[1],_components[5],_components[9],_components[13],rhs._components[0],rhs._components[1],rhs._components[2],rhs._components[3]),
							 DOT(_components[2],_components[6],_components[10],_components[14],rhs._components[0],rhs._components[1],rhs._components[2],rhs._components[3]),
							 DOT(_components[3],_components[7],_components[11],_components[15],rhs._components[0],rhs._components[1],rhs._components[2],rhs._components[3]),
							 DOT(_components[0],_components[4],_components[8],_components[12],rhs._components[4],rhs._components[5],rhs._components[6],rhs._components[7]),
							 DOT(_components[1],_components[5],_components[9],_components[13],rhs._components[4],rhs._components[5],rhs._components[6],rhs._components[7]),
							 DOT(_components[2],_components[6],_components[10],_components[14],rhs._components[4],rhs._components[5],rhs._components[6],rhs._components[7]),
							 DOT(_components[3],_components[7],_components[11],_components[15],rhs._components[4],rhs._components[5],rhs._components[6],rhs._components[7]),
							 DOT(_components[0],_components[4],_components[8],_components[12],rhs._components[8],rhs._components[9],rhs._components[10],rhs._components[11]),
							 DOT(_components[1],_components[5],_components[9],_components[13],rhs._components[8],rhs._components[9],rhs._components[10],rhs._components[11]),
							 DOT(_components[2],_components[6],_components[10],_components[14],rhs._components[8],rhs._components[9],rhs._components[10],rhs._components[11]),
							 DOT(_components[3],_components[7],_components[11],_components[15],rhs._components[8],rhs._components[9],rhs._components[10],rhs._components[11]),
							 DOT(_components[0],_components[4],_components[8],_components[12],rhs._components[12],rhs._components[13],rhs._components[14],rhs._components[15]),
							 DOT(_components[1],_components[5],_components[9],_components[13],rhs._components[12],rhs._components[13],rhs._components[14],rhs._components[15]),
							 DOT(_components[2],_components[6],_components[10],_components[14],rhs._components[12],rhs._components[13],rhs._components[14],rhs._components[15]),
							 DOT(_components[3],_components[7],_components[11],_components[15],rhs._components[12],rhs._components[13],rhs._components[14],rhs._components[15]) );
		}

		inline Matrix4x4 componentMultiplication(const Matrix4x4& rhs) const
		{
			return Matrix4x4(_components[0]*rhs._components[0]  , _components[1]*rhs._components[1]  ,
							 _components[2]*rhs._components[2]  , _components[3]*rhs._components[3]  ,
							 _components[4]*rhs._components[4]  , _components[5]*rhs._components[5]  ,
							 _components[6]*rhs._components[6]  , _components[7]*rhs._components[7]  ,
							 _components[8]*rhs._components[8]  , _components[9]*rhs._components[9]  ,
							 _components[10]*rhs._components[10], _components[11]*rhs._components[11],
							 _components[12]*rhs._components[12], _components[13]*rhs._components[13],
							 _components[14]*rhs._components[14], _components[15]*rhs._components[15] );
		}

		inline Matrix4x4 componentDivision(const Matrix4x4& rhs) const
		{
			return Matrix4x4(_components[0]/rhs._components[0]  , _components[1]/rhs._components[1]  ,
							 _components[2]/rhs._components[2]  , _components[3]/rhs._components[3]  ,
							 _components[4]/rhs._components[4]  , _components[5]/rhs._components[5]  ,
							 _components[6]/rhs._components[6]  , _components[7]/rhs._components[7]  ,
							 _components[8]/rhs._components[8]  , _components[9]/rhs._components[9]  ,
							 _components[10]/rhs._components[10], _components[11]/rhs._components[11],
							 _components[12]/rhs._components[12], _components[13]/rhs._components[13],
							 _components[14]/rhs._components[14], _components[15]/rhs._components[15] );
		}

		//------------------------------------------------------------
		// Access interface implementation
		//------------------------------------------------------------

		inline void GetComponents(Component *pVec) const
		{
			memcpy(pVec, _components, sizeof(Component)*16);
		}

		inline Real GetC11()
		{
			return _components[0];
		}

		inline Real GetC21()
		{
			return _components[1];
		}

		inline Real GetC31()
		{
			return _components[2];
		}

		inline Real GetC41()
		{
			return _components[3];
		}

		inline Real GetC12()
		{
			return _components[4];
		}

		inline Real GetC22()
		{
			return _components[5];
		}

		inline Real GetC32()
		{
			return _components[6];
		}

		inline Real GetC42()
		{
			return _components[7];
		}

		inline Real GetC13()
		{
			return _components[8];
		}

		inline Real GetC23()
		{
			return _components[9];
		}

		inline Real GetC33()
		{
			return _components[10];
		}

		inline Real GetC43()
		{
			return _components[11];
		}

		inline Real GetC14()
		{
			return _components[12];
		}

		inline Real GetC24()
		{
			return _components[13];
		}

		inline Real GetC34()
		{
			return _components[14];
		}

		inline Real GetC44()
		{
			return _components[15];
		}

		/*
		inline void SetX(Real x)
		{
			simd_set_element(_xyzw, x, 0);
		}

		inline void SetY(Real y)
		{
			simd_set_element(_xyzw, y, 1);
		}

		inline void SetZ(Real z)
		{
			simd_set_element(_xyzw, z, 2);
		}

		inline void SetW(Real w)
		{
			simd_set_element(_xyzw, w, 3);
		}*/

		//------------------------------------------------------------
		// Static operantions interface implementation
		//------------------------------------------------------------


	private:
		Component _components[16];
	} ;	


	typedef Matrix4x4<float> Matrix4x4f;

}

#undef DOT

#endif // end MATRIX4X4_H