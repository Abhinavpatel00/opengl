#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <stdlib.h>
char* read_file(const char* path)
{
	FILE* file = fopen(path, "rb");
	assert(file);
	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	rewind(file);
	char* buffer = malloc(size + 1);
	fread(buffer, 1, size, file);
	buffer[size] = '\0';
	fclose(file);
	return buffer;
}

GLuint compile_shader(const char* file, GLenum type)
{
	char* source = read_file(file);
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, (const char**)&source, NULL);
	glCompileShader(shader);
	free(source);
	return shader;
}

int main()
{
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(800, 600, "shader", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewInit();
	GLuint vertex_shader = compile_shader("vert.glsl", GL_VERTEX_SHADER);
	GLuint fragment_shader = compile_shader("frag.glsl", GL_FRAGMENT_SHADER);
	GLuint program = glCreateProgram();

	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	glUseProgram(program);
	float vertices[9] = {

	    -0.5f, -0.9f, 0.0f,
	    0.5f, -0.9f, 0.0f,
	    0.0f, -0.1f, 0.0f

	};
	GLuint VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Set clear color (black)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Main rendering loop
	while (!glfwWindowShouldClose(window))
	{
		// Handle input
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, 1);

		// Clear screen
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw triangle
		glUseProgram(program);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(program);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// Terminate GLFW
	glfwTerminate();
	return 0;
}
