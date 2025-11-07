NAME :=			run_renderer

## SOURCE FILES
SRCS :=			main.c

## OBJECT FILES
OBJS_DIR :=		obj
OBJS :=			$(addprefix $(OBJS_DIR)/, $(SRCS:.c=.o))

## DETECT OS
OS := $(shell uname -s)

## LIBRARIES
LIBS_DIR :=		lib
LDFLAGS :=		-L./${LIBS_DIR} # tell linker to look for libs here

# OS-specific libraries
ifeq ($(OS), Darwin) # Darwin is macOS
	LIBS = -lrender_x86_64 -framework Cocoa -framework AudioToolbox
	INFO = "Building for macOS"
else ifeq ($(OS), Linux) # Check for Linux
	LIBS = -lrender -lX11 -lasound
	INFO = "Building for Linux"
else # Stop compilation if the OS is not supported
	$(error Unsupported OS: '$(OS)'. Only 'Darwin' (macOS) and 'Linux' are supported.)
endif

## FORMATTING
BOLD :=		\033[1m
YELLOW :=	\033[33m
RED :=		\033[91m
GREEN :=	\033[92m
RESET :=	\033[0m

###########
## RULES ##
###########

# Default rule
all:	$(NAME)

# Rule to link the final executable
$(NAME):	$(OBJS)
	@echo "$(YELLOW)Linking...$(RESET)"
	@$(CC) $(OBJS) -o $(NAME) $(LDFLAGS) $(LIBS)
	@echo "$(GREEN)$(BOLD)\n$(NAME)$(RESET)$(GREEN) successfully compiled. ($(INFO))$(RESET)"

# Pattern rule to compile .c files into .o files
$(OBJS_DIR)/%.o: %.c | $(OBJS_DIR)
	@echo "$(YELLOW)Compiling$(RESET)  $<...$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

# Rule to create the object directory
$(OBJS_DIR):
	@mkdir -p $@

## Clean up ##

clean:
	@echo "$(RED)Cleaning object files...$(RESET)"
	@rm -rf $(OBJS_DIR)

fclean: clean
	@echo "$(RED)Cleaning executable...$(RESET)"
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
