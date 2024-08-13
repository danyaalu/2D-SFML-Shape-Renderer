#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

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

Configuration LoadConfiguration(std::string& configurationPath) {
	std::ifstream file(configurationPath);
	std::string line;

	if (!file.is_open()) {
		std::cerr << "Error: Unable to open file." << std::endl;
		exit(-1);
	}

	Configuration config;

	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string dataType;
		iss >> dataType;

		if (dataType == "Circle") {
			Circle circle;

			iss >> circle.name >> circle.posX >> circle.posY >> circle.speedX >> circle.speedY
				>> circle.r >> circle.g >> circle.g >> circle.radius;

			config.shapes.push_back(std::make_shared<Circle>(circle));
		}
		else if (dataType == "Rectangle") {
			Rectangle rectangle;

			iss >> rectangle.name >> rectangle.posX >> rectangle.posY >> rectangle.speedX >> rectangle.speedY
				>> rectangle.r >> rectangle.g >> rectangle.g >> rectangle.width >> rectangle.height;

			config.shapes.push_back(std::make_shared<Rectangle>(rectangle));
		}
		else if (dataType == "Font") {
			iss >> config.font.path >> config.font.size >> config.font.r >> config.font.g >> config.font.b;
		}
		else if (dataType == "Window") {
			iss >> config.window.width >> config.window.height;
		}
	}

	return config;
}

int main(int argc, char* argv[]) {
	std::string configurationPath = "config.txt";
	auto config = LoadConfiguration(configurationPath);
}