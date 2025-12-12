# Chinese Chess (Qt)

This project provides a Qt-based Chinese chess playground featuring local games, human-versus-AI play, and LAN matchmaking. Core features include:

- Basic Chinese chess rules with selectable pieces and validation.
- Human vs AI (random-move baseline) and local two-player support.
- LAN host/client play with move and draw offer synchronization.
- Undo and draw actions.
- Lightweight heuristic win-probability indicator and persistent in-session scoreboard.

## Building

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
./chinese_chess
```

Qt 6 with the Widgets and Network modules is required.
