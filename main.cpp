#include <ncurses.h>

#include <algorithm>
#include <array>
#include <fstream>
#include <random>
#include <string>
#include <vector>

#define O_PIECE 3
#define I_PIECE 6

struct Point
{
  int y;
  int x;
};

struct Block
{
  Point pos;
  int color;
};

struct Piece
{
  int type;
  Point pos;
  int color;
  std::array<std::array<Point, 4>, 4> shapes;
  int rotation;
};

void draw_block(WINDOW *win, Block block, int offset_x = 0)
{
  wattron(win, COLOR_PAIR(block.color));
  mvwprintw(win, block.pos.y + 1, block.pos.x * 2 + 1 + offset_x, "  ");
  wattroff(win, COLOR_PAIR(block.color));
}

void draw_piece(WINDOW *win, Piece piece, int offset_x = 0)
{
  for (auto point : piece.shapes[piece.rotation])
  {
    // Transform point to block
    Block block{
        piece.pos.y + point.y,
        piece.pos.x + point.x,
        piece.color,
    };

    // To prevent drawing over the border at the
    // top when rotating the piece
    if (block.pos.y < 0) continue;

    draw_block(win, block, offset_x);
  }
}

bool check_collision(const Piece piece, const std::vector<Block> &static_blocks)
{
  for (auto point : piece.shapes[piece.rotation])
  {
    // Collision with wall or floor
    if (piece.pos.x + point.x < 0 || piece.pos.x + point.x >= 10 ||
        piece.pos.y + point.y >= 20)
      return true;

    // Collision with another piece
    for (auto block : static_blocks)
      if (piece.pos.x + point.x == block.pos.x &&
          piece.pos.y + point.y == block.pos.y)
        return true;
  }

  return false;
}

void clear_line(std::vector<Block> &static_blocks, int y)
{
  // Remove all points in the line
  static_blocks.erase(
      std::remove_if(static_blocks.begin(), static_blocks.end(),
                     [y](const Block &block) { return block.pos.y == y; }),
      static_blocks.end());

  // Move all points above the line down
  for (auto &block : static_blocks)
    if (block.pos.y < y) block.pos.y++;
}

Piece random_piece(
    const std::array<std::array<std::array<Point, 4>, 4>, 7> &piece_shapes)
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 6);
  const int rand_idx{dis(gen)};

  return {rand_idx, {0, 0}, rand_idx + 1, piece_shapes[rand_idx], 0};
}

int main()
{
  initscr();
  cbreak();               // Disable line buffering
  noecho();               // Do not display input
  curs_set(0);            // Hide cursor
  keypad(stdscr, TRUE);   // Enable special keys
  nodelay(stdscr, TRUE);  // Make getch non-blocking
  start_color();          // Enable color functionality
  refresh();

  // Colors
  init_pair(1, COLOR_BLACK, COLOR_MAGENTA);  // T piece
  init_pair(2, COLOR_BLACK, COLOR_BLUE);     // J piece
  init_pair(3, COLOR_BLACK, COLOR_RED);      // Z piece
  init_pair(4, COLOR_BLACK, COLOR_YELLOW);   // O piece
  init_pair(5, COLOR_BLACK, COLOR_GREEN);    // S piece
  init_pair(6, COLOR_BLACK, COLOR_WHITE);    // L piece
  init_pair(7, COLOR_BLACK, COLOR_CYAN);     // I piece

  std::array<std::array<std::array<Point, 4>, 4>, 7> piece_shapes{{
      // T piece: 4 rotations
      {{
          {{{2, 1}, {2, 2}, {2, 3}, {3, 2}}},
          {{{1, 2}, {2, 1}, {2, 2}, {3, 2}}},
          {{{1, 2}, {2, 1}, {2, 2}, {2, 3}}},
          {{{1, 2}, {2, 2}, {2, 3}, {3, 2}}},
      }},
      // J piece: 4 rotations
      {{
          {{{2, 1}, {2, 2}, {2, 3}, {3, 3}}},
          {{{1, 2}, {2, 2}, {3, 1}, {3, 2}}},
          {{{1, 1}, {2, 1}, {2, 2}, {2, 3}}},
          {{{1, 2}, {1, 3}, {2, 2}, {3, 2}}},
      }},
      // Z piece: 2 rotations
      {{
          {{{2, 1}, {2, 2}, {3, 2}, {3, 3}}},
          {{{1, 3}, {2, 2}, {2, 3}, {3, 2}}},
          {{{2, 1}, {2, 2}, {3, 2}, {3, 3}}},  // Repeated
          {{{1, 3}, {2, 2}, {2, 3}, {3, 2}}},  // Repeated
      }},
      // O piece: 1 rotation
      {{
          {{{2, 1}, {2, 2}, {3, 1}, {3, 2}}},
          {{{2, 1}, {2, 2}, {3, 1}, {3, 2}}},  // Repeated
          {{{2, 1}, {2, 2}, {3, 1}, {3, 2}}},  // Repeated
          {{{2, 1}, {2, 2}, {3, 1}, {3, 2}}},  // Repeated
      }},
      // S piece: 2 rotations
      {{
          {{{2, 2}, {2, 3}, {3, 1}, {3, 2}}},
          {{{1, 2}, {2, 2}, {2, 3}, {3, 3}}},
          {{{2, 2}, {2, 3}, {3, 1}, {3, 2}}},  // Repeated
          {{{1, 2}, {2, 2}, {2, 3}, {3, 3}}},  // Repeated
      }},
      // L piece: 4 rotations
      {{
          {{{2, 1}, {2, 2}, {2, 3}, {3, 1}}},
          {{{1, 1}, {1, 2}, {2, 2}, {3, 2}}},
          {{{1, 3}, {2, 1}, {2, 2}, {2, 3}}},
          {{{1, 2}, {2, 2}, {3, 2}, {3, 3}}},
      }},
      // I piece: 2 rotations
      {{
          {{{2, 0}, {2, 1}, {2, 2}, {2, 3}}},
          {{{0, 2}, {1, 2}, {2, 2}, {3, 2}}},
          {{{2, 0}, {2, 1}, {2, 2}, {2, 3}}},  // Repeated
          {{{0, 2}, {1, 2}, {2, 2}, {3, 2}}},  // Repeated
      }},
  }};

  bool running{true};

  int width{22};
  int height{22};
  int y{(LINES - height) / 2};
  int x{(COLS - width) / 2};

  int fall_time{0};    // How many frames have passed
  int fall_speed{32};  // Frames between each fall
  bool fast_fall{false};

  int total_lines_cleared{0};
  int level{0};
  int score{0};
  int top{0};

  std::vector<int> stats(7, 0);

  WINDOW *main_win{newwin(height, width, y + 1, x)};
  WINDOW *lines_win{newwin(3, width, y - 2, x)};
  WINDOW *score_win{newwin(6, 12, y - 2, x + width + 1)};
  WINDOW *next_win{newwin(6, 12, y + 4, x + width + 1)};
  WINDOW *level_win{newwin(4, 12, y + 10, x + width + 1)};
  WINDOW *stats_win{newwin(24, 16, y - 2, x - 17)};

  Piece curr_piece{random_piece(piece_shapes)};
  Piece next_piece{random_piece(piece_shapes)};
  stats[curr_piece.type]++;
  curr_piece.pos = {-2, 3};

  std::vector<Block> static_blocks;  // Pieces that have landed

  std::vector<int> block_counter(20, 0);  // Track number of blocks in each row

  while (running)
  {
    switch (getch())
    {
      case KEY_LEFT:
        curr_piece.pos.x--;

        if (check_collision(curr_piece, static_blocks)) curr_piece.pos.x++;

        break;

      case KEY_RIGHT:
        curr_piece.pos.x++;

        if (check_collision(curr_piece, static_blocks)) curr_piece.pos.x--;

        break;

      case KEY_DOWN:
        fast_fall = true;
        break;

      case 'z':
      case 'Z':
        curr_piece.rotation = (curr_piece.rotation + 1) % 4;

        if (check_collision(curr_piece, static_blocks))
          curr_piece.rotation = (curr_piece.rotation + 3) % 4;

        break;

      case 'x':
      case 'X':
        curr_piece.rotation = (curr_piece.rotation + 3) % 4;

        if (check_collision(curr_piece, static_blocks))
          curr_piece.rotation = (curr_piece.rotation + 1) % 4;

        break;

      case ERR:
        fast_fall = false;
        break;

      case 'q':
      case 'Q':
        running = false;
        break;
    }

    // Lines
    wclear(lines_win);
    box(lines_win, 0, 0);
    mvwprintw(lines_win, 1, 2, "LINES - %03d", total_lines_cleared);
    wrefresh(lines_win);

    // Score
    wclear(score_win);
    box(score_win, 0, 0);
    mvwprintw(score_win, 1, 2, "TOP");
    mvwprintw(score_win, 2, 2, "%06d", top);
    mvwprintw(score_win, 3, 2, "SCORE");
    mvwprintw(score_win, 4, 2, "%06d", score);
    wrefresh(score_win);

    // Next piece
    wclear(next_win);
    box(next_win, 0, 0);
    mvwprintw(next_win, 1, 2, "NEXT");
    draw_piece(
        next_win, next_piece,
        next_piece.type == O_PIECE || next_piece.type == I_PIECE ? 1 : 0);
    wrefresh(next_win);

    // Level
    wclear(level_win);
    box(level_win, 0, 0);
    mvwprintw(level_win, 1, 2, "LEVEL");
    mvwprintw(level_win, 2, 2, "%d", level);
    wrefresh(level_win);

    // Statistics
    wclear(stats_win);
    box(stats_win, 0, 0);
    mvwprintw(stats_win, 1, 2, "STATISTICS");

    for (int i = 0; i < 7; i++)
    {
      const Piece piece{i, {i * 3, 0}, i + 1, piece_shapes[i], 0};
      draw_piece(stats_win, piece, i == O_PIECE || i == I_PIECE ? 1 : 0);
      mvwprintw(stats_win, (i + 1) * 3, 11, "%03d", stats[i]);
    }

    wrefresh(stats_win);

    // Main window
    wclear(main_win);
    box(main_win, 0, 0);

    if (++fall_time >= fall_speed || fast_fall)
    {
      fall_time = 0;
      curr_piece.pos.y++;

      if (check_collision(curr_piece, static_blocks))
      {
        curr_piece.pos.y--;

        for (auto point : curr_piece.shapes[curr_piece.rotation])
        {
          // Transform point to block
          Block block{
              curr_piece.pos.y + point.y,
              curr_piece.pos.x + point.x,
              curr_piece.color,
          };
          static_blocks.push_back(block);
          block_counter[block.pos.y]++;
        }

        int lines_cleared{0};

        for (int i = 0; i < 20; i++)
        {
          if (block_counter[i] == 10)
          {
            block_counter.erase(block_counter.begin() + i);
            block_counter.insert(block_counter.begin(), 0);

            clear_line(static_blocks, i);
            lines_cleared++;

            i--;
          }
        }

        switch (lines_cleared)
        {
          case 1:
            score += 40 * (level + 1);
            break;
          case 2:
            score += 100 * (level + 1);
            break;
          case 3:
            score += 300 * (level + 1);
            break;
          case 4:
            score += 1200 * (level + 1);
            break;
        }

        total_lines_cleared += lines_cleared;
        lines_cleared = 0;
        level = total_lines_cleared / 10;

        // Spawn new piece
        curr_piece = next_piece;
        curr_piece.pos = {-2, 3};
        next_piece = random_piece(piece_shapes);
        stats[curr_piece.type]++;
      }
    }

    for (auto point : static_blocks) draw_block(main_win, point);
    draw_piece(main_win, curr_piece);
    wrefresh(main_win);

    napms(16);
  }

  endwin();

  return 0;
}
