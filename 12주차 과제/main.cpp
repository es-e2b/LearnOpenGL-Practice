#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void initSphere(float**, int*, int*); // 구 만들기
void initSaturnRing(float** vertices, int* nVert, int* nAttr); // 토성 고리
class Sphere
{
public:
    float rts;
    float rvs;
    glm::vec3 distance;
    glm::vec3 scale;
    std::string texturePath;
    void texturing(std::string texturePath)
    { // 텍스쳐링
        // sphere VAO and VBO
        float* Verts = nullptr;
        initSphere(&Verts, &nVert, &nAttr); // 구 만들기 **
        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, nVert * nAttr * sizeof(float), Verts, GL_STATIC_DRAW);
        
        glBindVertexArray(VAO);
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, nAttr * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, nAttr * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // texCoord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, nAttr * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
        
        free(Verts);
        
        // 텍스처 로드
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        // 텍스처 이미지 로드 및 설정
        int Width, Height, Channels;
        unsigned char* Data = stbi_load(texturePath.c_str(), &Width, &Height, &Channels, 0);
        if (Data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load sun texture" << std::endl;
        }
        stbi_image_free(Data);
    }
    Sphere(std::string texturePath, float distance, float rts, float rvs, float scale)
    {          //                   텍스쳐 경로, 태양으로부터의 거리, 자전 속도, 공전 속도, 크기
        this->distance = glm::vec3(distance,0.0f,0.0f);
        this->rts = rts;
        this->rvs = rvs;
        this->scale = glm::vec3(scale,scale,scale);
        this->texturePath = texturePath;
        texturing(texturePath);
    }
    void BindTexure()
    {
        glBindTexture(GL_TEXTURE_2D, Texture);
    }

    void Draw()
    {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, nVert);
    }
    
    void Delete()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
    glm::mat4 Transform()
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians((float)glfwGetTime()*rvs), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, distance);
        model = glm::scale(model, scale);
        model = glm::rotate(model, glm::radians((float)glfwGetTime()*rts), glm::vec3(0.0f, 0.0f, 1.0f));
        return model;
    }
    
    void Create(Shader shader)
    {
        shader.setMat4("model", Transform());
        BindTexure();
        Draw();
    }
    
    private :
    unsigned int VAO, VBO;
    unsigned int Texture;
    int nVert, nAttr;
};
class Satellite : public Sphere // 위성 클래스 **
{
public :
    float st_rts;
    float st_rvs;
    glm::vec3 st_distance;
    glm::vec3 st_scale;
    Satellite(const std::string texturePath, float st_distance, float st_rts, float st_rvs, float st_scale, Sphere& sphere) : Sphere(texturePath, st_distance, st_rts, st_rvs, st_scale)
    {//                   텍스쳐 경로, 태양으로부터의 거리, 자전 속도, 공전 속도, 크기, 부착할 행성
        this->st_distance = glm::vec3(st_distance,0.0f,0.0f);
        this->st_rts = st_rts;
        this->st_rvs = st_rvs;
        this->st_scale = glm::vec3(st_scale,st_scale,st_scale);
        this->distance = sphere.distance;
        this->rts = sphere.rts;
        this->rvs = sphere.rvs;
        texturing(texturePath);
    }
    glm::mat4 Transform()
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians((float)glfwGetTime()*rvs), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, distance);
        model = glm::rotate(model, glm::radians((float)glfwGetTime()*rts), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::rotate(model, glm::radians((float)glfwGetTime()*st_rvs), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, st_distance);
        model = glm::scale(model, st_scale);
        model = glm::rotate(model, glm::radians((float)glfwGetTime()*st_rts), glm::vec3(0.0f, 0.0f, 1.0f));
        return model;
    }
    void Create(Shader shader)
    {
        shader.setMat4("model", Transform());
        BindTexure();
        Draw();
    }
};
class Saturnring // class Sphere 과 유사하지만, 텍스쳐가 png 파일이라 텍스쳐링 메서드 일부를 RGBA로 변경했고, 구 만들기가 아니라 토성 고리 만들기를 이용했습니다.
{
public :
    float rts;
    float rvs;
    float st_rts = 5.0f;
    glm::vec3 distance;
    glm::vec3 st_scale;
    Saturnring(const std::string texturePath, Sphere sphere, float st_scale)
    {                               //텍스쳐 경로, 부착할 행성, 크기
        this->distance = sphere.distance;
        this->rts = sphere.rts;
        this->rvs = sphere.rvs;
        this->st_scale = glm::vec3(st_scale,st_scale,st_scale);
        texturing(texturePath);
    }
    void texturing(std::string texturePath)
    {
        // sphere VAO and VBO
        float* Verts = nullptr;
        initSaturnRing(&Verts, &nVert, &nAttr); // 토성 고리 만들기 **
        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, nVert * nAttr * sizeof(float), Verts, GL_STATIC_DRAW);
        
        glBindVertexArray(VAO);
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, nAttr * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, nAttr * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // texCoord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, nAttr * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
        
        free(Verts);
        
        // 텍스처 로드
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        // 텍스처 이미지 로드 및 설정
        int Width, Height, Channels;
        unsigned char* Data = stbi_load(texturePath.c_str(), &Width, &Height, &Channels, 0);
        if (Data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load sun texture" << std::endl;
        }
        stbi_image_free(Data);
    }
    glm::mat4 Transform()
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians((float)glfwGetTime()*rvs), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, distance);
        model = glm::rotate(model, glm::radians((float)glfwGetTime()*rts), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, st_scale);
        model = glm::rotate(model, glm::radians((float)glfwGetTime()*st_rts), glm::vec3(0.0f, 1.0f, 0.0f));
        return model;
    }
    void Create(Shader shader)
    {
        shader.setMat4("model", Transform());
        BindTexure();
        Draw();
    }
    void BindTexure()
    {
        glBindTexture(GL_TEXTURE_2D, Texture);
    }

    void Draw()
    {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, nVert);
    }
    
    void Delete()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
    private :
    unsigned int VAO, VBO;
    unsigned int Texture;
    int nVert, nAttr;
};

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
//glm::vec3 lightPos(0.0f, 2.0f, 0.0f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "1891090 Jang Eunsu", NULL, NULL);
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
    Shader sphereShader1("/Users/J/Documents/Xcode/GL/GL/sphere.vs", "/Users/J/Documents/Xcode/GL/GL/sphere.fs");
    
    Sphere Earth("/opt/homebrew/Cellar/glfw/3.3.8/textures/earth.jpg", // 텍스쳐 경로 및 파일 이름
                     2*109.1*0.06f, // 태양으로부터의 거리
                     1.0f,          // 자전 속도 (지구가 1.0f 기준)
                     1.0f,          // 공전 속도 (지구가 1.0f 기준)
                     1.0*0.5f);     // 크기
    Sphere Sun("/opt/homebrew/Cellar/glfw/3.3.8/textures/sun.jpg",
                   0.0f,
                   25.38f,
                   0.0f,
                   109.1*0.05f);
    Satellite Moon("/opt/homebrew/Cellar/glfw/3.3.8/textures/moon.jpg", // 위성 객체 **
                   20.27*0.05f,
                   0.0f,
                   0.0748f,
                   0.27*0.5f,
                   Earth); // 부착할 행성 ( 지구 )
    Sphere Mercury("/opt/homebrew/Cellar/glfw/3.3.8/textures/mercury.jpg",
                     1.39*109.1*0.05f,
                     0.37f,
                     0.2408f,
                     0.383*0.5f);
    Sphere Venus("/opt/homebrew/Cellar/glfw/3.3.8/textures/venus.jpg",
                     1.72*109.1*0.05f,
                     0.92f,
                     0.6152f,
                     0.949*0.5f);
    Sphere Mars("/opt/homebrew/Cellar/glfw/3.3.8/textures/mars.jpg",
                     2.52*109.1*0.06f,
                     1.03f,
                     1.8808f,
                     0.532*0.5f);
    Sphere Jupiter("/opt/homebrew/Cellar/glfw/3.3.8/textures/jupiter.jpg",
                     4.2*109.1*0.05f,
                     2.41f,
                     0.843f,
                     11.21*0.25f);
    Satellite Callisto("/opt/homebrew/Cellar/glfw/3.3.8/textures/callisto.jpg", // 위성 객체 **
                    7.19*11.21*0.1f,
                   -10.31*2.41f,// 자전 방향이 반대라고 하여 음수로 표현
                   0.2917f,
                   0.378*0.5f,
                   Jupiter); // 목성에 부착
    Satellite Io("/opt/homebrew/Cellar/glfw/3.3.8/textures/io.jpg",             // 위성 객체 **
                   2.82*11.21*0.1f,
                   -10.44*2.41f,// 자전 방향이 반대라고 하여 음수로 표현
                   0.2039f,
                   0.286*0.5f,
                   Jupiter);// 목성에 부착
    Satellite Ganymede("/opt/homebrew/Cellar/glfw/3.3.8/textures/ganymede.jpg", // 위성 객체 **
                   4.21*11.21*0.1f,
                   -10.41*2.41f, // 자전 방향이 반대라고 하여 음수로 표현
                   0.1483f,
                   0.413*0.5f,
                   Jupiter);// 목성에 부착
    Satellite Europa("/opt/homebrew/Cellar/glfw/3.3.8/textures/europa.jpg",     // 위성 객체 **
                   5.87*11.21*0.1f,
                   10.22*2.41f,
                   0.4651f,
                   0.245*0.5f,
                   Jupiter);// 목성에 부착
    Sphere Saturn("/opt/homebrew/Cellar/glfw/3.3.8/textures/saturn.jpg",
                     7.58*109.1*0.05f,
                     0.44f,
                     0.7343f,
                     9.45*0.25f);
    Saturnring Saturnring("/opt/homebrew/Cellar/glfw/3.3.8/textures/saturn_ring.png",
                          Saturn, // 토성에 부착
                          6.45*0.25f);
    Sphere Uranus("/opt/homebrew/Cellar/glfw/3.3.8/textures/uranus.jpg",
                     10.18*109.1*0.05f,
                     0.72f,
                     0.39f,
                     4.01*0.25f);
    Sphere Neptune("/opt/homebrew/Cellar/glfw/3.3.8/textures/neptune.jpg",
                     12.07*109.1*0.05f,
                     0.68f,
                     0.306f,
                     3.88*0.25f);
    Sphere Milkyway("/opt/homebrew/Cellar/glfw/3.3.8/textures/milkyway.jpg",
                    0.0f,
                    0.0f,
                    0.0f,
                    80.0f);
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = (currentFrame - lastFrame)*10;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // be sure to activate shader when setting uniforms/drawing objects
        sphereShader1.use();
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        sphereShader1.setMat4("projection", projection);
        sphereShader1.setMat4("view", view);
        // eye position
        sphereShader1.setVec3("eyePos", camera.Position);

        // draw the sphere object
        // light properties
        /*glm::vec3 lightColor(1.0, 1.0, 1.0);
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influenc
        sphereShader1.setVec3("light.ambient", ambientColor);
        sphereShader1.setVec3("light.diffuse", diffuseColor);
        sphereShader1.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        sphereShader1.setVec3("light.position", lightPos);
        // material properties
        sphereShader1.setVec3("material.ambient", 0.8f, 0.8f, 0.7f);
        sphereShader1.setVec3("material.diffuse", 1.0f, 1.0f, 0.9f);
        sphereShader1.setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
        sphereShader1.setFloat("material.shininess", 30.0f);*/
        
        // ****** 태양에 그림자가 생기는 게 싫어서 라이팅을 이용하지 않았습니다. 대신 은하수 객체를 만들어 배경을 우주처럼 꾸며봤습니다.
        
        Sun.Create(sphereShader1);
        Earth.Create(sphereShader1);
        Moon.Create(sphereShader1);
        Mercury.Create(sphereShader1);
        Venus.Create(sphereShader1);
        Mars.Create(sphereShader1);
        Jupiter.Create(sphereShader1);
        Saturn.Create(sphereShader1);
        Saturnring.Create(sphereShader1);
        Uranus.Create(sphereShader1);
        Neptune.Create(sphereShader1);
        Ganymede.Create(sphereShader1);
        Io.Create(sphereShader1);
        Europa.Create(sphereShader1);
        Callisto.Create(sphereShader1);
        Milkyway.Create(sphereShader1); // 우주 배경


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    Earth.Delete();
    Sun.Delete();
    Moon.Delete();
    Mercury.Delete();
    Venus.Delete();
    Mars.Delete();
    Jupiter.Delete();
    Saturn.Delete();
    Saturnring.Delete();
    Uranus.Delete();
    Neptune.Delete();
    Ganymede.Delete();
    Io.Delete();
    Europa.Delete();
    Callisto.Delete();
    Milkyway.Delete();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
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
    camera.ProcessMouseScroll(yoffset);
}

// initalize vertices of a sphere : position, normal, tex_coords.
//void initSphere(std::vector <float> data, int* nVert, int* nAttr)
void initSphere(float** vertices, int* nVert, int* nAttr)
{
    //----------------------------------------
    // sphere: set up vertex data and configure vertex attributes
    float pi = acosf(-1.0f);    // pi = 3.14152...
    float pi2 = 2.0f * pi;
    int nu = 40, nv = 10;
    const double du = pi2 / nu;
    const double dv = pi / nv;

    *nVert = 6 * nu * nv;        // triangles
    *nAttr = 8;
    *vertices = (float*)malloc(sizeof(float) * (*nVert) * (*nAttr));

    float u, v;
    int k = 0;

    for (v = 0.0f; v < pi; v += dv)
    {
        for (u = 0.0f; u < pi2; u += du)
        {
            // p(u,v)
            (*vertices)[k++] = sinf(v) * cosf(u);     // position x
            (*vertices)[k++] = sinf(v) * sinf(u);     // position y
            (*vertices)[k++] = cosf(v);               // position z
            (*vertices)[k++] = sinf(v) * cosf(u);     // normal x
            (*vertices)[k++] = sinf(v) * sinf(u);     // normal y
            (*vertices)[k++] = cosf(v);               // normal z
            (*vertices)[k++] = u / pi2;               // texture coord u
            (*vertices)[k++] = v / pi;                // texture coord v

            // p(u+du,v)
            (*vertices)[k++] = sinf(v) * cosf(u + du);
            (*vertices)[k++] = sinf(v) * sinf(u + du);
            (*vertices)[k++] = cosf(v);
            (*vertices)[k++] = sinf(v) * cosf(u + du);
            (*vertices)[k++] = sinf(v) * sinf(u + du);
            (*vertices)[k++] = cosf(v);
            (*vertices)[k++] = (u + du) / pi2;
            (*vertices)[k++] = v / pi;

            // p(u,v+dv)
            (*vertices)[k++] = sinf(v + dv) * cosf(u);
            (*vertices)[k++] = sinf(v + dv) * sinf(u);
            (*vertices)[k++] = cosf(v + dv);
            (*vertices)[k++] = sinf(v + dv) * cosf(u);
            (*vertices)[k++] = sinf(v + dv) * sinf(u);
            (*vertices)[k++] = cosf(v + dv);
            (*vertices)[k++] = u / pi2;
            (*vertices)[k++] = (v + dv) / pi;

            // p(u+du,v)
            (*vertices)[k++] = sinf(v) * cosf(u + du);
            (*vertices)[k++] = sinf(v) * sinf(u + du);
            (*vertices)[k++] = cosf(v);
            (*vertices)[k++] = sinf(v) * cosf(u + du);
            (*vertices)[k++] = sinf(v) * sinf(u + du);
            (*vertices)[k++] = cosf(v);
            (*vertices)[k++] = (u + du) / pi2;
            (*vertices)[k++] = v / pi;

            // p(u+du,v+dv)
            (*vertices)[k++] = sinf(v + dv) * cosf(u + du);
            (*vertices)[k++] = sinf(v + dv) * sinf(u + du);
            (*vertices)[k++] = cosf(v + dv);
            (*vertices)[k++] = sinf(v + dv) * cosf(u + du);
            (*vertices)[k++] = sinf(v + dv) * sinf(u + du);
            (*vertices)[k++] = cosf(v + dv);
            (*vertices)[k++] = (u + du) / pi2;
            (*vertices)[k++] = (v + dv) / pi;

            // p(u,v+dv)
            (*vertices)[k++] = sinf(v + dv) * cosf(u);
            (*vertices)[k++] = sinf(v + dv) * sinf(u);
            (*vertices)[k++] = cosf(v + dv);
            (*vertices)[k++] = sinf(v + dv) * cosf(u);
            (*vertices)[k++] = sinf(v + dv) * sinf(u);
            (*vertices)[k++] = cosf(v + dv);
            (*vertices)[k++] = u / pi2;
            (*vertices)[k++] = (v + dv) / pi;
        }
    }
}

void initSaturnRing(float** vertices, int* nVert, int* nAttr)
{
    // Set up vertex data and configure vertex attributes
    int numSegments = 100;
    float innerRadius = 2.0f;
    float outerRadius = 4.0f;

    *nVert = 6 * numSegments;
    *nAttr = 8;
    *vertices = (float*)malloc(sizeof(float) * (*nVert) * (*nAttr));

    float theta = 0.0f;
    float dTheta = (2.0f * M_PI) / numSegments;
    int k = 0;

    for (int i = 0; i < numSegments; i++)
    {
        // Vertex 1
        (*vertices)[k++] = innerRadius * sinf(theta);    // position x
        (*vertices)[k++] = 0.0f;                         // position y
        (*vertices)[k++] = innerRadius * cosf(theta);    // position z
        (*vertices)[k++] = 0.0f;                         // normal x
        (*vertices)[k++] = 0.0f;                         // normal y
        (*vertices)[k++] = 1.0f;                         // normal z
        (*vertices)[k++] = (float)i / numSegments;       // texture coord u
        (*vertices)[k++] = 0.0f;                         // texture coord v

        // Vertex 2
        (*vertices)[k++] = outerRadius * sinf(theta);    // position x
        (*vertices)[k++] = 0.0f;                         // position y
        (*vertices)[k++] = outerRadius * cosf(theta);    // position z
        (*vertices)[k++] = 0.0f;                         // normal x
        (*vertices)[k++] = 0.0f;                         // normal y
        (*vertices)[k++] = 1.0f;                         // normal z
        (*vertices)[k++] = (float)i / numSegments;       // texture coord u
        (*vertices)[k++] = 1.0f;                         // texture coord v

        // Vertex 3
        (*vertices)[k++] = innerRadius * sinf(theta + dTheta);    // position x
        (*vertices)[k++] = 0.0f;                                 // position y
        (*vertices)[k++] = innerRadius * cosf(theta + dTheta);    // position z
        (*vertices)[k++] = 0.0f;                                 // normal x
        (*vertices)[k++] = 0.0f;                                 // normal y
        (*vertices)[k++] = 1.0f;                                 // normal z
        (*vertices)[k++] = (float)(i + 1) / numSegments;         // texture coord u
        (*vertices)[k++] = 0.0f;                                 // texture coord v

        // Vertex 4
        (*vertices)[k++] = innerRadius * sinf(theta + dTheta);    // position x
        (*vertices)[k++] = 0.0f;                                 // position y
        (*vertices)[k++] = innerRadius * cosf(theta + dTheta);    // position z
        (*vertices)[k++] = 0.0f;                                 // normal x
        (*vertices)[k++] = 0.0f;                                 // normal y
        (*vertices)[k++] = 1.0f;                                 // normal z
        (*vertices)[k++] = (float)(i + 1) / numSegments;         // texture coord u
        (*vertices)[k++] = 0.0f;                                 // texture coord v

        // Vertex 5
        (*vertices)[k++] = outerRadius * sinf(theta + dTheta);    // position x
        (*vertices)[k++] = 0.0f;                                 // position y
        (*vertices)[k++] = outerRadius * cosf(theta + dTheta);    // position z
        (*vertices)[k++] = 0.0f;                                 // normal x
        (*vertices)[k++] = 0.0f;                                 // normal y
        (*vertices)[k++] = 1.0f;                                 // normal z
        (*vertices)[k++] = (float)(i + 1) / numSegments;         // texture coord u
        (*vertices)[k++] = 1.0f;                                 // texture coord v

        // Vertex 6
        (*vertices)[k++] = outerRadius * sinf(theta);    // position x
        (*vertices)[k++] = 0.0f;                         // position y
        (*vertices)[k++] = outerRadius * cosf(theta);    // position z
        (*vertices)[k++] = 0.0f;                         // normal x
        (*vertices)[k++] = 0.0f;                         // normal y
        (*vertices)[k++] = 1.0f;                         // normal z
        (*vertices)[k++] = (float)i / numSegments;       // texture coord u
        (*vertices)[k++] = 1.0f;                         // texture coord v

        theta += dTheta;
    }
}
