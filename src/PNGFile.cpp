#include "PNGFile.h"

#include <iostream>
#include <fstream>

//-------------------------------------------------------------//
//-------------			PNGFile			-----------------------//
//-------------------------------------------------------------//

namespace SoftRenderer
{

	void leerDatosfn(png_structp pPNGReadStruct, png_bytep data, png_size_t length) {
		//Ahora obtenemos el std::istream de donde estamos leyendo del Struct de lectura PNG
		//Este lo pasamos en el llamado a la funcion png_set_read_fn()
		png_voidp a = png_get_io_ptr(pPNGReadStruct);

		//Hacemos un cast ya que a es en realidad un puntero a std::istream
		//Y leemos length cantidad de bytes en el buffer data
		((std::istream*)a)->read((char*)data, length);
	}


	PNGFile::PNGFile(): m_pHeader(NULL), m_pPNGReadStruct(NULL)
	{}

	PNGFile::~PNGFile()//Llamara al destructor de ImageFile asi que no habra memory leaks
	{
		if( m_pPNGReadStruct )
			png_destroy_read_struct(&m_pPNGReadStruct, &m_pHeader,(png_infopp)0);//liberamos el png y el encabezado
	}

	void PNGFile::_Load(std::istream &Stream)
	{
		//Leemos y verificamos la firma del archivo para asegurarnos que sea un png
		png_bytep pFirma = new png_byte[8];
		Stream.read((char*)pFirma, sizeof(png_byte)*8);

		if( png_sig_cmp(pFirma, 0, 8) != 0 )
			throw std::exception();//"El archivo cargado no es un png");
		delete[] pFirma;


		m_pPNGReadStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if ( m_pPNGReadStruct == NULL )//Si fallo la creacion del struct de lectura del png
			throw std::exception();//"No se pudo crear el struct de lectura de png");

		//Establecemos la funcion/callback que usare LibPNG para leer los datos
		png_set_read_fn(m_pPNGReadStruct,(png_voidp)&Stream, leerDatosfn);

		//Intentamos crear el encabezado del archivo png
		m_pHeader = png_create_info_struct(m_pPNGReadStruct);
		if ( m_pHeader == NULL ) 
		{
			png_destroy_read_struct(&m_pPNGReadStruct, (png_infopp)0, (png_infopp)0);//liberamos el png
			throw std::exception();//"No se pudo crear el encabezado del archivo png");
		}


		//Apuntara a cada fila de la imagen en m_EspacioColores... ver mas abajo una explicacion mas detallada
		png_bytep* ArregloFilas = NULL;

		//Informamos que si ocurre un error leyendo el archivo salte a este if
		if (setjmp(png_jmpbuf(m_pPNGReadStruct))) {
			png_destroy_read_struct(&m_pPNGReadStruct, &m_pHeader,(png_infopp)0);//liberamos el png y el encabezado
			if (ArregloFilas != NULL) 
				delete [] ArregloFilas;
			if (m_ColorArray != NULL) 
				delete [] m_ColorArray;

			throw std::exception();//"Sucedio un error leyendo el archivo png");
		}

		//Avisamos a libpng que la firma ya fue leida y que lea el encabezado
		png_set_sig_bytes(m_pPNGReadStruct, 8);

		//Leemos el encabezado del png
		png_read_info(m_pPNGReadStruct, m_pHeader);

		//Leemos las dimensiones de la imagen del encabezado
		m_Width  = png_get_image_width (m_pPNGReadStruct, m_pHeader);

		m_Height = png_get_image_height(m_pPNGReadStruct, m_pHeader);

		//Bits por canal de la imagen(NO POR PIXEL, SINO POR CANAL... EN GENERAL 1 PIXEL TIENE MUCHOS CANALES)
		png_uint_32 bitdepth   = png_get_bit_depth(m_pPNGReadStruct, m_pHeader);
		//Numero de canales de la imagen
		png_uint_32 channels   = png_get_channels(m_pPNGReadStruct, m_pHeader);
		//Tipo de espacio de colores... luego lo convertiremos a m_EspacioColores
		png_uint_32 color_type = png_get_color_type(m_pPNGReadStruct, m_pHeader);

		//Si el formato de la imagen no es R8_G8_B8_ o R8_G8_B8_A8 intentamos convertirla a
		//estos usando funciones de LibPNG
		switch (color_type)
		{
			case PNG_COLOR_TYPE_RGB:
				m_ColorSpace = ImageFile::R8_G8_B8;
				break;
			case PNG_COLOR_TYPE_RGBA:
				m_ColorSpace = ImageFile::R8_G8_B8_A8;
				break;
			case PNG_COLOR_TYPE_PALETTE:
				png_set_palette_to_rgb(m_pPNGReadStruct);
				m_ColorSpace = ImageFile::R8_G8_B8;
				channels = 3;//ahora tendremos 3 canales
				break;
			case PNG_COLOR_TYPE_GRAY:
				png_set_gray_to_rgb(m_pPNGReadStruct);
				m_ColorSpace = ImageFile::R8_G8_B8;
				bitdepth = 8;
				channels = 3;//ahora tendremos 3 canales
				break;
			default:
				throw std::exception();//"La implementacion de PNGFile no soporta el espacio de colores de la imagen que se esta cargando");
		}

		//Si la imagen tiene transparencia la convertimos al canal alpha
		if (png_get_valid(m_pPNGReadStruct, m_pHeader, PNG_INFO_tRNS)) {
			png_set_tRNS_to_alpha(m_pPNGReadStruct);
			channels+=1;//Un canal mas
		}

		//Si cada canal tiene 16 bits de profundidad lo convertimos a 8 bits
		//ya que nuestros formatos son de 8 bits de profundidad
		if( bitdepth == 16 )
			png_set_strip_16(m_pPNGReadStruct);


		//LibPNG lee los datos de color por filas de la imagen
		//Para hacerlo requiere que le pasemos un arreglo de punteros a donde se almacenara cada 
		//fila de la imagen
		ArregloFilas = new png_bytep[m_Height];

		//Almacenamos el buffer donde se almacenara la imagen leida(ArregloFilas apuntara a direciones dentro de este buffer)
		m_ColorArray = new unsigned char[m_Width * m_Height * (bitdepth * channels / 8)];

		//El tamaño que ocupa una fila de la imagen
		const unsigned int rowTotSize = m_Width * bitdepth * channels / 8;

		//Ahora apuntamos los punteros de ArregloFilas a las direciones de m_ArregloColores
		//donde cada elemento de ArregloFilas es la dir de una fila de la imagen
		for (unsigned int i = 0; i < m_Height; i++)
			ArregloFilas[i] = (png_bytep)m_ColorArray + i*rowTotSize;

		//Aca leemos los datos de la imagen del archivo
		//Alamcena los cada fila en las direciones señaladas por ArregloFilas
		//que a su vez apuntan dentro de m_ArregloColores... lo cual significa
		//que luego de esto m_ArregloColores tendra los datos de la imagen como los deseamos
		png_read_image(m_pPNGReadStruct, ArregloFilas);
	
		//Limpiamos el arreglo de punteros a filas(NO LOS DATOS SINO SOLO LOS PUNTEROS)
		delete[] (png_bytep)ArregloFilas;
		png_destroy_read_struct(&m_pPNGReadStruct, &m_pHeader,(png_infopp)0);//liberamos el png y el encabezado


		if( Stream.bad() || Stream.fail() || Stream.eof() )
			throw std::exception();//"La carga del archivo bmp tuvo errores");
	}

}