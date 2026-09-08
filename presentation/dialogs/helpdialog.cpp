#include "helpdialog.h"


HelpDialog::HelpDialog(const QList<ImageProcessorInfo> &algorithms, QWidget *parent)
    : QDialog(parent), mAlgorithms(algorithms) {
    // Set dialog properties
    setWindowTitle("Help");
    resize(1000, 600);

    // Create instruction label
    QLabel *instructionLabel = new QLabel("Select an algorithm from the list to see its reference information.");
    instructionLabel->setWordWrap(true);

    // Create list widget for algorithms
    mListWidget = new QListWidget();
    for (const auto &algorithm : algorithms) {
        mListWidget->addItem(algorithm.name);
    }

    // Create text browser for algorithm details
    mInfoBrowser = new QTextBrowser();
    mInfoBrowser->setText("Select an algorithm to see details.");

    // Layout for algorithm details tab
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(mListWidget, 1);
    mainLayout->addWidget(mInfoBrowser, 2);

    QWidget *algorithmTab = new QWidget();
    QVBoxLayout *algorithmTabLayout = new QVBoxLayout(algorithmTab);
    algorithmTabLayout->addWidget(instructionLabel);
    algorithmTabLayout->addLayout(mainLayout);

    // Create text browser for "Image Area Selection" tab
    mImageAreaSelectionBrowser = new QTextBrowser();
    mImageAreaSelectionBrowser->setHtml(HelpHtmlFormatter::imageAreaSelectionHelp());

    // Create tab widget
    QTabWidget *tabWidget = new QTabWidget();
    tabWidget->addTab(algorithmTab, "Algorithms");
    tabWidget->addTab(mImageAreaSelectionBrowser, "Image Area Selection");

    // Main layout for the dialog
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(tabWidget);

    // Connect signals and slots
    connect(mListWidget, &QListWidget::currentRowChanged, this, &HelpDialog::onAlgorithmSelected);
}

void HelpDialog::onAlgorithmSelected(int index) {
    if (index >= 0 && index < mAlgorithms.size()) {
        const ImageProcessorInfo &selectedAlgorithm = mAlgorithms.at(index);
        QString html = HelpHtmlFormatter::formatImageProcessorInfo(selectedAlgorithm);
        mInfoBrowser->setHtml(html);
    } else {
        mInfoBrowser->setText("Select an algorithm to see details.");
    }
}
