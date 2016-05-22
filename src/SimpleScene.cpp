#include "SimpleScene.h"

#include <time.h>

#include <iostream>

#include <vector>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/constants.hpp"

SimpleScene::SimpleScene(int w, int h, int numObjects) : IScene(w, h), numObjects(numObjects) { }
SimpleScene::~SimpleScene() {
}

void SimpleScene::initScene() {
	compileAndLinkShader();
	glClearColor(0.5f,0.5f,0.5f,1.0f);

	object1 = new Mesh("../models/DarthVader.obj");
	object2 = new Mesh("../models/torus.obj");
	object3 = new Mesh("../models/monkeyhead.obj");
	lightPosition = glm::vec3(0.0f, 2.5f, 0.0f);
	lightColor = glm::vec3(1.0, 1.0, 1.0);

	sphere = new Mesh("../models/sphere.obj");

	srand(time(NULL));
	for(int i = 0; i < 38; i++) {
		float r = ((float)( std::rand() % 1000)) * 0.001;
		r = r < 0.1 ? 0.2 : r;
		float g = ((float)( std::rand() % 1000)) * 0.001;
		float b = ((float)( std::rand() % 1000)) * 0.001;
		colors.push_back(glm::vec3(r, g, b));
	}

	randomNumber = 1234;// std::rand();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}


void SimpleScene::update( float t ) {
	if (!animating()) {
		angle = (angle > 3.141592f * 2.0f) ? 0 : angle + 0.001f / 3 * t;
		lightPosition.x = sin(glfwGetTime() * 0.5) * 5.0;
		lightPosition.z = cos(glfwGetTime() * 0.5) * 5.0;
	}
}

void SimpleScene::draw(Camera* camera) {
	draw1();
	draw2(camera);
}

void SimpleScene::resize(int w, int h) {
	glViewport(0, 0, w, h);
	width = w;
	height = h;
}

void SimpleScene::compileAndLinkShader() {
	simpleShader.load("../shader/renderScene.vert", "../shader/renderScene.frag");
	simpleShader.compile_and_link();

	simpleShader.use();
	simpleShader.add_uniform("depthCubeMap");
	simpleShader.send_uniform("depthCubeMap", 0);

	simpleShader.add_uniform("viewProjection");
	simpleShader.add_uniform("model");
	simpleShader.add_uniform("lightPosition");
	simpleShader.add_uniform("lightColor");
	simpleShader.add_uniform("viewPos");
	simpleShader.add_uniform("far");
	simpleShader.add_uniform("Color");
	simpleShader.add_uniform("reverse");
	simpleShader.add_uniform("enableShadows");
	simpleShader.add_uniform("cartoon");
	simpleShader.send_uniform_b("cartoon", true);
}
void SimpleScene::draw1() {
	depthCubeMap.use(lightPosition);
	drawScene(depthCubeMap.shadowShader);
	depthCubeMap.unuse();
}
void SimpleScene::draw2(Camera* camera) {
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	simpleShader.use();
	projection = camera->GetProjectionMatrix();
	view = camera->GetViewMatrix();

	simpleShader.send_uniform("viewProjection", projection * view);
	simpleShader.send_uniform("lightPosition", lightPosition);
	simpleShader.send_uniform("viewPos", camera->Position);
	simpleShader.send_uniform("far", depthCubeMap.far);

	glActiveTexture(GL_TEXTURE0);
	depthCubeMap.bindCubeMap();
	drawScene(simpleShader);
}
void SimpleScene::drawScene(SimpleGLShader& shader) {
	static bool drawobject1 = true;

	drawobject1 = !drawobject1;

	shader.send_uniform_b("enableShadows", true);
	int n = 0;
	glDisable(GL_CULL_FACE);
	glm::mat4 model;
	model = glm::scale(model, glm::vec3(10.0));
	shader.send_uniform_b("reverse", true);
	shader.send_uniform("model", model);
	shader.send_uniform("Color", colors[n++ % colors.size()]);
	sphere->render();
	shader.send_uniform_b("reverse", false);

	if (drawobject1) {
		glEnable(GL_CULL_FACE);
	}

	shader.send_uniform_b("cartoon", true);

	// G3D Code (Marcos García Lorenzo) (modified)
	std::srand(randomNumber);
	for (unsigned int i = 0; i < numObjects; i++) {
		float size = float(std::rand() % 3 + 1);

		glm::vec3 axis(glm::vec3(float(std::rand() % 2),
			float(std::rand() % 2), float(std::rand() % 2)));
		if (glm::all(glm::equal(axis, glm::vec3(0.0f))))
			axis = glm::vec3(1.0f);

		float trans = float(std::rand() % 7 + 3) * 1.00f + 0.5f;
		glm::vec3 transVec = axis * trans;
		transVec.x *= (std::rand() % 2) ? 1.0f : -1.0f;
		transVec.y *= (std::rand() % 2) ? 1.0f : -1.0f;
		transVec.z *= (std::rand() % 2) ? 1.0f : -1.0f;

		model = glm::rotate(glm::mat4(1.0f), angle*2.0f*size, axis);
		model = glm::translate(model, transVec);
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 0, 1));
		model = glm::rotate(model, angle*2.0f*size, axis);
		model = glm::scale(model, glm::vec3(1.0f / (size*0.7f)));
		shader.send_uniform("model", model);
		shader.send_uniform("Color", colors[n++ % colors.size()]);
		switch (i % 3) {
		case 0:
			object1->render();
			break;
		case 1:
			object2->render();
			break;
		case 2:
			object3->render();
			break;
		}
	}

	if (drawobject1) {
		shader.send_uniform_b("cartoon", false);
		model = glm::translate(glm::mat4(), lightPosition);
		model = glm::scale(model, glm::vec3(0.2));
		shader.send_uniform("model", model);
		shader.send_uniform("Color", lightColor);
		shader.send_uniform_b("enableShadows", false);
		shader.send_uniform("lightColor", lightColor);
		sphere->render();
		shader.send_uniform_b("cartoon", true);
	}
}