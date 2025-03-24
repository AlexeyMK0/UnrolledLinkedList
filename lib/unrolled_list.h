#pragma once

#include <memory>
#include <iterator>
#include <cstdint>
#include <ranges>

#include "Node.h"
#include "DebugPrint.h"

template<typename T, size_t NodeMaxSize = 10, typename Allocator = std::allocator<T>>
class unrolled_list {
 public:
    using same_unrolled_list = unrolled_list<T, NodeMaxSize, Allocator>;
    using NodeT = Node<T, NodeMaxSize, Allocator>;
    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using const_reference = const T&;
    using difference_type = std::ptrdiff_t;
    using size_type = std::size_t;
    class const_iterator;
    class iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using allocator_type = Allocator;

 private:
    using AllocatorTraitsT = std::allocator_traits<Allocator>;
    using AllocatorNode = AllocatorTraitsT::template rebind_alloc<NodeT>;
    using AllocatorTraitsNode = std::allocator_traits<AllocatorNode>;
    // unrolled_list(Allocator alloc_t, AllocatorNode alloc_node,
    //     difference_type sz = 0, NodeT* last_node = nullptr, NodeT* first_node = nullptr)
    //     : alloc_t_(alloc_t), alloc_node_(alloc_node), size_(sz)
    //     , last_node_(last_node), first_node_(first_node) {}
    class BaseIterator {
     protected:
        using _MyList = unrolled_list<T, NodeMaxSize, Allocator>;
        static const difference_type kStartInd = 0;
        BaseIterator() = default;
        BaseIterator(NodeT* node, difference_type pos = kStartInd)
            : node_ptr(node), ind(pos) {}
     public:
        bool operator==(const BaseIterator& other) const { return node_ptr == other.node_ptr && ind == other.ind; }
        bool operator!=(const BaseIterator& other) const { return !((*this) == other); }
        BaseIterator& operator++() {
            difference_type new_ind = ind + 1;
            if (!IsLastNode(node_ptr) && IsEndOfNode()) {
                node_ptr = node_ptr->next();
                new_ind = kStartInd;
            }
            ind = new_ind;
            return *this;
        }
        BaseIterator& operator--() {
            difference_type new_ind = ind - 1;
            if (!IsFirstNode(node_ptr) && IsBeginOfNode()) {
                node_ptr = node_ptr->prev();
                new_ind = node_ptr->size() - 1 + kStartInd;
            }
            ind = new_ind;
            return *this;
        }
        void Print() {
            Println("Printing iterator");
            Println(node_ptr, "node_ptr");
            Println(ind, "ind");
        }
        friend class unrolled_list<T, NodeMaxSize, Allocator>;
     protected:
        bool IsEndOfNode() const { return node_ptr->size() - 1 + kStartInd == ind; }
        bool IsBeginOfNode() const { return ind == kStartInd; }
     protected:
        NodeT* node_ptr;
        difference_type ind;
    };
 public:
    class iterator : public BaseIterator
    {
        using _MyBase = unrolled_list<T, NodeMaxSize, Allocator>::BaseIterator;
        using _MyList = _MyBase::_MyList;
        // static const difference_type kStartInd = 0;
     public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = _MyList::difference_type;
        using pointer = _MyList::pointer;
        using reference = _MyList::reference;
        using value_type = _MyList::value_type;
        iterator() = default;
        iterator(NodeT* node, difference_type pos = _MyBase::kStartInd)
            : _MyBase(node, pos) {}
        bool operator==(const iterator& other) const { return _MyBase::operator==(other); }
        bool operator!=(const iterator& other) const { return !((*this) == other); }
        T& operator*() const { return (*_MyBase::node_ptr)[_MyBase::ind - _MyBase::kStartInd]; }
        pointer operator->() const {
            return &(**this);
        }
        iterator& operator++() { 
            _MyBase::operator++();
            return (*this);
        }
        iterator operator++(int) {
            iterator copy = (*this);
            ++(*this);
            return copy;
        }
        iterator& operator--() {
            _MyBase::operator--();
            return (*this);
        }
        iterator operator--(int) {
            iterator copy = (*this);
            --(*this);
            return copy;
        }
        friend _MyList;
    };
    class const_iterator : public BaseIterator
    {
        using _MyList = unrolled_list<T, NodeMaxSize, Allocator>;
        using _MyBase = _MyList::BaseIterator;
        using _MyBase::kStartInd;
        using _MyBase::ind;
        using _MyBase::node_ptr;
     public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = _MyList::difference_type;
        using pointer = const _MyList::pointer;
        using reference = _MyList::const_reference;
        using value_type = _MyList::value_type;
        const_iterator() = default;
        const_iterator(NodeT* node, difference_type pos = _MyBase::kStartInd)
            : BaseIterator(node, pos) {}
        const_iterator(const _MyList::iterator& it)
            : BaseIterator(it) {}
        const T& operator*() const {
            // Println(ind, "iter ind");
            return (*node_ptr)[ind - kStartInd];
        }
        const pointer operator->() const {
            return &(**this);
        }
        const_iterator& operator++() {
            _MyBase::operator++();
            return (*this);
        }
        const_iterator operator++(int) {
            const_iterator copy = (*this);
            ++(*this);
            return copy;
        }
        const_iterator& operator--() {
            _MyBase::operator--();
            return (*this);
        }
        const_iterator operator--(int) {
            const_iterator copy = (*this);
            --(*this);
            return copy;
        }
        friend _MyList;
    };
 private:
    unrolled_list(Allocator alloc_t, AllocatorNode alloc_node,
        difference_type sz = 0, NodeT* last_node = nullptr, NodeT* first_node = nullptr)
        : alloc_t_(alloc_t), alloc_node_(alloc_node), size_(sz)
        , last_node_(last_node), first_node_(first_node) {}
 public:
    void Print() {
        NodeT* cur_node = first_node_;
        size_t ind = 0;
        while(cur_node) {
            Println(ind);
            PrintNode(*cur_node);
            cur_node = cur_node->next();
            ++ind;
        }
    }
    // Constructors from Container
    explicit unrolled_list(const Allocator& alloc = Allocator()) 
        : unrolled_list(alloc, alloc) {}
    unrolled_list(const unrolled_list& other, const Allocator& alloc)
        : unrolled_list(other.begin(), other.end(), alloc) {}
    unrolled_list(const unrolled_list& other)
        : unrolled_list(other, other.get_allocator()) {}
    unrolled_list& operator=(const unrolled_list& other) {
        assign(other.begin(), other.end());
        return *this;
    }
    // Constructors from Sequence container
    explicit unrolled_list(size_type n, T t = T(), Allocator alloc = Allocator())
        : unrolled_list(alloc, alloc)
    {
        for (size_type i = 0; i < n; ++i) {
            push_back(t);
        }
    }
    template<std::input_iterator InputIt>
    unrolled_list(InputIt start, InputIt end, Allocator alloc = Allocator())
        : unrolled_list(alloc, alloc)
    {
        try {
            for (auto it = start; it != end; ++it) {
                push_back(*it);
            }
        } catch (const std::exception& ex) {
            clear();
            throw;
        }
    }
    unrolled_list(std::initializer_list<value_type> init_list, Allocator alloc = Allocator())
        : unrolled_list(init_list.begin(), init_list.end(), alloc) {}
    unrolled_list& operator=(std::initializer_list<value_type> init_list) {
        assign(init_list.begin(), init_list.end());
        return *this;
    }
    ~unrolled_list() {
        clear();
    }
    // operations from Container
    iterator begin() { return iterator(first_node_, 0); }
    iterator end() { return iterator(last_node_, GetNodeSize(last_node_)); }
    const_iterator begin() const { return const_iterator(first_node_, 0); }
    const_iterator end() const { return const_iterator(last_node_, GetNodeSize(last_node_)); }
    const_iterator cbegin() const noexcept { return begin(); }
    const_iterator cend() const noexcept { return end(); }
    void swap(unrolled_list& other) {
        std::swap(alloc_t_, other.alloc_t_);
        std::swap(alloc_node_, other.alloc_node_);
        std::swap(size_, other.size_);
        std::swap(last_node_, other.last_node_);
        std::swap(first_node_, other.first_node_);
    }
    size_type size() const { return size_; };
    size_type max_size() const { return std::numeric_limits<size_type>::max(); }
    bool empty() const { return size_ == 0; };
    // end of Container 

    bool operator==(const same_unrolled_list& other) {
        if (size_ != other.size_) {
            return false;
        }
        return std::equal(begin(), end(), other.begin());
    }
    bool operator!=(const same_unrolled_list& other) {
        return !((*this) == other);
    }

    // operations from Sequence container
    iterator insert(const_iterator pos, const_reference val)
    {
        Println("In insert function");
        NodeT* node_ptr = pos.node_ptr;
        if (!node_ptr) {
            if (empty() && !first_node_) {
                first_node_ = last_node_ = node_ptr = CreateEmptyNode();
            } else {
                // >:(
                throw std::runtime_error("bad iterator in insert: nullptr in non empty container");
            }
        }
        NodeT* prev_node_ptr = node_ptr->prev();
        NodeT* next_node_ptr = node_ptr->next();
        bool is_last = last_node_ == node_ptr;
        bool is_first = first_node_ == node_ptr;
        NodeT tmp_new_node = *node_ptr;
        NodeT* top_node_ptr = &tmp_new_node;
        difference_type ind = pos.ind;
        top_node_ptr = AddToNode(top_node_ptr, ind, val);
        std::swap(*node_ptr, tmp_new_node);
        if (top_node_ptr == &tmp_new_node) {
            top_node_ptr = node_ptr;
        }
        LinkNodes(prev_node_ptr, node_ptr);
        LinkNodes(top_node_ptr, next_node_ptr);
        if (is_first) {
            first_node_ = node_ptr;
        }
        if (is_last) {
            last_node_ = top_node_ptr;
        }
        NodeT* cur_node = node_ptr;
        while (ind >= cur_node->size()) {
            ind -= cur_node->size();
            cur_node = cur_node->next();
        }
        iterator ret_val = iterator{cur_node, ind};
        ++size_;
        return ret_val;
    }
    iterator insert(const_iterator pos, size_type n, const_reference t)
    {
        if (n == 0) {
            return iterator{pos.node_ptr, pos.ind};
        }
        auto range = std::views::repeat(t, n);
        return insert(pos, range.begin(), range.end());
    }
    iterator insert(const_iterator pos, std::initializer_list<value_type> init_list)
    {
        return insert(pos, init_list.begin(), init_list.end());
    }
    template<std::input_iterator InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        if (first == last) {
            return iterator {pos.node_ptr, pos.ind};
        }
        NodeT* node_ptr = pos.node_ptr;
        bool created_first_node = false;
        if (!node_ptr) {
            if (empty() && !first_node_) {
                created_first_node = true;
                first_node_ = last_node_ = node_ptr = CreateEmptyNode();
            } else {
                // >:(
                throw std::runtime_error("bad iterator in insert: nullptr in non empty container");
            }
        }
        NodeT* prev_node_ptr = node_ptr->prev();
        NodeT* next_node_ptr = node_ptr->next();
        bool is_last = last_node_ == node_ptr;
        bool is_first = first_node_ == node_ptr;
        NodeT tmp_new_node = *node_ptr;
        NodeT* top_node_ptr = &tmp_new_node;
        difference_type ind = pos.ind;
        size_type sz_dif = 0;
        try {
            while (first != last) {
                top_node_ptr = AddToNode(top_node_ptr, ind, *first);
                ++sz_dif;
                ++first;
            }
            std::swap(*node_ptr, tmp_new_node);
        } catch(const std::exception& ex) {
            if (created_first_node) {
                DeleteNode(node_ptr);
                first_node_ = last_node_ = nullptr;
            }
            throw;
        }
        if (top_node_ptr == &tmp_new_node) {
            top_node_ptr = node_ptr;
        }
        LinkNodes(prev_node_ptr, node_ptr);
        LinkNodes(top_node_ptr, next_node_ptr);
        if (is_first) {
            first_node_ = node_ptr;
        }
        if (is_last) {
            last_node_ = top_node_ptr;
        }
        NodeT* cur_node = node_ptr;
        while (ind >= cur_node->size()) {
            ind -= cur_node->size();
            cur_node = cur_node->next();
        }
        iterator ret_val = iterator{cur_node, ind};
        size_ += sz_dif;
        return ret_val;
    }
    iterator erase(const_iterator pos) noexcept {
        NodeT* node_ptr = pos.node_ptr;
        difference_type ind = pos.ind;
        if (!ValidateIteratorInErase(pos)) {
            return iterator{node_ptr, ind};
        }
        node_ptr->erase(ind);
        --size_;
        NodeT* prev = node_ptr->prev();
        NodeT* next = node_ptr->next();
        if (node_ptr->empty()) {
            EraseNode(node_ptr);
            iterator ret_it;
            if (IsFirstNode(node_ptr)) {
                ret_it = begin();
            } else if (IsLastNode(node_ptr)) {
                ret_it = end();
            } else {
                ret_it = Begin(next);
            }
            return ret_it;
        }
        if (NeedToFill(node_ptr)) {
            if (prev && CanBeMerged(*prev, *node_ptr)) {
                ind += prev->size();
                MergeNodeRetVal ret_val = MergeNodes(prev, node_ptr);
                EraseNode(ret_val.reduced_node);
                node_ptr = ret_val.filled_node;
                prev = node_ptr->prev();
            } else if (next && CanBeMerged(*node_ptr, *next)) {
                MergeNodeRetVal ret_val = MergeNodes(node_ptr, next);
                EraseNode(ret_val.reduced_node);
                node_ptr = ret_val.filled_node;
                next = node_ptr->next();
            } else if (prev && CanBeFilled(prev, node_ptr)) {
                ind += FillRightToRequired(prev, node_ptr);
            } else if (next && CanBeFilled(node_ptr, next)) {
                FillLeftToRequired(node_ptr, next);
            }
        }
        Println("erased");
        if (ind == GetNodeSize(node_ptr)) {
            node_ptr = node_ptr->next();
            ind = 0;
        }
        return iterator{node_ptr, ind};
    }
    iterator erase(const_iterator first, const_iterator last) noexcept {
        if (first == last) {
            return CastToIterator(last);
        }
        Println("Erasing completly covered nodes");
        NodeT* first_node = first.node_ptr;
        NodeT* last_node = last.node_ptr;
        size_type first_sz = GetNodeSize(first_node);
        size_type last_sz = GetNodeSize(last_node);
        EraseCompletlyCoveredNodes(first, last);
        size_type elem_cnt = first_sz - first.ind + last.ind;
        if (first.ind == 0) {
            elem_cnt = last.ind;
        }
        Println(first_sz, "first_sz");
        Println(first.ind, "first.ind");
        Println(last.ind, "last.ind");
        Println(elem_cnt, "elem_cnt");
        Println("Erasing remaining elements");
        iterator ret_it = (first.ind == 0 ? Begin(last_node) : CastToIterator(first));
        for (size_type i = 0; i < elem_cnt; ++i) {
            ret_it = erase(ret_it);
            // ret_it.Print();
            // PrintNode(*ret_it.node_ptr);
        }
        Println("end of erase range");
        return ret_it;
    }
    bool ValidateIteratorInErase(const const_iterator& it) {
        NodeT* node_ptr = it.node_ptr;
        difference_type ind = it.ind;
        if (!node_ptr || ind >= node_ptr->size()) {
            return false;
        }
        return true;
    }
    void clear() {
        NodeT* cur_node = first_node_;
        while (cur_node) {
            NodeT* next_node = cur_node->next();
            DeleteNode(cur_node);
            cur_node = next_node;
        }
        first_node_ = nullptr;
        last_node_  = nullptr;
        size_       = 0;
    }
    template<std::input_iterator InputIt>
    void assign(InputIt start, InputIt finish) {
        same_unrolled_list new_list {start, finish};
        swap(new_list);
    }
    reference front() {
        return *begin();
    }
    const_reference front() const {
        return *begin();
    }
    reference back() {
        return *(--end());
    }
    const_reference back() const {
        return *(--end());
    }
    void push_front(const_reference val) {
        NodeT* cur_node = first_node_;
        bool created_new_node = false;
        if (!cur_node || cur_node->full()) {
            cur_node = CreateEmptyNode();
            created_new_node = true;
        }
        try {
            cur_node->push_front(val);
        } catch (const std::exception& ex) {
            if (created_new_node) {
                DeleteNode(cur_node);
            }
            throw;
        }
        if (created_new_node) {
            if (empty()) {
                first_node_ = last_node_ = cur_node;
            } else {
                LinkNodes(cur_node, first_node_);
                first_node_ = cur_node;
            }
        }
        ++size_;
    }
    // void push_front(T&& rv);
    void pop_front() noexcept {
        if (empty()) {
            return;
        }
        first_node_->pop_front();
        --size_;
        if (first_node_->empty()) {
            EraseNode(first_node_);
        }
    }
    void push_back(const_reference val) {
        NodeT* cur_node = last_node_;
        bool created_new_node = false;
        if (!cur_node || cur_node->full()) {
            cur_node = CreateEmptyNode();
            created_new_node = true;
        }
        try {
            Println("entering node->push_back(val)");
            cur_node->push_back(val);
        } catch (const std::exception& ex) {
            Println("Ooops... found error");
            if (created_new_node) {
                Println("Entering DeleteNode");
                DeleteNode(cur_node);
            }
            throw;
        }
        if (created_new_node) {
            Println(created_new_node, "created_new_node");
            if (empty()) {
                first_node_ = last_node_ = cur_node;
            } else {
                LinkNodes(last_node_, cur_node);
                Println("linked nodes");
                last_node_ = cur_node;
            }
        }
        ++size_;
    }
    // void push_back(T&& rv);
    void pop_back() noexcept {
        if (empty()) {
            return;
        }
        last_node_->pop_back();
        --size_;
        if (last_node_->empty()) {
            EraseNode(last_node_);
        }
    }
    // end of Sequence container
    

    // operations from ReversibleContainer
    reverse_iterator rbegin() { return std::make_reverse_iterator<iterator>(end()); }
    reverse_iterator rend() { return std::make_reverse_iterator<iterator>(begin()); }
    const_reverse_iterator rbegin() const { return std::make_reverse_iterator<iterator>(end()); }
    const_reverse_iterator rend() const { return std::make_reverse_iterator<iterator>(begin()); }
    const_reverse_iterator crbegin() const noexcept { return std::make_reverse_iterator<iterator>(cend()); }
    const_reverse_iterator crend() const noexcept { return std::make_reverse_iterator<iterator>(cbegin()); }
    // end of ReversibleContainer

    // opertions from AllocatorAwareContainer 
    Allocator get_allocator() const { return alloc_t_; }
    // end of AllocatorAwareContainer 
 private:
    static size_type RequiredNodeSize() { return NodeMaxSize - NodeMaxSize / 2; }
    static bool IsLastNode(const NodeT* node) { return node->next() == nullptr; }
    static bool IsFirstNode(const NodeT* node) { return node->prev() == nullptr; }
    static iterator CastToIterator(const_iterator cit) { return iterator{cit.node_ptr, cit.ind}; }
    static size_t GetNodeSize(const NodeT* node) { return (node ? node->size() : 0); }
    bool NeedToFill(const NodeT* node_ptr) { return node_ptr && GetNodeSize(node_ptr) < NodeMaxSize - NodeMaxSize / 2; }
    bool CanBeFilled(const NodeT* left, const NodeT* right) const {
        return GetNodeSize(left) + GetNodeSize(right) < 2 * RequiredNodeSize();
    }
    iterator Begin(NodeT* node) {
        return iterator{node, 0};
    }
    size_type FillLeftToRequired(NodeT* left, NodeT* right) {
        size_type added_elems = 0;
        if (!CanBeFilled(left, right)) {
            return added_elems;
        }
        while (GetNodeSize(left) < RequiredNodeSize()) {
            left->push_back(right->front());
            right->pop_front();
            ++added_elems;
        }
        return added_elems;
    }
    size_type FillRightToRequired(NodeT* left, NodeT* right) {
        size_type added_elems = 0;
        if (!CanBeFilled(left, right)) {
            return added_elems;
        }
        while (GetNodeSize(right) < RequiredNodeSize()) {
            right->push_front(left->back());
            left->pop_back();
            ++added_elems;
        }
        return added_elems;
    }

    void DeleteNode(NodeT* node_to_delete) {
        Println("deleting node");
        AllocatorTraitsNode::destroy(alloc_node_, node_to_delete);
        AllocatorTraitsNode::deallocate(alloc_node_, node_to_delete, 1);
    }
    void EraseCompletlyCoveredNodes(const_iterator first, const_iterator last) noexcept {
        first.Print();
        last.Print();
        auto [first_node, last_node] = GetDeleteNodeRange(first, last);
        NodeT* next_node = first_node;
        Println(last_node, "last_node ptr");
        while (next_node != last_node) {
            Println(next_node, "cur_node ptr");
            first_node = next_node;
            next_node = first_node->next();
            EraseNode(first_node);
        }
    }
    // [first, second)
    std::pair<NodeT*, NodeT*> GetDeleteNodeRange(const_iterator first, const_iterator last) {
        Println("entering GetDeleteNodeRange");
        NodeT* first_node = first.node_ptr;
        NodeT* last_node = last.node_ptr;
        Println(first_node, "first_node ptr");
        Println(last_node, "last_node ptr");
        if (first_node == last_node) {
            return {first_node, last_node};
        }
        Println(first_node->next(), "first_node->next()");
        if (first.ind != 0) {
            first_node = first_node->next();
        }
        return {first_node, last_node};
    }
    void EraseNode(NodeT* node_to_erase) {
        if (!node_to_erase) {
            return;
        }
        NodeT* prev = node_to_erase->prev();
        NodeT* next = node_to_erase->next();
        if (IsFirstNode(node_to_erase)) {
            first_node_ = next;
        }
        if (IsLastNode(node_to_erase)) {
            last_node_ = prev;
        }
        size_ -= GetNodeSize(node_to_erase);
        DeleteNode(node_to_erase);
        LinkNodes(prev, next);
    }
    NodeT* AddToNode(NodeT* node_ptr, difference_type ind, const_reference val) {
        NodeT* ret_val = node_ptr;
        // iterator ret_val;
        if (node_ptr->full()) {
            Println("Node is full");
            ret_val = AddToFullNode(node_ptr, ind, val);
        } else {
            Println("Just calling insert");
            node_ptr->insert(val, ind);
        }
        Println("end of add to node");
        return ret_val;
    }

    NodeT* AddToFullNode(NodeT* node_ptr, difference_type ind, const_reference val) {
        NodeT* prev_node_ptr = node_ptr->prev();
        NodeT* next_node_ptr = node_ptr->next();
        Println<bool>(!node_ptr, "node_ptr is null");
        Println(ind, "ind");
        // for swap and copy idiom
        // NodeT old_node = *node_ptr;
        Println("assigned old node");
        NodeT* new_node = CreateEmptyNode();
        Println("Created an empty node");
        bool add_to_left = false;
        try {
            difference_type left_sz = NodeMaxSize / 2;
            Println(left_sz, "left size");
            difference_type right_sz = NodeMaxSize - left_sz;
            Println(right_sz, "right size");
            if (ind <= left_sz) {
                add_to_left = true;
            } else {
                add_to_left = false;
                if (left_sz <= right_sz) {
                    ++left_sz;
                    --right_sz;
                }
                ind -= left_sz;
            }
            Println(left_sz, "updated left_size");
            Println(right_sz, "updated right_size");
            Println(ind, "updated index");
            Println("Moving to right");
            MoveToRight(*node_ptr, *new_node, right_sz);
            PrintNode(*node_ptr);
            PrintNode(*new_node);
            if (add_to_left) {
                Println("adding to old node");
                node_ptr->insert(val, ind);
                PrintNode(*node_ptr);
            } else {
                Println("adding to new node");
                new_node->insert(val, ind);
                PrintNode(*new_node);
            }
        } catch(const std::exception& ex) {
            Println(ex.what(), "catched an error");
            DeleteNode(new_node);
            throw;
        }
        // Println("swap *node_ptr, old_node");
        // std::swap(*node_ptr, old_node);
        Println("Linking nodes");
        LinkNodes(prev_node_ptr, node_ptr);
        LinkNodes(node_ptr, new_node);
        LinkNodes(new_node, next_node_ptr);
        Println("Linked nodes:");
        Println("*node_ptr->prev()");
        if (node_ptr->prev())
            PrintNode(*node_ptr->prev());
        Println("*node_ptr");
        PrintNode(*node_ptr);
        Println("*node_ptr->next()");
        if (node_ptr->next())
            PrintNode(*node_ptr->next());
        Println("*new_node->prev()");
        if (new_node->prev())
            PrintNode(*new_node->prev());
        Println("*new_node");
        PrintNode(*new_node);
        Println("*new_node->next()");
        if (new_node->next())
            PrintNode(*new_node->next());

        Println("End of function");
        Println(ind, "ind");
        return new_node;
    }
    // iterator AddToFullNode(const_iterator pos, const_reference val) {
    //     NodeT* node_ptr = pos.node_ptr;
    //     NodeT* prev_node_ptr = node_ptr->prev();
    //     NodeT* next_node_ptr = node_ptr->next();
    //     bool is_first = first_node_ == node_ptr;
    //     bool is_last = last_node_ == node_ptr;
    //     Println<bool>(!node_ptr, "node_ptr is null");
    //     difference_type ind = pos.ind;
    //     Println(ind, "ind");
    //     // for swap and copy idiom
    //     NodeT old_node = *node_ptr;
    //     Println("assigned old node");
    //     NodeT* new_node = CreateEmptyNode();
    //     Println("Created an empty node");
    //     bool add_to_left = false;
    //     try {
    //         difference_type left_sz = NodeMaxSize / 2;
    //         Println(left_sz, "left size");
    //         difference_type right_sz = NodeMaxSize - left_sz;
    //         Println(right_sz, "right size");
    //         if (ind <= left_sz) {
    //             add_to_left = true;
    //         } else {
    //             add_to_left = false;
    //             if (left_sz <= right_sz) {
    //                 ++left_sz;
    //                 --right_sz;
    //             }
    //             ind -= left_sz;
    //         }
    //         Println(left_sz, "updated left_size");
    //         Println(right_sz, "updated right_size");
    //         Println(ind, "updated index");
    //         Println("Moving to right");
    //         MoveToRight(old_node, *new_node, right_sz);
    //         PrintNode(old_node);
    //         PrintNode(*new_node);
    //         if (add_to_left) {
    //             Println("adding to old node");
    //             old_node.insert(val, ind);
    //             PrintNode(old_node);
    //         } else {
    //             Println("adding to new node");
    //             new_node->insert(val, ind);
    //             PrintNode(*new_node);
    //         }
    //     } catch(const std::exception& ex) {
    //         Println(ex.what(), "catched an error");
    //         delete new_node;
    //         throw;
    //     }
    //     Println("swap *node_ptr, old_node");
    //     std::swap(*node_ptr, old_node);
    //     Println("Linking nodes");
    //     Println(last_node_);
    //     LinkNodes(prev_node_ptr, node_ptr);
    //     LinkNodes(node_ptr, new_node);
    //     LinkNodes(new_node, next_node_ptr);
    //     Println("Linked nodes:");
    //     Println("*node_ptr->prev()");
    //     if (node_ptr->prev())
    //         PrintNode(*node_ptr->prev());
    //     Println("*node_ptr");
    //     PrintNode(*node_ptr);
    //     Println("*node_ptr->next()");
    //     if (node_ptr->next())
    //         PrintNode(*node_ptr->next());
    //     Println("*new_node->prev()");
    //     if (new_node->prev())
    //         PrintNode(*new_node->prev());
    //     Println("*new_node");
    //     PrintNode(*new_node);
    //     Println("*new_node->next()");
    //     if (new_node->next())
    //         PrintNode(*new_node->next());

    //     if (is_last) {
    //         last_node_ = new_node;
    //     }
    //     if (is_first) {
    //         first_node_ = node_ptr;
    //     }
    //     Println("End of function");
    //     Println(ind, "ind");
    //     if (add_to_left) {
    //         Println("added to left");
    //         return iterator{node_ptr, ind};
    //     } else {
    //         Println("added to right");
    //         return iterator{new_node, ind};
    //     }
    // }
    void LinkNodes(NodeT* left, NodeT* right) noexcept {
        if (right) {
            right->SetPrev(left);
        }
        if (left) {
            left->SetNext(right);
        }
    }
    NodeT* CreateEmptyNode(/* NodeT* prev = nullptr, NodeT* next = nullptr */) {
        NodeT* new_node = AllocatorTraitsNode::allocate(alloc_node_, 1);
        AllocatorTraitsNode::construct(alloc_node_, new_node, alloc_t_);
        return new_node;
    }
    std::pair<NodeT*, NodeT*> SplitNode(NodeT* node, size_t pos = NodeMaxSize / 2) {
        NodeT* left_node = CreateEmptyNode();
        try {
            MoveToLeft(*left_node, node, pos);
        } catch(const std::exception& ex) {
            DeleteNode(left_node);
            throw;
        }
        return {left_node, node};
    }
    struct MergeNodeRetVal {
        NodeT* filled_node;
        NodeT* reduced_node;
    };
    MergeNodeRetVal MergeNodes(NodeT* left_node, NodeT* right_node) {
        MergeNodeRetVal ret_val {nullptr, nullptr};
        if (!left_node) {
            ret_val.filled_node = right_node;
            ret_val.reduced_node = left_node;
            return ret_val;
        }
        if (!right_node) {
            ret_val.filled_node = left_node;
            ret_val.reduced_node = right_node;
            return ret_val;
        }
        if (left_node->size() < right_node->size()) {
            ret_val.filled_node = right_node;
            ret_val.reduced_node = left_node;
            MoveToRight(*left_node, *right_node);
            return ret_val;
        } else {
            ret_val.filled_node = left_node;
            ret_val.reduced_node = right_node;
            MoveToLeft(*left_node, *right_node);
            return ret_val;
        }
    }
 private:
    Allocator alloc_t_;
    AllocatorNode alloc_node_;
    difference_type size_;
    NodeT* last_node_;
    NodeT* first_node_;
};

template<typename T, size_t NodeMaxSize, typename Allocator>
void swap(unrolled_list<T, NodeMaxSize, Allocator>& l1, unrolled_list<T, NodeMaxSize, Allocator>& l2) {
    l1.swap(l2);
}

// template<typename T, size_t NodeMaxSize, typename Allocator>
// class unrolled_list<T, NodeMaxSize, Allocator>::BaseIterator {
//  protected:
//     using _MyList = unrolled_list<T, NodeMaxSize, Allocator>;
//     static const difference_type kStartInd = 0;
//     BaseIterator() = default;
//     BaseIterator(NodeT* node, difference_type pos = kStartInd)
//         : node_ptr(node), ind(pos) {}
//  public:
//     bool operator==(const BaseIterator& other) const { return node_ptr == other.node_ptr && ind == other.ind; }
//     bool operator!=(const BaseIterator& other) const { return !((*this) == other); }
//     BaseIterator& operator++() {
//         difference_type new_ind = ind + 1;
//         if (!IsLastNode(node_ptr) && IsEndOfNode()) {
//             node_ptr = node_ptr->next();
//             new_ind = kStartInd;
//         }
//         ind = new_ind;
//         return *this;
//     }
//     BaseIterator& operator--() {
//         difference_type new_ind = ind - 1;
//         if (!IsFirstNode(node_ptr) && IsBeginOfNode()) {
//             node_ptr = node_ptr->prev();
//             new_ind = node_ptr->size() - 1 + kStartInd;
//         }
//         ind = new_ind;
//         return *this;
//     }
//     friend class unrolled_list<T, NodeMaxSize, Allocator>;
//  protected:
//     bool IsEndOfNode() const { return node_ptr->size() - 1 + kStartInd == ind; }
//     bool IsBeginOfNode() const { return ind == kStartInd; }
//  protected:
//     NodeT* node_ptr;
//     difference_type ind;
// };

// template<typename T, size_t NodeMaxSize, typename Allocator>
// class unrolled_list<T, NodeMaxSize, Allocator>::const_iterator
//     : public unrolled_list<T, NodeMaxSize, Allocator>::BaseIterator
// {
//     using _MyList = unrolled_list<T, NodeMaxSize, Allocator>;
//     using _MyBase = _MyList::BaseIterator;
//     using _MyBase::kStartInd;
//     using _MyBase::ind;
//     using _MyBase::node_ptr;
//  public:
//     using iterator_category = std::bidirectional_iterator_tag;
//     using difference_type = _MyList::difference_type;
//     using pointer = const _MyList::pointer;
//     using reference = _MyList::const_reference;
//     using value_type = _MyList::value_type;
//     const_iterator() = default;
//     const_iterator(NodeT* node, difference_type pos = _MyBase::kStartInd)
//         : BaseIterator(node, pos) {}
//     const_iterator(const _MyList::iterator& it)
//         : BaseIterator(it) {}
//     const T& operator*() const {
//         // Println(ind, "iter ind");
//         return (*node_ptr)[ind - kStartInd];
//     }
//     const pointer operator->() const {
//         return &(**this);
//     }
//     const_iterator& operator++() {
//         _MyBase::operator++();
//         return (*this);
//     }
//     const_iterator operator++(int) {
//         const_iterator copy = (*this);
//         ++(*this);
//         return copy;
//     }
//     const_iterator& operator--() {
//         _MyBase::operator--();
//         return (*this);
//     }
//     const_iterator operator--(int) {
//         const_iterator copy = (*this);
//         --(*this);
//         return copy;
//     }
// };

// template<typename T, size_t NodeMaxSize, typename Allocator>
// class unrolled_list<T, NodeMaxSize, Allocator>::iterator
//     : public unrolled_list<T, NodeMaxSize, Allocator>::BaseIterator
// {
//     using _MyBase = unrolled_list<T, NodeMaxSize, Allocator>::BaseIterator;
//     using _MyList = _MyBase::_MyList;
//     // static const difference_type kStartInd = 0;
//  public:
//     using iterator_category = std::bidirectional_iterator_tag;
//     using difference_type = _MyList::difference_type;
//     using pointer = _MyList::pointer;
//     using reference = _MyList::reference;
//     using value_type = _MyList::value_type;
//     iterator() = default;
//     iterator(NodeT* node, difference_type pos = _MyBase::kStartInd)
//         : _MyBase(node, pos) {}
//     bool operator==(const iterator& other) const { return _MyBase::operator==(other); }
//     bool operator!=(const iterator& other) const { return !((*this) == other); }
//     T& operator*() const { return (*_MyBase::node_ptr)[_MyBase::ind - _MyBase::kStartInd]; }
//     pointer operator->() const {
//         return &(**this);
//     }
//     iterator& operator++() { 
//         _MyBase::operator++();
//         return (*this);
//     }
//     iterator operator++(int) {
//         iterator copy = (*this);
//         ++(*this);
//         return copy;
//     }
//     iterator& operator--() {
//         _MyBase::operator--();
//         return (*this);
//     }
//     iterator operator--(int) {
//         iterator copy = (*this);
//         --(*this);
//         return copy;
//     }
// };
