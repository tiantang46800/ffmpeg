#! /bin/sh
gcc encoder.c  -g -o a.out \
 -lavformat -lavcodec -lavutil -lm \
-lpthread -ldl -lrt -lm \
-lavutil \
-lswscale \
-lz  \
-lx264 \
-lmp3lame \
-lswresample \
-lstdc++
#-I /usr/local/include -L /usr/local/lib -lavformat -lavcodec -lavutil -lm \
