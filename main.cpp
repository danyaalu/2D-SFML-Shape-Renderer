#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

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

	void print() {
		std::cout << "Circle created: "
			<< name << " "
			<< posX << " " << posY << " "
			<< speedX << " " << speedY << " "
			<< r << " " << g << " " << b << " "
			<< radius
			<< std::endl;
	}
};

class Rectangle : public Shape {
public:
	float width, height;

	void print() {
		std::cout << "Rectangle created: "
			<< name << " "
			<< posX << " " << posY << " "
			<< speedX << " " << speedY << " "
			<< r << " " << g << " " << b << " "
			<< width << " " << height
			<< std::endl;
	}
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
				>> circle.r >> circle.g >> circle.b >> circle.radius;

			circle.print();
			config.shapes.push_back(std::make_shared<Circle>(circle)); // Adds shape to shapes vector
		}
		else if (dataType == "Rectangle") {
			Rectangle rectangle;

			iss >> rectangle.name >> rectangle.posX >> rectangle.posY >> rectangle.speedX >> rectangle.speedY
				>> rectangle.r >> rectangle.g >> rectangle.b >> rectangle.width >> rectangle.height;

			rectangle.print();
			config.shapes.push_back(std::make_shared<Rectangle>(rectangle)); // Adds shape to shapes vector
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

	sf::RenderWindow window(sf::VideoMode(config.window.width, config.window.height), "2D SFML Shape Renderer");
	window.setFramerateLimit(60);

	// Initialise ImGUI and create a clock used for its internal timing
	ImGui::SFML::Init(window);
	sf::Clock deltaClock;

	// Scale the ImGui UI by a given factor, does not affect text size
	ImGui::GetStyle().ScaleAllSizes(1.0f);

	// The ImGui colour {r,g,b} wheel requires floats from 0-1 rather than integers from 0-255
	float c[3] = { 0.0f, 1.0f, 1.0f };

	// Main game loop
	while (window.isOpen()) {

		window.display();
	}
	ImGui::SFML::Shutdown();
}