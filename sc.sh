#! /usr/bin/env bash

set -euo pipefail

PROGRAM_NAME="sc-motor-ag"
ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR=""

usage() {
    echo "Uso:"
    echo "    $0 build [debug|release]"
    echo "    $0 run [debug|release]"
    echo "    $0 clean"
}

build() {
    local profile="${1:-debug}"
    local build_type

    case "$profile" in
        debug)
            build_type="Debug"
            ;;
        release)
            build_type="Release"
            ;;
        *)
            echo "Perfil no válido: $profile" >&2
            usage >&2
            exit 1
            ;;
    esac

    BUILD_DIR="$ROOT_DIR/build/$profile"

    cmake -S "$ROOT_DIR" -B "$BUILD_DIR" \
        -DCMAKE_BUILD_TYPE="$build_type" \
        -DCMAKE_C_COMPILER=/usr/bin/gcc
    cmake --build "$BUILD_DIR" --verbose
}

run() {
    local profile="${1:-debug}"
    local executable="$ROOT_DIR/build/$profile/$PROGRAM_NAME"

    if [[ ! -x "$executable" ]]; then
        echo "La app no existe o no es ejecutable: $executable" >&2
        echo "Ejecuta primero: $0 build $profile" >&2
        exit 1
    fi

    "$executable"
}

clean() {
    rm -rf "$ROOT_DIR/build"
    echo "rm -rf $ROOT_DIR/build"
}

COMMAND="${1:-}"
PROFILE="${2:-debug}"

case "$COMMAND" in
    build)
        build "$PROFILE"
        ;;
    run)
        run "$PROFILE"
        ;;
    clean)
        clean
        ;;
    *)
        usage >&2
        exit 1
        ;;
esac