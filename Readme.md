# xembed-sni-proxy standalone

![license](https://img.shields.io/badge/license-GPL--2.0-blue)
![platform](https://img.shields.io/badge/platform-Linux%20%2F%20X11-informational)
![KDE Frameworks](https://img.shields.io/badge/KDE%20Frameworks-not%20required-success)

Didn't want to install all the plasma-workspace stuff for this module alone and basically just for a Battle.net icon, so here we are.

## Why this exists

When you run Battle.net (and other programs) through Wine, the system tray icon floats around your screen because it uses an X11 protocol called **XEmbed**.

Waybar (and I think also swaybar and quickshell) don't support XEmbed, they use a D-Bus based protocol called **StatusNotifierItem** (SNI).

To fix that you need `xembed-sni-proxy`, which normally ships inside `plasma-workspace`. Problem is:

| | `plasma-workspace` | this repo |
|---|---|---|
| install size | ~200 MB | ~5 MB |
| KDE Frameworks (KF6) required | yes | **no** |
| extra-cmake-modules required | yes | **no** |
| side effects | can pull in unwanted deps on `pacman -Syu`, adds unwanted session options to SDDM | none |

This fork only needs **Qt6, XCB and X11**: no KDE Frameworks library, no extra-cmake-modules, at build time or runtime.

## Dependencies

| Package | Needed for |
|---|---|
| `base-devel`, `cmake` | building |
| `qt6-base` | Qt6 Core/Gui/DBus |
| `libxcb` | XCB core + X11 extensions (damage, composite, randr, shm, shape, xfixes) |
| `xcb-util`, `xcb-util-wm`, `xcb-util-image` | XCB helper libs (ICCCM, image handling) |
| `libxtst` | synthetic click injection (XTest) |

## How to compile

```bash
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make
sudo make install
```

## Usage

Add this to your `hyprland.conf`:

```
exec-once = xembedsniproxy
```

or, if you just downloaded the binary:

```
exec-once = path/to/xembedsniproxy
```

## License

GPL-2.0, see [LICENSE](LICENSE).
