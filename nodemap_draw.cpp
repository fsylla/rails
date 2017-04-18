
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

#include "destseq.h"
#include "nodemap.h"


#define MAP_SCALE                      49.2
#define MAP_SNAP                        0.05

#define MODE_NONE                       0
#define MODE_NODE                       1
#define MODE_LINE                       2
#define MODE_ARC                        3
#define MODE_DEMO                       4

#define SIZE_TRAINS                     4


struct glob_t
{
    cairo_surface_t *image;
    GtkWidget       *pmenu;
    int             img_h;
    int             img_w;
    uint16_t        idNear;             // node id near mouse pointer
    uint16_t        idSel;              // node id selected
    uint16_t        dest[SIZE_TRAINS];
    uint8_t         back;               // flag background display
    uint8_t         cross;              // flag hair cross display
    uint8_t         num;                // flag node numbers display
    uint8_t         mode;               // one of MODE_* values
    uint8_t         step;               // step counter for MODE_ARC
    double          arc_r;              // arc radius
    double          arc_x;              // arc x position (mouse coordinates)
    double          arc_y;              // arc y position (mouse coordinates)
    double          mouse_x;
    double          mouse_y;
    double          bottom;             // screen bottom (map coordinates)
    double          top;                // screen top    (map coordinates)
    double          height;             // screen height (map coordinates)
    double          width;              // screen width  (map coordinates)
    double          scale;              // mouse units per map unit
    double          snap;               // near distance (map coordinates)
} glob;


void dump()
{
    printf("img_h      = %5d\n", glob.img_h);
    printf("img_w      = %5d\n", glob.img_w);
    printf("mouse_x    = %8.2f\n", glob.mouse_x);
    printf("mouse_y    = %8.2f\n", glob.mouse_y);
    printf("bottom     = %8.2f\n", glob.bottom);
    printf("top        = %8.2f\n", glob.top);
    printf("height     = %8.2f\n", glob.height);
    printf("width      = %8.2f\n", glob.width);
    printf("\n");
}


double snap(double x)
{
    return(glob.snap * round(x / glob.snap));
}


void refresh(GtkWidget *widget)
{
    int                     width   = gtk_widget_get_allocated_width(widget);
    int                     height  = gtk_widget_get_allocated_height(widget);

    /* Now invalidate the affected region of the drawing area. */
    gtk_widget_queue_draw_area(widget, 0, 0, width, height);
    printf(".");                        // linux only hack !!!
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
    cairo_line_to(cr, glob.width, glob.mouse_y);
    cairo_stroke(cr);

    cairo_move_to(cr, glob.mouse_x, glob.bottom);
    cairo_line_to(cr, glob.mouse_x, glob.top);
    cairo_stroke(cr);
}


void draw_line(cairo_t *cr, double x1, double y1, double x2, double y2)
{
    cairo_move_to(cr, x1, y1);
    cairo_line_to(cr, x2, y2);
    cairo_stroke(cr);
}


void draw_arc(cairo_t *cr, double xc, double yc, double r, double a1, double a2)
{
    if (r > 0) {
        cairo_arc(cr, xc, yc, r, a1, a2);
    } else {
        cairo_arc_negative(cr, xc, yc, -r, a1, a2);
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
    
    double                  a1;
    double                  a2;
    double                  a3;
    double                  dx;
    double                  dy;
    double                  x       = snap(event->x / glob.scale);
    double                  y       = snap((glob.img_h - event->y) / glob.scale);

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
                        glob.arc_x  = x;
                        glob.arc_y  = y;
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
                            dx      = nodeMap->nodeGetX(glob.idSel) - glob.arc_x;
                            dy      = nodeMap->nodeGetY(glob.idSel) - glob.arc_y;
                            a1      = atan2(dy, dx);

                            dx      = nodeMap->nodeGetX(glob.idNear) - glob.arc_x;
                            dy      = nodeMap->nodeGetY(glob.idNear) - glob.arc_y;

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
        refresh(widget);
    }
    else if (event->button == GDK_BUTTON_SECONDARY)
    {
        dump();
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

    int                     width   = gtk_widget_get_allocated_width(widget);
    int                     height  = gtk_widget_get_allocated_height(widget);

    glob.mouse_x        = event->x / glob.scale;
    glob.mouse_y        = (glob.img_h - event->y) / glob.scale;
    glob.bottom         = (glob.img_h - height) / glob.scale;
    glob.top            = glob.img_h / glob.scale;
    glob.height         = height / glob.scale;
    glob.width          = width / glob.scale;
    glob.idNear         = 0;

    if (glob.mode == MODE_ARC && glob.step == 1) {
        glob.arc_x      = glob.mouse_x;
        glob.arc_y      = glob.mouse_y;

        double dx       = nodeMap->nodeGetX(glob.idSel) - glob.arc_x;
        double dy       = nodeMap->nodeGetY(glob.idSel) - glob.arc_y;
        glob.arc_r      = sqrt(dx * dx + dy * dy);
    }

    for (std::map<uint16_t,Node>::iterator it = nodes->begin(); it != nodes->end(); ++it) {
        if (fabs(it->second.getX() - glob.mouse_x) < 0.1) {
            if (fabs(it->second.getY() - glob.mouse_y) < 0.1) {
                glob.idNear = it->first;
            }
        }
    }
            
    /* Now invalidate the affected region of the drawing area. */
    gtk_widget_queue_draw_area(widget, 0, 0, width, height);

    return TRUE;
}


static gboolean on_back_event(GtkWidget *widget, NodeMap *nodeMap)
{
    glob.back   = 1 - glob.back;
    glob.cross  = 1;

    return FALSE;
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


static gboolean on_demo_event(GtkWidget *widget, NodeMap *nodeMap)
{
    glob.mode   = MODE_DEMO;
    glob.num    = 0;

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

    int                     width   = gtk_widget_get_allocated_width(widget);
    int                     height  = gtk_widget_get_allocated_height(widget);

    if (glob.back) {
        cairo_set_source_surface(cr, glob.image, 0, 0);
        cairo_paint(cr);
    }

    cairo_set_line_width(cr, 0.04);
    cairo_set_source_rgb(cr, 1, 0, 0);

    cairo_translate(cr, 0, glob.img_h);
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
            draw_line(cr, it->second.getX(1), it->second.getY(1), it->second.getX(2), it->second.getY(2));
        } else {
            draw_arc(cr, it->second.getX(0), it->second.getY(0), it->second.getR(), it->second.getA(1), it->second.getA(2));
        }
    }

    cairo_set_source_rgb(cr, 1, 0, 0);

    switch (glob.mode) {
        case MODE_DEMO:
            for (std::map<uint16_t,Rail>::iterator it = rails->begin(); it != rails->end(); ++it) {
                if (it->second.getA(3) != it->second.getA(4) ||
                    it->second.getX(3) != it->second.getX(4) ||
                    it->second.getY(3) != it->second.getY(4)) 
                {
                    if (it->second.getR() == 0) {
                        draw_line(cr, it->second.getX(3), it->second.getY(3), it->second.getX(4), it->second.getY(4));
                    } else {
                        draw_arc(cr, it->second.getX(0), it->second.getY(0), it->second.getR(), it->second.getA(3), it->second.getA(4));
                    }
                }
            }

            break;

        case MODE_LINE:
            if (glob.idSel) {
                draw_line(cr, nodeMap->nodeGetX(glob.idSel), nodeMap->nodeGetY(glob.idSel), glob.mouse_x, glob.mouse_y);
            }

            break;
    
        case MODE_ARC:
            if (glob.step) {
                draw_circle(cr);
            }

            break;
    }

    if (glob.cross) {
        draw_cross(cr);
    }

    if (glob.num) {
        draw_numbers(cr, nodeMap);
    }
    
    return FALSE;
}


gboolean time_handler(NodeMap *nodeMap)
{
    if (glob.mode == MODE_DEMO) {
        for (int i = 0; i < SIZE_TRAINS; ++i) {
            if (nodeMap->trainRun(i + 1) == done) {
                glob.dest[i]    = destNxt[glob.dest[i]];

                if (glob.dest[i]) {
                    nodeMap->trainSetDest(i + 1, glob.dest[i]);
                }
            }
        }
    }
    
    return TRUE;
}


int main (int argc, char *argv[])
{
    GtkWidget   *window;
    GtkWidget   *darea;
    GtkWidget   *itemBack;
    GtkWidget   *itemNode;
    GtkWidget   *itemNum;
    GtkWidget   *itemLine;
    GtkWidget   *itemArc;
    GtkWidget   *itemDemo;
    GtkWidget   *itemSave;
    GtkWidget   *itemQuit;

    NodeMap     *nodeMap;

    printf("initializing nodemap\n");

    nodeMap = new NodeMap();
    nodeMap->nodesLoad("nodes.txt");
    nodeMap->hopsLoad("hops.txt");
    nodeMap->railsLoad("rails.txt");

    nodeMap->trainAdd(95, 124, 3);
    nodeMap->trainAdd(81, 82, 4);
    nodeMap->trainAdd(64, 62, 4);
    nodeMap->trainAdd(70, 113, 3);

    gtk_init(&argc, &argv);
    initDestSeq();

    window      = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    darea       = gtk_drawing_area_new();

    gtk_container_add(GTK_CONTAINER(window), darea);
    
    glob.image  = cairo_image_surface_create_from_png("data/rails000.png");
    glob.img_h  = cairo_image_surface_get_height(glob.image);
    glob.img_w  = cairo_image_surface_get_width(glob.image);
    glob.back   = 1;
    glob.cross  = 1;
    glob.dest[0]   = 95;
    glob.dest[1]   = 81;
    glob.dest[2]   = 64;
    glob.dest[3]   = 70;
    glob.num    = 1;
    glob.mode   = MODE_NODE;
    glob.pmenu  = gtk_menu_new();
    glob.scale  = MAP_SCALE;
    glob.snap   = MAP_SNAP;
    glob.step   = 0;

    itemBack    = gtk_menu_item_new_with_label("Background");
    gtk_widget_show(itemBack);
    gtk_menu_shell_append(GTK_MENU_SHELL(glob.pmenu), itemBack);
  
    itemNum     = gtk_menu_item_new_with_label("Numbers");
    gtk_widget_show(itemNum);
    gtk_menu_shell_append(GTK_MENU_SHELL(glob.pmenu), itemNum);
  
    itemNode    = gtk_menu_item_new_with_label("Node");
    gtk_widget_show(itemNode);
    gtk_menu_shell_append(GTK_MENU_SHELL(glob.pmenu), itemNode);
  
    itemLine    = gtk_menu_item_new_with_label("Line");
    gtk_widget_show(itemLine);
    gtk_menu_shell_append(GTK_MENU_SHELL(glob.pmenu), itemLine);
  
    itemArc     = gtk_menu_item_new_with_label("Arc");
    gtk_widget_show(itemArc);
    gtk_menu_shell_append(GTK_MENU_SHELL(glob.pmenu), itemArc);
  
    itemDemo    = gtk_menu_item_new_with_label("Demo");
    gtk_widget_show(itemDemo);
    gtk_menu_shell_append(GTK_MENU_SHELL(glob.pmenu), itemDemo);
  
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

    g_signal_connect(G_OBJECT(itemBack), "activate", G_CALLBACK(on_back_event), nodeMap);
    g_signal_connect(G_OBJECT(itemNode), "activate", G_CALLBACK(on_node_event), nodeMap);
    g_signal_connect(G_OBJECT(itemNum),  "activate", G_CALLBACK(on_num_event),  nodeMap);
    g_signal_connect(G_OBJECT(itemLine), "activate", G_CALLBACK(on_line_event), nodeMap);
    g_signal_connect(G_OBJECT(itemArc),  "activate", G_CALLBACK(on_arc_event),  nodeMap);
    g_signal_connect(G_OBJECT(itemDemo), "activate", G_CALLBACK(on_demo_event), nodeMap);
    g_signal_connect(G_OBJECT(itemSave), "activate", G_CALLBACK(on_save_event), nodeMap);
    g_signal_connect(G_OBJECT(itemQuit), "activate", G_CALLBACK(gtk_main_quit), NULL);  

    g_timeout_add(40, (GSourceFunc) time_handler, nodeMap);
    g_timeout_add(40, (GSourceFunc) refresh, darea);
    
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), glob.img_w, glob.img_h);
    gtk_window_set_title(GTK_WINDOW(window), "nodemap_draw");

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}

