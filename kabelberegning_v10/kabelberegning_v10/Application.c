#include <windows.h>
#include <windowsx.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <string.h>
#include <tchar.h>
#include <strsafe.h>
#include "resource.h"
#include "Utility.h"


#define REDPEN CreatePen(PS_SOLID,1,RGB(255,0,0))
#define BLUEPEN CreatePen(PS_SOLID,1,RGB(0,0,255))
#define GREENPEN CreatePen(PS_SOLID,1,RGB(0,255,0))
#define BLACKPEN CreatePen(PS_SOLID,1,RGB(0,0,0))
#define WHITEPEN CreatePen(PS_SOLID,1,RGB(255,255,255))

PRIVATE BOOL hasDrawnCoords = FALSE;


VOID SetupEditControls(HWND hwnd)
{
	HWND child;
	TCHAR szText[128];
	child = GetDlgItem(hwnd,IDC_GOD_OHM);
	LoadString(GetModuleHandle(0),IDS_GOD_OHM,szText,sizeof(szText) / sizeof(szText[0]));
	Static_SetText(child,szText);
	child = GetDlgItem(hwnd,IDC_GOD_LEN);
	LoadString(GetModuleHandle(0),(IDS_STRING103),szText,sizeof(szText) / sizeof(szText[0]));
	Static_SetText(child,szText);
	child = GetDlgItem(hwnd,IDC_BAD_OHM);
	LoadString(GetModuleHandle(0),(IDS_STRING104),szText,sizeof(szText) / sizeof(szText[0]));
	Static_SetText(child,szText);
	child = GetDlgItem(hwnd,IDC_BAD_LEN);
	LoadString(GetModuleHandle(0),(IDS_STRING105),szText,sizeof(szText) / sizeof(szText[0]));
	Static_SetText(child,szText);
	child = GetDlgItem(hwnd,IDC_ZFOR);
	LoadString(GetModuleHandle(0),(IDS_STRING106),szText,sizeof(szText) / sizeof(szText[0]));
	Static_SetText(child,szText);
	child = GetDlgItem(hwnd,IDC_RESULTAT);
	LoadString(GetModuleHandle(0),(IDS_STRING107),szText,sizeof(szText) / sizeof(szText[0]));
	Static_SetText(child,szText);

}


//////////////////////////////////////////////////////////////////////////
/// Linked List for the values
//////////////////////////////////////////////////////////////////////////
typedef struct ListElmt_
{
	void *data;
	struct ListElmt_ *next;
}ListElmt;

typedef struct List_
{
	int size;
	int (*match) (const void *key1,const void *key2);
	void (*destroy)(void*data);
	ListElmt *head;
	ListElmt *tail;

}List;

void list_init(List *list, void(*destroy)(void *data))
{

}

void list_destroy(List *list)
{


}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 


PRIVATE VOID PlotA(HDC hdc,HPEN colorPen,DOUBLE x1,DOUBLE y1,DOUBLE x2,DOUBLE y2,INT width,INT height,INT pad)
{
	
	SelectPen(hdc,colorPen);
	MoveToEx (hdc,x1,height + (-1 * y1),NULL);
	LineTo(hdc,x2,height + (-1 * y2));
	DeletePen(colorPen);
	//Ellipse (hdc,x+4, height + (-1 * y)+4,x-4, height + (-1 * y)-4);
}
PRIVATE VOID DrawShortLine(HDC hdc,INT x,INT y)
{
	MoveToEx (hdc,x-5,y,NULL);
	LineTo (hdc,x+5,y);
}
/*
 * height - pad / max = y scale
 * max - The biggest value in the matrix
 * size - The value you want to compare against
 * pad - padding from one value to the next value
 */
PRIVATE DOUBLE Scale(DOUBLE max,INT size,INT pad)
{
	DOUBLE result;
	result = size - pad / max;
	return result;
}

PRIVATE VOID PaintContent(HWND hwnd,HDC hdc)
{
	
	int width = 200;
	int height = 200;
	int pad = 20;
	int offset = 5;
	int centerX,centerY;
	DOUBLE x,y,scale,inc;
	DOUBLE x1,x2,y1,y2;
	int tellerX,tellerY;
	INT yMin,yMax,xMax,xMin;
	HPEN grayPen = CreatePen(PS_SOLID,1,RGB(128,128,128));

	Rectangle(hdc,offset,offset,width,height);
	centerX = (width + offset) / 2;
	centerY = (height + offset) / 2;
	
	
	if(hasDrawnCoords == TRUE)
	{	
		yMin = height;
		yMax = offset;
		xMin = width;
		xMax = offset;
		
		x1 = 100.0;
		x2 = 150.0;
		y1 = 75.0;
		y2 = 57.0;
		scale = Scale (y1,height,pad);
		y = height + (-1 * y1) + pad;
		printf("y = %.2f\n",y);
		y = height + (-1 * y2) + pad;
		printf("y = %.2f\n",y);
		MoveToEx (hdc,offset,centerY,NULL);
		LineTo (hdc,width,centerY);
		MoveToEx (hdc,centerX,offset,NULL);
		LineTo (hdc,centerX,height);
	}
	
}
PRIVATE VOID OnPaint(HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint (hwnd,&ps);
	HFONT hFont = (HFONT)SendMessage(hwnd,WM_GETFONT,0,0);
	SetBkMode(hdc,TRANSPARENT);
	SelectFont(hdc,hFont);
	PaintContent(hwnd,hdc);
	EndPaint (hwnd,&ps);
}

DOUBLE GetDoubleMax(DOUBLE data[],int len)
{
	DOUBLE ymin = DBL_MAX;
	DOUBLE ymax = -DBL_MAX;
	INT i;
	for(i = 0; i < len; i++)
	{
		if(data[i] > ymax)
			return data[i];
	}
	return ymax;
}

VOID Setup_List(HWND hwnd)
{
	/*
	 * X1 = GOD Ohm
	 * Y1 = God længde
	 * X2 = Dårlig ohm
	 * Y2 = Dårlig længde
	 */
	double x1,x2,y1,y2,zfor,resultat;
	DOUBLE yMax,xMax,yDiff,xDiff,yMin,xMin;
	DOUBLE yScale,xScale;
	DOUBLE xP,yP;
	TCHAR szBuffer[256];
	HDC hdc;
	Pair *xPair,*yPair;
	float xx,*yy;
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////	
	//////////////////////////////////////////////////////////////////////////	

	x1 = Edit_GetDouble (hwnd,IDC_EDIT1);
	y1 = Edit_GetDouble (hwnd,IDC_EDIT2);
	x2 = Edit_GetDouble (hwnd,IDC_EDIT3);
	y2 = Edit_GetDouble (hwnd,IDC_EDIT4);
	zfor = Edit_GetDouble (hwnd,IDC_EDIT5);
	resultat =  y1 - (y1 - y2) * ((zfor - x1) /( x2 - x1));
	StringCchPrintf(szBuffer,sizeof(szBuffer) / sizeof(szBuffer[0]),TEXT("%.2f"),resultat);	
	Edit_SetText(GetDlgItem(hwnd,IDC_EDIT6),szBuffer);
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////// 

	printf("%.2f\n",xx);
}
BOOL CALLBACK AppDlgProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	HANDLE_MSG(hwnd,WM_PAINT,OnPaint);
	case WM_INITDIALOG:
		SetupEditControls (hwnd);
		
		break;
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDOK:
				{
					Setup_List(hwnd);
				}
				break;
			}
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog (hwnd,0);
		break;
	default:
		break;
	}
	return 0;
}
int main(int argc, char **argv){
	
	DialogBox(0,MAKEINTRESOURCE(IDD_DIALOG1),0,&AppDlgProc);
	//DOUBLE x1,x2,y1,y2,y,x;
	//Pair *one, *two;
	//x1 = 100;
	//x2 = 150;
	//y1 = 75;
	//y2 = 57;


	//one = Make_Pair (&x1,&y1);
	//two = Make_Pair (&x2,&y2);
	//printf("Max %.2f\n\nyScale %.2f\n\n",GetMaxPair (one,two),GetScale (GetMaxPair (one,two),200,20));
	//y = y1 * GetScale (GetMaxPair (one,two),200,20);
	//printf("y = %.2f\n\n",y);
	//y = y2 * GetScale (GetMaxPair (one,two),200,20);
	//printf("y = %.2f\n\n",y);
	//x = GetIncrement (x1,200,20);
	//printf("x = %.2f\n\n",x);
#ifdef _DEBUG
	system("pause");
#endif

	return 0;
}