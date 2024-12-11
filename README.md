# MineSweeper_GUI
Updated version of Minesweeper using Win32 Graphics (C++)

## 1. Different Modules

Let's break the program down into modules:

![image](https://github.com/user-attachments/assets/7e89564e-f110-456f-be28-36b3733b9580)

To make our program somehow 'clear', we shall put different stuff into different modules (in our case different classes)

Minesweeper GUI has a main module 'MineSweeperGame' and three sub modules 'WndClassWin32' (handles Win32 Window) 'MineSweeper' (Minesweeper game logic) and 'LL_Queue' (not shown on the graph. this is a implementation of Queue)

We noticed 'MineSweeperGame' needs to communicate with 'WndClassWin32' and 'MineSweeper', so for the commands, we directly call corresponding functions and for those messages, we use Queue (ADT). [That's why we need to implement a Queue (I used link list to implement a Queue)]

Now, let's dive into separate modules

## 2. Class 'Minesweeper' (the game logic)

This class has two 2D arrays to store map data (one for real and one for the user (current)) [can either use stack memory for that or dynamically allocated memory], functions to set up and initialize the arrays, function to set up (or load) the mine map, a recursive function to perform map exploration and some other attributive member variables and helper functions. Overall, somehow like this:

## 3. The Queue (ADT)

A link-list implemented queue

Note that we have a node data structure like this:
```
struct LL_NodeData
{
 unsigned int data;
 unsigned int type;
 LL_NodeData* next;
};
```

## 4. Class 'WndClassWin32'

This class creates window on Windows operating system (using Windows API) and handles Windows messages.

## 5. The 'MineSweeperGame' class

This class has "everything" it handles game operation and the update of GUI. 

## 6. Notes

### a) Interesting macros

if you read 'MineSweeperGame.h' you'll see this:
```
#define GAME_CELL_FONT_COLOR(c) \
 ( c == '1' ? RGB(44, 60, 229) \
 : c == '2' ? RGB(44, 223, 89) \
 : c == '3' ? RGB(254, 77, 23) \
 : c == '4' ? RGB(0, 65, 180) \
 : RGB(200, 2, 0))
```
macros do can take parameters, however, they just simplify replace the space with its definition.

### b) Bit fiddling (bit operation)

```
#define ENCODE_DWORD(l,h) (l | (h << 16))
#define GET_LOWORD(l) ((WORD)(l) & 0xffff)
#define GET_HIWORD(h) ((WORD)(h >> 16) & 0xffff)
```
this code basically does something like this:
```
A = 0x11
B = 0x22
C = ENCODE_DWORD(A,B) -> C = 0x2211
hmm, interesting. We can put more information in a single integer(4 bytes on Windows machines) or so.
```

### c) Function pointers

```
bool WndDrawElements(void (*drawFunction)(HDC hdc, void* dataX, void* dataY), void* dataX, void* dataY, WndDrawType drawBufType, bool clear) const;
```
function pointers represents the address of that specific function, and you can call that function using the pointer pointed to it, like this:
```
 // call draw function (if there is one)
 if (NULL != drawFunction)
 {
...
  (*drawFunction)(hdcDrawer, dataX, dataY);
...
 }
```
we 'de-reference' the pointer of the function and call it [using this word is not accurate and C standard also has different versions on this -> you can just understand function pointers in this way]

### d) another weird #define -> #ifdef _DEBUG

You may see this scattered around:
```
#ifdef _DEBUG
 std::wcout << this << " CLASS: 'WndClassWin32' constructor being called\n";
#endif // debug message (you will see this message printed when Visual Studio is in DEBUG config)
```
well, this _DEBUG is automatically defined if Visual Studio is configured to Debug Mode.

### e) WCHAR (wchar_t) wcout

this THING is just for the support of UNICODE, when using these types you have to add a L in front of those literal constants (like-> L"Sample")
