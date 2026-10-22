#! /usr/bin/env bash

readonly GREEN="\e[1;32m"
readonly YELLOW="\e[1;33m"
readonly RED="\e[1;31m"
readonly RESET="\e[0m"

TESTLOG="controller_test_output.txt"
SALIDAPNG="simulacion.png"

PROGRAM_NAME="sc-motor-ag"
ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR=""

CALL_ERROR=1
BUILD_ERROR=2
RUN_ERROR=3

usage() {
    echo -e "Uso: ${YELLOW}$0 <opcion>${RESET}"
    echo -e "    ${GREEN}build${RESET} [debug|release] [traces|--traces] [verbose|--verbose]"
    echo -e "    ${GREEN}run${RESET} [debug|release]"
    echo -e "    ${GREEN}clean${RESET}"
}

run_cmd() {
    local cmd="$1"
    echo -e "Ejecutando ${YELLOW}$cmd${RESET}"

    if eval "$cmd"; then
        echo -n ""
    else
        echo -e "${RED}Error:${RESET} ${YELLOW}$cmd${RESET} falló ($?)"
        exit 1
    fi
}

build() {
    local profile="debug"
    local build_options
    local build_type
    local arg
    local print_traces="-DPRINT_TRACES=OFF"

    for arg in "$@"; do
        case "$arg" in
            debug|release)
                profile="$arg"
                ;;
            verbose|--verbose)
                build_options+=(--verbose)
                ;;
            traces|--traces)
                print_traces="-DPRINT_TRACES=ON"
                ;;
            *)
                echo -e "${RED}Error:${RESET} Opción no válida: $arg" >&2
                usage >&2
                exit $BUILD_ERROR
                ;;
        esac
    done

    echo -e "Ejecutando ${YELLOW}build${RESET}"

    case "$profile" in
        debug) build_type="Debug" ;;
        release) build_type="Release" ;;
        *)
            echo -e "${RED}Error:${RESET} Perfil no válido: $profile" >&2
            usage >&2
            exit $BUILD_ERROR
            ;;
    esac

    BUILD_DIR="$ROOT_DIR/build/$profile"

    echo -e "${GREEN}Configurando${RESET} compilación ${build_type}"
    run_cmd "cmake -S ${ROOT_DIR} -B ${BUILD_DIR} \
            ${print_traces} \
            -DCMAKE_BUILD_TYPE=${build_type} \
            -DCMAKE_C_COMPILER=/usr/bin/gcc"

    echo -e "${GREEN}Compilando${RESET}"
    run_cmd "cmake --build ${BUILD_DIR} ${build_options[@]}"
}

run() {
    local profile="${1:-debug}"
    local executable="$ROOT_DIR/build/$profile/$PROGRAM_NAME"

    if [[ ! -x "$executable" ]]; then
        echo "La app no existe o no es ejecutable: $executable" >&2
        echo "Ejecuta primero: $0 build $profile" >&2
        exit $RUN_ERROR
    fi

    run_cmd "${executable} ${TESTLOG}"
    run_cmd "uv run python3 -m scripts.plot ${TESTLOG} ${SALIDAPNG}"
}

clean() {
    rm -rf "$ROOT_DIR/build"
    echo "rm -rf $ROOT_DIR/build"
}

set -eo pipefail

if [ -z "$1" ]; then
    usage
    exit 0
fi

COMMAND="${1:-}"
shift # Quita el primer argumento para que $0 solo contenga los restantes

case "$COMMAND" in
    build)
        build "$@"
        ;;
    run)
        run "$@"
        ;;
    clean)
        clean
        ;;
    *)
        usage >&2
        exit $CALL_ERROR
        ;;
esac