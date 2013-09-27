/*
============================================================================
Name        : main.c
Author      : Janne Pakarinen
Version     : 1.0
Copyright   : Janne Pakarinen (c) 2013
Description : Experiments in Allegro
============================================================================
*/
#define ALLEGRO_STATICLINK

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#define ROWS (48)
#define COLS (64)
#define FPS (60.0f)

// --- Headers ---

// -- Game of Life logic --
void handle_rows(void);
void populate_rows(bool random);
// -- Game Handling --
void destroy(void);
void init(void);
void update(void);
void render(void);

// --- Global variables ---
ALLEGRO_DISPLAY *display;
ALLEGRO_EVENT_QUEUE *event_queue;
ALLEGRO_TIMER *timer;
uint64_t *rows;
uint64_t tick;

// --- Private interface ---

// -- Game of Life logic --
void handle_rows(void)
{
  uint32_t i, j;
  uint64_t new_rows[ROWS] = { 0x00 };
  
  for (i = 0; i < ROWS; i++)
  {
    uint64_t row, prev, next, new_row;
    row = rows[i];
    prev = 0x00;
    next = 0x00;
    new_row = 0x00;

    if (i > 0)
      prev = rows[i - 1];
    if (i + 1 < ROWS)
      next = rows[i + 1];

    for (j = 0; j < COLS; j++)
    {
      bool alive;
      uint64_t neigh_prev, neigh_cur, neigh_next;
      uint32_t neighbours = 0, steps = (j - 1);

      alive = ((uint64_t)((row >> steps) & 0x02) > 0);

      neigh_prev = (uint64_t)((prev >> steps) & 0x07); // 0111
      neigh_cur = (uint64_t)((row >> steps) & 0x05);   // 0101
      neigh_next = (uint64_t)((next >> steps) & 0x07); // 0111

      if (neigh_prev > 0)
      {
        if (neigh_prev == 7)
          neighbours += 3;
        else if (neigh_prev == 1 || neigh_prev == 2 || neigh_prev == 4)
          neighbours += 1;
        else
          neighbours += 2;
      }

      if (neigh_next > 0)
      {
        if (neigh_next == 7)
          neighbours += 3;
        else if (neigh_next == 1 || neigh_next == 2 || neigh_next == 4)
          neighbours += 1;
        else
          neighbours += 2;
      }

      if (neigh_cur > 0)
      {
        if (neigh_cur == 1 || neigh_cur == 2 || neigh_cur == 4)
          neighbours += 1;
        else
          neighbours += 2;
      }

      if (neighbours == 3 || (alive && neighbours == 2))
        new_row |= (uint64_t)(((uint64_t)0x02) << steps);
    }

    new_rows[i] = new_row;
  }
  
  for (i = 0; i < ROWS; i++)
    rows[i] = new_rows[i];
}

void populate_rows(bool random)
{
  if (random)
  {
    uint32_t i;
    for (i = 0; i < ROWS; i++)
    {
      long double mod = (((long double)rand()) / ((long double)RAND_MAX));
      rows[i] = (uint64_t)(((uint64_t) -1) * mod);
    }
  }
}

// -- Game Handling --

void destroy(void)
{
  if (display)
    al_destroy_display(display);
  if (event_queue)
    al_destroy_event_queue(event_queue);
  if (timer)
    al_destroy_timer(timer);
  if (rows)
    free(rows);
}

void init(void)
{
  uint32_t i;
  timer = NULL;
  display = NULL;
  event_queue = NULL;
  
  // Allegro
  if (!al_init())
  {
    fprintf(stderr, "Failed to initialize Allegro!\n");
    exit(1);
  }
  
  // Primitives addon
  if (!al_init_primitives_addon())
  {
    fprintf(stderr, "Failed to initialize Allegro Primitives addon!\n");
    exit(1);
  }
  
  // Keyboard input
  if (!al_install_keyboard())
  {
    fprintf(stderr, "Failed to initialize keyboard!\n");
    exit(1);
  }
  
  // Timer
  timer = al_create_timer(1.0 / FPS);
  if (!timer)
  {
    fprintf(stderr, "Failed to initialize timer!\n");
    exit(1);
  }

  // Display
  display = al_create_display(640, 480);
  if (!display)
  {
    fprintf(stderr, "Failed to create display!\n");
    al_destroy_timer(timer);
    exit(1);
  }

  // Event queue
  event_queue = al_create_event_queue();
  if (!event_queue)
  {
    fprintf(stderr, "Failed to create event queue!\n");
    al_destroy_display(display);
    al_destroy_timer(timer);
    exit(1);
  }
  
  // Event sources
  al_register_event_source(event_queue, al_get_display_event_source(display));
  al_register_event_source(event_queue, al_get_timer_event_source(timer));
  al_register_event_source(event_queue, al_get_keyboard_event_source());

  // Rows
  rows = malloc(sizeof(uint64_t)* ROWS);
  for (i = 0; i < ROWS; i++)
    rows[i] = (uint64_t)0x00;

  // Random number generator
  srand(time(NULL));
  populate_rows(true);

  // Initial screen contents are black
  al_clear_to_color(al_map_rgb(0, 0, 0));
  al_flip_display();
}

void update(void)
{
  if (tick % (int)(FPS * 0.125f) == 0)
  {
    handle_rows();
    tick = 0;
  }
  tick++;
}

void render(void)
{
  uint32_t i, j;
  int win_height, win_width;
  float block_height, block_width;
  win_height = al_get_display_height(display);
  win_width = al_get_display_width(display);
  block_height = (float)(win_height / ROWS);
  block_width = (float)(win_width / COLS);

  for (i = 0; i < ROWS; i++)
  {
    uint64_t row = rows[i];
    for (j = 0; j < COLS; j++)
    {
      float x1,x2,y1,y2;
      ALLEGRO_COLOR color;
      if (((uint64_t)(row >> j) & 0x01))
        color = al_map_rgb(255, 255, 0);
      else
        color = al_map_rgb(0, 0, 0);
                               
      x1 = j * block_width;
      x2 = x1 + block_width;   
      y1 = i * block_height;
      y2 = y1 + block_height;
      
      al_draw_filled_rectangle(x1,y1,x2,y2,color);
    }
  }

  al_flip_display();
  al_clear_to_color(al_map_rgb(0, 0, 0));
}

// --- Public Interface ---
int
main(int argc, char **argv)
{
  bool quit = false;

  init();

  al_start_timer(timer);
  tick = 0;
  
  while (!quit)
  {
    ALLEGRO_EVENT ev;

    al_wait_for_event(event_queue, &ev);
    if (ev.type != ALLEGRO_EVENT_TIMER)
    {
      switch (ev.type)
      {
      case ALLEGRO_EVENT_DISPLAY_CLOSE:
        quit = true;
        break;
      case ALLEGRO_EVENT_KEY_DOWN:
        switch (ev.keyboard.keycode)
        {
        case ALLEGRO_KEY_ESCAPE:
          quit = true;
          break;
        case ALLEGRO_KEY_F2:
          populate_rows(true);
          break;
        default:
          break;
        }
        break;
      default:
        break;
      }
    }
    else
    {
      update();
      render();
    }
  }

  destroy();
  exit(0);
}

