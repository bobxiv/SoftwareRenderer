#include "ImageFile.h"

#include <fstream>
#include <sstream>

//================================================
/////////////		ImageFile		//////////////
//================================================

namespace SoftRenderer
{

	ImageFile::ImageFile(): m_ColorArray(NULL), m_Width(0), m_Height(0)
	{}

	ImageFile::~ImageFile()
	{
		if( m_ColorArray )
			delete[] m_ColorArray;
	}

	void ImageFile::LoadFromFile(std::string filename) throw(std::exception)
	{
		std::ifstream fileStream(filename.c_str(), std::ios::binary);

		if( !fileStream.is_open() )
			throw (new std::exception( std::string(std::string("Serialize::Load(std::string fileName) => The file ")
									+ filename + std::string(" cannot be open to be Loaded") ).c_str() )
									);

		try
		{
			this->_Load(fileStream);
		}catch(std::exception* e)
		{
			throw e;
		}

		fileStream.close();
	}

	void ImageFile::LoadFromFile(char* pMemoryBuffer) throw(std::exception)
	{
		std::istringstream sstr(pMemoryBuffer);

		std::istream* pInputStream = dynamic_cast<std::istream*>(&sstr);
	
		if( pInputStream->fail() || pInputStream->eof() || pInputStream->bad() )
			throw (new std::exception( std::string(std::string("Serialize::Load(std::string fileName) => The file ")
									 + std::string(" cannot be open to be Loaded") ).c_str() )
									);

		try
		{
			this->_Load(*pInputStream);
		}catch(std::exception* e)
		{
			throw e;
		}
	}

	unsigned char* ImageFile::GetColors()
	{
		return m_ColorArray;
	}

	unsigned int ImageFile::GetWidth()
	{
		return m_Width;
	}

	unsigned int ImageFile::GetHeight()
	{
		return m_Height;
	}

	ImageFile::ColorSpace ImageFile::GetColorSpace()
	{
		return m_ColorSpace;
	}

}