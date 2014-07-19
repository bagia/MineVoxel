#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "turtle.h"

static turtle_xy_position turtle_position;
static long turtle_z = 0;
static int turnLeft = 0;
static int turnRight = 0;
static unsigned short slot = 1;
static int blocksPlaced = 0;
static char ** actions;
static long last_action = -1;

void turtle_add_action(const char* action, ...)
{
    va_list args;
    char *str = malloc(50 * sizeof(char));

    va_start(args, action);
    vsprintf(str, action, args);
    va_end(args);

    actions[++last_action] = str;
}

void turtle_print_n_times(const char* action, long n)
{
    if (n == 0)
        return;

    if (n > 1)
        printf("for i = 0, %ld, 1 do\n", n - 1);

    printf(action);

    if (n > 1)
        printf("end\n");
}

void turtle_print_actions()
{
    long i;
    long f = 0;
    long fp = 0;

    for(i=0;i<=last_action;i++)
    {
        const char *action = actions[i];
        char *next_action;
        if (i != last_action)
            next_action = actions[i+1];
        else
            next_action = NULL;

        if (strcmp(action,"f()\n") == 0 && strcmp(next_action,"f()\n") == 0)
        {
            if (fp > 0)
            {
                turtle_print_n_times("f()\np()\n", fp);
                fp = 0;
            }
            f += 2;
            i++;
            continue;
        }
        else if (strcmp(action,"f()\n") == 0 && strcmp(next_action,"p()\n") == 0)
        {
            if (f > 0)
            {
                turtle_print_n_times("f()\n", f);
                f = 0;
            }
            fp++;
            i++;
            continue;
        }
        else
        {
            if (fp > 0)
            {
                turtle_print_n_times("f()\np()\n", fp);
                fp = 0;
            }
            if (f > 0)
            {
                turtle_print_n_times("f()\n", f);
                f = 0;
            }
            printf(action);
        }

    } // for each action

    turtle_print_n_times("f()\n\tp()\n", fp);
    turtle_print_n_times("f()\n", f);

}

void turtle_init(long max_steps)
{
    long i;
    actions = malloc(max_steps * sizeof(char*));
    for(i=0;i<max_steps;i++) actions[i] = NULL;
    turtle_position.x = 0;
    turtle_position.y = 0;
}

void turtle_free()
{
    long i;
    for(i=0;i<=last_action;i++) free(actions[i]);
    free(actions);
}

void turtle_set_position(int x, int y)
{
    turtle_position.x = x;
    turtle_position.y = y;

}

turtle_xy_position turtle_get_position()
{
    return turtle_position;
}

void turtle_set_slot(short slot_number)
{
    slot = slot_number;
}
unsigned short turtle_get_slot()
{
    return slot;
}

void turtle_pivot_ahead()
{
    // Compensate the turns
    turnLeft %= 4;
    turnRight %= 4;
    int turn_to_left = turnLeft - turnRight;
    if (abs(turn_to_left) > 2)
    {
        turn_to_left *= -1;
        turn_to_left %= 2;
    }

    // Execute the turns
    if (turn_to_left != 0)
    {
        if (abs(turn_to_left) > 1)
            turtle_add_action("for i = 0, %d, 1 do\n\t", abs(turn_to_left) -1);

        if (turn_to_left > 0)
        {
            turtle_left();
        }
        else
        {
            turtle_right();
        }

        if (abs(turn_to_left) > 1)
            turtle_add_action("end\n");
    }

    // Reinitialize the turn counts
    turnLeft = 0;
    turnRight = 0;
}

void turtle_move_forward_times(int times)
{
    int n;
    times = abs(times);

    // Get the right orientation if needed
    turtle_pivot_ahead();

    // Move ahead
    for(n=0;n<times;n++)
        turtle_add_action("f()\n");
}

void turtle_move_to(turtle_xy_position to)
{
    // We are always facing the positive way of the x axis

    int x = to.x - turtle_position.x;
    int y = to.y - turtle_position.y;

    if (x != 0)
    {
        if (x < 0)
        {
            // Turn around
            turnRight += 2;
        }

        turtle_move_forward_times(abs(x));

        if (x<0)
        {
            turnRight += 2;
        }

    } // x!=0

    if (y != 0)
    {
        if (y>0)
        {
            turnRight++;
        }
        else
        {
            turnLeft++;
        }

        turtle_move_forward_times(abs(y));

        if (y>0)
        {
            turnLeft++;
        }
        else
        {
            turnRight++;
        }
    } // y !=0


    turtle_position.x = to.x;
    turtle_position.y = to.y;
}

void turtle_forward()
{
    turtle_add_action("f()\n");
}

void turtle_left()
{
    turtle_add_action("l()\n");

}

void turtle_right()
{
    turtle_add_action("r()\n");

}

void turtle_up()
{
    turtle_add_action("u()\n");
    turtle_z++;
}

void turtle_down()
{
    turtle_add_action("d()\n");
    turtle_z--;
}

void turtle_reload ()
{
    long z;
    turtle_xy_position original_position = turtle_position;
    long original_z_position = turtle_z;

    turtle_xy_position to;
    to.x = -1;
    to.y = -1;
    turtle_move_to(to);

    for (z=original_z_position; z>0; z--)
    {
        turtle_down();
    }

    turtle_pivot_ahead();

    turtle_add_action("s(1)\n");
    slot = 1;

    // refuel first, only if needed
    turtle_add_action("if fl() < 6000 then\n");
    turtle_add_action("\tsk()\n");
    turtle_add_action("\trl()\n");
    turtle_add_action("\tdr()\n");
    turtle_add_action("end\n");

    to.y = -2;
    turtle_move_to(to);
    turtle_pivot_ahead();

    // Get materials
    turtle_add_action("for i = 0,15,1 do\n");
    turtle_add_action("\tsk()\n");
    turtle_add_action("end\n");

    for (z=original_z_position; z>0; z--)
    {
        turtle_up();
    }

    turtle_move_to(original_position);
}

void turtle_place()
{
    turtle_add_action("p()\n");
    blocksPlaced++;
    if ((blocksPlaced % 64) == 0)
    {
        // We need to select the next slot
        if (slot < 16)
        {
            turtle_add_action("s(%d)\n", ++slot);
        }
        else
        {
            // We emptied the last slot (16) and need
            // to go get some more material
            turtle_reload();
        }
    }

}

void turtle_define_functions()
{
    printf("function f()\n");
    printf("\twhile not turtle.forward() do\n");
    printf("\t\tif turtle.detect() then\n");
    printf("\t\t\tturtle.dig()\n");
    printf("\t\tend\n");
    printf("\tend\n");
    printf("end\n");

    printf("function u()\n");
    printf("\twhile not turtle.up() do\n");
    printf("\t\tif turtle.detectUp() then\n");
    printf("\t\t\tturtle.digUp()\n");
    printf("\t\tend\n");
    printf("\tend\n");
    printf("end\n");

    printf("function d()\n");
    printf("\twhile not turtle.down() do\n");
    printf("\tend\n");
    printf("end\n");

    printf("function l()\n");
    printf("\tturtle.turnLeft()\n");
    printf("end\n");

    printf("function r()\n");
    printf("\tturtle.turnRight()\n");
    printf("end\n");

    printf("function p()\n");
    printf("\tturtle.placeDown()\n");
    printf("end\n");

    printf("function s(slot)\n");
    printf("\tturtle.select(slot)\n");
    printf("end\n");

    printf("function sk()\n");
    printf("\tturtle.suck()\n");
    printf("end\n");

    printf("function dr()\n");
    printf("\tturtle.drop()\n");
    printf("end\n");

    printf("function rl()\n");
    printf("\tturtle.refuel()\n");
    printf("end\n");

    printf("function fl()\n");
    printf("\treturn turtle.getFuelLevel()\n");
    printf("end\n");
}

