# aryyya, 2016
# Use `make` to build anji.
# Use `make install` to build anji and install it to the system.

APP_NAME=anji
CFLAGS=-Wextra -Wall -std=c11           \
       -Wno-missing-braces              \
       -Wno-missing-field-initializers

# Build anji.
$(APP_NAME): src/*.c
	$(CC) $(CFLAGS) -I./src src/*.c -o $(APP_NAME)

# Clean anji build files.
.PHONY: clean
clean:
	rm $(APP_NAME)

# Install anji to the system.
install: $(APP_NAME)
	cp $(APP_NAME) /usr/local/bin/
