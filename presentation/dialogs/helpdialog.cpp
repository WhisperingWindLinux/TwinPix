#include "helpdialog.h"


HelpDialog::HelpDialog(const QList<ImageProcessorInfo> &algorithms, QWidget *parent)
    : QDialog(parent), algorithms(algorithms) {
    // Set dialog properties
    setWindowTitle("Help");
    resize(1000, 600);

    // Create instruction label
    QLabel *instructionLabel = new QLabel("Select an algorithm from the list to see its reference information.");
    instructionLabel->setWordWrap(true);

    // Create list widget for algorithms
    listWidget = new QListWidget();
    for (const auto &algorithm : algorithms) {
        listWidget->addItem(algorithm.name);
    }

    // Create text browser for algorithm details
    infoBrowser = new QTextBrowser();
    infoBrowser->setText("Select an algorithm to see details.");

    // Layout for algorithm details tab
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(listWidget, 1);
    mainLayout->addWidget(infoBrowser, 2);

    QWidget *algorithmTab = new QWidget();
    QVBoxLayout *algorithmTabLayout = new QVBoxLayout(algorithmTab);
    algorithmTabLayout->addWidget(instructionLabel);
    algorithmTabLayout->addLayout(mainLayout);

    // Create text browser for "Image Area Selection" tab
    imageAreaSelectionBrowser = new QTextBrowser();
    QString infoText = R"(
        <html>
        <head>
            <style>
                body { font-family: Arial, sans-serif; font-size: 13px; }
                p { margin-bottom: 10px; }
                ul { margin-left: 20px; }
            </style>
        </head>
        <body>
            <p>You can select an area of the image using the left mouse button along with three modifier keys: <b>Option</b>, <b>Shift</b>, and <b>Command</b>.</p>
            <ul>
                <li><b>Option:</b> If you select an area while holding the <b>Option</b> key, the selected area of both compared images will be saved in a temporary directory as two PNG files. These files will be opened in a new instance of the application. Upon closing this new instance, the files will be automatically deleted. If you need these files, save each of them as a copy (<b>Command+S</b>).</li>
                <li><b>Shift:</b> If you select an area while holding the <b>Shift</b> key, the selected part of the image will be zoomed to fit the screen.</li>
                <li><b>Command + Comparator Hotkey:</b> If you select an area while holding <b>Command</b> and the comparator hotkey, the comparator will run only for the selected area. If the comparator returns an image and the selected area does not match the resolution of the original image, the comparison result will be opened in a separate viewer window. This is because the scaling feature in the main application window requires all images to have the same resolution when switching between them.</li>
            </ul>
        </body>
        </html>
    )";
    imageAreaSelectionBrowser->setHtml(infoText);

    // Create tab widget
    QTabWidget *tabWidget = new QTabWidget();
    tabWidget->addTab(algorithmTab, "Algorithms");
    tabWidget->addTab(imageAreaSelectionBrowser, "Image Area Selection");

    // Main layout for the dialog
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(tabWidget);

    // Connect signals and slots
    connect(listWidget, &QListWidget::currentRowChanged, this, &HelpDialog::onAlgorithmSelected);
}

void HelpDialog::onAlgorithmSelected(int index) {
    if (index >= 0 && index < algorithms.size()) {
        const ImageProcessorInfo &selectedAlgorithm = algorithms.at(index);
        QString html = HelpHtmlFormatter::formatImageProcessorInfo(selectedAlgorithm);
        infoBrowser->setHtml(html);
    } else {
        infoBrowser->setText("Select an algorithm to see details.");
    }
}
