#ifndef USD_NOTICE_FRAMEWORK_CAPTURE_PREDICATE_H
#define USD_NOTICE_FRAMEWORK_CAPTURE_PREDICATE_H

/// \file unf/capturePredicate.h

#include "unf/notice.h"

#include <functional>
#include <vector>
#include <string>

namespace unf {

/// Function defining whether a UnfNotice::StageNotice notice
/// can be captured.
using CapturePredicateFunc =
    std::function<bool(const UnfNotice::StageNotice&)>;

class CapturePredicate
{
public:
    CapturePredicate(const CapturePredicateFunc&);

    bool operator()(const UnfNotice::StageNotice&) const;

    static CapturePredicate Default();
    static CapturePredicate BlockAll();

private:
    CapturePredicateFunc _function = nullptr;
};

}  // namespace unf

#endif  // USD_NOTICE_FRAMEWORK_CAPTURE_PREDICATE_H
