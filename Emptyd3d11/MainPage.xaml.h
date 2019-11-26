//
// MainPage.xaml.h
// MainPage 类的声明。
//

#pragma once

extern "C" {
#include "mpv/render.h"
#include "mpv/client.h"
	IDXGISwapChain* mpv_get_swapchain(mpv_handle* ctx);
	void mpv_set_custom_d3d11device(mpv_handle* ctx, ID3D11Device* d3d11device);
	typedef struct d3d11_comp_opts {
		int width;
		int height;
		bool is_fullscreen;
	} d3d11_comp_opts;
	void mpv_bind_d3d11_comp_opts(d3d11_comp_opts* opts);
	void mpv_invoke_d3d11_resize(void);
}

#include "MainPage.g.h"

namespace Emptyd3d11
{
	/// <summary>
	/// 可用于自身或导航至 Frame 内部的空白页。
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

	private:
		void CreateDevice(); // create d3d11 device

		void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		mpv_handle* m_mpv;
		IDXGISwapChain* swap_chain;
		Windows::UI::Xaml::Controls::SwapChainPanel^ m_swapChainPanel;
		void Button_Click_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

		// Direct3D 对象。
		D3D_FEATURE_LEVEL								m_d3dFeatureLevel;
		Microsoft::WRL::ComPtr<ID3D11Device3>			m_d3dDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext3>	m_d3dContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain3>			m_swapChain;

		d3d11_comp_opts comp_opts;
		void m_swc_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
	};
}

namespace DX
{
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			// 在此行中设置断点，以捕获 Win32 API 错误。
			throw Platform::Exception::CreateException(hr);
		}
	}
}
