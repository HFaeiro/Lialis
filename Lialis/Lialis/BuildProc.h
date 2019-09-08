#pragma once
#include "stdafx.h"
#include "Resource.h"
#include "Theme.h"


LRESULT CALLBACK BuildProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

/*

//Problem 1//

Button struct hold items from list starting from the position of a scroll bar.
Each time you scroll down/Up the first item in Buttons is the position of the bar.

example:
if the scroll position is 13. 
Then Button[0] will equal List[13],
Button[1] will equal List[14],
Button[2] will equal List[15] and so on. 

//Problem 2//

My application will need to sort this List alphabetically.
I'm not sure how to order anything. Need your help with this one mostly JT.

*/

//struct Buttons {
//	HWND hWnd;
//	wchar_t name;
//};
//
//struct List {
//	wchar_t name;
//
//};
//Buttons buttons[13]; //thinking about using vectors to make this dynamic
//List list[265]; //thinking about using vectors to make this dynamic
//
//static void
//Insert(int Positon) {
//
//	for (int i = 0; i <= sizeof(buttons); i++)
//	{
//		buttons[i].name = list[Positon + i].name; //not worried about error checking for end of list at the moment
//	}
//
//
//}
//
//static void 
//Sort(int iAction)
//{
//	switch (iAction)
//	{
//	case BYNAME:
//		//not sure how to do this 
//	}
//}