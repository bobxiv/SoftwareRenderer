#pragma once

#include "ImageFile.h"
#include <string>
#include "png.h"

namespace SoftRenderer
{

	//Esta funcion es un callback que sirve para que LibPNG lea los datos de un objeto std::istream
	//Parametros:
	//- pPNGReadStruct   El struct de lectura de PNG que estamos usando
	//- data             Un puntero a un buffer a donde deberemos leer datos del std::istream
	//- length           La cantidad de bytes a leer de std::istream
	void leerDatosfn(png_structp pPNGReadStruct, png_bytep data, png_size_t length);

	//Carga archivos de imagenes PNG
	//Depende de libPNG
	class PNGFile: public ImageFile
	{
	private:

		//Encabezado de archivos PNG
		png_infop   m_pHeader;

		png_structp m_pPNGReadStruct;

		//Carga la imagen del stream de dato, sea cual sea la fuente de este
		void _Load(std::istream& Stream);

	public:

		PNGFile();

		~PNGFile();
	};

}