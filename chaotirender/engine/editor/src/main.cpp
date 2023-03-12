#include <iostream>

#include <runtime/render_pipeline.h>
#include <editor/camera.h>
#include <editor/render_scene.h>

#include <runtime/test.h>

#include <imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

// TODO: 
// 1. shader define variables: atrribute, varying, uniform
// 2. does renderPipeline need to be global?
// 3. tile based: performance and mipmap

float m_mouse_x = 0;
float m_mouse_y = 0;
Chaotirender::Camera camera;

std::shared_ptr<Chaotirender::SimpleVertexShader> simple_vertex_shader {nullptr};
std::shared_ptr<Chaotirender::TexturePixelShader> texture_pixel_shader {nullptr};

enum class EditorCommand : unsigned int
{
    camera_left      = 1 << 0,  // A
    camera_back      = 1 << 1,  // S
    camera_foward    = 1 << 2,  // W
    camera_right     = 1 << 3,  // D
    camera_up        = 1 << 4,  // Q
    camera_down      = 1 << 5,  // E
    translation_mode = 1 << 6,  // T
    rotation_mode    = 1 << 7,  // R
    scale_mode       = 1 << 8,  // C
    exit             = 1 << 9,  // Esc
    delete_object    = 1 << 10, // Delete
};

unsigned int m_editor_command {0};
unsigned int k_complement_control_command = 0xFFFFFFFF;

static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processEditorCommand();

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void initScene()
{
    auto vertex_buffer = std::make_unique<Chaotirender::VertexBuffer>();
    auto index_buffer = std::make_unique<Chaotirender::IndexBuffer>();

    simple_vertex_shader = std::make_shared<Chaotirender::SimpleVertexShader>();
    texture_pixel_shader = std::make_shared<Chaotirender::TexturePixelShader>();

    Chaotirender::setUp(*vertex_buffer, *index_buffer, *simple_vertex_shader, *texture_pixel_shader);

    camera.setFov(glm::half_pi<float>() / 2);

    camera.lookAt(glm::vec3(0, 0, 2.8f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    Chaotirender::buffer_id vid = Chaotirender::g_render_pipeline.addVertexBuffer(std::move(vertex_buffer));
    Chaotirender::g_render_pipeline.bindVertexBuffer(vid);

    Chaotirender::buffer_id iid = Chaotirender::g_render_pipeline.addIndexBuffer(std::move(index_buffer));
    Chaotirender::g_render_pipeline.bindIndexBuffer(iid);

    Chaotirender::g_render_pipeline.setVertexShader(simple_vertex_shader);
    Chaotirender::g_render_pipeline.setPixelShader(texture_pixel_shader);

    int w, h, n;
    Chaotirender::RenderResource render_resource;
    // auto texture = render_resource.loadTexture("asset/spot/spot_texture.png");
    //  texture->m_sample_type = SampleType::BILINEAR;
    // texture_pixel_shader->texture = texture;
    auto raw = render_resource.loadRawTexture("asset/spot/spot_texture.png", w, h, n);
    auto id = Chaotirender::g_render_pipeline.addTexture(w, h, n, raw);
    Chaotirender::g_render_pipeline.bindPixelShaderTexture(id, "texture", Chaotirender::SampleType::BILINEAR);
}

void initClippingScene()
{
    auto vertex_buffer = std::make_unique<Chaotirender::VertexBuffer>();
    auto index_buffer = std::make_unique<Chaotirender::IndexBuffer>();

    vertex_buffer->push_back(Chaotirender::Vertex(0, 0, 0));
    vertex_buffer->push_back(Chaotirender::Vertex(0.8f, -0.5f, -2));
    vertex_buffer->push_back(Chaotirender::Vertex(0.8f, 0.5f, -2));
    vertex_buffer->push_back(Chaotirender::Vertex(-0.8f, -0.5f, -2));
    vertex_buffer->push_back(Chaotirender::Vertex(-0.8f, 0.5f, -2));

    // index_buffer->push_back(0);
    // index_buffer->push_back(1);
    // index_buffer->push_back(2);

    // index_buffer->push_back(0);
    // index_buffer->push_back(2);
    // index_buffer->push_back(4);

    index_buffer->push_back(0);
    index_buffer->push_back(4);
    index_buffer->push_back(3);

    // index_buffer->push_back(0);
    // index_buffer->push_back(3);
    // index_buffer->push_back(1);

    // index_buffer->push_back(1);
    // index_buffer->push_back(2);
    // index_buffer->push_back(4);

    // index_buffer->push_back(4);
    // index_buffer->push_back(3);
    // index_buffer->push_back(1);

    simple_vertex_shader = std::make_shared<Chaotirender::SimpleVertexShader>();
    texture_pixel_shader = std::make_shared<Chaotirender::TexturePixelShader>();

    camera.setFov(glm::half_pi<float>() / 2);

    camera.lookAt(glm::vec3(0, 0, -1.f), glm::vec3(0, 0, -5), glm::vec3(0, 1, 0));

    Chaotirender::buffer_id vid = Chaotirender::g_render_pipeline.addVertexBuffer(std::move(vertex_buffer));
    Chaotirender::g_render_pipeline.bindVertexBuffer(vid);

    Chaotirender::buffer_id iid = Chaotirender::g_render_pipeline.addIndexBuffer(std::move(index_buffer));
    Chaotirender::g_render_pipeline.bindIndexBuffer(iid);

    Chaotirender::g_render_pipeline.setVertexShader(simple_vertex_shader);
    Chaotirender::g_render_pipeline.setPixelShader(texture_pixel_shader);
}

void initScene(std::string asset_path)
{
    Chaotirender::loadAsset(asset_path);
    std::vector<Chaotirender::RawMesh> meshes;
    Chaotirender::getMeshData(meshes);
    
}

int main(int argc, char** argv) 
{   
    // TODO: 
    // 1. shader define variables: atrribute, varying, uniform
    // 2. does renderPipeline need to be global?
    // 3. when render area is large, fps drops

    // Chaotirender::initScene();
    // int w, h;
    // const uint8_t* data;
    // Chaotirender::drawAndGetScene(w, h, data);
    // Chaotirender::test();

    // 1. wrap window, seperate ui, io
    // 2. load meterial?
    // testLoad();

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, keyCallback);

    // opengl loader
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to initialize GLAD!\n");
        return -1;
    }

     // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

     // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // display texture
    GLuint scene_tex_id;
    int w, h;
    const uint8_t* scene_data = nullptr;

    initScene();
    // initClippingScene();

    glGenTextures(1, &scene_tex_id);
    glBindTexture(GL_TEXTURE_2D, scene_tex_id);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Chaotirender::drawAndGetScene(w, h, scene_data);
    // glBindTexture(GL_TEXTURE_2D, scene_tex_id);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, scene_data);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        processEditorCommand();

        simple_vertex_shader->view_matrix = camera.getViewMatrix();
        simple_vertex_shader->projection_matrix = camera.getProjectionMatrix();
        // TICK(draw)
        texture_pixel_shader->camera_position = camera.position();
        Chaotirender::drawAndGetScene(w, h, scene_data);
        // TOCK(draw)

        // TICK(GUI)
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        // if (show_demo_window)
            // ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
            
            
            glBindTexture(GL_TEXTURE_2D, scene_tex_id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, scene_data);

            ImGui::Image((void*)(intptr_t)scene_tex_id, ImVec2(w, h), ImVec2(0, 1), ImVec2(1, 0));

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        // TOCK(GUI)

        // TICK(render)
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
        // TOCK(render)
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

bool isMouseButtonDown(GLFWwindow* window, int button)
{
    if (button < GLFW_MOUSE_BUTTON_1 || button > GLFW_MOUSE_BUTTON_LAST)
    {
        return false;
    }
    return glfwGetMouseButton(window, button) == GLFW_PRESS;
}

static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    float angularVelocity =
            180.0f / glm::max(Chaotirender::g_render_pipeline.frame_buffer.getWidth(), Chaotirender::g_render_pipeline.frame_buffer.getHeight()); // 180 degrees while moving full screen
    if (m_mouse_x >= 0.0f && m_mouse_y >= 0.0f)
    {
        if (isMouseButtonDown(window, GLFW_MOUSE_BUTTON_RIGHT))
        {
            glfwSetInputMode(
                window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            camera.rotate(
                (ypos - m_mouse_y) * angularVelocity, (xpos - m_mouse_x) * angularVelocity);
        }
        else
            glfwSetInputMode(
                window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        m_mouse_x = xpos;
        m_mouse_y = ypos;
    }
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.zoom(2.f * yoffset);
}

void processEditorCommand()
{
    float           camera_speed  = 0.05f;

    // glm::quat      camera_rotate = editor_camera->rotation().inverse();
    glm::vec3         camera_relative_pos(0, 0, 0);

    if ((unsigned int)EditorCommand::camera_foward & m_editor_command)
    {
        camera_relative_pos += camera_speed * camera.forward();
    }
    if ((unsigned int)EditorCommand::camera_back & m_editor_command)
    {
        camera_relative_pos += -camera_speed * camera.forward();
    }
    if ((unsigned int)EditorCommand::camera_left & m_editor_command)
    {
        camera_relative_pos += -camera_speed * camera.right();
    }
    if ((unsigned int)EditorCommand::camera_right & m_editor_command)
    {
        camera_relative_pos += camera_speed * camera.right();
    }
    if ((unsigned int)EditorCommand::camera_up & m_editor_command)
    {
        camera_relative_pos += glm::vec3(0, camera_speed, 0);
    }
    if ((unsigned int)EditorCommand::camera_down & m_editor_command)
    {
        camera_relative_pos += glm::vec3(0, -camera_speed, 0);;
    }

    camera.move(camera_relative_pos);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
            case GLFW_KEY_A:
                m_editor_command |= (unsigned int)EditorCommand::camera_left;
                break;
            case GLFW_KEY_S:
                m_editor_command |= (unsigned int)EditorCommand::camera_back;
                break;
            case GLFW_KEY_W:
                m_editor_command |= (unsigned int)EditorCommand::camera_foward;
                break;
            case GLFW_KEY_D:
                m_editor_command |= (unsigned int)EditorCommand::camera_right;
                break;
            case GLFW_KEY_Q:
                m_editor_command |= (unsigned int)EditorCommand::camera_up;
                break;
            case GLFW_KEY_E:
                m_editor_command |= (unsigned int)EditorCommand::camera_down;
                break;
            case GLFW_KEY_T:
                m_editor_command |= (unsigned int)EditorCommand::translation_mode;
                break;
            case GLFW_KEY_R:
                m_editor_command |= (unsigned int)EditorCommand::rotation_mode;
                break;
            case GLFW_KEY_C:
                m_editor_command |= (unsigned int)EditorCommand::scale_mode;
                break;
            case GLFW_KEY_DELETE:
                m_editor_command |= (unsigned int)EditorCommand::delete_object;
                break;
            default:
                break;
        }
    }
    else if (action == GLFW_RELEASE)
    {
        switch (key)
        {
            case GLFW_KEY_ESCAPE:
                m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::exit);
                break;
            case GLFW_KEY_A:
                m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::camera_left);
                break;
            case GLFW_KEY_S:
                m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::camera_back);
                break;
            case GLFW_KEY_W:
                m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::camera_foward);
                break;
            case GLFW_KEY_D:
                m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::camera_right);
                break;
            case GLFW_KEY_Q:
                m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::camera_up);
                break;
            case GLFW_KEY_E:
                m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::camera_down);
                break;
            case GLFW_KEY_T:
                m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::translation_mode);
                break;
            case GLFW_KEY_R:
                m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::rotation_mode);
                break;
            case GLFW_KEY_C:
                m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::scale_mode);
                break;
            case GLFW_KEY_DELETE:
                m_editor_command &= (k_complement_control_command ^ (unsigned int)EditorCommand::delete_object);
                break;
            default:
                break;
        }
    }
}