#!/bin/bash

g++ -std=c++0x tcp_test.cc date_time.cc socket_utils.cc main.cc -o pingpong
