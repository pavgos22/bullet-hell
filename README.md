# Bullet Hell Game SDL C/C++

## Overview
A 2D bullet hell game where the player controls a character from a top-down perspective, navigating through waves of enemy fire. The primary goal is to dodge incoming bullets while strategically shooting at enemies. Developed using the SDL2 library in C, the game offers smooth performance and responsive controls.

## Features

### Core Gameplay
- **Top-Down Shooter**: The player navigates the game world from a top-down perspective, using the arrow keys for movement and the spacebar to shoot.
- **Enemy Interaction**: The game includes a variety of enemies with different behaviors and shooting patterns. The player must dodge bullets and return fire to progress.

### Levels
The game consists of three levels, each offering unique challenges:

- **Level 1**: This level serves as an introduction, with simple enemies and straightforward bullet patterns. It allows the player to familiarize themselves with the controls and mechanics.
- **Level 2**: The difficulty ramps up with more complex enemy formations and faster bullet patterns. The enemies start using more advanced shooting tactics, such as spirals and waves.
- **Level 3**: The final level features the most challenging enemies, including those that move across the screen and shoot in unpredictable patterns. This level tests the player’s reflexes and mastery of the game mechanics.

### Game Mechanics
- **Shooting and Movement**: The player can move in four directions and shoot in the direction they are facing.
- **Invincibility Frames**: After taking damage, the player becomes invincible for a short period, preventing immediate consecutive hits.
- **Health System**: The player has a health bar that decreases when hit by enemy bullets or colliding with enemies. If health reaches zero, the game ends. Player has 3 health points.
- **Score System**: Points are awarded for hitting enemies, with bonus points for combos (multiple hits in a short period). The player's score is displayed prominently on the screen. The more you score, the better rank you gain.
- **Enemies**: In first two levels enemies are static, while in level 3 enemies move horizontally and shoot bullets. Each enemy in every level has 3hp, which means you have to hit him three times to kill him.
- **Consumables and traps**: Every few seconds there is a chance a health potion or a trap will spawn on the map. When collected, fills health to maximum instantly. Traps on the opposite reduce your hp by one, when player is in their hitbox area for longer than about 2 seconds.

- ### Advanced Enemy Patterns
- **Dynamic Bullet Patterns**: Enemies fire bullets in various patterns, including spirals, waves, and expanding circles.
- **Moving Enemies**: Some enemies in higher levels move horizontally or vertically, making them harder to hit and adding to the challenge.

### Visual and Audio Elements
- **Animations**: The game features animated sprites for player movement, enemy actions, gaining ranks and explosions. These animations enhance the visual appeal and provide feedback on actions.
- **Backgrounds**: Each level has a unique background that scrolls as the player moves, creating a sense of a larger world.

### High Scores and Menu
- **High Score Tracking**: After completing a level, players can save their scores, which are displayed in a high score table accessible from the main menu.
- **Main Menu**: The game starts with a main menu where players can select a level, view high scores, or quit the game.

## Technical Details
- **SDL2 Integration**: The game is built using the <a href="https://www.libsdl.org/">SDL2 (2.0.3)</a> library for handling graphics, input, and events. 

## How to Build and Run
1. Clone the repository or download the zip file.
2. Ensure directory contains the libSDL2.a file (libSDL2-64.a when compiling a 64-bit version) and assets folder.
3. Run main.cpp script or build the program in Visual Studio 2019 (recommended)

## Additional Information
- **SDL2 Documentation**: For more information on SDL2, visit the [official SDL2 documentation](https://wiki.libsdl.org/).
- **Expanding the Game**: Future expansions could include customizable levels, new enemy types, and more complex bullet patterns.

Enjoy the game and have fun dodging bullets!
