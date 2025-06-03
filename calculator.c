#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// Global variables for calculator state
static GtkWidget *display;
static char current_input[256] = "0";
static char stored_value[256] = "";
static char current_operator = '\0';
static gboolean new_calculation = TRUE;
static gboolean has_decimal = FALSE;

// Function prototypes
static void on_number_clicked(GtkWidget *widget, gpointer data);
static void on_operator_clicked(GtkWidget *widget, gpointer data);
static void on_equals_clicked(GtkWidget *widget, gpointer data);
static void on_clear_clicked(GtkWidget *widget, gpointer data);
static void on_clear_entry_clicked(GtkWidget *widget, gpointer data);
static void on_decimal_clicked(GtkWidget *widget, gpointer data);
static void on_backspace_clicked(GtkWidget *widget, gpointer data);
static void on_sqrt_clicked(GtkWidget *widget, gpointer data);
static void on_percent_clicked(GtkWidget *widget, gpointer data);
static void on_sign_clicked(GtkWidget *widget, gpointer data);
static double calculate(double a, double b, char op);
static void update_display(void);
static GtkWidget *create_button(const char *label, const char *css_class);

// Update the display with current input
static void update_display(void) {
    gtk_entry_set_text(GTK_ENTRY(display), current_input);
}

// Create a styled button
static GtkWidget *create_button(const char *label, const char *css_class) {
    GtkWidget *button = gtk_button_new_with_label(label);
    gtk_widget_set_size_request(button, 80, 60);
    
    // Add CSS class for styling
    GtkStyleContext *context = gtk_widget_get_style_context(button);
    gtk_style_context_add_class(context, css_class);
    
    return button;
}

// Number button click handler
static void on_number_clicked(GtkWidget *widget, gpointer data) {
    int number = GPOINTER_TO_INT(data);
    
    if (new_calculation || strcmp(current_input, "0") == 0) {
        sprintf(current_input, "%d", number);
        new_calculation = FALSE;
    } else {
        if (strlen(current_input) < 15) {  // Prevent overflow
            char temp[256];
            sprintf(temp, "%s%d", current_input, number);
            strcpy(current_input, temp);
        }
    }
    
    update_display();
}

// Operator button click handler
static void on_operator_clicked(GtkWidget *widget, gpointer data) {
    char op = *(char*)data;
    
    if (current_operator != '\0' && !new_calculation) {
        // Perform pending calculation
        double a = atof(stored_value);
        double b = atof(current_input);
        double result = calculate(a, b, current_operator);
        sprintf(current_input, "%.10g", result);
        update_display();
    }
    
    strcpy(stored_value, current_input);
    current_operator = op;
    new_calculation = TRUE;
    has_decimal = FALSE;
}

// Equals button click handler
static void on_equals_clicked(GtkWidget *widget, gpointer data) {
    if (current_operator != '\0' && strlen(stored_value) > 0) {
        double a = atof(stored_value);
        double b = atof(current_input);
        double result = calculate(a, b, current_operator);
        
        sprintf(current_input, "%.10g", result);
        update_display();
        
        current_operator = '\0';
        strcpy(stored_value, "");
        new_calculation = TRUE;
        has_decimal = strchr(current_input, '.') != NULL;
    }
}

// Clear all button click handler
static void on_clear_clicked(GtkWidget *widget, gpointer data) {
    strcpy(current_input, "0");
    strcpy(stored_value, "");
    current_operator = '\0';
    new_calculation = TRUE;
    has_decimal = FALSE;
    update_display();
}

// Clear entry button click handler
static void on_clear_entry_clicked(GtkWidget *widget, gpointer data) {
    strcpy(current_input, "0");
    new_calculation = TRUE;
    has_decimal = FALSE;
    update_display();
}

// Decimal point button click handler
static void on_decimal_clicked(GtkWidget *widget, gpointer data) {
    if (!has_decimal) {
        if (new_calculation) {
            strcpy(current_input, "0.");
            new_calculation = FALSE;
        } else {
            if (strlen(current_input) < 14) {
                strcat(current_input, ".");
            }
        }
        has_decimal = TRUE;
        update_display();
    }
}

// Backspace button click handler
static void on_backspace_clicked(GtkWidget *widget, gpointer data) {
    if (!new_calculation && strlen(current_input) > 1) {
        if (current_input[strlen(current_input) - 1] == '.') {
            has_decimal = FALSE;
        }
        current_input[strlen(current_input) - 1] = '\0';
    } else {
        strcpy(current_input, "0");
        new_calculation = TRUE;
        has_decimal = FALSE;
    }
    update_display();
}

// Square root button click handler
static void on_sqrt_clicked(GtkWidget *widget, gpointer data) {
    double value = atof(current_input);
    if (value >= 0) {
        double result = sqrt(value);
        sprintf(current_input, "%.10g", result);
        has_decimal = strchr(current_input, '.') != NULL;
    } else {
        strcpy(current_input, "Error");
        has_decimal = FALSE;
    }
    new_calculation = TRUE;
    update_display();
}

// Percent button click handler
static void on_percent_clicked(GtkWidget *widget, gpointer data) {
    double value = atof(current_input);
    double result = value / 100.0;
    sprintf(current_input, "%.10g", result);
    has_decimal = strchr(current_input, '.') != NULL;
    new_calculation = TRUE;
    update_display();
}

// Sign change button click handler
static void on_sign_clicked(GtkWidget *widget, gpointer data) {
    if (strcmp(current_input, "0") != 0 && strcmp(current_input, "Error") != 0) {
        if (current_input[0] == '-') {
            // Remove negative sign
            memmove(current_input, current_input + 1, strlen(current_input));
        } else {
            // Add negative sign
            char temp[256];
            sprintf(temp, "-%s", current_input);
            strcpy(current_input, temp);
        }
        update_display();
    }
}

// Perform calculation
static double calculate(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': 
            if (b != 0) return a / b;
            else return 0; // Handle division by zero
        default: return b;
    }
}

// Keyboard event handler
static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data) {
    switch (event->keyval) {
        case GDK_KEY_0: case GDK_KEY_KP_0:
            on_number_clicked(NULL, GINT_TO_POINTER(0));
            break;
        case GDK_KEY_1: case GDK_KEY_KP_1:
            on_number_clicked(NULL, GINT_TO_POINTER(1));
            break;
        case GDK_KEY_2: case GDK_KEY_KP_2:
            on_number_clicked(NULL, GINT_TO_POINTER(2));
            break;
        case GDK_KEY_3: case GDK_KEY_KP_3:
            on_number_clicked(NULL, GINT_TO_POINTER(3));
            break;
        case GDK_KEY_4: case GDK_KEY_KP_4:
            on_number_clicked(NULL, GINT_TO_POINTER(4));
            break;
        case GDK_KEY_5: case GDK_KEY_KP_5:
            on_number_clicked(NULL, GINT_TO_POINTER(5));
            break;
        case GDK_KEY_6: case GDK_KEY_KP_6:
            on_number_clicked(NULL, GINT_TO_POINTER(6));
            break;
        case GDK_KEY_7: case GDK_KEY_KP_7:
            on_number_clicked(NULL, GINT_TO_POINTER(7));
            break;
        case GDK_KEY_8: case GDK_KEY_KP_8:
            on_number_clicked(NULL, GINT_TO_POINTER(8));
            break;
        case GDK_KEY_9: case GDK_KEY_KP_9:
            on_number_clicked(NULL, GINT_TO_POINTER(9));
            break;
        case GDK_KEY_plus: case GDK_KEY_KP_Add:
            on_operator_clicked(NULL, "+");
            break;
        case GDK_KEY_minus: case GDK_KEY_KP_Subtract:
            on_operator_clicked(NULL, "-");
            break;
        case GDK_KEY_asterisk: case GDK_KEY_KP_Multiply:
            on_operator_clicked(NULL, "*");
            break;
        case GDK_KEY_slash: case GDK_KEY_KP_Divide:
            on_operator_clicked(NULL, "/");
            break;
        case GDK_KEY_Return: case GDK_KEY_KP_Enter: case GDK_KEY_equal:
            on_equals_clicked(NULL, NULL);
            break;
        case GDK_KEY_period: case GDK_KEY_KP_Decimal:
            on_decimal_clicked(NULL, NULL);
            break;
        case GDK_KEY_BackSpace:
            on_backspace_clicked(NULL, NULL);
            break;
        case GDK_KEY_Escape:
            on_clear_clicked(NULL, NULL);
            break;
        case GDK_KEY_Delete:
            on_clear_entry_clicked(NULL, NULL);
            break;
    }
    return FALSE;
}

// Apply CSS styling
static void apply_css(void) {
    GtkCssProvider *provider = gtk_css_provider_new();
    const char *css = 
        "window {"
        "    background-color: #2d2d2d;"
        "}"
        ".display {"
        "    font-family: 'Courier New', monospace;"
        "    font-size: 24px;"
        "    font-weight: bold;"
        "    padding: 10px;"
        "    margin: 5px;"
        "    background-color: #1a1a1a;"
        "    color: #ffffff;"
        "    border: 2px solid #404040;"
        "    border-radius: 5px;"
        "}"
        ".number-btn {"
        "    background: linear-gradient(to bottom, #4a4a4a, #3a3a3a);"
        "    color: #ffffff;"
        "    border: 1px solid #555555;"
        "    border-radius: 3px;"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "    margin: 2px;"
        "}"
        ".number-btn:hover {"
        "    background: linear-gradient(to bottom, #5a5a5a, #4a4a4a);"
        "}"
        ".operator-btn {"
        "    background: linear-gradient(to bottom, #ff9500, #e6850e);"
        "    color: #ffffff;"
        "    border: 1px solid #cc7700;"
        "    border-radius: 3px;"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "    margin: 2px;"
        "}"
        ".operator-btn:hover {"
        "    background: linear-gradient(to bottom, #ffaa00, #ff9500);"
        "}"
        ".function-btn {"
        "    background: linear-gradient(to bottom, #666666, #555555);"
        "    color: #ffffff;"
        "    border: 1px solid #777777;"
        "    border-radius: 3px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    margin: 2px;"
        "}"
        ".function-btn:hover {"
        "    background: linear-gradient(to bottom, #777777, #666666);"
        "}"
        ".clear-btn {"
        "    background: linear-gradient(to bottom, #cc0000, #aa0000);"
        "    color: #ffffff;"
        "    border: 1px solid #880000;"
        "    border-radius: 3px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    margin: 2px;"
        "}"
        ".clear-btn:hover {"
        "    background: linear-gradient(to bottom, #dd0000, #cc0000);"
        "}";
    
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    g_object_unref(provider);
}

// Create the main calculator interface
static GtkWidget *create_calculator(void) {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *grid;
    GtkWidget *button;
    
    // Create main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 350, 450);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    
    // Create vertical box container
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    
    // Create display
    display = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(display), "0");
    gtk_entry_set_alignment(GTK_ENTRY(display), 1.0); // Right align
    gtk_editable_set_editable(GTK_EDITABLE(display), FALSE);
    gtk_widget_set_size_request(display, -1, 50);
    GtkStyleContext *display_context = gtk_widget_get_style_context(display);
    gtk_style_context_add_class(display_context, "display");
    gtk_box_pack_start(GTK_BOX(vbox), display, FALSE, FALSE, 0);
    
    // Create button grid
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 3);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 3);
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0);
    
    // Row 0: Memory and Clear functions
    button = create_button("C", "clear-btn");
    g_signal_connect(button, "clicked", G_CALLBACK(on_clear_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 0, 1, 1);
    
    button = create_button("CE", "function-btn");
    g_signal_connect(button, "clicked", G_CALLBACK(on_clear_entry_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 1, 0, 1, 1);
    
    button = create_button("⌫", "function-btn");
    g_signal_connect(button, "clicked", G_CALLBACK(on_backspace_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 2, 0, 1, 1);
    
    button = create_button("÷", "operator-btn");
    g_signal_connect(button, "clicked", G_CALLBACK(on_operator_clicked), "/");
    gtk_grid_attach(GTK_GRID(grid), button, 3, 0, 1, 1);
    
    // Row 1: 7, 8, 9, ×
    button = create_button("7", "number-btn");
    g_signal_connect(button, "clicked", G_CALLBACK(on_number_clicked), GINT_TO_POINTER(7));
    gtk_grid_attach(GTK_GRID(grid), button, 0, 1, 1, 1);
    
    button = create_button("8", "number-btn");
    g_signal_connect(button, "clicked", G_CALLBACK(on_number_clicked), GINT_TO_POINTER(8));
    gtk_grid_attach(GTK_GRID(grid), button, 1, 1, 1, 1);
    
    button = create_button("9", "number-btn");
    g_signal_connect(button, "clicked", G_CALLBACK(on_number_clicked), GINT_TO_POINTER(9));
    gtk_grid_attach(GTK_GRID(grid), button, 2, 1, 1, 1);
    
    button = create_button("×", "operator-btn");
    g_signal_connect(button, "clicked", G_CALLBACK(on_operator_clicked), "*");
    gtk_grid_attach(GTK_GRID(grid), button, 3, 1, 1, 1);
    
    // Row 2: 4, 5, 6, -
    button = create_button("4", "number-btn");
    g_signal_connect(button, "clicked", G_CALLBACK(on_number_clicked), GINT_TO_POINTER(4));
    gtk_grid_attach(GTK_GRID(grid), button, 0, 2, 1, 1);
    
    button = create_button("5", "number-btn");
    g_signal_connect(button, "clicked", G_CALLBACK(on_number_clicked), GINT_TO_POINTER(5));
    gtk_grid_attach(GTK_GRID(grid), button, 1, 2, 1, 1);
    
    button = create_button("6", "number-btn");
    g_signal_connect(button, "clicked", G_CALLBACK(on_number_clicked), GINT_TO_POINTER(6));
    gtk_grid_attach(GTK_GRID(grid), button, 2, 2, 1, 1);
    
    button = create_button("-", "operator-btn");
    g_signal_connect(button, "clicked", G_CALLBACK(on_operator_clicked), "-");
    gtk_grid_attach(GTK_GRID(grid), button, 3, 2, 1, 1);
    
    // Row 3: 1, 2, 3, +
    button = create_button("1", "number-btn");
    g_signal_connect(button, "clicked", G_CALLBACK(on_number_clicked), GINT_TO_POINTER(1));
    gtk_grid_attach(GTK_GRID(grid), button, 0, 3, 1, 1);
    
    button = create_button("2", "number-btn");
    g_signal_connect(button, "clicked", G_CALLBACK(on_number_clicked), GINT_TO_POINTER(2));
    gtk_grid_attach(GTK_GRID(grid), button, 1, 3, 1, 1);
    
    button = create_button("3", "number-btn");
    g_signal_connect(button, "clicked", G_CALLBACK(on_number_clicked), GINT_TO_POINTER(3));
    gtk_grid_attach(GTK_GRID(grid), button, 2, 3, 1, 1);
    
    button = create_button("+", "operator-btn");
    g_signal_connect(button, "clicked", G_CALLBACK(on_operator_clicked), "+");
    gtk_grid_attach(GTK_GRID(grid), button, 3, 3, 1, 1);
    
    // Row 4: ±, 0, ., =
    button = create_button("±", "function-btn");
    g_signal_connect(button, "clicked", G_CALLBACK(on_sign_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 4, 1, 1);
    
    button = create_button("0", "number-btn");
    g_signal_connect(button, "clicked", G_CALLBACK(on_number_clicked), GINT_TO_POINTER(0));
    gtk_grid_attach(GTK_GRID(grid), button, 1, 4, 1, 1);
    
    button = create_button(".", "function-btn");
    g_signal_connect(button, "clicked", G_CALLBACK(on_decimal_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 2, 4, 1, 1);
    
    button = create_button("=", "operator-btn");
    g_signal_connect(button, "clicked", G_CALLBACK(on_equals_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 3, 4, 1, 1);
    
    // Row 5: Advanced functions
    button = create_button("√", "function-btn");
    g_signal_connect(button, "clicked", G_CALLBACK(on_sqrt_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 5, 1, 1);
    
    button = create_button("%", "function-btn");
    g_signal_connect(button, "clicked", G_CALLBACK(on_percent_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), button, 1, 5, 1, 1);
    
    // Connect keyboard events
    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), NULL);
    gtk_widget_set_can_focus(window, TRUE);
    
    // Connect window close event
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    return window;
}

// Main function
int main(int argc, char *argv[]) {
    GtkWidget *window;
    
    // Initialize GTK
    gtk_init(&argc, &argv);
    
    // Apply CSS styling
    apply_css();
    
    // Create calculator window
    window = create_calculator();
    
    // Show all widgets
    gtk_widget_show_all(window);
    
    // Start GTK main loop
    gtk_main();
    
    return 0;
}