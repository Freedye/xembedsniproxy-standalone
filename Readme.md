# xembed-sni-proxy-standalone
Didn't want to install all the plasma-workspace stuff for this module alone and basically just for a Battle.net icon, so here we are

# Why this exists:
Basically, when you run Battle.net (and other programs) through wine, the system tray icons float around your screen because it uses a X11 protocol called XEmbed.
Waybar (and i think also swaybar and quickshell) do not support XEmbed, they use a DBus-based protocol called StatusNotifierItem (SNI).

To fix that, you need to install xembed-sni-proxy, which is included in plasma-workspace.

The problem is pacman -S plasma-workspace is ~200mb, pacman -Sdd plasma-workspace sometimes install unwanted dependecies after a pacman -Syu and also adds unwanted session options to SDDM, with this method should be around 5mb without all the bloat.

## Dependencies:
- `base-devel`, `cmake`, `extra-cmake-modules`
- `qt6-base`
- `kwindowsystem`, `kdbusaddons`, `kcoreaddons`, `kcrash`
- `libxcb`, `xcb-util-keysyms`, `xcb-util-wm`, `xcb-util-image`, `libxtst`

## How to compile:
```bash
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make
sudo make install
```

## Usage:
Add this to your `hyprland.conf`:
`exec-once = xembedsniproxy`

or if you just downloaded the binary:
`exec-once = path/to/xembedsniproxy`
