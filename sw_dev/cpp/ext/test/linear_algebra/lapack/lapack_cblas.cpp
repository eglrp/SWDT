#if defined(__cplusplus)
extern "C" {
#endif

//#define NO_BLAS_WRAP
#include <clapack/f2c.h>
//#include <clapack/blaswrap.h>
#include <clapack/clapack.h>

#if defined(abs)
#undef abs
#endif

#if defined(__cplusplus)
}
#endif

#include <iostream>


namespace {
namespace local {

}  // namespace local
}  // unnamed namespace

namespace lapack {

void cblas_main()
{
	const double m[] = {
		3, 1, 3,
		1, 5, 9,
		2, 6, 5
	};
	const double x[] = { -1, -1, 1 };
	double y[] = { 0, 0, 0 };

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
			std::cout << m[i*3 + j] << " ";
		std::cout << std::endl;
	}

#if 0
	cblas_dgemv(CblasRowMajor, CblasNoTrans, 3, 3, 1.0, m, 3, x, 1, 0.0, y, 1);
#else
	long rdim = 3, cdim = 3;
	double alpha = 1.0, beta = 0.0;
	long lda = rdim;
	long incx = 1, incy = 1;
	dgemv_("No transpose", &rdim, &cdim, &alpha, (double *)m, &lda, (double *)x, &incx, &beta, y, &incy);
#endif

	for (int i = 0; i < 3; ++i)
		std::cout << y[i] << std::endl;
}

}  // namespace lapack