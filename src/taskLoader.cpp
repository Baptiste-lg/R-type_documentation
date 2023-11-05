/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** taskLoader.cpp
*/

/**
** taskLoader.cpp
**
** This file implements the functionality to execute a list of loading tasks in a separate thread while updating a loading screen.
** The primary function defined is executeLoadingTasks which accepts a GUI object and a vector of tasks represented by std::function objects.
**
** Functions:
**
** - void executeLoadingTasks(GUI& gui, const std::vector<std::function<void(int&)>>& tasks):
**   This function orchestrates the execution of loading tasks and the updating of the loading screen.
**   It creates a detached thread to execute the tasks and computes the progress as a percentage.
**   The loading screen is updated with the progress percentage until all tasks are complete.
**
** Dependencies:
**
** - "taskLoader.hpp" for the declaration of executeLoadingTasks function.
** - <thread> and <chrono> for managing threads and sleep duration.
** - <vector> and <functional> for holding and representing the tasks.
**
** Usage:
**
** - Create a vector of tasks where each task is a std::function<void(int&)>.
** - Pass the GUI object and the vector of tasks to executeLoadingTasks.
** - The function will execute the tasks in a separate thread, updating the loading screen with the progress.
**
*/

#include "taskLoader.hpp"

void executeLoadingTasks(GUI& gui, const std::vector<std::function<void(int&)>>& tasks) {
    // Create a progress variable to hold the loading status
    int progress(0);
    // Create a detached thread to run the loading tasks
    std::thread loadingThread([&]() {
        int totalTasks = tasks.size();
        int currentTask = 0;
        for (auto& task : tasks) {
            progress = (currentTask * 100) / totalTasks;
            task(progress);  // Execute the task
            currentTask++;
        }
        progress = 100;  // Loading complete
    });
    loadingThread.detach();
    // Update the loading screen until all tasks are complete
    while (progress < 100) {
        gui.updateLoadingScreen(progress);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

