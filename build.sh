gcc -fsanitize=address -fsanitize=undefined -g -o cdraw main.c quadtree.c window.c surface.c -lX11 -lm
