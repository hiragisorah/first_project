#include "main_window.hpp"

#include <seed/window>

#define WIN32_LEAD_AND_MEAN
#include <Windows.h>
#include <unordered_map>

//static std::unordered_map<unsigned int, bool> _key_map;

static LRESULT __stdcall WndProc(HWND hwnd, UINT i_msg, WPARAM w_param, LPARAM l_param)
{
	switch (i_msg)
	{
	case WM_DESTROY: PostQuitMessage(0); break;
	case WM_ACTIVATEAPP:
	case WM_KEYDOWN:
		//_key_map[w_param] = true;
		//break;
	case WM_KEYUP:
		//_key_map[w_param] = false;
		//break;
	case WM_SYSKEYUP: break;
	default:
		return DefWindowProc(hwnd, i_msg, w_param, l_param);
	}
	return 0;
}

class Seed::MainWindow::Impl
{
	friend Seed::MainWindow;

private:
	Seed::Window window_;
};

Seed::MainWindow::MainWindow(void)
	: impl_(std::make_unique<MainWindow::Impl>())
{

}

Seed::MainWindow::~MainWindow(void)
{

}

const bool Seed::MainWindow::Initialize(void)
{
	return this->impl_->window_.Initalize("window_title", 1280U, 720U);
}

const bool Seed::MainWindow::Run(void)
{
	return this->impl_->window_.Run();
}

const bool Seed::MainWindow::Finalize(void)
{
	return this->impl_->window_.Finalize();
}

void Seed::MainWindow::Destroy(void)
{
	delete this;
}

Seed::MainWindow * Seed::MainWindowDLL::Make(void)
{
	return new Seed::MainWindow;
}
