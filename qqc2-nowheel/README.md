# qqc2-nowheel

Rebuilds Fedora's kf6-qqc2-desktop-style so dropdowns, spinboxes and sliders in
QML apps (System Settings, Discover, other Kirigami apps) don't change value when
you scroll. The wheel scrolls the page instead.

Plasma's panel and widgets (e.g. the volume applet) use a different style and
keep working with the wheel.

## Build and install

    cd ~/projects/qqc2-nowheel
    ./build.sh

It prints the install command at the end, like:

    sudo dnf install ~/rpmbuild/RPMS/x86_64/kf6-qqc2-desktop-style-6.xx.0-1.fc44.nowheel.x86_64.rpm

Then close and reopen System Settings.

## After updates

When Fedora updates kf6-qqc2-desktop-style (roughly monthly, with KDE
Frameworks), the old behavior comes back. Run ./build.sh again and install.
If KDE ever rewrites the relevant code, nowheel-prep.sh stops the build with a
clear message instead of producing a broken package.

## Undo

    sudo dnf distro-sync kf6-qqc2-desktop-style
