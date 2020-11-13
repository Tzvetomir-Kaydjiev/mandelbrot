# Prerequisites
bash, SDL2, gcc

# Build
make  
  
or  
  
make build_static for static linking of libSDL2   

# Usage
Usage:  
  mandelbrot [OPTIONS ....]  
OPTIONS:  
 -r or -rect followed by four floats x_start:x_end:y_start:y_end  
 -s or -size followed by two unsigned integers WIDTHxHEIGTH  
 -t or -tasks followed by one unsigned integer N  
 -g or -granularity followed by one unsigned integer N  
 -q or -quiet for quiet execution  
 -o or -output followed by string name  
    The name must be *.bmp because the program saves only bmp files.  