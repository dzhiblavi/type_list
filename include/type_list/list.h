#pragma once

#include <array>
#include <cstddef>
#include <type_traits>

namespace tl {

template <typename T>
struct Type {
    using type = T;

    template <typename U>
    constexpr bool operator==(const Type<U>&) const noexcept {
        return std::is_same_v<U, T>;
    }
};

template <typename T>
static constexpr Type<T> type{};  // NOLINT

template <typename... Ts>
struct List {
    static constexpr size_t npos = size_t(-1);  // NOLINT

    template <typename... Us>
    constexpr bool operator==(const List<Us...>&) const noexcept {
        return false;
    }

    constexpr bool operator==(const List&) const noexcept {
        return true;
    }
};

template <typename... Ts>
static constexpr List<Ts...> list{};  // NOLINT

template <auto Start, auto End, auto Inc, class F>
constexpr void forEach(const F& f) {
    if constexpr (Start < End) {
        f(std::integral_constant<decltype(Start), Start>());
        forEach<Start + Inc, End, Inc>(f);
    }
}

template <class F, class... Args>
constexpr void forEach(F f, Args&&... args) {
    (f(std::forward<Args>(args)), ...);
}

template <typename F, typename... Ts>
constexpr void forEach(F f, List<Ts...>) {
    forEach(std::move(f), Type<Ts>{}...);
}

template <typename F, typename... Ts>
constexpr void forEachIndexed(F f, List<Ts...>) {
    size_t i = 0;
    (f(i++, Type<Ts>{}), ...);
}

template <typename... Ts>
constexpr size_t size(List<Ts...>) noexcept {
    return sizeof...(Ts);
}

template <typename L>
static constexpr size_t Size = size(L{});

template <typename... Ts>
constexpr bool empty(List<Ts...> list) noexcept {
    return size(list) == 0;
}

template <typename L>
static constexpr bool Empty = empty(L{});

template <typename T, typename... Ts>
constexpr bool contains(List<Ts...>, Type<T> = {}) noexcept {
    return (std::is_same_v<T, Ts> || ...);
}

template <typename L, typename T>
concept Contains = contains(L{}, Type<T>{});

template <typename T, Contains<T> L>
constexpr size_t find(L list, Type<T> = {}) noexcept {
    size_t result = L::npos;

    forEachIndexed(
        [&result]<typename U>(size_t i, Type<U>) {
            if (std::is_same_v<T, U>) {
                result = i;
            }
        },
        list);

    return result;
}

template <typename T, typename L>
static constexpr size_t Find = find(L{}, Type<T>{});

template <typename T, typename... Ts>
constexpr auto head(List<T, Ts...>) noexcept {
    return Type<T>{};
}

template <typename L>
using Head = decltype(head(L{}));

template <typename T, typename... Ts>
constexpr auto tail(List<T, Ts...>) noexcept {
    return List<Ts...>{};
}

template <typename L>
using Tail = decltype(tail(L{}));

template <size_t Index, typename... Ts>
requires(Index < sizeof...(Ts))
constexpr auto at(List<Ts...> list) noexcept {
    if constexpr (Index == 0) {
        return head(list);
    } else {
        return at<Index - 1>(tail(list));
    }
}

template <size_t Index, typename L>
using At = decltype(at<Index>(L{}));

namespace detail {

template <typename... As, typename... Bs>
constexpr List<As..., Bs...> concat2(List<As...>, List<Bs...>) noexcept {
    return {};
}

}  // namespace detail

template <typename... Ts>
constexpr List<Ts...> concat(List<Ts...>) noexcept {
    return {};
}

template <typename... Ts, typename... Lists>
constexpr auto concat(List<Ts...> head, Lists... tail) noexcept {
    return detail::concat2(head, concat(tail...));
}

template <typename... Lists>
using Concat = decltype(concat(Lists{}...));

template <typename T, typename... Ts>
constexpr List<Ts..., T> pushBack(List<Ts...>, Type<T> = {}) noexcept {
    return {};
}

template <typename L, typename T>
using PushBack = decltype(pushBack(L{}, Type<T>{}));

template <typename T, typename... Ts>
constexpr List<T, Ts...> pushFront(List<Ts...>, Type<T> = {}) noexcept {
    return {};
}

template <typename L, typename T>
using PushFront = decltype(pushFront(L{}, Type<T>{}));

template <typename... Lists>
constexpr auto flatten(List<Lists...>) noexcept {
    return concat(Lists{}...);
}

template <typename L>
using Flatten = decltype(flatten(L{}));

template <size_t Count, typename... Ts>
constexpr auto take(List<Ts...> list) {
    if constexpr (Count == 0) {
        return List<>{};
    } else {
        auto first = head(list);
        auto rest = take<Count - 1>(tail(list));
        return pushFront(rest, first);
    }
}

template <size_t Count, typename L>
using Take = decltype(take<Count>(L{}));

template <typename Predicate, typename... Ts>
constexpr auto filter(List<Ts...> list) {
    if constexpr (empty(list)) {
        return list;
    } else {
        auto first = head(list);
        auto rest = filter<Predicate>(tail(list));
        using T = typename decltype(first)::type;

        if constexpr (Predicate::template test<T>()) {
            return pushFront(rest, first);
        } else {
            return rest;
        }
    }
}

template <typename Predicate, typename L>
using Filter = decltype(filter<Predicate>(L{}));

template <typename Mapper, typename... Ts>
constexpr auto map(List<Ts...>) {
    return List<typename Mapper::template Map<Ts>...>{};
}

namespace detail {

template <typename... Ts>
constexpr List<List<Ts>...> listify(List<Ts...>) {
    return {};
}

template <typename... As, typename... Bs>
constexpr auto prodcat2(List<As...>, List<Bs...>) noexcept {
    return List<decltype(concat(As{}, Bs{}))...>{};
}

}  // namespace detail

template <typename... Ts>
constexpr auto prod(List<Ts...> list) noexcept {
    return detail::listify(list);
}

template <typename... Ts, typename... Lists>
constexpr auto prod(List<Ts...> head, Lists... tail) noexcept {
    auto first = detail::listify(head);
    auto rest = prod(tail...);
    return detail::prodcat2(first, rest);
}

template <typename... Ts>
constexpr auto unique(List<Ts...> list) noexcept {
    if constexpr (empty(list)) {
        return list;
    } else {
        auto first = head(list);
        auto rest = unique(tail(list));

        if constexpr (contains(rest, first)) {
            return rest;
        } else {
            return pushFront(rest, first);
        }
    }
}

template <typename L>
constexpr auto isASet(L list) noexcept {
    return size(list) == size(unique(list));
}

template <typename L>
concept Set = isASet(L{});

template <typename... Ts, typename... Lists>
constexpr auto intersect(List<Ts...> list, Lists... lists) noexcept {
    if constexpr (empty(list)) {
        return list;
    } else {
        auto first = head(list);
        auto rest = intersect(tail(list), lists...);

        if constexpr (sizeof...(lists) == 0 || (contains(lists, first) && ...)) {
            return pushFront(rest, first);
        } else {
            return rest;
        }
    }
}

template <typename... Lists>
constexpr auto unite(Lists... lists) noexcept {
    return unique(concat(lists...));
}

template <typename From, typename What>
constexpr auto subtract(From from, What what) noexcept {
    if constexpr (empty(from) || empty(what)) {
        return from;
    } else {
        auto first = head(from);
        auto rest = subtract(tail(from), what);

        if constexpr (contains(what, first)) {
            return rest;
        } else {
            return pushFront(rest, first);
        }
    }
}

template <typename... As, typename Super>
constexpr auto isSubset(List<As...>, Super sup) noexcept {
    return (contains(sup, Type<As>{}) && ...);
}

template <typename L, typename Superset>
concept SubsetOf = Set<L> && isSubset(L{}, Superset{});

template <typename L, typename Subset>
concept SupersetOf = Set<L> && isSubset(Subset{}, L{});

namespace detail {

template <typename T>
struct PushFrontMapper {
    template <typename L>
    using Map = decltype(pushFront(L{}, T{}));
};

}  // namespace detail

template <typename L>
constexpr auto powerset(L list) noexcept {
    if constexpr (empty(list)) {
        return List<List<>>{};
    } else {
        auto first = head(list);
        auto rest = powerset(tail(list));

        using T = decltype(first);
        using Mapper = detail::PushFrontMapper<T>;

        return concat(map<Mapper>(rest), rest);
    }
}

template <Set From, SupersetOf<From> To>
constexpr auto injection(From from, To to) noexcept {
    std::array<size_t, Size<From>> inj;
    forEachIndexed([&]<typename U>(size_t i, Type<U>) { inj[i] = find<U>(to); }, from);
    return inj;
}

}  // namespace tl
