#if defined(WIN32) || defined(_WIN32)
#include <vld/vld.h>
#endif
#include <iostream>
#include <stdexcept>
#include <cstdlib>


int main(int argc, char* argv[])
{
	int ftensor_main(int argc, char* argv[]);
	int ltensor_main(int argc, char* argv[]);

	int retval = EXIT_SUCCESS;
	try
	{
		std::cout << "FTensor library -----------------------------------------------------" << std::endl;
		//retval = ftensor_main(argc, argv);  // not yet implemented.
		std::cout << "\nltensor library -----------------------------------------------------" << std::endl;
		retval = ltensor_main(argc, argv);
	}
    catch (const std::bad_alloc &e)
	{
		std::cout << "std::bad_alloc caught: " << e.what() << std::endl;
		retval = EXIT_FAILURE;
	}
	catch (const std::exception &e)
	{
		std::cout << "std::exception caught: " << e.what() << std::endl;
		retval = EXIT_FAILURE;
	}
	catch (...)
	{
		std::cout << "unknown exception caught" << std::endl;
		retval = EXIT_FAILURE;
	}

	std::cout << "press any key to exit ..." << std::endl;
	std::cin.get();

	return retval;
}
