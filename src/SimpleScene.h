#ifndef _SIMPLE_SCENE_H
#define _SIMPLE_SCENE_H

#include "IScene.h"
#include "SimpleGLShader.h"
#include "objects/Cube.h"
#include "objects/MonkeyHead.h"
#include "objects/Mesh.h"

#include "glm/glm.hpp"
#include <vector>

#include "DepthCubeMap.h"

class SimpleScene : public IScene {
protected:
	void draw1();
	void draw2(Camera* camera);
	void drawScene(SimpleGLShader& shader);

    void compileAndLinkShader();

	float angle = 0.0f;

public:
	std::vector<glm::vec3> colors;
	SimpleScene(int w, int h, int numObjects = 5);
	~SimpleScene();
	RenderObject *object1, *object2, *object3;

	int numObjects = 5;
	int objectsInScene() { return numObjects; }
	void addNewObject() {
		numObjects++;
	}
	void removeObject() {
		if (numObjects > 1)
			numObjects--;
	}

	DepthCubeMap depthCubeMap;

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

    void initScene();
    void update( float t );
	void draw(Camera* camera);
    void resize(int, int);

	std::vector<glm::mat4> shadowTransforms;
	glm::mat4 shadowProjection;

	glm::vec3 lightPosition;
	glm::vec3 lightColor;

	SimpleGLShader simpleShader;

	RenderObject* sphere;
	int randomNumber;
};

#endif // _SIMPLE_SCENE_H
