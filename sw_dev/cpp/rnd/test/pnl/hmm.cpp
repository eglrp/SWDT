#include "stdafx.h"
#include <pnl_dll.hpp>
#include <boost/smart_ptr.hpp>
#include <iostream>


namespace {
namespace local {

pnl::CDBN * create_simple_hmm()
{
#if 0
/*
	a simple HMM
		X0 -> X1
		|     | 
		v     v
		Y0    Y1 
*/
	
/*
	states = ('Rainy', 'Sunny')
 
	observations = ('walk', 'shop', 'clean')
 
	start_probability = {'Rainy': 0.6, 'Sunny': 0.4}
 
	transition_probability = {
	   'Rainy' : {'Rainy': 0.7, 'Sunny': 0.3},
	   'Sunny' : {'Rainy': 0.4, 'Sunny': 0.6},
	}
 
	emission_probability = {
	   'Rainy' : {'walk': 0.1, 'shop': 0.4, 'clean': 0.5},
	   'Sunny' : {'walk': 0.6, 'shop': 0.3, 'clean': 0.1},
	}
*/

	// create static model
	const int numNodes = 4;  // number of nodes
	const int numNodeTypes = 2;  // number of node types (all nodes are discrete)

	// specify the graph structure of the model
	const int numNeigh[] = { 2, 1, 2, 1 };
	const int neigh0[] = { 1, 2 };
	const int neigh1[] = { 0 };
	const int neigh2[] = { 0, 3 };
	const int neigh3[] = { 2 };
    const int *neigh[] = { neigh0, neigh1, neigh2, neigh3 };

    const pnl::ENeighborType orient0[] = { pnl::ntChild, pnl::ntChild };
    const pnl::ENeighborType orient1[] = { pnl::ntParent };
    const pnl::ENeighborType orient2[] = { pnl::ntParent, pnl::ntChild };
    const pnl::ENeighborType orient3[] = { pnl::ntParent };
    const pnl::ENeighborType *orient[] = { orient0, orient1, orient2, orient3 };
	
	pnl::CGraph *pGraph = pnl::CGraph::Create(numNodes, numNeigh, neigh, orient);

	// create static BNet
	pnl::nodeTypeVector nodeTypes(numNodeTypes);
	nodeTypes[0].SetType(true, 2);
	nodeTypes[1].SetType(true, 3);

	pnl::intVector nodeAssociation(numNodes);
	nodeAssociation[0] = 0;
	nodeAssociation[1] = 1;
	nodeAssociation[2] = 0;
	nodeAssociation[3] = 1;
	
	pnl::CBNet *pBNet = pnl::CBNet::Create(numNodes, nodeTypes, nodeAssociation, pGraph);

	// create raw data tables for CPDs
	const float table0[] = { 0.6f, 0.4f };
	const float table1[] = { 0.1f, 0.4f, 0.5f, 0.6f, 0.3f, 0.1f };
	const float table2[] = { 0.7f, 0.3f, 0.4f, 0.6f };
	const float table3[] = { 0.1f, 0.4f, 0.5f, 0.6f, 0.3f, 0.1f };

	// create factors and attach their to model
	pBNet->AllocFactors();
	pBNet->AllocFactor(0); 
	pBNet->GetFactor(0)->AllocMatrix(table0, pnl::matTable);
	pBNet->AllocFactor(1);
	pBNet->GetFactor(1)->AllocMatrix(table1, pnl::matTable);
	pBNet->AllocFactor(2);
	pBNet->GetFactor(2)->AllocMatrix(table2, pnl::matTable);
	pBNet->AllocFactor(3);
	pBNet->GetFactor(3)->AllocMatrix(table3, pnl::matTable);
#else
	pnl::CBNet *pBNet = pnl::pnlExCreateRndArHMM();

	const int numFactors = pBNet->GetNumberOfFactors();
	const pnl::CFactor *factor = pBNet->GetFactor(3);
	const pnl::CMatrix<float> *cpd = factor->GetMatrix(pnl::matTable);

	const int dims = cpd->GetNumberDims();

	int ddims;
	int *ranges = new int [dims];
	cpd->GetRanges(&ddims, (const int **)&ranges);
	delete [] ranges;
#endif;

	// create DBN
	pnl::CDBN *pDBN = pnl::CDBN::Create(pBNet);

	return pDBN;
}

pnl::CDBN * create_hmm_with_ar_gaussian_observations()
{
/*
	an HMM with autoregressive Gaussian observations
		X0 -> X1
		|     | 
		v     v
		Y0 -> Y1 
*/

	// create static model
	const int numNodes = 4;  // number of nodes    
	const int numNodeTypes = 2;  // number of node types (all nodes are discrete)

	pnl::CNodeType *nodeTypes = new pnl::CNodeType [numNodeTypes];
	nodeTypes[0].SetType(true, 2);
	nodeTypes[1].SetType(false, 1);

	const int nodeAssociation[] = { 0, 1, 0, 1 };

	// create a DAG
	const int numNeigh[] = { 2, 2, 2, 2 };
	const int neigh0[] = { 1, 2 };
	const int neigh1[] = { 0, 3 };
	const int neigh2[] = { 0, 3 };
	const int neigh3[] = { 1, 2 };
	const int *neigh[] = { neigh0, neigh1, neigh2, neigh3 };

	const pnl::ENeighborType orient0[] = { pnl::ntChild, pnl::ntChild };
	const pnl::ENeighborType orient1[] = { pnl::ntParent, pnl::ntChild };
	const pnl::ENeighborType orient2[] = { pnl::ntParent, pnl::ntChild };
	const pnl::ENeighborType orient3[] = { pnl::ntParent, pnl::ntParent };
	const pnl::ENeighborType *orient[] = { orient0, orient1, orient2, orient3 };

	pnl::CGraph *pGraph = pnl::CGraph::Create(numNodes, numNeigh, neigh, orient);

	// create static BNet
	pnl::CBNet *pBNet = pnl::CBNet::Create(numNodes, numNodeTypes, nodeTypes, nodeAssociation, pGraph);
	pBNet->AllocFactors();

	// let arbitrary distribution is
	const float tableNode0[] = { 0.95f, 0.05f };
	const float tableNode2[] = { 0.1f, 0.9f, 0.5f, 0.5f };

	const float mean1w0 = -3.2f;  const float cov1w0 = 0.00002f; 
	const float mean1w1 = -0.5f;  const float cov1w1 = 0.0001f;

	const float mean3w0 = 6.5f;  const float cov3w0 = 0.03f;  const float weight3w0 = 1.0f;
	const float mean3w1 = 7.5f;  const float cov3w1 = 0.04f;  const float weight3w1 = 0.5f;

	pBNet->AllocFactor(0);
	pBNet->GetFactor(0)->AllocMatrix(tableNode0, pnl::matTable);

	pBNet->AllocFactor(1);
	int parent[] = { 0 };
	pBNet->GetFactor(1)->AllocMatrix(&mean1w0, pnl::matMean, -1, parent);
	pBNet->GetFactor(1)->AllocMatrix(&cov1w0, pnl::matCovariance, -1, parent);
	parent[0] = 1;
	pBNet->GetFactor(1)->AllocMatrix(&mean1w1, pnl::matMean, -1, parent);
	pBNet->GetFactor(1)->AllocMatrix(&cov1w1, pnl::matCovariance, -1, parent);

	pBNet->AllocFactor(2);
	pBNet->GetFactor(2)->AllocMatrix(tableNode2, pnl::matTable);

	pBNet->AllocFactor(3);
	parent[0] = 0;
	pBNet->GetFactor(3)->AllocMatrix(&mean3w0, pnl::matMean, -1, parent);
	pBNet->GetFactor(3)->AllocMatrix(&cov3w0, pnl::matCovariance, -1, parent);
	pBNet->GetFactor(3)->AllocMatrix(&weight3w0, pnl::matWeights, 0, parent);
	parent[0] = 1;
	pBNet->GetFactor(3)->AllocMatrix(&mean3w1, pnl::matMean, -1, parent);
	pBNet->GetFactor(3)->AllocMatrix(&cov3w1, pnl::matCovariance, -1, parent);
	pBNet->GetFactor(3)->AllocMatrix(&weight3w1, pnl::matWeights, 0, parent);

	// create DBN using BNet	
	pnl::CDBN *pArHMM = pnl::CDBN::Create(pBNet);

    return pArHMM;
}

}  // namespace local
}  // unnamed namespace

void hmm()
{
	// simple HMM
	{
		boost::scoped_ptr<pnl::CDBN> simpleHMM(local::create_simple_hmm());
		//boost::scoped_ptr<pnl::CDBN> simpleHMM(local::create_hmm_with_ar_gaussian_observations());

		// get content of Graph
		simpleHMM->GetGraph()->Dump();
 
		{
			const pnl::CGraph *pGraph = simpleHMM->GetGraph();

			int numNbrs1, numNbrs2;
			const int *nbrs1, *nbrs2;
			const pnl::ENeighborType *nbrsTypes1, *nbrsTypes2;
			pGraph->GetNeighbors(0, &numNbrs1, &nbrs1, &nbrsTypes1);
			pGraph->GetNeighbors(1, &numNbrs2, &nbrs2, &nbrsTypes2);
		}

		// create an inference engine
		boost::scoped_ptr<pnl::C1_5SliceJtreeInfEngine> inferEng(pnl::C1_5SliceJtreeInfEngine::Create(simpleHMM.get()));

		// number of time slices for unrolling
		const int numTimeSlices = 3;
		const pnl::CPotential *pQueryJPD = NULL;

		// create evidence for every slice
		pnl::CEvidence **pEvidences = new pnl::CEvidence *[numTimeSlices];

		//
		const int obsNodesNums[] = { 1 };
		pnl::valueVector obsNodesVals(1);
		obsNodesVals[0].SetInt(0);
		pEvidences[0] = pnl::CEvidence::Create(simpleHMM.get(), 1, obsNodesNums, obsNodesVals);
		obsNodesVals[0].SetInt(1);
		pEvidences[1] = pnl::CEvidence::Create(simpleHMM.get(), 1, obsNodesNums, obsNodesVals);
		obsNodesVals[0].SetInt(2);
		pEvidences[2] = pnl::CEvidence::Create(simpleHMM.get(), 1, obsNodesNums, obsNodesVals);

		inferEng->DefineProcedure(pnl::ptViterbi, numTimeSlices);
		inferEng->EnterEvidence(pEvidences, numTimeSlices);
		inferEng->FindMPE();

		const pnl::CEvidence *mpe = inferEng->GetMPE();

		//
		for (int slice = 0; slice < numTimeSlices; ++slice)
			delete pEvidences[slice];
		delete [] pEvidences;
	}

	// HMM with AR Gaussian observations
	{
		boost::scoped_ptr<pnl::CDBN> arHMM(local::create_hmm_with_ar_gaussian_observations());

		// get content of Graph
		arHMM->GetGraph()->Dump();
	}
}