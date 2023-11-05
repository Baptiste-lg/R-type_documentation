/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** ZipperIterator
*/

/**
** ZipperIterator
**
** This class implements an iterator for the Zipper class to
** traverse multiple containers simultaneously.
**
** Methods:
**
** - ZipperIterator()
**   Constructs the iterator with begin tuple, max size, and index.
**
** - operator++()
**   Prefix increment. Increments index and all underlying iterators.
**
** - operator++(int)
**   Postfix increment.
**
** - operator*()
**   Dereference. Returns tuple of references to current elements.
**
** - operator->()
**   Arrow operator.
**
** - operator==()
**   Equality comparison. Compares indices.
**
** - operator!=()
**   Inequality comparison.
**
** Members:
**
** - _current
**   Tuple of current iterators.
**
** - _max
**   Max size of the zipper.
**
** - _idx
**   Current index.
**
** - _seq
**   Integer sequence for template parameters.
**
*/

#ifndef ZIPPERITTERATOR_HPP_
#define ZIPPERITTERATOR_HPP_

#include <tuple>
#include <utility>
#include <iterator>
#include <type_traits>
#include "Entity.hpp"
#include "SparseArray.hpp"
#include "Component.hpp"
#include "Zipper.hpp"

template <class... Containers>
class zipper;  // Forward declaration

// Define a zipper_iterator for Containers
template <class... Containers>
class zipper_iterator {
    template <class Container>
    using iterator_t = typename Container::iterator;

    template <class Container>
    using it_reference_t = typename iterator_t<Container>::reference;

    // Define iterator properties
    using value_type = std::tuple<it_reference_t<Containers>...>;
    using reference = value_type;
    using pointer = void;
    using difference_type = size_t;
    using iterator_category = std::forward_iterator_tag;

    // Use a tuple to hold iterators for each Container
    using iterator_tuple = std::tuple<iterator_t<Containers>...>;

public:
    // Copy constructor
    zipper_iterator(const zipper_iterator& z)
            : _current(z._current), _max(z._max), _idx(z._idx) {}

    // Allow zipper class to access private members
    friend class zipper<Containers...>;

    // Construct a zipper_iterator
    zipper_iterator(iterator_tuple const& it_tuple, size_t max, size_t idx = 0)
            : _current(it_tuple), _max(max), _idx(idx) {}

    // Prefix increment
    zipper_iterator& operator++() {
        ++_idx;
        incr_all(_seq);
        return *this;
    }

    // Postfix increment
    zipper_iterator operator++(int) {
        zipper_iterator tmp(*this);
        operator++();
        return tmp;
    }

    // Dereference iterator
    reference operator*() {
        return to_value(_seq);
    }

    // Arrow operator overload
    pointer operator->() {
        return &to_value(_seq);
    }

    // Equality operator
    friend bool operator==(const zipper_iterator& lhs, const zipper_iterator& rhs) {
        return lhs._idx == rhs._idx;
    }

    // Inequality operator
    friend bool operator!=(const zipper_iterator& lhs, const zipper_iterator& rhs) {
        return lhs._idx != rhs._idx;
    }

    // Increment all underlying iterators
    template <std::size_t... Is>
    void incr_all(std::index_sequence<Is...>) {
        ((++std::get<Is>(_current)), ...);
    }

    // Check if all elements are set
    template <std::size_t... Is>
    bool all_set(std::index_sequence<Is...>) {
        return (... && static_cast<bool>(std::get<Is>(_current)));
    }

    // Convert to value_type (tuple)
    template <std::size_t... Is>
    auto to_value(std::index_sequence<Is...>) {
        return std::tie(*std::get<Is>(_current)...);
    }

private:
    size_t _max;
    size_t _idx;
    iterator_tuple _current;
    static constexpr std::index_sequence_for<Containers...> _seq{};
};

#endif /* !ZIPPERITTERATOR_HPP_ */
