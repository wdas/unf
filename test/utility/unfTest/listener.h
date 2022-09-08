#ifndef TEST_NOTICE_BROKER_LISTENER_H
#define TEST_NOTICE_BROKER_LISTENER_H

#include "testBroadcaster.h"

#include "unf/hierarchycache.h"
#include "unf/broadcasterNotice.h"

#include <pxr/pxr.h>
#include <pxr/base/tf/notice.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/usd/usd/stage.h>

#include <string>
#include <unordered_map>
#include <typeinfo>
#include <iostream>

namespace Test {
using UnorderedSdfPathSet = std::unordered_set<SdfPath, SdfPath::Hash>;
using ChangedFieldMap = unf::ChangedFieldMap;

// Interface to examine content of notice received.
template <class T>
class ListenerBase : public PXR_NS::TfWeakBase
{
public:
    ListenerBase(const PXR_NS::UsdStageWeakPtr &stage) {
        auto self = PXR_NS::TfCreateWeakPtr(this);
        _key =  PXR_NS::TfNotice::Register(
            PXR_NS::TfCreateWeakPtr(this),
            &ListenerBase::OnReceiving,
            stage);
    }

    virtual ~ListenerBase() { PXR_NS::TfNotice::Revoke(_key); }

private:
    virtual void OnReceiving(const T&, const PXR_NS::UsdStageWeakPtr&) =0;

    PXR_NS::TfNotice::Key _key;
};

// Container to listen to several types of Tf notices.
template <class... Types>
class Listener : public PXR_NS::TfWeakBase
{
public:
    Listener() = default;
    Listener(const PXR_NS::UsdStageWeakPtr &stage) {
        SetStage(stage);
    }

    virtual ~Listener() {
        for (auto& element: _keys) {
            PXR_NS::TfNotice::Revoke(element.second);
        }
    }

    void SetStage(const PXR_NS::UsdStageWeakPtr &stage)
    {
        auto self = PXR_NS::TfCreateWeakPtr(this);
        _keys = std::unordered_map<std::string, PXR_NS::TfNotice::Key>({
            _Register<Types>(self, stage)...
        });
    }

    template <class T>
    size_t Received()
    {
        std::string name = typeid(T).name();
        if (_received.find(name) == _received.end())
            return 0;

        return _received.at(name);
    }

    void Reset()
    {
        for (auto& element: _received) {
            element.second = 0;
        }
    }

private:
    template<class T>
    std::pair<std::string, PXR_NS::TfNotice::Key> _Register(
        const PXR_NS::TfWeakPtr<Listener>& self,
        const PXR_NS::UsdStageWeakPtr &stage)
    {
        auto cb = &Listener::_Callback<T>;
        std::string name = typeid(T).name();
        auto key =  PXR_NS::TfNotice::Register(self, cb, stage);

        return std::make_pair(name, key);
    }

    template<class T>
    void _Callback(const T& notice, const PXR_NS::UsdStageWeakPtr &sender)
    {
        std::string name = typeid(T).name();

        if (_received.find(name) == _received.end())
            _received[name] = 0;

        _received[name] += 1;
    }

    std::unordered_map<std::string, PXR_NS::TfNotice::Key> _keys;
    std::unordered_map<std::string, size_t> _received;
};

// Usd ObjectsChanged notice listener
class ObjChangedListener : public TfWeakBase {
    public:
        ObjChangedListener(unf::HierarchyCache* c) : _cache(c) {
            _key = TfNotice::Register(TfCreateWeakPtr(this), &ObjChangedListener::_CallBack);
        }
        ~ObjChangedListener() {
            PXR_NS::TfNotice::Revoke(_key);
        }
    
    private:
        void _CallBack(const UsdNotice::ObjectsChanged& notice) {
            SdfPathVector resyncedChanges;
            for (const auto& path: notice.GetResyncedPaths()) {
                resyncedChanges.push_back(path);
            }
            _cache->Update(resyncedChanges);
        }

        TfNotice::Key _key;
        unf::HierarchyCache* _cache;
 };

// unf ObjectsChanged notice listener
class unfObjChangedListener : public TfWeakBase {
    public:
        unfObjChangedListener(unf::HierarchyCache* c) : _cache(c) {
            _key = TfNotice::Register(TfCreateWeakPtr(this), &unfObjChangedListener::_CallBack);
        }
        ~unfObjChangedListener() {
            PXR_NS::TfNotice::Revoke(_key);
        }
    
    private:
        void _CallBack(const unf::BrokerNotice::ObjectsChanged& notice) {
            _cache->Update(notice.GetResyncedPaths());
        }

        TfNotice::Key _key;
        unf::HierarchyCache* _cache;
 };

// unf HierarchyChanged notice listener
class HierarchyChangedListener : public TfWeakBase {
    public:
        HierarchyChangedListener(){
            _key = TfNotice::Register(TfCreateWeakPtr(this), &HierarchyChangedListener::_CallBack);
            _count = 0;
        }
        ~HierarchyChangedListener() {
            PXR_NS::TfNotice::Revoke(_key);
        }

        const UnorderedSdfPathSet& GetAdded() const{
            return _added;
        }
        const UnorderedSdfPathSet& GetRemoved() const{
            return _removed;
        }
        const UnorderedSdfPathSet& GetModified() const{
            return _modified;
        }
        const ChangedFieldMap& GetChangedFields() const {
            return _changedFields;
        }

        int GetCount() const {
            return _count;
        }

        void ResetCount() {
            _count = 0;
        }
    
    
    private:
        void _CallBack(const unf::BroadcasterNotice::HierarchyChanged& notice) {
            _added = notice.GetAdded();
            _removed = notice.GetRemoved();
            _modified = notice.GetModified();
            _changedFields = notice.GetChangedFields();
            _count ++;
        }

        TfNotice::Key _key;
        UnorderedSdfPathSet _added;
        UnorderedSdfPathSet _removed;
        UnorderedSdfPathSet _modified;
        ChangedFieldMap _changedFields;
        int _count;
 };

 // ::Test:: ChildBroadcasterNotice notice listener
class ChildBroadcasterNoticeListener : public TfWeakBase {
    public:
        ChildBroadcasterNoticeListener(){
            _key = TfNotice::Register(TfCreateWeakPtr(this), &ChildBroadcasterNoticeListener::_CallBack);
            _count = 0;
        }
        ~ChildBroadcasterNoticeListener() {
            PXR_NS::TfNotice::Revoke(_key);
        }

        const UnorderedSdfPathSet& GetAdded() const{
            return _added;
        }
        const UnorderedSdfPathSet& GetRemoved() const{
            return _removed;
        }
        const UnorderedSdfPathSet& GetModified() const{
            return _modified;
        }
        const ChangedFieldMap& GetChangedFields() const {
            return _changedFields;
        }

        int GetCount() const {
            return _count;
        }

        void ResetCount() {
            _count = 0;
        }
    
    private:
        void _CallBack(const ChildBroadcasterNotice& notice) {
            _added = notice.GetAdded();
            _removed = notice.GetRemoved();
            _modified = notice.GetModified();
            _changedFields = notice.GetChangedFields();
            _count ++;
        }

        TfNotice::Key _key;
        UnorderedSdfPathSet _added;
        UnorderedSdfPathSet _removed;
        UnorderedSdfPathSet _modified;
        ChangedFieldMap _changedFields;
        int _count;
 };
} // namespace Test

#endif // TEST_NOTICE_BROKER_LISTENER_H
