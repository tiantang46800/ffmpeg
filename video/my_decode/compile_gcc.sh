#! /bin/sh
#gcc decoder.c -static -g -o a.out \
gcc decoder.c  -g -o a.out \
-lx264 -lavformat -lavcodec -lavutil -lm \
-lpthread -ldl -lrt -lm \
-lavutil \
-lswscale \
-lz  \
-lx264 \
-lmp3lame \
-lswresample \
-lstdc++
