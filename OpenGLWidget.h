#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <gtkmm/widget.h>
#include <optional>

class OpenGLWidget: public Gtk::Widget
{

    struct BufferDimension
    {
      int scale;
      int width;
      int height;
    };
public:
    OpenGLWidget();

public:
    sigc::signal<void> signal_draw_gl();
    sigc::signal<void> signal_init_gl();

public:
    void draw_content();
protected:
    void get_preferred_width_vfunc(int& minimum_width, int& natural_width) const override;
    void get_preferred_height_for_width_vfunc(int width, int& minimum_height, int& natural_height) const  override;
    void get_preferred_height_vfunc(int& minimum_height, int& natural_height) const override;
    void get_preferred_width_for_height_vfunc(int height, int& minimum_width, int& natural_width) const override;
    void on_size_allocate(Gtk::Allocation& allocation) override;
    void on_realize() override;
    void on_unrealize() override;
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
    void on_map() override;
    void on_unmap() override;
private:
    void bind_buffers();
    void unbind_buffers();
private:
    Glib::RefPtr<Gdk::Window> m_refGdkWindow;
    Glib::RefPtr< Gdk::GLContext > 	m_Context;
    guint frame_buffer {0};
    guint render_buffer {0};
    std::optional<BufferDimension> m_BufferDimensions;
    sigc::signal<void> m_DrawSignal;
    sigc::signal<void> m_InitGL;
};

#endif // OPENGLWIDGET_H
