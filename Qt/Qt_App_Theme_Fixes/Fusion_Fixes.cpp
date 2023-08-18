#include "Fusion_Fixes.h"
#include <QPalette>

void Fusion_Fixes::Apply_Fusion_Fixed(QApplication &application) {
    application.setStyle("fusion"); //use fusion theme on Windows

    // Check if the application is in dark mode
    QPalette palette = application.palette();
    bool isDarkMode = (palette.color(QPalette::Window).value() < 128);

    // Apply custom styles only in dark mode
    if (isDarkMode) {
        //Fix the Qt6 colors issues with Fusion in some Windows 10 Dark Themes
        QString checkBoxStyle = "QCheckBox::indicator:unchecked { background-color: #222; border: 1px solid #555; }"
                                "QCheckBox::indicator:unchecked:disabled { background-color: #222; border: 1px solid #444; opacity: 0.5; }";

        QString radioButtonStyle = "QRadioButton::indicator:unchecked { background-color: #222; border: 1px solid #555; width: 12px; height: 12px; border-radius: 7px; }"
                                   "QRadioButton::indicator:unchecked:disabled { background-color: #222; border: 1px solid #444; width: 12px; height: 12px; border-radius: 7px; opacity: 0.5; }";;

        application.setStyleSheet(checkBoxStyle + radioButtonStyle);
    }
}
