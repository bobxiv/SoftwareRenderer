#pragma once

#include "ImageFile.h"
#include <string>
#include "windows.h"

namespace SoftRenderer
{

	//Carga archivos de imagenes BMP
	//Limitaciones:
	// - Soporta solo formatos RGB y RGBA
	// - No soporta paletas de colores
	// - No soporta datos comprimidos(RLE)
	class BMPFile: public ImageFile
	{
	private:

		//Encabezado de archivos BMP
		BITMAPFILEHEADER        m_FileHeader;

		//Encabezado de imagen BMP
		BITMAPINFOHEADER        m_InfoHeader;

		//Carga la imagen del stream de dato, sea cual sea la fuente de este
		void _Load(std::istream& Stream);

	};

}