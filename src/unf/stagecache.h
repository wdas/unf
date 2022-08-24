#ifndef NOTICE_BROKER_STAGE_CACHE_H
#define NOTICE_BROKER_STAGE_CACHE_H

#include <pxr/pxr.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/base/tf/notice.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/base/tf/anyWeakPtr.h>
#include <pxr/base/tf/refPtr.h>

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>

PXR_NAMESPACE_OPEN_SCOPE

namespace unf {
using UnorderedSdfPathSet = std::unordered_set<SdfPath, SdfPath::Hash>;

struct Node : public TfRefBase {
    Node(const UsdPrim& prim){
        prim_path = prim.GetPath();
        for (const auto& child : prim.GetChildren()) {
            children[child.GetName()] = TfCreateRefPtr(new Node(child));
        }
    }
    SdfPath prim_path;
    std::unordered_map<TfToken, TfRefPtr<Node>, TfToken::HashFunctor> children;
};

class Cache : public TfRefBase, TfWeakBase {
    public:
        Cache(const UsdStageWeakPtr stage) : _stage(stage) {
            _root = TfCreateRefPtr(new Node(stage->GetPseudoRoot()));
        }

        void Update(SdfPathVector resynced);

        bool FindNode(const SdfPath& path);

        const UnorderedSdfPathSet& GetAdded() const {
            return _added;
        }

        const UnorderedSdfPathSet& GetRemoved() const {
            return _removed;
        }

        const UnorderedSdfPathSet& GetModified() const {
            return _modified;
        }

        void Clear() {
            _added.clear();
            _removed.clear();
            _modified.clear();
        }

    private:
        // for string delimiter
        std::vector<std::string> _split (const std::string& s, const std::string& delimiter);

        void _addToRemoved(TfRefPtr<Node> node);

        void _addToAdded(TfRefPtr<Node> node);

        TfRefPtr<Node> _findNodeOrUpdate(const SdfPath& path);

        void _sync(TfRefPtr<Node> node, const UsdPrim& prim);


        TfRefPtr<Node> _root;
        UnorderedSdfPathSet _added;
        UnorderedSdfPathSet _removed;
        UnorderedSdfPathSet _modified;
        UsdStageWeakPtr _stage;

};

} // namespace unf

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROKER_STAGE_CACHE_H