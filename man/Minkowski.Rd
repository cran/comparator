% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/Minkowski.R
\name{Minkowski}
\alias{Minkowski}
\title{Minkowski Numeric Comparator}
\usage{
Minkowski(p = 2)
}
\arguments{
\item{p}{a positive numeric specifying the order of the distance. Defaults
to 2 (Euclidean distance). If \code{p < 1} the Minkowski distance does not
satisfy the triangle inequality and is therefore not a proper distance
metric.}
}
\value{
A \code{Minkowski} instance is returned, which is an S4 class inheriting
from \code{\linkS4class{NumericComparator}}.
}
\description{
The Minkowski distance (a.k.a. L-p distance) between two vectors \eqn{x} and
\eqn{y} is the p-th root of the sum of the absolute differences of their
Cartesian coordinates raised to the p-th power:
\deqn{\mathrm{Minkowski}(x,y) = \left(\sum_{i = 1}^{n} |x_i - y_i|^p\right)^{1/p}.}{Minkowski(x,y) = (sum_i { |x_i - y_i|^p })^(1/p).}
}
\examples{
## Distance between two vectors
x <- c(0, 1, 0, 1, 0)
y <- seq_len(5)
Minkowski()(x, y)

## Distance between rows (elementwise) of two matrices
comparator <- Minkowski()
x <- matrix(rnorm(25), nrow = 5)
y <- matrix(rnorm(5), nrow = 1)
elementwise(comparator, x, y)

## Distance between rows (pairwise) of two matrices
pairwise(comparator, x, y)

}
\seealso{
Other numeric comparators include \code{\link{Manhattan}}, \code{\link{Euclidean}} and
\code{\link{Chebyshev}}.
}
