#include "BMPFile.h"

#include <iostream>
#include <fstream>

#include "Config.h"

//-------------------------------------------------------------//
//-------------			BMPFile			-----------------------//
//-------------------------------------------------------------//

namespace SoftRenderer
{

	void BMPFile::_Load(std::istream &Stream)
	{
		Stream.read((char*)&m_FileHeader, sizeof(m_FileHeader));

		if( m_FileHeader.bfType != 0x4D42 )
			throw std::exception();//"El archivo cargado no es un bmp");

		Stream.read((char*)&m_InfoHeader, sizeof(m_InfoHeader));

		//Leemos las dimensiones de la imagen del encabezado
		m_Width  = m_InfoHeader.biWidth;

		m_Height = m_InfoHeader.biHeight;

		if( m_InfoHeader.biPlanes != 1 )
			throw std::exception();//"El archivo bmp tiene mas de 1 plano de color");

		if( m_InfoHeader.biCompression != BI_RGB )
			throw std::exception();//"El archivo bmp tiene una compresion de datos no soportada por la implementacion");

		Stream.seekg(m_FileHeader.bfOffBits, std::ios::beg);//Nos posicionamos en el arreglo de Colores/Indices

		int padding = 0;//canitdad de bytes de padding por fila(entre [0,3])
		int scanlinebytes = m_Width * (m_InfoHeader.biBitCount/8);
		while ( ( scanlinebytes + padding ) % 4 != 0 )
			padding++;
		int rowTotSize = scanlinebytes + padding;//Tamaño en bytes de una fila 
												 //del bitmapconsiderando el padding

		//Solo una comprobacion paraverificar que no halla "cosas raras", basicamente el tamaño
		//total de datos dividido lo que ocupa cada fila es igual a la altura
		ASSERT( ( (m_FileHeader.bfSize-m_FileHeader.bfOffBits) / rowTotSize ) == m_Height, std::string(__FILE__).append(": ").append(", Function: _Load(std::istream &Stream) -> The BMP header is corrupted").c_str() );

		switch( m_InfoHeader.biBitCount )
		{
			case 24:
				{
				m_ColorSpace = ImageFile::R8_G8_B8;
				m_ColorArray = new unsigned char[m_Width*m_Height*3];

				unsigned char* scanlineBuff = new unsigned char[rowTotSize];//Una fila del bitmap con padding
				for(unsigned int i=0; i < m_Height ; ++i)//Por cada fila del bitmap
				{
					Stream.read((char*)scanlineBuff, sizeof(unsigned char)*rowTotSize);

					for(unsigned int j=0; j < m_Width ; ++j)//Por cada pixel de la fila
					{
						int pos = ( m_Height - i - 1 ) * m_Width*3 + j*3;

						m_ColorArray[pos+2] = scanlineBuff[j*3];  //B -> Azul
						m_ColorArray[pos+1] = scanlineBuff[j*3+1];//G -> Verde
						m_ColorArray[pos]   = scanlineBuff[j*3+2];//R -> Rojo
					}
				}
				delete[] scanlineBuff;

				}
			break;
			case 32:
				{

				m_ColorSpace = ImageFile::R8_G8_B8_A8;
				m_ColorArray = new unsigned char[m_Width*m_Height*4];

				unsigned char* scanlineBuff = new unsigned char[rowTotSize];//Una fila del bitmap con padding
				for(unsigned int i=0; i < m_Height ; ++i)//Por cada fila del bitmap
				{
					Stream.read((char*)scanlineBuff, sizeof(unsigned char)*rowTotSize);
					for(unsigned int j=0; j < m_Width ; ++j)//Por cada pixel de la fila
					{
						int pos = ( m_Height - i - 1 ) * m_Width*4 + j*4;

						m_ColorArray[pos+3] = scanlineBuff[j*4+3];//A -> Alpha
						m_ColorArray[pos+2] = scanlineBuff[j*4];  //B -> Azul
						m_ColorArray[pos+1] = scanlineBuff[j*4+1];//G -> Verde
						m_ColorArray[pos]   = scanlineBuff[j*4+2];//R -> Rojo
						
					}
				}
				delete[] scanlineBuff;

				}
			break;
			default:
				throw std::exception();//"El archivo bmp esta en espacio de colores que esta implementacion no soporta");
			break;
		}

		if( Stream.bad() || Stream.fail() || Stream.eof() )
			throw std::exception();//"La carga del archivo bmp tuvo errores");
	}

}