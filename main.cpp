#include <iostream>
#include <vector>

// Classes for shapes
class Shape {
public:
	std::string name;
	float posX, posY;
	float speedX, speedY;
	int r, g, b;
	bool shapeDrawn = true;

	Shape() {}
};

class Circle : public Shape {
public:
	float radius;
	int segments = 64;

};

class Rectangle : public Shape {
public:
	float width, height;
};

// --------------------------------------------------------------------------

// Structures & class for configuration
struct WindowConfig {
	int width;
	int height;
};

struct FontConfig {
	std::string path;
	int size;
	int r, g, b;
};

struct Configuration {
	WindowConfig window;
	FontConfig font;
	std::vector<std::shared_ptr<Shape>> shapes;
};

// --------------------------------------------------------------------------

int main(int argc, char* argv[]) {

}