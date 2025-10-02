#!/bin/bash
#
# Build script for Lambda Shell
# Usage: ./build.sh [clean|test|install]
#

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Function to clean build files
clean_build() {
    print_status "Cleaning build files..."
    rm -rf bin/ build/ CMakeCache.txt CMakeFiles/ cmake_install.cmake Makefile
    print_success "Build files cleaned!"
}

# Function to build the project
build_project() {
    print_status "Building Lambda Shell..."

    # Run cmake and make
    if cmake . && make; then
        print_success "Build completed successfully!"
        echo
        print_status "Built executables:"
        if [ -f "bin/shell" ]; then
            echo "  • Shell: bin/shell"
        fi
        if [ -f "bin/tests" ]; then
            echo "  • Tests: bin/tests"
        fi
    else
        print_error "Build failed!"
        exit 1
    fi
}

# Function to run tests
run_tests() {
    if [ -f "bin/tests" ]; then
        print_status "Running tests..."
        if ./bin/tests; then
            print_success "All tests passed!"
        else
            print_error "Some tests failed!"
            exit 1
        fi
    else
        print_warning "Tests not built (CUnit not available or tests disabled)"
    fi
}

# Function to install
install_project() {
    print_status "Installing Lambda Shell..."
    if make install; then
        print_success "Installation completed!"
    else
        print_error "Installation failed!"
        exit 1
    fi
}

# Function to show help
show_help() {
    echo "Lambda Shell Build Script"
    echo
    echo "Usage: $0 [OPTION]"
    echo
    echo "Options:"
    echo "  clean     Clean build files and start fresh"
    echo "  test      Build and run tests"
    echo "  install   Build and install the project"
    echo "  help      Show this help message"
    echo "  (no args) Just build the project"
    echo
    echo "Examples:"
    echo "  $0              # Build the project"
    echo "  $0 clean        # Clean and build"
    echo "  $0 test         # Build and test"
    echo "  $0 install      # Build and install"
    echo
}

# Main script logic
case "${1:-build}" in
    "clean")
        clean_build
        build_project
        ;;
    "test")
        build_project
        run_tests
        ;;
    "install")
        build_project
        install_project
        ;;
    "help"|"--help"|"-h")
        show_help
        ;;
    "build"|"")
        build_project
        ;;
    *)
        print_error "Unknown option: $1"
        echo
        show_help
        exit 1
        ;;
esac

echo
print_success "Done! λ"
