#include "OpenGLWidget.h"
#include <gdkmm/glcontext.h>
#include <gdkmm/drawingcontext.h>
#include <epoxy/gl.h>
#include <GL/gl.h>

#include <iostream>

OpenGLWidget::OpenGLWidget()
    : Glib::ObjectBase("openglwidget"),
      Gtk::Widget()
{
  set_has_window(true);
  set_name("opengl-widget");
  set_app_paintable(false);
}


sigc::signal<void> OpenGLWidget::signal_draw_gl()
{
    return m_DrawSignal;
}

sigc::signal<void> OpenGLWidget::signal_init_gl()
{
    return m_InitGL;
}

void OpenGLWidget::draw_content()
{
    if(m_refGdkWindow && m_refGdkWindow->is_visible())
    {
        auto region = m_refGdkWindow->get_visible_region();
        auto context = m_refGdkWindow->begin_draw_frame(region);
        draw(context->get_cairo_context());
        m_refGdkWindow->end_draw_frame(context);
    }
}


void OpenGLWidget::get_preferred_width_vfunc(int &minimum_width, int &natural_width) const
{
    minimum_width = 60;
    natural_width = 100;
}

void OpenGLWidget::get_preferred_height_for_width_vfunc(int /*width*/, int &minimum_height, int &natural_height) const
{
    minimum_height = 50;
    natural_height = 70;
}

void OpenGLWidget::get_preferred_height_vfunc(int &minimum_height, int &natural_height) const
{
    minimum_height = 50;
    natural_height = 70;
}

void OpenGLWidget::get_preferred_width_for_height_vfunc(int /*height*/, int &minimum_width, int &natural_width) const
{
    minimum_width = 60;
    natural_width = 100;
}

void OpenGLWidget::on_size_allocate(Gtk::Allocation &allocation)
{
    set_allocation(allocation);
    if(m_refGdkWindow)
    {
      m_refGdkWindow->move_resize( allocation.get_x(), allocation.get_y(),
              allocation.get_width(), allocation.get_height() );
    }
}

void OpenGLWidget::on_realize()
{
    set_realized();

     if(!m_refGdkWindow)
     {
       //Create the GdkWindow:

       GdkWindowAttr attributes;
       memset(&attributes, 0, sizeof(attributes));

       Gtk::Allocation allocation = get_allocation();

       //Set initial position and size of the Gdk::Window:
       attributes.x = allocation.get_x();
       attributes.y = allocation.get_y();
       attributes.width = allocation.get_width();
       attributes.height = allocation.get_height();

       attributes.event_mask = get_events ();
       attributes.window_type = GDK_WINDOW_CHILD;
       attributes.wclass = GDK_INPUT_OUTPUT;

       m_refGdkWindow = Gdk::Window::create(get_parent_window(), &attributes, GDK_WA_X | GDK_WA_Y);
       set_window(m_refGdkWindow);
       register_window(m_refGdkWindow);
     }

     if(!m_Context)
     {
         m_Context = m_refGdkWindow->create_gl_context();
         if(!m_Context->realize())
         {
             std::cerr << "Context realize failed" << std::endl;
             m_Context.reset();
             return;
         }
     }

     m_Context->make_current();
     glGenFramebuffersEXT (1, &frame_buffer);
     glGenRenderbuffersEXT (1, &render_buffer);
     m_InitGL.emit();
}

void OpenGLWidget::on_unrealize()
{
    m_refGdkWindow.reset();
    if(m_Context)
    {
        m_Context->make_current();
        glBindRenderbuffer (GL_RENDERBUFFER, 0);
        glDeleteRenderbuffersEXT (1, &render_buffer);
        glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
        glDeleteFramebuffersEXT (1, &frame_buffer);
        Gdk::GLContext::clear_current();
        //Call base class:
        m_Context.reset();
    }
    Gtk::Widget::on_unrealize();

}

bool OpenGLWidget::on_draw(const Cairo::RefPtr<Cairo::Context> &cr)
{
    if(!m_Context || render_buffer == 0)
    {
        return false;
    }


    bind_buffers();
    if (auto status = glCheckFramebufferStatusEXT (GL_FRAMEBUFFER_EXT);status == GL_FRAMEBUFFER_COMPLETE_EXT)
    {
        m_DrawSignal.emit();
        gdk_cairo_draw_from_gl (cr->cobj(),
                                m_refGdkWindow->gobj(),
                                render_buffer,
                                GL_RENDERBUFFER,
                                m_BufferDimensions->scale, 0, 0, m_BufferDimensions->width, m_BufferDimensions->height);
    }
    else
    {
        std::cerr << "Status of buffers is not complete: " << std::hex << status << std::endl;
    }
    unbind_buffers();
    return true;
}

void OpenGLWidget::on_map()
{
    if(m_refGdkWindow)
    {
        m_refGdkWindow->show();
    }

    Gtk::Widget::on_map();
}

void OpenGLWidget::on_unmap()
{
    if(m_refGdkWindow)
    {
        m_refGdkWindow->hide();
    }

    Gtk::Widget::on_unmap();
}

void OpenGLWidget::bind_buffers()
{
    m_Context->make_current();

    auto scale = get_scale_factor ();

    m_BufferDimensions = {
        scale,
        get_allocated_width () * scale,
        get_allocated_height () * scale
    };


    glBindRenderbuffer (GL_RENDERBUFFER, render_buffer);
    glRenderbufferStorage (GL_RENDERBUFFER, GL_RGB8, m_BufferDimensions->width, m_BufferDimensions->height);
    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, frame_buffer);
    glFramebufferRenderbufferEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                                      GL_RENDERBUFFER_EXT, render_buffer);
    glViewport (0, 0, m_BufferDimensions->width, m_BufferDimensions->height );
}

void OpenGLWidget::unbind_buffers()
{
    m_Context->make_current();
    glBindRenderbuffer (GL_RENDERBUFFER, 0);
    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
    m_Context->clear_current();
}
