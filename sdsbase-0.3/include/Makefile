HEADERS = SharedData_pub.h \
	  SharedData.h \
	  color.h \
	  parea.h \
	  linestyle.h \
	  qpl.h \
	  symbolstyle.h \
	  util.h

all: ;

clean:
	rm -f *~

install:
	install -d -m 755 -o 0 -g 0 /usr/local/include/sd
	install -m 644 -o 0 -g 0 $(HEADERS) /usr/local/include/sd

