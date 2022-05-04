#ifndef NOTICE_BROKER_TEST_LISTENER_H
#define NOTICE_BROKER_TEST_LISTENER_H

#include "../notice.h"
#include "./notice.h"

#include "pxr/pxr.h"
#include "pxr/base/tf/notice.h"
#include "pxr/base/tf/weakBase.h"
#include "pxr/usd/usd/common.h"

#include <string>
#include <unordered_map>
#include <typeinfo>

class Listener : public PXR_NS::TfWeakBase
{
public:
    Listener(const PXR_NS::UsdStageWeakPtr &stage) {
        auto self = PXR_NS::TfCreateWeakPtr(this);

        _Register<PXR_NS::UsdBrokerNotice::StageNotice>(self, stage);
        _Register<PXR_NS::UsdBrokerNotice::StageContentsChanged>(self, stage);
        _Register<PXR_NS::UsdBrokerNotice::ObjectsChanged>(self, stage);
        _Register<PXR_NS::UsdBrokerNotice::StageEditTargetChanged>(self, stage);
        _Register<PXR_NS::UsdBrokerNotice::LayerMutingChanged>(self, stage);

        _Register<MergeableNotice>(self, stage);
        _Register<UnMergeableNotice>(self, stage);
    }

    void Reset()
    {
        for (auto it = _received.begin(); it != _received.end(); it++) {
            it->second = 0;
        }
    }

    int StageNotices()
    {
        std::string name = 
            typeid(PXR_NS::UsdBrokerNotice::StageNotice).name();
        return _received.at(name);
    }

    int StageContentsChangedNotices()
    {
        std::string name = 
            typeid(PXR_NS::UsdBrokerNotice::StageContentsChanged).name();
        return _received.at(name);
    }

    int ObjectsChangedNotices()
    {
        std::string name = 
            typeid(PXR_NS::UsdBrokerNotice::ObjectsChanged).name();
        return _received.at(name);
    }

    int StageEditTargetChangedNotices()
    {
        std::string name = 
            typeid(PXR_NS::UsdBrokerNotice::StageEditTargetChanged).name();
        return _received.at(name);
    }

    int LayerMutingChangedNotices()
    {
        std::string name = 
            typeid(PXR_NS::UsdBrokerNotice::LayerMutingChanged).name();
        return _received.at(name);
    }

    int CustomMergeableNotices()
    {
        std::string name = typeid(MergeableNotice).name();
        return _received.at(name);
    }

    int CustomUnMergeableNotices()
    {
        std::string name = typeid(UnMergeableNotice).name();
        return _received.at(name);
    }

private:
    template<class T>
    void _Register(
        const PXR_NS::TfWeakPtr<Listener>& self, 
        const PXR_NS::UsdStageWeakPtr &stage)
    {
        auto cb = &Listener::_Callback<T>;
        std::string name = typeid(T).name();

        _keys[name] = PXR_NS::TfNotice::Register(self, cb, stage);
        _received[name] = 0;
    }

    template<class T>
    void _Callback(const T& notice, const PXR_NS::UsdStageWeakPtr &sender)
    {
        _received[typeid(T).name()] += 1;
    }

    std::unordered_map<std::string, PXR_NS::TfNotice::Key> _keys;
    std::unordered_map<std::string, int> _received;
};

#endif // NOTICE_BROKER_TEST_LISTENER_H
