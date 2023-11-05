/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** SparseArray
*/
/**
 * sparse_array is a templated container for storing sparse component data.
 *
 * It allows efficient insertion, removal and random access of components.
 * Components are stored in a std::optional wrapper.
 *
 * Template Parameters:
 * - Component: The component type stored in the array.
 *
 * Usage:
 * - sparse_array<Position> positions;
 * - positions.insert_at(entity, Position{x, y});
 * - auto& pos = positions[entity];
 * - pos->x = 10;
 *
 * Methods:
 * - insert_at: Inserts a component at an index.
 * - erase: Removes the component at an index.
 * - operator[]: Accesses the component optional at an index.
 * - contains: Checks if an index has a component.
*/

#ifndef SPARSEARRAY_HPP_
#define SPARSEARRAY_HPP_
#include <vector>
#include <optional>
#include <mutex>

template <typename Component>
class sparse_array {
public:
    using value_type = std::optional<Component>;
    using reference_type = value_type&;
    using const_reference_type = const value_type&;
    using container_t = std::vector<value_type>;
    using size_type = typename container_t::size_type;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

public:
    sparse_array() = default;
    sparse_array(const sparse_array&) = default;
    sparse_array(sparse_array&&) noexcept = default;
    ~sparse_array() = default;

    sparse_array& operator=(const sparse_array&) = default;
    sparse_array& operator=(sparse_array&&) noexcept = default;
    reference_type operator[](size_t idx) {
        if (idx >= _data.size()) {
            _data.resize(std::max(_data.size() * 2, idx + 1)); // Geometric growth
        }
        return _data[idx];
    }
    const_reference_type operator[](size_t idx) const {
        return _data[idx];
    }

    iterator begin() {
        return _data.begin();
    }

    const_iterator begin() const {
        return _data.begin();
    }

    const_iterator cbegin() const {
        return _data.cbegin();
    }

    iterator end() {
        return _data.end();
    }

    const_iterator end() const {
        return _data.end();
    }

    const_iterator cend() const {
        return _data.cend();
    }

    size_type size() const {
        return _data.size();
    }
    size_type actual_size() const {
        return count_if(_data.begin(), _data.end(), [](const auto& opt) {
            return opt.has_value();
        });
    }

    reference_type insert_at(size_type pos, const Component& comp) {
        if (pos >= _data.size()) {
            _data.resize(std::max(_data.size() * 2, pos + 1)); // Geometric growth
        }
        _data[pos] = comp;
        ensure_flag_size(pos);
        _active_flags[pos] = true;
        return _data[pos];
    }

    reference_type insert_at(size_type pos, Component&& comp) {
        if (pos >= _data.size()) {
            _data.resize(std::max(_data.size() * 2, pos + 1)); // Geometric growth
        }
        _data[pos] = std::move(comp);
        ensure_flag_size(pos);
        _active_flags[pos] = true;
        return _data[pos];
    }
    template <class... Params>

    reference_type emplace_at(size_type pos, Params&&... params) {
        if (pos >= _data.size()) {
            _data.resize(std::max(_data.size() * 2, pos + 1)); // Geometric growth
        }
        _data[pos] = std::make_optional<Component>(std::forward<Params>(params)...);
        return _data[pos];
    }
    void erase(size_type pos) {
        if (pos >= _data.size()) {
            // Out of bounds
            return;
        }
        if (pos < _active_flags.size()) {
            _active_flags[pos] = false;
        }
        // Erase the component at the given index
        _data[pos] = std::nullopt;
    }
    bool contains(size_type pos) const {
        if (pos >= _data.size()) {
            return false;
        }
        return _data[pos].has_value();
    }

    void clear() {
        _data.clear();
    }
    bool empty() const {
        for (const auto& elem : _data) {
            if (elem.has_value()) {
                return false;
            }
        }
        return true;
    }
    void activate(size_type pos) {
        if (pos < _active_flags.size()) {
            _active_flags[pos] = true;
        }
    }

    void deactivate(size_type pos) {
        if (pos < _active_flags.size()) {
            _active_flags[pos] = false;
        }
    }

    bool is_active(size_type pos) const {
        return pos < _active_flags.size() && _active_flags[pos];
    }
    void ensure_flag_size(size_type pos) {
        if (pos >= _active_flags.size()) {
            _active_flags.resize(std::max(_active_flags.size() * 2, pos + 1), false); // Geometric growth
        }
    }
private:
    container_t _data;
    std::vector<bool> _active_flags;  // New member to store active state

};

#endif //SPARSEARRAY_HPP_