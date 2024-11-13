# tetris-cli

NES Tetris in the terminal

🚧 This project is under construction 🚧

## Running

Install Ncurses:

```bash
# Debian/Ubuntu
sudo apt install libncurses5-dev libncursesw5-dev

# Fedora
sudo dnf install ncurses-devel

# Arch
sudo pacman -S ncurses
```

Compile and run:

```bash
make && ./tetris-cli
```

## TODO

- [x] Menu system (main menu, pause menu, game over menu)
- [x] Add game over detection
- [ ] Save and display top scores
- [ ] Increase speed with level
- [ ] Improve fast drop
