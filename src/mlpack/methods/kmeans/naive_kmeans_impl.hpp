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
#include "acml.h"
#include "papi.h"
namespace mlpack {
namespace kmeans {

template<typename MetricType, typename MatType>
NaiveKMeans<MetricType, MatType>::NaiveKMeans(const MatType& dataset,
                                              MetricType& metric) :
    dataset(dataset),
    metric(metric),
    distanceCalculations(0)
{ 
  
  ddt.zeros(dataset.n_cols, 1);
  dataset_t = dataset.t();
   for (size_t i = 0; i < dataset.n_cols; i++)
   {
     arma::mat temp = dataset_t.row(i) * dataset.col(i);
     ddt(i, 0) = temp(0, 0);
   }
/* Nothing to do. */ }

// Run a single iteration.
template<typename MetricType, typename MatType>
double NaiveKMeans<MetricType, MatType>::Iterate(arma::mat& centroids,
                                                 arma::mat& newCentroids,
                                                 arma::Col<size_t>& counts)
{
   newCentroids.zeros(centroids.n_rows, centroids.n_cols);
   counts.zeros(centroids.n_cols);


    arma::mat dist_matrix(dataset.n_cols, centroids.n_cols);
    float real_time = 0.0;
    float proc_time = 0.0;
    long long flpins = 0.0;
    float mflops = 0.0;


   

  
  

  //d * c^T
  dist_matrix = dataset_t * centroids;

  double * data_ptr = dataset_t.memptr();
  double * cent_ptr = centroids.memptr();
  double * dist_ptr = dist_matrix.memptr();

  dgemm('N', 'N', 
  		dataset_t.n_rows, 
  		centroids.n_cols, 
  		dataset_t.n_cols, 
  		1.0,
  		data_ptr, dataset_t.n_rows, 
  		cent_ptr, centroids.n_rows,
  		0.0,
  		dist_ptr, dist_matrix.n_rows);

 //timing start
  if (PAPI_flops(&real_time, &proc_time, &flpins, &mflops) < PAPI_OK) {
    std::cout << "PAPI ERROR" << std::endl;
    //return -1;                                                                                                                                                                                                                           
  }

  // c * c^T
  arma::mat cct(1, centroids.n_cols);
  arma::mat centroids_t = centroids.t();
  for (size_t i = 0; i < centroids.n_cols; i++)
  {
    arma::mat temp = centroids_t.row(i) * centroids.col(i);
    cct(0, i) = temp(0, 0);
  }

  // d * c^T -> -2 * d * c^T
  dist_matrix = -2 * dist_matrix;

  // //d * d^T - 2 * d * c^T
  for (size_t i = 0; i < centroids.n_cols; i++)
  {
    dist_matrix.col(i) = dist_matrix.col(i) + ddt;
  }

  //d * d^T + c * c^T - 2 * d * c^T
  for (size_t i = 0; i < dataset.n_cols; i++)
  {
    dist_matrix.row(i) = dist_matrix.row(i) + cct;
  }



  //calculate distance
  // for (size_t i = 0; i < dataset.n_cols; i++)
  // {
  //   // Find the closest centroid to this point.
  //   for (size_t j = 0; j < centroids.n_cols; j++)
  //   {
  //     dist_matrix(i ,j) = metric.Evaluate(dataset.col(i), centroids.col(j));

  //   }
  // }


  //timing end
  if (PAPI_flops(&real_time, &proc_time, &flpins, &mflops) < PAPI_OK) {
      std::cout << "PAPI ERROR" << std::endl;
      //return -1;                                                                                                                                                                                                                           
  }
  std::cout << "time:" << real_time <<"---flpins:"<<flpins<< "---mflops:" << mflops << std::endl;
  PAPI_shutdown();


  //timing start
   if (PAPI_flops(&real_time, &proc_time, &flpins, &mflops) < PAPI_OK) {
      std::cout << "PAPI ERROR" << std::endl;
      //return -1;                                                                                                                                                                                                                           
    }


  // Find the closest centroid to each point and update the new centroids.
  for (size_t i = 0; i < dataset.n_cols; i++)
  {
    // Find the closest centroid to this point.
    double minDistance = std::numeric_limits<double>::infinity();
    size_t closestCluster = centroids.n_cols; // Invalid value.

    for (size_t j = 0; j < centroids.n_cols; j++)
    {
      const double distance = dist_matrix(i, j);
      //const double distance = pow(dist_matrix(i, j), (1.0 / 2.0));
      if (distance < minDistance)
      {
        minDistance = distance;
        closestCluster = j;
      }
    }

    Log::Assert(closestCluster != centroids.n_cols);

    // We now have the minimum distance centroid index.  Update that centroid.
    newCentroids.col(closestCluster) += arma::vec(dataset.col(i));
    counts(closestCluster)++;
  }
 
//timing end
  if (PAPI_flops(&real_time, &proc_time, &flpins, &mflops) < PAPI_OK) {
      std::cout << "PAPI ERROR" << std::endl;
      //return -1;                                                                                                                                                                                                                           
  }
  std::cout << "time:" << real_time <<"---flpins:"<<flpins<< "---mflops:" << mflops << std::endl;
  PAPI_shutdown();
   

  // Now normalize the centroid.
  for (size_t i = 0; i < centroids.n_cols; ++i)
    if (counts(i) != 0)
      newCentroids.col(i) /= counts(i);
    else
      newCentroids.col(i).fill(DBL_MAX); // Invalid value.

  distanceCalculations += centroids.n_cols * dataset.n_cols;

  // Calculate cluster distortion for this iteration.
  double cNorm = 0.0;
  for (size_t i = 0; i < centroids.n_cols; ++i)
  {
    cNorm += std::pow(metric.Evaluate(centroids.col(i), newCentroids.col(i)),
        2.0);
  }
  distanceCalculations += centroids.n_cols;

  return std::sqrt(cNorm);
}

} // namespace kmeans
} // namespace mlpack

#endif
