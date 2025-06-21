// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

/**
 * Gotham City SDL2 GUI Demo
 * 
 * This file demonstrates the key features and visual design of the Gotham City interface.
 * It showcases the Batman-inspired theme, smooth animations, and modern UI components.
 */

#include <iostream>
#include <string>

void PrintGothamCityDemo() {
    std::cout << R"(
🦇 ═══════════════════════════════════════════════════════════════════════════════ 🦇
                              GOTHAM CITY
                         SDL2 GUI for Gotham Core
🦇 ═══════════════════════════════════════════════════════════════════════════════ 🦇

┌─────────────────────────────────────────────────────────────────────────────────┐
│                                                                                 │
│ ██████╗  ██████╗ ████████╗██╗  ██╗ █████╗ ███╗   ███╗     ██████╗██╗████████╗██╗   ██╗ │
│██╔════╝ ██╔═══██╗╚══██╔══╝██║  ██║██╔══██╗████╗ ████║    ██╔════╝██║╚══██╔══╝╚██╗ ██╔╝ │
│██║  ███╗██║   ██║   ██║   ███████║███████║██╔████╔██║    ██║     ██║   ██║    ╚████╔╝  │
│██║   ██║██║   ██║   ██║   ██╔══██║██╔══██║██║╚██╔╝██║    ██║     ██║   ██║     ╚██╔╝   │
│╚██████╔╝╚██████╔╝   ██║   ██║  ██║██║  ██║██║ ╚═╝ ██║    ╚██████╗██║   ██║      ██║    │
│ ╚═════╝  ╚═════╝    ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝     ╚═╝     ╚═════╝╚═╝   ╚═╝      ╚═╝    │
│                                                                                 │
│                        🌃 Cryptocurrency Wallet 🌃                             │
└─────────────────────────────────────────────────────────────────────────────────┘

🎨 VISUAL FEATURES:
   ▪ Dark theme with Gotham gold accents (#FFD700)
   ▪ Atmospheric background effects and gradients
   ▪ Smooth animations and transitions
   ▪ Modern, game-like interface design
   ▪ Batman-inspired visual elements

💰 WALLET FEATURES:
   ▪ Real-time balance display
   ▪ Send and receive transactions
   ▪ Transaction history with details
   ▪ Address management and QR codes
   ▪ Network status monitoring
   ▪ Debug console for advanced users

🎮 UI COMPONENTS:
   ▪ Interactive buttons with hover effects
   ▪ Text input fields with cursor support
   ▪ Animated progress bars
   ▪ Responsive panels and layouts
   ▪ Screen-based navigation system

┌─ MAIN SCREEN LAYOUT ────────────────────────────────────────────────────────────┐
│                                                                                 │
│ [GOTHAM CITY]                                    [Balance: 1.23456789 GTC] ⚙️   │
│ ═══════════════════════════════════════════════════════════════════════════════ │
│ │ 🦇 Wallet      │                                                             │
│ │ 💸 Send        │  Welcome to Gotham City                                     │
│ │ 📥 Receive     │                                                             │
│ │ 📊 Transactions│  Network: Connected ✅                                       │
│ │ 🖥️  Console     │  Synchronization: Up to date ✅                            │
│ │                │                                                             │
│ │                │  [Latest Transactions]                                      │
│ │                │  ┌─────────────────────────────────────────────────────┐   │
│ │                │  │ ↗️ Sent    0.12345678 GTC  2024-01-15 14:30:22     │   │
│ │                │  │ ↙️ Received 0.50000000 GTC  2024-01-15 12:15:45     │   │
│ │                │  │ ↗️ Sent    0.25000000 GTC  2024-01-14 18:45:12     │   │
│ │                │  └─────────────────────────────────────────────────────┘   │
│ └────────────────┘                                                             │
│ ═══════════════════════════════════════════════════════════════════════════════ │
│ Connections: 8 | Block: 1,234,567 | Status: Synchronized                       │
└─────────────────────────────────────────────────────────────────────────────────┘

🚀 GETTING STARTED:

1. Install Dependencies:
   Ubuntu/Debian: sudo apt-get install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev
   Fedora: sudo dnf install SDL2-devel SDL2_ttf-devel SDL2_image-devel
   macOS: brew install sdl2 sdl2_ttf sdl2_image

2. Build:
   ./build.sh

3. Run:
   cd build && ./gotham-city

🎮 CONTROLS:
   F11     - Toggle fullscreen
   Mouse   - Navigate and interact
   ESC     - Back/Cancel
   Enter   - Confirm actions

🌃 SCREENS:
   ▪ Splash Screen - Animated startup with loading progress
   ▪ Main Screen   - Dashboard with wallet overview
   ▪ Wallet Screen - Detailed wallet management
   ▪ Send Screen   - Transaction creation interface
   ▪ Receive Screen- Address generation with QR codes
   ▪ Transactions  - Complete transaction history
   ▪ Settings      - Application configuration
   ▪ Console       - Debug and advanced features

🦇 "It's not who I am underneath, but what I do that defines me." - Batman

   Welcome to Gotham City - where cryptocurrency meets the Dark Knight's aesthetic!

🦇 ═══════════════════════════════════════════════════════════════════════════════ 🦇
)" << std::endl;
}

int main() {
    PrintGothamCityDemo();
    return 0;
}