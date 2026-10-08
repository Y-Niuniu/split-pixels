# Split Pixels — Cooperative Tower Defence on STM32

[中文说明](README.zh-CN.md)

Split Pixels is a second-year group embedded-systems project at the University of Leeds. This personal portfolio copy highlights **Game 1**, which I developed independently: a two-player game combining the tower-defence mechanics of *Plants vs. Zombies* with the directional input sequences of *Helldivers 2*.

One player builds the defensive line; the other follows on-screen arrow sequences to recharge defensive units. Their different, interdependent roles turn a single-player tower-defence concept into a cooperative game on a microcontroller.

## My contribution

I implemented Game 1's gameplay, including the 3 × 5 battlefield, plant selection and placement, directional charging state machines, enemy waves, combat, scoring, win/loss conditions, instructions, pause options and rendering logic.

I hand-drew all character images used in Game 1 and used a Python script to convert them into the pixel-data format required by the course display interface. The conversion script is not included in this repository.

**Game 2 and Game 3 were developed by other group members.** The shared menu, input handling, music and hardware drivers include course-provided or jointly developed components. They are retained for attribution and because the build depends on the complete project.

## Gameplay and controls

Enemies advance from the right towards the left edge of the battlefield. Defensive units provide shooting, blocking and area damage, subject to charging and scoring conditions.

| Player | Role | Controls |
|---|---|---|
| Player 1 | Select, place and remove units | First joystick: move cursor; BTN3: select; BTN2: place/remove |
| Player 2 | Recharge units | BTN9: start an available charging task; second joystick: enter the displayed directions in order |

Enemy waves increase with the game timer, with a victory threshold of 180 seconds. The timer does not exclude paused time.

## Implementation

| Location | What to inspect |
|---|---|
| [`game_1/Game_1.c`](game_1/Game_1.c) | `Game1_Run`, game states, instructions and update/render loop |
| [`game_1/engine/engine.c`](game_1/engine/engine.c) | Battlefield, placement and directional charging |
| [`game_1/engine/interact.c`](game_1/engine/interact.c) | Enemy waves, movement, combat and outcomes |
| [`game_1/data/`](game_1/data/) | Plant and enemy parameters |
| [`game_1/sprites/`](game_1/sprites/) | Embedded pixel resources, including arrow prompts |

The C implementation uses finite-state machines, fixed-capacity arrays and `HAL_GetTick()` for timing, integrated with shared LCD, input and sound interfaces.

## Hardware and build

- STM32 Nucleo-L476RG, ST7789V2-driven 240 × 240 LCD, two analogue joysticks, buttons and a buzzer.
- C11, STM32 HAL/CMSIS, CMake 3.22+, Ninja and an Arm GNU Toolchain containing `arm-none-eabi-gcc`.
- Hardware configuration: [`Core/Inc/main.h`](Core/Inc/main.h) and [`Unit_4_1_Menu_Template.ioc`](Unit_4_1_Menu_Template.ioc).

From the repository root:

```sh
cmake --preset Debug
cmake --build --preset Debug
```

Firmware output is configured under `build/Debug/`. Program the matching board using an STM32-compatible programmer/debugger, then select Game 1 from the menu. Keep all three games and shared components for the supplied build.

This is a hardware-dependent course-project snapshot. The documented build commands have not been re-run for this README update, and no new hardware verification is claimed. Pause timing and state resets on re-entry remain areas for improvement.

## Artwork, music and provenance

Character drawings are my own artwork inspired by the games above. The team manually transcribed score material found online, transposed it and adapted it for buzzer playback, referencing *Star Wars* piano scores, *Helldivers 2*'s **“A Cup of Liber-Tea”**, and the *Battlefield 1* theme. This is musical arrangement and embedded implementation, rather than original composition; the source scores' reuse permissions have not been verified.

Original group repository: [YidingNiu/split_pixels](https://github.com/YidingNiu/split_pixels). This portfolio copy starts from `60e2af5`, matching my supplied local project snapshot; the upstream repository contains a later group balancing revision. This copy retains the original Git history, [`LICENSE`](LICENSE) and third-party notices. The root MIT licence does not establish permission for every image, melody or referenced work.
