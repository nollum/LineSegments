// Vectors.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include "Vector.h"
#include <vector>
#include <iostream>
#include <cmath>

std::vector<double> getIntersection(sf::Vertex p0, sf::Vertex p1, sf::Vertex p2, sf::Vertex p3) {
	std::vector<double> intersect_p;

	double A1 = p1.position.y - p0.position.y;
	double B1 = p0.position.x - p1.position.x;
	double C1 = A1 * p0.position.x + B1 * p0.position.y;
	double A2 = p3.position.y - p2.position.y;
	double B2 = p2.position.x - p3.position.x;
	double C2 = A2 * p2.position.x + B2 * p2.position.y;
	double denominator = A1 * B2 - A2 * B1;
	double rx0, ry0, rx1, ry1;


	intersect_p = { (B2 * C1 - B1 * C2) / denominator, (A1 * C2 - A2 * C1) / denominator };
	rx0 = (intersect_p[0] - p0.position.x) / (p1.position.x - p0.position.x);
	ry0 = (intersect_p[1] - p0.position.y) / (p1.position.y - p0.position.y);
	rx1 = (intersect_p[0] - p2.position.x) / (p3.position.x - p2.position.x);
	ry1 = (intersect_p[1] - p2.position.y) / (p3.position.y - p2.position.y);

	if (((rx0 >= 0 && rx0 <= 1) || (ry0 >= 0 && ry0 <= 1)) && ((rx1 >= 0 && rx1 <= 1) || (ry1 >= 0 && ry1 <= 1)))
		return intersect_p;
	else
		return { -100,-100 };
	
}

int main()
{
	const int WINDOW_HEIGHT = 600;
	const int WINDOW_WIDTH = 600;
	bool moving_start_point = false;
	bool moving_end_point = false;
	bool change_radius = true;
	int circle_ctr{ 0 };
	int moving_vector_ind{ 0 };
	int vector_index{ 0 };
	int lines_index{ 0 };
	int intersect_vect_ind_outer{ 0 };
	int intersect_vect_ind_inner{ 0 };
	int intersection_circle_radius = 5;
	int vector_circle_radius = 5;
	double intersection_x, intersection_y;

	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Lines and intersections", (sf::Style::Titlebar, sf::Style::Close));

	sf::Font font;
	if (!(font.loadFromFile("arial.ttf")))
		std::cout << "Error loading font" << "\n";

	sf::Text usage_instructions;

	usage_instructions.setFont(font);
	usage_instructions.setString("Press 'V' to add a Line or 'C' to clear the screen\nUse LMB to drag the endpoints\nUse RMB to change size of endpoints");
	usage_instructions.setCharacterSize(16);
	usage_instructions.setFillColor(sf::Color::White);
	usage_instructions.setPosition(0, WINDOW_HEIGHT - usage_instructions.getLocalBounds().height*1.2); //1.2 adds some extra padding on the bottom of the text field


	std::vector<Vector> vectors;

	std::vector<std::vector<sf::Vertex> > lines;
	
	std::vector<sf::CircleShape> intersection_circles;
	sf::CircleShape circle;
	circle.setRadius(intersection_circle_radius);
	circle.setOutlineColor(sf::Color::Red);
	circle.setFillColor(sf::Color::Transparent);
	circle.setOutlineThickness(1.f);
	circle.setOrigin(intersection_circle_radius, intersection_circle_radius);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && window.hasFocus()) {
				for (auto ind = 0; ind < vectors.size(); ++ind) {
					if ((sf::Mouse::getPosition(window).x >= vectors[ind].c_start.getPosition().x - vector_circle_radius && sf::Mouse::getPosition(window).x <= vectors[ind].c_start.getPosition().x + vector_circle_radius) &&
						(sf::Mouse::getPosition(window).y >= vectors[ind].c_start.getPosition().y - vector_circle_radius && sf::Mouse::getPosition(window).y <= vectors[ind].c_start.getPosition().y + vector_circle_radius)) {
						moving_start_point = true;
						moving_end_point = false;
						vector_index = ind;
						window.sf::Window::setMouseCursorVisible(false);
					}
					else if ((sf::Mouse::getPosition(window).x >= vectors[ind].c_end.getPosition().x - vector_circle_radius && sf::Mouse::getPosition(window).x <= vectors[ind].c_end.getPosition().x + vector_circle_radius) &&
						(sf::Mouse::getPosition(window).y >= vectors[ind].c_end.getPosition().y - vector_circle_radius && sf::Mouse::getPosition(window).y <= vectors[ind].c_end.getPosition().y + vector_circle_radius)) {
						moving_end_point = true;
						moving_start_point = false;
						vector_index = ind;			
					}

				}
			}
			else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && window.hasFocus()) {
				if (vector_circle_radius >= 20)
					vector_circle_radius = 5;
				else
					vector_circle_radius *= 2;
				for (auto &vec : vectors) {
					vec.setCircleRadius(vector_circle_radius);
					vector_circle_radius = vec.getCircleRadius();
				}

			}

			else if (event.type == sf::Event::MouseButtonReleased) {
				moving_start_point = false;
				moving_end_point = false;
				window.sf::Window::setMouseCursorVisible(true);
			}
			else if (event.type == sf::Event::KeyReleased) {
				if (event.key.code == sf::Keyboard::V) {
					vectors.push_back(Vector({ 10,10 }, { 10,50 }, vector_circle_radius));
					for (circle_ctr = 0; circle_ctr < ((pow(vectors.size(), 2) - vectors.size()) / 2) - intersection_circles.size(); ++circle_ctr)
						intersection_circles.push_back(circle);
					lines.push_back({ sf::Vector2f{ 10,10 }, sf::Vector2f{ 10,50 } });
				} else if (event.key.code == sf::Keyboard::C) {
					vectors.clear();
					lines.clear();
					intersection_circles.clear();
				}
			}
				
			
		}

		if (vectors.size() > 0) {
			if (moving_start_point) {
				vectors[vector_index].setStartPoint(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
				moving_vector_ind = vector_index;
			}
			else if (moving_end_point) {
				vectors[vector_index].setEndPoint(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
				moving_vector_ind = vector_index;
			}
		}
		

		//check boundaries
		if (moving_start_point || moving_end_point) {
			for (auto &vec : vectors) {
				if (vec.getStartPoint().position.x < 0)
					vec.setStartPoint(0, vec.getStartPoint().position.y);
				if (vec.getEndPoint().position.x < 0)
					vec.setEndPoint(0, vec.getEndPoint().position.y);
				if (vec.getStartPoint().position.y < 0)
					vec.setStartPoint(vec.getStartPoint().position.x, 0);
				if (vec.getEndPoint().position.y < 0)
					vec.setEndPoint(vec.getEndPoint().position.x, 0);

				if (vec.getStartPoint().position.x > WINDOW_WIDTH)
					vec.setStartPoint(WINDOW_WIDTH, vec.getStartPoint().position.y);
				if (vec.getEndPoint().position.x > WINDOW_WIDTH)
					vec.setEndPoint(WINDOW_WIDTH, vec.getEndPoint().position.y);
				if (vec.getStartPoint().position.y > WINDOW_HEIGHT)
					vec.setStartPoint(vec.getStartPoint().position.x, WINDOW_HEIGHT);
				if (vec.getEndPoint().position.y > WINDOW_HEIGHT)
					vec.setEndPoint(vec.getEndPoint().position.x, WINDOW_HEIGHT);
			}
		}


		window.clear();

		for (lines_index = 0; lines_index < lines.size(); ++lines_index) {
			lines[lines_index] = { vectors[lines_index].getStartPoint(), vectors[lines_index].getEndPoint() };
			window.draw(&lines[lines_index][0], lines[lines_index].size(), sf::Lines);
			window.draw(vectors[lines_index].c_start);
			window.draw(vectors[lines_index].c_end);
		}

		if (vectors.size() > 0) {
			for (intersect_vect_ind_outer = 0; intersect_vect_ind_outer < vectors.size() - 1; ++intersect_vect_ind_outer) {
				for (intersect_vect_ind_inner = intersect_vect_ind_outer + 1; intersect_vect_ind_inner < vectors.size(); ++intersect_vect_ind_inner) {
					intersection_x = getIntersection(vectors[intersect_vect_ind_outer].getStartPoint(), vectors[intersect_vect_ind_outer].getEndPoint(), vectors[intersect_vect_ind_inner].getStartPoint(), vectors[intersect_vect_ind_inner].getEndPoint())[0];
					intersection_y = getIntersection(vectors[intersect_vect_ind_outer].getStartPoint(), vectors[intersect_vect_ind_outer].getEndPoint(), vectors[intersect_vect_ind_inner].getStartPoint(), vectors[intersect_vect_ind_inner].getEndPoint())[1];

					for (auto circle : intersection_circles) {
						circle.setPosition(intersection_x, intersection_y);
						window.draw(circle);
					}
				}
			}
		}
		

		window.draw(usage_instructions);

		window.display();

	}

	return 0;
}

