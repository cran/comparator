#pragma once

#include "PairwiseMatrix.h"
#include <Rcpp.h>

template<class Range> 
class Comparator {
protected:
  bool symmetric_;
  bool distance_;
  bool similarity_;
public:
  Comparator(bool symmetric, bool distance, bool similarity) : 
  symmetric_(symmetric), 
  distance_(distance), 
  similarity_(similarity) {}
  
  virtual double eval(const Range& x, const Range& y) const = 0;
  
  template <class ForwardIterator1, class ForwardIterator2>
  PairwiseMatrix pairwise(ForwardIterator1 first1, ForwardIterator1 last1, 
                          ForwardIterator2 first2, ForwardIterator2 last2) const;
  
  template <class ForwardIterator>
  PairwiseMatrix pairwise(ForwardIterator first1, ForwardIterator last1) const;
  
  template <class ForwardIterator1, class ForwardIterator2>
  std::vector<double> elementwise(ForwardIterator1 first1, ForwardIterator1 last1, 
                                  ForwardIterator2 first2, ForwardIterator2 last2) const;
  
  bool is_symmetric() const { return symmetric_; }
  
  bool is_distance() const { return distance_; }
  
  bool is_similarity() const { return similarity_; }
  
  virtual ~Comparator() = default;
};

template<class Range> 
class NormalizableComparator : public Comparator<Range> {
protected:
  bool normalize_;
public:
  NormalizableComparator(bool normalize, bool symmetric, bool distance, bool similarity) : 
  Comparator<Range>(symmetric, distance, similarity),
  normalize_(normalize) {}
  
  bool normalized() const { return normalize_; }
};


// Returns false if the range x is comparable with another range, e.g. because it contains missing values. 
template<class Range>
bool is_incomparable(Range x) {
  return false;
}

// Deal with incomparable NA values in R vectors
template<int RTYPE>
bool is_incomparable(Rcpp::internal::generic_proxy<RTYPE> x) {
  SEXP xx(x);  
  switch (TYPEOF(xx)) {
  case INTSXP: {
    Rcpp::IntegerVector y(xx);
    return Rcpp::any(Rcpp::is_na(y));
  }
  case REALSXP: {
    Rcpp::NumericVector y(xx);
    return Rcpp::any(Rcpp::is_na(y));
  }
  // case STRSXP: {
  //   CharacterVector y;
  //   return Rcpp::any(Rcpp::is_na(y));
  // }
  case LGLSXP: {
    Rcpp::LogicalVector y(xx);
    return Rcpp::any(Rcpp::is_na(y));
  }
  default: {
    return false;
  }
  }
} 

template <class RangeIterator>
template <class ForwardIterator1, class ForwardIterator2>
PairwiseMatrix Comparator<RangeIterator>::pairwise(ForwardIterator1 first1, ForwardIterator1 last1, 
                                                ForwardIterator2 first2, ForwardIterator2 last2) const {
  ForwardIterator1 curr1;
  size_t nrow = std::distance(first1, last1);
  size_t ncol = std::distance(first2, last2);
  
  PairwiseMatrix result(nrow, ncol, true, true);
  auto itresult = result.begin();
  while (first2 != last2) {
    curr1 = first1;
    while (curr1 != last1) {
      if (is_incomparable(*curr1) || is_incomparable(*first2)) {
        *itresult = R_NaReal;
      } else {
        *itresult = eval(*curr1, *first2);
      }
      ++itresult;
      ++curr1;
    }
    ++first2;
  }
  return result;
}


template <class RangeIterator>
template <class ForwardIterator>
PairwiseMatrix Comparator<RangeIterator>::pairwise(ForwardIterator first1, 
                                                ForwardIterator last1) const {
  if (!is_symmetric()) {
    return pairwise(first1, last1, first1, last1);
  } else {
    // only need to compute lower triangle
    ForwardIterator curr1;

    size_t n = std::distance(first1, last1);
    PairwiseMatrix result(n, n, false, !is_distance());
    auto itresult = result.begin();
    while (first1 != last1) {
      curr1 = first1;
      if (is_distance()) ++curr1; // only need to compute lower triangle
      while (curr1 != last1) {
        if (is_incomparable(*curr1) || is_incomparable(*first1)) {
          *itresult = R_NaReal;
        } else {
          *itresult = eval(*curr1, *first1);
        }
        ++itresult;
        ++curr1;
      }
      ++first1;
    }
    return result;
  }
}


template <class RangeIterator>
template <class ForwardIterator1, class ForwardIterator2>
std::vector<double> Comparator<RangeIterator>::elementwise(ForwardIterator1 first1, ForwardIterator1 last1, 
                                                        ForwardIterator2 first2, ForwardIterator2 last2) const {
  std::vector<double> result;
  std::size_t size1 = std::distance(first1, last1);
  std::size_t size2 = std::distance(first2, last2);
  
  // Can't compare if either range is empty
  if (size1 == 0 || size2 == 0) return result;
  
  // Ensure first range is larger than second
  if (size2 > size1) {
    std::swap(first1, first2);
    std::swap(last1, last2);
    std::swap(size1, size2);
  }
  
  result.resize(size1);
  auto itresult = result.begin();

  // Iterate over first range, recycling elements from second range
  ForwardIterator2 curr2 = first2;
  while (first1 != last1) {
    if (curr2 == last2) curr2 = first2; // recycle
    if (is_incomparable(*first1) || is_incomparable(*curr2)) {
      *itresult = R_NaReal;
    } else {
      *itresult = eval(*first1, *curr2);
    }
    ++itresult;
    ++curr2;
    ++first1;
  }

  return result;
}
