# RASCAL: a Roguelike

Rascal is a post-apocalyptic science fiction roguelike.

## Compilation
Currently macOS only.

### Set up a clean machine

1. Install SFML with homebrew.
2. Install boost with homebrew.
3. Run `mkdir obj` – this should ideally be in `makefile`.
4. On macOS, clone https://github.com/rkitover/mac-third-party-libs-tool to ../
5. Run `mkdir frameworks` and copy SFML framework files there. It's likely this is redundant, but this needs to be looked into later.

### Building with Meson

```
$ mkdir build
$ meson build
$ ninja -C build build OR $ ninja -C build bundle
```

## Dependencies
- SFML 2.4.2 or higher
- Boost 1.65.1 or higher

## Acknowledgments
Includes toml11 library licensed under MIT License.
Includes freetype library licensed under FTL.
Includes SFML licensed under Zlib License.
Current macOS builds include boost libraries.
All copyrights of external libraries belong to their authors.

Current builds include Square typeface by Wouter van Oortmerssen licensed under CC BY 3.0. Typeface available at <http://strlen.com/square/>.
Current builds include Fixedsys Excelsior typeface by Darien Valentine released into public domain.

Thanks to Aleksi, Valtteri and Valtteri for testing.
Special thanks to Tommi for invaluable help in early testing and concepting.
Special thanks to Ville for consultations on architecture and design.
