// language: C++, file: desktop_main.cpp, target: Windows MSVC (Desktop Preview)
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

// Подключаем наш готовый интерфейс
#include "src/gui.hpp"

int main() {
    if (!glfwInit()) return -1;

    // Настройка контекста OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Obsidian.cc - Liquid Glass Preview", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Включаем VSync

    // Инициализация Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Применяем настройки скруглений Obsidian
    ObsidianGUI::SetupLiquidStyle();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Главный цикл отрисовки превью
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui.NewFrame();

        // Отрисовка нашего ClickGUI и вотермарки Obsidian
        ObsidianGUI::RenderClickGUI();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.06f, 0.07f, 0.10f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Очистка
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}