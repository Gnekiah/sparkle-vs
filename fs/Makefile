OBJS := main.o 

all: $(OBJS)
	cp *.o $(OBJ_DIR)

$(OBJS): %.o:%.c
	$(CC) $(CC_FLAGS) -c $^ -o $@ $(INC_DIR)

.PHONY: clean

clean:
	rm -f $(OBJS)
