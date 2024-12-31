#include "type_list/list.h"

namespace tl {

static constexpr auto l0 = list<>;                  // NOLINT
static constexpr auto l1 = list<int, float, char>;  // NOLINT

static_assert(size(l0) == 0);
static_assert(empty(l0));

static_assert(size(l1) == 3);
static_assert(!empty(l1));

static_assert(contains<int>(l1));
static_assert(contains<float>(l1));
static_assert(contains<char>(l1));

// Alternative
static_assert(contains(l1, type<int>));

static_assert(!contains<double>(l1));
static_assert(!contains<void>(l1));
static_assert(!contains<double>(l0));
static_assert(!contains<void>(l0));

template <typename L>
constexpr size_t countTypes(L list) {
    size_t c = 0;
    forEach([&c](auto&&) { ++c; }, list);
    return c;
}

static_assert(countTypes(l0) == 0);
static_assert(countTypes(l1) == 3);

static_assert(find<int>(l1) == 0);
static_assert(find<float>(l1) == 1);
static_assert(find<char>(l1) == 2);

// Alternative
static_assert(find(l1, type<char>) == 2);

static_assert(head(l1) == type<int>);
static_assert(tail(l1) == list<float, char>);

static_assert(at<0>(l1) == type<int>);
static_assert(at<1>(l1) == type<float>);
static_assert(at<2>(l1) == type<char>);

static_assert(concat() == list<>);
static_assert(concat(l0, l0) == list<>);
static_assert(concat(l1, l0) == list<int, float, char>);
static_assert(concat(l0, l1) == list<int, float, char>);
static_assert(concat(l1, l1) == list<int, float, char, int, float, char>);

static_assert(pushBack<double>(l1) == list<int, float, char, double>);
static_assert(pushBack<double>(l0) == list<double>);

// Alternative
static_assert(pushBack(l0, Type<double>{}) == list<double>);

static_assert(pushFront<double>(l1) == list<double, int, float, char>);
static_assert(pushFront<double>(l0) == list<double>);

// Alternative
static_assert(pushFront(l0, type<double>) == list<double>);

static constexpr auto ll = list<List<int, float>, List<float, char>>;  // NOLINT
static_assert(flatten(ll) == list<int, float, float, char>);

static_assert(take<0>(l1) == list<>);
static_assert(take<1>(l1) == list<int>);
static_assert(take<2>(l1) == list<int, float>);
static_assert(take<3>(l1) == list<int, float, char>);

struct TestPred {
    template <typename T>
    static constexpr bool test() noexcept {
        return !std::is_same_v<T, float>;
    }
};

static_assert(filter<TestPred>(l1) == list<int, char>);
static_assert(filter<TestPred>(l0) == list<>);

struct TestMapper {
    template <typename T>
    using Map = void;
};

static_assert(map<TestMapper>(l1) == list<void, void, void>);

static_assert(prod(l0) == l0);
static_assert(prod(l1) == list<List<int>, List<float>, List<char>>);
static_assert(prod(l0, l0) == l0);
static_assert(prod(l1, l1) == list<List<int, int>, List<float, float>, List<char, char>>);
static_assert(prod(list<List<>>, list<int>) == list<List<List<>, int>>);

static_assert(unique(l0) == l0);
static_assert(unique(l1) == l1);
static_assert(unique(list<int, int, float, char, char>) == list<int, float, char>);
static_assert(unique(list<int, int, int, int>) == list<int>);

static_assert(intersect(l0, l0) == l0);
static_assert(intersect(l1, l1) == l1);
static_assert(intersect(l1, l0) == l0);
static_assert(intersect(l0, l1) == l0);
static_assert(intersect(list<int, float, char>, list<int, char, double>) == list<int, char>);
static_assert(intersect(list<int>, list<float>) == list<>);
static_assert(intersect(list<int>, list<int>, list<float>) == list<>);
static_assert(intersect(list<int, char>, list<float, int>, list<int, double>) == list<int>);

static_assert(unite(list<>, list<int>, list<char, int>) == list<char, int>);

static_assert(isSubset(l0, l1));
static_assert(!isSubset(l1, l0));
static_assert(isSubset(list<int, float>, list<char, float, double, int>));

static_assert(isASet(l0));
static_assert(isASet(l1));
static_assert(!isASet(list<int, int>));
static_assert(!isASet(list<int, float, int>));

static_assert(subtract(l0, l0) == l0);
static_assert(subtract(l1, l0) == l1);
static_assert(subtract(l0, l1) == l0);
static_assert(subtract(l1, l1) == l0);
static_assert(subtract(list<int>, list<float>) == list<int>);
static_assert(subtract(list<int>, list<int>) == list<>);
static_assert(subtract(list<int, float>, list<int>) == list<float>);
static_assert(subtract(list<int, float>, list<float>) == list<int>);

static_assert(powerset(l0) == list<List<>>);
static_assert(powerset(list<int>) == list<List<int>, List<>>);
static_assert(powerset(list<int, float>) == list<List<int, float>, List<int>, List<float>, List<>>);

static_assert(injection(l0, l1).empty());
static_assert(injection(l0, l0).empty());
static_assert(injection(l1, l1).size() == 3);
static_assert(injection(l1, l1) == std::array{0UL, 1UL, 2UL});
static_assert(injection(list<int>, list<int, float>) == std::array{0UL});
static_assert(injection(list<int>, list<float, int>) == std::array{1UL});
static_assert(injection(list<int>, list<float, int, char>) == std::array{1UL});

}  // namespace tl
