#include <unrolled_list.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>
#include <list>

/*
    В данном файле представлен ряд тестов, где используются (вместе, раздельно и по-очереди):
        - push_back
        - push_front
        - insert
    Методы применяются на unrolled_list и на std::list.
    Ожидается, что в итоге порядок элементов в контейнерах будут идентичен
*/

TEST(UnrolledLinkedList, pushBack) {
    std::list<int> std_list;
    unrolled_list<int> unrolled_list;

    for (int i = 0; i < 1000; ++i) {
        std_list.push_back(i);
        unrolled_list.push_back(i);
    }

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));
}

TEST(UnrolledLinkedList, pushFront) {
    std::list<int> std_list;
    unrolled_list<int> unrolled_list;

    for (int i = 0; i < 1000; ++i) {
        std_list.push_front(i);
        unrolled_list.push_front(i);
    }

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));
}

TEST(UnrolledLinkedList, pushMixed) {
    std::list<int> std_list;
    unrolled_list<int> unrolled_list;

    for (int i = 0; i < 1000; ++i) {
        if (i % 2 == 0) {
            std_list.push_front(i);
            unrolled_list.push_front(i);
        } else {
            std_list.push_back(i);
            unrolled_list.push_back(i);
        }
    }

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));
}

TEST(UnrolledLinkedList, insertAndPushMixed) {
    std::list<int> std_list;
    unrolled_list<int> unrolled_list;

    for (int i = 0; i < 10000; ++i) {
        if (i % 3 == 0) {
            std_list.push_front(i);
            unrolled_list.push_front(i);
        } else if (i % 3 == 1) {
            std_list.push_back(i);
            unrolled_list.push_back(i);
        } else {
            auto std_it = std_list.begin();
            auto unrolled_it = unrolled_list.begin();
            std::advance(std_it, std_list.size() / 2);
            std::advance(unrolled_it, std_list.size() / 2);
            std_list.insert(std_it, i);
            unrolled_list.insert(unrolled_it, i);
        }
    }
    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));
}

TEST(UnrolledLinkedList, popFrontBack) {
    std::list<int> std_list;
    unrolled_list<int> unrolled_list;

    for (int i = 0; i < 1000; ++i) {
        std_list.push_back(i);
        unrolled_list.push_back(i);
    }

    for (int i = 0; i < 500; ++i) {
        if (i % 2 == 0) {
            std_list.pop_back();
            unrolled_list.pop_back();
        } else {
            std_list.pop_front();
            unrolled_list.pop_front();
        }
    }

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));

    for (int i = 0; i < 500; ++i) {
        std_list.pop_back();
        unrolled_list.pop_back();
    }

    ASSERT_TRUE(unrolled_list.empty());
}

TEST(UnrolledLinkedListAdditionalTests, nodePushPopTest){
    static const int kMaxElemSize = 5;
    unrolled_list<int, kMaxElemSize> ui;
    std::list<int> result;

    for (int i = 0; i < kMaxElemSize + 1; ++i) {
        ui.push_back(i * i);
        result.push_back(i * i);
    }

    for (int i = 0; i < kMaxElemSize - 1; ++i) {
        ui.pop_front();
        result.pop_front();
    }
    ASSERT_TRUE(std::equal(result.begin(), result.end(), ui.begin()));
}

TEST(UnrolledLinkedListAdditionalTests, assignTest) {
    const size_t kMaxNodeSize = 5;
    using MyList = unrolled_list<int, kMaxNodeSize>;
    std::list<int> init_list1, init_list2;
    MyList ul1, ul2;
    for(int i = 0; i < kMaxNodeSize + 1; ++i) {
        init_list1.push_back(i * i);
        init_list2.push_back(i * i * i);
    }
    ul1.assign(init_list1.begin(), init_list1.end());
    ul2.assign(init_list2.begin(), init_list2.end());
    ASSERT_TRUE(std::equal(init_list1.begin(), init_list1.end(), ul1.begin()));
    ASSERT_TRUE(std::equal(init_list2.begin(), init_list2.end(), ul2.begin()));
}

TEST(UnrolledLinkedListAdditionalTests, assignAndSwapTest) {
    const size_t kMaxNodeSize = 5;
    using MyList = unrolled_list<int, kMaxNodeSize>;
    std::list<int> init_list1, init_list2;
    MyList ul1, ul2;
    for(int i = 0; i < kMaxNodeSize + 1; ++i) {
        init_list1.push_back(i * i);
        init_list2.push_back(i * i * i);
    }
    ul1.assign(init_list1.begin(), init_list1.end());
    ul2.assign(init_list2.begin(), init_list2.end());
    swap(ul1, ul2);
    std::swap(init_list1, init_list2);
    ASSERT_TRUE(std::equal(init_list1.begin(), init_list1.end(), ul1.begin()));
    ASSERT_TRUE(std::equal(init_list2.begin(), init_list2.end(), ul2.begin()));
    *ul1.begin() = 100;
    *init_list1.begin() = 100;
    ASSERT_TRUE(std::equal(init_list1.begin(), init_list1.end(), ul1.begin()));
    ASSERT_TRUE(std::equal(init_list2.begin(), init_list2.end(), ul2.begin()));
}

TEST(NodeTests, InsertTest) {
    // (p_-)
    const int MaxSize = 10;
    using AllocT = std::allocator<int>;
    using Traits = std::allocator_traits<AllocT>;
    using NodeT = Node<int, MaxSize, AllocT>;

    std::allocator<int> alloc;
    std::vector<int> vec;
    NodeT node(alloc);
    for (int i = 0; i < MaxSize; ++i) {
        node.insert(i, i / 2);
        vec.insert(vec.cbegin() + i / 2, i);
    }
    for (size_t ind = 0; ind < MaxSize; ++ind) {
        ASSERT_EQ(node[ind], vec[ind]);
    }
}

TEST(UnrolledLinkedListAdditionalTests, EndInsertTest) {
    const int MaxSize = 5;
    using AllocT = std::allocator<int>;
    using Traits = std::allocator_traits<AllocT>;
    using NodeT = Node<int, MaxSize, AllocT>;
    
    unrolled_list<int, MaxSize, AllocT> ul;
    std::list<int> res;
    for (int i = 0; i < 2 * MaxSize + 1; ++i) {
        ul.insert(ul.end(), i * i);
        res.insert(res.end(), i * i);
    }
    ASSERT_TRUE(std::equal(res.begin(), res.end(), ul.begin()));
}

TEST(UnrolledLinkedListAdditionalTests, PopEraseMixed) {
    const int MaxSize = 5;
    using AllocT = std::allocator<int>;
    using Traits = std::allocator_traits<AllocT>;
    using NodeT = Node<int, MaxSize, AllocT>;
    using List = unrolled_list<int, MaxSize, AllocT>;
    using StdList = std::list<int>;
    List ul;
    StdList res;
    for (int i = 0; i < 1000; ++i) {
        ul.push_back(i);
        res.push_back(i);
    }
    for (int i = 0; i < 500; ++i) {
        switch (i % 3) {
        case 0:
            ul.pop_back();
            res.pop_back();
            break;
        case 1:
            ul.pop_front();
            res.pop_front();
            break;
        case 2:
            List::const_iterator it = ul.cbegin();
            std::advance(it, i);
            StdList::const_iterator std_it = res.begin();
            std::advance(std_it, i);
            ul.erase(it);
            res.erase(std_it);
            break;
        }
    }
    ASSERT_THAT(ul, ::testing::ElementsAreArray(res));
}

TEST(UnrolledLinkedListAdditionalTests, EraseRange) {
    const int MaxSize = 5;
    using AllocT = std::allocator<int>;
    using Traits = std::allocator_traits<AllocT>;
    using NodeT = Node<int, MaxSize, AllocT>;
    using List = unrolled_list<int, MaxSize, AllocT>;
    using StdList = std::list<int>;
    List ul;
    StdList res;
    for (int i = 0; i < 973; ++i) {
        ul.push_back(i);
        res.push_back(i);
    }
    const int kFrom = 120;
    const int kTo = 973;
    List::const_iterator from = ul.begin();
    std::advance(from, kFrom);
    List::const_iterator to = ul.begin();
    std::advance(to, kTo);
    StdList::const_iterator std_from = res.begin();
    std::advance(std_from, kFrom);
    StdList::const_iterator std_to = res.begin();
    std::advance(std_to, kTo);

    ul.erase(from, to);
    res.erase(std_from, std_to);
    ASSERT_THAT(ul, ::testing::ElementsAreArray(res));
}