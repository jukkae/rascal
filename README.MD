# RASCAL: a Roguelike

Rascal is a post-apocalyptic science fiction roguelike.

## Compilation
Currently macOS only.

### Set up a clean machine

1. Install SFML with homebrew.
2. Install boost with homebrew.
3. Run `mkdir obj` – this should ideally be in `makefile`.
4. Clone https://github.com/auriamg/macdylibbundler.git to ../ and run make there
5. Run `mkdir frameworks` and copy SFML framework files there. It's likely this is redundant, but this needs to be looked into later.

### Building bundle

When running `make bundle`, it's possible that macdylibbundler complains `/!\ WARNING : can't get path for '@rpath/libsfml-system.2.4.dylib'`. If this happens, the quickest workaround is to just paste a zillion times `/usr/local/lib`. Should fix this at some point. It's entirely possible that there's something wrong with the whole `rpath` thing here.

## Dependencies
- SFML 2.4.2
- Boost 1.65.1

## Acknowledgments
Includes freetype library licensed under GPL 2.
Current macOS builds include SFML and boost libraries.

Current builds include Square typeface by Wouter van Oortmerssen licensed under CC BY 3.0. Typeface available at <http://strlen.com/square/>.
Current builds include Fixedsys Excelsior typeface by Darien Valentine released into public domain.

Thanks to Aleksi, Valtteri and Valtteri for testing.
Special thanks to Tommi for invaluable help in early testing and concepting.
Special thanks to Ville for consultations on architecture and design.
