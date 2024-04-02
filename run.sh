set -ex

BUILD_DIR=build

echo "Building cmake from ${BASH_SOURCE%/*}..."
(exec cmake -S "${BASH_SOURCE%/*}" -B ${BUILD_DIR})
# -DCMAKE_VERBOSE_MAKEFILE=ON

echo "Building using make using build/Makefile..."
(exec make -C ${BUILD_DIR} -n)

./build/application/application