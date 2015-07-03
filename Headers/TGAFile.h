#pragma once

#include "ImageFile.h"
#include <string>

namespace SoftRenderer
{

	//Carga archivos de imagenes TGA
	//Limitaciones:
	// - Soporta solo formatos RGB y RGBA
	// - No soporta paletas de colores
	// - No soporta datos comprimidos
	// - No lee metadatos
	class TGAFile: public ImageFile
	{
	private:

		//Encabezado de archivos TGA
		#pragma pack(push, 1)
		struct {
			char      idlength;
			char      colourmaptype;
			char      datatypecode;
			short int colourmaporigin; // --
			short int colourmaplength; // | Especificacion del mapa de colores(si es que tiene)
			char      colourmapdepth;  // --
			short int x_origin;        // --
			short int y_origin;        // |
			short     width;           // | Especificacion de la Imagen
			short     height;          // |
			char      bitsperpixel;    // | 
			char      imagedescriptor; // --
			} m_Header;
		#pragma pack(pop)

		//Carga la imagen del stream de dato, sea cual sea la fuente de este
		void _Load(std::istream& Stream);

	};

}