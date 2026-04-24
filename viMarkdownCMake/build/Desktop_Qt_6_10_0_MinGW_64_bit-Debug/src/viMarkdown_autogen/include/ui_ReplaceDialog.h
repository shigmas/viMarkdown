/********************************************************************************
** Form generated from reading UI file 'ReplaceDialog.ui'
**
** Created by: Qt User Interface Compiler version 6.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REPLACEDIALOG_H
#define UI_REPLACEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ReplaceDialogClass
{
public:
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_2;
    QSpacerItem *verticalSpacer;
    QFormLayout *formLayout;
    QLabel *label;
    QComboBox *searchCB;
    QLabel *label_2;
    QComboBox *replaceCB;
    QCheckBox *ignoreCase;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *undoPB;
    QPushButton *redoPB;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer_4;
    QPushButton *searchPrev;
    QPushButton *searchNext;
    QPushButton *replaceNext;
    QPushButton *replaceAll;
    QPushButton *closeButton;

    void setupUi(QDialog *ReplaceDialogClass)
    {
        if (ReplaceDialogClass->objectName().isEmpty())
            ReplaceDialogClass->setObjectName("ReplaceDialogClass");
        ReplaceDialogClass->resize(414, 170);
        horizontalLayout = new QHBoxLayout(ReplaceDialogClass);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName("horizontalLayout");
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        formLayout = new QFormLayout();
        formLayout->setSpacing(6);
        formLayout->setObjectName("formLayout");
        label = new QLabel(ReplaceDialogClass);
        label->setObjectName("label");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, label);

        searchCB = new QComboBox(ReplaceDialogClass);
        searchCB->setObjectName("searchCB");
        searchCB->setEditable(true);

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, searchCB);

        label_2 = new QLabel(ReplaceDialogClass);
        label_2->setObjectName("label_2");

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, label_2);

        replaceCB = new QComboBox(ReplaceDialogClass);
        replaceCB->setObjectName("replaceCB");
        replaceCB->setEditable(true);

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, replaceCB);


        verticalLayout_2->addLayout(formLayout);

        ignoreCase = new QCheckBox(ReplaceDialogClass);
        ignoreCase->setObjectName("ignoreCase");

        verticalLayout_2->addWidget(ignoreCase);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_2);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        undoPB = new QPushButton(ReplaceDialogClass);
        undoPB->setObjectName("undoPB");

        horizontalLayout_2->addWidget(undoPB);

        redoPB = new QPushButton(ReplaceDialogClass);
        redoPB->setObjectName("redoPB");

        horizontalLayout_2->addWidget(redoPB);


        verticalLayout_2->addLayout(horizontalLayout_2);


        horizontalLayout->addLayout(verticalLayout_2);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName("verticalLayout");
        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer_4);

        searchPrev = new QPushButton(ReplaceDialogClass);
        searchPrev->setObjectName("searchPrev");

        verticalLayout->addWidget(searchPrev);

        searchNext = new QPushButton(ReplaceDialogClass);
        searchNext->setObjectName("searchNext");

        verticalLayout->addWidget(searchNext);

        replaceNext = new QPushButton(ReplaceDialogClass);
        replaceNext->setObjectName("replaceNext");
        replaceNext->setMaximumSize(QSize(200, 16777215));

        verticalLayout->addWidget(replaceNext);

        replaceAll = new QPushButton(ReplaceDialogClass);
        replaceAll->setObjectName("replaceAll");

        verticalLayout->addWidget(replaceAll);

        closeButton = new QPushButton(ReplaceDialogClass);
        closeButton->setObjectName("closeButton");
        closeButton->setMaximumSize(QSize(200, 16777215));

        verticalLayout->addWidget(closeButton);


        horizontalLayout->addLayout(verticalLayout);

        horizontalLayout->setStretch(0, 1);

        retranslateUi(ReplaceDialogClass);
        QObject::connect(closeButton, &QPushButton::clicked, ReplaceDialogClass, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(ReplaceDialogClass);
    } // setupUi

    void retranslateUi(QDialog *ReplaceDialogClass)
    {
        ReplaceDialogClass->setWindowTitle(QCoreApplication::translate("ReplaceDialogClass", "ReplaceDialog", nullptr));
        label->setText(QCoreApplication::translate("ReplaceDialogClass", "Search Text:", nullptr));
        label_2->setText(QCoreApplication::translate("ReplaceDialogClass", "Replace To:", nullptr));
        ignoreCase->setText(QCoreApplication::translate("ReplaceDialogClass", "IgnoreCase", nullptr));
        undoPB->setText(QCoreApplication::translate("ReplaceDialogClass", "&Undo", nullptr));
        redoPB->setText(QCoreApplication::translate("ReplaceDialogClass", "Re&do", nullptr));
        searchPrev->setText(QCoreApplication::translate("ReplaceDialogClass", "Search &Prev", nullptr));
        searchNext->setText(QCoreApplication::translate("ReplaceDialogClass", "Search &Next", nullptr));
        replaceNext->setText(QCoreApplication::translate("ReplaceDialogClass", "&Replace & Next", nullptr));
        replaceAll->setText(QCoreApplication::translate("ReplaceDialogClass", "Replace&All", nullptr));
        closeButton->setText(QCoreApplication::translate("ReplaceDialogClass", "&Close", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ReplaceDialogClass: public Ui_ReplaceDialogClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REPLACEDIALOG_H
