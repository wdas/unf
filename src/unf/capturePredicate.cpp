#include "unf/capturePredicate.h"

#include <typeinfo>
#include <functional>
#include <vector>
#include <string>
#include <algorithm>

namespace unf {

CapturePredicate::CapturePredicate(const CapturePredicateFunc& function)
    : _function(function)
{

}

bool CapturePredicate::operator()(
    const BrokerNotice::StageNotice& notice) const
{
    if (!_function) return true;
    return _function(notice);
}

CapturePredicate CapturePredicate::Default()
{
    auto function = [](const BrokerNotice::StageNotice&) { return true; };
    return CapturePredicate(function);
}

CapturePredicate CapturePredicate::BlockAll()
{
    auto function = [](const BrokerNotice::StageNotice&) { return false; };
    return CapturePredicate(function);
}

CapturePredicate CapturePredicate::BlockType(std::string noticeType)
{
    auto function = [&](const BrokerNotice::StageNotice& notice) {
        return (notice.GetTypeId() != noticeType);
    };

    return CapturePredicate(function);
}

CapturePredicate CapturePredicate::BlockTypes(
    const std::vector<std::string>& noticeTypes)
{
    auto function = [&](const BrokerNotice::StageNotice& notice) {
        auto it = std::find_if(
            noticeTypes.begin(), noticeTypes.end(),
            [&](std::string noticeType) {
                return notice.GetTypeId() == noticeType;
            }
        );

        return it == noticeTypes.end();
    };

    return CapturePredicate(function);
}

}  // namespace unf
