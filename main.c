#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <tchar.h>
#include <windows.h>

#define ROW 30
#define COLUMN 20

enum
{
  BALL_DIR_UP = 0,
  BALL_DIR_DOWN = 1,
  BALL_DIR_LEFT = 2,
  BALL_DIR_RIGHT = 3,
};

int BD_X = BALL_DIR_RIGHT;
int BD_Y = BALL_DIR_DOWN;

void clrscr (void);
void draw_screen (void);
void move_ball (void);
void choose_multp (void);

DWORD WINAPI keyboard_rt (void *);

int BALL_X = 1;
int BALL_Y = 1;

int GAME_OVER = 0;
int GAME_ESCAPED = 0;
int MULTP = 1;

int BARRIER_Y = 0;
int SCORE = 0;

HANDLE stdIn;

int
main (int argc, char **argv)
{
  stdIn = GetStdHandle (STD_INPUT_HANDLE);
  srand (time (NULL));

  DWORD thId;
  HANDLE bg = CreateThread (0, 0, keyboard_rt, NULL, 0, &thId);

  while (!GAME_OVER)
    {
      clrscr ();
      draw_screen ();
      move_ball ();

      Sleep (100);
    }

  CloseHandle (bg);
  CloseHandle (stdIn);

  clrscr ();
  draw_screen ();

  if (!GAME_ESCAPED)
    puts ("Game Over");

  return 0;
}

void
clrscr (void)
{
  system ("cls");
}

void
move_ball (void)
{
  switch (BD_X)
    {
    case BALL_DIR_RIGHT:
      {
        if (BALL_X >= COLUMN - 1)
          {
            BD_X = BALL_DIR_LEFT;
            BALL_X -= MULTP;
          }

        else
          BALL_X += MULTP;
      }
      break;

    case BALL_DIR_LEFT:
      {
        if (BALL_X <= 1)
          {
            if (BALL_Y >= BARRIER_Y - 1 && BALL_Y <= BARRIER_Y + 1)
              {
                BD_X = BALL_DIR_RIGHT;
                BALL_X += MULTP;
                SCORE++;
              }

            else
              GAME_OVER = 1;
          }

        else
          BALL_X -= MULTP;
      }
      break;

    default:
      break;
    }

  switch (BD_Y)
    {
    case BALL_DIR_UP:
      {
        if (BALL_Y <= 1)
          {
            BD_Y = BALL_DIR_DOWN;
            BALL_Y += MULTP;
          }

        else
          BALL_Y -= MULTP;
      }
      break;

    case BALL_DIR_DOWN:
      {
        if (BALL_Y >= ROW - 1)
          {
            BD_Y = BALL_DIR_UP;
            BALL_Y -= MULTP;
          }

        else
          BALL_Y += MULTP;
      }
      break;

    default:
      break;
    }
}

void
draw_screen (void)
{
  char LINE[COLUMN + 5];
  *LINE = '\0';

  for (int i = 0; i < ROW; i++)
    {
      if (i >= BARRIER_Y - 1 && i <= BARRIER_Y + 1)
        strcat (LINE, "||");

      else
        strcat (LINE, "  ");

      for (int j = 0; j < COLUMN; j++)
        {
          if (i == BALL_Y && j == BALL_X)
            strcat (LINE, "o");

          else
            strcat (LINE, " ");
        }

      if (i >= BALL_Y - 1 && i <= BALL_Y + 1)
        strcat (LINE, "||");

      puts (LINE);

      *LINE = '\0';
    }

  printf ("SCORE: %d\n", SCORE);
}

DWORD WINAPI
keyboard_rt (void *args)
{
  while (!GAME_OVER)
    {
      DWORD ev;
      GetNumberOfConsoleInputEvents (stdIn, &ev);

      if (ev != 0)
        {
          INPUT_RECORD inpRec[ev];
          DWORD ev_count;
          ReadConsoleInput (stdIn, inpRec, ev, &ev_count);

          for (size_t i = 0; i < ev_count; i++)
            {
              if (inpRec[i].EventType == KEY_EVENT
                  && inpRec[i].Event.KeyEvent.bKeyDown)
                {
                  WORD w = inpRec[i].Event.KeyEvent.wVirtualKeyCode;

                  if (w == VK_DOWN)
                    BARRIER_Y += 1;

                  else if (w == VK_UP)
                    BARRIER_Y -= 1;

                  else if (w == VK_ESCAPE)
                    {
                      GAME_ESCAPED = 1;
                      GAME_OVER = 1;

                      goto end;
                    }
                }
            }
        }
    }

end:
  return 0;
}