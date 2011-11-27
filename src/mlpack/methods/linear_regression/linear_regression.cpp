#include "linear_regression.hpp"

namespace mlpack {
namespace linear_regression {

LinearRegression::LinearRegression(arma::mat& predictors,
                                   const arma::colvec& responses)
{

  /*
   * We want to calculate the a_i coefficients of:
   * \sum_{i=0}^n (a_i * x_i^i)
   * In order to get the intercept value, we will add a row of ones.
   */

  // We store the number of rows of the predictors.
  // Reminder: Armadillo stores the data transposed from how we think of it,
  //           that is, columns are actually rows (see: column major order).
  size_t n_cols;
  n_cols = predictors.n_cols;

  // Here we add the row of ones to the predictors.
  arma::rowvec ones;
  ones.ones(n_cols);
  predictors.insert_rows(0, ones);

  // We set the parameters to the correct size and initialize them to zero.
  parameters.zeros(n_cols);

  // We compute the QR decomposition of the predictors.
  // We transpose the predictors because they are in column major order.
  arma::mat Q, R;
  arma::qr(Q, R, arma::trans(predictors));

  // We compute the parameters, B, like so:
  // R * B = Q^T * responses
  // B = Q^T * responses * R^-1
  arma::solve(parameters, R, arma::trans(Q) * responses);

  // We now remove the row of ones we added so the user's data is unmodified.
  predictors.shed_row(0);
}

LinearRegression::LinearRegression(const std::string& filename)
{
  parameters.load(filename);
}

LinearRegression::~LinearRegression()
{ }

void LinearRegression::predict(arma::rowvec& predictions,
                               const arma::mat& points)
{
  // We get the number of columns and rows of the dataset.
  size_t n_cols, n_rows;
  n_cols = points.n_cols;
  n_rows = points.n_rows;

  // We want to be sure we have the correct number of dimensions in the dataset.
  assert(n_rows == parameters.n_rows - 1);

  predictions.zeros(n_cols);
  // We set all the predictions to the intercept value initially.
  predictions += parameters(0);

  // Now we iterate through the dimensions of the data and parameters.
  for (size_t i = 1; i < n_rows + 1; ++i)
  {
    // Now we iterate through each row, or point, of the data.
    for (size_t j = 0; j < n_cols; ++j)
    {
      // Increment each prediction value by x_i * a_i, or the next dimensional
      // coefficient and x value.
      predictions(j) += parameters(i) * points(i - 1, j);

    }
  }
}

arma::vec LinearRegression::getParameters()
{
  return parameters;
}


bool LinearRegression::load(const std::string& filename)
{
  return data::Load(filename, parameters);
}

bool LinearRegression::save(const std::string& filename)
{
  return data::Save(filename, parameters);
}

}; // namespace linear_regression
}; // namespace mlpack