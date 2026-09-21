#pragma once
// Это аналог GetValues из Assault Cube
// Тут описываем то что мы будем читать и запоминать в структуры

// Здесь описываешь нормальные структуры твоей программы.
#include <string>

struct Vector3
{
	float x{};
	float y{};
	float z{};
};

struct UnitInfo
{
	std::uintptr_t id{};

	Vector3 position{};

	int team{};
	int type{};
	int state{};

	std::string name;
};