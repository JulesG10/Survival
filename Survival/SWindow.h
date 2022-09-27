#pragma once

#include "stdafx.h"
#include "SWorld.h"

class SWindow
{
public:
	SWindow();
	int Start(char*);

private:
	void UpdateFrame();

	void Load();
	void UnLoad();

	SWorld world;
};
