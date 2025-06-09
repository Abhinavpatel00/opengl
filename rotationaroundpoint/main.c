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

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		printf("Shader compilation error (%s): %s\n", file, infoLog);
	}
	free(source);
	return shader;
}

/* 
 * real time rendering eq 4.9
 * cem yuksel(he is extremely cool professor although i prefer books and research papers more but 
 * please watch his all lectures you will not regret it) also explained it https://youtu.be/EKN7dTJ4ep8?si=cIYeaIUQX6cLoWku
 * keenan crane is also extremely good professor he explains most complex concepts in so simple way that you think you already know it https://www.youtube.com/watch?v=QmFBHSJS0Gw&list=PL9_jI1bdZmz2emSh0UQ5iOdT2xRHFHL7E&index=6
 * Rotation about point p: T(p) * Rz(φ) * T(-p)
 *   where T(v) = translation matrix by vector v
 *         Rz(φ) = rotation matrix about z-axis by angle φ
 *
 * Steps:
 *   1. T(-p): Translate p to origin
 *   2. Rz(φ): Rotate about origin
 *   3. T(p): Translate back to original position
 *
 * Matrix composition: X = T(p) × Rz(φ) × T(-p)  rotate around point p using homogeneous transforms
 * matrix rotations like glm_rotate_z() always rotate around the origin (0,0,0).

 *So, to rotate around point p, you must temporarily shift p to the origin, do the rotation there, and then shift back.
 */

void rotate_around_point(float angle_rad, vec3 p, mat4 out_transform)
{
	glm_mat4_identity(out_transform);
	glm_translate(out_transform, p);
	glm_rotate_z(out_transform, angle_rad, out_transform);
	glm_translate(out_transform, (vec3){-p[0], -p[1], -p[2]});
}
int main()
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Rotation Around Point", NULL, NULL);

	glfwMakeContextCurrent(window);

	glewInit();
	GLuint vertex_shader = compile_shader("vert.glsl", GL_VERTEX_SHADER);
	GLuint fragment_shader = compile_shader("frag.glsl", GL_FRAGMENT_SHADER);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		printf("Shader program linking error: %s\n", infoLog);
	}

	float vertices[] = {
	    -0.5f, -0.5f, 0.0f,
	    0.5f, -0.5f, 0.0f,
	    0.0f, 0.5f, 0.0f};

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	GLint transformLoc = glGetUniformLocation(program, "transform");

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	vec3 rotation_point = {0.0f, 0.5f, 0.0f};
	float rotation_speed = 1.0f;

	while (!glfwWindowShouldClose(window))
	{

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, 1);

		glClear(GL_COLOR_BUFFER_BIT);

		float angle = (float)glfwGetTime() * rotation_speed;

		mat4 transform;
		rotate_around_point(angle, rotation_point, transform);

		glUseProgram(program);
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, (float*)transform);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(program);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glfwTerminate();
	return 0;
}
