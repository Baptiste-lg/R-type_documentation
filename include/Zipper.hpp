/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** Zipper
*/

/**
** Zipper
**
** This class provides a zipper iterator to traverse multiple containers simultaneously.
**
** Methods:
**
** - zipper()
**   Constructor that stores begin and end iterators for each container.
**
** - begin()
**   Returns a zipper iterator pointing to the beginning.
**
** - end()
**   Returns a zipper iterator pointing to the end.
**
** Members:
**
** - _begin
**   Tuple of begin iterators for each container.
**
** - _end
**   Tuple of end iterators for each container.
**
** - _size
**   Size of the zipper sequence.
**
** The zipper_iterator is declared as friend to allow accessing private members.
*/

#ifndef ZIPPER_HPP_
#define ZIPPER_HPP_

#include <tuple>
#include <utility>
#include <iterator>
#include <type_traits>
#include "Entity.hpp"
#include "SparseArray.hpp"
#include "Component.hpp"
#include "ZipperIterator.hpp"

template <class... Containers>
class zipper {
public:
    using iterator = zipper_iterator<Containers...>;
    using iterator_tuple = typename iterator::iterator_tuple;
    friend class zipper_iterator<Containers...>;  // Declare as friend

    zipper(Containers&... cs)
            : _begin(std::make_tuple(std::begin(cs)...)),
              _end(std::make_tuple(std::end(cs)...)),
              _size(_compute_size(cs...)) {
    }

    iterator begin() {
        return iterator(_begin, _size, 0);  // Added an additional argument for _idx
    }

    iterator end() {
        return iterator(_end, _size, _size);  // Initialize with _idx = _size
    }

private:
    // Compute the size of the sequence. For simplicity, we are assuming all containers are of the same size.
    static size_t _compute_size(Containers&... containers) {
        return std::min({containers.size()...});
    }

private:
    iterator_tuple _begin;
    iterator_tuple _end;
    size_t _size;
};


#endif /* !ZIPPER_HPP_ */
