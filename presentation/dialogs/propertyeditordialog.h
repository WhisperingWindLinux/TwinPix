#ifndef PROPERTYEDITORDIALOG_H
#define PROPERTYEDITORDIALOG_H

#include <QDialog>
#include <QList>
#include <QLabel>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

#include <domain/valueobjects/property.h>

class PropertyEditorDialog : public QDialog {
    Q_OBJECT

public:
    explicit PropertyEditorDialog(const QString &processorName,
                                  const QString &processorDescription,
                                  const QList<Property> &properties,
                                  QWidget *parent = nullptr
                                  );

    // Returns the updated list of properties
    QList<Property> getUpdatedProperties() const;

private slots:
    void onRun();

private:
    QList<Property> deafultProperties; // List of Property objects
    QList<Property> updatedProperties; // List of Property objects
    QList<QWidget*> editors;           // List of dynamically created editors for each property

    QVBoxLayout *mainLayout;
};

#endif // PROPERTYEDITORDIALOG_H
