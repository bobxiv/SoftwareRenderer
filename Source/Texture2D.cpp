#include "Texture2D.h"
#include <exception>

#include "BMPFile.h"
#include "TGAFile.h"
#include "PPMFile.h"
#include "PNGFile.h"

namespace SoftRenderer
{

	Texture2D::Texture2D(std::string filename, unsigned int IDTexture): m_IDTexture(IDTexture)
	{
		ImageFile* pImage = NULL;
		//Intenta cargar el archivo de imagen(dependiendo de la extension del archivo es la clase que usa)
		try
		{
			_LoadImage(filename, pImage);
		}catch( std::exception& e )
		{
			throw e;
		}

		//Crear y configurar la textura

		// la textura siempre tiene canal alpha, si la imagen no lo tiene le damos 255(segun nuestra convencion)
		m_ColorArray = new unsigned char[m_Width*m_Height*4];
	
		switch( m_ColorSpace )
		{
		case ImageFile::R8_G8_B8:
			{
				unsigned char* src = pImage->GetColors();
				for(unsigned int i=0; i < (m_Width*m_Height) ; ++i)
				{
					m_ColorArray[i*4] = src[i*3];
					m_ColorArray[i*4+1] = src[i*3+1];
					m_ColorArray[i*4+2] = src[i*3+2];
					m_ColorArray[i*4+3] = 255;
				}
			}
			break;
		case ImageFile::R8_G8_B8_A8:
			{
				memcpy(m_ColorArray, pImage->GetColors(), sizeof(unsigned char)*m_Width*m_Height*4);
			}
			break;
		}
		//Luego de esto ya se podria eliminar el arreglo m_ArregloColores, pero lo conservaremos y lo eliminaremos
		//en el destructor de la clase
		delete pImage;
	}

	Texture2D::~Texture2D()
	{
		delete[] m_ColorArray;
	}

	Vector4f Texture2D::Sample2DNearest(const float u, const float v)
	{
		ASSERT(u >= 0.0f && v >= 0.0f && u <= 1.0f && v <= 1.0f, std::string(__FILE__).append(": ").append(", Function: Texture2D::Sample2DNearest(const float u, const float v) -> Texture coordinates(u,v) out of range").c_str() );

		const float xCoord = LERP(0.0f, static_cast<float>(m_Width-1), u);
		const float yCoord = LERP(0.0f, static_cast<float>(m_Height-1), v);

		const unsigned int xCoordTexture = floor(xCoord + 0.5f);
		const unsigned int yCoordTexture = floor(yCoord + 0.5f);
		
		Vector4f Color = Vector4f(static_cast<float>(m_ColorArray[yCoordTexture*m_Width*4+xCoordTexture*4]),
								  static_cast<float>(m_ColorArray[yCoordTexture*m_Width*4+xCoordTexture*4+1]),
								  static_cast<float>(m_ColorArray[yCoordTexture*m_Width*4+xCoordTexture*4+2]),
								  static_cast<float>(m_ColorArray[yCoordTexture*m_Width*4+xCoordTexture*4+3]) );

		return ( Color / 255.0f );//pass each color channel in the range [0.0f, 1.0f]
	}

	Vector4f Texture2D::Sample2DBilineal(float u, float v)
	{
		//Not optimized, to much verifications
		u = Saturate<float>(u);
		v = Saturate<float>(v);
		ASSERT(u >= 0.0f && v >= 0.0f && u <= 1.0f && v <= 1.0f, std::string(__FILE__).append(": ").append(", Function: Texture2D::Sample2DBilineal(const float u, const float v) -> Texture coordinates(u,v) out of range").c_str() );

		const float xCoord = LERP(0.0f, static_cast<float>(m_Width-1), u);
		const float yCoord = LERP(0.0f, static_cast<float>(m_Height-1), v);

		//Top Left
		unsigned int xCoordTL = floor(xCoord);
		unsigned int yCoordTL = floor(yCoord);

		xCoordTL = Clamp<unsigned int>(xCoordTL, 0, m_Width-1);
		yCoordTL = Clamp<unsigned int>(yCoordTL, 0, m_Height-1);

		//Top Right
		unsigned int xCoordTR = ceil(xCoord);
		unsigned int yCoordTR = floor(yCoord);

		xCoordTR = Clamp<unsigned int>(xCoordTR, 0, m_Width-1);
		yCoordTR = Clamp<unsigned int>(yCoordTR, 0, m_Height-1);

		//Bottom Left
		unsigned int xCoordBL = floor(xCoord);
		unsigned int yCoordBL = ceil(yCoord);

		xCoordBL = Clamp<unsigned int>(xCoordBL, 0, m_Width-1);
		yCoordBL = Clamp<unsigned int>(yCoordBL, 0, m_Height-1);

		//Bottom Right
		unsigned int xCoordBR = ceil(xCoord);
		unsigned int yCoordBR = ceil(yCoord);

		xCoordBR = Clamp<unsigned int>(xCoordBR, 0, m_Width-1);
		yCoordBR = Clamp<unsigned int>(yCoordBR, 0, m_Height-1);

		const Vector4f TLColor = Vector4f(static_cast<float>(m_ColorArray[yCoordTL*m_Width*4+xCoordTL*4]),
										  static_cast<float>(m_ColorArray[yCoordTL*m_Width*4+xCoordTL*4+1]),
										  static_cast<float>(m_ColorArray[yCoordTL*m_Width*4+xCoordTL*4+2]),
										  static_cast<float>(m_ColorArray[yCoordTL*m_Width*4+xCoordTL*4+3]) );
		const Vector4f TRColor = Vector4f(static_cast<float>(m_ColorArray[yCoordTR*m_Width*4+xCoordTR*4]),
										  static_cast<float>(m_ColorArray[yCoordTR*m_Width*4+xCoordTR*4+1]),
										  static_cast<float>(m_ColorArray[yCoordTR*m_Width*4+xCoordTR*4+2]),
										  static_cast<float>(m_ColorArray[yCoordTR*m_Width*4+xCoordTR*4+3]) );
		const Vector4f BLColor = Vector4f(static_cast<float>(m_ColorArray[yCoordBL*m_Width*4+xCoordBL*4]),
										  static_cast<float>(m_ColorArray[yCoordBL*m_Width*4+xCoordBL*4+1]),
										  static_cast<float>(m_ColorArray[yCoordBL*m_Width*4+xCoordBL*4+2]),
										  static_cast<float>(m_ColorArray[yCoordBL*m_Width*4+xCoordBL*4+3]) );
		const Vector4f BRColor = Vector4f(static_cast<float>(m_ColorArray[yCoordBR*m_Width*4+xCoordBR*4]),
										  static_cast<float>(m_ColorArray[yCoordBR*m_Width*4+xCoordBR*4+1]),
										  static_cast<float>(m_ColorArray[yCoordBR*m_Width*4+xCoordBR*4+2]),
										  static_cast<float>(m_ColorArray[yCoordBR*m_Width*4+xCoordBR*4+3]) );

		float y2 = yCoordBR;
		float y1 = yCoordTR;
		if( (y2-y1) == 0 )//temp fix
		{
			if( (y2+1) < m_Width )
				++y2;
			else
				--y1;
		}
		float x2 = xCoordTR;
		float x1 = xCoordTL;
		if( (x2-x1) == 0 )//temp fix
		{
			if( (x2+1) < m_Width )
				++x2;
			else
				--x1;
		}
		const float& x = xCoord;
		const float& y = yCoord;

		// Bilineal interpolation:
		// R2 = TL (x2-x)/(x2-x1) + TR (x-x1)/(x2-x1) 
		// R1 = BL (x2-x)/(x2-x1) + BR (x-x1)/(x2-x1)
		// C = R2 (y2-y)/(y2-y1) + R1 (y-y1)/(y2-y1)
		// Then:
		//
		//		C = (y2-y)/(y2-y1)*[(x2-x)/(x2-x1 TL + (x-x1)/(x2-x1) TR] + (y-y1)/(y2-y1)*[(x2-x)/(x2-x1) BL + (x-x1)/(x2-x1) BR]
		Vector4f C = ( TLColor * (x2-x)/(x2-x1) + TRColor * (x-x1)/(x2-x1) )*(y2-y)/(y2-y1) +
				     ( BLColor * (x2-x)/(x2-x1) + BRColor * (x-x1)/(x2-x1) )*(y-y1)/(y2-y1);

		return (C / 255.0f);//pass each color channel in the range [0.0f, 1.0f]

		// Average(not bilineal of course)
		//Vector4f res = (TLColor*0.25f+TRColor*0.25f+BLColor*0.25f+BRColor*0.25f) / 255.0f;
		//return res;
	}

	void Texture2D::_LoadImage(std::string fileName, ImageFile* &pImage)
	{
		std::string::size_type pos = fileName.rfind('.');
		std::string extension = fileName.substr(pos+1);

		if( extension == "bmp" )
		{
			pImage = new BMPFile;
		}else if( extension == "tga" )
		{
			pImage = new TGAFile;
		}else if( extension == "ppm" )
		{
			pImage = new PPMFile;
		}else if( extension == "png" )
		{
			pImage = new PNGFile;
		}else if( extension == "jpg" )
		{
			//JPGFile(nombre);
		}

		pImage->LoadFromFile(fileName);
		m_Width          = pImage->GetWidth();
		m_Height         = pImage->GetHeight();
		m_ColorSpace     = pImage->GetColorSpace();
	}

	ImageFile::ColorSpace Texture2D::GetColorSpace()
	{
		return m_ColorSpace;
	}

	int Texture2D::GetWidth()
	{
		return m_Width;
	}

	int Texture2D::GetHeight()
	{
		return m_Height;
	}

	int Texture2D::GetIDTexture()
	{
		return m_IDTexture;
	}

}