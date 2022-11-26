#!/bin/bash

component="-DBUILD_ALL_COMPONENTS=ON"
ut=""
build_only=false
cmake_only=false
clear=false

while [[ $# -gt 0 ]]; do
    case $1 in
    --common)
        component=""
        shift
        ;;
    --node)
        component="-DBUILD_NODE=ON"
        shift
        ;;
    --gateway)
        component="-DBUILD_GATEWAY=ON"
        shift
        ;;
    --client)
        component="-DBUILD_CLIENT=ON"
        shift
        ;;
    --ut)
        ut="-DENABLE_UNIT_TESTS=ON"
        shift
        ;;
    --cmake_only)
        cmake_only=true
        shift
        ;;
    --build_only)
        build_only=true
        shift
        ;;
    --clear)
        clear=true
        shift
        ;;
    *)
        echo "Unknown option $1"
        exit 1
        ;;
    esac
done

if [[ "${clear}" = true ]]; then
    rm -rf build &> /dev/null
fi

if [[ "${cmake_only}" = true ]]; then
    cmake -S . -B build -DBUILD_ALL_COMPONENTS=ON -DENABLE_UNIT_TESTS=ON
    exit 0
fi

if [[ "${build_only}" = true ]]; then
    cmake --build build -j $(nproc)
    exit 0
fi

cmake -S . -B build "${component}" "${ut}" 
cmake --build build -j $(nproc)
