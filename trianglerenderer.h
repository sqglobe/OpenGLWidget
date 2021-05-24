#ifndef TRIANGLERENDERER_H
#define TRIANGLERENDERER_H

#include <vector>
#include <glm/gtc/matrix_transform.hpp>


class TriangleRenderer
{
public:
    TriangleRenderer(const std::vector<float> &vertices);
    ~TriangleRenderer();

public:
    void draw();

private:
    unsigned int programID;
    int transformLocation;
    unsigned int VAO;
    unsigned int VBO;
    std::size_t m_VerticiesCount;
    float m_Grad{0.0f};
};

#endif // TRIANGLERENDERER_H
