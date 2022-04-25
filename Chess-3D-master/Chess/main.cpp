#include "engine\\TCP.h"
#include "engine\\TCPServer.h"
#include "engine\\TCPClient.h"

#include "engine\\engine.h"														
#include "resource.h"
#include "game.cpp"																
#include "hint.cpp"
#include "about.cpp"															
#include "ListView.cpp"															
#include "CLWindow.cpp"															
#include "title_layer.h"														

#define ENABLE_MENU_ITEM(HANDLE, ID) EnableMenuItem(GetMenu(HANDLE), ID, MF_ENABLED)
#define DISABLE_MENU_ITEM(HANDLE, ID) EnableMenuItem(GetMenu(HANDLE), ID, MF_GRAYED)

#define ID_LOCAL_GAME		WM_APP + 1
#define ID_NET_SERVER		WM_APP + 2
#define ID_NET_CLIENT		WM_APP + 3
#define ID_WHITE_THEME		WM_APP + 4
#define ID_COLORED_THEME	WM_APP + 5

#define WINDOW_DEFAULT_WIDHT	860												
#define WINDOW_DEFAULT_HEIGHT	645												

#define ID_LAYER_GAME	1														
#define ID_LAYER_TITLE	2

Engine *Render = NULL;
Hint *hint = NULL;
About *about;

std::vector<CLButton> buttons;
std::vector<CLButton> themeButtons;
CLWindow *selectGame;

Layer *game = NULL;
Layer *title = NULL;

TCPClient *client = NULL;
TCPServer *server = NULL;


void EnableWindowSizing(HWND hWnd, bool sizing = true){
	
	int shift = 4;

	if(sizing){
		SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) | (WS_MAXIMIZEBOX | WS_SIZEBOX));
			
		RECT rect, windowRect;
		GetWindowRect(hWnd, &windowRect);
		GetClientRect(hWnd, &rect);

		AdjustWindowRectEx(&rect, GetWindowLong(hWnd, GWL_STYLE), true, GetWindowLong(hWnd, GWL_EXSTYLE));
		
		SetWindowPos(hWnd, 
						HWND_NOTOPMOST, 
						windowRect.left - shift, 
						windowRect.top - shift, 
						rect.right - rect.left, 
						rect.bottom - rect.top, 
						SWP_FRAMECHANGED);

	}
	else{
		SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~(WS_MAXIMIZEBOX | WS_SIZEBOX));
				
		RECT rect, windowRect;
		GetWindowRect(hWnd, &windowRect);
		GetClientRect(hWnd, &rect);

		AdjustWindowRectEx(&rect, GetWindowLong(hWnd, GWL_STYLE), true, GetWindowLong(hWnd, GWL_EXSTYLE));
		SetWindowPos(hWnd, 
						HWND_NOTOPMOST, 
						windowRect.left + shift, 
						windowRect.top + shift, 
						rect.right - rect.left, 
						rect.bottom - rect.top, 
						SWP_FRAMECHANGED);
	}
};


LRESULT  WINAPI  MsgProc(HWND hWnd,			
						 UINT msg,			
						 WPARAM wParam,		
						 LPARAM lParam)		
{
	switch (msg)																
	{
		case WM_SYSCOMMAND:														
			{
				switch (wParam)
				{
					case SC_SCREENSAVE:											
					case SC_MONITORPOWER:										
					return 0;
				}
				break;
			}

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
			
			case ID_GAME_NEW:													
				{
				UINT gameType;

				Render->BeforeFreezing();
				selectGame = new CLWindow(hWnd, NULL);
				gameType = selectGame->Show(TEXT("MessageBox"), buttons, true, 300);
				delete selectGame;
				Render->AfterFreezing();

				switch (gameType) {												
					case ID_LOCAL_GAME:
					case ID_NET_SERVER:
					case ID_NET_CLIENT:
						{
							
							if(game != NULL){	
								UINT endGame = Render->GetParameter(ID_LAYER_GAME, GAME_END);
								if(endGame != GAME_YES){						
									UINT endGame = MessageBox(hWnd, 
														TEXT("Do you want to quit?"),
														TEXT("Alert"),
														MB_YESNO);

									if(endGame == IDYES){
										DISABLE_MENU_ITEM(hWnd, ID_CHANGE_THEME);
										Render->EnableLayer(ID_LAYER_TITLE);
										Render->ResetAnimation();
										Render->UnregisterLayerID(ID_LAYER_GAME);
										Render->SetParameter(ID_LAYER_TITLE, ID_LOADING_TEXT, TEXT_ENABLE);
										Render->Rendering();
						
										delete game;
										game = NULL;

										

										Render->EnableLayer(ID_LAYER_TITLE);
										if(server != NULL) {
											delete server;
											server = NULL;
										}

										if(client != NULL) {
											delete client;
											client = NULL;
										}
					
										
										SendMessage(hWnd, WM_COMMAND, gameType, NULL);
									}
								}
								else{
									DISABLE_MENU_ITEM(hWnd, ID_CHANGE_THEME);
									Render->EnableLayer(ID_LAYER_TITLE);
									Render->ResetAnimation();
									Render->UnregisterLayerID(ID_LAYER_GAME);
									Render->SetParameter(ID_LAYER_TITLE, ID_LOADING_TEXT, TEXT_ENABLE);
									Render->Rendering();
						
									delete game;
									game = NULL;

									Render->EnableLayer(ID_LAYER_TITLE);
									if(server != NULL) {
										delete server;
										server = NULL;
									}

									if(client != NULL) {
										delete client;
										client = NULL;
									}
					
									SendMessage(hWnd, WM_COMMAND, gameType, NULL);
							}
							}
							else{
								if(server == NULL && client == NULL) {			
									EnableWindowSizing(hWnd, true);				
								}
								else{
									if(server != NULL) {
										delete server;
										server = NULL;
									}

									if(client != NULL) {
										delete client;
										client = NULL;
									}
								}
								Render->ResetAnimation();
								SendMessage(hWnd, WM_COMMAND, gameType, NULL);
							}
							break;
					}
								
						default:
							break;
					}
				}
				break;

			case ID_CHANGE_THEME:												
				if(game != NULL){
					Render->BeforeFreezing();
					selectGame = new CLWindow(hWnd, NULL);
					UINT themeType = selectGame->Show(TEXT("Themes"), themeButtons, true, 300);
					delete selectGame;
					Render->AfterFreezing();

					switch (themeType) {										
						case ID_WHITE_THEME:									
							game->SetParameter(GAME_CHANGE_THEME, THEME_WHITE);
							break;
								
						case ID_COLORED_THEME:
							game->SetParameter(GAME_CHANGE_THEME, THEME_COLORED);
							break;
								
						default:
							break;
					}
				}
				break;

			case ID_GAME_EXIT:													
				SendMessage(hWnd, WM_CLOSE, NULL, NULL);	
				break;
					
			case ID_HELP_URL:													
				
				ShellExecute(NULL, TEXT("open"), TEXT("http://ru.wikipedia.org/wiki/%D0%A8%D0%B0%D1%85%D0%BC%D0%B0%D1%82%D1%8B"), NULL, NULL, SW_SHOWNORMAL);
				break;
					
			case ID_HELP_ABOUT:													
				Render->BeforeFreezing();
				about = new About(hWnd, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE));
				about->Show();													
				delete about;
				Render->AfterFreezing();
				break;

			case ID_LOCAL_GAME:													
				Render->SetParameter(ID_LAYER_TITLE, ID_LOADING_TEXT, TEXT_ENABLE);
				Render->Rendering();											

				game = new LocalChessGame(Render->Get3DDevice(), hWnd);			
				Render->SetParameter(ID_LAYER_TITLE, ID_LOADING_TEXT, TEXT_DISABLE);
				Render->DisableLayer(ID_LAYER_TITLE);
				Render->RegisterLayer(game, ID_LAYER_GAME);						
				Render->Rendering();

				ENABLE_MENU_ITEM(hWnd, ID_CHANGE_THEME);						
				break;
								
			case ID_NET_SERVER:													
				{
					
					Render->SetParameter(ID_LAYER_TITLE, ID_WAITING_TEXT, TEXT_ENABLE);
					Render->Rendering();

					server = new TCPServer(hWnd, TEXT("Server"));				

					int result = server->WaitForClient();						

					switch (result) {											
						case ERROR_NOTINITED:
							MessageBox(hWnd, TEXT("Неможливо ініціалізувати сокет"), TEXT("Alert"), MB_OK);
							DestroyWindow(hWnd);
							break;
								
						case ERROR_ALREADY_WORKING:
							MessageBox(hWnd, TEXT("Повторний запуск сервера"), TEXT("Alert"), MB_OK);
							DestroyWindow(hWnd);
							break;
								
						case ERROR_ALREADY_CONNECTED:
							MessageBox(hWnd, TEXT("Сервер вже встановив з'єднання"), TEXT("Alert"), MB_OK);
							DestroyWindow(hWnd);
							break;
							
						case ERROR_CREATE_THREAD_FAIL:
							MessageBox(hWnd, TEXT("Неможливо ініціалізувати сервер"), TEXT("Alert"), MB_OK);
							DestroyWindow(hWnd);
							break;
						case 0:
							break;
					}
				}
				break;
								
			case ID_NET_CLIENT:													// Створення мережевої гри, клієнта
				{
					Render->SetParameter(ID_LAYER_TITLE, ID_SEARCH_TEXT, TEXT_ENABLE);
					Render->Rendering();
						
					client = new TCPClient(hWnd, TEXT("Client"));				// Створюємо об'єкт клієнт
					
					// Виводимо модально вікно, яке отримує і відображає доступні сервера
					int result = DialogBoxParam((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDD_SERVER_SELECT), hWnd, DlgProc, (LPARAM)client);

					ShowWindow(hWnd, SW_SHOWNORMAL);							// Показуємо вікно
					SetFocus(hWnd);												// Встановлюємо фокус
					UpdateWindow(hWnd);											// Оновлюємо вікно

					switch (result) {											// Аналізуємо результат повернений вікном(клієнтським зєднанням)
						case ERROR_NOTINITED:
							MessageBox(hWnd, TEXT("Notinied"), TEXT("Alert"), MB_OK);
							DestroyWindow(hWnd);
							break;
								
						case ERROR_ALREADY_WORKING:
							MessageBox(hWnd, TEXT("Already Working"), TEXT("Alert"), MB_OK);
							DestroyWindow(hWnd);
							break;
								
						case ERROR_ALREADY_CONNECTED:
							MessageBox(hWnd, TEXT("Already Connected"), TEXT("Alert"), MB_OK);
							DestroyWindow(hWnd);
							break;
								
						case ERROR_CREATE_THREAD_FAIL:
							MessageBox(hWnd, TEXT("Connection Failed..."), TEXT("Alert"), MB_OK);
							DestroyWindow(hWnd);
							break;
								
						case ERROR_WRONG_SERVER_NUMBER:
							MessageBox(hWnd, TEXT("Wrong server no..."), TEXT("Alert"), MB_OK);
							DestroyWindow(hWnd);
							break;
								
						case 0:
							break;
								
						case -1:
							DISABLE_MENU_ITEM(hWnd, ID_CHANGE_THEME);
							break;
					}
					Render->SetParameter(ID_LAYER_TITLE, ID_SEARCH_TEXT, TEXT_DISABLE);
				}
				break;
					
			default:															// Обробка повідомлень за замовчуванням			
				return DefWindowProc(hWnd, msg, wParam, lParam);
			}

		// Повідомлення клавіатури
		case WM_KEYDOWN:														// Якщо натиснута кнопка
			if (LOWORD(wParam) == VK_F1) {
				ShellExecute(NULL, TEXT("open"), TEXT("http://ru.wikipedia.org/wiki/%D0%A8%D0%B0%D1%85%D0%BC%D0%B0%D1%82%D1%8B"), NULL, NULL, SW_SHOWNORMAL);
			}
			Render->KeyDown(wParam);											// Посилаємо код клавіші на обробку
			return 0;

		case WM_KEYUP:															// Якщо відпущена клавіша
			Render->KeyUp(wParam);
			return 0;
		
		// Повідомлення мишки
		case WM_LBUTTONDOWN:
			Render->LBtnDown(LOWORD(lParam), HIWORD(lParam));
			return 0;

		case WM_LBUTTONUP:
			Render->LBtnUp(LOWORD(lParam), HIWORD(lParam));
			return 0;

		case WM_LBUTTONDBLCLK:
			Render->LBtnDblClk(LOWORD(lParam), HIWORD(lParam));
			return 0;

		case WM_RBUTTONDOWN:
			Render->RBtnDown(LOWORD(lParam), HIWORD(lParam));
			return 0;

		case WM_RBUTTONUP:
			Render->RBtnUp(LOWORD(lParam), HIWORD(lParam));
			return 0;

		case WM_RBUTTONDBLCLK:
			Render->RBtnDblClk(LOWORD(lParam), HIWORD(lParam));
			return 0;

		case WM_MOUSEMOVE:
			Render->MouseMove(LOWORD(lParam), HIWORD(lParam));
			return 0;

		case WM_MOUSEWHEEL:
			// wParam містить зміщення коліщатка мишки
			Render->Wheel(GET_WHEEL_DELTA_WPARAM(wParam));
			return 0;
		
		case WM_ACTIVATE:
			// wParam є WA_ACTIVE або WA_INACTIVE повідомленням вікна
			Render->WindowActive(LOWORD(wParam));
			return 0;

		case WM_SIZE:															// Обробка повідомлення зміни вікна
			Render->ResizeScene(LOWORD(lParam), HIWORD(lParam));				// Змінюємо відношення сторін в сені і переініціалізуємо пристрій
			Render->Resize(LOWORD(lParam), HIWORD(lParam));						// Посилаємо повідомлення слоям про зміну розміру сцени
			Render->Timer(GetTickCount());										// Робимо відображення внесених змін
			Render->Rendering();
			return 0;
			
		case WM_WINDOWPOSCHANGED:												// Обробка повідомлення зміни позиції вікна
			if(hint != NULL) hint->Update();									// Змінюємо положення підказки, якщо вона активна
			Render->BeforeFreezing();
			return DefWindowProc(hWnd, msg, wParam, lParam);

		case WM_ENTERMENULOOP:													// Обробка моменту перед натисненням меню
			Render->BeforeFreezing();											// Посилаємо повідомлення слоям "заморозити" сцену
			return 0;
		
		case WM_EXITSIZEMOVE:													// Обробка переміщення вікна
			Render->AfterFreezing();											// Посилаємо повідомлення слоям продовжити роботу
			return DefWindowProc(hWnd, msg, wParam, lParam);

		case WM_EXITMENULOOP:													// Обробка моменту після виходу з меню
			Render->AfterFreezing();
			return 0;
	
		case SUCCESS_CONNECTED:													// Обробка повідомлення від серверу або клієнту про успішне
			{																	// встановлення з'єднання
					
				Render->DisableLayer(ID_LAYER_TITLE);							// Вимикаємо фон
				Render->SetParameter(ID_LAYER_TITLE, ID_WAITING_TEXT, TEXT_DISABLE);
				Render->SetParameter(ID_LAYER_TITLE, ID_SEARCH_TEXT, TEXT_DISABLE);
				
				Render->ResetAnimation();										// Скидуємо прапор анімації
				game = new NetChessGame(Render->Get3DDevice(), hWnd, (client == NULL)? WHITE : BLACK);
				Render->RegisterLayer(game, ID_LAYER_GAME);
				Render->Rendering();
				ENABLE_MENU_ITEM(hWnd, ID_CHANGE_THEME);						// Активізація пункту меню "Змінити тему"
			};
			break;
			
		case SUCCESS_DATA_RECEIVED:												// Обробка повідомлення про успішний прийом данних
			{
				int len = 0;
				char* mes = NULL;
				
				if(client != NULL){												// Визначаємо "хто" прийняв данні
					if (client->GetData(mes, len) != ERROR_QUEUE_IS_EMPTY) {	// Зчитуємо данні
						Render->SendData(ID_LAYER_GAME, mes, len);				// і посилаємо слоям на обробку
					}
				}
				else{
					if (server->GetData(mes, len) != ERROR_QUEUE_IS_EMPTY) {
						Render->SendData(ID_LAYER_GAME, mes, len);
					}
				}
				
				delete[] mes;													// Очищуємо буфер

			};
			break;
			
		case ERROR_MESSAGE:														// Обробка помилок

			if (wParam == WSAEADDRINUSE) {										// Намагаємося на одному порту запустити сервер ще раз
				MessageBox(hWnd, TEXT("Сервер вже запущений на даному комп'ютері"), TEXT("Alert"), MB_OK);
				DestroyWindow(hWnd);

			}
			
			Render->SetParameter(ID_LAYER_GAME, GAME_DISCONNECTION, GAME_YES);	// Посилаємо слоям повідомлення про помилки
			break;
	
		case WM_DESTROY:														// Обробка повідомлення про закриття програми
		case WM_CLOSE:
		{
			if(game != NULL){
					
				UINT endGame = Render->GetParameter(ID_LAYER_GAME, GAME_END);
				if(endGame != GAME_YES){
					endGame = MessageBox(hWnd, 
										TEXT("Sure, You want to quite?"),
										TEXT("Alert"),
										MB_YESNO);
					
					if(endGame == IDYES){
						Render->UnregisterLayerID(ID_LAYER_GAME);
						delete game;
						Render->EnableLayer(ID_LAYER_TITLE);
						PostQuitMessage(0);
					}
				}
				else{
					Render->UnregisterLayerID(ID_LAYER_GAME);
					delete game;
					Render->EnableLayer(ID_LAYER_TITLE);
					PostQuitMessage(0);
				}
			}
			else{
				PostQuitMessage(0);
			}
			return 0;
		}
	}
	// Пересилаємо всі інші повідомлення в DefWindowProc
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


// Початкова точка програми
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	RECT WindowRect;
	HWND hWnd;	
	WNDCLASSEX wc;
	TCHAR ClassName[] = TEXT("Application");
	TCHAR WindowName[] = TEXT("Chess 3D");
	MSG msg;


	// Створення програми
	// Реєстрація класу вікна
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_DBLCLKS;
	wc.lpfnWndProc		= MsgProc;
	wc.cbClsExtra		= 0L;
	wc.cbWndExtra		= 0L;
	wc.hInstance		= GetModuleHandle(NULL);
	wc.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON));	// Загружаємо створену іконку вікна
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);									// Загружаємо стандартний курсор
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= MAKEINTRESOURCE(IDR_MENU);
	wc.lpszClassName	= ClassName;
	wc.hIconSm	        = NULL;

    RegisterClassEx(&wc);															// Реєструємо клас вікна

	DWORD WindowStyle, WindowExStyle;
		
	WindowExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;								// Визначення Window Extended Style
	WindowStyle = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_SIZEBOX);				// Визначення Windows Style

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	WindowRect.left = (long)0;														// Встановлюємо позицію осі X
	WindowRect.right = (long)WINDOW_DEFAULT_WIDHT;									// Встановлюємо ширину 
	WindowRect.top = (long)0;														// Встановлюємо позицію по осі Y
	WindowRect.bottom = (long)WINDOW_DEFAULT_HEIGHT;								// Встановлюємо висоту
	
	AdjustWindowRectEx(&WindowRect, WindowStyle, true, WindowExStyle);				// Погодження розмірів із стилем

	int windowWidth = WindowRect.right - WindowRect.left;
	int windowHeight = WindowRect.bottom - WindowRect.top;

	// Создаем окно
	if (!(hWnd=CreateWindowEx(	WindowExStyle,										// Розширений стиль вікна
								ClassName,											// Ім'я класу
								WindowName,											// Заголовок выкна
								WindowStyle |										// Стиль вікна
								WS_CLIPSIBLINGS |
								WS_CLIPCHILDREN,
								(screenWidth / 2) - (windowWidth / 2),				// Позиція вікна X
								(screenHeight / 2) - (windowHeight / 2),			// Позиція вікна Y
								windowWidth,										// Ширина вікна
								windowHeight,										// Висота вікна
								NULL,												// Немає батьківського вікна
								NULL,												// Без меню (додамо його пізніше)
								wc.hInstance,										// Дескриптор екземпляра програми
								NULL)))												// Вказівник на дані створення вікна
	{
		MessageBox(NULL, TEXT("Неможливо створити вікно!"), TEXT("Alert"), MB_OK | MB_ICONEXCLAMATION);
		return 1;
	}

	CLButton button;																// Заповнюємо значення кнопок

	button.BtnID = ID_LOCAL_GAME;
	button.BtnText = TEXT("Play Offline");
	button.BtnNote = TEXT("Chess is usually played by two players against each other.");
	button.BtnIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON_LOCAL_GAME));
	buttons.push_back(button);

	button.BtnID = ID_NET_SERVER;
	button.BtnText = TEXT("Play on Server");
	button.BtnNote = TEXT("Chess is usually played by two players against each other.");
	button.BtnIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON_NET_SERVER));
	buttons.push_back(button);

	/*button.BtnID = ID_NET_CLIENT;
	button.BtnText = TEXT("Play Online");
	button.BtnNote = TEXT("Chess is usually played by two players against each other.");
	button.BtnIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON_NET_CLIENT));
	buttons.push_back(button);*/

	button.BtnID = ID_WHITE_THEME;
	button.BtnText = TEXT("Simple Theme");
	button.BtnNote = TEXT("");
	button.BtnIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_WHITE_THEME));
	themeButtons.push_back(button);

	button.BtnID = ID_COLORED_THEME;
	button.BtnText = TEXT("Colorful Theme");
	button.BtnNote = TEXT("");
	button.BtnIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_COLORED_THEME));
	themeButtons.push_back(button);

	hint = new Hint(hWnd, hInst);														// Створюємо об'єкт підказки

	try{

		Render = new Engine(hWnd);														// Створення рендеру
		Render->Init();																	// Ініціалізація Direct3D
		
		title = new Title(Render->Get3DDevice(), hWnd);
		Render->RegisterLayer(title, ID_LAYER_TITLE);

		Render->BeforeFreezing();

		ShowWindow(hWnd, SW_SHOWNORMAL);												// Показуємо вікно
		SetFocus(hWnd);																	// Встановлюємо фокус
		UpdateWindow(hWnd);																// Оновлюємо вікно
		SetForegroundWindow(hWnd);														// Підвищуємо пріоритет

		Render->AfterFreezing();

		DISABLE_MENU_ITEM(hWnd, ID_CHANGE_THEME);

		// Починаємо головний процес рендерингу
		do{
			if(Render->IsAnimated()){													// Якщо рухаються об'єкти(фігури) на сцені
				if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				Render->Timer(GetTickCount());											// Прорахунок нових положень об'єктів
				Render->Rendering();													// Рендеринг сцени
			}
			else{
				if(GetMessage(&msg, NULL, 0, 0)){										// Обробока всіх повідомлень
					TranslateMessage(&msg);
					DispatchMessage(&msg);
					Render->Rendering();
				}
			}
		}while(msg.message != WM_QUIT);
					
		Render->Destroy();																// Знищуємо рендер

		Render->UnregisterLayerID(ID_LAYER_TITLE);
		
		delete title;

		delete Render;																	// Звільняємо ресурси
	}
	catch(EngineException &e){
		MessageBox(hWnd, e.what(), TEXT("Alert"), MB_OK | MB_ICONEXCLAMATION);
	}
	
	UnregisterClass(ClassName, wc.hInstance);
    
	return 0;
}