/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** taskLoader
*/

/**
** taskLoader
**
** This file contains functions related to loading tasks.
**
** Functions:
**
** - executeLoadingTasks()
**   Executes a vector of loading task functions.
**
**   Parameters:
**   - gui: Reference to the GUI
**   - tasks: Vector of loading task functions
**
**   The loading task functions take an int& progress
**   parameter and perform some incremental loading work.
**
*/

#ifndef TASKLOADER_HPP_
#define TASKLOADER_HPP_
#include "Component.hpp"
#include "SparseArray.hpp"
#include "GUI/GUI.hpp"


void executeLoadingTasks(GUI& gui, const std::vector<std::function<void(int&)>>& tasks);

#endif /* !TASKLOADER_HPP_*/