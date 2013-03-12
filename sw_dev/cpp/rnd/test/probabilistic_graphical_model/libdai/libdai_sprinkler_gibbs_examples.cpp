//#include "stdafx.h"
#include <dai/factorgraph.h>
#include <dai/gibbs.h>
#include <dai/properties.h>
#include <fstream>
#include <iostream>
#include <stdexcept>


namespace {
namespace local {

}  // namespace local
}  // unnamed namespace

namespace my_libdai {

// [ref] ${LIBDAI_HOME}/examples/example_sprinkler_gibbs.cpp
void sprinkler_gibbs_example()
{
    // This example program illustrates how to use Gibbs sampling to sample from a joint probability distribution described by a factor graph, using the sprinkler network example discussed at
    // http://www.cs.ubc.ca/~murphyk/Bayes/bnintro.html

	// The file sprinkler.fg has to be generated by first running example_sprinkler

    // Read the factorgraph from the file
    dai::FactorGraph SprinklerNetwork;
    SprinklerNetwork.ReadFromFile("./probabilistic_graphical_model_data/libdai/sprinkler.fg");
    std::cout << "Sprinkler network read from sprinkler.fg" << std::endl;

    // Output some information about the factorgraph
    std::cout << SprinklerNetwork.nrVars() << " variables" << std::endl;
    std::cout << SprinklerNetwork.nrFactors() << " factors" << std::endl;

    // Prepare a Gibbs sampler
    dai::PropertySet gibbsProps;
    gibbsProps.set("maxiter", std::size_t(100));  // number of Gibbs sampler iterations
    gibbsProps.set("burnin", std::size_t(0));
    gibbsProps.set("verbose", std::size_t(0));
    dai::Gibbs gibbsSampler(SprinklerNetwork, gibbsProps);

    // Open a .tab file for writing
    std::ofstream outfile;
    outfile.open("./probabilistic_graphical_model_data/libdai/sprinkler.tab");
    if (!outfile.is_open())
		throw std::runtime_error("Cannot write to file!");

    // Write header (consisting of variable labels)
    for (std::size_t i = 0; i < SprinklerNetwork.nrVars(); ++i)
        outfile << (i == 0 ? "" : "\t") << SprinklerNetwork.var(i).label();
    outfile << std::endl << std::endl;

    // Draw samples from joint distribution using Gibbs sampling and write them to the .tab file
    const std::size_t nrSamples = 1000;
    std::vector<std::size_t> state;
    for (std::size_t t = 0; t < nrSamples; ++t)
	{
        gibbsSampler.init();
        gibbsSampler.run();
        state = gibbsSampler.state();
        for (std::size_t i = 0; i < state.size(); ++i)
            outfile << (i == 0 ? "" : "\t") << state[i];
        outfile << std::endl;
    }
    std::cout << nrSamples << " samples written to sprinkler.tab" << std::endl;

    // Close the .tab file
    outfile.close();
}

}  // namespace my_libdai
