/**
 * @file naive_kmeans_impl.hpp
 * @author Ryan Curtin
 *
 * An implementation of a naively-implemented step of the Lloyd algorithm for
 * k-means clustering.  This may still be the best choice for small datasets or
 * datasets with very high dimensionality.
 *
 * This file is part of mlpack 2.0.1.
 *
 * mlpack is free software; you may redstribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef __MLPACK_METHODS_KMEANS_NAIVE_KMEANS_IMPL_HPP
#define __MLPACK_METHODS_KMEANS_NAIVE_KMEANS_IMPL_HPP

// In case it hasn't been included yet.
#include "naive_kmeans.hpp"
//#include "acml.h"
//#include "papi.h"
namespace mlpack {
namespace kmeans {

template<typename MetricType, typename MatType>
NaiveKMeans<MetricType, MatType>::NaiveKMeans(const MatType& dataset,
		MetricType& metric) :
		dataset(dataset), metric(metric), distanceCalculations(0) {
	ddt.zeros(dataset.n_cols, 1);
	dataset_t = dataset.t();
	double sum;
	for (size_t i = 0; i < dataset.n_cols; i++) {
		sum = 0;
		for (size_t j = 0; j < dataset.n_rows; j++)
			sum += dataset(j, i) * dataset(j, i);
		ddt(i, 0) = sum;
	}

}

template<typename MetricType, typename MatType>
int NaiveKMeans<MetricType, MatType>::EmptyClusterAdjust(const MatType& data,
		const size_t emptyCluster, const arma::mat& oldCentroids,
		arma::mat& newCentroids, arma::Col<size_t>& clusterCounts,
		MetricType& metric, const size_t iteration) {
	this->iteration = iteration;

	// Now find the cluster with maximum variance.
	arma::uword maxVarCluster = 0;
	variances.max(maxVarCluster);

	// If the cluster with maximum variance has variance of 0, then we can't
	// continue.  All the points are the same.
	if (variances[maxVarCluster] == 0.0)
		return 0;

	// Now, inside this cluster, find the point which is furthest away.
	size_t furthestPoint = data.n_cols;
	double maxDistance = -DBL_MAX;
	for (size_t i = 0; i < data.n_cols; ++i) {
		if (assignments[i] == maxVarCluster) {
			const double distance = std::pow(
					metric.Evaluate(data.col(i),
							newCentroids.col(maxVarCluster)), 2.0);

			if (distance > maxDistance) {
				maxDistance = distance;
				furthestPoint = i;
			}
		}
	}

	// Take that point and add it to the empty cluster.
	newCentroids.col(maxVarCluster) *= (double(clusterCounts[maxVarCluster])
			/ double(clusterCounts[maxVarCluster] - 1));
	newCentroids.col(maxVarCluster) -= (1.0
			/ (clusterCounts[maxVarCluster] - 1.0))
			* arma::vec(data.col(furthestPoint));
	clusterCounts[maxVarCluster]--;
	clusterCounts[emptyCluster]++;
	newCentroids.col(emptyCluster) = arma::vec(data.col(furthestPoint));
	assignments[furthestPoint] = emptyCluster;

	// Modify the variances, as necessary.
	variances[emptyCluster] = 0;
	// One has already been subtracted from clusterCounts[maxVarCluster].  If
	// EmptyCluster() is called again, we can't pull another point from
	// maxVarCluster (we'd be making an empty cluster), so force a call to
	// Precalculate() if EmptyCluster() is called again by changing
	// this->iteration.
	if (clusterCounts[maxVarCluster] <= 1) {
		variances[maxVarCluster] = 0;
		--this->iteration;
	} else {
		variances[maxVarCluster] = (1.0 / clusterCounts[maxVarCluster])
				* ((clusterCounts[maxVarCluster] + 1) * variances[maxVarCluster]
						- maxDistance);
	}

	// Output some debugging information.
	Log::Debug << "Point " << furthestPoint << " assigned to empty cluster "
			<< emptyCluster << ".\n";

	return 1; // We only changed one point.

}

// Run a single iteration.
template<typename MetricType, typename MatType>
double NaiveKMeans<MetricType, MatType>::Iterate(arma::mat& centroids,
		arma::mat& newCentroids, arma::Col<size_t>& counts) {

	newCentroids.zeros(centroids.n_rows, centroids.n_cols);
	counts.zeros(centroids.n_cols);
	variances.zeros(centroids.n_cols);
	assignments.set_size(dataset.n_cols);

	arma::mat dist_matrix(dataset.n_cols, centroids.n_cols);
	dist_matrix = dataset_t * centroids;
	arma::mat cct(centroids.n_cols, 1);

	double sum;
	for (size_t i = 0; i < centroids.n_cols; i++) {
		sum = 0;
		for (size_t j = 0; j < centroids.n_rows; j++)
			sum += centroids(j, i) * centroids(j, i);
		cct(i, 0) = sum;
	}
	dist_matrix = -2 * dist_matrix;
	dist_matrix.each_col() += ddt;
	arma::mat dist_matrix_t = dist_matrix.t();
	dist_matrix_t.each_col() += cct;

	for (size_t i = 0; i < dataset.n_cols; i++) {

		arma::uword closestCluster; // Invalid value.
		dist_matrix_t.col(i).min(closestCluster);

		Log::Assert(closestCluster != centroids.n_cols);

		// We now have the minimum distance centroid index.  Update that centroid.
		newCentroids.col(closestCluster) += dataset.col(i);
		++counts(closestCluster);
		assignments[i] = closestCluster;
		variances[closestCluster] += std::pow(
				metric.Evaluate(dataset.col(i), centroids.col(closestCluster)),
				2.0);
	}

	// Now normalize the centroid.
	for (size_t i = 0; i < centroids.n_cols; ++i)
		if (counts(i) != 0)
			newCentroids.col(i) /= counts(i);
		else
			newCentroids.col(i).fill(DBL_MAX); // Invalid value.

	distanceCalculations += centroids.n_cols * dataset.n_cols;

	// Calculate cluster distortion for this iteration.
	double cNorm;
	for (size_t i = 0; i < centroids.n_cols; ++i) {
		cNorm = std::pow(metric.Evaluate(centroids.col(i), newCentroids.col(i)),
				2.0);
	}
	distanceCalculations += centroids.n_cols;

	for (size_t i = 0; i < counts.n_elem; ++i)
		if (counts[i] <= 1)
			variances[i] = 0;
		else
			variances[i] /= counts[i];

	return std::sqrt(cNorm);
}

} // namespace kmeans
} // namespace mlpack

#endif
