CXX = g++

INCDIR = include
SRCDIR = src
LIBDIR = lib
OBJDIR = obj
BINDIR = bin

INCFLAGS = -I./$(INCDIR)/
LIBFLAGS = -L./$(LIBDIR)/ 

all: clean $(LIBDIR)/libDlibFaceDetect.a
	$(CXX) src/main.cpp $(DLIB_INSTALL)/dlib/all/source.cpp $(INCFLAGS) $(LIBFLAGS) -lDlibFaceDetect -lX11 -DDLIB_JPEG_SUPPORT -ljpeg -o $(BINDIR)/output `pkg-config opencv --cflags --libs`

$(LIBDIR)/libDlibFaceDetect.a: $(OBJDIR)/DlibFaceDetect.o $(OBJDIR)/ReadDirectory.o
	ar -crs $(LIBDIR)/libDlibFaceDetect.a $(OBJDIR)/DlibFaceDetect.o $(OBJDIR)/ReadDirectory.o

$(OBJDIR)/DlibFaceDetect.o:
	$(CXX) -c $(INCFLAGS) $(SRCDIR)/DlibFaceDetect.cpp -o $(OBJDIR)/DlibFaceDetect.o `pkg-config opencv --cflags --libs` -O3 -DDLIB_JPEG_SUPPORT -lpthread -lX11 -ljpeg -DUSE_AVX_INSTRUCTIONS=ON
	
$(OBJDIR)/ReadDirectory.o:
	$(CXX) -c $(INCFLAGS) $(SRCDIR)/ReadDirectory.cpp -o $(OBJDIR)/ReadDirectory.o
	
clean: 
	rm -f $(OBJDIR)/*.o  $(LIBDIR)/lib* a.out $(BINDIR)/output
