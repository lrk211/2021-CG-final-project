#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "SkyBox.h"

#include <iostream>

#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

// camera
Camera camera(glm::vec3(0.0f, 3.0f, -1.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 modelPositions[]=
{
  glm::vec3(0.0f, 0.0f, 0.0f),
  glm::vec3(0.0f, 1.0f, 0.0f)
};

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

GLfloat skyBoxVertex[] = {
  -100.0f,  100.0f, -100.0f,
  -100.0f, -100.0f, -100.0f,
  100.0f, -100.0f, -100.0f,
  100.0f, -100.0f, -100.0f,
         100.0f,  100.0f, -100.0f,
        -100.0f,  100.0f, -100.0f,
  
        -100.0f, -100.0f,  100.0f,
        -100.0f, -100.0f, -100.0f,
        -100.0f,  100.0f, -100.0f,
        -100.0f,  100.0f, -100.0f,
        -100.0f,  100.0f,  100.0f,
        -100.0f, -100.0f,  100.0f,
  
         100.0f, -100.0f, -100.0f,
         100.0f, -100.0f,  100.0f,
         100.0f,  100.0f,  100.0f,
         100.0f,  100.0f,  100.0f,
         100.0f,  100.0f, -100.0f,
         100.0f, -100.0f, -100.0f,
   
        -100.0f, -100.0f,  100.0f,
        -100.0f,  100.0f,  100.0f,
         100.0f,  100.0f,  100.0f,
         100.0f,  100.0f,  100.0f,
         100.0f, -100.0f,  100.0f,
        -100.0f, -100.0f,  100.0f,
  
        -100.0f,  100.0f, -100.0f,
         100.0f,  100.0f, -100.0f,
         100.0f,  100.0f,  100.0f,
         100.0f,  100.0f,  100.0f,
        -100.0f,  100.0f,  100.0f,
        -100.0f,  100.0f, -100.0f,
  
        -100.0f, -100.0f, -100.0f,
        -100.0f, -100.0f,  100.0f,
         100.0f, -100.0f, -100.0f,
         100.0f, -100.0f, -100.0f,
        -100.0f, -100.0f,  100.0f,
         100.0f, -100.0f,  100.0f
};

int main()
{
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // glfw window creation
  // --------------------
  GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "cg-project", NULL, NULL);
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
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK)
  {
    std::cout << "Failed to initialize GLEW" << std::endl;
    return -1;
  }

  // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
  stbi_set_flip_vertically_on_load(true);

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);

  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

  // build and compile shaders
  // -------------------------
  //Shader humanShader("shader/model_loading.vs", "shader/model_loading.fs");
  Shader humanShader("shader/robot.vs", "shader/robot.fs");
  Shader skyShader("shader/skybox.vs", "shader/skybox.fs");
  // load models
  // -----------
  Model ourModel("resource/ironman/IronMan.obj");

  Model houseModel("texture/house.obj");

  // Model ironManModel("resource/ironman/IronMan.obj");

  // draw in wireframe
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // render loop
  // -----------

  //  天空盒
  GLuint skyBoxVAO, skyBoxVBO;
  glGenVertexArrays(1, &skyBoxVAO);
  glGenBuffers(1, &skyBoxVBO);
  glBindVertexArray(skyBoxVAO);
  glBindBuffer(GL_ARRAY_BUFFER, skyBoxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyBoxVertex), skyBoxVertex, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

  vector<const GLchar*> faces;
  faces.push_back("resource/sky/right.jpg");
  faces.push_back("resource/sky/left.jpg");
  faces.push_back("resource/sky/top.jpg");
  faces.push_back("resource/sky/bottom.jpg");
  faces.push_back("resource/sky/back.jpg");
  faces.push_back("resource/sky/front.jpg");
  GLuint skyBoxTexture = loadCubeMap(faces);
  while (!glfwWindowShouldClose(window))
  {
    // per-frame time logic
    // --------------------
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // input
    // -----
    processInput(window);

    // render
    // ------
    glClearColor(0.12f, 0.63f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    
    skyShader.use();
    glm::mat4 projection1 = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);
    glm::mat4 view1 = camera.GetViewMatrix();
    skyShader.setMat4("projection", projection1);
    skyShader.setMat4("view", view1);

    glBindVertexArray(skyBoxVAO);
    glActiveTexture(GL_TEXTURE0);
    skyShader.setInt("skyBox", 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);

    // don't forget to enable shader before setting uniforms
    humanShader.use();

    humanShader.setVec3("light.position", lightPos);
    humanShader.setVec3("viewPos", camera.Position);

    //light properties
    humanShader.setVec3("light.ambient", 1.0f, 1.0f, 1.0f);
    humanShader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
    humanShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    //ourShader.setInt("texture_diffuse1", 0);
    //ourShader.setInt("texture_specular1", 1);

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    humanShader.setVec3("cameraPostion", camera.Position);
    glm::mat4 view = camera.GetViewMatrix();
    humanShader.setMat4("projection", projection);
    humanShader.setMat4("view", view);

    // render the loaded model
    glm::mat4 model1 = glm::mat4(1.0f);
    model1 = glm::translate(model1, modelPositions[0]); // translate it down so it's at the center of the scene
    
    // model1 = glm::rotate(model1, theta, camera.Up);
    model1 = glm::scale(model1, glm::vec3(0.01f, 0.01f, 0.01f));     // it's a bit too big for our scene, so scale it down
    humanShader.setMat4("model", model1);
    ourModel.Draw(humanShader);

    glm::mat4 housemodel = glm::mat4(1.0f);
    housemodel = glm::translate(housemodel, glm::vec3(0.0f, -1.0f, 1.0f));
    housemodel = glm::scale(housemodel, glm::vec3(0.01f, 0.01f, 0.01f));
    humanShader.setMat4("model", housemodel);
    houseModel.Draw(humanShader);
    /*
    glm::mat4 model2 = glm::mat4(1.0f);
    model2 = glm::translate(model2, modelPositions[1]); // translate it down so it's at the center of the scene
    model2 = glm::scale(model2, glm::vec3(0.1f, 0.1f, 0.1f));     // it's a bit too big for our scene, so scale it down
    humanShader.setMat4("model", model2);
    ourModel.Draw(ourShader);

    glm::mat4 housemodel = glm::mat4(1.0f);
    housemodel = glm::translate(housemodel, glm::vec3(0.0f, 2.0f, 1.0f));
    housemodel = glm::scale(housemodel, glm::vec3(0.01f, 0.01f, 0.01f));
    ourShader.setMat4("model", housemodel);
    chairModel.Draw(houseShader);
    */
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
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

  modelPositions[0] = camera.Position + camera.Front + camera.Front + camera.Front + camera.Front - camera.Up;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
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
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll(yoffset);
}