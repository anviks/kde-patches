#!/bin/sh
# Runs inside the unpacked kf6-qqc2-desktop-style source during the RPM build.
# Stops QML dropdowns, spinboxes and sliders from changing value on scroll,
# so the wheel scrolls the page instead.
set -e
d=org.kde.desktop

for f in "$d/ComboBox.qml" "$d/SpinBox.qml" "$d/DoubleSpinBox.qml"; do
    [ -f "$f" ] || continue
    sed -i 's/^\([[:space:]]*\)wheelEnabled: true$/\1wheelEnabled: false \/\/ nowheel/' "$f"
done

# Slider has its own wheel handler; make it pass the event on to the page.
sed -i 's/^\([[:space:]]*\)onWheel: wheel => {$/&\n\1    wheel.accepted = false; return; \/\/ nowheel/' "$d/Slider.qml"

# Fail loudly if KDE changed the code and nothing matched.
grep -q 'wheelEnabled: false // nowheel' "$d/ComboBox.qml" || { echo "nowheel: ComboBox.qml not patched" >&2; exit 1; }
grep -q 'wheelEnabled: false // nowheel' "$d/SpinBox.qml"  || { echo "nowheel: SpinBox.qml not patched"  >&2; exit 1; }
grep -q 'nowheel' "$d/Slider.qml"                         || { echo "nowheel: Slider.qml not patched"   >&2; exit 1; }
echo "nowheel: QML controls patched"
