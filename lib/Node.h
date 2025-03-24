#pragma once

#include <memory>
#include <iterator>
#include <cstdint>
#include <stdexcept>

#include <utility>
#include <vector>

#include "DebugPrint.h"

template<typename T, size_t MaxSize = 10, typename Allocator = std::allocator<T>>
class Node {
public:
    constexpr static size_t kMaxSize = MaxSize; 
    using AllocatorTraitsT = std::allocator_traits<Allocator>;
    using value_type = T;
    Node(const Allocator& allocator) 
        : allocator_(allocator) {
            data_ = reinterpret_cast<T*>(raw_memory_);
            Println("creating node");
        }
    Node& operator=(const Node& other) {
        for (size_t i = 0; i < size_; ++i) {
            size_t memory_ind = CalcModInd(left_ + i);
            AllocatorTraitsT::destroy(allocator_, data_ + memory_ind);
        }
        left_ = other.left_;
        size_ = other.size_;
        prev_ = other.prev_;
        next_ = other.next_;
        for (size_t i = 0; i < size_; ++i) {
            size_t memory_ind = CalcModInd(left_ + i);
            AllocatorTraitsT::construct(allocator_, data_ + memory_ind, other[i]);
        }
        return *this;
    }
    Node(const Node& other)
        : Node(other.allocator_)
    {
        left_ = other.left_;
        size_ = other.size_;
        prev_ = other.prev_;
        next_ = other.next_;
        Println("in constructor");
        for (size_t i = 0; i < size_; ++i) {
            size_t memory_ind = CalcModInd(left_ + i);
            AllocatorTraitsT::construct(allocator_, data_ + memory_ind, other[i]);
        }
        Println("end of node constructor");
    }
    ~Node() {
        for (size_t ind = 0; ind < size_; ++ind) {
            AllocatorTraitsT::destroy(allocator_, data_ + CalcModInd(left_ + ind));
        }
    }
    void SetNext(Node* next) {
        next_ = next;
    }
    void SetPrev(Node* prev) {
        prev_ = prev;
    }
    Node* next() { return next_; }
    Node* prev() { return prev_; }
    const Node* next() const { return next_; }
    const Node* prev() const { return prev_; }
    size_t size() const { return size_; }
    size_t capacity() const { return MaxSize; }
    bool full() const { return size_ == MaxSize; }
    bool empty() const { return size_ == 0; }
    const T& front() const { return data_[left_]; }
    const T& back() const { return data_[CalcModInd(left_ + size_ - 1)];}
    // returns true if pushing value succeded
    bool push_back(const T& val) {
        if (full()) {
            return false;
        }
        if (size_ == 0) {
            SetStartLeft();
        }
        size_t real_new_ind = (left_ + size_) % MaxSize;
        Println("constructing new element");
        AllocatorTraitsT::construct(allocator_, data_ + real_new_ind, val);
        Println("constructed succesfully!");
        ++size_;
        return true;
    }
    // use only if size != 0
    void pop_back() noexcept {
        if (empty()) {
            return;
        }
        size_t real_del_ind = CalcModInd(left_ + size_);
        try {
            AllocatorTraitsT::destroy(allocator_, data_ + real_del_ind);
        } catch (const std::exception& ex) {
            return;
        }
        --size_;
    }
    // returns true if pushing value succeded
    bool push_front(const T& val) {
        if (full()) {
            return false;
        }
        if (size_ == 0) {
            SetStartLeft();
        }
        size_t memory_ind = CalcModInd(left_ - 1);
        AllocatorTraitsT::construct(allocator_, data_ + memory_ind, val);
        left_ = memory_ind;
        ++size_;
        return true;
    }
    // use only if size != 0
    void pop_front() noexcept {
        if (empty()) {
            return;
        }
        size_t real_del_ind = left_;
        try {
            AllocatorTraitsT::destroy(allocator_, data_ + real_del_ind);
        } catch (const std::exception& ex) {
            return;
        }
        --size_;
        left_ = CalcModInd(left_ + 1);
    }
    // use only if size < MaxSize
    void insert(const T& val, size_t ind) {
        Println(ind, "ind");
        Println(size_, "size");
        Println(left_, "left");
        if (full()) {
            return;
        }
        if (empty()) {
            push_back(val);
            return;
        }
        size_t new_memory_index;
        size_t required_memory_index;
        if (ind < (size_ + 1) / 2) {
            new_memory_index = FirstBeforeBeginMemoryIndex();
            required_memory_index = CalcModInd(new_memory_index + ind);
            push_front(val);
        } else {
            new_memory_index = EndMemoryIndex();
            required_memory_index = CalcModInd(new_memory_index - (size_ - ind));
            push_back(val);
        }
        Println(new_memory_index, "new memory index");
        Println(required_memory_index, "required memory index");
        SiftInsert(new_memory_index, required_memory_index);
    }
    // use only if size != 0
    void erase(size_t ind) noexcept {
        if (empty()) {
            return;
        }
        try {
            size_t del_cell_ind = CalcModInd(left_ + ind);
            if (ind < size_ / 2) {
                SiftInsert(del_cell_ind, BeginMemoryIndex());
                pop_front();
            } else {
                SiftInsert(del_cell_ind, FirstBeforeEndMemoryIndex());
                pop_back();
            }
        } catch (const std::exception& ex) {}
        // size_t required_memory_ind = CalcModInd(left_ + ind);
        // size_t del_memory_ind = FindClosest(required_memory_ind
        //         , BeginMemoryIndex(), FirstBeforeEndMemoryIndex());
        // SiftInsert(required_memory_ind, del_memory_ind);
        // DeleteCell(del_memory_ind);
    }
    // ind is [0 size)
    T& operator[](size_t ind) {
        size_t real_pos = CalcModInd(left_ + ind);
        return data_[real_pos];
    }
    const T& operator[](size_t ind) const {
        size_t real_pos = CalcModInd(left_ + ind);
        return data_[real_pos];
    }
 private:
    size_t EndMemoryIndex() const { return CalcModInd(left_ + size_); }
    size_t FirstBeforeBeginMemoryIndex() const { return CalcModInd(left_ - 1); }
    size_t BeginMemoryIndex() const { return CalcModInd(left_); }
    size_t FirstBeforeEndMemoryIndex() const { return CalcModInd(left_ + size_ - 1); }
    static size_t CalcModInd(size_t ind) {
        return (ind + MaxSize) % MaxSize;
    }
    size_t FindClosest(size_t required_pos, size_t memory_ind1, size_t memory_ind2) const {
        return (CalcDist(required_pos, memory_ind1) < CalcDist(required_pos, memory_ind2) ? memory_ind1 : memory_ind2);
    }
    size_t CalcDist(size_t p1, size_t p2) const {
        if (p1 < p2) {
            std::swap(p1, p2);
        }
        return std::min(p1 - p2, MaxSize - (p1 - p2));
    }
    void IncludeCell(size_t memory_ind, const T& val) {
        if (memory_ind == FirstBeforeBeginMemoryIndex()) {
            push_front(val);
        } else if (memory_ind == EndMemoryIndex()) {
            push_back(val);
        } else {
            return;
        }
    }
    void DeleteCell(size_t memory_ind) {
        if (memory_ind == BeginMemoryIndex()) {
            pop_front();
        } else if (memory_ind == FirstBeforeEndMemoryIndex()) {
            pop_back();
        } else {
            return;
        }
    }
    void SetStartLeft() {
        left_ = MaxSize / 2;
    }
    void SiftInsert(size_t from, size_t to) {
        Println("In SiftInsert");
        size_t count = CalcDist(from, to);
        Println(to, "to");
        Println(from, "from");
        Println(count, "count");
        if (CalcModInd(from + count) == to) {
            SiftRight(from, count);
        } else {
            SiftLeft(from, count);
        }
    }
    void SiftRight(size_t from, size_t count) {
        Println("sifting right");
        Println(count, "count");
        while(count--) {
            std::swap(data_[CalcModInd(from)], data_[CalcModInd(from + 1)]);
            ++from;
        }
    }
    void SiftLeft(size_t from, size_t count) {
        Println("sifting left");
        Println(count, "count");
        while (count--) {
            std::swap(data_[CalcModInd(from)], data_[CalcModInd(from - 1)]);
            --from;
        }
    }
    // friend Node<T, MaxSize, Allocator> Split(Node<T, MaxSize, Allocator>& node, size_t pos);
    // friend bool CanBeMerged(const Node<T, MaxSize, Allocator>& left, const Node<T, MaxSize, Allocator>& right) noexcept;
    // friend void MoveToLeft(Node<T, MaxSize, Allocator>& left, Node<T, MaxSize, Allocator>& right);
    // friend void MoveToRight(Node<T, MaxSize, Allocator>& left, Node<T, MaxSize, Allocator>& right);
 private:
    alignas(T) char raw_memory_[MaxSize * sizeof(T)];
    Allocator allocator_;
    T* data_;
    size_t size_ = 0;
    size_t left_;
    Node* next_ = nullptr;
    Node* prev_ = nullptr;
};

// template<typename T, size_t MaxSize = 10, typename Allocator = std::allocator<T>>
// Node<T, MaxSize, Allocator> Split(Node<T, MaxSize, Allocator>& node, size_t pos) {
// }
template<typename T, size_t MaxSize = 10, typename Allocator = std::allocator<T>>
bool CanBeMerged(const Node<T, MaxSize, Allocator>& left, const Node<T, MaxSize, Allocator>& right) noexcept {
    return left.size() + right.size() <= MaxSize;
}
template<typename T, size_t MaxSize = 10, typename Allocator = std::allocator<T>>
void MoveToLeft(Node<T, MaxSize, Allocator>& left, Node<T, MaxSize, Allocator>& right, size_t max_len = MaxSize) {
    if (right.empty()) {
        return;
    }
    max_len = std::min(right.size(), max_len);
    max_len = std::min(MaxSize - left.size(), max_len);
    for (int i = 0; i < max_len; ++i) {
        Println(right.front(), "right.front()");
        Println(left.size(), "left.size()");
        left.push_back(right.front());
        right.pop_front();
    }
}
template<typename T, size_t MaxSize = 10, typename Allocator = std::allocator<T>>
void MoveToRight(Node<T, MaxSize, Allocator>& left, Node<T, MaxSize, Allocator>& right, size_t max_len = MaxSize) {
    if (left.empty()) {
        return;
    }
    max_len = std::min(left.size(), max_len);
    max_len = std::min(MaxSize - right.size(), max_len);
    for (int i = 0; i < max_len; ++i) {
        right.push_front(left.back());
        left.pop_back();
    }
}

// template<typename T, size_t MaxSize = 10, typename Allocator = std::allocator<T>>
// std::pair<Node<T, MaxSize, Allocator>*, Node<T, MaxSize, Allocator>*> Split(Node<T, MaxSize, Allocator>* node) {
//     using NodePtr =Node<T, MaxSize, Allocator>*;

// } 
