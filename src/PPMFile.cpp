#include "PPMFile.h"

#include <iostream>
#include <fstream>

#include <limits>

//-------------------------------------------------------------//
//-------------			PPMFile			-----------------------//
//-------------------------------------------------------------//

namespace SoftRenderer
{

	void PPMFile::_Load(std::istream& Stream)
	{
		char dummy; int maxc;
		Stream.get(dummy);
		if (dummy!='P')
			throw std::exception();//"El archivo cargado no es un ppm");
		Stream.get(dummy);
		if (dummy!='6')
			throw std::exception();//"El archivo cargado no es un ppm");
		Stream.get(dummy);
		dummy=Stream.peek();
		if (dummy=='#') do {
			Stream.get(dummy);
		} while (dummy!=10);
		Stream >> m_Width >> m_Height;//Leemos las dimensiones de la imagen en pixeles
		Stream >> maxc;//Valor de color maximo
		Stream.get(dummy);
	
		m_ColorSpace = ImageFile::R8_G8_B8;
		m_ColorArray = new unsigned char[m_Width*m_Height*3];
		float aux = std::numeric_limits<unsigned char>::max()/(float)maxc;//factor para convertir de lo leido a unsigned char
		for(unsigned int i=0; i < m_Width*m_Height ; ++i)
		{
			Stream.read((char*)&m_ColorArray[i*3]  , sizeof(unsigned char));//R -> Rojo
			Stream.read((char*)&m_ColorArray[i*3+1], sizeof(unsigned char));//G -> Verde
			Stream.read((char*)&m_ColorArray[i*3+2], sizeof(unsigned char));//B -> Azul

			m_ColorArray[i*3]   *= aux;
			m_ColorArray[i*3+1] *= aux;
			m_ColorArray[i*3+2] *= aux;
		}
	}

}