//
// MainPage.xaml.cpp
// MainPage 类的实现。
//

#include "pch.h"
#include "MainPage.xaml.h"
#include <windows.ui.xaml.media.dxinterop.h>

using namespace Emptyd3d11;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

using namespace D2D1;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;
using namespace Platform;

// https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x804 上介绍了“空白页”项模板

void Emptyd3d11::MainPage::CreateDevice()
{
	// 此标志为颜色通道排序与 API 默认设置不同的图面
	// 添加支持。要与 Direct2D 兼容，必须满足此要求。
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	// 此数组定义此应用程序将支持的 DirectX 硬件功能级别组。
	// 请注意，应保留顺序。
	// 请不要忘记在应用程序的说明中声明其所需的
	// 最低功能级别。除非另行说明，否则假定所有应用程序均支持 9.1。
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	// 创建 Direct3D 11 API 设备对象和对应的上下文。
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	HRESULT hr = D3D11CreateDevice(
		nullptr,					// 指定 nullptr 以使用默认适配器。
		D3D_DRIVER_TYPE_HARDWARE,	// 创建使用硬件图形驱动程序的设备。
		0,							// 应为 0，除非驱动程序是 D3D_DRIVER_TYPE_SOFTWARE。
		creationFlags,				// 设置调试和 Direct2D 兼容性标志。
		featureLevels,				// 此应用程序可以支持的功能级别的列表。
		ARRAYSIZE(featureLevels),	// 上面的列表的大小。
		D3D11_SDK_VERSION,			// 对于 Microsoft Store 应用，始终将此值设置为 D3D11_SDK_VERSION。
		&device,					// 返回创建的 Direct3D 设备。
		&m_d3dFeatureLevel,			// 返回所创建设备的功能级别。
		&context					// 返回设备的即时上下文。
	);

	if (FAILED(hr))
	{
		// 如果初始化失败，则回退到 WARP 设备。
		// 有关 WARP 的详细信息，请参阅: 
		// https://go.microsoft.com/fwlink/?LinkId=286690
		DX::ThrowIfFailed(
			D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_WARP, // 创建 WARP 设备而不是硬件设备。
				0,
				creationFlags,
				featureLevels,
				ARRAYSIZE(featureLevels),
				D3D11_SDK_VERSION,
				&device,
				&m_d3dFeatureLevel,
				&context
			)
		);
	}

	// 将指针存储到 Direct3D 11.3 API 设备和即时上下文中。
	DX::ThrowIfFailed(
		device.As(&m_d3dDevice)
	);

	DX::ThrowIfFailed(
		context.As(&m_d3dContext)
	);
}

MainPage::MainPage() : m_d3dFeatureLevel(D3D_FEATURE_LEVEL_9_1)
{
	InitializeComponent();
	m_swapChainPanel = m_swc;
	swap_chain = NULL;
	CreateDevice();

	m_mpv = mpv_create();
	int error = 0;
	error = mpv_set_property_string(m_mpv, "vo", "gpu");
	error = mpv_set_property_string(m_mpv, "gpu-context", "d3d11");
	error = mpv_set_property_string(m_mpv, "gpu-api", "d3d11");
	error = mpv_initialize(m_mpv);

	// mpv set context
	m_d3dDevice->AddRef();
	mpv_set_custom_d3d11device(m_mpv, m_d3dDevice.Get());
}


void Emptyd3d11::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	std::string path = std::string("test.mp4");
	const char* cmd[] = { "loadfile", path.c_str(), NULL };
	mpv_command(m_mpv, cmd);
}


void Emptyd3d11::MainPage::Button_Click_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	swap_chain = mpv_get_swapchain(m_mpv);

	mpv_bind_d3d11_comp_opts(&comp_opts);

	m_swapChainPanel->Dispatcher->RunAsync(CoreDispatcherPriority::High, ref new DispatchedHandler([=]()
		{
			//获取 SwapChainPanel 的受支持的本机接口
			ComPtr<ISwapChainPanelNative> panelNative;
			DX::ThrowIfFailed(
				reinterpret_cast<IUnknown*>(m_swapChainPanel)->QueryInterface(IID_PPV_ARGS(&panelNative))
			);

			DX::ThrowIfFailed(
				panelNative->SetSwapChain(swap_chain)
			);
		}, CallbackContext::Any));
}

void Emptyd3d11::MainPage::m_swc_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
{
	comp_opts.height = e->NewSize.Height;
	comp_opts.width = e->NewSize.Width;
	mpv_invoke_d3d11_resize();
}
