#pragma once

class OpenGLObject
{
public:
	virtual void CreateState() = 0;
	virtual void RefreshState() {};
	virtual void Clear() = 0;
};