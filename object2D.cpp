#include "../tema1/object2D.h"

#include <vector>
#include <math.h>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh *object2D::CreateSquare(
        const std::string &name,
        glm::vec3 leftBottomCorner,
        float length,
        glm::vec3 color,
        bool fill) {
    // Reference Corner
    glm::vec3 corner = leftBottomCorner;

    // Generate the vertices vector
    std::vector <VertexFormat> vertices =
            {
                    VertexFormat(corner, color),
                    VertexFormat(corner + glm::vec3(length, 0, 0), color),
                    VertexFormat(corner + glm::vec3(length, length, 0), color),
                    VertexFormat(corner + glm::vec3(0, length, 0), color)
            };

    // Create a new Mesh
    Mesh *square = new Mesh(name);

    // Generate the indices vectors
    std::vector<unsigned int> indices = {0, 1, 2, 3};

    // Set the Draw Mode according to the Fill option
    if (!fill) {
        glLineWidth(4.0f);
        square->SetDrawMode(GL_LINE_LOOP);
    } else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    // Create the Square according to the generated vertices and indices 
    square->InitFromData(vertices, indices);
    return square;
}

Mesh *
object2D::CreateCircle(const std::string &name, glm::vec3 center, float radius, glm::vec3 color, bool fill) {
    std::vector <VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // Generate the vertices and indices vectors
    for (int i = 0; i < 360; i++) {
        vertices.push_back(
                VertexFormat(center + glm::vec3(radius * cos(i * M_PI / 180), radius * sin(i * M_PI / 180), 0), color));
        indices.push_back(i);
    }

    // Create a new Mesh
    Mesh *circle = new Mesh(name);

    // Set the Draw Mode according to the Fill option
    if (fill) {
        circle->SetDrawMode(GL_TRIANGLE_FAN);
    } else {
        circle->SetDrawMode(GL_POINTS);
    }

    // Create the Circle according to the generated vertices and indices 
    circle->InitFromData(vertices, indices);

    return circle;
}