# Gotham City - SDL2 GUI for Gotham Core

Gotham City is a modern, game-like SDL2-based graphical user interface for the Gotham cryptocurrency wallet. Inspired by the Batman universe, it provides a sleek, dark-themed interface with smooth animations and an intuitive user experience.

## Features

### 🦇 Batman-Inspired Design
- Dark theme with Gotham gold accents
- Atmospheric background effects
- Smooth animations and transitions
- Modern, game-like interface

### 💰 Cryptocurrency Wallet Functionality
- Wallet overview and balance display
- Send and receive transactions
- Transaction history
- Address management
- Network status monitoring
- Debug console access

### 🎮 Advanced UI Components
- Custom button controls with hover effects
- Text input fields with cursor support
- Progress bars with animations
- Panels and layouts
- Label components with text alignment
- Screen-based navigation system

### 🎨 Visual Effects
- Gradient backgrounds
- Glow effects
- Smooth animations
- Responsive design
- Theme management system

## Architecture

### Core Components

#### Application Layer
- `GothamCityApp`: Main application class handling initialization and lifecycle
- `GothamCityGUI`: GUI manager coordinating screens and rendering
- `WindowManager`: SDL2 window management and fullscreen support
- `Renderer`: Abstraction layer for SDL2 rendering operations

#### UI Framework
- `Screen`: Base class for all application screens
- `Button`: Interactive button component with animations
- `Label`: Text display component with alignment options
- `Panel`: Container component for organizing UI elements
- `TextInput`: Text input field with cursor and selection support
- `ProgressBar`: Animated progress indicator

#### Utility Systems
- `ThemeManager`: Color scheme and styling management
- `FontManager`: Font loading and caching system
- `TextureManager`: Image and texture resource management
- `AnimationManager`: Animation system with easing functions

#### Screens
- `SplashScreen`: Application startup screen with loading animation
- `MainScreen`: Dashboard with wallet overview and navigation
- `WalletScreen`: Detailed wallet information and management
- `SendScreen`: Transaction sending interface
- `ReceiveScreen`: Address generation and QR codes
- `TransactionsScreen`: Transaction history and details
- `SettingsScreen`: Application configuration
- `ConsoleScreen`: Debug console for advanced users

## Building

### Prerequisites
- SDL2 development libraries
- SDL2_ttf for font rendering
- SDL2_image for image loading
- CMake 3.16 or higher
- C++17 compatible compiler

### Ubuntu/Debian
```bash
sudo apt-get install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev
```

### Build Instructions
```bash
mkdir build
cd build
cmake ..
make gotham-city
```

## Usage

### Running Gotham City
```bash
./gotham-city [options]
```

### Command Line Options
- `-h, --help`: Show help message
- `--datadir=<dir>`: Specify data directory
- `--testnet`: Run on testnet
- `--regtest`: Run on regression test network

### Controls
- **F11**: Toggle fullscreen mode
- **Mouse**: Navigate and interact with UI elements
- **Keyboard**: Text input and navigation shortcuts

## Screens Overview

### Splash Screen
- Animated Gotham City logo
- Loading progress indicator
- Smooth fade-in effects
- Automatic transition to main screen

### Main Screen
- Wallet balance display
- Network connection status
- Quick navigation sidebar
- Real-time updates

### Navigation Sidebar
- **Wallet**: Overview and management
- **Send**: Create and send transactions
- **Receive**: Generate receiving addresses
- **Transactions**: View transaction history
- **Console**: Debug and advanced features
- **Settings**: Application preferences

## Theming

### Color Scheme
- **Background**: Dark blue-black (#0F0F14)
- **Surface**: Dark surface (#191923)
- **Primary**: Gotham gold (#FFD700)
- **Secondary**: Steel blue (#4682B4)
- **Text**: Light colors for readability
- **Accents**: Orange and blue highlights

### Fonts
- **Title**: Large, bold fonts for headings
- **Body**: Medium fonts for content
- **Monospace**: Fixed-width fonts for addresses and hashes

## Development

### Adding New Screens
1. Create header and source files in `screens/`
2. Inherit from `Screen` base class
3. Implement required virtual methods
4. Add to `ScreenType` enum
5. Update `CreateScreen()` method

### Creating UI Components
1. Create component class with event handling
2. Implement `HandleEvent()`, `Update()`, and `Render()` methods
3. Add to parent screen or panel
4. Handle user interactions with callbacks

### Animation System
```cpp
// Create fade animation
auto fade_anim = animation_manager->CreateFadeAnimation(
    1.0f,           // Duration
    0.0f, 1.0f,     // From/to alpha
    [this](float alpha) {
        // Update callback
        component->SetAlpha(alpha);
    }
);

// Start animation
animation_manager->AddAnimation("fade_in", std::move(fade_anim));
animation_manager->StartAnimation("fade_in");
```

## Future Enhancements

### Planned Features
- [ ] QR code generation and scanning
- [ ] Multi-wallet support
- [ ] Hardware wallet integration
- [ ] Advanced transaction features
- [ ] Network graph visualization
- [ ] Sound effects and audio feedback
- [ ] Customizable themes
- [ ] Plugin system
- [ ] Mobile-responsive design

### Technical Improvements
- [ ] Vulkan renderer backend
- [ ] GPU-accelerated effects
- [ ] Better font rendering
- [ ] Accessibility features
- [ ] Internationalization support
- [ ] Performance optimizations

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- SDL2 development team for the excellent multimedia library
- Batman/DC Comics for the visual inspiration
- Gotham Core development team
- Open source cryptocurrency community

---

*"It's not who I am underneath, but what I do that defines me."* - Batman

Welcome to Gotham City - where cryptocurrency meets the Dark Knight's aesthetic.