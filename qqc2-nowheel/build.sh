#!/usr/bin/env bash
# Rebuilds Fedora's kf6-qqc2-desktop-style with the nowheel change.
# Run it again whenever Fedora updates that package.
set -euo pipefail
here="$(cd "$(dirname "$0")" && pwd)"
pkg=kf6-qqc2-desktop-style

sudo dnf install -y rpm-build rpmdevtools dnf-plugins-core
rpmdev-setuptree

work="$(mktemp -d)"
trap 'rm -rf "$work"' EXIT

echo "==> Downloading the source package matching your installed version"
(cd "$work" && dnf download --source "$pkg")
rpm -i "$work"/$pkg-*.src.rpm

spec="$HOME/rpmbuild/SPECS/$pkg.spec"
cp "$here/nowheel-prep.sh" "$HOME/rpmbuild/SOURCES/"

echo "==> Adding the nowheel step to the spec file"
awk '{print} !done && /^%(autosetup|setup)/ {print "sh %{_sourcedir}/nowheel-prep.sh"; done=1}' \
    "$spec" > "$spec.new" && mv "$spec.new" "$spec"
grep -q nowheel-prep "$spec" || { echo "Couldn't find the setup step in $spec" >&2; exit 1; }
sed -i -E '/^Release:/ { /nowheel/! s/[[:space:]]*$/.nowheel/ }' "$spec"

echo "==> Installing build dependencies"
sudo dnf builddep -y "$spec"

echo "==> Building"
rpmbuild -bb "$spec"

rpms=$(find "$HOME/rpmbuild/RPMS" -name "$pkg-[0-9]*nowheel*.rpm" -newer "$work")
[ -n "$rpms" ] || { echo "No built package found" >&2; exit 1; }

echo
echo "==> Built:"
echo "$rpms"
echo
echo "Install it with:"
echo "  sudo dnf install $rpms"
