#ifndef USD_NOTICE_FRAMEWORK_CAPTURE_PREDICATE_H
#define USD_NOTICE_FRAMEWORK_CAPTURE_PREDICATE_H

/// \file unf/capturePredicate.h

#include "unf/api.h"
#include "unf/notice.h"

#include <functional>
#include <string>
#include <vector>

namespace unf {

/// Convenient alias for function defining whether notice can be captured.
using CapturePredicateFunc = std::function<bool(const UnfNotice::StageNotice&)>;

/// \class CapturePredicate
///
/// \brief
/// Predicate functor which indicates whether a notice can be captured during
/// a transaction.
///
/// Common predicates are provided as static methods for convenience.
///
/// \note
/// We used a functor embedding a CapturePredicateFunc instead of defining
/// common predicates via free functions to simplify the Python binding process.
class CapturePredicate {
  public:
    /// \brief
    /// Create predicate from a \p function.
    ///
    /// The following example will create a predicate which will return false
    /// for a 'Foo' notice.
    ///
    /// \code{.cpp}
    /// CapturePredicate([&](const unf::UnfNotice::StageNotice& n) {
    ///     return (n.GetTypeId() != typeid(Foo).name());
    /// });
    /// \endcode
    UNF_API CapturePredicate(const CapturePredicateFunc&);

    /// Invoke boolean predicate on UnfNotice::StageNotice \p notice.
    UNF_API bool operator()(const UnfNotice::StageNotice&) const;

    /// Create a predicate which return true for each notice type.
    UNF_API static CapturePredicate Default();

    /// Create a predicate which return false for each notice type.
    UNF_API static CapturePredicate BlockAll();

  private:
    CapturePredicateFunc _function = nullptr;
};

}  // namespace unf

#endif  // USD_NOTICE_FRAMEWORK_CAPTURE_PREDICATE_H
