//#include "stdafx.h"
#include <shogun/io/SGIO.h>
#include <shogun/lib/ShogunException.h>
#include <shogun/labels/MulticlassLabels.h>
#include <shogun/kernel/CustomKernel.h>
#include <shogun/kernel/CombinedKernel.h>
#include <shogun/classifier/mkl/MKLMulticlass.h>
#include <iostream>
#include <vector>
#include <cmath>


namespace {
namespace local {

void getgauss(float64_t &y1, float64_t &y2)
{
	float x1, x2, w;

	do
	{
		x1 = 2.0 * std::rand() / (float64_t)RAND_MAX - 1.0;
		x2 = 2.0 * std::rand() / (float64_t)RAND_MAX - 1.0;
		w = x1 * x1 + x2 * x2;
	} while ((w >= 1.0)|| (w < 1e-9));

	w = std::sqrt((-2.0 * std::log(w)) / w);
	y1 = x1 * w;
	y2 = x2 * w;
}

void gendata(std::vector<float64_t> &x, std::vector<float64_t> &y, CMulticlassLabels *&lab)
{
	const int32_t totalsize = 240;
	const int32_t class1size = 80;
	const int32_t class2size = 70;

	//generating three class data set
	x.resize(totalsize);
	y.resize(totalsize);
	for (size_t i = 0; i < x.size(); ++i)
		getgauss(x[i], y[i]);

	for (size_t i = 0; i < x.size(); ++i)
	{
		if ((int32_t)i < class1size)
		{
			x[i] += 0;
			y[i] += 0;
		}
		else if ((int32_t)i < class1size + class2size)
		{
			x[i] += +1;
			y[i] += -1;
		}
		else
		{
			x[i] += -1;
			y[i] += +1;
		}
	}

	// set labels
	lab = new shogun::CMulticlassLabels(x.size());
	for (size_t i = 0; i < x.size(); ++i)
	{
		if ((int32_t)i < class1size)
			lab->set_int_label(i, 0);
		else if ((int32_t)i< class1size + class2size)
			lab->set_int_label(i, 1);
		else
			lab->set_int_label(i, 2);
	}
}

void gentrainkernel(float64_t *&ker1, float64_t *&ker2, float64_t *&ker3, float64_t &autosigma, float64_t &n1, float64_t &n2, float64_t &n3, const std::vector<float64_t> &x, const std::vector<float64_t> &y)
{
	autosigma = 0;

	for (size_t l = 0; l < x.size(); ++l)
	{
		for (size_t r = 0; r <= l; ++r)
		{
			const float64_t dist = ((x[l] - x[r]) * (x[l] - x[r]) + (y[l] - y[r]) * (y[l] - y[r]));
			autosigma += dist * 2.0 / (float64_t)x.size() / ((float64_t)x.size() + 1);
		}
	}

	float64_t fm1 = 0, mean1 = 0, fm2 = 0, mean2 = 0, fm3 = 0, mean3 = 0;

	ker1 = SG_MALLOC(float64_t, x.size() * x.size());
	ker2 = SG_MALLOC(float64_t, x.size() * x.size());
	ker3 = SG_MALLOC(float64_t, x.size() * x.size());


	for (size_t l = 0; l < x.size(); ++l)
	{
		for (size_t r = 0; r < x.size(); ++r)
		{
			const float64_t dist = ((x[l] - x[r]) * (x[l] - x[r]) + (y[l] - y[r]) * (y[l] - y[r]));

			ker1[l + r * x.size()] = std::exp(-dist / autosigma / autosigma);
			//ker2[l + r * x.size()] = std::exp(-dist/ sigma2 / sigma2);
			ker2[l + r * x.size()]= x[l] * x[r] + y[l] * y[r];

			ker3[l + r * x.size()]= (x[l] * x[r] + y[l] * y[r] + 1) * (x[l] * x[r] + y[l] * y[r] + 1);

			fm1 += ker1[l + r * x.size()] / (float64_t)x.size() / ((float64_t)x.size());
			fm2 += ker2[l + r * x.size()] / (float64_t)x.size() / ((float64_t)x.size());
			fm3 += ker3[l + r * x.size()] / (float64_t)x.size() / ((float64_t)x.size());

			if (l == r)
			{
				mean1 += ker1[l + r * x.size()] / (float64_t)x.size();
				mean2 += ker2[l + r * x.size()] / (float64_t)x.size();
				mean3 += ker3[l + r * x.size()] / (float64_t)x.size();
			}
		}
	}

	n1 = (mean1 - fm1);
	n2 = (mean2 - fm2);
	n3 = (mean3 - fm3);

	for (size_t l = 0; l < x.size(); ++l)
	{
		for (size_t r = 0; r < x.size(); ++r)
		{
			ker1[l + r * x.size()] = ker1[l + r * x.size()] / n1;
			ker2[l + r * x.size()] = ker2[l + r * x.size()] / n2;
			ker3[l + r * x.size()] = ker3[l + r * x.size()] / n3;
		}
	}
}

void gentestkernel(float64_t *&ker1, float64_t *&ker2, float64_t *&ker3,
	const float64_t autosigma, const float64_t n1, const float64_t n2, const float64_t n3,
	const std::vector<float64_t> &x,const std::vector<float64_t> &y,
	const std::vector<float64_t> &tx,const std::vector<float64_t> &ty
)
{
	ker1 = SG_MALLOC(float64_t, x.size() * tx.size());
	ker2 = SG_MALLOC(float64_t, x.size() * tx.size());
	ker3 = SG_MALLOC(float64_t, x.size() * tx.size());

	for (size_t l = 0; l < x.size(); ++l)
	{
		for (size_t r = 0; r < tx.size(); ++r)
		{
			const float64_t dist = ((x[l] - tx[r]) * (x[l] - tx[r]) + (y[l] - ty[r]) * (y[l] - ty[r]));

			ker1[l + r * x.size()] = std::exp(-dist / autosigma / autosigma);
			ker2[l + r * x.size()] = x[l] * tx[r] + y[l] * ty[r];
			ker3[l + r * x.size()] = (x[l] * tx[r] + y[l] * ty[r] + 1) * (x[l] * tx[r] + y[l] * ty[r] + 1);
		}
	}

	for (size_t l = 0; l < x.size(); ++l)
	{
		for (size_t r = 0; r < tx.size(); ++r)
		{
			ker1[l + r * x.size()] = ker1[l + r * x.size()] / n1;
			ker2[l + r * x.size()] = ker2[l + r * x.size()] / n2;
			ker3[l + r * x.size()] = ker3[l + r * x.size()] / n2;
		}
	}
}

}  // namespace local
}  // unnamed namespace

namespace my_shogun {

using namespace shogun;

// [ref] ${SHOGUN_HOME}/examples/undocumented/libshogun/classifier_mklmulticlass.cpp
void classifier_mklmulticlass_example()
{
	shogun::CMulticlassLabels *lab = NULL;
	std::vector<float64_t> x, y;

	local::gendata(x, y, lab);
	SG_REF(lab);

	float64_t *ker1 = NULL;
	float64_t *ker2 = NULL;
	float64_t *ker3 = NULL;
	const float64_t autosigma = 1;
	float64_t n1 = 0;
	float64_t n2 = 0;
	float64_t n3 = 0;

	int32_t numdata = 0;
	local::gentrainkernel(ker1, ker2, ker3, autosigma, n1, n2, n3, x, y);
	numdata = x.size();

	shogun::CCombinedKernel *ker = new shogun::CCombinedKernel();

	shogun::CCustomKernel *kernel1 = new shogun::CCustomKernel();
	shogun::CCustomKernel *kernel2 = new shogun::CCustomKernel();
	shogun::CCustomKernel *kernel3 = new shogun::CCustomKernel();

	kernel1->set_full_kernel_matrix_from_full(shogun::SGMatrix<float64_t>(ker1, numdata, numdata, false));
	kernel2->set_full_kernel_matrix_from_full(shogun::SGMatrix<float64_t>(ker2, numdata, numdata, false));
	kernel3->set_full_kernel_matrix_from_full(shogun::SGMatrix<float64_t>(ker3, numdata, numdata, false));

	SG_FREE(ker1);
	SG_FREE(ker2);
	SG_FREE(ker3);

	ker->append_kernel(kernel1);
	ker->append_kernel(kernel2);
	ker->append_kernel(kernel3);

	//here comes the core stuff
	float64_t regconst = 1.0;

	shogun::CMKLMulticlass *tsvm = new shogun::CMKLMulticlass(regconst, ker, lab);

	tsvm->set_epsilon(0.0001);  // SVM epsilon
	// MKL parameters
	tsvm->set_mkl_epsilon(0.01);  // subkernel weight L2 norm termination criterion
	tsvm->set_max_num_mkliters(120);  // well it will be just three iterations
	tsvm->set_mkl_norm(1.5);  // mkl norm
	// starting svm training
	tsvm->train();

	SG_SPRINT("finished svm training\n");

	// starting svm testing on training data
	shogun::CMulticlassLabels *res = shogun::CMulticlassLabels::obtain_from_generic(tsvm->apply());
	ASSERT(res);

	float64_t err = 0;
	for (int32_t i = 0; i < numdata; ++i)
	{
		ASSERT(i < res->get_num_labels());
		if (lab->get_int_label(i) != res->get_int_label(i))
			err += 1;
	}
	err /= (float64_t)res->get_num_labels();
	SG_SPRINT("prediction error on training data (3 classes): %f ", err);
	SG_SPRINT("random guess error would be: %f \n", 2/3.0);

	// generate test data
	shogun::CMulticlassLabels *tlab = NULL;

	std::vector<float64_t> tx, ty;

	local::gendata(tx, ty, tlab);
	SG_REF(tlab);

	float64_t *tker1 = NULL;
	float64_t *tker2 = NULL;
	float64_t *tker3 = NULL;

	local::gentestkernel(tker1, tker2, tker3, autosigma, n1, n2, n3, x, y, tx, ty);
	int32_t numdatatest = tx.size();

	shogun::CCombinedKernel *tker = new shogun::CCombinedKernel();
	SG_REF(tker);
	shogun::CCustomKernel *tkernel1 = new shogun::CCustomKernel();
	shogun::CCustomKernel *tkernel2 = new shogun::CCustomKernel();
	shogun::CCustomKernel *tkernel3 = new shogun::CCustomKernel();

	tkernel1->set_full_kernel_matrix_from_full(shogun::SGMatrix<float64_t>(tker1, numdata, numdatatest, false));
	tkernel2->set_full_kernel_matrix_from_full(shogun::SGMatrix<float64_t>(tker2, numdata, numdatatest, false));
	tkernel3->set_full_kernel_matrix_from_full(shogun::SGMatrix<float64_t>(tker2, numdata, numdatatest, false));

	SG_FREE(tker1);
	SG_FREE(tker2);
	SG_FREE(tker3);

	tker->append_kernel(tkernel1);
	tker->append_kernel(tkernel2);
	tker->append_kernel(tkernel3);

	int32_t numweights;
	float64_t *weights = tsvm->getsubkernelweights(numweights);

	SG_SPRINT("test kernel weights\n");

	for (int32_t i = 0; i < numweights; ++i)
		SG_SPRINT("%f ", weights[i]);
	SG_SPRINT("\n");

	// set kernel
	tker->set_subkernel_weights(shogun::SGVector<float64_t>(weights, numweights));
	tsvm->set_kernel(tker);

	// compute classification error, check mem
	shogun::CMulticlassLabels *tres = shogun::CMulticlassLabels::obtain_from_generic(tsvm->apply());

	float64_t terr = 0;
	for (int32_t i = 0; i < numdatatest; ++i)
	{
		ASSERT(i < tres->get_num_labels());
		if (tlab->get_int_label(i) != tres->get_int_label(i))
			terr += 1;
	}
	terr /= (float64_t)tres->get_num_labels();
	SG_SPRINT("prediction error on test data (3 classes): %f ", terr);
	SG_SPRINT("random guess error would be: %f \n", 2 / 3.0);

	SG_UNREF(tsvm);
	SG_UNREF(res);
	SG_UNREF(tres);
	SG_UNREF(lab);
	SG_UNREF(tlab);
	SG_UNREF(tker);

	SG_SPRINT("finished \n");
}

}  // namespace my_shogun
