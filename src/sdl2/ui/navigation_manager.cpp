// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "navigation_manager.h"
#include "../gotham_city_gui.h" // For ScreenType enum
#include <algorithm>
#include <iostream>

NavigationManager::NavigationManager()
{
    InitializeScreenTitles();
}

void NavigationManager::NavigateTo(ScreenType screen_type, const NavigationContext& context)
{
    // Create navigation context
    NavigationContext nav_context = context;
    nav_context.from_screen = GetCurrentScreen();
    nav_context.to_screen = screen_type;
    
    // Add to history
    AddToHistory(screen_type, nav_context);
    
    // Update current context
    m_current_context = nav_context;
    
    // Trigger navigation
    TriggerNavigation(screen_type, nav_context);
    
    std::cout << "Navigated to " << GetScreenTitle(screen_type) << std::endl;
}

bool NavigationManager::NavigateBack()
{
    if (!CanGoBack()) {
        return false;
    }
    
    m_current_index--;
    const auto& entry = m_history[m_current_index];
    
    // Update context
    m_current_context = entry.context;
    m_current_context.from_screen = m_history[m_current_index + 1].screen_type;
    m_current_context.to_screen = entry.screen_type;
    
    // Trigger navigation
    TriggerNavigation(entry.screen_type, m_current_context);
    
    std::cout << "Navigated back to " << GetScreenTitle(entry.screen_type) << std::endl;
    return true;
}

bool NavigationManager::NavigateForward()
{
    if (!CanGoForward()) {
        return false;
    }
    
    m_current_index++;
    const auto& entry = m_history[m_current_index];
    
    // Update context
    m_current_context = entry.context;
    m_current_context.from_screen = m_history[m_current_index - 1].screen_type;
    m_current_context.to_screen = entry.screen_type;
    
    // Trigger navigation
    TriggerNavigation(entry.screen_type, m_current_context);
    
    std::cout << "Navigated forward to " << GetScreenTitle(entry.screen_type) << std::endl;
    return true;
}

void NavigationManager::ReplaceScreen(ScreenType screen_type, const NavigationContext& context)
{
    // Replace current entry instead of adding new one
    if (!m_history.empty() && m_current_index >= 0) {
        m_history[m_current_index].screen_type = screen_type;
        m_history[m_current_index].context = context;
        m_history[m_current_index].title = GetScreenTitle(screen_type);
    } else {
        AddToHistory(screen_type, context);
    }
    
    // Update current context
    NavigationContext nav_context = context;
    nav_context.to_screen = screen_type;
    m_current_context = nav_context;
    
    // Trigger navigation
    TriggerNavigation(screen_type, nav_context);
    
    std::cout << "Replaced screen with " << GetScreenTitle(screen_type) << std::endl;
}

void NavigationManager::ClearHistory()
{
    m_history.clear();
    m_current_index = -1;
    m_current_context = NavigationContext{};
}

std::vector<NavigationEntry> NavigationManager::GetBreadcrumbs() const
{
    std::vector<NavigationEntry> breadcrumbs;
    
    // Return up to the current index
    for (int i = 0; i <= m_current_index && i < static_cast<int>(m_history.size()); ++i) {
        breadcrumbs.push_back(m_history[i]);
    }
    
    return breadcrumbs;
}

bool NavigationManager::CanGoBack() const
{
    return m_current_index > 0 && !m_history.empty();
}

bool NavigationManager::CanGoForward() const
{
    return m_current_index < static_cast<int>(m_history.size()) - 1;
}

ScreenType NavigationManager::GetCurrentScreen() const
{
    if (m_current_index >= 0 && m_current_index < static_cast<int>(m_history.size())) {
        return m_history[m_current_index].screen_type;
    }
    return ScreenType::MAIN; // Default
}

ScreenType NavigationManager::GetPreviousScreen() const
{
    if (m_current_index > 0) {
        return m_history[m_current_index - 1].screen_type;
    }
    return ScreenType::MAIN; // Default
}

void NavigationManager::SetNavigationCallback(std::function<void(ScreenType, const NavigationContext&)> callback)
{
    m_navigation_callback = callback;
}

std::string NavigationManager::GetScreenTitle(ScreenType screen_type) const
{
    auto it = m_screen_titles.find(screen_type);
    return (it != m_screen_titles.end()) ? it->second : "Unknown Screen";
}

void NavigationManager::SetScreenTitle(ScreenType screen_type, const std::string& title)
{
    m_screen_titles[screen_type] = title;
}

// Navigation shortcuts for common patterns
void NavigationManager::NavigateToWalletFromMain()
{
    NavigationContext context;
    context.SetParameter("source", "main_dashboard");
    NavigateTo(ScreenType::WALLET, context);
}

void NavigationManager::NavigateToSendFromWallet(const std::string& address)
{
    NavigationContext context;
    context.SetParameter("source", "wallet");
    if (!address.empty()) {
        context.SetParameter("prefill_address", address);
    }
    NavigateTo(ScreenType::SEND, context);
}

void NavigationManager::NavigateToReceiveFromWallet()
{
    NavigationContext context;
    context.SetParameter("source", "wallet");
    NavigateTo(ScreenType::RECEIVE, context);
}

void NavigationManager::NavigateToTransactionsFromWallet()
{
    NavigationContext context;
    context.SetParameter("source", "wallet");
    NavigateTo(ScreenType::TRANSACTIONS, context);
}

void NavigationManager::NavigateToSettingsFromMain()
{
    NavigationContext context;
    context.SetParameter("source", "main");
    NavigateTo(ScreenType::SETTINGS, context);
}

void NavigationManager::NavigateBackToWallet()
{
    // Smart navigation - if wallet is in history, go back to it
    for (int i = m_current_index - 1; i >= 0; --i) {
        if (m_history[i].screen_type == ScreenType::WALLET) {
            m_current_index = i + 1; // Set to one past target
            NavigateBack();
            return;
        }
    }
    
    // If wallet not in history, navigate to it fresh
    NavigateToWalletFromMain();
}

void NavigationManager::NavigateBackToMain()
{
    // Smart navigation - if main is in history, go back to it
    for (int i = m_current_index - 1; i >= 0; --i) {
        if (m_history[i].screen_type == ScreenType::MAIN) {
            m_current_index = i + 1; // Set to one past target
            NavigateBack();
            return;
        }
    }
    
    // If main not in history, navigate to it fresh
    NavigateTo(ScreenType::MAIN);
}

// State persistence helpers
void NavigationManager::SaveScreenState(ScreenType screen_type, const std::string& key, const std::string& value)
{
    m_screen_states[screen_type][key] = value;
}

std::string NavigationManager::GetScreenState(ScreenType screen_type, const std::string& key, const std::string& default_value) const
{
    auto screen_it = m_screen_states.find(screen_type);
    if (screen_it != m_screen_states.end()) {
        auto state_it = screen_it->second.find(key);
        if (state_it != screen_it->second.end()) {
            return state_it->second;
        }
    }
    return default_value;
}

void NavigationManager::ClearScreenState(ScreenType screen_type)
{
    m_screen_states[screen_type].clear();
}

void NavigationManager::InitializeScreenTitles()
{
    m_screen_titles[ScreenType::SPLASH] = "Loading";
    m_screen_titles[ScreenType::MAIN] = "Dashboard";
    m_screen_titles[ScreenType::WALLET] = "Wallet";
    m_screen_titles[ScreenType::SEND] = "Send";
    m_screen_titles[ScreenType::RECEIVE] = "Receive";
    m_screen_titles[ScreenType::TRANSACTIONS] = "Transactions";
    m_screen_titles[ScreenType::SETTINGS] = "Settings";
    m_screen_titles[ScreenType::CONSOLE] = "Console";
}

void NavigationManager::AddToHistory(ScreenType screen_type, const NavigationContext& context)
{
    // Remove any forward history when navigating to new screen
    if (m_current_index < static_cast<int>(m_history.size()) - 1) {
        m_history.erase(m_history.begin() + m_current_index + 1, m_history.end());
    }
    
    // Add new entry
    NavigationEntry entry(screen_type, GetScreenTitle(screen_type));
    entry.context = context;
    m_history.push_back(entry);
    m_current_index = static_cast<int>(m_history.size()) - 1;
}

void NavigationManager::TriggerNavigation(ScreenType screen_type, const NavigationContext& context)
{
    if (m_navigation_callback) {
        m_navigation_callback(screen_type, context);
    }
}

// BreadcrumbBuilder implementation
std::vector<NavigationBreadcrumb> BreadcrumbBuilder::BuildBreadcrumbs(const NavigationManager& nav_manager)
{
    std::vector<NavigationBreadcrumb> breadcrumbs;
    auto entries = nav_manager.GetBreadcrumbs();
    
    for (size_t i = 0; i < entries.size(); ++i) {
        NavigationBreadcrumb crumb;
        crumb.title = entries[i].title;
        crumb.screen_type = entries[i].screen_type;
        crumb.is_current = (i == entries.size() - 1);
        crumb.is_clickable = !crumb.is_current && entries[i].can_go_back;
        breadcrumbs.push_back(crumb);
    }
    
    return breadcrumbs;
}

std::string BreadcrumbBuilder::BuildBreadcrumbText(const NavigationManager& nav_manager, const std::string& separator)
{
    auto breadcrumbs = BuildBreadcrumbs(nav_manager);
    std::string result;
    
    for (size_t i = 0; i < breadcrumbs.size(); ++i) {
        if (i > 0) {
            result += separator;
        }
        result += breadcrumbs[i].title;
    }
    
    return result;
}