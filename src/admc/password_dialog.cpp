/*
 * ADMC - AD Management Center
 *
 * Copyright (C) 2020 BaseALT Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "password_dialog.h"
#include "ad_interface.h"
#include "edits/password_edit.h"
#include "edits/unlock_edit.h"
#include "edits/account_option_edit.h"
#include "status.h"

#include <QGridLayout>
#include <QLabel>
#include <QDialogButtonBox>

// TODO: find out exact password rules and add them to dialog?
// or display more info about constraint violations

PasswordDialog::PasswordDialog(const QString &target_arg)
: QDialog()
{
    target = target_arg;

    const QString target_name = AdInterface::instance()->get_name_for_display(target);
    const QString title_label_text = QString(tr("Resetting password of \"%1\"")).arg(target_name);
    const auto title_label = new QLabel(title_label_text, this);

    auto edits_layout = new QGridLayout();

    edits.append(new PasswordEdit(this));
    edits.append(new AccountOptionEdit(AccountOption_PasswordExpired, this));
    edits.append(new UnlockEdit(this));

    layout_attribute_edits(edits, edits_layout);

    auto button_box = new QDialogButtonBox(QDialogButtonBox::Ok |  QDialogButtonBox::Cancel, this);

    const auto layout = new QVBoxLayout();
    setLayout(layout);
    layout->addWidget(title_label);
    layout->addLayout(edits_layout);
    layout->addWidget(button_box);

    connect(
        button_box, &QDialogButtonBox::accepted,
        this, &PasswordDialog::accept);
    connect(
        button_box, &QDialogButtonBox::rejected,
        this, &QDialog::reject);
}

void PasswordDialog::accept() {
    const bool verify_success = verify_attribute_edits(edits, this);

    if (verify_success) {
        const int errors_index = Status::instance()->get_errors_size();

        const bool success = apply_attribute_edits(edits, target, this);

        if (success) {
            QDialog::accept();
        }

        Status::instance()->show_errors_popup(errors_index);
    }
}
