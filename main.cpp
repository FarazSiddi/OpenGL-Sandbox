#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// IMGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Helper functions
#include "shader_m.h"
#include "camera.h"
#include "filesystem.h"

// Polyhedrons
#include "cube.h"
#include "sphere.h"
#include "pyramid.h"
#include "cylinder.h"

// Built-in libraries
#include <iostream>
#include <vector>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// STB Image
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void loadTexture(unsigned int& textureName, const std::string& path);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

// camera
Camera camera(glm::vec3(0.0f, 3.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

bool guiMode = false; // Start in simulation mode

// default values
float bgColor[3] = { 0.2f, 0.6f, 0.8f }; // Default background color

// GUI variables
bool showGlobalSettings = false;

// Baseplate settings
bool showBaseplate = false;                  
float baseplateSize = 125.0f;               // Default size (250x250)
// A 1x1 square centered at (0,0):
float baseplateVertices[] = {
    -0.5f, 0.0f, -0.5f,  // Bottom-left
     0.5f, 0.0f, -0.5f,  // Bottom-right
     0.5f, 0.0f,  0.5f,  // Top-right
    -0.5f, 0.0f,  0.5f   // Top-left
};

float baseplateColor[3] = { 0.1f, 0.5f, 0.1f }; // Default color (green)
glm::vec3 baseplatePosition(0.0f, 0.0f, 0.0f); // Default position (origin)

// A container of shape pointers
std::vector<BaseShape*> g_Shapes;


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();

    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Demo", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // build and compile our shader program
    // ------------------------------------
    Shader mainShader("vertex.vert", "fragment.frag");
	Shader baseplateShader("baseplate.vert", "baseplate.frag");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // texture coords
        // ------------------------------------------------------------------
        // bottom
        -1.73205f, 1.0f, 1.0f,  0.0f, 0.0f,
         0.0f, -2.0f, 0.0f,  1.0f, 0.0f,
         0.0f, 1.0f, 2.0f,  1.0f, 1.0f,

         0.0f, 1.0f, 2.0f,  0.0f, 0.0f,
         0.0f, -2.0f, 0.0f,  1.0f, 0.0f,
         1.73205f,  1.0f, 1.0f,  1.0f, 1.0f,

        -1.73205f, 1.0f, 1.0f,  0.0f, 0.0f,
         0.0f, -2.0f, 0.0f,  1.0f, 0.0f,
        -1.73205f, 1.0f, -1.0f,  1.0f, 1.0f,

        1.73205f, 1.0f, 1.0f,  0.0f, 0.0f,
         0.0f, -2.0f, 0.0f,  1.0f, 0.0f,
        1.73205f, 1.0f, -1.0f,  1.0f, 1.0f,

        -1.73205f, 1.0f, -1.0f,  0.0f, 0.0f,
         0.0f, -2.0f, 0.0f,  1.0f, 0.0f,
         0.0f, 1.0f, -2.0f,  1.0f, 1.0f,

        1.73205f, 1.0f, -1.0f,  0.0f, 0.0f,
         0.0f, -2.0f, 0.0f,  1.0f, 0.0f,
         0.0f, 1.0f, -2.0f,  1.0f, 1.0f,

         // top
         -1.73205f, 1.0f, 1.0f,  0.0f, 0.0f,
         0.0f, 1.0f, 2.0f,  1.0f, 0.0f,
         0.0f, 1.0f, 0.0f,  1.0f, 1.0f,

         0.0f, 1.0f, 2.0f,  0.0f, 0.0f,
         1.73205f,  1.0f, 1.0f,  1.0f, 0.0f,
         0.0f, 1.0f, 0.0f,  1.0f, 1.0f,

         -1.73205f, 1.0f, 1.0f,  0.0f, 0.0f,
        -1.73205f, 1.0f, -1.0f,  1.0f, 0.0f,
         0.0f, 1.0f, 0.0f,  1.0f, 1.0f,

         1.73205f, 1.0f, 1.0f,  0.0f, 0.0f,
         1.73205f, 1.0f, -1.0f,  1.0f, 0.0f,
         0.0f, 1.0f, 0.0f,  1.0f, 1.0f,

         -1.73205f, 1.0f, -1.0f,  0.0f, 0.0f,
         0.0f, 1.0f, -2.0f,  1.0f, 0.0f,
         0.0f, 1.0f, 0.0f,  1.0f, 1.0f,

         1.73205f, 1.0f, -1.0f,  0.0f, 0.0f,
         0.0f, 1.0f, -2.0f,  1.0f, 0.0f,
         0.0f, 1.0f, 0.0f,  1.0f, 1.0f,

         // tree base
		-0.1f, 1.0f, -0.1f,  0.0f, 0.0f,
        0.1f, 1.0f, -0.1f,  1.0f, 0.0f,
        0.1f, 2.0f, -0.1f,  1.0f, 1.0f,
        0.1f, 2.0f, -0.1f,  0.0f, 0.0f,
		-0.1f, 2.0f, -0.1f,  1.0f, 0.0f,
		-0.1f, 1.0f, -0.1f,  1.0f, 1.0f,

        0.1f, 1.0f, -0.1f,  0.0f, 0.0f,
        0.1f, 1.0f, 0.1f,  1.0f, 0.0f,
        0.1f, 2.0f, 0.1f,  1.0f, 1.0f,
        0.1f, 2.0f, 0.1f,  0.0f, 0.0f,
        0.1f, 2.0f, -0.1f,  1.0f, 0.0f,
        0.1f, 1.0f, -0.1f,  1.0f, 1.0f,

        0.1f, 1.0f, 0.1f,  0.0f, 0.0f,
		-0.1f, 1.0f, 0.1f,  1.0f, 0.0f,
		-0.1f, 2.0f, 0.1f,  1.0f, 1.0f,
		-0.1f, 2.0f, 0.1f,  0.0f, 0.0f,
        0.1f, 2.0f, 0.1f,  1.0f, 0.0f,
        0.1f, 1.0f, 0.1f,  1.0f, 1.0f,

		-0.1f, 1.0f, 0.1f,  0.0f, 0.0f,
		-0.1f, 1.0f, -0.1f,  1.0f, 0.0f,
		-0.1f, 2.0f, -0.1f,  1.0f, 1.0f,
		-0.1f, 2.0f, -0.1f,  0.0f, 0.0f,
		-0.1f, 2.0f, 0.1f,  1.0f, 0.0f,
		-0.1f, 1.0f, 0.1f,  1.0f, 1.0f,

		-0.1f, 2.0f, -0.1f,  0.0f, 0.0f,
		0.1f, 2.0f, -0.1f,  1.0f, 0.0f,
		0.1f, 2.0f, 0.1f,  1.0f, 1.0f,
		0.1f, 2.0f, 0.1f,  0.0f, 0.0f,
		-0.1f, 2.0f, 0.1f,  1.0f, 0.0f,
		-0.1f, 2.0f, -0.1f,  1.0f, 1.0f,

         // tree leaves
		-0.5f, 2.0f, -0.5f,  0.0f, 0.0f,
		0.0f, 3.5f, 0.0f,  1.0f, 0.0f,
		0.5f, 2.0f, -0.5f,  1.0f, 1.0f,

		0.5f, 2.0f, -0.5f,  0.0f, 0.0f,
		0.0f, 3.5f, 0.0f,  1.0f, 0.0f,
		0.5f, 2.0f, 0.5f,  1.0f, 1.0f,

		0.5f, 2.0f, 0.5f,  0.0f, 0.0f,
		0.0f, 3.5f, 0.0f,  1.0f, 0.0f,
		-0.5f, 2.0f, 0.5f,  1.0f, 1.0f,

		-0.5f, 2.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 3.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, 2.0f, -0.5f, 1.0f, 1.0f,

		-0.5f, 2.0f, -0.5f, 0.0f, 0.0f,
        0.5f, 2.0f, -0.5f, 1.0f, 1.0f,
		0.5f, 2.0f, 0.5f, 1.0f, 0.0f,
		0.5f, 2.0f, 0.5f, 0.0f, 0.0f,
		-0.5f, 2.0f, 0.5f, 1.0f, 0.0f,
		-0.5f, 2.0f, -0.5f, 1.0f, 1.0f

    };
    // world space positions of our cubes
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  -3.0f,  -3.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, 12.2f, -2.5f),
        glm::vec3(-6.8f, -2.0f, -12.3f),
        glm::vec3(20.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, 7.5f),
        glm::vec3(1.3f, -2.4f, 5.5f),
        glm::vec3(-11.5f,  2.0f, 2.5f),
        glm::vec3(7.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f, -17.0f, -1.5f)
    };
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // load and create a texture 
    // -------------------------
    unsigned int texture1, texture2, texture3, texture4, texture5;
    std::string texturePath[5] = { "resources/textures/dirt.png", "resources/textures/grass.jpg", "resources/textures/tree.jpg", "resources/textures/leaf.jpg", "resources/textures/snow.jpg" };
    loadTexture(texture1, texturePath[0]);
    loadTexture(texture2, texturePath[1]);
    loadTexture(texture3, texturePath[2]);
    loadTexture(texture4, texturePath[3]);
    loadTexture(texture5, texturePath[4]);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    mainShader.use();
    mainShader.setInt("texture1", 0);
    mainShader.setInt("texture2", 1);
	mainShader.setInt("texture3", 2);
	mainShader.setInt("texture4", 3);
    mainShader.setInt("texture5", 4);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable keyboard
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad 

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

		glm::vec3 cameraPosition = camera.Position;

        processInput(window);

        glfwPollEvents();
        glfwSetInputMode(window, GLFW_CURSOR, guiMode ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

        glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        if (showBaseplate)
        {
            unsigned int baseplateIndices[] = {
                0, 1, 2, // First triangle
                2, 3, 0  // Second triangle
            };

            // Static VAO/VBO/EBO
            static unsigned int baseplateVAO = 0, baseplateVBO, baseplateEBO;
            if (baseplateVAO == 0)
            {
                glGenVertexArrays(1, &baseplateVAO);
                glGenBuffers(1, &baseplateVBO);
                glGenBuffers(1, &baseplateEBO);

                glBindVertexArray(baseplateVAO);

                glBindBuffer(GL_ARRAY_BUFFER, baseplateVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(baseplateVertices), baseplateVertices, GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, baseplateEBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(baseplateIndices), baseplateIndices, GL_STATIC_DRAW);

                // Position attribute
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);
            }

            // Render the baseplate
            baseplateShader.use();

            // Pass the camera matrices
            glm::mat4 view = camera.GetViewMatrix();
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
            baseplateShader.setMat4("view", view);
            baseplateShader.setMat4("projection", projection);

            // ---------- Build/translate the model matrix ----------
            // 1. Build the base (translation) part:
            glm::mat4 baseplateModel = glm::mat4(1.0f);
            baseplateModel = glm::translate(baseplateModel, baseplatePosition);

            // 2. Scale in X and Z by baseplateSize
            baseplateModel = glm::scale(baseplateModel, glm::vec3(baseplateSize, 1.0f, baseplateSize));

            baseplateShader.setMat4("model", baseplateModel);


            // Set the baseplate color from the GUI color palette
            baseplateShader.setVec3("baseplateColor", glm::vec3(baseplateColor[0], baseplateColor[1], baseplateColor[2]));

            // Bind VAO and draw
            glBindVertexArray(baseplateVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        // Before starting ImGui's new frame in the main loop
        ImGui::GetIO().WantCaptureMouse = guiMode;
        ImGui::GetIO().WantCaptureKeyboard = guiMode;

        //std::cout << "WantCaptureMouse: " << ImGui::GetIO().WantCaptureMouse
        //    << ", WantCaptureKeyboard: " << ImGui::GetIO().WantCaptureKeyboard << std::endl;


        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Display the main window
        ImGui::SetNextWindowSize(ImVec2(400, 150), ImGuiCond_FirstUseEver);
        ImGui::Begin("Mode Toggle");
        ImGui::Text("Press Tab to toggle between GUI and Simulation");
        if (guiMode)
        {
            ImGui::Text("Mode: GUI");
        }
        else
        {
            ImGui::Text("Mode: Simulation");
        }

        // Add camera coordinates
        ImGui::Separator(); // Visual separator
        ImGui::Text("Camera Coordinates: X: %.2f, Y: %.2f, Z: %.2f",
            cameraPosition.x, cameraPosition.y, cameraPosition.z);
        ImGui::Separator();

        if (ImGui::Button(showGlobalSettings ? "Hide Global Settings" : "Show Global Settings"))
        {
            showGlobalSettings = !showGlobalSettings;
        }
        ImGui::End();

        // Show Global Settings Window if toggled on
        if (showGlobalSettings)
        {
            ImGui::SetNextWindowSize(ImVec2(350, 300), ImGuiCond_FirstUseEver);
            ImGui::Begin("Global Settings");

            // Background color settings
            ImGui::Text("Adjust Background Color");
            ImGui::ColorEdit3("Background Color", bgColor);

            // Baseplate settings
            ImGui::Separator();
            ImGui::Text("Baseplate Settings");
            ImGui::Checkbox("Show Baseplate", &showBaseplate);
            ImGui::DragFloat("Baseplate Size", &baseplateSize, 1.0f, 1.0f, 1000.0f, "%.1f");
            ImGui::ColorEdit3("Baseplate Color", baseplateColor);
            ImGui::DragFloat3("Baseplate Position", &baseplatePosition[0], 1.0f, -500.0f, 500.0f, "%.1f");

            ImGui::End();
        }


        // Perform camera movement and render the OpenGL scene
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);


        // Render your OpenGL scene here...
        // Use camera for movement and scene rendering

        // Activate shader and render the objects
        mainShader.use();

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        mainShader.setMat4("projection", projection);

        glm::mat4 view = camera.GetViewMatrix();
        mainShader.setMat4("view", view);

        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < sizeof(cubePositions) / sizeof(cubePositions[0]); i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);

            float angle = 20.0f * i + glfwGetTime() * 12.5f;  // Continuous rotation
            if (i == 0) {
                model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate first object on Y-axis
            }
            else {
                model = glm::rotate(model, glm::radians(angle), glm::vec3(10.0f, 20.0f, 5.0f)); // Rotate other objects
            }
            mainShader.setMat4("model", model);

            glBindTexture(GL_TEXTURE_2D, texture1);
            glDrawArrays(GL_TRIANGLES, 0, 18);
            glBindTexture(GL_TEXTURE_2D, texture2);
            glDrawArrays(GL_TRIANGLES, 18, 18);
            glBindTexture(GL_TEXTURE_2D, texture3);
            glDrawArrays(GL_TRIANGLES, 36, 30);
            glBindTexture(GL_TEXTURE_2D, texture4);
            glDrawArrays(GL_TRIANGLES, 66, 18);
        }

        // Shape-specific shaders go here:

        for (auto shape : g_Shapes)
        {
            // You might choose to pass the ID of the shader for shapes
            // or each shape might store its own shader handle
            shape->draw(view, projection, mainShader.ID);
        }

        // Render ImGui UI after OpenGL scene
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers
        glfwSwapBuffers(window);
        
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

	// Delete all shapes upon exit
    for (auto shape : g_Shapes)
        delete shape;
    g_Shapes.clear();


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

bool tabPressedLastFrame = false; // To track Tab key state

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    //std::cout << "GUI Mode: " << guiMode
    //<< ", WantCaptureMouse: " << ImGui::GetIO().WantCaptureMouse
    //<< ", WantCaptureKeyboard: " << ImGui::GetIO().WantCaptureKeyboard << std::endl;

	static bool tabPressedLastFrame = false; // To track Tab key state
    static bool key1PressedLastFrame = false;
	static bool key2PressedLastFrame = false;
    static bool key3PressedLastFrame = false;
    static bool key4PressedLastFrame = false;

    // Check if Tab was pressed
    bool tabPressed = glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS;
    if (tabPressed && !tabPressedLastFrame)
    {
        guiMode = !guiMode;

        // Update cursor mode
        glfwSetInputMode(window, GLFW_CURSOR, guiMode ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }
    tabPressedLastFrame = tabPressed;

    // Suppose user presses numeric keys to spawn shapes
    bool key1IsPressed = (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS);
    if (key1IsPressed && !key1PressedLastFrame)
    {
        // Create a new Cube
        Cube* newCube = new Cube();
        newCube->init(); // sets up VAO

        // Distance in front of the camera to place the cube
        float spawnDistance = 2.0f;
        glm::vec3 spawnPos = camera.Position + camera.Front * spawnDistance;

        // Assign that position to the new cube
        newCube->position = spawnPos;
        newCube->scale = glm::vec3(1.0f);

        // Finally, add it to the vector so it�s drawn each frame
        g_Shapes.push_back(newCube);
    }
    key1PressedLastFrame = key1IsPressed;

	bool key2IsPressed = (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS);
    if (key2IsPressed && !key2PressedLastFrame)
    {
        // e.g. create a sphere with 16 slices, 16 stacks
        Sphere* newSphere = new Sphere(16, 16);
        newSphere->init();

        // Put it 2 units in front of the camera
        float spawnDistance = 2.0f;
        glm::vec3 spawnPos = camera.Position + camera.Front * spawnDistance;
        newSphere->position = spawnPos;
        newSphere->scale = glm::vec3(1.0f);

        g_Shapes.push_back(newSphere);
    }
    key2PressedLastFrame = key2IsPressed;

    bool key3IsPressed = (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS);
    if (key3IsPressed && !key3PressedLastFrame)
    {
        Pyramid* newPyramid = new Pyramid();
        newPyramid->init();

        // Place 2 units in front of the camera
        float spawnDistance = 2.0f;
        glm::vec3 spawnPos = camera.Position + camera.Front * spawnDistance;
        newPyramid->position = spawnPos;
        newPyramid->scale = glm::vec3(1.0f);

        g_Shapes.push_back(newPyramid);
    }
    key3PressedLastFrame = key3IsPressed;

    bool key4IsPressed = (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS);
    if (key4IsPressed && !key4PressedLastFrame)
    {
        // Create a new Cylinder with 16 slices, radius=0.5, height=1.0
        Cylinder* newCyl = new Cylinder(16, 0.5f, 1.0f);
        newCyl->init();

        // Place it in front of the camera
        float spawnDistance = 2.0f;
        glm::vec3 spawnPos = camera.Position + camera.Front * spawnDistance;
        newCyl->position = spawnPos;
        newCyl->scale = glm::vec3(1.0f);

        g_Shapes.push_back(newCyl);
    }
    key4PressedLastFrame = key4IsPressed;

    //else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    //{
    //    // create sphere, etc.
    //}


    // Handle input based on current mode
    if (guiMode)
    {
        // ImGui captures input
        ImGui::GetIO().WantCaptureMouse = true;
        ImGui::GetIO().WantCaptureKeyboard = true;
    }
    else
    {
        // GLFW should handle input in simulation mode
        ImGui::GetIO().WantCaptureMouse = false;
        ImGui::GetIO().WantCaptureKeyboard = false;

        // GLFW keyboard controls for simulation
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    
    if (!guiMode)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            //std::cout << "W key pressed for FORWARD movement\n";
            camera.ProcessKeyboard(FORWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            //std::cout << "S key pressed for BACKWARD movement\n";
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            //std::cout << "A key pressed for LEFT movement\n";
            camera.ProcessKeyboard(LEFT, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            //std::cout << "D key pressed for RIGHT movement\n";
            camera.ProcessKeyboard(RIGHT, deltaTime);
        }
    }

}

void loadTexture(unsigned int& textureName, const std::string& path)
{
    glGenTextures(1, &textureName);
    glBindTexture(GL_TEXTURE_2D, textureName);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load(FileSystem::getPath(path).c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (!guiMode) // Only process mouse input in simulation mode
    {
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}