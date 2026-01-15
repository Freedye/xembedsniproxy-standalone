# xembed-sni-proxy-standalone

Didn't want to install all the plasma-workspace stuff for this module alone and basically just for a Battle.net icon, so here we are

# why does this exists:
pacman -S plasma-workspace is ~200mb, pacman -Sdd plasma-workspace sometimes install unwanted dependecies after a pacman -Syu, with this method should be around 5mb and less stuff to worry about.

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