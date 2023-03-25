#include <iostream>

#include <runtime/pipeline/render_pipeline.h>
#include <runtime/render/camera.h>
#include <runtime/resource/render_object.h>
#include <runtime/engine.h>
#include <runtime/global/engine_global_context.h>

#include <runtime/test.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>


// TODO: 
// 1. shader define variables: atrribute, varying, uniform
// 2. does renderPipeline need to be global?
// 3. tile based: performance and mipmap
// 4. ** gamma correction
// 5. ** refractor folder structure

float m_mouse_x = 0;
float m_mouse_y = 0;
// Chaotirender::Camera camera;
// Chaotirender::Camera old_camera;

std::shared_ptr<Chaotirender::SimpleVertexShader> simple_vertex_shader {nullptr};
std::shared_ptr<Chaotirender::TexturePixelShader> texture_pixel_shader {nullptr};

Chaotirender::ChaotirenderEngine engine;
Chaotirender::Camera& camera = engine.m_render_system->m_camera;

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

void showEditorMenu()
{   
    bool enable_parallel = true;
    bool draw_triangle = true;
    bool draw_line = false;
    bool back_face_culling = true;

    ImGuiDockNodeFlags dock_flags   = ImGuiDockNodeFlags_DockSpace;
    // ImGuiWindowFlags   window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar |
    //                             ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
    //                             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
    //                             ImGuiConfigFlags_NoMouseCursorChange | ImGuiWindowFlags_NoBringToFrontOnFocus;
                    
    ImGuiWindowFlags   window_flags = ImGuiWindowFlags_NoTitleBar |
                                ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_NoMove |
                                ImGuiConfigFlags_NoMouseCursorChange | ImGuiWindowFlags_NoBringToFrontOnFocus;

    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(main_viewport->WorkPos, ImGuiCond_Always);
    // std::array<int, 2> window_size = g_editor_global_context.m_window_system->getWindowSize();
    ImGui::SetNextWindowSize(ImVec2(1920.f, 1080.f), ImGuiCond_Always);

    ImGui::SetNextWindowViewport(main_viewport->ID);

    ImGui::Begin("Editor menu", NULL, window_flags);

    ImGuiID main_docking_id = ImGui::GetID("Main Docking");
    if (ImGui::DockBuilderGetNode(main_docking_id) == nullptr)
    {
        ImGui::DockBuilderRemoveNode(main_docking_id);

        ImGui::DockBuilderAddNode(main_docking_id, dock_flags);
        ImGui::DockBuilderSetNodePos(main_docking_id,
                                        ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y + 18.0f));
        ImGui::DockBuilderSetNodeSize(main_docking_id,
                                        ImVec2(1920.f, 1080.f - 18.0f));

        ImGuiID center = main_docking_id;
        ImGuiID left;
        ImGuiID right = ImGui::DockBuilderSplitNode(center, ImGuiDir_Right, 0.25f, nullptr, &left);

        ImGuiID left_other;
        ImGuiID left_file_content = ImGui::DockBuilderSplitNode(left, ImGuiDir_Down, 0.30f, nullptr, &left_other);

        ImGuiID left_game_engine;
        ImGuiID left_asset =
            ImGui::DockBuilderSplitNode(left_other, ImGuiDir_Left, 0.30f, nullptr, &left_game_engine);

        ImGui::DockBuilderDockWindow("World Objects", left_asset);
        ImGui::DockBuilderDockWindow("Components Details", right);
        ImGui::DockBuilderDockWindow("Render Resource", left_file_content);
        ImGui::DockBuilderDockWindow("Render Scene", left_game_engine);

        ImGui::DockBuilderFinish(main_docking_id);
    }

    ImGui::DockSpace(main_docking_id);

    if (ImGui::BeginMainMenuBar())
    {   
        if (ImGui::BeginMenu("Render"))
        {   
            if (ImGui::MenuItem("parallel", NULL, enable_parallel))
                enable_parallel = !enable_parallel;

            if (ImGui::BeginMenu("primitive"))
            {   
                if (ImGui::MenuItem("triangle", NULL, draw_triangle))
                {   
                    draw_triangle = !draw_triangle;
                    draw_line = !draw_line;
                }

                if (ImGui::MenuItem("line", NULL, draw_line))
                {   
                    draw_triangle = !draw_triangle;
                    draw_line = !draw_line;
                } 
                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("backe face culling", NULL, back_face_culling))
                back_face_culling = !back_face_culling;

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    ImGui::End();
}

void showEditorWorldObjectsWindow(bool* p_open)
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

    if (!*p_open)
        return;

    if (!ImGui::Begin("World Objects", p_open, window_flags))
    {
        ImGui::End();
        return;
    }
    auto& obj_list = Chaotirender::g_engine_global_context.m_scene_manager->m_object_instance_list;
    for (int ind = 0; ind < obj_list.size(); ++ind)
    {
        const std::string name = obj_list[ind].m_name;
        int selected_ind = Chaotirender::g_engine_global_context.m_scene_manager->m_selected_obj_ins_ind;
        if (ImGui::Selectable(name.c_str(),  selected_ind== ind))
        {   
            if (selected_ind != ind)
                Chaotirender::g_engine_global_context.m_scene_manager->m_selected_obj_ins_ind = ind;
            else
                Chaotirender::g_engine_global_context.m_scene_manager->m_selected_obj_ins_ind = -1;
            break;
        }
    }

    ImGui::End();
}

void showEditorRenderScene(bool* p_open, int scene_tex_id, int w, int h, const uint8_t* scene_data)
{
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

    if (!*p_open)
        return;

    if (!ImGui::Begin("Render Scene", p_open))
    {
        ImGui::End();
        return;
    }

    glBindTexture(GL_TEXTURE_2D, scene_tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, scene_data);

    ImGui::Image((void*)(intptr_t)scene_tex_id, ImVec2(w, h), ImVec2(0, 1), ImVec2(1, 0));

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

void showEditorFileContentWindow(bool* p_open)
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

        if (!*p_open)
            return;

        if (!ImGui::Begin("Render Resource", p_open, window_flags))
        {
            ImGui::End();
            return;
        }

        static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH |
                                       ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg |
                                       ImGuiTableFlags_NoBordersInBody;

        auto& obj_res_list = Chaotirender::g_engine_global_context.m_asset_manager->m_object_resource_list;

        for (int ind = 0; ind < obj_res_list.size(); ++ind)
        {
            auto& obj_res = obj_res_list[ind];

            const std::string name = obj_res.m_name;
            const std::string path = obj_res.m_path;
            const std::string mesh_file = obj_res.m_mesh_source_desc.m_mesh_file;
            const std::string base_color_file = obj_res.m_material_source_desc.m_base_color_file;

            if (ImGui::TreeNode(name.c_str()))
            {
                ImGui::Text(("path: " + path).c_str());
                ImGui::Text(("mesh file: " + mesh_file).c_str());
                if (!base_color_file.empty())
                    ImGui::Text(("material: " + base_color_file).c_str());

                std::string default_name = Chaotirender::g_engine_global_context.m_scene_manager->getNextObjInstanceName(ind);
                static char cname[32] = "";
                memset(cname, 0, 128);
                memcpy(cname, default_name.c_str(), default_name.size());
                
                ImGui::Text("Name");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(150.f);
                ImGui::InputText("##Name", cname, IM_ARRAYSIZE(cname));
                ImGui::SameLine();

                if (ImGui::Button("Add Object"))
                {
                    std::string obj_name = cname;
                    if (obj_name.empty())
                        obj_name = default_name;
                    Chaotirender::g_engine_global_context.m_scene_manager->addObjectInstance(ind, obj_name);
                }

                ImGui::TreePop();
            }
        }

        ImGui::End();
}

void DrawVecControl(const std::string& label, glm::vec3& values, float resetValue = 0.f, float columnWidth = 100.f) 
{
    ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text("%s", label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});

        float  lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.8f, 0.1f, 0.15f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.9f, 0.2f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.8f, 0.1f, 0.15f, 1.0f});
        if (ImGui::Button("X", buttonSize))
            values.x = resetValue;
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.2f, 0.45f, 0.2f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.3f, 0.55f, 0.3f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.2f, 0.45f, 0.2f, 1.0f});
        if (ImGui::Button("Y", buttonSize))
            values.y = resetValue;
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.1f, 0.25f, 0.8f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.2f, 0.35f, 0.9f, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.1f, 0.25f, 0.8f, 1.0f});
        if (ImGui::Button("Z", buttonSize))
            values.z = resetValue;
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);
        ImGui::PopID();
}

void showEditorDetailWindow(bool* p_open)
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

    if (!*p_open)
        return;

    if (!ImGui::Begin("Components Details", p_open, window_flags))
    {
        ImGui::End();
        return;
    }

    // if select object
    int selected_ind = Chaotirender::g_engine_global_context.m_scene_manager->m_selected_obj_ins_ind;
    if (selected_ind != -1)
    {
        // get selected obj
        auto& selected_obj = Chaotirender::g_engine_global_context.m_scene_manager->m_object_instance_list[selected_ind];

        // name
        const std::string& name = selected_obj.m_name;
        static char        cname[128];
        memset(cname, 0, 128);
        memcpy(cname, name.c_str(), name.size());

        ImGui::Text("Name");
        ImGui::SameLine();
        ImGui::InputText("##Name", cname, IM_ARRAYSIZE(cname)); // ImGuiInputTextFlags_ReadOnly

        std::string new_name = cname;
        if (!new_name.empty())
            selected_obj.m_name = new_name;
        
        // draw or not
        ImGui::Checkbox("visible", &selected_obj.m_draw);

        // components
        // transform
        DrawVecControl("Position", selected_obj.m_transform.translation);
        DrawVecControl("Scale", selected_obj.m_transform.scale);

        // materials
        // selected_obj.
    }

    ImGui::End();
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
    // Chaotirender::testAssetManager();

    // Chaotirender::ChaotirenderEngine engine;
    // engine.init();
    engine.run();
    // camera = engine.m_render_system->m_camera;

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Chaotirender", NULL, NULL);
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
    // ImGuiIO& io = ImGui::GetIO(); (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    // //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    // //io.ConfigViewportsNoAutoMerge = true;
    // //io.ConfigViewportsNoTaskBarIcon = true;


     // set ui content scale
    float x_scale, y_scale;
    glfwGetWindowContentScale(window, &x_scale, &y_scale);
    float content_scale = fmaxf(1.0f, fmaxf(x_scale, y_scale));
    
    // load font for imgui
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigDockingAlwaysTabBar         = true;
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.Fonts->AddFontFromFileTTF(
        "resource/editorFont.TTF", content_scale * 16, nullptr, nullptr);
    io.Fonts->Build();

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

    // initScene();
    // set scene view port
    Chaotirender::g_render_pipeline.setViewPort(1280, 720);
    // render_scene.m_camera.setAspect(16.f / 9.f);
    camera.setAspect(16.f /9.f);

    // initScene("asset/mary/Marry.obj");
    // initScene();

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

    bool enable_parallel = true;
    bool draw_triangle = true;
    bool draw_line = false;
    bool back_face_culling = true;
    Chaotirender::PrimitiveType primitive = Chaotirender::PrimitiveType::triangle;

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

        // simple_vertex_shader->view_matrix = camera.getViewMatrix();
        // simple_vertex_shader->projection_matrix = camera.getProjectionMatrix();
        // // // // TICK(draw)
        // texture_pixel_shader->camera_position = camera.position();
        // Chaotirender::drawAndGetScene(w, h, scene_data);

        engine.m_render_system->swapRenderData();

        // TICK(outdraw)
        Chaotirender::g_render_pipeline.render_config.enable_parallel = enable_parallel;
        Chaotirender::g_render_pipeline.render_config.rasterize_config.back_face_culling = back_face_culling;
        Chaotirender::g_render_pipeline.render_config.rasterize_config.primitive = primitive;
        Chaotirender::g_render_pipeline.render_config.rasterize_config.line_color = Chaotirender::Color(255, 255, 255);

        engine.m_render_system->drawOneFrame(w, h, scene_data);
        // TOCK(outdraw)

        // drawAndGetScene(w, h, scene_data);
        // TOCK(draw)

        // TICK(GUI)
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        showEditorMenu();

        bool show_obj_window = true;
        showEditorWorldObjectsWindow(&show_obj_window);

        // // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        bool show_scene_window = true;
        showEditorRenderScene(&show_scene_window, scene_tex_id, w, h, scene_data);

        // {
        //     ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
            
        //     glBindTexture(GL_TEXTURE_2D, scene_tex_id);
        //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, scene_data);

        //     ImGui::Image((void*)(intptr_t)scene_tex_id, ImVec2(w, h), ImVec2(0, 1), ImVec2(1, 0));

        //     ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        //     ImGui::End();
        // }
        // TOCK(GUI)

        // TICK(render)
        bool show_file_window = true;
        showEditorFileContentWindow(&show_file_window);

        bool show_detail_window = true;
        showEditorDetailWindow(&show_detail_window);


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