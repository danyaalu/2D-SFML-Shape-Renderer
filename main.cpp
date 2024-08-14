#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>

#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

// Classes for shapes
class Shape {
public:
	std::string name;
	float posX, posY;
	float speedX, speedY;
	float r, g, b;
	bool shapeDrawn = true;

	Shape() {}
	virtual ~Shape() {}
};

class Circle : public Shape {
public:
	float radius;
	float segments = 64.0f;
	sf::CircleShape circle;

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
	sf::RectangleShape rectangle;

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

void UpdatePosition(const std::shared_ptr<Shape> shape, const sf::RenderWindow& window) {
	// Update position
	shape->posX += shape->speedX;
	shape->posY += shape->speedY;

	// Check for collision with the window boundaries and reverse the direction of the respective axis
	if (auto circle = std::dynamic_pointer_cast<Circle>(shape)) {
		float left = circle->posX  ;
		float right = circle->posX + (circle->radius * 2);
		float top = circle->posY ;
		float bottom = circle->posY + (circle->radius * 2);


		if (left < 0 || right > window.getSize().x) {
			shape->speedX *= -1;
		}
		if (top < 0 || bottom > window.getSize().y) {
			shape->speedY *= -1;
		}
	}
	else if (auto rectangle = std::dynamic_pointer_cast<Rectangle>(shape)) {
		float left = rectangle->posX;
		float right = rectangle->posX + rectangle->width;
		float top = rectangle->posY;
		float bottom = rectangle->posY + rectangle->height;

		if (left < 0 || right > window.getSize().x) {
			shape->speedX *= -1;
		}
		if (top < 0 || bottom > window.getSize().y) {
			shape->speedY *= -1;
		}
	}
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

	// Create a single string with all shape names separated by '\0'
	std::string shapeNamesStr;
	for (const auto& shape : config.shapes) {
		shapeNamesStr += shape->name + '\0';
	}
	shapeNamesStr += '\0'; // Double-null terminate the string

	// Store the index of the selected shape
	int selectedShapeIndex = 0;

	// Create shape objects
	for (const auto& shape : config.shapes) {
		if (shape->shapeDrawn) {
			if (auto circle = std::dynamic_pointer_cast<Circle>(shape)) {
				circle->circle = sf::CircleShape(circle->radius, circle->segments);
				circle->circle.setFillColor(sf::Color(circle->r, circle->g, circle->b));
			}
			else if (auto rectangle = std::dynamic_pointer_cast<Rectangle>(shape)) {
				rectangle->rectangle = sf::RectangleShape(sf::Vector2f(rectangle->width, rectangle->height));
				rectangle->rectangle.setFillColor(sf::Color(rectangle->r, rectangle->g, rectangle->b));
			}
		}
	}


	// Main game loop
	while (window.isOpen()) {
		// Event handling
		sf::Event event;
		
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		// Start a new ImGui frame
		ImGui::SFML::Update(window, deltaClock.restart());

		// Create a window called "Debug Panel" and use it to display the ImGui widgets
		ImGui::Begin("Debug Panel");
		ImGui::Text("Parameters of shapes");

		// Use the constructed string in the ImGui::Combo function
		ImGui::Combo("Shapes", &selectedShapeIndex, shapeNamesStr.c_str());

		// Display and modify parameters of the selected shape
		auto selectedShape = config.shapes[selectedShapeIndex];
		ImGui::Checkbox(("Draw " + selectedShape->name).c_str(), &selectedShape->shapeDrawn);
		
		// Check if the selected shape is a Circle
		if (auto circle = std::dynamic_pointer_cast<Circle>(selectedShape)) {
			ImGui::SliderFloat("Size##Radius", &circle->radius, 0.0f, 255.0f);
			ImGui::SliderFloat("Segments##Segments", &circle->segments, 0.0f, 64.0f);


			// Set a custom width for the sliders
			ImGui::PushItemWidth(237.0f); // Adjust the width as needed

			ImGui::SliderFloat("##SpeedX", &circle->speedX, -5.0f, 5.0f);
			ImGui::SameLine();
			ImGui::SliderFloat("Speed##SpeedY", &circle->speedY, -5.0f, 5.0f);

			// Restore the default item width
			ImGui::PopItemWidth();

			// For colour ------------------------------
			// Set a custom width for the sliders
			ImGui::PushItemWidth(155.0f); // Adjust the width as needed

			ImGui::SliderFloat("##Red", &circle->r, 0.0f, 255.0f);
			ImGui::SameLine();
			ImGui::SliderFloat("##Green", &circle->g, 0.0f, 255.0f);
			ImGui::SameLine();
			ImGui::SliderFloat("Colour##Blue", &circle->b, 0.0f, 255.0f);

			// Restore the default item width
			ImGui::PopItemWidth();
		}
		// Check if the selected shape is a Rectangle
		else if (auto rectangle = std::dynamic_pointer_cast<Rectangle>(selectedShape)) {
			// Set a custom width for the sliders
			ImGui::PushItemWidth(237.0f); // Adjust the width as needed

			ImGui::SliderFloat("##Width", &rectangle->width, 0.0f, 200.0f);
			ImGui::SameLine();
			ImGui::SliderFloat("Size##Height", &rectangle->height, 0.0f, 200.0f);

			ImGui::SliderFloat("##SpeedX", &rectangle->speedX, -5.0f, 5.0f);
			ImGui::SameLine();
			ImGui::SliderFloat("Speed##SpeedY", &rectangle->speedY, -5.0f, 5.0f);

			// Restore the default item width
			ImGui::PopItemWidth();

			// For colour ------------------------------
			// Set a custom width for the sliders
			ImGui::PushItemWidth(155.0f); // Adjust the width as needed

			ImGui::SliderFloat("##Red", &rectangle->r, 0.0f, 255.0f);
			ImGui::SameLine();
			ImGui::SliderFloat("##Green", &rectangle->g, 0.0f, 255.0f);
			ImGui::SameLine();
			ImGui::SliderFloat("Colour##Blue", &rectangle->b, 0.0f, 255.0f);

			// Restore the default item width
			ImGui::PopItemWidth();
		}

		ImGui::End();

		// Clear the window
		window.clear();

		// Draw shapes
		for (const auto& shape : config.shapes) {
			if (shape->shapeDrawn) {
				if (auto circle = std::dynamic_pointer_cast<Circle>(shape)) {
					circle->circle.setRadius(circle->radius);
					circle->circle.setPointCount(circle->segments);
					circle->circle.setFillColor(sf::Color(circle->r, circle->g, circle->b));
					UpdatePosition(shape, window);
					circle->circle.setPosition(circle->posX, circle->posY);
					window.draw(circle->circle);
				}
				else if (auto rectangle = std::dynamic_pointer_cast<Rectangle>(shape)) {
					rectangle->rectangle.setSize(sf::Vector2f(rectangle->width, rectangle->height));
					rectangle->rectangle.setFillColor(sf::Color(rectangle->r, rectangle->b, rectangle->g));
					UpdatePosition(shape, window);
					rectangle->rectangle.setPosition(rectangle->posX, rectangle->posY);
					window.draw(rectangle->rectangle);
				}
			}
		}

		ImGui::SFML::Render(window);
		window.display();
	}
	ImGui::SFML::Shutdown();
}