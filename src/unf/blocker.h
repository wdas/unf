#ifndef USD_NOTICE_FRAMEWORK_BLOCKER_H
#define USD_NOTICE_FRAMEWORK_BLOCKER_H

#include "unf/broker.h"

#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>

namespace unf {

class NoticeBlocker {
  public:
    NoticeBlocker(const BrokerPtr &);
    NoticeBlocker(const PXR_NS::UsdStageRefPtr &);

    ~NoticeBlocker();

    // Don't allow copies
    NoticeBlocker(const NoticeBlocker &) = delete;
    NoticeBlocker &operator=(const NoticeBlocker &) = delete;

    BrokerPtr GetBroker() { return _broker; }

  private:
    BrokerPtr _broker;
};

}  // namespace unf

#endif  // USD_NOTICE_FRAMEWORK_BLOCKER_H
