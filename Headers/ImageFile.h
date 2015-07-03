#pragma once

#include <string>
#include <iostream>

namespace SoftRenderer
{

	// La clase base de las imagenes. Es abstracta
	class ImageFile
	{
	public:

		// Tipos:
		// - R8_G8_B8    significa RGB con 8 bits por canal
		// - R8_G8_B8_A8 significa RGBA con 8 bits por canal
		enum ColorSpace{R8_G8_B8, R8_G8_B8_A8};

	protected:

		//El arreglo de colores de la imagen. Esta en el espacio de colores especificado por m_EspacioColores y sin padding.
		//Cada canal del espacio de colores tiene una profundidad de 8 bits.
		unsigned char* m_ColorArray;

		//El ancho de la imagen en pixeles
		unsigned int m_Width;

		//El ancho de la imagen en pixeles
		unsigned int m_Height;

		//El espacio de colores de la imagen
		ColorSpace m_ColorSpace;

		virtual void _Load(std::istream& Stream)      =0;//Carga la imagen del stream de dato, sea cual sea la fuente de este

	public:	

		ImageFile();
		virtual ~ImageFile();

		//Carga la imagen desde un archivo en disco
		//Parametros:
		// - nombre         El nombre del archivo a cargar(debe tener la extension correcta)
		virtual void LoadFromFile(std::string filename);

		//Carga la imagen desde la memoria
		//Parametros:
		// - pMemoryBuffer  La direcion de memoria donde comienza la imagen
		virtual void LoadFromFile(char* pMemoryBuffer);
	
		//Devuelve el arreglo de colores en formato RGB/A segun diga GetEspacioColores()
		//El arreglo tiene alineacion de 1 byte(ie sin padding). La profundidad de color por cada canal es 8 bits.
		//Almacenando cada valor en un unsigned char
		unsigned char* GetColors();

		//Devuelve ancho de la imagen en pixeles
		unsigned int GetWidth();

		//Devuelve alto de la imagen en pixeles
		unsigned int GetHeight();

		//Devuelve espacio de colores de la imagen
		ColorSpace GetColorSpace();

	};

}