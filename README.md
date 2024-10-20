# COMP-426 A1: Multicore Implementation of 2D Bouncing Balls Simulation Using Multiple Threads

Name: Denis Oh

Student ID: 40208580

![BouncingBallsDemo](https://github.com/user-attachments/assets/7398048c-47db-49b0-816c-f88c7a9f2b1f)

#### How to run:
- `cd build`
- `cmake ..`
- `make`
- `./BouncingBalls`

## Introduction:
This 2D bouncing balls simulation uses multiple threads to leverage multicore processing. The simulation consists of several coloured balls moving and bouncing within a window, affected by gravity and interacting with each other upon collision. Each ball runs on a separate thread, and the simulation was designed to achieve smooth animation and efficient performance.

## Design and Optimization:

### Multithreaded Design:
Computation Threads: Each ball in the simulation is managed by its own thread. This design choice allows each ball to update its position, velocity, and handle collisions independently of the others, taking full advantage of multicore processing. The std::thread library was used to create these threads.

Control Thread: The main thread in the main function acts as the control thread. It is responsible for initializing the simulation, rendering the balls on the screen using OpenGL, and cleaning up the threads when the program exits.

### Gravity and Motion:
A constant gravitational force is applied to each ball's vertical velocity (vy) during each update, simulating the effect of gravity pulling the balls downward.

Each ball's position (x, y) is updated based on its current velocity (vx, vy) to reflect its movement across the screen.

### Handling Collisions:
Wall Collisions: The simulation detects when a ball reaches the window's boundaries and reflects its velocity to simulate bouncing.

Ball-to-Ball Collisions: When two balls collide, their x and y velocities are re-calculated. The change in velocity is calculated based on the relative velocity and the collision vector between the two balls.

### Rendering with OpenGL:
The balls on screen are rendered with the OpenGL library. The drawBall function draws each ball as a fan of triangles with OpenGL's GL_TRIANGLE_FAN. The result resembles a circle.

### Mutex Locking: 
To avoid data races when multiple threads access shared resources (e.g., the balls vector), a std::mutex (balls_mutex) is used to protect access to shared data. This ensures thread-safe updates of ball positions and collision handling.

