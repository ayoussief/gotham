// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_UTILS_ANIMATION_H
#define GOTHAM_SDL2_UTILS_ANIMATION_H

#include <functional>
#include <vector>
#include <memory>
#include <string>

enum class EasingType {
    LINEAR,
    EASE_IN,
    EASE_OUT,
    EASE_IN_OUT,
    BOUNCE,
    ELASTIC
};

/**
 * Represents a single animation
 */
class Animation
{
public:
    Animation(float duration, std::function<void(float)> update_callback, 
              std::function<void()> complete_callback = nullptr);
    ~Animation() = default;

    /**
     * Update the animation
     * @param delta_time Time elapsed since last update
     * @return true if animation is still running
     */
    bool Update(float delta_time);

    /**
     * Start the animation
     */
    void Start();

    /**
     * Pause the animation
     */
    void Pause();

    /**
     * Resume the animation
     */
    void Resume();

    /**
     * Stop the animation
     */
    void Stop();

    /**
     * Reset the animation to the beginning
     */
    void Reset();

    /**
     * Check if animation is running
     */
    bool IsRunning() const { return m_running && !m_paused; }

    /**
     * Check if animation is paused
     */
    bool IsPaused() const { return m_paused; }

    /**
     * Check if animation is complete
     */
    bool IsComplete() const { return m_complete; }

    /**
     * Set easing type
     */
    void SetEasing(EasingType easing) { m_easing = easing; }

    /**
     * Set loop count (-1 for infinite)
     */
    void SetLoopCount(int loops) { m_loop_count = loops; }

    /**
     * Get current progress (0.0 to 1.0)
     */
    float GetProgress() const { return m_progress; }

private:
    float m_duration;
    float m_current_time{0.0f};
    float m_progress{0.0f};
    bool m_running{false};
    bool m_paused{false};
    bool m_complete{false};
    int m_loop_count{1};
    int m_current_loop{0};
    EasingType m_easing{EasingType::LINEAR};
    
    std::function<void(float)> m_update_callback;
    std::function<void()> m_complete_callback;

    float ApplyEasing(float t) const;
};

/**
 * Manages multiple animations
 */
class AnimationManager
{
public:
    AnimationManager() = default;
    ~AnimationManager() = default;

    /**
     * Update all animations
     * @param delta_time Time elapsed since last update
     */
    void Update(float delta_time);

    /**
     * Add an animation
     * @param name Animation name for reference
     * @param animation Animation to add
     */
    void AddAnimation(const std::string& name, std::unique_ptr<Animation> animation);

    /**
     * Remove an animation
     * @param name Animation name
     */
    void RemoveAnimation(const std::string& name);

    /**
     * Get an animation by name
     * @param name Animation name
     * @return Animation pointer or nullptr if not found
     */
    Animation* GetAnimation(const std::string& name);

    /**
     * Start an animation
     * @param name Animation name
     */
    void StartAnimation(const std::string& name);

    /**
     * Pause an animation
     * @param name Animation name
     */
    void PauseAnimation(const std::string& name);

    /**
     * Stop an animation
     * @param name Animation name
     */
    void StopAnimation(const std::string& name);

    /**
     * Clear all animations
     */
    void ClearAnimations();

    /**
     * Create a fade animation
     * @param duration Animation duration in seconds
     * @param from_alpha Starting alpha value (0.0 to 1.0)
     * @param to_alpha Ending alpha value (0.0 to 1.0)
     * @param update_callback Callback to update the alpha value
     * @return Animation pointer
     */
    std::unique_ptr<Animation> CreateFadeAnimation(float duration, float from_alpha, float to_alpha,
                                                   std::function<void(float)> update_callback);

    /**
     * Create a slide animation
     * @param duration Animation duration in seconds
     * @param from_x Starting X position
     * @param from_y Starting Y position
     * @param to_x Ending X position
     * @param to_y Ending Y position
     * @param update_callback Callback to update the position
     * @return Animation pointer
     */
    std::unique_ptr<Animation> CreateSlideAnimation(float duration, float from_x, float from_y,
                                                    float to_x, float to_y,
                                                    std::function<void(float, float)> update_callback);

    /**
     * Create a scale animation
     * @param duration Animation duration in seconds
     * @param from_scale Starting scale value
     * @param to_scale Ending scale value
     * @param update_callback Callback to update the scale
     * @return Animation pointer
     */
    std::unique_ptr<Animation> CreateScaleAnimation(float duration, float from_scale, float to_scale,
                                                    std::function<void(float)> update_callback);

private:
    std::vector<std::pair<std::string, std::unique_ptr<Animation>>> m_animations;
};

#endif // GOTHAM_SDL2_UTILS_ANIMATION_H// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_UTILS_ANIMATION_H
#define GOTHAM_SDL2_UTILS_ANIMATION_H

#include <functional>
#include <vector>
#include <memory>
#include <string>

enum class EasingType {
    LINEAR,
    EASE_IN,
    EASE_OUT,
    EASE_IN_OUT,
    BOUNCE,
    ELASTIC
};

/**
 * Represents a single animation
 */
class Animation
{
public:
    Animation(float duration, std::function<void(float)> update_callback, 
              std::function<void()> complete_callback = nullptr);
    ~Animation() = default;

    /**
     * Update the animation
     * @param delta_time Time elapsed since last update
     * @return true if animation is still running
     */
    bool Update(float delta_time);

    /**
     * Start the animation
     */
    void Start();

    /**
     * Pause the animation
     */
    void Pause();

    /**
     * Resume the animation
     */
    void Resume();

    /**
     * Stop the animation
     */
    void Stop();

    /**
     * Reset the animation to the beginning
     */
    void Reset();

    /**
     * Check if animation is running
     */
    bool IsRunning() const { return m_running && !m_paused; }

    /**
     * Check if animation is paused
     */
    bool IsPaused() const { return m_paused; }

    /**
     * Check if animation is complete
     */
    bool IsComplete() const { return m_complete; }

    /**
     * Set easing type
     */
    void SetEasing(EasingType easing) { m_easing = easing; }

    /**
     * Set loop count (-1 for infinite)
     */
    void SetLoopCount(int loops) { m_loop_count = loops; }

    /**
     * Get current progress (0.0 to 1.0)
     */
    float GetProgress() const { return m_progress; }

private:
    float m_duration;
    float m_current_time{0.0f};
    float m_progress{0.0f};
    bool m_running{false};
    bool m_paused{false};
    bool m_complete{false};
    int m_loop_count{1};
    int m_current_loop{0};
    EasingType m_easing{EasingType::LINEAR};
    
    std::function<void(float)> m_update_callback;
    std::function<void()> m_complete_callback;

    float ApplyEasing(float t) const;
};

/**
 * Manages multiple animations
 */
class AnimationManager
{
public:
    AnimationManager() = default;
    ~AnimationManager() = default;

    /**
     * Update all animations
     * @param delta_time Time elapsed since last update
     */
    void Update(float delta_time);

    /**
     * Add an animation
     * @param name Animation name for reference
     * @param animation Animation to add
     */
    void AddAnimation(const std::string& name, std::unique_ptr<Animation> animation);

    /**
     * Remove an animation
     * @param name Animation name
     */
    void RemoveAnimation(const std::string& name);

    /**
     * Get an animation by name
     * @param name Animation name
     * @return Animation pointer or nullptr if not found
     */
    Animation* GetAnimation(const std::string& name);

    /**
     * Start an animation
     * @param name Animation name
     */
    void StartAnimation(const std::string& name);

    /**
     * Pause an animation
     * @param name Animation name
     */
    void PauseAnimation(const std::string& name);

    /**
     * Stop an animation
     * @param name Animation name
     */
    void StopAnimation(const std::string& name);

    /**
     * Clear all animations
     */
    void ClearAnimations();

    /**
     * Create a fade animation
     * @param duration Animation duration in seconds
     * @param from_alpha Starting alpha value (0.0 to 1.0)
     * @param to_alpha Ending alpha value (0.0 to 1.0)
     * @param update_callback Callback to update the alpha value
     * @return Animation pointer
     */
    std::unique_ptr<Animation> CreateFadeAnimation(float duration, float from_alpha, float to_alpha,
                                                   std::function<void(float)> update_callback);

    /**
     * Create a slide animation
     * @param duration Animation duration in seconds
     * @param from_x Starting X position
     * @param from_y Starting Y position
     * @param to_x Ending X position
     * @param to_y Ending Y position
     * @param update_callback Callback to update the position
     * @return Animation pointer
     */
    std::unique_ptr<Animation> CreateSlideAnimation(float duration, float from_x, float from_y,
                                                    float to_x, float to_y,
                                                    std::function<void(float, float)> update_callback);

    /**
     * Create a scale animation
     * @param duration Animation duration in seconds
     * @param from_scale Starting scale value
     * @param to_scale Ending scale value
     * @param update_callback Callback to update the scale
     * @return Animation pointer
     */
    std::unique_ptr<Animation> CreateScaleAnimation(float duration, float from_scale, float to_scale,
                                                    std::function<void(float)> update_callback);

private:
    std::vector<std::pair<std::string, std::unique_ptr<Animation>>> m_animations;
};

#endif // GOTHAM_SDL2_UTILS_ANIMATION_H