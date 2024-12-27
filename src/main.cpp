#include "MemoryManager.hpp"
#include "Button.hpp"

#include <optional>
#include <exception>

/// Offsets were found by creating pointer maps and then using a pointer scan with cheat engine
static const std::vector<uintptr_t> SETTLERS_5_GAME_SPEED_OFFSETS = { 0x495DF0, 0x40, 0x28 };
static const std::string SETTLERS_5_ENGLISH_NAME = "THE SETTLERS - Heritage of Kings";
static const std::wstring SETTLERS_5_GERMAN_NAME = L"DIE SIEDLER - Das Erbe der Könige";
static const std::string SETTLERS_5_MODULE = "settlershok.exe";

/**
	After creating some pointer maps and performing a pointer scan with cheat engine, we get a pointer to the desired variable
	In this case the game speed, however multiple hops must be made to reliable reach the game speed variable
*/
/// This function gets the address of the desired variable, with the offsets gotten from cheat engine
static uintptr_t getAddressWithMultipleOffsets(uintptr_t currentAddress, MemoryManager& memory, const std::vector<uintptr_t>& offsets, bool* success)
{
	for (int i = 0; i < offsets.size() - 1; i++)
	{
		const auto currentOffset = offsets[i];
		currentAddress += currentOffset;

		currentAddress = memory.read_memory<DWORD>(currentAddress, success);
		if (!success)
			return 0;
	}

	if (!offsets.empty())
		currentAddress += offsets.back();

	*success = true;
	return currentAddress;
}

namespace
{
	class Settlers5GameSpeedTrainer
	{
	public:
		Settlers5GameSpeedTrainer()
		{
			m_normalSpeed = Button("Normal speed", VK_F9);
			m_2xSpeed = Button("2 times speed", VK_F10);
			m_5xSpeed = Button("5 times speed", VK_F11);
			m_10xSpeed = Button("10 times speed", VK_F12);

			m_normalSpeed.setOnClickCallback([this]() { setCurrentGameSpeed(1.0); });
			m_2xSpeed.setOnClickCallback([this]() { setCurrentGameSpeed(2.0); });
			m_5xSpeed.setOnClickCallback([this]() { setCurrentGameSpeed(5.0); });
			m_10xSpeed.setOnClickCallback([this]() { setCurrentGameSpeed(10.0); });
		};

		bool attachToProcess() 
		{
			if (m_memory.is_attached_to_process())
				return true;

			if (!m_memory.attach_to_process(SETTLERS_5_ENGLISH_NAME.c_str())
				&& !m_memory.attach_to_process(SETTLERS_5_GERMAN_NAME.c_str())) 
			{
				std::cout << "Error: Unable to attach to the settlers5 process" << std::endl;
				return false;
			}

			std::cout << "Successfully attached to settlers5 process" << std::endl;

			m_moduleAddress = m_memory.get_module_address(std::regex("settlershok.*\\.exe"));
			if (!m_moduleAddress) 
			{
				std::cout << "Critical Error: Unable to get module address for settlers5, is the executable named settlershok?" << std::endl;
				throw std::exception();
			}

			return true;
		}

		std::optional<double> readCurrentGameSpeed()
		{
			if (!attachToProcess())
				return {};

			bool success = false;
			auto gameSpeedAddress = getAddressWithMultipleOffsets(m_moduleAddress, m_memory, SETTLERS_5_GAME_SPEED_OFFSETS, &success);
			if (!success)
			{
				std::cout << "Error: Unable to get game speed: was not able to get the game speed address" << std::endl;
				return {};
			}

			auto gameSpeed = m_memory.read_memory<double>(gameSpeedAddress, &success);
			if (!success) 
			{
				std::cout << "Error: Unable to get game speed: was not able to get the game speed from the read address" << std::endl;
				return {};
			}
			std::cout << "Current game speed" << gameSpeed << std::endl;
			return gameSpeed;
		}

		void setCurrentGameSpeed(double value)
		{
			if (!attachToProcess())
				return;

			bool success = false;
			auto gameSpeedAddress = getAddressWithMultipleOffsets(m_moduleAddress, m_memory, SETTLERS_5_GAME_SPEED_OFFSETS, &success);
			if (!success) 
			{
				std::cout << "Error: Unable to set game speed: was not able to get the game speed address" << std::endl;
				return;
			}

			if (!m_memory.write_memory<double>(gameSpeedAddress, value)) 
			{
				std::cout << "Error: Unable to set game speed: was not able to write the new speed value" << std::endl;
				return;
			}

			std::cout << "Set game speed to " << std::to_string(value) << std::endl;
		}

		void run() 
		{
			while (true) 
			{
				Sleep(1);
				for (auto button : m_buttons)
					button->update();
			}
		}

	private:
		MemoryManager m_memory;
		uintptr_t m_moduleAddress;
		Button m_normalSpeed;
		Button m_2xSpeed;
		Button m_5xSpeed;
		Button m_10xSpeed;
		Button* m_buttons[4] = { &m_normalSpeed, &m_2xSpeed, &m_5xSpeed, &m_10xSpeed };
	};
}

int main(int argc, char* argv[])
{
	try
	{
		Settlers5GameSpeedTrainer trainer;
		trainer.run();
	}
	catch (const std::exception& e)
	{
		std::cout << "Critical error occured, quitting trainer" << std::endl;
	}

	return 0;
}
