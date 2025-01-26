#include "EasyWindows32.hpp"

using namespace easywindows32;

REdit    edit1;
REdit    edit2;
RButton  btnSolve;
RStatic  staticRes;

void clicked_BtnSolve(easywindows32::Button &btn) {
    int num1 = std::stoi(edit1->getText());
    int num2 = std::stoi(edit2->getText());
    staticRes->setText(std::to_wstring(num1 + num2));
}

Font mainFont(L"Arial", 25);

void easywindows32::Initialize() {
    setWindowSize(400, 300);
    setWindowTitle(L"Adder");
    IElement::setFontDefault(mainFont);
    edit1       = addEdit(35, 80, 160, 30, true, Align::Center, L"0");
    edit2       = addEdit(205, 80, 160, 30, true, Align::Center, L"0");
    btnSolve    = addButton(120, 125, 160, 30, L"Add", clicked_BtnSolve);
    staticRes   = addStatic(120, 170, 160, 30, L"0", Align::Center);
}
