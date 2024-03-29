/**
 * @file kmeans_main.cpp
 * @author Ryan Curtin
 *
 * Executable for running K-Means.
 *
 * This file is part of mlpack 2.0.1.
 *
 * mlpack is free software; you may redstribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#include <mlpack/core.hpp>

#include "kmeans.hpp"
#include "allow_empty_clusters.hpp"
#include "refined_start.hpp"
#include "elkan_kmeans.hpp"
#include "hamerly_kmeans.hpp"
#include "pelleg_moore_kmeans.hpp"
#include "dual_tree_kmeans.hpp"
//#include "papi.h"

using namespace mlpack;
using namespace mlpack::kmeans;
using namespace std;

// Define parameters for the executable.
PROGRAM_INFO("K-Means Clustering", "This program performs K-Means clustering "
		"on the given dataset, storing the learned cluster assignments either as "
		"a column of labels in the file containing the input dataset or in a "
		"separate file.  Empty clusters are not allowed by default; when a cluster "
		"becomes empty, the point furthest from the centroid of the cluster with "
		"maximum variance is taken to fill that cluster."
		"\n\n"
		"Optionally, the Bradley and Fayyad approach (\"Refining initial points for"
		" k-means clustering\", 1998) can be used to select initial points by "
		"specifying the --refined_start (-r) option.  This approach works by taking"
		" random samples of the dataset; to specify the number of samples, the "
		"--samples parameter is used, and to specify the percentage of the dataset "
		"to be used in each sample, the --percentage parameter is used (it should "
		"be a value between 0.0 and 1.0)."
		"\n\n"
		"There are several options available for the algorithm used for each Lloyd "
		"iteration, specified with the --algorithm (-a) option.  The standard O(kN)"
		" approach can be used ('naive').  Other options include the Pelleg-Moore "
		"tree-based algorithm ('pelleg-moore'), Elkan's triangle-inequality based "
		"algorithm ('elkan'), Hamerly's modification to Elkan's algorithm "
		"('hamerly'), the dual-tree k-means algorithm ('dualtree'), and the "
		"dual-tree k-means algorithm using the cover tree ('dualtree-covertree')."
		"\n\n"
		"As of October 2014, the --overclustering option has been removed.  If you "
		"want this support back, let us know -- file a bug at "
		"https://github.com/mlpack/mlpack/ or get in touch through another means.");

// Required options.
PARAM_STRING_REQ("input_file", "Input dataset to perform clustering on.", "i");
PARAM_INT_REQ("clusters", "Number of clusters to find (0 autodetects from "
		"initial centroids).", "c");

// Output options.
PARAM_FLAG("in_place", "If specified, a column containing the learned cluster "
		"assignments will be added to the input dataset file.  In this case, "
		"--outputFile is overridden.", "P");
PARAM_STRING("output_file", "File to write output labels or labeled data to.",
		"o", "");
PARAM_STRING("centroid_file", "If specified, the centroids of each cluster will"
		" be written to the given file.", "C", "");

// k-means configuration options.
PARAM_FLAG("allow_empty_clusters", "Allow empty clusters to be created.", "e");
PARAM_FLAG("labels_only", "Only output labels into output file.", "l");
PARAM_INT("max_iterations", "Maximum number of iterations before K-Means "
		"terminates.", "m", 1000);
PARAM_INT("seed", "Random seed.  If 0, 'std::time(NULL)' is used.", "s", 0);
PARAM_STRING("initial_centroids", "Start with the specified initial centroids.",
		"I", "");

// Parameters for "refined start" k-means.
PARAM_FLAG("refined_start", "Use the refined initial point strategy by Bradley "
		"and Fayyad to choose initial points.", "r");
PARAM_INT("samplings", "Number of samplings to perform for refined start (use "
		"when --refined_start is specified).", "S", 100);
PARAM_DOUBLE("percentage", "Percentage of dataset to use for each refined start"
		" sampling (use when --refined_start is specified).", "p", 0.02);

PARAM_STRING("algorithm", "Algorithm to use for the Lloyd iteration ('naive', "
		"'pelleg-moore', 'elkan', 'hamerly', 'dualtree', or 'dualtree-covertree').",
		"a", "naive");

// Given the type of initial partition policy, figure out the empty cluster
// policy and run k-means.
template<typename InitialPartitionPolicy>
void FindEmptyClusterPolicy(const InitialPartitionPolicy& ipp);

// Given the initial partitionining policy and empty cluster policy, figure out
// the Lloyd iteration step type and run k-means.
template<typename InitialPartitionPolicy, typename EmptyClusterPolicy>
void FindLloydStepType(const InitialPartitionPolicy& ipp);

// Given the template parameters, sanitize/load input and run k-means.
template<typename InitialPartitionPolicy, typename EmptyClusterPolicy, template<
		class, class > class LloydStepType>
void RunKMeans(const InitialPartitionPolicy& ipp);

int main(int argc, char** argv) {
	CLI::ParseCommandLine(argc, argv);

	// Initialize random seed.
	if (CLI::GetParam<int>("seed") != 0)
		math::RandomSeed((size_t) CLI::GetParam<int>("seed"));
	else
		math::RandomSeed((size_t) std::time(NULL));

	// Now, start building the KMeans type that we'll be using.  Start with the
	// initial partition policy.  The call to FindEmptyClusterPolicy<> results in
	// a call to RunKMeans<> and the algorithm is completed.
	if (CLI::HasParam("refined_start")) {
		const int samplings = CLI::GetParam<int>("samplings");
		const double percentage = CLI::GetParam<double>("percentage");

		if (samplings < 0)
			Log::Fatal << "Number of samplings (" << samplings << ") must be "
					<< "greater than 0!" << endl;
		if (percentage <= 0.0 || percentage > 1.0)
			Log::Fatal << "Percentage for sampling (" << percentage
					<< ") must be "
					<< "greater than 0.0 and less than or equal to 1.0!"
					<< endl;

		FindEmptyClusterPolicy<RefinedStart>(
				RefinedStart(samplings, percentage));
	} else {
		FindEmptyClusterPolicy<RandomPartition>(RandomPartition());
	}
}

// Given the type of initial partition policy, figure out the empty cluster
// policy and run k-means.
template<typename InitialPartitionPolicy>
void FindEmptyClusterPolicy(const InitialPartitionPolicy& ipp) {
	if (CLI::HasParam("allow_empty_clusters"))
		FindLloydStepType<InitialPartitionPolicy, AllowEmptyClusters>(ipp);
	else
		FindLloydStepType<InitialPartitionPolicy, MaxVarianceNewCluster>(ipp);
}

// Given the initial partitionining policy and empty cluster policy, figure out
// the Lloyd iteration step type and run k-means.
template<typename InitialPartitionPolicy, typename EmptyClusterPolicy>
void FindLloydStepType(const InitialPartitionPolicy& ipp) {
	const string algorithm = CLI::GetParam < string > ("algorithm");
	/*
	 if (algorithm == "elkan")
	 RunKMeans<InitialPartitionPolicy, EmptyClusterPolicy, ElkanKMeans>(ipp);
	 else if (algorithm == "hamerly")
	 RunKMeans<InitialPartitionPolicy, EmptyClusterPolicy, HamerlyKMeans>(ipp);
	 else if (algorithm == "pelleg-moore")
	 RunKMeans<InitialPartitionPolicy, EmptyClusterPolicy,
	 PellegMooreKMeans>(ipp);
	 else if (algorithm == "dualtree")
	 RunKMeans<InitialPartitionPolicy, EmptyClusterPolicy,
	 DefaultDualTreeKMeans>(ipp);
	 else if (algorithm == "dualtree-covertree")
	 RunKMeans<InitialPartitionPolicy, EmptyClusterPolicy,
	 CoverTreeDualTreeKMeans>(ipp);
	 else */
	if (algorithm == "naive")
		RunKMeans<InitialPartitionPolicy, EmptyClusterPolicy, NaiveKMeans>(ipp);
	else
		Log::Fatal << "Unknown algorithm: '" << algorithm
				<< "'.  Supported options"
				<< " are 'naive', 'pelleg-moore', 'elkan', 'hamerly', 'dualtree', and "
				<< "'dualtree-covertree'." << endl;
}

// Given the template parameters, sanitize/load input and run k-means.
template<typename InitialPartitionPolicy, typename EmptyClusterPolicy, template<
		class, class > class LloydStepType>
void RunKMeans(const InitialPartitionPolicy& ipp) {
	// Now, do validation of input options.
	const string inputFile = CLI::GetParam < string > ("input_file");
	int clusters = CLI::GetParam<int>("clusters");
	if (clusters < 0) {
		Log::Fatal << "Invalid number of clusters requested (" << clusters
				<< ")! " << "Must be greater than or equal to 0." << endl;
	} else if (clusters == 0 && CLI::HasParam("initial_centroids")) {
		Log::Info << "Detecting number of clusters automatically from input "
				<< "centroids." << endl;
	} else if (clusters == 0) {
		Log::Fatal
				<< "Number of clusters requested is 0, and no initial centroids "
				<< "provided!" << endl;
	}

	const int maxIterations = CLI::GetParam<int>("max_iterations");
	if (maxIterations < 0) {
		Log::Fatal << "Invalid value for maximum iterations (" << maxIterations
				<< ")! Must be greater than or equal to 0." << endl;
	}

	// Make sure we have an output file if we're not doing the work in-place.
	if (!CLI::HasParam("in_place") && !CLI::HasParam("output_file")
			&& !CLI::HasParam("centroid_file")) {
		Log::Warn
				<< "--output_file, --in_place, and --centroid_file are not set; "
				<< "no results will be saved." << std::endl;
	}

	// Load our dataset.
	arma::mat dataset;
	data::Load(inputFile, dataset, true); // Fatal upon failure.

	arma::mat centroids;

	const bool initialCentroidGuess = CLI::HasParam("initial_centroids");
	// Load initial centroids if the user asked for it.
	if (initialCentroidGuess) {
		string initialCentroidsFile = CLI::GetParam < string
				> ("initial_centroids");
		data::Load(initialCentroidsFile, centroids, true);
		if (clusters == 0)
			clusters = centroids.n_cols;

		if (CLI::HasParam("refined_start"))
			Log::Warn << "Initial centroids are specified, but will be ignored "
					<< "because --refined_start is also specified!" << endl;
		else
			Log::Info << "Using initial centroid guesses from '"
					<< initialCentroidsFile << "'." << endl;
	}

	Timer::Start("clustering");
	KMeans<metric::EuclideanDistance, InitialPartitionPolicy,
			EmptyClusterPolicy, LloydStepType> kmeans(maxIterations,
			metric::EuclideanDistance(), ipp);

	if (CLI::HasParam("output_file") || CLI::HasParam("in_place")) {
		// We need to get the assignments.
		arma::Row<size_t> assignments;

		kmeans.Cluster(dataset, clusters, assignments, centroids, false,
				initialCentroidGuess);
		Timer::Stop("clustering");

		// Now figure out what to do with our results.
		if (CLI::HasParam("in_place")) {
			// Add the column of assignments to the dataset; but we have to convert
			// them to type double first.
			arma::rowvec converted(assignments.n_elem);
			for (size_t i = 0; i < assignments.n_elem; i++)
				converted(i) = (double) assignments(i);

			dataset.insert_rows(dataset.n_rows, converted);

			// Save the dataset.
			data::Save(inputFile, dataset);
		} else {
			if (CLI::HasParam("labels_only")) {
				// Save only the labels.
				string outputFile = CLI::GetParam < string > ("output_file");
				data::Save(outputFile, assignments);
			} else {
				// Convert the assignments to doubles.
				arma::rowvec converted(assignments.n_elem);
				for (size_t i = 0; i < assignments.n_elem; i++)
					converted(i) = (double) assignments(i);

				dataset.insert_rows(dataset.n_rows, converted);

				// Now save, in the different file.
				string outputFile = CLI::GetParam < string > ("output_file");
				data::Save(outputFile, dataset);
			}
		}
	} else {
		kmeans.Cluster(dataset, clusters, centroids, initialCentroidGuess);
		Timer::Stop("clustering");
	}

	// Should we write the centroids to a file?
	if (CLI::HasParam("centroid_file"))
		data::Save(CLI::GetParam < std::string > ("centroid_file"), centroids);
}
