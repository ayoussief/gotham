// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "layout_manager.h"
#include "ui_style_guide.h"
#include <algorithm>
#include <numeric>

LayoutManager::LayoutManager() = default;

void LayoutManager::CreateVerticalLayout(const Rect& container, std::vector<LayoutItem>& items, 
                                       int spacing, Alignment alignment)
{
    if (items.empty()) return;

    int available_height = container.h;
    int total_spacing = (items.size() - 1) * spacing;
    int content_height = available_height - total_spacing;

    // Calculate total weight and fixed heights
    float total_weight = 0.0f;
    int fixed_height = 0;
    
    for (auto& item : items) {
        if (!item.visible) continue;
        
        if (item.constraints.preferred_height > 0) {
            fixed_height += item.constraints.preferred_height;
        } else {
            total_weight += item.constraints.weight;
        }
        fixed_height += item.margin.top + item.margin.bottom;
    }

    // Distribute remaining space among flexible items
    int flexible_height = content_height - fixed_height;
    int current_y = container.y;

    for (auto& item : items) {
        if (!item.visible) continue;

        current_y += item.margin.top;

        int item_height;
        if (item.constraints.preferred_height > 0) {
            item_height = item.constraints.preferred_height;
        } else {
            item_height = static_cast<int>((flexible_height * item.constraints.weight) / total_weight);
        }

        // Apply constraints
        if (item.constraints.min_height > 0) {
            item_height = std::max(item_height, item.constraints.min_height);
        }
        if (item.constraints.max_height > 0) {
            item_height = std::min(item_height, item.constraints.max_height);
        }

        // Calculate width based on alignment
        int item_width = container.w - item.margin.left - item.margin.right;
        int item_x = container.x + item.margin.left;

        if (alignment != Alignment::STRETCH && item.constraints.preferred_width > 0) {
            item_width = std::min(item.constraints.preferred_width, item_width);
            
            switch (alignment) {
                case Alignment::CENTER:
                    item_x = container.x + (container.w - item_width) / 2;
                    break;
                case Alignment::END:
                    item_x = container.x + container.w - item_width - item.margin.right;
                    break;
                default: // START
                    break;
            }
        }

        item.bounds = Rect(item_x, current_y, item_width, item_height);
        
        if (item.on_bounds_changed) {
            item.on_bounds_changed(item.bounds);
        }

        current_y += item_height + item.margin.bottom + spacing;
    }
}

void LayoutManager::CreateHorizontalLayout(const Rect& container, std::vector<LayoutItem>& items, 
                                         int spacing, Alignment alignment)
{
    if (items.empty()) return;

    int available_width = container.w;
    int total_spacing = (items.size() - 1) * spacing;
    int content_width = available_width - total_spacing;

    // Calculate total weight and fixed widths
    float total_weight = 0.0f;
    int fixed_width = 0;
    int total_margins = 0;
    
    for (auto& item : items) {
        if (!item.visible) continue;
        
        // Add margins for all items
        total_margins += item.margin.left + item.margin.right;
        
        if (item.constraints.preferred_width > 0) {
            fixed_width += item.constraints.preferred_width;
        } else {
            total_weight += item.constraints.weight;
        }
    }
    
    // Account for margins in available space
    fixed_width += total_margins;

    // Distribute remaining space among flexible items
    int flexible_width = content_width - fixed_width;
    int current_x = container.x;

    for (auto& item : items) {
        if (!item.visible) continue;

        current_x += item.margin.left;

        int item_width;
        if (item.constraints.preferred_width > 0) {
            item_width = item.constraints.preferred_width;
        } else {
            item_width = static_cast<int>((flexible_width * item.constraints.weight) / total_weight);
        }

        // Apply constraints
        if (item.constraints.min_width > 0) {
            item_width = std::max(item_width, item.constraints.min_width);
        }
        if (item.constraints.max_width > 0) {
            item_width = std::min(item_width, item.constraints.max_width);
        }

        // Calculate height based on alignment
        int item_height = container.h - item.margin.top - item.margin.bottom;
        int item_y = container.y + item.margin.top;

        if (alignment != Alignment::STRETCH && item.constraints.preferred_height > 0) {
            item_height = std::min(item.constraints.preferred_height, item_height);
            
            switch (alignment) {
                case Alignment::CENTER:
                    item_y = container.y + (container.h - item_height) / 2;
                    break;
                case Alignment::END:
                    item_y = container.y + container.h - item_height - item.margin.bottom;
                    break;
                default: // START
                    break;
            }
        }

        item.bounds = Rect(current_x, item_y, item_width, item_height);
        
        if (item.on_bounds_changed) {
            item.on_bounds_changed(item.bounds);
        }

        current_x += item_width + item.margin.right + spacing;
    }
}

void LayoutManager::CreateGridLayout(const Rect& container, std::vector<LayoutItem>& items, 
                                   int columns, int spacing)
{
    if (items.empty() || columns <= 0) return;

    int rows = (items.size() + columns - 1) / columns;
    int cell_width = (container.w - (columns - 1) * spacing) / columns;
    int cell_height = (container.h - (rows - 1) * spacing) / rows;

    for (size_t i = 0; i < items.size(); ++i) {
        if (!items[i].visible) continue;

        int row = i / columns;
        int col = i % columns;

        int x = container.x + col * (cell_width + spacing);
        int y = container.y + row * (cell_height + spacing);

        items[i].bounds = Rect(x + items[i].margin.left, 
                              y + items[i].margin.top,
                              cell_width - items[i].margin.left - items[i].margin.right,
                              cell_height - items[i].margin.top - items[i].margin.bottom);

        if (items[i].on_bounds_changed) {
            items[i].on_bounds_changed(items[i].bounds);
        }
    }
}

void LayoutManager::CreateFormLayout(const Rect& container, std::vector<LayoutItem>& items, 
                                   int label_width, int spacing)
{
    if (items.empty()) return;

    int row_height = 40; // Standard form row height
    int current_y = container.y;

    for (size_t i = 0; i < items.size(); i += 2) { // Assuming pairs of label+input
        if (i + 1 >= items.size()) break;

        auto& label_item = items[i];
        auto& input_item = items[i + 1];

        if (!label_item.visible || !input_item.visible) {
            current_y += spacing;
            continue;
        }

        // Label bounds
        label_item.bounds = Rect(container.x + label_item.margin.left,
                               current_y + label_item.margin.top,
                               label_width - label_item.margin.left - label_item.margin.right,
                               row_height - label_item.margin.top - label_item.margin.bottom);

        // Input bounds
        int input_x = container.x + label_width + spacing;
        int input_width = container.w - label_width - spacing - input_item.margin.left - input_item.margin.right;
        
        input_item.bounds = Rect(input_x + input_item.margin.left,
                               current_y + input_item.margin.top,
                               input_width,
                               row_height - input_item.margin.top - input_item.margin.bottom);

        if (label_item.on_bounds_changed) {
            label_item.on_bounds_changed(label_item.bounds);
        }
        if (input_item.on_bounds_changed) {
            input_item.on_bounds_changed(input_item.bounds);
        }

        current_y += row_height + spacing;
    }
}

void LayoutManager::CreateHeaderLayout(const Rect& container, std::vector<LayoutItem>& items)
{
    if (items.empty()) return;

    // Header layout: Title (left), Balance (center), Settings (right)
    // Use UIStyleGuide spacing for consistency
    int padding = UIStyleGuide::Spacing::LG;
    int button_width = 100;
    int min_spacing = UIStyleGuide::Spacing::MD;

    for (size_t i = 0; i < items.size(); ++i) {
        auto& item = items[i];
        if (!item.visible) continue;

        if (i == 0) { // Title - left aligned
            item.bounds = Rect(container.x + padding, container.y + padding, 200, container.h - 2 * padding);
        } else if (i == items.size() - 1) { // Last item - right aligned (settings button)
            item.bounds = Rect(container.x + container.w - button_width - padding, 
                             container.y + padding, button_width, container.h - 2 * padding);
        } else { // Middle items - positioned with proper spacing to avoid overlap
            // Calculate available space between title and settings button
            int title_end = container.x + padding + 200;
            int settings_start = container.x + container.w - button_width - padding;
            int available_start = title_end + min_spacing;
            int available_end = settings_start - min_spacing;
            int available_width = available_end - available_start;
            
            // Only position if there's enough space
            if (available_width > 100) { // Minimum width for balance display
                int balance_width = std::min(250, available_width);
                int balance_x = available_start + (available_width - balance_width) / 2;
                item.bounds = Rect(balance_x, container.y + padding, balance_width, container.h - 2 * padding);
            } else {
                // If not enough space, position balance below title (responsive behavior)
                item.bounds = Rect(container.x + padding, container.y + padding + 30, 
                                 std::min(250, container.w - 2 * padding), 20);
            }
        }

        if (item.on_bounds_changed) {
            item.on_bounds_changed(item.bounds);
        }
    }
}

void LayoutManager::CreateSidebarLayout(const Rect& container, std::vector<LayoutItem>& items, int spacing)
{
    if (items.empty()) return;

    int button_height = 50;
    int padding = 10;
    int current_y = container.y + padding;

    for (auto& item : items) {
        if (!item.visible) continue;

        item.bounds = Rect(container.x + padding,
                          current_y,
                          container.w - 2 * padding,
                          button_height);

        if (item.on_bounds_changed) {
            item.on_bounds_changed(item.bounds);
        }

        current_y += button_height + spacing;
    }
}

void LayoutManager::CreateStatusBarLayout(const Rect& container, std::vector<LayoutItem>& items)
{
    if (items.empty()) return;

    int padding = 10;
    int item_width = 150;
    int current_x = container.x + padding;

    for (auto& item : items) {
        if (!item.visible) continue;

        item.bounds = Rect(current_x, container.y, item_width, container.h);

        if (item.on_bounds_changed) {
            item.on_bounds_changed(item.bounds);
        }

        current_x += item_width + padding;
    }
}

// Standard layout bounds
Rect LayoutManager::StandardLayouts::GetHeaderBounds(int screen_width, int screen_height)
{
    return Rect(0, 0, screen_width, 80);
}

Rect LayoutManager::StandardLayouts::GetSidebarBounds(int screen_width, int screen_height)
{
    return Rect(0, 80, 200, screen_height - 110);
}

Rect LayoutManager::StandardLayouts::GetMainContentBounds(int screen_width, int screen_height)
{
    return Rect(200, 80, screen_width - 200, screen_height - 110);
}

Rect LayoutManager::StandardLayouts::GetStatusBarBounds(int screen_width, int screen_height)
{
    return Rect(0, screen_height - 30, screen_width, 30);
}

Rect LayoutManager::StandardLayouts::GetModalBounds(int screen_width, int screen_height, 
                                                   int modal_width, int modal_height)
{
    int x = (screen_width - modal_width) / 2;
    int y = (screen_height - modal_height) / 2;
    return Rect(x, y, modal_width, modal_height);
}

void LayoutManager::CreateStackLayout(const Rect& container, std::vector<LayoutItem>& items, 
                                     Alignment horizontal_align, Alignment vertical_align)
{
    if (items.empty()) return;

    for (auto& item : items) {
        if (!item.visible) continue;

        // Apply margins
        int content_x = container.x + item.margin.left;
        int content_y = container.y + item.margin.top;
        int content_w = container.w - item.margin.left - item.margin.right;
        int content_h = container.h - item.margin.top - item.margin.bottom;

        // Get preferred size or use container size
        int item_w = item.constraints.preferred_width > 0 ? 
                     item.constraints.preferred_width : content_w;
        int item_h = item.constraints.preferred_height > 0 ? 
                     item.constraints.preferred_height : content_h;

        // Apply constraints
        if (item.constraints.max_width > 0) {
            item_w = std::min(item_w, item.constraints.max_width);
        }
        if (item.constraints.max_height > 0) {
            item_h = std::min(item_h, item.constraints.max_height);
        }
        item_w = std::max(item_w, item.constraints.min_width);
        item_h = std::max(item_h, item.constraints.min_height);

        // Calculate position based on alignment
        int item_x = content_x;
        int item_y = content_y;

        switch (horizontal_align) {
            case Alignment::CENTER:
                item_x = content_x + (content_w - item_w) / 2;
                break;
            case Alignment::END:
                item_x = content_x + content_w - item_w;
                break;
            case Alignment::START:
            case Alignment::STRETCH:
            default:
                item_x = content_x;
                if (horizontal_align == Alignment::STRETCH) {
                    item_w = content_w;
                }
                break;
        }

        switch (vertical_align) {
            case Alignment::CENTER:
                item_y = content_y + (content_h - item_h) / 2;
                break;
            case Alignment::END:
                item_y = content_y + content_h - item_h;
                break;
            case Alignment::START:
            case Alignment::STRETCH:
            default:
                item_y = content_y;
                if (vertical_align == Alignment::STRETCH) {
                    item_h = content_h;
                }
                break;
        }

        item.bounds = Rect(item_x, item_y, item_w, item_h);

        if (item.on_bounds_changed) {
            item.on_bounds_changed(item.bounds);
        }
    }
}

void LayoutManager::CreateCardLayout(const Rect& container, std::vector<LayoutItem>& items, 
                                   const Padding& padding)
{
    if (items.empty()) return;

    // Create content area with padding
    Rect content_area = Rect(
        container.x + padding.left,
        container.y + padding.top,
        container.w - padding.left - padding.right,
        container.h - padding.top - padding.bottom
    );

    // Use vertical layout for card content
    CreateVerticalLayout(content_area, items, UIStyleGuide::Spacing::MD, Alignment::STRETCH);
}

void LayoutManager::CreateContentLayout(const Rect& container, std::vector<LayoutItem>& items, 
                                      int max_width, Alignment alignment)
{
    if (items.empty()) return;

    // Calculate content width (constrained by max_width)
    int content_width = std::min(container.w, max_width);
    int content_x = container.x;

    // Apply alignment for content positioning
    switch (alignment) {
        case Alignment::CENTER:
            content_x = container.x + (container.w - content_width) / 2;
            break;
        case Alignment::END:
            content_x = container.x + container.w - content_width;
            break;
        case Alignment::START:
        case Alignment::STRETCH:
        default:
            content_x = container.x;
            if (alignment == Alignment::STRETCH) {
                content_width = container.w;
            }
            break;
    }

    // Create content area
    Rect content_area = Rect(content_x, container.y, content_width, container.h);

    // Use vertical layout for content
    CreateVerticalLayout(content_area, items, UIStyleGuide::Spacing::LG, Alignment::STRETCH);
}