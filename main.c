// main.c
// Compile using aarch64:
// MacOS: cc  main.c -lrender_x86_64 -L. -framework Cocoa -framework AudioToolbox
// Linux: cc main.c -lrender -L. -lX11 -lasound

#include <stdio.h>	// printf()
#include <stdlib.h> // malloc(), EXIT_SUCCESS, EXIT_FAILURE, atexit()

#define RENDERER_STATE_SIZE	1072 // from memset(param_1, 0, 0x430) in gfx_create_context()

// REVERSE ENGINEERED STRUCT //

/**
 * The core data structure for the renderer library.
 *
 * This structure's layout was reverse-engineered from the librenderer binary
 * by analyzing the constructor (gfx_create_context), the main loop (gfx_loop)
 * and the rendering function (gfx_render).
 *
 * The library expects the caller to allocate this structure at a total
 * size of 1072 bytes (0x430), as determined from the memset call in
 * gfx_create_context.
 *
 * All fields are mapped to their exact memory offsets as used by the
 * library's internal functions.
 */
typedef struct s_state
{
	// --- gfx_create_context ---
	char	*title;			// Offset 0x00 (8 bytes, pointer):
	int		windowWidth;	// Offset 0x08 (4 bytes, int)
	int		windowHeight;	// Offset 0x0C (4 bytes, int)

	// --- gfx_render ---
	void*	frameBuffer;	// Offset 0x10 (8 bytes, pointer)

	// --- gfx_loop ---
	// Offset 0x18: Start of the keyboard state array.
	// This array fills the entire gap until the mouseX field.
	// Size = 0x41C (mouseX offset) - 0x18 (start offset) = 1028 bytes.
	char	keyBoardState[0x41C - 0x18];
	int		mouseX;		// Offset 0x41C (4 bytes, int)
	int		mouseY;		// Offset 0x420 (4 bytes, int)

	// Offset 0x424 (4 bytes, int): Bitmask for mouse buttons.
	// case 1 (press) sets bit 0, case 2 (release) clears it.
	int		mouseButtonState;

	// Offset 0x428 (8 bytes, pointer): Pointer to the NSWindow object.
	// Identified by the [_objc_msgSend(ptr, "contentView")] call.
	void*	windowPtr;

	// Total size = offset 0x428 + 8 bytes = 1072 bytes (0x430).
	// This matches the _memset size, so no further padding is needed.
}	t_state;

// API FUNCTIONS //

void	*gfx_create_context(void *state, int windowWidth, int windowHeight, void *windowTitle);
int		gfx_init_context(void *state);
int		gfx_loop(void *state, double param2, long state_ptr);
void	gfx_render(void *state, int animate);
void	gfx_sleep(int ms);
void	gfx_close(void *state);
int		gfx_get_width_screen(void);
int		gfx_get_height_screen(void);
char	*gfx_get_window_title(void);

// Global pointer to state struct
t_state	*g_state = NULL;

// Cleanup function to be called on exit, using global g_state
static void	cleanup(void)
{
	if (g_state)
	{
		printf("Window closing...\n");
		gfx_close(g_state);

		if (g_state->frameBuffer)
		{
			printf("Freeing frame buffer...\n");
			free(g_state->frameBuffer);
		}

		free(g_state);
		printf("Cleanup complete. Exiting.\n");
	}
}

int	main(void)
{
	atexit(cleanup); // Register cleanup function to be called on exit

	// Allocate memory for the renderer state structure.
	g_state = (t_state *)malloc(RENDERER_STATE_SIZE);
	if (!g_state)
	{
		fprintf(stderr, "Error: Memory allocation for renderer state struct failed.\n");
		return (EXIT_FAILURE);
	}

	// Initialize the struct fields
	gfx_create_context(g_state, gfx_get_width_screen(), gfx_get_height_screen(), gfx_get_window_title());

	// Allocate frame buffer that gfx_render() will use.
	// Size = width * height * 4 bytes / pixel (RGBA)
	g_state->frameBuffer = malloc(gfx_get_width_screen() * gfx_get_height_screen() * 4);
	if (!g_state->frameBuffer)
	{
		fprintf(stderr, "Error: Memory allocation for frame buffer failed.\n");
		gfx_close(g_state);
		free(g_state);
		return (EXIT_FAILURE);
	}
	printf("Frame buffer allocated.\n");

	// Create the window and drawing context
	if (gfx_init_context(g_state) != 0)
	{
		fprintf(stderr, "Error: gfx_init_context() failed.\n");
		free(g_state);
		return (EXIT_FAILURE);
	}

	// Main rendering loop
	printf("Renderer running. Close the window to quit.\n");
	while (1)
	{
		gfx_loop(g_state, 0.00, 0L);	// Process events (keyboard, mouse, etc.)
		gfx_render(g_state, 1);		// Render the current frame
		gfx_sleep(0);				// nanosleep call with '0' -> animate as fast as possible without hogging entire CPU
	}

	// This is never reached due to the infinite loop. Cleanup is handled by the atexit() function.
	return (EXIT_SUCCESS);
}
