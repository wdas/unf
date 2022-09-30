#ifndef USD_NOTICE_FRAMEWORK_HIERARCHY_CACHE_H
#define USD_NOTICE_FRAMEWORK_HIERARCHY_CACHE_H

#include <pxr/base/tf/anyWeakPtr.h>
#include <pxr/base/tf/notice.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/pxr.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/stage.h>
#include "pxr/usd/sdf/path.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace unf{

struct Node;

using NodeRefPtr = PXR_NS::TfRefPtr<Node>;

struct Node : public PXR_NS::TfRefBase {
    Node(const PXR_NS::UsdPrim& prim);

    static NodeRefPtr CreateWithDescendants(const PXR_NS::UsdPrim& prim);

    // TODO: Does prim_path take any time to calculate when getting from prim?
    PXR_NS::SdfPath prim_path;
    PXR_NS::UsdPrim prim;
    std::unordered_map<
        PXR_NS::TfToken, NodeRefPtr, PXR_NS::TfToken::HashFunctor>
        children;
    int flag = 0;
};

class HierarchyCache : public PXR_NS::TfRefBase, PXR_NS::TfWeakBase {
  public:
    HierarchyCache(const PXR_NS::UsdStageWeakPtr stage);

    void Update(const PXR_NS::SdfPathVector& resynced);

    bool FindNode(const PXR_NS::SdfPath& path);

    bool DidCacheChange() const
    {
        return _added.size() != 0 || _removed.size() != 0
               || _modified.size() != 0;
    }

    const PXR_NS::SdfPathVector& GetAdded() const { return _added; }

    const PXR_NS::SdfPathVector& GetRemoved() const { return _removed; }

    const PXR_NS::SdfPathVector& GetModified() const { return _modified; }

    PXR_NS::SdfPathVector&& TakeAdded() { return std::move(_added); }

    PXR_NS::SdfPathVector&& TakeRemoved() { return std::move(_removed); }

    PXR_NS::SdfPathVector&& TakeModified()
    {
        return std::move(_modified);
    }

    void Clear()
    {
        _added.clear();
        _removed.clear();
        _modified.clear();
    }

  private:
    void _addToRemoved(NodeRefPtr& node);

    void _addToAdded(NodeRefPtr& node);

    NodeRefPtr _findNodeOrUpdate(const PXR_NS::SdfPath& path);

    void _sync(NodeRefPtr& node);

    NodeRefPtr _root;
    PXR_NS::SdfPathVector _added;
    PXR_NS::SdfPathVector _removed;
    PXR_NS::SdfPathVector _modified;
    PXR_NS::UsdStageWeakPtr _stage;
};

}  // namespace unfTest

#endif  // USD_NOTICE_FRAMEWORK_HIERARCHY_CACHE_H
