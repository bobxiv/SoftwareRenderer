#pragma once
#include <string>

//#include <GL/glut.h>

#include "ImageFile.h"

#include "Config.h"

using namespace MathModule;

namespace SoftRenderer
{

	class Renderer;

	class Texture2D
	{
	private:
	
		ImageFile::ColorSpace	  m_ColorSpace;

		unsigned int              m_Width;
		unsigned int              m_Height;

		unsigned char*			  m_ColorArray;

		unsigned int	          m_IDTexture;

		void _LoadImage(std::string fileName, ImageFile* &pImage);

		//Only accesible by Renderer class(the factory)
		Texture2D(std::string fileName, unsigned int IDTexture);

		~Texture2D();

		Vector4f Sample2DNearest(const float u, const float v);

		Vector4f Sample2DBilineal(float u, float v);

	public:

		friend class Renderer;

		ImageFile::ColorSpace GetColorSpace();

		int GetWidth();

		int GetHeight();

		int GetIDTexture();
	};

}