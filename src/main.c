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

#define FPS (60.0f)

// --- Headers ---

// -- Game Handling --
void destroy(void);
void init(void);
void update(void);
void render(void);

// --- Global variables ---
ALLEGRO_DISPLAY *display;
ALLEGRO_EVENT_QUEUE *event_queue;
ALLEGRO_TIMER *timer;

// --- Private interface ---

// -- Game Handling --

void destroy(void)
{
  if (display)
    al_destroy_display(display);
  if (event_queue)
    al_destroy_event_queue(event_queue);
  if (timer)
    al_destroy_timer(timer);
}

void init(void)
{
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

  // Initial screen contents are black
  al_clear_to_color(al_map_rgb(0, 0, 0));
  al_flip_display();
}

void update(void)
{

}

void render(void)
{
  al_flip_display();
  al_clear_to_color(al_map_rgb(0, 0, 0));
}

// --- Public Interface ---
int
main(int argc, char **argv)
{
  bool quit = false, redraw = false;

  init();

  al_start_timer(timer);

  while (!quit)
  {
    ALLEGRO_EVENT ev;

    al_wait_for_event(event_queue, &ev);
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
        break;
      default:
        break;
      }
      break;
    case ALLEGRO_EVENT_TIMER:
      redraw = true;
      break;
    default:
      break;
    }
    if (redraw && al_is_event_queue_empty(event_queue))
    {
      update();
      render();
      redraw = false;
    }
  }

  destroy();
  exit(0);
}

