#pragma once


struct Caret
{
	float X;
	float Y;
	float Height;
	float Width;


	/*auto DrawCaret() -> void
	{
		g->DrawLine(D2D1::Point2F(X, Y), D2D1::Point2F(Width, Height));
	}*/
};

struct Mouse
{
	float X1;
	float Y1;
	float X2;
	float Y2;
	bool Moving;
	bool Down;
};
