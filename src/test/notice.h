#ifndef NOTICE_BROKER_TEST_NOTICE_H
#define NOTICE_BROKER_TEST_NOTICE_H

#include "../notice.h"

#include "pxr/pxr.h"
#include "pxr/base/tf/notice.h"
#include "pxr/usd/usd/notice.h"

class MergeableNotice : public PXR_NS::UsdBrokerNotice::StageNotice
{
public:
    MergeableNotice() {}
    virtual ~MergeableNotice() {}
};

class UnMergeableNotice : public PXR_NS::UsdBrokerNotice::StageNotice
{
public:
    UnMergeableNotice() {}
    virtual ~UnMergeableNotice() {}

    virtual bool IsMergeable() const { return false; }
};

PXR_NAMESPACE_OPEN_SCOPE

TF_REGISTRY_FUNCTION(TfType)
{
    TfType::Define<
        MergeableNotice,
        TfType::Bases<UsdBrokerNotice::StageNotice> >();

    TfType::Define<
        UnMergeableNotice,
        TfType::Bases<UsdBrokerNotice::StageNotice> >();
}

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROKER_TEST_NOTICE_H
