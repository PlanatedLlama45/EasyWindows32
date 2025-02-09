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

#define _M_EZW32_ELEM_NAME_STATIC L"static"
#define _M_EZW32_ELEM_NAME_BUTTON L"button"
#define _M_EZW32_ELEM_NAME_EDIT L"edit"
#define _M_EZW32_ELEM_NAME_LISTBOX L"listbox"


namespace easywindows32 {

/**
 * @brief Класс для исключений
 */
class Exception : public std::exception {
public:
    /**
     * @brief Конструктор
     * @param msg текст исключения
     */
    Exception(const char *msg) : m_msg(msg) { }

    /**
     * @brief Получить текст исключения
     * @return Текст исключения
     */
    const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override {
        return m_msg;
    }

protected:
    const char *m_msg;
};

/**
 * @brief Класс-обёртка для шрифта
 */
class Font {
public:
    /**
     * @brief Конструктор
     * @param fontName имя шрифта в ОС Windows
     * @param size размер шрифта
     * @param weight вес (жирность) шрифта
     * @param italic курсивный (T/F)
     * @param underlined подчёркнутый (T/F)
     * @param striked перечёркнутый (T/F)
     */
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

    /**
     * @brief Получить дескриптор шрифта
     * @return Дескриптор шрифта
     */
    const HFONT getHandle() const { return m_handle; }

protected:
    HFONT m_handle;
    LPCWSTR m_fontName;
    int m_size, m_weight;
    bool m_italic, m_underlined, m_striked;
};


/**
 * @brief Абстрактный класс элемента UI
 */
class IElement {
public:
    /**
     * @brief Конструктор
     * @param className внутреннее имя класса элемента
     */
    IElement(LPCWSTR className) :
        m_className(className),
        m_id(++s_elemCount),
        m_handle(NULL),
        m_font(s_fontDefault)
        { }

    /**
     * @brief Получить ИД элемента
     * @return ИД
     */
    const uint64_t getID() const { return m_id; }
    /**
     * @brief Получить внутреннее имя класса элемента
     * @return имя класса
     */
    const LPCWSTR getClassName() const { return m_className; }
    /**
     * @brief Получить дескриптор элемента
     * @return дескриптор
     */
    const HWND getHandle() const { return m_handle; }
    /**
     * @brief Установить шрифт
     * @param font ссылка на шрифт
     */
    void setFont(const Font &font) { m_font = &font; }

    /**
     * @brief Установить шрифт по умолчанию (автоматически применяется ко всем элементам, созданным после установки)
     * @param font ссылка на шрифт
     */
    static void setFontDefault(const Font &font) { s_fontDefault = &font; }

    /**
     * @brief Инициализирует дескриптор и прикрепляет к нему шрифт
     * @param parent дескриптор родительского элемента
     */
    virtual void create(HWND parent) = 0;

protected:
    static uint64_t s_elemCount;
    static const Font *s_fontDefault;

    const uint64_t m_id;
    const LPCWSTR m_className;
    HWND m_handle;
    const Font *m_font;

    void m_bindFont() {
        SendMessage(m_handle, WM_SETFONT, (WPARAM)m_font->getHandle(), (LPARAM)TRUE);
    }
};


uint64_t IElement::s_elemCount = 0;
const Font *IElement::s_fontDefault = nullptr;

/**
 * @brief Энумерация выравнивания элементов/текста
 */
enum class Align { Left, Right, Center };


/**
 * @brief Абстрактный класс текстового элемента UI
 */
class ITextElement {
public:
    /**
     * @brief Конструктор
     * @param text текст
     * @param alignText выравнивание текста
     */
    ITextElement(const std::wstring &text, Align alignText) :
        m_text(text), m_textAlign(alignText)
        { }

    /**
     * @brief Получить текст
     * @return Объект строки
     */
    const std::wstring &getText() const { return m_text; }
    /**
     * @brief Получить текст
     * @return Указатель на строку (c-style)
     */
    const LPCWSTR getTextCstr() const { return m_text.c_str(); }
    /**
     * @brief Установить текст
     * @param text объект строки
     */
    void setText(const std::wstring &text) { m_text = text; m_updateHandleText(); }
    /**
     * @brief Установить строку
     * @return указатель на строку (c-style)
     */
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


/**
 * @brief Абстрактный класс элемента UI с позицией
 */
class IPositionElement {
public:
    /**
     * @brief Конструктор
     * @param posX X-координата
     * @param posY Y-координата
     */
    IPositionElement(SHORT posX, SHORT posY) :
        m_pos({ posX, posY })
        { }

    /**
     * @brief Получить X-координату
     * @return X-координата
     */
    const SHORT getPosX() const { return m_pos.X; }
    /**
     * @brief Получить Y-координату
     * @return Y-координата
     */
    const SHORT getPosY() const { return m_pos.Y; }
    /**
     * @brief Получить позицию
     * @return позиция (COORD)
     */
    const COORD getPos() const { return m_pos; }

protected:
    COORD m_pos;
};


/**
 * @brief Абстрактный класс элемента UI с позицией
 */
class ISizeElement {
public:
    /**
     * @brief Конструктор
     * @param width ширина (размер по X)
     * @param height высота (размер по Y)
     */
    ISizeElement(SHORT width, SHORT height) :
        m_size({ width, height })
        { }

    /**
     * @brief Получить ширину (размер по X)
     * @return ширина
     */
    const SHORT getWidth() const { return m_size.X; }
    /**
     * @brief Получить высоту (размер по Y)
     * @return высота
     */
    const SHORT getHeight() const { return m_size.Y; }
    /**
     * @brief Получить размер
     * @return размер (COORD)
     */
    const COORD getSize() const { return m_size; }

protected:
    COORD m_size;
};


/**
 * @brief Класс-обёртка для статичного текстового элемента
 */
class Static : public IElement, public ITextElement, public IPositionElement, ISizeElement {
public:
    /**
     * @brief Конструктор
     * @param posX X-координата
     * @param posY Y-координата
     * @param width ширина
     * @param height высота
     * @param text текст = ""
     * @param alignText выравнивание = Center
     */
    Static(SHORT posX, SHORT posY, SHORT width, SHORT height, const std::wstring &text = L"", Align alignText = Align::Center) :
        IElement(_M_EZW32_ELEM_NAME_STATIC),
        ITextElement(text, alignText),
        IPositionElement(posX, posY),
        ISizeElement(width, height)
        { }

    /**
     * @brief Инициализирует дескриптор и прикрепляет к нему шрифт
     * @param parent дескриптор родительского элемента
     */
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


/**
 * @brief Класс-обёртка для элемента кнопки
 */
class Button;
/**
 * @brief Тип функции для вызова внутри класса T
 */
template <class T>
using FuncCall = void (*)(T &);

class Button : public IElement, public ITextElement, public IPositionElement, public ISizeElement {
public:
    /**
     * @brief Конструктор
     * @param posX X-координата
     * @param posY Y-координата
     * @param width ширина
     * @param height высота
     * @param text текст = ""
     * @param onClick ответная функция на нажатие = NULL
     * @param alignText выравнивание = Center
     */
    Button(SHORT posX, SHORT posY, SHORT width, SHORT height, const std::wstring &text = L"", FuncCall<Button> onClick = nullptr, Align alignText = Align::Center) :
        IElement(_M_EZW32_ELEM_NAME_BUTTON),
        ITextElement(text, alignText),
        IPositionElement(posX, posY),
        ISizeElement(width, height),
        m_onClick(onClick)
        { }

    /**
     * @brief Инициализирует дескриптор и прикрепляет к нему шрифт
     * @param parent дескриптор родительского элемента
     */
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

    /**
     * @brief Получить функцию при нажатии
     * @return ответная функция
     */
    FuncCall<Button> getOnClick() { return m_onClick; }

protected:
    FuncCall<Button> m_onClick;

    _M_EZW32_I_TEXT_ELEMENT_UPDATE_HANDLE_TEXT_DEFINE()
    _M_EZW32_I_TEXT_ELEMENT_GET_TEXT_ALIGN_FLAG_DEFINE(BS_LEFT, BS_CENTER, BS_RIGHT)
};


/**
 * @brief Класс-обёртка для элемента текстового ввода
 */
class Edit : public IElement, public ITextElement, public IPositionElement, public ISizeElement {
public:
    /**
     * @brief Конструктор
     * @param posX X-координата
     * @param posY Y-координата
     * @param width ширина
     * @param height высота
     * @param isNumberOnly только численный ввод (T/F) = F
     * @param alignText выравнивание текста = Left
     * @param presetText начальный текст = ""
     */
    Edit(SHORT posX, SHORT posY, SHORT width, SHORT height, bool isNumberOnly = false, Align alignText = Align::Left, const std::wstring &presetText = L"") :
        IElement(_M_EZW32_ELEM_NAME_EDIT),
        ITextElement(presetText, alignText),
        IPositionElement(posX, posY),
        ISizeElement(width, height),
        m_isNumberOnly(isNumberOnly)
        { }

    /**
     * @brief Инициализирует дескриптор и прикрепляет к нему шрифт
     * @param parent дескриптор родительского элемента
     */
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

    /**
     * @brief Получить текст
     * @return Объект строки
     */
    const std::wstring &getText() { m_updateTextFromHandle(); return m_text; }
    /**
     * @brief Получить текст
     * @return Указатель на строку (c-style)
     */
    const LPCWSTR getTextCstr() { m_updateTextFromHandle(); return m_text.c_str(); }

protected:
    bool m_isNumberOnly;

    void m_updateTextFromHandle() {
        int len = 1+GetWindowTextLength(m_handle);
        if (len == 1) {
            m_text.clear();
            return;
        }
        m_text.resize(len, '\0');
        GetWindowText(m_handle, &m_text[0], len);
    }

    _M_EZW32_I_TEXT_ELEMENT_UPDATE_HANDLE_TEXT_DEFINE()
    _M_EZW32_I_TEXT_ELEMENT_GET_TEXT_ALIGN_FLAG_DEFINE(ES_LEFT, ES_CENTER, ES_RIGHT)
};


/**
 * @brief Класс-обёртка для элемента списка
 */
class ListBox : public IElement, public IPositionElement, public ISizeElement {
public:
    /**
     * @brief Конструктор
     * @param posX X-координата
     * @param posY Y-координата
     * @param width ширина
     * @param height высота
     */
    ListBox(SHORT posX, SHORT posY, SHORT width, SHORT height, FuncCall<ListBox> onSelect = nullptr) :
        IElement(_M_EZW32_ELEM_NAME_LISTBOX),
        IPositionElement(posX, posY),
        ISizeElement(width, height),
        m_onSelect(onSelect)
        { }

    /**
     * @brief Инициализирует дескриптор и прикрепляет к нему шрифт и ранее добавленные элементы
     * @param parent дескриптор родительского элемента
     */
    void create(HWND parent) override {
        m_handle = CreateWindow(
            m_className,
            L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
            m_pos.X, m_pos.Y,
            m_size.X, m_size.Y,
            parent, (HMENU)m_id,
            NULL,
            NULL
        );
        m_bindFont();
        for (auto &item : m_items)
            SendMessage(m_handle, LB_ADDSTRING, (WPARAM)NULL, (LPARAM)&item[0]);
    }

    /**
     * @brief Получить ответную функцию
     * @return ответная функция
     */
    FuncCall<ListBox> getOnSelect() { return m_onSelect; }

    /**
     * @brief Добавить элемент в список
     * @param value значение элемента (c-style)
     */
    void addItem(LPCSTR value) {
        m_items.emplace_back(reinterpret_cast<const wchar_t *>(value));
        if (m_handle) SendMessage(m_handle, LB_ADDSTRING, (WPARAM)NULL, (LPARAM)value);
    }
    /**
     * @brief Добавить элемент в список
     * @param value значение элемента
     */
    void addItem(const std::wstring &value) {
        m_items.push_back(value);
        if (m_handle) SendMessage(m_handle, LB_ADDSTRING, (WPARAM)NULL, (LPARAM)value.c_str());
    }
    /**
     * @brief Удалить элемент из списка
     * @param index индекс элемента, который нужно удалить
     * @throws Если индекс не входит в границы списка (easywindows::Exception)
     */
    void removeItem(int64_t index) {
        if (index < 0 || index >= m_items.size())
            throw Exception("Index out of range (easywindows32::ListBox::setSelectedIndex)");
        SendMessage(m_handle, LB_DELETESTRING, (WPARAM)index, (LPARAM)NULL);
        m_items.erase(m_items.begin() + index);
    }
    /**
     * @brief Очистить список
     */
    void clear() {
        for (LRESULT i = 0; i < m_items.size(); i++)
            SendMessage(m_handle, LB_DELETESTRING, (WPARAM)0, (LPARAM)NULL);
        m_items.clear();
    }

    /**
     * @brief Получить индекс выделенного элемента
     * @return Индекс выделенного элемента (если ни один не выбран, то LB_ERR)
     */
    int64_t getSelectedIndex() const {
        return SendMessage(m_handle, LB_GETCURSEL, (WPARAM)NULL, (LPARAM)NULL);
    }
    /**
     * @brief Получить значение выделенного элемента
     * @return Текст выделенного элемента (std::wstring)
     * @throws Если не выбран ни один элемент (easywindows32::Exception)
     */
    std::wstring getSelectedItem() const {
        int64_t id = getSelectedIndex();
        if (id == LB_ERR)
            throw Exception("No item has been selected (easywindows32::ListBox::getSelectedItem)");
        return m_items[id];
    }
    /**
     * @brief Получить индекс элемента с определённым значением
     * @param item значение элемента
     * @return Индекс элемента (если эелемент не найден, то LB_ERR)
     */
    LPARAM findItem(const std::wstring &item) const {
        for (LPARAM i = 0; i < m_items.size(); i++) {
            if (m_items[i] == item)
                return i;
        }
        return LB_ERR;
    }
    /**
     * @brief Установить выделение на определённый элемент
     * @param index индекс элемента, который нужно выделть (если -1, то выделение сбрасывается)
     * @throws Если индекс не входит в границы списка (easywindows::Exception)
     */
    void setSelectedItem(int64_t index) const {
        if (index < -1 || index >= m_items.size())
            throw Exception("Index out of range (easywindows32::ListBox::setSelectedIndex)");
        SendMessage(m_handle, LB_SETCURSEL, (WPARAM)index, (LPARAM)NULL);
    }
    /**
     * @brief Сбросить выделение
     */
    void resetSelection() const {
        SendMessage(m_handle, LB_SETCURSEL, (WPARAM)(-1), (LPARAM)NULL);
    }

protected:
    std::vector<std::wstring> m_items;
    FuncCall<ListBox> m_onSelect;
};


/**
 * @brief Класс-обёртка ссылки на объект
 * @tparam T тип объекта
 */
template <class T>
class Reference {
public:
    /**
     * @brief Конструктор по умолчанию
     */
    Reference() : m_ptr(nullptr) { };
    /**
     * @brief Конструктор
     * @param ref lvalue-ссылка на объект
     */
    Reference(T &ref) : m_ptr(&ref) { };
    /**
     * @brief Конструктор
     * @param ref rvalue-ссылка на объект
     */
    Reference(T &&ref) : m_ptr(&ref) { };
    /**
     * @brief Конструктор
     * @param ref lvalue-ссылка на другую ссылку
     */
    Reference(const Reference<T> &ref) : m_ptr(ref->m_ptr) { };
    /**
     * @brief Конструктор
     * @param ref rvalue-ссылка на другую ссылку
     */
    Reference(Reference<T> &&ref) : m_ptr(ref->m_ptr) { };
    
    /**
     * @brief Установка значения
     * @param ref lvalue-ссылка на объект
     */
    Reference &operator =(T &ref) { m_ptr = &ref; return *this; }
    /**
     * @brief Установка значения
     * @param ref rvalue-ссылка на объект
     */
    Reference &operator =(T &&ref) { m_ptr = &ref; return *this; }
    /**
     * @brief Установка значения
     * @param ref lvalue-ссылка на другую ссылку
     */
    Reference &operator =(const Reference &ref) { m_ptr = ref.m_ptr; return *this; }
    /**
     * @brief Установка значения
     * @param ref rvalue-ссылка на другую ссылку
     */
    Reference &operator =(Reference &&ref) { m_ptr = ref.m_ptr; return *this; }
    /**
     * @brief Обращение к объекту
     * @return указатель на объект
     */
    T *operator ->() { return m_ptr; }

    /**
     * @brief Получение объекта
     * @return ссылка на объект
     */
    T &get() { return *m_ptr; }

    /**
     * @brief Хранит ли объект ссылку?
     * @return T - объект ссылки пуст, F - имеет объект
     */
    bool isnull() const { return (m_ptr == nullptr); }

    /**
     * @brief Проверка равенства ссылок
     * @param ref1 ссылка А
     * @param ref2 ссылка на объект Б
     * @return T/F
     */
    friend bool operator ==(const Reference &ref1, const T &ref2) { return (ref1.m_ptr == &ref2); }
    /**
     * @brief Проверка равенства ссылок
     * @param ref1 ссылка А
     * @param ref2 ссылка Б
     * @return T/F
     */
    friend bool operator ==(const Reference &ref1, const Reference &ref2) { return (ref1.m_ptr == ref2.m_ptr); }

protected:
    T *m_ptr;
};

/**
 * @brief Ссылка на Static
 */
using RStatic = Reference<Static>;
/**
 * @brief Ссылка на Button
 */
using RButton = Reference<Button>;
/**
 * @brief Ссылка на Edit
 */
using REdit = Reference<Edit>;
/**
 * @brief Ссылка на ListBox
 */
using RListBox = Reference<ListBox>;


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

/**
 * @brief Установить положение окна на экране
 * @param x X-координата
 * @param y Y-координата
 */
void setWindowPosition(DWORD x, DWORD y) {
    _m_appData.m_posX = x;
    _m_appData.m_posY = y;
}
/**
 * @brief Установить размер окна
 * @param width ширина (X-размер)
 * @param height высота (Y-размер)
 */
void setWindowSize(DWORD width, DWORD height) {
    _m_appData.m_width = width;
    _m_appData.m_height = height;
}
/**
 * @brief Установить название окна
 * @param title название
 */
void setWindowTitle(LPCWSTR title) {
    _m_appData.m_title = title;
}
/**
 * @brief Установить стиль окна
 * @param style флаги стилей (DWORD)
 */
void setWindowStyle(DWORD style) {
    _m_appData.m_style = style;
}
/**
 * @brief Установить возможность измены размера окна
 * @param value T/F
 */
void setWindowResizeable(bool value) {
    _m_appData.m_isResizeable = value;
    if (_m_appData.m_isResizeable)
        _m_appData.m_style |= WS_THICKFRAME;
    else
        _m_appData.m_style &= ~WS_THICKFRAME;
}

/**
 * @brief Добавить статичный текстовый элемент
 * @param posX X-координата
 * @param posY Y-координата
 * @param width ширина
 * @param height высота
 * @param text текст = ""
 * @param alignText выравнивание = Center
 * @return Ссылка на добавленный статичный текстовый элемент
 */
Static &addStatic(SHORT posX, SHORT posY, SHORT width, SHORT height, const std::wstring &text = L"", Align alignText = Align::Center) {
    Static *newStatic = new Static(posX, posY, width, height, text, alignText);
    _m_appData.m_elements.push_back(dynamic_cast<IElement *>(newStatic));
    return *newStatic;
}
/**
 * @brief Добавить кнопку
 * @param posX X-координата
 * @param posY Y-координата
 * @param width ширина
 * @param height высота
 * @param text текст = ""
 * @param onClick ответная функция на нажатие = NULL
 * @param alignText выравнивание = Center
 * @return Ссылка на добавленную кнопку
 */
Button &addButton(SHORT posX, SHORT posY, SHORT width, SHORT height, const std::wstring &text = L"", FuncCall<Button> onClick = nullptr, Align alignText = Align::Center) {
    Button *newBtn = new Button(posX, posY, width, height, text, onClick, alignText);
    _m_appData.m_elements.push_back(dynamic_cast<IElement *>(newBtn));
    return *newBtn;
}
/**
 * @brief Добавить элемент текстового ввода
 * @param posX X-координата
 * @param posY Y-координата
 * @param width ширина
 * @param height высота
 * @param isNumberOnly только численный ввод (T/F) = F
 * @param alignText выравнивание текста = Left
 * @param presetText начальный текст = ""
 * @return Ссылка на добавленный элемент текстового ввода
 */
Edit &addEdit(SHORT posX, SHORT posY, SHORT width, SHORT height, bool isNumberOnly = false, Align alignText = Align::Left, const std::wstring &presetText = L"") {
    Edit *newEdit = new Edit(posX, posY, width, height, isNumberOnly, alignText, presetText);
    _m_appData.m_elements.push_back(dynamic_cast<IElement *>(newEdit));
    return *newEdit;
}
/**
 * @brief Добавить элемент списка
 * @param posX X-координата
 * @param posY Y-координата
 * @param width ширина
 * @param height высота
 */
ListBox &addListBox(SHORT posX, SHORT posY, SHORT width, SHORT height, FuncCall<ListBox> onSelect = nullptr) {
    ListBox *newList = new ListBox(posX, posY, width, height, onSelect);
    _m_appData.m_elements.push_back(dynamic_cast<IElement *>(newList));
    return *newList;
}

/**
 * @brief Энумерация стандартных звуков Windows
 */
enum class WindowsSound { Warning, Error };

/**
 * @brief Проиграть стандартный звук Windows
 * @param sound звук
 */
void playWindowsSound(WindowsSound sound) {
    switch (sound) {
    case WindowsSound::Warning: MessageBeep(MB_ICONWARNING);    break;
    case WindowsSound::Error:   MessageBeep(MB_ICONERROR);      break;
    default: break;
    }
}

/**
 * @brief Функция инициализации элементов окна, вызывается автоматически (один раз)
 * @warning Функция должна быть определена в файле приложения, иначе код не скомпилируется
 */
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

    case WM_INITDIALOG:
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
        if (HIWORD(wParam) == BN_CLICKED) {
            IElement *elem = _m_appData.m_elements[LOWORD(wParam) - 1];
            Button *btn = dynamic_cast<Button *>(elem);
            if (!btn)
                return 0;
            FuncCall<Button> func = btn->getOnClick();
            if (!func)
                return 0;
            (*func)(*btn);
        } else if (HIWORD(wParam) == LBN_SELCHANGE) {
            IElement *elem = _m_appData.m_elements[LOWORD(wParam) - 1];
            ListBox *lb = dynamic_cast<ListBox *>(elem);
            if (!lb)
                return 0;
            FuncCall<ListBox> func = lb->getOnSelect();
            (*func)(*lb);
        }
        return 0;

    case WM_CTLCOLORSTATIC: {
        SetBkMode((HDC)wParam, TRANSPARENT);
        return (LRESULT)CreateSolidBrush(0xFFFFFF);                     
    }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
