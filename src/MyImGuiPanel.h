#pragma once

#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw.h>
#include <imgui\imgui_impl_opengl3.h>

#include <string>

class MyImGuiPanel
{
public:
	MyImGuiPanel();
	virtual ~MyImGuiPanel();

public:
	void update();
	void setAvgFPS(const double avgFPS);
	void setAvgFrameTime(const double avgFrameTime);

	bool isNormalMappingEnabled() const { return m_normalMappingEnabled; }

private:
	double m_avgFPS;
	double m_avgFrameTime;

	// normal mapping switch
	bool m_normalMappingEnabled;
};

