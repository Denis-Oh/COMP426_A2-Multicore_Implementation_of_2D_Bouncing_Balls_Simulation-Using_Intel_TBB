#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <mutex>

struct Ball {
    float x, y;          // Position
    float vx, vy;        // Velocity
    float radius;        // Radius
    float r, g, b;       // Color (RGB)
    std::thread thread;  // Thread to handle ball movement
    bool running;        // Flag to indicate if the thread should keep running
};

// Vector to store multiple balls
std::vector<Ball> balls;
std::mutex balls_mutex;  // Mutex to protect shared access to balls' positions

// Function to handle ball collision response
void handleCollision(Ball &ball1, Ball &ball2) {
    // Calculate the distance between the centers of the two balls
    float dx = ball2.x - ball1.x;
    float dy = ball2.y - ball1.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    // Check if the balls are colliding
    if (distance < ball1.radius + ball2.radius) {
        // Normalize the collision vector
        float nx = dx / distance;
        float ny = dy / distance;

        // Calculate relative velocity
        float vx_relative = ball2.vx - ball1.vx;
        float vy_relative = ball2.vy - ball1.vy;

        // Calculate the velocity along the normal direction
        float velocity_along_normal = vx_relative * nx + vy_relative * ny;

        // If the balls are moving apart, don't calculate the collision
        if (velocity_along_normal > 0) return;

        // Adjust the velocities to simulate the collision
        float bounce = 1.0f; // bounciness (max 1.0 = no energy lost in collision)

        float impulse = (-(1 + bounce) * velocity_along_normal) / 2.0f;

        // Apply impulse to both balls
        ball1.vx -= impulse * nx;
        ball1.vy -= impulse * ny;
        ball2.vx += impulse * nx;
        ball2.vy += impulse * ny;
    }
}

// Gravity constant
const float gravity = -0.03f;

// Updated function to update ball position and handle collisions
void updateBall(Ball &ball) {
    while (ball.running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(33)); // milliseconds(33) ~= 30 FPS, milliseconds(16) ~= 60 FPS

        // Lock the mutex to ensure thread-safe access
        {
            std::lock_guard<std::mutex> lock(balls_mutex);

            // Apply gravity to the ball's y-velocity
            ball.vy += gravity;

            // Update ball position
            ball.x += ball.vx;
            ball.y += ball.vy;

            // Bounce off the window boundaries
            if (ball.x - ball.radius < 0) {
                ball.x = ball.radius;   // Prevent overlap
                ball.vx = -ball.vx;     // Reflect x-velocity
            }
            if (ball.x + ball.radius > 800) {
                ball.x = 800 - ball.radius; // Prevent overlap
                ball.vx = -ball.vx;         // Reflect x-velocity
            }
            if (ball.y - ball.radius < 0) {
                ball.y = ball.radius;   // Prevent overlap
                ball.vy = -ball.vy;     // Reflect y-velocity
            }
            if (ball.y + ball.radius > 600) {
                ball.y = 600 - ball.radius; // Prevent overlap
                ball.vy = -ball.vy;         // Reflect y-velocity
            }

            // Check for collisions with other balls
            for (auto& other : balls) {
                if (&ball != &other) {
                    handleCollision(ball, other);
                }
            }
        }
    }
}

// Function to initialize balls and start threads
void initializeBalls(int numBalls) {
    srand(static_cast<unsigned int>(time(0)));
    for (int i = 0; i < numBalls; ++i) {
        Ball ball;
        ball.x = static_cast<float>(rand() % 800);
        ball.y = static_cast<float>(rand() % 600);
        ball.vx = static_cast<float>((rand() % 200) - 100) / 100.0f; // Random velocity between -1.00 and 1.00
        ball.vy = static_cast<float>((rand() % 200) - 100) / 100.0f;
        ball.radius = static_cast<float>((rand() % 3 + 1) * 25); // Random radius (25, 50, or 75)

        // Assign random colors (red, green, or blue)
        if (i % 3 == 0) {
            ball.r = 1.0f; ball.g = 0.0f; ball.b = 0.0f; // Red
        } else if (i % 3 == 1) {
            ball.r = 0.0f; ball.g = 1.0f; ball.b = 0.0f; // Green
        } else {
            ball.r = 0.0f; ball.g = 0.0f; ball.b = 1.0f; // Blue
        }

        // Set the running flag to true
        ball.running = true;

        balls.push_back(std::move(ball));
    }

    // Start a thread for each ball after adding them to the vector
    for (auto &ball : balls) {
        ball.thread = std::thread(updateBall, std::ref(ball));
    }
}

// Function to clean up and stop all ball threads
void cleanupBalls() {
    for (auto& ball : balls) {
        ball.running = false;
        if (ball.thread.joinable()) {
            ball.thread.join();
        }
    }
}

// Function to draw a ball
void drawBall(const Ball& ball) {
    glColor3f(ball.r, ball.g, ball.b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(ball.x, ball.y); // Define center of the ball

    // Draw the ball as a circle
    for (int i = 0; i <= 360; ++i) {
        float angle = i * 3.14159f / 180.0f; // convert i to radians
        glVertex2f(ball.x + cos(angle) * ball.radius, ball.y + sin(angle) * ball.radius); // Define all points around ball's edge
    }
    glEnd();
}

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(800, 600, "Multithreaded 2D Bouncing Balls Simulation", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Set up the OpenGL viewport and projection
    glViewport(0, 0, 800, 600);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); // reset the current matrix to the identity matrix
    glOrtho(0, 800, 0, 600, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Set the background color to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 

    // Initialize balls
    initializeBalls(5); // Start with 5 balls

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Render
        glClear(GL_COLOR_BUFFER_BIT);

        // Lock mutex before drawing to ensure thread-safe access
        {
            std::lock_guard<std::mutex> lock(balls_mutex);
            // Draw all balls
            for (const auto& ball : balls) {
                drawBall(ball);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up threads
    cleanupBalls();

    glfwTerminate();
    return 0;
}
