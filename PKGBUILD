```bash
# Maintainer: Freedye <federico.bolc@gmail.com>
pkgname=xembedsniproxy-standalone
pkgver=1.0
pkgrel=1
pkgdesc="Standalone version of KDE XEmbed-to-SNI proxy for minimal setups"
arch=('x86_64')
url="https://github.com/TUO_UTENTE/xembedsniproxy-standalone"
license=('GPL')
depends=('qt6-base' 'kwindowsystem' 'kdbusaddons' 'kcoreaddons' 'kcrash' 'libxtst' 'libxcb' 'xcb-util-keysyms' 'xcb-util-wm' 'xcb-util-image')
makedepends=('cmake' 'extra-cmake-modules')
source=("git+$url.git")
md5sums=('SKIP')

build() {
    cmake -B build -S "$pkgname" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr
    cmake --build build
}

package() {
    DESTDIR="$pkgdir" cmake --install build
}