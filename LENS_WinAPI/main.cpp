#define WIN32_LEAN_AND_MEAN //No agrega librerías que no se vayan a utilizar

#include <Windows.h>
#include <stdio.h>
#include <dinput.h>
#include <gdiplus.h>
#include <gl/gl.h>
#include <gl/glu.h>

using namespace Gdiplus;

//Variables constantes
bool cambio = true;
bool cambiox2 = true;
bool cambiox3 = true;
bool cambioy1 = true;
const int ANCHO_VENTANA = 800;
const int ALTO_VENTANA = 600;
const int BPP = 4;
const int TICK = 105;
int fanx1 = 200;
int fanx2 = 300;
int fanx3 = 550;
int fany1 = 100;
int fany2 = 200;
const unsigned int BLUE = 0xFF0000FF;
const unsigned int GREEN = 0xFF00FF00;
const unsigned int RED = 0xFFFF0000;
int movimiento = 0;
int movFondo = 0;
int indice = 0;
int indMov = 0;
bool escala;
bool colora = FALSE;
bool coloraa = FALSE;
bool coloraav = FALSE;
bool coloraavr = FALSE;

bool colorv = FALSE;
int contador = 0;
int contadora = 0;
int contadorav = 0;
int contadoravr = 0;

struct POSITION {
	int X;
	int Y;
};

struct DIMENSION {
	int ANCHO;
	int ALTO;
};

struct Input
{
	enum Keys
	{
		Backspace = 0x08, Tab,
		Clear = 0x0C, Enter,
		Shift = 0x10, Control, Alt,
		Escape = 0x1B,
		Space = 0x20, PageUp, PageDown, End, Home, Left, Up, Right, Down,
		Zero = 0x30, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
		A = 0x41, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		NumPad0 = 0x60, NumPad1, NumPad2, NumPad3, NumPad4, NumPad5, NumPad6, NumPad7, NumPad8, NumPad9,
		F1 = 0x70, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12
	};
}input;

//Variables Globales
bool mirror;
int *ptrBuffer;
int direccionGiro;
unsigned char * ptrBack;
unsigned char * ptrSprite;
unsigned char * ptrfantasma;

DIMENSION dmnBack;
DIMENSION dmnSprite;
DIMENSION dmnfantasma;
POSITION posFigura;

bool KEYS[256];

//Definicion de funciones
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void MainRender(HWND hWnd);
void Init();
void KeysEvents();
unsigned char * CargaImagen(WCHAR rutaImagen[], DIMENSION * dmn);
POSITION setPosition(int x, int y);
void DibujaFondo(int *buffer, int *imagen, DIMENSION dmn, bool mirror);
void DibujaSprite(int *buffer, int *sprite, DIMENSION dmn, POSITION pos);
void rotaFigura(int *ptrBuffer, int *ptrBmpSprite, DIMENSION dmn, POSITION posSprite, int direccion);
void rotaFigura1(int *ptrBuffer, int *ptrBmpSprite, DIMENSION dmn, POSITION posSprite, int direccion);
void rotaFigura2(int *ptrBuffer, int *ptrBmpSprite, DIMENSION dmn, POSITION posSprite, int direccion);

enum DireccionGiro {
	ARRIBA, DERECHA, ABAJO, IZQUIERDA
};

int WINAPI wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PWSTR pCmdLine,
	int nCmdShow)
{
	WNDCLASSEX wc;									// Windows Class Structure
	HWND hWnd;
	MSG msg;

	TCHAR szAppName[] = TEXT("MyWinAPIApp");
	TCHAR szAppTitle[] = TEXT("Aplicación en Lenguaje Ensamblador / C++");

	hInstance = GetModuleHandle(NULL);				// Grab An Instance For Our Window

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc = (WNDPROC)WndProc;					// WndProc Handles Messages
	wc.cbClsExtra = 0;									// No Extra Window Data
	wc.cbWndExtra = 0;									// No Extra Window Data
	wc.hInstance = hInstance;							// Set The Instance	
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);			// No Background Required For GL
	wc.lpszMenuName = NULL;									// We Don't Want A Menu	
	wc.lpszClassName = szAppName;							// Set The Class Name
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,
			L"Fallo al registrar clase (Failed To Register The Window Class).",
			L"ERROR",
			MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	hWnd = CreateWindowEx(
		WS_EX_CLIENTEDGE | WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,	// Extended Style For The Window
		szAppName,							// Class Name
		szAppTitle,							// Window Title
		WS_OVERLAPPEDWINDOW |				// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		0, 0,								// Window Position
		ANCHO_VENTANA,						// Calculate Window Width
		ALTO_VENTANA,						// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		hInstance,							// Instance
		NULL);								// Pass this class To WM_CREATE								

	if (hWnd == NULL) {
		MessageBox(NULL,
			L"Error al crear ventana (Window Creation Error).",
			L"ERROR",
			MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	Init();
	ShowWindow(hWnd, nCmdShow);
	SetFocus(hWnd);

	SetTimer(hWnd, TICK, TICK, NULL);
	ZeroMemory(&msg, sizeof(MSG));

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return(int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)									// Check For Windows Messages
	{
	case WM_TIMER:
		if (wParam == TICK)
		{

			//Mover el fantasma 1-----------------------
			if (cambio == true){
				fanx1 = fanx1 + 3;

				if (fanx1 >= 500){
					cambio = false;
				}
			}
			else{
				fanx1 = fanx1 - 3;
				if (fanx1 <= 30){
					cambio = true;
				}
			}

			//Mover al fantasma 2 -------------------------
			if (cambiox2 == true){
				fanx2 = fanx2 + 10;

				if (fanx2 >= 700){
					cambiox2 = false;
				}
			}
			else{
				fanx2 = fanx2 - 10;
				if (fanx2 <= 30){
					cambiox2 = true;
				}
			}

			//Mover al fantasma 3 -------------------------
			if (cambiox3 == true){
				fanx3 = fanx3 + 20;

				if (fanx3 >= 650){
					cambiox3 = false;
				}
			}
			else{
				fanx3 = fanx3 - 20;
				if (fanx3 <= 30){
					cambiox3 = true;
				}
			}

			//Plano del fantasma de Y1 -------------------------
			if (cambioy1 == true){
				fany1 = fany1 - 5;

				if (fany1 <= 50){
					cambioy1 = false;
				}
			}
			else{
				fany1 = fany1 + 5;
				if (fany1 >= 200){
					cambioy1 = true;
				}
			}

			MainRender(hWnd);
		}
		break;
	case WM_PAINT:
	{
		HDC hdc;
		PAINTSTRUCT ps;
		hdc = BeginPaint(hWnd, &ps);

		BITMAP bm;
		HBITMAP h_CMC = CreateBitmap(ANCHO_VENTANA, ALTO_VENTANA, 1, 32, ptrBuffer);
		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, h_CMC);
		GetObject(h_CMC, sizeof(bm), &bm);

		BitBlt(hdc, 0, 0, ANCHO_VENTANA, ALTO_VENTANA, hdcMem, 0, 0, SRCCOPY);

		DeleteObject(h_CMC);
		SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem);
		DeleteObject(hbmOld);
	}
	break;
	case WM_KEYDOWN:
	{
		KEYS[wParam] = true;
	}
	break;
	case WM_KEYUP:
	{
		KEYS[wParam] = false;
	}
	break;
	case WM_CLOSE:
	{
		DestroyWindow(hWnd);
	}
	break;
	case WM_DESTROY: //Send A Quit Message
	{
		KillTimer(hWnd, TICK);
		PostQuitMessage(0);
	}
	break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

/* Inicializacion de variables y reserva de espacio en memoria. COLORCAR VARIABLES DE MOV FANTASMAS
*/
void Init()
{
	posFigura.X = 20;
	posFigura.Y = 400;
	direccionGiro = DireccionGiro::ARRIBA; // INICIALIZANDO EL VALOR CON 0

	for (int i = 0; i < 256; i++)
	{
		KEYS[i] = false;
	}
	mirror = false;
	ptrBuffer = new int[ANCHO_VENTANA * ALTO_VENTANA];
	ptrBack = CargaImagen(TEXT("fondoo.jpg"), &dmnBack); //puntero a la imagen
	ptrSprite = CargaImagen(TEXT("green.png"), &dmnSprite);
	ptrfantasma = CargaImagen(TEXT("fantasmae.png"), &dmnfantasma);
}

/* Funcion principal. Encargada de hacer el redibujado en pantalla cada intervalo (o "tick") del timer que se haya creado.
@param hWnd. Manejador de la ventana.
*/

void MainRender(HWND hWnd)
{
	KeysEvents();
	//Solo a 250
		DibujaFondo(ptrBuffer, (int*)ptrBack, dmnBack, mirror);
		DibujaSprite(ptrBuffer, (int*)ptrSprite, dmnSprite, posFigura);
		rotaFigura(ptrBuffer, (int*)ptrfantasma, dmnfantasma, setPosition(fanx1, fany1), direccionGiro);
		rotaFigura1(ptrBuffer, (int*)ptrfantasma, dmnfantasma, setPosition(fanx2, 280), direccionGiro);
		rotaFigura2(ptrBuffer, (int*)ptrfantasma, dmnfantasma, setPosition(fanx3, fany2), direccionGiro);

	InvalidateRect(hWnd, NULL, FALSE);
	UpdateWindow(hWnd);
}

POSITION setPosition(int x, int y) {
	POSITION p;
	p.X = x;
	p.Y = y;
	return p;
}

void KeysEvents()
{
	if (KEYS[input.W] || KEYS[input.Up])
	{
		posFigura.Y -= 5;
		if (posFigura.Y < 350){
			posFigura.Y = 350;
		}

	}
	if (KEYS[input.D] || KEYS[input.Right])
	{
		/*mirror = false;
		posFigura.X += 10;
		movimiento += 1;
		if (movimiento == 3){
		movimiento = 0;
		}*/
		movimiento += 1;
		if (movimiento == 3){
			movimiento = 0;
		}
		mirror = FALSE;

		if (posFigura.X != 30){
			posFigura.X += 10;
		}


		//Límite movimiento
		if (movFondo <= 1500){
			//Movimento fondo
			if (posFigura.X == 30){
				movFondo += 10;
			}

			__asm{
				mov eax, indMov
					cmp eax, 4
					jna FondoMov
					xor eax, eax
					mov indMov, eax
				FondoMov :
				cld
					mov eax, indMov
					add eax, 1
					mov indMov, eax
			}
		}
		else{
			__asm{
				mov eax, indice
					cmp eax, 15
					jna FondoMov2
					xor eax, eax
					mov indice, eax

				FondoMov2 :

				cld
					mov eax, indice
					add eax, 1
					mov indice, eax
			}
		}
	}



	if (KEYS[input.S] || KEYS[input.Down])
	{
		posFigura.Y += 5;
		if (posFigura.Y > 430){
			posFigura.Y = 430;
		}
	}
	if (KEYS[input.A] || KEYS[input.Left])
	{
		mirror = true;
		movimiento += 1;
		if (movimiento == 3){
			movimiento = 0;
		}

		//Codificación del movimiento de la escena
		//Limitar movimiento del fondo
		if (movFondo != 0){
			//Movimento fondo
			if (posFigura.X == 30){
				movFondo -= 10;
			}
			if (posFigura.X == 30 && movFondo == 0){
				posFigura.X -= 10;
			}
			
			__asm{
				
				mov eax, indMov
					cmp eax, 4
					jna FondoMova
					xor eax, eax
					mov indMov, eax
				FondoMova :
				cld
					mov eax, indMov
					sub eax, 1
					mov indMov, eax
			}
		}
		else{
			__asm{
				mov eax, indice
					cmp eax, 15
					jna FondoMov2a
					xor eax, eax
					mov indice, eax

				FondoMov2a :

				cld
					mov eax, indice
					sub eax, 1
					mov indice, eax
			}
		}
	}



	if (KEYS[input.Space])
	{
		escala = !escala;
	}
	if (KEYS[input.V]){
		contador += 1;
		if (contador == 1){
			colora = TRUE;
		}
		if (contador == 2){
			colorv = TRUE;
		}

		if (contador >= 3){
			colora = FALSE;
			colorv = FALSE;
			contador = 0;
		}
	}

	if (KEYS[input.B]){
		contadora += 1;
		if (contadora == 1){
			coloraa = TRUE;
		}

		if (contadora >= 2){
			coloraa = FALSE;
			contadora = 0;
		}
	}

	if (KEYS[input.N]){
		contadorav += 1;
		if (contadorav == 1){
			coloraav = TRUE;
		}

		if (contadorav >= 2){
			coloraav = FALSE;
			contadorav = 0;
		}
	}

	if (KEYS[input.M]){
		contadoravr += 1;
		if (contadoravr == 1){
			coloraavr = TRUE;
		}

		if (contadoravr >= 2){
			coloraavr = FALSE;
			contadoravr = 0;
		}
	}

	if (KEYS[input.C]) {
		direccionGiro = direccionGiro > 3 ? 0 : direccionGiro + 1;
	}

}



/* Funcion para cargar imagenes y obtener un puntero al area de memoria reservada para la misma.
@param rutaImagen.			Nombre o ruta de la imagen a cargar en memoria.
@return unsigned char *.	Direccion base de la imagen.
*/
unsigned char * CargaImagen(WCHAR rutaImagen[], DIMENSION * dmn)
{
	unsigned char * ptrImagen;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR  gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	Bitmap *bitmap = new Bitmap(rutaImagen);
	BitmapData *bitmapData = new BitmapData;

	dmn->ANCHO = bitmap->GetWidth();
	dmn->ALTO = bitmap->GetHeight();

	Rect rect(0, 0, dmn->ANCHO, dmn->ALTO);

	//Reservamos espacio en memoria para la imagen
	bitmap->LockBits(&rect, ImageLockModeRead, PixelFormat32bppRGB, bitmapData);

	//"pixels" es el puntero al area de memoria que ocupa la imagen
	unsigned char* pixels = (unsigned char*)bitmapData->Scan0;

	//"tamaño" lo usaremos para reservar los bytes que necesita la imagen. 
	//Para calcular la cantidad de bytes total necesitamos multiplicamos el area de la imagen * 4. 
	//Se multiplica por 4 debido a que cada pixel ocupa 4 bytes de memoria. Noten el 3er parametro de la funcion LockBits, dos lineas de codigo arriba.
	//PixelFormat32bppARGB -> Specifies that the format is 32 bits per pixel; 8 bits each are used for the alpha, red, green, and blue components.
	//Mas info: https://msdn.microsoft.com/en-us/library/system.drawing.imaging.pixelformat(v=vs.110).aspx
	int tamaño;
	tamaño = dmn->ANCHO * dmn->ALTO * 4;
	//hagamos un try de la reserva de memoria
	try
	{
		ptrImagen = new unsigned char[tamaño];
	}
	catch (...)
	{
		return NULL;
	}

	//Después de este for, ptrImagen contiene la direccion en memoria de la imagen.
	for (int i = 0, j = tamaño; i < j; i++)
	{
		ptrImagen[i] = pixels[i];
	}

	//Es necesario liberar el espacio en memoria, de lo contrario marcaria una excepcion de no hay espacio de memoria suficiente.
	bitmap->UnlockBits(bitmapData);
	delete bitmapData;
	delete bitmap;

	GdiplusShutdown(gdiplusToken);

	return ptrImagen;
}

#pragma region LENS_CODE
void DibujaFondo(int * buffer, int * imagen, DIMENSION dmn, bool mirror) {
	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	int total = w * h;
	int tmp = 0;
	__asm {
		/*mov eax, imagew
		mul BPP
		add esi, eax
		mov eax, spriteW
		sub esi, eax
		mov eax, ANCHO_VENTANA
		mul BPP
		add esi, eax
		mov eax, spriteW


		mov esi, imagen
		mov edi, buffer

		mov eax, w
		mov ebx, h
		mul ebx
		mov ecx, eax
		rep movsd */

		cld
			//Pinta pixel x pixel
			mov esi, imagen
			mov eax, movFondo
			mul BPP
			add esi, eax
			mov edi, buffer
			mov ecx, ALTO_VENTANA

		PintarA :
		push ecx
			mov ecx, ANCHO_VENTANA

		PintarL :
		mov eax, [esi]
			mov[edi], eax
			add edi, BPP
			add esi, BPP
			loop PintarL
			mov eax, 2400 //Tamaño de la imagen
			mul BPP
			add esi, eax
			mov eax, ANCHO_VENTANA
			mul BPP
			sub esi, eax
			pop ecx
			loop PintarA

	}
}

//Funcion para desplegar los sprites en pantalla
// @param *buffer. Puntero al buffer, area de memoria destinada para el despliegue de las imagenes previamente cargadas
// @param *sprite. Puntero a la imagen almacenada en memoria.
// @param dmn. Objeto de tipo DIMENSION que contiene el ancho y alto del sprite a desplegar.
// @param pos. Posicionar el sprite dentro de la pantalla.
void DibujaSprite(int *buffer, int *sprite, DIMENSION dmn, POSITION pos) {
	int x = pos.X; int y = pos.Y;
	int width = dmn.ANCHO;
	int ancho = width / 3;
	int height = dmn.ALTO;
	int tmp = 2;
	int z = 0;
	int anchoc = ancho*tmp;

	//OR eax, 0x000000F0
	if (!escala) {
		if (!mirror) {
			__asm {
				//CAMINA HACIA DELANTE SIN AGRANDAR
				mov esi, sprite
					mov edi, buffer

					mov eax, x
					mul BPP
					add edi, eax
					mov eax, y
					mul BPP
					mul ANCHO_VENTANA
					add edi, eax


					mov eax, movimiento
					mul ancho
					mul BPP
					add esi, eax
					cmp esi, 155
					jne salto
					mov eax, width
					sub eax, ancho
					mul BPP
					sub esi, eax

				salto :


				mov ecx, height
				dib_alto :
				push ecx

					mov ecx, ancho
				dib_ancho :
				mov eax, [esi] //Eliminación de fondo y colores

					cmp colora, 1
					jnz u
					OR eax, 0x000000F0

					cmp colorv, 1
					jnz uu
					OR eax, GREEN
				u :
			uu :


				cmp eax, 0xFFFFFFFF
				jz sss
				cmp eax, 0x00000000
				jz ss2
				mov[edi], eax
			ss2 :
		   sss :
			   add esi, BPP
			   add edi, BPP
			   loop dib_ancho
			   mov eax, ANCHO_VENTANA
			   sub eax, ancho
			   mul BPP
			   add edi, eax
			   mov eax, width
			   sub eax, ancho
			   mul BPP
			   add esi, eax

			   pop ecx
			   loop dib_alto

			}

		}
		else {
			__asm {
				//CAMINA HACIA ATRAS SIN AGRANDAR
				mov esi, sprite
					mov edi, buffer

					mov eax, x
					mul BPP
					add edi, eax
					mov eax, y
					mul BPP
					mul ANCHO_VENTANA
					add edi, eax

					mov eax, movimiento
					mul ancho
					mul BPP
					add esi, eax
					cmp esi, 155
					jne salto2
					mov eax, width
					sub eax, ancho
					mul BPP
					sub esi, eax
				salto2 :


				mov ecx, height
				dib_alto2 :
				push ecx
					mov eax, ancho
					mul BPP
					add esi, eax
					sub esi, BPP
					mov ecx, ancho
				dib_ancho2 :
				mov eax, [esi] //Eliminación de fondo y colores

					cmp colora, 1
					jnz u2
					OR eax, 0x000000F0
					cmp colorv, 1
					jnz uu2
					OR eax, GREEN
				uu2 :
			u2 :
				cmp eax, 0xFFFFFFFF
				jz s
				cmp eax, 0x00000000
				jz s2
				mov[edi], eax
			s2 :
		   s :
			   sub esi, BPP
			   add edi, BPP
			   loop dib_ancho2
			   mov eax, ANCHO_VENTANA
			   sub eax, ancho
			   mul BPP
			   add edi, eax
			   mov eax, width
			   mul BPP
			   add esi, eax
			   add esi, BPP

			   pop ecx
			   loop dib_alto2

			}
		}
	}
	else {
		if (!mirror) {
			__asm {
				//HACIA DELANTE AGRANDADO
				mov esi, sprite
					mov edi, buffer

					mov eax, x
					mul BPP
					add edi, eax
					mov eax, y
					mul BPP
					mul ANCHO_VENTANA
					add edi, eax

					mov eax, movimiento
					mul ancho
					mul BPP
					add esi, eax
					cmp esi, 155
					jne salto3
					mov eax, width
					sub eax, ancho
					mul BPP
					sub esi, eax

				salto3 :


				mov ecx, height
				dib_alto3 :
				push ecx
					mov ecx, tmp
				escalar :
				push ecx
					mov ecx, anchoc
				dib_ancho3 :
				inc z
					mov eax, [esi] //Eliminación de fondo y colores
					cmp eax, 0xFFFFFFFF
					jz sq
					cmp eax, 0x00000000
					jz sq2

					mov[edi], eax
				sq2 :
			sq :
				mov eax, tmp
				cmp z, eax
				jne salto_ancho
				add esi, BPP
				mov z, 0
			salto_ancho :
						add edi, BPP
						loop dib_ancho3
						mov eax, ANCHO_VENTANA
						sub eax, anchoc
						mul BPP
						add edi, eax
						mov eax, ancho
						mul BPP
						sub esi, eax
						mov z, 0
						pop ecx
						loop escalar

						mov eax, width
						mul BPP
						add esi, eax
						pop ecx
						loop dib_alto3
			}

		}
		else {
			__asm {

				//HACIA ATRAS AGRANDADO
				mov esi, sprite
					mov edi, buffer

					mov eax, x
					mul BPP
					add edi, eax
					mov eax, y
					mul BPP
					mul ANCHO_VENTANA
					add edi, eax

					mov eax, movimiento
					mul ancho
					mul BPP
					add esi, eax
					cmp esi, 155
					jne salto4
					mov eax, width
					sub eax, ancho
					mul BPP
					sub esi, eax

				salto4 :


				mov ecx, height
				dib_alto4 :
				push ecx
					mov ecx, tmp
				escalar2 :
				push ecx
					mov ecx, anchoc
				dib_ancho4 :
				inc z
					mov eax, [esi] //Eliminación de fondo y colores

					cmp eax, 0xFFFFFFFF
					jz sqq
					cmp eax, 0x00000000
					jz sqq2

					mov[edi], eax

				sqq2 :
			sqq :
				mov eax, tmp
				cmp z, eax
				jne salto_ancho2
				sub esi, BPP //cambia de add a sub
				mov z, 0
			salto_ancho2 :
						 add edi, BPP 
						 loop dib_ancho4
						 mov eax, ANCHO_VENTANA
						 sub eax, anchoc
						 mul BPP
						 add edi, eax
						 mov eax, ancho
						 mul BPP
						 add esi, eax //cambia de sub a add
						 mov z, 0
						 pop ecx
						 loop escalar2

						 mov eax, width
						 mul BPP
						 add esi, eax

						 pop ecx
						 loop dib_alto4

			}
		}
	}
}

//ROTAR LA FIGURA
void rotaFigura(int *ptrBuffer, int *ptrBmpSprite, DIMENSION dmn, POSITION posSprite, int direccion) {
	int width = dmn.ANCHO;
	int height = dmn.ALTO;
	int posX = posSprite.X;
	int posY = posSprite.Y;
	int index = 0;
	int lastPx = 0;

	__asm {
		// registro EDI se convierte en puntero al Buffer (area de memoria reservada para el despliegue en pantalla
		mov edi, ptrBuffer

			// registro ESI se convierte en puntero al sprite a girar/rotar
			mov esi, ptrBmpSprite
	}
	if (direccion == DireccionGiro::ARRIBA) {
		__asm {
			mov eax, posX
				mul BPP
				add edi, eax
				mov eax, ANCHO_VENTANA
				mul BPP
				mul posY
				add edi, eax

				mov ecx, height
			LU2 :
			push ecx
				mov ecx, width
			LU1 :
			mov eax, [esi]

				cmp coloraa, 1
				jnz aa
				OR eax, 0x000000F0
			aa :

			   cmp eax, 0xFFFFFFFF
			   jz sssp
			   cmp eax, 0x00000000
			   jz ss2p


			   mov[edi], eax
		   sssp :
		ss2p :
			add esi, 4
			add edi, 4
			loop LU1

			mov eax, ANCHO_VENTANA
			mul BPP
			add edi, eax
			mov eax, width
			mul BPP
			sub edi, eax

			pop ecx
			loop LU2
		}
	}
	else if (direccion == DireccionGiro::DERECHA) {
		__asm {
			mov eax, width
				mul BPP
				mul height
				add esi, eax
				mov lastPx, esi

				mov eax, posX
				mul BPP
				add edi, eax
				mov eax, ANCHO_VENTANA
				mul BPP
				mul posY
				add edi, eax

				mov ecx, width
			LR2 :
			push ecx
				mov ecx, height
			LR1 :
			mov eax, width
				mul BPP
				sub esi, eax
				mov eax, [esi]

				cmp coloraa, 1
				jnz aao
				OR eax, 0x000000F0
			aao:


			cmp eax, 0xFFFFFFFF
				jz pp
				cmp eax, 0x00000000
				jz pp2

				mov[edi], eax

			pp2 :
		pp :

			add edi, 4
			loop LR1
			add lastPx, 4
			mov esi, lastPx

			mov eax, ANCHO_VENTANA
			mul BPP
			add edi, eax
			mov eax, width
			mul BPP
			sub edi, eax

			pop ecx
			loop LR2
		}
	}
	else if (direccion == DireccionGiro::ABAJO) {
		__asm {
			mov eax, width
				mul BPP
				mul height
				sub eax, 4
				add esi, eax

				mov eax, posX
				mul BPP
				add edi, eax
				mov eax, ANCHO_VENTANA
				mul BPP
				mul posY
				add edi, eax

				mov ecx, height
			LD2 :
			push ecx
				mov ecx, width
			LD1 :
			mov eax, [esi]

				cmp coloraa, 1
				jnz aaoo
				OR eax, 0x000000F0
			aaoo :

				 cmp eax, 0xFFFFFFFF
				 jz ppp
				 cmp eax, 0x00000000
				 jz ppp2

				 mov[edi], eax

			 ppp :
		ppp2 :
			sub esi, 4
			add edi, 4
			loop LD1
			mov eax, ANCHO_VENTANA
			mul BPP
			add edi, eax
			mov eax, width
			mul BPP
			sub edi, eax

			pop ecx
			loop LD2
		}
	}
	else { // ya no hay de otra mas que la variable 'direccion' sea igual a DireccionGiro::IZQUIERDA (= 3)
		__asm {
			sub esi, 4
				mov lastPx, esi

				mov eax, posX
				mul BPP
				add edi, eax
				mov eax, ANCHO_VENTANA
				mul BPP
				mul posY
				add edi, eax

				mov ecx, width
			LL2 :
			push ecx
				mov ecx, height
			LL1 :
			mov eax, width
				mul BPP
				add esi, eax
				mov eax, [esi]

				cmp coloraa, 1
				jnz aaooo
				OR eax, 0x000000F0
			aaooo :

			cmp eax, 0xFFFFFFFF
				jz pppp
				cmp eax, 0x00000000
				jz pppp2

				mov[edi], eax
			pppp :
		pppp2 :
			add edi, 4
			loop LL1
			sub lastPx, 4
			mov esi, lastPx

			mov eax, ANCHO_VENTANA
			mul BPP
			add edi, eax
			mov eax, height
			mul BPP
			sub edi, eax

			pop ecx
			loop LL2
		}
	}
}

void rotaFigura1(int *ptrBuffer, int *ptrBmpSprite, DIMENSION dmn, POSITION posSprite, int direccion) {
	int width = dmn.ANCHO;
	int height = dmn.ALTO;
	int posX = posSprite.X;
	int posY = posSprite.Y;
	int index = 0;
	int lastPx = 0;
	__asm {
		// registro EDI se convierte en puntero al Buffer (area de memoria reservada para el despliegue en pantalla
		mov edi, ptrBuffer

			// registro ESI se convierte en puntero al sprite a girar/rotar
			mov esi, ptrBmpSprite
	}
	if (direccion == DireccionGiro::ARRIBA) {
		__asm {
			mov eax, posX
				mul BPP
				add edi, eax
				mov eax, ANCHO_VENTANA
				mul BPP
				mul posY
				add edi, eax

				mov ecx, height
			LU2 :
			push ecx
				mov ecx, width
			LU1 :
			mov eax, [esi]

				cmp coloraav, 1
				jnz lul
				OR eax, GREEN
			lul :

			cmp eax, 0xFFFFFFFF
				jz sssp
				cmp eax, 0x00000000
				jz ss2p


				mov[edi], eax
			sssp :
		ss2p :
			add esi, 4
			add edi, 4
			loop LU1

			mov eax, ANCHO_VENTANA
			mul BPP
			add edi, eax
			mov eax, width
			mul BPP
			sub edi, eax

			pop ecx
			loop LU2
		}
	}
	else if (direccion == DireccionGiro::DERECHA) {
		__asm {
			mov eax, width
				mul BPP
				mul height
				add esi, eax
				mov lastPx, esi

				mov eax, posX
				mul BPP
				add edi, eax
				mov eax, ANCHO_VENTANA
				mul BPP
				mul posY
				add edi, eax

				mov ecx, width
			LR2 :
			push ecx
				mov ecx, height
			LR1 :
			mov eax, width
				mul BPP
				sub esi, eax
				mov eax, [esi]

				cmp coloraav, 1
				jnz luul
				OR eax, GREEN
			luul :

			cmp eax, 0xFFFFFFFF
				jz pp
				cmp eax, 0x00000000
				jz pp2

				mov[edi], eax

			pp2 :
		pp :

			add edi, 4
			loop LR1
			add lastPx, 4
			mov esi, lastPx

			mov eax, ANCHO_VENTANA
			mul BPP
			add edi, eax
			mov eax, width
			mul BPP
			sub edi, eax

			pop ecx
			loop LR2
		}
	}
	else if (direccion == DireccionGiro::ABAJO) {
		__asm {
			mov eax, width
				mul BPP
				mul height
				sub eax, 4
				add esi, eax

				mov eax, posX
				mul BPP
				add edi, eax
				mov eax, ANCHO_VENTANA
				mul BPP
				mul posY
				add edi, eax

				mov ecx, height
			LD2 :
			push ecx
				mov ecx, width
			LD1 :
			mov eax, [esi]

				cmp coloraav, 1
				jnz luuul
				OR eax, GREEN
			luuul :

			cmp eax, 0xFFFFFFFF
				jz ppp
				cmp eax, 0x00000000
				jz ppp2

				mov[edi], eax

			ppp :
		ppp2 :
			sub esi, 4
			add edi, 4
			loop LD1
			mov eax, ANCHO_VENTANA
			mul BPP
			add edi, eax
			mov eax, width
			mul BPP
			sub edi, eax

			pop ecx
			loop LD2
		}
	}
	else { // ya no hay de otra mas que la variable 'direccion' sea igual a DireccionGiro::IZQUIERDA (= 3)
		__asm {
			sub esi, 4
				mov lastPx, esi

				mov eax, posX
				mul BPP
				add edi, eax
				mov eax, ANCHO_VENTANA
				mul BPP
				mul posY
				add edi, eax

				mov ecx, width
			LL2 :
			push ecx
				mov ecx, height
			LL1 :
			mov eax, width
				mul BPP
				add esi, eax
				mov eax, [esi]

				cmp coloraav, 1
				jnz luuuul
				OR eax, GREEN
			luuuul :
			cmp eax, 0xFFFFFFFF
				jz pppp
				cmp eax, 0x00000000
				jz pppp2

				mov[edi], eax
			pppp :
		pppp2 :
			add edi, 4
			loop LL1
			sub lastPx, 4
			mov esi, lastPx

			mov eax, ANCHO_VENTANA
			mul BPP
			add edi, eax
			mov eax, height
			mul BPP
			sub edi, eax

			pop ecx
			loop LL2
		}
	}
}

void rotaFigura2(int *ptrBuffer, int *ptrBmpSprite, DIMENSION dmn, POSITION posSprite, int direccion) {
	int width = dmn.ANCHO;
	int height = dmn.ALTO;
	int posX = posSprite.X;
	int posY = posSprite.Y;
	int index = 0;
	int lastPx = 0;

	__asm {
		// registro EDI se convierte en puntero al Buffer (area de memoria reservada para el despliegue en pantalla
		mov edi, ptrBuffer

			// registro ESI se convierte en puntero al sprite a girar/rotar
			mov esi, ptrBmpSprite
	}
	if (direccion == DireccionGiro::ARRIBA) {
		__asm {
			mov eax, posX
				mul BPP
				add edi, eax
				mov eax, ANCHO_VENTANA
				mul BPP
				mul posY
				add edi, eax

				mov ecx, height
			LU2 :
			push ecx
				mov ecx, width
			LU1 :
			mov eax, [esi]

				cmp coloraavr, 1
				jnz lul
				OR eax, RED
			lul :

			cmp eax, 0xFFFFFFFF
				jz sssp
				cmp eax, 0x00000000
				jz ss2p


				mov[edi], eax
			sssp :
		ss2p :
			add esi, 4
			add edi, 4
			loop LU1

			mov eax, ANCHO_VENTANA
			mul BPP
			add edi, eax
			mov eax, width
			mul BPP
			sub edi, eax

			pop ecx
			loop LU2
		}
	}
	else if (direccion == DireccionGiro::DERECHA) {
		__asm {
			mov eax, width
				mul BPP
				mul height
				add esi, eax
				mov lastPx, esi

				mov eax, posX
				mul BPP
				add edi, eax
				mov eax, ANCHO_VENTANA
				mul BPP
				mul posY
				add edi, eax

				mov ecx, width
			LR2 :
			push ecx
				mov ecx, height
			LR1 :
			mov eax, width
				mul BPP
				sub esi, eax
				mov eax, [esi]

				cmp coloraavr, 1
				jnz luul
				OR eax, RED
			luul :

			cmp eax, 0xFFFFFFFF
				jz pp
				cmp eax, 0x00000000
				jz pp2

				mov[edi], eax

			pp2 :
		pp :

			add edi, 4
			loop LR1
			add lastPx, 4
			mov esi, lastPx

			mov eax, ANCHO_VENTANA
			mul BPP
			add edi, eax
			mov eax, width
			mul BPP
			sub edi, eax

			pop ecx
			loop LR2
		}
	}
	else if (direccion == DireccionGiro::ABAJO) {
		__asm {
			mov eax, width
				mul BPP
				mul height
				sub eax, 4
				add esi, eax

				mov eax, posX
				mul BPP
				add edi, eax
				mov eax, ANCHO_VENTANA
				mul BPP
				mul posY
				add edi, eax

				mov ecx, height
			LD2 :
			push ecx
				mov ecx, width
			LD1 :
			mov eax, [esi]

				cmp coloraavr, 1
				jnz luuul
				OR eax, RED
			luuul :

			cmp eax, 0xFFFFFFFF
				jz ppp
				cmp eax, 0x00000000
				jz ppp2

				mov[edi], eax

			ppp :
		ppp2 :
			sub esi, 4
			add edi, 4
			loop LD1
			mov eax, ANCHO_VENTANA
			mul BPP
			add edi, eax
			mov eax, width
			mul BPP
			sub edi, eax

			pop ecx
			loop LD2
		}
	}
	else { // ya no hay de otra mas que la variable 'direccion' sea igual a DireccionGiro::IZQUIERDA (= 3)
		__asm {
			sub esi, 4
				mov lastPx, esi

				mov eax, posX
				mul BPP
				add edi, eax
				mov eax, ANCHO_VENTANA
				mul BPP
				mul posY
				add edi, eax

				mov ecx, width
			LL2 :
			push ecx
				mov ecx, height
			LL1 :
			mov eax, width
				mul BPP
				add esi, eax
				mov eax, [esi]

				cmp coloraavr, 1
				jnz luuuul
				OR eax, RED
			luuuul :
			cmp eax, 0xFFFFFFFF
				jz pppp
				cmp eax, 0x00000000
				jz pppp2

				mov[edi], eax
			pppp :
		pppp2 :
			add edi, 4
			loop LL1
			sub lastPx, 4
			mov esi, lastPx

			mov eax, ANCHO_VENTANA
			mul BPP
			add edi, eax
			mov eax, height
			mul BPP
			sub edi, eax

			pop ecx
			loop LL2
		}
	}
}

#pragma endregion
