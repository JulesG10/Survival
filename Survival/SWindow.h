#pragma once

#include "stdafx.h"
#include "SWorld.h"

class SWindow
{
public:
	SWindow();
	int Start(char*);

	friend void* WindowLoadThread(void* data);
private:
	
	void UpdateFrame();
	
	void Load();
	void UnLoad();

	SWorld world;
	bool loading = true;
};

static void* WindowLoadThread(void* data);