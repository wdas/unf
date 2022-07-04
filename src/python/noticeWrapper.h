#ifndef NOTICE_BROKER_NOTICE_WRAPPER_H
#define NOTICE_BROKER_NOTICE_WRAPPER_H

#include "notice.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/notice.h>
#include <pxr/usd/usd/notice.h>

PXR_NAMESPACE_OPEN_SCOPE

/*
TfNotices could not be passed by TfRefPtrs through Python. We needed a workaround for the issue.
The solution was the NoticeWrapper interface that gives back a TfRefPtr of the notice.
*/
class NoticeWrapper : public TfRefBase, public TfWeakBase {
public:
    NoticeWrapper(){};

    virtual TfRefPtr<UsdBrokerNotice::StageNotice> Get(){return nullptr;}

    virtual boost::python::object GetWrap() { return {}; }

    virtual void Send() {}
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROKER_NOTICE_WRAPPER/
