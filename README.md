### About
Simple .obj viewer based on code from an [old project](http://openglsamples.sourceforge.net/projects/index.php/blog/index/)

#### Features

* Easy to compile with very few dependencies.
* Ability to zoom / pan view
* Multiple lights that can be toggled on/off
* Uses VBOs for faster rendering

On the down side: can load only simple meshes with triangular faces. Materials are ignored.

### Keyboard shortcuts

| Key | Action|
|-----|-------|
| `w`, `s` | Pan Up, Down |
| `a`, `d` | Pan Left, Right |
| `+`, `-` | Zoom in / out |
| `W`, `S` | Move in / out |
| `1`, `2`, `3` | Toggle Lights One, Two Or Three respectively. Light One is enabled by default. |
| `SPACE` | Toggle autorotation |
| `,` , `.` | Increase rotation speed towards left / right |


### Compile time configuration
* VBOs (default = enabled)
* Antialiasing (default = enabled)
* FPS counter (default = disabled)

### Other similar software
Just my luck! After implementing this project, I found other similar projects.

* [Mesh Viewer](http://mview.sourceforge.net/)
* [Obj Loader](http://sourceforge.net/projects/objloader/)
* [3d object viewer](https://code.google.com/p/3d-object-viewer/downloads/list)
* [obj-loader](https://github.com/roneygomes/obj-loader)
