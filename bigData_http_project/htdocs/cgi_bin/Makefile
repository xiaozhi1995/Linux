.PHONY:all 
all:cgi_add cgi_sub cgi_mul cgi_div cgi_remain

cgi_add:cgi_add.cpp cgi_math.cpp
	g++ -o $@ $^
cgi_sub:cgi_sub.cpp cgi_math.cpp
	g++ -o $@ $^
cgi_mul:cgi_mul.cpp cgi_math.cpp
	g++ -o $@ $^
cgi_div:cgi_div.cpp cgi_math.cpp
	g++ -o $@ $^
cgi_remain:cgi_remain.cpp cgi_math.cpp
	g++ -o $@ $^
.PHONY:clean
clean:
	rm -f cgi_add cgi_sub cgi_mul cgi_div cgi_remain

