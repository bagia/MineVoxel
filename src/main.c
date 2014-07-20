#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kv6.h"
#include "turtle.h"

void generate_code();
turtle_xy_position find_closest_to(turtle_xy_position position, long z);

void debug_print_layer(long z);
void how_to_use();

static int debug_mode = 0;

int main(int argc, const char **argv)
{
    const char* filename;

    if (argc < 2)
        goto how_to_use;

    if (strcmpi(argv[1], "-d") == 0)
    {
        debug_mode = 1;
        if (argc < 3)
            goto how_to_use;
        filename = argv[2];
    }
    else if (strcmpi(argv[1], "-dc") == 0)
    {
        debug_mode = 2;
        if (argc < 3)
            goto how_to_use;
        filename = argv[2];
    }
    else
    {
        filename = argv[1];
    }

    if (!kv6_load(filename))
    {
        printf("Unable to load KV6 file!\n");
        goto exit;
    }

    generate_code();

exit:
    kv6_free();
    return 0;

how_to_use:
    how_to_use();
    return -1;
}

void generate_code()
{
    unsigned short ***points = kv6_points();
    long z, zsiz = kv6_zsiz();

    printf("-- Total: %ld blocks\n", kv6_total_visible_blocks());

    if (!debug_mode)
        turtle_define_functions();

    printf("\n");

    long max_actions = kv6_xsiz() * kv6_ysiz() * kv6_zsiz() * 3;
    turtle_init(max_actions);

    // The KV6 referential starts on the top of the figure
    // so we need to invert it.
    // We also skip the bottom layer because it is usually plain.
    for (z=zsiz-1-1; z>=0; z--)
    {
        if (debug_mode == 1)
        {
            debug_print_layer(z);
            continue;
        }

        // Go to the next layer
        turtle_up();

        turtle_xy_position closest;
        for(;;)
        {
            closest = find_closest_to(turtle_get_position(),z);
            if (closest.x == -1)
            {
                // No more block to place on this layer. Moving on.
                // We break here because we don't want to move anymore.
                break;
            }

            turtle_move_to(closest);
            turtle_place();

            // Mark the point as filled:
            points[closest.x][closest.y][z] = 2;

            if (debug_mode == 2)
            {
                debug_print_layer(z);
            }
        } // for(;;)
    }

    turtle_print_actions();

    turtle_free();
}

turtle_xy_position closest_to_position(turtle_xy_position position, turtle_xy_position candidate1, turtle_xy_position candidate2)
{
    if (debug_mode == 2)
    {
        printf("xsiz=%ld,ysiz=%ld\n", kv6_xsiz(), kv6_ysiz());
        printf("Test if (%d,%d) is closer than (%d,%d) to (%d,%d)\n", candidate1.x, candidate1.y, candidate2.x, candidate2.y, position.x, position.y);
    }

    if (candidate1.x == -1)
        return candidate2;

    if (candidate2.x == -1)
        return candidate1;

    long distance_to_candidate1 = abs(position.x - candidate1.x) + abs(position.y - candidate1.y);
    long distance_to_candidate2 = abs(position.x - candidate2.x) + abs(position.y - candidate2.y);
    if (distance_to_candidate2 < distance_to_candidate1)
        return candidate2;

    return candidate1;
}

int point_needs_block(long x, long y, long z)
{
    if (x>=kv6_xsiz() || x<0)
        return 0;
    if (y>=kv6_ysiz() || y<0)
        return 0;
    if (z>=kv6_zsiz() || z<0)
        return 0;

    unsigned short ***points = kv6_points();
    if (points[x][y][z] == 1)
        return 1;

    return 0;
}

turtle_xy_position find_closest_to(turtle_xy_position position, long z)
{
    long xsiz = kv6_xsiz();
    long ysiz = kv6_ysiz();
    turtle_xy_position closest, candidate;
    long step;
    closest.x = -1;
    closest.y = -1;

    long max_x_left = position.x;
    long max_x_right = xsiz - position.x;
    long max_x = (max_x_left > max_x_right) ? max_x_left : max_x_right;
    long max_y_top = position.y;
    long max_y_bottom = ysiz - position.y;
    long max_y = (max_y_top > max_y_bottom) ? max_y_top : max_y_bottom;

    for(step=0; step < max_x || step < max_y; step++)
    {
        for(candidate.x=position.x - step -1; candidate.x<=position.x+step+1; candidate.x++)
        {
            candidate.y = position.y + step + 1;
            if (point_needs_block(candidate.x, candidate.y, z))
                closest = closest_to_position(position, closest, candidate);

            candidate.y = position.y - step -1;
            if (point_needs_block(candidate.x, candidate.y, z))
                closest = closest_to_position(position, closest, candidate);
        }
        for(candidate.x--, candidate.y = position.y + step +1; candidate.y>=position.y-step-1; candidate.y--)
        {
            candidate.x = position.x + step + 1;
            if (point_needs_block(candidate.x, candidate.y, z))
                closest = closest_to_position(position, closest, candidate);

            candidate.x = position.x - step -1;
            if (point_needs_block(candidate.x, candidate.y, z))
                closest = closest_to_position(position, closest, candidate);
        }

        if (closest.x != -1)
            break;
    } // for step

    return closest;
}

void debug_print_layer(long z)
{
    unsigned short ***points = kv6_points();
    long xsiz = kv6_xsiz(), ysiz = kv6_ysiz();
    long x, y;

    printf("z=%ld \n", z);
    for(y=0; y<ysiz; y++)
    {
        for (x=0; x<xsiz; x++)
        {

            if (points[x][y][z] == 1)
            {
                printf(" ");
            }
            else if ( points[x][y][z] == 2)
            {

                printf("*");
            }
            else
            {
                printf("#");
            }

        } //x
        printf("\n");

    } //y
    printf("\n");
}

void how_to_use()
{
    printf("Usage:\n");
    printf("\tMineVoxel.exe [-d|-dc] kv6_file\n");
    printf("\t\tkv6_file: path to a .kv6 file\n");
    printf("\t\t-d: debug output\n");
    printf("\t\t-dc: comprehensive debug output\n");
}


