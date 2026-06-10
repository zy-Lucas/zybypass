#pragma once

#include "memory/metaspace/counters.hpp"
#include "runtime/jvmObject.hpp"

namespace hotspot::memory::metaspace
{
class BlockTree
{

  public:
    BlockTree() noexcept : root_(nullptr) {}

    void add_block(uint64_t *p, uint64_t word_size) noexcept;
    uint64_t *remove_block(uint64_t word_size, uint64_t *p_real_word_size) noexcept;

    uint32_t count() const noexcept { return counter_.count(); }

    uint64_t total_size() const noexcept { return counter_.total_size(); }

    bool is_empty() const noexcept { return root_ == nullptr; }

    //void print_all_nodes() const;

  private:
    struct Node
    {
        static constexpr int64_t canary_value_ = 0x4e4f44454e4f4445ULL;

        int64_t canary_;

        Node *parent_;
        Node *left_;
        Node *right_;

        Node *next_;

        uint64_t word_size_;

        Node(uint64_t word_size) noexcept
            : canary_(canary_value_), parent_(NULL), left_(NULL), right_(NULL), next_(NULL), word_size_(word_size)
        {
        }
    };

    void replace_node_in_parent(Node *child, Node *replace) noexcept;

    void insert(Node *insertion_point, Node *n) noexcept;

    Node *find_closest_fit(Node *n, uint64_t s) noexcept;
    Node *find_closest_fit(uint64_t s);

    void remove_node_from_tree(Node *n) noexcept;

    static void add_to_list(Node *n, Node *head) noexcept;
    static Node *remove_from_list(Node *head) noexcept;

    static void set_left_child(Node *p, Node *c) noexcept;
    static void set_right_child(Node *p, Node *c) noexcept;

    static Node *successor(Node *n) noexcept;

    //void print_node_recursive(Node *n) const;

    Node *root_;

    MemRangeCounter counter_;

  public:
    const static uint64_t MinWordSize = (sizeof(Node) + sizeof(uint64_t *) - 1) / sizeof(uint64_t *);
};
} // namespace hotspot::memory::metaspace