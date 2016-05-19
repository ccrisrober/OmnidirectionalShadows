#pragma once

#include <GL/glew.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "SimpleGLShader.h"

class DepthCubeMap {
public:
	DepthCubeMap() {
		glGenFramebuffers(1, &depthMapFBO);
		glGenTextures(1, &depthCubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		//set texture parameters
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		for (GLuint face = 0; face < 6; face++) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "Framebuffer not complete!" << std::endl;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		shadowTransforms = std::vector<glm::mat4>(6);

		shadowShader.load("../shader/shadowmap.vert", "../shader/shadowmap.frag");
		shadowShader.load("../shader/shadowmap.geom", GL_GEOMETRY_SHADER);
		shadowShader.compile_and_link();

		for (GLuint i = 0; i < 6; i++) {
			shadowShader.add_uniform("shadowTransforms[" + std::to_string(i) + "]");
		}
		shadowShader.add_uniform("lightPosition");
		shadowShader.add_uniform("far");
	}
	void use(glm::vec3 lightPosition) {
		GLfloat aspect = (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT;
		shadowProjection = glm::perspective(glm::radians(90.0f), 1.0f, near, far);
		shadowTransforms[0] = (shadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms[1] = (shadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms[2] = (shadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms[3] = (shadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms[4] = (shadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms[5] = (shadowProjection * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		shadowShader.use();
		for (GLuint i = 0; i < 6; i++) {
			shadowShader.send_uniform("shadowTransforms[" + std::to_string(i) + "]", shadowTransforms[i]);
		}
		shadowShader.send_uniform("far", far);
		shadowShader.send_uniform("lightPosition", lightPosition);
	}
	void unuse() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void bindCubeMap() {
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	}
public:
	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	GLuint depthMapFBO;
	GLuint depthCubemap;
	GLfloat near = 1.0f;
	GLfloat far = 1000.0f;

	std::vector<glm::mat4> shadowTransforms;
	glm::mat4 shadowProjection;
	SimpleGLShader shadowShader;
};