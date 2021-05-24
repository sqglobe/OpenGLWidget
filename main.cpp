#include <iostream>
#include <gtkmm.h>
#include "OpenGLWidget.h"
#include <epoxy/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include "trianglerenderer.h"
#include <memory>

static void GLAPIENTRY MessageCallback( GLenum,
                 GLenum type,
                 GLuint,
                 GLenum severity,
                 GLsizei,
                 const GLchar* message,
                 const void* )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

int main(int argc, char *argv[])
{
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");
    Gtk::Window window;

    window.set_title("Test opengl widget window");
    window.set_default_size(900, 500);
    OpenGLWidget widget;

    window.add(widget);

    std::unique_ptr<TriangleRenderer> renderer;

    widget.add_tick_callback(
                [&widget](const Glib::RefPtr<Gdk::FrameClock>&) -> bool
    {
        widget.draw_content();
        return true;
    });

    widget.signal_init_gl().connect([&renderer]()
    {
        glDebugMessageCallback(MessageCallback, nullptr);
        static std::vector<float> vVertices = {  0.0f,  0.5f, 0.0f,
                                   -0.5f, -0.5f, 0.0f,
                                    0.5f, -0.5f, 0.0f };
        renderer = std::make_unique<TriangleRenderer>(vVertices);

    });

    widget.signal_draw_gl().connect([&renderer]()
    {
        renderer->draw();
    });

    window.show_all();

    return app->run(window);
}

