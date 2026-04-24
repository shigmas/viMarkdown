/********************************************************************************
** Form generated from reading UI file 'SettingsDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSDIALOG_H
#define UI_SETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SettingsDialogClass
{
public:
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QLabel *label;
    QSpinBox *fontSize;
    QLabel *label_2;
    QPushButton *headingsColorPB;
    QLabel *label_6;
    QPushButton *boldColorPB;
    QLabel *label_9;
    QPushButton *matchColorPB;
    QLabel *label_10;
    QLabel *label_11;
    QPushButton *italicColorPB;
    QPushButton *strikethroughColorPB;
    QLabel *label_15;
    QPushButton *boldItalicColorPB;
    QGroupBox *groupBox_2;
    QFormLayout *formLayout_2;
    QLabel *label_3;
    QPushButton *CSVHeaderPB;
    QLabel *label_4;
    QPushButton *CSVZebra1PB;
    QLabel *label_5;
    QPushButton *CSVZebra2PB;
    QLabel *label_12;
    QPushButton *quotePB;
    QLabel *label_13;
    QLabel *label_14;
    QPushButton *codeBlockPB;
    QPushButton *keisenBlockPB;
    QHBoxLayout *horizontalLayout_4;
    QFormLayout *formLayout_3;
    QLabel *label_7;
    QPushButton *activeLineColorPB;
    QLabel *label_8;
    QPushButton *inactiveLineColorPB;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *SettingsDialogClass)
    {
        if (SettingsDialogClass->objectName().isEmpty())
            SettingsDialogClass->setObjectName("SettingsDialogClass");
        SettingsDialogClass->resize(600, 400);
        horizontalLayout_3 = new QHBoxLayout(SettingsDialogClass);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        groupBox = new QGroupBox(SettingsDialogClass);
        groupBox->setObjectName("groupBox");
        formLayout = new QFormLayout(groupBox);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName("formLayout");
        label = new QLabel(groupBox);
        label->setObjectName("label");
        label->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, label);

        fontSize = new QSpinBox(groupBox);
        fontSize->setObjectName("fontSize");
        fontSize->setMaximumSize(QSize(100, 16777215));
        fontSize->setMinimum(3);
        fontSize->setMaximum(64);

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, fontSize);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");
        label_2->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, label_2);

        headingsColorPB = new QPushButton(groupBox);
        headingsColorPB->setObjectName("headingsColorPB");
        headingsColorPB->setMaximumSize(QSize(100, 16777215));

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, headingsColorPB);

        label_6 = new QLabel(groupBox);
        label_6->setObjectName("label_6");
        label_6->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        formLayout->setWidget(3, QFormLayout::ItemRole::LabelRole, label_6);

        boldColorPB = new QPushButton(groupBox);
        boldColorPB->setObjectName("boldColorPB");
        boldColorPB->setMaximumSize(QSize(100, 16777215));

        formLayout->setWidget(3, QFormLayout::ItemRole::FieldRole, boldColorPB);

        label_9 = new QLabel(groupBox);
        label_9->setObjectName("label_9");

        formLayout->setWidget(8, QFormLayout::ItemRole::LabelRole, label_9);

        matchColorPB = new QPushButton(groupBox);
        matchColorPB->setObjectName("matchColorPB");
        matchColorPB->setMaximumSize(QSize(100, 16777215));

        formLayout->setWidget(8, QFormLayout::ItemRole::FieldRole, matchColorPB);

        label_10 = new QLabel(groupBox);
        label_10->setObjectName("label_10");

        formLayout->setWidget(4, QFormLayout::ItemRole::LabelRole, label_10);

        label_11 = new QLabel(groupBox);
        label_11->setObjectName("label_11");

        formLayout->setWidget(5, QFormLayout::ItemRole::LabelRole, label_11);

        italicColorPB = new QPushButton(groupBox);
        italicColorPB->setObjectName("italicColorPB");
        italicColorPB->setMaximumSize(QSize(100, 16777215));

        formLayout->setWidget(4, QFormLayout::ItemRole::FieldRole, italicColorPB);

        strikethroughColorPB = new QPushButton(groupBox);
        strikethroughColorPB->setObjectName("strikethroughColorPB");
        strikethroughColorPB->setMaximumSize(QSize(100, 16777215));

        formLayout->setWidget(5, QFormLayout::ItemRole::FieldRole, strikethroughColorPB);

        label_15 = new QLabel(groupBox);
        label_15->setObjectName("label_15");

        formLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, label_15);

        boldItalicColorPB = new QPushButton(groupBox);
        boldItalicColorPB->setObjectName("boldItalicColorPB");
        boldItalicColorPB->setMaximumSize(QSize(100, 16777215));

        formLayout->setWidget(2, QFormLayout::ItemRole::FieldRole, boldItalicColorPB);


        horizontalLayout_2->addWidget(groupBox);

        groupBox_2 = new QGroupBox(SettingsDialogClass);
        groupBox_2->setObjectName("groupBox_2");
        formLayout_2 = new QFormLayout(groupBox_2);
        formLayout_2->setSpacing(6);
        formLayout_2->setContentsMargins(11, 11, 11, 11);
        formLayout_2->setObjectName("formLayout_2");
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName("label_3");
        label_3->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        formLayout_2->setWidget(0, QFormLayout::ItemRole::LabelRole, label_3);

        CSVHeaderPB = new QPushButton(groupBox_2);
        CSVHeaderPB->setObjectName("CSVHeaderPB");
        CSVHeaderPB->setMaximumSize(QSize(100, 16777215));

        formLayout_2->setWidget(0, QFormLayout::ItemRole::FieldRole, CSVHeaderPB);

        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName("label_4");
        label_4->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        formLayout_2->setWidget(1, QFormLayout::ItemRole::LabelRole, label_4);

        CSVZebra1PB = new QPushButton(groupBox_2);
        CSVZebra1PB->setObjectName("CSVZebra1PB");
        CSVZebra1PB->setMaximumSize(QSize(100, 16777215));

        formLayout_2->setWidget(1, QFormLayout::ItemRole::FieldRole, CSVZebra1PB);

        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName("label_5");
        label_5->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        formLayout_2->setWidget(2, QFormLayout::ItemRole::LabelRole, label_5);

        CSVZebra2PB = new QPushButton(groupBox_2);
        CSVZebra2PB->setObjectName("CSVZebra2PB");
        CSVZebra2PB->setMaximumSize(QSize(100, 16777215));

        formLayout_2->setWidget(2, QFormLayout::ItemRole::FieldRole, CSVZebra2PB);

        label_12 = new QLabel(groupBox_2);
        label_12->setObjectName("label_12");

        formLayout_2->setWidget(3, QFormLayout::ItemRole::LabelRole, label_12);

        quotePB = new QPushButton(groupBox_2);
        quotePB->setObjectName("quotePB");

        formLayout_2->setWidget(3, QFormLayout::ItemRole::FieldRole, quotePB);

        label_13 = new QLabel(groupBox_2);
        label_13->setObjectName("label_13");

        formLayout_2->setWidget(4, QFormLayout::ItemRole::LabelRole, label_13);

        label_14 = new QLabel(groupBox_2);
        label_14->setObjectName("label_14");

        formLayout_2->setWidget(5, QFormLayout::ItemRole::LabelRole, label_14);

        codeBlockPB = new QPushButton(groupBox_2);
        codeBlockPB->setObjectName("codeBlockPB");

        formLayout_2->setWidget(4, QFormLayout::ItemRole::FieldRole, codeBlockPB);

        keisenBlockPB = new QPushButton(groupBox_2);
        keisenBlockPB->setObjectName("keisenBlockPB");

        formLayout_2->setWidget(5, QFormLayout::ItemRole::FieldRole, keisenBlockPB);


        horizontalLayout_2->addWidget(groupBox_2);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        formLayout_3 = new QFormLayout();
        formLayout_3->setSpacing(6);
        formLayout_3->setObjectName("formLayout_3");
        label_7 = new QLabel(SettingsDialogClass);
        label_7->setObjectName("label_7");
        label_7->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        formLayout_3->setWidget(0, QFormLayout::ItemRole::LabelRole, label_7);

        activeLineColorPB = new QPushButton(SettingsDialogClass);
        activeLineColorPB->setObjectName("activeLineColorPB");
        activeLineColorPB->setMaximumSize(QSize(100, 16777215));

        formLayout_3->setWidget(0, QFormLayout::ItemRole::FieldRole, activeLineColorPB);

        label_8 = new QLabel(SettingsDialogClass);
        label_8->setObjectName("label_8");
        label_8->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        formLayout_3->setWidget(1, QFormLayout::ItemRole::LabelRole, label_8);

        inactiveLineColorPB = new QPushButton(SettingsDialogClass);
        inactiveLineColorPB->setObjectName("inactiveLineColorPB");
        inactiveLineColorPB->setMaximumSize(QSize(100, 16777215));

        formLayout_3->setWidget(1, QFormLayout::ItemRole::FieldRole, inactiveLineColorPB);


        horizontalLayout_4->addLayout(formLayout_3);


        verticalLayout->addLayout(horizontalLayout_4);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton = new QPushButton(SettingsDialogClass);
        pushButton->setObjectName("pushButton");

        horizontalLayout->addWidget(pushButton);

        pushButton_2 = new QPushButton(SettingsDialogClass);
        pushButton_2->setObjectName("pushButton_2");

        horizontalLayout->addWidget(pushButton_2);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout);


        horizontalLayout_3->addLayout(verticalLayout);

        QWidget::setTabOrder(fontSize, headingsColorPB);
        QWidget::setTabOrder(headingsColorPB, boldItalicColorPB);
        QWidget::setTabOrder(boldItalicColorPB, boldColorPB);
        QWidget::setTabOrder(boldColorPB, italicColorPB);
        QWidget::setTabOrder(italicColorPB, strikethroughColorPB);
        QWidget::setTabOrder(strikethroughColorPB, matchColorPB);
        QWidget::setTabOrder(matchColorPB, CSVHeaderPB);
        QWidget::setTabOrder(CSVHeaderPB, CSVZebra1PB);
        QWidget::setTabOrder(CSVZebra1PB, CSVZebra2PB);
        QWidget::setTabOrder(CSVZebra2PB, quotePB);
        QWidget::setTabOrder(quotePB, codeBlockPB);
        QWidget::setTabOrder(codeBlockPB, keisenBlockPB);
        QWidget::setTabOrder(keisenBlockPB, activeLineColorPB);
        QWidget::setTabOrder(activeLineColorPB, inactiveLineColorPB);
        QWidget::setTabOrder(inactiveLineColorPB, pushButton);
        QWidget::setTabOrder(pushButton, pushButton_2);

        retranslateUi(SettingsDialogClass);
        QObject::connect(pushButton, &QPushButton::clicked, SettingsDialogClass, qOverload<>(&QDialog::accept));
        QObject::connect(pushButton_2, &QPushButton::clicked, SettingsDialogClass, qOverload<>(&QDialog::reject));

        pushButton->setDefault(true);


        QMetaObject::connectSlotsByName(SettingsDialogClass);
    } // setupUi

    void retranslateUi(QDialog *SettingsDialogClass)
    {
        SettingsDialogClass->setWindowTitle(QCoreApplication::translate("SettingsDialogClass", "SettingsDialog", nullptr));
        groupBox->setTitle(QCoreApplication::translate("SettingsDialogClass", "Editor", nullptr));
        label->setText(QCoreApplication::translate("SettingsDialogClass", "Font Size:", nullptr));
        label_2->setText(QCoreApplication::translate("SettingsDialogClass", "Title/Headdings:", nullptr));
        headingsColorPB->setText(QString());
        label_6->setText(QCoreApplication::translate("SettingsDialogClass", "Bold color:", nullptr));
        boldColorPB->setText(QString());
        label_9->setText(QCoreApplication::translate("SettingsDialogClass", "Match color:", nullptr));
        matchColorPB->setText(QString());
        label_10->setText(QCoreApplication::translate("SettingsDialogClass", "Italic color:", nullptr));
        label_11->setText(QCoreApplication::translate("SettingsDialogClass", "strikethrough:", nullptr));
        italicColorPB->setText(QString());
        strikethroughColorPB->setText(QString());
        label_15->setText(QCoreApplication::translate("SettingsDialogClass", "Bold Italic color:", nullptr));
        boldItalicColorPB->setText(QString());
        groupBox_2->setTitle(QCoreApplication::translate("SettingsDialogClass", "Preview", nullptr));
        label_3->setText(QCoreApplication::translate("SettingsDialogClass", "Table Header color:", nullptr));
        CSVHeaderPB->setText(QString());
        label_4->setText(QCoreApplication::translate("SettingsDialogClass", "Table Zebra-1 color:", nullptr));
        CSVZebra1PB->setText(QString());
        label_5->setText(QCoreApplication::translate("SettingsDialogClass", "Table Zebra-2 color:", nullptr));
        CSVZebra2PB->setText(QString());
        label_12->setText(QCoreApplication::translate("SettingsDialogClass", "Quote color:", nullptr));
        quotePB->setText(QString());
        label_13->setText(QCoreApplication::translate("SettingsDialogClass", "Code block color:", nullptr));
        label_14->setText(QCoreApplication::translate("SettingsDialogClass", "Keisen block color:", nullptr));
        codeBlockPB->setText(QString());
        keisenBlockPB->setText(QString());
        label_7->setText(QCoreApplication::translate("SettingsDialogClass", "active line corsor:", nullptr));
        activeLineColorPB->setText(QString());
        label_8->setText(QCoreApplication::translate("SettingsDialogClass", "inactive line cursor:", nullptr));
        inactiveLineColorPB->setText(QString());
        pushButton->setText(QCoreApplication::translate("SettingsDialogClass", "O K", nullptr));
        pushButton_2->setText(QCoreApplication::translate("SettingsDialogClass", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingsDialogClass: public Ui_SettingsDialogClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSDIALOG_H
