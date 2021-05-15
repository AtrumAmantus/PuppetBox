#pragma once

struct ImageOptions
{
	const enum Mode {
		REPEAT,
		CLAMP_TO_BORDER
	};
	Mode repeatMode = REPEAT;
	float colour[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
};