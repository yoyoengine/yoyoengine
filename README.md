<div align="center">
    <picture style="width: 100%; height: auto;">
        <source srcset=".github/media/lightheader.png"  media="(prefers-color-scheme: dark)">
        <img src=".github/media/darkheader.png">
    </picture>
</div>

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![wakatime](https://wakatime.com/badge/user/3e73d21c-9ccb-4e77-ab4d-6f58f0296cfa/project/d703c117-3f87-4f6f-96f5-e0c40088a6a0.svg)](https://wakatime.com/badge/user/3e73d21c-9ccb-4e77-ab4d-6f58f0296cfa/project/d703c117-3f87-4f6f-96f5-e0c40088a6a0?style=for-the-badge)
![GitHub repo size](https://img.shields.io/github/repo-size/zoogies/yoyoengine)
![GitHub Tag](https://img.shields.io/github/v/tag/zoogies/yoyoengine)

---

A custom, hackable 2D Game Engine written in C.

## What?

Yoyoengine started as a small game in SDL, and evolved into a fully featured, general purpose 2D game engine.

<table>
    <tr>
        <td><img src="https://raw.githubusercontent.com/yoyoengine/yoyoeditor/main/.github/media/zoogies.png" alt="Zoogies Logo"/></td>
        <td><img src="https://raw.githubusercontent.com/yoyoengine/yoyoeditor/main/.github/media/theriac.png" alt="Theriac"/></td>
    </tr>
</table>
<img src="https://raw.githubusercontent.com/yoyoengine/yoyoeditor/main/.github/media/vannie.png" alt="Raise A Vannie"/>

### Feature List:

- Easy to use visual editor
- Multi platform support (Windows, Linux, Web)
- Asset bundling by default, for optimized builds.
- Pleasant to use event-based C scripting API

### Made with yoyoengine:

<table>
  <tr>
    <td align="center" width="50%">
      <a href="https://zoogies.itch.io/raise-a-vannie">
        <img src="https://img.itch.zone/aW1hZ2UvMjUzOTE4OC8xNTExOTIzMS5wbmc%3D/347x500/m5fla8.png" alt="Raise A Vannie" width="100%">
      </a>
      <br />
      <strong><a href="https://zoogies.itch.io/raise-a-vannie">Raise A Vannie</a></strong>
      <br />
      <sub>Released: 2024-02-20</sub>
      <br />
      <sub>A gambling, plant-growing clicker.</sub>
    </td>
    <td align="center" width="50%">
      <a href="https://zoogies.itch.io/theriac">
        <img src="https://img.itch.zone/aW1hZ2UvMjU2OTE3Ny8xNTM2MjYzMC5wbmc%3D/347x500/TLeRLy.png" alt="Theriac" width="100%">
      </a>
      <br />
      <strong><a href="https://zoogies.itch.io/theriac">Theriac</a></strong>
      <br />
      <sub>Released: 2024-03-13</sub>
      <br />
      <sub>GOTY 2024</sub>
    </td>
  </tr>
  <tr>
    <td align="center" width="50%">
      <a href="https://ud-game-development-club.itch.io/ailurophobia">
        <img src="https://img.itch.zone/aW1hZ2UvMzY2MjkwMy8yMTgwODYwOS5wbmc%3D/347x500/Sl6GHk.png" alt="Ailurophobia" width="100%">
      </a>
      <br />
      <strong><a href="https://ud-game-development-club.itch.io/ailurophobia">Ailurophobia</a></strong>
      <br />
      <sub>Released: 2025-06-23</sub>
      <br />
      <sub>A horror visual novel.</sub>
    </td>
    <td align="center" width="50%">
      <a href="https://zoogies.itch.io/milliomare">
        <img src="https://img.itch.zone/aW1hZ2UvNDA1NzU4NS8yNDE4ODYwMy5wbmc%3D/347x500/HAW6uv.png" alt="Milliomare" width="100%">
      </a>
      <br />
      <strong><a href="https://zoogies.itch.io/milliomare">Milliomare</a></strong>
      <br />
      <sub>Released: 2025-11-21</sub>
      <br />
      <sub>Gamble your way to a million dollars by horse racing.</sub>
    </td>
  </tr>
</table>

### Tech Stack:

- [SDL3](https://www.libsdl.org/) (windowing, rendering, audio, input etc)
- [Nuklear](https://github.com/Immediate-Mode-UI/Nuklear) (UI)
- [Uthash](https://github.com/troydhanson/uthash) (Hash tables)
- [Jansson](https://github.com/akheron/jansson) (JSON parsing)
- Custom Entity Component System
- Custom binary packing format for assets, with [zlib](https://zlib.net/) for compression
- Custom physics and collision system

## Why?

In May 2023, I started working on a visual novel game in SDL2 as a way to learn C for the very first time. While I have yet to finish that game, the dopamine loop of working with a lower level language quickly spiraled me into a scope creep grind, and I ended up with a fully featured, general purpose game engine.

## Who?

### Zoogies.

I work on random projects of interest, and occasionally build games with this engine.

Feel free to check out my work on [GitHub](https://github.com/zoogies) or [Itch.io](https://zoogies.itch.io/).

You can get in touch with me through my [portfolio](https://zmuda.dev), on discord `@zoogies`, or by opening an issue on this repository. I don't usually check my email, but you can try that too.

## Credit

- The listed dependencies, for making this project possible.
- [Kaidiak](https://linktr.ee/kaidiak), for producing the engine splash screen sound effect.
- Ben Mathes, for creating icons used in the editor.
- The [SDL discord](https://discord.gg/BwpFGBWsv8), a must join community if you use SDL.
