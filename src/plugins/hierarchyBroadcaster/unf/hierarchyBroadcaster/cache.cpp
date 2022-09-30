#include "cache.h"
#include <queue>

PXR_NAMESPACE_USING_DIRECTIVE

namespace {

std::vector<std::string> _split(
    const std::string& s, const std::string& delimiter)
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

}  // namespace

namespace unf{

Node::Node(const PXR_NS::UsdPrim& prim) {
    this->prim = prim;
    prim_path = prim.GetPath();
}

NodeRefPtr Node::CreateWithDescendants(const PXR_NS::UsdPrim& prim) {
    NodeRefPtr root = PXR_NS::TfCreateRefPtr(new Node(prim));
    std::queue<NodeRefPtr*> nodes;
    nodes.push(&root);
    while (!nodes.empty()) {
        NodeRefPtr& node = *nodes.front();
        nodes.pop();
        
        for (const auto& child_prim : node->prim.GetChildren()) {
            NodeRefPtr child_node = PXR_NS::TfCreateRefPtr(new Node(child_prim));
            node->children[child_prim.GetName()] = child_node;
        }

        for (auto& entry : node->children) {
            nodes.push(&entry.second);
        }
    }

    return root;
}

HierarchyCache::HierarchyCache(const PXR_NS::UsdStageWeakPtr stage) : _stage(stage) {
    _root = Node::CreateWithDescendants(stage->GetPseudoRoot());
}

void HierarchyCache::Update(const PXR_NS::SdfPathVector& resynced)
{
    for (auto& p : resynced) {
        if(!p.IsPrimPath() && p != _root->prim_path) {
            _modified.push_back(p);
            continue;
        }
        NodeRefPtr node = _findNodeOrUpdate(p);
        if (node) {
            _sync(node);
        }
    }
}

bool HierarchyCache::FindNode(const SdfPath& path)
{
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

void HierarchyCache::_addToRemoved(NodeRefPtr& node)
{
    _removed.push_back(node->prim_path);

    for (auto c : node->children) {
        _addToRemoved(c.second);
    }
}

void HierarchyCache::_addToAdded(NodeRefPtr& node)
{
    _added.push_back(node->prim_path);

    for (auto c : node->children) {
        _addToAdded(c.second);
    }
}

NodeRefPtr HierarchyCache::_findNodeOrUpdate(const SdfPath& path)
{
    if (path == _stage->GetPseudoRoot().GetPath()) {
        return _root;
    }
    std::vector<std::string> split_paths = _split(path.GetString(), "/");
    NodeRefPtr curr_node = _root;
    // TODO: make this sdfpath
    std::string partial_path = "";
    for (size_t i = 1; i < split_paths.size(); i++) {
        partial_path += "/";
        partial_path += split_paths[i];
        TfToken p_token = TfToken(split_paths[i]);
        auto child_token = curr_node->children.find(p_token);
        // If we are at the final node of the path, then this is the
        // resyncedpath.
        if (i == split_paths.size() - 1) {
            SdfPath prim_path = SdfPath(partial_path);
            PXR_NS::UsdPrim child = _stage->GetPrimAtPath(prim_path);
            // Doesn't exit in stage nor cache -- don't do anything.
            if (!child && child_token == curr_node->children.end()) {
                return nullptr;
            }
            // Doesn't exist in the stage -- then the prim was removed.
            else if (!child) {
                _addToRemoved(curr_node->children[p_token]);
                curr_node->children.erase(p_token);
                return nullptr;
            }
        }
        if (child_token == curr_node->children.end()) {
            SdfPath prim_path = SdfPath(partial_path);
            PXR_NS::UsdPrim child = _stage->GetPrimAtPath(prim_path);
            // Doesn't exist in the cache but in stage -- need to add prim
            if (child) {
                curr_node->children[p_token] = Node::CreateWithDescendants(child);
                _addToAdded(curr_node->children[p_token]);
            }
            return nullptr;
        }
        curr_node = child_token->second;
    }
    return curr_node;
}

void HierarchyCache::_sync(NodeRefPtr& start_node)
{
    std::queue<NodeRefPtr*> nodes;
    nodes.push(&start_node);
    while (!nodes.empty()) {
        NodeRefPtr& node = *nodes.front();
        nodes.pop();

        _modified.push_back(node->prim_path);
        
        if(!node->prim.IsValid()) {
            node->prim = _stage->GetPrimAtPath(node->prim_path);
        }
        
        for (const auto& stage_child : node->prim.GetChildren()) {
            const auto& cache_child = node->children.find(stage_child.GetName());
            if (cache_child == node->children.end()) {
                // Not in cache, so added
                NodeRefPtr new_child = Node::CreateWithDescendants(stage_child);
                new_child->flag = 1;
                node->children[stage_child.GetName()] = std::move(new_child);
            } else {
                // Is in cache, so modified
                cache_child->second->flag = 2;
            }
        }

        for (auto it = node->children.begin(); it != node->children.end();) {
            NodeRefPtr& cache_child = it->second;
            if (cache_child->flag == 0) {
                // Flag not set, so must not exist anymore
                _addToRemoved(cache_child);
                it = node->children.erase(it);
                continue;
            } else if (cache_child->flag == 1) {
                // Node was added, so do nothing
                _addToAdded(cache_child);
            } else if (cache_child->flag == 2) {
                // Because we don't insert in this loop, and erase does not invalidate references
                // (other than the erased reference) this is safe
                nodes.push(&cache_child);
            }
            // Reset the flag
            cache_child->flag = 0;
            ++it;
        }
    }
}

}  // namespace unfTest
