CXX = g++

INCDIR = include
SRCDIR = src
LIBDIR = lib
OBJDIR = obj
BINDIR = bin

INCFLAGS = -I./$(INCDIR)/
LIBFLAGS = -L./$(LIBDIR)/ 

all: clean $(LIBDIR)/libHaarCascades.a
	$(CXX) src/main.cpp $(INCFLAGS) $(LIBFLAGS) -lHaarCascades -o $(BINDIR)/output `pkg-config opencv --cflags --libs`

$(LIBDIR)/libHaarCascades.a: $(OBJDIR)/HaarCascades.o $(OBJDIR)/ReadDirectory.o
	ar -crs $(LIBDIR)/libHaarCascades.a $(OBJDIR)/HaarCascades.o $(OBJDIR)/ReadDirectory.o

$(OBJDIR)/HaarCascades.o:
	$(CXX) -c $(INCFLAGS) $(SRCDIR)/HaarCascades.cpp -o $(OBJDIR)/HaarCascades.o `pkg-config opencv --cflags --libs`
	
$(OBJDIR)/ReadDirectory.o:
	$(CXX) -c $(INCFLAGS) $(SRCDIR)/ReadDirectory.cpp -o $(OBJDIR)/ReadDirectory.o
	
clean: 
	rm -f $(OBJDIR)/*.o  $(LIBDIR)/lib* a.out $(BINDIR)/output
