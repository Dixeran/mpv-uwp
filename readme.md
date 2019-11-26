# mpv-uwp

Basic thoughts of embedding mpv into UWP application with NO ANGLE required.

## What?

It’s an example about embedding MPV player into an UWP application. We create a SwapChainPanel element, which can be used to render d3d11 or d2d scene.  Since MPV player have an render context based on  3D11 and SPIRV-Cross, we now have a opportunity to borrow that render driver for our custom usage.

## How?

Firstly, I’m using a custom version of MPV player (https://github.com/Dixeran/mpv/tree/preview) which I have manipulate some codes around ra_d3d11 (really ugly, I confess).  Thus, following methods are exposed:

```cpp
// Get the swapchain that created by mpv render driver
IDXGISwapChain* mpv_get_swapchain(mpv_handle* ctx);

// Shared a custom d3d11 device to mpv.
void mpv_set_custom_d3d11device(mpv_handle* ctx, ID3D11Device* d3d11device);

// Struct to control resize.
typedef struct d3d11_comp_opts {
    int width;
    int height;
    bool is_fullscreen;
} d3d11_comp_opts;
void mpv_bind_d3d11_comp_opts(d3d11_comp_opts* opts);

// Rise resize event.
void mpv_invoke_d3d11_resize(void);
```

So we can render the video into our SwapChainPanel step by step:

1. Create a mpv context with `mpv_create()` and `mpv_initialize()`.
2. Create a D3D11 device, then use `mpv_set_custom_d3d11device()` to share your device into mpv.
3. Play a video file, mpv will run into the initialize process and create a swapchain.
4. Use `mpv_get_swapchain()` to bind it to SwapChainPanel.
5. Your video will render inside the SwapChainPanel. Use APIs in `client.h` to control the playback.

We use the d3d11_comp_opts struct to pass the current SCP size into mpv, then by calling `mpv_invode_d3d11_resize()`, VO will handle the resize event.