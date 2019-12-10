#pragma once

#ifndef RENDERER_H
#define RENDERER_H

#include <windows.h>

#include "Config.h"

#include <cmath>
#include <vector>
#include <map>
#include <limits>
#include <algorithm>

#include "Texture2D.h"

using namespace MathModule;

#define _MAX(a,b,c) (a > b)? ((a > c)? a: c) : ((b > c)? b: c)
#define _MIN(a,b,c) (a < b)? ((a < c)? a: c) : ((b < c)? b: c)

#define HALF_SPACE(x, y, x1, y1, x2, y2) (y-y1)*(x2-x1)-(x-x1)*(y2-y1)

namespace SoftRenderer
{

	// The posible winding order of the vertex information
	enum VertexWinding{CCW,CW};

	class Renderer
	{
		public:

		// The posible clear colors for the Clear method
		enum ClearColor{White, Black};

		VertexWinding m_Winding;//For now we only have CCW

		// The ligth direction(vector) in the scene. Must be in world
		// space and point towards the direction of lighting.
		Vector4f	 m_Light;

		// Ambient Color of Light
		Vector4f	 m_ACLight;

		// Difuse Color of Light
		Vector4f	 m_DCLight;

		// Specular Color of Light
		Vector4f	 m_SCLight;

		//Material components(temporal)
		float		 m_ka;
		float		 m_kd;
		float		 m_ks;
		float		 m_q;
		
		// View position in world space
		Vector4f	 m_ViewPos;

		// The ID number 0 is reserved to the state NO TEXTURE
		unsigned int m_CurrentTextureID;


		struct Vertex
		{
				Vector4f Position;
				Vector4f Normal;
				Vector4f Texture;
		};

		Vertex*		  m_pVertexBuffer;
		unsigned int  m_VBCount;
		unsigned int* m_pIndexBuffer;
		unsigned int  m_IBCount;

		Matrix4x4f	 m_WorldT;
		Matrix4x4f	 m_CameraT;
		Matrix4x4f	 m_ProjectionT;
		Matrix4x4f	 m_ViewportT;

	private:

		//The current back-buffer(frame buffer or color buffer) in the buffer chain
		//The origin of the buffer is at the top left of the window
		//The color color space is B8G8R8 (24 bits of pixel depth)
		char*        m_CurrentColorBuffer;

		//The current depth buffer in the buffer chain
		//The origin of the buffer is at the top left of the window
		//The bit depth is 32 bits
		char*		 m_CurrentDepthBuffer;

		//The stride of a pixel row in the back-buffer
		unsigned int m_Stride;

		//The width of the back-buffer and the rest of the assosiated buffers
		unsigned int m_Width;

		//The height of the back-buffer and the rest of the assosiated buffers
		unsigned int m_Height;

		//The rendering window handle
		HWND		 m_hWnd;

		//The rendering window device context
		HDC			 m_hWndDC;

		//The index of the current back-buffer
		int			 m_CurrentBackBufferIndex;

		//Back-buffer device context chain
		HDC			 m_hBackBufferDC[2];
		HBITMAP		 m_hBackBufferDIB[2];
		char*        m_ColorBuffer[2];

		//Reference the default 1x1 monochrome bitmap created in each memory device context
		HGDIOBJ		 m_hDefaultBitmapObject[2];


		std::map<unsigned int, Texture2D*> m_Textures;


		// A shading function to be called for rach fragment. Return the corresponding color.
		// Actualy is a Phong shading.
		// Parameters:
		// Normal	The interpolated normal information
		// 
		//
		// Return: the color of the fragment
		INLINE void _PixelShader(const Vector4f &Normal, const Vector4f &Texture, Vector4f &ColorOut)
		{
			//the normal point outside the surface
			//the ligth point in the direction of the ligth, so to compara with normal we take -ligth
			//m_ViewPos(really is ViewDir) point in the direction that the camera is looking so to comprare with reflect we use -ViewPos
			Vector4f vReflect = Normal * Vector4f::Dot(Normal, -m_Light) * 2.0f - (-m_Light);
			vReflect.Normalize();
			Vector4f cAmbient = m_ACLight * m_ka;
			Vector4f cDiffuse = m_DCLight * max( 0, Vector4f::Dot(-m_Light,Normal)) * m_kd;
			Vector4f cSpecular = m_SCLight * pow(max(0,Vector4f::Dot(vReflect, -m_ViewPos)),m_q) * m_ks;
			Vector4f FinalColor = cAmbient+cDiffuse+cSpecular;
			//ColorOut = Vector4f::Saturate(FinalColor);
			ColorOut = Vector4f::Saturate( m_Textures[m_CurrentTextureID]->Sample2DNearest(Texture.GetX(), Texture.GetY()) );
			//ColorOut = Vector4f::Saturate( m_Textures[m_CurrentTextureID]->Sample2DBilineal(Texture.GetX(), Texture.GetY()) );
		}

		INLINE int round( float value )
		{
			return floor( value + 0.5f );
		}

		// ISNT GIVING CORRECT RESULTS NOW!!!
		// Rasterize a triangle primitive using a space binary partition method. Uses
		// the painter algorithm to check for depth.
		// The triangles should be passed counter-clock wise or nothing will be rasterized.
		// For each fragment shade using a call to _PixelShader with corresponding interpolated data.
		// Parameters:
		// V1	The first vertex of the primitive
		// V2	The second vertex of the primitive
		// V3	The third vertex of the primitive
		INLINE void _RasterTriangleCorrect(const Vertex& V1, const Vertex& V2, const Vertex& V3)
		{
			// Gets the position coordinates
			// int 32 precision
			const int y1 = round( 16.0f * V1.Position.GetY() );
			const int y2 = round( 16.0f * V2.Position.GetY() );
			const int y3 = round( 16.0f * V3.Position.GetY() );
			
			const int x1 = round( 16.0f * V1.Position.GetX() );
			const int x2 = round( 16.0f * V2.Position.GetX() );
			const int x3 = round( 16.0f * V3.Position.GetX() );
			
			// Deltas
			const int DY12 = y1 - y2;
			const int DY23 = y2 - y3;
			const int DY31 = y3 - y1;
			
			const int DX12 = x1 - x2;
			const int DX23 = x2 - x3;
			const int DX31 = x3 - x1;

			// Fixed-point deltas
			const int FDX12 = DX12 << 4;
			const int FDX23 = DX23 << 4;
			const int FDX31 = DX31 << 4;
			
			const int FDY12 = DY12 << 4;
			const int FDY23 = DY23 << 4;
			const int FDY31 = DY31 << 4;
			
			// Triangle bounding rectangle (convert from int 32 precision to actual screen coordinates)
			const int minx = ( _MIN(x1, x2, x3) + 0xF) >> 4;
			const int maxx = ( _MAX(x1, x2, x3) + 0xF) >> 4;
			const int miny = ( _MIN(y1, y2, y3) + 0xF) >> 4;
			const int maxy = ( _MAX(y1, y2, y3) + 0xF) >> 4;

			// Buffers pointers initializations to triangle section
			char* colorBufferPixel = m_CurrentColorBuffer;
			char* depthBufferPixel = m_CurrentDepthBuffer;
			colorBufferPixel += miny * m_Stride;
			depthBufferPixel += miny * m_Stride;

			// Half-edge constants
			int C1 = DY12 * x1 - DX12 * y1;
			int C2 = DY23 * x2 - DX23 * y2;
			int C3 = DY31 * x3 - DX31 * y3;
			
			// Correct for fill convention
			if(DY12 < 0 || (DY12 == 0 && DX12 > 0)) C1++;
			if(DY23 < 0 || (DY23 == 0 && DX23 > 0)) C2++;
			if(DY31 < 0 || (DY31 == 0 && DX31 > 0)) C3++;
			
			int CY1 = C1 + DX12 * (miny << 4) - DY12 * (minx << 4);
			int CY2 = C2 + DX23 * (miny << 4) - DY23 * (minx << 4);
			int CY3 = C3 + DX31 * (miny << 4) - DY31 * (minx << 4);
			
			
			float b1, b2, b3;//baricentric coordinates
			Vector4f normal, texture;//interpolated data
			Vector4f color;//fragment color result from _PixelShader

			// Scan through bounding rectangle
			for(int y = miny; y < maxy; ++y)
			{
				int CX1 = CY1;
				int CX2 = CY2;
				int CX3 = CY3;
				
				for(int x = minx; x < maxx; ++x)
				{
					
					if(CX1 > 0 && CX2 > 0 && CX3 > 0)//half space test
					{	
						
						Vector4f::CalcBaricentricCoords2D(V1.Position, V2.Position, V3.Position, 
									Vector4f(x,y,0.0f,1.0f), b1, b2, b3);

						Vector4f position = Vector4f::EvalBaricentricCoords(V1.Position, V2.Position, V3.Position,
												b1, b2, b3);

						if( position.GetZ() < *( (float*) (depthBufferPixel+x*4) ) )//Painter algorithm test
						{
							normal = Vector4f::EvalBaricentricCoords(V1.Normal, V2.Normal, V3.Normal, 
													b1, b2, b3);

							texture = Vector4f::EvalBaricentricCoords(V1.Texture, V2.Texture, V3.Texture, 
													b1, b2, b3);

							_PixelShader(normal, texture, color);// gets the color by reference inside color

							// Sets fragment color
							colorBufferPixel[x*3]   = color.GetZ()*255;//B		Because of bitmap little endianness
							colorBufferPixel[x*3+1] = color.GetY()*255;//G
							colorBufferPixel[x*3+2] = color.GetX()*255;//R
							//*( (int*) (colorBufferPixel+x*3) ) = 0xFFFFFF; // White color

							// Sets fragment depth
							*( (float*) (depthBufferPixel+x*4) ) = position.GetZ();
						}
					}

					CX1 -= FDY12;
					CX2 -= FDY23;
					CX3 -= FDY31;
				}
				
				CY1 += FDX12;
				CY2 += FDX23;
				CY3 += FDX31;
				
				// Increment the buffers pointers to next line
				colorBufferPixel += m_Stride;
				depthBufferPixel += m_Stride;
			}
		}

		// JUST FOR TEST!!!
		// Rasterize a triangle primitive using a space binary partition method. Uses
		// the painter algorithm to check for depth.
		// The triangles should be passed counter-clock wise or nothing will be rasterized.
		// For each fragment shade using a call to _PixelShader with corresponding interpolated data.
		// Parameters:
		// V1	The first vertex of the primitive
		// V2	The second vertex of the primitive
		// V3	The third vertex of the primitive
		INLINE void _RasterTriangleDouble(const Vertex& V1, const Vertex& V2, const Vertex& V3)
		{
			// Gets the position coordinates
			const double y1 = V1.Position.GetY();
			const double y2 = V2.Position.GetY();
			const double y3 = V3.Position.GetY();

			const double x1 = V1.Position.GetX();
			const double x2 = V2.Position.GetX();
			const double x3 = V3.Position.GetX();

			// Triangle bounding rectangle (convert from int 32 precision to actual screen coordinates)
			const int minx = floor(_MIN(x1, x2, x3));
			const int maxx = ceil(_MAX(x1, x2, x3));
			const int miny = floor(_MIN(y1, y2, y3));
			const int maxy = ceil(_MAX(y1, y2, y3));

			// Buffers pointers initializations to triangle section
			char* colorBufferPixel = m_CurrentColorBuffer;
			char* depthBufferPixel = m_CurrentDepthBuffer;
			colorBufferPixel += miny * m_Stride;
			depthBufferPixel += miny * m_Stride;

			float b1, b2, b3;//baricentric coordinates
			Vector4f normal, texture;//interpolated data
			Vector4f color;//fragment color result from _PixelShader

			// Scan through bounding rectangle
			for(int y = miny; y < maxy; ++y)
			{
				for(int x = minx; x < maxx; ++x)
				{
					// When all half-space functions positive, pixel is inside the triangle
					
					/*if( HALF_SPACE(x, y, x1, y1, x2, y2) > 0 &&
					    HALF_SPACE(x, y, x2, y2, x3, y3) > 0 &&
					    HALF_SPACE(x, y, x3, y3, x1, y1) > 0)*/
					/*if( (HALF_SPACE(x, y, x1, y1, x3, y3) > 0.005 || HALF_SPACE(x, y, x1, y1, x3, y3) < 0.005) &&
						(HALF_SPACE(x, y, x3, y3, x2, y2) > 0.005 || HALF_SPACE(x, y, x3, y3, x2, y2) < 0.005) &&
						(HALF_SPACE(x, y, x2, y2, x1, y1) > 0.005 || HALF_SPACE(x, y, x2, y2, x1, y1) < 0.005) )*/
					/*if( SIGN(HALF_SPACE(x, y, x1, y1, x3, y3)) == SIGN(HALF_SPACE(x, y, x3, y3, x2, y2)) &&
						SIGN(HALF_SPACE(x, y, x3, y3, x2, y2)) == SIGN(HALF_SPACE(x, y, x2, y2, x1, y1)) )*/
					/*if( HALF_SPACE(x, y, x1, y1, x3, y3) > 0 &&
						HALF_SPACE(x, y, x3, y3, x2, y2) > 0 &&
						HALF_SPACE(x, y, x2, y2, x1, y1) > 0)*/
					//float a = HALF_SPACE(x, float(-y), x1, float(-y1), x2, float(-y2));
					//float b = HALF_SPACE(x, float(-y), x2, float(-y2), x3, float(-y3));
					//float c = HALF_SPACE(x, float(-y), x3, float(-y3), x1, float(-y1));
					if( HALF_SPACE(x, double(-y), x1, double(-y1), x2, double(-y2)) >= -10.1 &&
						HALF_SPACE(x, double(-y), x2, double(-y2), x3, double(-y3)) >= -10.1 &&
						HALF_SPACE(x, double(-y), x3, double(-y3), x1, double(-y1)) >= -10.1)
					{	
						
						Vector4f::CalcBaricentricCoords2D(V1.Position, V2.Position, V3.Position, 
									Vector4f(x,y,0.0f,1.0f), b1, b2, b3);

						Vector4f position = Vector4f::EvalBaricentricCoords(V1.Position, V2.Position, V3.Position,
												b1, b2, b3);

						if( position.GetZ() < *( (float*) (depthBufferPixel+x*4) ) )//Painter algorithm test
						{
							normal = Vector4f::EvalBaricentricCoords(V1.Normal, V2.Normal, V3.Normal, 
													b1, b2, b3);

							texture = Vector4f::EvalBaricentricCoords(V1.Texture, V2.Texture, V3.Texture, 
													b1, b2, b3);

							_PixelShader(normal, texture, color);// gets the color by reference inside color

							// Sets fragment color
							colorBufferPixel[x*3]   = color.GetZ()*255;//B		Because of bitmap little endianness
							colorBufferPixel[x*3+1] = color.GetY()*255;//G
							colorBufferPixel[x*3+2] = color.GetX()*255;//R
							//*( (int*) (colorBufferPixel+x*3) ) = 0xFFFFFF; // White color

							// Sets fragment depth
							*( (float*) (depthBufferPixel+x*4) ) = position.GetZ();
						}
					}
				}

				// Increment the buffers pointers to next line
				colorBufferPixel += m_Stride;
				depthBufferPixel += m_Stride;
			}
		}

		// Rasterize a triangle primitive using a space binary partition method. Uses
		// the painter algorithm to check for depth.
		// The triangles should be passed counter-clock wise or nothing will be rasterized.
		// For each fragment shade using a call to _PixelShader with corresponding interpolated data.
		// Parameters:
		// V1	The first vertex of the primitive
		// V2	The second vertex of the primitive
		// V3	The third vertex of the primitive
		INLINE void _RasterTriangle(const Vertex& V1, const Vertex& V2, const Vertex& V3)
		{
			// Gets the position coordinates
			const float y1 = V1.Position.GetY();
			const float y2 = V2.Position.GetY();
			const float y3 = V3.Position.GetY();

			const float x1 = V1.Position.GetX();
			const float x2 = V2.Position.GetX();
			const float x3 = V3.Position.GetX();

			// Triangle bounding rectangle
			// taking the floor and ceil is just a trick of mine
			const int minx = floor(_MIN(x1, x2, x3));
			const int maxx = ceil(_MAX(x1, x2, x3));
			const int miny = floor(_MIN(y1, y2, y3));
			const int maxy = ceil(_MAX(y1, y2, y3));

			// Buffers pointers initializations to triangle section
			char* colorBufferPixel = m_CurrentColorBuffer;
			char* depthBufferPixel = m_CurrentDepthBuffer;
			colorBufferPixel += miny * m_Stride;
			depthBufferPixel += miny * m_Stride;

			float b1, b2, b3;//baricentric coordinates
			Vector4f normal, texture;//interpolated data
			Vector4f color;//fragment color result from _PixelShader

			// Scan through bounding rectangle
			for(int y = miny; y < maxy; ++y)
			{
				for(int x = minx; x < maxx; ++x)
				{
					// When all half-space functions positive, pixel is inside the triangle
					
					/*if( HALF_SPACE(x, y, x1, y1, x2, y2) > 0 &&
					    HALF_SPACE(x, y, x2, y2, x3, y3) > 0 &&
					    HALF_SPACE(x, y, x3, y3, x1, y1) > 0)*/
					/*if( (HALF_SPACE(x, y, x1, y1, x3, y3) > 0.005 || HALF_SPACE(x, y, x1, y1, x3, y3) < 0.005) &&
						(HALF_SPACE(x, y, x3, y3, x2, y2) > 0.005 || HALF_SPACE(x, y, x3, y3, x2, y2) < 0.005) &&
						(HALF_SPACE(x, y, x2, y2, x1, y1) > 0.005 || HALF_SPACE(x, y, x2, y2, x1, y1) < 0.005) )*/
					/*if( SIGN(HALF_SPACE(x, y, x1, y1, x3, y3)) == SIGN(HALF_SPACE(x, y, x3, y3, x2, y2)) &&
						SIGN(HALF_SPACE(x, y, x3, y3, x2, y2)) == SIGN(HALF_SPACE(x, y, x2, y2, x1, y1)) )*/
					/*if( HALF_SPACE(x, y, x1, y1, x3, y3) > 0 &&
						HALF_SPACE(x, y, x3, y3, x2, y2) > 0 &&
						HALF_SPACE(x, y, x2, y2, x1, y1) > 0)*/
					//float a = HALF_SPACE(x, float(-y), x1, float(-y1), x2, float(-y2));
					//float b = HALF_SPACE(x, float(-y), x2, float(-y2), x3, float(-y3));
					//float c = HALF_SPACE(x, float(-y), x3, float(-y3), x1, float(-y1));
					if( HALF_SPACE(x, float(-y), x1, float(-y1), x2, float(-y2)) > 0.0f &&
						HALF_SPACE(x, float(-y), x2, float(-y2), x3, float(-y3)) > 0.0f &&
						HALF_SPACE(x, float(-y), x3, float(-y3), x1, float(-y1)) > 0.0f)
					{	
						
						Vector4f::CalcBaricentricCoords2D(V1.Position, V2.Position, V3.Position, 
									Vector4f(x,y,0.0f,1.0f), b1, b2, b3);

						Vector4f position = Vector4f::EvalBaricentricCoords(V1.Position, V2.Position, V3.Position,
												b1, b2, b3);

						if( position.GetZ() < *( (float*) (depthBufferPixel+x*4) ) )//Painter algorithm test
						{

							
							//Prueba pespective correct
							/*
							Vector4f P = Vector4f(x*position.GetZ(), y*position.GetZ(), position.GetZ(), 1.0f);
							Vector4f _V1 = Vector4f(V1.Position.GetX()*V1.Position.GetZ(), V1.Position.GetY()*V1.Position.GetZ(), V1.Position.GetZ(), 1.0f);
							Vector4f _V2 = Vector4f(V2.Position.GetX()*V2.Position.GetZ(), V2.Position.GetY()*V2.Position.GetZ(), V2.Position.GetZ(), 1.0f);
							Vector4f _V3 = Vector4f(V3.Position.GetX()*V3.Position.GetZ(), V3.Position.GetY()*V3.Position.GetZ(), V3.Position.GetZ(), 1.0f);

							Vector4f::CalcBaricentricCoords2D(_V1, _V2, _V3, 
									P, b1, b2, b3);
								*/
							//-------------------------
							
							Vector4f P1_z = Vector4f(1.0f/V1.Position.GetZ());
							Vector4f P2_z = Vector4f(1.0f/V2.Position.GetZ());
							Vector4f P3_z = Vector4f(1.0f/V3.Position.GetZ());
							Vector4f invZ = Vector4f::EvalBaricentricCoords(P1_z, P2_z, P3_z, 
													b1, b2, b3);

							/*
							double zP1 = 1.0f/V1.Position.GetZ();
							double zP2 = 1.0f/V2.Position.GetZ();
							double zP3 = 1.0f/V3.Position.GetZ();
							double zres = zP1*b1+zP2*b2+zP3*b3;
							*/

							Vector4f t1 = V1.Texture*P1_z;//V1.Position.GetZ();
							Vector4f t2 = V2.Texture*P2_z;///V2.Position.GetZ();
							Vector4f t3 = V3.Texture*P3_z;///V3.Position.GetZ();
							Vector4f texture_z = Vector4f::EvalBaricentricCoords(t1, t2, t3, 
													b1, b2, b3);

							/*
							double _t1[2] = {V1.Texture.GetX()/V1.Position.GetZ(), V1.Texture.GetY()/V1.Position.GetZ()};
							double _t2[2] = {V2.Texture.GetX()/V2.Position.GetZ(), V2.Texture.GetY()/V2.Position.GetZ()};
							double _t3[2] = {V3.Texture.GetX()/V3.Position.GetZ(), V3.Texture.GetY()/V3.Position.GetZ()};
							double res[2] = {_t1[0]*b1+_t2[0]*b2+_t3[0]*b3, _t1[1]*b1+_t2[1]*b2+_t3[1]*b3};
							*/

							double z = static_cast<double>(1) / static_cast<double>(invZ.GetX());
							//texture = texture_z / invZ;
							texture.SetX( static_cast<double>(texture_z.GetX()) * z );//u = (u/z) * z 
							texture.SetY( static_cast<double>(texture_z.GetY()) * z );//v = (v/z) * z ( 

							/*
							texture.SetX( res[0]/zres );
							texture.SetY( res[1]/zres );*/
							
							//-------------------------




							normal = Vector4f::EvalBaricentricCoords(V1.Normal, V2.Normal, V3.Normal, 
													b1, b2, b3);

							// the next line calc the texture coordinates in screen space(also called affine space)
							// is commented in favor to the perspective correct texture
							//texture = Vector4f::EvalBaricentricCoords(V1.Texture, V2.Texture, V3.Texture, 
								//					b1, b2, b3);

							_PixelShader(normal, texture, color);// gets the color by reference inside color

							// Sets fragment color
							colorBufferPixel[x*3]   = color.GetZ()*255;//B		Because of bitmap little endianness
							colorBufferPixel[x*3+1] = color.GetY()*255;//G
							colorBufferPixel[x*3+2] = color.GetX()*255;//R
							//*( (int*) (colorBufferPixel+x*3) ) = 0xFFFFFF; // White color

							// Sets fragment depth
							*( (float*) (depthBufferPixel+x*4) ) = position.GetZ();
						}
					}
				}

				// Increment the buffers pointers to next line
				colorBufferPixel += m_Stride;
				depthBufferPixel += m_Stride;
			}
		}

		// Check if the primitive should be clipped out, clipped of nothing.
		// The position of each vertex should be in device space and after
		// perspective division.
		// Parameters:
		// V1	The first vertex of the primitive
		// V2	The second vertex of the primitive
		// V3	The third vertex of the primitive
		//
		// Return: true if clipped out, false otherwise
		bool _isClippedOut(const Vertex& V1, const Vertex& V2, const Vertex& V3)
		{
			//Gets the position coordinates
			float z1 = V1.Position.GetZ();
			float z2 = V2.Position.GetZ();
			float z3 = V3.Position.GetZ();

			float y1 = V1.Position.GetY();
			float y2 = V2.Position.GetY();
			float y3 = V3.Position.GetY();

			float x1 = V1.Position.GetX();
			float x2 = V2.Position.GetX();
			float x3 = V3.Position.GetX();

			// Bounding rectangle
			float minx = (float)_MIN(x1, x2, x3);
			float maxx = (float)_MAX(x1, x2, x3);
			float miny = (float)_MIN(y1, y2, y3);
			float maxy = (float)_MAX(y1, y2, y3);
			float minz = (float)_MIN(z1, z2, z3);
			float maxz = (float)_MAX(z1, z2, z3);

			if( maxx > 1.0f )
				return true;//clipped
			if( maxy > 1.0f )
				return true;//clipped
			if( maxz > 1.0f )
				return true;//clipped
			if( minx < -1.0f )
				return true;//clipped
			if( miny < -1.0f )
				return true;//clipped
			if( minz < 0.0f )
				return true;//clipped

			return false;
		}

	public:

		Renderer(HWND hWnd, int width, int height): m_Width(width), m_Height(height),
			m_hWnd(hWnd), m_hWndDC(NULL), m_CurrentBackBufferIndex(0), m_CurrentColorBuffer(NULL), m_Light(-1,-1,0,0),
			m_pVertexBuffer(NULL), m_VBCount(0), m_pIndexBuffer(NULL), m_IBCount(0), m_Winding(CCW), m_CurrentDepthBuffer(NULL),
			m_CurrentTextureID(0)
		{
			m_hBackBufferDC[0] = NULL;
			m_hBackBufferDC[1] = NULL;

			m_hBackBufferDIB[0] = NULL;
			m_hBackBufferDIB[1] = NULL;

			m_ColorBuffer[0] = NULL;
			m_ColorBuffer[1] = NULL;

			m_hDefaultBitmapObject[0] = NULL;
			m_hDefaultBitmapObject[1] = NULL;

			m_CurrentDepthBuffer = new char[m_Width*m_Height*sizeof(float)];
			//Clear the deth buffer to FAR
			for(int i=0; i < (m_Width*m_Height) ; ++i) 
				((float*)m_CurrentDepthBuffer)[i] = 1.0f;

			m_Stride = width*3;//row size in bytes
			//Increase the stride until a line is aligned with 4 bytes(a 32bits word), so the bitmap format requires
			while( m_Stride % 4 != 0 )
				++m_Stride;

			ASSERT(m_hWnd != NULL, std::string(__FILE__).append(": ").append(", Function: Renderer::Renderer(HWND hWnd, unsigned int width, unsigned int height) -> Parameter hWnd is NULL").c_str() );
			ASSERT(width > 0, std::string(__FILE__).append(": ").append(", Function: Renderer::Renderer(HWND hWnd, unsigned int width, unsigned int height) -> Parameter width is 0 or negative").c_str() );
			ASSERT(height > 0, std::string(__FILE__).append(": ").append(", Function: Renderer::Renderer(HWND hWnd, unsigned int width, unsigned int height) -> Parameter height is 0 or negative").c_str() );

			//Gets the window device context
			m_hWndDC = ::GetDC(hWnd);
			if( m_hWndDC == NULL )
				exit(1);
			//Creates i memory device contexts for the back-buffer chain
			m_hBackBufferDC[0] = ::CreateCompatibleDC(m_hWndDC);
			if( m_hBackBufferDC[0] == NULL )
				exit(1);

			m_hBackBufferDC[1] = ::CreateCompatibleDC(m_hWndDC);
			if( m_hBackBufferDC[1] == NULL )
				exit(1);

			//Creates i DIB objects for the back-buffer chain to be selected by the memory device contexts
			BITMAPINFO bitmapInfo;
			ZeroMemory(&bitmapInfo, sizeof(BITMAPINFO));
			bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
			bitmapInfo.bmiHeader.biWidth = width;
			bitmapInfo.bmiHeader.biHeight = -height;//we pass the height as negative so the image origin is moved from the top left
			bitmapInfo.bmiHeader.biPlanes = 1;		//to the bottom left, this plus the memory organization of a bitmap image with
			bitmapInfo.bmiHeader.biBitCount = 24;	//the "higer" rows first, we simulate the origin at the top left in memory of the back-buffer
			bitmapInfo.bmiHeader.biCompression = BI_RGB;
			bitmapInfo.bmiHeader.biSizeImage = width*height*3;

			//Sets the back-buffers pointers, m_ColorBuffer[i], to the DIB pixels
			m_hBackBufferDIB[0] = ::CreateDIBSection(m_hBackBufferDC[0], &bitmapInfo, DIB_RGB_COLORS, (void**)&m_ColorBuffer[0], NULL, NULL);
			if( m_hBackBufferDIB[0] == NULL || m_ColorBuffer[0] == NULL )
				exit(1);

			m_hBackBufferDIB[1] = ::CreateDIBSection(m_hBackBufferDC[1], &bitmapInfo, DIB_RGB_COLORS, (void**)&m_ColorBuffer[1], NULL, NULL);
			if( m_hBackBufferDIB[1] == NULL || m_ColorBuffer[1] == NULL )
				exit(1);

			//Sets the current back-buffer pixels array
			m_CurrentColorBuffer = m_ColorBuffer[m_CurrentBackBufferIndex];

			//Set the back-buffers bitmaps into the memory device contexts
			m_hDefaultBitmapObject[0] = ::SelectObject(m_hBackBufferDC[0], m_hBackBufferDIB[0]);
			m_hDefaultBitmapObject[1] = ::SelectObject(m_hBackBufferDC[1], m_hBackBufferDIB[1]);
		}

		~Renderer()
		{
			int res = 0;
			m_hBackBufferDIB[1] = (HBITMAP)::SelectObject(m_hBackBufferDC[1], m_hDefaultBitmapObject[1]);
			m_hBackBufferDIB[0] = (HBITMAP)::SelectObject(m_hBackBufferDC[0], m_hDefaultBitmapObject[0]);
			//Deletes the backbuffer DIB object
			res = ::DeleteObject(m_hBackBufferDIB[1]);
			if(res == 0)
				exit(1);
			res = ::DeleteObject(m_hBackBufferDIB[0]);
			if(res == 0)
				exit(1);
			//Delete the backbuffer memory device context
			res = ::DeleteDC(m_hBackBufferDC[1]);
			if(res == 0)
				exit(1);
			res = ::DeleteDC(m_hBackBufferDC[0]);
			if(res == 0)
				exit(1);
			//Release the window device context
			res = ::ReleaseDC(m_hWnd, m_hWndDC);
			if(res == 0)
				exit(1);

			delete[] m_CurrentDepthBuffer;
		}

		// Present the current back buffer surface to the rendering window.
		void Present()
		{
			//Bit Block Transfer the current backbuffer memory device context to the window device context
			BOOL res = BitBlt(m_hWndDC, 0, 0, m_Width, m_Height, m_hBackBufferDC[m_CurrentBackBufferIndex], 0, 0, SRCCOPY);

			ASSERT( res, std::string(__FILE__).append(": ").append(", Function: Renderer::Present() -> Cannot BitBlt").c_str() );

			//Chain the current back-buffer pixel array
			++m_CurrentBackBufferIndex;
			m_CurrentBackBufferIndex %= 2;
			m_CurrentColorBuffer = m_ColorBuffer[m_CurrentBackBufferIndex];
		}

		// Clears the current back buffer(the color and deth buffer)
		// Parameters:
		// color	The clear color. Must be a value of the ClearColor enumerator.
		void Clear(ClearColor color)
		{
			//Bit Block Transfer black or white color to the backbuffer memory device context

			BOOL res = false;
			switch(color)
			{
				case Black:
					res = BitBlt(m_hBackBufferDC[m_CurrentBackBufferIndex], 0, 0, m_Width, m_Height, NULL, 0, 0, BLACKNESS);
					break;
				case White:
					res = BitBlt(m_hBackBufferDC[m_CurrentBackBufferIndex], 0, 0, m_Width, m_Height, NULL, 0, 0, WHITENESS);
					break;
			}
			
			//Clear the deth buffer to FAR
			for(int i=0; i < (m_Width*m_Height) ; ++i) 
				((float*)m_CurrentDepthBuffer)[i] = 1.0f;

			ASSERT( res, std::string(__FILE__).append(": ").append(", Function: Renderer::Present() -> Cannot BitBlt").c_str() );
		}

		// Renders the vertex buffer and index buffer mesh.
		void Render()
		{
			Matrix4x4f T = m_ProjectionT*m_CameraT*m_WorldT;

			Vertex V1, V2, V3;

			for(int i=0; i < m_IBCount ; i+=3)
			{
				V1 = m_pVertexBuffer[m_pIndexBuffer[i]];
				V2 = m_pVertexBuffer[m_pIndexBuffer[i+1]];
				V3 = m_pVertexBuffer[m_pIndexBuffer[i+2]];

				// Transform to Clip or Device space
				V1.Position = T*V1.Position;
				V2.Position = T*V2.Position;
				V3.Position = T*V3.Position;

				// perform the perspective division to project
				V1.Position.PerspectiveDivision();
				V2.Position.PerspectiveDivision();
				V3.Position.PerspectiveDivision();

				// Check if the primitive pass the clipping
				if( _isClippedOut(V1, V2, V3) )
					continue;

				// Transform to Viewport or Screen space
				V1.Position = m_ViewportT*V1.Position;
				V2.Position = m_ViewportT*V2.Position;
				V3.Position = m_ViewportT*V3.Position;

				// Transform to the normals to World space
				//if there are any scale of shear should be transpose(inv(m_WorldT))
				V1.Normal = m_WorldT*V1.Normal;
				V2.Normal = m_WorldT*V2.Normal;
				V3.Normal = m_WorldT*V3.Normal;

				// Rasterize the triangle
				_RasterTriangle(V1, V2, V3);
				//_RasterTriangleCorrect(V1, V2, V3);//not correct but nick says so
				//_RasterTriangleDouble(V1, V2, V3);//correct but not nice
			}
		}

		// Creates a texture with the desired file
		// Parameters:
		// filename		Name and path of the file
		//
		// Return: Reference to the Texture2D created
		Texture2D& CreateTexture2D(const std::string &filename)
		{
			bool success = false;
			unsigned int id;
			//OK the id system is extremely ineficient... but will be unique and we can have lots of textures :D
			// the id 0 is reserved to the state NO TEXTURE SET
			for(id = 1; id <= MAXUINT32 ; ++id)
			{
				if( m_Textures.find( id ) != m_Textures.end() );
				{
					m_Textures[id] = new Texture2D(filename, id);
					success = true;
					break;
				}
			}
			if( !success )
				throw std::exception();
			return *m_Textures[id];
		}

		void DisposeTexture2D(Texture2D& texture)
		{
			m_Textures.erase( texture.GetIDTexture() );
		}

	};

	// Creates a icosahedron mesh in the range: 
	//	x [??, ??]
	//	y [??, ??]
	//	z [??, ??]
	//						ie each edge is ? long
	// The normal information is calculated.
	// Parameters:
	// pVertexBuffer	 The output vertex buffer[OUT]
	// VertexBufferCount The vertex buffer elemento count[OUT]
	// pIndexBuffer		 The output index buffer[OUT]
	// IndexBufferCount	 The index buffer elemento count[OUT]
	// winding			 The desired vertex winding[IN]
	void IcosahedronMesh(Renderer::Vertex* &pVertexBuffer, unsigned int &VertexBufferCount, unsigned int* &pIndexBuffer, unsigned int &IndexBufferCount,
		const VertexWinding winding)
	{
		enum { eNumVertices = 12, eNumTriangles = 20 };//The num of vertexs and triangles for the platonic solid
		VertexBufferCount = eNumVertices;
		IndexBufferCount = eNumTriangles*3;

		const float a = (float) sqrt(2.0f/(5.0f + sqrt(5.0f)));
		const float b = (float) sqrt(2.0f/(5.0f - sqrt(5.0f)));

		//pVertexBuffer = new Renderer::Vertex[eNumVertices];//new can return un align memory and every thing breaks!
		pVertexBuffer = (Renderer::Vertex*)_aligned_malloc(sizeof(Renderer::Vertex)*eNumVertices, __alignof(Renderer::Vertex));
		pVertexBuffer[0].Position = Vector4f(-a, 0.0f, b, 1.0f);
		pVertexBuffer[1].Position = Vector4f(a, 0.0f, b, 1.0f);
		pVertexBuffer[2].Position = Vector4f(-a, 0.0f, -b, 1.0f);
		pVertexBuffer[3].Position = Vector4f(a, 0.0f, -b, 1.0f);
		pVertexBuffer[4].Position = Vector4f(0.0f, b, a, 1.0f);
		pVertexBuffer[5].Position = Vector4f(0.0f, b, -a, 1.0f);
		pVertexBuffer[6].Position = Vector4f(0.0f, -b, a, 1.0f);
		pVertexBuffer[7].Position = Vector4f(0.0f, -b, -a, 1.0f);
		pVertexBuffer[8].Position = Vector4f(b, a, 0.0f, 1.0f);
		pVertexBuffer[9].Position = Vector4f(-b, a, 0.0f, 1.0f);
		pVertexBuffer[10].Position = Vector4f(b, -a, 0.0f, 1.0f);
		pVertexBuffer[11].Position = Vector4f(-b, -a, 0.0f, 1.0f);

		
		pIndexBuffer = new unsigned int[eNumTriangles*3];
		switch(winding)
		{
			case CCW:
				pIndexBuffer[0]  = 1; pIndexBuffer[1]  = 4; pIndexBuffer[2]  = 0;//Triangle 1
				pIndexBuffer[3]  = 4; pIndexBuffer[4]  = 9; pIndexBuffer[5]  = 0;//Triangle 2
				pIndexBuffer[6]  = 4; pIndexBuffer[7]  = 5; pIndexBuffer[8]  = 9;//Triangle 3
				pIndexBuffer[9]  = 8; pIndexBuffer[10] = 5; pIndexBuffer[11] = 4;//Triangle 4
				pIndexBuffer[12] = 1; pIndexBuffer[13] = 8; pIndexBuffer[14] = 4;//Triangle 5
				pIndexBuffer[15] = 1; pIndexBuffer[16] = 10; pIndexBuffer[17] = 8;//Triangle 6
				pIndexBuffer[18] = 10; pIndexBuffer[19] = 3; pIndexBuffer[20] = 8;//Triangle 7
				pIndexBuffer[21] = 8; pIndexBuffer[22] = 3; pIndexBuffer[23] = 5;//Triangle 8
				pIndexBuffer[24] = 3; pIndexBuffer[25] = 2; pIndexBuffer[26] = 5;//Triangle 9
				pIndexBuffer[27] = 3; pIndexBuffer[28] = 7; pIndexBuffer[29] = 2;//Triangle 10
				pIndexBuffer[30] = 3; pIndexBuffer[31] = 10; pIndexBuffer[32] = 7;//Triangle 11
				pIndexBuffer[33] = 10; pIndexBuffer[34] = 6; pIndexBuffer[35] = 7;//Triangle 12
				pIndexBuffer[36] = 6; pIndexBuffer[37] = 11; pIndexBuffer[38] = 7;//Triangle 13
				pIndexBuffer[39] = 6; pIndexBuffer[40] = 0; pIndexBuffer[41] = 11;//Triangle 14
				pIndexBuffer[42] = 6; pIndexBuffer[43] = 1; pIndexBuffer[44] = 0;//Triangle 15
				pIndexBuffer[45] = 10; pIndexBuffer[46] = 1; pIndexBuffer[47] = 6;//Triangle 16
				pIndexBuffer[48] = 11; pIndexBuffer[49] = 0; pIndexBuffer[50] = 9;//Triangle 17
				pIndexBuffer[51] = 2; pIndexBuffer[52] = 11; pIndexBuffer[53] = 9;//Triangle 18
				pIndexBuffer[54] = 5; pIndexBuffer[55] = 2; pIndexBuffer[56] = 9;//Triangle 19
				pIndexBuffer[57] = 11; pIndexBuffer[58] = 2; pIndexBuffer[59] = 7;//Triangle 20
				break;
			case CW:
				pIndexBuffer[0]  = 1; pIndexBuffer[1]  = 0; pIndexBuffer[2]  = 4;//Triangle 1
				pIndexBuffer[3]  = 4; pIndexBuffer[4]  = 0; pIndexBuffer[5]  = 9;//Triangle 2
				pIndexBuffer[6]  = 4; pIndexBuffer[7]  = 9; pIndexBuffer[8]  = 5;//Triangle 3
				pIndexBuffer[9]  = 8; pIndexBuffer[10] = 4; pIndexBuffer[11] = 5;//Triangle 4
				pIndexBuffer[12] = 1; pIndexBuffer[13] = 4; pIndexBuffer[14] = 8;//Triangle 5
				pIndexBuffer[15] = 1; pIndexBuffer[16] = 8; pIndexBuffer[17] = 10;//Triangle 6
				pIndexBuffer[18] = 10; pIndexBuffer[19] = 8; pIndexBuffer[20] = 3;//Triangle 7
				pIndexBuffer[21] = 8; pIndexBuffer[22] = 5; pIndexBuffer[23] = 3;//Triangle 8
				pIndexBuffer[24] = 3; pIndexBuffer[25] = 5; pIndexBuffer[26] = 2;//Triangle 9
				pIndexBuffer[27] = 3; pIndexBuffer[28] = 2; pIndexBuffer[29] = 7;//Triangle 10
				pIndexBuffer[30] = 3; pIndexBuffer[31] = 7; pIndexBuffer[32] = 10;//Triangle 11
				pIndexBuffer[33] = 10; pIndexBuffer[34] = 7; pIndexBuffer[35] = 6;//Triangle 12
				pIndexBuffer[36] = 6; pIndexBuffer[37] = 7; pIndexBuffer[38] = 11;//Triangle 13
				pIndexBuffer[39] = 6; pIndexBuffer[40] = 11; pIndexBuffer[41] = 0;//Triangle 14
				pIndexBuffer[42] = 6; pIndexBuffer[43] = 0; pIndexBuffer[44] = 1;//Triangle 15
				pIndexBuffer[45] = 10; pIndexBuffer[46] = 6; pIndexBuffer[47] = 1;//Triangle 16
				pIndexBuffer[48] = 11; pIndexBuffer[49] = 9; pIndexBuffer[50] = 0;//Triangle 17
				pIndexBuffer[51] = 2; pIndexBuffer[52] = 9; pIndexBuffer[53] = 11;//Triangle 18
				pIndexBuffer[54] = 5; pIndexBuffer[55] = 9; pIndexBuffer[56] = 2;//Triangle 19
				pIndexBuffer[57] = 11; pIndexBuffer[58] = 7; pIndexBuffer[59] = 2;//Triangle 20
				break;
			default:
				ASSERT(false, std::string(__FILE__).append(": ").append(", Function: IcosahedronMesh(Renderer::Vertex* &pVertexBuffer, unsigned int &VertexBufferCount, unsigned int* &pIndexBuffer, unsigned int &IndexBufferCount, const VertexWinding winding) -> Bad vertex winding requested").c_str());
		}

		//calculates the normals of the mesh
		for( int i=0; i < eNumVertices ; ++i )
			pVertexBuffer[i].Normal = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);

		//Calculate face normals and adds it to each vertex of the face
		for( int i=0; i < (eNumTriangles*3) ; i+=3 )
		{
			Vector4f d1, d2;
			//the vertexs need to be follow in its winding order to get the plane normal
			if( CCW == winding )
			{
				//Counter Clockwise path
				d1 = pVertexBuffer[pIndexBuffer[i+2]].Position-pVertexBuffer[pIndexBuffer[i]].Position;
				d2 = pVertexBuffer[pIndexBuffer[i+1]].Position-pVertexBuffer[pIndexBuffer[i+2]].Position;
			}else
			{
				//Clockwise path
				d1 = pVertexBuffer[pIndexBuffer[i+1]].Position-pVertexBuffer[pIndexBuffer[i]].Position;
				d2 = pVertexBuffer[pIndexBuffer[i+2]].Position-pVertexBuffer[pIndexBuffer[i+1]].Position;
			}

			Vector4f normal = Vector4f::Cross(d1, d2);

			pVertexBuffer[pIndexBuffer[i]].Normal   += normal;
			pVertexBuffer[pIndexBuffer[i+1]].Normal += normal;
			pVertexBuffer[pIndexBuffer[i+2]].Normal += normal;
		}

		//normalize each vertex normal
		for( int i=0; i < eNumVertices ; ++i )
		{
			pVertexBuffer[i].Normal.SetW(0.0f);//they are vectors
			pVertexBuffer[i].Normal.Normalize();
		}
	}

	// Creates a cube mesh in the range: 
	//	x [-0.5, 0.5]
	//	y [-0.5, 0.5]
	//	z [-0.5, 0.5]
	//						ie each edge is 1 long
	// The normal information is calculated.
	// Parameters:
	// pVertexBuffer	 The output vertex buffer[OUT]
	// VertexBufferCount The vertex buffer elemento count[OUT]
	// pIndexBuffer		 The output index buffer[OUT]
	// IndexBufferCount	 The index buffer elemento count[OUT]
	// winding			 The desired vertex winding[IN]
	void CubeMesh(Renderer::Vertex* &pVertexBuffer, unsigned int &VertexBufferCount, unsigned int* &pIndexBuffer, unsigned int &IndexBufferCount,
		const VertexWinding winding)
	{
		enum { eNumVertices = 8, eNumTriangles = 12 };//The num of vertexs and triangles for the platonic solid
		VertexBufferCount = eNumVertices;
		IndexBufferCount = eNumTriangles*3;
		
		//pVertexBuffer = new Renderer::Vertex[eNumVertices];//new can return un align memory and every thing breaks!
		pVertexBuffer = (Renderer::Vertex*)_aligned_malloc(sizeof(Renderer::Vertex)*eNumVertices, __alignof(Renderer::Vertex));
		pVertexBuffer[0].Position = Vector4f(-0.5f, -0.5f, -0.5f, 1.0f);//Point a
		pVertexBuffer[1].Position = Vector4f( 0.5f, -0.5f, -0.5f, 1.0f);//Point b
		pVertexBuffer[2].Position = Vector4f(-0.5f,  0.5f, -0.5f, 1.0f);//Point c
		pVertexBuffer[3].Position = Vector4f( 0.5f,  0.5f, -0.5f, 1.0f);//Point d
		pVertexBuffer[4].Position = Vector4f(-0.5f, -0.5f,  0.5f, 1.0f);//Point e
		pVertexBuffer[5].Position = Vector4f( 0.5f, -0.5f,  0.5f, 1.0f);//Point f
		pVertexBuffer[6].Position = Vector4f(-0.5f,  0.5f,  0.5f, 1.0f);//Point g
		pVertexBuffer[7].Position = Vector4f( 0.5f,  0.5f,  0.5f, 1.0f);//Point h
		
		pVertexBuffer[0].Texture = Vector4f( 0.0f, 1.0f, 0.0f, 0.0f);//Point a
		pVertexBuffer[1].Texture = Vector4f( 1.0f, 1.0f, 0.0f, 0.0f);//Point b
		pVertexBuffer[2].Texture = Vector4f( 0.0f,  0.0f, 0.0f, 0.0f);//Point c
		pVertexBuffer[3].Texture = Vector4f( 1.0f,  0.0f, 0.0f, 0.0f);//Point d
		pVertexBuffer[4].Texture = Vector4f( 0.0f, 0.0f,  0.0f, 0.0f);//Point e
		pVertexBuffer[5].Texture = Vector4f( 1.0f, 0.0f,  0.0f, 0.0f);//Point f
		pVertexBuffer[6].Texture = Vector4f( 0.0f,  1.0f,  0.0f, 0.0f);//Point g
		pVertexBuffer[7].Texture = Vector4f( 1.0f,  1.0f,  0.0f, 0.0f);//Point h

		pIndexBuffer = new unsigned int[eNumTriangles*3];
		switch(winding)
		{
			case CCW:
				pIndexBuffer[0]  = 0; pIndexBuffer[1]  = 1; pIndexBuffer[2]  = 3;//Triangle 1
				pIndexBuffer[3]  = 0; pIndexBuffer[4]  = 3; pIndexBuffer[5]  = 2;//Triangle 2
				pIndexBuffer[6]  = 1; pIndexBuffer[7]  = 5; pIndexBuffer[8]  = 7;//Triangle 3
				pIndexBuffer[9]  = 1; pIndexBuffer[10] = 7; pIndexBuffer[11] = 3;//Triangle 4
				pIndexBuffer[12] = 2; pIndexBuffer[13] = 7; pIndexBuffer[14] = 6;//Triangle 5
				pIndexBuffer[15] = 2; pIndexBuffer[16] = 3; pIndexBuffer[17] = 7;//Triangle 6
				pIndexBuffer[18] = 0; pIndexBuffer[19] = 5; pIndexBuffer[20] = 1;//Triangle 7
				pIndexBuffer[21] = 0; pIndexBuffer[22] = 4; pIndexBuffer[23] = 5;//Triangle 8
				pIndexBuffer[24] = 0; pIndexBuffer[25] = 2; pIndexBuffer[26] = 6;//Triangle 9
				pIndexBuffer[27] = 0; pIndexBuffer[28] = 6; pIndexBuffer[29] = 4;//Triangle 10
				pIndexBuffer[30] = 4; pIndexBuffer[31] = 6; pIndexBuffer[32] = 7;//Triangle 11
				pIndexBuffer[33] = 4; pIndexBuffer[34] = 7; pIndexBuffer[35] = 5;//Triangle 12
				break;
			case CW:
				pIndexBuffer[0]  = 0; pIndexBuffer[1]  = 3; pIndexBuffer[2]  = 1;//Triangle 1
				pIndexBuffer[3]  = 0; pIndexBuffer[4]  = 2; pIndexBuffer[5]  = 3;//Triangle 2
				pIndexBuffer[6]  = 1; pIndexBuffer[7]  = 7; pIndexBuffer[8]  = 5;//Triangle 3
				pIndexBuffer[9]  = 1; pIndexBuffer[10] = 3; pIndexBuffer[11] = 7;//Triangle 4
				pIndexBuffer[12] = 2; pIndexBuffer[13] = 6; pIndexBuffer[14] = 7;//Triangle 5
				pIndexBuffer[15] = 2; pIndexBuffer[16] = 7; pIndexBuffer[17] = 3;//Triangle 6
				pIndexBuffer[18] = 0; pIndexBuffer[19] = 1; pIndexBuffer[20] = 5;//Triangle 7
				pIndexBuffer[21] = 0; pIndexBuffer[22] = 5; pIndexBuffer[23] = 4;//Triangle 8
				pIndexBuffer[24] = 0; pIndexBuffer[25] = 6; pIndexBuffer[26] = 2;//Triangle 9
				pIndexBuffer[27] = 0; pIndexBuffer[28] = 4; pIndexBuffer[29] = 6;//Triangle 10
				pIndexBuffer[30] = 4; pIndexBuffer[31] = 7; pIndexBuffer[32] = 6;//Triangle 11
				pIndexBuffer[33] = 4; pIndexBuffer[34] = 5; pIndexBuffer[35] = 7;//Triangle 12
				break;
			default:
				ASSERT(false, std::string(__FILE__).append(": ").append(", Function: CubeMesh(Renderer::Vertex* &pVertexBuffer, unsigned int &VertexBufferCount, unsigned int* &pIndexBuffer, unsigned int &IndexBufferCount, const VertexWinding winding) -> Bad vertex winding requested").c_str());
		}

		//calculates the normals of the mesh
		for( int i=0; i < eNumVertices ; ++i )
			pVertexBuffer[i].Normal = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);

		//Calculate face normals and adds it to each vertex of the face
		for( int i=0; i < (eNumTriangles*3) ; i+=3 )
		{
			Vector4f d1, d2;
			//the vertexs need to be follow in its winding order to get the plane normal
			if( CCW == winding )
			{
				//Counter Clockwise path
				d1 = pVertexBuffer[pIndexBuffer[i+2]].Position-pVertexBuffer[pIndexBuffer[i]].Position;
				d2 = pVertexBuffer[pIndexBuffer[i+1]].Position-pVertexBuffer[pIndexBuffer[i+2]].Position;
			}else
			{
				//Clockwise path
				d1 = pVertexBuffer[pIndexBuffer[i+1]].Position-pVertexBuffer[pIndexBuffer[i]].Position;
				d2 = pVertexBuffer[pIndexBuffer[i+2]].Position-pVertexBuffer[pIndexBuffer[i+1]].Position;
			}

			Vector4f normal = Vector4f::Cross(d1, d2);

			pVertexBuffer[pIndexBuffer[i]].Normal   += normal;
			pVertexBuffer[pIndexBuffer[i+1]].Normal += normal;
			pVertexBuffer[pIndexBuffer[i+2]].Normal += normal;
		}

		//normalize each vertex normal
		for( int i=0; i < eNumVertices ; ++i )
		{
			pVertexBuffer[i].Normal.SetW(0.0f);//they are vectors
			pVertexBuffer[i].Normal.Normalize();
		}
	}

	// Creates a plane mesh in the range: 
	//	x [-0.5, 0.5]
	//	y [-0.5, 0.5]
	//	z = 0.0
	//						ie each edge is 1 long
	// The normal information is calculated and texture.
	// Parameters:
	// pVertexBuffer	 The output vertex buffer[OUT]
	// VertexBufferCount The vertex buffer elemento count[OUT]
	// pIndexBuffer		 The output index buffer[OUT]
	// IndexBufferCount	 The index buffer elemento count[OUT]
	// winding			 The desired vertex winding[IN]
	void PlaneMesh(Renderer::Vertex* &pVertexBuffer, unsigned int &VertexBufferCount, unsigned int* &pIndexBuffer, unsigned int &IndexBufferCount,
		const VertexWinding winding)
	{
		enum { eNumVertices = 4, eNumTriangles = 2 };//The num of vertexs and triangles for the plane
		VertexBufferCount = eNumVertices;
		IndexBufferCount = eNumTriangles*3;
		
		//pVertexBuffer = new Renderer::Vertex[eNumVertices];//new can return un align memory and every thing breaks!
		pVertexBuffer = (Renderer::Vertex*)_aligned_malloc(sizeof(Renderer::Vertex)*eNumVertices, __alignof(Renderer::Vertex));
		pVertexBuffer[0].Position = Vector4f(-0.5f, -0.5f, 0.0f, 1.0f);//Point a
		pVertexBuffer[1].Position = Vector4f( 0.5f, -0.5f, 0.0f, 1.0f);//Point b
		pVertexBuffer[2].Position = Vector4f(-0.5f,  0.5f, 0.0f, 1.0f);//Point c
		pVertexBuffer[3].Position = Vector4f( 0.5f,  0.5f, 0.0f, 1.0f);//Point d
		
		pVertexBuffer[0].Texture = Vector4f( 0.0f, 1.0f, 0.0f, 0.0f);//Point a
		pVertexBuffer[1].Texture = Vector4f( 1.0f, 1.0f, 0.0f, 0.0f);//Point b
		pVertexBuffer[2].Texture = Vector4f( 0.0f,  0.0f, 0.0f, 0.0f);//Point c
		pVertexBuffer[3].Texture = Vector4f( 1.0f,  0.0f, 0.0f, 0.0f);//Point d

		pVertexBuffer[0].Normal = Vector4f(0.0f, 0.0f, -1.0f, 0.0f);//Point a
		pVertexBuffer[1].Normal = Vector4f(0.0f, 0.0f, -1.0f, 0.0f);//Point b
		pVertexBuffer[2].Normal = Vector4f(0.0f, 0.0f, -1.0f, 0.0f);//Point c
		pVertexBuffer[3].Normal = Vector4f(0.0f, 0.0f, -1.0f, 0.0f);//Point d
		
		pIndexBuffer = new unsigned int[eNumTriangles*3];
		switch(winding)
		{
			case CCW:
				pIndexBuffer[0]  = 0; pIndexBuffer[1]  = 1; pIndexBuffer[2]  = 3;//Triangle 1
				pIndexBuffer[3]  = 0; pIndexBuffer[4]  = 3; pIndexBuffer[5]  = 2;//Triangle 2
				break;
			case CW:
				pIndexBuffer[0]  = 0; pIndexBuffer[1]  = 3; pIndexBuffer[2]  = 1;//Triangle 1
				pIndexBuffer[3]  = 0; pIndexBuffer[4]  = 2; pIndexBuffer[5]  = 3;//Triangle 2
				break;
			default:
				ASSERT(false, std::string(__FILE__).append(": ").append(", Function: PlaneMesh(Renderer::Vertex* &pVertexBuffer, unsigned int &VertexBufferCount, unsigned int* &pIndexBuffer, unsigned int &IndexBufferCount, const VertexWinding winding) -> Bad vertex winding requested").c_str());
		}

	}


	// Creates a circle mesh in the range: 
	//	x [-1, 1]
	//	y [-1, 1]
	//	z [-1, 1]
	//						ie radius is 1
	// The normal information is calculated.
	// Parameters:
	// pVertexBuffer	 The output vertex buffer[OUT]
	// VertexBufferCount The vertex buffer elemento count[OUT]
	// pIndexBuffer		 The output index buffer[OUT]
	// IndexBufferCount	 The index buffer elemento count[OUT]
	// winding			 The desired vertex winding[IN]
	// LOD				 The desired level of detail, each level is a subdivision steep
	//					 (0 means no subdivision, 0+i means i subdivisions)[IN]
	//
	// IMPORTANT: THE ALGORITHM DUPLICATES THE NEW CREATED VERTEXS, BECAUSE 2 ADYANCENT TRIANGLES 
	// CREATES THE SAME VERTEXS!!! THIS IS A WEAKNESS TO SOLVE
	void CircleMesh(Renderer::Vertex* &pVertexBuffer, unsigned int &VertexBufferCount, unsigned int* &pIndexBuffer, unsigned int &IndexBufferCount,
		const VertexWinding winding, unsigned int LOD)
	{
		/* We start with a primitive, in this case an icosahedron, then the subdivision schema is as follow:

			For each triangle:
							v1
					     *		*	
				      *			 *
				   *			  *		
			    *				   *
			v2	*  *  *  *  *  *  *  v3	
						|
						|		Next subdivision
						Y
							v1
					     *		*	
				     b1	 +	+  + b3
				   *	+	  +	  *		
			    *		 +	+	   *
			v2	*  *  *  b2  *  *  *  v3	

			Adds b1, b2 and b3 in the middle of the pairs of vertexs
			If CCW then creates 4 triangles usign the 6 vertexs like:
								triangle 1: {b1, v2, b2}
								triangle 1: {v1, b1, b3}
								triangle 1: {b1, b2, b3}
								triangle 1: {b3, b2, v3}
			If CW we can create CCW, and after the creation we swap in every triangle in the IndexBuffer
			the second and third vertex index of the triangle(that makes the mesh CW).
		*/

		// start with a icosahedron as the circle primitive
		// Then refine the mesh with subdivision
		IcosahedronMesh(pVertexBuffer, VertexBufferCount, pIndexBuffer, IndexBufferCount, CCW);

		// sets the points to a circle setting
		for( int i=0; i < VertexBufferCount ; ++i )
			pVertexBuffer[i].Position.Normalize();

		// Index_i = Index_0 * 4^(LOD)
		// Index_i means index count in iteration i, where 0 means the primitive indexs
		unsigned int pow4LOD = 4;
		for(unsigned int k=1; k < LOD ; ++k)
			pow4LOD *= 4;
		unsigned int IBCount = IndexBufferCount*pow4LOD;// each level of detail creates 4 triangles out of 1(ie four more times the indexs)

		// Vertex_i = Vertex_0 + (Index_1/3)*3 + (Index_2/3)*3 + ... + (Index_i/3)*3
		// Vertex_i means vertex count in iteration i, where 0 means the primitive vertexs
		// Index_i means as in the comment before
		// Note that (Index_i/3) is the amount of triangles in iteration i

		// each level of detail adds 3 vertexs for each triangle in that iteration, plus the vertexs
		// of the previous iterations(see that the vertexs keeps the ones from previous iterations, the indexs do not!)
		unsigned int VBCount = VertexBufferCount;
		unsigned int tmpTriangles = IndexBufferCount/3;//num of triangles in that iterations, start with the primitive triangles
		for(unsigned int k=0; k < LOD ; ++k)
		{
			VBCount += tmpTriangles*3;// vertexs to add in next iteration
			tmpTriangles *= 4;// triangles in next iteration
		}

		// creates the vertex and index buffers for the circle
		Renderer::Vertex* pVB = (Renderer::Vertex*)_aligned_malloc(sizeof(Renderer::Vertex)*VBCount, __alignof(Renderer::Vertex));
		memcpy(pVB, pVertexBuffer, sizeof(Renderer::Vertex)*VertexBufferCount);

		_aligned_free(pVertexBuffer);

		// pPrevIB will keep the indexs from the previous LOD
		// pCurIB will keep the indexs from the current LOD
		unsigned int* pPrevIB = new unsigned int[IBCount];
		memcpy(pPrevIB, pIndexBuffer, sizeof(unsigned int)*IndexBufferCount);
		unsigned int* pCurIB = new unsigned int[IBCount];

		delete[] pIndexBuffer;

		// Start LOD iterations(subdivisions)
		Vector4f b1, b2, b3;
		unsigned int trianglesCount;
		//for each LOD we make a subdivision
		for(unsigned int it=0; it < LOD ; ++it)
		{
			trianglesCount = (IndexBufferCount/3);
			IndexBufferCount = 0;
			//for each triangle subdivide
			for(unsigned int t=0; t < trianglesCount ; ++t)
			{
				unsigned int v1Index = pPrevIB[t*3];
				unsigned int v2Index = pPrevIB[t*3+1];
				unsigned int v3Index = pPrevIB[t*3+2];

				// triangle vertexs
				Vector4f& v1 = pVB[v1Index].Position;
				Vector4f& v2 = pVB[v2Index].Position;
				Vector4f& v3 = pVB[v3Index].Position;

				// creates the new vertexs
				b1 = Vector4f::Lerp(v1, v2, 0.5f);
				b2 = Vector4f::Lerp(v2, v3, 0.5f);
				b3 = Vector4f::Lerp(v3, v1, 0.5f);

				// put the new vertexs to a circle position setting
				b1.Normalize();
				b2.Normalize();
				b3.Normalize();

				// adds the new vertexs
				pVB[VertexBufferCount++].Position = b1;
				pVB[VertexBufferCount++].Position = b2;
				pVB[VertexBufferCount++].Position = b3;

				unsigned int b1Index = VertexBufferCount-3;
				unsigned int b2Index = VertexBufferCount-2;
				unsigned int b3Index = VertexBufferCount-1;

				// adds the 4 new triangles(12 indexs)
				pCurIB[IndexBufferCount++]=b1Index; pCurIB[IndexBufferCount++]=v2Index; pCurIB[IndexBufferCount++]=b2Index;
				pCurIB[IndexBufferCount++]=v1Index; pCurIB[IndexBufferCount++]=b1Index; pCurIB[IndexBufferCount++]=b3Index;
				pCurIB[IndexBufferCount++]=b1Index; pCurIB[IndexBufferCount++]=b2Index; pCurIB[IndexBufferCount++]=b3Index;
				pCurIB[IndexBufferCount++]=b3Index; pCurIB[IndexBufferCount++]=b2Index; pCurIB[IndexBufferCount++]=v3Index;
			}

			std::swap(pPrevIB, pCurIB);
		}

		// set the buffers pointers
		pVertexBuffer = pVB;
		pIndexBuffer = pPrevIB;

		delete[] pCurIB;//it was just temporal

		// until now the mesh is CCW, if we were requested CW then we make it CW
		switch(winding)
		{
			case CW:
				// swap the second and third index of each triangle
				for( int i=0; i < IndexBufferCount ; i+=3 )
					std::swap(pIndexBuffer[i+1], pIndexBuffer[i+2]);
				break;
			default:
				ASSERT(false, std::string(__FILE__).append(": ").append(", Function: CircleMesh(Renderer::Vertex* &pVertexBuffer, unsigned int &VertexBufferCount, unsigned int* &pIndexBuffer, unsigned int &IndexBufferCount, const VertexWinding winding, unsigned int LOD) -> Bad vertex winding requested").c_str());
		}
		

		//calculates the normals of the mesh
		for( int i=0; i < VertexBufferCount ; ++i )
			pVertexBuffer[i].Normal = Vector4f(0.0f, 0.0f, 0.0f, 0.0f);

		//Calculate face normals and adds it to each vertex of the face
		for( int i=0; i < IndexBufferCount ; i+=3 )
		{
			Vector4f d1, d2;

			//the vertexs need to be follow in its winding order to get the plane normal
			if( CCW == winding )
			{
				//Counter Clockwise path
				d1 = pVertexBuffer[pIndexBuffer[i+2]].Position-pVertexBuffer[pIndexBuffer[i]].Position;
				d2 = pVertexBuffer[pIndexBuffer[i+1]].Position-pVertexBuffer[pIndexBuffer[i+2]].Position;
			}else
			{
				//Clockwise path
				d1 = pVertexBuffer[pIndexBuffer[i+1]].Position-pVertexBuffer[pIndexBuffer[i]].Position;
				d2 = pVertexBuffer[pIndexBuffer[i+2]].Position-pVertexBuffer[pIndexBuffer[i+1]].Position;
			}

			Vector4f normal = Vector4f::Cross(d1, d2);

			pVertexBuffer[pIndexBuffer[i]].Normal   += normal;
			pVertexBuffer[pIndexBuffer[i+1]].Normal += normal;
			pVertexBuffer[pIndexBuffer[i+2]].Normal += normal;
		}

		//normalize each vertex normal
		for( int i=0; i < VertexBufferCount ; ++i )
		{
			pVertexBuffer[i].Normal.SetW(0.0f);//they are vectors
			pVertexBuffer[i].Normal.Normalize();
		}

	}


}

#undef HALF_SPACE
#undef _MIN
#undef _MAX


#endif // end RENDERER_H