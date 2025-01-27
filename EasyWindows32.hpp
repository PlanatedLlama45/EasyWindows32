/*

MIT License

Copyright (c) 2025 PlanatedLlama45

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#pragma once

#ifndef UNICODE
    #define UNICODE
#endif

#include <Windows.h>
#include <cstdint>
#include <vector>
#include <string>

#ifndef _M_EZW32_CLASS_NAME
    #define _M_EZW32_CLASS_NAME L"window_class"
#endif // !_M_EZW32_CLASS_NAME

namespace easywindows32 {

class Font {
public:
    Font(LPCWSTR fontName, int size, int weight = FW_DONTCARE, bool italic = false, bool underlined = false, bool striked = false) :
        m_fontName(fontName),
        m_size(size), m_weight(weight),
        m_italic(italic), m_underlined(underlined), m_striked(striked)
    {
        m_handle = CreateFont(
            m_size, 0, 0, 0,
            m_weight,
            (DWORD)m_italic, (DWORD)m_underlined, (DWORD)m_striked,
            ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_SWISS,
            m_fontName
        );
    }

    const HFONT getHandle() const { return m_handle; }

protected:
    HFONT m_handle;
    LPCWSTR m_fontName;
    int m_size, m_weight;
    bool m_italic, m_underlined, m_striked;
};


class IElement {
public:
    IElement(LPCWSTR className) :
        m_className(className),
        m_id(++s_elemCount),
        m_handle(NULL),
        m_font(s_fontDefault)
        { }

    const uint64_t getID() const { return m_id; }
    const LPCWSTR getClassName() const { return m_className; }
    const HWND getHandle() const { return m_handle; }
    void setFont(const Font &font) { m_font = &font; }

    static void setFontDefault(const Font &font) { s_fontDefault = &font; }

    // Initializes the handle and the font
    virtual void create(HWND parent) = 0;

protected:
    static uint64_t s_elemCount;
    static const Font *s_fontDefault;

    const uint64_t m_id;
    const LPCWSTR m_className;
    HWND m_handle;
    const Font *m_font;

    void m_bindFont() {
        if (!m_font)
            return;
        SendMessage(m_handle, WM_SETFONT, (WPARAM)m_font->getHandle(), TRUE);
    }
};

uint64_t IElement::s_elemCount = 0;
const Font *IElement::s_fontDefault = nullptr;


enum class Align { Left, Right, Center };

class ITextElement {
public:
    ITextElement(const std::wstring &text, Align alignText) :
        m_text(text), m_textAlign(alignText)
        { }

    const std::wstring &getText() const { return m_text; }
    const LPCWSTR getTextCstr() const { return m_text.c_str(); }
    void setText(const std::wstring &text) { m_text = text; m_updateHandleText(); }
    void setText(LPCWSTR text) { m_text = text; m_updateHandleText(); }

protected:
    std::wstring m_text;
    Align m_textAlign;

    virtual void m_updateHandleText() const = 0;
    #define _M_EZW32_I_TEXT_ELEMENT_UPDATE_HANDLE_TEXT_DEFINE() \
        void m_updateHandleText() const override { \
            SetWindowText(m_handle, m_text.c_str()); \
        }

    virtual DWORD m_getTextAlignFlag() const = 0;
    #define _M_EZW32_I_TEXT_ELEMENT_GET_TEXT_ALIGN_FLAG_DEFINE(left, center, right) \
        DWORD m_getTextAlignFlag() const override { \
            switch (m_textAlign) { \
            case Align::Left:   return left; \
            case Align::Center: return center; \
            case Align::Right:  return right; \
            default:            return 0; \
            } \
        }
};

class IPositionElement {
public:
    IPositionElement(SHORT posX, SHORT posY) :
        m_pos({ posX, posY })
        { }

    const SHORT getPosX() const { return m_pos.X; }
    const SHORT getPosY() const { return m_pos.Y; }
    const COORD getPos() const { return m_pos; }

protected:
    COORD m_pos;
};

class ISizeElement {
public:
    ISizeElement(SHORT width, SHORT height) :
        m_size({ width, height })
        { }

    const SHORT getWidth() const { return m_size.X; }
    const SHORT getHeight() const { return m_size.Y; }
    const COORD getSize() const { return m_size; }

protected:
    COORD m_size;
};


class Static : public IElement, public ITextElement, public IPositionElement, ISizeElement {
public:
    Static(SHORT posX, SHORT posY, SHORT width, SHORT height, const std::wstring &text = L"", Align alignText = Align::Center) :
        IElement(L"static"),
        ITextElement(text, alignText),
        IPositionElement(posX, posY),
        ISizeElement(width, height)
        { }

    void create(HWND parent) override {
        m_handle = CreateWindow(
            m_className,
            m_text.c_str(),
            WS_CHILD | WS_VISIBLE | m_getTextAlignFlag() | SS_CENTERIMAGE,
            m_pos.X, m_pos.Y,
            m_size.X, m_size.Y,
            parent, (HMENU)m_id,
            NULL,
            NULL
        );
        m_bindFont();
    }

protected:
    _M_EZW32_I_TEXT_ELEMENT_UPDATE_HANDLE_TEXT_DEFINE()
    _M_EZW32_I_TEXT_ELEMENT_GET_TEXT_ALIGN_FLAG_DEFINE(SS_LEFT, SS_CENTER, SS_RIGHT)
};


class Button;
template <class T>
using FuncCall = void (*)(T &);

class Button : public IElement, public ITextElement, public IPositionElement, public ISizeElement {
public:
    Button(SHORT posX, SHORT posY, SHORT width, SHORT height, const std::wstring &text = L"", FuncCall<Button> onClick = nullptr, Align alignText = Align::Center) :
        IElement(L"button"),
        ITextElement(text, alignText),
        IPositionElement(posX, posY),
        ISizeElement(width, height),
        m_onClick(onClick)
        { }

    void create(HWND parent) override {
        m_handle = CreateWindow(
            m_className,
            m_text.c_str(),
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | m_getTextAlignFlag() | BS_VCENTER,
            m_pos.X, m_pos.Y,
            m_size.X, m_size.Y,
            parent, (HMENU)m_id,
            NULL,
            NULL
        );
        m_bindFont();
    }

    FuncCall<Button> getClickFunction() { return m_onClick; }

protected:
    FuncCall<Button> m_onClick;

    _M_EZW32_I_TEXT_ELEMENT_UPDATE_HANDLE_TEXT_DEFINE()
    _M_EZW32_I_TEXT_ELEMENT_GET_TEXT_ALIGN_FLAG_DEFINE(BS_LEFT, BS_CENTER, BS_RIGHT)
};


class Edit : public IElement, public ITextElement, public IPositionElement, public ISizeElement {
public:
    Edit(SHORT posX, SHORT posY, SHORT width, SHORT height, bool isNumberOnly = false, Align alignText = Align::Left, const std::wstring &presetText = L"") :
        IElement(L"edit"),
        ITextElement(presetText, alignText),
        IPositionElement(posX, posY),
        ISizeElement(width, height),
        m_isNumberOnly(isNumberOnly)
        { }

    void create(HWND parent) override {
        m_handle = CreateWindow(
            m_className,
            m_text.c_str(),
            WS_CHILD | WS_VISIBLE | WS_BORDER | (m_isNumberOnly ? ES_NUMBER : 0) | m_getTextAlignFlag(),
            m_pos.X, m_pos.Y,
            m_size.X, m_size.Y,
            parent, (HMENU)m_id,
            NULL,
            NULL
        );
        m_bindFont();
    }

    const std::wstring &getText() const = delete;
    const LPCWSTR getTextCstr() const = delete;

    const std::wstring &getText() { m_updateTextFromHandle(); return m_text; }
    const LPCWSTR getTextCstr() { m_updateTextFromHandle(); return m_text.c_str(); }

protected:
    bool m_isNumberOnly;

    void m_updateTextFromHandle() {
        int len = 1+GetWindowTextLength(m_handle);
        m_text.resize(len, '\0');
        GetWindowText(m_handle, &m_text[0], len);
    }

    _M_EZW32_I_TEXT_ELEMENT_UPDATE_HANDLE_TEXT_DEFINE()
    _M_EZW32_I_TEXT_ELEMENT_GET_TEXT_ALIGN_FLAG_DEFINE(ES_LEFT, ES_CENTER, ES_RIGHT)
};


template <class T>
class Reference {
public:
    Reference(const Reference &ref) : m_ptr(ref->m_ptr) { };
    Reference(Reference &&ref) : m_ptr(ref->m_ptr) { };
    Reference() : m_ptr(nullptr) { };
    Reference(T &ref) : m_ptr(&ref) { };
    
    Reference &operator =(T &ref) { m_ptr = &ref; return *this; }
    Reference &operator =(T &&ref) { m_ptr = &ref; return *this; }
    Reference &operator =(const Reference &ref) { m_ptr = ref.m_ptr; return *this; }
    Reference &operator =(Reference &&ref) { m_ptr = ref.m_ptr; return *this; }
    T *operator ->() { return m_ptr; }

    friend bool operator ==(const Reference &ref1, const T &ref2) { return (ref1.m_ptr == &ref2); }
    friend bool operator ==(const Reference &ref1, const Reference &ref2) { return (ref1.m_ptr == ref2.m_ptr); }

protected:
    T *m_ptr;
};

using RStatic = Reference<Static>;
using RButton = Reference<Button>;
using REdit = Reference<Edit>;


struct _M_AppData {
    ~_M_AppData() {
        for (IElement *elem : m_elements)
            delete elem;
    }

    int m_posX, m_posY;
    int m_width, m_height;
    bool m_isResizeable;
    DWORD m_style;
    LPCWSTR m_title;
    bool m_isRunning;
    std::vector<IElement *> m_elements;
};

_M_AppData _m_appData = {
    .m_posX          = CW_USEDEFAULT,
    .m_posY          = CW_USEDEFAULT,
    .m_width         = CW_USEDEFAULT,
    .m_height        = CW_USEDEFAULT,
    .m_isResizeable  = false,
    .m_style         = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME,
    .m_title         = L"window title",
    .m_elements      = { }
};

void setWindowPosition(DWORD x, DWORD y) {
    _m_appData.m_posX = x;
    _m_appData.m_posY = y;
}
void setWindowSize(DWORD width, DWORD height) {
    _m_appData.m_width = width;
    _m_appData.m_height = height;
}
void setWindowTitle(LPCWSTR title) {
    _m_appData.m_title = title;
}
void setWindowStyle(DWORD style) {
    _m_appData.m_style = style;
}
void setWindowResizeable(bool value) {
    _m_appData.m_isResizeable = value;
    if (_m_appData.m_isResizeable)
        _m_appData.m_style |= WS_THICKFRAME;
    else
        _m_appData.m_style &= ~WS_THICKFRAME;
}

Static &addStatic(SHORT posX, SHORT posY, SHORT width, SHORT height, const std::wstring &text = L"", Align alignText = Align::Center) {
    Static *newStatic = new Static(posX, posY, width, height, text, alignText);
    _m_appData.m_elements.push_back(dynamic_cast<IElement *>(newStatic));
    return *newStatic;
}
Button &addButton(SHORT posX, SHORT posY, SHORT width, SHORT height, const std::wstring &text = L"", FuncCall<Button> onClick = nullptr, Align alignText = Align::Center) {
    Button *newBtn = new Button(posX, posY, width, height, text, onClick, alignText);
    _m_appData.m_elements.push_back(dynamic_cast<IElement *>(newBtn));
    return *newBtn;
}
Edit &addEdit(SHORT posX, SHORT posY, SHORT width, SHORT height, bool isNumberOnly = false, Align alignText = Align::Left, const std::wstring &presetText = L"") {
    Edit *newEdit = new Edit(posX, posY, width, height, isNumberOnly, alignText, presetText);
    _m_appData.m_elements.push_back(dynamic_cast<IElement *>(newEdit));
    return *newEdit;
}

extern void Initialize();

} // namespace easywindows32

LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    using namespace easywindows32;

    Initialize();
    // Register the window class
    WNDCLASS wc = {
        .style = 0,
        .lpfnWndProc = MainWindowProc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = hInstance,
        .hIcon = NULL,
        .hCursor = NULL,
        .hbrBackground = (HBRUSH)(COLOR_WINDOW+1),
        .lpszMenuName = NULL,
        .lpszClassName = _M_EZW32_CLASS_NAME,
    };

    if (!RegisterClass(&wc))
        return FALSE;

    // Create the window.
    HWND hWnd = CreateWindow(
        _M_EZW32_CLASS_NAME,
        _m_appData.m_title,
        _m_appData.m_style,

        _m_appData.m_posX,
        _m_appData.m_posY,

        _m_appData.m_width,
        _m_appData.m_height,

        NULL,       // Parent window
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (!hWnd) return FALSE;

    // HICON hIcon = LoadIcon(NULL, IDI_EXCLAMATION);
    // SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    // SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

    ShowWindow(hWnd, nCmdShow);

    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    using namespace easywindows32;

    switch (uMsg) {
    case WM_CREATE:
        for (IElement *elem : _m_appData.m_elements)
            elem->create(hWnd);
        return 0;

    case WM_KEYDOWN:
        if (wParam != VK_ESCAPE)
            return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.

            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
            // FillRect(hdc, &ps.rcPaint, (HBRUSH)WHITE_BRUSH);

            EndPaint(hWnd, &ps);
        }
        return 0;

        case WM_COMMAND:
            if (HIWORD(wParam) == 0) {
                IElement *elem = _m_appData.m_elements[LOWORD(wParam) - 1];
                Button *btn = dynamic_cast<Button *>(elem);
                if (!btn)
                    return 0;
                FuncCall<Button> func = btn->getClickFunction();
                if (!func)
                    return 0;
                (*func)(*btn);
            }
            return 0;

    // case WM_DRAWITEM:
    //     {
    //         LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
    //         if (pDIS->hwndItem == hStatic) {
    //             SetTextColor(pDIS->hDC, RGB(0, 0, 0));
    //             CHAR staticText[99];
    //             int len = GetWindowText(hStatic, staticText, ARRAYSIZE(staticText));

    //             TextOut(pDIS->hDC, pDIS->rcItem.left, pDIS->rcItem.top, staticText, len);
    //         }
    //         return 1;
    //     }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
