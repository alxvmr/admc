/*
 * ADMC - AD Management Center
 *
 * Copyright (C) 2020-2021 BaseALT Ltd.
 * Copyright (C) 2020-2021 Dmitry Degtyarev
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

#include "editors/bool_editor.h"
#include "editors/ui_bool_editor.h"

#include "adldap.h"
#include "globals.h"
#include "settings.h"

BoolEditor::BoolEditor(QWidget *parent)
: AttributeEditor(parent) {
    ui = new Ui::BoolEditor();
    ui->setupUi(this);

    AttributeEditor::set_attribute_label(ui->attribute_label);

    settings_setup_dialog_geometry(SETTING_bool_editor_geometry, this);
}

BoolEditor::~BoolEditor() {
    delete ui;
}

void BoolEditor::set_read_only(const bool read_only) {
    ui->true_button->setEnabled(!read_only);
    ui->false_button->setEnabled(!read_only);
    ui->unset_button->setEnabled(!read_only);
}

void BoolEditor::set_value_list(const QList<QByteArray> &values) {
    if (values.isEmpty()) {
        ui->unset_button->setChecked(true);
    } else {
        const QByteArray value = values[0];
        const QString value_string = QString(value);
        const bool value_bool = ad_string_to_bool(value_string);

        if (value_bool) {
            ui->true_button->setChecked(true);
        } else {
            ui->false_button->setChecked(true);
        }
    }
}

QList<QByteArray> BoolEditor::get_value_list() const {
    if (ui->unset_button->isChecked()) {
        return QList<QByteArray>();
    } else if (ui->true_button->isChecked()) {
        const QByteArray value = QString(LDAP_BOOL_TRUE).toUtf8();
        return {value};
    } else if (ui->false_button->isChecked()) {
        const QByteArray value = QString(LDAP_BOOL_FALSE).toUtf8();
        return {value};
    } else {
        return QList<QByteArray>();
    }
}
