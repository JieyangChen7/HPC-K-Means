/**
 * @file nmf_main.cpp
 * @author Mohan Rajendran
 *
 * Main executable to run NMF.
 *
 * This file is part of mlpack 2.0.1.
 *
 * mlpack is free software; you may redstribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#include <mlpack/core.hpp>

#include <mlpack/methods/amf/amf.hpp>

#include <mlpack/methods/amf/init_rules/random_init.hpp>
#include <mlpack/methods/amf/update_rules/nmf_mult_dist.hpp>
#include <mlpack/methods/amf/update_rules/nmf_mult_div.hpp>
#include <mlpack/methods/amf/update_rules/nmf_als.hpp>
#include <mlpack/methods/amf/termination_policies/simple_residue_termination.hpp>

using namespace mlpack;
using namespace mlpack::amf;
using namespace std;

// Document program.
PROGRAM_INFO("Non-negative Matrix Factorization", "This program performs "
    "non-negative matrix factorization on the given dataset, storing the "
    "resulting decomposed matrices in the specified files.  For an input "
    "dataset V, NMF decomposes V into two matrices W and H such that "
    "\n\n"
    "V = W * H"
    "\n\n"
    "where all elements in W and H are non-negative.  If V is of size (n x m),"
    " then W will be of size (n x r) and H will be of size (r x m), where r is "
    "the rank of the factorization (specified by --rank)."
    "\n\n"
    "Optionally, the desired update rules for each NMF iteration can be chosen "
    "from the following list:"
    "\n\n"
    " - multdist: multiplicative distance-based update rules (Lee and Seung "
    "1999)\n"
    " - multdiv: multiplicative divergence-based update rules (Lee and Seung "
    "1999)\n"
    " - als: alternating least squares update rules (Paatero and Tapper 1994)"
    "\n\n"
    "The maximum number of iterations is specified with --max_iterations, and "
    "the minimum residue required for algorithm termination is specified with "
    "--min_residue.");

// Parameters for program.
PARAM_STRING_REQ("input_file", "Input dataset to perform NMF on.", "i");
PARAM_STRING_REQ("w_file", "File to save the calculated W matrix to.", "W");
PARAM_STRING_REQ("h_file", "File to save the calculated H matrix to.", "H");
PARAM_INT_REQ("rank", "Rank of the factorization.", "r");

PARAM_INT("max_iterations", "Number of iterations before NMF terminates (0 runs"
    " until convergence.", "m", 10000);
PARAM_INT("seed", "Random seed.  If 0, 'std::time(NULL)' is used.", "s", 0);
PARAM_DOUBLE("min_residue", "The minimum root mean square residue allowed for "
    "each iteration, below which the program terminates.", "e", 1e-5);

PARAM_STRING("update_rules", "Update rules for each iteration; ( multdist | "
    "multdiv | als ).", "u", "multdist");

int main(int argc, char** argv)
{
  // Parse command line.
  CLI::ParseCommandLine(argc, argv);

  // Initialize random seed.
  if (CLI::GetParam<int>("seed") != 0)
    math::RandomSeed((size_t) CLI::GetParam<int>("seed"));
  else
    math::RandomSeed((size_t) std::time(NULL));

  // Gather parameters.
  const string inputFile = CLI::GetParam<string>("input_file");
  const string hOutputFile = CLI::GetParam<string>("h_file");
  const string wOutputFile = CLI::GetParam<string>("w_file");
  const size_t r = CLI::GetParam<int>("rank");
  const size_t maxIterations = CLI::GetParam<int>("max_iterations");
  const double minResidue = CLI::GetParam<double>("min_residue");
  const string updateRules = CLI::GetParam<string>("update_rules");

  // Validate rank.
  if (r < 1)
  {
    Log::Fatal << "The rank of the factorization cannot be less than 1."
        << std::endl;
  }

  if ((updateRules != "multdist") &&
      (updateRules != "multdiv") &&
      (updateRules != "als"))
  {
    Log::Fatal << "Invalid update rules ('" << updateRules << "'); must be '"
        << "multdist', 'multdiv', or 'als'." << std::endl;
  }

  // Load input dataset.
  arma::mat V;
  data::Load(inputFile, V, true);

  arma::mat W;
  arma::mat H;

  // Perform NMF with the specified update rules.
  if (updateRules == "multdist")
  {
    Log::Info << "Performing NMF with multiplicative distance-based update "
        << "rules." << std::endl;

    SimpleResidueTermination srt(minResidue, maxIterations);
    AMF<> amf(srt);
    amf.Apply(V, r, W, H);
  }
  else if (updateRules == "multdiv")
  {
    Log::Info << "Performing NMF with multiplicative divergence-based update "
        << "rules." << std::endl;
    SimpleResidueTermination srt(minResidue, maxIterations);
    AMF<SimpleResidueTermination,
        RandomInitialization,
        NMFMultiplicativeDivergenceUpdate> amf(srt);
    amf.Apply(V, r, W, H);
  }
  else if (updateRules == "als")
  {
    Log::Info << "Performing NMF with alternating least squared update rules."
        << std::endl;
    SimpleResidueTermination srt(minResidue, maxIterations);
    AMF<SimpleResidueTermination,
        RandomInitialization,
        NMFALSUpdate> amf(srt);
    amf.Apply(V, r, W, H);
  }

  // Save results.
  data::Save(wOutputFile, W, false);
  data::Save(hOutputFile, H, false);
}
