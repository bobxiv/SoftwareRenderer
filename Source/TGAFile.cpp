#include "TGAFile.h"

#include <iostream>
#include <fstream>

//-------------------------------------------------------------//
//-------------			TGAFile			-----------------------//
//-------------------------------------------------------------//

namespace SoftRenderer
{

	void TGAFile::_Load(std::istream& Stream)
	{
		Stream.read((char*)&m_Header, sizeof(m_Header));

		//Saltamos el id del archivo
		Stream.seekg(m_Header.idlength, std::ios::cur);

		//Como cargamos imagenes sin paleta de colores no hay que cargar nada

		m_Width = m_Header.width;

		m_Height = m_Header.height;

		if( m_Header.datatypecode != 2 )
			throw std::exception();//No es formato RGB sin compresion

		if( m_Header.colourmaptype != 0 )
			throw std::exception();//El archivo tga tiene un mapa de colores

		int bit5TopToBottom = m_Header.imagedescriptor & 32;//00100000b; Si es 0 esta "dado vuelta", si es 1 esta "derecho"
		int bit4LeftToRight = m_Header.imagedescriptor & 16;//00010000b; Si es 0 esta de izquierda a derecha, si es 1 esta de derecha a izquierda

		//Como no tiene color map(paleta) luego cargamos el RGB/A
		//Hay muchas alternativas porque el formato tga permite poner el origen de la imagen en distintas partes, lo
		//que implica que los pixeles esten en otro orden
		switch( m_Header.bitsperpixel )
		{
			case 24:
				m_ColorSpace = ImageFile::R8_G8_B8;
				m_ColorArray = new unsigned char[m_Width*m_Height*3];

				if( bit5TopToBottom != 0 )//Origen arriba
				{
					if( bit4LeftToRight != 0 )//Origen derecha
					{
						unsigned char* scanlineBuff = new unsigned char[m_Width*3];//Una fila del bitmap
						for(unsigned int i=0; i < m_Height ; ++i)//Por cada fila del tga
						{
							Stream.read((char*)scanlineBuff, sizeof(unsigned char)*m_Width*3);
							for(unsigned int j=0; j < m_Width ; ++j)//Por cada pixel de la fila
							{
								int pos = i * m_Width*3 + ( m_Width - j - 1 ) * 3;

								m_ColorArray[pos+2] = scanlineBuff[j*3];  //B -> Azul
								m_ColorArray[pos+1] = scanlineBuff[j*3+1];//G -> Verde
								m_ColorArray[pos]   = scanlineBuff[j*3+2];//R -> Rojo
							}
						}
						delete[] scanlineBuff;
					}else//Origen izquierda
					{
						for(unsigned int i=0; i < m_Width*m_Height ; ++i)
						{
							Stream.read((char*)&m_ColorArray[i*3+2], sizeof(unsigned char));//B -> Azul
							Stream.read((char*)&m_ColorArray[i*3+1], sizeof(unsigned char));//G -> Verde
							Stream.read((char*)&m_ColorArray[i*3]  , sizeof(unsigned char));//R -> Rojo
						}
					}
				}else//Origen abajo
				{
					if( bit4LeftToRight != 0 )//Origen derecha
					{
						unsigned char* scanlineBuff = new unsigned char[m_Width*3];//Una fila del bitmap
						for(unsigned int i=0; i < m_Height ; ++i)//Por cada fila del tga
						{
							Stream.read((char*)scanlineBuff, sizeof(unsigned char)*m_Width*3);
							for(unsigned int j=0; j < m_Width ; ++j)//Por cada pixel de la fila
							{
								int pos = ( m_Height - i - 1 ) * m_Width*3 + ( m_Width - j - 1 ) * 3;

								m_ColorArray[pos+2] = scanlineBuff[j*3];  //B -> Azul
								m_ColorArray[pos+1] = scanlineBuff[j*3+1];//G -> Verde
								m_ColorArray[pos]   = scanlineBuff[j*3+2];//R -> Rojo
							}
						}
						delete[] scanlineBuff;
					}else//Origen izquierda
					{
						unsigned char* scanlineBuff = new unsigned char[m_Width*3];//Una fila del bitmap
						for(unsigned int i=0; i < m_Height ; ++i)//Por cada fila del tga
						{
							Stream.read((char*)scanlineBuff, sizeof(unsigned char)*m_Width*3);
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
				}

			break;
			case 32:
				{

				m_ColorSpace = ImageFile::R8_G8_B8_A8;
				m_ColorArray = new unsigned char[m_Width*m_Height*4];

				if( bit5TopToBottom != 0 )//Origen arriba
				{
					if( bit4LeftToRight != 0 )//Origen derecha
					{
						unsigned char* scanlineBuff = new unsigned char[m_Width*4];//Una fila del bitmap
						for(unsigned int i=0; i < m_Height ; ++i)//Por cada fila del tga
						{
							Stream.read((char*)scanlineBuff, sizeof(unsigned char)*m_Width*4);
							for(unsigned int j=0; j < m_Width ; ++j)//Por cada pixel de la fila
							{
								int pos = i * m_Width*4 + ( m_Width - j - 1 ) * 4;

								m_ColorArray[pos+2] = scanlineBuff[j*4];  //B -> Azul
								m_ColorArray[pos+1] = scanlineBuff[j*4+1];//G -> Verde
								m_ColorArray[pos]   = scanlineBuff[j*4+2];//R -> Rojo
								m_ColorArray[pos+3] = scanlineBuff[j*4+3];//A -> Alpha
							}
						}
						delete[] scanlineBuff;
					}else
					{
						for(unsigned int i=0; i < m_Width*m_Height ; ++i)
						{
							Stream.read((char*)&m_ColorArray[i*4+2], sizeof(unsigned char));//B -> Azul
							Stream.read((char*)&m_ColorArray[i*4+1], sizeof(unsigned char));//G -> Verde
							Stream.read((char*)&m_ColorArray[i*4]  , sizeof(unsigned char));//R -> Rojo
							Stream.read((char*)&m_ColorArray[i*4+3], sizeof(unsigned char));//A -> Alpha
						}
					}
				}else//Origen abajo
				{
					if( bit4LeftToRight != 0 )//Origen derecha
					{
						unsigned char* scanlineBuff = new unsigned char[m_Width*4];//Una fila del bitmap
						for(unsigned int i=0; i < m_Height ; ++i)//Por cada fila del tga
						{
							Stream.read((char*)scanlineBuff, sizeof(unsigned char)*m_Width*4);
							for(unsigned int j=0; j < m_Width ; ++j)//Por cada pixel de la fila
							{
								int pos = ( m_Height - i - 1 ) * m_Width*4 + ( m_Width - j - 1 ) * 4;

								m_ColorArray[pos+2] = scanlineBuff[j*4];  //B -> Azul
								m_ColorArray[pos+1] = scanlineBuff[j*4+1];//G -> Verde
								m_ColorArray[pos]   = scanlineBuff[j*4+2];//R -> Rojo
								m_ColorArray[pos+3] = scanlineBuff[j*4+3];//A -> Alpha
							}
						}
						delete[] scanlineBuff;
					}else
					{
						unsigned char* scanlineBuff = new unsigned char[m_Width*4];//Una fila del bitmap
						for(unsigned int i=0; i < m_Height ; ++i)//Por cada fila del tga
						{
							Stream.read((char*)scanlineBuff, sizeof(unsigned char)*m_Width*4);
							for(unsigned int j=0; j < m_Width ; ++j)//Por cada pixel de la fila
							{
								int pos = ( m_Height - i - 1 ) * m_Width*4 + j*4;

								m_ColorArray[pos+2] = scanlineBuff[j*4];  //B -> Azul
								m_ColorArray[pos+1] = scanlineBuff[j*4+1];//G -> Verde
								m_ColorArray[pos]   = scanlineBuff[j*4+2];//R -> Rojo
								m_ColorArray[pos+3] = scanlineBuff[j*4+3];//A -> Alpha
							}
						}
						delete[] scanlineBuff;
					}
				}

				}
			break;
			default:
				throw std::exception();//El archivo tga esta en espacio de colores que esta implementacion no soporta
			break;
		}

		if( Stream.bad() || Stream.fail() || Stream.eof() )
			throw std::exception();//La carga del archivo tga tuvo errores
	}

}