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

        void Update(const UsdBrokerNotice::ObjectsChanged& notice) {
            //Remove Descendants
            SdfPathVector resynced = notice.GetResyncedPaths();
            SdfPath::RemoveDescendentPaths(&resynced);

            for(auto& p : resynced) {
                bool was_created;
                TfRefPtr<Node> resynced_node = FindOrCreateNode(p, &was_created);
                if(!was_created) {
                    Sync(resynced_node, _stage->GetPrimAtPath(p));
                }
            }
        }

        void Clear() {
            added.clear();
            removed.clear();
        }

    private:
        // for string delimiter
        std::vector<std::string> split (const std::string& s, const std::string& delimiter) {
            size_t pos_start = 0, pos_end, delim_len = delimiter.length();
            std::string token;
            std::vector<std::string> res;

            while ((pos_end = s.find (delimiter, pos_start)) != std::string::npos) {
                token = s.substr (pos_start, pos_end - pos_start);
                pos_start = pos_end + delim_len;
                res.push_back (token);
            }

            res.push_back (s.substr (pos_start));
            return res;
        }

        TfRefPtr<Node> FindOrCreateNode(const SdfPath& path, bool* was_created) {
            std::vector<std::string> split_paths = split(path.GetString(), "/");
            TfRefPtr<Node> curr_node = _root;
            //TODO: make this sdfpath
            std::string partial_path = "";
            for(std::string& p : split_paths) {
                partial_path += "/" + p;
                TfToken p_token = TfToken(p);
                if(curr_node->children.count(p_token)) {
                    UsdPrim child = _stage->GetPrimAtPath(SdfPath(partial_path));
                    curr_node->children[p_token] = TfCreateRefPtr(new Node(child));
                    added.push_back(SdfPath(partial_path));
                    *was_created = true;
                    return curr_node->children[p_token];
                }
                curr_node = curr_node->children[p_token];
            }
            *was_created = false;
            return curr_node;
        }

        void Sync(TfRefPtr<Node> node, const UsdPrim& prim) {
            modified.push_back(prim.GetPrimPath());
            //Used to track nodes that exist in tree but not in stage
            std::unordered_set<SdfPath, SdfPath::Hash> node_children_copy;
                for(auto& child : node->children) {
                    node_children_copy.insert(child.second->prim_path);
                }

                //Loop over real prim's children
                for(const auto& child_prim : prim.GetChildren()) {
                    TfToken child_name = child_prim.GetName();
                    if(node->children.count(child_name)) {
                        //Sync on child
                        Sync(node->children[child_name], child_prim);
                        //Remove child from temporary list
                        node_children_copy.erase(child_prim.GetPath());
                    } else {
                        //Doesn't exist in tree
                        node->children[child_prim.GetName()] = TfCreateRefPtr(new Node(child_prim));
                        added.push_back(child_prim.GetPath());
                    }
                }

                //Add the non-existant children to the removed set
                for (auto& child_prim_path : node_children_copy) {
                    node->children.erase(TfToken(child_prim_path.GetName()));
                    removed.push_back(child_prim_path);
                }
            }

        TfRefPtr<Node> _root;
        SdfPathVector added;
        SdfPathVector removed;
        SdfPathVector modified;
        UsdStageWeakPtr _stage;

};

} // namespace unf

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROKER_STAGE_CACHE_H