#include "shader.hpp"
#include <cassert>
#include <GL/glew.h>

namespace CBlocks {
	std::string Shader::SHADER_DIR = ".\\shaders\\";

	Shader::Shader() {
		mProgram = glCreateProgram();
		if(0 == mProgram) {
			//Do error stuff here. For now we're just going to crash horribly
			assert(false);
		}
		//Load default forward passthrough shaders for now. Should add ambient to them
		load_vertex_shader("basic.vert");
		load_fragment_shader("basic.frag");
	}

	Shader::Shader(const char *vShaderFileName, const char *fShaderFileName) {
		mProgram = glCreateProgram();
		if(0 == mProgram) {
			assert(false);
		}
		load_vertex_shader(vShaderFileName);
		load_fragment_shader(fShaderFileName);
	}

	Shader::~Shader() {
		for(GLint i : mShaders) {
			glDetachShader(mProgram, i);
			glDeleteShader(i);
		}
		glDeleteProgram(mProgram);
	}

	void Shader::load_vertex_shader(std::string shaderFileName) {
		shaderFileName = SHADER_DIR + shaderFileName;
		std::ifstream fs(shaderFileName);
		std::stringstream buffer;
		buffer << fs.rdbuf();
		add_vertex_shader(buffer.str().c_str());
	}

	void Shader::load_fragment_shader(std::string shaderFileName) {
		shaderFileName = SHADER_DIR + shaderFileName;
		std::ifstream fs(shaderFileName);
		std::stringstream buffer;
		buffer << fs.rdbuf();
		add_fragment_shader(buffer.str().c_str());
	}

	void Shader::bind() {
		glUseProgram(mProgram);
	}

	void Shader::add_vertex_shader(const GLchar *text) {
		add_program(text, GL_VERTEX_SHADER);
	}

	void Shader::add_fragment_shader(const GLchar *text) {
		add_program(text, GL_FRAGMENT_SHADER);
	}

	void Shader::add_program(const GLchar *text, GLuint type) {
		GLint shader = glCreateShader(type);
		if(0 == shader) {
			assert(false);
		}
		glShaderSource(shader, 1, &text, nullptr);
		glCompileShader(shader);
		GLint success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if(!success) {
			GLchar InfoLog[1024];

			glGetShaderInfoLog(shader, 1024, NULL, InfoLog);
			fprintf(stderr, "Error compiling shader type %d: '%s'\n", shader, InfoLog);

			exit(1);
		}
		glAttachShader(mProgram, shader);
		mShaders.push_back(shader);
		compile_shader();
	}

	void Shader::compile_shader() {
		glLinkProgram(mProgram);
		GLint success;
		glGetProgramiv(mProgram, GL_LINK_STATUS, &success);
		if(!success) {
			GLchar InfoLog[1024];
			glGetProgramInfoLog(mProgram, sizeof(InfoLog), NULL, InfoLog);
			fprintf(stderr, "Error linking shader!: %s\n", InfoLog);
			exit(1);
		}
		glValidateProgram(mProgram);
		glGetProgramiv(mProgram, GL_VALIDATE_STATUS, &success);
		if(!success) {
			fprintf(stderr, "Error linking shader!\n");
			exit(1);
		}
	}
}
