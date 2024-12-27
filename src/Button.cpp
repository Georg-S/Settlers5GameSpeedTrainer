#include "Button.hpp"

Button::Button(std::string buttonName, int button)
	: m_buttonName(std::move(buttonName))
	, m_button(button)
{
}

void Button::update()
{
	const int keyState = GetAsyncKeyState(m_button);

	auto callCallback = [](const std::function<void()>& callback) 
	{
		if (callback)
			callback();
	};

	if (isKeyDown(keyState))
	{
		if (!m_wasPreviousUpdateDown)
		{
			m_newClick = true;
			m_toggled = !m_toggled;

			if (m_toggled)
				callCallback(m_onToggleOnCallback);
			else
				callCallback(m_onToggleOffCallback);

			callCallback(m_onClickCallback);
		}
		else
		{
			m_newClick = false;
		}

		m_wasPreviousUpdateDown = true;
	}
	else
	{
		if (m_wasPreviousUpdateDown)
			callCallback(m_onReleaseCallback);

		m_wasPreviousUpdateDown = false;
		m_newClick = false;
	}

	if (m_toggled)
		callCallback(m_whileToggledOnCallback);
}

bool Button::isKeyDown(unsigned short keyState) const
{
	static constexpr unsigned short mostSignificantBit = 0x8000;

	return keyState & mostSignificantBit;
}

void Button::setOnClickCallback(std::function<void()> callback)
{
	m_onClickCallback = std::move(callback);
}

void Button::setOnReleaseCallback(std::function<void()> callback)
{
	m_onReleaseCallback = std::move(callback);
}

void Button::setOnToggleOnCallback(std::function<void()> callback)
{
	m_onToggleOnCallback = std::move(callback);
}

void Button::setOnToggleOffCallback(std::function<void()> callback)
{
	m_onToggleOffCallback = std::move(callback);
}

void Button::setWhileToggledOnCallback(std::function<void()> callback)
{
	m_whileToggledOnCallback = std::move(callback);
}

void Button::setToggleButton(int button)
{
	m_button = button;
	update();
}

bool Button::wasClicked() const
{
	return m_newClick;
}

bool Button::isToggled() const
{
	return m_toggled;
}
