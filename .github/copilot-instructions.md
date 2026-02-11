# Crawl
## Project structure
The project is structured as follows:
- `src/`: Contains all the source code of the game
- `assets/`: Contains all the assets of the game (images, sounds, etc.)
- `out/`: Contains the build files and the compiled executable
- `Makefile`: The makefile to build the project

## Code style
- The code is written in modern C++
- For standalone functions I like to use static class methods of static classes
- For normal classes I create both a header and a source file, and I put the class declaration in the header file and the implementation in the source file (I don't put the implementation in the header file)
- I use `camelCase` for variable and function names, and `PascalCase` for class names

## Conventions
- When I have a position attribute of an entity, this is the location of the bottom-left corner unless otherwise specified