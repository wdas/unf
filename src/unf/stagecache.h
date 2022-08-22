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

        void Update(SdfPathVector resynced) {
            //Remove Descendants
            SdfPath::RemoveDescendentPaths(&resynced);

            //Turn into set
            resyncedSet = UnorderedSdfPathSet{resynced.begin(), resynced.end()};

            UnorderedSdfPathSet syncRoots;
            
            for(auto& p : resynced) {
                TfRefPtr<Node> ancestor_node = FindNodeOrAncestor(p);
                syncRoots.insert(ancestor_node->prim_path);
            }
            
            SdfPathVector syncRootsFiltered(syncRoots.begin(), syncRoots.end()); 
            SdfPath::RemoveDescendentPaths(&syncRootsFiltered);
            for(auto& p : syncRootsFiltered) {
                Sync(FindNodeOrAncestor(p), _stage->GetPrimAtPath(p));
            }
        }

        bool FindNode(const SdfPath& path) {
            std::vector<std::string> split_paths = _split(path.GetString(), "/");
            TfRefPtr<Node> curr_node = _root;
            //TODO: make this sdfpath
            for(size_t i = 1; i < split_paths.size(); i++) {
                TfToken p_token = TfToken(split_paths[i]);
                if(!curr_node->children.count(p_token)) {
                    return false;
                }
                curr_node = curr_node->children[p_token];
            }
           return true;
        }

        const UnorderedSdfPathSet& GetAdded() const {
            return added;
        }

        const UnorderedSdfPathSet& GetRemoved() const {
            return removed;
        }

        const UnorderedSdfPathSet& GetModified() const {
            return modified;
        }

        void Clear() {
            added.clear();
            removed.clear();
            modified.clear();
        }

    private:
        // for string delimiter
        std::vector<std::string> _split (const std::string& s, const std::string& delimiter) {
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

        void _addToRemoved(TfRefPtr<Node> node) {
            removed.insert(node->prim_path);

            for(auto c : node->children) {
                _addToRemoved(c.second);
            }
        }

        void _addToAdded(TfRefPtr<Node> node) {
            added.insert(node->prim_path);

            for(auto c : node->children) {
                _addToAdded(c.second);
            }
        }

        TfRefPtr<Node> FindNodeOrAncestor(const SdfPath& path) {
            std::vector<std::string> split_paths = _split(path.GetString(), "/");
            TfRefPtr<Node> curr_node = _root;
            //TODO: make this sdfpath
            std::string partial_path = "";
            for(size_t i = 1; i < split_paths.size(); i++) {
                partial_path += "/" + split_paths[i];
                TfToken p_token = TfToken(split_paths[i]);
                UsdPrim child = _stage->GetPrimAtPath(SdfPath(partial_path));
                if (!child || !curr_node->children.count(p_token)) {
                    return curr_node;
                }
                curr_node = curr_node->children[p_token];
            }
            return curr_node;
        }

        void Sync(TfRefPtr<Node> node, const UsdPrim& prim, bool mark_modified = false) {
            if (resyncedSet.find(node->prim_path)!= resyncedSet.end()) {
                mark_modified = true;
            }
            if (mark_modified) {
                modified.insert(node->prim_path);
            }
            //Used to track nodes that exist in tree but not in stage
            std::unordered_set<SdfPath, SdfPath::Hash> node_children_copy;
            for(auto& child : node->children) {
                node_children_copy.insert(child.second->prim_path);
            }

            //Loop over real prim's children
            for(const auto& child_prim : prim.GetChildren()) {
                TfToken child_name = child_prim.GetName();
                if(node->children.count(child_name)) {
                    //Exists in cache
                    //Sync on child and on stage
                    Sync(node->children[child_name], child_prim, mark_modified);
                    //Remove child from temporary list
                    node_children_copy.erase(child_prim.GetPath());
                } else {
                    //Doesn't exist in cache but in stage
                    //Doesn't exist in tree
                    node->children[child_prim.GetName()] = TfCreateRefPtr(new Node(child_prim));
                    _addToAdded(node->children[child_prim.GetName()]);

                }
            }
            
            //Exists only on cache, not on stage
            //Add the non-existant children to the removed set
            for (auto& child_prim_path : node_children_copy) {
                TfToken childName = TfToken(child_prim_path.GetName());
                _addToRemoved(node->children[childName]);
                node->children.erase(childName);
            }
        }

        TfRefPtr<Node> _root;
        UnorderedSdfPathSet added;
        UnorderedSdfPathSet removed;
        UnorderedSdfPathSet modified;
        UnorderedSdfPathSet resyncedSet;
        UsdStageWeakPtr _stage;

};

} // namespace unf

PXR_NAMESPACE_CLOSE_SCOPE

#endif // NOTICE_BROKER_STAGE_CACHE_H