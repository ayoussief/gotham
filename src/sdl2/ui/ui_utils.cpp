// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "ui_utils.h"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace UIUtils {

void ResponsiveButtonLayout::Apply(LayoutManager& layout_manager, const Rect& container, 
                                 std::vector<LayoutItem>& button_items, int screen_width)
{
    UIStyleGuide::Responsive::ScreenSize screen_size = UIStyleGuide::Responsive::GetScreenSize(screen_width);
    
    // Ensure minimum touch target size for all buttons
    for (auto& item : button_items) {
        if (item.constraints.preferred_height < UIStyleGuide::Accessibility::MIN_TOUCH_TARGET) {
            item.constraints.preferred_height = UIStyleGuide::Accessibility::MIN_TOUCH_TARGET;
        }
    }
    
    switch (screen_size) {
        case UIStyleGuide::Responsive::ScreenSize::MOBILE:
            // Stack vertically on mobile for better touch interaction
            layout_manager.CreateVerticalLayout(container, button_items, 
                                              UIStyleGuide::Spacing::SM, Alignment::STRETCH);
            break;
            
        case UIStyleGuide::Responsive::ScreenSize::TABLET:
            // Use horizontal layout but with more spacing
            layout_manager.CreateHorizontalLayout(container, button_items, 
                                                 UIStyleGuide::Spacing::MD, Alignment::CENTER);
            break;
            
        case UIStyleGuide::Responsive::ScreenSize::DESKTOP:
            // Standard horizontal layout
            layout_manager.CreateHorizontalLayout(container, button_items, 
                                                 UIStyleGuide::Spacing::SM, Alignment::CENTER);
            break;
    }
}

std::vector<LayoutItem> FormLayoutHelper::CreateFormFields(
    const std::vector<std::string>& field_names,
    const std::vector<std::function<void(const Rect&)>>& callbacks,
    int container_width)
{
    std::vector<LayoutItem> form_items;
    
    if (field_names.size() != callbacks.size()) {
        ErrorHandler::LogUIError("Mismatch between field names and callbacks", "FormLayoutHelper");
        return form_items;
    }
    
    for (size_t i = 0; i < field_names.size(); ++i) {
        // Label item
        LayoutItem label_item;
        label_item.constraints.preferred_width = UIStyleGuide::Layout::FormLayout::LABEL_WIDTH;
        label_item.constraints.preferred_height = UIStyleGuide::Layout::FormLayout::FIELD_HEIGHT;
        label_item.margin = Margin(UIStyleGuide::Spacing::SM, 0, UIStyleGuide::Spacing::SM, UIStyleGuide::Spacing::MD);
        form_items.push_back(label_item);
        
        // Input item
        LayoutItem input_item;
        input_item.constraints.preferred_width = container_width - UIStyleGuide::Layout::FormLayout::LABEL_WIDTH - UIStyleGuide::Spacing::MD;
        input_item.constraints.preferred_height = UIStyleGuide::Dimensions::INPUT_HEIGHT;
        input_item.margin = Margin(UIStyleGuide::Spacing::SM, UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::SM, 0);
        input_item.on_bounds_changed = callbacks[i];
        form_items.push_back(input_item);
    }
    
    return form_items;
}

bool AccessibilityValidator::ValidateMinimumTouchTargets(const std::vector<Rect>& button_bounds)
{
    bool all_valid = true;
    
    for (size_t i = 0; i < button_bounds.size(); ++i) {
        const Rect& bounds = button_bounds[i];
        if (bounds.w < UIStyleGuide::Accessibility::MIN_TOUCH_TARGET || 
            bounds.h < UIStyleGuide::Accessibility::MIN_TOUCH_TARGET) {
            std::cerr << "WARNING: Button " << i << " size (" << bounds.w << "x" << bounds.h 
                      << ") is below minimum touch target (" << UIStyleGuide::Accessibility::MIN_TOUCH_TARGET 
                      << "x" << UIStyleGuide::Accessibility::MIN_TOUCH_TARGET << ")" << std::endl;
            all_valid = false;
        }
    }
    
    return all_valid;
}

bool AccessibilityValidator::ValidateColorContrast(const Color& foreground, const Color& background)
{
    // Simplified contrast ratio calculation
    // In a real implementation, you'd use proper luminance calculation
    float fg_luminance = (0.299f * foreground.r + 0.587f * foreground.g + 0.114f * foreground.b) / 255.0f;
    float bg_luminance = (0.299f * background.r + 0.587f * background.g + 0.114f * background.b) / 255.0f;
    
    float contrast_ratio = (std::max(fg_luminance, bg_luminance) + 0.05f) / 
                          (std::min(fg_luminance, bg_luminance) + 0.05f);
    
    return contrast_ratio >= UIStyleGuide::Accessibility::MIN_CONTRAST_RATIO;
}

void AccessibilityValidator::LogAccessibilityWarnings(const std::string& component_name, const Rect& bounds)
{
    if (bounds.w < UIStyleGuide::Accessibility::MIN_TOUCH_TARGET || 
        bounds.h < UIStyleGuide::Accessibility::MIN_TOUCH_TARGET) {
        std::cerr << "ACCESSIBILITY WARNING: " << component_name << " size (" << bounds.w << "x" << bounds.h 
                  << ") is below minimum touch target" << std::endl;
    }
}

Point AlignmentHelper::CenterInContainer(const Rect& container, const Rect& element)
{
    return Point(
        container.x + (container.w - element.w) / 2,
        container.y + (container.h - element.h) / 2
    );
}

Point AlignmentHelper::AlignToGrid(const Point& position, int grid_size)
{
    return Point(
        (position.x / grid_size) * grid_size,
        (position.y / grid_size) * grid_size
    );
}

Rect AlignmentHelper::ConstrainToContainer(const Rect& element, const Rect& container, const Margin& margin)
{
    Rect constrained = element;
    
    // Adjust position to stay within container bounds
    constrained.x = std::max(container.x + margin.left, 
                           std::min(constrained.x, container.x + container.w - constrained.w - margin.right));
    constrained.y = std::max(container.y + margin.top, 
                           std::min(constrained.y, container.y + container.h - constrained.h - margin.bottom));
    
    // Adjust size if necessary
    int max_width = container.w - margin.left - margin.right;
    int max_height = container.h - margin.top - margin.bottom;
    
    constrained.w = std::min(constrained.w, max_width);
    constrained.h = std::min(constrained.h, max_height);
    
    return constrained;
}

Color AnimationHelper::InterpolateColor(const Color& start, const Color& end, float progress)
{
    progress = std::clamp(progress, 0.0f, 1.0f);
    
    return Color(
        static_cast<Uint8>(start.r + (end.r - start.r) * progress),
        static_cast<Uint8>(start.g + (end.g - start.g) * progress),
        static_cast<Uint8>(start.b + (end.b - start.b) * progress),
        static_cast<Uint8>(start.a + (end.a - start.a) * progress)
    );
}

int AnimationHelper::InterpolateInt(int start, int end, float progress)
{
    progress = std::clamp(progress, 0.0f, 1.0f);
    return static_cast<int>(start + (end - start) * progress);
}

float AnimationHelper::EaseInOut(float t)
{
    return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
}

void LayoutDebugger::LogLayoutInfo(const std::string& name, const Rect& bounds)
{
    std::cout << "LAYOUT DEBUG: " << name << " bounds: (" << bounds.x << ", " << bounds.y 
              << ") size: " << bounds.w << "x" << bounds.h << std::endl;
}

void LayoutDebugger::ValidateLayoutConstraints(const std::vector<LayoutItem>& items, const Rect& container)
{
    int total_min_width = 0;
    int total_min_height = 0;
    
    for (const auto& item : items) {
        total_min_width += item.constraints.min_width + item.margin.left + item.margin.right;
        total_min_height += item.constraints.min_height + item.margin.top + item.margin.bottom;
    }
    
    if (total_min_width > container.w) {
        std::cerr << "LAYOUT WARNING: Total minimum width (" << total_min_width 
                  << ") exceeds container width (" << container.w << ")" << std::endl;
    }
    
    if (total_min_height > container.h) {
        std::cerr << "LAYOUT WARNING: Total minimum height (" << total_min_height 
                  << ") exceeds container height (" << container.h << ")" << std::endl;
    }
}

void LayoutDebugger::DrawDebugBounds(Renderer& renderer, const Rect& bounds, const Color& color)
{
    // This would draw debug rectangles around UI elements
    // Implementation depends on the renderer's drawing capabilities
    std::cout << "DEBUG BOUNDS: Drawing rectangle at (" << bounds.x << ", " << bounds.y 
              << ") size: " << bounds.w << "x" << bounds.h << std::endl;
}

Rect CommonPatterns::CreateCardContainer(const Rect& available_space, const Margin& margin)
{
    return Rect(
        available_space.x + margin.left,
        available_space.y + margin.top,
        available_space.w - margin.left - margin.right,
        available_space.h - margin.top - margin.bottom
    );
}

std::vector<LayoutItem> CommonPatterns::CreateHeaderSection(
    const std::string& title,
    const std::vector<std::function<void(const Rect&)>>& action_callbacks)
{
    std::vector<LayoutItem> header_items;
    
    // Title item
    LayoutItem title_item;
    title_item.constraints.preferred_height = UIStyleGuide::FontSize::HEADING;
    title_item.constraints.weight = 1.0f; // Take remaining space
    title_item.margin = Margin(UIStyleGuide::Spacing::MD);
    header_items.push_back(title_item);
    
    // Action buttons
    for (const auto& callback : action_callbacks) {
        LayoutItem action_item;
        action_item.constraints.preferred_width = UIStyleGuide::Dimensions::BUTTON_HEIGHT;
        action_item.constraints.preferred_height = UIStyleGuide::Dimensions::BUTTON_HEIGHT;
        action_item.margin = Margin(UIStyleGuide::Spacing::SM);
        action_item.on_bounds_changed = callback;
        header_items.push_back(action_item);
    }
    
    return header_items;
}

std::vector<LayoutItem> CommonPatterns::CreateStatusPanel(
    const std::vector<std::pair<std::string, std::string>>& status_items)
{
    std::vector<LayoutItem> panel_items;
    
    for (const auto& status_pair : status_items) {
        LayoutItem status_item;
        status_item.constraints.preferred_height = UIStyleGuide::FontSize::CAPTION;
        status_item.margin = Margin(UIStyleGuide::Spacing::XS, UIStyleGuide::Spacing::MD, 
                                   UIStyleGuide::Spacing::XS, UIStyleGuide::Spacing::MD);
        // Note: status_pair contains (label, value) data for the status item
        (void)status_pair; // Suppress unused variable warning
        panel_items.push_back(status_item);
    }
    
    return panel_items;
}

void ErrorHandler::HandleLayoutError(const std::string& error_message, const std::string& component_name)
{
    std::cerr << "LAYOUT ERROR in " << component_name << ": " << error_message << std::endl;
    // In a production system, you might want to log to a file or send to a crash reporting service
}

bool ErrorHandler::RecoverFromInvalidBounds(Rect& bounds, const Rect& fallback)
{
    bool was_invalid = false;
    
    if (bounds.w <= 0 || bounds.h <= 0) {
        std::cerr << "ERROR: Invalid bounds detected, using fallback" << std::endl;
        bounds = fallback;
        was_invalid = true;
    }
    
    return was_invalid;
}

void ErrorHandler::LogUIError(const std::string& error, const std::string& context)
{
    std::cerr << "UI ERROR [" << context << "]: " << error << std::endl;
}

} // namespace UIUtils