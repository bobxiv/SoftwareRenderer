#pragma once
#include <string>

#include "Texture2D.h"

namespace SoftRenderer
{

	//Carga archivos de imagenes PPM
	//Limitaciones:
	// - Soporta solo formatos RGB de 24 bits
	// - Solo carga formato "binario" P6
	class PPMFile: public ImageFile
	{
	private:

		//Encabezado de archivos PPM
		#pragma pack(push, 1)
		struct {
			short int numMagico;
			int       width;
			int       height;
			int       maxColor;
			} m_Header;
		#pragma pack(pop)

		//Carga la imagen del stream de dato, sea cual sea la fuente de este
		void _Load(std::istream& Stream);
	};

}