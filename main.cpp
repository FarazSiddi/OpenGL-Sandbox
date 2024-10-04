#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "shader_m.h"
#include "camera.h"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "filesystem.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

bool guiMode = false; // Start in simulation mode

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

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("vertex.vert", "fragment.frag");

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
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load(FileSystem::getPath("resources/textures/dirt.png").c_str(), &width, &height, &nrChannels, 0);
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

    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load(FileSystem::getPath("resources/textures/grass.jpg").c_str(), &width, &height, &nrChannels, 0);
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

    // texture 3
    // ---------
    glGenTextures(1, &texture3);
    glBindTexture(GL_TEXTURE_2D, texture3);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load(FileSystem::getPath("resources/textures/tree.jpg").c_str(), &width, &height, &nrChannels, 0);
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

    // texture 4
    // ---------
    glGenTextures(1, &texture4);
    glBindTexture(GL_TEXTURE_2D, texture4);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load(FileSystem::getPath("resources/textures/leaf.jpg").c_str(), &width, &height, &nrChannels, 0);
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

    // texture 5
    // ---------
    glGenTextures(1, &texture5);
    glBindTexture(GL_TEXTURE_2D, texture5);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load(FileSystem::getPath("resources/textures/snow.jpg").c_str(), &width, &height, &nrChannels, 0);
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

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use();
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);
	ourShader.setInt("texture3", 2);
	ourShader.setInt("texture4", 3);
    ourShader.setInt("texture5", 4);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable 
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad 

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Example: Show a simple window
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
        ImGui::End();

        // Check mode and process rendering logic accordingly
        if (guiMode)
        {
            // Show cursor when in GUI mode
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

            // ImGui renders on top of OpenGL content
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
        else
        {
            // Hide cursor when in simulation mode
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        }

        // Perform camera movement and render the OpenGL scene
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        //glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClearColor(0.2f, 0.6f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render your OpenGL scene here...
        // Use camera for movement and scene rendering

        // activate shader and render your objects
        ourShader.use();

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

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
            ourShader.setMat4("model", model);

            glBindTexture(GL_TEXTURE_2D, texture1);
            glDrawArrays(GL_TRIANGLES, 0, 18);
            glBindTexture(GL_TEXTURE_2D, texture2);
            glDrawArrays(GL_TRIANGLES, 18, 18);
            glBindTexture(GL_TEXTURE_2D, texture3);
            glDrawArrays(GL_TRIANGLES, 36, 30);
            glBindTexture(GL_TEXTURE_2D, texture4);
            glDrawArrays(GL_TRIANGLES, 66, 18);
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

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    static bool tabPressedLastFrame = false;  // Keeps track of the tab key state

    // Check if Tab is pressed and not previously held down (to toggle mode)
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !tabPressedLastFrame)
    {
        guiMode = !guiMode;  // Toggle the GUI mode
        tabPressedLastFrame = true;  // Mark the tab as pressed this frame
    }

    // Reset tabPressedLastFrame when the Tab key is released
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
    {
        tabPressedLastFrame = false;  // Allow detection of new key press
    }

    // If we are in simulation mode, handle camera movements
    if (!guiMode)
    {
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

    // In GUI mode, let ImGui capture inputs
    if (guiMode)
    {
        ImGui::GetIO().WantCaptureKeyboard = true;
        ImGui::GetIO().WantCaptureMouse = true;
    }
    else
    {
        ImGui::GetIO().WantCaptureKeyboard = false;
        ImGui::GetIO().WantCaptureMouse = false;
    }
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
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}