/*
** EPITECH PROJECT, 2024
** Rtype
** File description:
** SYSTEM
*/

/**
* FFTSystem.hpp
*
* System to handle audio FFT analysis.
* Inherits from ISystem.
* Requires FFT component.
*
* Methods:
*
* - operator()
*   Main update loop. Loads song, updates FFT, spawns enemies.
*
* - load()
*   Loads audio file.
*
* - update()
*   Updates FFT analysis.
*
* - spawnEnemiesBasedOnFFT()
*   Spawns enemies based on precalculated timestamps.
*
* - PostUpdate()
*   Sets needsUpdate flag.
*
* - map_amplitude_to_radius()
*   Maps FFT bin amplitude to radius for visual.
*
* - analyzeSong()
*   Analyzes full song to get average volume.
*
* - create_hamming_window()
*   Applies Hamming window to samples.
*
* Members:
*
* - deltaTime
*   Reference to game delta time.
*
* - registry
*   Reference to global ECS registry.
*
* - state
*   Loading state machine.
*
* - song
*   sf::Sound for playback.
*
* - fftClock
*   Measures playback time.
*
* - needsUpdate
*   Flag to update FFT.
*
* - precalculatedSpawns
*   Timestamped enemy spawn events.
*
*/

#ifndef FFTSYSTEM_HPP_
#define FFTSYSTEM_HPP_
#define BUFFER_SIZE 8192
#define NB_BARS 7
#define BIG_BUFFER
using namespace std;
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
#include "Spawner.hpp"
#include "Component.hpp"
#include <fftw3.h>
#include <complex>
#include <cmath>
#include <fstream>
#include "Component.hpp"
#include "System/EffectsFunctions.hpp"

enum class FFTState {
    IDLE,
    LOADED,
    LOADING,
    NOTLOADED
};

struct FFTActive{
    float frequencyAverages;
    float threshold;
    float lastSpawnTime;
    bool isOnCooldown;
    sf::RectangleShape bars;
    sf::RectangleShape thresholdBar;
};
struct SpawnEvent {
    float timestamp;  // Time in seconds
    float posY;       // Y position for spawn
    std::string behavior = "MoveLeft";
    std::string textureName = "e_ship1";
    entity_t entityId;
    int syncId = -1;
};



class FFTSystem : public ISystem<FFT> {
private:
    float &deltaTime;
    Registry& registry;  // Add this line
    FFTState state = FFTState::IDLE;
    sf::SoundBuffer buffer;
    sf::Sound song;
    float globalAverage = 0.0f;
    float globalPeak = 0.0f;
    bool averageCalculated = false;
    bool needsUpdate = true;
    sf::Clock fftClock;  // Clock to manage FFT timing

    float sample_rate = 0;
    sf::Uint64 sample_count = 0;
    sf::Uint64 buffer_size = 0;
    std::string path = "asset/Song/music.wav"; //TODO: change to path to song
    std::vector<Complex> samples;
    fftwf_complex *in =(fftwf_complex*) fftw_malloc(sizeof(fftwf_complex) * BUFFER_SIZE);
    fftwf_complex *out = (fftwf_complex*) fftw_malloc(sizeof(fftwf_complex) * BUFFER_SIZE);
    fftwf_plan p = fftwf_plan_dft_1d(BUFFER_SIZE, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    std::vector<std::complex<float>> bin = std::vector<std::complex<float>>(BUFFER_SIZE / 2);
    std::vector<float> window;
    sf::Time duration = sf::seconds(0);
    sf::Time offsetTime = sf::seconds(0);
    float spawnCooldown = 1.0f; // Cooldown time in seconds
    // debug visual
    std::vector<FFTActive> FFTActiveVector;
    std::vector<SpawnEvent> precalculatedSpawns;
    float thresholdBass = 0.0f; // 20% above average
    float thresholdSubBass = 0.0f; // 20% above average

    std::vector<std::pair<float, float>> frequencyRange = {
            {0, 60},
            {60, 250},
            {250, 500},
            {500, 2000},
            {2000, 4000},
            {4000, 6000},
            {6000, 20000}
    };
public:


    float map_amplitude_to_radius(float amplitude) {
        return amplitude * 1000;
    }
    std::string const getSystemName() override {
        return "FFTSystem";
    }
    FFTSystem(float &deltaTime, Registry& registry) : deltaTime(deltaTime), registry(registry) {
        FFTActiveVector.resize(NB_BARS);  // Resize the vector to hold NB_BARS elements

        registry.eventBus.addListener(FFTStart, [this](const Event &event) {
            restartForNewGame();
        });
        registry.eventBus.addListener(FFTPlay, [this](const Event &event) {
            state = FFTState::LOADED;
            fftClock.restart();
        });
        registry.eventBus.addListener(BackToMenu, [this, &registry](const Event &event) {
            // stop the song
            state = FFTState::IDLE;
            song.stop();
            //clear all the entities
            precalculatedSpawns.clear();
        });
    }

    void operator()(entity_t entity, sparse_array<FFT> &ffts) override {
        auto fftOpt = ffts[entity];
        if (!fftOpt){
            return;
        }
        if (!needsUpdate) {
            return;
        }
        if (state == FFTState::NOTLOADED) {
            state = FFTState::LOADING;
            load();  // Load the song
            if (!averageCalculated) {
                analyzeSong();
                precalculateSpawns();
            }
            averageCalculated = false;
        } else if (state == FFTState::LOADING) {

        } else if (state == FFTState::LOADED) {
            if (song.getStatus() != sf::SoundSource::Status::Playing) {
                song.play();
            }
            update();  // Update the FFT calculations
            spawnEnemiesBasedOnFFT();
            // You could also call update_by_sound() here depending on your requirements.
        }
        needsUpdate = false;  // Set this to false after doing the FFT calculation
    }

    void spawnEnemiesBasedOnFFT() {
        float currentTimestamp = fftClock.getElapsedTime().asSeconds();
        auto it = precalculatedSpawns.begin();
        while (it != precalculatedSpawns.end()) {
            auto& event = *it;
            if (event.timestamp <= currentTimestamp) {

                auto it1 = EffectFunctionMap.find("addScore");
                if (it1 != EffectFunctionMap.end()) {
                    auto addScore = it1->second;
                    auto it2 =deathApplyTypeMap.find("TO_HOST");
                    if (it2 != deathApplyTypeMap.end()) {
                        auto entityType = it2->second;
                        registry.add_component(event.entityId, DeathEffect{"addScore", addScore, entityType});
                    }
                }
                registry.activate_entity(event.entityId);  // This will now also add the components
                it = precalculatedSpawns.erase(it);  // Remove this event
            } else {
                break;  // Future events, no need to check further
            }
        }
    }



    void PostUpdate() override {
        needsUpdate = true;
    }

    void load() {
        state = FFTState::LOADING;
        std::ifstream ifs(path);
        if (!ifs.good())
        {
            return;
        }
        buffer.loadFromFile(path);
        song.setBuffer(buffer);

        sample_count = buffer.getSampleCount();
        sample_rate = buffer.getSampleRate() * buffer.getChannelCount();
#ifdef BIG_BUFFER
        if (BUFFER_SIZE < sample_count)
    {
        buffer_size = BUFFER_SIZE;
    }
    else
    {
        buffer_size = sample_count;
    }
#endif

#ifndef BIG_BUFFER
        buffer_size = 1920;//TODO: change THIS
#endif

        song.setLoop(true);
        samples.resize(buffer_size);
        create_hamming_window();
        duration = sf::seconds(static_cast<float>(sample_count) / sample_rate);
    }

    bool update() {
        //check if its the last pourcent of the song if so then fire event to notify the clientSystem
        if (song.getPlayingOffset() >= duration - sf::seconds(0.1f)) {
            Event NewEvent;
            NewEvent.type= EventType::EndOfSong;
            registry.eventBus.fireEvent(NewEvent);
        }
        return true;
    }



    void create_hamming_window()
    {
        for (int i = 0; i < buffer_size; i++) {
            window.push_back(static_cast<float>(0.54) - static_cast<float>(0.46) * cos((2 * PI * i) / static_cast<float>(buffer_size)));
        }
    }


    void analyzeSong() {
        // Initialize variables
        if(averageCalculated) return;  // Add this line
        float sum = 0.0f;
        float peak = 0.0f;
        float numBins = 0;



        float sumBass = 0.0f;
        float countBass = 0;
        float sumSubBass = 0.0f;
        float countSubBass = 0;

        // Initialize vectors to hold sum and count for each frequency group
        std::vector<float> sumPerGroup(frequencyRange.size(), 0.0f);
        std::vector<float> numBinsPerGroup(frequencyRange.size(), 0);
        float binSize = static_cast<float>(sample_rate) / buffer_size;

        // Set the step for FFT analysis
        int analyzeStep = 1;
        // Loop through the song samples
        for (sf::Uint64 offset = 0; offset + buffer_size < sample_count; offset += analyzeStep * buffer_size) {
            // Prepare FFT input for this chunk
            for (sf::Uint64 i = 0; i < buffer_size; i++) {
                in[i][0] = static_cast<float>(buffer.getSamples()[offset + i]) * window[i];
                in[i][1] = 0.0;
            }

            // Execute FFTW3 plan
            fftwf_execute(p);

            // Calculate group size
            float subBassIntensity = update_by_sound(0, 60, 10000);
            sumSubBass += subBassIntensity;
            countSubBass++;

            // Calculate Bass intensity (60-250 Hz)
            float bassIntensity = update_by_sound(60, 250, 10000);
            sumBass += bassIntensity;
            countBass++;
            // Analyze this chunk

            for (sf::Uint64 i = 0; i < frequencyRange.size(); ++i) {
                float from = frequencyRange[i].first;
                float to = frequencyRange[i].second;

                sf::Uint64 fromIndex = static_cast<sf::Uint64>(floor(from / binSize));
                sf::Uint64 toIndex = static_cast<sf::Uint64>(floor(to / binSize));

                for (sf::Uint64 j = fromIndex; j < toIndex; ++j) {
                    float magnitude = std::abs(std::complex<float>(out[j][0], out[j][1]));
                    sumPerGroup[i] += magnitude;
                    numBinsPerGroup[i]++;
                }
            }
        }
        // Calculate global average and peak
        for (sf::Uint64 i = 0; i < NB_BARS; ++i) {
            sum += sumPerGroup[i];
            numBins += numBinsPerGroup[i];
            if (sumPerGroup[i] / numBinsPerGroup[i] > peak) {
                peak = sumPerGroup[i] / numBinsPerGroup[i];
            }
        }

        globalAverage = sum / numBins;
        globalPeak = peak;
        averageCalculated = true;
        // Calculate frequency averages
        for (int i = 0; i < NB_BARS; ++i) {
            FFTActive &active = FFTActiveVector[i];
            active.frequencyAverages = sumPerGroup[i] / numBinsPerGroup[i];
            active.threshold = (active.frequencyAverages + (globalPeak - active.frequencyAverages) * 0.2f);
            active.thresholdBar.setPosition(1900 - active.threshold / 35, active.thresholdBar.getPosition().y);
        }
        float averageBass = sumBass / countBass;
        float averageSubBass = sumSubBass / countSubBass;

        // Use these averages to set the threshold for triggering spawns
        // For example:
        thresholdBass = averageBass * 1.2f; // 20% above average
        thresholdSubBass = averageSubBass * 1.2f; // 20% above average
        float dynamicThreshold = (globalAverage + (globalPeak - globalAverage) * 0.2f) / 10;
    }

    float update_by_sound(const float from, const float to, const float scale_factor)
    {
        float sum = 0.0f;
        float count = to - from;

        if (count <= 0)
            return 0.0f;

        for (int i = from; i < to; ++i) {
            sum += std::abs(std::complex<float>(out[i][0], out[i][1]));
        }
        return (sum / count) / scale_factor;
    }

    void precalculateSpawns() {
        if (!averageCalculated)
            analyzeSong();

        std::vector<float> lastSpawnTime(NB_BARS, 0.0f); // Initialize a vector to keep track of the last spawn time for each bar

        float currentTimestamp = 0.0f; // Initialize a variable to keep track of the current time in the song
        int analyzeStep = 1; // Analyze every frame
        float lastSpawnTimeSubBase = 0.0f; // Initialize a variable to keep track of the last spawn time for the base bar
        float lastSpawnTimeBase = 0.0f; // Initialize a variable to keep track of the last spawn time for the base bar
        float binSize = static_cast<float>(sample_rate) / buffer_size;

        // Clear any precalculated spawns
        precalculatedSpawns.clear();

        for (int offset = 0; offset + buffer_size < sample_count; offset += analyzeStep * buffer_size) {
            // Prepare FFT input for this chunk
            for (int i = 0; i < buffer_size; i++) {
                in[i][0] = static_cast<float>(buffer.getSamples()[offset + i]) * window[i];
                in[i][1] = 0.0;
            }

            // Execute FFTW3 plan
            fftwf_execute(p);

            // Calculate group size

            float subBassIntensity = 1.f + update_by_sound(0, 60, 10000);
            if (subBassIntensity > thresholdSubBass) {
                if (currentTimestamp - lastSpawnTimeSubBase >= spawnCooldown) {  // Check if enough time has passed for this bar
                    SpawnEvent event;
                    event.timestamp = currentTimestamp;
                    event.posY = 540;
                    event.behavior = "SineWave";
                    event.textureName = "e_ship3";
                    precalculatedSpawns.push_back(event);
                    lastSpawnTimeSubBase = currentTimestamp;  // Update the last spawn time for this bar
                }
            }
            float bassIntensity = 1.f + update_by_sound(60, 250, 10000);
            if (bassIntensity > thresholdBass ) {
                if (currentTimestamp - lastSpawnTimeBase >= spawnCooldown) {  // Check if enough time has passed for this bar
                    SpawnEvent event;
                    event.timestamp = currentTimestamp;
                    event.posY = 600;
                    event.behavior = "SineWave";
                    event.textureName = "e_ship2";
                    precalculatedSpawns.push_back(event);
                    lastSpawnTimeBase = currentTimestamp;  // Update the last spawn time for this bar
                }
            }


            // Analyze this chunk

// Then, in your loop:
            for (int i = 0; i < frequencyRange.size(); ++i) {
                float from = frequencyRange[i].first;
                float to = frequencyRange[i].second;

                sf::Uint64 fromIndex = static_cast<sf::Uint64>(floor(from / binSize));
                sf::Uint64 toIndex = static_cast<sf::Uint64>(floor(to / binSize));

                float sum = 0.0f;
                for (sf::Uint64 j = fromIndex; j < toIndex; ++j) {
                    float magnitude = std::abs(std::complex<float>(out[j][0], out[j][1]));
                    sum += magnitude;
                }

                // Calculate average magnitude for this frequency range
                float avg = sum / (toIndex - fromIndex);
                FFTActive &active = FFTActiveVector[i];

                // Use precalculated threshold to decide if an enemy should spawn
                if (avg >= active.threshold) {
                    if (currentTimestamp - lastSpawnTime[i] >= spawnCooldown) {  // Check if enough time has passed for this bar
                        SpawnEvent event;
                        event.timestamp = currentTimestamp;
                        event.posY = 100 + i * 100;
                        event.syncId = precalculatedSpawns.size(  );
                        precalculatedSpawns.push_back(event);
                        lastSpawnTime[i] = currentTimestamp;  // Update the last spawn time for this bar
                    }
                }

            }

            currentTimestamp += (float)buffer_size / sample_rate; // Update the current time based on the buffer size and sample rate
        }

        // Sort precalculatedSpawns by timestamp
        std::sort(precalculatedSpawns.begin(), precalculatedSpawns.end(),
                  [](const SpawnEvent& a, const SpawnEvent& b) {
                      return a.timestamp < b.timestamp;
                  });

        size_t totalSpawns = precalculatedSpawns.size();
        int i = 0;
        for (auto& event : precalculatedSpawns)
            i++;
        //now prepare pool of enemies

        for (auto& event : precalculatedSpawns) {
            event.entityId = FFTSpawnEnemy(registry, event.behavior, 1750, event.posY, event.textureName, {0, 0, 128, 128}, {1.0f, 1.0f}, event.syncId);
        }
        //fire FFTEnd
        Event NewEvent;
        NewEvent.type= EventType::FFTEnd;
        registry.eventBus.fireEvent(NewEvent);

    }
    void restartForNewGame() {
        // Reset any state necessary to allow a new game to start fresh
        state = FFTState::NOTLOADED;
        averageCalculated = false;
        needsUpdate = true;
        fftClock.restart();
        precalculatedSpawns.clear();
        song.stop();
        path = registry.getPath() + "/Song.wav";
        // ... any additional cleanup
    }


    ~FFTSystem() override {

        fftwf_destroy_plan(p);
        fftw_free(in);
        fftw_free(out);
    }
};


#endif /* !FFTSYSTEM_HPP_ */