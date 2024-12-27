#pragma once
#include <functional>
#include <iostream>
#include <string>
#include <Windows.h>

static constexpr int plusKeyCode = 0xBB;

class Button
{
public:
	Button(std::string buttonName = "Button", int button = plusKeyCode);
	virtual ~Button() = default;
	void update();
	void setToggleButton(int button);
	bool wasClicked() const;
	bool isToggled() const;
	bool isKeyDown(unsigned short keyState) const;
	void setOnClickCallback(std::function<void()> callback);
	void setOnReleaseCallback(std::function<void()> callback);
	void setOnToggleOnCallback(std::function<void()> callback);
	void setOnToggleOffCallback(std::function<void()> callback);
	void setWhileToggledOnCallback(std::function<void()> callback);

private:
	std::function<void()> m_onClickCallback;
	std::function<void()> m_onReleaseCallback;
	std::function<void()> m_onToggleOnCallback;
	std::function<void()> m_onToggleOffCallback;
	std::function<void()> m_whileToggledOnCallback;
	std::string m_buttonName;
	int m_button = 0;
	bool m_wasPreviousUpdateDown = false;
	bool m_newClick = false;
	bool m_toggled = false;
};
