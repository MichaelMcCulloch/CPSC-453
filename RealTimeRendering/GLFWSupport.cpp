#include "GLFWSupport.h"


GLFWwindow* InitializeGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(VIEW_WIDTH, VIEW_HEIGHT, "Orrery", NULL, NULL);
	if (window == NULL) {
		cout << "Could not create window" << endl;
		glfwTerminate();
		return NULL;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Could not initalize GLAD" << endl;
		return NULL;
	}

	glViewport(0, 0, VIEW_WIDTH, VIEW_HEIGHT);

	glfwSetFramebufferSizeCallback(window, WindowSizeCallback);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetErrorCallback(ErrorCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	return window;
}
void ErrorCallback(int error, const char *description) {
	cout << "GLFW ERROR " << error << ":" << endl;
	cout << description << endl;
}
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int modifiers) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}
void WindowSizeCallback(GLFWwindow *window, int w, int h) {
	VIEW_WIDTH = w;
	VIEW_HEIGHT = h;
	glViewport(0, 0, w, h);
}
void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {

}