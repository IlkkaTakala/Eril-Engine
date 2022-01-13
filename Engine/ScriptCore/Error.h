#pragma once

void error(const char* string = 0);

bool isError();

const char* GetError();

void ClearError();