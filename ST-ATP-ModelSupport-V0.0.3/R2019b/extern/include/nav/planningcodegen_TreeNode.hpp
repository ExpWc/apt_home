// Copyright 2019 The MathWorks, Inc.

/**
* @file
* @brief Tree node that comprises the search tree
*/


#ifndef PLANNINGCODEGEN_TREENODE_HPP
#define PLANNINGCODEGEN_TREENODE_HPP

#include <vector>
#include <iostream>
#include <algorithm>

namespace nav
{
    template <typename T>
    class TreeNode
    {

        template <typename>
        friend class SearchTree;

    protected:
        std::size_t m_dim;

    public:
        TreeNode(std::size_t dim) : m_dim(dim), m_state(dim, 0.0)
        {
            m_parent = nullptr;
            m_targetReached = false;
            m_costFromParent = static_cast<T>(0.0);
            m_costFromRoot = static_cast<T>(0.0);
        }

        ~TreeNode() {/*tree node destructor*/}

        bool isTargetReached()
        {
            return m_targetReached;
        }

        TreeNode<T> * getChild(int idx)
        {
            return m_children[idx];
        }

        std::vector<T>& getState()
        {
            return m_state;
        }

        void setState(std::vector<T> newState)
        {
            m_state = newState;
        }

        std::size_t getNodeID()
        {
            return m_nodeID;
        }

        T getCostFromRoot()
        {
            return m_costFromRoot;
        }

        T getCostFromParent()
        {
            return m_costFromParent;
        }

        void setNodeID(std::size_t id)
        {
            m_nodeID = id;
        }

        void addToChildren(TreeNode<T> * newNode)
        {
            m_children.push_back(newNode);
            newNode->m_parent = this;
        }

        void removeChild(TreeNode<T> * nodePtr)
        {
            // erase-remove idiom
            m_children.erase(std::remove_if(m_children.begin(), m_children.end(), [nodePtr](TreeNode<T> * ptr) {return nodePtr == ptr; }));
        }

    public:

        TreeNode<T>* m_parent;

        T m_costFromParent;

        T m_costFromRoot;

        std::vector<T> trajectoryFromParent;

    protected:
        bool m_targetReached;

        std::vector<TreeNode<T>*> m_children;

        std::vector<T> m_state;

        std::size_t m_nodeID;

    };

}

#endif
