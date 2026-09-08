#include "aboutdialog.h"

#include <QPushButton>
#include <QTextBrowser>
#include <qboxlayout.h>
#include <qlabel.h>

#include <presentation/dialogs/formatters/helphtmlformatter.h>

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("About");

    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Application information
    QLabel *appNameLabel = new QLabel(HelpHtmlFormatter::aboutApplicationName(), this);
    appNameLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(appNameLabel);

    QLabel *appVersion = new QLabel("version 0.11.0-alpha");
    appVersion->setAlignment(Qt::AlignCenter);
    layout->addWidget(appVersion);

    QLabel *warning = new QLabel(HelpHtmlFormatter::aboutAlphaWarning());
    warning->setWordWrap(true);
    warning->setAlignment(Qt::AlignCenter) ;
    layout->addWidget(warning);

    QLabel *warning2 = new QLabel(HelpHtmlFormatter::aboutBugWarning());
    warning2->setAlignment(Qt::AlignCenter);
    layout->addWidget(warning2);

    QLabel *authorLabel = new QLabel("Author: WhisperingWind", this);
    authorLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(authorLabel);

    QLabel *licenseLabel = new QLabel("GNU GENERAL PUBLIC LICENSE\nVersion 3, 29 June 2007", this);
    licenseLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(licenseLabel);

    QLabel *repoLinkLabel = new QLabel(this);
    repoLinkLabel->setTextFormat(Qt::RichText);
    repoLinkLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    repoLinkLabel->setOpenExternalLinks(true);
    repoLinkLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(repoLinkLabel);

    // License details (optional, displayed in a scrollable text box)
    QTextBrowser *licenseDetails = new QTextBrowser(this);
    licenseDetails->setText(
        "This program is licensed under the GNU General Public License "
        "Version 3.\n\n"
        "You can redistribute it and/or modify it under the terms of the GNU "
        "General Public License as published by the Free Software Foundation.\n\n"
        "This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; "
        "without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. "
        "See the GNU General Public License for more details.\n\n"
        "You should have received a copy of the GNU General Public License along with this program. "
        "If not, see https://www.gnu.org/licenses/gpl-3.0.txt.\n\n"
        "You can find me on https://github.com/WhisperingWindLinux.");
    licenseDetails->setReadOnly(true);
    licenseDetails->setMinimumHeight(150);
    layout->addWidget(licenseDetails);

    // Close button
    QPushButton *closeButton = new QPushButton("Close", this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(closeButton);

    // Set layout
    setLayout(layout);

    // Set fixed size for the dialog
    setFixedSize(450, 400);
}
