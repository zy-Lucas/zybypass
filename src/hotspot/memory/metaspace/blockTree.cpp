#include "blockTree.hpp"

namespace hotspot::memory::metaspace
{
void BlockTree::add_block(uint64_t *p, uint64_t word_size) noexcept
{
    if (word_size < MinWordSize)
        return;
    Node *n = new (p) Node(word_size);
    if (!root_)
        root_ = n;
    else
        insert(root_, n);
    counter_.add(word_size);
}

uint64_t *BlockTree::remove_block(uint64_t word_size, uint64_t *p_real_word_size) noexcept
{
    *p_real_word_size = 0;

    if (word_size < MinWordSize)
        return nullptr;
    Node *n = find_closest_fit(word_size);
    if (!n)
        return nullptr;
    if (n->next_)
        n = remove_from_list(n);
    else
        remove_node_from_tree(n);
    *p_real_word_size = n->word_size_;
    counter_.sub(n->word_size_);
    return (uint64_t *)n;
}

void BlockTree::replace_node_in_parent(Node *child, Node *replace) noexcept
{
    if (Node *parent = child->parent_; parent)
    {
        if (parent->left_ == child)
            set_left_child(parent, replace);
        else
            set_right_child(parent, replace);
        return;
    }

    if (child != root_)
        return;
    root_ = replace;
    if (replace)
        replace->parent_ = nullptr;
}

void BlockTree::insert(Node *insertion_point, Node *n) noexcept
{
    if (n->parent_ != nullptr)
        return;
    while (true)
    {
        if (n->word_size_ == insertion_point->word_size_)
        {
            add_to_list(n, insertion_point);
            return;
        }

        Node *&child = n->word_size_ > insertion_point->word_size_ ? insertion_point->right_ : insertion_point->left_;
        if (!child)
        {
            child = n;
            n->parent_ = insertion_point;
            return;
        }

        insertion_point = child;
    }
}

BlockTree::Node *BlockTree::find_closest_fit(Node *n, uint64_t s) noexcept
{
    Node *best = nullptr;
    while (n)
    {
        if (n->word_size_ >= s)
        {
            if (n->word_size_ == s)
                return n;
            best = n;
            n = n->left_;
        }
        else
            n = n->right_;
    }
    return best;
}

BlockTree::Node *BlockTree::find_closest_fit(uint64_t s)
{
    if (root_)
        return find_closest_fit(root_, s);
    return nullptr;
}

void BlockTree::remove_node_from_tree(Node *n) noexcept
{
    if (n->next_)
        return;
    if (!n->left_ || !n->right_)
    {
        replace_node_in_parent(n, n->left_ ? n->left_ : n->right_);
        return;
    }
    Node *succ = successor(n);
    if (!succ || succ->left_ || succ->word_size_ <= n->word_size_)
        return;
    Node *succ_parent = succ->parent_;
    if (succ_parent == n)
    {
        if (n->right_ != succ)
            return;
        replace_node_in_parent(n, succ);
        set_left_child(succ, n->left_);
    }
    else
    {
        if (succ_parent->left_ != succ)
            return;
        set_left_child(succ_parent, succ->right_);
        replace_node_in_parent(n, succ);
        set_left_child(succ, n->left_);
        set_right_child(succ, n->right_);
    }
}

void BlockTree::add_to_list(Node *n, Node *head) noexcept
{
    if (head->word_size_ != n->word_size_)
        return;
    n->next_ = head->next_;
    head->next_ = n;
}

BlockTree::Node *BlockTree::remove_from_list(Node *head) noexcept
{
    if (!head->next_)
        return nullptr;
    Node *n = head->next_;
    head->next_ = n->next_;
    return n;
}

void BlockTree::set_left_child(Node *p, Node *c) noexcept
{
    p->left_ = c;
    if (c)
        c->parent_ = p;
}

void BlockTree::set_right_child(Node *p, Node *c) noexcept
{
    p->right_ = c;
    if (c)
        c->parent_ = p;
}

BlockTree::Node *BlockTree::successor(Node *n) noexcept
{
    if (n->right_)
    {
        Node *succ = n->right_;
        while (succ->left_)
            succ = succ->left_;
        return succ;
    }

    Node *curr = n;
    Node *parent = n->parent_;
    while (parent && curr == parent->right_)
    {
        curr = parent;
        parent = parent->parent_;
    }
    return parent;
}

// void BlockTree::print_all_nodes() const
// {
//     if (!root_)
//     {
//         std::cout << "[BlockTree] empty" << std::endl;
//         return;
//     }
//     std::cout << "[BlockTree] dump begin (count=" << counter_.count() << ", total_size=" << counter_.total_size() << ")"
//               << std::endl;
//     print_node_recursive(root_);
//     std::cout << "[BlockTree] dump end" << std::endl;
// }

// void BlockTree::print_node_recursive(Node *n) const
// {
//     if (!n)
//         return;

//     // 中序遍历：左子树 -> 当前节点（含链表） -> 右子树
//     print_node_recursive(n->left_);

//     // 打印当前 BST 节点以及挂在它上面的同尺寸链表
//     Node *curr = n;
//     while (curr)
//     {
//         std::cout << "  Node@" << curr << " word_size=" << curr->word_size_
//                   << " canary=" << (curr->canary_ == Node::canary_value_ ? "OK" : "CORRUPTED")
//                   << " parent=" << curr->parent_ << " left=" << curr->left_ << " right=" << curr->right_
//                   << " next=" << curr->next_ << std::endl;
//         curr = curr->next_;
//     }

//     print_node_recursive(n->right_);
// }
}