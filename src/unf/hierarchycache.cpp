#include "hierarchycache.h"

PXR_NAMESPACE_USING_DIRECTIVE

namespace {
    std::vector<std::string> _split (const std::string& s, const std::string& delimiter) {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        std::string token;
        std::vector<std::string> res;

        while ((pos_end = s.find (delimiter, pos_start)) != std::string::npos) {
            token = s.substr (pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);
        }

        res.push_back(s.substr (pos_start));
        return res;
    }
}

namespace unf {
    void HierarchyCache::Update(SdfPathVector resynced) {
        //Remove Descendants
        SdfPath::RemoveDescendentPaths(&resynced);
        
        for (auto& p : resynced) {
            NodeRefPtr node = _findNodeOrUpdate(p);
            if (node){
                _noDescModified.push_back(p);
                _sync(node, _stage->GetPrimAtPath(p));
            }
        }
    }

    bool HierarchyCache::FindNode(const SdfPath& path) {
        std::vector<std::string> split_paths = _split(path.GetString(), "/");
        NodeRefPtr curr_node = _root;
        for (size_t i = 1; i < split_paths.size(); i++) {
            TfToken p_token = TfToken(split_paths[i]);
            if (curr_node->children.find(p_token) == curr_node->children.end()) {
                return false;
            }
            curr_node = curr_node->children[p_token];
        }
        return true;
    }

    void HierarchyCache::_addToRemoved(NodeRefPtr node) {
            _removed.insert(node->prim_path);

            for (auto c : node->children) {
                _addToRemoved(c.second);
            }
        }

    void HierarchyCache::_addToAdded(NodeRefPtr node) {
        _added.insert(node->prim_path);

        for (auto c : node->children) {
            _addToAdded(c.second);
        }
    }

    NodeRefPtr HierarchyCache::_findNodeOrUpdate(const SdfPath& path) {
        if (path == _stage->GetPseudoRoot().GetPath()) {
            return _root;
        }
        std::vector<std::string> split_paths = _split(path.GetString(), "/");
        NodeRefPtr curr_node = _root;
        //TODO: make this sdfpath
        std::string partial_path = "";
        for (size_t i = 1; i < split_paths.size(); i++) {
            partial_path += "/" + split_paths[i];
            TfToken p_token = TfToken(split_paths[i]);
            auto child_token = curr_node->children.find(p_token);
            //If we are at the final node of the path, then this is the resyncedpath.
            if (i == split_paths.size() - 1) {
                SdfPath prim_path = SdfPath(partial_path);
                UsdPrim child = _stage->GetPrimAtPath(prim_path);
                //Doesn't exit in stage nor cache -- don't do anything. 
                if (!child && child_token == curr_node->children.end()) {
                    return nullptr;
                }
                //Doesn't exist in the stage -- then the prim was removed.
                else if (!child) {
                    _addToRemoved(curr_node->children[p_token]);
                    _noDescRemoved.push_back(prim_path);
                    curr_node->children.erase(p_token);
                    return nullptr;
                }
            }
            if (child_token == curr_node->children.end()) {
                SdfPath prim_path = SdfPath(partial_path);
                UsdPrim child = _stage->GetPrimAtPath(prim_path);
                //Doesn't exist in the cache but in stage -- need to add prim
                if (child){
                    curr_node->children[p_token] = TfCreateRefPtr(new Node(child));
                    _addToAdded(curr_node->children[p_token]);
                    _noDescAdded.push_back(prim_path);
                }
                return nullptr;
            }
            curr_node = child_token->second;
        }
        return curr_node;
    }

    void HierarchyCache::_sync(NodeRefPtr node, const UsdPrim& prim) {
        if (node->prim_path != _stage->GetPseudoRoot().GetPath()) {
            _modified.insert(node->prim_path);
        }
        //Used to track nodes that exist in tree but not in stage
        std::unordered_set<SdfPath, SdfPath::Hash> node_children_copy;
        for (auto& child : node->children) {
            node_children_copy.insert(child.second->prim_path);
        }

        //Loop over real prim's children
        for (const auto& child_prim : prim.GetChildren()) {
            TfToken child_name = child_prim.GetName();
            SdfPath child_prim_path = child_prim.GetPath();
            if (node->children.find(child_name) != node->children.end()) {
                //Exists in cache
                //Sync on child and on stage
                _sync(node->children[child_name], child_prim);
                //Remove child from temporary list
                node_children_copy.erase(child_prim_path);
            } else {
                //Doesn't exist in cache but in stage
                //Doesn't exist in tree
                node->children[child_prim.GetName()] = TfCreateRefPtr(new Node(child_prim));
                _addToAdded(node->children[child_prim.GetName()]);
                _noDescAdded.push_back(child_prim_path);
            }
        }
        
        //Exists only on cache, not on stage
        //Add the non-existant children to the removed set
        for (auto& child_prim_path : node_children_copy) {
            TfToken childName = child_prim_path.GetNameToken();
            _addToRemoved(node->children[childName]);
            _noDescRemoved.push_back(child_prim_path);
            node->children.erase(childName);
        }
    }

} // namespace unf

PXR_NAMESPACE_CLOSE_SCOPE
