#ifndef UTIL_HPP
#define UTIL_HPP

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 550
#define SCALE 30.f
#define PI 3.1459
#define EXP 2.71828182845904523536
#define DELIM ","

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>

namespace ublas = boost::numeric::ublas;
typedef ublas::matrix<float> matrix;
typedef ublas::vector<float> vector;

inline float
random_float (float min, float max)
{
  float random = ((float) rand()) / (float) RAND_MAX;
  float range = max - min;
  return (random*range) + min;
}

inline vector
sigmoid(vector z)
{
  for (unsigned long i=0; i < z.size(); ++i)
  {
    z(i) = (1.f / (1.f + std::pow(EXP, -z(i))));
  }
  return z;
}

inline std::vector<matrix> split_theta(vector theta, int inputs, int hidden)
{
  std::vector<matrix> split_T;
  matrix Theta1(inputs,hidden);
  matrix Theta2(hidden+1,1);

  int w_index = 0;
  for (int i=0; i < hidden; ++i)
  {
    for(int k=0; k < inputs; ++k)
    {
      Theta1(k,i) = theta(w_index);
      ++w_index;
    }
  }

  for(int j=0; j < hidden; ++j)
  {
    Theta2(j,0) = theta(w_index);
    ++w_index;
  }

  split_T.push_back(Theta1);
  split_T.push_back(Theta2);

  return(split_T);
}

#endif
