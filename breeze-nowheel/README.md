# Breeze-NoWheel

A Qt widget style that looks exactly like Breeze, but the mouse wheel no longer
changes dropdowns or spinboxes. Over those controls the wheel scrolls the page
instead. Sliders inside scrolling pages get the same treatment, while standalone
sliders (like a volume slider in a toolbar) keep working with the wheel.

Applies to QtWidgets apps only. QML/Qt Quick pages (much of System Settings) and
Flatpak apps are unaffected.

## Build and install (Fedora)

    sudo dnf install cmake gcc-c++ qt6-qtbase-devel
    cd breeze-nowheel
    cmake -B build
    cmake --build build
    sudo cmake --install build

The plugin lands in /usr/lib64/qt6/plugins/styles/libbreezenowheel.so.

## Turn it on

System Settings → Colors & Themes → Application Style → choose "BreezeNoWheel".

Or from a terminal:

    kwriteconfig6 --file kdeglobals --group KDE --key widgetStyle BreezeNoWheel

Already-open apps need to be restarted (or log out and back in).

## Options (environment variables)

- NOWHEEL_ALLOW_FOCUSED=1: allow the wheel to change a control after you've
  clicked it.
- NOWHEEL_BASE_STYLE=Fusion: wrap a style other than Breeze.

To set one for your whole Plasma session, put it in a script such as
~/.config/plasma-workspace/env/nowheel.sh:

    export NOWHEEL_ALLOW_FOCUSED=1

## Uninstall

Switch Application Style back to Breeze, then:

    sudo rm /usr/lib64/qt6/plugins/styles/libbreezenowheel.so

## After Qt updates

It normally keeps working, since it only uses Qt's public plugin API. If it ever
disappears from the style list after a big Qt upgrade, rebuild it:

    rm -rf build && cmake -B build && cmake --build build && sudo cmake --install build
