#include <windows.h>
#include <windowsx.h>

#include <stdio.h>
#include <stdlib.h>
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
DOUBLE Edit_GetDouble(HWND hwnd,UINT uID)
{
	HWND child;
	TCHAR szAnsiBuffer[256];
	double result;
	int len;

	child = GetDlgItem(hwnd,uID);
	len = Edit_GetTextLength(child) + 1;
	Edit_GetText(child,szAnsiBuffer,len);
	result = _ttof(szAnsiBuffer);
	return result;
}
DOUBLE GetMaxPair(Pair *pair1,Pair *pair2)
{
	double y1,y2;
	y1 = *(double*)pair1->second;
	y2 = *(double*)pair2->second;
	return max(y1,y2);
}
DOUBLE GetScale(DOUBLE y, DOUBLE height, DOUBLE pad)
{
	return ((height - pad) / y);
}
DOUBLE GetIncrement(DOUBLE x,DOUBLE width,DOUBLE pad)
{
	DOUBLE xInc = (width - 2 * pad ) / width;
	return xInc * x;
}
PRIVATE BOOL hasDrawnCoords = FALSE;

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
	HPEN blackPen = CreatePen(PS_SOLID,1,RGB(0,0,0));
	HPEN redPen = CreatePen(PS_SOLID,1,RGB(255,0,0));
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
		SelectPen(hdc,redPen);
		MoveToEx (hdc,offset,centerY,NULL);
		LineTo (hdc,width,centerY);
		MoveToEx (hdc,centerX,offset,NULL);
		LineTo (hdc,centerX,height);
		SelectPen(hdc,grayPen);
	}
	DeletePen(grayPen);
	DeletePen(blackPen);
	DeletePen(redPen);
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



VOID Setup_List(HWND hwnd)
{
	/*
	 * X1 = GOD Ohm
	 * Y1 = God l�ngde
	 * X2 = D�rlig ohm
	 * Y2 = D�rlig l�ngde
	 */
	double x1,x2,y1,y2,zfor,resultat;
	DOUBLE yMax,xMax,yDiff,xDiff;
	TCHAR szBuffer[256];
	HDC hdc;
	x1 = Edit_GetDouble (hwnd,IDC_EDIT1);
	y1 = Edit_GetDouble (hwnd,IDC_EDIT2);
	x2 = Edit_GetDouble (hwnd,IDC_EDIT3);
	y2 = Edit_GetDouble (hwnd,IDC_EDIT4);
	zfor = Edit_GetDouble (hwnd,IDC_EDIT5);
	resultat =  y1 - (y1 - y2) * ((zfor - x1) /( x2 - x1));
	StringCchPrintf(szBuffer,sizeof(szBuffer) / sizeof(szBuffer[0]),TEXT("%.2f"),resultat);	
	Edit_SetText(GetDlgItem(hwnd,IDC_EDIT6),szBuffer);
	StringCchPrintf(szBuffer,256,L"%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, ",x1,y1,x2,y2,zfor,resultat);
	MessageBox(hwnd,szBuffer,L"",MB_OK);
	xMax = max(x1,x2);
	yMax = max(y1,y2);
	yDiff = y
	PlotA (GetDC(hwnd),REDPEN,x1,y1,x2,y2,200,200,20);
	PlotA (GetDC (hwnd),BLUEPEN,zfor,resultat,zfor,resultat-(resultat-y2),200,200,20);
	
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