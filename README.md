# rayedit

Started as exam-period procrastination with raylib, gradually became
something resembling a text editor. Not sure how far it'll go.

## How to build & use

1. Download
   [raylib5](https://github.com/raysan5/raylib/releases#release-5.5)
   and extract tarball somewhere
2. `make RAYLIB_PREFIX=/path/to/raylib`
3. `./red <file>`
4. Ctrl+s to save

## Something implemented

- mmap-based fast loading and rendering
- lazy allocation when modifying each line

## Something not implemented (yet)

- Scroll
- Handling when the cursor goes off the screen because a line is too
  long
