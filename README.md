# DVD Screensaver Simulation
_____________________________________________
The inspiration for this project is to replicate the frustration of watching
the bouncing DVD logo screensaver barely miss hitting the corner perfectly
just like that one scene from The Office.

### Project Features
- A bouncing rectangle to model the DVD logo
- Color change on the left click of a mouse
- Velocity change using the arrow keys
- Statistics tracking (walls hit and corners hit)
- A surprise when the rectangle finally hits a corner!
_____________________________________________
### Project Libraries
This project utilizes the following libraries:
- freetype
- glad
- glfw
- glm
- stb
_____________________________________________
#### Project contributions authored by myself
- In engine.cpp
  - initshapes()
  - processInput()
  - checkBounds()
  - checkConfettiBounds()
  - update()
  - render()
  - spawnConfetti()

Aside from small implementation updates in other classes, the remainder of the
code was generously contributed by Professor Lisa Dion in the form of starter 
code for other projects in the course.

