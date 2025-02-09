#include "EasyWindows32.hpp"

using namespace easywindows32;

RListBox    list;
REdit       edit;
RButton     btnAdd;
RButton     btnRemove;
RButton     btnRemoveSel;
RStatic     selectedItem;

void list_onSelect(ListBox &_lb) {
    selectedItem->setText(L"Selected: " + list->getSelectedItem());
}

void btnAdd_onClick(Button &_btn) {
    std::wstring text = edit->getText();
    if (!text.empty())
        list->addItem(text);
}

void btnRemove_onClick(Button &_btn) {
    std::wstring text = edit->getText();
    int64_t id = list->findItem(text);
    if (id == LB_ERR) {
        playWindowsSound(WindowsSound::Warning);
        return;
    }
    list->removeItem(id);
}

void btnRemoveSel_onClick(Button &_btn) {
    int64_t id = list->getSelectedIndex();
    if (id == LB_ERR) {
        playWindowsSound(WindowsSound::Warning);
        return;
    }
    list->removeItem(id);
}

Font mainFont(L"Arial", 20);

void easywindows32::Initialize() {
    setWindowSize(400, 300);
    setWindowTitle(L"List");
    IElement::setFontDefault(mainFont);
    list            = addListBox(10, 10, 100, 200, list_onSelect);
    edit            = addEdit(120, 10, 200, 30);
    btnAdd          = addButton(120, 50, 200, 30, L"Add to list", btnAdd_onClick);
    btnRemove       = addButton(120, 90, 200, 30, L"Remove from list", btnRemove_onClick);
    btnRemoveSel    = addButton(120, 130, 200, 30, L"Remove selected", btnRemoveSel_onClick);
    selectedItem    = addStatic(120, 170, 200, 30, L"Selected: ");
}
