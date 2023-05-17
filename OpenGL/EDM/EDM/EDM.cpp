#include <GLFW/glfw3.h>

int main(void)
{
    GLFWwindow* window;

    // GLFW 라이브러리 초기화
    if (!glfwInit())
        return -1;

    // 윈도우 생성
    window = glfwCreateWindow(1024, 768, "Endless Death Match", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    // 컨텍스트 생성
    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        // 프론트, 백 버퍼 교체
        glfwSwapBuffers(window);

        // 이벤트 폴링 : 충돌 방지!
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}