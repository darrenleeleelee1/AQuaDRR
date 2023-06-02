CXX = g++
CXXFLAGS = -std=c++14 -O3 -Wall -shared -fPIC `python3 -m pybind11 --includes` -I../inc
TARGET = router
SOURCES = ../src/router.cpp ../src/router_main.cpp binding.cpp
OBJECTS = $(addprefix ../obj/,$(notdir $(SOURCES:.cpp=.o)))

all: $(TARGET).so

$(TARGET).so: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

../obj/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

../obj/%.o: ../src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET).so
