// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "animation.h"
#include <algorithm>
#include <cmath>
#include <iostream>

// Animation class implementation

Animation::Animation(float duration, std::function<void(float)> update_callback, 
                     std::function<void()> complete_callback)
    : m_duration(duration)
    , m_update_callback(update_callback)
    , m_complete_callback(complete_callback)
{
}

bool Animation::Update(float delta_time)
{
    if (!m_running || m_paused || m_complete) {
        return !m_complete;
    }

    m_current_time += delta_time;
    m_progress = std::min(m_current_time / m_duration, 1.0f);

    // Apply easing
    float eased_progress = ApplyEasing(m_progress);

    // Call update callback
    if (m_update_callback) {
        m_update_callback(eased_progress);
    }

    // Check if animation is complete
    if (m_progress >= 1.0f) {
        m_current_loop++;
        
        if (m_loop_count < 0 || m_current_loop < m_loop_count) {
            // Continue looping
            m_current_time = 0.0f;
            m_progress = 0.0f;
        } else {
            // Animation complete
            m_complete = true;
            m_running = false;
            
            if (m_complete_callback) {
                m_complete_callback();
            }
        }
    }

    return !m_complete;
}

void Animation::Start()
{
    m_running = true;
    m_paused = false;
    m_complete = false;
}

void Animation::Pause()
{
    m_paused = true;
}

void Animation::Resume()
{
    m_paused = false;
}

void Animation::Stop()
{
    m_running = false;
    m_paused = false;
    m_complete = true;
}

void Animation::Reset()
{
    m_current_time = 0.0f;
    m_progress = 0.0f;
    m_current_loop = 0;
    m_complete = false;
    m_paused = false;
}

float Animation::ApplyEasing(float t) const
{
    switch (m_easing) {
        case EasingType::LINEAR:
            return t;
            
        case EasingType::EASE_IN:
            return t * t;
            
        case EasingType::EASE_OUT:
            return 1.0f - (1.0f - t) * (1.0f - t);
            
        case EasingType::EASE_IN_OUT:
            return t < 0.5f ? 2.0f * t * t : 1.0f - 2.0f * (1.0f - t) * (1.0f - t);
            
        case EasingType::BOUNCE:
            if (t < 1.0f / 2.75f) {
                return 7.5625f * t * t;
            } else if (t < 2.0f / 2.75f) {
                t -= 1.5f / 2.75f;
                return 7.5625f * t * t + 0.75f;
            } else if (t < 2.5f / 2.75f) {
                t -= 2.25f / 2.75f;
                return 7.5625f * t * t + 0.9375f;
            } else {
                t -= 2.625f / 2.75f;
                return 7.5625f * t * t + 0.984375f;
            }
            
        case EasingType::ELASTIC:
            if (t == 0.0f) return 0.0f;
            if (t == 1.0f) return 1.0f;
            return -std::pow(2.0f, 10.0f * (t - 1.0f)) * std::sin((t - 1.1f) * 5.0f * M_PI);
            
        default:
            return t;
    }
}

// AnimationManager class implementation

void AnimationManager::Update(float delta_time)
{
    // Update all animations and remove completed ones
    auto it = m_animations.begin();
    while (it != m_animations.end()) {
        if (!it->second->Update(delta_time)) {
            it = m_animations.erase(it);
        } else {
            ++it;
        }
    }
}

void AnimationManager::AddAnimation(const std::string& name, std::unique_ptr<Animation> animation)
{
    // Remove existing animation with same name
    RemoveAnimation(name);
    
    // Add new animation
    m_animations.emplace_back(name, std::move(animation));
}

void AnimationManager::RemoveAnimation(const std::string& name)
{
    auto it = std::find_if(m_animations.begin(), m_animations.end(),
                          [&name](const auto& pair) { return pair.first == name; });
    
    if (it != m_animations.end()) {
        m_animations.erase(it);
    }
}

Animation* AnimationManager::GetAnimation(const std::string& name)
{
    auto it = std::find_if(m_animations.begin(), m_animations.end(),
                          [&name](const auto& pair) { return pair.first == name; });
    
    return (it != m_animations.end()) ? it->second.get() : nullptr;
}

void AnimationManager::StartAnimation(const std::string& name)
{
    Animation* anim = GetAnimation(name);
    if (anim) {
        anim->Start();
    }
}

void AnimationManager::PauseAnimation(const std::string& name)
{
    Animation* anim = GetAnimation(name);
    if (anim) {
        anim->Pause();
    }
}

void AnimationManager::StopAnimation(const std::string& name)
{
    Animation* anim = GetAnimation(name);
    if (anim) {
        anim->Stop();
    }
}

void AnimationManager::ClearAnimations()
{
    m_animations.clear();
}

std::unique_ptr<Animation> AnimationManager::CreateFadeAnimation(float duration, float from_alpha, float to_alpha,
                                                                 std::function<void(float)> update_callback)
{
    auto animation = std::make_unique<Animation>(duration, 
        [from_alpha, to_alpha, update_callback](float progress) {
            float alpha = from_alpha + (to_alpha - from_alpha) * progress;
            update_callback(alpha);
        });
    
    return animation;
}

std::unique_ptr<Animation> AnimationManager::CreateSlideAnimation(float duration, float from_x, float from_y,
                                                                  float to_x, float to_y,
                                                                  std::function<void(float, float)> update_callback)
{
    auto animation = std::make_unique<Animation>(duration,
        [from_x, from_y, to_x, to_y, update_callback](float progress) {
            float x = from_x + (to_x - from_x) * progress;
            float y = from_y + (to_y - from_y) * progress;
            update_callback(x, y);
        });
    
    return animation;
}

std::unique_ptr<Animation> AnimationManager::CreateScaleAnimation(float duration, float from_scale, float to_scale,
                                                                  std::function<void(float)> update_callback)
{
    auto animation = std::make_unique<Animation>(duration,
        [from_scale, to_scale, update_callback](float progress) {
            float scale = from_scale + (to_scale - from_scale) * progress;
            update_callback(scale);
        });
    
    return animation;
}