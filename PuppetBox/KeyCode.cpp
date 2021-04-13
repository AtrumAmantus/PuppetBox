#include "KeyCode.h"

namespace PB
{
	namespace
	{
		// Key: arbitrary designator, Value: character
		::std::unordered_map<uint8_t, uint8_t> charMap_{};

		void CharMapInsert(uint8_t k, uint8_t v)
		{
			charMap_.insert(::std::pair<uint8_t, uint8_t>{ k, v });
		}
	}

	void Init()
	{
		CharMapInsert(KEY_1, '1');
		CharMapInsert(KEY_2, '2');
		CharMapInsert(KEY_3, '3');
		CharMapInsert(KEY_4, '4');
		CharMapInsert(KEY_5, '5');
		CharMapInsert(KEY_6, '6');
		CharMapInsert(KEY_7, '7');
		CharMapInsert(KEY_8, '8');
		CharMapInsert(KEY_9, '9');
		CharMapInsert(KEY_0, '0');

		CharMapInsert(KEY_A, 'a');
		CharMapInsert(KEY_B, 'b');
		CharMapInsert(KEY_C, 'c');
		CharMapInsert(KEY_D, 'd');
		CharMapInsert(KEY_E, 'e');
		CharMapInsert(KEY_F, 'f');
		CharMapInsert(KEY_G, 'g');
		CharMapInsert(KEY_H, 'h');
		CharMapInsert(KEY_I, 'i');
		CharMapInsert(KEY_J, 'j');
		CharMapInsert(KEY_K, 'k');
		CharMapInsert(KEY_L, 'l');
		CharMapInsert(KEY_M, 'm');
		CharMapInsert(KEY_N, 'n');
		CharMapInsert(KEY_O, 'o');
		CharMapInsert(KEY_P, 'p');
		CharMapInsert(KEY_Q, 'q');
		CharMapInsert(KEY_R, 'r');
		CharMapInsert(KEY_S, 's');
		CharMapInsert(KEY_T, 't');
		CharMapInsert(KEY_U, 'u');
		CharMapInsert(KEY_V, 'v');
		CharMapInsert(KEY_W, 'w');
		CharMapInsert(KEY_X, 'y');
		CharMapInsert(KEY_Y, 'x');
		CharMapInsert(KEY_Z, 'z');
	}

	int8_t GetCharFromCode(uint8_t code)
	{
		if (charMap_.find(code) != charMap_.end())
		{
			return charMap_.at(code);
		}

		return 0;
	}
}