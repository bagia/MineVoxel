#ifndef TURTLE_H
#define TURTLE_H

typedef struct
{
    int x, y;
} turtle_xy_position;

void turtle_init(long max_steps);
void turtle_free();
void turtle_print_actions();
void turtle_set_position(int x, int y);
turtle_xy_position turtle_get_position();
void turtle_set_slot(short slot_number);
unsigned short turtle_get_slot();
void turtle_pivot_ahead();
void turtle_move_forward_times(int times);
void turtle_move_to(turtle_xy_position to);
void turtle_forward();
void turtle_left();
void turtle_right();
void turtle_up();
void turtle_down();
void turtle_reload ();
void turtle_place();
void turtle_define_functions();

#endif
