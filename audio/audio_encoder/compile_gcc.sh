#! /bin/sh
gcc audio_encoder.c  -g -o a.out \
-lavformat -lavcodec -lavutil -lm \
-lpthread -ldl -lrt -lm \
-lavutil \
-lswscale \
-lz  \
-lx264 \
-lmp3lame \
-lswresample \
-lstdc++
