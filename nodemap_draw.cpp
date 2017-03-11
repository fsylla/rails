
/* ____________________________________________________________________________
 *
 * file:        nodemap_draw.cpp
 * created:     Mo 23. Jan 17:51:52 CET 2017
 * by:          frank
 *
 * description: simple nodemap drawing utility
 * ____________________________________________________________________________
 */


#include <cairo.h>
#include <gtk/gtk.h>
#include <math.h>

#include "nodemap.h"


#define MAP_HEIGHT                    720
#define MAP_WIDTH                    1280
#define MAP_SCALE                      49.2
#define MAP_SNAP                        0.05
#define MAP_OFFSET_X                    0.26
#define MAP_OFFSET_Y                    0.53

#define MODE_NONE                       0
#define MODE_NODE                       1
#define MODE_LINE                       2
#define MODE_ARC                        3


struct glob_t
{
    cairo_surface_t *image;
    GtkWidget       *pmenu;
    uint16_t        idNear;             // node id near mouse pointer
    uint16_t        idSel;              // node id selected
    uint8_t         cross;              // flag hair cross display
    uint8_t         num;                // flag node numbers display
    uint8_t         mode;               // one of MODE_* values
    uint8_t         step;               // step counter for MODE_ARC
    double          arc_r;              // arc radius
    double          arc_x;              // arc x position (mouse coordinates)
    double          arc_y;              // arc y position (mouse coordinates)
    double          mouse_x;
    double          mouse_y;
    double          height;             // screen height (map coordinates)
    double          width;              // screen height (map coordinates)
    double          scale;              // mouse units per map unit
    double          snap;               // near distance (map coordinates)
} glob;


double snap(double x)
{
    return(glob.snap * round(x / glob.snap));
}


void refresh(GtkWidget *widget)
{
    GtkWidget   *win    = gtk_widget_get_toplevel(widget);

    int width, height;
    gtk_window_get_size(GTK_WINDOW(win), &width, &height);

    /* Now invalidate the affected region of the drawing area. */
    gtk_widget_queue_draw_area(widget, 0, 0, width, height);
}


void draw_circle(cairo_t *cr)
{
    cairo_set_line_width(cr, 0.02);
    cairo_set_source_rgb(cr, 0, 0, 0);

    cairo_arc(cr, glob.arc_x, glob.arc_y, glob.arc_r, 0, 2 * M_PI);
    cairo_stroke(cr);
}


void draw_cross(cairo_t *cr)
{
    cairo_set_line_width(cr, 0.02);
    cairo_set_source_rgb(cr, 0, 0, 0);

    cairo_move_to(cr, 0, glob.mouse_y);
    cairo_line_to(cr, glob.width + 10, glob.mouse_y);
    cairo_stroke(cr);

    cairo_move_to(cr, glob.mouse_x, 0);
    cairo_line_to(cr, glob.mouse_x, glob.height + 10);
    cairo_stroke(cr);
}


void draw_line(cairo_t *cr, double x1, double y1, double x2, double y2)
{
    cairo_move_to(cr, x1 + MAP_OFFSET_X, y1 + MAP_OFFSET_Y);
    cairo_line_to(cr, x2 + MAP_OFFSET_X, y2 + MAP_OFFSET_Y);
    cairo_stroke(cr);
}


void draw_arc(cairo_t *cr, double xc, double yc, double r, double a1, double a2)
{
    if (r > 0) {
        cairo_arc(cr, xc + MAP_OFFSET_X, yc + MAP_OFFSET_Y, r, a1, a2);
    } else {
        cairo_arc_negative(cr, xc + MAP_OFFSET_X, yc + MAP_OFFSET_Y, -r, a1, a2);
    }

    cairo_stroke(cr);
}


void draw_numbers(cairo_t *cr, NodeMap *nodeMap)
{
    std::map<uint16_t,Node> *nodes  = nodeMap->getNodes();
    char                    number[8];
    cairo_matrix_t          matrix;

  cairo_matrix_init(&matrix,
      0.3,  0.0,
      0.0, -0.3,
      0.0,  0.0);

    cairo_set_font_matrix(cr, &matrix);
    cairo_set_source_rgb(cr, 0, 0, 0);

    for (std::map<uint16_t,Node>::iterator it = nodes->begin(); it != nodes->end(); ++it) {
        sprintf(number, "%d", it->first);
        cairo_move_to(cr, it->second.getX(), it->second.getY());
        cairo_show_text(cr, number);
        cairo_stroke(cr);
    }
}


static gboolean on_button_press_event(GtkWidget *widget, GdkEventButton *event, NodeMap *nodeMap)
{
    std::map<uint16_t,Node> *nodes  = nodeMap->getNodes();
    GtkWidget               *win    = gtk_widget_get_toplevel(widget);
    int                     height;
    int                     width;

    double                  a1;
    double                  a2;
    double                  a3;
    double                  dx;
    double                  dy;
    double                  x       = snap(event->x / glob.scale - MAP_OFFSET_X);
    double                  y       = snap(glob.height - event->y / glob.scale - MAP_OFFSET_Y);

    gtk_window_get_size(GTK_WINDOW(win), &width, &height);

    if (event->button == GDK_BUTTON_PRIMARY) {
        switch (glob.mode) {
            case MODE_NODE:
                if (glob.idNear) {
                    glob.idSel     = glob.idNear;
                } else {
                    glob.idSel     = nodeMap->nodeAdd(x, y);
                }

                break;

            case MODE_LINE:
                if (glob.idSel) {
                    if (glob.idNear == 0) {
                        glob.idNear = nodeMap->nodeAdd(x, y);
                    }

                    if (glob.idSel != glob.idNear) {
                        nodeMap->nodeLink(glob.idSel, glob.idNear, 1);
                        nodeMap->railAdd(glob.idSel, glob.idNear, 0);
                        glob.idSel = glob.idNear;
                    }
                } else {
                    if (glob.idNear) {
                        glob.idSel = glob.idNear;
                    } else {
                        glob.idSel = nodeMap->nodeAdd(x, y);
                    }
                }

                break;

            case MODE_ARC:
                switch (glob.step) {
                    case 0:
                        if (glob.idNear) {
                            glob.idSel = glob.idNear;
                        } else {
                            glob.idSel = nodeMap->nodeAdd(x, y);
                        }

                        glob.arc_r  = 0;
                        glob.arc_x  = x + MAP_OFFSET_X;
                        glob.arc_y  = y + MAP_OFFSET_Y;
                        glob.step   = 1;

                        break;

                    case 1:
                        glob.step   = 2;
                        break;

                    case 2:
                        if (glob.idNear == 0) {
                            glob.idNear = nodeMap->nodeAdd(x, y);
                        }

                        if (glob.idSel != glob.idNear) {
                            dx      = nodeMap->nodeGetX(glob.idSel) + MAP_OFFSET_X - glob.arc_x;
                            dy      = nodeMap->nodeGetY(glob.idSel) + MAP_OFFSET_Y - glob.arc_y;
                            a1      = atan2(dy, dx);

                            dx      = nodeMap->nodeGetX(glob.idNear) + MAP_OFFSET_X - glob.arc_x;
                            dy      = nodeMap->nodeGetY(glob.idNear) + MAP_OFFSET_Y - glob.arc_y;

                            a2      = atan2(dy, dx);

                            a3      = a2 - a1;

                            if (a3 < 0) {
                                a3  = 2 * M_PI + a3;
                            }

                            printf("a1 = %f, a2 = %f\n", 180 * a1 / M_PI, 180 * a2 / M_PI);
                            
                            nodeMap->nodeLink(glob.idSel, glob.idNear, 1);

                            if (a3 < M_PI) {
                                nodeMap->railAdd(glob.idSel, glob.idNear, glob.arc_r);
                            } else {
                                nodeMap->railAdd(glob.idSel, glob.idNear, -glob.arc_r);
                            }

                            glob.idSel = glob.idNear;
                        }

                        glob.step   = 0;
                        break;
                }

                break;

        }

        printf("node %d: x = %6.2f y = %6.2f\n", glob.idSel, x, y);
        gtk_widget_queue_draw_area(widget, 0, 0, width, height);
    }
    else if (event->button == GDK_BUTTON_SECONDARY)
    {
        glob.cross  = 0;
        glob.idNear = 0;
        glob.idSel = 0;

        refresh(widget);
        gtk_menu_popup(GTK_MENU(glob.pmenu), NULL, NULL, NULL, NULL, event->button, event->time);
    }

    return TRUE;
}


static gboolean on_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, NodeMap *nodeMap)
{
    std::map<uint16_t,Node> *nodes  = nodeMap->getNodes();
    GtkWidget               *win    = gtk_widget_get_toplevel(widget);

    int width, height;
    gtk_window_get_size(GTK_WINDOW(win), &width, &height);

    glob.mouse_x        = event->x / glob.scale;
    glob.mouse_y        = (MAP_HEIGHT - event->y) / glob.scale;
    glob.height         = height / glob.scale;
    glob.width          = width / glob.scale;
    glob.idNear         = 0;

    if (glob.mode == MODE_ARC && glob.step == 1) {
        glob.arc_x      = glob.mouse_x;
        glob.arc_y      = glob.mouse_y;

        double dx       = nodeMap->nodeGetX(glob.idSel) + MAP_OFFSET_X - glob.arc_x;
        double dy       = nodeMap->nodeGetY(glob.idSel) + MAP_OFFSET_Y - glob.arc_y;
        glob.arc_r      = sqrt(dx * dx + dy * dy);
    }

    for (std::map<uint16_t,Node>::iterator it = nodes->begin(); it != nodes->end(); ++it) {
        if (fabs(it->second.getX() + MAP_OFFSET_X - glob.mouse_x) < 0.1) {
            if (fabs(it->second.getY() + MAP_OFFSET_Y - glob.mouse_y) < 0.1) {
                glob.idNear = it->first;
            }
        }
    }
            
    /* Now invalidate the affected region of the drawing area. */
    gtk_widget_queue_draw_area(widget, 0, 0, width, height);

    return TRUE;
}


static gboolean on_node_event(GtkWidget *widget, NodeMap *nodeMap)
{
    glob.mode   = MODE_NODE;
    glob.cross  = 1;

    return FALSE;
}


static gboolean on_num_event(GtkWidget *widget, NodeMap *nodeMap)
{
    glob.num    = 1 - glob.num;
    glob.cross  = 1;

    return FALSE;
}


static gboolean on_line_event(GtkWidget *widget, NodeMap *nodeMap)
{
    glob.mode   = MODE_LINE;
    glob.cross  = 1;

    return FALSE;
}


static gboolean on_arc_event(GtkWidget *widget, NodeMap *nodeMap)
{
    glob.mode   = MODE_ARC;
    glob.cross  = 1;

    return FALSE;
}


static gboolean on_save_event(GtkWidget *widget, NodeMap *nodeMap)
{
    nodeMap->nodesSave("nodes.txt");
    nodeMap->railsSave("rails.txt");

    glob.cross  = 1;

    return FALSE;
}


static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, NodeMap *nodeMap)
{
    std::map<uint16_t,Node> *nodes  = nodeMap->getNodes();
    std::map<uint16_t,Rail> *rails  = nodeMap->getRails();
    GtkWidget               *win    = gtk_widget_get_toplevel(widget);

    int width, height;
    gtk_window_get_size(GTK_WINDOW(win), &width, &height);

    cairo_set_source_surface(cr, glob.image, 10, 10);
    cairo_paint(cr);
    cairo_set_line_width(cr, 0.04);
    cairo_set_source_rgb(cr, 1, 0, 0);

    cairo_translate(cr, 0, MAP_HEIGHT);
    cairo_scale(cr, glob.scale, -glob.scale);

    for (std::map<uint16_t,Node>::iterator it = nodes->begin(); it != nodes->end(); ++it) {
        draw_arc(cr, it->second.getX(), it->second.getY(), 0.04, 0, 2 * M_PI);
    }

//  Draw node in green color if near enough for selection 

    if (glob.idNear) {
        cairo_set_source_rgb(cr, 0, 1, 0);
        draw_arc(cr, nodeMap->nodeGetX(glob.idNear), nodeMap->nodeGetY(glob.idNear), 0.04, 0, 2 * M_PI);
    }

    cairo_set_source_rgb(cr, 1, 1, 0);

    for (std::map<uint16_t,Rail>::iterator it = rails->begin(); it != rails->end(); ++it) {
        if (it->second.getR() == 0) {
            draw_line(cr, it->second.getX1(), it->second.getY1(), it->second.getX2(), it->second.getY2());
        } else {
            draw_arc(cr, it->second.getXc(), it->second.getYc(), it->second.getR(), it->second.getA1(), it->second.getA2());
        }
    }

    if (glob.mode == MODE_LINE) {
        if (glob.idSel) {
            draw_line(cr, nodeMap->nodeGetX(glob.idSel), nodeMap->nodeGetY(glob.idSel), glob.mouse_x - MAP_OFFSET_X, glob.mouse_y - MAP_OFFSET_Y);
        }
    }

    if (glob.mode == MODE_ARC) {
        if (glob.step) {
            draw_circle(cr);
        }
    }

    if (glob.cross) {
        draw_cross(cr);
    }

    if (glob.num) {
        draw_numbers(cr, nodeMap);
    }

    return FALSE;
}


int main (int argc, char *argv[])
{
    GtkWidget   *window;
    GtkWidget   *darea;
    GtkWidget   *itemNode;
    GtkWidget   *itemNum;
    GtkWidget   *itemLine;
    GtkWidget   *itemArc;
    GtkWidget   *itemSave;
    GtkWidget   *itemQuit;

    NodeMap     *nodeMap;

    printf("initializing nodemap\n");

    nodeMap = new NodeMap();
    nodeMap->nodesLoad("nodes.txt");
    nodeMap->railsLoad("rails.txt");

    gtk_init(&argc, &argv);

    window      = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    darea       = gtk_drawing_area_new();

    gtk_container_add(GTK_CONTAINER(window), darea);
    
    glob.image  = cairo_image_surface_create_from_png("data/rails000.png");
    glob.cross  = 1;
    glob.num    = 1;
    glob.mode   = MODE_NODE;
    glob.pmenu  = gtk_menu_new();
    glob.scale  = MAP_SCALE;
    glob.snap   = MAP_SNAP;
    glob.step   = 0;

    itemNode    = gtk_menu_item_new_with_label("Node");
    gtk_widget_show(itemNode);
    gtk_menu_shell_append(GTK_MENU_SHELL(glob.pmenu), itemNode);
  
    itemNum     = gtk_menu_item_new_with_label("Numbers");
    gtk_widget_show(itemNum);
    gtk_menu_shell_append(GTK_MENU_SHELL(glob.pmenu), itemNum);
  
    itemLine    = gtk_menu_item_new_with_label("Line");
    gtk_widget_show(itemLine);
    gtk_menu_shell_append(GTK_MENU_SHELL(glob.pmenu), itemLine);
  
    itemArc     = gtk_menu_item_new_with_label("Arc");
    gtk_widget_show(itemArc);
    gtk_menu_shell_append(GTK_MENU_SHELL(glob.pmenu), itemArc);
  
    itemSave    = gtk_menu_item_new_with_label("Save");
    gtk_widget_show(itemSave);
    gtk_menu_shell_append(GTK_MENU_SHELL(glob.pmenu), itemSave);
  
    itemQuit    = gtk_menu_item_new_with_label("Quit");
    gtk_widget_show(itemQuit);
    gtk_menu_shell_append(GTK_MENU_SHELL(glob.pmenu), itemQuit);
  
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(darea), "draw", G_CALLBACK(on_draw_event), nodeMap);
    g_signal_connect(G_OBJECT(darea), "motion-notify-event", G_CALLBACK(on_motion_notify_event), nodeMap);
    g_signal_connect(G_OBJECT(darea), "button-press-event", G_CALLBACK(on_button_press_event), nodeMap);

    /* Ask to receive events the drawing area doesn't normally
     * subscribe to. In particular, we need to ask for the
     * button press and motion notify events that want to handle.
     */
    gtk_widget_set_events(darea, gtk_widget_get_events(darea)
                          | GDK_BUTTON_PRESS_MASK
                          | GDK_POINTER_MOTION_MASK);

    g_signal_connect(G_OBJECT(itemNode), "activate", G_CALLBACK(on_node_event), nodeMap);
    g_signal_connect(G_OBJECT(itemNum),  "activate", G_CALLBACK(on_num_event),  nodeMap);
    g_signal_connect(G_OBJECT(itemLine), "activate", G_CALLBACK(on_line_event), nodeMap);
    g_signal_connect(G_OBJECT(itemArc),  "activate", G_CALLBACK(on_arc_event),  nodeMap);
    g_signal_connect(G_OBJECT(itemSave), "activate", G_CALLBACK(on_save_event), nodeMap);
    g_signal_connect(G_OBJECT(itemQuit), "activate", G_CALLBACK(gtk_main_quit), NULL);  

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), MAP_WIDTH, MAP_HEIGHT);
    gtk_window_set_title(GTK_WINDOW(window), "nodemap_draw");

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}

