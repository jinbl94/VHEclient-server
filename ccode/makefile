TARGET=vhe
OBJS=vhe.o main.o
CC=g++
FLAGS=-lntl -g

default:$(TARGET)
$(TARGET):$(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(FLAGS)
$(OBJS):%.o:%.cpp %.h
	$(CC) -c $< -o $@ $(FLAGS)

.PHONY:clean
clean:
	@rm -f $(OBJS) $(TARGET)
