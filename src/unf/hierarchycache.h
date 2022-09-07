#ifndef NOTICE_BROKER_HIERARCHY_CACHE_H
#define NOTICE_BROKER_HIERARCHY_CACHE_H

#include <pxr/pxr.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/base/tf/notice.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/base/tf/anyWeakPtr.h>
#include <pxr/base/tf/refPtr.h>
#include "pxr/usd/sdf/path.h"

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>

namespace unf {

struct Node;
using NodeRefPtr = PXR_NS::TfRefPtr<Node>;
using UnorderedSdfPathSet = std::unordered_set<PXR_NS::SdfPath, PXR_NS::SdfPath::Hash>;

struct Node : public PXR_NS::TfRefBase {
    Node(const UsdPrim& prim){
        prim_path = prim.GetPath();
        for (const auto& child : prim.GetChildren()) {
            children[child.GetName()] = PXR_NS::TfCreateRefPtr(new Node(child));
        }
    }
    PXR_NS::SdfPath prim_path;
    std::unordered_map<PXR_NS::TfToken, NodeRefPtr, PXR_NS::TfToken::HashFunctor> children;
};

class HierarchyCache : public PXR_NS::TfRefBase, PXR_NS::TfWeakBase {
    public:
        HierarchyCache(const PXR_NS::UsdStageWeakPtr stage) : _stage(stage) {
            _root = PXR_NS::TfCreateRefPtr(new Node(stage->GetPseudoRoot()));
        }

        void Update(SdfPathVector resynced);

        bool FindNode(const PXR_NS::SdfPath& path);

        bool DidCacheChange() const {
            return _added.size() != 0 || _removed.size() != 0 || _modified.size() != 0;
        }

        const UnorderedSdfPathSet& GetAdded() const {
            return _added;
        }

        const UnorderedSdfPathSet& GetRemoved() const {
            return _removed;
        }

        const UnorderedSdfPathSet& GetModified() const {
            return _modified;
        }

        UnorderedSdfPathSet&& TakeAdded() {
            return std::move(_noDescAdded);
        }

        UnorderedSdfPathSet&& TakeRemoved() {
            return std::move(_noDescRemoved);
        }
        
        UnorderedSdfPathSet&& TakeModified() {
            return std::move(_noDescModified);
        }

        void Clear() {
            _added.clear();
            _removed.clear();
            _modified.clear();
            _noDescAdded.clear();
            _noDescRemoved.clear();
            _noDescModified.clear();
        }

    private:
        // for string delimiter
        std::vector<std::string> _split (const std::string& s, const std::string& delimiter);

        void _addToRemoved(NodeRefPtr node);

        void _addToAdded(NodeRefPtr node);

        NodeRefPtr _findNodeOrUpdate(const PXR_NS::SdfPath& path);

        void _sync(NodeRefPtr node, const PXR_NS::UsdPrim& prim);


        NodeRefPtr _root;
        UnorderedSdfPathSet _added;
        UnorderedSdfPathSet _removed;
        UnorderedSdfPathSet _modified;
        UnorderedSdfPathSet _noDescAdded;
        UnorderedSdfPathSet _noDescRemoved;
        UnorderedSdfPathSet _noDescModified;
        PXR_NS::UsdStageWeakPtr _stage;

};

} // namespace unf

#endif // NOTICE_BROKER_HIERARCHY_CACHE_H