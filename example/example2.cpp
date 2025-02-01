#include "EasyWindows32.hpp"

using namespace easywindows32;

RListBox    list;
REdit       edit;
RButton     btn;

void btn_onClick(Button &_btn) {
    std::wstring text = edit->getText();
    if (!text.empty())
        list->addItem(text);
}

Font mainFont(L"Arial", 20);

void easywindows32::Initialize() {
    setWindowSize(400, 300);
    setWindowTitle(L"List");
    IElement::setFontDefault(mainFont);
    list = addListBox(10, 10, 100, 200);
    edit = addEdit(120, 10, 200, 30);
    btn = addButton(120, 50, 200, 30, L"Add to list", btn_onClick);
}
