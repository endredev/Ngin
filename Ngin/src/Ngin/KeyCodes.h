#pragma once

namespace Ngin {

	enum class Key : int
	{
		// Printable
		Space        = 32,
		Apostrophe   = 39,
		Comma        = 44,
		Minus        = 45,
		Period       = 46,
		Slash        = 47,

		D0 = 48, D1, D2, D3, D4, D5, D6, D7, D8, D9,

		Semicolon    = 59,
		Equal        = 61,

		A = 65, B, C, D, E, F, G, H, I, J, K, L, M,
		N,      O, P, Q, R, S, T, U, V, W, X, Y, Z,

		// Function keys
		Escape    = 256,
		Enter     = 257,
		Tab       = 258,
		Backspace = 259,
		Insert    = 260,
		Delete    = 261,
		Right     = 262,
		Left      = 263,
		Down      = 264,
		Up        = 265,

		F1  = 290, F2,  F3,  F4,  F5,  F6,
		F7  = 296, F8,  F9,  F10, F11, F12,

		LeftShift    = 340,
		LeftControl  = 341,
		LeftAlt      = 342,
		RightShift   = 344,
		RightControl = 345,
		RightAlt     = 346,
	};

}
