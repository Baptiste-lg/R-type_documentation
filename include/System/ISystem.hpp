/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** SYSTEM
*/

/**
 * ISystem is an interface that systems inherit from.
 * It defines the operator() method that systems must implement to process entities.
 *
 * Template parameters:
 * - Components: The component types that this system needs to operate on.
 *
 * Parameters:
 * - entity: The entity being processed.
 * - components: Sparse arrays containing components of the types specified in the template.
 *
 * Usage:
 * - Systems implement operator() to access components and perform processing.
 * - The ECS framework calls operator() and passes the entity and its components.
 * - The system can then read/write the components to do its job.
*/

#ifndef ISYSTEM_HPP_
#define ISYSTEM_HPP_
#include <SFML/Graphics.hpp>
#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include <memory>
#include <utility>
#include <functional>
#include "Entity.hpp"
#include "SparseArray.hpp"
#include "Component.hpp"
#include <fftw3.h>




class ISystemBase {
public:
    virtual void PostUpdate() {}
    virtual std::string const getSystemName() { return "ISystemBase"; }
    virtual ~ISystemBase() = default;
};

template<typename... Components>
class ISystem : public ISystemBase {
public:
    virtual void operator()(entity_t entity, sparse_array<Components>&... components) = 0;
    virtual ~ISystem() = default;
};
#endif /* !ISYSTEM_HPP_ */