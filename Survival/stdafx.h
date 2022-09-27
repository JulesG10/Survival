#pragma once

#ifndef STDAFX_H

#define STDAFX_H
#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include<string>
#include<vector>

#include<raylib.h>
#include<raymath.h>
#include<rlgl.h>


#if defined(_WIN32)           
#define NOGDI
#define NOUSER
#endif

#include <Windows.h>


#if defined(_WIN32)        
#undef near
#undef far
#endif

#include <ode/ode.h>


inline std::string SGetParentPath(std::string p)
{
	return p.substr(0, p.find_last_of("/\\"));
}

inline std::string SGetApplicationPath()
{
	char* filepath = new char[MAX_PATH];
	GetModuleFileNameA(NULL, filepath, MAX_PATH);
	return SGetParentPath(filepath);
}

inline std::string Vector3String(Vector3 vec)
{
	return "\nx: " + std::to_string(vec.x) + "\ny : " + std::to_string(vec.y) + "\nz : " + std::to_string(vec.z);
}

#endif