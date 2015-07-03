//Conclusion:
//
//Para tener buen rendimiento con operaciones SSE estas deben:
//	1) No tener sobrecarga en el codigo vecino ya que si por ejemplo hay saltos, comparaciones, incrementos de int
//		va a hacer que termine rapido la SSE pero que la otra tarde mucho y por eso no halla mejora con respecto(o muy poca)
//		al FPU. Osea si ponemos operaciones en un for que halla muchas operaciones por cada loop!!!
//		Y si podemos hacer un unroll del for muchisimo mejor!!!
//	2) Intentar si es posible estar dentro de la linea de cache o unas lineas de cache para que las operaciones con memoria
//		sean mucho mas rapidas y no sean el cuello de botella de las operaciones. Sino intentar que queden registros SSE para
//		usar en las operaciones

#include <windows.h>

#include "Config.h"


using namespace MathModule;

#include "Renderer.h"

//#include <xmmintrin.h>

#include <cmath>
#include <iostream>
#include <time.h>
#include <cassert>


//Configuracion de la prueba
//--------------------------

//#define NOT_ALIGNED_MEMORY

//Variables Globales
//iteraciones es un define entonces el for de las pruebas puede hacer un unroll
//#define iterations 5000000//Para vectores
#define iterations 1000000//Para matrices
//la cantidad de pruebas es un define entonces el for de las pruebas puede hacer un unroll
//#define cantPruebas 60//Para vectores
#define cantPruebas 20//Para matrices



enum TipoOperacion{Mix, Suma, Resta, Multiplicacion, Division, RaizCuadrada, Dot, Cross};
TipoOperacion op = TipoOperacion::Multiplicacion;//TipoOperacion::Multiplicacion;

//			Prueba
//--------------------------


void TestVector4(double* TiemposPruebas)
{
	#ifdef NOT_ALIGNED_MEMORY
		//Vector4* data = new Vector4[iterations];
		Vector4f* origData = (Vector4f*)_aligned_malloc(iterations*sizeof(Vector4f)+1, __alignof(Vector4f));
		Vector4f* data     = (Vector4f*) (((char*)origData)+2);
	#else
		Vector4f* data = (Vector4f*)_aligned_malloc(iterations*sizeof(Vector4f), __alignof(Vector4f));
		if( data == NULL )
		{
			std::cerr<<"No hay suficiente memoria para correr la prueba!!!"<<std::endl<<std::endl;
			exit(0);
		}
	#endif


	std::cerr<<"--------------------------------------------------"<<std::endl;
	std::cerr<<"Alineacion:"<<std::endl;
	//Siempre deberia decir que la memoria esta alineada pero como esto puede afectar mucho el rendimiento lo 
	//controlamos
	if (((int)data % __alignof(Vector4f) ) == 0)
        printf_s( "El puntero: %d  --- esta alineado a %d bytes\n", data, __alignof(Vector4f));
    else
        printf_s( "El puntero: %d  --- no esta esta alineado a %d bytes\n", data, __alignof(Vector4f));
	std::cerr<<"--------------------------------------------------"<<std::endl<<std::endl;
		
	srand(static_cast<unsigned int>(time(NULL)));

	//La frequencia del contador de alto rendiemiento no varia
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	std::cerr<<"--------------------------------------------------"<<std::endl;
	std::cerr<<"La operacion a probar es: ";
	switch( op )
	{
		case TipoOperacion::Mix:
			std::cerr<<"Mix"<<std::endl;
			break;
		case TipoOperacion::Suma:
			std::cerr<<"Suma"<<std::endl;
			break;
		case TipoOperacion::Resta:
			std::cerr<<"Resta"<<std::endl;
			break;
		case TipoOperacion::Multiplicacion:
			std::cerr<<"Multiplicacion"<<std::endl;
			break;
		case TipoOperacion::Division:
			std::cerr<<"Division"<<std::endl;
			break;
		case TipoOperacion::RaizCuadrada:
			std::cerr<<"RaizCuadrada"<<std::endl;
			break;
		case TipoOperacion::Dot:
			std::cerr<<"Dot"<<std::endl;
			break;
		case TipoOperacion::Cross:
			std::cerr<<"Cross"<<std::endl;
			break;
		default:
			std::cerr<<"OPERACION NO RECONOCIDA!!! SALIENDO"<<std::endl;
			exit(0);
			break;
	}
	std::cerr<<"--------------------------------------------------"<<std::endl<<std::endl;

	std::cerr<<std::endl<<std::endl<<"Comienza la prueba"<<std::endl;

	//Por cada tipo de operacion
	switch( op )
	{
		case TipoOperacion::Mix:

			for(unsigned int k=0; k<cantPruebas ;++k)
			{
				//inicializamos los operandos
				for(unsigned int i=0; i < iterations ;++i)
					data[i]   = Vector4f(static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100));

				//Comienza la medicion de tiempo
				LARGE_INTEGER startCount;
				LARGE_INTEGER endCount;
				QueryPerformanceCounter(&startCount);

				//COMIENZA LA PRUEBA!
				for(unsigned int i=0; i < (iterations-1) ;++i)
				{
					//data[i]   = data[i] +data[i+1];
					//data[i+1] = data[i] -data[i+1];
					//data[i+2] = data[i+2]*data[i+3];
					//data[i+3] = data[i+2]/data[i+3];
					//data[i+4] = data[i+3].SquareLength();
				}

				//Cacula el tiempo tardado
				QueryPerformanceCounter(&endCount);
				TiemposPruebas[k] = (endCount.QuadPart - startCount.QuadPart)/(double)freq.QuadPart;
			}

		break;

		case TipoOperacion::Suma:

			for(unsigned int k=0; k<cantPruebas ;++k)
			{
				//inicializamos los operandos
				for(unsigned int i=0; i < iterations ;++i)
					data[i]   = Vector4f(static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100));

				//Comienza la medicion de tiempo
				LARGE_INTEGER startCount;
				LARGE_INTEGER endCount;
				QueryPerformanceCounter(&startCount);

				//COMIENZA LA PRUEBA!
				for(unsigned int i=0; i < (iterations-1) ;++i)
				{
					//data[i]   = data[i]  +data[i+1];//15 operaciones vectoriales para sacarle el jugo a la vel de 
					//data[i]   = data[i]  +data[i+1];//SSE y que no tengamos el cuello de botella en el for
					//data[i]   = data[i]  +data[i+1];//si hace un for unroll.. no deberia dar problemas pero por las dudas
					//data[i]   = data[i]  +data[i+1];//es mejor muchas instruciones sin ninguna salto
					//data[i]   = data[i]  +data[i+1];
					//data[i]   = data[i]  +data[i+1];
					//data[i]   = data[i]  +data[i+1];
					//data[i]   = data[i]  +data[i+1];
					//data[i]   = data[i]  +data[i+1];
					//data[i]   = data[i]  +data[i+1];
					//data[i]   = data[i]  +data[i+1];
					//data[i]   = data[i]  +data[i+1];
					//data[i]   = data[i]  +data[i+1];
					//data[i]   = data[i]  +data[i+1];
					//data[i]   = data[i]  +data[i+1];
				}

				//Cacula el tiempo tardado
				QueryPerformanceCounter(&endCount);
				TiemposPruebas[k] = (endCount.QuadPart - startCount.QuadPart)/(double)freq.QuadPart;
			}

			break;

		case TipoOperacion::Resta:

			for(unsigned int k=0; k<cantPruebas ;++k)
			{
				//inicializamos los operandos
				for(unsigned int i=0; i < iterations ;++i)
					data[i]   = Vector4f(static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100));

				//Comienza la medicion de tiempo
				LARGE_INTEGER startCount;
				LARGE_INTEGER endCount;
				QueryPerformanceCounter(&startCount);

				//COMIENZA LA PRUEBA!
				for(unsigned int i=0; i < (iterations-1) ;++i)
				{
					//data[i]   = data[i]  -data[i+1];//15 operaciones vectoriales para sacarle el jugo a la vel de 
					//data[i]   = data[i]  -data[i+1];//SSE y que no tengamos el cuello de botella en el for
					//data[i]   = data[i]  -data[i+1];//si hace un for unroll.. no deberia dar problemas pero por las dudas
					//data[i]   = data[i]  -data[i+1];//es mejor muchas instruciones sin ninguna salto
					//data[i]   = data[i]  -data[i+1];
					//data[i]   = data[i]  -data[i+1];
					//data[i]   = data[i]  -data[i+1];
					//data[i]   = data[i]  -data[i+1];
					//data[i]   = data[i]  -data[i+1];
					//data[i]   = data[i]  -data[i+1];
					//data[i]   = data[i]  -data[i+1];
					//data[i]   = data[i]  -data[i+1];
					//data[i]   = data[i]  -data[i+1];
					//data[i]   = data[i]  -data[i+1];
					//data[i]   = data[i]  -data[i+1];
				}

				//Cacula el tiempo tardado
				QueryPerformanceCounter(&endCount);
				TiemposPruebas[k] = (endCount.QuadPart - startCount.QuadPart)/(double)freq.QuadPart;
			}

			break;

		case TipoOperacion::Multiplicacion:

			for(unsigned int k=0; k<cantPruebas ;++k)
			{
				//inicializamos los operandos
				for(unsigned int i=0; i < iterations ;++i)
					data[i]   = Vector4f(static_cast<float>(rand()%10), static_cast<float>(rand()%10), static_cast<float>(rand()%10), static_cast<float>(rand()%10));

				//Comienza la medicion de tiempo
				LARGE_INTEGER startCount;
				LARGE_INTEGER endCount;
				QueryPerformanceCounter(&startCount);

				//COMIENZA LA PRUEBA!
				for(unsigned int i=7; i < (iterations-10) ;++i)//los limites son para operar de forma de tener variedad
				{											   //pero no llegar a tener +inf que es muy lento en sus operaciones de fpu
					
					data[i]   = data[i+10]  * data[i+1] * data[i+1];//15 operaciones vectoriales para sacarle el jugo a la vel de 
					data[i-1] = data[i+9]  * data[i+2] * data[i+2];//SSE y que no tengamos el cuello de botella en el for
					data[i]   = data[i+8]  * data[i+3] * data[i+3];//si hace un for unroll.. no deberia dar problemas pero por las dudas
					data[i-2] = data[i+7]  * data[i+4] * data[i+4];//es mejor muchas instruciones sin ninguna salto
					data[i]   = data[i+6]  * data[i+5] * data[i+5];
					data[i-3] = data[i+5]  * data[i+6] * data[i+6];
					data[i]   = data[i+4]  * data[i+7] * data[i+7];
					data[i-4] = data[i+3]  * data[i+8] * data[i+8];
					data[i]   = data[i+2]  * data[i+9] * data[i+9];
					data[i-5] = data[i+1]  * data[i+10]* data[i+10];
					data[i]   = data[i+2]  * data[i+1] * data[i+1];
					data[i-6] = data[i+3]  * data[i+2] * data[i+2];
					data[i]   = data[i+4]  * data[i+3] * data[i+3];
					data[i-7] = data[i+5]  * data[i+4] * data[i+4];
					data[i]   = data[i+6]  * data[i+5] * data[i+5];
					//assert( data[i] == data[i] );//si es distinto es infinito o nan
				}

				//Cacula el tiempo tardado
				QueryPerformanceCounter(&endCount);
				TiemposPruebas[k] = (endCount.QuadPart - startCount.QuadPart)/(double)freq.QuadPart;
			}

			break;

		case TipoOperacion::Division:

			for(unsigned int k=0; k<cantPruebas ;++k)
			{
			//inicializamos los operandos
			for(unsigned int i=0; i < iterations ;++i)
				data[i]   = Vector4f(static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100));

			//Comienza la medicion de tiempo
			LARGE_INTEGER startCount;
			LARGE_INTEGER endCount;
			QueryPerformanceCounter(&startCount);

			//COMIENZA LA PRUEBA!
			for(unsigned int i=0; i < (iterations-1) ;++i)
			{
				/*data[i]   = data[i]  /data[i+1];
				data[i+1] = data[i]  /data[i+1];
				data[i+2] = data[i+2]/data[i+3];
				data[i+3] = data[i+2]/data[i+3];
				data[i+4] = data[i+3]/data[i+4];*/
			}

			//Cacula el tiempo tardado
			QueryPerformanceCounter(&endCount);
			TiemposPruebas[k] = (endCount.QuadPart - startCount.QuadPart)/(double)freq.QuadPart;
			}

			break;

		case TipoOperacion::RaizCuadrada:

			for(unsigned int k=0; k<cantPruebas ;++k)
			{
				//inicializamos los operandos
				for(unsigned int i=0; i < iterations ;++i)
					data[i]   = Vector4f(static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100));

				//Comienza la medicion de tiempo
				LARGE_INTEGER startCount;
				LARGE_INTEGER endCount;
				QueryPerformanceCounter(&startCount);

				//COMIENZA LA PRUEBA!
				for(unsigned int i=0; i < (iterations-1) ;++i)
				{
					/*data[i]   = data[i].SquareLength();
					data[i+1] = data[i+1].SquareLength();
					data[i+2] = data[i+2].SquareLength();
					data[i+3] = data[i+3].SquareLength();
					data[i+4] = data[i+4].SquareLength();*/
				}

				//Cacula el tiempo tardado
				QueryPerformanceCounter(&endCount);
				TiemposPruebas[k] = (endCount.QuadPart - startCount.QuadPart)/(double)freq.QuadPart;
			}

			break;

		case TipoOperacion::Dot:

			for(unsigned int k=0; k<cantPruebas ;++k)
			{
				//inicializamos los operandos
				for(unsigned int i=0; i < iterations ;++i)
					data[i]   = Vector4f(static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100));

				//Comienza la medicion de tiempo
				LARGE_INTEGER startCount;
				LARGE_INTEGER endCount;
				QueryPerformanceCounter(&startCount);

				//COMIENZA LA PRUEBA!
				for(unsigned int i=7; i < (iterations-10) ;++i)//los limites son para operar de forma de tener variedad
				{											   //pero no llegar a tener +inf que es muy lento en sus operaciones de fpu
					data[i].SetX( Vector4f::Dot(data[i+10], data[i+1]) );//15 operaciones vectoriales para sacarle el jugo a la vel de 
					data[i-1].SetX( Vector4f::Dot(data[i+9], data[i+2]) );//SSE y que no tengamos el cuello de botella en el for
					data[i].SetX( Vector4f::Dot(data[i+8], data[i+3]) );//si hace un for unroll.. no deberia dar problemas pero por las dudas
					data[i-2].SetX( Vector4f::Dot(data[i+7], data[i+4]) );//es mejor muchas instruciones sin ninguna salto
					data[i].SetX( Vector4f::Dot(data[i+6], data[i+5]) );
					data[i-3].SetX( Vector4f::Dot(data[i+5], data[i+6]) );
					data[i].SetX( Vector4f::Dot(data[i+4], data[i+7]) );
					data[i-4].SetX( Vector4f::Dot(data[i+3], data[i+8]) );
					data[i].SetX( Vector4f::Dot(data[i+2], data[i+9]) );
					data[i-5].SetX( Vector4f::Dot(data[i+1], data[i+10]) );
					data[i].SetX( Vector4f::Dot(data[i+2], data[i+1]) );
					data[i-6].SetX( Vector4f::Dot(data[i+3], data[i+2]) );
					data[i].SetX( Vector4f::Dot(data[i+4], data[i+3]) );
					data[i-7].SetX( Vector4f::Dot(data[i+5], data[i+4]) );
					data[i].SetX( Vector4f::Dot(data[i+6], data[i+5]) );
					/*float a   = Vector4f::Dot(data[i+10], data[i+1]);//15 operaciones vectoriales para sacarle el jugo a la vel de 
					a = Vector4f::Dot(data[i+9], data[i+2]);//SSE y que no tengamos el cuello de botella en el for
					a   = Vector4f::Dot(data[i+8], data[i+3]);//si hace un for unroll.. no deberia dar problemas pero por las dudas
					a = Vector4f::Dot(data[i+7], data[i+4]);//es mejor muchas instruciones sin ninguna salto
					a   = Vector4f::Dot(data[i+6], data[i+5]);
					a = Vector4f::Dot(data[i+5], data[i+6]);
					a   = Vector4f::Dot(data[i+4], data[i+7]);
					a = Vector4f::Dot(data[i+3], data[i+8]);
					a   = Vector4f::Dot(data[i+2], data[i+9]);
					a = Vector4f::Dot(data[i+1], data[i+10]);
					a   = Vector4f::Dot(data[i+2], data[i+1]);
					a = Vector4f::Dot(data[i+3], data[i+2]);
					a   = Vector4f::Dot(data[i+4], data[i+3]);
					a = Vector4f::Dot(data[i+5], data[i+4]);
					a   = Vector4f::Dot(data[i+6], data[i+5]);*/
				}

				//Cacula el tiempo tardado
				QueryPerformanceCounter(&endCount);
				TiemposPruebas[k] = (endCount.QuadPart - startCount.QuadPart)/(double)freq.QuadPart;
			}

			break;

		case TipoOperacion::Cross:

			for(unsigned int k=0; k<cantPruebas ;++k)
			{
				//inicializamos los operandos
				for(unsigned int i=0; i < iterations ;++i)
					data[i]   = Vector4f(static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100));

				//Comienza la medicion de tiempo
				LARGE_INTEGER startCount;
				LARGE_INTEGER endCount;
				QueryPerformanceCounter(&startCount);

				//COMIENZA LA PRUEBA!
				for(unsigned int i=7; i < (iterations-10) ;++i)//los limites son para operar de forma de tener variedad
				{											   //pero no llegar a tener +inf que es muy lento en sus operaciones de fpu
					data[i]   = Vector4f::Cross(data[i+10], data[i+1]);//15 operaciones vectoriales para sacarle el jugo a la vel de 
					data[i-1] = Vector4f::Cross(data[i+9], data[i+2]);//SSE y que no tengamos el cuello de botella en el for
					data[i]   = Vector4f::Cross(data[i+8], data[i+3]);//si hace un for unroll.. no deberia dar problemas pero por las dudas
					data[i-2] = Vector4f::Cross(data[i+7], data[i+4]);//es mejor muchas instruciones sin ninguna salto
					data[i]   = Vector4f::Cross(data[i+6], data[i+5]);
					data[i-3] = Vector4f::Cross(data[i+5], data[i+6]);
					data[i]   = Vector4f::Cross(data[i+4], data[i+7]);
					data[i-4] = Vector4f::Cross(data[i+3], data[i+8]);
					data[i]   = Vector4f::Cross(data[i+2], data[i+9]);
					data[i-5] = Vector4f::Cross(data[i+1], data[i+10]);
					data[i]   = Vector4f::Cross(data[i+2], data[i+1]);
					data[i-6] = Vector4f::Cross(data[i+3], data[i+2]);
					data[i]   = Vector4f::Cross(data[i+4], data[i+3]);
					data[i-7] = Vector4f::Cross(data[i+5], data[i+4]);
					data[i]   = Vector4f::Cross(data[i+6], data[i+5]);
				}

				//Cacula el tiempo tardado
				QueryPerformanceCounter(&endCount);
				TiemposPruebas[k] = (endCount.QuadPart - startCount.QuadPart)/(double)freq.QuadPart;
			}

			break;

	}

	#ifdef NOT_ALIGNED_MEMORY
		//delete[] data;
		_aligned_free(origData);
	#else
		_aligned_free(data);
	#endif
}


void TestMatrix4x4(double* TiemposPruebas)
{
	#ifdef NOT_ALIGNED_MEMORY
		//Vector4* data = new Vector4[iterations];
		Matrix4x4f* origData = (Matrix4x4f*)_aligned_malloc(iterations*sizeof(Matrix4x4f)+1, __alignof(Matrix4x4f));
		Matrix4x4f* data     = (Matrix4x4f*) (((char*)origData)+2);
	#else
		Matrix4x4f* data = (Matrix4x4f*)_aligned_malloc(iterations*sizeof(Matrix4x4f), __alignof(Matrix4x4f));
		if( data == NULL )
		{
			std::cerr<<"No hay suficiente memoria para correr la prueba!!!"<<std::endl<<std::endl;
			exit(0);
		}
	#endif


	std::cerr<<"--------------------------------------------------"<<std::endl;
	std::cerr<<"Alineacion:"<<std::endl;
	//Siempre deberia decir que la memoria esta alineada pero como esto puede afectar mucho el rendimiento lo 
	//controlamos
	if (((int)data % __alignof(Matrix4x4f) ) == 0)
        printf_s( "El puntero: %d  --- esta alineado a %d bytes\n", data, __alignof(Matrix4x4f));
    else
        printf_s( "El puntero: %d  --- no esta esta alineado a %d bytes\n", data, __alignof(Matrix4x4f));
	std::cerr<<"--------------------------------------------------"<<std::endl<<std::endl;
		
	srand(static_cast<unsigned int>(time(NULL)));

	//La frequencia del contador de alto rendiemiento no varia
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	std::cerr<<"--------------------------------------------------"<<std::endl;
	std::cerr<<"La operacion a probar es: ";
	switch( op )
	{
		case TipoOperacion::Mix:
			std::cerr<<"Mix"<<std::endl;
			break;
		case TipoOperacion::Suma:
			std::cerr<<"Suma"<<std::endl;
			break;
		case TipoOperacion::Resta:
			std::cerr<<"Resta"<<std::endl;
			break;
		case TipoOperacion::Multiplicacion:
			std::cerr<<"Multiplicacion"<<std::endl;
			break;
		case TipoOperacion::Division:
			std::cerr<<"Division"<<std::endl;
			break;
		case TipoOperacion::RaizCuadrada:
			std::cerr<<"RaizCuadrada"<<std::endl;
			break;
		default:
			std::cerr<<"OPERACION NO RECONOCIDA!!! SALIENDO"<<std::endl;
			exit(0);
			break;
	}
	std::cerr<<"--------------------------------------------------"<<std::endl<<std::endl;

	std::cerr<<std::endl<<std::endl<<"Comienza la prueba"<<std::endl;

	//Por cada tipo de operacion
	switch( op )
	{
		case TipoOperacion::Mix:

			for(unsigned int k=0; k<cantPruebas ;++k)
			{
				//inicializamos los operandos
				for(unsigned int i=0; i < iterations ;++i)
					data[i]   = Matrix4x4f(static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100),
										   static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100),
										   static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100),
										   static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100));

				//Comienza la medicion de tiempo
				LARGE_INTEGER startCount;
				LARGE_INTEGER endCount;
				QueryPerformanceCounter(&startCount);

				//COMIENZA LA PRUEBA!
				for(unsigned int i=0; i < (iterations-1) ;++i)
				{
					//data[i]   = data[i] +data[i+1];
					//data[i+1] = data[i] -data[i+1];
					//data[i+2] = data[i+2]*data[i+3];
					//data[i+3] = data[i+2]/data[i+3];
					//data[i+4] = data[i+3].SquareLength();
				}

				//Cacula el tiempo tardado
				QueryPerformanceCounter(&endCount);
				TiemposPruebas[k] = (endCount.QuadPart - startCount.QuadPart)/(double)freq.QuadPart;
			}

		break;

		case TipoOperacion::Suma:

			for(unsigned int k=0; k<cantPruebas ;++k)
			{
				//inicializamos los operandos
				for(unsigned int i=0; i < iterations ;++i)
					data[i]   = Matrix4x4f(static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100),
										   static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100),
										   static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100),
										   static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100));

				//Comienza la medicion de tiempo
				LARGE_INTEGER startCount;
				LARGE_INTEGER endCount;
				QueryPerformanceCounter(&startCount);

				//COMIENZA LA PRUEBA!
				for(unsigned int i=0; i < (iterations-1) ;++i)
				{
					//data[i]   = data[i]  +data[i+1];//15 operaciones vectoriales para sacarle el jugo a la vel de 
					//data[i]   = data[i]  +data[i+1];//SSE y que no tengamos el cuello de botella en el for
					//data[i]   = data[i]  +data[i+1];//si hace un for unroll.. no deberia dar problemas pero por las dudas
					//data[i]   = data[i]  +data[i+1];//es mejor muchas instruciones sin ninguna salto
					//data[i]   = data[i]  +data[i+1];
					//data[i]   = data[i]  +data[i+1];
					//data[i]   = data[i]  +data[i+1];
					//data[i]   = data[i]  +data[i+1];
					//data[i]   = data[i]  +data[i+1];
					//data[i]   = data[i]  +data[i+1];
					//data[i]   = data[i]  +data[i+1];
					//data[i]   = data[i]  +data[i+1];
					//data[i]   = data[i]  +data[i+1];
					//data[i]   = data[i]  +data[i+1];
					//data[i]   = data[i]  +data[i+1];
				}

				//Cacula el tiempo tardado
				QueryPerformanceCounter(&endCount);
				TiemposPruebas[k] = (endCount.QuadPart - startCount.QuadPart)/(double)freq.QuadPart;
			}

			break;

		case TipoOperacion::Resta:

			for(unsigned int k=0; k<cantPruebas ;++k)
			{
				//inicializamos los operandos
				for(unsigned int i=0; i < iterations ;++i)
					data[i]   = Matrix4x4f(static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100),
										   static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100),
										   static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100),
										   static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100));

				//Comienza la medicion de tiempo
				LARGE_INTEGER startCount;
				LARGE_INTEGER endCount;
				QueryPerformanceCounter(&startCount);

				//COMIENZA LA PRUEBA!
				for(unsigned int i=0; i < (iterations-1) ;++i)
				{
					//data[i]   = data[i]  -data[i+1];//15 operaciones vectoriales para sacarle el jugo a la vel de 
					//data[i]   = data[i]  -data[i+1];//SSE y que no tengamos el cuello de botella en el for
					//data[i]   = data[i]  -data[i+1];//si hace un for unroll.. no deberia dar problemas pero por las dudas
					//data[i]   = data[i]  -data[i+1];//es mejor muchas instruciones sin ninguna salto
					//data[i]   = data[i]  -data[i+1];
					//data[i]   = data[i]  -data[i+1];
					//data[i]   = data[i]  -data[i+1];
					//data[i]   = data[i]  -data[i+1];
					//data[i]   = data[i]  -data[i+1];
					//data[i]   = data[i]  -data[i+1];
					//data[i]   = data[i]  -data[i+1];
					//data[i]   = data[i]  -data[i+1];
					//data[i]   = data[i]  -data[i+1];
					//data[i]   = data[i]  -data[i+1];
					//data[i]   = data[i]  -data[i+1];
				}

				//Cacula el tiempo tardado
				QueryPerformanceCounter(&endCount);
				TiemposPruebas[k] = (endCount.QuadPart - startCount.QuadPart)/(double)freq.QuadPart;
			}

			break;

		case TipoOperacion::Multiplicacion:

			for(unsigned int k=0; k<cantPruebas ;++k)
			{
				//inicializamos los operandos
				for(unsigned int i=0; i < iterations ;++i)
					data[i]   = Matrix4x4f(static_cast<float>(rand()%10), static_cast<float>(rand()%10), static_cast<float>(rand()%10), static_cast<float>(rand()%10),
										   static_cast<float>(rand()%10), static_cast<float>(rand()%10), static_cast<float>(rand()%10), static_cast<float>(rand()%10),
										   static_cast<float>(rand()%10), static_cast<float>(rand()%10), static_cast<float>(rand()%10), static_cast<float>(rand()%10),
										   static_cast<float>(rand()%10), static_cast<float>(rand()%10), static_cast<float>(rand()%10), static_cast<float>(rand()%10));

				//Comienza la medicion de tiempo
				LARGE_INTEGER startCount;
				LARGE_INTEGER endCount;
				QueryPerformanceCounter(&startCount);

				//COMIENZA LA PRUEBA!
				for(unsigned int i=7; i < (iterations-10) ;++i)//los limites son para operar de forma de tener variedad
				{											   //pero no llegar a tener +inf que es muy lento en sus operaciones de fpu
					
					data[i]   = data[i+10]  * data[i+1] * data[i+1];//15 operaciones matriciales para sacarle el jugo a la vel de 
					data[i-1] = data[i+9]  * data[i+2] * data[i+2];//SSE y que no tengamos el cuello de botella en el for
					data[i]   = data[i+8]  * data[i+3] * data[i+3];//si hace un for unroll.. no deberia dar problemas pero por las dudas
					data[i-2] = data[i+7]  * data[i+4] * data[i+4];//es mejor muchas instruciones sin ninguna salto
					data[i]   = data[i+6]  * data[i+5] * data[i+5];
					data[i-3] = data[i+5]  * data[i+6] * data[i+6];
					data[i]   = data[i+4]  * data[i+7] * data[i+7];
					data[i-4] = data[i+3]  * data[i+8] * data[i+8];
					data[i]   = data[i+2]  * data[i+9] * data[i+9];
					data[i-5] = data[i+1]  * data[i+10]* data[i+10];
					data[i]   = data[i+2]  * data[i+1] * data[i+1];
					data[i-6] = data[i+3]  * data[i+2] * data[i+2];
					data[i]   = data[i+4]  * data[i+3] * data[i+3];
					data[i-7] = data[i+5]  * data[i+4] * data[i+4];
					data[i]   = data[i+6]  * data[i+5] * data[i+5];
					//assert( data[i] == data[i] );//si es distinto es infinito o nan
				}

				//Cacula el tiempo tardado
				QueryPerformanceCounter(&endCount);
				TiemposPruebas[k] = (endCount.QuadPart - startCount.QuadPart)/(double)freq.QuadPart;
			}

			break;

		case TipoOperacion::Division:

			for(unsigned int k=0; k<cantPruebas ;++k)
			{
			//inicializamos los operandos
			for(unsigned int i=0; i < iterations ;++i)
				data[i]   = Matrix4x4f(static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100),
									   static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100),
									   static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100),
									   static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100));

			//Comienza la medicion de tiempo
			LARGE_INTEGER startCount;
			LARGE_INTEGER endCount;
			QueryPerformanceCounter(&startCount);

			//COMIENZA LA PRUEBA!
			for(unsigned int i=0; i < (iterations-1) ;++i)
			{
				/*data[i]   = data[i]  /data[i+1];
				data[i+1] = data[i]  /data[i+1];
				data[i+2] = data[i+2]/data[i+3];
				data[i+3] = data[i+2]/data[i+3];
				data[i+4] = data[i+3]/data[i+4];*/
			}

			//Cacula el tiempo tardado
			QueryPerformanceCounter(&endCount);
			TiemposPruebas[k] = (endCount.QuadPart - startCount.QuadPart)/(double)freq.QuadPart;
			}

			break;

		case TipoOperacion::RaizCuadrada:

			for(unsigned int k=0; k<cantPruebas ;++k)
			{
				//inicializamos los operandos
				for(unsigned int i=0; i < iterations ;++i)
					data[i]   = Matrix4x4f(static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100),
										   static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100),
										   static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100),
										   static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100));

				//Comienza la medicion de tiempo
				LARGE_INTEGER startCount;
				LARGE_INTEGER endCount;
				QueryPerformanceCounter(&startCount);

				//COMIENZA LA PRUEBA!
				for(unsigned int i=0; i < (iterations-1) ;++i)
				{
					/*data[i]   = data[i].SquareLength();
					data[i+1] = data[i+1].SquareLength();
					data[i+2] = data[i+2].SquareLength();
					data[i+3] = data[i+3].SquareLength();
					data[i+4] = data[i+4].SquareLength();*/
				}

				//Cacula el tiempo tardado
				QueryPerformanceCounter(&endCount);
				TiemposPruebas[k] = (endCount.QuadPart - startCount.QuadPart)/(double)freq.QuadPart;
			}

			break;
	}

	#ifdef NOT_ALIGNED_MEMORY
		//delete[] data;
		_aligned_free(origData);
	#else
		_aligned_free(data);
	#endif
}


//Para comprobar las caracteristicas del procesador
#include "cpuid.h"

void expand(int avail, int mask)
{
    if (mask & _CPU_FEATURE_MMX) {
        printf("\t%s\t_CPU_FEATURE_MMX\n",
               avail & _CPU_FEATURE_MMX ? "yes" : "no");
    }
    if (mask & _CPU_FEATURE_SSE) {
        printf("\t%s\t_CPU_FEATURE_SSE\n",
               avail & _CPU_FEATURE_SSE ? "yes" : "no");
    }
    if (mask & _CPU_FEATURE_SSE2) {
        printf("\t%s\t_CPU_FEATURE_SSE2\n",
               avail & _CPU_FEATURE_SSE2 ? "yes" : "no");
    }
    if (mask & _CPU_FEATURE_3DNOW) {
        printf("\t%s\t_CPU_FEATURE_3DNOW\n",
               avail & _CPU_FEATURE_3DNOW ? "yes" : "no");
    }
}


HWND hWnd = NULL;
HINSTANCE hInstance = NULL;

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		{
		hdc = BeginPaint(hWnd, &ps);

		//CurrentPiece->Draw(hdc,CurrentPiece->GetX(),CurrentPiece->GetY());
		//H->HeapPrint(hdc);
		
		EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;

}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;//MAKEINTRESOURCE(IDC_TETRIS);
	wcex.lpszClassName	= "Renderer";
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND& HWnd)
{
   hWnd = CreateWindow("Renderer", "Render Example", WS_DLGFRAME | WS_SYSMENU,
      CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

#include "Renderer.h"

int main()
{
	int nCmdShow = SW_SHOW;
	hInstance = GetModuleHandle(NULL);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow, hWnd))
	{
		return FALSE;
	}

	MSG msg;

	//Vector4f v1(500, 500, 0, 1);
	//Vector4f v2(300, 600, 0, 1);
	//Vector4f v3(100, 100, 0, 1);

	/*
	//Right Bottom triangle
	Vector4f v1(800, 0, 0, 1);
	Vector4f v2(800, 600, 0, 1);
	Vector4f v3(0, 0, 0, 1);
	*/

	/*
	//Left Top triangle
	Vector4f v1(800, 600, 0, 1);
	Vector4f v2(0, 600, 0, 1);
	Vector4f v3(0, 0, 0, 1);
	*/

	int width = 800;
	int height = 600;

	SoftRenderer::Renderer MyRenderer(hWnd, width, height);
	MyRenderer.m_Light = Vector4f(0,0,1,0);
	MyRenderer.m_DCLight = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);//White ligth
	MyRenderer.m_ACLight = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	MyRenderer.m_SCLight = Vector4f(1.0f, 0.0f, 0.0f, 0.0f);//Red

	MyRenderer.m_ka = 0.1f;
	MyRenderer.m_kd = 0.6f;
	MyRenderer.m_ks = 0.3f;
	MyRenderer.m_q = 1;

	MyRenderer.m_ViewPos = Vector4f(0.0f, 0.0f, 1.0f, 0.0f);

	//SoftRenderer::Renderer::Vertex* pVertexBuffer = NULL;
	//unsigned int* pIndexBuffer = NULL;
	

	
	//MyRenderer.m_WorldT      = Matrix4x4f::Identity;
	Matrix4x4f trans;
	Matrix4x4f::CreateTranslation(trans, Vector4f(0,0,1.3f,0.0f));
	Matrix4x4f rotY;
	Matrix4x4f rotX;
	Matrix4x4f::CreateRotationAroundY(rotY, 0.0f);
	Matrix4x4f::CreateRotationAroundX(rotX, 0.0f);
	MyRenderer.m_WorldT      = trans*rotX*rotY;
	MyRenderer.m_CameraT     = Matrix4x4f::Identity;
	MyRenderer.m_ProjectionT = Matrix4x4f::CreatePerspectiveProjection(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 70.0f);
	MyRenderer.m_ViewportT   = Matrix4x4f::CreateViewport(width/2.0f, height/2.0f, width, height);

	//SoftRenderer::CircleMesh(MyRenderer.m_pVertexBuffer, MyRenderer.m_pIndexBuffer);
	//SoftRenderer::CubeMesh(MyRenderer.m_pVertexBuffer, MyRenderer.m_VBCount, MyRenderer.m_pIndexBuffer, MyRenderer.m_IBCount,
		//MyRenderer.m_Winding);
	//SoftRenderer::IcosahedronMesh(MyRenderer.m_pVertexBuffer, MyRenderer.m_VBCount, MyRenderer.m_pIndexBuffer, MyRenderer.m_IBCount,
		//MyRenderer.m_Winding);
	//SoftRenderer::CircleMesh(MyRenderer.m_pVertexBuffer, MyRenderer.m_VBCount, MyRenderer.m_pIndexBuffer, MyRenderer.m_IBCount,
		//	MyRenderer.m_Winding, 6);
	SoftRenderer::PlaneMesh(MyRenderer.m_pVertexBuffer, MyRenderer.m_VBCount, MyRenderer.m_pIndexBuffer, MyRenderer.m_IBCount,
			MyRenderer.m_Winding);

	SoftRenderer::Texture2D& rock = MyRenderer.CreateTexture2D("..\\Resources\\Chessboard_Texture_by_Sveinjo.png");
	//SoftRenderer::Texture2D& rock = MyRenderer.CreateTexture2D("..\\Resources\\cross.png");
	//SoftRenderer::Texture2D& rock = MyRenderer.CreateTexture2D("..\\Resources\\rock.bmp");
	//SoftRenderer::Texture2D& rock = MyRenderer.CreateTexture2D("..\\Resources\\Plane01DiffuseMap.png");
	//SoftRenderer::Texture2D& rock = MyRenderer.CreateTexture2D("..\\Resources\\grass.ppm");
	//SoftRenderer::Texture2D& rock = MyRenderer.CreateTexture2D("..\\Resources\\azul.png");
	//SoftRenderer::Texture2D& rock = MyRenderer.CreateTexture2D("..\\Resources\\coso.bmp");
	MyRenderer.m_CurrentTextureID = rock.GetIDTexture();

	/*
	SoftRenderer::Renderer::Vertex V[3];
	Vector4f v1(1, 1, 0, 1);
	Vector4f v2(-1, 1, 0, 1);
	Vector4f v3(-1, -1, 0, 1);

	V[0].Normal = Vector4f::Cross(v3-v2, v2-v1);
	V[0].Normal.Normalize();
	V[1].Normal = V[0].Normal;
	V[2].Normal = V[0].Normal;


	v1 = viewport*v1;
	Real _x = v1.GetX();
	Real _y = v1.GetY();
	Real _z = v1.GetZ();
	Real _w = v1.GetW();
	v2 = viewport*v2;
	_x = v2.GetX();
	_y = v2.GetY();
	_z = v2.GetZ();
	_w = v2.GetW();
	v3 = viewport*v3;
	_x = v3.GetX();
	_y = v3.GetY();
	_z = v3.GetZ();
	_w = v3.GetW();

	V[0].Position = v1;
	V[1].Position = v2;
	V[2].Position = v3;
	*/

	float dist = 1.5f;
	Matrix4x4f scale;
	// Main message loop:
	while(true)
	{
		if( PeekMessage(&msg,NULL,0,0,PM_REMOVE) )//Window message segment
		{
			if( msg.message == WM_QUIT )
				break;

			switch(msg.message)
			{
				case WM_KEYDOWN:
				{
					switch(msg.wParam)
					{
						case VK_UP:
							dist += 0.1f;
							if( dist > 15.0f )
								dist = 15.0f;
							break;
						case VK_DOWN:
							dist -= 0.1f;
							if( dist < 1.5f )
								dist = 1.5f;
							break;
					}
				}
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}else//Render segment
		{
			MyRenderer.Clear(SoftRenderer::Renderer::Black);

			Matrix4x4f::CreateTranslation(trans, Vector4f(0,0,dist,0.0f));

			static float radians = 0.0f;
			if( (radians - 3.1415f*2.0f) > 0.0f )
				radians -= 3.1415f*2.0f;
			else
				radians += 0.02f;

			//Matrix4x4f::CreateRotationAroundX(rotX, 3.1415f/4.0f);
			Matrix4x4f::CreateRotationAroundY(rotY, 3.1415f/3.0f);
			//Matrix4x4f::CreateScale(scale, 5, 5, 5);
			MyRenderer.m_WorldT      = trans*rotY;//*scale;

			//Matrix4x4f::CreateRotationAroundY(rotY, radians);
			//Matrix4x4f::CreateRotationAroundX(rotX, radians);
			//MyRenderer.m_WorldT      = trans*rotX*rotY;
			//Matrix4x4f::CreateRotationAroundY(rotY, 3.141516/2.0f);
			//MyRenderer.m_WorldT      = trans*rotY;
			MyRenderer.Render();

			MyRenderer.Present();
		}
	}

	return 0;

	/*
	Vector4f Vx = Vector4f(1.0f, 0.0f, 0.0f, 0.0f);
	Vector4f Vy = Vector4f(0.0f, 1.0f, 0.0f, 0.0f);
	Vector4f Vz = Vector4f::Cross(Vx,Vy);
	Real x = Vz.GetX();
	Real y = Vz.GetY();
	Real z = Vz.GetZ();
	Real w = Vz.GetW();*/
	/*float res = Vector4f::Dot(Vx,Vy);
	res = Vy.GetLength();
	res = Vy.GetSquareLength();
	Vy.Normalize();
	Real x = Vy.GetX();
	Real y = Vy.GetY();
	Real z = Vy.GetZ();
	Real w = Vy.GetW();*/

	Matrix4x4f mat(2,0,0,0,0,2,0,0,0,0,2,0,0,0,0,2);
	
	Matrix4x4f mat2 = mat;
	Matrix4x4f mat3 = mat*mat2;
	float c11 = mat3.GetC11();
	float c21 = mat3.GetC21();
	float c31 = mat3.GetC31();
	float c41 = mat3.GetC41();
	float c12 = mat3.GetC12();
	float c22 = mat3.GetC22();
	float c32 = mat3.GetC32();
	float c42 = mat3.GetC42();
	float c13 = mat3.GetC13();
	float c23 = mat3.GetC23();
	float c33 = mat3.GetC33();
	float c43 = mat3.GetC43();
	float c14 = mat3.GetC14();
	float c24 = mat3.GetC24();
	float c34 = mat3.GetC34();
	float c44 = mat3.GetC44();

	//Matrix4x4f proj(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
	//Matrix4x4f proj = Matrix4x4f::CreatePerspectiveProjection(3.1415/2.0f, 800.0f/600.0f,/* -400.0f, 400.0f, 300.0f, -300.0f,*/ 1.0f, 100.0f);
	Matrix4x4f proj = Matrix4x4f::CreatePerspectiveProjection(-400.0f, 400.0f, -300.0f, 300.0f, 1.0f, 100.0f);
	Vector4f uno(400.0f,300.0f,1.0f,1.0f);
	Vector4f res = proj*uno;
	Real x = res.GetX()/res.GetW();
	Real y = res.GetY()/res.GetW();
	Real z = res.GetZ()/res.GetW();
	Real w = res.GetW()/res.GetW();
	
	float n=1.0f;
	float X = 400.0f;
	float Z = 1.0f;
	float r = 400.0f;
	float l = -400.0f;
	float Xpri = ( -2*n*X/(r-l) + (r+l)/(r-l)*Z ) / (-Z);
	float Xpri2 = ( 2*X/(r-l) + ((r+l)*Z)/((r-l)*n) ) * (n/Z);
	

	std::cerr<<"Comprobando caracteristicas del procesador:"<<std::endl<<std::endl;
	_p_info info;

	_cpuid(&info);

	printf("v_name:\t\t%s\n", info.v_name);
	printf("model:\t\t%s\n", info.model_name);
	printf("family:\t\t%d\n", info.family);
	printf("model:\t\t%d\n", info.model);
	printf("stepping:\t%d\n", info.stepping);
	printf("feature:\t%08x\n", info.feature);
	expand(info.feature, info.checks);
	printf("os_support:\t%08x\n", info.os_support);
	expand(info.os_support, info.checks);
	printf("checks:\t\t%08x\n", info.checks);


	std::cerr<<std::endl;

	#ifdef SIMD_EXTENSION
		if (info.checks & _CPU_FEATURE_SSE) 
			if( (info.feature & _CPU_FEATURE_SSE) == false )
			{
				printf("Su procesador no soporta SSE que es requisito para esta prueba!");
				system("pause");
				exit(0);
			}
	#endif

	std::cerr<<"--------------------------------------------------"<<std::endl;	
	std::cerr<<std::endl;

	std::cerr<<"Inicializando la prueba"<<std::endl<<std::endl;

	std::cerr<<"--------------------------------------------------"<<std::endl;
	#ifdef SIMD_EXTENSION
		std::cerr<<"USANDO extensiones SIMD"<<std::endl;
	#else
		std::cerr<<"SIN usar extensiones SIMD"<<std::endl;
	#endif
	std::cerr<<"--------------------------------------------------"<<std::endl<<std::endl;

	double* TiemposPruebas = new double[cantPruebas];

	//TestVector4(TiemposPruebas);
	TestMatrix4x4(TiemposPruebas);

	double promedioTiempoPrueba = 0;
	for(unsigned int k=0; k<cantPruebas ;++k)
		promedioTiempoPrueba += TiemposPruebas[k];
	promedioTiempoPrueba /= cantPruebas;

	std::cout<<"El tiempo promedio de calculo fue de: "<<promedioTiempoPrueba<<" [s]"<<std::endl<<std::endl<<std::endl;

	double varianzaTiempoPrueba = 0;
	for(unsigned int k=0; k<cantPruebas ;++k)
	{
		double aux = (TiemposPruebas[k]-promedioTiempoPrueba);
		varianzaTiempoPrueba += aux*aux;
	}
	varianzaTiempoPrueba /= cantPruebas;

	std::cout<<"La desviasion estandar del tiempo de calculo fue de: "<<sqrt(varianzaTiempoPrueba)<<" [s]"<<std::endl<<std::endl;
	delete[] TiemposPruebas;

	system("pause");

	return 0;
}

#pragma region Prueba de intrinsics directa

//int main()
//{
//
//	std::cerr<<"Comprobando caracteristicas del procesador:"<<std::endl<<std::endl;
//	_p_info info;
//
//	_cpuid(&info);
//
//	printf("v_name:\t\t%s\n", info.v_name);
//	printf("model:\t\t%s\n", info.model_name);
//	printf("family:\t\t%d\n", info.family);
//	printf("model:\t\t%d\n", info.model);
//	printf("stepping:\t%d\n", info.stepping);
//	printf("feature:\t%08x\n", info.feature);
//	expand(info.feature, info.checks);
//	printf("os_support:\t%08x\n", info.os_support);
//	expand(info.os_support, info.checks);
//	printf("checks:\t\t%08x\n", info.checks);
//
//
//	std::cerr<<std::endl;
//
//	#ifdef SIMD_EXTENSION
//		if (info.checks & _CPU_FEATURE_SSE) 
//			if( (info.feature & _CPU_FEATURE_SSE) == false )
//			{
//				printf("Su procesador no soporta SSE que es requisito para esta prueba!");
//				system("pause");
//				exit(0);
//			}
//	#endif
//
//	std::cerr<<"--------------------------------------------------"<<std::endl;	
//	std::cerr<<std::endl;
//
//	std::cerr<<"Inicializando la prueba"<<std::endl<<std::endl;
//
//	std::cerr<<"--------------------------------------------------"<<std::endl;
//	#ifdef SIMD_EXTENSION
//		std::cerr<<"USANDO extensiones SIMD"<<std::endl;
//	#else
//		std::cerr<<"SIN usar extensiones SIMD"<<std::endl;
//	#endif
//	std::cerr<<"--------------------------------------------------"<<std::endl<<std::endl;
//	
//	#ifdef NOT_ALIGNED_MEMORY
//		//Vector4* data = new Vector4[iterations];
//		__m128* origData = (__m128*)_aligned_malloc(iterations*sizeof(__m128)+1, __alignof(__m128));
//		__m128* data     = (__m128*) (((char*)origData)+2);
//	#else
//		__m128* data = (__m128*)_aligned_malloc(iterations*sizeof(__m128), __alignof(__m128));
//		if( data == NULL )
//		{
//			std::cerr<<"No hay suficiente memoria para correr la prueba!!!"<<std::endl<<std::endl;
//			exit(0);
//		}
//	#endif
//
//
//	std::cerr<<"--------------------------------------------------"<<std::endl;
//	std::cerr<<"Alineacion:"<<std::endl;
//	//Siempre deberia decir que la memoria esta alineada pero como esto puede afectar mucho el rendimiento lo 
//	//controlamos
//	if (((int)data % __alignof(__m128) ) == 0)
//        printf_s( "El puntero: %d  --- esta alineado a %d bytes\n", data, __alignof(__m128));
//    else
//        printf_s( "El puntero: %d  --- no esta esta alineado a %d bytes\n", data, __alignof(__m128));
//	std::cerr<<"--------------------------------------------------"<<std::endl<<std::endl;
//		
//	srand(static_cast<unsigned int>(time(NULL)));
//
//	//La frequencia del contador de alto rendiemiento no varia
//	LARGE_INTEGER freq;
//	QueryPerformanceFrequency(&freq);
//
//	double* TiemposPruebas = new double[cantPruebas];
//
//	std::cerr<<"--------------------------------------------------"<<std::endl;
//	std::cerr<<"La operacion a probar es: ";
//	switch( op )
//	{
//		case TipoOperacion::Mix:
//			std::cerr<<"Mix"<<std::endl;
//			break;
//		case TipoOperacion::Suma:
//			std::cerr<<"Suma"<<std::endl;
//			break;
//		case TipoOperacion::Resta:
//			std::cerr<<"Resta"<<std::endl;
//			break;
//		case TipoOperacion::Multiplicacion:
//			std::cerr<<"Multiplicacion"<<std::endl;
//			break;
//		case TipoOperacion::Division:
//			std::cerr<<"Division"<<std::endl;
//			break;
//		case TipoOperacion::RaizCuadrada:
//			std::cerr<<"RaizCuadrada"<<std::endl;
//			break;
//		case TipoOperacion::Dot:
//			std::cerr<<"Dot"<<std::endl;
//			break;
//		default:
//			std::cerr<<"OPERACION NO RECONOCIDA!!! SALIENDO"<<std::endl;
//			exit(0);
//			break;
//	}
//	std::cerr<<"--------------------------------------------------"<<std::endl<<std::endl;
//
//	std::cerr<<std::endl<<std::endl<<"Comienza la prueba"<<std::endl;
//
//	//Por cada tipo de operacion
//	switch( op )
//	{
//		case TipoOperacion::Mix:
//
//			for(unsigned int k=0; k<cantPruebas ;++k)
//			{
//				//inicializamos los operandos
//				for(unsigned int i=0; i < iterations ;++i)
//					data[i]   = _mm_set_ps(static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100));
//
//				//Comienza la medicion de tiempo
//				LARGE_INTEGER startCount;
//				LARGE_INTEGER endCount;
//				QueryPerformanceCounter(&startCount);
//
//				//COMIENZA LA PRUEBA!
//				for(unsigned int i=0; i < (iterations-1) ;++i)
//				{
//					//data[i]   = data[i] +data[i+1];
//					//data[i+1] = data[i] -data[i+1];
//					//data[i+2] = data[i+2]*data[i+3];
//					//data[i+3] = data[i+2]/data[i+3];
//					//data[i+4] = data[i+3].SquareLength();
//				}
//
//				//Cacula el tiempo tardado
//				QueryPerformanceCounter(&endCount);
//				TiemposPruebas[k] = (endCount.QuadPart - startCount.QuadPart)/(double)freq.QuadPart;
//			}
//
//		break;
//
//		case TipoOperacion::Suma:
//
//			for(unsigned int k=0; k<cantPruebas ;++k)
//			{
//				//inicializamos los operandos
//				for(unsigned int i=0; i < iterations ;++i)
//					data[i]   = _mm_set_ps(static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100));
//
//				//Comienza la medicion de tiempo
//				LARGE_INTEGER startCount;
//				LARGE_INTEGER endCount;
//				QueryPerformanceCounter(&startCount);
//
//				//COMIENZA LA PRUEBA!
//				for(unsigned int i=0; i < (iterations-1) ;++i)
//				{
//					//data[i]   = data[i]  +data[i+1];//15 operaciones vectoriales para sacarle el jugo a la vel de 
//					//data[i]   = data[i]  +data[i+1];//SSE y que no tengamos el cuello de botella en el for
//					//data[i]   = data[i]  +data[i+1];//si hace un for unroll.. no deberia dar problemas pero por las dudas
//					//data[i]   = data[i]  +data[i+1];//es mejor muchas instruciones sin ninguna salto
//					//data[i]   = data[i]  +data[i+1];
//					//data[i]   = data[i]  +data[i+1];
//					//data[i]   = data[i]  +data[i+1];
//					//data[i]   = data[i]  +data[i+1];
//					//data[i]   = data[i]  +data[i+1];
//					//data[i]   = data[i]  +data[i+1];
//					//data[i]   = data[i]  +data[i+1];
//					//data[i]   = data[i]  +data[i+1];
//					//data[i]   = data[i]  +data[i+1];
//					//data[i]   = data[i]  +data[i+1];
//					//data[i]   = data[i]  +data[i+1];
//				}
//
//				//Cacula el tiempo tardado
//				QueryPerformanceCounter(&endCount);
//				TiemposPruebas[k] = (endCount.QuadPart - startCount.QuadPart)/(double)freq.QuadPart;
//			}
//
//			break;
//
//		case TipoOperacion::Resta:
//
//			for(unsigned int k=0; k<cantPruebas ;++k)
//			{
//				//inicializamos los operandos
//				for(unsigned int i=0; i < iterations ;++i)
//					data[i]   = _mm_set_ps(static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100));
//
//				//Comienza la medicion de tiempo
//				LARGE_INTEGER startCount;
//				LARGE_INTEGER endCount;
//				QueryPerformanceCounter(&startCount);
//
//				//COMIENZA LA PRUEBA!
//				for(unsigned int i=0; i < (iterations-1) ;++i)
//				{
//					//data[i]   = data[i]  -data[i+1];//15 operaciones vectoriales para sacarle el jugo a la vel de 
//					//data[i]   = data[i]  -data[i+1];//SSE y que no tengamos el cuello de botella en el for
//					//data[i]   = data[i]  -data[i+1];//si hace un for unroll.. no deberia dar problemas pero por las dudas
//					//data[i]   = data[i]  -data[i+1];//es mejor muchas instruciones sin ninguna salto
//					//data[i]   = data[i]  -data[i+1];
//					//data[i]   = data[i]  -data[i+1];
//					//data[i]   = data[i]  -data[i+1];
//					//data[i]   = data[i]  -data[i+1];
//					//data[i]   = data[i]  -data[i+1];
//					//data[i]   = data[i]  -data[i+1];
//					//data[i]   = data[i]  -data[i+1];
//					//data[i]   = data[i]  -data[i+1];
//					//data[i]   = data[i]  -data[i+1];
//					//data[i]   = data[i]  -data[i+1];
//					//data[i]   = data[i]  -data[i+1];
//				}
//
//				//Cacula el tiempo tardado
//				QueryPerformanceCounter(&endCount);
//				TiemposPruebas[k] = (endCount.QuadPart - startCount.QuadPart)/(double)freq.QuadPart;
//			}
//
//			break;
//
//		case TipoOperacion::Multiplicacion:
//
//			for(unsigned int k=0; k<cantPruebas ;++k)
//			{
//				//inicializamos los operandos
//				for(unsigned int i=0; i < iterations ;++i)
//					data[i]   = _mm_set_ps(static_cast<float>(rand()%10), static_cast<float>(rand()%10), static_cast<float>(rand()%10), static_cast<float>(rand()%10));
//
//				//Comienza la medicion de tiempo
//				LARGE_INTEGER startCount;
//				LARGE_INTEGER endCount;
//				QueryPerformanceCounter(&startCount);
//
//				//COMIENZA LA PRUEBA!
//				for(unsigned int i=7; i < (iterations-10) ;++i)//los limites son para operar de forma de tener variedad
//				{											   //pero no llegar a tener +inf que es muy lento en sus operaciones de fpu
//					
//					//Prueba linea
//					/*
//					data[i]   = _mm_mul_ps( data[i+10], _mm_mul_ps(data[i+10], _mm_mul_ps(data[i+1], data[i+1])));//15 operaciones vectoriales para sacarle el jugo a la vel de 
//					data[i-1] = _mm_mul_ps( data[i+9], _mm_mul_ps(data[i+9], _mm_mul_ps(data[i+2], data[i+2])));//SSE y que no tengamos el cuello de botella en el for
//					data[i]   = _mm_mul_ps( data[i+8], _mm_mul_ps(data[i+8], _mm_mul_ps(data[i+3], data[i+3])));//si hace un for unroll.. no deberia dar problemas pero por las dudas
//					data[i-2] = _mm_mul_ps( data[i+7], _mm_mul_ps(data[i+7], _mm_mul_ps(data[i+4], data[i+4])));//es mejor muchas instruciones sin ninguna salto
//					data[i]   = _mm_mul_ps( data[i+6], _mm_mul_ps(data[i+6], _mm_mul_ps(data[i+5], data[i+5])));
//					data[i-3] = _mm_mul_ps( data[i+5], _mm_mul_ps(data[i+5], _mm_mul_ps(data[i+6], data[i+6])));
//					data[i]   = _mm_mul_ps( data[i+4], _mm_mul_ps(data[i+4], _mm_mul_ps(data[i+7], data[i+7])));
//					data[i-4] = _mm_mul_ps( data[i+3], _mm_mul_ps(data[i+3], _mm_mul_ps(data[i+8], data[i+8])));
//					data[i]   = _mm_mul_ps( data[i+2], _mm_mul_ps(data[i+2], _mm_mul_ps(data[i+9], data[i+9])));
//					data[i-5] = _mm_mul_ps( data[i+1], _mm_mul_ps(data[i+1], _mm_mul_ps(data[i+10], data[i+10])));
//					data[i]   = _mm_mul_ps( data[i+2], _mm_mul_ps(data[i+2], _mm_mul_ps(data[i+1], data[i+1])));
//					data[i-6] = _mm_mul_ps( data[i+3], _mm_mul_ps(data[i+3], _mm_mul_ps(data[i+2], data[i+2])));
//					data[i]   = _mm_mul_ps( data[i+4], _mm_mul_ps(data[i+4], _mm_mul_ps(data[i+3], data[i+3])));
//					data[i-7] = _mm_mul_ps( data[i+5], _mm_mul_ps(data[i+5], _mm_mul_ps(data[i+4], data[i+4])));
//					data[i]   = _mm_mul_ps( data[i+6], _mm_mul_ps(data[i+6], _mm_mul_ps(data[i+5], data[i+5])));
//					*/
//
//					//Prueba linea auxiliares peor
//					
//					__m128 aux1 = _mm_mul_ps(data[i+1], data[i+1]);
//					__m128 jux1 = _mm_mul_ps(data[i+10], aux1);
//					data[i]     = _mm_mul_ps(data[i+10], jux1);//15 operaciones vectoriales para sacarle el jugo a la vel de 
//					__m128 aux2 = _mm_mul_ps(data[i+2], data[i+2]);
//					__m128 jux2 = _mm_mul_ps(data[i+9], aux2);
//					data[i-1] = _mm_mul_ps(data[i+9], jux2);//SSE y que no tengamos el cuello de botella en el for
//					__m128 aux3 = _mm_mul_ps(data[i+3], data[i+3]);
//					__m128 jux3 = _mm_mul_ps(data[i+8], aux3);
//					data[i]   = _mm_mul_ps(data[i+8], jux3);//si hace un for unroll.. no deberia dar problemas pero por las dudas
//					__m128 aux4 = _mm_mul_ps(data[i+4], data[i+4]);
//					__m128 jux4 = _mm_mul_ps(data[i+7], aux4);
//					data[i-2] = _mm_mul_ps(data[i+7], jux4);//es mejor muchas instruciones sin ninguna salto
//					__m128 aux5 = _mm_mul_ps(data[i+5], data[i+5]);
//					__m128 jux5 = _mm_mul_ps(data[i+6], aux5);
//					data[i]   = _mm_mul_ps(data[i+6], jux5);
//					__m128 aux6 = _mm_mul_ps(data[i+6], data[i+6]);
//					__m128 jux6 = _mm_mul_ps(data[i+5], aux6);
//					data[i-3] = _mm_mul_ps(data[i+5], jux6);
//					__m128 aux7 = _mm_mul_ps(data[i+7], data[i+7]);
//					__m128 jux7 = _mm_mul_ps(data[i+4], aux7);
//					data[i]   = _mm_mul_ps(data[i+4], jux7);
//					__m128 aux8 = _mm_mul_ps(data[i+8], data[i+8]);
//					__m128 jux8 = _mm_mul_ps(data[i+3], aux8);
//					data[i-4] = _mm_mul_ps(data[i+3], jux8);
//					__m128 aux9 = _mm_mul_ps(data[i+9], data[i+9]);
//					__m128 jux9 = _mm_mul_ps(data[i+2], aux9);
//					data[i]   = _mm_mul_ps(data[i+2], jux9);
//					__m128 aux10 = _mm_mul_ps(data[i+10], data[i+10]);
//					__m128 jux10 = _mm_mul_ps(data[i+1], aux10);
//					data[i-5] = _mm_mul_ps(data[i+1], jux10);
//					__m128 aux11 = _mm_mul_ps(data[i+1], data[i+1]);
//					__m128 jux11 = _mm_mul_ps(data[i+2], aux11);
//					data[i]   = _mm_mul_ps(data[i+2], jux11);
//					__m128 aux12 = _mm_mul_ps(data[i+2], data[i+2]);
//					__m128 jux12 = _mm_mul_ps(data[i+3], aux12);
//					data[i-6] = _mm_mul_ps(data[i+3], jux12);
//					__m128 aux13 = _mm_mul_ps(data[i+3], data[i+3]);
//					__m128 jux13 = _mm_mul_ps(data[i+4], aux13);
//					data[i]   = _mm_mul_ps(data[i+4], jux13);
//					__m128 aux14 = _mm_mul_ps(data[i+4], data[i+4]);
//					__m128 jux14 = _mm_mul_ps(data[i+5], aux14);
//					data[i-7] = _mm_mul_ps(data[i+5], jux14);
//					__m128 aux15 = _mm_mul_ps(data[i+5], data[i+5]);
//					__m128 jux15 = _mm_mul_ps(data[i+6], aux15);
//					data[i]   = _mm_mul_ps(data[i+6], jux15);
//					
//
//					//assert( data[i] == data[i] );//si es distinto es infinito o nan
//				}
//
//				//Cacula el tiempo tardado
//				QueryPerformanceCounter(&endCount);
//				TiemposPruebas[k] = (endCount.QuadPart - startCount.QuadPart)/(double)freq.QuadPart;
//			}
//
//			break;
//
//		case TipoOperacion::Division:
//
//			for(unsigned int k=0; k<cantPruebas ;++k)
//			{
//			//inicializamos los operandos
//			for(unsigned int i=0; i < iterations ;++i)
//				data[i]   = _mm_set_ps(static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100));
//
//			//Comienza la medicion de tiempo
//			LARGE_INTEGER startCount;
//			LARGE_INTEGER endCount;
//			QueryPerformanceCounter(&startCount);
//
//			//COMIENZA LA PRUEBA!
//			for(unsigned int i=0; i < (iterations-1) ;++i)
//			{
//				/*data[i]   = data[i]  /data[i+1];
//				data[i+1] = data[i]  /data[i+1];
//				data[i+2] = data[i+2]/data[i+3];
//				data[i+3] = data[i+2]/data[i+3];
//				data[i+4] = data[i+3]/data[i+4];*/
//			}
//
//			//Cacula el tiempo tardado
//			QueryPerformanceCounter(&endCount);
//			TiemposPruebas[k] = (endCount.QuadPart - startCount.QuadPart)/(double)freq.QuadPart;
//			}
//
//			break;
//
//		case TipoOperacion::RaizCuadrada:
//
//			for(unsigned int k=0; k<cantPruebas ;++k)
//			{
//				//inicializamos los operandos
//				for(unsigned int i=0; i < iterations ;++i)
//					data[i]   = _mm_set_ps(static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100));
//
//				//Comienza la medicion de tiempo
//				LARGE_INTEGER startCount;
//				LARGE_INTEGER endCount;
//				QueryPerformanceCounter(&startCount);
//
//				//COMIENZA LA PRUEBA!
//				for(unsigned int i=0; i < (iterations-1) ;++i)
//				{
//					/*data[i]   = data[i].SquareLength();
//					data[i+1] = data[i+1].SquareLength();
//					data[i+2] = data[i+2].SquareLength();
//					data[i+3] = data[i+3].SquareLength();
//					data[i+4] = data[i+4].SquareLength();*/
//				}
//
//				//Cacula el tiempo tardado
//				QueryPerformanceCounter(&endCount);
//				TiemposPruebas[k] = (endCount.QuadPart - startCount.QuadPart)/(double)freq.QuadPart;
//			}
//
//			break;
//
//		case TipoOperacion::Dot:
//
//			for(unsigned int k=0; k<cantPruebas ;++k)
//			{
//				//inicializamos los operandos
//				for(unsigned int i=0; i < iterations ;++i)
//					data[i]   = _mm_set_ps(static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100), static_cast<float>(rand()%100));
//
//				//Comienza la medicion de tiempo
//				LARGE_INTEGER startCount;
//				LARGE_INTEGER endCount;
//				QueryPerformanceCounter(&startCount);
//
//				//COMIENZA LA PRUEBA!
//				for(unsigned int i=0; i < (iterations-1) ;++i)
//				{
//					/*data[i].m_x   = data[i].Dot(  data[i+1]);
//					data[i+1].m_x = data[i].Dot(  data[i+1]);
//					data[i+2].m_x = data[i+2].Dot(data[i+3]);
//					data[i+3].m_x = data[i+2].Dot(data[i+3]);
//					data[i+4].m_x = data[i+3].Dot(data[i+4]);*/
//				}
//
//				//Cacula el tiempo tardado
//				QueryPerformanceCounter(&endCount);
//				TiemposPruebas[k] = (endCount.QuadPart - startCount.QuadPart)/(double)freq.QuadPart;
//			}
//
//			break;
//
//	}
//
//	#ifdef NOT_ALIGNED_MEMORY
//		//delete[] data;
//		_aligned_free(origData);
//	#else
//		_aligned_free(data);
//	#endif
//		
//	double promedioTiempoPrueba = 0;
//	for(unsigned int k=0; k<cantPruebas ;++k)
//		promedioTiempoPrueba += TiemposPruebas[k];
//	promedioTiempoPrueba /= cantPruebas;
//
//	std::cout<<"El tiempo promedio de calculo fue de: "<<promedioTiempoPrueba<<" [s]"<<std::endl<<std::endl<<std::endl;
//
//	double varianzaTiempoPrueba = 0;
//	for(unsigned int k=0; k<cantPruebas ;++k)
//	{
//		double aux = (TiemposPruebas[k]-promedioTiempoPrueba);
//		varianzaTiempoPrueba += aux*aux;
//	}
//	varianzaTiempoPrueba /= cantPruebas;
//
//	std::cout<<"La desviasion estandar del tiempo de calculo fue de: "<<sqrt(varianzaTiempoPrueba)<<" [s]"<<std::endl<<std::endl;
//	delete[] TiemposPruebas;
//
//	system("pause");
//
//	return 0;
//}

#pragma endregion