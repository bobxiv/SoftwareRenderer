
#pragma once

#ifndef VECTOR_H
#define VECTOR_H
 
#include <cmath>
#include <algorithm>
#include <ostream>

#include "sse_type.h"

namespace MathModule
{
	using namespace SIMD;
 
	template<typename Real, typename SIMD> class Vector;
 
	//base class for all expression templates
	template<typename Real, typename SIMD, class ExprOperator>
	class VectorExpr 
	{
	public:
		inline operator const ExprOperator&() const
		{
			return *static_cast<const ExprOperator*>(this);
		}
	};

	template<typename Real, typename SIMD , class Expr>
	class VectorEvalTrait
	{
	public :
		typedef const Vector<Real, SIMD> ET;
	};

	template<typename Real, typename SIMD>
	class VectorEvalTrait<Real, SIMD , Vector<Real, SIMD> >
	{
	public :
		typedef const Vector<Real, SIMD>& ET;
	};
 
	//better use macros instead of copy pasting this stuff all over the place
	#define MAKE_VEC_VEC_EXPRESSION(NAME, EXPR, FUNCTION)                                                                          \
	template<typename Real, typename SIMD, class Expr1, class Expr2>                                                               \
	class NAME : public VectorExpr<Real, SIMD, NAME<Real, SIMD, Expr1, Expr2> >                                                    \
	{                                                                                                                              \
	public:                                                                                                                        \
		NAME(const Expr1& pa, const Expr2& pb) : a(pa), b(pb)   { }                                                                \
		inline void eval(Vector<Real, SIMD>& res) const																			   \
		{                                                                                                                          \
			typename VectorEvalTrait<Real, SIMD, Expr1 >::ET lhs( a );													           \
			typename VectorEvalTrait<Real, SIMD, Expr2 >::ET rhs( b );												               \
			res._data = EXPR;																								       \
		}                                                                                                                          \
	private:                                                                                                                       \
		const Expr1& a;                                                                                                            \
		const Expr2& b;                                                                                                            \
	};                                                                                                                             \
																			                                                       \
	template<typename Real, typename SIMD, class Expr1, class Expr2>                                                               \
	inline NAME<Real,SIMD,Expr1,Expr2> FUNCTION(const  VectorExpr<Real,SIMD,Expr1> &a, const VectorExpr<Real,SIMD,Expr2> &b)       \
	{                                                                                                                              \
		return NAME<Real,SIMD,Expr1,Expr2>(a, b);                                                                                  \
	}                                                                       
 
	/*inline Real operator[](unsigned int i) const { return EXPR; }                                                              \*/
 
	//create actual functions and operators
	MAKE_VEC_VEC_EXPRESSION(EMulExpr, simd_multiply( lhs._data , rhs._data ),  operator*)
	
	//sub vector proxy
	/*template<typename Real, typename SIMD, class Expr>                            
	class SubVectorExpr : public VectorExpr<Real, SIMD, SubVectorExpr<Real, SIMD, Expr> > {                
	public:                                                                 
		SubVectorExpr(const Expr& pa, const unsigned int& o)
		: a(pa), offset(o) { }

		inline Real operator[](unsigned int i) const
		{
			return a[i+offset]; 
		}             
	private:                                                                
		const Expr& a;
		const unsigned int offset;                                                                                                                      
	};                                                                      
                                                                         
	template<unsigned D1, class T, unsigned D2, class A>                          
	inline SubVectorExpr<T,D1,A> SubVector(const VectorExpr<T,D2,A> &a, const unsigned &o)     
	{                                                                       
		return SubVectorExpr<T,D1,A>(a, o);                                           
	}*/
 
	//actual vector class
	template<typename Real, typename SIMD>
	class Vector : public VectorExpr<Real, SIMD, Vector<Real,SIMD> > 
	{
	public:
     
		inline Vector()
		{}
     
		inline Vector(const Real &p1, const Real &p2, const Real &p3, const Real &p4)
		{
			_data = simd_set(p1,p2,p3,p4);
		}
     
		template<class ExprOperator>
		inline Vector(const VectorExpr<Real, SIMD, ExprOperator>& a)
		{
			const ExprOperator& ao ( a );  
			//_data = ao.eval();
			ao.eval(*this);
			/*_data[0] = ao[0];
			_data[1] = ao[1];
			_data[2] = ao[2];
			_data[3] = ao[3];*/
		}
     
		inline Real& operator[] (unsigned int i) 
		{ 
			return simd_get_element(_data, i); 
		}

		inline const Real& operator[] (unsigned int i) const 
		{
			return simd_get_element(_data[i], i); 
		}

		inline void eval(SIMD& res) const
		{
			res = _data;
		}
     
		template<class ExprOperator>
		inline Vector& operator=(const VectorExpr<Real, SIMD, ExprOperator>& a)
		{
			const ExprOperator& ao ( a );  
			//_data = ao.eval();
			 ao.eval(*this);
			/*data[0] = ao[0];
			_data[1] = ao[1];
			_data[2] = ao[2];
			_data[3] = ao[3];*/
			return *this;
		}
     
	public:
		SIMD _data;
	};

	typedef Vector<float, sse_type> Vector4f;
}
 
#undef MAKE_VEC_VEC_EXPRESSION
#undef MAKE_VEC_SCAL_EXPRESSION
#undef MAKE_SCAL_VEC_EXPRESSION
#undef MAKE_VEC_EXPRESSION
 
#endif